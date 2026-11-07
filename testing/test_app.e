note
	description: "Test application for simple_clipboard"
	date: "$Date$"
	revision: "$Revision$"

class
	TEST_APP

create
	make

feature -- Initialization

	make
			-- Run the tests.
		do
			create tests
			run_test (agent tests.test_basic_set_get, "test_basic_set_get")
			run_test (agent tests.test_has_text_query, "test_has_text_query")
			run_test (agent tests.test_clear_clipboard, "test_clear_clipboard")
			run_test (agent tests.test_multiline_text, "test_multiline_text")
			run_test (agent tests.test_format_count_query, "test_format_count_query")
			run_test (agent tests.test_copy_text_alias, "test_copy_text_alias")
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
			failed := failed + 1
			l_retried := True
			retry
		end

end
