# S05: CONSTRAINTS - simple_clipboard

**Status:** BACKWASH (reverse-engineered from implementation)
**Date:** 2026-01-23
**Library:** simple_clipboard

## Technical Constraints

### 1. Windows Only
- **Constraint:** Uses Win32 clipboard API
- **Impact:** Not portable to Unix/Mac
- **Mitigation:** Could add platform-specific implementations

### 2. Text Only (CF_TEXT)
- **Constraint:** Only supports ANSI text format
- **Impact:** No images, rich text, files
- **Mitigation:** Future CF_UNICODETEXT support

### 3. Single-Threaded Access
- **Constraint:** Windows clipboard is single-access
- **Impact:** Operations may fail if clipboard is locked
- **Mitigation:** Check last_operation_succeeded

### 4. No Notifications
- **Constraint:** No clipboard change monitoring
- **Impact:** Can't react to external changes
- **Mitigation:** Poll has_text if needed

### 5. Memory Ownership
- **Constraint:** Clipboard takes ownership of allocated memory
- **Impact:** Cannot reuse clipboard data memory
- **Mitigation:** C code handles GlobalAlloc properly

## Resource Limits

| Resource | Limit | Notes |
|----------|-------|-------|
| Text size | Windows limit | ~2GB theoretical |
| Concurrent access | 1 | Windows restriction |

## Performance Constraints

| Operation | Expected Time |
|-----------|---------------|
| set_text | < 1ms |
| text | < 1ms |
| has_text | < 1ms |
| clear | < 1ms |
