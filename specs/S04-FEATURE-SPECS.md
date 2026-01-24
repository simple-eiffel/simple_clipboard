# S04: FEATURE SPECS - simple_clipboard

**Status:** BACKWASH (reverse-engineered from implementation)
**Date:** 2026-01-23
**Library:** simple_clipboard

## Feature Specifications

### Access Features

| Feature | Signature | Description |
|---------|-----------|-------------|
| text | : detachable STRING_32 | Get clipboard text, Void if none |
| paste | : detachable STRING_32 | Alias for text |

### Status Report Features

| Feature | Signature | Description |
|---------|-----------|-------------|
| has_text | : BOOLEAN | Does clipboard have text? |
| is_empty | : BOOLEAN | Is clipboard empty? |
| format_count | : INTEGER | Number of formats available |

### Modification Features

| Feature | Signature | Description |
|---------|-----------|-------------|
| set_text | (text: READABLE_STRING_GENERAL) | Put text on clipboard |
| copy_text | (text: READABLE_STRING_GENERAL) | Alias for set_text |
| clear | | Clear all clipboard contents |

### Status Features

| Feature | Signature | Description |
|---------|-----------|-------------|
| last_operation_succeeded | : BOOLEAN | Did last op succeed? |

## C External Mapping

| Eiffel Feature | C Function | Win32 APIs Used |
|----------------|------------|-----------------|
| text | scb_get_text | OpenClipboard, GetClipboardData, CloseClipboard |
| set_text | scb_set_text | OpenClipboard, EmptyClipboard, SetClipboardData, CloseClipboard |
| clear | scb_clear | OpenClipboard, EmptyClipboard, CloseClipboard |
| has_text | scb_has_text | IsClipboardFormatAvailable |
| is_empty | scb_is_empty | CountClipboardFormats |
| format_count | scb_format_count | CountClipboardFormats |

## Usage Patterns

### Basic Copy
```eiffel
clip.set_text ("Hello")
```

### Safe Paste
```eiffel
if clip.has_text then
    if attached clip.text as t then
        process (t)
    end
end
```

### Clear After Sensitive Data
```eiffel
clip.set_text (password)
-- use password
clip.clear
```
