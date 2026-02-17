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
// -------------------------------------------------------------------------------- 

/* =============================================================================
   str_compare tests
   ============================================================================= */

static void test_string_str_compare_equal(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("abc", 0u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    assert_int_equal(str_compare(s, "abc"), (int8_t)0);

    return_string(s);
}

static void test_string_str_compare_diff_middle_less(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("abc", 0u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    /* 'b' < 'x' at index 1 -> s < "axc" */
    assert_int_equal(str_compare(s, "axc"), (int8_t)-1);

    return_string(s);
}

static void test_string_str_compare_diff_middle_greater(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("axc", 0u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    /* 'x' > 'b' at index 1 -> s > "abc" */
    assert_int_equal(str_compare(s, "abc"), (int8_t)1);

    return_string(s);
}

static void test_string_str_compare_bounded_by_string_len_str_longer(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("abc", 0u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    /* All 3 chars match; C string continues -> s shorter => s < str */
    assert_int_equal(str_compare(s, "abcXYZ"), (int8_t)-1);

    return_string(s);
}

static void test_string_str_compare_bounded_by_string_len_str_shorter(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("abc", 0u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    /* C string ends early -> s is greater (since s still has non-NUL chars) */
    assert_int_equal(str_compare(s, "ab"), (int8_t)1);

    return_string(s);
}

static void test_string_str_compare_truncated_string_instance(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    /* capacity 2 => "he" (len==2) */
    string_expect_t r = init_string("hello", 2u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    assert_int_equal(string_size(s), 2u);
    assert_string_equal(const_string(s), "he");

    /* Compare bounded string "he" against longer literal "hello" -> s < str */
    assert_int_equal(str_compare(s, "hello"), (int8_t)-1);

    return_string(s);
}

// static void test_string_str_compare_invalid_args(void **state)
// {
//     (void)state;
//
//     allocator_vtable_t a = heap_allocator();
//     string_expect_t r = init_string("abc", 0u, a);
//     assert_true(r.has_value);
//
//     string_t *s = r.u.value;
//
//     assert_int_equal(str_compare(NULL, "abc"), (int8_t)INT8_MIN);
//     assert_int_equal(str_compare(s, NULL), (int8_t)INT8_MIN);
//
//     /* Corrupt state: s->str == NULL should be treated as invalid */
//     s->str = NULL;
//     
//     assert_int_equal(str_compare(s, "abc"), (int8_t)INT8_MIN);
//     
//     /* Don't call return_string(s) now because we intentionally corrupted it */
// }

/* =============================================================================
   string_compare tests
   ============================================================================= */

static void test_string_string_compare_equal(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t r1 = init_string("abc", 0u, a);
    string_expect_t r2 = init_string("abc", 0u, a);
    assert_true(r1.has_value);
    assert_true(r2.has_value);

    string_t *s1 = r1.u.value;
    string_t *s2 = r2.u.value;

    assert_int_equal(string_compare(s1, s2), (int8_t)0);

    return_string(s1);
    return_string(s2);
}

static void test_string_string_compare_less_and_greater(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t r1 = init_string("delta", 0u, a);
    string_expect_t r2 = init_string("gamma", 0u, a);
    assert_true(r1.has_value);
    assert_true(r2.has_value);

    string_t *s1 = r1.u.value;
    string_t *s2 = r2.u.value;

    assert_int_equal(string_compare(s1, s2), (int8_t)-1);
    assert_int_equal(string_compare(s2, s1), (int8_t)1);

    return_string(s1);
    return_string(s2);
}

static void test_string_string_compare_length_ordering(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t r1 = init_string("abc", 0u, a);
    string_expect_t r2 = init_string("abcd", 0u, a);
    assert_true(r1.has_value);
    assert_true(r2.has_value);

    string_t *s1 = r1.u.value;
    string_t *s2 = r2.u.value;

    /* "abc" is shorter but equal prefix => less */
    assert_int_equal(string_compare(s1, s2), (int8_t)-1);
    assert_int_equal(string_compare(s2, s1), (int8_t)1);

    return_string(s1);
    return_string(s2);
}

static void test_string_string_compare_truncated_instances(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    /* "hello" truncated to "he" */
    string_expect_t r1 = init_string("hello", 2u, a);
    /* "help" truncated to "he" as well */
    string_expect_t r2 = init_string("help", 2u, a);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    string_t *s1 = r1.u.value;
    string_t *s2 = r2.u.value;

    assert_string_equal(const_string(s1), "he");
    assert_string_equal(const_string(s2), "he");

    assert_int_equal(string_compare(s1, s2), (int8_t)0);

    return_string(s1);
    return_string(s2);
}

// static void test_string_string_compare_invalid_args(void **state)
// {
//     (void)state;
//
//     allocator_vtable_t a = heap_allocator();
//
//     string_expect_t r = init_string("abc", 0u, a);
//     assert_true(r.has_value);
//
//     string_t *s = r.u.value;
//
//     assert_int_equal(string_compare(NULL, s), (int8_t)INT8_MIN);
//     assert_int_equal(string_compare(s, NULL), (int8_t)INT8_MIN);
//
//     /* Corrupt str->str for defensive path */
//     s->str = NULL;
//     assert_int_equal(string_compare(s, s), (int8_t)INT8_MIN);
//
//     /* Don't return_string(s) now (corrupted) */
// }


static void test_string_macro_compare_dispatches_cstr(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("abc", 0u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    /* Should dispatch to str_compare(s, "abc") -> 0 */
    assert_int_equal(compare_string(s, "abc"), (int8_t)0);

    /* Should dispatch to str_compare(s, "abd") -> -1 */
    assert_int_equal(compare_string(s, "abd"), (int8_t)-1);

    return_string(s);
}

static void test_string_macro_compare_dispatches_char_ptr(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("abc", 0u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    /* Using a non-const char* RHS should still dispatch to str_compare */
    char rhs_buf[] = "abc";
    char *rhs = rhs_buf;

    assert_int_equal(compare_string(s, rhs), (int8_t)0);

    return_string(s);
}

static void test_string_macro_compare_dispatches_string_t(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t r1 = init_string("delta", 0u, a);
    string_expect_t r2 = init_string("gamma", 0u, a);
    assert_true(r1.has_value);
    assert_true(r2.has_value);

    string_t *s1 = r1.u.value;
    string_t *s2 = r2.u.value;

    /* Should dispatch to string_compare(s1, s2) -> -1 */
    assert_int_equal(compare_string(s1, s2), (int8_t)-1);

    return_string(s1);
    return_string(s2);
}

static void test_string_macro_compare_dispatches_const_string_t(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t r1 = init_string("abc", 0u, a);
    string_expect_t r2 = init_string("abc", 0u, a);
    assert_true(r1.has_value);
    assert_true(r2.has_value);

    string_t *s1 = r1.u.value;
    string_t *s2 = r2.u.value;

    const string_t *cs2 = (const string_t*)s2;

    /* Should dispatch to string_compare(s1, cs2) -> 0 */
    assert_int_equal(compare_string(s1, cs2), (int8_t)0);

    return_string(s1);
    return_string(s2);
}
// -------------------------------------------------------------------------------- 

static void test_string_reset_sets_len_zero_and_nul_terminator(void **state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("hello", 0u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    assert_int_equal(string_size(s), 5);
    assert_non_null(const_string(s));
    assert_string_equal(const_string(s), "hello");

    reset_string(s);

    assert_int_equal(string_size(s), 0);
    assert_non_null(const_string(s));
    assert_int_equal((unsigned char)const_string(s)[0], 0u);
    assert_string_equal(const_string(s), "");

    return_string(s);
}

static void test_string_reset_allows_concat_from_start(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("hello", 0u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    reset_string(s);

    /* Concatenation should start at index 0 after reset */
    assert_true(str_concat(s, "abc"));
    assert_string_equal(const_string(s), "abc");
    assert_int_equal(string_size(s), 3);

    return_string(s);
}

static void test_string_reset_idempotent(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("x", 0u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    reset_string(s);
    reset_string(s);

    assert_string_equal(const_string(s), "");
    assert_int_equal(string_size(s), 0);

    return_string(s);
}

static void test_string_reset_null_is_safe_noop(void **state)
{
    (void)state;

    /* Should not crash */
    reset_string(NULL);
}

static void test_string_reset_struct_with_null_buffer_is_safe_noop(void **state)
{
    (void)state;

    /* Create a fake string_t with no backing buffer */
    string_t fake;
    memset(&fake, 0, sizeof(fake));
    fake.str = NULL;
    fake.len = 123u;

    reset_string(&fake);

    /* We expect no crash and no change required; len may remain unchanged
       depending on your chosen semantics. The important part is: safe. */
    assert_int_equal(fake.len, 123u);
}
// -------------------------------------------------------------------------------- 

static void test_string_copy_string_copies_value_and_min_capacity(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    /* Create a source string with slack capacity */
    string_expect_t rsrc = init_string("hello", 20u, a);
    assert_true(rsrc.has_value);

    const string_t *src = rsrc.u.value;
    assert_non_null(src);

    /* Copy (value copy: capacity should be len+1, not src->alloc) */
    string_expect_t rcpy = copy_string(src, a);
    assert_true(rcpy.has_value);

    string_t *cpy = rcpy.u.value;
    assert_non_null(cpy);

    /* Same contents */
    assert_string_equal(const_string(cpy), "hello");
    assert_int_equal(string_size(cpy), 5);

    /* Minimal capacity */
    assert_int_equal(string_alloc(cpy), 6);

    /* Deep copy: different buffers and different objects */
    assert_ptr_not_equal((void*)cpy, (void*)src);
    assert_ptr_not_equal((void*)const_string(cpy), (void*)const_string(src));

    return_string((string_t*)src);
    return_string(cpy);
}

static void test_string_copy_string_allows_independent_mutation(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t rsrc = init_string("abc", 0u, a);
    assert_true(rsrc.has_value);

    string_t *src = rsrc.u.value;

    string_expect_t rcpy = copy_string(src, a);
    assert_true(rcpy.has_value);

    string_t *cpy = rcpy.u.value;

    /* Mutate the copy; source should remain unchanged */
    assert_true(str_concat(cpy, "XYZ"));
    assert_string_equal(const_string(cpy), "abcXYZ");
    assert_string_equal(const_string(src), "abc");

    return_string(src);
    return_string(cpy);
}

static void test_string_copy_string_empty_string(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t rsrc = init_string("", 0u, a);
    assert_true(rsrc.has_value);

    string_t *src = rsrc.u.value;

    string_expect_t rcpy = copy_string(src, a);
    assert_true(rcpy.has_value);

    string_t *cpy = rcpy.u.value;

    assert_string_equal(const_string(cpy), "");
    assert_int_equal(string_size(cpy), 0);
    assert_int_equal(string_alloc(cpy), 1);

    return_string(src);
    return_string(cpy);
}

static void test_string_copy_string_null_input_returns_error(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t r = copy_string(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_string_copy_string_null_buffer_returns_error(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    /* Construct a fake string_t with NULL buffer */
    string_t fake;
    memset(&fake, 0, sizeof(fake));
    fake.str = NULL;
    fake.len = 3u;
    fake.alloc = 10u;

    string_expect_t r = copy_string(&fake, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
// -------------------------------------------------------------------------------- 

static void test_string_is_string_ptr_null_inputs_return_false(void **state)
{
    (void)state;

    assert_false(is_string_ptr(NULL, (const void*)0x1));

    /* valid string pointer but NULL ptr */
    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("abc", 0u, a);
    assert_true(r.has_value);
    string_t *s = r.u.value;

    assert_false(is_string_ptr(s, NULL));

    return_string(s);
}

static void test_string_is_string_ptr_null_buffer_returns_false(void **state)
{
    (void)state;

    string_t fake;
    memset(&fake, 0, sizeof(fake));
    fake.str   = NULL;
    fake.len   = 3u;
    fake.alloc = 10u;

    assert_false(is_string_ptr(&fake, (const void*)0x1));
}

static void test_string_is_string_ptr_begin_and_end_boundaries(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    /* Give slack so alloc > len+1 */
    string_expect_t r = init_string("hello", 20u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    /* begin is valid */
    assert_true(is_string_ptr(s, (const void*)s->str));

    /* last byte in allocation is valid */
    assert_true(is_string_ptr(s, (const void*)(s->str + (s->alloc - 1u))));

    /* one-past-end is invalid */
    assert_false(is_string_ptr(s, (const void*)(s->str + s->alloc)));

    return_string(s);
}

static void test_string_is_string_ptr_pointer_before_begin_is_false(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("hello", 0u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    /* pointer before start (only do this if alloc>0 so subtraction is defined) */
    assert_true(s->alloc > 0u);

    const char *p = s->str - 1; /* not dereferenced, just compared */
    assert_false(is_string_ptr(s, (const void*)p));

    return_string(s);
}

static void test_string_is_string_ptr_inside_slack_region_is_true(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    /* Request slack capacity. Expect alloc == 21 if your init_string stores alloc including NUL */
    string_expect_t r = init_string("abc", 20u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    /* Choose a pointer beyond used length but still within allocation */
    assert_true(s->alloc > (s->len + 1u));

    const char *p = s->str + s->len + 1u; /* first slack byte */
    assert_true(is_string_ptr(s, (const void*)p));

    return_string(s);
}
// -------------------------------------------------------------------------------- 

static void test_string_is_string_ptr_sized_null_inputs_return_false(void **state)
{
    (void)state;

    assert_false(is_string_ptr_sized(NULL, (const void*)0x1, 1u));

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("abc", 0u, a);
    assert_true(r.has_value);
    string_t *s = r.u.value;

    assert_false(is_string_ptr_sized(s, NULL, 1u));
    assert_false(is_string_ptr_sized(s, (const void*)s->str, 0u)); /* bytes==0 => false */

    return_string(s);
}

static void test_string_is_string_ptr_sized_null_buffer_returns_false(void **state)
{
    (void)state;

    string_t fake;
    memset(&fake, 0, sizeof(fake));
    fake.str   = NULL;
    fake.len   = 3u;
    fake.alloc = 10u;

    assert_false(is_string_ptr_sized(&fake, (const void*)0x1, 1u));
}

static void test_string_is_string_ptr_sized_range_fits_within_used_region(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("hello", 0u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    /* "hello\0" is 6 bytes total; check the whole used region fits */
    assert_true(is_string_ptr_sized(s, (const void*)s->str, s->len + 1u));

    /* also check a sub-range inside */
    assert_true(is_string_ptr_sized(s, (const void*)(s->str + 2), 2u)); /* "ll" */

    return_string(s);
}

static void test_string_is_string_ptr_sized_range_fits_within_slack_region(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    /* request slack capacity */
    string_expect_t r = init_string("abc", 20u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    /* pick a start beyond used bytes but within alloc */
    assert_true(s->alloc > (s->len + 1u));

    const char *p = s->str + (s->len + 1u);
    size_t remaining = s->alloc - (s->len + 1u);

    /* entire slack range should fit */
    assert_true(is_string_ptr_sized(s, (const void*)p, remaining));

    return_string(s);
}

static void test_string_is_string_ptr_sized_allows_end_at_one_past_end(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("abc", 10u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    /* Start at last byte, size 1 fits (ends exactly at one-past-end) */
    assert_true(is_string_ptr_sized(s,
                                   (const void*)(s->str + (s->alloc - 1u)),
                                   1u));

    /* Start at begin, size == alloc fits (ends at one-past-end) */
    assert_true(is_string_ptr_sized(s, (const void*)s->str, s->alloc));

    return_string(s);
}

static void test_string_is_string_ptr_sized_rejects_range_past_end(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("abc", 10u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    /* Start at last byte, size 2 extends past end */
    assert_false(is_string_ptr_sized(s,
                                    (const void*)(s->str + (s->alloc - 1u)),
                                    2u));

    /* Start inside, ask for too much */
    assert_false(is_string_ptr_sized(s,
                                    (const void*)(s->str + 3),
                                    s->alloc)); /* definitely exceeds */

    return_string(s);
}

static void test_string_is_string_ptr_sized_rejects_pointer_outside_buffer(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();
    string_expect_t r = init_string("abc", 0u, a);
    assert_true(r.has_value);

    string_t *s = r.u.value;

    char other = 0;
    assert_false(is_string_ptr_sized(s, (const void*)&other, 1u));

    return_string(s);
}
// -------------------------------------------------------------------------------- 

static void test_string_find_substr_forward_finds_first_occurrence(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t rh = init_string("bananana", 0u, a);
    assert_true(rh.has_value);
    string_t *h = rh.u.value;

    string_expect_t rn = init_string("ana", 0u, a);
    assert_true(rn.has_value);
    string_t *n = rn.u.value;

    /* "bananana": "ana" occurs at 0-based indices 1, 3, 5.
       FORWARD should return first => 1 (absolute position). */
    size_t pos = find_substr(h, n,
                             (const uint8_t*)h->str,
                             (const uint8_t*)h->str + h->len,
                             FORWARD);
    assert_int_equal(pos, 1u);

    return_string(n);
    return_string(h);
}
// --------------------------------------------------------------------------------

static void test_string_find_substr_reverse_finds_last_occurrence(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t rh = init_string("bananana", 0u, a);
    assert_true(rh.has_value);
    string_t *h = rh.u.value;

    string_expect_t rn = init_string("ana", 0u, a);
    assert_true(rn.has_value);
    string_t *n = rn.u.value;

    /* Last occurrence at 0-based index 5 (absolute position). */
    size_t pos = find_substr(h, n,
                             (const uint8_t*)h->str,
                             (const uint8_t*)h->str + h->len,
                             REVERSE);
    assert_int_equal(pos, 5u);

    return_string(n);
    return_string(h);
}
// --------------------------------------------------------------------------------

static void test_string_find_substr_bounded_window_changes_result(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t rh = init_string("hello world hello", 0u, a);
    assert_true(rh.has_value);
    string_t *h = rh.u.value;

    string_expect_t rn = init_string("hello", 0u, a);
    assert_true(rn.has_value);
    string_t *n = rn.u.value;

    const uint8_t *base  = (const uint8_t*)h->str;
    const uint8_t *begin = base + 12u;           /* second "hello" */
    const uint8_t *end   = base + h->len;        /* end exclusive */

    /* Within this window, match is at absolute position 12 */
    size_t pos_f = find_substr(h, n, begin, end, FORWARD);
    assert_int_equal(pos_f, 12u);  /* CHANGED: now expects absolute position */

    /* Reverse within same window also finds at absolute position 12 */
    size_t pos_r = find_substr(h, n, begin, end, REVERSE);
    assert_int_equal(pos_r, 12u);  /* CHANGED: now expects absolute position */

    return_string(n);
    return_string(h);
}
// --------------------------------------------------------------------------------

static void test_string_find_substr_default_begin_end_when_null(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t rh = init_string("hello world hello", 0u, a);
    assert_true(rh.has_value);
    string_t *h = rh.u.value;

    string_expect_t rn = init_string("hello", 0u, a);
    assert_true(rn.has_value);
    string_t *n = rn.u.value;

    /* begin/end NULL => defaults to [str, str+len) */
    size_t pos_f = find_substr(h, n, NULL, NULL, FORWARD);
    assert_int_equal(pos_f, 0u);  /* first "hello" at absolute position 0 */

    size_t pos_r = find_substr(h, n, NULL, NULL, REVERSE);
    assert_int_equal(pos_r, 12u); /* second "hello" at absolute position 12 */

    return_string(n);
    return_string(h);
}
// --------------------------------------------------------------------------------

static void test_string_find_substr_not_found_returns_size_max(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t rh = init_string("abcdef", 0u, a);
    assert_true(rh.has_value);
    string_t *h = rh.u.value;

    string_expect_t rn = init_string("xyz", 0u, a);
    assert_true(rn.has_value);
    string_t *n = rn.u.value;

    size_t pos = find_substr(h, n, NULL, NULL, FORWARD);
    assert_int_equal(pos, SIZE_MAX);

    return_string(n);
    return_string(h);
}
// --------------------------------------------------------------------------------

static void test_string_find_substr_empty_needle_returns_zero(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t rh = init_string("abcdef", 0u, a);
    assert_true(rh.has_value);
    string_t *h = rh.u.value;

    /* Empty needle */
    string_expect_t rn = init_string("", 0u, a);
    assert_true(rn.has_value);
    string_t *n = rn.u.value;

    /* Convention: empty needle matches at the start (position 0) */
    size_t pos_f = find_substr(h, n, NULL, NULL, FORWARD);
    assert_int_equal(pos_f, 0u);

    size_t pos_r = find_substr(h, n, NULL, NULL, REVERSE);
    assert_int_equal(pos_r, 0u);

    return_string(n);
    return_string(h);
}
// --------------------------------------------------------------------------------

static void test_string_find_substr_invalid_region_returns_size_max(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t rh = init_string("abcdef", 0u, a);
    assert_true(rh.has_value);
    string_t *h = rh.u.value;

    string_expect_t rn = init_string("bc", 0u, a);
    assert_true(rn.has_value);
    string_t *n = rn.u.value;

    /* begin/end not within h->str allocation */
    uint8_t other_buf[8];
    size_t pos = find_substr(h, n, other_buf, other_buf + 4u, FORWARD);
    assert_int_equal(pos, SIZE_MAX);

    return_string(n);
    return_string(h);
}
// --------------------------------------------------------------------------------

static void test_string_find_substr_from_middle_forward(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t rh = init_string("hello world hello", 0u, a);
    assert_true(rh.has_value);
    string_t *h = rh.u.value;

    string_expect_t rn = init_string("hello", 0u, a);
    assert_true(rn.has_value);
    string_t *n = rn.u.value;

    const uint8_t *base  = (const uint8_t*)h->str;
    const uint8_t *begin = base + 1u;   /* Skip first "hello" */
    const uint8_t *end   = base + h->len;

    /* Should find second "hello" at absolute position 12 */
    size_t pos = find_substr(h, n, begin, end, FORWARD);
    assert_int_equal(pos, 12u);

    return_string(n);
    return_string(h);
}
// --------------------------------------------------------------------------------

static void test_string_find_substr_exact_window_match(void **state)
{
    (void)state;

    allocator_vtable_t a = heap_allocator();

    string_expect_t rh = init_string("hello world", 0u, a);
    assert_true(rh.has_value);
    string_t *h = rh.u.value;

    string_expect_t rn = init_string("world", 0u, a);
    assert_true(rn.has_value);
    string_t *n = rn.u.value;

    const uint8_t *base  = (const uint8_t*)h->str;
    const uint8_t *begin = base + 6u;   /* Start of "world" */
    const uint8_t *end   = base + 11u;  /* End of "world" */

    /* Window exactly contains "world" at absolute position 6 */
    size_t pos = find_substr(h, n, begin, end, FORWARD);
    assert_int_equal(pos, 6u);

    return_string(n);
    return_string(h);
}
// -------------------------------------------------------------------------------- 

static inline string_t sview_alloc(char *buf, size_t used_len, size_t alloc_len) {
    string_t s;
    s.str   = buf;
    s.len   = used_len;
    s.alloc = alloc_len;
    return s;
}

static void test_find_substr_lit_null_haystack_returns_sizemax(void **state) {
    (void)state;
    assert_int_equal(find_substr_lit(NULL, "hi", NULL, NULL, FORWARD), SIZE_MAX);
}

static void test_find_substr_lit_null_haystack_str_returns_sizemax(void **state) {
    (void)state;
    string_t s = { .str = NULL, .len = 0, .alloc = 0 };
    assert_int_equal(find_substr_lit(&s, "hi", NULL, NULL, FORWARD), SIZE_MAX);
}

static void test_find_substr_lit_null_needle_returns_sizemax(void **state) {
    (void)state;

    char buf[] = "hello";
    string_t s = sview_alloc(buf, strlen(buf), sizeof(buf));

    assert_int_equal(find_substr_lit(&s, NULL, NULL, NULL, FORWARD), SIZE_MAX);
}

static void test_find_substr_lit_empty_needle_returns_begin_offset(void **state) {
    (void)state;

    char buf[] = "abcdef";
    string_t s = sview_alloc(buf, strlen(buf), sizeof(buf));

    const uint8_t *base = (const uint8_t*)(const void*)s.str;

    /* Empty needle => found at start of region */
    assert_int_equal(find_substr_lit(&s, "", NULL, NULL, FORWARD), 0u);

    /* If begin is moved, should return offset == (begin - base) */
    const uint8_t *begin = base + 3;
    assert_int_equal(find_substr_lit(&s, "", begin, NULL, FORWARD), 3u);
}

static void test_find_substr_lit_finds_first_occurrence_default_range(void **state) {
    (void)state;

    char buf[] = "Hello my name is jonhello and hello again";
    string_t s = sview_alloc(buf, strlen(buf), sizeof(buf));

    /* Case-sensitive search: "hello" occurs first inside "jonhello".
       "Hello" at the front is different case, so first "hello" begins at:
       "Hello " (6 chars) + "my " (3) + "name " (5) + "is " (3) + "jon" (3) = 20
       index 20 is the 'h' in "jonhello".
    */
    size_t pos = find_substr_lit(&s, "hello", NULL, NULL, FORWARD);
    assert_int_equal(pos, 20u);
}

static void test_find_substr_lit_respects_begin_end_window(void **state) {
    (void)state;

    char buf[] = "hello hello hello";
    string_t s = sview_alloc(buf, strlen(buf), sizeof(buf));

    const uint8_t *base = (const uint8_t*)(const void*)s.str;

    /* Layout: hello(0-4) space(5) hello(6-10) space(11) hello(12-16) */

    /* Search only middle hello: [6, 11) */
    const uint8_t *begin = base + 6;
    const uint8_t *end   = base + 11;

    size_t pos = find_substr_lit(&s, "hello", begin, end, FORWARD);
    assert_int_equal(pos, 6u);

    /* Search region that excludes any full match */
    pos = find_substr_lit(&s, "hello", base + 1, base + 4, FORWARD);
    assert_int_equal(pos, SIZE_MAX);
}

static void test_find_substr_lit_clamps_end_to_used_len(void **state) {
    (void)state;

    /* Buffer has extra capacity beyond used length */
    char buf[64];
    memset(buf, 0, sizeof(buf));
    memcpy(buf, "hello world", 11);

    /* used_len = 11, alloc = 64 */
    string_t s = sview_alloc(buf, 11u, sizeof(buf));

    const uint8_t *base = (const uint8_t*)(const void*)s.str;

    /* Provide an end pointer inside allocation but beyond used length.
       Your function should clamp end to hs_used_end and still succeed. */
    const uint8_t *end = base + 40; /* within alloc */

    size_t pos = find_substr_lit(&s, "world", NULL, end, FORWARD);
    assert_int_equal(pos, 6u);
}

static void test_find_substr_lit_begin_end_outside_alloc_rejected(void **state) {
    (void)state;

    char buf[] = "hello";
    string_t s = sview_alloc(buf, strlen(buf), sizeof(buf));

    const uint8_t *base = (const uint8_t*)(const void*)s.str;

    /* end outside allocation => _range_within_alloc_ should reject */
    const uint8_t *bad_end = base + s.alloc + 1;

    assert_int_equal(find_substr_lit(&s, "he", base, bad_end, FORWARD), SIZE_MAX);
}

// --------------------------------------------------------------------------------

static inline string_t sview(const char *cstr) {
    string_t s;
    memset(&s, 0, sizeof s);          // safest: clears all fields
    s.str = (char*)cstr;
    s.len = strlen(cstr);
    s.alloc = s.len + 1;              // include NUL storage (recommended)
    return s;
}

static inline string_t nview(const char *cstr) {
    string_t s;
    memset(&s, 0, sizeof s);
    s.str = (char*)cstr;
    s.len = strlen(cstr);
    s.alloc = s.len + 1;
    return s;
}
// /* Helper to create a string_t view over a C string literal/buffer */
// static inline string_t sview(const char *cstr) {
//     string_t s;
//     s.str = (char*)cstr;            /* adjust type if your string_t uses uint8_t* */
//     s.len = (size_t)strlen(cstr);
//     return s;
// }
//
// /* Helper to make a needle view */
// static inline string_t nview(const char *cstr) {
//     string_t s;
//     s.str = (char*)cstr;
//     s.len = (size_t)strlen(cstr);
//     return s;
// }

static void test_word_count_null_s_returns_0(void **state) {
    (void)state;

    string_t w = nview("hello");
    assert_int_equal(word_count(NULL, &w, NULL, NULL), 0);
}

static void test_word_count_null_word_returns_0(void **state) {
    (void)state;

    string_t s = sview("hello hello");
    assert_int_equal(word_count(&s, NULL, NULL, NULL), 0);
}

static void test_word_count_null_str_members_returns_0(void **state) {
    (void)state;

    string_t s = { .str = NULL, .len = 5 };
    string_t w = nview("hello");
    assert_int_equal(word_count(&s, &w, NULL, NULL), 0);

    string_t s2 = sview("hello");
    string_t w2 = { .str = NULL, .len = 5 };
    assert_int_equal(word_count(&s2, &w2, NULL, NULL), 0);
}

static void test_word_count_empty_word_returns_0(void **state) {
    (void)state;

    string_t s = sview("anything");
    string_t w = nview("");
    assert_int_equal(word_count(&s, &w, NULL, NULL), 0);
}

static void test_word_count_case_sensitive_example(void **state) {
    (void)state;

    /* Case-sensitive: "Hello" != "hello" */
    string_t s = sview("Hello my name is jonhello and hello again");
    string_t w = nview("hello");

    /* Matches: "jonhello" + "hello" => 2 */
    size_t count = word_count(&s, &w, NULL, NULL);
    assert_int_equal(count, 2);
}

static void test_word_count_simple_multiple_occurrences(void **state) {
    (void)state;

    string_t s = sview("hello hello hello");
    string_t w = nview("hello");
    size_t count = word_count(&s, &w, NULL, NULL);
    assert_int_equal(count, 3);
}

static void test_word_count_non_overlapping_behavior(void **state) {
    (void)state;

    /* Non-overlapping semantics: "aaaa" contains "aa" at [0..2) and [2..4) => 2 */
    string_t s = sview("aaaa");
    string_t w = nview("aa");
    size_t count = word_count(&s, &w, NULL, NULL);
    assert_int_equal(count, 2);
}

static void test_word_count_window_start_end_limits(void **state) {
    (void)state;

    const char *txt = "hello hello hello";
    string_t s = sview(txt);
    string_t w = nview("hello");

    const uint8_t *base = (const uint8_t*)(const void*)s.str;

    /* Count only the middle "hello" by restricting range to " hello " region */
    /* Layout: "hello(0-4) space(5) hello(6-10) space(11) hello(12-16)" */
    const uint8_t *start = base + 6;     /* start at middle hello */
    const uint8_t *end   = base + 11;    /* end right after middle hello */

    assert_int_equal(word_count(&s, &w, start, end), 1);

    /* Range excluding all occurrences */
    assert_int_equal(word_count(&s, &w, base + 1, base + 4), 0);
}

static void test_word_count_window_end_null_means_to_end(void **state) {
    (void)state;

    const char *txt = "xxhello hello";
    string_t s = sview(txt);
    string_t w = nview("hello");

    const uint8_t *base = (const uint8_t*)(const void*)s.str;

    /* Start at the first 'h', no explicit end => should count both */
    assert_int_equal(word_count(&s, &w, base + 2, NULL), 2);
}

static void test_word_count_start_after_end_returns_0(void **state) {
    (void)state;

    string_t s = sview("hello hello");
    string_t w = nview("hello");

    const uint8_t *base = (const uint8_t*)(const void*)s.str;

    /* start > end should produce 0 (find_substr will reject range) */
    assert_int_equal(word_count(&s, &w, base + 8, base + 3), 0);
}
// -------------------------------------------------------------------------------- 

static void test_word_count_lit_null_s_returns_0(void **state) {
    (void)state;
    assert_int_equal(word_count_lit(NULL, "hello", NULL, NULL), 0);
}

static void test_word_count_lit_null_s_str_returns_0(void **state) {
    (void)state;
    string_t s = { .str = NULL, .len = 10 };
    assert_int_equal(word_count_lit(&s, "hello", NULL, NULL), 0);
}

static void test_word_count_lit_null_word_returns_0(void **state) {
    (void)state;
    string_t s = sview("hello hello");
    assert_int_equal(word_count_lit(&s, NULL, NULL, NULL), 0);
}

static void test_word_count_lit_empty_word_returns_0(void **state) {
    (void)state;
    string_t s = sview("hello hello");
    assert_int_equal(word_count_lit(&s, "", NULL, NULL), 0);
}

static void test_word_count_lit_no_matches_returns_0(void **state) {
    (void)state;
    string_t s = sview("abcdefg");
    assert_int_equal(word_count_lit(&s, "hello", NULL, NULL), 0);
}

static void test_word_count_lit_case_sensitive_example(void **state) {
    (void)state;

    /* Case-sensitive: "Hello" != "hello" */
    string_t s = sview("Hello my name is jonhello and hello again");

    /* "hello" matches: "jonhello" + "hello" => 2 */
    size_t count = word_count_lit(&s, "hello", NULL, NULL);
    assert_int_equal(count, 2);
    count = word_count_lit(&s, "Hello", NULL, NULL);
    /* "Hello" matches only the first "Hello" => 1 */
    assert_int_equal(count, 1);
}

static void test_word_count_lit_simple_multiple_occurrences(void **state) {
    (void)state;

    string_t s = sview("hello hello hello");
    size_t count = word_count_lit(&s, "hello", NULL, NULL);
    assert_int_equal(count, 3);
}

static void test_word_count_lit_non_overlapping_behavior(void **state) {
    (void)state;

    /* Non-overlapping: "aaaa" with "aa" => 2 (at 0 and 2) */
    string_t s = sview("aaaa");
    size_t count = word_count_lit(&s, "aa", NULL, NULL);
    assert_int_equal(count, 2);
}

static void test_word_count_lit_overlapping_not_counted(void **state) {
    (void)state;

    /* If overlapping were allowed, "aaaa"/"aa" could be 3. We expect 2. */
    string_t s = sview("aaaa");
    size_t count = word_count_lit(&s, "aa", NULL, NULL);
    assert_int_equal(count, 2);
}

static void test_word_count_lit_window_limits_middle_only(void **state) {
    (void)state;

    const char *txt = "hello hello hello";
    string_t s = sview(txt);

    const uint8_t *base = (const uint8_t*)(const void*)s.str;

    /* Layout:
       hello(0-4) space(5) hello(6-10) space(11) hello(12-16)
       Choose [6,11) => exactly middle "hello"
    */
    const uint8_t *start = base + 6;
    const uint8_t *end   = base + 11;

    assert_int_equal(word_count_lit(&s, "hello", start, end), 1);
}

static void test_word_count_lit_window_excludes_all(void **state) {
    (void)state;

    const char *txt = "hello hello";
    string_t s = sview(txt);
    const uint8_t *base = (const uint8_t*)(const void*)s.str;

    /* [1,4) is "ell" - no full match */
    assert_int_equal(word_count_lit(&s, "hello", base + 1, base + 4), 0);
}

static void test_word_count_lit_start_and_end_null_defaults(void **state) {
    (void)state;

    string_t s = sview("xxhello hello");
    size_t count = word_count_lit(&s, "hello", NULL, NULL);
    /* start NULL + end NULL => entire string */
    assert_int_equal(count, 2);
}

static void test_word_count_lit_start_only_defaults_end(void **state) {
    (void)state;

    const char *txt = "xxhello hello";
    string_t s = sview(txt);
    const uint8_t *base = (const uint8_t*)(const void*)s.str;

    /* Start at the first 'h', implicit end => should count both */
    size_t count = word_count_lit(&s, "hello", base + 2, NULL);
    assert_int_equal(count, 2);
}

static void test_word_count_lit_start_after_end_returns_0(void **state) {
    (void)state;

    string_t s = sview("hello hello");
    const uint8_t *base = (const uint8_t*)(const void*)s.str;

    /* find_substr should reject begin > end => SIZE_MAX => word_count_lit returns 0 */
    assert_int_equal(word_count_lit(&s, "hello", base + 8, base + 3), 0);
}
// -------------------------------------------------------------------------------- 

#if ARENA_USE_CONVENIENCE_MACROS && !defined(NO_FUNCTION_MACROS)
static void test_count_words_macro_char(void **state) {
    (void)state;
    /* Case-sensitive: "Hello" != "hello" */
    string_t s = sview("Hello my name is jonhello and hello again");

    /* "hello" matches: "jonhello" + "hello" => 2 */
    size_t count = count_words(&s, "hello", NULL, NULL);
    assert_int_equal(count, 2);
    count = count_words(&s, "Hello", NULL, NULL);
    /* "Hello" matches only the first "Hello" => 1 */
    assert_int_equal(count, 1);
}
// -------------------------------------------------------------------------------- 

static void test_count_words_macro_string(void **state) {
    (void)state;

    const char *txt = "hello hello hello";
    string_t s = sview(txt);
    string_t w = nview("hello");

    const uint8_t *base = (const uint8_t*)(const void*)s.str;

    /* Count only the middle "hello" by restricting range to " hello " region */
    /* Layout: "hello(0-4) space(5) hello(6-10) space(11) hello(12-16)" */
    const uint8_t *start = base + 6;     /* start at middle hello */
    const uint8_t *end   = base + 11;    /* end right after middle hello */

    assert_int_equal(count_words(&s, &w, start, end), 1);

    /* Range excluding all occurrences */
    assert_int_equal(count_words(&s, &w, base + 1, base + 4), 0);
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

    cmocka_unit_test(test_string_str_compare_equal),
    cmocka_unit_test(test_string_str_compare_diff_middle_less),
    cmocka_unit_test(test_string_str_compare_diff_middle_greater),
    cmocka_unit_test(test_string_str_compare_bounded_by_string_len_str_longer),
    cmocka_unit_test(test_string_str_compare_bounded_by_string_len_str_shorter),
    cmocka_unit_test(test_string_str_compare_truncated_string_instance),
//    cmocka_unit_test(test_string_str_compare_invalid_args),

    cmocka_unit_test(test_string_string_compare_equal),
    cmocka_unit_test(test_string_string_compare_less_and_greater),
    cmocka_unit_test(test_string_string_compare_length_ordering),
    cmocka_unit_test(test_string_string_compare_truncated_instances),
//    cmocka_unit_test(test_string_string_compare_invalid_args),

    cmocka_unit_test(test_string_macro_compare_dispatches_cstr),
    cmocka_unit_test(test_string_macro_compare_dispatches_char_ptr),
    cmocka_unit_test(test_string_macro_compare_dispatches_string_t),
    cmocka_unit_test(test_string_macro_compare_dispatches_const_string_t),

    cmocka_unit_test(test_string_reset_sets_len_zero_and_nul_terminator),
    cmocka_unit_test(test_string_reset_allows_concat_from_start),
    cmocka_unit_test(test_string_reset_idempotent),
    cmocka_unit_test(test_string_reset_null_is_safe_noop),
    cmocka_unit_test(test_string_reset_struct_with_null_buffer_is_safe_noop),

    cmocka_unit_test(test_string_copy_string_copies_value_and_min_capacity),
    cmocka_unit_test(test_string_copy_string_allows_independent_mutation),
    cmocka_unit_test(test_string_copy_string_empty_string),
    cmocka_unit_test(test_string_copy_string_null_input_returns_error),
    cmocka_unit_test(test_string_copy_string_null_buffer_returns_error),

    cmocka_unit_test(test_string_is_string_ptr_null_inputs_return_false),
    cmocka_unit_test(test_string_is_string_ptr_null_buffer_returns_false),
    cmocka_unit_test(test_string_is_string_ptr_begin_and_end_boundaries),
    cmocka_unit_test(test_string_is_string_ptr_pointer_before_begin_is_false),
    cmocka_unit_test(test_string_is_string_ptr_inside_slack_region_is_true),

    cmocka_unit_test(test_string_is_string_ptr_sized_null_inputs_return_false),
    cmocka_unit_test(test_string_is_string_ptr_sized_null_buffer_returns_false),
    cmocka_unit_test(test_string_is_string_ptr_sized_range_fits_within_used_region),
    cmocka_unit_test(test_string_is_string_ptr_sized_range_fits_within_slack_region),
    cmocka_unit_test(test_string_is_string_ptr_sized_allows_end_at_one_past_end),
    cmocka_unit_test(test_string_is_string_ptr_sized_rejects_range_past_end),
    cmocka_unit_test(test_string_is_string_ptr_sized_rejects_pointer_outside_buffer),

    cmocka_unit_test(test_string_find_substr_forward_finds_first_occurrence),
    cmocka_unit_test(test_string_find_substr_reverse_finds_last_occurrence),
    cmocka_unit_test(test_string_find_substr_bounded_window_changes_result),
    cmocka_unit_test(test_string_find_substr_default_begin_end_when_null),
    cmocka_unit_test(test_string_find_substr_not_found_returns_size_max),
    cmocka_unit_test(test_string_find_substr_empty_needle_returns_zero),
    cmocka_unit_test(test_string_find_substr_invalid_region_returns_size_max),
    cmocka_unit_test(test_string_find_substr_from_middle_forward),
    cmocka_unit_test(test_string_find_substr_exact_window_match),

    cmocka_unit_test(test_find_substr_lit_null_haystack_returns_sizemax),
    cmocka_unit_test(test_find_substr_lit_null_haystack_str_returns_sizemax),
    cmocka_unit_test(test_find_substr_lit_null_needle_returns_sizemax),
    cmocka_unit_test(test_find_substr_lit_empty_needle_returns_begin_offset),
    cmocka_unit_test(test_find_substr_lit_finds_first_occurrence_default_range),
    cmocka_unit_test(test_find_substr_lit_respects_begin_end_window),
    cmocka_unit_test(test_find_substr_lit_clamps_end_to_used_len),
    cmocka_unit_test(test_find_substr_lit_begin_end_outside_alloc_rejected),

    cmocka_unit_test(test_word_count_null_s_returns_0),
    cmocka_unit_test(test_word_count_null_word_returns_0),
    cmocka_unit_test(test_word_count_null_str_members_returns_0),
    cmocka_unit_test(test_word_count_empty_word_returns_0),
    cmocka_unit_test(test_word_count_case_sensitive_example),
    cmocka_unit_test(test_word_count_simple_multiple_occurrences),
    cmocka_unit_test(test_word_count_non_overlapping_behavior),
    cmocka_unit_test(test_word_count_window_start_end_limits),
    cmocka_unit_test(test_word_count_window_end_null_means_to_end),
    cmocka_unit_test(test_word_count_start_after_end_returns_0),

    cmocka_unit_test(test_word_count_lit_null_s_returns_0),
    cmocka_unit_test(test_word_count_lit_null_s_str_returns_0),
    cmocka_unit_test(test_word_count_lit_null_word_returns_0),
    cmocka_unit_test(test_word_count_lit_empty_word_returns_0),
    cmocka_unit_test(test_word_count_lit_no_matches_returns_0),
    cmocka_unit_test(test_word_count_lit_case_sensitive_example),
    cmocka_unit_test(test_word_count_lit_simple_multiple_occurrences),
    cmocka_unit_test(test_word_count_lit_non_overlapping_behavior),
    cmocka_unit_test(test_word_count_lit_overlapping_not_counted),
    cmocka_unit_test(test_word_count_lit_window_limits_middle_only),
    cmocka_unit_test(test_word_count_lit_window_excludes_all),
    cmocka_unit_test(test_word_count_lit_start_and_end_null_defaults),
    cmocka_unit_test(test_word_count_lit_start_only_defaults_end),
    cmocka_unit_test(test_word_count_lit_start_after_end_returns_0),
#if ARENA_USE_CONVENIENCE_MACROS && !defined(NO_FUNCTION_MACROS)
    cmocka_unit_test(test_count_words_macro_char),
    cmocka_unit_test(test_count_words_macro_string),
#endif
};

const size_t test_string_count = sizeof(test_string) / sizeof(test_string[0]);
// ================================================================================
// ================================================================================
// eof
