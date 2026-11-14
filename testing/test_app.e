note
	description: "[
		Test application for simple_clipboard.

		The tests overwrite the clipboard, so the text on it beforehand is
		saved and put back afterwards (other formats, such as images, are not).
	]"
	date: "$Date$"
	revision: "$Revision$"

class
	TEST_APP

create
	make

feature -- Initialization

	make
			-- Run the tests.
		local
			l_clip: SIMPLE_CLIPBOARD
			l_saved: detachable STRING_32
		do
			create l_clip
			l_saved := l_clip.text
			create tests
			run_test (agent tests.test_availability_reported, "test_availability_reported")
			run_test (agent tests.test_unicode_round_trip, "test_unicode_round_trip")
			run_test (agent tests.test_has_text_query, "test_has_text_query")
			run_test (agent tests.test_clear_clipboard, "test_clear_clipboard")
			run_test (agent tests.test_multiline_text, "test_multiline_text")
			run_test (agent tests.test_format_count_query, "test_format_count_query")
			run_test (agent tests.test_copy_text_alias, "test_copy_text_alias")
			if attached l_saved as l_text then
				l_clip.set_text (l_text)
			end
			print ("%N========================%N")
			print ("Results: " + passed.out + " passed, " + failed.out + " failed%N")
		end

feature {NONE} -- Implementation

	tests: LIB_TESTS

	passed: INTEGER

	failed: INTEGER

	run_test (a_test: PROCEDURE; a_name: STRING)
			-- Run a single test and update counters.
		local
			l_retried: BOOLEAN
		do
			if not l_retried then
				a_test.call (Void)
				print ("  PASS: " + a_name + "%N")
				passed := passed + 1
			end
		rescue
			print ("  FAIL: " + a_name + "%N")
			if attached (create {EXCEPTION_MANAGER}).last_exception as l_exception and then attached l_exception.description as l_description then
				print ("    " + {UTF_CONVERTER}.string_32_to_utf_8_string_8 (l_description.to_string_32) + "%N")
			end
			failed := failed + 1
			l_retried := True
			retry
		end

end
