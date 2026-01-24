# 7S-01: SCOPE - simple_clipboard


**Date**: 2026-01-23

**Status:** BACKWASH (reverse-engineered from implementation)
**Date:** 2026-01-23
**Library:** simple_clipboard

## Problem Domain

System clipboard access for Windows applications. The clipboard is the standard mechanism for copy/paste operations, allowing data exchange between applications.

## Target Users

- Eiffel applications needing copy/paste functionality
- CLI tools that work with clipboard data
- Automation scripts
- Text processing utilities

## Boundaries

### In Scope
- Text clipboard read/write (CF_TEXT format)
- Clipboard status queries (has text, is empty)
- Format count query
- Clear operation
- SCOOP-compatible design

### Out of Scope
- Rich text (RTF)
- HTML format
- Images (bitmap, PNG)
- Files/file lists
- Custom clipboard formats
- Clipboard change notifications
- Delayed rendering

## Dependencies

- Win32 API (User32.dll)
- No external DLLs required
