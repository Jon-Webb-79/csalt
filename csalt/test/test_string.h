// ================================================================================
// ================================================================================
// - File:    test_string.h
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

#ifndef test_str_H
#define test_str_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <unistd.h>
#include <fcntl.h>

#include "../include/str.h"

void test_string_init_one_var(void **state);
void test_string_init_two_var(void **state);
void test_string_init_two_var_less(void **state);
#ifdef __GNUC__
void test_string_init_gbc(void **state);
#endif
void test_string_init_post_free(void **state);
void test_get_string_fail_one(void **state);
void test_get_string_fail_two(void **state);
void test_string_length_fail_one(void **state);
void test_string_length_fail_two(void **state);
void test_insert_string_lit_insert_end(void **state);
void test_insert_string_lit_insert_middle(void **state);
void test_insert_string_lit_error_one(void **state);
void test_insert_string_lit_error_two(void **state);
void test_insert_string_lit_error_three(void **state);
void test_insert_string_str_insert_end(void **state);
void test_insert_string_str_insert_middle(void **state);
void test_trim_string_equal(void **state);
void test_trim_string_greater(void **state);
void test_trim_string_error_one(void **state);
void test_trim_string_error_two(void **state);
void test_copy_string(void **state);
#ifdef __GNUC__
void test_copy_string_w_gbc(void **state);
#endif
void test_compare_strings_lit_equal(void **state);
void test_compare_strings_lit_greater(void **state);
void test_compare_strings_lit_less(void **state);
void test_compare_strings_lit_oversize(void **state);
void test_compare_strings_str_equal(void **state);
void test_compare_strings_str_greater(void **state);
void test_compare_strings_str_less(void **state);
void test_compare_strings_str_oversize(void **state);
void test_find_first_char(void **state);
void test_find_first_char_does_not_exist(void **state);
void test_find_first_char_null_terminator(void **state);
void test_find_last_char(void **state);
void test_find_last_char_does_not_exist(void **state);
void test_find_last_char_null_terminator(void **state);
void test_find_first_string_lit(void **state);
void test_find_first_string_lit_null(void **state);
void test_find_first_string_lit_oversized(void **state);
void test_find_first_string_str(void **state);
void test_find_first_string_str_null(void **state);
void test_find_first_string_str_oversized(void **state);
void test_find_last_string_lit(void **state);
void test_find_last_string_lit_null(void **state);
void test_find_last_string_lit_oversized(void **state);
void test_find_last_string_str(void **state);
void test_find_last_string_str_null(void **state);
void test_find_last_string_str_oversized(void **state);
void test_pop_string_end_char(void **state);
void test_pop_string_end_null_struct(void **state);
void test_pop_string_end_null_data(void **state);
void test_pop_string_char(void **state);
void test_pop_string_null_struct(void **state);
void test_pop_string_null_data(void **state);
void test_pop_string_out_of_bounds(void **state);
void test_pop_string_token(void **state);
void test_pop_string_token_no_token(void **state);
void test_pop_string_token_null_struct(void **state);
void test_pop_string_token_null_data(void **state);
void test_pop_string_token_gbc_true(void **state);
#ifdef __GNUC__
void test_pop_string_token_no_token_gbc_true(void **state);
#endif
// ================================================================================
// ================================================================================
#endif /* test_str_H */
// ================================================================================
// ================================================================================
// eof
