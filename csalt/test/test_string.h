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

void uppercase_char(char* a);

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
void test_find_first_char_btw_pointers(void **state);
void test_find_first_char_does_not_exist(void **state);
void test_find_first_char_null_terminator(void **state);
void test_find_last_char(void **state);
void test_find_last_char_btw_pointers(void **state);
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
void test_char_iterator_for_loop(void **state);
void test_char_iterator_while_loop(void **state);
void test_char_iterator_forward(void **state);
void test_char_iterator_reverse(void **state);
void test_convert_to_uppercase(void **state);
void test_convert_to_lowercase(void **state);
void test_ptr_in_container(void **state);
void test_ptr_not_in_container(void **state);
void test_ptr_in_literal(void **state);
void test_ptr_not_in_literal(void **state);
void test_find_first_literal_btw_pointers(void **state);
void test_find_first_str_btw_pointers(void **state);
void test_find_last_str_between_pointers(void **state);
void test_drop_str_substring_char(void **state);
void test_drop_str_substring_str(void **state);
// ================================================================================
// ================================================================================
// Test replace sub-string functions

void test_replace_string_equal_size(void **state);
void test_replace_string_smaller_size(void **state);
void test_replace_string_larger_size(void **state);
void test_replace_string_equal_size_ptr(void **state);
void test_replace_string_smaller_size_ptr(void **state);
void test_replace_string_larger_size_ptr(void **state);
void test_replace_literal_equal_size(void **state);
void test_replace_literal_smaller_size(void **state);
void test_replace_literal_larger_size(void **state);
// ================================================================================
// ================================================================================
#endif /* test_str_H */
// ================================================================================
// ================================================================================
// eof
