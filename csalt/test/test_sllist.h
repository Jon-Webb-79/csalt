// ================================================================================
// ================================================================================
// - File:    test_sllist.h
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

#ifndef test_sllist_H
#define test_sllist_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "../include/slist.h"
// ================================================================================
// ================================================================================

void test_push_char_sllist(void **state);
void test_push_uchar_sllist(void **state);
void test_push_short_sllist(void **state);
void test_push_ushort_sllist(void **state);
void test_push_int_sllist(void **state);
void test_push_uint_sllist(void **state);
void test_push_long_sllist(void **state);
void test_push_ulong_sllist(void **state);
void test_push_llong_sllist(void **state);
void test_push_ullong_sllist(void **state);
void test_push_float_sllist(void **state);
void test_push_double_sllist(void **state);
void test_push_ldouble_sllist(void **state);
void test_push_bool_sllist(void **state);
void test_push_string_sllist(void **state);
// ================================================================================
// ================================================================================ 
// TEST POP SLLIST FUNCTIONS

void test_pop_char_sllist(void **state);
void test_pop_uchar_sllist(void **state);
void test_pop_short_sllist(void **state);
void test_pop_ushort_sllist(void **state);
void test_pop_int_sllist(void **state);
void test_pop_uint_sllist(void **state);
void test_pop_long_sllist(void **state);
void test_pop_ulong_sllist(void **state);
void test_pop_llong_sllist(void **state);
void test_pop_ullong_sllist(void **state);
void test_pop_float_sllist(void **state);
void test_pop_double_sllist(void **state);
void test_pop_ldouble_sllist(void **state);
void test_pop_bool_sllist(void **state);
void test_pop_string_sllist(void **state);
// ================================================================================
// ================================================================================
// TEST REVERSE LIST

void test_reverse_char_sllist(void **state);
void test_reverse_uchar_sllist(void **state);
void test_reverse_short_sllist(void **state);
void test_reverse_ushort_sllist(void **state);
void test_reverse_int_sllist(void **state);
void test_reverse_uint_sllist(void **state);
void test_reverse_long_sllist(void **state);
void test_reverse_ulong_sllist(void **state);
void test_reverse_llong_sllist(void **state);
void test_reverse_ullong_sllist(void **state);
void test_reverse_float_sllist(void **state);
void test_reverse_double_sllist(void **state);
void test_reverse_ldouble_sllist(void **state);
void test_reverse_bool_sllist(void **state);
void test_reverse_string_sllist(void **state);
// ================================================================================
// ================================================================================
// TEST BUBBLE SORT

void test_bubble_sort_char_sllist_forward(void **state);
void test_bubble_sort_uchar_sllist_forward(void **state);
void test_bubble_sort_short_sllist_forward(void **state);
void test_bubble_sort_ushort_sllist_forward(void **state);
void test_bubble_sort_int_sllist_forward(void **state);
void test_bubble_sort_uint_sllist_forward(void **state);
void test_bubble_sort_long_sllist_forward(void **state);
void test_bubble_sort_ulong_sllist_forward(void **state);
void test_bubble_sort_llong_sllist_forward(void **state);
void test_bubble_sort_ullong_sllist_forward(void **state);
void test_bubble_sort_float_sllist_forward(void **state);
void test_bubble_sort_double_sllist_forward(void **state);
void test_bubble_sort_ldouble_sllist_forward(void **state);
void test_bubble_sort_bool_sllist_forward(void **state);
void test_bubble_sort_string_sllist_forward(void **state);
void test_bubble_sort_char_sllist_reverse(void **state);
void test_bubble_sort_uchar_sllist_reverse(void **state);
void test_bubble_sort_short_sllist_reverse(void **state);
void test_bubble_sort_ushort_sllist_reverse(void **state);
void test_bubble_sort_int_sllist_reverse(void **state);
void test_bubble_sort_uint_sllist_reverse(void **state);
void test_bubble_sort_long_sllist_reverse(void **state);
void test_bubble_sort_ulong_sllist_reverse(void **state);
void test_bubble_sort_llong_sllist_reverse(void **state);
void test_bubble_sort_ullong_sllist_reverse(void **state);
void test_bubble_sort_float_sllist_reverse(void **state);
void test_bubble_sort_double_sllist_reverse(void **state);
void test_bubble_sort_ldouble_sllist_reverse(void **state);
void test_bubble_sort_bool_sllist_reverse(void **state);
void test_bubble_sort_string_sllist_reverse(void **state);
// ================================================================================
// ================================================================================
// TEST SELECTION SORT 

void test_selection_sort_char_sllist_forward(void **state);
void test_selection_sort_uchar_sllist_forward(void **state);
void test_selection_sort_short_sllist_forward(void **state);
void test_selection_sort_ushort_sllist_forward(void **state);
void test_selection_sort_int_sllist_forward(void **state);
void test_selection_sort_uint_sllist_forward(void **state);
void test_selection_sort_long_sllist_forward(void **state);
void test_selection_sort_ulong_sllist_forward(void **state);
void test_selection_sort_llong_sllist_forward(void **state);
void test_selection_sort_ullong_sllist_forward(void **state);
void test_selection_sort_float_sllist_forward(void **state);
void test_selection_sort_double_sllist_forward(void **state);
void test_selection_sort_ldouble_sllist_forward(void **state);
void test_selection_sort_bool_sllist_forward(void **state);
void test_selection_sort_string_sllist_forward(void **state);
void test_selection_sort_char_sllist_reverse(void **state);
void test_selection_sort_uchar_sllist_reverse(void **state);
void test_selection_sort_short_sllist_reverse(void **state);
void test_selection_sort_ushort_sllist_reverse(void **state);
void test_selection_sort_int_sllist_reverse(void **state);
void test_selection_sort_uint_sllist_reverse(void **state);
void test_selection_sort_long_sllist_reverse(void **state);
void test_selection_sort_ulong_sllist_reverse(void **state);
void test_selection_sort_llong_sllist_reverse(void **state);
void test_selection_sort_ullong_sllist_reverse(void **state);
void test_selection_sort_float_sllist_reverse(void **state);
void test_selection_sort_double_sllist_reverse(void **state);
void test_selection_sort_ldouble_sllist_reverse(void **state);
void test_selection_sort_bool_sllist_reverse(void **state);
void test_selection_sort_string_sllist_reverse(void **state);
// ================================================================================
// ================================================================================
// TEST INSERT SORT 

void test_insert_sort_char_sllist_forward(void **state);
void test_insert_sort_uchar_sllist_forward(void **state);
void test_insert_sort_short_sllist_forward(void **state);
void test_insert_sort_ushort_sllist_forward(void **state);
void test_insert_sort_int_sllist_forward(void **state);
void test_insert_sort_uint_sllist_forward(void **state);
void test_insert_sort_long_sllist_forward(void **state);
void test_insert_sort_ulong_sllist_forward(void **state);
void test_insert_sort_llong_sllist_forward(void **state);
void test_insert_sort_ullong_sllist_forward(void **state);
void test_insert_sort_float_sllist_forward(void **state);
void test_insert_sort_double_sllist_forward(void **state);
void test_insert_sort_ldouble_sllist_forward(void **state);
void test_insert_sort_bool_sllist_forward(void **state);
void test_insert_sort_string_sllist_forward(void **state);
void test_insert_sort_char_sllist_reverse(void **state);
void test_insert_sort_uchar_sllist_reverse(void **state);
void test_insert_sort_short_sllist_reverse(void **state);
void test_insert_sort_ushort_sllist_reverse(void **state);
void test_insert_sort_int_sllist_reverse(void **state);
void test_insert_sort_uint_sllist_reverse(void **state);
void test_insert_sort_long_sllist_reverse(void **state);
void test_insert_sort_ulong_sllist_reverse(void **state);
void test_insert_sort_llong_sllist_reverse(void **state);
void test_insert_sort_ullong_sllist_reverse(void **state);
void test_insert_sort_float_sllist_reverse(void **state);
void test_insert_sort_double_sllist_reverse(void **state);
void test_insert_sort_ldouble_sllist_reverse(void **state);
void test_insert_sort_bool_sllist_reverse(void **state);
void test_insert_sort_string_sllist_reverse(void **state);
// ================================================================================
// ================================================================================
// TEST MERGE SORT 

void test_merge_sort_char_sllist_forward(void **state);
void test_merge_sort_uchar_sllist_forward(void **state);
void test_merge_sort_short_sllist_forward(void **state);
void test_merge_sort_ushort_sllist_forward(void **state);
void test_merge_sort_int_sllist_forward(void **state);
void test_merge_sort_uint_sllist_forward(void **state);
void test_merge_sort_long_sllist_forward(void **state);
void test_merge_sort_ulong_sllist_forward(void **state);
void test_merge_sort_llong_sllist_forward(void **state);
void test_merge_sort_ullong_sllist_forward(void **state);
void test_merge_sort_float_sllist_forward(void **state);
void test_merge_sort_double_sllist_forward(void **state);
void test_merge_sort_ldouble_sllist_forward(void **state);
void test_merge_sort_bool_sllist_forward(void **state);
void test_merge_sort_string_sllist_forward(void **state);
void test_merge_sort_char_sllist_reverse(void **state);
void test_merge_sort_uchar_sllist_reverse(void **state);
void test_merge_sort_short_sllist_reverse(void **state);
void test_merge_sort_ushort_sllist_reverse(void **state);
void test_merge_sort_int_sllist_reverse(void **state);
void test_merge_sort_uint_sllist_reverse(void **state);
void test_merge_sort_long_sllist_reverse(void **state);
void test_merge_sort_ulong_sllist_reverse(void **state);
void test_merge_sort_llong_sllist_reverse(void **state);
void test_merge_sort_ullong_sllist_reverse(void **state);
void test_merge_sort_float_sllist_reverse(void **state);
void test_merge_sort_double_sllist_reverse(void **state);
void test_merge_sort_ldouble_sllist_reverse(void **state);
void test_merge_sort_bool_sllist_reverse(void **state);
void test_merge_sort_string_sllist_reverse(void **state);
// ================================================================================
// ================================================================================

void test_sllist_char_iterator(void **state);
void test_sllist_uchar_iterator(void **state);
void test_sllist_short_iterator(void **state);
void test_sllist_ushort_iterator(void **state);
void test_sllist_int_iterator(void **state);
void test_sllist_uint_iterator(void **state);
void test_sllist_long_iterator(void **state);
void test_sllist_ulong_iterator(void **state);
void test_sllist_llong_iterator(void **state);
void test_sllist_ullong_iterator(void **state);
void test_sllist_float_iterator(void **state);
void test_sllist_double_iterator(void **state);
void test_sllist_ldouble_iterator(void **state);
void test_sllist_bool_iterator(void **state);
void test_sllist_string_iterator(void **state);
// ================================================================================
// ================================================================================
// TEST FREE_GBC 
#ifdef __GNUC__ 
void test_sllist_char_gbc(void **state);
void test_sllist_uchar_gbc(void **state);
void test_sllist_short_gbc(void **state);
void test_sllist_ushort_gbc(void **state);
void test_sllist_int_gbc(void **state);
void test_sllist_uint_gbc(void **state);
void test_sllist_long_gbc(void **state);
void test_sllist_ulong_gbc(void **state);
void test_sllist_llong_gbc(void **state);
void test_sllist_ullong_gbc(void **state);
void test_sllist_float_gbc(void **state);
void test_sllist_double_gbc(void **state);
void test_sllist_ldouble_gbc(void **state);
void test_sllist_bool_gbc(void **state);
void test_sllist_string_gbc(void **state);
#endif /* __GNUC__ */
// ================================================================================
// ================================================================================ 
#endif /* test_sllist_H */
// ================================================================================
// ================================================================================
// eof
