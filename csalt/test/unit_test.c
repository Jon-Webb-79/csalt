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
#include "test_vector.h"
#include "test_swap.h"
#include "test_array.h"

const struct CMUnitTest test_swap[] = {
	cmocka_unit_test(test_swap_char),
    cmocka_unit_test(test_swap_uchar),
    cmocka_unit_test(test_swap_short),
    cmocka_unit_test(test_swap_ushort),
    cmocka_unit_test(test_swap_int),
    cmocka_unit_test(test_swap_uint),
    cmocka_unit_test(test_swap_long),
    cmocka_unit_test(test_swap_ulong),
    cmocka_unit_test(test_swap_llong),
    cmocka_unit_test(test_swap_ullong),
    cmocka_unit_test(test_swap_float),
    cmocka_unit_test(test_swap_double),
    cmocka_unit_test(test_swap_ldouble),
    cmocka_unit_test(test_swap_bool),
    cmocka_unit_test(test_swap_string)
};

const struct CMUnitTest test_string[] = {
	cmocka_unit_test(test_string_init_one_var),
    cmocka_unit_test(test_string_init_two_var),
    cmocka_unit_test(test_string_init_two_var_less),
    #ifdef __GNUC__
    cmocka_unit_test(test_string_init_gbc),
    #endif
    cmocka_unit_test(test_string_init_post_free),
    cmocka_unit_test(test_get_string_fail_one),
    cmocka_unit_test(test_get_string_fail_two),
    cmocka_unit_test(test_string_length_fail_one),
    cmocka_unit_test(test_string_length_fail_two),
    cmocka_unit_test(test_insert_string_lit_insert_end),
    cmocka_unit_test(test_insert_string_lit_insert_middle),
    cmocka_unit_test(test_insert_string_lit_error_two),
    cmocka_unit_test(test_insert_string_lit_error_three),
    cmocka_unit_test(test_insert_string_str_insert_end),
    cmocka_unit_test(test_insert_string_str_insert_middle),
    cmocka_unit_test(test_trim_string_equal),
    cmocka_unit_test(test_trim_string_greater),
    cmocka_unit_test(test_trim_string_error_one),
    cmocka_unit_test(test_trim_string_error_two),
    cmocka_unit_test(test_copy_string),
    #ifdef __GNUC__
    cmocka_unit_test(test_copy_string_w_gbc),
    #endif
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
    cmocka_unit_test(test_find_last_string_lit),
    cmocka_unit_test(test_find_last_string_lit_null),
    cmocka_unit_test(test_find_last_string_lit_oversized),
    cmocka_unit_test(test_find_last_string_str),
    cmocka_unit_test(test_find_last_string_str_null),
    cmocka_unit_test(test_find_last_string_str_oversized),
    cmocka_unit_test(test_pop_string_end_char),
    cmocka_unit_test(test_pop_string_end_null_struct),
    cmocka_unit_test(test_pop_string_end_null_data),
    cmocka_unit_test(test_pop_string_char),
    cmocka_unit_test(test_pop_string_null_struct),
    cmocka_unit_test(test_pop_string_null_data),
    cmocka_unit_test(test_pop_string_out_of_bounds),
    cmocka_unit_test(test_pop_string_token),
    cmocka_unit_test(test_pop_string_token_no_token),
    cmocka_unit_test(test_pop_string_token_null_struct),
    cmocka_unit_test(test_pop_string_token_null_data),
    #ifdef __GNUC__
    cmocka_unit_test(test_pop_string_token_gbc_true),
    #endif
    cmocka_unit_test(test_char_iterator_for_loop),
    cmocka_unit_test(test_char_iterator_while_loop),
    cmocka_unit_test(test_char_iterator_forward),
    cmocka_unit_test(test_char_iterator_forward),
    cmocka_unit_test(test_convert_to_uppercase),
    cmocka_unit_test(test_convert_to_lowercase),
    cmocka_unit_test(test_ptr_in_container),
    cmocka_unit_test(test_ptr_not_in_container),
    cmocka_unit_test(test_ptr_in_literal),
    cmocka_unit_test(test_ptr_not_in_literal),
    cmocka_unit_test(test_find_first_char_btw_pointers),
    cmocka_unit_test(test_find_last_char_btw_pointers),
    cmocka_unit_test(test_find_first_literal_btw_pointers),
    cmocka_unit_test(test_find_first_str_btw_pointers),
    cmocka_unit_test(test_find_last_str_between_pointers),
    cmocka_unit_test(test_drop_str_substring_char),
    cmocka_unit_test(test_drop_str_substring_str),
    cmocka_unit_test(test_replace_string_equal_size),
    cmocka_unit_test(test_replace_string_smaller_size),
    cmocka_unit_test(test_replace_string_larger_size),
    cmocka_unit_test(test_replace_string_equal_size_ptr),
    cmocka_unit_test(test_replace_string_smaller_size_ptr),
    cmocka_unit_test(test_replace_string_larger_size_ptr),
    cmocka_unit_test(test_replace_literal_equal_size),
    cmocka_unit_test(test_replace_literal_smaller_size),
    cmocka_unit_test(test_replace_literal_larger_size)
};

// Test vector functions 
const struct CMUnitTest test_vector[] = {
    cmocka_unit_test(test_init_char_vector),
    cmocka_unit_test(test_init_uchar_vector),
    cmocka_unit_test(test_init_short_vector),
    cmocka_unit_test(test_init_ushort_vector),
    cmocka_unit_test(test_init_int_vector),
    cmocka_unit_test(test_init_uint_vector),
    cmocka_unit_test(test_init_long_vector),
    cmocka_unit_test(test_init_ulong_vector),
    cmocka_unit_test(test_init_llong_vector),
    cmocka_unit_test(test_init_ullong_vector),
    cmocka_unit_test(test_init_float_vector),
    cmocka_unit_test(test_init_double_vector),
    cmocka_unit_test(test_init_ldouble_vector),
    cmocka_unit_test(test_init_bool_vector),
    cmocka_unit_test(test_init_string_vector),
    cmocka_unit_test(test_init_str_vector),
    cmocka_unit_test(test_get_char_vector_null_struct), 
    cmocka_unit_test(test_get_char_vector_null_data),
    cmocka_unit_test(test_get_char_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_uchar_vector_null_struct), 
    cmocka_unit_test(test_get_uchar_vector_null_data),
    cmocka_unit_test(test_get_uchar_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_short_vector_null_struct), 
    cmocka_unit_test(test_get_short_vector_null_data),
    cmocka_unit_test(test_get_short_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_ushort_vector_null_struct), 
    cmocka_unit_test(test_get_ushort_vector_null_data),
    cmocka_unit_test(test_get_ushort_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_int_vector_null_struct), 
    cmocka_unit_test(test_get_int_vector_null_data),
    cmocka_unit_test(test_get_int_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_uint_vector_null_struct), 
    cmocka_unit_test(test_get_uint_vector_null_data),
    cmocka_unit_test(test_get_uint_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_long_vector_null_struct), 
    cmocka_unit_test(test_get_long_vector_null_data),
    cmocka_unit_test(test_get_long_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_ulong_vector_null_struct), 
    cmocka_unit_test(test_get_ulong_vector_null_data),
    cmocka_unit_test(test_get_ulong_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_llong_vector_null_struct), 
    cmocka_unit_test(test_get_llong_vector_null_data),
    cmocka_unit_test(test_get_llong_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_ullong_vector_null_struct), 
    cmocka_unit_test(test_get_ullong_vector_null_data),
    cmocka_unit_test(test_get_ullong_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_float_vector_null_struct), 
    cmocka_unit_test(test_get_float_vector_null_data),
    cmocka_unit_test(test_get_float_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_double_vector_null_struct), 
    cmocka_unit_test(test_get_double_vector_null_data),
    cmocka_unit_test(test_get_double_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_ldouble_vector_null_struct), 
    cmocka_unit_test(test_get_ldouble_vector_null_data),
    cmocka_unit_test(test_get_ldouble_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_bool_vector_null_struct), 
    cmocka_unit_test(test_get_bool_vector_null_data),
    cmocka_unit_test(test_get_bool_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_string_vector_null_struct), 
    cmocka_unit_test(test_get_string_vector_null_data),
    #ifdef __GNUC__
    cmocka_unit_test(test_pop_char_vector),
    cmocka_unit_test(test_pop_uchar_vector),
    cmocka_unit_test(test_pop_short_vector),
    cmocka_unit_test(test_pop_ushort_vector),
    cmocka_unit_test(test_pop_int_vector),
    cmocka_unit_test(test_pop_uint_vector),
    cmocka_unit_test(test_pop_long_vector),
    cmocka_unit_test(test_pop_ulong_vector),
    cmocka_unit_test(test_pop_llong_vector),
    cmocka_unit_test(test_pop_ullong_vector),
    cmocka_unit_test(test_pop_float_vector),
    cmocka_unit_test(test_pop_double_vector),
    cmocka_unit_test(test_pop_ldouble_vector),
    cmocka_unit_test(test_pop_bool_vector),
    cmocka_unit_test(test_pop_string_vector),
    cmocka_unit_test(test_char_garbage_vector),
    cmocka_unit_test(test_uchar_garbage_vector),
    cmocka_unit_test(test_short_garbage_vector),
    cmocka_unit_test(test_ushort_garbage_vector),
    cmocka_unit_test(test_int_garbage_vector),
    cmocka_unit_test(test_uint_garbage_vector),
    cmocka_unit_test(test_long_garbage_vector),
    cmocka_unit_test(test_ulong_garbage_vector),
    cmocka_unit_test(test_llong_garbage_vector),
    cmocka_unit_test(test_ullong_garbage_vector),
    cmocka_unit_test(test_float_garbage_vector),
    cmocka_unit_test(test_double_garbage_vector),
    cmocka_unit_test(test_ldouble_garbage_vector),
    cmocka_unit_test(test_bool_garbage_vector),
    cmocka_unit_test(test_string_garbage_vector),
    #endif
    cmocka_unit_test(test_reverse_char_vector),
    cmocka_unit_test(test_reverse_uchar_vector),
    cmocka_unit_test(test_reverse_short_vector),
    cmocka_unit_test(test_reverse_ushort_vector),
    cmocka_unit_test(test_reverse_int_vector),
    cmocka_unit_test(test_reverse_uint_vector),
    cmocka_unit_test(test_reverse_long_vector),
    cmocka_unit_test(test_reverse_ulong_vector),
    cmocka_unit_test(test_reverse_llong_vector),
    cmocka_unit_test(test_reverse_ullong_vector),
    cmocka_unit_test(test_reverse_float_vector),
    cmocka_unit_test(test_reverse_double_vector),
    cmocka_unit_test(test_reverse_ldouble_vector),
    cmocka_unit_test(test_reverse_bool_vector),
    cmocka_unit_test(test_reverse_string_vector),
    cmocka_unit_test(test_max_char_vector),
    cmocka_unit_test(test_max_uchar_vector),
    cmocka_unit_test(test_max_short_vector),
    cmocka_unit_test(test_max_ushort_vector),
    cmocka_unit_test(test_max_int_vector),
    cmocka_unit_test(test_max_uint_vector),
    cmocka_unit_test(test_max_long_vector),
    cmocka_unit_test(test_max_ulong_vector),
    cmocka_unit_test(test_max_llong_vector),
    cmocka_unit_test(test_max_ullong_vector),
    cmocka_unit_test(test_max_float_vector),
    cmocka_unit_test(test_max_double_vector),
    cmocka_unit_test(test_max_ldouble_vector),
    cmocka_unit_test(test_min_char_vector),
    cmocka_unit_test(test_min_uchar_vector),
    cmocka_unit_test(test_min_short_vector),
    cmocka_unit_test(test_min_ushort_vector),
    cmocka_unit_test(test_min_int_vector),
    cmocka_unit_test(test_min_uint_vector),
    cmocka_unit_test(test_min_long_vector),
    cmocka_unit_test(test_min_ulong_vector),
    cmocka_unit_test(test_min_llong_vector),
    cmocka_unit_test(test_min_ullong_vector),
    cmocka_unit_test(test_min_float_vector),
    cmocka_unit_test(test_min_double_vector),
    cmocka_unit_test(test_min_ldouble_vector),
    cmocka_unit_test(test_sum_char_vector),
    cmocka_unit_test(test_sum_uchar_vector),
    cmocka_unit_test(test_sum_short_vector),
    cmocka_unit_test(test_sum_ushort_vector),
    cmocka_unit_test(test_sum_int_vector),
    cmocka_unit_test(test_sum_uint_vector),
    cmocka_unit_test(test_sum_long_vector),
    cmocka_unit_test(test_sum_ulong_vector),
    cmocka_unit_test(test_sum_llong_vector),
    cmocka_unit_test(test_sum_ullong_vector),
    cmocka_unit_test(test_sum_float_vector),
    cmocka_unit_test(test_sum_double_vector),
    cmocka_unit_test(test_sum_ldouble_vector),
    cmocka_unit_test(test_average_char_vector),
    cmocka_unit_test(test_average_uchar_vector),
    cmocka_unit_test(test_average_short_vector),
    cmocka_unit_test(test_average_ushort_vector),
    cmocka_unit_test(test_average_int_vector),
    cmocka_unit_test(test_average_uint_vector),
    cmocka_unit_test(test_average_long_vector),
    cmocka_unit_test(test_average_ulong_vector),
    cmocka_unit_test(test_average_llong_vector),
    cmocka_unit_test(test_average_ullong_vector),
    cmocka_unit_test(test_average_float_vector),
    cmocka_unit_test(test_average_double_vector),
    cmocka_unit_test(test_average_ldouble_vector),
    cmocka_unit_test(test_stdev_char_vector),
    cmocka_unit_test(test_stdev_uchar_vector),
    cmocka_unit_test(test_stdev_short_vector),
    cmocka_unit_test(test_stdev_ushort_vector),
    cmocka_unit_test(test_stdev_int_vector),
    cmocka_unit_test(test_stdev_uint_vector),
    cmocka_unit_test(test_stdev_long_vector),
    cmocka_unit_test(test_stdev_ulong_vector),
    cmocka_unit_test(test_stdev_llong_vector),
    cmocka_unit_test(test_stdev_ullong_vector),
    cmocka_unit_test(test_stdev_float_vector),
    cmocka_unit_test(test_stdev_double_vector),
    cmocka_unit_test(test_stdev_ldouble_vector),
    cmocka_unit_test(test_cumsum_char_vector),
    cmocka_unit_test(test_cumsum_uchar_vector),
    cmocka_unit_test(test_cumsum_short_vector),
    cmocka_unit_test(test_cumsum_ushort_vector),
    cmocka_unit_test(test_cumsum_int_vector),
    cmocka_unit_test(test_cumsum_uint_vector),
    cmocka_unit_test(test_cumsum_long_vector),
    cmocka_unit_test(test_cumsum_ulong_vector),
    cmocka_unit_test(test_cumsum_llong_vector),
    cmocka_unit_test(test_cumsum_ullong_vector),
    cmocka_unit_test(test_cumsum_float_vector),
    cmocka_unit_test(test_cumsum_double_vector),
    cmocka_unit_test(test_cumsum_ldouble_vector),
    cmocka_unit_test(test_copy_char_vector),
    cmocka_unit_test(test_copy_uchar_vector),
    cmocka_unit_test(test_copy_short_vector),
    cmocka_unit_test(test_copy_ushort_vector),
    cmocka_unit_test(test_copy_int_vector),
    cmocka_unit_test(test_copy_uint_vector),
    cmocka_unit_test(test_copy_long_vector),
    cmocka_unit_test(test_copy_ulong_vector),
    cmocka_unit_test(test_copy_llong_vector),
    cmocka_unit_test(test_copy_ullong_vector),
    cmocka_unit_test(test_copy_float_vector),
    cmocka_unit_test(test_copy_double_vector),
    cmocka_unit_test(test_copy_ldouble_vector),
    cmocka_unit_test(test_copy_bool_vector),
    cmocka_unit_test(test_copy_string_vector),
    cmocka_unit_test(test_trim_char_vector),
    cmocka_unit_test(test_trim_uchar_vector),
    cmocka_unit_test(test_trim_short_vector),
    cmocka_unit_test(test_trim_ushort_vector),
    cmocka_unit_test(test_trim_int_vector),
    cmocka_unit_test(test_trim_uint_vector),
    cmocka_unit_test(test_trim_long_vector),
    cmocka_unit_test(test_trim_ulong_vector),
    cmocka_unit_test(test_trim_llong_vector),
    cmocka_unit_test(test_trim_ullong_vector),
    cmocka_unit_test(test_trim_float_vector),
    cmocka_unit_test(test_trim_double_vector),
    cmocka_unit_test(test_trim_ldouble_vector),
    cmocka_unit_test(test_trim_bool_vector),
    cmocka_unit_test(test_copy_string_vector),
    cmocka_unit_test(test_replace_char_vector),
    cmocka_unit_test(test_replace_uchar_vector),
    cmocka_unit_test(test_replace_short_vector),
    cmocka_unit_test(test_replace_ushort_vector),
    cmocka_unit_test(test_replace_int_vector),
    cmocka_unit_test(test_replace_uint_vector),
    cmocka_unit_test(test_replace_long_vector),
    cmocka_unit_test(test_replace_ulong_vector),
    cmocka_unit_test(test_replace_llong_vector),
    cmocka_unit_test(test_replace_ullong_vector),
    cmocka_unit_test(test_replace_float_vector),
    cmocka_unit_test(test_replace_double_vector),
    cmocka_unit_test(test_replace_ldouble_vector),
    cmocka_unit_test(test_replace_bool_vector),
    cmocka_unit_test(test_replace_string_vector),
    cmocka_unit_test(test_replace_str_vector),
    cmocka_unit_test(test_iter_char_vector),
    cmocka_unit_test(test_iter_uchar_vector),
    cmocka_unit_test(test_iter_short_vector),
    cmocka_unit_test(test_iter_ushort_vector),
    cmocka_unit_test(test_iter_int_vector),
    cmocka_unit_test(test_iter_uint_vector),
    cmocka_unit_test(test_iter_long_vector),
    cmocka_unit_test(test_iter_ulong_vector),
    cmocka_unit_test(test_iter_llong_vector),
    cmocka_unit_test(test_iter_ullong_vector),
    cmocka_unit_test(test_iter_float_vector),
    cmocka_unit_test(test_iter_double_vector),
    cmocka_unit_test(test_iter_ldouble_vector),
    cmocka_unit_test(test_iter_bool_vector),
    cmocka_unit_test(test_iter_string_vector),
    cmocka_unit_test(test_bubble_sort_char_vector_forward),
    cmocka_unit_test(test_bubble_sort_char_vector_reverse),
    cmocka_unit_test(test_bubble_sort_uchar_vector_forward),
    cmocka_unit_test(test_bubble_sort_uchar_vector_reverse),
    cmocka_unit_test(test_bubble_sort_short_vector_forward),
    cmocka_unit_test(test_bubble_sort_short_vector_reverse),
    cmocka_unit_test(test_bubble_sort_ushort_vector_forward),
    cmocka_unit_test(test_bubble_sort_ushort_vector_reverse),
    cmocka_unit_test(test_bubble_sort_int_vector_forward),
    cmocka_unit_test(test_bubble_sort_int_vector_reverse),
    cmocka_unit_test(test_bubble_sort_uint_vector_forward),
    cmocka_unit_test(test_bubble_sort_uint_vector_reverse),
    cmocka_unit_test(test_bubble_sort_long_vector_forward),
    cmocka_unit_test(test_bubble_sort_long_vector_reverse),
    cmocka_unit_test(test_bubble_sort_ulong_vector_forward),
    cmocka_unit_test(test_bubble_sort_ulong_vector_reverse),
    cmocka_unit_test(test_bubble_sort_llong_vector_forward),
    cmocka_unit_test(test_bubble_sort_llong_vector_reverse),
    cmocka_unit_test(test_bubble_sort_ullong_vector_forward),
    cmocka_unit_test(test_bubble_sort_ullong_vector_reverse),
    cmocka_unit_test(test_bubble_sort_float_vector_forward),
    cmocka_unit_test(test_bubble_sort_float_vector_reverse),
    cmocka_unit_test(test_bubble_sort_double_vector_forward),
    cmocka_unit_test(test_bubble_sort_double_vector_reverse),
    cmocka_unit_test(test_bubble_sort_ldouble_vector_forward),
    cmocka_unit_test(test_bubble_sort_ldouble_vector_reverse),
    cmocka_unit_test(test_bubble_sort_bool_vector_forward),
    cmocka_unit_test(test_bubble_sort_bool_vector_reverse),
    cmocka_unit_test(test_bubble_sort_string_vector_forward),
    cmocka_unit_test(test_bubble_sort_string_vector_reverse),
    cmocka_unit_test(test_insert_sort_char_vector_forward),
    cmocka_unit_test(test_insert_sort_char_vector_reverse),
    cmocka_unit_test(test_insert_sort_uchar_vector_forward),
    cmocka_unit_test(test_insert_sort_uchar_vector_reverse),
    cmocka_unit_test(test_insert_sort_short_vector_forward),
    cmocka_unit_test(test_insert_sort_short_vector_reverse),
    cmocka_unit_test(test_insert_sort_ushort_vector_forward),
    cmocka_unit_test(test_insert_sort_ushort_vector_reverse),
    cmocka_unit_test(test_insert_sort_int_vector_forward),
    cmocka_unit_test(test_insert_sort_int_vector_reverse),
    cmocka_unit_test(test_insert_sort_uint_vector_forward),
    cmocka_unit_test(test_insert_sort_uint_vector_reverse),
    cmocka_unit_test(test_insert_sort_long_vector_forward),
    cmocka_unit_test(test_insert_sort_long_vector_reverse),
    cmocka_unit_test(test_insert_sort_ulong_vector_forward),
    cmocka_unit_test(test_insert_sort_ulong_vector_reverse),
    cmocka_unit_test(test_insert_sort_llong_vector_forward),
    cmocka_unit_test(test_insert_sort_llong_vector_reverse),
    cmocka_unit_test(test_insert_sort_ullong_vector_forward),
    cmocka_unit_test(test_insert_sort_ullong_vector_reverse),
    cmocka_unit_test(test_insert_sort_float_vector_forward),
    cmocka_unit_test(test_insert_sort_float_vector_reverse),
    cmocka_unit_test(test_insert_sort_double_vector_forward),
    cmocka_unit_test(test_insert_sort_double_vector_reverse),
    cmocka_unit_test(test_insert_sort_ldouble_vector_forward),
    cmocka_unit_test(test_insert_sort_ldouble_vector_reverse),
    cmocka_unit_test(test_insert_sort_bool_vector_forward),
    cmocka_unit_test(test_insert_sort_bool_vector_reverse),
    cmocka_unit_test(test_insert_sort_string_vector_forward),
    cmocka_unit_test(test_insert_sort_string_vector_reverse),
    cmocka_unit_test(test_merge_sort_char_vector_forward),
    cmocka_unit_test(test_merge_sort_char_vector_reverse),
    cmocka_unit_test(test_merge_sort_uchar_vector_forward),
    cmocka_unit_test(test_merge_sort_uchar_vector_reverse),
    cmocka_unit_test(test_merge_sort_short_vector_forward),
    cmocka_unit_test(test_merge_sort_short_vector_reverse),
    cmocka_unit_test(test_merge_sort_ushort_vector_forward),
    cmocka_unit_test(test_merge_sort_ushort_vector_reverse),
    cmocka_unit_test(test_merge_sort_int_vector_forward),
    cmocka_unit_test(test_merge_sort_int_vector_reverse),
    cmocka_unit_test(test_merge_sort_uint_vector_forward),
    cmocka_unit_test(test_merge_sort_uint_vector_reverse),
    cmocka_unit_test(test_merge_sort_long_vector_forward),
    cmocka_unit_test(test_merge_sort_long_vector_reverse),
    cmocka_unit_test(test_merge_sort_ulong_vector_forward),
    cmocka_unit_test(test_merge_sort_ulong_vector_reverse),
    cmocka_unit_test(test_merge_sort_llong_vector_forward),
    cmocka_unit_test(test_merge_sort_llong_vector_reverse),
    cmocka_unit_test(test_merge_sort_ullong_vector_forward),
    cmocka_unit_test(test_merge_sort_ullong_vector_reverse),
    cmocka_unit_test(test_merge_sort_float_vector_forward),
    cmocka_unit_test(test_merge_sort_float_vector_reverse),
    cmocka_unit_test(test_merge_sort_double_vector_forward),
    cmocka_unit_test(test_merge_sort_double_vector_reverse),
    cmocka_unit_test(test_merge_sort_ldouble_vector_forward),
    cmocka_unit_test(test_merge_sort_ldouble_vector_reverse),
    cmocka_unit_test(test_merge_sort_bool_vector_forward),
    cmocka_unit_test(test_merge_sort_bool_vector_reverse),
    cmocka_unit_test(test_merge_sort_string_vector_forward),
    cmocka_unit_test(test_merge_sort_string_vector_reverse),
    cmocka_unit_test(test_quick_sort_char_vector_forward),
    cmocka_unit_test(test_quick_sort_char_vector_reverse),
    cmocka_unit_test(test_quick_sort_uchar_vector_forward),
    cmocka_unit_test(test_quick_sort_uchar_vector_reverse),
    cmocka_unit_test(test_quick_sort_short_vector_forward),
    cmocka_unit_test(test_quick_sort_short_vector_reverse),
    cmocka_unit_test(test_quick_sort_ushort_vector_forward),
    cmocka_unit_test(test_quick_sort_ushort_vector_reverse),
    cmocka_unit_test(test_quick_sort_int_vector_forward),
    cmocka_unit_test(test_quick_sort_int_vector_reverse),
    cmocka_unit_test(test_quick_sort_uint_vector_forward),
    cmocka_unit_test(test_quick_sort_uint_vector_reverse),
    cmocka_unit_test(test_quick_sort_long_vector_forward),
    cmocka_unit_test(test_quick_sort_long_vector_reverse),
    cmocka_unit_test(test_quick_sort_ulong_vector_forward),
    cmocka_unit_test(test_quick_sort_ulong_vector_reverse),
    cmocka_unit_test(test_quick_sort_llong_vector_forward),
    cmocka_unit_test(test_quick_sort_llong_vector_reverse),
    cmocka_unit_test(test_quick_sort_ullong_vector_forward),
    cmocka_unit_test(test_quick_sort_ullong_vector_reverse),
    cmocka_unit_test(test_quick_sort_float_vector_forward),
    cmocka_unit_test(test_quick_sort_float_vector_reverse),
    cmocka_unit_test(test_quick_sort_double_vector_forward),
    cmocka_unit_test(test_quick_sort_double_vector_reverse),
    cmocka_unit_test(test_quick_sort_ldouble_vector_forward),
    cmocka_unit_test(test_quick_sort_ldouble_vector_reverse),
    cmocka_unit_test(test_quick_sort_bool_vector_forward),
    cmocka_unit_test(test_quick_sort_bool_vector_reverse),
    cmocka_unit_test(test_quick_sort_string_vector_forward),
    cmocka_unit_test(test_quick_sort_string_vector_reverse),
    cmocka_unit_test(test_tim_sort_char_vector_forward),
    cmocka_unit_test(test_tim_sort_char_vector_reverse),
    cmocka_unit_test(test_tim_sort_uchar_vector_forward),
    cmocka_unit_test(test_tim_sort_uchar_vector_reverse),
    cmocka_unit_test(test_tim_sort_short_vector_forward),
    cmocka_unit_test(test_tim_sort_short_vector_reverse),
    cmocka_unit_test(test_tim_sort_ushort_vector_forward),
    cmocka_unit_test(test_tim_sort_ushort_vector_reverse),
    cmocka_unit_test(test_tim_sort_int_vector_forward),
    cmocka_unit_test(test_tim_sort_int_vector_reverse),
    cmocka_unit_test(test_tim_sort_uint_vector_forward),
    cmocka_unit_test(test_tim_sort_uint_vector_reverse),
    cmocka_unit_test(test_tim_sort_long_vector_forward),
    cmocka_unit_test(test_tim_sort_long_vector_reverse),
    cmocka_unit_test(test_tim_sort_ulong_vector_forward),
    cmocka_unit_test(test_tim_sort_ulong_vector_reverse),
    cmocka_unit_test(test_tim_sort_llong_vector_forward),
    cmocka_unit_test(test_tim_sort_llong_vector_reverse),
    cmocka_unit_test(test_tim_sort_ullong_vector_forward),
    cmocka_unit_test(test_tim_sort_ullong_vector_reverse),
    cmocka_unit_test(test_tim_sort_float_vector_forward),
    cmocka_unit_test(test_tim_sort_float_vector_reverse),
    cmocka_unit_test(test_tim_sort_double_vector_forward),
    cmocka_unit_test(test_tim_sort_double_vector_reverse),
    cmocka_unit_test(test_tim_sort_ldouble_vector_forward),
    cmocka_unit_test(test_tim_sort_ldouble_vector_reverse),
    cmocka_unit_test(test_tim_sort_bool_vector_forward),
    cmocka_unit_test(test_tim_sort_bool_vector_reverse),
    cmocka_unit_test(test_tim_sort_string_vector_forward),
    cmocka_unit_test(test_tim_sort_string_vector_reverse),
    cmocka_unit_test(test_char_binary_vector_search),
    cmocka_unit_test(test_uchar_binary_vector_search),
    cmocka_unit_test(test_short_binary_vector_search),
    cmocka_unit_test(test_ushort_binary_vector_search),
    cmocka_unit_test(test_int_binary_vector_search),
    cmocka_unit_test(test_uint_binary_vector_search),
    cmocka_unit_test(test_long_binary_vector_search),
    cmocka_unit_test(test_ulong_binary_vector_search),
    cmocka_unit_test(test_llong_binary_vector_search),
    cmocka_unit_test(test_ullong_binary_vector_search),
    cmocka_unit_test(test_float_binary_vector_search),
    cmocka_unit_test(test_double_binary_vector_search),
    cmocka_unit_test(test_ldouble_binary_vector_search),
    cmocka_unit_test(test_bool_binary_vector_search),
    cmocka_unit_test(test_string_binary_vector_search),
    cmocka_unit_test(test_str_binary_vector_search)
};

const struct CMUnitTest test_array[] = {
	cmocka_unit_test(test_init_char_array),
    cmocka_unit_test(test_init_char_array),
    cmocka_unit_test(test_init_short_array),
    cmocka_unit_test(test_init_ushort_array),
    cmocka_unit_test(test_init_int_array),
    cmocka_unit_test(test_init_uint_array),
    cmocka_unit_test(test_init_long_array),
    cmocka_unit_test(test_init_ulong_array),
    cmocka_unit_test(test_init_llong_array),
    cmocka_unit_test(test_init_ullong_array),
    cmocka_unit_test(test_init_float_array),
    cmocka_unit_test(test_init_double_array),
    cmocka_unit_test(test_init_ldouble_array),
    cmocka_unit_test(test_init_bool_array),
    cmocka_unit_test(test_pop_char_array),
    cmocka_unit_test(test_pop_uchar_array),
    cmocka_unit_test(test_pop_short_array),
    cmocka_unit_test(test_pop_ushort_array),
    cmocka_unit_test(test_pop_int_array),
    cmocka_unit_test(test_pop_uint_array),
    cmocka_unit_test(test_pop_long_array),
    cmocka_unit_test(test_pop_ulong_array),
    cmocka_unit_test(test_pop_llong_array),
    cmocka_unit_test(test_pop_ullong_array),
    cmocka_unit_test(test_pop_float_array),
    cmocka_unit_test(test_pop_double_array),
    cmocka_unit_test(test_pop_ldouble_array),
    cmocka_unit_test(test_pop_bool_array),
    cmocka_unit_test(test_reverse_char_array),
    cmocka_unit_test(test_reverse_uchar_array),
    cmocka_unit_test(test_reverse_short_array),
    cmocka_unit_test(test_reverse_ushort_array),
    cmocka_unit_test(test_reverse_int_array),
    cmocka_unit_test(test_reverse_uint_array),
    cmocka_unit_test(test_reverse_long_array),
    cmocka_unit_test(test_reverse_ulong_array),
    cmocka_unit_test(test_reverse_llong_array),
    cmocka_unit_test(test_reverse_ullong_array),
    cmocka_unit_test(test_reverse_float_array),
    cmocka_unit_test(test_reverse_double_array),
    cmocka_unit_test(test_reverse_ldouble_array),
    cmocka_unit_test(test_reverse_bool_array),
    cmocka_unit_test(test_bubble_sort_char_array_forward),
    cmocka_unit_test(test_bubble_sort_char_array_reverse),
    cmocka_unit_test(test_bubble_sort_uchar_array_forward),
    cmocka_unit_test(test_bubble_sort_uchar_array_reverse),
    cmocka_unit_test(test_bubble_sort_short_array_forward),
    cmocka_unit_test(test_bubble_sort_short_array_reverse),
    cmocka_unit_test(test_bubble_sort_ushort_array_forward),
    cmocka_unit_test(test_bubble_sort_ushort_array_reverse),
    cmocka_unit_test(test_bubble_sort_int_array_forward),
    cmocka_unit_test(test_bubble_sort_int_array_reverse),
    cmocka_unit_test(test_bubble_sort_uint_array_forward),
    cmocka_unit_test(test_bubble_sort_uint_array_reverse),
    cmocka_unit_test(test_bubble_sort_long_array_forward),
    cmocka_unit_test(test_bubble_sort_long_array_reverse),
    cmocka_unit_test(test_bubble_sort_ulong_array_forward),
    cmocka_unit_test(test_bubble_sort_ulong_array_reverse),
    cmocka_unit_test(test_bubble_sort_llong_array_forward),
    cmocka_unit_test(test_bubble_sort_llong_array_reverse),
    cmocka_unit_test(test_bubble_sort_ullong_array_forward),
    cmocka_unit_test(test_bubble_sort_ullong_array_reverse),
    cmocka_unit_test(test_bubble_sort_float_array_forward),
    cmocka_unit_test(test_bubble_sort_float_array_reverse),
    cmocka_unit_test(test_bubble_sort_double_array_forward),
    cmocka_unit_test(test_bubble_sort_double_array_reverse),
    cmocka_unit_test(test_bubble_sort_ldouble_array_forward),
    cmocka_unit_test(test_bubble_sort_ldouble_array_reverse),
    cmocka_unit_test(test_bubble_sort_bool_array_forward),
    cmocka_unit_test(test_bubble_sort_bool_array_reverse),
    cmocka_unit_test(test_insert_sort_char_array_forward),
    cmocka_unit_test(test_insert_sort_char_array_reverse),
    cmocka_unit_test(test_insert_sort_uchar_array_forward),
    cmocka_unit_test(test_insert_sort_uchar_array_reverse),
    cmocka_unit_test(test_insert_sort_short_array_forward),
    cmocka_unit_test(test_insert_sort_short_array_reverse),
    cmocka_unit_test(test_insert_sort_ushort_array_forward),
    cmocka_unit_test(test_insert_sort_ushort_array_reverse),
    cmocka_unit_test(test_insert_sort_int_array_forward),
    cmocka_unit_test(test_insert_sort_int_array_reverse),
    cmocka_unit_test(test_insert_sort_uint_array_forward),
    cmocka_unit_test(test_insert_sort_uint_array_reverse),
    cmocka_unit_test(test_insert_sort_long_array_forward),
    cmocka_unit_test(test_insert_sort_long_array_reverse),
    cmocka_unit_test(test_insert_sort_ulong_array_forward),
    cmocka_unit_test(test_insert_sort_ulong_array_reverse),
    cmocka_unit_test(test_insert_sort_llong_array_forward),
    cmocka_unit_test(test_insert_sort_llong_array_reverse),
    cmocka_unit_test(test_insert_sort_ullong_array_forward),
    cmocka_unit_test(test_insert_sort_ullong_array_reverse),
    cmocka_unit_test(test_insert_sort_float_array_forward),
    cmocka_unit_test(test_insert_sort_float_array_reverse),
    cmocka_unit_test(test_insert_sort_double_array_forward),
    cmocka_unit_test(test_insert_sort_double_array_reverse),
    cmocka_unit_test(test_insert_sort_ldouble_array_forward),
    cmocka_unit_test(test_insert_sort_ldouble_array_reverse),
    cmocka_unit_test(test_insert_sort_bool_array_forward),
    cmocka_unit_test(test_insert_sort_bool_array_reverse),
    cmocka_unit_test(test_merge_sort_char_array_forward),
    cmocka_unit_test(test_merge_sort_char_array_reverse),
    cmocka_unit_test(test_merge_sort_uchar_array_forward),
    cmocka_unit_test(test_merge_sort_uchar_array_reverse),
    cmocka_unit_test(test_merge_sort_short_array_forward),
    cmocka_unit_test(test_merge_sort_short_array_reverse),
    cmocka_unit_test(test_merge_sort_ushort_array_forward),
    cmocka_unit_test(test_merge_sort_ushort_array_reverse),
    cmocka_unit_test(test_merge_sort_int_array_forward),
    cmocka_unit_test(test_merge_sort_int_array_reverse),
    cmocka_unit_test(test_merge_sort_uint_array_forward),
    cmocka_unit_test(test_merge_sort_uint_array_reverse),
    cmocka_unit_test(test_merge_sort_long_array_forward),
    cmocka_unit_test(test_merge_sort_long_array_reverse),
    cmocka_unit_test(test_merge_sort_ulong_array_forward),
    cmocka_unit_test(test_merge_sort_ulong_array_reverse),
    cmocka_unit_test(test_merge_sort_llong_array_forward),
    cmocka_unit_test(test_merge_sort_llong_array_reverse),
    cmocka_unit_test(test_merge_sort_ullong_array_forward),
    cmocka_unit_test(test_merge_sort_ullong_array_reverse),
    cmocka_unit_test(test_merge_sort_float_array_forward),
    cmocka_unit_test(test_merge_sort_float_array_reverse),
    cmocka_unit_test(test_merge_sort_double_array_forward),
    cmocka_unit_test(test_merge_sort_double_array_reverse),
    cmocka_unit_test(test_merge_sort_ldouble_array_forward),
    cmocka_unit_test(test_merge_sort_ldouble_array_reverse),
    cmocka_unit_test(test_merge_sort_bool_array_forward),
    cmocka_unit_test(test_merge_sort_bool_array_reverse),
    cmocka_unit_test(test_quick_sort_char_array_forward),
    cmocka_unit_test(test_quick_sort_char_array_reverse),
    cmocka_unit_test(test_quick_sort_uchar_array_forward),
    cmocka_unit_test(test_quick_sort_uchar_array_reverse),
    cmocka_unit_test(test_quick_sort_short_array_forward),
    cmocka_unit_test(test_quick_sort_short_array_reverse),
    cmocka_unit_test(test_quick_sort_ushort_array_forward),
    cmocka_unit_test(test_quick_sort_ushort_array_reverse),
    cmocka_unit_test(test_quick_sort_int_array_forward),
    cmocka_unit_test(test_quick_sort_int_array_reverse),
    cmocka_unit_test(test_quick_sort_uint_array_forward),
    cmocka_unit_test(test_quick_sort_uint_array_reverse),
    cmocka_unit_test(test_quick_sort_long_array_forward),
    cmocka_unit_test(test_quick_sort_long_array_reverse),
    cmocka_unit_test(test_quick_sort_ulong_array_forward),
    cmocka_unit_test(test_quick_sort_ulong_array_reverse),
    cmocka_unit_test(test_quick_sort_llong_array_forward),
    cmocka_unit_test(test_quick_sort_llong_array_reverse),
    cmocka_unit_test(test_quick_sort_ullong_array_forward),
    cmocka_unit_test(test_quick_sort_ullong_array_reverse),
    cmocka_unit_test(test_quick_sort_float_array_forward),
    cmocka_unit_test(test_quick_sort_float_array_reverse),
    cmocka_unit_test(test_quick_sort_double_array_forward),
    cmocka_unit_test(test_quick_sort_double_array_reverse),
    cmocka_unit_test(test_quick_sort_ldouble_array_forward),
    cmocka_unit_test(test_quick_sort_ldouble_array_reverse),
    cmocka_unit_test(test_quick_sort_bool_array_forward),
    cmocka_unit_test(test_quick_sort_bool_array_reverse),
    cmocka_unit_test(test_tim_sort_char_array_forward),
    cmocka_unit_test(test_tim_sort_char_array_reverse),
    cmocka_unit_test(test_tim_sort_uchar_array_forward),
    cmocka_unit_test(test_tim_sort_uchar_array_reverse),
    cmocka_unit_test(test_tim_sort_short_array_forward),
    cmocka_unit_test(test_tim_sort_short_array_reverse),
    cmocka_unit_test(test_tim_sort_ushort_array_forward),
    cmocka_unit_test(test_tim_sort_ushort_array_reverse),
    cmocka_unit_test(test_tim_sort_int_array_forward),
    cmocka_unit_test(test_tim_sort_int_array_reverse),
    cmocka_unit_test(test_tim_sort_uint_array_forward),
    cmocka_unit_test(test_tim_sort_uint_array_reverse),
    cmocka_unit_test(test_tim_sort_long_array_forward),
    cmocka_unit_test(test_tim_sort_long_array_reverse),
    cmocka_unit_test(test_tim_sort_ulong_array_forward),
    cmocka_unit_test(test_tim_sort_ulong_array_reverse),
    cmocka_unit_test(test_tim_sort_llong_array_forward),
    cmocka_unit_test(test_tim_sort_llong_array_reverse),
    cmocka_unit_test(test_tim_sort_ullong_array_forward),
    cmocka_unit_test(test_tim_sort_ullong_array_reverse),
    cmocka_unit_test(test_tim_sort_float_array_forward),
    cmocka_unit_test(test_tim_sort_float_array_reverse),
    cmocka_unit_test(test_tim_sort_double_array_forward),
    cmocka_unit_test(test_tim_sort_double_array_reverse),
    cmocka_unit_test(test_tim_sort_ldouble_array_forward),
    cmocka_unit_test(test_tim_sort_ldouble_array_reverse),
    cmocka_unit_test(test_tim_sort_bool_array_forward),
    cmocka_unit_test(test_tim_sort_bool_array_reverse)
};
// Begin code
int main(int argc, const char * argv[]) {
    int status;

    status = cmocka_run_group_tests(test_swap, NULL, NULL);
    if (status != 0) {
        return status;
    }
    status = cmocka_run_group_tests(test_string, NULL, NULL);
    if (status != 0) {
        return status;
    }
    status = cmocka_run_group_tests(test_vector, NULL, NULL);
    if (status != 0) {
        return status;
    }
    status = cmocka_run_group_tests(test_array, NULL, NULL);
	return status;
}
// ================================================================================
// ================================================================================
// eof

