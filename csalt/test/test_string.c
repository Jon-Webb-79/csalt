// ================================================================================
// ================================================================================
// - File:    test_error.c
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

#include "c_string.h"
#include "c_allocator.h"
#include "test_suite.h"

#include <limits.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
// ================================================================================ 
// ================================================================================ 
// TEST ERROR_TO_STRING 

// =============================================================================
// Core construction behavior
// =============================================================================

static void test_string_init_default_full_copy(void **state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("hello world!", 0, a);

    assert_true(r.has_value);
    //printf("%s\n", error_to_string(r.u.error));

    string_t* s = r.u.value;

    assert_string_equal(const_string(s), "hello world!");
    assert_int_equal(string_size(s), strlen("hello world!"));
    assert_int_equal(string_alloc(s), strlen("hello world!") + 1);

    return_string(s);
}
// -------------------------------------------------------------------------------- 

static void test_string_init_truncate_to_capacity(void **state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("hello world!", 2, a);

    assert_true(r.has_value);

    string_t* s = r.u.value;

    assert_string_equal(const_string(s), "he");
    assert_int_equal(string_size(s), 2);
    assert_int_equal(string_alloc(s), 3);

    return_string(s);
}
// -------------------------------------------------------------------------------- 

static void test_string_init_exact_capacity_copy(void **state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    size_t cap = strlen("hello world!");

    string_expect_t r = init_string("hello world!", cap, a);

    assert_true(r.has_value);

    string_t* s = r.u.value;

    assert_string_equal(const_string(s), "hello world!");
    assert_int_equal(string_size(s), cap);
    assert_int_equal(string_alloc(s), cap + 1);

    return_string(s);
}
// -------------------------------------------------------------------------------- 

static void test_string_init_larger_capacity_has_slack(void **state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("hello world!", 20, a);

    assert_true(r.has_value);

    string_t* s = r.u.value;

    assert_string_equal(const_string(s), "hello world!");
    assert_int_equal(string_size(s), strlen("hello world!"));
    assert_int_equal(string_alloc(s), 21);

    return_string(s);
}
// -------------------------------------------------------------------------------- 

static void test_string_init_null_input_returns_error(void **state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string(NULL, 0, a);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
// -------------------------------------------------------------------------------- 

static void test_string_getters_on_null_are_safe(void **state) {
    (void)state;

    assert_null(const_string(NULL));
    assert_int_equal(string_size(NULL), 0);
    assert_int_equal(string_alloc(NULL), 0);
}
// -------------------------------------------------------------------------------- 

static void test_string_init_empty_literal_default_capacity(void **state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("", 0, a);

    assert_true(r.has_value);

    string_t* s = r.u.value;

    assert_string_equal(const_string(s), "");
    assert_int_equal(string_size(s), 0);
    assert_int_equal(string_alloc(s), 1);

    return_string(s);
}
// -------------------------------------------------------------------------------- 

static void test_string_init_empty_literal_with_capacity(void **state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("", 5, a);

    assert_true(r.has_value);

    string_t* s = r.u.value;

    assert_string_equal(const_string(s), "");
    assert_int_equal(string_size(s), 0);
    assert_int_equal(string_alloc(s), 6);

    return_string(s);
}
// -------------------------------------------------------------------------------- 

static void test_string_init_capacity_one_truncates_to_single_char(void **state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("abc", 1, a);

    assert_true(r.has_value);

    string_t* s = r.u.value;

    assert_string_equal(const_string(s), "a");
    assert_int_equal(string_size(s), 1);
    assert_int_equal(string_alloc(s), 2);

    return_string(s);
}
// -------------------------------------------------------------------------------- 

static void test_string_arena_init_default_full_copy(void **state) {
    (void)state;

    arena_expect_t ar = init_dynamic_arena(/*bytes=*/8192u, /*resize=*/false,
                                          /*min_chunk_in=*/0u,
                                          /*base_align_in=*/0u);
    assert_true(ar.has_value);
    arena_t* arena = ar.u.value;

    allocator_vtable_t a = arena_allocator(arena);

    string_expect_t r = init_string("hello world!", 0u, a);
    assert_true(r.has_value);

    string_t* s = r.u.value;

    assert_string_equal(const_string(s), "hello world!");
    assert_int_equal(string_size(s), strlen("hello world!"));
    assert_int_equal(string_alloc(s), strlen("hello world!") + 1u);

    // NOTE: return_string() won't reclaim arena allocations (arena_v_return is a no-op).
    // It is safe to call, but it doesn't "free" the memory.
    return_string(s);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_string_arena_init_truncate_and_slack(void **state) {
    (void)state;

    arena_expect_t ar = init_dynamic_arena(/*bytes=*/8192u, /*resize=*/false,
                                          /*min_chunk_in=*/0u,
                                          /*base_align_in=*/0u);
    assert_true(ar.has_value);
    arena_t* arena = ar.u.value;

    allocator_vtable_t a = arena_allocator(arena);

    // Truncate to 2 chars + NUL
    string_expect_t r1 = init_string("hello world!", 2u, a);
    assert_true(r1.has_value);
    assert_string_equal(const_string(r1.u.value), "he");
    assert_int_equal(string_size(r1.u.value), 2u);
    assert_int_equal(string_alloc(r1.u.value), 3u);
    return_string(r1.u.value);

    // Slack capacity: request 20 payload chars -> alloc 21 bytes
    string_expect_t r2 = init_string("hello world!", 20u, a);
    assert_true(r2.has_value);
    assert_string_equal(const_string(r2.u.value), "hello world!");
    assert_int_equal(string_size(r2.u.value), strlen("hello world!"));
    assert_int_equal(string_alloc(r2.u.value), 21u);
    return_string(r2.u.value);
    free_arena(arena);
}

// ================================================================================ 
// ================================================================================ 

const struct CMUnitTest test_string[] = {
    cmocka_unit_test(test_string_init_default_full_copy),
    cmocka_unit_test(test_string_init_truncate_to_capacity),
    cmocka_unit_test(test_string_init_exact_capacity_copy),
    cmocka_unit_test(test_string_init_larger_capacity_has_slack),

    cmocka_unit_test(test_string_init_null_input_returns_error),
    cmocka_unit_test(test_string_getters_on_null_are_safe),

    cmocka_unit_test(test_string_init_empty_literal_default_capacity),
    cmocka_unit_test(test_string_init_empty_literal_with_capacity),
    cmocka_unit_test(test_string_init_capacity_one_truncates_to_single_char),

    cmocka_unit_test(test_string_arena_init_default_full_copy),
    cmocka_unit_test(test_string_arena_init_truncate_and_slack)
};

const size_t test_string_count = sizeof(test_string) / sizeof(test_string[0]);
// ================================================================================
// ================================================================================
// eof
