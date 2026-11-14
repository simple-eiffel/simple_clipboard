# SIMPLE_CLIPBOARD

![Windows: tests pass](https://img.shields.io/badge/Windows-tests%20pass-0078D6?logo=windows&logoColor=white) ![Linux: tests pass](https://img.shields.io/badge/Linux-tests%20pass-FCC624?logo=linux&logoColor=black) ![macOS: tests pass](https://img.shields.io/badge/macOS-tests%20pass-000000?logo=apple&logoColor=white)

SCOOP-compatible clipboard text access for Windows, macOS and Linux.

## Features

- Get and set clipboard text
- Check if clipboard contains text
- Clear clipboard contents
- Check clipboard format availability (on macOS and Linux, text only: 0 or 1)
- Thread-safe (SCOOP-compatible)

## Installation

Add to your ECF file:

```xml
<library name="simple_clipboard" location="$SIMPLE_EIFFEL/simple_clipboard/simple_clipboard.ecf"/>
```

Set the environment variable (one-time setup for all simple_* libraries):
```
SIMPLE_EIFFEL=D:\prod
```
On macOS or Linux:
```bash
export SIMPLE_EIFFEL=$HOME/simple_eiffel
```

## Quick Start

```eiffel
local
    cb: SIMPLE_CLIPBOARD
do
    create cb

    -- Copy text to clipboard
    cb.set_text ("Hello, World!")

    -- Paste text from clipboard
    if attached cb.text as t then
        print (t)
    end

    -- Check if clipboard has text
    if cb.has_text then
        print ("Clipboard contains text%N")
    end

    -- Clear clipboard
    cb.clear
end
```

## API Overview

### SIMPLE_CLIPBOARD

| Feature | Description |
|---------|-------------|
| `text` | Get text from clipboard (Void if empty) |
| `set_text (str)` | Set clipboard text |
| `has_text` | Check if clipboard has text |
| `is_empty` | Check if clipboard is empty |
| `clear` | Clear clipboard contents |
| `copy_text (str)` | Alias for set_text |
| `paste` | Alias for text |
| `format_count` | Number of formats on clipboard (macOS/Linux: 1 if text, else 0) |
| `is_available` | Is there a clipboard to reach? Always on Windows and macOS; on Linux it takes a display and a clipboard tool |
| `unavailable_reason` | Why there is none (empty when `is_available`) |
| `backend_name` | What reaches the clipboard: `win32`, `pbcopy`, `wl-clipboard`, `xclip`, `xsel`, or `none` |

Text is Unicode on every platform (Hebrew, Greek and emoji round-trip). The tests set and read the real clipboard, restoring its text afterwards, and fail when it can't be reached.

## Documentation

- [API Documentation](https://simple-eiffel.github.io/simple_clipboard/)

## Platform Support

- **Windows** - Win32 clipboard API
- **macOS** - text through `pbcopy`/`pbpaste`; `clear` sets empty text
- **Linux** - text through `wl-copy`/`wl-paste` (Wayland), else `xclip` or `xsel` (X11). Needs a display and one of these tools; without them every call fails.

## License

MIT License - see LICENSE file for details.

## Author

Larry Rix
