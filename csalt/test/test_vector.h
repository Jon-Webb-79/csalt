// ================================================================================
// ================================================================================
// - File:    test_vector.h
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

#ifndef test_vector_H
#define test_vector_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <unistd.h>
#include <fcntl.h>

#include "../include/vector.h"
// ================================================================================
// ================================================================================
// TEST INIT, PUSH, and FREE FUNCTIONS

void test_init_char_vector(void **state);
void test_init_uchar_vector(void **state);
void test_init_short_vector(void **state);
void test_init_ushort_vector(void **state);
void test_init_int_vector(void **state);
void test_init_uint_vector(void **state);
void test_init_long_vector(void **state);
void test_init_ulong_vector(void **state);
void test_init_ulong_vector(void **state);
void test_init_llong_vector(void **state);
void test_init_ullong_vector(void **state);
void test_init_float_vector(void **state);
void test_init_double_vector(void **state);
void test_init_ldouble_vector(void **state);
void test_init_bool_vector(void **state);
void test_init_string_vector(void **state);
void test_init_str_vector(void **state);
// ================================================================================
// ================================================================================
// TEST GET_VECTOR FAILURE MODES 

void test_get_char_vector_null_struct(void **state);
void test_get_char_vector_null_data(void **state);
void test_get_char_vector_index_out_of_bounds(void **state);
void test_get_uchar_vector_null_struct(void **state);
void test_get_uchar_vector_null_data(void **state);
void test_get_uchar_vector_index_out_of_bounds(void **state);
void test_get_short_vector_null_struct(void **state);
void test_get_short_vector_null_data(void **state);
void test_get_short_vector_index_out_of_bounds(void **state);
void test_get_ushort_vector_null_struct(void **state);
void test_get_ushort_vector_null_data(void **state);
void test_get_ushort_vector_index_out_of_bounds(void **state);
void test_get_int_vector_null_struct(void **state);
void test_get_int_vector_null_data(void **state);
void test_get_int_vector_index_out_of_bounds(void **state);
void test_get_uint_vector_null_struct(void **state);
void test_get_uint_vector_null_data(void **state);
void test_get_uint_vector_index_out_of_bounds(void **state);
void test_get_long_vector_null_struct(void **state);
void test_get_long_vector_null_data(void **state);
void test_get_long_vector_index_out_of_bounds(void **state);
void test_get_ulong_vector_null_struct(void **state);
void test_get_ulong_vector_null_data(void **state);
void test_get_ulong_vector_index_out_of_bounds(void **state);
void test_get_llong_vector_null_struct(void **state);
void test_get_llong_vector_null_data(void **state);
void test_get_llong_vector_index_out_of_bounds(void **state);
void test_get_ullong_vector_null_struct(void **state);
void test_get_ullong_vector_null_data(void **state);
void test_get_ullong_vector_index_out_of_bounds(void **state);
void test_get_float_vector_null_struct(void **state);
void test_get_float_vector_null_data(void **state);
void test_get_float_vector_index_out_of_bounds(void **state);
void test_get_double_vector_null_struct(void **state);
void test_get_double_vector_null_data(void **state);
void test_get_double_vector_index_out_of_bounds(void **state);
void test_get_ldouble_vector_null_struct(void **state);
void test_get_ldouble_vector_null_data(void **state);
void test_get_ldouble_vector_index_out_of_bounds(void **state);
void test_get_bool_vector_null_struct(void **state);
void test_get_bool_vector_null_data(void **state);
void test_get_bool_vector_index_out_of_bounds(void **state);
void test_get_string_vector_null_struct(void **state);
void test_get_string_vector_null_data(void **state);
void test_print(void **state);
// ================================================================================
// ================================================================================
// TEST POP DATA 

void test_pop_char_vector(void **state);
void test_pop_uchar_vector(void **state);
void test_pop_short_vector(void **state);
void test_pop_ushort_vector(void **state);
void test_pop_int_vector(void **state);
void test_pop_uint_vector(void **state);
void test_pop_long_vector(void **state);
void test_pop_ulong_vector(void **state);
void test_pop_llong_vector(void **state);
void test_pop_ullong_vector(void **state);
void test_pop_float_vector(void **state);
void test_pop_double_vector(void **state);
void test_pop_ldouble_vector(void **state);
void test_pop_bool_vector(void **state);
void test_pop_string_vector(void **state);
// ================================================================================
// ================================================================================
// TEST GARBAGE COLLECTION MACROS

void test_char_garbage_vector(void **state);
void test_uchar_garbage_vector(void **state);
void test_short_garbage_vector(void **state);
void test_ushort_garbage_vector(void **state);
void test_int_garbage_vector(void **state);
void test_uint_garbage_vector(void **state);
void test_long_garbage_vector(void **state);
void test_ulong_garbage_vector(void **state);
void test_llong_garbage_vector(void **state);
void test_ullong_garbage_vector(void **state);
void test_float_garbage_vector(void **state);
void test_double_garbage_vector(void **state);
void test_ldouble_garbage_vector(void **state);
void test_bool_garbage_vector(void **state);
void test_string_garbage_vector(void **state);
// ================================================================================
// ================================================================================
// TEST REVERSE VECTOR FUNCTIONS 

void test_reverse_char_vector(void **state);
void test_reverse_uchar_vector(void **state);
void test_reverse_short_vector(void **state);
void test_reverse_ushort_vector(void **state);
void test_reverse_int_vector(void **state);
void test_reverse_uint_vector(void **state);
void test_reverse_long_vector(void **state);
void test_reverse_ulong_vector(void **state);
void test_reverse_llong_vector(void **state);
void test_reverse_ullong_vector(void **state);
void test_reverse_float_vector(void **state);
void test_reverse_double_vector(void **state);
void test_reverse_ldouble_vector(void **state);
void test_reverse_bool_vector(void **state);
void test_reverse_string_vector(void **state);
// ================================================================================
// ================================================================================
// TEST MAX VECTOR FUNCTIONS 

void test_max_char_vector(void **state);
void test_max_uchar_vector(void **state);
void test_max_short_vector(void **state);
void test_max_ushort_vector(void **state);
void test_max_int_vector(void **state);
void test_max_uint_vector(void **state);
void test_max_long_vector(void **state);
void test_max_ulong_vector(void **state);
void test_max_llong_vector(void **state);
void test_max_ullong_vector(void **state);
void test_max_float_vector(void **state);
void test_max_double_vector(void **state);
void test_max_ldouble_vector(void **state);
// ================================================================================
// ================================================================================
// TEST MIN VECTOR FUNCTIONS 

void test_min_char_vector(void **state);
void test_min_uchar_vector(void **state);
void test_min_short_vector(void **state);
void test_min_ushort_vector(void **state);
void test_min_int_vector(void **state);
void test_min_uint_vector(void **state);
void test_min_long_vector(void **state);
void test_min_ulong_vector(void **state);
void test_min_llong_vector(void **state);
void test_min_ullong_vector(void **state);
void test_min_float_vector(void **state);
void test_min_double_vector(void **state);
void test_min_ldouble_vector(void **state);
// ================================================================================
// ================================================================================
// TEST SUM VECTOR FUNCTIONS 

void test_sum_char_vector(void **state);
void test_sum_uchar_vector(void **state);
void test_sum_short_vector(void **state);
void test_sum_ushort_vector(void **state);
void test_sum_int_vector(void **state);
void test_sum_uint_vector(void **state);
void test_sum_long_vector(void **state);
void test_sum_ulong_vector(void **state);
void test_sum_llong_vector(void **state);
void test_sum_ullong_vector(void **state);
void test_sum_float_vector(void **state);
void test_sum_double_vector(void **state);
void test_sum_ldouble_vector(void **state);
// ================================================================================
// ================================================================================
// TEST AVERAGE VECTOR CUNTIONS 

void test_average_char_vector(void **state);
void test_average_uchar_vector(void **state);
void test_average_short_vector(void **state);
void test_average_ushort_vector(void **state);
void test_average_int_vector(void **state);
void test_average_uint_vector(void **state);
void test_average_long_vector(void **state);
void test_average_ulong_vector(void **state);
void test_average_llong_vector(void **state);
void test_average_ullong_vector(void **state);
void test_average_float_vector(void **state);
void test_average_double_vector(void **state);
void test_average_ldouble_vector(void **state);
// ================================================================================
// ================================================================================
// TEST STDEV FUNCTIONS 

void test_stdev_char_vector(void **state);
void test_stdev_uchar_vector(void **state);
void test_stdev_short_vector(void **state);
void test_stdev_ushort_vector(void **state);
void test_stdev_int_vector(void **state);
void test_stdev_uint_vector(void **state);
void test_stdev_long_vector(void **state);
void test_stdev_ulong_vector(void **state);
void test_stdev_llong_vector(void **state);
void test_stdev_ullong_vector(void **state);
void test_stdev_float_vector(void **state);
void test_stdev_double_vector(void **state);
void test_stdev_ldouble_vector(void **state);
// ================================================================================
// ================================================================================
// TEST CUMSUM FUNCTIONS 

void test_cumsum_char_vector(void **state);
void test_cumsum_uchar_vector(void **state);
void test_cumsum_short_vector(void **state);
void test_cumsum_ushort_vector(void **state);
void test_cumsum_int_vector(void **state);
void test_cumsum_uint_vector(void **state);
void test_cumsum_long_vector(void **state);
void test_cumsum_ulong_vector(void **state);
void test_cumsum_llong_vector(void **state);
void test_cumsum_ullong_vector(void **state);
void test_cumsum_float_vector(void **state);
void test_cumsum_double_vector(void **state);
void test_cumsum_ldouble_vector(void **state);
// ================================================================================
// ================================================================================
// TEST COPY VECTOR 

void test_copy_char_vector(void **state);
void test_copy_uchar_vector(void **state);
void test_copy_short_vector(void **state);
void test_copy_ushort_vector(void **state);
void test_copy_int_vector(void **state);
void test_copy_uint_vector(void **state);
void test_copy_long_vector(void **state);
void test_copy_ulong_vector(void **state);
void test_copy_llong_vector(void **state);
void test_copy_ullong_vector(void **state);
void test_copy_float_vector(void **state);
void test_copy_double_vector(void **state);
void test_copy_ldouble_vector(void **state);
void test_copy_bool_vector(void **state);
void test_copy_string_vector(void **state);
// ================================================================================
// ================================================================================
// TEST TRIM VECTOR FUNCTIONS 

void test_trim_char_vector(void **state);
void test_trim_uchar_vector(void **state);
void test_trim_short_vector(void **state);
void test_trim_ushort_vector(void **state);
void test_trim_int_vector(void **state);
void test_trim_uint_vector(void **state);
void test_trim_long_vector(void **state);
void test_trim_ulong_vector(void **state);
void test_trim_llong_vector(void **state);
void test_trim_ullong_vector(void **state);
void test_trim_float_vector(void **state);
void test_trim_double_vector(void **state);
void test_trim_ldouble_vector(void **state);
void test_trim_bool_vector(void **state);
void test_trim_string_vector(void **state);
// ================================================================================
// ================================================================================
// TEST REPLACE VECTOR FUNCTIONS 

void test_replace_char_vector(void **state);
void test_replace_uchar_vector(void **state);
void test_replace_short_vector(void **state);
void test_replace_ushort_vector(void **state);
void test_replace_int_vector(void **state);
void test_replace_uint_vector(void **state);
void test_replace_long_vector(void **state);
void test_replace_ulong_vector(void **state);
void test_replace_llong_vector(void **state);
void test_replace_ullong_vector(void **state);
void test_replace_float_vector(void **state);
void test_replace_double_vector(void **state);
void test_replace_ldouble_vector(void **state);
void test_replace_bool_vector(void **state);
void test_replace_string_vector(void **state);
void test_replace_str_vector(void **state);
// ================================================================================ 
// ================================================================================
// TEST ITERATORS 

void test_iter_char_vector(void **state);
void test_iter_uchar_vector(void **state);
void test_iter_short_vector(void **state);
void test_iter_ushort_vector(void **state);
void test_iter_int_vector(void **state);
void test_iter_uint_vector(void **state);
void test_iter_long_vector(void **state);
void test_iter_ulong_vector(void **state);
void test_iter_llong_vector(void **state);
void test_iter_ullong_vector(void **state);
void test_iter_float_vector(void **state);
void test_iter_double_vector(void **state);
void test_iter_ldouble_vector(void **state);
void test_iter_bool_vector(void **state);
void test_iter_string_vector(void **state);
void test_iter_str_vector(void **state);
// ================================================================================
// ================================================================================
// TEST BUBBLE SORT FUNCTIONS 

void test_bubble_sort_char_vector_forward(void **state);
void test_bubble_sort_char_vector_reverse(void **state);
void test_bubble_sort_uchar_vector_forward(void **state);
void test_bubble_sort_uchar_vector_reverse(void **state);
void test_bubble_sort_short_vector_forward(void **state);
void test_bubble_sort_short_vector_reverse(void **state);
void test_bubble_sort_ushort_vector_forward(void **state);
void test_bubble_sort_ushort_vector_reverse(void **state);
void test_bubble_sort_int_vector_forward(void **state);
void test_bubble_sort_int_vector_reverse(void **state);
void test_bubble_sort_uint_vector_forward(void **state);
void test_bubble_sort_uint_vector_reverse(void **state);
void test_bubble_sort_long_vector_forward(void **state);
void test_bubble_sort_long_vector_reverse(void **state);
void test_bubble_sort_ulong_vector_forward(void **state);
void test_bubble_sort_ulong_vector_reverse(void **state);
void test_bubble_sort_llong_vector_forward(void **state);
void test_bubble_sort_llong_vector_reverse(void **state);
void test_bubble_sort_ullong_vector_forward(void **state);
void test_bubble_sort_ullong_vector_reverse(void **state);
void test_bubble_sort_float_vector_forward(void **state);
void test_bubble_sort_float_vector_reverse(void **state);
void test_bubble_sort_double_vector_forward(void **state);
void test_bubble_sort_double_vector_reverse(void **state);
void test_bubble_sort_ldouble_vector_forward(void **state);
void test_bubble_sort_ldouble_vector_reverse(void **state);
void test_bubble_sort_bool_vector_forward(void **state);
void test_bubble_sort_bool_vector_reverse(void **state);
void test_bubble_sort_string_vector_forward(void **state);
void test_bubble_sort_string_vector_reverse(void **state);
// ================================================================================
// ================================================================================
// TEST INSERT SORT FUNCTIONS

void test_insert_sort_char_vector_forward(void **state);
void test_insert_sort_char_vector_reverse(void **state);
void test_insert_sort_uchar_vector_forward(void **state);
void test_insert_sort_uchar_vector_reverse(void **state);
void test_insert_sort_short_vector_forward(void **state);
void test_insert_sort_short_vector_reverse(void **state);
void test_insert_sort_ushort_vector_forward(void **state);
void test_insert_sort_ushort_vector_reverse(void **state);
void test_insert_sort_int_vector_forward(void **state);
void test_insert_sort_int_vector_reverse(void **state);
void test_insert_sort_uint_vector_forward(void **state);
void test_insert_sort_uint_vector_reverse(void **state);
void test_insert_sort_long_vector_forward(void **state);
void test_insert_sort_long_vector_reverse(void **state);
void test_insert_sort_ulong_vector_forward(void **state);
void test_insert_sort_ulong_vector_reverse(void **state);
void test_insert_sort_llong_vector_forward(void **state);
void test_insert_sort_llong_vector_reverse(void **state);
void test_insert_sort_ullong_vector_forward(void **state);
void test_insert_sort_ullong_vector_reverse(void **state);
void test_insert_sort_float_vector_forward(void **state);
void test_insert_sort_float_vector_reverse(void **state);
void test_insert_sort_double_vector_forward(void **state);
void test_insert_sort_double_vector_reverse(void **state);
void test_insert_sort_ldouble_vector_forward(void **state);
void test_insert_sort_ldouble_vector_reverse(void **state);
void test_insert_sort_bool_vector_forward(void **state);
void test_insert_sort_bool_vector_reverse(void **state);
void test_insert_sort_string_vector_forward(void **state);
void test_insert_sort_string_vector_reverse(void **state);
// ================================================================================
// ================================================================================
// TEST MERGE SORT FUNCTIONS

void test_merge_sort_char_vector_forward(void **state);
void test_merge_sort_char_vector_reverse(void **state);
void test_merge_sort_uchar_vector_forward(void **state);
void test_merge_sort_uchar_vector_reverse(void **state);
void test_merge_sort_short_vector_forward(void **state);
void test_merge_sort_short_vector_reverse(void **state);
void test_merge_sort_ushort_vector_forward(void **state);
void test_merge_sort_ushort_vector_reverse(void **state);
void test_merge_sort_int_vector_forward(void **state);
void test_merge_sort_int_vector_reverse(void **state);
void test_merge_sort_uint_vector_forward(void **state);
void test_merge_sort_uint_vector_reverse(void **state);
void test_merge_sort_long_vector_forward(void **state);
void test_merge_sort_long_vector_reverse(void **state);
void test_merge_sort_ulong_vector_forward(void **state);
void test_merge_sort_ulong_vector_reverse(void **state);
void test_merge_sort_llong_vector_forward(void **state);
void test_merge_sort_llong_vector_reverse(void **state);
void test_merge_sort_ullong_vector_forward(void **state);
void test_merge_sort_ullong_vector_reverse(void **state);
void test_merge_sort_float_vector_forward(void **state);
void test_merge_sort_float_vector_reverse(void **state);
void test_merge_sort_double_vector_forward(void **state);
void test_merge_sort_double_vector_reverse(void **state);
void test_merge_sort_ldouble_vector_forward(void **state);
void test_merge_sort_ldouble_vector_reverse(void **state);
void test_merge_sort_bool_vector_forward(void **state);
void test_merge_sort_bool_vector_reverse(void **state);
void test_merge_sort_string_vector_forward(void **state);
void test_merge_sort_string_vector_reverse(void **state);
// ================================================================================
// ================================================================================
// TEST QUICK SORT FUNCTIONS

void test_quick_sort_char_vector_forward(void **state);
void test_quick_sort_char_vector_reverse(void **state);
void test_quick_sort_uchar_vector_forward(void **state);
void test_quick_sort_uchar_vector_reverse(void **state);
void test_quick_sort_short_vector_forward(void **state);
void test_quick_sort_short_vector_reverse(void **state);
void test_quick_sort_ushort_vector_forward(void **state);
void test_quick_sort_ushort_vector_reverse(void **state);
void test_quick_sort_int_vector_forward(void **state);
void test_quick_sort_int_vector_reverse(void **state);
void test_quick_sort_uint_vector_forward(void **state);
void test_quick_sort_uint_vector_reverse(void **state);
void test_quick_sort_long_vector_forward(void **state);
void test_quick_sort_long_vector_reverse(void **state);
void test_quick_sort_ulong_vector_forward(void **state);
void test_quick_sort_ulong_vector_reverse(void **state);
void test_quick_sort_llong_vector_forward(void **state);
void test_quick_sort_llong_vector_reverse(void **state);
void test_quick_sort_ullong_vector_forward(void **state);
void test_quick_sort_ullong_vector_reverse(void **state);
void test_quick_sort_float_vector_forward(void **state);
void test_quick_sort_float_vector_reverse(void **state);
void test_quick_sort_double_vector_forward(void **state);
void test_quick_sort_double_vector_reverse(void **state);
void test_quick_sort_ldouble_vector_forward(void **state);
void test_quick_sort_ldouble_vector_reverse(void **state);
void test_quick_sort_bool_vector_forward(void **state);
void test_quick_sort_bool_vector_reverse(void **state);
void test_quick_sort_string_vector_forward(void **state);
void test_quick_sort_string_vector_reverse(void **state);
// ================================================================================
// ================================================================================
// TEST QUICK SORT FUNCTIONS

void test_tim_sort_char_vector_forward(void **state);
void test_tim_sort_char_vector_reverse(void **state);
void test_tim_sort_uchar_vector_forward(void **state);
void test_tim_sort_uchar_vector_reverse(void **state);
void test_tim_sort_short_vector_forward(void **state);
void test_tim_sort_short_vector_reverse(void **state);
void test_tim_sort_ushort_vector_forward(void **state);
void test_tim_sort_ushort_vector_reverse(void **state);
void test_tim_sort_int_vector_forward(void **state);
void test_tim_sort_int_vector_reverse(void **state);
void test_tim_sort_uint_vector_forward(void **state);
void test_tim_sort_uint_vector_reverse(void **state);
void test_tim_sort_long_vector_forward(void **state);
void test_tim_sort_long_vector_reverse(void **state);
void test_tim_sort_ulong_vector_forward(void **state);
void test_tim_sort_ulong_vector_reverse(void **state);
void test_tim_sort_llong_vector_forward(void **state);
void test_tim_sort_llong_vector_reverse(void **state);
void test_tim_sort_ullong_vector_forward(void **state);
void test_tim_sort_ullong_vector_reverse(void **state);
void test_tim_sort_float_vector_forward(void **state);
void test_tim_sort_float_vector_reverse(void **state);
void test_tim_sort_double_vector_forward(void **state);
void test_tim_sort_double_vector_reverse(void **state);
void test_tim_sort_ldouble_vector_forward(void **state);
void test_tim_sort_ldouble_vector_reverse(void **state);
void test_tim_sort_bool_vector_forward(void **state);
void test_tim_sort_bool_vector_reverse(void **state);
void test_tim_sort_string_vector_forward(void **state);
void test_tim_sort_string_vector_reverse(void **state);
// ================================================================================
// ================================================================================
// TEST BINARY SEARCH FUNCTIONS 

void test_char_binary_vector_search(void **state);
void test_uchar_binary_vector_search(void **state);
void test_short_binary_vector_search(void **state);
void test_ushort_binary_vector_search(void **state);
void test_int_binary_vector_search(void **state);
void test_uint_binary_vector_search(void **state);
void test_long_binary_vector_search(void **state);
void test_ulong_binary_vector_search(void **state);
void test_llong_binary_vector_search(void **state);
void test_ullong_binary_vector_search(void **state);
void test_float_binary_vector_search(void **state);
void test_double_binary_vector_search(void **state);
void test_ldouble_binary_vector_search(void **state);
void test_bool_binary_vector_search(void **state);
void test_string_binary_vector_search(void **state);
void test_str_binary_vector_search(void **state);
// ================================================================================
// ================================================================================

// ================================================================================
// ================================================================================
#endif /* test_vector_H */
// ================================================================================
// ================================================================================
// eof
