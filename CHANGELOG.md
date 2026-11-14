# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Fixed
- Unicode text: `set_text` no longer fails a precondition on text outside 8 bits; text travels as UTF-8 on every platform and as CF_UNICODETEXT on Windows, so Hebrew and emoji round-trip
- Windows `is_empty` and `format_count` no longer report an empty clipboard when another program holds it open
- Tests no longer pass when the clipboard cannot be reached; `test_clear_clipboard` checks the clipboard is empty

### Added
- `is_available`, `backend_name` and `unavailable_reason`: a Linux session with no display or no clipboard tool says so

### Changed
- Post-session update 2025-12-08 20:52
- Post-session update 2025-12-08 20:45
- Post-session update 2025-12-08 20:16
- Inline C code into Eiffel class 
- Testing config updates, AutoTest fixes, .gitignore cleanup
- Migrate to simple_testing library
- Add GitHub Pages documentation
- Initial implementation: Windows clipboard library
- first commit

## [1.0.0] - 2025-12-08

### Added
- Initial release
- Core functionality implemented
- Test suite with comprehensive coverage
- Documentation and examples

[Unreleased]: https://github.com/simple-eiffel/simple_clipboard/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/simple-eiffel/simple_clipboard/releases/tag/v1.0.0
