// ================================================================================
// ================================================================================
// - File:    test_hash.h
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

#ifndef test_char_H
#define test_char_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "../include/hash.h"
#include "../include/print.h"
// ================================================================================
// ================================================================================

void test_char_hash_map(void **state);
void test_uchar_hash_map(void **state);
void test_short_hash_map(void **state);
void test_ushort_hash_map(void **state);
void test_int_hash_map(void **state);
void test_uint_hash_map(void **state);
void test_long_hash_map(void **state);
void test_ulong_hash_map(void **state);
void test_llong_hash_map(void **state);
void test_ullong_hash_map(void **state);
void test_float_hash_map(void **state);
void test_double_hash_map(void **state);
void test_ldouble_hash_map(void **state);
void test_bool_hash_map(void **state);
void test_string_hash_map(void **state);
// ================================================================================
// ================================================================================

void test_char_hash_map_gbc(void **state);
void test_uchar_hash_map_gbc(void **state);
void test_short_hash_map_gbc(void **state);
void test_ushort_hash_map_gbc(void **state);
void test_int_hash_map_gbc(void **state);
void test_uint_hash_map_gbc(void **state);
void test_long_hash_map_gbc(void **state);
void test_ulong_hash_map_gbc(void **state);
void test_llong_hash_map_gbc(void **state);
void test_ullong_hash_map_gbc(void **state);
void test_float_hash_map_gbc(void **state);
void test_double_hash_map_gbc(void **state);
void test_ldouble_hash_map_gbc(void **state);
void test_bool_hash_map_gbc(void **state);
void test_string_hash_map_gbc(void **state);
// ================================================================================
// ================================================================================ 
#endif /* test_H */
// ================================================================================
// ================================================================================
// eof
