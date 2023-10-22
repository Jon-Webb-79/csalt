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
        cmocka_unit_test(test_string_init_two_var_less_gbc)
	};
	return cmocka_run_group_tests(test_string, NULL, NULL);
}
// ================================================================================
// ================================================================================
// eof

