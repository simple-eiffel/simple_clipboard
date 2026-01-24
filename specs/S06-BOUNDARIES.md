# S06: BOUNDARIES - simple_clipboard

**Status:** BACKWASH (reverse-engineered from implementation)
**Date:** 2026-01-23
**Library:** simple_clipboard

## API Boundaries

### Public Interface (Exported to ANY)

#### Access
- `text` - Get clipboard text
- `paste` - Alias for text

#### Status
- `has_text` - Check for text
- `is_empty` - Check if empty
- `format_count` - Count formats
- `last_operation_succeeded` - Operation status

#### Modification
- `set_text` - Set clipboard text
- `copy_text` - Alias for set_text
- `clear` - Clear clipboard

### Internal Interface (NONE)

- `pointer_to_string` - C string conversion
- `c_scb_*` - All C external functions
- `c_free` - Memory deallocation

## Integration Points

| Component | Interface | Direction |
|-----------|-----------|-----------|
| Win32 API | C inline | Outbound |
| Caller code | Public API | Inbound |
| System clipboard | Win32 API | Both |

## Data Flow

```
Eiffel Code
    |
    v
SIMPLE_CLIPBOARD
    |
    v (inline C)
simple_clipboard.h (scb_* functions)
    |
    v (Win32 API)
Windows Clipboard
```

## Format Support

| Format | Supported | Notes |
|--------|-----------|-------|
| CF_TEXT | Yes | ANSI text |
| CF_UNICODETEXT | No | Future |
| CF_BITMAP | No | Not planned |
| CF_DIB | No | Not planned |
| CF_HDROP | No | Not planned |
