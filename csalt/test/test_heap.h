// ================================================================================
// ================================================================================
// - File:    test_heap.h
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

#ifndef test_heap_H
#define test__heap_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "../include/heap.h"
#include "../include/print.h"
// ================================================================================
// ================================================================================
// TEST ENQUE MIN HEAP

void test_min_enqueue_char(void **state);
void test_min_enqueue_uchar(void **state);
void test_min_enqueue_short(void **state);
void test_min_enqueue_ushort(void **state);
void test_min_enqueue_int(void **state);
void test_min_enqueue_uint(void **state);
void test_min_enqueue_long(void **state);
void test_min_enqueue_ulong(void **state);
void test_min_enqueue_llong(void **state);
void test_min_enqueue_ullong(void **state);
void test_min_enqueue_float(void **state);
void test_min_enqueue_double(void **state);
void test_min_enqueue_ldouble(void **state);
void test_min_enqueue_bool(void **state);
void test_min_enqueue_string(void **state);
// ================================================================================
// ================================================================================
// TEST MIN HEAP Garbage Collection

void test_min_enqueue_char_gbc(void **state);
void test_min_enqueue_uchar_gbc(void **state);
void test_min_enqueue_short_gbc(void **state);
void test_min_enqueue_ushort_gbc(void **state);
void test_min_enqueue_int_gbc(void **state);
void test_min_enqueue_uint_gbc(void **state);
void test_min_enqueue_long_gbc(void **state);
void test_min_enqueue_ulong_gbc(void **state);
void test_min_enqueue_llong_gbc(void **state);
void test_min_enqueue_ullong_gbc(void **state);
void test_min_enqueue_float_gbc(void **state);
void test_min_enqueue_double_gbc(void **state);
void test_min_enqueue_ldouble_gbc(void **state);
void test_min_enqueue_bool_gbc(void **state);
void test_min_enqueue_string_gbc(void **state);
#endif /* test_heap_H */
// ================================================================================
// ================================================================================
// eof
