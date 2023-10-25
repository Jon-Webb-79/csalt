// ================================================================================
// ================================================================================
// - File:    unit_test.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    October 20, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "test_string.h"

// Begin code
int main(int argc, const char * argv[]) {
	const struct CMUnitTest test_string[] = {
	    cmocka_unit_test(test_string_init_one_var),
        cmocka_unit_test(test_string_init_two_var),
        cmocka_unit_test(test_string_init_two_var_less),
        cmocka_unit_test(test_string_init_one_var_gbc),
        cmocka_unit_test(test_string_init_two_var_gbc),
        cmocka_unit_test(test_string_init_two_var_less_gbc),
        cmocka_unit_test(test_get_string_fail_one),
        cmocka_unit_test(test_get_string_fail_two),
        cmocka_unit_test(test_string_length_fail_one),
        cmocka_unit_test(test_string_length_fail_two),
        cmocka_unit_test(test_insert_string_lit_insert_end),
        cmocka_unit_test(test_insert_string_lit_insert_middle),
        cmocka_unit_test(test_insert_string_lit_error_one),
        cmocka_unit_test(test_insert_string_lit_error_two),
        cmocka_unit_test(test_insert_string_lit_error_three),
        cmocka_unit_test(test_insert_string_str_insert_end),
        cmocka_unit_test(test_insert_string_str_insert_middle),
        cmocka_unit_test(test_trim_string_equal),
        cmocka_unit_test(test_trim_string_greater),
        cmocka_unit_test(test_trim_string_error_one),
        cmocka_unit_test(test_trim_string_error_two),
        cmocka_unit_test(test_copy_string),
        cmocka_unit_test(test_compare_strings_lit_equal),
        cmocka_unit_test(test_compare_strings_lit_greater),
        cmocka_unit_test(test_compare_strings_lit_less),
        cmocka_unit_test(test_compare_strings_lit_oversize),
        cmocka_unit_test(test_compare_strings_str_equal),
        cmocka_unit_test(test_compare_strings_str_greater),
        cmocka_unit_test(test_compare_strings_str_less),
        cmocka_unit_test(test_compare_strings_str_oversize),
        cmocka_unit_test(test_find_first_char),
        cmocka_unit_test(test_find_first_char_does_not_exist),
        cmocka_unit_test(test_find_first_char_null_terminator),
        cmocka_unit_test(test_find_last_char),
        cmocka_unit_test(test_find_last_char_does_not_exist),
        cmocka_unit_test(test_find_last_char_null_terminator),
        cmocka_unit_test(test_find_first_string_lit),
        cmocka_unit_test(test_find_first_string_lit_null),
        cmocka_unit_test(test_find_first_string_lit_oversized),
        cmocka_unit_test(test_find_first_string_str),
        cmocka_unit_test(test_find_first_string_str_null),
        cmocka_unit_test(test_find_first_string_str_oversized),
	};
	return cmocka_run_group_tests(test_string, NULL, NULL);
}
// ================================================================================
// ================================================================================
// eof

