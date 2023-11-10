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
// ================================================================================
// ================================================================================

#endif /* test_H */
// ================================================================================
// ================================================================================
// eof
