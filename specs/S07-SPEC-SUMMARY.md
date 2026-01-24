# S07: SPEC SUMMARY - simple_clipboard

**Status:** BACKWASH (reverse-engineered from implementation)
**Date:** 2026-01-23
**Library:** simple_clipboard

## Executive Summary

simple_clipboard provides minimal, SCOOP-compatible clipboard access for Windows Eiffel applications. It focuses exclusively on text operations with a clean, simple API.

## Key Design Decisions

### 1. Text-Only Focus
Only CF_TEXT format for simplicity. Covers 90% of use cases.

### 2. Operation Status
Uses `last_operation_succeeded` instead of exceptions for error handling.

### 3. Alias Features
Provides `copy_text`/`paste` aliases for familiar semantics.

### 4. Detachable Return
`text` returns detachable STRING_32 - Void when clipboard empty.

### 5. SCOOP Compatibility
Designed for safe use in concurrent Eiffel programs.

## Class Summary

| Class | Purpose | Lines |
|-------|---------|-------|
| SIMPLE_CLIPBOARD | Clipboard access | 159 |

## Feature Summary

| Category | Features |
|----------|----------|
| Read | text, paste |
| Write | set_text, copy_text |
| Query | has_text, is_empty, format_count |
| Clear | clear |
| Status | last_operation_succeeded |

## Contract Coverage

- `set_text` requires non-void text
- `set_text` ensures has_text on success
- `clear` ensures is_empty on success
- No class invariants needed (stateless)

## API Simplicity

Total public features: 9
- 2 read operations
- 2 write operations
- 3 query operations
- 1 clear operation
- 1 status query
