// ================================================================================
// ================================================================================
// - File:    test_dllist.h
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

#ifndef test_dlist_H
#define test_dlist_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "../include/dlist.h"
#include "../include/print.h"
// ================================================================================
// ================================================================================

void test_push_front_char_dllist(void **state);
void test_push_front_uchar_dllist(void **state);
void test_push_front_short_dllist(void **state);
void test_push_front_ushort_dllist(void **state);
void test_push_front_int_dllist(void **state);
void test_push_front_uint_dllist(void **state);
void test_push_front_long_dllist(void **state);
void test_push_front_ulong_dllist(void **state);
void test_push_front_llong_dllist(void **state);
void test_push_front_ullong_dllist(void **state);
void test_push_front_float_dllist(void **state);
void test_push_front_double_dllist(void **state);
void test_push_front_ldouble_dllist(void **state);
void test_push_front_bool_dllist(void **state);
void test_push_front_string_dllist(void **state);
// ================================================================================
// ================================================================================

#ifdef __GNUC__ 
void test_char_dlist_gbc(void **state);
void test_uchar_dlist_gbc(void **state);
void test_short_dlist_gbc(void **state);
void test_ushort_dlist_gbc(void **state);
void test_int_dlist_gbc(void **state);
void test_uint_dlist_gbc(void **state);
void test_long_dlist_gbc(void **state);
void test_ulong_dlist_gbc(void **state);
void test_llong_dlist_gbc(void **state);
void test_ullong_dlist_gbc(void **state);
void test_float_dlist_gbc(void **state);
void test_double_dlist_gbc(void **state);
void test_ldouble_dlist_gbc(void **state);
void test_bool_dlist_gbc(void **state);
void test_string_dlist_gbc(void **state);
#endif /* __GNUC__ */
// ================================================================================
// ================================================================================

void test_char_dlist_push_back(void **state);
void test_uchar_dlist_push_back(void **state);
void test_short_dlist_push_back(void **state);
void test_ushort_dlist_push_back(void **state);
void test_int_dlist_push_back(void **state);
void test_uint_dlist_push_back(void **state);
void test_long_dlist_push_back(void **state);
void test_ulong_dlist_push_back(void **state);
void test_llong_dlist_push_back(void **state);
void test_ullong_dlist_push_back(void **state);
void test_float_dlist_push_back(void **state);
void test_double_dlist_push_back(void **state);
void test_ldouble_dlist_push_back(void **state);
void test_bool_dlist_push_back(void **state);
void test_string_dlist_push_back(void **state);
// ================================================================================
// ================================================================================

void test_char_insert_dlist(void **state);
void test_uchar_insert_dlist(void **state);
void test_short_insert_dlist(void **state);
void test_ushort_insert_dlist(void **state);
void test_int_insert_dlist(void **state);
void test_uint_insert_dlist(void **state);
void test_long_insert_dlist(void **state);
void test_ulong_insert_dlist(void **state);
void test_llong_insert_dlist(void **state);
void test_ullong_insert_dlist(void **state);
void test_float_insert_dlist(void **state);
void test_double_insert_dlist(void **state);
void test_ldouble_insert_dlist(void **state);
void test_bool_insert_dlist(void **state);
void test_string_insert_dlist(void **state);
// ================================================================================
// ================================================================================

void test_pop_char_front_dlist(void **state);
void test_pop_uchar_front_dlist(void **state);
void test_pop_short_front_dlist(void **state);
void test_pop_ushort_front_dlist(void **state);
void test_pop_int_front_dlist(void **state);
void test_pop_uint_front_dlist(void **state);
void test_pop_long_front_dlist(void **state);
void test_pop_ulong_front_dlist(void **state);
void test_pop_llong_front_dlist(void **state);
void test_pop_ullong_front_dlist(void **state);
void test_pop_float_front_dlist(void **state);
void test_pop_double_front_dlist(void **state);
void test_pop_ldouble_front_dlist(void **state);
void test_pop_bool_front_dlist(void **state);
void test_pop_string_front_dlist(void **state);
// ================================================================================
// ================================================================================

void test_pop_char_back_dlist(void **state);
void test_pop_uchar_back_dlist(void **state);
void test_pop_short_back_dlist(void **state);
void test_pop_ushort_back_dlist(void **state);
void test_pop_int_back_dlist(void **state);
void test_pop_uint_back_dlist(void **state);
void test_pop_long_back_dlist(void **state);
void test_pop_ulong_back_dlist(void **state);
void test_pop_llong_back_dlist(void **state);
void test_pop_ullong_back_dlist(void **state);
void test_pop_float_back_dlist(void **state);
void test_pop_double_back_dlist(void **state);
void test_pop_ldouble_back_dlist(void **state);
void test_pop_bool_back_dlist(void **state);
void test_pop_string_back_dlist(void **state);
// ================================================================================
// ================================================================================

void test_pop_char_at_dlist(void **state);
void test_pop_uchar_at_dlist(void **state);
void test_pop_short_at_dlist(void **state);
void test_pop_ushort_at_dlist(void **state);
void test_pop_int_at_dlist(void **state);
void test_pop_uint_at_dlist(void **state);
void test_pop_long_at_dlist(void **state);
void test_pop_ulong_at_dlist(void **state);
void test_pop_llong_at_dlist(void **state);
void test_pop_ullong_at_dlist(void **state);
void test_pop_float_at_dlist(void **state);
void test_pop_double_at_dlist(void **state);
void test_pop_ldouble_at_dlist(void **state);
void test_pop_bool_at_dlist(void **state);
void test_pop_string_at_dlist(void **state);
// ================================================================================
// ================================================================================

void test_get_char_dllist(void **state);
void test_get_uchar_dllist(void **state);
void test_get_short_dllist(void **state);
void test_get_ushort_dllist(void **state);
void test_get_int_dllist(void **state);
void test_get_uint_dllist(void **state);
void test_get_long_dllist(void **state);
void test_get_ulong_dllist(void **state);
void test_get_llong_dllist(void **state);
void test_get_ullong_dllist(void **state);
void test_get_float_dllist(void **state);
void test_get_double_dllist(void **state);
void test_get_ldouble_dllist(void **state);
void test_get_bool_dllist(void **state);
void test_get_string_dllist(void **state);
// ================================================================================ 
// ================================================================================

void test_char_dllist_iterator(void **state);
void test_uchar_dllist_iterator(void **state);
void test_short_dllist_iterator(void **state);
void test_ushort_dllist_iterator(void **state);
void test_int_dllist_iterator(void **state);
void test_uint_dllist_iterator(void **state);
void test_long_dllist_iterator(void **state);
void test_ulong_dllist_iterator(void **state);
void test_llong_dllist_iterator(void **state);
void test_ullong_dllist_iterator(void **state);
void test_float_dllist_iterator(void **state);
void test_double_dllist_iterator(void **state);
void test_ldouble_dllist_iterator(void **state);
void test_bool_dllist_iterator(void **state);
void test_string_dllist_iterator(void **state);
// ================================================================================
// ================================================================================

void test_bubble_sort_char_dllist_forward(void **state);
void test_bubble_sort_uchar_dllist_forward(void **state);
void test_bubble_sort_short_dllist_forward(void **state);
void test_bubble_sort_ushort_dllist_forward(void **state);
void test_bubble_sort_int_dllist_forward(void **state);
void test_bubble_sort_uint_dllist_forward(void **state);
void test_bubble_sort_long_dllist_forward(void **state);
void test_bubble_sort_ulong_dllist_forward(void **state);
void test_bubble_sort_llong_dllist_forward(void **state);
void test_bubble_sort_ullong_dllist_forward(void **state);
void test_bubble_sort_float_dllist_forward(void **state);
void test_bubble_sort_double_dllist_forward(void **state);
void test_bubble_sort_ldouble_dllist_forward(void **state);
void test_bubble_sort_bool_dllist_forward(void **state);
void test_bubble_sort_string_dllist_forward(void **state);
// ================================================================================ 
// ================================================================================

void test_selection_sort_char_dllist_forward(void **state);
void test_selection_sort_uchar_dllist_forward(void **state);
void test_selection_sort_short_dllist_forward(void **state);
void test_selection_sort_ushort_dllist_forward(void **state);
void test_selection_sort_int_dllist_forward(void **state);
void test_selection_sort_uint_dllist_forward(void **state);
void test_selection_sort_long_dllist_forward(void **state);
void test_selection_sort_ulong_dllist_forward(void **state);
void test_selection_sort_llong_dllist_forward(void **state);
void test_selection_sort_ullong_dllist_forward(void **state);
void test_selection_sort_float_dllist_forward(void **state);
void test_selection_sort_double_dllist_forward(void **state);
void test_selection_sort_ldouble_dllist_forward(void **state);
void test_selection_sort_bool_dllist_forward(void **state);
void test_selection_sort_string_dllist_forward(void **state);
// ================================================================================
// ================================================================================

void test_insertion_sort_char_dllist_forward(void **state);
void test_insertion_sort_uchar_dllist_forward(void **state);
void test_insertion_sort_short_dllist_forward(void **state);
void test_insertion_sort_ushort_dllist_forward(void **state);
void test_insertion_sort_int_dllist_forward(void **state);
void test_insertion_sort_uint_dllist_forward(void **state);
void test_insertion_sort_long_dllist_forward(void **state);
void test_insertion_sort_ulong_dllist_forward(void **state);
void test_insertion_sort_llong_dllist_forward(void **state);
void test_insertion_sort_ullong_dllist_forward(void **state);
void test_insertion_sort_float_dllist_forward(void **state);
void test_insertion_sort_double_dllist_forward(void **state);
void test_insertion_sort_ldouble_dllist_forward(void **state);
void test_insertion_sort_bool_dllist_forward(void **state);
void test_insertion_sort_string_dllist_forward(void **state);
// ================================================================================
// ================================================================================

void test_merge_sort_char_dllist_forward(void **state);
void test_merge_sort_uchar_dllist_forward(void **state);
void test_merge_sort_short_dllist_forward(void **state);
void test_merge_sort_ushort_dllist_forward(void **state);
void test_merge_sort_int_dllist_forward(void **state);
void test_merge_sort_uint_dllist_forward(void **state);
void test_merge_sort_long_dllist_forward(void **state);
void test_merge_sort_ulong_dllist_forward(void **state);
void test_merge_sort_llong_dllist_forward(void **state);
void test_merge_sort_ullong_dllist_forward(void **state);
void test_merge_sort_float_dllist_forward(void **state);
void test_merge_sort_double_dllist_forward(void **state);
void test_merge_sort_ldouble_dllist_forward(void **state);
void test_merge_sort_bool_dllist_forward(void **state);
void test_merge_sort_string_dllist_forward(void **state);
// ================================================================================
// ================================================================================

void test_bubble_sort_char_dllist_reverse(void **state);
void test_bubble_sort_uchar_dllist_reverse(void **state);
void test_bubble_sort_short_dllist_reverse(void **state);
void test_bubble_sort_ushort_dllist_reverse(void **state);
void test_bubble_sort_int_dllist_reverse(void **state);
void test_bubble_sort_uint_dllist_reverse(void **state);
void test_bubble_sort_long_dllist_reverse(void **state);
void test_bubble_sort_ulong_dllist_reverse(void **state);
void test_bubble_sort_llong_dllist_reverse(void **state);
void test_bubble_sort_ullong_dllist_reverse(void **state);
void test_bubble_sort_float_dllist_reverse(void **state);
void test_bubble_sort_double_dllist_reverse(void **state);
void test_bubble_sort_ldouble_dllist_reverse(void **state);
void test_bubble_sort_bool_dllist_reverse(void **state);
void test_bubble_sort_string_dllist_reverse(void **state);
// ================================================================================
// ================================================================================

void test_selection_sort_char_dllist_reverse(void **state);
void test_selection_sort_uchar_dllist_reverse(void **state);
void test_selection_sort_short_dllist_reverse(void **state);
void test_selection_sort_ushort_dllist_reverse(void **state);
void test_selection_sort_int_dllist_reverse(void **state);
void test_selection_sort_uint_dllist_reverse(void **state);
void test_selection_sort_long_dllist_reverse(void **state);
void test_selection_sort_ulong_dllist_reverse(void **state);
void test_selection_sort_llong_dllist_reverse(void **state);
void test_selection_sort_ullong_dllist_reverse(void **state);
void test_selection_sort_float_dllist_reverse(void **state);
void test_selection_sort_double_dllist_reverse(void **state);
void test_selection_sort_ldouble_dllist_reverse(void **state);
void test_selection_sort_bool_dllist_reverse(void **state);
void test_selection_sort_string_dllist_reverse(void **state);
// ================================================================================
// ================================================================================

void test_insertion_sort_char_dllist_reverse(void **state);
void test_insertion_sort_uchar_dllist_reverse(void **state);
void test_insertion_sort_short_dllist_reverse(void **state);
void test_insertion_sort_ushort_dllist_reverse(void **state);
void test_insertion_sort_int_dllist_reverse(void **state);
void test_insertion_sort_uint_dllist_reverse(void **state);
void test_insertion_sort_long_dllist_reverse(void **state);
void test_insertion_sort_ulong_dllist_reverse(void **state);
void test_insertion_sort_llong_dllist_reverse(void **state);
void test_insertion_sort_ullong_dllist_reverse(void **state);
void test_insertion_sort_float_dllist_reverse(void **state);
void test_insertion_sort_double_dllist_reverse(void **state);
void test_insertion_sort_ldouble_dllist_reverse(void **state);
void test_insertion_sort_bool_dllist_reverse(void **state);
void test_insertion_sort_string_dllist_reverse(void **state);
// ================================================================================
// ================================================================================

void test_merge_sort_char_dllist_reverse(void **state);
void test_merge_sort_uchar_dllist_reverse(void **state);
void test_merge_sort_short_dllist_reverse(void **state);
void test_merge_sort_ushort_dllist_reverse(void **state);
void test_merge_sort_int_dllist_reverse(void **state);
void test_merge_sort_uint_dllist_reverse(void **state);
void test_merge_sort_long_dllist_reverse(void **state);
void test_merge_sort_ulong_dllist_reverse(void **state);
void test_merge_sort_llong_dllist_reverse(void **state);
void test_merge_sort_ullong_dllist_reverse(void **state);
void test_merge_sort_float_dllist_reverse(void **state);
void test_merge_sort_double_dllist_reverse(void **state);
void test_merge_sort_ldouble_dllist_reverse(void **state);
void test_merge_sort_bool_dllist_reverse(void **state);
void test_merge_sort_string_dllist_reverse(void **state);
// ================================================================================
// ================================================================================ 
#endif /* test_H */
// ================================================================================
// ================================================================================
// eof
