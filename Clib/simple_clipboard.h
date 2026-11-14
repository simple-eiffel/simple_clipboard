/*
 * simple_clipboard.h - Cross-platform Clipboard helper functions for Eiffel
 *
 * Every platform takes and gives UTF-8 text:
 * Windows: Win32 Clipboard API, CF_UNICODETEXT, converted to and from UTF-8
 * Linux: wl-copy/wl-paste (Wayland), else xclip or xsel (X11)
 * macOS: pbcopy/pbpaste, part of the system
 *
 * Following Eric Bezault's recommended pattern: implementations in .h file,
 * called from Eiffel inline C with use directive. No mutable statics: each
 * generated C file gets its own copy of this header.
 *
 * Copyright (c) 2025 Larry Rix - MIT License
 */

#ifndef SIMPLE_CLIPBOARD_H
#define SIMPLE_CLIPBOARD_H

#include <stdlib.h>
#include <string.h>

/* Backends, as scb_detect_backend answers. */
#define SCB_BACKEND_NONE 0
#define SCB_BACKEND_XCLIP 1
#define SCB_BACKEND_WL_CLIPBOARD 2
#define SCB_BACKEND_XSEL 3
#define SCB_BACKEND_PBCOPY 4
#define SCB_BACKEND_WIN32 5

/* Problems, as scb_problem answers. */
#define SCB_PROBLEM_NONE 0
#define SCB_PROBLEM_NO_DISPLAY 1
#define SCB_PROBLEM_NO_TOOL 2

/* Attempts to open a clipboard another program holds, 20 milliseconds apart. */
#define SCB_ATTEMPTS 10

#if defined(_WIN32) || defined(EIF_WINDOWS)
/* ============ WINDOWS IMPLEMENTATION ============ */

#include <windows.h>

static int scb_detect_backend(void) {
    return SCB_BACKEND_WIN32;
}

static int scb_problem(void) {
    return SCB_PROBLEM_NONE;
}

/* Open the clipboard, waiting briefly for another program to let go of it. */
static int scb_open(void) {
    int attempts;
    for (attempts = 0; attempts < SCB_ATTEMPTS; attempts++) {
        if (OpenClipboard(NULL)) return 1;
        Sleep(20);
    }
    return 0;
}

/* Clipboard text as UTF-8, or NULL if there is none or the clipboard cannot
   be opened. Caller must free result with free(). */
static char* scb_get_text(void) {
    HANDLE hData;
    const WCHAR* pData;
    char* result = NULL;
    int len;

    if (!scb_open()) return NULL;
    hData = GetClipboardData(CF_UNICODETEXT);
    if (hData != NULL) {
        pData = (const WCHAR*)GlobalLock(hData);
        if (pData != NULL) {
            len = WideCharToMultiByte(CP_UTF8, 0, pData, -1, NULL, 0, NULL, NULL);
            if (len > 0) {
                result = (char*)malloc((size_t)len);
                if (result && WideCharToMultiByte(CP_UTF8, 0, pData, -1, result, len, NULL, NULL) != len) {
                    free(result);
                    result = NULL;
                }
            }
            GlobalUnlock(hData);
        }
    }
    CloseClipboard();
    return result;
}

/* Set clipboard text from UTF-8. Returns 1 on success, 0 on failure. */
static int scb_set_text(const char* text) {
    HGLOBAL hMem;
    WCHAR* pMem;
    int len;
    int success = 0;

    if (!text) return 0;

    len = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    if (len <= 0) return 0;

    hMem = GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)len * sizeof(WCHAR));
    if (!hMem) return 0;
    pMem = (WCHAR*)GlobalLock(hMem);
    if (!pMem) {
        GlobalFree(hMem);
        return 0;
    }
    MultiByteToWideChar(CP_UTF8, 0, text, -1, pMem, len);
    GlobalUnlock(hMem);

    if (scb_open()) {
        if (EmptyClipboard() && SetClipboardData(CF_UNICODETEXT, hMem) != NULL) {
            success = 1;
        }
        CloseClipboard();
    }
    /* The system owns hMem only once SetClipboardData succeeded. */
    if (!success) GlobalFree(hMem);
    return success;
}

/* Clear clipboard. Returns 1 on success, 0 on failure. */
static int scb_clear(void) {
    int success = 0;
    if (scb_open()) {
        success = EmptyClipboard() ? 1 : 0;
        CloseClipboard();
    }
    return success;
}

/* Check if clipboard has text. Returns 1 if true, 0 if false. */
static int scb_has_text(void) {
    return IsClipboardFormatAvailable(CF_UNICODETEXT) ? 1 : 0;
}

/* Get number of clipboard formats available. Needs no open clipboard. */
static int scb_format_count(void) {
    return CountClipboardFormats();
}

/* Check if clipboard is empty. Returns 1 if empty, 0 if not. */
static int scb_is_empty(void) {
    return (scb_format_count() == 0) ? 1 : 0;
}

#else
/* ============ UNIX/LINUX IMPLEMENTATION ============ */
/* Uses wl-copy/wl-paste for Wayland, else xclip or xsel for X11 */

#include <stdio.h>
#include <unistd.h>

/* Check if a command is available */
static int scb_command_exists(const char* cmd) {
    char check[256];
    snprintf(check, sizeof(check), "command -v %s > /dev/null 2>&1", cmd);
    return system(check) == 0;
}

/* Detect clipboard backend: one of the SCB_BACKEND_ values. */
static int scb_detect_backend(void) {
#ifdef __APPLE__
    return SCB_BACKEND_PBCOPY;
#else
    /* Check for Wayland first */
    if (getenv("WAYLAND_DISPLAY") != NULL) {
        if (scb_command_exists("wl-copy") && scb_command_exists("wl-paste")) {
            return SCB_BACKEND_WL_CLIPBOARD;
        }
    }
    /* Fall back to X11 */
    if (getenv("DISPLAY") != NULL) {
        if (scb_command_exists("xclip")) {
            return SCB_BACKEND_XCLIP;
        }
        if (scb_command_exists("xsel")) {
            return SCB_BACKEND_XSEL;
        }
    }
    return SCB_BACKEND_NONE;
#endif
}

/* Why no backend: one of the SCB_PROBLEM_ values. */
static int scb_problem(void) {
    if (scb_detect_backend() != SCB_BACKEND_NONE) return SCB_PROBLEM_NONE;
    if (getenv("WAYLAND_DISPLAY") == NULL && getenv("DISPLAY") == NULL) return SCB_PROBLEM_NO_DISPLAY;
    return SCB_PROBLEM_NO_TOOL;
}

/* Get text from clipboard. Caller must free result with free(). */
static char* scb_get_text(void) {
    FILE* pipe;
    char* result = NULL;
    char buffer[4096];
    size_t total_size = 0;
    size_t buffer_size = 4096;
    size_t read_size;
    int backend = scb_detect_backend();
    const char* cmd;

    switch (backend) {
        case SCB_BACKEND_XCLIP: cmd = "xclip -selection clipboard -o 2>/dev/null"; break;
        case SCB_BACKEND_WL_CLIPBOARD: cmd = "wl-paste -n 2>/dev/null"; break;
        case SCB_BACKEND_XSEL: cmd = "xsel --clipboard --output 2>/dev/null"; break;
        case SCB_BACKEND_PBCOPY: cmd = "LC_CTYPE=UTF-8 pbpaste 2>/dev/null"; break;
        default: return NULL;
    }

    pipe = popen(cmd, "r");
    if (!pipe) return NULL;

    result = (char*)malloc(buffer_size);
    if (!result) {
        pclose(pipe);
        return NULL;
    }

    while ((read_size = fread(buffer, 1, sizeof(buffer), pipe)) > 0) {
        if (total_size + read_size >= buffer_size) {
            buffer_size *= 2;
            char* new_result = (char*)realloc(result, buffer_size);
            if (!new_result) {
                free(result);
                pclose(pipe);
                return NULL;
            }
            result = new_result;
        }
        memcpy(result + total_size, buffer, read_size);
        total_size += read_size;
    }

    pclose(pipe);

    if (total_size == 0) {
        free(result);
        return NULL;
    }

    result[total_size] = '\0';
    return result;
}

/* Set clipboard text. Returns 1 on success, 0 on failure. */
static int scb_set_text(const char* text) {
    FILE* pipe;
    int backend;
    const char* cmd;
    size_t len;

    if (!text) return 0;

    backend = scb_detect_backend();
    switch (backend) {
        case SCB_BACKEND_XCLIP: cmd = "xclip -selection clipboard 2>/dev/null"; break;
        case SCB_BACKEND_WL_CLIPBOARD: cmd = "wl-copy 2>/dev/null"; break;
        case SCB_BACKEND_XSEL: cmd = "xsel --clipboard --input 2>/dev/null"; break;
        case SCB_BACKEND_PBCOPY: cmd = "LC_CTYPE=UTF-8 pbcopy 2>/dev/null"; break;
        default: return 0;
    }

    pipe = popen(cmd, "w");
    if (!pipe) return 0;

    len = strlen(text);
    if (fwrite(text, 1, len, pipe) != len) {
        pclose(pipe);
        return 0;
    }

    return pclose(pipe) == 0 ? 1 : 0;
}

/* Clear clipboard. Returns 1 on success, 0 on failure. */
static int scb_clear(void) {
    /* Set empty string to clipboard */
    return scb_set_text("");
}

/* Check if clipboard has text. Returns 1 if true, 0 if false. */
static int scb_has_text(void) {
    char* text = scb_get_text();
    if (text) {
        int has = (strlen(text) > 0) ? 1 : 0;
        free(text);
        return has;
    }
    return 0;
}

/* Check if clipboard is empty. Returns 1 if empty, 0 if not. */
static int scb_is_empty(void) {
    return scb_has_text() ? 0 : 1;
}

/* Get number of clipboard formats available. */
static int scb_format_count(void) {
    /* Linux clipboard doesn't expose format count easily.
       Return 1 if has text, 0 otherwise */
    return scb_has_text() ? 1 : 0;
}

#endif /* _WIN32 */

#endif /* SIMPLE_CLIPBOARD_H */
