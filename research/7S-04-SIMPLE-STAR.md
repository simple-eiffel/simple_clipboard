# 7S-04: SIMPLE-STAR - simple_clipboard

**Status:** BACKWASH (reverse-engineered from implementation)
**Date:** 2026-01-23
**Library:** simple_clipboard

## Ecosystem Integration

### Dependencies on Other simple_* Libraries
- None (standalone utility)

### Libraries That May Depend on simple_clipboard
- **simple_gui** - Copy/paste in GUI applications
- **simple_editor** - Text editor functionality
- **simple_cli** - CLI tools with clipboard integration

### Integration Patterns

#### Copy operation
```eiffel
local
    clip: SIMPLE_CLIPBOARD
do
    create clip
    clip.set_text ("Hello, World!")
    -- or using alias
    clip.copy_text ("Hello, World!")
end
```

#### Paste operation
```eiffel
local
    clip: SIMPLE_CLIPBOARD
    content: detachable STRING_32
do
    create clip
    content := clip.text
    -- or using alias
    content := clip.paste
end
```

#### Check before paste
```eiffel
if clip.has_text then
    content := clip.text
end
```

## Namespace Conventions
- Single class: SIMPLE_CLIPBOARD
- C header: simple_clipboard.h
- C functions: scb_* prefix
