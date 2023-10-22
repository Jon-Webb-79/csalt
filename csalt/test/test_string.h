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

#include "../include/str.h"

void test_string_init_one_var(void **state);
void test_string_init_two_var(void **state);
void test_string_init_two_var_less(void **state);
void test_string_init_one_var_gbc(void **state);
void test_string_init_two_var_gbc(void **state);
void test_string_init_two_var_less_gbc(void **state);
#endif /* test_str_H */
// ================================================================================
// ================================================================================
// eof
