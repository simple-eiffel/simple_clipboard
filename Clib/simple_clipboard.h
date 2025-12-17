/*
 * simple_clipboard.h - Cross-platform Clipboard helper functions for Eiffel
 *
 * Windows: Uses Win32 Clipboard API
 * Linux: Uses xclip (X11) or wl-copy/wl-paste (Wayland)
 *
 * Following Eric Bezault's recommended pattern: implementations in .h file,
 * called from Eiffel inline C with use directive.
 *
 * Copyright (c) 2025 Larry Rix - MIT License
 */

#ifndef SIMPLE_CLIPBOARD_H
#define SIMPLE_CLIPBOARD_H

#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(EIF_WINDOWS)
/* ============ WINDOWS IMPLEMENTATION ============ */

#include <windows.h>

/* Get text from clipboard. Caller must free result with free(). */
static char* scb_get_text(void) {
    HANDLE hData;
    char* pData;
    char* result = NULL;
    size_t len;
    int retries = 3;

    while (retries > 0) {
        if (OpenClipboard(NULL)) {
            hData = GetClipboardData(CF_TEXT);
            if (hData != NULL) {
                pData = (char*)GlobalLock(hData);
                if (pData != NULL) {
                    len = strlen(pData);
                    result = (char*)malloc(len + 1);
                    if (result) {
                        strcpy(result, pData);
                    }
                    GlobalUnlock(hData);
                }
            }
            CloseClipboard();
            break;
        }
        retries--;
        if (retries > 0) Sleep(10);
    }
    return result;
}

/* Set clipboard text. Returns 1 on success, 0 on failure. */
static int scb_set_text(const char* text) {
    HGLOBAL hMem;
    char* pMem;
    size_t len;
    int success = 0;
    int retries = 3;

    if (!text) return 0;

    len = strlen(text) + 1;

    while (retries > 0 && !success) {
        hMem = GlobalAlloc(GMEM_MOVEABLE, len);
        if (!hMem) return 0;

        pMem = (char*)GlobalLock(hMem);
        if (!pMem) {
            GlobalFree(hMem);
            return 0;
        }

        memcpy(pMem, text, len);
        GlobalUnlock(hMem);

        if (OpenClipboard(NULL)) {
            EmptyClipboard();
            if (SetClipboardData(CF_TEXT, hMem) != NULL) {
                success = 1;
            } else {
                GlobalFree(hMem);
            }
            CloseClipboard();
            if (success) break;
        } else {
            GlobalFree(hMem);
        }

        retries--;
        if (retries > 0) Sleep(10);
    }
    return success;
}

/* Clear clipboard. Returns 1 on success, 0 on failure. */
static int scb_clear(void) {
    int retries = 3;
    int success = 0;

    while (retries > 0 && !success) {
        if (OpenClipboard(NULL)) {
            if (EmptyClipboard()) {
                success = 1;
            }
            CloseClipboard();
            if (success) break;
        }
        retries--;
        if (retries > 0) Sleep(10);
    }
    return success;
}

/* Check if clipboard has text. Returns 1 if true, 0 if false. */
static int scb_has_text(void) {
    return IsClipboardFormatAvailable(CF_TEXT) ? 1 : 0;
}

/* Check if clipboard is empty. Returns 1 if empty, 0 if not. */
static int scb_is_empty(void) {
    int count;
    if (!OpenClipboard(NULL)) return 1;
    count = CountClipboardFormats();
    CloseClipboard();
    return (count == 0) ? 1 : 0;
}

/* Get number of clipboard formats available. */
static int scb_format_count(void) {
    int count;
    if (!OpenClipboard(NULL)) return 0;
    count = CountClipboardFormats();
    CloseClipboard();
    return count;
}

#else
/* ============ UNIX/LINUX IMPLEMENTATION ============ */
/* Uses xclip for X11 or wl-copy/wl-paste for Wayland */

#include <stdio.h>
#include <unistd.h>

/* Check if a command is available */
static int scb_command_exists(const char* cmd) {
    char check[256];
    snprintf(check, sizeof(check), "command -v %s > /dev/null 2>&1", cmd);
    return system(check) == 0;
}

/* Detect clipboard backend: 0=none, 1=xclip, 2=wl-clipboard */
static int scb_detect_backend(void) {
    /* Check for Wayland first */
    if (getenv("WAYLAND_DISPLAY") != NULL) {
        if (scb_command_exists("wl-copy") && scb_command_exists("wl-paste")) {
            return 2;
        }
    }
    /* Fall back to X11 */
    if (getenv("DISPLAY") != NULL) {
        if (scb_command_exists("xclip")) {
            return 1;
        }
        if (scb_command_exists("xsel")) {
            return 3;
        }
    }
    return 0;
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

    if (backend == 0) return NULL;

    switch (backend) {
        case 1: cmd = "xclip -selection clipboard -o 2>/dev/null"; break;
        case 2: cmd = "wl-paste -n 2>/dev/null"; break;
        case 3: cmd = "xsel --clipboard --output 2>/dev/null"; break;
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
    if (backend == 0) return 0;

    switch (backend) {
        case 1: cmd = "xclip -selection clipboard 2>/dev/null"; break;
        case 2: cmd = "wl-copy 2>/dev/null"; break;
        case 3: cmd = "xsel --clipboard --input 2>/dev/null"; break;
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
