// ================================================================================
// ================================================================================
// - File:    test_array.h
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

#include "../include/array.h" 
#include "../include/print.h"
// ================================================================================
// ================================================================================
// TEST INIT, PUSH, and GET_VECTOR FUNCTIONS 

void test_init_char_array(void **state);
void test_init_uchar_array(void **state);
void test_init_short_array(void **state);
void test_init_ushort_array(void **state);
void test_init_int_array(void **state);
void test_init_uint_array(void **state);
void test_init_long_array(void **state);
void test_init_ulong_array(void **state);
void test_init_ulong_array(void **state);
void test_init_llong_array(void **state);
void test_init_ullong_array(void **state);
void test_init_float_array(void **state);
void test_init_double_array(void **state);
void test_init_ldouble_array(void **state);
void test_init_bool_array(void **state);
void test_init_string_array(void **state);
void test_init_str_array(void **state);
// ================================================================================
// ================================================================================
// TEST POP ARRAY FUNCTIONS 

void test_pop_char_array(void **state);
void test_pop_uchar_array(void **state);
void test_pop_short_array(void **state);
void test_pop_ushort_array(void **state);
void test_pop_int_array(void **state);
void test_pop_uint_array(void **state);
void test_pop_long_array(void **state);
void test_pop_ulong_array(void **state);
void test_pop_ulong_array(void **state);
void test_pop_llong_array(void **state);
void test_pop_ullong_array(void **state);
void test_pop_float_array(void **state);
void test_pop_double_array(void **state);
void test_pop_ldouble_array(void **state);
void test_pop_bool_array(void **state);
void test_pop_string_array(void **state);
void test_pop_str_array(void **state);
// ================================================================================
// ================================================================================

void test_reverse_char_array(void **state);
void test_reverse_uchar_array(void **state);
void test_reverse_short_array(void **state);
void test_reverse_ushort_array(void **state);
void test_reverse_int_array(void **state);
void test_reverse_uint_array(void **state);
void test_reverse_long_array(void **state);
void test_reverse_ulong_array(void **state);
void test_reverse_ulong_array(void **state);
void test_reverse_llong_array(void **state);
void test_reverse_ullong_array(void **state);
void test_reverse_float_array(void **state);
void test_reverse_double_array(void **state);
void test_reverse_ldouble_array(void **state);
void test_reverse_bool_array(void **state);
// ================================================================================
// ================================================================================
// TEST BUBBLE SORT FUNCTIONS 

void test_bubble_sort_char_array_forward(void **state);
void test_bubble_sort_char_array_reverse(void **state);
void test_bubble_sort_uchar_array_forward(void **state);
void test_bubble_sort_uchar_array_reverse(void **state);
void test_bubble_sort_short_array_forward(void **state);
void test_bubble_sort_short_array_reverse(void **state);
void test_bubble_sort_ushort_array_forward(void **state);
void test_bubble_sort_ushort_array_reverse(void **state);
void test_bubble_sort_int_array_forward(void **state);
void test_bubble_sort_int_array_reverse(void **state);
void test_bubble_sort_uint_array_forward(void **state);
void test_bubble_sort_uint_array_reverse(void **state);
void test_bubble_sort_long_array_forward(void **state);
void test_bubble_sort_long_array_reverse(void **state);
void test_bubble_sort_ulong_array_forward(void **state);
void test_bubble_sort_ulong_array_reverse(void **state);
void test_bubble_sort_llong_array_forward(void **state);
void test_bubble_sort_llong_array_reverse(void **state);
void test_bubble_sort_ullong_array_forward(void **state);
void test_bubble_sort_ullong_array_reverse(void **state);
void test_bubble_sort_float_array_forward(void **state);
void test_bubble_sort_float_array_reverse(void **state);
void test_bubble_sort_double_array_forward(void **state);
void test_bubble_sort_double_array_reverse(void **state);
void test_bubble_sort_ldouble_array_forward(void **state);
void test_bubble_sort_ldouble_array_reverse(void **state);
void test_bubble_sort_bool_array_forward(void **state);
void test_bubble_sort_bool_array_reverse(void **state);
// ================================================================================
// ================================================================================

void test_insert_sort_char_array_forward(void **state);
void test_insert_sort_char_array_reverse(void **state);
void test_insert_sort_uchar_array_forward(void **state);
void test_insert_sort_uchar_array_reverse(void **state);
void test_insert_sort_short_array_forward(void **state);
void test_insert_sort_short_array_reverse(void **state);
void test_insert_sort_ushort_array_forward(void **state);
void test_insert_sort_ushort_array_reverse(void **state);
void test_insert_sort_int_array_forward(void **state);
void test_insert_sort_int_array_reverse(void **state);
void test_insert_sort_uint_array_forward(void **state);
void test_insert_sort_uint_array_reverse(void **state);
void test_insert_sort_long_array_forward(void **state);
void test_insert_sort_long_array_reverse(void **state);
void test_insert_sort_ulong_array_forward(void **state);
void test_insert_sort_ulong_array_reverse(void **state);
void test_insert_sort_llong_array_forward(void **state);
void test_insert_sort_llong_array_reverse(void **state);
void test_insert_sort_ullong_array_forward(void **state);
void test_insert_sort_ullong_array_reverse(void **state);
void test_insert_sort_float_array_forward(void **state);
void test_insert_sort_float_array_reverse(void **state);
void test_insert_sort_double_array_forward(void **state);
void test_insert_sort_double_array_reverse(void **state);
void test_insert_sort_ldouble_array_forward(void **state);
void test_insert_sort_ldouble_array_reverse(void **state);
void test_insert_sort_bool_array_forward(void **state);
void test_insert_sort_bool_array_reverse(void **state);
// ================================================================================
// ================================================================================

void test_merge_sort_char_array_forward(void **state);
void test_merge_sort_char_array_reverse(void **state);
void test_merge_sort_uchar_array_forward(void **state);
void test_merge_sort_uchar_array_reverse(void **state);
void test_merge_sort_short_array_forward(void **state);
void test_merge_sort_short_array_reverse(void **state);
void test_merge_sort_ushort_array_forward(void **state);
void test_merge_sort_ushort_array_reverse(void **state);
void test_merge_sort_int_array_forward(void **state);
void test_merge_sort_int_array_reverse(void **state);
void test_merge_sort_uint_array_forward(void **state);
void test_merge_sort_uint_array_reverse(void **state);
void test_merge_sort_long_array_forward(void **state);
void test_merge_sort_long_array_reverse(void **state);
void test_merge_sort_ulong_array_forward(void **state);
void test_merge_sort_ulong_array_reverse(void **state);
void test_merge_sort_llong_array_forward(void **state);
void test_merge_sort_llong_array_reverse(void **state);
void test_merge_sort_ullong_array_forward(void **state);
void test_merge_sort_ullong_array_reverse(void **state);
void test_merge_sort_float_array_forward(void **state);
void test_merge_sort_float_array_reverse(void **state);
void test_merge_sort_double_array_forward(void **state);
void test_merge_sort_double_array_reverse(void **state);
void test_merge_sort_ldouble_array_forward(void **state);
void test_merge_sort_ldouble_array_reverse(void **state);
void test_merge_sort_bool_array_forward(void **state);
void test_merge_sort_bool_array_reverse(void **state);
// ================================================================================
// ================================================================================

void test_quick_sort_char_array_forward(void **state);
void test_quick_sort_char_array_reverse(void **state);
void test_quick_sort_uchar_array_forward(void **state);
void test_quick_sort_uchar_array_reverse(void **state);
void test_quick_sort_short_array_forward(void **state);
void test_quick_sort_short_array_reverse(void **state);
void test_quick_sort_ushort_array_forward(void **state);
void test_quick_sort_ushort_array_reverse(void **state);
void test_quick_sort_int_array_forward(void **state);
void test_quick_sort_int_array_reverse(void **state);
void test_quick_sort_uint_array_forward(void **state);
void test_quick_sort_uint_array_reverse(void **state);
void test_quick_sort_long_array_forward(void **state);
void test_quick_sort_long_array_reverse(void **state);
void test_quick_sort_ulong_array_forward(void **state);
void test_quick_sort_ulong_array_reverse(void **state);
void test_quick_sort_llong_array_forward(void **state);
void test_quick_sort_llong_array_reverse(void **state);
void test_quick_sort_ullong_array_forward(void **state);
void test_quick_sort_ullong_array_reverse(void **state);
void test_quick_sort_float_array_forward(void **state);
void test_quick_sort_float_array_reverse(void **state);
void test_quick_sort_double_array_forward(void **state);
void test_quick_sort_double_array_reverse(void **state);
void test_quick_sort_ldouble_array_forward(void **state);
void test_quick_sort_ldouble_array_reverse(void **state);
void test_quick_sort_bool_array_forward(void **state);
void test_quick_sort_bool_array_reverse(void **state);
// ================================================================================
// ================================================================================

void test_tim_sort_char_array_forward(void **state);
void test_tim_sort_char_array_reverse(void **state);
void test_tim_sort_uchar_array_forward(void **state);
void test_tim_sort_uchar_array_reverse(void **state);
void test_tim_sort_short_array_forward(void **state);
void test_tim_sort_short_array_reverse(void **state);
void test_tim_sort_ushort_array_forward(void **state);
void test_tim_sort_ushort_array_reverse(void **state);
void test_tim_sort_int_array_forward(void **state);
void test_tim_sort_int_array_reverse(void **state);
void test_tim_sort_uint_array_forward(void **state);
void test_tim_sort_uint_array_reverse(void **state);
void test_tim_sort_long_array_forward(void **state);
void test_tim_sort_long_array_reverse(void **state);
void test_tim_sort_ulong_array_forward(void **state);
void test_tim_sort_ulong_array_reverse(void **state);
void test_tim_sort_llong_array_forward(void **state);
void test_tim_sort_llong_array_reverse(void **state);
void test_tim_sort_ullong_array_forward(void **state);
void test_tim_sort_ullong_array_reverse(void **state);
void test_tim_sort_float_array_forward(void **state);
void test_tim_sort_float_array_reverse(void **state);
void test_tim_sort_double_array_forward(void **state);
void test_tim_sort_double_array_reverse(void **state);
void test_tim_sort_ldouble_array_forward(void **state);
void test_tim_sort_ldouble_array_reverse(void **state);
void test_tim_sort_bool_array_forward(void **state);
void test_tim_sort_bool_array_reverse(void **state);
// ================================================================================
// ================================================================================

void test_max_char_array(void **state);
void test_max_uchar_array(void **state);
void test_max_short_array(void **state);
void test_max_ushort_array(void **state);
void test_max_int_array(void **state);
void test_max_uint_array(void **state);
void test_max_long_array(void **state);
void test_max_ulong_array(void **state);
void test_max_llong_array(void **state);
void test_max_ullong_array(void **state);
void test_max_float_array(void **state);
void test_max_double_array(void **state);
void test_max_ldouble_array(void **state);
// ================================================================================
// ================================================================================

void test_min_char_array(void **state);
void test_min_uchar_array(void **state);
void test_min_short_array(void **state);
void test_min_ushort_array(void **state);
void test_min_int_array(void **state);
void test_min_uint_array(void **state);
void test_min_long_array(void **state);
void test_min_ulong_array(void **state);
void test_min_llong_array(void **state);
void test_min_ullong_array(void **state);
void test_min_float_array(void **state);
void test_min_double_array(void **state);
void test_min_ldouble_array(void **state);
// ================================================================================
// ================================================================================

void test_binary_search_char_array(void **state);
void test_binary_search_uchar_array(void **state);
void test_binary_search_short_array(void **state);
void test_binary_search_ushort_array(void **state);
void test_binary_search_int_array(void **state);
void test_binary_search_uint_array(void **state);
void test_binary_search_long_array(void **state);
void test_binary_search_ulong_array(void **state);
void test_binary_search_llong_array(void **state);
void test_binary_search_ullong_array(void **state);
void test_binary_search_float_array(void **state);
void test_binary_search_double_array(void **state);
void test_binary_search_ldouble_array(void **state);
void test_binary_search_bool_array(void **state);
// ================================================================================
// ================================================================================
// TEST SUM ARRAY FUNCTIONS 

void test_sum_char_array(void **state);
void test_sum_uchar_array(void **state);
void test_sum_short_array(void **state);
void test_sum_ushort_array(void **state);
void test_sum_int_array(void **state);
void test_sum_uint_array(void **state);
void test_sum_long_array(void **state);
void test_sum_ulong_array(void **state);
void test_sum_llong_array(void **state);
void test_sum_ullong_array(void **state);
void test_sum_float_array(void **state);
void test_sum_double_array(void **state);
void test_sum_ldouble_array(void **state);
// ================================================================================
// ================================================================================

void test_average_char_array(void **state);
void test_average_uchar_array(void **state);
void test_average_short_array(void **state);
void test_average_ushort_array(void **state);
void test_average_int_array(void **state);
void test_average_uint_array(void **state);
void test_average_long_array(void **state);
void test_average_ulong_array(void **state);
void test_average_llong_array(void **state);
void test_average_ullong_array(void **state);
void test_average_float_array(void **state);
void test_average_double_array(void **state);
void test_average_ldouble_array(void **state);
// ================================================================================
// ================================================================================
// TEST STDEV FUNCTIONS 

void test_stdev_char_array(void **state);
void test_stdev_uchar_array(void **state);
void test_stdev_short_array(void **state);
void test_stdev_ushort_array(void **state);
void test_stdev_int_array(void **state);
void test_stdev_uint_array(void **state);
void test_stdev_long_array(void **state);
void test_stdev_ulong_array(void **state);
void test_stdev_llong_array(void **state);
void test_stdev_ullong_array(void **state);
void test_stdev_float_array(void **state);
void test_stdev_double_array(void **state);
void test_stdev_ldouble_array(void **state);
// ================================================================================
// ================================================================================
#endif /* test_H */
// ================================================================================
// ================================================================================
// eof
