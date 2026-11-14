note
	description: "[
		SCOOP-compatible clipboard access.
		Provides text clipboard operations: Win32 on Windows, pbcopy and
		pbpaste on macOS, wl-clipboard (Wayland) or xclip or xsel (X11) on
		Linux. Text travels as UTF-8 everywhere (UTF-16 inside Windows), so
		any Unicode text round-trips.

		A Linux session with no display, or with no clipboard tool, has no
		clipboard: `is_available' is then False and `unavailable_reason' says
		why, and every operation fails.
	]"
	author: "Larry Rix"
	date: "$Date$"
	revision: "$Revision$"

class
	SIMPLE_CLIPBOARD

feature -- Access

	text: detachable STRING_32
			-- Get text from clipboard.
			-- Returns Void if no text available.
		local
			l_result: POINTER
		do
			l_result := c_scb_get_text
			if l_result /= default_pointer then
				Result := pointer_to_string (l_result)
				c_free (l_result)
			end
		ensure
			none_when_unavailable: not is_available implies Result = Void
		end

	backend_name: STRING_32
			-- What reaches the clipboard here: "win32", "pbcopy", "wl-clipboard",
			-- "xclip", "xsel", or "none".
		do
			inspect c_scb_detect_backend
			when 1 then
				Result := {STRING_32} "xclip"
			when 2 then
				Result := {STRING_32} "wl-clipboard"
			when 3 then
				Result := {STRING_32} "xsel"
			when 4 then
				Result := {STRING_32} "pbcopy"
			when 5 then
				Result := {STRING_32} "win32"
			else
				Result := {STRING_32} "none"
			end
		ensure
			not_empty: not Result.is_empty
		end

	unavailable_reason: STRING_32
			-- Why there is no clipboard here; empty when `is_available'.
		do
			inspect c_scb_problem
			when 1 then
				Result := {STRING_32} "No display: neither WAYLAND_DISPLAY nor DISPLAY is set"
			when 2 then
				Result := {STRING_32} "No clipboard tool: install wl-clipboard (Wayland), or xclip or xsel (X11)"
			else
				create Result.make_empty
			end
		ensure
			empty_exactly_when_available: Result.is_empty = is_available
		end

feature -- Status Report

	is_available: BOOLEAN
			-- Is there a clipboard to reach? Always on Windows and macOS; on Linux
			-- it takes a display and a clipboard tool.
		do
			Result := c_scb_detect_backend /= 0
		end

	has_text: BOOLEAN
			-- Does clipboard contain text?
		do
			Result := c_scb_has_text /= 0
		end

	is_empty: BOOLEAN
			-- Is clipboard empty (no data in any format)?
		do
			Result := c_scb_is_empty /= 0
		end

	format_count: INTEGER
			-- Number of different formats available on clipboard.
		do
			Result := c_scb_format_count
		end

feature -- Modification

	set_text (a_text: READABLE_STRING_GENERAL)
			-- Put `a_text' on clipboard.
		require
			text_not_void: a_text /= Void
		local
			l_text: C_STRING
		do
			create l_text.make ({UTF_CONVERTER}.string_32_to_utf_8_string_8 (a_text.to_string_32))
			last_operation_succeeded := c_scb_set_text (l_text.item) /= 0
		ensure
			fails_when_unavailable: not is_available implies not last_operation_succeeded
			text_set: (last_operation_succeeded and not a_text.is_empty) implies has_text
		end

	clear
			-- Clear all clipboard contents.
		do
			last_operation_succeeded := c_scb_clear /= 0
		ensure
			fails_when_unavailable: not is_available implies not last_operation_succeeded
			cleared: last_operation_succeeded implies is_empty
		end

	copy_text (a_text: READABLE_STRING_GENERAL)
			-- Copy `a_text' to clipboard (alias for set_text).
		require
			text_not_void: a_text /= Void
		do
			set_text (a_text)
		end

	paste: detachable STRING_32
			-- Paste text from clipboard (alias for text).
		do
			Result := text
		end

feature -- Status

	last_operation_succeeded: BOOLEAN
			-- Did the last operation succeed?

feature {NONE} -- Implementation

	pointer_to_string (a_ptr: POINTER): STRING_32
			-- Convert UTF-8 C string pointer to STRING_32.
		local
			l_c_string: C_STRING
		do
			create l_c_string.make_by_pointer (a_ptr)
			Result := {UTF_CONVERTER}.utf_8_string_8_to_string_32 (l_c_string.string)
		end

feature {NONE} -- C externals (using simple_clipboard.h)

	c_scb_get_text: POINTER
			-- Get text from clipboard. Caller must free result.
		external
			"C inline use %"simple_clipboard.h%""
		alias
			"return scb_get_text();"
		end

	c_scb_set_text (a_text: POINTER): INTEGER
			-- Set clipboard text. Returns 1 on success.
		external
			"C inline use %"simple_clipboard.h%""
		alias
			"return scb_set_text((const char*)$a_text);"
		end

	c_scb_clear: INTEGER
			-- Clear clipboard. Returns 1 on success.
		external
			"C inline use %"simple_clipboard.h%""
		alias
			"return scb_clear();"
		end

	c_scb_has_text: INTEGER
			-- Check if clipboard has text. Returns 1 if true.
		external
			"C inline use %"simple_clipboard.h%""
		alias
			"return scb_has_text();"
		end

	c_scb_is_empty: INTEGER
			-- Check if clipboard is empty. Returns 1 if true.
		external
			"C inline use %"simple_clipboard.h%""
		alias
			"return scb_is_empty();"
		end

	c_scb_format_count: INTEGER
			-- Get number of clipboard formats available.
		external
			"C inline use %"simple_clipboard.h%""
		alias
			"return scb_format_count();"
		end

	c_scb_detect_backend: INTEGER
			-- Backend in use: 0 none, 1 xclip, 2 wl-clipboard, 3 xsel, 4 pbcopy, 5 Win32.
		external
			"C inline use %"simple_clipboard.h%""
		alias
			"return scb_detect_backend();"
		end

	c_scb_problem: INTEGER
			-- Why there is no backend: 0 none, 1 no display, 2 no clipboard tool.
		external
			"C inline use %"simple_clipboard.h%""
		alias
			"return scb_problem();"
		end

	c_free (a_ptr: POINTER)
			-- Free allocated memory.
		external
			"C inline use <stdlib.h>"
		alias
			"free($a_ptr);"
		end

end
