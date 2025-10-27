// ================================================================================
// ================================================================================
// - File:    test_error.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 31, 2022
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef test_H
#define test_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
// ================================================================================ 
// ================================================================================ 
// TEST ERROR_TO_STRING 

void test_error_to_string_no_error(void **state);
void test_error_to_string_invalid_error(void **state);

void test_error_to_string_argument_cases(void **state);
void test_error_to_string_memory_cases(void **state);
void test_error_to_string_state_cases(void **state);
void test_error_to_string_math_cases(void **state);
void test_error_to_string_io_cases(void **state);
void test_error_to_string_format_cases(void **state);
void test_error_to_string_concurrency_cases(void **state);
void test_error_to_string_config_cases(void **state);
void test_error_to_string_generic_cases(void **state);

void test_error_to_string_fallback_unrecognized(void **state);
void test_error_to_string_pointer_stability(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST ERROR_CAT_TO_STRING 

void test_error_cat_to_string_all(void **state);
void test_error_cat_to_string_unknown(void **state);
void test_error_cat_to_string_pointer_stability(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST ERRNO_FROM_ERROR 

void test_set_errno_no_error(void **state);
void test_set_errno_invalid_error(void **state);
void test_set_errno_argument_group(void **state);
void test_set_errno_uninitialized_special(void **state);
void test_set_errno_memory_group(void **state);
void test_set_errno_overflow_group(void **state);
void test_set_errno_state_group(void **state);
void test_set_errno_math_group(void **state);
void test_set_errno_io_group(void **state);
void test_set_errno_format_group(void **state);
void test_set_errno_concurrency_group(void **state);
void test_set_errno_config_group(void **state);
void test_set_errno_generic_group(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST ERROR_FROM_ERRNO

void test_error_from_errno_zero_success(void **state);
void test_error_from_errno_core_mappings(void **state);
void test_error_from_errno_wouldblock_again(void **state);
void test_error_from_errno_optional_codes(void **state);
void test_error_from_errno_deadlock_aliases(void **state);
void test_error_from_errno_notsup_aliases(void **state);
void test_error_from_errno_does_not_touch_errno(void **state);
void test_error_from_errno_unknown_fallback(void **state);
// ================================================================================ 
// ================================================================================ 
//TEST BOOL FUNCTIONS

void test_ec_predicates_each_category(void **state);
void test_ec_predicates_nonerror_values(void **state);
void test_ec_predicates_unknown_negative(void **state);
// ================================================================================ 
// ================================================================================ 
#endif /* test_H */
// ================================================================================
// ================================================================================
// eof
