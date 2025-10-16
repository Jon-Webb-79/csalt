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
    uint8_t buffer[400];
    Arena* arena = init_static_arena(buffer, 400);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena); 
    assert_int_equal(alloc_type, STATIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 304);
    assert_int_equal(total_alloc, 400);
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
// eof
