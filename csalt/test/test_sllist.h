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
#include "../include/print.h"
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
#endif /* test_sllist_H */
// ================================================================================
// ================================================================================
// eof
