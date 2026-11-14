note
	description: "[
		Tests for SIMPLE_CLIPBOARD library.

		Each test fails when the clipboard cannot be reached. The one
		exception is a platform with no clipboard at all (Linux with no display
		or no clipboard tool): there the test checks that the library says so
		through `is_available' and `unavailable_reason', and that every
		operation fails, and prints the reason.
	]"
	testing: "covers"

class
	LIB_TESTS

inherit
	TEST_SET_BASE

feature -- Test routines

	test_availability_reported
			-- The library names its backend and reason, and agrees with itself.
		local
			l_clip: SIMPLE_CLIPBOARD
		do
			create l_clip
			print ("%N    backend: " + utf_8 (l_clip.backend_name) + "%N    ")
			if {PLATFORM}.is_windows or {PLATFORM}.is_mac then
				assert_true ("always available here", l_clip.is_available)
			end
			if l_clip.is_available then
				assert_true ("no reason when available", l_clip.unavailable_reason.is_empty)
				assert_false ("backend named", l_clip.backend_name.same_string ("none"))
			else
				assert_reported_unavailable (l_clip)
			end
		end

	test_unicode_round_trip
			-- Text with Hebrew and an emoji comes back exactly as set.
		local
			l_clip: SIMPLE_CLIPBOARD
		do
			create l_clip
			if l_clip.is_available then
				l_clip.set_text (Unicode_sample)
				assert_true ("set succeeded", l_clip.last_operation_succeeded)
				assert_true ("has text", l_clip.has_text)
				assert_false ("not empty", l_clip.is_empty)
				assert_attached ("text read back", l_clip.text)
				if attached l_clip.text as l_text then
					assert_strings_equal_diff ("same text", Unicode_sample, l_text)
				end
			else
				assert_reported_unavailable (l_clip)
			end
		end

	test_has_text_query
			-- has_text follows what was set.
		local
			l_clip: SIMPLE_CLIPBOARD
		do
			create l_clip
			if l_clip.is_available then
				l_clip.set_text ("HasTextTest")
				assert_true ("set succeeded", l_clip.last_operation_succeeded)
				assert_true ("has_text_after_set", l_clip.has_text)
			else
				assert_reported_unavailable (l_clip)
			end
		end

	test_clear_clipboard
			-- After clear the clipboard is empty and holds no text.
		local
			l_clip: SIMPLE_CLIPBOARD
		do
			create l_clip
			if l_clip.is_available then
				l_clip.set_text ("ToClear")
				assert_true ("set succeeded", l_clip.last_operation_succeeded)
				l_clip.clear
				assert_true ("clear succeeded", l_clip.last_operation_succeeded)
				assert_true ("empty", l_clip.is_empty)
				assert_false ("no text", l_clip.has_text)
				assert_void ("nothing to read", l_clip.text)
				assert_integers_equal ("no formats", 0, l_clip.format_count)
			else
				assert_reported_unavailable (l_clip)
			end
		end

	test_multiline_text
			-- Line breaks come back exactly as set.
		local
			l_clip: SIMPLE_CLIPBOARD
		do
			create l_clip
			if l_clip.is_available then
				l_clip.set_text ({STRING_32} "Line1%R%NLine2%R%NLine3")
				assert_true ("set succeeded", l_clip.last_operation_succeeded)
				assert_attached ("text read back", l_clip.text)
				if attached l_clip.text as l_text then
					assert_strings_equal_diff ("same lines", {STRING_32} "Line1%R%NLine2%R%NLine3", l_text)
				end
			else
				assert_reported_unavailable (l_clip)
			end
		end

	test_format_count_query
			-- Text on the clipboard gives at least one format.
		local
			l_clip: SIMPLE_CLIPBOARD
		do
			create l_clip
			if l_clip.is_available then
				l_clip.set_text ("FormatTest")
				assert_true ("set succeeded", l_clip.last_operation_succeeded)
				assert_greater_or_equal ("a format", l_clip.format_count, 1)
			else
				assert_reported_unavailable (l_clip)
			end
		end

	test_copy_text_alias
			-- copy_text and paste round-trip like set_text and text.
		local
			l_clip: SIMPLE_CLIPBOARD
		do
			create l_clip
			if l_clip.is_available then
				l_clip.copy_text ("CopyTest")
				assert_true ("copy succeeded", l_clip.last_operation_succeeded)
				assert_attached ("paste returned text", l_clip.paste)
				if attached l_clip.paste as l_text then
					assert_strings_equal_diff ("same text", {STRING_32} "CopyTest", l_text)
				end
			else
				assert_reported_unavailable (l_clip)
			end
		end

feature {NONE} -- Support

	Unicode_sample: STRING_32
			-- Latin, Hebrew (shalom), an emoji outside the Basic Multilingual Plane, and a tab.
		once
			Result := {STRING_32} "Shalom %/0x05E9/%/0x05DC/%/0x05D5/%/0x05DD/ smile %/0x1F600/%Tend"
		end

	assert_reported_unavailable (a_clip: SIMPLE_CLIPBOARD)
			-- `a_clip' has no clipboard: allowed only off Windows and macOS, it
			-- must say why, and every operation must fail.
		do
			print ("%N    clipboard not available: " + utf_8 (a_clip.unavailable_reason) + "%N    ")
			assert_false ("only linux may lack a clipboard", {PLATFORM}.is_windows or {PLATFORM}.is_mac)
			assert_false ("reason given", a_clip.unavailable_reason.is_empty)
			assert_true ("no backend", a_clip.backend_name.same_string ("none"))
			a_clip.set_text ("Unreachable")
			assert_false ("set fails", a_clip.last_operation_succeeded)
			a_clip.clear
			assert_false ("clear fails", a_clip.last_operation_succeeded)
			assert_void ("nothing to read", a_clip.text)
		end

	utf_8 (a_text: READABLE_STRING_32): STRING_8
			-- `a_text' as UTF-8, for printing.
		do
			Result := {UTF_CONVERTER}.string_32_to_utf_8_string_8 (a_text)
		end

end
