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
#include "c_int8.h"
#include "c_uint16.h"
#include "c_int16.h"


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

static int8_dict_t* _make_int8_dict(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    int8_dict_expect_t r = init_int8_dict(cap, true, a);
    assert_true(r.has_value);
    return r.u.value;
}
 
typedef struct { int count; int sum; } _i8d_iter_ctx_t;
 
static void _i8d_sum_iter(const char* key, size_t key_len,
                      int8_t value, void* ud) {
    (void)key; (void)key_len;
    _i8d_iter_ctx_t* c = (_i8d_iter_ctx_t*)ud;
    c->count++;
    c->sum += (int)value;
}
 
// ================================================================================
// Group 1: init_int8_dict / return_int8_dict
// ================================================================================
 
static void test_i8d_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    int8_dict_expect_t r = init_int8_dict(8, true, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_i8d_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_expect_t r = init_int8_dict(0, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_i8d_init_data_size_is_one_byte(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal((int)dict_data_size(d), 1);
    return_int8_dict(d);
}
 
static void test_i8d_init_dtype_is_int8(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal((int)d->dtype, (int)INT8_TYPE);
    return_int8_dict(d);
}
 
static void test_i8d_return_null_is_safe(void** state) {
    (void)state;
    return_int8_dict(NULL);
}
 
// ================================================================================
// Group 2: insert_int8_dict
// ================================================================================
 
static void test_i8d_insert_null_dict_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    assert_int_equal(insert_int8_dict(NULL, "k", 0, a), NULL_POINTER);
}
 
static void test_i8d_insert_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal(insert_int8_dict(d, NULL, 0, a), NULL_POINTER);
    return_int8_dict(d);
}
 
static void test_i8d_insert_duplicate_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal(insert_int8_dict(d, "k", 1, a), NO_ERROR);
    assert_int_equal(insert_int8_dict(d, "k", 2, a), INVALID_ARG);
    assert_int_equal((int)int8_dict_hash_size(d), 1);
    return_int8_dict(d);
}
 
static void test_i8d_insert_positive_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal(insert_int8_dict(d, "pos", 127, a), NO_ERROR);
    int8_t v = 0;
    assert_int_equal(get_int8_dict_value(d, "pos", &v), NO_ERROR);
    assert_int_equal((int)v, 127);
    return_int8_dict(d);
}
 
static void test_i8d_insert_negative_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal(insert_int8_dict(d, "neg", -128, a), NO_ERROR);
    int8_t v = 0;
    assert_int_equal(get_int8_dict_value(d, "neg", &v), NO_ERROR);
    assert_int_equal((int)v, -128);
    return_int8_dict(d);
}
 
static void test_i8d_insert_zero_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal(insert_int8_dict(d, "zero", 0, a), NO_ERROR);
    int8_t v = 99;
    assert_int_equal(get_int8_dict_value(d, "zero", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    return_int8_dict(d);
}
 
// ================================================================================
// Group 3: insert_int8_dict_n
// ================================================================================
 
static void test_i8d_insert_n_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal(insert_int8_dict_n(d, NULL, 3, 1, a), NULL_POINTER);
    return_int8_dict(d);
}
 
static void test_i8d_insert_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal(insert_int8_dict_n(d, "k", 0, 1, a), INVALID_ARG);
    return_int8_dict(d);
}
 
static void test_i8d_insert_n_uses_only_specified_bytes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    /* Buffer "delta_fine" — insert only "delta" (5 bytes) with value -3 */
    const char* buf = "delta_fine";
    assert_int_equal(insert_int8_dict_n(d, buf, 5, -3, a), NO_ERROR);
    int8_t v = 0;
    /* Retrieve via bounded key */
    assert_int_equal(get_int8_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal((int)v, -3);
    /* Full buffer is a different key — not found */
    assert_int_equal(get_int8_dict_value_n(d, buf, 10, &v), NOT_FOUND);
    /* Plain null-terminated "delta" also finds it */
    assert_int_equal(get_int8_dict_value(d, "delta", &v), NO_ERROR);
    assert_int_equal((int)v, -3);
    return_int8_dict(d);
}
 
// ================================================================================
// Group 4: pop_int8_dict
// ================================================================================
 
static void test_i8d_pop_null_dict_returns_null_pointer(void** state) {
    (void)state;
    int8_t v;
    assert_int_equal(pop_int8_dict(NULL, "k", &v), NULL_POINTER);
}
 
static void test_i8d_pop_null_key_returns_null_pointer(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    int8_t v;
    assert_int_equal(pop_int8_dict(d, NULL, &v), NULL_POINTER);
    return_int8_dict(d);
}
 
static void test_i8d_pop_missing_key_returns_not_found(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal(pop_int8_dict(d, "missing", NULL), NOT_FOUND);
    return_int8_dict(d);
}
 
static void test_i8d_pop_returns_signed_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    insert_int8_dict(d, "gain", -64, a);
    int8_t v = 0;
    assert_int_equal(pop_int8_dict(d, "gain", &v), NO_ERROR);
    assert_int_equal((int)v, -64);
    assert_int_equal((int)int8_dict_hash_size(d), 0);
    return_int8_dict(d);
}
 
static void test_i8d_pop_null_out_value_safe(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    insert_int8_dict(d, "x", -1, a);
    assert_int_equal(pop_int8_dict(d, "x", NULL), NO_ERROR);
    assert_int_equal((int)int8_dict_hash_size(d), 0);
    return_int8_dict(d);
}
 
// ================================================================================
// Group 5: pop_int8_dict_n
// ================================================================================
 
static void test_i8d_pop_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal(pop_int8_dict_n(d, "k", 0, NULL), INVALID_ARG);
    return_int8_dict(d);
}
 
static void test_i8d_pop_n_removes_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    const char* buf = "gain_fine";
    insert_int8_dict_n(d, buf, 4, -64, a);   /* key = "gain" */
    int8_t v = 0;
    assert_int_equal(pop_int8_dict_n(d, buf, 4, &v), NO_ERROR);
    assert_int_equal((int)v, -64);
    assert_int_equal((int)int8_dict_hash_size(d), 0);
    return_int8_dict(d);
}
 
// ================================================================================
// Group 6: update_int8_dict
// ================================================================================
 
static void test_i8d_update_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(update_int8_dict(NULL, "k", 1), NULL_POINTER);
}
 
static void test_i8d_update_null_key_returns_null_pointer(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal(update_int8_dict(d, NULL, 1), NULL_POINTER);
    return_int8_dict(d);
}
 
static void test_i8d_update_missing_key_returns_not_found(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal(update_int8_dict(d, "missing", 0), NOT_FOUND);
    return_int8_dict(d);
}
 
static void test_i8d_update_flips_sign(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    insert_int8_dict(d, "offset", 10, a);
    assert_int_equal(update_int8_dict(d, "offset", -10), NO_ERROR);
    int8_t v = 0;
    assert_int_equal(get_int8_dict_value(d, "offset", &v), NO_ERROR);
    assert_int_equal((int)v, -10);
    assert_int_equal((int)int8_dict_hash_size(d), 1);
    return_int8_dict(d);
}
 
// ================================================================================
// Group 7: update_int8_dict_n
// ================================================================================
 
static void test_i8d_update_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal(update_int8_dict_n(d, "k", 0, 1), INVALID_ARG);
    return_int8_dict(d);
}
 
static void test_i8d_update_n_updates_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    const char* buf = "offset_fine";
    insert_int8_dict_n(d, buf, 6, 10, a);   /* key = "offset" */
    assert_int_equal(update_int8_dict_n(d, buf, 6, -10), NO_ERROR);
    int8_t v = 0;
    assert_int_equal(get_int8_dict_value_n(d, buf, 6, &v), NO_ERROR);
    assert_int_equal((int)v, -10);
    return_int8_dict(d);
}
 
// ================================================================================
// Group 8: get_int8_dict_value
// ================================================================================
 
static void test_i8d_get_null_dict_returns_null_pointer(void** state) {
    (void)state;
    int8_t v;
    assert_int_equal(get_int8_dict_value(NULL, "k", &v), NULL_POINTER);
}
 
static void test_i8d_get_null_key_returns_null_pointer(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    int8_t v;
    assert_int_equal(get_int8_dict_value(d, NULL, &v), NULL_POINTER);
    return_int8_dict(d);
}
 
static void test_i8d_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    insert_int8_dict(d, "k", 1, a);
    assert_int_equal(get_int8_dict_value(d, "k", NULL), NULL_POINTER);
    return_int8_dict(d);
}
 
static void test_i8d_get_missing_key_returns_not_found(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    int8_t v;
    assert_int_equal(get_int8_dict_value(d, "missing", &v), NOT_FOUND);
    return_int8_dict(d);
}
 
static void test_i8d_get_retrieves_signed_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    insert_int8_dict(d, "min", INT8_MIN, a);
    insert_int8_dict(d, "max", INT8_MAX, a);
    insert_int8_dict(d, "zero", 0, a);
    int8_t v = 0;
    assert_int_equal(get_int8_dict_value(d, "min", &v), NO_ERROR);
    assert_int_equal((int)v, INT8_MIN);
    assert_int_equal(get_int8_dict_value(d, "max", &v), NO_ERROR);
    assert_int_equal((int)v, INT8_MAX);
    assert_int_equal(get_int8_dict_value(d, "zero", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    return_int8_dict(d);
}
 
// ================================================================================
// Group 9: get_int8_dict_value_n
// ================================================================================
 
static void test_i8d_get_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    int8_t v;
    assert_int_equal(get_int8_dict_value_n(d, "k", 0, &v), INVALID_ARG);
    return_int8_dict(d);
}
 
static void test_i8d_get_n_retrieves_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    const char* buf = "bias_coarse";
    insert_int8_dict_n(d, buf, 4, -64, a);   /* key = "bias" */
    int8_t v = 0;
    assert_int_equal(get_int8_dict_value_n(d, buf, 4, &v), NO_ERROR);
    assert_int_equal((int)v, -64);
    /* Longer slice is a different key */
    assert_int_equal(get_int8_dict_value_n(d, buf, 11, &v), NOT_FOUND);
    return_int8_dict(d);
}
 
// ================================================================================
// Group 10: get_int8_dict_ptr
// ================================================================================
 
static void test_i8d_ptr_null_dict_returns_null(void** state) {
    (void)state;
    assert_null(get_int8_dict_ptr(NULL, "k"));
}
 
static void test_i8d_ptr_null_key_returns_null(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    assert_null(get_int8_dict_ptr(d, NULL));
    return_int8_dict(d);
}
 
static void test_i8d_ptr_missing_returns_null(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    assert_null(get_int8_dict_ptr(d, "missing"));
    return_int8_dict(d);
}
 
static void test_i8d_ptr_points_to_signed_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    insert_int8_dict(d, "trim", -3, a);
    const int8_t* p = get_int8_dict_ptr(d, "trim");
    assert_non_null(p);
    assert_int_equal((int)*p, -3);
    return_int8_dict(d);
}
 
// ================================================================================
// Group 11: get_int8_dict_ptr_n
// ================================================================================
 
static void test_i8d_ptr_n_zero_len_returns_null(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    assert_null(get_int8_dict_ptr_n(d, "k", 0));
    return_int8_dict(d);
}
 
static void test_i8d_ptr_n_points_to_bounded_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    const char* buf = "trim_fine";
    insert_int8_dict_n(d, buf, 4, -3, a);   /* key = "trim" */
    const int8_t* p = get_int8_dict_ptr_n(d, buf, 4);
    assert_non_null(p);
    assert_int_equal((int)*p, -3);
    return_int8_dict(d);
}
 
// ================================================================================
// Group 12: has_int8_dict_key
// ================================================================================
 
static void test_i8d_has_null_dict_returns_false(void** state) {
    (void)state;
    assert_false(has_int8_dict_key(NULL, "k"));
}
 
static void test_i8d_has_null_key_returns_false(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    assert_false(has_int8_dict_key(d, NULL));
    return_int8_dict(d);
}
 
static void test_i8d_has_present_and_absent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    insert_int8_dict(d, "present", -1, a);
    assert_true(has_int8_dict_key(d, "present"));
    assert_false(has_int8_dict_key(d, "absent"));
    return_int8_dict(d);
}
 
// ================================================================================
// Group 13: has_int8_dict_key_n
// ================================================================================
 
static void test_i8d_has_n_zero_len_returns_false(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    assert_false(has_int8_dict_key_n(d, "k", 0));
    return_int8_dict(d);
}
 
static void test_i8d_has_n_distinguishes_by_length(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    insert_int8_dict_n(d, "abc", 2, -1, a);  /* key = "ab" */
    insert_int8_dict_n(d, "abc", 3, -2, a);  /* key = "abc" */
    assert_true(has_int8_dict_key_n(d, "abc", 2));
    assert_true(has_int8_dict_key_n(d, "abc", 3));
    assert_false(has_int8_dict_key_n(d, "abc", 1));
    return_int8_dict(d);
}
 
// ================================================================================
// Group 14: clear_int8_dict
// ================================================================================
 
static void test_i8d_clear_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_int8_dict(NULL), NULL_POINTER);
}
 
static void test_i8d_clear_resets_and_is_reusable(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    insert_int8_dict(d, "a", -1, a);
    insert_int8_dict(d, "b",  2, a);
    assert_int_equal(clear_int8_dict(d), NO_ERROR);
    assert_int_equal((int)int8_dict_hash_size(d), 0);
    assert_true(is_int8_dict_empty(d));
    /* Re-insert same key after clear */
    assert_int_equal(insert_int8_dict(d, "a", -99, a), NO_ERROR);
    int8_t v = 0;
    assert_int_equal(get_int8_dict_value(d, "a", &v), NO_ERROR);
    assert_int_equal((int)v, -99);
    return_int8_dict(d);
}
 
// ================================================================================
// Group 15: copy_int8_dict
// ================================================================================
 
static void test_i8d_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_expect_t r = copy_int8_dict(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_i8d_copy_is_independent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* orig = _make_int8_dict(4);
    insert_int8_dict(orig, "x", -10, a);
    insert_int8_dict(orig, "y",  20, a);
 
    int8_dict_expect_t cr = copy_int8_dict(orig, a);
    assert_true(cr.has_value);
 
    /* Mutate original */
    update_int8_dict(orig, "x", 99);
 
    /* Copy retains original value */
    int8_t v = 0;
    assert_int_equal(get_int8_dict_value(cr.u.value, "x", &v), NO_ERROR);
    assert_int_equal((int)v, -10);
    assert_int_equal((int)int8_dict_hash_size(cr.u.value), 2);
 
    return_int8_dict(cr.u.value);
    return_int8_dict(orig);
}
 
// ================================================================================
// Group 16: merge_int8_dict
// ================================================================================
 
static void test_i8d_merge_null_first_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    int8_dict_expect_t r = merge_int8_dict(NULL, d, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_int8_dict(d);
}
 
static void test_i8d_merge_no_overwrite_keeps_first_signed_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* da = _make_int8_dict(4);
    int8_dict_t* db = _make_int8_dict(4);
    insert_int8_dict(da, "shared", -10, a);
    insert_int8_dict(db, "shared",  99, a);
    insert_int8_dict(db, "new",      5, a);
 
    int8_dict_expect_t mr = merge_int8_dict(da, db, false, a);
    assert_true(mr.has_value);
    int8_t v = 0;
    assert_int_equal(get_int8_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_int_equal((int)v, -10);   /* da's value kept */
    assert_int_equal(get_int8_dict_value(mr.u.value, "new", &v), NO_ERROR);
    assert_int_equal((int)v, 5);
 
    return_int8_dict(mr.u.value);
    return_int8_dict(da);
    return_int8_dict(db);
}
 
static void test_i8d_merge_overwrite_uses_second_signed_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* da = _make_int8_dict(4);
    int8_dict_t* db = _make_int8_dict(4);
    insert_int8_dict(da, "shared", -10, a);
    insert_int8_dict(db, "shared", -99, a);
 
    int8_dict_expect_t mr = merge_int8_dict(da, db, true, a);
    assert_true(mr.has_value);
    int8_t v = 0;
    assert_int_equal(get_int8_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_int_equal((int)v, -99);   /* db's value wins */
 
    return_int8_dict(mr.u.value);
    return_int8_dict(da);
    return_int8_dict(db);
}
 
// ================================================================================
// Group 17: foreach_int8_dict
// ================================================================================
 
static void test_i8d_foreach_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(foreach_int8_dict(NULL, _i8d_sum_iter, NULL), NULL_POINTER);
}
 
static void test_i8d_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    int8_dict_t* d = _make_int8_dict(4);
    assert_int_equal(foreach_int8_dict(d, NULL, NULL), NULL_POINTER);
    return_int8_dict(d);
}
 
static void test_i8d_foreach_signed_sum(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    /* Insert values that partly cancel: -10 + 5 + 5 = 0 */
    insert_int8_dict(d, "a", -10, a);
    insert_int8_dict(d, "b",   5, a);
    insert_int8_dict(d, "c",   5, a);
    _i8d_iter_ctx_t ctx = { 0, 0 };
    assert_int_equal(foreach_int8_dict(d, _i8d_sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 3);
    assert_int_equal(ctx.sum, 0);
    return_int8_dict(d);
}
 
static void test_i8d_foreach_delivers_signed_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    insert_int8_dict(d, "min", INT8_MIN, a);
    insert_int8_dict(d, "max", INT8_MAX, a);
    _i8d_iter_ctx_t ctx = { 0, 0 };
    foreach_int8_dict(d, _i8d_sum_iter, &ctx);
    assert_int_equal(ctx.count, 2);
    /* INT8_MIN + INT8_MAX == -128 + 127 == -1 */
    assert_int_equal(ctx.sum, -1);
    return_int8_dict(d);
}
 
// ================================================================================
// Group 18: introspection
// ================================================================================
 
static void test_i8d_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int8_dict_size(NULL), 0);
}
 
static void test_i8d_hash_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int8_dict_hash_size(NULL), 0);
}
 
static void test_i8d_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int8_dict_alloc(NULL), 0);
}
 
static void test_i8d_is_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_int8_dict_empty(NULL));
}
 
static void test_i8d_is_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_t* d = _make_int8_dict(4);
    assert_true(is_int8_dict_empty(d));
    insert_int8_dict(d, "x", -1, a);
    assert_false(is_int8_dict_empty(d));
    pop_int8_dict(d, "x", NULL);
    assert_true(is_int8_dict_empty(d));
    return_int8_dict(d);
}
 
// ================================================================================
// Group 19: growth stress with signed values
// ================================================================================
 
static void test_i8d_growth_signed_values_survive_resize(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_dict_expect_t r = init_int8_dict(2, true, a);
    assert_true(r.has_value);
    int8_dict_t* d = r.u.value;
 
    /* Insert 100 entries with alternating positive and negative values */
    for (int i = 0; i < 100; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        int8_t val = (int8_t)((i % 2 == 0) ? -(i % 128) : (i % 128));
        assert_int_equal(insert_int8_dict(d, key, val, a), NO_ERROR);
    }
    assert_int_equal((int)int8_dict_hash_size(d), 100);
 
    /* Verify every value round-trips correctly */
    for (int i = 0; i < 100; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        int8_t expected = (int8_t)((i % 2 == 0) ? -(i % 128) : (i % 128));
        int8_t v = 0;
        assert_int_equal(get_int8_dict_value(d, key, &v), NO_ERROR);
        assert_int_equal((int)v, (int)expected);
    }
 
    return_int8_dict(d);
}
 
// ================================================================================
// main
// ================================================================================
 
const struct CMUnitTest test_int8_dict[] = {

    /* Group 1: init / return */
    cmocka_unit_test(test_i8d_init_null_allocator_fails),
    cmocka_unit_test(test_i8d_init_zero_capacity_fails),
    cmocka_unit_test(test_i8d_init_data_size_is_one_byte),
    cmocka_unit_test(test_i8d_init_dtype_is_int8),
    cmocka_unit_test(test_i8d_return_null_is_safe),

    /* Group 2: insert */
    cmocka_unit_test(test_i8d_insert_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i8d_insert_null_key_returns_null_pointer),
    cmocka_unit_test(test_i8d_insert_duplicate_returns_invalid_arg),
    cmocka_unit_test(test_i8d_insert_positive_value),
    cmocka_unit_test(test_i8d_insert_negative_value),
    cmocka_unit_test(test_i8d_insert_zero_value),

    /* Group 3: insert_n */
    cmocka_unit_test(test_i8d_insert_n_null_key_returns_null_pointer),
    cmocka_unit_test(test_i8d_insert_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i8d_insert_n_uses_only_specified_bytes),

    /* Group 4: pop */
    cmocka_unit_test(test_i8d_pop_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i8d_pop_null_key_returns_null_pointer),
    cmocka_unit_test(test_i8d_pop_missing_key_returns_not_found),
    cmocka_unit_test(test_i8d_pop_returns_signed_value),
    cmocka_unit_test(test_i8d_pop_null_out_value_safe),

    /* Group 5: pop_n */
    cmocka_unit_test(test_i8d_pop_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i8d_pop_n_removes_bounded_key),

    /* Group 6: update */
    cmocka_unit_test(test_i8d_update_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i8d_update_null_key_returns_null_pointer),
    cmocka_unit_test(test_i8d_update_missing_key_returns_not_found),
    cmocka_unit_test(test_i8d_update_flips_sign),

    /* Group 7: update_n */
    cmocka_unit_test(test_i8d_update_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i8d_update_n_updates_bounded_key),

    /* Group 8: get_value */
    cmocka_unit_test(test_i8d_get_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i8d_get_null_key_returns_null_pointer),
    cmocka_unit_test(test_i8d_get_null_out_returns_null_pointer),
    cmocka_unit_test(test_i8d_get_missing_key_returns_not_found),
    cmocka_unit_test(test_i8d_get_retrieves_signed_extremes),

    /* Group 9: get_value_n */
    cmocka_unit_test(test_i8d_get_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i8d_get_n_retrieves_bounded_key),

    /* Group 10: get_ptr */
    cmocka_unit_test(test_i8d_ptr_null_dict_returns_null),
    cmocka_unit_test(test_i8d_ptr_null_key_returns_null),
    cmocka_unit_test(test_i8d_ptr_missing_returns_null),
    cmocka_unit_test(test_i8d_ptr_points_to_signed_value),

    /* Group 11: get_ptr_n */
    cmocka_unit_test(test_i8d_ptr_n_zero_len_returns_null),
    cmocka_unit_test(test_i8d_ptr_n_points_to_bounded_value),

    /* Group 12: has_key */
    cmocka_unit_test(test_i8d_has_null_dict_returns_false),
    cmocka_unit_test(test_i8d_has_null_key_returns_false),
    cmocka_unit_test(test_i8d_has_present_and_absent),

    /* Group 13: has_key_n */
    cmocka_unit_test(test_i8d_has_n_zero_len_returns_false),
    cmocka_unit_test(test_i8d_has_n_distinguishes_by_length),

    /* Group 14: clear */
    cmocka_unit_test(test_i8d_clear_null_returns_null_pointer),
    cmocka_unit_test(test_i8d_clear_resets_and_is_reusable),

    /* Group 15: copy */
    cmocka_unit_test(test_i8d_copy_null_src_fails),
    cmocka_unit_test(test_i8d_copy_is_independent),

    /* Group 16: merge */
    cmocka_unit_test(test_i8d_merge_null_first_fails),
    cmocka_unit_test(test_i8d_merge_no_overwrite_keeps_first_signed_value),
    cmocka_unit_test(test_i8d_merge_overwrite_uses_second_signed_value),

    /* Group 17: foreach */
    cmocka_unit_test(test_i8d_foreach_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i8d_foreach_null_fn_returns_null_pointer),
    cmocka_unit_test(test_i8d_foreach_signed_sum),
    cmocka_unit_test(test_i8d_foreach_delivers_signed_extremes),

    /* Group 18: introspection */
    cmocka_unit_test(test_i8d_size_null_returns_zero),
    cmocka_unit_test(test_i8d_hash_size_null_returns_zero),
    cmocka_unit_test(test_i8d_alloc_null_returns_zero),
    cmocka_unit_test(test_i8d_is_empty_null_returns_true),
    cmocka_unit_test(test_i8d_is_empty_reflects_contents),

    /* Group 19: growth stress */
    cmocka_unit_test(test_i8d_growth_signed_values_survive_resize),

};
const size_t test_int8_dict_count = sizeof(test_int8_dict) / sizeof(test_int8_dict[0]);
// ================================================================================ 
// ================================================================================ 

static uint16_dict_t* _make_uint16_dict(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    uint16_dict_expect_t r = init_uint16_dict(cap, true, a);
    assert_true(r.has_value);
    return r.u.value;
}
 
typedef struct { int count; uint32_t sum; } _u16d_iter_ctx_t;
 
static void _u16d_sum_iter(const char* key, size_t key_len,
                            uint16_t value, void* ud) {
    (void)key; (void)key_len;
    _u16d_iter_ctx_t* c = (_u16d_iter_ctx_t*)ud;
    c->count++;
    c->sum += value;
}
 
// ================================================================================
// Group 1: init_uint16_dict / return_uint16_dict
// ================================================================================
 
static void test_u16d_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    uint16_dict_expect_t r = init_uint16_dict(8, true, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_u16d_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_expect_t r = init_uint16_dict(0, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_u16d_init_data_size_is_two_bytes(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal((int)dict_data_size(d), 2);
    return_uint16_dict(d);
}
 
static void test_u16d_init_dtype_is_uint16(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal((int)d->dtype, (int)UINT16_TYPE);
    return_uint16_dict(d);
}
 
static void test_u16d_return_null_is_safe(void** state) {
    (void)state;
    return_uint16_dict(NULL);
}
 
// ================================================================================
// Group 2: insert_uint16_dict
// ================================================================================
 
static void test_u16d_insert_null_dict_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    assert_int_equal(insert_uint16_dict(NULL, "k", 1u, a), NULL_POINTER);
}
 
static void test_u16d_insert_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal(insert_uint16_dict(d, NULL, 1u, a), NULL_POINTER);
    return_uint16_dict(d);
}
 
static void test_u16d_insert_duplicate_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal(insert_uint16_dict(d, "k", 1u, a), NO_ERROR);
    assert_int_equal(insert_uint16_dict(d, "k", 2u, a), INVALID_ARG);
    assert_int_equal((int)uint16_dict_hash_size(d), 1);
    return_uint16_dict(d);
}
 
static void test_u16d_insert_stores_zero(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal(insert_uint16_dict(d, "zero", 0u, a), NO_ERROR);
    uint16_t v = 99u;
    assert_int_equal(get_uint16_dict_value(d, "zero", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    return_uint16_dict(d);
}
 
static void test_u16d_insert_stores_max(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal(insert_uint16_dict(d, "max", UINT16_MAX, a), NO_ERROR);
    uint16_t v = 0u;
    assert_int_equal(get_uint16_dict_value(d, "max", &v), NO_ERROR);
    assert_int_equal((int)v, (int)UINT16_MAX);
    return_uint16_dict(d);
}
 
static void test_u16d_insert_stores_midrange(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal(insert_uint16_dict(d, "port", 8080u, a), NO_ERROR);
    uint16_t v = 0u;
    assert_int_equal(get_uint16_dict_value(d, "port", &v), NO_ERROR);
    assert_int_equal((int)v, 8080);
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 3: insert_uint16_dict_n
// ================================================================================
 
static void test_u16d_insert_n_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal(insert_uint16_dict_n(d, NULL, 3, 1u, a), NULL_POINTER);
    return_uint16_dict(d);
}
 
static void test_u16d_insert_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal(insert_uint16_dict_n(d, "k", 0, 1u, a), INVALID_ARG);
    return_uint16_dict(d);
}
 
static void test_u16d_insert_n_uses_only_specified_bytes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    /* "width_max" — insert only "width" (5 bytes) with value 1920 */
    const char* buf = "width_max";
    assert_int_equal(insert_uint16_dict_n(d, buf, 5, 1920u, a), NO_ERROR);
    uint16_t v = 0u;
    assert_int_equal(get_uint16_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal((int)v, 1920);
    /* Full buffer key "width_max" is different — not found */
    assert_int_equal(get_uint16_dict_value_n(d, buf, 9, &v), NOT_FOUND);
    /* Plain null-terminated lookup also works */
    assert_int_equal(get_uint16_dict_value(d, "width", &v), NO_ERROR);
    assert_int_equal((int)v, 1920);
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 4: pop_uint16_dict
// ================================================================================
 
static void test_u16d_pop_null_dict_returns_null_pointer(void** state) {
    (void)state;
    uint16_t v;
    assert_int_equal(pop_uint16_dict(NULL, "k", &v), NULL_POINTER);
}
 
static void test_u16d_pop_null_key_returns_null_pointer(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    uint16_t v;
    assert_int_equal(pop_uint16_dict(d, NULL, &v), NULL_POINTER);
    return_uint16_dict(d);
}
 
static void test_u16d_pop_missing_key_returns_not_found(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal(pop_uint16_dict(d, "missing", NULL), NOT_FOUND);
    return_uint16_dict(d);
}
 
static void test_u16d_pop_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    insert_uint16_dict(d, "port", 8080u, a);
    uint16_t v = 0u;
    assert_int_equal(pop_uint16_dict(d, "port", &v), NO_ERROR);
    assert_int_equal((int)v, 8080);
    assert_int_equal((int)uint16_dict_hash_size(d), 0);
    return_uint16_dict(d);
}
 
static void test_u16d_pop_null_out_value_discards_safely(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    insert_uint16_dict(d, "x", 100u, a);
    assert_int_equal(pop_uint16_dict(d, "x", NULL), NO_ERROR);
    assert_int_equal((int)uint16_dict_hash_size(d), 0);
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 5: pop_uint16_dict_n
// ================================================================================
 
static void test_u16d_pop_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal(pop_uint16_dict_n(d, "k", 0, NULL), INVALID_ARG);
    return_uint16_dict(d);
}
 
static void test_u16d_pop_n_removes_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    const char* buf = "port_http";
    insert_uint16_dict_n(d, buf, 4, 8080u, a);   /* key = "port" */
    uint16_t v = 0u;
    assert_int_equal(pop_uint16_dict_n(d, buf, 4, &v), NO_ERROR);
    assert_int_equal((int)v, 8080);
    assert_int_equal((int)uint16_dict_hash_size(d), 0);
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 6: update_uint16_dict
// ================================================================================
 
static void test_u16d_update_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(update_uint16_dict(NULL, "k", 1u), NULL_POINTER);
}
 
static void test_u16d_update_null_key_returns_null_pointer(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal(update_uint16_dict(d, NULL, 1u), NULL_POINTER);
    return_uint16_dict(d);
}
 
static void test_u16d_update_missing_key_returns_not_found(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal(update_uint16_dict(d, "missing", 1u), NOT_FOUND);
    return_uint16_dict(d);
}
 
static void test_u16d_update_overwrites_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    insert_uint16_dict(d, "port", 8080u, a);
    assert_int_equal(update_uint16_dict(d, "port", 443u), NO_ERROR);
    uint16_t v = 0u;
    assert_int_equal(get_uint16_dict_value(d, "port", &v), NO_ERROR);
    assert_int_equal((int)v, 443);
    assert_int_equal((int)uint16_dict_hash_size(d), 1);
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 7: update_uint16_dict_n
// ================================================================================
 
static void test_u16d_update_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal(update_uint16_dict_n(d, "k", 0, 1u), INVALID_ARG);
    return_uint16_dict(d);
}
 
static void test_u16d_update_n_updates_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    const char* buf = "port_http";
    insert_uint16_dict_n(d, buf, 4, 8080u, a);   /* key = "port" */
    assert_int_equal(update_uint16_dict_n(d, buf, 4, 443u), NO_ERROR);
    uint16_t v = 0u;
    assert_int_equal(get_uint16_dict_value_n(d, buf, 4, &v), NO_ERROR);
    assert_int_equal((int)v, 443);
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 8: get_uint16_dict_value
// ================================================================================
 
static void test_u16d_get_null_dict_returns_null_pointer(void** state) {
    (void)state;
    uint16_t v;
    assert_int_equal(get_uint16_dict_value(NULL, "k", &v), NULL_POINTER);
}
 
static void test_u16d_get_null_key_returns_null_pointer(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    uint16_t v;
    assert_int_equal(get_uint16_dict_value(d, NULL, &v), NULL_POINTER);
    return_uint16_dict(d);
}
 
static void test_u16d_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    insert_uint16_dict(d, "k", 1u, a);
    assert_int_equal(get_uint16_dict_value(d, "k", NULL), NULL_POINTER);
    return_uint16_dict(d);
}
 
static void test_u16d_get_missing_key_returns_not_found(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    uint16_t v;
    assert_int_equal(get_uint16_dict_value(d, "missing", &v), NOT_FOUND);
    return_uint16_dict(d);
}
 
static void test_u16d_get_retrieves_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    insert_uint16_dict(d, "min",  0u,         a);
    insert_uint16_dict(d, "max",  UINT16_MAX, a);
    insert_uint16_dict(d, "mid",  32768u,     a);
    uint16_t v = 0u;
    assert_int_equal(get_uint16_dict_value(d, "min", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    assert_int_equal(get_uint16_dict_value(d, "max", &v), NO_ERROR);
    assert_int_equal((int)v, (int)UINT16_MAX);
    assert_int_equal(get_uint16_dict_value(d, "mid", &v), NO_ERROR);
    assert_int_equal((int)v, 32768);
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 9: get_uint16_dict_value_n
// ================================================================================
 
static void test_u16d_get_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    uint16_t v;
    assert_int_equal(get_uint16_dict_value_n(d, "k", 0, &v), INVALID_ARG);
    return_uint16_dict(d);
}
 
static void test_u16d_get_n_retrieves_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    const char* buf = "width_max";
    insert_uint16_dict_n(d, buf, 5, 1920u, a);   /* key = "width" */
    uint16_t v = 0u;
    assert_int_equal(get_uint16_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal((int)v, 1920);
    assert_int_equal(get_uint16_dict_value_n(d, buf, 9, &v), NOT_FOUND);
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 10: get_uint16_dict_ptr
// ================================================================================
 
static void test_u16d_ptr_null_dict_returns_null(void** state) {
    (void)state;
    assert_null(get_uint16_dict_ptr(NULL, "k"));
}
 
static void test_u16d_ptr_null_key_returns_null(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_null(get_uint16_dict_ptr(d, NULL));
    return_uint16_dict(d);
}
 
static void test_u16d_ptr_missing_returns_null(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_null(get_uint16_dict_ptr(d, "missing"));
    return_uint16_dict(d);
}
 
static void test_u16d_ptr_points_to_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    insert_uint16_dict(d, "port", 8080u, a);
    const uint16_t* p = get_uint16_dict_ptr(d, "port");
    assert_non_null(p);
    assert_int_equal((int)*p, 8080);
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 11: get_uint16_dict_ptr_n
// ================================================================================
 
static void test_u16d_ptr_n_zero_len_returns_null(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_null(get_uint16_dict_ptr_n(d, "k", 0));
    return_uint16_dict(d);
}
 
static void test_u16d_ptr_n_points_to_bounded_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    const char* buf = "port_http";
    insert_uint16_dict_n(d, buf, 4, 8080u, a);   /* key = "port" */
    const uint16_t* p = get_uint16_dict_ptr_n(d, buf, 4);
    assert_non_null(p);
    assert_int_equal((int)*p, 8080);
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 12: has_uint16_dict_key
// ================================================================================
 
static void test_u16d_has_null_dict_returns_false(void** state) {
    (void)state;
    assert_false(has_uint16_dict_key(NULL, "k"));
}
 
static void test_u16d_has_null_key_returns_false(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_false(has_uint16_dict_key(d, NULL));
    return_uint16_dict(d);
}
 
static void test_u16d_has_present_and_absent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    insert_uint16_dict(d, "present", 1u, a);
    assert_true(has_uint16_dict_key(d, "present"));
    assert_false(has_uint16_dict_key(d, "absent"));
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 13: has_uint16_dict_key_n
// ================================================================================
 
static void test_u16d_has_n_zero_len_returns_false(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_false(has_uint16_dict_key_n(d, "k", 0));
    return_uint16_dict(d);
}
 
static void test_u16d_has_n_distinguishes_by_length(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    insert_uint16_dict_n(d, "abc", 2, 10u, a);   /* key = "ab" */
    insert_uint16_dict_n(d, "abc", 3, 20u, a);   /* key = "abc" */
    assert_true(has_uint16_dict_key_n(d, "abc", 2));
    assert_true(has_uint16_dict_key_n(d, "abc", 3));
    assert_false(has_uint16_dict_key_n(d, "abc", 1));
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 14: clear_uint16_dict
// ================================================================================
 
static void test_u16d_clear_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_uint16_dict(NULL), NULL_POINTER);
}
 
static void test_u16d_clear_resets_and_is_reusable(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    insert_uint16_dict(d, "a", 100u, a);
    insert_uint16_dict(d, "b", 200u, a);
    assert_int_equal(clear_uint16_dict(d), NO_ERROR);
    assert_int_equal((int)uint16_dict_hash_size(d), 0);
    assert_true(is_uint16_dict_empty(d));
    assert_int_equal(insert_uint16_dict(d, "a", 999u, a), NO_ERROR);
    uint16_t v = 0u;
    assert_int_equal(get_uint16_dict_value(d, "a", &v), NO_ERROR);
    assert_int_equal((int)v, 999);
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 15: copy_uint16_dict
// ================================================================================
 
static void test_u16d_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_expect_t r = copy_uint16_dict(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_u16d_copy_is_independent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* orig = _make_uint16_dict(4);
    insert_uint16_dict(orig, "x", 1000u, a);
    insert_uint16_dict(orig, "y", 2000u, a);
 
    uint16_dict_expect_t cr = copy_uint16_dict(orig, a);
    assert_true(cr.has_value);
 
    update_uint16_dict(orig, "x", 9999u);
 
    uint16_t v = 0u;
    assert_int_equal(get_uint16_dict_value(cr.u.value, "x", &v), NO_ERROR);
    assert_int_equal((int)v, 1000);
    assert_int_equal((int)uint16_dict_hash_size(cr.u.value), 2);
 
    return_uint16_dict(cr.u.value);
    return_uint16_dict(orig);
}
 
// ================================================================================
// Group 16: merge_uint16_dict
// ================================================================================
 
static void test_u16d_merge_null_first_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    uint16_dict_expect_t r = merge_uint16_dict(NULL, d, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_uint16_dict(d);
}
 
static void test_u16d_merge_no_overwrite_keeps_first_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* da = _make_uint16_dict(4);
    uint16_dict_t* db = _make_uint16_dict(4);
    insert_uint16_dict(da, "shared", 100u,  a);
    insert_uint16_dict(db, "shared", 999u,  a);
    insert_uint16_dict(db, "new",    500u,  a);
 
    uint16_dict_expect_t mr = merge_uint16_dict(da, db, false, a);
    assert_true(mr.has_value);
    uint16_t v = 0u;
    assert_int_equal(get_uint16_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_int_equal((int)v, 100);
    assert_int_equal(get_uint16_dict_value(mr.u.value, "new", &v), NO_ERROR);
    assert_int_equal((int)v, 500);
 
    return_uint16_dict(mr.u.value);
    return_uint16_dict(da);
    return_uint16_dict(db);
}
 
static void test_u16d_merge_overwrite_uses_second_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* da = _make_uint16_dict(4);
    uint16_dict_t* db = _make_uint16_dict(4);
    insert_uint16_dict(da, "shared", 100u, a);
    insert_uint16_dict(db, "shared", 999u, a);
 
    uint16_dict_expect_t mr = merge_uint16_dict(da, db, true, a);
    assert_true(mr.has_value);
    uint16_t v = 0u;
    assert_int_equal(get_uint16_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_int_equal((int)v, 999);
 
    return_uint16_dict(mr.u.value);
    return_uint16_dict(da);
    return_uint16_dict(db);
}
 
// ================================================================================
// Group 17: foreach_uint16_dict
// ================================================================================
 
static void test_u16d_foreach_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(foreach_uint16_dict(NULL, _u16d_sum_iter, NULL),
                     NULL_POINTER);
}
 
static void test_u16d_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_int_equal(foreach_uint16_dict(d, NULL, NULL), NULL_POINTER);
    return_uint16_dict(d);
}
 
static void test_u16d_foreach_correct_sum(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    insert_uint16_dict(d, "a", 1000u, a);
    insert_uint16_dict(d, "b", 2000u, a);
    insert_uint16_dict(d, "c", 3000u, a);
    _u16d_iter_ctx_t ctx = { 0, 0u };
    assert_int_equal(foreach_uint16_dict(d, _u16d_sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 3);
    assert_int_equal((int)ctx.sum, 6000);
    return_uint16_dict(d);
}
 
static void test_u16d_foreach_delivers_uint16_max(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    insert_uint16_dict(d, "top", UINT16_MAX, a);
    _u16d_iter_ctx_t ctx = { 0, 0u };
    foreach_uint16_dict(d, _u16d_sum_iter, &ctx);
    assert_int_equal(ctx.count, 1);
    assert_int_equal((int)ctx.sum, (int)UINT16_MAX);
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 18: introspection
// ================================================================================
 
static void test_u16d_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint16_dict_size(NULL), 0);
}
 
static void test_u16d_hash_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint16_dict_hash_size(NULL), 0);
}
 
static void test_u16d_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint16_dict_alloc(NULL), 0);
}
 
static void test_u16d_is_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_uint16_dict_empty(NULL));
}
 
static void test_u16d_is_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_t* d = _make_uint16_dict(4);
    assert_true(is_uint16_dict_empty(d));
    insert_uint16_dict(d, "x", 1u, a);
    assert_false(is_uint16_dict_empty(d));
    pop_uint16_dict(d, "x", NULL);
    assert_true(is_uint16_dict_empty(d));
    return_uint16_dict(d);
}
 
// ================================================================================
// Group 19: growth stress
// ================================================================================
 
static void test_u16d_growth_values_survive_resize(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_dict_expect_t r = init_uint16_dict(2, true, a);
    assert_true(r.has_value);
    uint16_dict_t* d = r.u.value;
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        uint16_t val = (uint16_t)(i * 300u);   /* exercises > 255 values */
        assert_int_equal(insert_uint16_dict(d, key, val, a), NO_ERROR);
    }
    assert_int_equal((int)uint16_dict_hash_size(d), 200);
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        uint16_t expected = (uint16_t)(i * 300u);
        uint16_t v = 0u;
        assert_int_equal(get_uint16_dict_value(d, key, &v), NO_ERROR);
        assert_int_equal((int)v, (int)expected);
    }
 
    return_uint16_dict(d);
}
 
// ================================================================================
// main
// ================================================================================
 
const struct CMUnitTest test_uint16_dict[] = {

    /* Group 1: init / return */
    cmocka_unit_test(test_u16d_init_null_allocator_fails),
    cmocka_unit_test(test_u16d_init_zero_capacity_fails),
    cmocka_unit_test(test_u16d_init_data_size_is_two_bytes),
    cmocka_unit_test(test_u16d_init_dtype_is_uint16),
    cmocka_unit_test(test_u16d_return_null_is_safe),

    /* Group 2: insert */
    cmocka_unit_test(test_u16d_insert_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u16d_insert_null_key_returns_null_pointer),
    cmocka_unit_test(test_u16d_insert_duplicate_returns_invalid_arg),
    cmocka_unit_test(test_u16d_insert_stores_zero),
    cmocka_unit_test(test_u16d_insert_stores_max),
    cmocka_unit_test(test_u16d_insert_stores_midrange),

    /* Group 3: insert_n */
    cmocka_unit_test(test_u16d_insert_n_null_key_returns_null_pointer),
    cmocka_unit_test(test_u16d_insert_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u16d_insert_n_uses_only_specified_bytes),

    /* Group 4: pop */
    cmocka_unit_test(test_u16d_pop_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u16d_pop_null_key_returns_null_pointer),
    cmocka_unit_test(test_u16d_pop_missing_key_returns_not_found),
    cmocka_unit_test(test_u16d_pop_returns_correct_value),
    cmocka_unit_test(test_u16d_pop_null_out_value_discards_safely),

    /* Group 5: pop_n */
    cmocka_unit_test(test_u16d_pop_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u16d_pop_n_removes_bounded_key),

    /* Group 6: update */
    cmocka_unit_test(test_u16d_update_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u16d_update_null_key_returns_null_pointer),
    cmocka_unit_test(test_u16d_update_missing_key_returns_not_found),
    cmocka_unit_test(test_u16d_update_overwrites_value),

    /* Group 7: update_n */
    cmocka_unit_test(test_u16d_update_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u16d_update_n_updates_bounded_key),

    /* Group 8: get_value */
    cmocka_unit_test(test_u16d_get_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u16d_get_null_key_returns_null_pointer),
    cmocka_unit_test(test_u16d_get_null_out_returns_null_pointer),
    cmocka_unit_test(test_u16d_get_missing_key_returns_not_found),
    cmocka_unit_test(test_u16d_get_retrieves_extremes),

    /* Group 9: get_value_n */
    cmocka_unit_test(test_u16d_get_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u16d_get_n_retrieves_bounded_key),

    /* Group 10: get_ptr */
    cmocka_unit_test(test_u16d_ptr_null_dict_returns_null),
    cmocka_unit_test(test_u16d_ptr_null_key_returns_null),
    cmocka_unit_test(test_u16d_ptr_missing_returns_null),
    cmocka_unit_test(test_u16d_ptr_points_to_correct_value),

    /* Group 11: get_ptr_n */
    cmocka_unit_test(test_u16d_ptr_n_zero_len_returns_null),
    cmocka_unit_test(test_u16d_ptr_n_points_to_bounded_value),

    /* Group 12: has_key */
    cmocka_unit_test(test_u16d_has_null_dict_returns_false),
    cmocka_unit_test(test_u16d_has_null_key_returns_false),
    cmocka_unit_test(test_u16d_has_present_and_absent),

    /* Group 13: has_key_n */
    cmocka_unit_test(test_u16d_has_n_zero_len_returns_false),
    cmocka_unit_test(test_u16d_has_n_distinguishes_by_length),

    /* Group 14: clear */
    cmocka_unit_test(test_u16d_clear_null_returns_null_pointer),
    cmocka_unit_test(test_u16d_clear_resets_and_is_reusable),

    /* Group 15: copy */
    cmocka_unit_test(test_u16d_copy_null_src_fails),
    cmocka_unit_test(test_u16d_copy_is_independent),

    /* Group 16: merge */
    cmocka_unit_test(test_u16d_merge_null_first_fails),
    cmocka_unit_test(test_u16d_merge_no_overwrite_keeps_first_value),
    cmocka_unit_test(test_u16d_merge_overwrite_uses_second_value),

    /* Group 17: foreach */
    cmocka_unit_test(test_u16d_foreach_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u16d_foreach_null_fn_returns_null_pointer),
    cmocka_unit_test(test_u16d_foreach_correct_sum),
    cmocka_unit_test(test_u16d_foreach_delivers_uint16_max),

    /* Group 18: introspection */
    cmocka_unit_test(test_u16d_size_null_returns_zero),
    cmocka_unit_test(test_u16d_hash_size_null_returns_zero),
    cmocka_unit_test(test_u16d_alloc_null_returns_zero),
    cmocka_unit_test(test_u16d_is_empty_null_returns_true),
    cmocka_unit_test(test_u16d_is_empty_reflects_contents),

    /* Group 19: growth stress */
    cmocka_unit_test(test_u16d_growth_values_survive_resize),
};
const size_t test_uint16_dict_count = sizeof(test_uint16_dict) / sizeof(test_uint16_dict[0]);
// ================================================================================ 
// ================================================================================ 

static int16_dict_t* _make_int16_dict(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    int16_dict_expect_t r = init_int16_dict(cap, true, a);
    assert_true(r.has_value);
    return r.u.value;
}
 
typedef struct { int count; int32_t sum; } _i16d_iter_ctx_t;
 
static void _i16d_sum_iter(const char* key, size_t key_len,
                            int16_t value, void* ud) {
    (void)key; (void)key_len;
    _i16d_iter_ctx_t* c = (_i16d_iter_ctx_t*)ud;
    c->count++;
    c->sum += (int32_t)value;
}
 
// ================================================================================
// Group 1: init_int16_dict / return_int16_dict
// ================================================================================
 
static void test_i16d_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    int16_dict_expect_t r = init_int16_dict(8, true, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_i16d_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_expect_t r = init_int16_dict(0, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_i16d_init_data_size_is_two_bytes(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal((int)dict_data_size(d), 2);
    return_int16_dict(d);
}
 
static void test_i16d_init_dtype_is_int16(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal((int)d->dtype, (int)INT16_TYPE);
    return_int16_dict(d);
}
 
static void test_i16d_return_null_is_safe(void** state) {
    (void)state;
    return_int16_dict(NULL);
}
 
// ================================================================================
// Group 2: insert_int16_dict
// ================================================================================
 
static void test_i16d_insert_null_dict_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    assert_int_equal(insert_int16_dict(NULL, "k", 1u, a), NULL_POINTER);
}
 
static void test_i16d_insert_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal(insert_int16_dict(d, NULL, 1u, a), NULL_POINTER);
    return_int16_dict(d);
}
 
static void test_i16d_insert_duplicate_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal(insert_int16_dict(d, "k", 1u, a), NO_ERROR);
    assert_int_equal(insert_int16_dict(d, "k", 2u, a), INVALID_ARG);
    assert_int_equal((int)int16_dict_hash_size(d), 1);
    return_int16_dict(d);
}
 
static void test_i16d_insert_stores_zero(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal(insert_int16_dict(d, "zero", 0, a), NO_ERROR);
    int16_t v = 99;
    assert_int_equal(get_int16_dict_value(d, "zero", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    return_int16_dict(d);
}
 
static void test_i16d_insert_stores_min_and_max(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal(insert_int16_dict(d, "min", INT16_MIN, a), NO_ERROR);
    assert_int_equal(insert_int16_dict(d, "max", INT16_MAX, a), NO_ERROR);
    int16_t v = 0;
    assert_int_equal(get_int16_dict_value(d, "min", &v), NO_ERROR);
    assert_int_equal((int)v, (int)INT16_MIN);
    assert_int_equal(get_int16_dict_value(d, "max", &v), NO_ERROR);
    assert_int_equal((int)v, (int)INT16_MAX);
    return_int16_dict(d);
}
 
static void test_i16d_insert_stores_negative(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal(insert_int16_dict(d, "temp", -273, a), NO_ERROR);
    int16_t v = 0;
    assert_int_equal(get_int16_dict_value(d, "temp", &v), NO_ERROR);
    assert_int_equal((int)v, -273);
    return_int16_dict(d);
}
 
// ================================================================================
// Group 3: insert_int16_dict_n
// ================================================================================
 
static void test_i16d_insert_n_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal(insert_int16_dict_n(d, NULL, 3, 1u, a), NULL_POINTER);
    return_int16_dict(d);
}
 
static void test_i16d_insert_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal(insert_int16_dict_n(d, "k", 0, 1u, a), INVALID_ARG);
    return_int16_dict(d);
}
 
static void test_i16d_insert_n_uses_only_specified_bytes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    /* "delta_fine" — insert only "delta" (5 bytes) with value -500 */
    const char* buf = "delta_fine";
    assert_int_equal(insert_int16_dict_n(d, buf, 5, -500, a), NO_ERROR);
    int16_t v = 0;
    assert_int_equal(get_int16_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal((int)v, -500);
    /* Full buffer key "delta_fine" is different — not found */
    assert_int_equal(get_int16_dict_value_n(d, buf, 10, &v), NOT_FOUND);
    /* Plain null-terminated lookup also works */
    assert_int_equal(get_int16_dict_value(d, "delta", &v), NO_ERROR);
    assert_int_equal((int)v, -500);
    return_int16_dict(d);
}
 
// ================================================================================
// Group 4: pop_int16_dict
// ================================================================================
 
static void test_i16d_pop_null_dict_returns_null_pointer(void** state) {
    (void)state;
    int16_t v;
    assert_int_equal(pop_int16_dict(NULL, "k", &v), NULL_POINTER);
}
 
static void test_i16d_pop_null_key_returns_null_pointer(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    int16_t v;
    assert_int_equal(pop_int16_dict(d, NULL, &v), NULL_POINTER);
    return_int16_dict(d);
}
 
static void test_i16d_pop_missing_key_returns_not_found(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal(pop_int16_dict(d, "missing", NULL), NOT_FOUND);
    return_int16_dict(d);
}
 
static void test_i16d_pop_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    insert_int16_dict(d, "low", -1000, a);
    int16_t v = 0;
    assert_int_equal(pop_int16_dict(d, "low", &v), NO_ERROR);
    assert_int_equal((int)v, -1000);
    assert_int_equal((int)int16_dict_hash_size(d), 0);
    return_int16_dict(d);
}
 
static void test_i16d_pop_null_out_value_discards_safely(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    insert_int16_dict(d, "x", -1, a);
    assert_int_equal(pop_int16_dict(d, "x", NULL), NO_ERROR);
    assert_int_equal((int)int16_dict_hash_size(d), 0);
    return_int16_dict(d);
}
 
// ================================================================================
// Group 5: pop_int16_dict_n
// ================================================================================
 
static void test_i16d_pop_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal(pop_int16_dict_n(d, "k", 0, NULL), INVALID_ARG);
    return_int16_dict(d);
}
 
static void test_i16d_pop_n_removes_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    const char* buf = "delta_fine";
    insert_int16_dict_n(d, buf, 5, -500, a);   /* key = "delta" */
    int16_t v = 0;
    assert_int_equal(pop_int16_dict_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal((int)v, -500);
    assert_int_equal((int)int16_dict_hash_size(d), 0);
    return_int16_dict(d);
}
 
// ================================================================================
// Group 6: update_int16_dict
// ================================================================================
 
static void test_i16d_update_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(update_int16_dict(NULL, "k", 1u), NULL_POINTER);
}
 
static void test_i16d_update_null_key_returns_null_pointer(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal(update_int16_dict(d, NULL, 1u), NULL_POINTER);
    return_int16_dict(d);
}
 
static void test_i16d_update_missing_key_returns_not_found(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal(update_int16_dict(d, "missing", 1u), NOT_FOUND);
    return_int16_dict(d);
}
 
static void test_i16d_update_overwrites_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    insert_int16_dict(d, "offset", 100, a);
    assert_int_equal(update_int16_dict(d, "offset", -100), NO_ERROR);
    int16_t v = 0;
    assert_int_equal(get_int16_dict_value(d, "offset", &v), NO_ERROR);
    assert_int_equal((int)v, -100);
    assert_int_equal((int)int16_dict_hash_size(d), 1);
    return_int16_dict(d);
}
 
// ================================================================================
// Group 7: update_int16_dict_n
// ================================================================================
 
static void test_i16d_update_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal(update_int16_dict_n(d, "k", 0, 1u), INVALID_ARG);
    return_int16_dict(d);
}
 
static void test_i16d_update_n_updates_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    const char* buf = "offset_fine";
    insert_int16_dict_n(d, buf, 6, 100, a);   /* key = "offset" */
    assert_int_equal(update_int16_dict_n(d, buf, 6, -100), NO_ERROR);
    int16_t v = 0;
    assert_int_equal(get_int16_dict_value_n(d, buf, 6, &v), NO_ERROR);
    assert_int_equal((int)v, -100);
    return_int16_dict(d);
}
 
// ================================================================================
// Group 8: get_int16_dict_value
// ================================================================================
 
static void test_i16d_get_null_dict_returns_null_pointer(void** state) {
    (void)state;
    int16_t v;
    assert_int_equal(get_int16_dict_value(NULL, "k", &v), NULL_POINTER);
}
 
static void test_i16d_get_null_key_returns_null_pointer(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    int16_t v;
    assert_int_equal(get_int16_dict_value(d, NULL, &v), NULL_POINTER);
    return_int16_dict(d);
}
 
static void test_i16d_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    insert_int16_dict(d, "k", 1, a);
    assert_int_equal(get_int16_dict_value(d, "k", NULL), NULL_POINTER);
    return_int16_dict(d);
}
 
static void test_i16d_get_missing_key_returns_not_found(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    int16_t v;
    assert_int_equal(get_int16_dict_value(d, "missing", &v), NOT_FOUND);
    return_int16_dict(d);
}
 
static void test_i16d_get_retrieves_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    insert_int16_dict(d, "min",  INT16_MIN, a);
    insert_int16_dict(d, "max",  INT16_MAX, a);
    insert_int16_dict(d, "neg",  -273,      a);
    int16_t v = 0;
    assert_int_equal(get_int16_dict_value(d, "min", &v), NO_ERROR);
    assert_int_equal((int)v, (int)INT16_MIN);
    assert_int_equal(get_int16_dict_value(d, "max", &v), NO_ERROR);
    assert_int_equal((int)v, (int)INT16_MAX);
    assert_int_equal(get_int16_dict_value(d, "neg", &v), NO_ERROR);
    assert_int_equal((int)v, -273);
    return_int16_dict(d);
}
 
// ================================================================================
// Group 9: get_int16_dict_value_n
// ================================================================================
 
static void test_i16d_get_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    int16_t v;
    assert_int_equal(get_int16_dict_value_n(d, "k", 0, &v), INVALID_ARG);
    return_int16_dict(d);
}
 
static void test_i16d_get_n_retrieves_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    const char* buf = "delta_fine";
    insert_int16_dict_n(d, buf, 5, -500, a);   /* key = "delta" */
    int16_t v = 0;
    assert_int_equal(get_int16_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal((int)v, -500);
    assert_int_equal(get_int16_dict_value_n(d, buf, 10, &v), NOT_FOUND);
    return_int16_dict(d);
}
 
// ================================================================================
// Group 10: get_int16_dict_ptr
// ================================================================================
 
static void test_i16d_ptr_null_dict_returns_null(void** state) {
    (void)state;
    assert_null(get_int16_dict_ptr(NULL, "k"));
}
 
static void test_i16d_ptr_null_key_returns_null(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    assert_null(get_int16_dict_ptr(d, NULL));
    return_int16_dict(d);
}
 
static void test_i16d_ptr_missing_returns_null(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    assert_null(get_int16_dict_ptr(d, "missing"));
    return_int16_dict(d);
}
 
static void test_i16d_ptr_points_to_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    insert_int16_dict(d, "temp", -273, a);
    const int16_t* p = get_int16_dict_ptr(d, "temp");
    assert_non_null(p);
    assert_int_equal((int)*p, -273);
    return_int16_dict(d);
}
 
// ================================================================================
// Group 11: get_int16_dict_ptr_n
// ================================================================================
 
static void test_i16d_ptr_n_zero_len_returns_null(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    assert_null(get_int16_dict_ptr_n(d, "k", 0));
    return_int16_dict(d);
}
 
static void test_i16d_ptr_n_points_to_bounded_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    const char* buf = "delta_fine";
    insert_int16_dict_n(d, buf, 5, -500, a);   /* key = "delta" */
    const int16_t* p = get_int16_dict_ptr_n(d, buf, 5);
    assert_non_null(p);
    assert_int_equal((int)*p, -500);
    return_int16_dict(d);
}
 
// ================================================================================
// Group 12: has_int16_dict_key
// ================================================================================
 
static void test_i16d_has_null_dict_returns_false(void** state) {
    (void)state;
    assert_false(has_int16_dict_key(NULL, "k"));
}
 
static void test_i16d_has_null_key_returns_false(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    assert_false(has_int16_dict_key(d, NULL));
    return_int16_dict(d);
}
 
static void test_i16d_has_present_and_absent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    insert_int16_dict(d, "present", -1, a);
    assert_true(has_int16_dict_key(d, "present"));
    assert_false(has_int16_dict_key(d, "absent"));
    return_int16_dict(d);
}
 
// ================================================================================
// Group 13: has_int16_dict_key_n
// ================================================================================
 
static void test_i16d_has_n_zero_len_returns_false(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    assert_false(has_int16_dict_key_n(d, "k", 0));
    return_int16_dict(d);
}
 
static void test_i16d_has_n_distinguishes_by_length(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    insert_int16_dict_n(d, "abc", 2, -10, a);   /* key = "ab" */
    insert_int16_dict_n(d, "abc", 3, -20, a);   /* key = "abc" */
    assert_true(has_int16_dict_key_n(d, "abc", 2));
    assert_true(has_int16_dict_key_n(d, "abc", 3));
    assert_false(has_int16_dict_key_n(d, "abc", 1));
    return_int16_dict(d);
}
 
// ================================================================================
// Group 14: clear_int16_dict
// ================================================================================
 
static void test_i16d_clear_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_int16_dict(NULL), NULL_POINTER);
}
 
static void test_i16d_clear_resets_and_is_reusable(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    insert_int16_dict(d, "a", -100, a);
    insert_int16_dict(d, "b",  200, a);
    assert_int_equal(clear_int16_dict(d), NO_ERROR);
    assert_int_equal((int)int16_dict_hash_size(d), 0);
    assert_true(is_int16_dict_empty(d));
    assert_int_equal(insert_int16_dict(d, "a", -999, a), NO_ERROR);
    int16_t v = 0;
    assert_int_equal(get_int16_dict_value(d, "a", &v), NO_ERROR);
    assert_int_equal((int)v, -999);
    return_int16_dict(d);
}
 
// ================================================================================
// Group 15: copy_int16_dict
// ================================================================================
 
static void test_i16d_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_expect_t r = copy_int16_dict(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_i16d_copy_is_independent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* orig = _make_int16_dict(4);
    insert_int16_dict(orig, "x", -1000, a);
    insert_int16_dict(orig, "y",  2000, a);
 
    int16_dict_expect_t cr = copy_int16_dict(orig, a);
    assert_true(cr.has_value);
 
    update_int16_dict(orig, "x", 9999);
 
    int16_t v = 0;
    assert_int_equal(get_int16_dict_value(cr.u.value, "x", &v), NO_ERROR);
    assert_int_equal((int)v, -1000);
    assert_int_equal((int)int16_dict_hash_size(cr.u.value), 2);
 
    return_int16_dict(cr.u.value);
    return_int16_dict(orig);
}
 
// ================================================================================
// Group 16: merge_int16_dict
// ================================================================================
 
static void test_i16d_merge_null_first_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    int16_dict_expect_t r = merge_int16_dict(NULL, d, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_int16_dict(d);
}
 
static void test_i16d_merge_no_overwrite_keeps_first_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* da = _make_int16_dict(4);
    int16_dict_t* db = _make_int16_dict(4);
    insert_int16_dict(da, "shared", -100,  a);
    insert_int16_dict(db, "shared",  999,  a);
    insert_int16_dict(db, "new",    -500,  a);
 
    int16_dict_expect_t mr = merge_int16_dict(da, db, false, a);
    assert_true(mr.has_value);
    int16_t v = 0;
    assert_int_equal(get_int16_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_int_equal((int)v, -100);
    assert_int_equal(get_int16_dict_value(mr.u.value, "new", &v), NO_ERROR);
    assert_int_equal((int)v, -500);
 
    return_int16_dict(mr.u.value);
    return_int16_dict(da);
    return_int16_dict(db);
}
 
static void test_i16d_merge_overwrite_uses_second_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* da = _make_int16_dict(4);
    int16_dict_t* db = _make_int16_dict(4);
    insert_int16_dict(da, "shared", -100, a);
    insert_int16_dict(db, "shared", -999, a);
 
    int16_dict_expect_t mr = merge_int16_dict(da, db, true, a);
    assert_true(mr.has_value);
    int16_t v = 0;
    assert_int_equal(get_int16_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_int_equal((int)v, -999);
 
    return_int16_dict(mr.u.value);
    return_int16_dict(da);
    return_int16_dict(db);
}
 
// ================================================================================
// Group 17: foreach_int16_dict
// ================================================================================
 
static void test_i16d_foreach_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(foreach_int16_dict(NULL, _i16d_sum_iter, NULL),
                     NULL_POINTER);
}
 
static void test_i16d_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    int16_dict_t* d = _make_int16_dict(4);
    assert_int_equal(foreach_int16_dict(d, NULL, NULL), NULL_POINTER);
    return_int16_dict(d);
}
 
static void test_i16d_foreach_correct_sum(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    insert_int16_dict(d, "a", -1000, a);
    insert_int16_dict(d, "b",  2000, a);
    insert_int16_dict(d, "c", -3000, a);
    _i16d_iter_ctx_t ctx = { 0, 0 };
    assert_int_equal(foreach_int16_dict(d, _i16d_sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 3);
    assert_int_equal((int)ctx.sum, -2000);
    return_int16_dict(d);
}
 
static void test_i16d_foreach_delivers_signed_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    insert_int16_dict(d, "min", INT16_MIN, a);
    insert_int16_dict(d, "max", INT16_MAX, a);
    _i16d_iter_ctx_t ctx = { 0, 0 };
    foreach_int16_dict(d, _i16d_sum_iter, &ctx);
    assert_int_equal(ctx.count, 2);
    /* INT16_MIN + INT16_MAX == -32768 + 32767 == -1 */
    assert_int_equal((int)ctx.sum, -1);
    return_int16_dict(d);
}
 
// ================================================================================
// Group 18: introspection
// ================================================================================
 
static void test_i16d_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int16_dict_size(NULL), 0);
}
 
static void test_i16d_hash_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int16_dict_hash_size(NULL), 0);
}
 
static void test_i16d_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int16_dict_alloc(NULL), 0);
}
 
static void test_i16d_is_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_int16_dict_empty(NULL));
}
 
static void test_i16d_is_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_t* d = _make_int16_dict(4);
    assert_true(is_int16_dict_empty(d));
    insert_int16_dict(d, "x", 1u, a);
    assert_false(is_int16_dict_empty(d));
    pop_int16_dict(d, "x", NULL);
    assert_true(is_int16_dict_empty(d));
    return_int16_dict(d);
}
 
// ================================================================================
// Group 19: growth stress
// ================================================================================
 
static void test_i16d_growth_values_survive_resize(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_dict_expect_t r = init_int16_dict(2, true, a);
    assert_true(r.has_value);
    int16_dict_t* d = r.u.value;
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        int16_t val = (int16_t)(i * 300u);   /* exercises > 255 values */
        assert_int_equal(insert_int16_dict(d, key, val, a), NO_ERROR);
    }
    assert_int_equal((int)int16_dict_hash_size(d), 200);
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        int16_t expected = (int16_t)(i * 300u);
        int16_t v = 0u;
        assert_int_equal(get_int16_dict_value(d, key, &v), NO_ERROR);
        assert_int_equal((int)v, (int)expected);
    }
 
    return_int16_dict(d);
}
 
// ================================================================================
// main
// ================================================================================
 
const struct CMUnitTest test_int16_dict[] = {

    /* Group 1: init / return */
    cmocka_unit_test(test_i16d_init_null_allocator_fails),
    cmocka_unit_test(test_i16d_init_zero_capacity_fails),
    cmocka_unit_test(test_i16d_init_data_size_is_two_bytes),
    cmocka_unit_test(test_i16d_init_dtype_is_int16),
    cmocka_unit_test(test_i16d_return_null_is_safe),

    /* Group 2: insert */
    cmocka_unit_test(test_i16d_insert_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i16d_insert_null_key_returns_null_pointer),
    cmocka_unit_test(test_i16d_insert_duplicate_returns_invalid_arg),
    cmocka_unit_test(test_i16d_insert_stores_zero),
    cmocka_unit_test(test_i16d_insert_stores_min_and_max),
    cmocka_unit_test(test_i16d_insert_stores_negative),

    /* Group 3: insert_n */
    cmocka_unit_test(test_i16d_insert_n_null_key_returns_null_pointer),
    cmocka_unit_test(test_i16d_insert_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i16d_insert_n_uses_only_specified_bytes),

    /* Group 4: pop */
    cmocka_unit_test(test_i16d_pop_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i16d_pop_null_key_returns_null_pointer),
    cmocka_unit_test(test_i16d_pop_missing_key_returns_not_found),
    cmocka_unit_test(test_i16d_pop_returns_correct_value),
    cmocka_unit_test(test_i16d_pop_null_out_value_discards_safely),

    /* Group 5: pop_n */
    cmocka_unit_test(test_i16d_pop_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i16d_pop_n_removes_bounded_key),

    /* Group 6: update */
    cmocka_unit_test(test_i16d_update_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i16d_update_null_key_returns_null_pointer),
    cmocka_unit_test(test_i16d_update_missing_key_returns_not_found),
    cmocka_unit_test(test_i16d_update_overwrites_value),

    /* Group 7: update_n */
    cmocka_unit_test(test_i16d_update_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i16d_update_n_updates_bounded_key),

    /* Group 8: get_value */
    cmocka_unit_test(test_i16d_get_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i16d_get_null_key_returns_null_pointer),
    cmocka_unit_test(test_i16d_get_null_out_returns_null_pointer),
    cmocka_unit_test(test_i16d_get_missing_key_returns_not_found),
    cmocka_unit_test(test_i16d_get_retrieves_extremes),

    /* Group 9: get_value_n */
    cmocka_unit_test(test_i16d_get_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i16d_get_n_retrieves_bounded_key),

    /* Group 10: get_ptr */
    cmocka_unit_test(test_i16d_ptr_null_dict_returns_null),
    cmocka_unit_test(test_i16d_ptr_null_key_returns_null),
    cmocka_unit_test(test_i16d_ptr_missing_returns_null),
    cmocka_unit_test(test_i16d_ptr_points_to_correct_value),

    /* Group 11: get_ptr_n */
    cmocka_unit_test(test_i16d_ptr_n_zero_len_returns_null),
    cmocka_unit_test(test_i16d_ptr_n_points_to_bounded_value),

    /* Group 12: has_key */
    cmocka_unit_test(test_i16d_has_null_dict_returns_false),
    cmocka_unit_test(test_i16d_has_null_key_returns_false),
    cmocka_unit_test(test_i16d_has_present_and_absent),

    /* Group 13: has_key_n */
    cmocka_unit_test(test_i16d_has_n_zero_len_returns_false),
    cmocka_unit_test(test_i16d_has_n_distinguishes_by_length),

    /* Group 14: clear */
    cmocka_unit_test(test_i16d_clear_null_returns_null_pointer),
    cmocka_unit_test(test_i16d_clear_resets_and_is_reusable),

    /* Group 15: copy */
    cmocka_unit_test(test_i16d_copy_null_src_fails),
    cmocka_unit_test(test_i16d_copy_is_independent),

    /* Group 16: merge */
    cmocka_unit_test(test_i16d_merge_null_first_fails),
    cmocka_unit_test(test_i16d_merge_no_overwrite_keeps_first_value),
    cmocka_unit_test(test_i16d_merge_overwrite_uses_second_value),

    /* Group 17: foreach */
    cmocka_unit_test(test_i16d_foreach_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i16d_foreach_null_fn_returns_null_pointer),
    cmocka_unit_test(test_i16d_foreach_correct_sum),
    cmocka_unit_test(test_i16d_foreach_delivers_signed_extremes),

    /* Group 18: introspection */
    cmocka_unit_test(test_i16d_size_null_returns_zero),
    cmocka_unit_test(test_i16d_hash_size_null_returns_zero),
    cmocka_unit_test(test_i16d_alloc_null_returns_zero),
    cmocka_unit_test(test_i16d_is_empty_null_returns_true),
    cmocka_unit_test(test_i16d_is_empty_reflects_contents),

    /* Group 19: growth stress */
    cmocka_unit_test(test_i16d_growth_values_survive_resize),

};


const size_t test_int16_dict_count = sizeof(test_int16_dict) / sizeof(test_int16_dict[0]);
// ================================================================================
// ================================================================================
// eof
