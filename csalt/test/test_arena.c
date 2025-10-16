// ================================================================================
// ================================================================================
// - File:    test_arena.c
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

#include "test_arena.h"
#include "c_arena.h"

#include <stdint.h>
#include <errno.h>
#include <stdalign.h>
#include <stddef.h>
#include <string.h>
// ================================================================================ 
// ================================================================================ 

void test_init_dynamic_arena(void **state)
{
    // Valgrind test to ensure memory is properly created and freed
	(void) state;
    Arena* arena = init_dynamic_arena(1000);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena);
    assert_int_equal(alloc_type, DYNAMIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 4096);
    assert_int_equal(total_alloc, 4184);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

void test_init_dynamic_arena_no_bytes(void **state) {
    // Verify allocation if user requests 0 bytes
    // Will throw compiler error if user tries to pass a negative number
	(void) state;
    Arena* arena = init_dynamic_arena(0);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena); 
    assert_int_equal(alloc_type, DYNAMIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 4096);
    assert_int_equal(total_alloc, 4184);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

void test_init_dynamic_arena_large_chunk(void **state) {
    // - Verify allocation if user requests more than 4096 bytes to ensure 
    //   proper padding is added
	(void) state;
    Arena* arena = init_dynamic_arena(4097);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena); 
    assert_int_equal(alloc_type, DYNAMIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 4112);
    assert_int_equal(total_alloc, 4200);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

void test_init_static_arena(void **state) {
    // Test for proper initialization of a static arena
    (void) state;
    uint8_t buffer[4097];
    Arena* arena = init_static_arena(buffer, 4097);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena); 
    assert_int_equal(alloc_type, STATIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 4001);
    assert_int_equal(total_alloc, 4097);
}
// -------------------------------------------------------------------------------- 

void test_invalid_free_static_arena(void **state) {
    // Test behavior when an attempt is made to free a static arena
    (void) state;
    uint8_t buffer[400];
    Arena* arena = init_static_arena(buffer, 400);
    free_arena(arena);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena);
    assert_int_equal(EINVAL, errno);
    assert_int_equal(alloc_type, STATIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 304);
    assert_int_equal(total_alloc, 400);
}
// -------------------------------------------------------------------------------- 

void test_default_alignment(void **state) {
    (void) state;
    size_t val = default_arena_alignment();
    assert_int_equal(val, alignof(max_align_t));
}
// -------------------------------------------------------------------------------- 

void test_set_default_alignment(void **state) {
    (void) state;
    set_default_arena_alignment(4096);
    assert_int_equal(4096, default_arena_alignment());
    reset_default_arena_alignment();
    assert_int_equal(alignof(max_align_t), default_arena_alignment());
}
// ================================================================================ 
// ================================================================================ 
// TEST ARENA ALLOC FUNCTIONS 

// struct for test
typedef struct {
    float one;
    int two;
} test;
// -------------------------------------------------------------------------------- 

void test_alloc_dynamic_arena(void **state) {
    (void) state;
    Arena* arena = init_dynamic_arena(10000);
    test* struct_val = (test*)alloc_arena(arena, sizeof(test), false);
    int* value = (int*)alloc_arena(arena, sizeof(int), false);
    struct_val->one = 3.4f;
    struct_val->two = 3;
    *value = 4;
    assert_int_equal(*value, 4);
    assert_int_equal(struct_val->two, 3);
    assert_float_equal(struct_val->one, 3.4, 0.001);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    size_t left_over = arena_remaining(arena);
    assert_int_equal(size, 20);
    assert_int_equal(alloc, 10000);
    assert_int_equal(total_alloc, 10088);
    assert_int_equal(left_over, 9980);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

void test_alloc_dynamic_arena_zeroed(void **state) {
    (void)state;

    Arena* arena = init_dynamic_arena(10000);
    assert_non_null(arena);

    test* struct_val = alloc_arena(arena, sizeof *struct_val, true);
    assert_non_null(struct_val);

    // verify every byte (including padding) is zero
    unsigned char zeros[sizeof *struct_val];
    memset(zeros, 0, sizeof zeros);
    assert_memory_equal(struct_val, zeros, sizeof *struct_val);

    // …the rest of your test…
    int* value = alloc_arena(arena, sizeof *value, false);
    assert_non_null(value);

    struct_val->one = 3.4f;
    struct_val->two = 3;
    *value = 4;

    assert_int_equal(*value, 4);
    assert_int_equal(struct_val->two, 3);
    assert_float_equal(struct_val->one, 3.4, 0.001);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

void test_alloc_dynamic_arena_null_value(void **state) {
    (void) state;
    int* value = alloc_arena(NULL, sizeof(*value), true);
    assert_null(value);
    assert_int_equal(EINVAL, errno);
}
// -------------------------------------------------------------------------------- 

void test_alloc_dynamic_arena_zero_input(void **state) {
    Arena* arena = init_dynamic_arena(10000);
    errno = 0;
    int* value = alloc_arena(arena, 0, true);
    assert_null(value);
    assert_int_equal(EINVAL, errno);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

void test_alloc_static_arena(void **state) {
    (void) state;
    uint8_t buffer[10000];
    Arena* arena = init_static_arena(buffer, 10000);
    test* struct_val = (test*)alloc_arena(arena, sizeof(test), false);
    int* value = (int*)alloc_arena(arena, sizeof(int), false);
    struct_val->one = 3.4f;
    struct_val->two = 3;
    *value = 4;
    assert_int_equal(*value, 4);
    assert_int_equal(struct_val->two, 3);
    assert_float_equal(struct_val->one, 3.4, 0.001);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    size_t left_over = arena_remaining(arena);
    assert_int_equal(size, 20);
    assert_int_equal(alloc, 9904);
    assert_int_equal(total_alloc, 10000);
    assert_int_equal(left_over, 9884);
}
// -------------------------------------------------------------------------------- 

void test_alloc_static_arena_zeroed(void **state) {
    (void)state;

    uint8_t buffer[10000];
    Arena* arena = init_static_arena(buffer, 10000);
    assert_non_null(arena);

    test* struct_val = alloc_arena(arena, sizeof *struct_val, true);
    assert_non_null(struct_val);

    // verify every byte (including padding) is zero
    unsigned char zeros[sizeof *struct_val];
    memset(zeros, 0, sizeof zeros);
    assert_memory_equal(struct_val, zeros, sizeof *struct_val);
    
    int* value = alloc_arena(arena, sizeof *value, false);
    assert_non_null(value);

    struct_val->one = 3.4f;
    struct_val->two = 3;
    *value = 4;

    assert_int_equal(*value, 4);
    assert_int_equal(struct_val->two, 3);
    assert_float_equal(struct_val->one, 3.4, 0.001);
}
// ================================================================================
// ================================================================================
// eof
