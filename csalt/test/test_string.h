// ================================================================================
// ================================================================================
// - File:    test_string.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    October 20, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef test_str_H
#define test_str_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <unistd.h>
#include <fcntl.h>

#include "../include/str.h"

void test_string_init_one_var(void **state);
void test_string_init_two_var(void **state);
void test_string_init_two_var_less(void **state);
void test_string_init_one_var_gbc(void **state);
void test_string_init_two_var_gbc(void **state);
void test_string_init_two_var_less_gbc(void **state);
void test_get_string_fail_one(void **state);
void test_get_string_fail_two(void **state);
void test_string_length_fail_one(void **state);
void test_string_length_fail_two(void **state);
void test_insert_string_lit_insert_end(void **state);
void test_insert_string_lit_insert_middle(void **state);
void test_insert_string_lit_error_one(void **state);
void test_insert_string_lit_error_two(void **state);
void test_insert_string_lit_error_three(void **state);
void test_insert_string_str_insert_end(void **state);
void test_insert_string_str_insert_middle(void **state);
void test_trim_string_equal(void **state);
void test_trim_string_greater(void **state);
void test_trim_string_error_one(void **state);
void test_trim_string_error_two(void **state);
void test_copy_string(void **state);
#endif /* test_str_H */
// ================================================================================
// ================================================================================
// eof
