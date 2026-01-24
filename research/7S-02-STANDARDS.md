# 7S-02: STANDARDS - simple_clipboard

**Status:** BACKWASH (reverse-engineered from implementation)
**Date:** 2026-01-23
**Library:** simple_clipboard

## Applicable Standards

### Win32 Clipboard API
- OpenClipboard / CloseClipboard - Access control
- GetClipboardData / SetClipboardData - Data transfer
- EmptyClipboard - Clear contents
- IsClipboardFormatAvailable - Format queries
- CountClipboardFormats - Format counting

### Clipboard Formats
- CF_TEXT (1) - ANSI text
- CF_UNICODETEXT (13) - Unicode text
- CF_OEMTEXT (7) - OEM text

### Memory Management
- GlobalAlloc / GlobalFree - Allocate clipboard memory
- GlobalLock / GlobalUnlock - Access memory
- Clipboard takes ownership of allocated memory

### Thread Safety
- Only one thread can have clipboard open
- Must close clipboard before other apps can access

## Compliance Notes

The implementation uses CF_TEXT format for simplicity. Unicode support would require CF_UNICODETEXT handling with proper encoding conversion.
