// ================================================================================
// ================================================================================
// - File:    test_string.h
// - Purpose: This file contains prototypes for functions that will test the  
//            dynamically allocated string 
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 31, 2022
// - Version: 0.1
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef test_string_H
#define test_string_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
// ================================================================================
// ================================================================================
// TEST STRING 

void test_init_string_nominal(void **state);
// --------------------------------------------------------------------------------

void test_init_string_empty(void **state);
// --------------------------------------------------------------------------------

void test_init_string_null(void **state);
// --------------------------------------------------------------------------------

void test_free_string_nominal(void **state);
// --------------------------------------------------------------------------------

void test_free_string_null(void **state);
// --------------------------------------------------------------------------------

void test_init_string_special_chars(void **state);
// --------------------------------------------------------------------------------

void test_init_string_long(void **state);
// --------------------------------------------------------------------------------

void test_getters_null(void **state);
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
    void test_string_cleanup_gbc(void **state);
#endif
// -------------------------------------------------------------------------------- 

/* Test cases for string concatenation */
void test_concat_string_nominal(void **state);
// --------------------------------------------------------------------------------

void test_concat_literal_nominal(void **state);
// --------------------------------------------------------------------------------

void test_concat_empty_string(void **state);
// --------------------------------------------------------------------------------

void test_concat_empty_literal(void **state);
// --------------------------------------------------------------------------------

void test_concat_null_inputs(void **state);
// --------------------------------------------------------------------------------

void test_concat_reallocation(void **state);
// --------------------------------------------------------------------------------

void test_concat_multiple_times(void **state);
// --------------------------------------------------------------------------------

void test_concat_special_characters(void **state);
// --------------------------------------------------------------------------------

void test_concat_large_strings(void **state);
// -------------------------------------------------------------------------------- 

void test_compare_strings_equal(void **state);
// -------------------------------------------------------------------------------- 

void test_compare_strings_less(void **state);
// --------------------------------------------------------------------------------

void test_compare_strings_greater(void **state);
// --------------------------------------------------------------------------------

void test_compare_strings_different_lengths(void **state);
// --------------------------------------------------------------------------------

void test_compare_strings_empty(void **state);
// -------------------------------------------------------------------------------- 

void test_compare_strings_null(void **state);
// -------------------------------------------------------------------------------- 

void test_compare_strings_case_sensitivity(void **state);
// --------------------------------------------------------------------------------

void test_copy_string_nominal(void **state);
// --------------------------------------------------------------------------------

void test_copy_string_empty(void **state);
// --------------------------------------------------------------------------------

void test_copy_string_null(void **state);
// --------------------------------------------------------------------------------

void test_reserve_string_nominal(void **state);
// --------------------------------------------------------------------------------

void test_reserve_string_smaller_size(void **state);
// --------------------------------------------------------------------------------

void test_reserve_string_null(void **state);
// --------------------------------------------------------------------------------

void test_trim_string_nominal(void **state);
// -------------------------------------------------------------------------------- 

void test_trim_already_minimal(void **state);
// -------------------------------------------------------------------------------- 

void test_trim_empty_string(void **state);
// --------------------------------------------------------------------------------

void test_trim_null_string(void **state);
// --------------------------------------------------------------------------------

void test_first_char_occurance_nominal(void **state);
// -------------------------------------------------------------------------------- 

void test_first_char_occurance_not_found(void **state);
// -------------------------------------------------------------------------------- 

void test_first_char_occurance_empty_string(void **state);
// -------------------------------------------------------------------------------- 

void test_first_char_occurance_null(void **state);
// -------------------------------------------------------------------------------- 

void test_last_char_occurance_nominal(void **state);
// -------------------------------------------------------------------------------- 

void test_last_char_occurance_first_char(void **state);
// -------------------------------------------------------------------------------- 

void test_last_char_occurance_not_found(void **state);
// -------------------------------------------------------------------------------- 

void test_last_char_occurance_empty_string(void **state);
// -------------------------------------------------------------------------------- 

void test_last_char_occurance_null(void **state);
// --------------------------------------------------------------------------------

void test_string_start_pointer_nominal(void **state);
// -------------------------------------------------------------------------------- 

void test_string_start_pointer_empty(void **state);
// --------------------------------------------------------------------------------

void test_string_start_pointer_null(void **state);
// --------------------------------------------------------------------------------

void test_string_end_pointer_nominal(void **state);
// -------------------------------------------------------------------------------- 

void test_string_end_pointer_single_char(void **state);
// --------------------------------------------------------------------------------

void test_string_end_pointer_empty(void **state);
// -------------------------------------------------------------------------------- 

void test_string_end_pointer_null(void **state);
// --------------------------------------------------------------------------------

void test_pointer_arithmetic(void **state);
// -------------------------------------------------------------------------------- 

void test_first_substr_literal_nominal(void **state);
// -------------------------------------------------------------------------------- 

void test_first_substr_literal_middle(void **state);
// -------------------------------------------------------------------------------- 

void test_first_substr_literal_not_found(void **state);
// --------------------------------------------------------------------------------

void test_first_substr_literal_empty_substring(void **state);
// --------------------------------------------------------------------------------

void test_first_substr_literal_too_long(void **state);
// -------------------------------------------------------------------------------- 

void test_first_substr_string_nominal(void **state);
// -------------------------------------------------------------------------------- 

void test_first_substr_string_middle(void **state);
// --------------------------------------------------------------------------------

void test_first_substr_string_not_found(void **state);
// --------------------------------------------------------------------------------

void test_first_substr_string_empty(void **state);
// -------------------------------------------------------------------------------- 

void test_first_substr_string_too_long(void **state);
// -------------------------------------------------------------------------------- 

void test_first_substr_null_inputs(void **state);
// --------------------------------------------------------------------------------

void test_last_substr_literal_nominal(void **state);
// --------------------------------------------------------------------------------

void test_last_substr_literal_single_occurrence(void **state);
// --------------------------------------------------------------------------------

void test_last_substr_literal_not_found(void **state);
// -------------------------------------------------------------------------------- 

void test_last_substr_literal_empty_substring(void **state);
// --------------------------------------------------------------------------------

void test_last_substr_literal_too_long(void **state);
// --------------------------------------------------------------------------------

void test_last_substr_string_nominal(void **state);
// --------------------------------------------------------------------------------

void test_last_substr_string_single_occurrence(void **state);
// --------------------------------------------------------------------------------

void test_last_substr_string_not_found(void **state);
// --------------------------------------------------------------------------------

void test_last_substr_string_empty(void **state);
// --------------------------------------------------------------------------------

void test_last_substr_string_too_long(void **state);
// --------------------------------------------------------------------------------

void test_is_string_ptr_within_bounds(void **state);
// --------------------------------------------------------------------------------

void test_is_string_ptr_at_boundaries(void **state);
// --------------------------------------------------------------------------------

void test_is_string_ptr_outside_bounds(void **state);
// --------------------------------------------------------------------------------

void test_is_string_ptr_empty_string(void **state);
// --------------------------------------------------------------------------------

void test_is_string_ptr_null_inputs(void **state);
// --------------------------------------------------------------------------------

void test_drop_substring_literal_nominal(void **state);
// --------------------------------------------------------------------------------

void test_drop_substring_literal_with_spaces(void **state);
// --------------------------------------------------------------------------------

void test_drop_substring_literal_partial_range(void **state);
// --------------------------------------------------------------------------------

void test_drop_substring_string_nominal(void **state);
// --------------------------------------------------------------------------------

void test_drop_substring_string_with_spaces(void **state);
// --------------------------------------------------------------------------------

void test_drop_substring_not_found(void **state);
// --------------------------------------------------------------------------------

void test_drop_substring_invalid_range(void **state);
// -------------------------------------------------------------------------------- 

void test_replace_substr_literal_nominal(void **state);
// --------------------------------------------------------------------------------

void test_replace_substr_literal_shorter_replacement(void **state);
// --------------------------------------------------------------------------------

void test_replace_substr_literal_longer_replacement(void **state);
// -------------------------------------------------------------------------------- 

void test_replace_substr_literal_partial_range(void **state);
// -------------------------------------------------------------------------------- 

void test_replace_substr_string_nominal(void **state);
// -------------------------------------------------------------------------------- 

void test_replace_substr_string_shorter_replacement(void **state);
// -------------------------------------------------------------------------------- 

void test_replace_substr_string_longer_replacement(void **state);
// -------------------------------------------------------------------------------- 

void test_replace_substr_string_partial_range(void **state);
// -------------------------------------------------------------------------------- 

void test_replace_substr_invalid_range(void **state);
// --------------------------------------------------------------------------------

void test_uppercase_char_nominal(void **state);
// --------------------------------------------------------------------------------

void test_lowercase_char_nominal(void **state);
// -------------------------------------------------------------------------------- 

void test_uppercase_char_null_value(void **state);
// --------------------------------------------------------------------------------

void test_lowercase_char_null_value(void **state);
// --------------------------------------------------------------------------------

void test_uppercase_string_nominal(void **state);
// --------------------------------------------------------------------------------

void test_lowercase_string_nominal(void **state);
// --------------------------------------------------------------------------------

void test_uppercase_string_null(void **state);
// --------------------------------------------------------------------------------

void test_lowercase_string_null(void **state);
// --------------------------------------------------------------------------------

void test_pop_token_nominal(void **state);
// --------------------------------------------------------------------------------

void test_pop_token_multiple_tokens(void **state);
// --------------------------------------------------------------------------------

void test_pop_token_at_end(void **state);
// --------------------------------------------------------------------------------

void test_pop_token_no_token(void **state);
// --------------------------------------------------------------------------------

void test_pop_token_empty_string(void **state);
// --------------------------------------------------------------------------------

void test_pop_token_null_input(void **state);
// --------------------------------------------------------------------------------

void test_pop_token_single_char(void **state);
// --------------------------------------------------------------------------------

void test_pop_token_consecutive_tokens(void **state);
// --------------------------------------------------------------------------------

void test_token_count_single_delimiter(void **state);
// --------------------------------------------------------------------------------

void test_token_count_multiple_delimiters(void **state);
// --------------------------------------------------------------------------------

void test_token_count_consecutive_delimiters(void **state);
// --------------------------------------------------------------------------------

void test_token_count_leading_trailing_delimiters(void **state);
// --------------------------------------------------------------------------------

void test_token_count_single_token(void **state);
// --------------------------------------------------------------------------------

void test_token_count_empty_string(void **state);
// --------------------------------------------------------------------------------

void test_token_count_null_inputs(void **state);
// --------------------------------------------------------------------------------

void test_token_count_only_delimiters(void **state);
// --------------------------------------------------------------------------------

void test_token_count_mixed_delimiters(void **state);
// --------------------------------------------------------------------------------

void test_get_char_nominal(void **state);
// --------------------------------------------------------------------------------

void test_get_char_out_of_bounds(void **state);
// --------------------------------------------------------------------------------

void test_get_char_null(void **state);
// --------------------------------------------------------------------------------

void test_replace_char_nominal(void **state);
// --------------------------------------------------------------------------------

void test_replace_char_out_of_bounds(void **state);
// --------------------------------------------------------------------------------

void test_replace_char_null(void **state);
// --------------------------------------------------------------------------------

void test_replace_char_special_chars(void **state);
// --------------------------------------------------------------------------------

void test_trim_leading_nominal(void **state);
  // --------------------------------------------------------------------------------

void test_trim_leading_multiple_types(void **state);
// --------------------------------------------------------------------------------

void test_trim_leading_no_whitespace(void **state);
// --------------------------------------------------------------------------------

void test_trim_trailing_nominal(void **state);
// --------------------------------------------------------------------------------

void test_trim_trailing_multiple_types(void **state);
// --------------------------------------------------------------------------------

void test_trim_trailing_no_whitespace(void **state);
// --------------------------------------------------------------------------------

void test_trim_all_nominal(void **state);
// --------------------------------------------------------------------------------

void test_trim_all_multiple_types(void **state);
// --------------------------------------------------------------------------------

void test_trim_all_no_whitespace(void **state);
// --------------------------------------------------------------------------------

void test_trim_null_input(void **state);
// --------------------------------------------------------------------------------

void test_trim_all_whitespace_string(void **state);
// --------------------------------------------------------------------------------

void test_string_iterator_forward(void **state);
// --------------------------------------------------------------------------------

void test_string_iterator_reverse(void **state);
// --------------------------------------------------------------------------------

void test_string_iterator_empty_string(void **state);
// --------------------------------------------------------------------------------

void test_string_iterator_null_string(void **state);
// --------------------------------------------------------------------------------

void test_string_iterator_bounds(void **state);
// --------------------------------------------------------------------------------

void test_tokenize_basic(void **state);
// --------------------------------------------------------------------------------

void test_tokenize_multiple_delimiters(void **state);
// --------------------------------------------------------------------------------

void test_tokenize_consecutive_delimiters(void **state);
// --------------------------------------------------------------------------------

void test_tokenize_empty_string(void **state);
// --------------------------------------------------------------------------------

void test_tokenize_no_delimiters_found(void **state);
// --------------------------------------------------------------------------------

void test_tokenize_only_delimiters(void **state);
// --------------------------------------------------------------------------------

void test_tokenize_null_inputs(void **state);
// --------------------------------------------------------------------------------

void test_tokenize_empty_delimiter(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST STRING VECTOR 

void test_str_vector_init(void **state);
// --------------------------------------------------------------------------------

void test_str_vector_push_back(void **state);
// --------------------------------------------------------------------------------

void test_str_vector_auto_resize(void **state);
// --------------------------------------------------------------------------------

void test_str_vector_null_inputs(void **state);
// --------------------------------------------------------------------------------

void test_str_vector_empty_string(void **state);
// --------------------------------------------------------------------------------

void test_str_vector_multiple_pushes(void **state);
// --------------------------------------------------------------------------------

void test_str_vector_gbc(void **state);
// --------------------------------------------------------------------------------

void test_push_front_empty_vector(void **state);
// -------------------------------------------------------------------------------- 

void test_push_front_existing_elements(void **state);
// -------------------------------------------------------------------------------- 

void test_push_front_reallocation(void **state);
// -------------------------------------------------------------------------------- 

void test_push_front_multiple(void **state);
// -------------------------------------------------------------------------------- 

void test_push_front_null_inputs(void **state);
// -------------------------------------------------------------------------------- 

void test_push_front_empty_string(void **state);
// --------------------------------------------------------------------------------

void test_insert_str_vector_middle(void **state);
// --------------------------------------------------------------------------------

void test_insert_str_vector_start(void **state);
// --------------------------------------------------------------------------------

void test_insert_str_vector_end(void **state);
// --------------------------------------------------------------------------------

void test_insert_str_vector_reallocation(void **state);
// --------------------------------------------------------------------------------

void test_insert_str_vector_invalid_index(void **state);
// --------------------------------------------------------------------------------

void test_insert_str_vector_null_inputs(void **state);
// --------------------------------------------------------------------------------

void test_insert_str_vector_empty_string(void **state);
// --------------------------------------------------------------------------------

void test_pop_back_nominal(void **state);
// --------------------------------------------------------------------------------

void test_pop_front_nominal(void **state);
// --------------------------------------------------------------------------------

void test_pop_empty_vector(void **state);
// --------------------------------------------------------------------------------

void test_pop_multiple_back(void **state);
// --------------------------------------------------------------------------------

void test_pop_multiple_front(void **state);
// --------------------------------------------------------------------------------

void test_pop_null_vector(void **state);
// --------------------------------------------------------------------------------

void test_pop_alternating(void **state);
// --------------------------------------------------------------------------------

void test_pop_any_nominal(void **state);
// --------------------------------------------------------------------------------

void test_pop_any_first(void **state);
// --------------------------------------------------------------------------------

void test_pop_any_last(void **state);
// --------------------------------------------------------------------------------

void test_pop_any_invalid_index(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_any_empty_vector(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_any_null_vector(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_any_multiple(void **state);
// --------------------------------------------------------------------------------

void test_sort_empty_vector(void **state);
// --------------------------------------------------------------------------------

void test_sort_single_element(void **state);
// --------------------------------------------------------------------------------

void test_sort_forward_already_sorted(void **state);
// --------------------------------------------------------------------------------

void test_sort_forward_reverse_sorted(void **state);
// --------------------------------------------------------------------------------

void test_sort_reverse_already_sorted(void **state);
// --------------------------------------------------------------------------------

void test_sort_large_vector(void **state);
// -------------------------------------------------------------------------------- 

void test_sort_duplicate_elements(void **state);
// --------------------------------------------------------------------------------

void test_sort_with_empty_strings(void **state);
// -------------------------------------------------------------------------------- 

void test_sort_null_vector(void **state);
// --------------------------------------------------------------------------------

void test_delete_back_nominal(void **state);
// --------------------------------------------------------------------------------

void test_delete_front_nominal(void **state);
// --------------------------------------------------------------------------------

void test_delete_empty_vector(void **state);
// --------------------------------------------------------------------------------

void test_delete_multiple_back(void **state);
// --------------------------------------------------------------------------------

void test_delete_multiple_front(void **state);
// --------------------------------------------------------------------------------

void test_delete_null_vector(void **state);
// --------------------------------------------------------------------------------

void test_delete_alternating(void **state);
// --------------------------------------------------------------------------------

void test_delete_any_nominal(void **state);
// --------------------------------------------------------------------------------

void test_delete_any_first(void **state);
// --------------------------------------------------------------------------------

void test_delete_any_last(void **state);
// --------------------------------------------------------------------------------

void test_delete_any_invalid_index(void **state);
// -------------------------------------------------------------------------------- 

void test_delete_any_empty_vector(void **state);
// -------------------------------------------------------------------------------- 

void test_delete_any_null_vector(void **state);
// -------------------------------------------------------------------------------- 

void test_delete_any_multiple(void **state);
// --------------------------------------------------------------------------------

void test_reverse_str_vector(void **state);
// ================================================================================
// ================================================================================ 
#endif /* test_string_H */
// ================================================================================
// ================================================================================
// eof
