// ================================================================================
// ================================================================================
// - File:    test_dict.c
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
#include "c_allocator.h"
#include "c_dtypes.h"
#include "c_error.h"
#include "c_dict.h"
#include "c_uint8.h"


#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Helpers
// ================================================================================
 
/* Build a heap uint8_dict_t with growth enabled. */
static uint8_dict_t* _make_dict(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    uint8_dict_expect_t r = init_uint8_dict(cap, true, a);
    assert_true(r.has_value);
    return r.u.value;
}
 
/* Iterator context used by the foreach tests. */
typedef struct { int count; unsigned sum; } _iter_ctx_t;
 
static void _sum_iter(const char* key, size_t key_len,
                      uint8_t value, void* ud) {
    (void)key; (void)key_len;
    _iter_ctx_t* c = (_iter_ctx_t*)ud;
    c->count++;
    c->sum += value;
}
 
// ================================================================================
// Group 1: init_uint8_dict
// ================================================================================
 
static void test_u8d_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    uint8_dict_expect_t r = init_uint8_dict(8, true, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_u8d_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_expect_t r = init_uint8_dict(0, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_u8d_init_returns_valid_dict(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_expect_t r = init_uint8_dict(8, true, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_true(is_uint8_dict_empty(r.u.value));
    assert_int_equal((int)uint8_dict_hash_size(r.u.value), 0);
    return_uint8_dict(r.u.value);
}
 
static void test_u8d_init_data_size_is_one_byte(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    /* data_size must be sizeof(uint8_t) == 1 */
    assert_int_equal((int)dict_data_size(d), 1);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 2: return_uint8_dict
// ================================================================================
 
static void test_u8d_return_null_is_safe(void** state) {
    (void)state;
    /* Must not crash */
    return_uint8_dict(NULL);
}
 
static void test_u8d_return_valid_dict_does_not_crash(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "x", 1u, a);
    return_uint8_dict(d);   /* must not crash or leak */
}
 
// ================================================================================
// Group 3: insert_uint8_dict
// ================================================================================
 
static void test_u8d_insert_null_dict_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    assert_int_equal(insert_uint8_dict(NULL, "key", 1u, a), NULL_POINTER);
}
 
static void test_u8d_insert_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    assert_int_equal(insert_uint8_dict(d, NULL, 1u, a), NULL_POINTER);
    return_uint8_dict(d);
}
 
static void test_u8d_insert_duplicate_key_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    assert_int_equal(insert_uint8_dict(d, "k", 1u, a), NO_ERROR);
    assert_int_equal(insert_uint8_dict(d, "k", 2u, a), INVALID_ARG);
    /* hash_size must remain 1 */
    assert_int_equal((int)uint8_dict_hash_size(d), 1);
    return_uint8_dict(d);
}
 
static void test_u8d_insert_stores_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    assert_int_equal(insert_uint8_dict(d, "brightness", 200u, a), NO_ERROR);
    uint8_t v = 0;
    assert_int_equal(get_uint8_dict_value(d, "brightness", &v), NO_ERROR);
    assert_int_equal((int)v, 200);
    return_uint8_dict(d);
}
 
static void test_u8d_insert_increments_hash_size(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "a", 1u, a);
    insert_uint8_dict(d, "b", 2u, a);
    insert_uint8_dict(d, "c", 3u, a);
    assert_int_equal((int)uint8_dict_hash_size(d), 3);
    return_uint8_dict(d);
}
 
static void test_u8d_insert_key_copy_is_independent(void** state) {
    (void)state;
    /* Mutating the key buffer after insert must not affect lookup. */
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    char key[8];
    strcpy(key, "hello");
    insert_uint8_dict(d, key, 77u, a);
    strcpy(key, "XXXXX");   /* clobber source */
    uint8_t v = 0;
    /* The original key "hello" should still be found. */
    assert_int_equal(get_uint8_dict_value(d, "hello", &v), NO_ERROR);
    assert_int_equal((int)v, 77);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 4: insert_uint8_dict_n
// ================================================================================
 
static void test_u8d_insert_n_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    assert_int_equal(insert_uint8_dict_n(d, NULL, 3, 1u, a), NULL_POINTER);
    return_uint8_dict(d);
}
 
static void test_u8d_insert_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    assert_int_equal(insert_uint8_dict_n(d, "key", 0, 1u, a), INVALID_ARG);
    return_uint8_dict(d);
}
 
static void test_u8d_insert_n_uses_only_specified_bytes(void** state) {
    (void)state;
    /* Buffer is "hello world" but we insert only "hello" (5 bytes). */
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    const char* buf = "hello world";
    assert_int_equal(insert_uint8_dict_n(d, buf, 5, 42u, a), NO_ERROR);
    /* "hello" found */
    uint8_t v = 0;
    assert_int_equal(get_uint8_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal((int)v, 42);
    /* "hello world" (11 bytes) is a different key — not found */
    assert_int_equal(get_uint8_dict_value_n(d, buf, 11, &v), NOT_FOUND);
    /* Plain "hello\0..." lookup also works */
    assert_int_equal(get_uint8_dict_value(d, "hello", &v), NO_ERROR);
    assert_int_equal((int)v, 42);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 5: pop_uint8_dict
// ================================================================================
 
static void test_u8d_pop_null_dict_returns_null_pointer(void** state) {
    (void)state;
    uint8_t v;
    assert_int_equal(pop_uint8_dict(NULL, "k", &v), NULL_POINTER);
}
 
static void test_u8d_pop_null_key_returns_null_pointer(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    uint8_t v;
    assert_int_equal(pop_uint8_dict(d, NULL, &v), NULL_POINTER);
    return_uint8_dict(d);
}
 
static void test_u8d_pop_missing_key_returns_not_found(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    assert_int_equal(pop_uint8_dict(d, "missing", NULL), NOT_FOUND);
    return_uint8_dict(d);
}
 
static void test_u8d_pop_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "score", 99u, a);
    uint8_t v = 0;
    assert_int_equal(pop_uint8_dict(d, "score", &v), NO_ERROR);
    assert_int_equal((int)v, 99);
    assert_int_equal((int)uint8_dict_hash_size(d), 0);
    return_uint8_dict(d);
}
 
static void test_u8d_pop_null_out_value_discards_safely(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "x", 7u, a);
    /* NULL out_value must not crash */
    assert_int_equal(pop_uint8_dict(d, "x", NULL), NO_ERROR);
    assert_int_equal((int)uint8_dict_hash_size(d), 0);
    return_uint8_dict(d);
}
 
static void test_u8d_pop_key_no_longer_found_after_pop(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "once", 1u, a);
    pop_uint8_dict(d, "once", NULL);
    assert_int_equal(pop_uint8_dict(d, "once", NULL), NOT_FOUND);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 6: pop_uint8_dict_n
// ================================================================================
 
static void test_u8d_pop_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    assert_int_equal(pop_uint8_dict_n(d, "k", 0, NULL), INVALID_ARG);
    return_uint8_dict(d);
}
 
static void test_u8d_pop_n_removes_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    const char* buf = "score_total";
    insert_uint8_dict_n(d, buf, 5, 88u, a);   /* key = "score" */
    uint8_t v = 0;
    assert_int_equal(pop_uint8_dict_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal((int)v, 88);
    assert_int_equal((int)uint8_dict_hash_size(d), 0);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 7: update_uint8_dict
// ================================================================================
 
static void test_u8d_update_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(update_uint8_dict(NULL, "k", 1u), NULL_POINTER);
}
 
static void test_u8d_update_null_key_returns_null_pointer(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    assert_int_equal(update_uint8_dict(d, NULL, 1u), NULL_POINTER);
    return_uint8_dict(d);
}
 
static void test_u8d_update_missing_key_returns_not_found(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    assert_int_equal(update_uint8_dict(d, "missing", 1u), NOT_FOUND);
    return_uint8_dict(d);
}
 
static void test_u8d_update_overwrites_existing_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "lives", 3u, a);
    assert_int_equal(update_uint8_dict(d, "lives", 2u), NO_ERROR);
    uint8_t v = 0;
    assert_int_equal(get_uint8_dict_value(d, "lives", &v), NO_ERROR);
    assert_int_equal((int)v, 2);
    /* hash_size must not change */
    assert_int_equal((int)uint8_dict_hash_size(d), 1);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 8: update_uint8_dict_n
// ================================================================================
 
static void test_u8d_update_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    assert_int_equal(update_uint8_dict_n(d, "k", 0, 1u), INVALID_ARG);
    return_uint8_dict(d);
}
 
static void test_u8d_update_n_updates_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    const char* buf = "lives_total";
    insert_uint8_dict_n(d, buf, 5, 3u, a);   /* key = "lives" */
    assert_int_equal(update_uint8_dict_n(d, buf, 5, 2u), NO_ERROR);
    uint8_t v = 0;
    assert_int_equal(get_uint8_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal((int)v, 2);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 9: get_uint8_dict_value
// ================================================================================
 
static void test_u8d_get_null_dict_returns_null_pointer(void** state) {
    (void)state;
    uint8_t v;
    assert_int_equal(get_uint8_dict_value(NULL, "k", &v), NULL_POINTER);
}
 
static void test_u8d_get_null_key_returns_null_pointer(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    uint8_t v;
    assert_int_equal(get_uint8_dict_value(d, NULL, &v), NULL_POINTER);
    return_uint8_dict(d);
}
 
static void test_u8d_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "k", 1u, a);
    assert_int_equal(get_uint8_dict_value(d, "k", NULL), NULL_POINTER);
    return_uint8_dict(d);
}
 
static void test_u8d_get_missing_key_returns_not_found(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    uint8_t v;
    assert_int_equal(get_uint8_dict_value(d, "missing", &v), NOT_FOUND);
    return_uint8_dict(d);
}
 
static void test_u8d_get_retrieves_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "r", 255u, a);
    insert_uint8_dict(d, "g", 128u, a);
    insert_uint8_dict(d, "b",   0u, a);
    uint8_t v = 0;
    assert_int_equal(get_uint8_dict_value(d, "r", &v), NO_ERROR);
    assert_int_equal((int)v, 255);
    assert_int_equal(get_uint8_dict_value(d, "g", &v), NO_ERROR);
    assert_int_equal((int)v, 128);
    assert_int_equal(get_uint8_dict_value(d, "b", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 10: get_uint8_dict_value_n
// ================================================================================
 
static void test_u8d_get_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    uint8_t v;
    assert_int_equal(get_uint8_dict_value_n(d, "k", 0, &v), INVALID_ARG);
    return_uint8_dict(d);
}
 
static void test_u8d_get_n_retrieves_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    const char* buf = "brightness_max";
    insert_uint8_dict_n(d, buf, 10, 200u, a);   /* key = "brightness" */
    uint8_t v = 0;
    assert_int_equal(get_uint8_dict_value_n(d, buf, 10, &v), NO_ERROR);
    assert_int_equal((int)v, 200);
    /* "brightness_max" (14 bytes) is a different key */
    assert_int_equal(get_uint8_dict_value_n(d, buf, 14, &v), NOT_FOUND);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 11: get_uint8_dict_ptr
// ================================================================================
 
static void test_u8d_ptr_null_dict_returns_null(void** state) {
    (void)state;
    assert_null(get_uint8_dict_ptr(NULL, "k"));
}
 
static void test_u8d_ptr_null_key_returns_null(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    assert_null(get_uint8_dict_ptr(d, NULL));
    return_uint8_dict(d);
}
 
static void test_u8d_ptr_missing_key_returns_null(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    assert_null(get_uint8_dict_ptr(d, "missing"));
    return_uint8_dict(d);
}
 
static void test_u8d_ptr_points_to_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "flags", 0x5Au, a);
    const uint8_t* p = get_uint8_dict_ptr(d, "flags");
    assert_non_null(p);
    assert_int_equal((int)*p, 0x5A);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 12: get_uint8_dict_ptr_n
// ================================================================================
 
static void test_u8d_ptr_n_zero_len_returns_null(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    assert_null(get_uint8_dict_ptr_n(d, "k", 0));
    return_uint8_dict(d);
}
 
static void test_u8d_ptr_n_points_to_bounded_key_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    const char* buf = "flags_ext";
    insert_uint8_dict_n(d, buf, 5, 0xABu, a);   /* key = "flags" */
    const uint8_t* p = get_uint8_dict_ptr_n(d, buf, 5);
    assert_non_null(p);
    assert_int_equal((int)*p, 0xAB);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 13: has_uint8_dict_key
// ================================================================================
 
static void test_u8d_has_null_dict_returns_false(void** state) {
    (void)state;
    assert_false(has_uint8_dict_key(NULL, "k"));
}
 
static void test_u8d_has_null_key_returns_false(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    assert_false(has_uint8_dict_key(d, NULL));
    return_uint8_dict(d);
}
 
static void test_u8d_has_present_key_returns_true(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "active", 1u, a);
    assert_true(has_uint8_dict_key(d, "active"));
    return_uint8_dict(d);
}
 
static void test_u8d_has_absent_key_returns_false(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    assert_false(has_uint8_dict_key(d, "inactive"));
    return_uint8_dict(d);
}
 
static void test_u8d_has_returns_false_after_pop(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "temp", 1u, a);
    pop_uint8_dict(d, "temp", NULL);
    assert_false(has_uint8_dict_key(d, "temp"));
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 14: has_uint8_dict_key_n
// ================================================================================
 
static void test_u8d_has_n_zero_len_returns_false(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    assert_false(has_uint8_dict_key_n(d, "k", 0));
    return_uint8_dict(d);
}
 
static void test_u8d_has_n_distinguishes_by_length(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    /* Insert "ab" (2 bytes) and "abc" (3 bytes) as distinct keys. */
    insert_uint8_dict_n(d, "abc", 2, 10u, a);   /* key = "ab" */
    insert_uint8_dict_n(d, "abc", 3, 20u, a);   /* key = "abc" */
    assert_true(has_uint8_dict_key_n(d, "abc", 2));
    assert_true(has_uint8_dict_key_n(d, "abc", 3));
    assert_false(has_uint8_dict_key_n(d, "abc", 1));  /* "a" not inserted */
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 15: clear_uint8_dict
// ================================================================================
 
static void test_u8d_clear_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_uint8_dict(NULL), NULL_POINTER);
}
 
static void test_u8d_clear_resets_hash_size_to_zero(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "a", 1u, a);
    insert_uint8_dict(d, "b", 2u, a);
    assert_int_equal(clear_uint8_dict(d), NO_ERROR);
    assert_int_equal((int)uint8_dict_hash_size(d), 0);
    assert_true(is_uint8_dict_empty(d));
    return_uint8_dict(d);
}
 
static void test_u8d_clear_dict_is_reusable_after_clear(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "x", 1u, a);
    clear_uint8_dict(d);
    /* Re-insert same key after clear */
    assert_int_equal(insert_uint8_dict(d, "x", 99u, a), NO_ERROR);
    uint8_t v = 0;
    assert_int_equal(get_uint8_dict_value(d, "x", &v), NO_ERROR);
    assert_int_equal((int)v, 99);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 16: copy_uint8_dict
// ================================================================================
 
static void test_u8d_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_expect_t r = copy_uint8_dict(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_u8d_copy_contains_all_entries(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "x", 10u, a);
    insert_uint8_dict(d, "y", 20u, a);
    uint8_dict_expect_t cr = copy_uint8_dict(d, a);
    assert_true(cr.has_value);
    assert_int_equal((int)uint8_dict_hash_size(cr.u.value),
                     (int)uint8_dict_hash_size(d));
    uint8_t v = 0;
    assert_int_equal(get_uint8_dict_value(cr.u.value, "x", &v), NO_ERROR);
    assert_int_equal((int)v, 10);
    assert_int_equal(get_uint8_dict_value(cr.u.value, "y", &v), NO_ERROR);
    assert_int_equal((int)v, 20);
    return_uint8_dict(cr.u.value);
    return_uint8_dict(d);
}
 
static void test_u8d_copy_is_independent_of_original(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "shared", 5u, a);
    uint8_dict_expect_t cr = copy_uint8_dict(d, a);
    assert_true(cr.has_value);
    /* Mutate original */
    update_uint8_dict(d, "shared", 99u);
    /* Copy must retain original value */
    uint8_t v = 0;
    assert_int_equal(get_uint8_dict_value(cr.u.value, "shared", &v), NO_ERROR);
    assert_int_equal((int)v, 5);
    return_uint8_dict(cr.u.value);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 17: merge_uint8_dict
// ================================================================================
 
static void test_u8d_merge_null_first_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    uint8_dict_expect_t r = merge_uint8_dict(NULL, d, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_uint8_dict(d);
}
 
static void test_u8d_merge_null_second_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    uint8_dict_expect_t r = merge_uint8_dict(d, NULL, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_uint8_dict(d);
}
 
static void test_u8d_merge_no_overwrite_keeps_first_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* da = _make_dict(4);
    uint8_dict_t* db = _make_dict(4);
    insert_uint8_dict(da, "shared", 10u, a);
    insert_uint8_dict(da, "only_a",  1u, a);
    insert_uint8_dict(db, "shared", 99u, a);
    insert_uint8_dict(db, "only_b",  2u, a);
 
    uint8_dict_expect_t mr = merge_uint8_dict(da, db, false, a);
    assert_true(mr.has_value);
    uint8_t v = 0;
    /* "shared" keeps da's value */
    assert_int_equal(get_uint8_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_int_equal((int)v, 10);
    assert_int_equal(get_uint8_dict_value(mr.u.value, "only_a", &v), NO_ERROR);
    assert_int_equal((int)v, 1);
    assert_int_equal(get_uint8_dict_value(mr.u.value, "only_b", &v), NO_ERROR);
    assert_int_equal((int)v, 2);
    assert_int_equal((int)uint8_dict_hash_size(mr.u.value), 3);
    return_uint8_dict(mr.u.value);
    return_uint8_dict(da);
    return_uint8_dict(db);
}
 
static void test_u8d_merge_overwrite_uses_second_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* da = _make_dict(4);
    uint8_dict_t* db = _make_dict(4);
    insert_uint8_dict(da, "shared", 10u, a);
    insert_uint8_dict(db, "shared", 99u, a);
 
    uint8_dict_expect_t mr = merge_uint8_dict(da, db, true, a);
    assert_true(mr.has_value);
    uint8_t v = 0;
    assert_int_equal(get_uint8_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_int_equal((int)v, 99);
    return_uint8_dict(mr.u.value);
    return_uint8_dict(da);
    return_uint8_dict(db);
}
 
static void test_u8d_merge_sources_are_not_modified(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* da = _make_dict(4);
    uint8_dict_t* db = _make_dict(4);
    insert_uint8_dict(da, "a", 1u, a);
    insert_uint8_dict(db, "b", 2u, a);
 
    uint8_dict_expect_t mr = merge_uint8_dict(da, db, false, a);
    assert_true(mr.has_value);
    /* Source dicts untouched */
    assert_int_equal((int)uint8_dict_hash_size(da), 1);
    assert_int_equal((int)uint8_dict_hash_size(db), 1);
    return_uint8_dict(mr.u.value);
    return_uint8_dict(da);
    return_uint8_dict(db);
}
 
// ================================================================================
// Group 18: foreach_uint8_dict
// ================================================================================
 
static void test_u8d_foreach_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(foreach_uint8_dict(NULL, _sum_iter, NULL), NULL_POINTER);
}
 
static void test_u8d_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    assert_int_equal(foreach_uint8_dict(d, NULL, NULL), NULL_POINTER);
    return_uint8_dict(d);
}
 
static void test_u8d_foreach_visits_all_entries(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "a", 10u, a);
    insert_uint8_dict(d, "b", 20u, a);
    insert_uint8_dict(d, "c", 30u, a);
    _iter_ctx_t ctx = { 0, 0 };
    assert_int_equal(foreach_uint8_dict(d, _sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 3);
    assert_int_equal((int)ctx.sum, 60);
    return_uint8_dict(d);
}
 
static void test_u8d_foreach_empty_dict_calls_fn_zero_times(void** state) {
    (void)state;
    uint8_dict_t* d = _make_dict(4);
    _iter_ctx_t ctx = { 0, 0 };
    assert_int_equal(foreach_uint8_dict(d, _sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 0);
    return_uint8_dict(d);
}
 
typedef struct { bool ok; } _str_ctx_t;
 
static void _check_term(const char* key, size_t key_len,
                         uint8_t value, void* ud) {
    (void)value;
    _str_ctx_t* c = (_str_ctx_t*)ud;
    if (strlen(key) == key_len) c->ok = true;
}
 
static void test_u8d_foreach_key_is_null_terminated(void** state) {
    (void)state;
    /* Verify that the key pointer delivered to the callback is null-terminated
     * (the dict copies the key bytes and appends \0 internally). */
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    insert_uint8_dict(d, "hello", 1u, a);
    _str_ctx_t sctx = { false };
    foreach_uint8_dict(d, _check_term, &sctx);
    assert_true(sctx.ok);
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 19: introspection
// ================================================================================
 
static void test_u8d_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint8_dict_size(NULL), 0);
}
 
static void test_u8d_hash_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint8_dict_hash_size(NULL), 0);
}
 
static void test_u8d_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint8_dict_alloc(NULL), 0);
}
 
static void test_u8d_is_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_uint8_dict_empty(NULL));
}
 
static void test_u8d_alloc_is_at_least_requested_capacity(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_expect_t r = init_uint8_dict(8, true, a);
    assert_true(r.has_value);
    assert_true(uint8_dict_alloc(r.u.value) >= 8);
    return_uint8_dict(r.u.value);
}
 
static void test_u8d_is_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_dict_t* d = _make_dict(4);
    assert_true(is_uint8_dict_empty(d));
    insert_uint8_dict(d, "x", 1u, a);
    assert_false(is_uint8_dict_empty(d));
    pop_uint8_dict(d, "x", NULL);
    assert_true(is_uint8_dict_empty(d));
    return_uint8_dict(d);
}
 
// ================================================================================
// Group 20: growth / resize stress
// ================================================================================
 
static void test_u8d_growth_all_values_survive_resize(void** state) {
    (void)state;
    /* Start with a tiny capacity so multiple resizes are forced. */
    allocator_vtable_t a = heap_allocator();
    uint8_dict_expect_t r = init_uint8_dict(2, true, a);
    assert_true(r.has_value);
    uint8_dict_t* d = r.u.value;
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "key%d", i);
        assert_int_equal(insert_uint8_dict(d, key, (uint8_t)(i % 256), a),
                         NO_ERROR);
    }
    assert_int_equal((int)uint8_dict_hash_size(d), 200);
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "key%d", i);
        uint8_t v = 0;
        assert_int_equal(get_uint8_dict_value(d, key, &v), NO_ERROR);
        assert_int_equal((int)v, i % 256);
    }
 
    return_uint8_dict(d);
}
 
static void test_u8d_growth_disabled_returns_capacity_overflow(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    /* capacity 2 → rounded to 8; growth disabled */
    uint8_dict_expect_t r = init_uint8_dict(2, false, a);
    assert_true(r.has_value);
    uint8_dict_t* d = r.u.value;
    size_t cap = uint8_dict_alloc(d);
 
    error_code_t err = NO_ERROR;
    for (size_t i = 0; i <= cap; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%zu", i);
        err = insert_uint8_dict(d, key, (uint8_t)i, a);
        if (err != NO_ERROR) break;
    }
    assert_int_equal(err, CAPACITY_OVERFLOW);
    return_uint8_dict(d);
}
 
// ================================================================================
// main
// ================================================================================
 
const struct CMUnitTest test_uint8_dict[] = {

    /* Group 1: init_uint8_dict */
    cmocka_unit_test(test_u8d_init_null_allocator_fails),
    cmocka_unit_test(test_u8d_init_zero_capacity_fails),
    cmocka_unit_test(test_u8d_init_returns_valid_dict),
    cmocka_unit_test(test_u8d_init_data_size_is_one_byte),

    /* Group 2: return_uint8_dict */
    cmocka_unit_test(test_u8d_return_null_is_safe),
    cmocka_unit_test(test_u8d_return_valid_dict_does_not_crash),

    /* Group 3: insert_uint8_dict */
    cmocka_unit_test(test_u8d_insert_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u8d_insert_null_key_returns_null_pointer),
    cmocka_unit_test(test_u8d_insert_duplicate_key_returns_invalid_arg),
    cmocka_unit_test(test_u8d_insert_stores_correct_value),
    cmocka_unit_test(test_u8d_insert_increments_hash_size),
    cmocka_unit_test(test_u8d_insert_key_copy_is_independent),

    /* Group 4: insert_uint8_dict_n */
    cmocka_unit_test(test_u8d_insert_n_null_key_returns_null_pointer),
    cmocka_unit_test(test_u8d_insert_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u8d_insert_n_uses_only_specified_bytes),

    /* Group 5: pop_uint8_dict */
    cmocka_unit_test(test_u8d_pop_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u8d_pop_null_key_returns_null_pointer),
    cmocka_unit_test(test_u8d_pop_missing_key_returns_not_found),
    cmocka_unit_test(test_u8d_pop_returns_correct_value),
    cmocka_unit_test(test_u8d_pop_null_out_value_discards_safely),
    cmocka_unit_test(test_u8d_pop_key_no_longer_found_after_pop),

    /* Group 6: pop_uint8_dict_n */
    cmocka_unit_test(test_u8d_pop_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u8d_pop_n_removes_bounded_key),

    /* Group 7: update_uint8_dict */
    cmocka_unit_test(test_u8d_update_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u8d_update_null_key_returns_null_pointer),
    cmocka_unit_test(test_u8d_update_missing_key_returns_not_found),
    cmocka_unit_test(test_u8d_update_overwrites_existing_value),

    /* Group 8: update_uint8_dict_n */
    cmocka_unit_test(test_u8d_update_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u8d_update_n_updates_bounded_key),

    /* Group 9: get_uint8_dict_value */
    cmocka_unit_test(test_u8d_get_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u8d_get_null_key_returns_null_pointer),
    cmocka_unit_test(test_u8d_get_null_out_returns_null_pointer),
    cmocka_unit_test(test_u8d_get_missing_key_returns_not_found),
    cmocka_unit_test(test_u8d_get_retrieves_correct_value),

    /* Group 10: get_uint8_dict_value_n */
    cmocka_unit_test(test_u8d_get_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u8d_get_n_retrieves_bounded_key),

    /* Group 11: get_uint8_dict_ptr */
    cmocka_unit_test(test_u8d_ptr_null_dict_returns_null),
    cmocka_unit_test(test_u8d_ptr_null_key_returns_null),
    cmocka_unit_test(test_u8d_ptr_missing_key_returns_null),
    cmocka_unit_test(test_u8d_ptr_points_to_correct_value),

    /* Group 12: get_uint8_dict_ptr_n */
    cmocka_unit_test(test_u8d_ptr_n_zero_len_returns_null),
    cmocka_unit_test(test_u8d_ptr_n_points_to_bounded_key_value),

    /* Group 13: has_uint8_dict_key */
    cmocka_unit_test(test_u8d_has_null_dict_returns_false),
    cmocka_unit_test(test_u8d_has_null_key_returns_false),
    cmocka_unit_test(test_u8d_has_present_key_returns_true),
    cmocka_unit_test(test_u8d_has_absent_key_returns_false),
    cmocka_unit_test(test_u8d_has_returns_false_after_pop),

    /* Group 14: has_uint8_dict_key_n */
    cmocka_unit_test(test_u8d_has_n_zero_len_returns_false),
    cmocka_unit_test(test_u8d_has_n_distinguishes_by_length),

    /* Group 15: clear_uint8_dict */
    cmocka_unit_test(test_u8d_clear_null_returns_null_pointer),
    cmocka_unit_test(test_u8d_clear_resets_hash_size_to_zero),
    cmocka_unit_test(test_u8d_clear_dict_is_reusable_after_clear),

    /* Group 16: copy_uint8_dict */
    cmocka_unit_test(test_u8d_copy_null_src_fails),
    cmocka_unit_test(test_u8d_copy_contains_all_entries),
    cmocka_unit_test(test_u8d_copy_is_independent_of_original),

    /* Group 17: merge_uint8_dict */
    cmocka_unit_test(test_u8d_merge_null_first_fails),
    cmocka_unit_test(test_u8d_merge_null_second_fails),
    cmocka_unit_test(test_u8d_merge_no_overwrite_keeps_first_value),
    cmocka_unit_test(test_u8d_merge_overwrite_uses_second_value),
    cmocka_unit_test(test_u8d_merge_sources_are_not_modified),

    /* Group 18: foreach_uint8_dict */
    cmocka_unit_test(test_u8d_foreach_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u8d_foreach_null_fn_returns_null_pointer),
    cmocka_unit_test(test_u8d_foreach_visits_all_entries),
    cmocka_unit_test(test_u8d_foreach_empty_dict_calls_fn_zero_times),
    cmocka_unit_test(test_u8d_foreach_key_is_null_terminated),

    /* Group 19: introspection */
    cmocka_unit_test(test_u8d_size_null_returns_zero),
    cmocka_unit_test(test_u8d_hash_size_null_returns_zero),
    cmocka_unit_test(test_u8d_alloc_null_returns_zero),
    cmocka_unit_test(test_u8d_is_empty_null_returns_true),
    cmocka_unit_test(test_u8d_alloc_is_at_least_requested_capacity),
    cmocka_unit_test(test_u8d_is_empty_reflects_contents),

    /* Group 20: growth / resize stress */
    cmocka_unit_test(test_u8d_growth_all_values_survive_resize),
    cmocka_unit_test(test_u8d_growth_disabled_returns_capacity_overflow),
};

const size_t test_uint8_dict_count = sizeof(test_uint8_dict) / sizeof(test_uint8_dict[0]);
// ================================================================================
// ================================================================================
// eof
