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
// -------------------------------------------------------------------------------- 

// -----------------------------------------------------------------------------
// Pool test helper
// -----------------------------------------------------------------------------
static allocator_vtable_t make_string_pool_allocator_or_fail(void) {
    /*
     * Choose conservative pool params:
     * - block_size: large enough for typical string_t objects and short buffers
     * - alignment: 0 => defaults to max_align_t inside pool
     * - blocks_per_chunk: 64 blocks per slice
     * - arena_seed_bytes: enough room for header + 1 slice; allow growth
     * - min_chunk_bytes: 0 => allow arena policy defaults
     * - grow_enabled: true (so realloc / more blocks can be satisfied)
     * - prewarm_one_chunk: true (so pool is immediately usable)
     */
    size_t block_size        = 256u;
    size_t alignment         = 0u;
    size_t blocks_per_chunk  = 64u;
    size_t arena_seed_bytes  = 64u * 1024u;
    size_t min_chunk_bytes   = 0u;
    bool   grow_enabled      = true;
    bool   prewarm_one_chunk = true;

    pool_expect_t pe = init_dynamic_pool(block_size,
                                         alignment,
                                         blocks_per_chunk,
                                         arena_seed_bytes,
                                         min_chunk_bytes,
                                         grow_enabled,
                                         prewarm_one_chunk);
    assert_true(pe.has_value);
    assert_non_null(pe.u.value);

    pool_t* p = pe.u.value;
    return pool_allocator(p);
}

static void destroy_pool_allocator(allocator_vtable_t* a) {
    if (!a || !a->deallocate) return;
    a->deallocate(a->ctx);  // pool_v_free(ctx) should free pool + owned arena
    a->ctx = NULL;
}

// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------

static void test_string_pool_init_and_concat_cstr(void **state) {
    (void)state;

    allocator_vtable_t a = make_string_pool_allocator_or_fail();

    string_expect_t r = init_string("Hello", 0u, a);
    assert_true(r.has_value);

    string_t* s = r.u.value;

    assert_true(str_concat(s, ", "));
    assert_true(str_concat(s, "world!"));

    assert_string_equal(const_string(s), "Hello, world!");
    assert_int_equal(string_size(s), strlen("Hello, world!"));
    assert_int_equal(string_alloc(s), strlen("Hello, world!") + 1u);

    return_string(s);
    destroy_pool_allocator(&a);
}

static void test_string_pool_init_truncate_then_concat(void **state) {
    (void)state;

    allocator_vtable_t a = make_string_pool_allocator_or_fail();

    // capacity=2 => alloc=3 => "he"
    string_expect_t r = init_string("hello world!", 2u, a);
    assert_true(r.has_value);

    string_t* s = r.u.value;

    assert_string_equal(const_string(s), "he");
    assert_int_equal(string_size(s), 2u);

    // should grow and append
    assert_true(str_concat(s, "llo"));
    assert_string_equal(const_string(s), "hello");
    assert_int_equal(string_size(s), 5u);

    return_string(s);
    destroy_pool_allocator(&a);
}

static void test_string_pool_string_concat_appends_other_string(void **state) {
    (void)state;

    allocator_vtable_t a = make_string_pool_allocator_or_fail();

    string_expect_t r1 = init_string("CSalt", 0u, a);
    string_expect_t r2 = init_string(" Library", 0u, a);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    string_t* s1 = r1.u.value;
    string_t* s2 = r2.u.value;

    assert_true(string_concat(s1, s2));
    assert_string_equal(const_string(s1), "CSalt Library");
    assert_int_equal(string_size(s1), strlen("CSalt Library"));

    return_string(s2);
    return_string(s1);
    destroy_pool_allocator(&a);
}

static void test_string_pool_concat_overlap_source_inside_dest(void **state) {
    (void)state;

    allocator_vtable_t a = make_string_pool_allocator_or_fail();

    // Create with slack to make overlap pointer stable, but still allow growth
    string_expect_t r = init_string("abcdef", 32u, a);
    assert_true(r.has_value);

    string_t* s = r.u.value;

    // Source points inside destination buffer ("cdef")
    const char* inside = const_string(s) + 2;

    // Append "cdef" => "abcdefcdef"
    assert_true(str_concat(s, inside));
    assert_string_equal(const_string(s), "abcdefcdef");

    return_string(s);
    destroy_pool_allocator(&a);
}

#if ARENA_USE_CONVENIENCE_MACROS && !defined(NO_FUNCTION_MACROS)
static void test_string_pool_concat_macro_dispatches_cstr(void **state) {
    (void)state;

    allocator_vtable_t a = make_string_pool_allocator_or_fail();

    string_expect_t r = init_string("A", 0u, a);
    assert_true(r.has_value);

    string_t* s = r.u.value;

    assert_true(concat_string(s, "B"));
    assert_string_equal(const_string(s), "AB");

    return_string(s);
    destroy_pool_allocator(&a);
}

static void test_string_pool_concat_macro_dispatches_string(void **state) {
    (void)state;

    allocator_vtable_t a = make_string_pool_allocator_or_fail();

    string_expect_t r1 = init_string("X", 0u, a);
    string_expect_t r2 = init_string("Y", 0u, a);
    assert_true(r1.has_value);
    assert_true(r2.has_value);

    string_t* s1 = r1.u.value;
    string_t* s2 = r2.u.value;

    assert_true(concat_string(s1, s2));
    assert_string_equal(const_string(s1), "XY");

    return_string(s2);
    return_string(s1);
    destroy_pool_allocator(&a);
}
#endif
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
    cmocka_unit_test(test_string_arena_init_truncate_and_slack),

    cmocka_unit_test(test_string_pool_init_and_concat_cstr),
    cmocka_unit_test(test_string_pool_init_truncate_then_concat),
    cmocka_unit_test(test_string_pool_string_concat_appends_other_string),
    cmocka_unit_test(test_string_pool_concat_overlap_source_inside_dest),

#if ARENA_USE_CONVENIENCE_MACROS && !defined(NO_FUNCTION_MACROS)
    cmocka_unit_test(test_string_pool_concat_macro_dispatches_cstr),
    cmocka_unit_test(test_string_pool_concat_macro_dispatches_string),
#endif
};

const size_t test_string_count = sizeof(test_string) / sizeof(test_string[0]);
// ================================================================================
// ================================================================================
// eof
