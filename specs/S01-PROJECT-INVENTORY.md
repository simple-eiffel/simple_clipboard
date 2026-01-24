# S01: PROJECT INVENTORY - simple_clipboard

**Status:** BACKWASH (reverse-engineered from implementation)
**Date:** 2026-01-23
**Library:** simple_clipboard

## Project Structure

```
simple_clipboard/
    src/
        simple_clipboard.e      -- Main class
    testing/
        test_app.e              -- Test application
        lib_tests.e             -- Test suite
    include/
        simple_clipboard.h      -- C wrapper header
    research/
        7S-01-SCOPE.md
        7S-02-STANDARDS.md
        7S-03-SOLUTIONS.md
        7S-04-SIMPLE-STAR.md
        7S-05-SECURITY.md
        7S-06-SIZING.md
        7S-07-RECOMMENDATION.md
    specs/
        S01-PROJECT-INVENTORY.md
        S02-CLASS-CATALOG.md
        S03-CONTRACTS.md
        S04-FEATURE-SPECS.md
        S05-CONSTRAINTS.md
        S06-BOUNDARIES.md
        S07-SPEC-SUMMARY.md
        S08-VALIDATION-REPORT.md
    simple_clipboard.ecf        -- Project configuration
```

## File Inventory

| File | Type | Lines | Purpose |
|------|------|-------|---------|
| simple_clipboard.e | Source | 159 | Main clipboard class |
| test_app.e | Test | ~50 | Test runner |
| lib_tests.e | Test | ~100 | Test cases |
| simple_clipboard.h | C Header | ~100 | Win32 wrapper |

## External Dependencies

| Dependency | Type | Location |
|------------|------|----------|
| User32.dll | System | Windows |
| EiffelBase | Library | ISE_LIBRARY |
