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
// TEST INIT STATIC AND DYNAMIC ARENASA

void test_init_dynamic_arena(void **state);
void test_init_dynamic_arena_no_bytes(void **state);
void test_init_dynamic_arena_large_chunk(void **state);
void test_init_static_arena(void **state);
void test_invalid_free_static_arena(void **state);
void test_default_alignment(void **state);
void test_set_default_alignment(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST ARENA ALLOC FUNCTIONS

void test_alloc_dynamic_arena(void **state);
void test_alloc_dynamic_arena_zeroed(void **state);
void test_alloc_dynamic_arena_null_value(void **state);
void test_alloc_dynamic_arena_zero_input(void **state);

void test_alloc_static_arena(void **state);
void test_alloc_static_arena_zeroed(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST RE-ALLOCATION STRATEGY 

static inline void assert_aligned_ptr(const void* p, size_t a) {
    assert_non_null(p);
    assert_true(((uintptr_t)p & (a - 1u)) == 0u);
}

int setup_small_arena(void **state);
int teardown_arena(void **state);

/* Tests */
void test_no_growth_within_capacity(void **state);
void test_geometric_growth_unaligned(void **state);
void test_aligned_growth_and_alignment(void **state);
void test_multiple_geometric_steps(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST ARENA_ALLOC_ALIGNED 

void test_aligned_basic_default_alignment(void **state);
void test_aligned_stricter_no_growth_after_misalignment(void **state);
void test_aligned_zeroed_memory(void **state);
void test_aligned_invalid_inputs(void **state);
void test_aligned_static_arena_within_capacity(void **state);
// ================================================================================ 
// ================================================================================ 
#endif /* test_arena_H */
// ================================================================================
// ================================================================================
// eof
