// ================================================================================
// ================================================================================
// - File:    test_avl.h
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

#ifndef test_AVL_H
#define test_AVL_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "../include/avl.h"
#include "../include/print.h"
// ================================================================================ 
// ================================================================================ 

void test_init_char_avl(void **state);
void test_init_uchar_avl(void **state);
void test_init_short_avl(void **state);
void test_init_ushort_avl(void **state);
void test_init_int_avl(void **state);
void test_init_uint_avl(void **state);
void test_init_long_avl(void **state);
void test_init_ulong_avl(void **state);
void test_init_llong_avl(void **state);
void test_init_ullong_avl(void **state);
void test_init_float_avl(void **state);
void test_init_double_avl(void **state);
void test_init_ldouble_avl(void **state);
void test_init_bool_avl(void **state);
void test_init_string_avl(void **state);
// ================================================================================
// ================================================================================

void test_insert_char_avl_no_duplicates(void **state);
void test_insert_char_avl_duplicates(void **state);
void test_insert_uchar_avl_no_duplicates(void **state);
void test_insert_uchar_avl_duplicates(void **state);
void test_insert_short_avl_no_duplicates(void **state);
void test_insert_short_avl_duplicates(void **state);
void test_insert_ushort_avl_no_duplicates(void **state);
void test_insert_ushort_avl_duplicates(void **state);
void test_insert_int_avl_no_duplicates(void **state);
void test_insert_int_avl_duplicates(void **state);
void test_insert_uint_avl_no_duplicates(void **state);
void test_insert_uint_avl_duplicates(void **state);
void test_insert_long_avl_no_duplicates(void **state);
void test_insert_long_avl_duplicates(void **state);
void test_insert_ulong_avl_no_duplicates(void **state);
void test_insert_ulong_avl_duplicates(void **state);
void test_insert_llong_avl_no_duplicates(void **state);
void test_insert_llong_avl_duplicates(void **state);
void test_insert_ullong_avl_no_duplicates(void **state);
void test_insert_ullong_avl_duplicates(void **state);
void test_insert_float_avl_no_duplicates(void **state);
void test_insert_float_avl_duplicates(void **state);
void test_insert_double_avl_no_duplicates(void **state);
void test_insert_double_avl_duplicates(void **state);
void test_insert_ldouble_avl_no_duplicates(void **state);
void test_insert_ldouble_avl_duplicates(void **state);
void test_insert_bool_avl_no_duplicates(void **state);
void test_insert_bool_avl_duplicates(void **state);
void test_insert_string_avl_no_duplicates(void **state);
void test_insert_string_avl_duplicates(void **state);
// ================================================================================
// ================================================================================

void test_char_avltree_gbc(void **state);
void test_uchar_avltree_gbc(void **state);
void test_short_avltree_gbc(void **state);
void test_ushort_avltree_gbc(void **state);
void test_int_avltree_gbc(void **state);
void test_uint_avltree_gbc(void **state);
void test_long_avltree_gbc(void **state);
void test_ulong_avltree_gbc(void **state);
void test_llong_avltree_gbc(void **state);
void test_ullong_avltree_gbc(void **state);
void test_float_avltree_gbc(void **state);
void test_double_avltree_gbc(void **state);
void test_ldouble_avltree_gbc(void **state);
void test_bool_avltree_gbc(void **state);
void test_string_avltree_gbc(void **state);
// ================================================================================ 
// ================================================================================

void test_min_char_avltree(void **state);
void test_min_uchar_avltree(void **state);
void test_min_short_avltree(void **state);
void test_min_ushort_avltree(void **state);
void test_min_int_avltree(void **state);
void test_min_uint_avltree(void **state);
void test_min_long_avltree(void **state);
void test_min_ulong_avltree(void **state);
void test_min_llong_avltree(void **state);
void test_min_ullong_avltree(void **state);
void test_min_float_avltree(void **state);
void test_min_double_avltree(void **state);
void test_min_ldouble_avltree(void **state);
void test_min_bool_avltree(void **state);
void test_min_string_avltree(void **state);
// ================================================================================
// ================================================================================ 
#endif /* test_AVL_H */
// ================================================================================
// ================================================================================
// eof
