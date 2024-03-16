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
#endif /* test_H */
// ================================================================================
// ================================================================================
// eof
