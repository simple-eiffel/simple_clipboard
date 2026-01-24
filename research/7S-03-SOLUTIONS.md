# 7S-03: SOLUTIONS - simple_clipboard


**Date**: 2026-01-23

**Status:** BACKWASH (reverse-engineered from implementation)
**Date:** 2026-01-23
**Library:** simple_clipboard

## Existing Solutions Comparison

### 1. EiffelStudio EV_CLIPBOARD
- **Pros:** Part of EiffelVision2, cross-platform
- **Cons:** Requires full EiffelVision2 dependency

### 2. Direct Win32 API calls
- **Pros:** Full control, no dependencies
- **Cons:** Complex, error-prone, not Eiffel-idiomatic

### 3. simple_clipboard (chosen solution)
- **Pros:** Simple API, inline C, SCOOP-compatible
- **Cons:** Text-only, Windows-specific

### 4. Third-party clipboard libraries
- **Pros:** Feature-rich
- **Cons:** External dependencies, licensing

## Why simple_clipboard?

1. **Minimal API** - Just text operations
2. **No dependencies** - Uses system API directly
3. **SCOOP-safe** - Designed for concurrency
4. **Self-contained** - Single class implementation
5. **Inline C pattern** - Consistent with simple_* ecosystem
