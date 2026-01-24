# S03: CONTRACTS - simple_clipboard

**Status:** BACKWASH (reverse-engineered from implementation)
**Date:** 2026-01-23
**Library:** simple_clipboard

## Design by Contract Summary

### SIMPLE_CLIPBOARD Contracts

#### set_text
```eiffel
require
    text_not_void: a_text /= Void
ensure
    text_set: last_operation_succeeded implies has_text
```

#### copy_text
```eiffel
require
    text_not_void: a_text /= Void
```

#### clear
```eiffel
ensure
    cleared: last_operation_succeeded implies is_empty
```

## Feature Contracts

### text
- No preconditions (always callable)
- Returns detachable STRING_32 (Void if no text)

### has_text
- No preconditions
- Pure query (no side effects)

### is_empty
- No preconditions
- Pure query (no side effects)

### format_count
- No preconditions
- Returns INTEGER >= 0

## Error Handling

The class uses `last_operation_succeeded` to track operation status:
- Set by `set_text`, `clear`
- Caller should check after operations
- No exceptions thrown

## Thread Safety Notes

While SCOOP-compatible, the underlying Windows clipboard:
- Only one application can have it open at a time
- Operations may fail if another app has it open
- Check `last_operation_succeeded` after operations
