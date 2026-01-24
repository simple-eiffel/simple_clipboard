# S02: CLASS CATALOG - simple_clipboard

**Status:** BACKWASH (reverse-engineered from implementation)
**Date:** 2026-01-23
**Library:** simple_clipboard

## Class Hierarchy

```
SIMPLE_CLIPBOARD (standalone class)
```

## Class Description

### SIMPLE_CLIPBOARD
**Purpose:** SCOOP-compatible clipboard access
**Role:** Provides text clipboard operations via Win32 API
**Key Features:**

#### Access
- `text` - Get text from clipboard (returns detachable STRING_32)

#### Status Report
- `has_text` - Does clipboard contain text?
- `is_empty` - Is clipboard empty?
- `format_count` - Number of formats available

#### Modification
- `set_text` - Put text on clipboard
- `clear` - Clear all clipboard contents
- `copy_text` - Alias for set_text
- `paste` - Alias for text

#### Status
- `last_operation_succeeded` - Did last operation succeed?

## Internal Structure

### C Externals (scb_* functions)
| Function | Purpose |
|----------|---------|
| scb_get_text | Get text from clipboard |
| scb_set_text | Set clipboard text |
| scb_clear | Clear clipboard |
| scb_has_text | Check for text |
| scb_is_empty | Check if empty |
| scb_format_count | Count formats |

### Memory Management
- `pointer_to_string` - Convert C string to STRING_32
- `c_free` - Free allocated C memory
