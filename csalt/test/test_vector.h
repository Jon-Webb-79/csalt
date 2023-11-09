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

#ifndef test_H
#define test_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

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

#endif /* test_H */
// ================================================================================
// ================================================================================
// eof
