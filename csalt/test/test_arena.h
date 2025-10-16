// ================================================================================
// ================================================================================
// - File:    test_arena.h
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

#ifndef test_arena_H
#define test_arena_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
// ================================================================================ 
// ================================================================================ 

void test_init_dynamic_arena(void **state);
void test_init_dynamic_arena_no_bytes(void **state);
void test_init_dynamic_arena_large_chunk(void **state);
void test_init_static_arena(void **state);
void test_invalid_free_static_arena(void **state);
void test_default_alignment(void **state);
void test_set_default_alignment(void **state);

// ================================================================================ 
// ================================================================================ 
#endif /* test_arena_H */
// ================================================================================
// ================================================================================
// eof
