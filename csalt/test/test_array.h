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
#endif /* test_H */
// ================================================================================
// ================================================================================
// eof
