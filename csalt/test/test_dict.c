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
#include "c_uint32.h"
#include "c_int32.h"
#include "c_uint64.h"
#include "c_int64.h"
#include "c_float.h"
#include "c_double.h"
#include "c_ldouble.h"

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <float.h>
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

static uint32_dict_t* _make_uint32_dict(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    uint32_dict_expect_t r = init_uint32_dict(cap, true, a);
    assert_true(r.has_value);
    return r.u.value;
}
 
typedef struct { int count; uint64_t sum; } _u32d_iter_ctx_t;
 
static void _u32d_sum_iter(const char* key, size_t key_len,
                            uint32_t value, void* ud) {
    (void)key; (void)key_len;
    _u32d_iter_ctx_t* c = (_u32d_iter_ctx_t*)ud;
    c->count++;
    c->sum += (uint64_t)value;
}
 
// ================================================================================
// Group 1: init_uint32_dict / return_uint32_dict
// ================================================================================
 
static void test_u32d_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    uint32_dict_expect_t r = init_uint32_dict(8, true, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_u32d_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_expect_t r = init_uint32_dict(0, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_u32d_init_data_size_is_four_bytes(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal((int)dict_data_size(d), 4);
    return_uint32_dict(d);
}
 
static void test_u32d_init_dtype_is_uint32(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal((int)d->dtype, (int)UINT32_TYPE);
    return_uint32_dict(d);
}
 
static void test_u32d_return_null_is_safe(void** state) {
    (void)state;
    return_uint32_dict(NULL);
}
 
// ================================================================================
// Group 2: insert_uint32_dict
// ================================================================================
 
static void test_u32d_insert_null_dict_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    assert_int_equal(insert_uint32_dict(NULL, "k", 1u, a), NULL_POINTER);
}
 
static void test_u32d_insert_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal(insert_uint32_dict(d, NULL, 1u, a), NULL_POINTER);
    return_uint32_dict(d);
}
 
static void test_u32d_insert_duplicate_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal(insert_uint32_dict(d, "k", 1u, a), NO_ERROR);
    assert_int_equal(insert_uint32_dict(d, "k", 2u, a), INVALID_ARG);
    assert_int_equal((int)uint32_dict_hash_size(d), 1);
    return_uint32_dict(d);
}
 
static void test_u32d_insert_stores_zero(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal(insert_uint32_dict(d, "zero", 0u, a), NO_ERROR);
    uint32_t v = 99u;
    assert_int_equal(get_uint32_dict_value(d, "zero", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    return_uint32_dict(d);
}
 
static void test_u32d_insert_stores_max(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal(insert_uint32_dict(d, "max", UINT32_MAX, a), NO_ERROR);
    uint32_t v = 0u;
    assert_int_equal(get_uint32_dict_value(d, "max", &v), NO_ERROR);
    assert_int_equal((int)v, (int)UINT32_MAX);
    return_uint32_dict(d);
}
 
static void test_u32d_insert_stores_midrange(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal(insert_uint32_dict(d, "pixels", 2073600u, a), NO_ERROR);
    uint32_t v = 0u;
    assert_int_equal(get_uint32_dict_value(d, "pixels", &v), NO_ERROR);
    assert_int_equal((int)v, (int)2073600u);
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 3: insert_uint32_dict_n
// ================================================================================
 
static void test_u32d_insert_n_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal(insert_uint32_dict_n(d, NULL, 3, 1u, a), NULL_POINTER);
    return_uint32_dict(d);
}
 
static void test_u32d_insert_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal(insert_uint32_dict_n(d, "k", 0, 1u, a), INVALID_ARG);
    return_uint32_dict(d);
}
 
static void test_u32d_insert_n_uses_only_specified_bytes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    /* "count_total" — insert only "count" (5 bytes) with value 1000000 */
    const char* buf = "count_total";
    assert_int_equal(insert_uint32_dict_n(d, buf, 5, 1000000u, a), NO_ERROR);
    uint32_t v = 0u;
    assert_int_equal(get_uint32_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal((int)v, (int)1000000u);
    /* Full buffer key "count_total" is different — not found */
    assert_int_equal(get_uint32_dict_value_n(d, buf, 11, &v), NOT_FOUND);
    /* Plain null-terminated lookup also works */
    assert_int_equal(get_uint32_dict_value(d, "count", &v), NO_ERROR);
    assert_int_equal((int)v, (int)1000000u);
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 4: pop_uint32_dict
// ================================================================================
 
static void test_u32d_pop_null_dict_returns_null_pointer(void** state) {
    (void)state;
    uint32_t v;
    assert_int_equal(pop_uint32_dict(NULL, "k", &v), NULL_POINTER);
}
 
static void test_u32d_pop_null_key_returns_null_pointer(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    uint32_t v;
    assert_int_equal(pop_uint32_dict(d, NULL, &v), NULL_POINTER);
    return_uint32_dict(d);
}
 
static void test_u32d_pop_missing_key_returns_not_found(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal(pop_uint32_dict(d, "missing", NULL), NOT_FOUND);
    return_uint32_dict(d);
}
 
static void test_u32d_pop_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    insert_uint32_dict(d, "size", 4096u, a);
    uint32_t v = 0u;
    assert_int_equal(pop_uint32_dict(d, "size", &v), NO_ERROR);
    assert_int_equal((int)v, (int)4096u);
    assert_int_equal((int)uint32_dict_hash_size(d), 0);
    return_uint32_dict(d);
}
 
static void test_u32d_pop_null_out_value_discards_safely(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    insert_uint32_dict(d, "x", 100000u, a);
    assert_int_equal(pop_uint32_dict(d, "x", NULL), NO_ERROR);
    assert_int_equal((int)uint32_dict_hash_size(d), 0);
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 5: pop_uint32_dict_n
// ================================================================================
 
static void test_u32d_pop_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal(pop_uint32_dict_n(d, "k", 0, NULL), INVALID_ARG);
    return_uint32_dict(d);
}
 
static void test_u32d_pop_n_removes_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    const char* buf = "count_total";
    insert_uint32_dict_n(d, buf, 5, 1000000u, a);   /* key = "count" */
    uint32_t v = 0u;
    assert_int_equal(pop_uint32_dict_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal((int)v, (int)1000000u);
    assert_int_equal((int)uint32_dict_hash_size(d), 0);
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 6: update_uint32_dict
// ================================================================================
 
static void test_u32d_update_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(update_uint32_dict(NULL, "k", 1u), NULL_POINTER);
}
 
static void test_u32d_update_null_key_returns_null_pointer(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal(update_uint32_dict(d, NULL, 1u), NULL_POINTER);
    return_uint32_dict(d);
}
 
static void test_u32d_update_missing_key_returns_not_found(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal(update_uint32_dict(d, "missing", 1u), NOT_FOUND);
    return_uint32_dict(d);
}
 
static void test_u32d_update_overwrites_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    insert_uint32_dict(d, "size", 4096u, a);
    assert_int_equal(update_uint32_dict(d, "size", 8192u), NO_ERROR);
    uint32_t v = 0u;
    assert_int_equal(get_uint32_dict_value(d, "size", &v), NO_ERROR);
    assert_int_equal((int)v, (int)8192u);
    assert_int_equal((int)uint32_dict_hash_size(d), 1);
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 7: update_uint32_dict_n
// ================================================================================
 
static void test_u32d_update_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal(update_uint32_dict_n(d, "k", 0, 1u), INVALID_ARG);
    return_uint32_dict(d);
}
 
static void test_u32d_update_n_updates_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    const char* buf = "count_total";
    insert_uint32_dict_n(d, buf, 5, 4096u, a);   /* key = "count" */
    assert_int_equal(update_uint32_dict_n(d, buf, 5, 8192u), NO_ERROR);
    uint32_t v = 0u;
    assert_int_equal(get_uint32_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal((int)v, (int)8192u);
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 8: get_uint32_dict_value
// ================================================================================
 
static void test_u32d_get_null_dict_returns_null_pointer(void** state) {
    (void)state;
    uint32_t v;
    assert_int_equal(get_uint32_dict_value(NULL, "k", &v), NULL_POINTER);
}
 
static void test_u32d_get_null_key_returns_null_pointer(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    uint32_t v;
    assert_int_equal(get_uint32_dict_value(d, NULL, &v), NULL_POINTER);
    return_uint32_dict(d);
}
 
static void test_u32d_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    insert_uint32_dict(d, "k", 100000u, a);
    assert_int_equal(get_uint32_dict_value(d, "k", NULL), NULL_POINTER);
    return_uint32_dict(d);
}
 
static void test_u32d_get_missing_key_returns_not_found(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    uint32_t v;
    assert_int_equal(get_uint32_dict_value(d, "missing", &v), NOT_FOUND);
    return_uint32_dict(d);
}
 
static void test_u32d_get_retrieves_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    insert_uint32_dict(d, "min",  0u,         a);
    insert_uint32_dict(d, "max",  UINT32_MAX, a);
    insert_uint32_dict(d, "mid",  32768u,     a);
    uint32_t v = 0u;
    assert_int_equal(get_uint32_dict_value(d, "min", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    assert_int_equal(get_uint32_dict_value(d, "max", &v), NO_ERROR);
    assert_int_equal((int)v, (int)UINT32_MAX);
    assert_int_equal(get_uint32_dict_value(d, "mid", &v), NO_ERROR);
    assert_int_equal((int)v, 32768);
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 9: get_uint32_dict_value_n
// ================================================================================
 
static void test_u32d_get_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    uint32_t v;
    assert_int_equal(get_uint32_dict_value_n(d, "k", 0, &v), INVALID_ARG);
    return_uint32_dict(d);
}
 
static void test_u32d_get_n_retrieves_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    const char* buf = "count_total";
    insert_uint32_dict_n(d, buf, 5, 1000000u, a);   /* key = "count" */
    uint32_t v = 0u;
    assert_int_equal(get_uint32_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal((int)v, (int)1000000u);
    assert_int_equal(get_uint32_dict_value_n(d, buf, 11, &v), NOT_FOUND);
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 10: get_uint32_dict_ptr
// ================================================================================
 
static void test_u32d_ptr_null_dict_returns_null(void** state) {
    (void)state;
    assert_null(get_uint32_dict_ptr(NULL, "k"));
}
 
static void test_u32d_ptr_null_key_returns_null(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_null(get_uint32_dict_ptr(d, NULL));
    return_uint32_dict(d);
}
 
static void test_u32d_ptr_missing_returns_null(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_null(get_uint32_dict_ptr(d, "missing"));
    return_uint32_dict(d);
}
 
static void test_u32d_ptr_points_to_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    insert_uint32_dict(d, "flags", 0xDEADBEEFu, a);
    const uint32_t* p = get_uint32_dict_ptr(d, "flags");
    assert_non_null(p);
    assert_int_equal((int)(unsigned)*p, (int)(unsigned)0xDEADBEEFu);
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 11: get_uint32_dict_ptr_n
// ================================================================================
 
static void test_u32d_ptr_n_zero_len_returns_null(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_null(get_uint32_dict_ptr_n(d, "k", 0));
    return_uint32_dict(d);
}
 
static void test_u32d_ptr_n_points_to_bounded_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    const char* buf = "count_total";
    insert_uint32_dict_n(d, buf, 5, 1000000u, a);   /* key = "count" */
    const uint32_t* p = get_uint32_dict_ptr_n(d, buf, 5);
    assert_non_null(p);
    assert_int_equal((int)(unsigned)*p, (int)(unsigned)1000000u);
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 12: has_uint32_dict_key
// ================================================================================
 
static void test_u32d_has_null_dict_returns_false(void** state) {
    (void)state;
    assert_false(has_uint32_dict_key(NULL, "k"));
}
 
static void test_u32d_has_null_key_returns_false(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_false(has_uint32_dict_key(d, NULL));
    return_uint32_dict(d);
}
 
static void test_u32d_has_present_and_absent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    insert_uint32_dict(d, "present", 100000u, a);
    assert_true(has_uint32_dict_key(d, "present"));
    assert_false(has_uint32_dict_key(d, "absent"));
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 13: has_uint32_dict_key_n
// ================================================================================
 
static void test_u32d_has_n_zero_len_returns_false(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_false(has_uint32_dict_key_n(d, "k", 0));
    return_uint32_dict(d);
}
 
static void test_u32d_has_n_distinguishes_by_length(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    insert_uint32_dict_n(d, "abc", 2, 100000u, a);   /* key = "ab" */
    insert_uint32_dict_n(d, "abc", 3, 200000u, a);   /* key = "abc" */
    assert_true(has_uint32_dict_key_n(d, "abc", 2));
    assert_true(has_uint32_dict_key_n(d, "abc", 3));
    assert_false(has_uint32_dict_key_n(d, "abc", 1));
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 14: clear_uint32_dict
// ================================================================================
 
static void test_u32d_clear_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_uint32_dict(NULL), NULL_POINTER);
}
 
static void test_u32d_clear_resets_and_is_reusable(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    insert_uint32_dict(d, "a", 100000u, a);
    insert_uint32_dict(d, "b", 200000u, a);
    assert_int_equal(clear_uint32_dict(d), NO_ERROR);
    assert_int_equal((int)uint32_dict_hash_size(d), 0);
    assert_true(is_uint32_dict_empty(d));
    assert_int_equal(insert_uint32_dict(d, "a", 999999u, a), NO_ERROR);
    uint32_t v = 0u;
    assert_int_equal(get_uint32_dict_value(d, "a", &v), NO_ERROR);
    assert_int_equal((int)(unsigned)v, (int)(unsigned)999999u);
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 15: copy_uint32_dict
// ================================================================================
 
static void test_u32d_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_expect_t r = copy_uint32_dict(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_u32d_copy_is_independent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* orig = _make_uint32_dict(4);
    insert_uint32_dict(orig, "x", 1000000u, a);
    insert_uint32_dict(orig, "y", 2000000u, a);
 
    uint32_dict_expect_t cr = copy_uint32_dict(orig, a);
    assert_true(cr.has_value);
 
    update_uint32_dict(orig, "x", 0xFFFFFFFFu);
 
    uint32_t v = 0u;
    assert_int_equal(get_uint32_dict_value(cr.u.value, "x", &v), NO_ERROR);
    assert_int_equal((int)(unsigned)v, (int)(unsigned)1000000u);
    assert_int_equal((int)uint32_dict_hash_size(cr.u.value), 2);
 
    return_uint32_dict(cr.u.value);
    return_uint32_dict(orig);
}
 
// ================================================================================
// Group 16: merge_uint32_dict
// ================================================================================
 
static void test_u32d_merge_null_first_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    uint32_dict_expect_t r = merge_uint32_dict(NULL, d, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_uint32_dict(d);
}
 
static void test_u32d_merge_no_overwrite_keeps_first_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* da = _make_uint32_dict(4);
    uint32_dict_t* db = _make_uint32_dict(4);
    insert_uint32_dict(da, "shared", 1000000u, a);
    insert_uint32_dict(db, "shared", 9999999u, a);
    insert_uint32_dict(db, "new",    5000000u, a);
 
    uint32_dict_expect_t mr = merge_uint32_dict(da, db, false, a);
    assert_true(mr.has_value);
    uint32_t v = 0u;
    assert_int_equal(get_uint32_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_int_equal((int)(unsigned)v, (int)(unsigned)1000000u);
    assert_int_equal(get_uint32_dict_value(mr.u.value, "new", &v), NO_ERROR);
    assert_int_equal((int)(unsigned)v, (int)(unsigned)5000000u);
 
    return_uint32_dict(mr.u.value);
    return_uint32_dict(da);
    return_uint32_dict(db);
}
 
static void test_u32d_merge_overwrite_uses_second_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* da = _make_uint32_dict(4);
    uint32_dict_t* db = _make_uint32_dict(4);
    insert_uint32_dict(da, "shared", 1000000u, a);
    insert_uint32_dict(db, "shared", 9999999u, a);
 
    uint32_dict_expect_t mr = merge_uint32_dict(da, db, true, a);
    assert_true(mr.has_value);
    uint32_t v = 0u;
    assert_int_equal(get_uint32_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_int_equal((int)(unsigned)v, (int)(unsigned)9999999u);
 
    return_uint32_dict(mr.u.value);
    return_uint32_dict(da);
    return_uint32_dict(db);
}
 
// ================================================================================
// Group 17: foreach_uint32_dict
// ================================================================================
 
static void test_u32d_foreach_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(foreach_uint32_dict(NULL, _u32d_sum_iter, NULL),
                     NULL_POINTER);
}
 
static void test_u32d_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_int_equal(foreach_uint32_dict(d, NULL, NULL), NULL_POINTER);
    return_uint32_dict(d);
}
 
static void test_u32d_foreach_correct_sum(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    insert_uint32_dict(d, "a", 1000000u, a);
    insert_uint32_dict(d, "b", 2000000u, a);
    insert_uint32_dict(d, "c", 3000000u, a);
    _u32d_iter_ctx_t ctx = { 0, 0u };
    assert_int_equal(foreach_uint32_dict(d, _u32d_sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 3);
    assert_int_equal((int)(unsigned long long)ctx.sum, 6000000);
    return_uint32_dict(d);
}
 
static void test_u32d_foreach_delivers_uint32_max(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    insert_uint32_dict(d, "top", UINT32_MAX, a);
    _u32d_iter_ctx_t ctx = { 0, 0u };
    foreach_uint32_dict(d, _u32d_sum_iter, &ctx);
    assert_int_equal(ctx.count, 1);
    assert_int_equal((int)ctx.sum, (int)UINT32_MAX);
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 18: introspection
// ================================================================================
 
static void test_u32d_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint32_dict_size(NULL), 0);
}
 
static void test_u32d_hash_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint32_dict_hash_size(NULL), 0);
}
 
static void test_u32d_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint32_dict_alloc(NULL), 0);
}
 
static void test_u32d_is_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_uint32_dict_empty(NULL));
}
 
static void test_u32d_is_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_t* d = _make_uint32_dict(4);
    assert_true(is_uint32_dict_empty(d));
    insert_uint32_dict(d, "x", 1u, a);
    assert_false(is_uint32_dict_empty(d));
    pop_uint32_dict(d, "x", NULL);
    assert_true(is_uint32_dict_empty(d));
    return_uint32_dict(d);
}
 
// ================================================================================
// Group 19: growth stress
// ================================================================================
 
static void test_u32d_growth_values_survive_resize(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_dict_expect_t r = init_uint32_dict(2, true, a);
    assert_true(r.has_value);
    uint32_dict_t* d = r.u.value;
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        uint32_t val = (uint32_t)(i * 70000u);   /* exercises > UINT16_MAX values */
        assert_int_equal(insert_uint32_dict(d, key, val, a), NO_ERROR);
    }
    assert_int_equal((int)uint32_dict_hash_size(d), 200);
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        uint32_t expected = (uint32_t)(i * 70000u);
        uint32_t v = 0u;
        assert_int_equal(get_uint32_dict_value(d, key, &v), NO_ERROR);
        assert_int_equal((int)(unsigned)v, (int)(unsigned)expected);
    }
 
    return_uint32_dict(d);
}
 
// ================================================================================
// main
// ================================================================================
 
const struct CMUnitTest test_uint32_dict[] = {

    /* Group 1: init / return */
    cmocka_unit_test(test_u32d_init_null_allocator_fails),
    cmocka_unit_test(test_u32d_init_zero_capacity_fails),
    cmocka_unit_test(test_u32d_init_data_size_is_four_bytes),
    cmocka_unit_test(test_u32d_init_dtype_is_uint32),
    cmocka_unit_test(test_u32d_return_null_is_safe),

    /* Group 2: insert */
    cmocka_unit_test(test_u32d_insert_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u32d_insert_null_key_returns_null_pointer),
    cmocka_unit_test(test_u32d_insert_duplicate_returns_invalid_arg),
    cmocka_unit_test(test_u32d_insert_stores_zero),
    cmocka_unit_test(test_u32d_insert_stores_max),
    cmocka_unit_test(test_u32d_insert_stores_midrange),

    /* Group 3: insert_n */
    cmocka_unit_test(test_u32d_insert_n_null_key_returns_null_pointer),
    cmocka_unit_test(test_u32d_insert_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u32d_insert_n_uses_only_specified_bytes),

    /* Group 4: pop */
    cmocka_unit_test(test_u32d_pop_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u32d_pop_null_key_returns_null_pointer),
    cmocka_unit_test(test_u32d_pop_missing_key_returns_not_found),
    cmocka_unit_test(test_u32d_pop_returns_correct_value),
    cmocka_unit_test(test_u32d_pop_null_out_value_discards_safely),

    /* Group 5: pop_n */
    cmocka_unit_test(test_u32d_pop_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u32d_pop_n_removes_bounded_key),

    /* Group 6: update */
    cmocka_unit_test(test_u32d_update_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u32d_update_null_key_returns_null_pointer),
    cmocka_unit_test(test_u32d_update_missing_key_returns_not_found),
    cmocka_unit_test(test_u32d_update_overwrites_value),

    /* Group 7: update_n */
    cmocka_unit_test(test_u32d_update_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u32d_update_n_updates_bounded_key),

    /* Group 8: get_value */
    cmocka_unit_test(test_u32d_get_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u32d_get_null_key_returns_null_pointer),
    cmocka_unit_test(test_u32d_get_null_out_returns_null_pointer),
    cmocka_unit_test(test_u32d_get_missing_key_returns_not_found),
    cmocka_unit_test(test_u32d_get_retrieves_extremes),

    /* Group 9: get_value_n */
    cmocka_unit_test(test_u32d_get_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u32d_get_n_retrieves_bounded_key),

    /* Group 10: get_ptr */
    cmocka_unit_test(test_u32d_ptr_null_dict_returns_null),
    cmocka_unit_test(test_u32d_ptr_null_key_returns_null),
    cmocka_unit_test(test_u32d_ptr_missing_returns_null),
    cmocka_unit_test(test_u32d_ptr_points_to_correct_value),

    /* Group 11: get_ptr_n */
    cmocka_unit_test(test_u32d_ptr_n_zero_len_returns_null),
    cmocka_unit_test(test_u32d_ptr_n_points_to_bounded_value),

    /* Group 12: has_key */
    cmocka_unit_test(test_u32d_has_null_dict_returns_false),
    cmocka_unit_test(test_u32d_has_null_key_returns_false),
    cmocka_unit_test(test_u32d_has_present_and_absent),

    /* Group 13: has_key_n */
    cmocka_unit_test(test_u32d_has_n_zero_len_returns_false),
    cmocka_unit_test(test_u32d_has_n_distinguishes_by_length),

    /* Group 14: clear */
    cmocka_unit_test(test_u32d_clear_null_returns_null_pointer),
    cmocka_unit_test(test_u32d_clear_resets_and_is_reusable),

    /* Group 15: copy */
    cmocka_unit_test(test_u32d_copy_null_src_fails),
    cmocka_unit_test(test_u32d_copy_is_independent),

    /* Group 16: merge */
    cmocka_unit_test(test_u32d_merge_null_first_fails),
    cmocka_unit_test(test_u32d_merge_no_overwrite_keeps_first_value),
    cmocka_unit_test(test_u32d_merge_overwrite_uses_second_value),

    /* Group 17: foreach */
    cmocka_unit_test(test_u32d_foreach_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u32d_foreach_null_fn_returns_null_pointer),
    cmocka_unit_test(test_u32d_foreach_correct_sum),
    cmocka_unit_test(test_u32d_foreach_delivers_uint32_max),

    /* Group 18: introspection */
    cmocka_unit_test(test_u32d_size_null_returns_zero),
    cmocka_unit_test(test_u32d_hash_size_null_returns_zero),
    cmocka_unit_test(test_u32d_alloc_null_returns_zero),
    cmocka_unit_test(test_u32d_is_empty_null_returns_true),
    cmocka_unit_test(test_u32d_is_empty_reflects_contents),

    /* Group 19: growth stress */
    cmocka_unit_test(test_u32d_growth_values_survive_resize),

};
const size_t test_uint32_dict_count = sizeof(test_uint32_dict) / sizeof(test_uint32_dict[0]);
// ================================================================================ 
// ================================================================================ 


static int32_dict_t* _make_int32_dict(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    int32_dict_expect_t r = init_int32_dict(cap, true, a);
    assert_true(r.has_value);
    return r.u.value;
}
 
typedef struct { int count; int64_t sum; } _i32d_iter_ctx_t;
 
static void _i32d_sum_iter(const char* key, size_t key_len,
                            int32_t value, void* ud) {
    (void)key; (void)key_len;
    _i32d_iter_ctx_t* c = (_i32d_iter_ctx_t*)ud;
    c->count++;
    c->sum += (int64_t)value;
}
 
// ================================================================================
// Group 1: init_int32_dict / return_int32_dict
// ================================================================================
 
static void test_i32d_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    int32_dict_expect_t r = init_int32_dict(8, true, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_i32d_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_expect_t r = init_int32_dict(0, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_i32d_init_data_size_is_four_bytes(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal((int)dict_data_size(d), 4);
    return_int32_dict(d);
}
 
static void test_i32d_init_dtype_is_int32(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal((int)d->dtype, (int)INT32_TYPE);
    return_int32_dict(d);
}
 
static void test_i32d_return_null_is_safe(void** state) {
    (void)state;
    return_int32_dict(NULL);
}
 
// ================================================================================
// Group 2: insert_int32_dict
// ================================================================================
 
static void test_i32d_insert_null_dict_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    assert_int_equal(insert_int32_dict(NULL, "k", 1u, a), NULL_POINTER);
}
 
static void test_i32d_insert_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal(insert_int32_dict(d, NULL, 1u, a), NULL_POINTER);
    return_int32_dict(d);
}
 
static void test_i32d_insert_duplicate_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal(insert_int32_dict(d, "k", 1u, a), NO_ERROR);
    assert_int_equal(insert_int32_dict(d, "k", 2u, a), INVALID_ARG);
    assert_int_equal((int)int32_dict_hash_size(d), 1);
    return_int32_dict(d);
}
 
static void test_i32d_insert_stores_zero(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal(insert_int32_dict(d, "zero", 0, a), NO_ERROR);
    int32_t v = 99;
    assert_int_equal(get_int32_dict_value(d, "zero", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    return_int32_dict(d);
}
 
static void test_i32d_insert_stores_max(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal(insert_int32_dict(d, "max", INT32_MAX, a), NO_ERROR);
    int32_t v = 0u;
    assert_int_equal(get_int32_dict_value(d, "max", &v), NO_ERROR);
    assert_int_equal((int)v, (int)INT32_MAX);
    return_int32_dict(d);
}
 
static void test_i32d_insert_stores_midrange(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal(insert_int32_dict(d, "altitude", -10000, a), NO_ERROR);
    int32_t v = 0;
    assert_int_equal(get_int32_dict_value(d, "altitude", &v), NO_ERROR);
    assert_int_equal(v, -10000);
    return_int32_dict(d);
}
 
// ================================================================================
// Group 3: insert_int32_dict_n
// ================================================================================
 
static void test_i32d_insert_n_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal(insert_int32_dict_n(d, NULL, 3, 1u, a), NULL_POINTER);
    return_int32_dict(d);
}
 
static void test_i32d_insert_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal(insert_int32_dict_n(d, "k", 0, 1u, a), INVALID_ARG);
    return_int32_dict(d);
}
 
static void test_i32d_insert_n_uses_only_specified_bytes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    /* "delta_total" — insert only "delta" (5 bytes) with value -500000 */
    const char* buf = "delta_total";
    assert_int_equal(insert_int32_dict_n(d, buf, 5, -500000, a), NO_ERROR);
    int32_t v = 0;
    assert_int_equal(get_int32_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal(v, -500000);
    /* Full buffer key "delta_total" is different — not found */
    assert_int_equal(get_int32_dict_value_n(d, buf, 11, &v), NOT_FOUND);
    /* Plain null-terminated lookup also works */
    assert_int_equal(get_int32_dict_value(d, "delta", &v), NO_ERROR);
    assert_int_equal(v, -500000);
    return_int32_dict(d);
}
 
// ================================================================================
// Group 4: pop_int32_dict
// ================================================================================
 
static void test_i32d_pop_null_dict_returns_null_pointer(void** state) {
    (void)state;
    int32_t v;
    assert_int_equal(pop_int32_dict(NULL, "k", &v), NULL_POINTER);
}
 
static void test_i32d_pop_null_key_returns_null_pointer(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    int32_t v;
    assert_int_equal(pop_int32_dict(d, NULL, &v), NULL_POINTER);
    return_int32_dict(d);
}
 
static void test_i32d_pop_missing_key_returns_not_found(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal(pop_int32_dict(d, "missing", NULL), NOT_FOUND);
    return_int32_dict(d);
}
 
static void test_i32d_pop_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    insert_int32_dict(d, "low", -1000000, a);
    int32_t v = 0;
    assert_int_equal(pop_int32_dict(d, "low", &v), NO_ERROR);
    assert_int_equal(v, -1000000);
    assert_int_equal((int)int32_dict_hash_size(d), 0);
    return_int32_dict(d);
}
 
static void test_i32d_pop_null_out_value_discards_safely(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    insert_int32_dict(d, "x", -100000, a);
    assert_int_equal(pop_int32_dict(d, "x", NULL), NO_ERROR);
    assert_int_equal((int)int32_dict_hash_size(d), 0);
    return_int32_dict(d);
}
 
// ================================================================================
// Group 5: pop_int32_dict_n
// ================================================================================
 
static void test_i32d_pop_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal(pop_int32_dict_n(d, "k", 0, NULL), INVALID_ARG);
    return_int32_dict(d);
}
 
static void test_i32d_pop_n_removes_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    const char* buf = "delta_total";
    insert_int32_dict_n(d, buf, 5, -500000, a);   /* key = "delta" */
    int32_t v = 0;
    assert_int_equal(pop_int32_dict_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal(v, -500000);
    assert_int_equal((int)int32_dict_hash_size(d), 0);
    return_int32_dict(d);
}
 
// ================================================================================
// Group 6: update_int32_dict
// ================================================================================
 
static void test_i32d_update_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(update_int32_dict(NULL, "k", 1u), NULL_POINTER);
}
 
static void test_i32d_update_null_key_returns_null_pointer(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal(update_int32_dict(d, NULL, 1u), NULL_POINTER);
    return_int32_dict(d);
}
 
static void test_i32d_update_missing_key_returns_not_found(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal(update_int32_dict(d, "missing", 1u), NOT_FOUND);
    return_int32_dict(d);
}
 
static void test_i32d_update_overwrites_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    insert_int32_dict(d, "offset", 100000, a);
    assert_int_equal(update_int32_dict(d, "offset", -100000), NO_ERROR);
    int32_t v = 0;
    assert_int_equal(get_int32_dict_value(d, "offset", &v), NO_ERROR);
    assert_int_equal(v, -100000);
    assert_int_equal((int)int32_dict_hash_size(d), 1);
    return_int32_dict(d);
}
 
// ================================================================================
// Group 7: update_int32_dict_n
// ================================================================================
 
static void test_i32d_update_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal(update_int32_dict_n(d, "k", 0, 1u), INVALID_ARG);
    return_int32_dict(d);
}
 
static void test_i32d_update_n_updates_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    const char* buf = "delta_total";
    insert_int32_dict_n(d, buf, 5, 100000, a);   /* key = "delta" */
    assert_int_equal(update_int32_dict_n(d, buf, 5, -100000), NO_ERROR);
    int32_t v = 0;
    assert_int_equal(get_int32_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal(v, -100000);
    return_int32_dict(d);
}
 
// ================================================================================
// Group 8: get_int32_dict_value
// ================================================================================
 
static void test_i32d_get_null_dict_returns_null_pointer(void** state) {
    (void)state;
    int32_t v;
    assert_int_equal(get_int32_dict_value(NULL, "k", &v), NULL_POINTER);
}
 
static void test_i32d_get_null_key_returns_null_pointer(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    int32_t v;
    assert_int_equal(get_int32_dict_value(d, NULL, &v), NULL_POINTER);
    return_int32_dict(d);
}
 
static void test_i32d_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    insert_int32_dict(d, "k", -100000, a);
    assert_int_equal(get_int32_dict_value(d, "k", NULL), NULL_POINTER);
    return_int32_dict(d);
}
 
static void test_i32d_get_missing_key_returns_not_found(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    int32_t v;
    assert_int_equal(get_int32_dict_value(d, "missing", &v), NOT_FOUND);
    return_int32_dict(d);
}
 
static void test_i32d_get_retrieves_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    insert_int32_dict(d, "min",  0u,         a);
    insert_int32_dict(d, "max",  INT32_MAX, a);
    insert_int32_dict(d, "mid",  32768u,     a);
    int32_t v = 0u;
    assert_int_equal(get_int32_dict_value(d, "min", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    assert_int_equal(get_int32_dict_value(d, "max", &v), NO_ERROR);
    assert_int_equal((int)v, (int)INT32_MAX);
    assert_int_equal(get_int32_dict_value(d, "mid", &v), NO_ERROR);
    assert_int_equal((int)v, 32768);
    return_int32_dict(d);
}
 
// ================================================================================
// Group 9: get_int32_dict_value_n
// ================================================================================
 
static void test_i32d_get_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    int32_t v;
    assert_int_equal(get_int32_dict_value_n(d, "k", 0, &v), INVALID_ARG);
    return_int32_dict(d);
}
 
static void test_i32d_get_n_retrieves_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    const char* buf = "delta_total";
    insert_int32_dict_n(d, buf, 5, -500000, a);   /* key = "delta" */
    int32_t v = 0;
    assert_int_equal(get_int32_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_int_equal(v, -500000);
    assert_int_equal(get_int32_dict_value_n(d, buf, 11, &v), NOT_FOUND);
    return_int32_dict(d);
}
 
// ================================================================================
// Group 10: get_int32_dict_ptr
// ================================================================================
 
static void test_i32d_ptr_null_dict_returns_null(void** state) {
    (void)state;
    assert_null(get_int32_dict_ptr(NULL, "k"));
}
 
static void test_i32d_ptr_null_key_returns_null(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    assert_null(get_int32_dict_ptr(d, NULL));
    return_int32_dict(d);
}
 
static void test_i32d_ptr_missing_returns_null(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    assert_null(get_int32_dict_ptr(d, "missing"));
    return_int32_dict(d);
}
 
static void test_i32d_ptr_points_to_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    insert_int32_dict(d, "altitude", -10000, a);
    const int32_t* p = get_int32_dict_ptr(d, "altitude");
    assert_non_null(p);
    assert_int_equal(*p, -10000);
    return_int32_dict(d);
}
 
// ================================================================================
// Group 11: get_int32_dict_ptr_n
// ================================================================================
 
static void test_i32d_ptr_n_zero_len_returns_null(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    assert_null(get_int32_dict_ptr_n(d, "k", 0));
    return_int32_dict(d);
}
 
static void test_i32d_ptr_n_points_to_bounded_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    const char* buf = "delta_total";
    insert_int32_dict_n(d, buf, 5, -500000, a);   /* key = "delta" */
    const int32_t* p = get_int32_dict_ptr_n(d, buf, 5);
    assert_non_null(p);
    assert_int_equal(*p, -500000);
    return_int32_dict(d);
}
 
// ================================================================================
// Group 12: has_int32_dict_key
// ================================================================================
 
static void test_i32d_has_null_dict_returns_false(void** state) {
    (void)state;
    assert_false(has_int32_dict_key(NULL, "k"));
}
 
static void test_i32d_has_null_key_returns_false(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    assert_false(has_int32_dict_key(d, NULL));
    return_int32_dict(d);
}
 
static void test_i32d_has_present_and_absent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    insert_int32_dict(d, "present", -100000, a);
    assert_true(has_int32_dict_key(d, "present"));
    assert_false(has_int32_dict_key(d, "absent"));
    return_int32_dict(d);
}
 
// ================================================================================
// Group 13: has_int32_dict_key_n
// ================================================================================
 
static void test_i32d_has_n_zero_len_returns_false(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    assert_false(has_int32_dict_key_n(d, "k", 0));
    return_int32_dict(d);
}
 
static void test_i32d_has_n_distinguishes_by_length(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    insert_int32_dict_n(d, "abc", 2, -100000, a);   /* key = "ab" */
    insert_int32_dict_n(d, "abc", 3, -200000, a);   /* key = "abc" */
    assert_true(has_int32_dict_key_n(d, "abc", 2));
    assert_true(has_int32_dict_key_n(d, "abc", 3));
    assert_false(has_int32_dict_key_n(d, "abc", 1));
    return_int32_dict(d);
}
 
// ================================================================================
// Group 14: clear_int32_dict
// ================================================================================
 
static void test_i32d_clear_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_int32_dict(NULL), NULL_POINTER);
}
 
static void test_i32d_clear_resets_and_is_reusable(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    insert_int32_dict(d, "a", -100000, a);
    insert_int32_dict(d, "b",  200000, a);
    assert_int_equal(clear_int32_dict(d), NO_ERROR);
    assert_int_equal((int)int32_dict_hash_size(d), 0);
    assert_true(is_int32_dict_empty(d));
    assert_int_equal(insert_int32_dict(d, "a", -999999, a), NO_ERROR);
    int32_t v = 0;
    assert_int_equal(get_int32_dict_value(d, "a", &v), NO_ERROR);
    assert_int_equal(v, -999999);
    return_int32_dict(d);
}
 
// ================================================================================
// Group 15: copy_int32_dict
// ================================================================================
 
static void test_i32d_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_expect_t r = copy_int32_dict(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_i32d_copy_is_independent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* orig = _make_int32_dict(4);
    insert_int32_dict(orig, "x", -1000000, a);
    insert_int32_dict(orig, "y",  2000000, a);
 
    int32_dict_expect_t cr = copy_int32_dict(orig, a);
    assert_true(cr.has_value);
 
    update_int32_dict(orig, "x", INT32_MAX);
 
    int32_t v = 0;
    assert_int_equal(get_int32_dict_value(cr.u.value, "x", &v), NO_ERROR);
    assert_int_equal(v, -1000000);
    assert_int_equal((int)int32_dict_hash_size(cr.u.value), 2);
 
    return_int32_dict(cr.u.value);
    return_int32_dict(orig);
}
 
// ================================================================================
// Group 16: merge_int32_dict
// ================================================================================
 
static void test_i32d_merge_null_first_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    int32_dict_expect_t r = merge_int32_dict(NULL, d, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_int32_dict(d);
}
 
static void test_i32d_merge_no_overwrite_keeps_first_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* da = _make_int32_dict(4);
    int32_dict_t* db = _make_int32_dict(4);
    insert_int32_dict(da, "shared", -1000000, a);
    insert_int32_dict(db, "shared",  9999999, a);
    insert_int32_dict(db, "new",    -5000000, a);
 
    int32_dict_expect_t mr = merge_int32_dict(da, db, false, a);
    assert_true(mr.has_value);
    int32_t v = 0;
    assert_int_equal(get_int32_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_int_equal(v, -1000000);
    assert_int_equal(get_int32_dict_value(mr.u.value, "new", &v), NO_ERROR);
    assert_int_equal(v, -5000000);
 
    return_int32_dict(mr.u.value);
    return_int32_dict(da);
    return_int32_dict(db);
}
 
static void test_i32d_merge_overwrite_uses_second_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* da = _make_int32_dict(4);
    int32_dict_t* db = _make_int32_dict(4);
    insert_int32_dict(da, "shared", -1000000, a);
    insert_int32_dict(db, "shared", -9999999, a);
 
    int32_dict_expect_t mr = merge_int32_dict(da, db, true, a);
    assert_true(mr.has_value);
    int32_t v = 0;
    assert_int_equal(get_int32_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_int_equal(v, -9999999);
 
    return_int32_dict(mr.u.value);
    return_int32_dict(da);
    return_int32_dict(db);
}
 
// ================================================================================
// Group 17: foreach_int32_dict
// ================================================================================
 
static void test_i32d_foreach_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(foreach_int32_dict(NULL, _i32d_sum_iter, NULL),
                     NULL_POINTER);
}
 
static void test_i32d_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    int32_dict_t* d = _make_int32_dict(4);
    assert_int_equal(foreach_int32_dict(d, NULL, NULL), NULL_POINTER);
    return_int32_dict(d);
}
 
static void test_i32d_foreach_correct_sum(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    insert_int32_dict(d, "a", -1000000, a);
    insert_int32_dict(d, "b",  2000000, a);
    insert_int32_dict(d, "c", -3000000, a);
    _i32d_iter_ctx_t ctx = { 0, 0 };
    assert_int_equal(foreach_int32_dict(d, _i32d_sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 3);
    assert_int_equal((int)ctx.sum, -2000000);
    return_int32_dict(d);
}
 
static void test_i32d_foreach_delivers_signed_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    insert_int32_dict(d, "min", INT32_MIN, a);
    insert_int32_dict(d, "max", INT32_MAX, a);
    _i32d_iter_ctx_t ctx = { 0, 0 };
    foreach_int32_dict(d, _i32d_sum_iter, &ctx);
    assert_int_equal(ctx.count, 2);
    /* INT32_MIN + INT32_MAX == -2147483648 + 2147483647 == -1 */
    assert_int_equal((int)ctx.sum, -1);
    return_int32_dict(d);
}
 
// ================================================================================
// Group 18: introspection
// ================================================================================
 
static void test_i32d_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int32_dict_size(NULL), 0);
}
 
static void test_i32d_hash_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int32_dict_hash_size(NULL), 0);
}
 
static void test_i32d_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int32_dict_alloc(NULL), 0);
}
 
static void test_i32d_is_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_int32_dict_empty(NULL));
}
 
static void test_i32d_is_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_t* d = _make_int32_dict(4);
    assert_true(is_int32_dict_empty(d));
    insert_int32_dict(d, "x", 1u, a);
    assert_false(is_int32_dict_empty(d));
    pop_int32_dict(d, "x", NULL);
    assert_true(is_int32_dict_empty(d));
    return_int32_dict(d);
}
 
// ================================================================================
// Group 19: growth stress
// ================================================================================
 
static void test_i32d_growth_values_survive_resize(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_dict_expect_t r = init_int32_dict(2, true, a);
    assert_true(r.has_value);
    int32_dict_t* d = r.u.value;
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        int32_t val = (i % 2 == 0) ? -(i * 10000) : (i * 10000);
        assert_int_equal(insert_int32_dict(d, key, val, a), NO_ERROR);
    }
    assert_int_equal((int)int32_dict_hash_size(d), 200);
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        int32_t expected = (i % 2 == 0) ? -(i * 10000) : (i * 10000);
        int32_t v = 0u;
        assert_int_equal(get_int32_dict_value(d, key, &v), NO_ERROR);
        assert_int_equal(v, expected);
    }
 
    return_int32_dict(d);
}
 
// ================================================================================
// main
// ================================================================================
 
const struct CMUnitTest test_int32_dict[] = {

    /* Group 1: init / return */
    cmocka_unit_test(test_i32d_init_null_allocator_fails),
    cmocka_unit_test(test_i32d_init_zero_capacity_fails),
    cmocka_unit_test(test_i32d_init_data_size_is_four_bytes),
    cmocka_unit_test(test_i32d_init_dtype_is_int32),
    cmocka_unit_test(test_i32d_return_null_is_safe),

    /* Group 2: insert */
    cmocka_unit_test(test_i32d_insert_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i32d_insert_null_key_returns_null_pointer),
    cmocka_unit_test(test_i32d_insert_duplicate_returns_invalid_arg),
    cmocka_unit_test(test_i32d_insert_stores_zero),
    cmocka_unit_test(test_i32d_insert_stores_max),
    cmocka_unit_test(test_i32d_insert_stores_midrange),

    /* Group 3: insert_n */
    cmocka_unit_test(test_i32d_insert_n_null_key_returns_null_pointer),
    cmocka_unit_test(test_i32d_insert_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i32d_insert_n_uses_only_specified_bytes),

    /* Group 4: pop */
    cmocka_unit_test(test_i32d_pop_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i32d_pop_null_key_returns_null_pointer),
    cmocka_unit_test(test_i32d_pop_missing_key_returns_not_found),
    cmocka_unit_test(test_i32d_pop_returns_correct_value),
    cmocka_unit_test(test_i32d_pop_null_out_value_discards_safely),

    /* Group 5: pop_n */
    cmocka_unit_test(test_i32d_pop_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i32d_pop_n_removes_bounded_key),

    /* Group 6: update */
    cmocka_unit_test(test_i32d_update_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i32d_update_null_key_returns_null_pointer),
    cmocka_unit_test(test_i32d_update_missing_key_returns_not_found),
    cmocka_unit_test(test_i32d_update_overwrites_value),

    /* Group 7: update_n */
    cmocka_unit_test(test_i32d_update_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i32d_update_n_updates_bounded_key),

    /* Group 8: get_value */
    cmocka_unit_test(test_i32d_get_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i32d_get_null_key_returns_null_pointer),
    cmocka_unit_test(test_i32d_get_null_out_returns_null_pointer),
    cmocka_unit_test(test_i32d_get_missing_key_returns_not_found),
    cmocka_unit_test(test_i32d_get_retrieves_extremes),

    /* Group 9: get_value_n */
    cmocka_unit_test(test_i32d_get_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i32d_get_n_retrieves_bounded_key),

    /* Group 10: get_ptr */
    cmocka_unit_test(test_i32d_ptr_null_dict_returns_null),
    cmocka_unit_test(test_i32d_ptr_null_key_returns_null),
    cmocka_unit_test(test_i32d_ptr_missing_returns_null),
    cmocka_unit_test(test_i32d_ptr_points_to_correct_value),

    /* Group 11: get_ptr_n */
    cmocka_unit_test(test_i32d_ptr_n_zero_len_returns_null),
    cmocka_unit_test(test_i32d_ptr_n_points_to_bounded_value),

    /* Group 12: has_key */
    cmocka_unit_test(test_i32d_has_null_dict_returns_false),
    cmocka_unit_test(test_i32d_has_null_key_returns_false),
    cmocka_unit_test(test_i32d_has_present_and_absent),

    /* Group 13: has_key_n */
    cmocka_unit_test(test_i32d_has_n_zero_len_returns_false),
    cmocka_unit_test(test_i32d_has_n_distinguishes_by_length),

    /* Group 14: clear */
    cmocka_unit_test(test_i32d_clear_null_returns_null_pointer),
    cmocka_unit_test(test_i32d_clear_resets_and_is_reusable),

    /* Group 15: copy */
    cmocka_unit_test(test_i32d_copy_null_src_fails),
    cmocka_unit_test(test_i32d_copy_is_independent),

    /* Group 16: merge */
    cmocka_unit_test(test_i32d_merge_null_first_fails),
    cmocka_unit_test(test_i32d_merge_no_overwrite_keeps_first_value),
    cmocka_unit_test(test_i32d_merge_overwrite_uses_second_value),

    /* Group 17: foreach */
    cmocka_unit_test(test_i32d_foreach_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i32d_foreach_null_fn_returns_null_pointer),
    cmocka_unit_test(test_i32d_foreach_correct_sum),
    cmocka_unit_test(test_i32d_foreach_delivers_signed_extremes),

    /* Group 18: introspection */
    cmocka_unit_test(test_i32d_size_null_returns_zero),
    cmocka_unit_test(test_i32d_hash_size_null_returns_zero),
    cmocka_unit_test(test_i32d_alloc_null_returns_zero),
    cmocka_unit_test(test_i32d_is_empty_null_returns_true),
    cmocka_unit_test(test_i32d_is_empty_reflects_contents),

    /* Group 19: growth stress */
    cmocka_unit_test(test_i32d_growth_values_survive_resize),
};
const size_t test_int32_dict_count = sizeof(test_int32_dict) / sizeof(test_int32_dict[0]);
// ================================================================================ 
// ================================================================================ 

static uint64_dict_t* _make_uint64_dict(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    uint64_dict_expect_t r = init_uint64_dict(cap, true, a);
    assert_true(r.has_value);
    return r.u.value;
}
 
typedef struct { int count; unsigned long long sum; } _u64d_iter_ctx_t;
 
static void _u64d_sum_iter(const char* key, size_t key_len,
                            uint64_t value, void* ud) {
    (void)key; (void)key_len;
    _u64d_iter_ctx_t* c = (_u64d_iter_ctx_t*)ud;
    c->count++;
    c->sum += (unsigned long long)value;
}
 
// ================================================================================
// Group 1: init_uint64_dict / return_uint64_dict
// ================================================================================
 
static void test_u64d_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    uint64_dict_expect_t r = init_uint64_dict(8, true, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_u64d_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_expect_t r = init_uint64_dict(0, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_u64d_init_data_size_is_eight_bytes(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal((int)dict_data_size(d), 8);
    return_uint64_dict(d);
}
 
static void test_u64d_init_dtype_is_uint64(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal((int)d->dtype, (int)UINT64_TYPE);
    return_uint64_dict(d);
}
 
static void test_u64d_return_null_is_safe(void** state) {
    (void)state;
    return_uint64_dict(NULL);
}
 
// ================================================================================
// Group 2: insert_uint64_dict
// ================================================================================
 
static void test_u64d_insert_null_dict_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    assert_int_equal(insert_uint64_dict(NULL, "k", 1u, a), NULL_POINTER);
}
 
static void test_u64d_insert_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal(insert_uint64_dict(d, NULL, 1u, a), NULL_POINTER);
    return_uint64_dict(d);
}
 
static void test_u64d_insert_duplicate_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal(insert_uint64_dict(d, "k", 1u, a), NO_ERROR);
    assert_int_equal(insert_uint64_dict(d, "k", 2u, a), INVALID_ARG);
    assert_int_equal((int)uint64_dict_hash_size(d), 1);
    return_uint64_dict(d);
}
 
static void test_u64d_insert_stores_zero(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal(insert_uint64_dict(d, "zero", 0ULL, a), NO_ERROR);
    uint64_t v = 99ULL;
    assert_int_equal(get_uint64_dict_value(d, "zero", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    return_uint64_dict(d);
}
 
static void test_u64d_insert_stores_max(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal(insert_uint64_dict(d, "max", UINT64_MAX, a), NO_ERROR);
    uint64_t v = 0u;
    assert_int_equal(get_uint64_dict_value(d, "max", &v), NO_ERROR);
    assert_int_equal((int)v, (int)UINT64_MAX);
    return_uint64_dict(d);
}
 
static void test_u64d_insert_stores_midrange(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal(insert_uint64_dict(d, "size", 1099511627776ULL, a), NO_ERROR);
    uint64_t v = 0ULL;
    assert_int_equal(get_uint64_dict_value(d, "size", &v), NO_ERROR);
    assert_true(v == 1099511627776ULL);
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 3: insert_uint64_dict_n
// ================================================================================
 
static void test_u64d_insert_n_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal(insert_uint64_dict_n(d, NULL, 3, 1u, a), NULL_POINTER);
    return_uint64_dict(d);
}
 
static void test_u64d_insert_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal(insert_uint64_dict_n(d, "k", 0, 1u, a), INVALID_ARG);
    return_uint64_dict(d);
}
 
static void test_u64d_insert_n_uses_only_specified_bytes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    /* "bytes_total" — insert only "bytes" (5 bytes) with value 1 TiB */
    const char* buf = "bytes_total";
    assert_int_equal(insert_uint64_dict_n(d, buf, 5, 1099511627776ULL, a), NO_ERROR);
    uint64_t v = 0ULL;
    assert_int_equal(get_uint64_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 1099511627776ULL);
    /* Full buffer key "bytes_total" is different — not found */
    assert_int_equal(get_uint64_dict_value_n(d, buf, 11, &v), NOT_FOUND);
    /* Plain null-terminated lookup also works */
    assert_int_equal(get_uint64_dict_value(d, "bytes", &v), NO_ERROR);
    assert_true(v == 1099511627776ULL);
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 4: pop_uint64_dict
// ================================================================================
 
static void test_u64d_pop_null_dict_returns_null_pointer(void** state) {
    (void)state;
    uint64_t v;
    assert_int_equal(pop_uint64_dict(NULL, "k", &v), NULL_POINTER);
}
 
static void test_u64d_pop_null_key_returns_null_pointer(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    uint64_t v;
    assert_int_equal(pop_uint64_dict(d, NULL, &v), NULL_POINTER);
    return_uint64_dict(d);
}
 
static void test_u64d_pop_missing_key_returns_not_found(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal(pop_uint64_dict(d, "missing", NULL), NOT_FOUND);
    return_uint64_dict(d);
}
 
static void test_u64d_pop_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    insert_uint64_dict(d, "size", 1099511627776ULL, a);
    uint64_t v = 0ULL;
    assert_int_equal(pop_uint64_dict(d, "size", &v), NO_ERROR);
    assert_true(v == 1099511627776ULL);
    assert_int_equal((int)uint64_dict_hash_size(d), 0);
    return_uint64_dict(d);
}
 
static void test_u64d_pop_null_out_value_discards_safely(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    insert_uint64_dict(d, "x", 1099511627776ULL, a);
    assert_int_equal(pop_uint64_dict(d, "x", NULL), NO_ERROR);
    assert_int_equal((int)uint64_dict_hash_size(d), 0);
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 5: pop_uint64_dict_n
// ================================================================================
 
static void test_u64d_pop_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal(pop_uint64_dict_n(d, "k", 0, NULL), INVALID_ARG);
    return_uint64_dict(d);
}
 
static void test_u64d_pop_n_removes_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    const char* buf = "bytes_total";
    insert_uint64_dict_n(d, buf, 5, 1099511627776ULL, a);   /* key = "bytes" */
    uint64_t v = 0ULL;
    assert_int_equal(pop_uint64_dict_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 1099511627776ULL);
    assert_int_equal((int)uint64_dict_hash_size(d), 0);
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 6: update_uint64_dict
// ================================================================================
 
static void test_u64d_update_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(update_uint64_dict(NULL, "k", 1u), NULL_POINTER);
}
 
static void test_u64d_update_null_key_returns_null_pointer(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal(update_uint64_dict(d, NULL, 1u), NULL_POINTER);
    return_uint64_dict(d);
}
 
static void test_u64d_update_missing_key_returns_not_found(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal(update_uint64_dict(d, "missing", 1u), NOT_FOUND);
    return_uint64_dict(d);
}
 
static void test_u64d_update_overwrites_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    insert_uint64_dict(d, "size", 1099511627776ULL, a);
    assert_int_equal(update_uint64_dict(d, "size", 2199023255552ULL), NO_ERROR);
    uint64_t v = 0ULL;
    assert_int_equal(get_uint64_dict_value(d, "size", &v), NO_ERROR);
    assert_true(v == 2199023255552ULL);
    assert_int_equal((int)uint64_dict_hash_size(d), 1);
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 7: update_uint64_dict_n
// ================================================================================
 
static void test_u64d_update_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal(update_uint64_dict_n(d, "k", 0, 1u), INVALID_ARG);
    return_uint64_dict(d);
}
 
static void test_u64d_update_n_updates_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    const char* buf = "bytes_total";
    insert_uint64_dict_n(d, buf, 5, 1099511627776ULL, a);   /* key = "bytes" */
    assert_int_equal(update_uint64_dict_n(d, buf, 5, 2199023255552ULL), NO_ERROR);
    uint64_t v = 0ULL;
    assert_int_equal(get_uint64_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 2199023255552ULL);
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 8: get_uint64_dict_value
// ================================================================================
 
static void test_u64d_get_null_dict_returns_null_pointer(void** state) {
    (void)state;
    uint64_t v;
    assert_int_equal(get_uint64_dict_value(NULL, "k", &v), NULL_POINTER);
}
 
static void test_u64d_get_null_key_returns_null_pointer(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    uint64_t v;
    assert_int_equal(get_uint64_dict_value(d, NULL, &v), NULL_POINTER);
    return_uint64_dict(d);
}
 
static void test_u64d_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    insert_uint64_dict(d, "k", 1099511627776ULL, a);
    assert_int_equal(get_uint64_dict_value(d, "k", NULL), NULL_POINTER);
    return_uint64_dict(d);
}
 
static void test_u64d_get_missing_key_returns_not_found(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    uint64_t v;
    assert_int_equal(get_uint64_dict_value(d, "missing", &v), NOT_FOUND);
    return_uint64_dict(d);
}
 
static void test_u64d_get_retrieves_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    insert_uint64_dict(d, "min",  0u,         a);
    insert_uint64_dict(d, "max",  UINT64_MAX, a);
    insert_uint64_dict(d, "mid",  32768u,     a);
    uint64_t v = 0u;
    assert_int_equal(get_uint64_dict_value(d, "min", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    assert_int_equal(get_uint64_dict_value(d, "max", &v), NO_ERROR);
    assert_int_equal((int)v, (int)UINT64_MAX);
    assert_int_equal(get_uint64_dict_value(d, "mid", &v), NO_ERROR);
    assert_int_equal((int)v, 32768);
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 9: get_uint64_dict_value_n
// ================================================================================
 
static void test_u64d_get_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    uint64_t v;
    assert_int_equal(get_uint64_dict_value_n(d, "k", 0, &v), INVALID_ARG);
    return_uint64_dict(d);
}
 
static void test_u64d_get_n_retrieves_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    const char* buf = "bytes_total";
    insert_uint64_dict_n(d, buf, 5, 1099511627776ULL, a);   /* key = "bytes" */
    uint64_t v = 0ULL;
    assert_int_equal(get_uint64_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 1099511627776ULL);
    assert_int_equal(get_uint64_dict_value_n(d, buf, 11, &v), NOT_FOUND);
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 10: get_uint64_dict_ptr
// ================================================================================
 
static void test_u64d_ptr_null_dict_returns_null(void** state) {
    (void)state;
    assert_null(get_uint64_dict_ptr(NULL, "k"));
}
 
static void test_u64d_ptr_null_key_returns_null(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_null(get_uint64_dict_ptr(d, NULL));
    return_uint64_dict(d);
}
 
static void test_u64d_ptr_missing_returns_null(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_null(get_uint64_dict_ptr(d, "missing"));
    return_uint64_dict(d);
}
 
static void test_u64d_ptr_points_to_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    insert_uint64_dict(d, "hash", 0xDEADBEEFCAFEBABEULL, a);
    const uint64_t* p = get_uint64_dict_ptr(d, "hash");
    assert_non_null(p);
    assert_true(*p == 0xDEADBEEFCAFEBABEULL);
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 11: get_uint64_dict_ptr_n
// ================================================================================
 
static void test_u64d_ptr_n_zero_len_returns_null(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_null(get_uint64_dict_ptr_n(d, "k", 0));
    return_uint64_dict(d);
}
 
static void test_u64d_ptr_n_points_to_bounded_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    const char* buf = "bytes_total";
    insert_uint64_dict_n(d, buf, 5, 1099511627776ULL, a);   /* key = "bytes" */
    const uint64_t* p = get_uint64_dict_ptr_n(d, buf, 5);
    assert_non_null(p);
    assert_true(*p == 1099511627776ULL);
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 12: has_uint64_dict_key
// ================================================================================
 
static void test_u64d_has_null_dict_returns_false(void** state) {
    (void)state;
    assert_false(has_uint64_dict_key(NULL, "k"));
}
 
static void test_u64d_has_null_key_returns_false(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_false(has_uint64_dict_key(d, NULL));
    return_uint64_dict(d);
}
 
static void test_u64d_has_present_and_absent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    insert_uint64_dict(d, "present", 1099511627776ULL, a);
    assert_true(has_uint64_dict_key(d, "present"));
    assert_false(has_uint64_dict_key(d, "absent"));
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 13: has_uint64_dict_key_n
// ================================================================================
 
static void test_u64d_has_n_zero_len_returns_false(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_false(has_uint64_dict_key_n(d, "k", 0));
    return_uint64_dict(d);
}
 
static void test_u64d_has_n_distinguishes_by_length(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    insert_uint64_dict_n(d, "abc", 2, 1099511627776ULL, a);   /* key = "ab" */
    insert_uint64_dict_n(d, "abc", 3, 2199023255552ULL, a);   /* key = "abc" */
    assert_true(has_uint64_dict_key_n(d, "abc", 2));
    assert_true(has_uint64_dict_key_n(d, "abc", 3));
    assert_false(has_uint64_dict_key_n(d, "abc", 1));
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 14: clear_uint64_dict
// ================================================================================
 
static void test_u64d_clear_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_uint64_dict(NULL), NULL_POINTER);
}
 
static void test_u64d_clear_resets_and_is_reusable(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    insert_uint64_dict(d, "a", 1099511627776ULL, a);
    insert_uint64_dict(d, "b", 2199023255552ULL, a);
    assert_int_equal(clear_uint64_dict(d), NO_ERROR);
    assert_int_equal((int)uint64_dict_hash_size(d), 0);
    assert_true(is_uint64_dict_empty(d));
    assert_int_equal(insert_uint64_dict(d, "a", 3298534883328ULL, a), NO_ERROR);
    uint64_t v = 0ULL;
    assert_int_equal(get_uint64_dict_value(d, "a", &v), NO_ERROR);
    assert_true(v == 3298534883328ULL);
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 15: copy_uint64_dict
// ================================================================================
 
static void test_u64d_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_expect_t r = copy_uint64_dict(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_u64d_copy_is_independent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* orig = _make_uint64_dict(4);
    insert_uint64_dict(orig, "x", 1099511627776ULL, a);
    insert_uint64_dict(orig, "y", 2199023255552ULL, a);
 
    uint64_dict_expect_t cr = copy_uint64_dict(orig, a);
    assert_true(cr.has_value);
 
    update_uint64_dict(orig, "x", UINT64_MAX);
 
    uint64_t v = 0ULL;
    assert_int_equal(get_uint64_dict_value(cr.u.value, "x", &v), NO_ERROR);
    assert_true(v == 1099511627776ULL);
    assert_int_equal((int)uint64_dict_hash_size(cr.u.value), 2);
 
    return_uint64_dict(cr.u.value);
    return_uint64_dict(orig);
}
 
// ================================================================================
// Group 16: merge_uint64_dict
// ================================================================================
 
static void test_u64d_merge_null_first_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    uint64_dict_expect_t r = merge_uint64_dict(NULL, d, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_uint64_dict(d);
}
 
static void test_u64d_merge_no_overwrite_keeps_first_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* da = _make_uint64_dict(4);
    uint64_dict_t* db = _make_uint64_dict(4);
    insert_uint64_dict(da, "shared", 1099511627776ULL,  a);
    insert_uint64_dict(db, "shared", 9999999999999ULL,  a);
    insert_uint64_dict(db, "new",    5000000000000ULL,  a);
 
    uint64_dict_expect_t mr = merge_uint64_dict(da, db, false, a);
    assert_true(mr.has_value);
    uint64_t v = 0ULL;
    assert_int_equal(get_uint64_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_true(v == 1099511627776ULL);
    assert_int_equal(get_uint64_dict_value(mr.u.value, "new", &v), NO_ERROR);
    assert_true(v == 5000000000000ULL);
 
    return_uint64_dict(mr.u.value);
    return_uint64_dict(da);
    return_uint64_dict(db);
}
 
static void test_u64d_merge_overwrite_uses_second_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* da = _make_uint64_dict(4);
    uint64_dict_t* db = _make_uint64_dict(4);
    insert_uint64_dict(da, "shared", 1099511627776ULL, a);
    insert_uint64_dict(db, "shared", 9999999999999ULL, a);
 
    uint64_dict_expect_t mr = merge_uint64_dict(da, db, true, a);
    assert_true(mr.has_value);
    uint64_t v = 0ULL;
    assert_int_equal(get_uint64_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_true(v == 9999999999999ULL);
 
    return_uint64_dict(mr.u.value);
    return_uint64_dict(da);
    return_uint64_dict(db);
}
 
// ================================================================================
// Group 17: foreach_uint64_dict
// ================================================================================
 
static void test_u64d_foreach_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(foreach_uint64_dict(NULL, _u64d_sum_iter, NULL),
                     NULL_POINTER);
}
 
static void test_u64d_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_int_equal(foreach_uint64_dict(d, NULL, NULL), NULL_POINTER);
    return_uint64_dict(d);
}
 
static void test_u64d_foreach_correct_sum(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    insert_uint64_dict(d, "a", 1000ULL, a);
    insert_uint64_dict(d, "b", 2000ULL, a);
    insert_uint64_dict(d, "c", 3000ULL, a);
    _u64d_iter_ctx_t ctx = { 0, 0ULL };
    assert_int_equal(foreach_uint64_dict(d, _u64d_sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 3);
    assert_true(ctx.sum == 6000ULL);
    return_uint64_dict(d);
}
 
static void test_u64d_foreach_delivers_uint64_max(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    insert_uint64_dict(d, "top", UINT64_MAX, a);
    _u64d_iter_ctx_t ctx = { 0, 0u };
    foreach_uint64_dict(d, _u64d_sum_iter, &ctx);
    assert_int_equal(ctx.count, 1);
    assert_int_equal((int)ctx.sum, (int)UINT64_MAX);
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 18: introspection
// ================================================================================
 
static void test_u64d_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint64_dict_size(NULL), 0);
}
 
static void test_u64d_hash_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint64_dict_hash_size(NULL), 0);
}
 
static void test_u64d_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)uint64_dict_alloc(NULL), 0);
}
 
static void test_u64d_is_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_uint64_dict_empty(NULL));
}
 
static void test_u64d_is_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_t* d = _make_uint64_dict(4);
    assert_true(is_uint64_dict_empty(d));
    insert_uint64_dict(d, "x", 1u, a);
    assert_false(is_uint64_dict_empty(d));
    pop_uint64_dict(d, "x", NULL);
    assert_true(is_uint64_dict_empty(d));
    return_uint64_dict(d);
}
 
// ================================================================================
// Group 19: growth stress
// ================================================================================
 
static void test_u64d_growth_values_survive_resize(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint64_dict_expect_t r = init_uint64_dict(2, true, a);
    assert_true(r.has_value);
    uint64_dict_t* d = r.u.value;
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        uint64_t val = (uint64_t)i * 5000000000ULL;   /* exercises > UINT32_MAX values */
        assert_int_equal(insert_uint64_dict(d, key, val, a), NO_ERROR);
    }
    assert_int_equal((int)uint64_dict_hash_size(d), 200);
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        uint64_t expected = (uint64_t)i * 5000000000ULL;
        uint64_t v = 0u;
        assert_int_equal(get_uint64_dict_value(d, key, &v), NO_ERROR);
        assert_true(v == expected);
    }
 
    return_uint64_dict(d);
}
 
// ================================================================================
// main
// ================================================================================
 
    const struct CMUnitTest test_uint64_dict[] = {
 
    /* Group 1: init / return */
    cmocka_unit_test(test_u64d_init_null_allocator_fails),
    cmocka_unit_test(test_u64d_init_zero_capacity_fails),
    cmocka_unit_test(test_u64d_init_data_size_is_eight_bytes),
    cmocka_unit_test(test_u64d_init_dtype_is_uint64),
    cmocka_unit_test(test_u64d_return_null_is_safe),

    /* Group 2: insert */
    cmocka_unit_test(test_u64d_insert_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u64d_insert_null_key_returns_null_pointer),
    cmocka_unit_test(test_u64d_insert_duplicate_returns_invalid_arg),
    cmocka_unit_test(test_u64d_insert_stores_zero),
    cmocka_unit_test(test_u64d_insert_stores_max),
    cmocka_unit_test(test_u64d_insert_stores_midrange),

    /* Group 3: insert_n */
    cmocka_unit_test(test_u64d_insert_n_null_key_returns_null_pointer),
    cmocka_unit_test(test_u64d_insert_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u64d_insert_n_uses_only_specified_bytes),

    /* Group 4: pop */
    cmocka_unit_test(test_u64d_pop_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u64d_pop_null_key_returns_null_pointer),
    cmocka_unit_test(test_u64d_pop_missing_key_returns_not_found),
    cmocka_unit_test(test_u64d_pop_returns_correct_value),
    cmocka_unit_test(test_u64d_pop_null_out_value_discards_safely),

    /* Group 5: pop_n */
    cmocka_unit_test(test_u64d_pop_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u64d_pop_n_removes_bounded_key),

    /* Group 6: update */
    cmocka_unit_test(test_u64d_update_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u64d_update_null_key_returns_null_pointer),
    cmocka_unit_test(test_u64d_update_missing_key_returns_not_found),
    cmocka_unit_test(test_u64d_update_overwrites_value),

    /* Group 7: update_n */
    cmocka_unit_test(test_u64d_update_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u64d_update_n_updates_bounded_key),

    /* Group 8: get_value */
    cmocka_unit_test(test_u64d_get_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u64d_get_null_key_returns_null_pointer),
    cmocka_unit_test(test_u64d_get_null_out_returns_null_pointer),
    cmocka_unit_test(test_u64d_get_missing_key_returns_not_found),
    cmocka_unit_test(test_u64d_get_retrieves_extremes),

    /* Group 9: get_value_n */
    cmocka_unit_test(test_u64d_get_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_u64d_get_n_retrieves_bounded_key),

    /* Group 10: get_ptr */
    cmocka_unit_test(test_u64d_ptr_null_dict_returns_null),
    cmocka_unit_test(test_u64d_ptr_null_key_returns_null),
    cmocka_unit_test(test_u64d_ptr_missing_returns_null),
    cmocka_unit_test(test_u64d_ptr_points_to_correct_value),

    /* Group 11: get_ptr_n */
    cmocka_unit_test(test_u64d_ptr_n_zero_len_returns_null),
    cmocka_unit_test(test_u64d_ptr_n_points_to_bounded_value),

    /* Group 12: has_key */
    cmocka_unit_test(test_u64d_has_null_dict_returns_false),
    cmocka_unit_test(test_u64d_has_null_key_returns_false),
    cmocka_unit_test(test_u64d_has_present_and_absent),

    /* Group 13: has_key_n */
    cmocka_unit_test(test_u64d_has_n_zero_len_returns_false),
    cmocka_unit_test(test_u64d_has_n_distinguishes_by_length),

    /* Group 14: clear */
    cmocka_unit_test(test_u64d_clear_null_returns_null_pointer),
    cmocka_unit_test(test_u64d_clear_resets_and_is_reusable),

    /* Group 15: copy */
    cmocka_unit_test(test_u64d_copy_null_src_fails),
    cmocka_unit_test(test_u64d_copy_is_independent),

    /* Group 16: merge */
    cmocka_unit_test(test_u64d_merge_null_first_fails),
    cmocka_unit_test(test_u64d_merge_no_overwrite_keeps_first_value),
    cmocka_unit_test(test_u64d_merge_overwrite_uses_second_value),

    /* Group 17: foreach */
    cmocka_unit_test(test_u64d_foreach_null_dict_returns_null_pointer),
    cmocka_unit_test(test_u64d_foreach_null_fn_returns_null_pointer),
    cmocka_unit_test(test_u64d_foreach_correct_sum),
    cmocka_unit_test(test_u64d_foreach_delivers_uint64_max),

    /* Group 18: introspection */
    cmocka_unit_test(test_u64d_size_null_returns_zero),
    cmocka_unit_test(test_u64d_hash_size_null_returns_zero),
    cmocka_unit_test(test_u64d_alloc_null_returns_zero),
    cmocka_unit_test(test_u64d_is_empty_null_returns_true),
    cmocka_unit_test(test_u64d_is_empty_reflects_contents),

    /* Group 19: growth stress */
    cmocka_unit_test(test_u64d_growth_values_survive_resize),
};
const size_t test_uint64_dict_count = sizeof(test_uint64_dict) / sizeof(test_uint64_dict[0]);
// ================================================================================ 
// ================================================================================ 

static int64_dict_t* _make_int64_dict(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    int64_dict_expect_t r = init_int64_dict(cap, true, a);
    assert_true(r.has_value);
    return r.u.value;
}
 
typedef struct { int count; long long sum; } _i64d_iter_ctx_t;
 
static void _i64d_sum_iter(const char* key, size_t key_len,
                            int64_t value, void* ud) {
    (void)key; (void)key_len;
    _i64d_iter_ctx_t* c = (_i64d_iter_ctx_t*)ud;
    c->count++;
    c->sum += (long long)value;
}
 
// ================================================================================
// Group 1: init_int64_dict / return_int64_dict
// ================================================================================
 
static void test_i64d_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    int64_dict_expect_t r = init_int64_dict(8, true, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_i64d_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_expect_t r = init_int64_dict(0, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_i64d_init_data_size_is_eight_bytes(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal((int)dict_data_size(d), 8);
    return_int64_dict(d);
}
 
static void test_i64d_init_dtype_is_int64(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal((int)d->dtype, (int)INT64_TYPE);
    return_int64_dict(d);
}
 
static void test_i64d_return_null_is_safe(void** state) {
    (void)state;
    return_int64_dict(NULL);
}
 
// ================================================================================
// Group 2: insert_int64_dict
// ================================================================================
 
static void test_i64d_insert_null_dict_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    assert_int_equal(insert_int64_dict(NULL, "k", 1u, a), NULL_POINTER);
}
 
static void test_i64d_insert_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal(insert_int64_dict(d, NULL, 1u, a), NULL_POINTER);
    return_int64_dict(d);
}
 
static void test_i64d_insert_duplicate_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal(insert_int64_dict(d, "k", 1u, a), NO_ERROR);
    assert_int_equal(insert_int64_dict(d, "k", 2u, a), INVALID_ARG);
    assert_int_equal((int)int64_dict_hash_size(d), 1);
    return_int64_dict(d);
}
 
static void test_i64d_insert_stores_zero(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal(insert_int64_dict(d, "zero", 0LL, a), NO_ERROR);
    int64_t v = 99LL;
    assert_int_equal(get_int64_dict_value(d, "zero", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    return_int64_dict(d);
}
 
static void test_i64d_insert_stores_max(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal(insert_int64_dict(d, "max", INT64_MAX, a), NO_ERROR);
    int64_t v = 0u;
    assert_int_equal(get_int64_dict_value(d, "max", &v), NO_ERROR);
    assert_int_equal((int)v, (int)INT64_MAX);
    return_int64_dict(d);
}
 
static void test_i64d_insert_stores_midrange(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal(insert_int64_dict(d, "depth", -10994000LL, a), NO_ERROR);
    int64_t v = 0LL;
    assert_int_equal(get_int64_dict_value(d, "depth", &v), NO_ERROR);
    assert_true(v == -10994000LL);
    return_int64_dict(d);
}
 
// ================================================================================
// Group 3: insert_int64_dict_n
// ================================================================================
 
static void test_i64d_insert_n_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal(insert_int64_dict_n(d, NULL, 3, 1u, a), NULL_POINTER);
    return_int64_dict(d);
}
 
static void test_i64d_insert_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal(insert_int64_dict_n(d, "k", 0, 1u, a), INVALID_ARG);
    return_int64_dict(d);
}
 
static void test_i64d_insert_n_uses_only_specified_bytes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    /* "delta_total" — insert only "delta" (5 bytes) with value -500000000000 */
    const char* buf = "delta_total";
    assert_int_equal(insert_int64_dict_n(d, buf, 5, -500000000000LL, a), NO_ERROR);
    int64_t v = 0LL;
    assert_int_equal(get_int64_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == -500000000000LL);
    /* Full buffer key "delta_total" is different — not found */
    assert_int_equal(get_int64_dict_value_n(d, buf, 11, &v), NOT_FOUND);
    /* Plain null-terminated lookup also works */
    assert_int_equal(get_int64_dict_value(d, "delta", &v), NO_ERROR);
    assert_true(v == -500000000000LL);
    return_int64_dict(d);
}
 
// ================================================================================
// Group 4: pop_int64_dict
// ================================================================================
 
static void test_i64d_pop_null_dict_returns_null_pointer(void** state) {
    (void)state;
    int64_t v;
    assert_int_equal(pop_int64_dict(NULL, "k", &v), NULL_POINTER);
}
 
static void test_i64d_pop_null_key_returns_null_pointer(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    int64_t v;
    assert_int_equal(pop_int64_dict(d, NULL, &v), NULL_POINTER);
    return_int64_dict(d);
}
 
static void test_i64d_pop_missing_key_returns_not_found(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal(pop_int64_dict(d, "missing", NULL), NOT_FOUND);
    return_int64_dict(d);
}
 
static void test_i64d_pop_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    insert_int64_dict(d, "depth", -10994000LL, a);
    int64_t v = 0LL;
    assert_int_equal(pop_int64_dict(d, "depth", &v), NO_ERROR);
    assert_true(v == -10994000LL);
    assert_int_equal((int)int64_dict_hash_size(d), 0);
    return_int64_dict(d);
}
 
static void test_i64d_pop_null_out_value_discards_safely(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    insert_int64_dict(d, "x", -1000000000000LL, a);
    assert_int_equal(pop_int64_dict(d, "x", NULL), NO_ERROR);
    assert_int_equal((int)int64_dict_hash_size(d), 0);
    return_int64_dict(d);
}
 
// ================================================================================
// Group 5: pop_int64_dict_n
// ================================================================================
 
static void test_i64d_pop_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal(pop_int64_dict_n(d, "k", 0, NULL), INVALID_ARG);
    return_int64_dict(d);
}
 
static void test_i64d_pop_n_removes_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    const char* buf = "delta_total";
    insert_int64_dict_n(d, buf, 5, -500000000000LL, a);   /* key = "delta" */
    int64_t v = 0LL;
    assert_int_equal(pop_int64_dict_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == -500000000000LL);
    assert_int_equal((int)int64_dict_hash_size(d), 0);
    return_int64_dict(d);
}
 
// ================================================================================
// Group 6: update_int64_dict
// ================================================================================
 
static void test_i64d_update_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(update_int64_dict(NULL, "k", 1u), NULL_POINTER);
}
 
static void test_i64d_update_null_key_returns_null_pointer(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal(update_int64_dict(d, NULL, 1u), NULL_POINTER);
    return_int64_dict(d);
}
 
static void test_i64d_update_missing_key_returns_not_found(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal(update_int64_dict(d, "missing", 1u), NOT_FOUND);
    return_int64_dict(d);
}
 
static void test_i64d_update_overwrites_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    insert_int64_dict(d, "offset", 1000000000000LL, a);
    assert_int_equal(update_int64_dict(d, "offset", -1000000000000LL), NO_ERROR);
    int64_t v = 0LL;
    assert_int_equal(get_int64_dict_value(d, "offset", &v), NO_ERROR);
    assert_true(v == -1000000000000LL);
    assert_int_equal((int)int64_dict_hash_size(d), 1);
    return_int64_dict(d);
}
 
// ================================================================================
// Group 7: update_int64_dict_n
// ================================================================================
 
static void test_i64d_update_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal(update_int64_dict_n(d, "k", 0, 1u), INVALID_ARG);
    return_int64_dict(d);
}
 
static void test_i64d_update_n_updates_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    const char* buf = "delta_total";
    insert_int64_dict_n(d, buf, 5, 1000000000000LL, a);   /* key = "delta" */
    assert_int_equal(update_int64_dict_n(d, buf, 5, -1000000000000LL), NO_ERROR);
    int64_t v = 0LL;
    assert_int_equal(get_int64_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == -1000000000000LL);
    return_int64_dict(d);
}
 
// ================================================================================
// Group 8: get_int64_dict_value
// ================================================================================
 
static void test_i64d_get_null_dict_returns_null_pointer(void** state) {
    (void)state;
    int64_t v;
    assert_int_equal(get_int64_dict_value(NULL, "k", &v), NULL_POINTER);
}
 
static void test_i64d_get_null_key_returns_null_pointer(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    int64_t v;
    assert_int_equal(get_int64_dict_value(d, NULL, &v), NULL_POINTER);
    return_int64_dict(d);
}
 
static void test_i64d_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    insert_int64_dict(d, "k", -1000000000000LL, a);
    assert_int_equal(get_int64_dict_value(d, "k", NULL), NULL_POINTER);
    return_int64_dict(d);
}
 
static void test_i64d_get_missing_key_returns_not_found(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    int64_t v;
    assert_int_equal(get_int64_dict_value(d, "missing", &v), NOT_FOUND);
    return_int64_dict(d);
}
 
static void test_i64d_get_retrieves_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    insert_int64_dict(d, "min",  0u,         a);
    insert_int64_dict(d, "max",  INT64_MAX, a);
    insert_int64_dict(d, "mid",  32768u,     a);
    int64_t v = 0u;
    assert_int_equal(get_int64_dict_value(d, "min", &v), NO_ERROR);
    assert_int_equal((int)v, 0);
    assert_int_equal(get_int64_dict_value(d, "max", &v), NO_ERROR);
    assert_int_equal((int)v, (int)INT64_MAX);
    assert_int_equal(get_int64_dict_value(d, "mid", &v), NO_ERROR);
    assert_int_equal((int)v, 32768);
    return_int64_dict(d);
}
 
// ================================================================================
// Group 9: get_int64_dict_value_n
// ================================================================================
 
static void test_i64d_get_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    int64_t v;
    assert_int_equal(get_int64_dict_value_n(d, "k", 0, &v), INVALID_ARG);
    return_int64_dict(d);
}
 
static void test_i64d_get_n_retrieves_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    const char* buf = "delta_total";
    insert_int64_dict_n(d, buf, 5, -500000000000LL, a);   /* key = "delta" */
    int64_t v = 0LL;
    assert_int_equal(get_int64_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == -500000000000LL);
    assert_int_equal(get_int64_dict_value_n(d, buf, 11, &v), NOT_FOUND);
    return_int64_dict(d);
}
 
// ================================================================================
// Group 10: get_int64_dict_ptr
// ================================================================================
 
static void test_i64d_ptr_null_dict_returns_null(void** state) {
    (void)state;
    assert_null(get_int64_dict_ptr(NULL, "k"));
}
 
static void test_i64d_ptr_null_key_returns_null(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    assert_null(get_int64_dict_ptr(d, NULL));
    return_int64_dict(d);
}
 
static void test_i64d_ptr_missing_returns_null(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    assert_null(get_int64_dict_ptr(d, "missing"));
    return_int64_dict(d);
}
 
static void test_i64d_ptr_points_to_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    insert_int64_dict(d, "depth", -10994000LL, a);
    const int64_t* p = get_int64_dict_ptr(d, "depth");
    assert_non_null(p);
    assert_true(*p == -10994000LL);
    return_int64_dict(d);
}
 
// ================================================================================
// Group 11: get_int64_dict_ptr_n
// ================================================================================
 
static void test_i64d_ptr_n_zero_len_returns_null(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    assert_null(get_int64_dict_ptr_n(d, "k", 0));
    return_int64_dict(d);
}
 
static void test_i64d_ptr_n_points_to_bounded_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    const char* buf = "delta_total";
    insert_int64_dict_n(d, buf, 5, -500000000000LL, a);   /* key = "delta" */
    const int64_t* p = get_int64_dict_ptr_n(d, buf, 5);
    assert_non_null(p);
    assert_true(*p == -500000000000LL);
    return_int64_dict(d);
}
 
// ================================================================================
// Group 12: has_int64_dict_key
// ================================================================================
 
static void test_i64d_has_null_dict_returns_false(void** state) {
    (void)state;
    assert_false(has_int64_dict_key(NULL, "k"));
}
 
static void test_i64d_has_null_key_returns_false(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    assert_false(has_int64_dict_key(d, NULL));
    return_int64_dict(d);
}
 
static void test_i64d_has_present_and_absent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    insert_int64_dict(d, "present", -1000000000000LL, a);
    assert_true(has_int64_dict_key(d, "present"));
    assert_false(has_int64_dict_key(d, "absent"));
    return_int64_dict(d);
}
 
// ================================================================================
// Group 13: has_int64_dict_key_n
// ================================================================================
 
static void test_i64d_has_n_zero_len_returns_false(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    assert_false(has_int64_dict_key_n(d, "k", 0));
    return_int64_dict(d);
}
 
static void test_i64d_has_n_distinguishes_by_length(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    insert_int64_dict_n(d, "abc", 2, -1000000000000LL, a);   /* key = "ab" */
    insert_int64_dict_n(d, "abc", 3, -2000000000000LL, a);   /* key = "abc" */
    assert_true(has_int64_dict_key_n(d, "abc", 2));
    assert_true(has_int64_dict_key_n(d, "abc", 3));
    assert_false(has_int64_dict_key_n(d, "abc", 1));
    return_int64_dict(d);
}
 
// ================================================================================
// Group 14: clear_int64_dict
// ================================================================================
 
static void test_i64d_clear_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_int64_dict(NULL), NULL_POINTER);
}
 
static void test_i64d_clear_resets_and_is_reusable(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    insert_int64_dict(d, "a", -1000000000000LL, a);
    insert_int64_dict(d, "b",  2000000000000LL, a);
    assert_int_equal(clear_int64_dict(d), NO_ERROR);
    assert_int_equal((int)int64_dict_hash_size(d), 0);
    assert_true(is_int64_dict_empty(d));
    assert_int_equal(insert_int64_dict(d, "a", -3000000000000LL, a), NO_ERROR);
    int64_t v = 0LL;
    assert_int_equal(get_int64_dict_value(d, "a", &v), NO_ERROR);
    assert_true(v == -3000000000000LL);
    return_int64_dict(d);
}
 
// ================================================================================
// Group 15: copy_int64_dict
// ================================================================================
 
static void test_i64d_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_expect_t r = copy_int64_dict(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_i64d_copy_is_independent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* orig = _make_int64_dict(4);
    insert_int64_dict(orig, "x", -1000000000000LL, a);
    insert_int64_dict(orig, "y",  2000000000000LL, a);
 
    int64_dict_expect_t cr = copy_int64_dict(orig, a);
    assert_true(cr.has_value);
 
    update_int64_dict(orig, "x", INT64_MAX);
 
    int64_t v = 0LL;
    assert_int_equal(get_int64_dict_value(cr.u.value, "x", &v), NO_ERROR);
    assert_true(v == -1000000000000LL);
    assert_int_equal((int)int64_dict_hash_size(cr.u.value), 2);
 
    return_int64_dict(cr.u.value);
    return_int64_dict(orig);
}
 
// ================================================================================
// Group 16: merge_int64_dict
// ================================================================================
 
static void test_i64d_merge_null_first_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    int64_dict_expect_t r = merge_int64_dict(NULL, d, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_int64_dict(d);
}
 
static void test_i64d_merge_no_overwrite_keeps_first_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* da = _make_int64_dict(4);
    int64_dict_t* db = _make_int64_dict(4);
    insert_int64_dict(da, "shared", -1000000000000LL,  a);
    insert_int64_dict(db, "shared",  9999999999999LL,  a);
    insert_int64_dict(db, "new",    -5000000000000LL,  a);
 
    int64_dict_expect_t mr = merge_int64_dict(da, db, false, a);
    assert_true(mr.has_value);
    int64_t v = 0LL;
    assert_int_equal(get_int64_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_true(v == -1000000000000LL);
    assert_int_equal(get_int64_dict_value(mr.u.value, "new", &v), NO_ERROR);
    assert_true(v == -5000000000000LL);
 
    return_int64_dict(mr.u.value);
    return_int64_dict(da);
    return_int64_dict(db);
}
 
static void test_i64d_merge_overwrite_uses_second_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* da = _make_int64_dict(4);
    int64_dict_t* db = _make_int64_dict(4);
    insert_int64_dict(da, "shared", -1000000000000LL, a);
    insert_int64_dict(db, "shared", -9999999999999LL, a);
 
    int64_dict_expect_t mr = merge_int64_dict(da, db, true, a);
    assert_true(mr.has_value);
    int64_t v = 0LL;
    assert_int_equal(get_int64_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_true(v == -9999999999999LL);
 
    return_int64_dict(mr.u.value);
    return_int64_dict(da);
    return_int64_dict(db);
}
 
// ================================================================================
// Group 17: foreach_int64_dict
// ================================================================================
 
static void test_i64d_foreach_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(foreach_int64_dict(NULL, _i64d_sum_iter, NULL),
                     NULL_POINTER);
}
 
static void test_i64d_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    int64_dict_t* d = _make_int64_dict(4);
    assert_int_equal(foreach_int64_dict(d, NULL, NULL), NULL_POINTER);
    return_int64_dict(d);
}
 
static void test_i64d_foreach_correct_sum(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    insert_int64_dict(d, "a", -1000LL, a);
    insert_int64_dict(d, "b",  2000LL, a);
    insert_int64_dict(d, "c", -3000LL, a);
    _i64d_iter_ctx_t ctx = { 0, 0LL };
    assert_int_equal(foreach_int64_dict(d, _i64d_sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 3);
    assert_true(ctx.sum == -2000LL);
    return_int64_dict(d);
}
 
static void test_i64d_foreach_delivers_signed_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    insert_int64_dict(d, "min", INT64_MIN, a);
    insert_int64_dict(d, "max", INT64_MAX, a);
    _i64d_iter_ctx_t ctx = { 0, 0LL };
    foreach_int64_dict(d, _i64d_sum_iter, &ctx);
    assert_int_equal(ctx.count, 2);
    /* INT64_MIN + INT64_MAX == -1 */
    assert_true(ctx.sum == -1LL);
    return_int64_dict(d);
}
 
// ================================================================================
// Group 18: introspection
// ================================================================================
 
static void test_i64d_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int64_dict_size(NULL), 0);
}
 
static void test_i64d_hash_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int64_dict_hash_size(NULL), 0);
}
 
static void test_i64d_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)int64_dict_alloc(NULL), 0);
}
 
static void test_i64d_is_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_int64_dict_empty(NULL));
}
 
static void test_i64d_is_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_t* d = _make_int64_dict(4);
    assert_true(is_int64_dict_empty(d));
    insert_int64_dict(d, "x", 1u, a);
    assert_false(is_int64_dict_empty(d));
    pop_int64_dict(d, "x", NULL);
    assert_true(is_int64_dict_empty(d));
    return_int64_dict(d);
}
 
// ================================================================================
// Group 19: growth stress
// ================================================================================
 
static void test_i64d_growth_values_survive_resize(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int64_dict_expect_t r = init_int64_dict(2, true, a);
    assert_true(r.has_value);
    int64_dict_t* d = r.u.value;
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        int64_t val = (i % 2 == 0) ? -((int64_t)i * 5000000000LL)
                                    :  ((int64_t)i * 5000000000LL);
        assert_int_equal(insert_int64_dict(d, key, val, a), NO_ERROR);
    }
    assert_int_equal((int)int64_dict_hash_size(d), 200);
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        int64_t expected = (i % 2 == 0) ? -((int64_t)i * 5000000000LL)
                                         :  ((int64_t)i * 5000000000LL);
        int64_t v = 0u;
        assert_int_equal(get_int64_dict_value(d, key, &v), NO_ERROR);
        assert_true(v == expected);
    }
 
    return_int64_dict(d);
}
 
// ================================================================================
// main
// ================================================================================
 
const struct CMUnitTest test_int64_dict[] = {

    /* Group 1: init / return */
    cmocka_unit_test(test_i64d_init_null_allocator_fails),
    cmocka_unit_test(test_i64d_init_zero_capacity_fails),
    cmocka_unit_test(test_i64d_init_data_size_is_eight_bytes),
    cmocka_unit_test(test_i64d_init_dtype_is_int64),
    cmocka_unit_test(test_i64d_return_null_is_safe),

    /* Group 2: insert */
    cmocka_unit_test(test_i64d_insert_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i64d_insert_null_key_returns_null_pointer),
    cmocka_unit_test(test_i64d_insert_duplicate_returns_invalid_arg),
    cmocka_unit_test(test_i64d_insert_stores_zero),
    cmocka_unit_test(test_i64d_insert_stores_max),
    cmocka_unit_test(test_i64d_insert_stores_midrange),

    /* Group 3: insert_n */
    cmocka_unit_test(test_i64d_insert_n_null_key_returns_null_pointer),
    cmocka_unit_test(test_i64d_insert_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i64d_insert_n_uses_only_specified_bytes),

    /* Group 4: pop */
    cmocka_unit_test(test_i64d_pop_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i64d_pop_null_key_returns_null_pointer),
    cmocka_unit_test(test_i64d_pop_missing_key_returns_not_found),
    cmocka_unit_test(test_i64d_pop_returns_correct_value),
    cmocka_unit_test(test_i64d_pop_null_out_value_discards_safely),

    /* Group 5: pop_n */
    cmocka_unit_test(test_i64d_pop_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i64d_pop_n_removes_bounded_key),

    /* Group 6: update */
    cmocka_unit_test(test_i64d_update_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i64d_update_null_key_returns_null_pointer),
    cmocka_unit_test(test_i64d_update_missing_key_returns_not_found),
    cmocka_unit_test(test_i64d_update_overwrites_value),

    /* Group 7: update_n */
    cmocka_unit_test(test_i64d_update_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i64d_update_n_updates_bounded_key),

    /* Group 8: get_value */
    cmocka_unit_test(test_i64d_get_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i64d_get_null_key_returns_null_pointer),
    cmocka_unit_test(test_i64d_get_null_out_returns_null_pointer),
    cmocka_unit_test(test_i64d_get_missing_key_returns_not_found),
    cmocka_unit_test(test_i64d_get_retrieves_extremes),

    /* Group 9: get_value_n */
    cmocka_unit_test(test_i64d_get_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_i64d_get_n_retrieves_bounded_key),

    /* Group 10: get_ptr */
    cmocka_unit_test(test_i64d_ptr_null_dict_returns_null),
    cmocka_unit_test(test_i64d_ptr_null_key_returns_null),
    cmocka_unit_test(test_i64d_ptr_missing_returns_null),
    cmocka_unit_test(test_i64d_ptr_points_to_correct_value),

    /* Group 11: get_ptr_n */
    cmocka_unit_test(test_i64d_ptr_n_zero_len_returns_null),
    cmocka_unit_test(test_i64d_ptr_n_points_to_bounded_value),

    /* Group 12: has_key */
    cmocka_unit_test(test_i64d_has_null_dict_returns_false),
    cmocka_unit_test(test_i64d_has_null_key_returns_false),
    cmocka_unit_test(test_i64d_has_present_and_absent),

    /* Group 13: has_key_n */
    cmocka_unit_test(test_i64d_has_n_zero_len_returns_false),
    cmocka_unit_test(test_i64d_has_n_distinguishes_by_length),

    /* Group 14: clear */
    cmocka_unit_test(test_i64d_clear_null_returns_null_pointer),
    cmocka_unit_test(test_i64d_clear_resets_and_is_reusable),

    /* Group 15: copy */
    cmocka_unit_test(test_i64d_copy_null_src_fails),
    cmocka_unit_test(test_i64d_copy_is_independent),

    /* Group 16: merge */
    cmocka_unit_test(test_i64d_merge_null_first_fails),
    cmocka_unit_test(test_i64d_merge_no_overwrite_keeps_first_value),
    cmocka_unit_test(test_i64d_merge_overwrite_uses_second_value),

    /* Group 17: foreach */
    cmocka_unit_test(test_i64d_foreach_null_dict_returns_null_pointer),
    cmocka_unit_test(test_i64d_foreach_null_fn_returns_null_pointer),
    cmocka_unit_test(test_i64d_foreach_correct_sum),
    cmocka_unit_test(test_i64d_foreach_delivers_signed_extremes),

    /* Group 18: introspection */
    cmocka_unit_test(test_i64d_size_null_returns_zero),
    cmocka_unit_test(test_i64d_hash_size_null_returns_zero),
    cmocka_unit_test(test_i64d_alloc_null_returns_zero),
    cmocka_unit_test(test_i64d_is_empty_null_returns_true),
    cmocka_unit_test(test_i64d_is_empty_reflects_contents),

    /* Group 19: growth stress */
    cmocka_unit_test(test_i64d_growth_values_survive_resize),

};
const size_t test_int64_dict_count = sizeof(test_int64_dict) / sizeof(test_int64_dict[0]);
// ================================================================================ 
// ================================================================================ 

static float_dict_t* _make_float_dict(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    float_dict_expect_t r = init_float_dict(cap, true, a);
    assert_true(r.has_value);
    return r.u.value;
}
 
typedef struct { int count; double sum; } _fd_iter_ctx_t;
 
static void _fd_sum_iter(const char* key, size_t key_len,
                          float value, void* ud) {
    (void)key; (void)key_len;
    _fd_iter_ctx_t* c = (_fd_iter_ctx_t*)ud;
    c->count++;
    c->sum += (double)value;
}
 
// ================================================================================
// Group 1: init_float_dict / return_float_dict
// ================================================================================
 
static void test_fd_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    float_dict_expect_t r = init_float_dict(8, true, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_fd_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_expect_t r = init_float_dict(0, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_fd_init_data_size_is_four_bytes(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal((int)dict_data_size(d), 4);
    return_float_dict(d);
}
 
static void test_fd_init_dtype_is_float(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal((int)d->dtype, (int)FLOAT_TYPE);
    return_float_dict(d);
}
 
static void test_fd_return_null_is_safe(void** state) {
    (void)state;
    return_float_dict(NULL);
}
 
// ================================================================================
// Group 2: insert_float_dict
// ================================================================================
 
static void test_fd_insert_null_dict_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    assert_int_equal(insert_float_dict(NULL, "k", 1u, a), NULL_POINTER);
}
 
static void test_fd_insert_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal(insert_float_dict(d, NULL, 1u, a), NULL_POINTER);
    return_float_dict(d);
}
 
static void test_fd_insert_duplicate_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal(insert_float_dict(d, "k", 1u, a), NO_ERROR);
    assert_int_equal(insert_float_dict(d, "k", 2u, a), INVALID_ARG);
    assert_int_equal((int)float_dict_hash_size(d), 1);
    return_float_dict(d);
}
 
static void test_fd_insert_stores_zero(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal(insert_float_dict(d, "zero", 0.0f, a), NO_ERROR);
    float v = 99.0f;
    assert_int_equal(get_float_dict_value(d, "zero", &v), NO_ERROR);
    assert_true(v == 0.0f);
    return_float_dict(d);
}
 
static void test_fd_insert_stores_float_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal(insert_float_dict(d, "max",  FLT_MAX, a), NO_ERROR);
    assert_int_equal(insert_float_dict(d, "min", -FLT_MAX, a), NO_ERROR);
    assert_int_equal(insert_float_dict(d, "tiny", FLT_MIN, a), NO_ERROR);
    float v = 0.0f;
    assert_int_equal(get_float_dict_value(d, "max", &v), NO_ERROR);
    assert_true(v == FLT_MAX);
    assert_int_equal(get_float_dict_value(d, "min", &v), NO_ERROR);
    assert_true(v == -FLT_MAX);
    assert_int_equal(get_float_dict_value(d, "tiny", &v), NO_ERROR);
    assert_true(v == FLT_MIN);
    return_float_dict(d);
}
 
static void test_fd_insert_stores_signed_float(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal(insert_float_dict(d, "pi",      3.14159265f, a), NO_ERROR);
    assert_int_equal(insert_float_dict(d, "gravity", 9.80665f,    a), NO_ERROR);
    assert_int_equal(insert_float_dict(d, "neg",    -1.5f,        a), NO_ERROR);
    float v = 0.0f;
    assert_int_equal(get_float_dict_value(d, "pi", &v), NO_ERROR);
    assert_true(v == 3.14159265f);
    assert_int_equal(get_float_dict_value(d, "gravity", &v), NO_ERROR);
    assert_true(v == 9.80665f);
    assert_int_equal(get_float_dict_value(d, "neg", &v), NO_ERROR);
    assert_true(v == -1.5f);
    return_float_dict(d);
}
 
// ================================================================================
// Group 3: insert_float_dict_n
// ================================================================================
 
static void test_fd_insert_n_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal(insert_float_dict_n(d, NULL, 3, 1.0f, a), NULL_POINTER);
    return_float_dict(d);
}
 
static void test_fd_insert_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal(insert_float_dict_n(d, "k", 0, 1.0f, a), INVALID_ARG);
    return_float_dict(d);
}
 
static void test_fd_insert_n_uses_only_specified_bytes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    /* "speed_max" — insert only "speed" (5 bytes) with value c (m/s) */
    const char* buf = "speed_max";
    assert_int_equal(insert_float_dict_n(d, buf, 5, 299792458.0f, a), NO_ERROR);
    float v = 0.0f;
    assert_int_equal(get_float_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 299792458.0f);
    /* Full buffer key "speed_max" is different — not found */
    assert_int_equal(get_float_dict_value_n(d, buf, 9, &v), NOT_FOUND);
    /* Plain null-terminated lookup also works */
    assert_int_equal(get_float_dict_value(d, "speed", &v), NO_ERROR);
    assert_true(v == 299792458.0f);
    return_float_dict(d);
}
 
// ================================================================================
// Group 4: pop_float_dict
// ================================================================================
 
static void test_fd_pop_null_dict_returns_null_pointer(void** state) {
    (void)state;
    float v;
    assert_int_equal(pop_float_dict(NULL, "k", &v), NULL_POINTER);
}
 
static void test_fd_pop_null_key_returns_null_pointer(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    float v;
    assert_int_equal(pop_float_dict(d, NULL, &v), NULL_POINTER);
    return_float_dict(d);
}
 
static void test_fd_pop_missing_key_returns_not_found(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal(pop_float_dict(d, "missing", NULL), NOT_FOUND);
    return_float_dict(d);
}
 
static void test_fd_pop_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    insert_float_dict(d, "gravity", 9.80665f, a);
    float v = 0.0f;
    assert_int_equal(pop_float_dict(d, "gravity", &v), NO_ERROR);
    assert_true(v == 9.80665f);
    assert_int_equal((int)float_dict_hash_size(d), 0);
    return_float_dict(d);
}
 
static void test_fd_pop_null_out_value_discards_safely(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    insert_float_dict(d, "x", 1.0f, a);
    assert_int_equal(pop_float_dict(d, "x", NULL), NO_ERROR);
    assert_int_equal((int)float_dict_hash_size(d), 0);
    return_float_dict(d);
}
 
// ================================================================================
// Group 5: pop_float_dict_n
// ================================================================================
 
static void test_fd_pop_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal(pop_float_dict_n(d, "k", 0, NULL), INVALID_ARG);
    return_float_dict(d);
}
 
static void test_fd_pop_n_removes_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    const char* buf = "speed_max";
    insert_float_dict_n(d, buf, 5, 299792458.0f, a);   /* key = "speed" */
    float v = 0.0f;
    assert_int_equal(pop_float_dict_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 299792458.0f);
    assert_int_equal((int)float_dict_hash_size(d), 0);
    return_float_dict(d);
}
 
// ================================================================================
// Group 6: update_float_dict
// ================================================================================
 
static void test_fd_update_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(update_float_dict(NULL, "k", 1u), NULL_POINTER);
}
 
static void test_fd_update_null_key_returns_null_pointer(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal(update_float_dict(d, NULL, 1u), NULL_POINTER);
    return_float_dict(d);
}
 
static void test_fd_update_missing_key_returns_not_found(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal(update_float_dict(d, "missing", 1u), NOT_FOUND);
    return_float_dict(d);
}
 
static void test_fd_update_overwrites_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    insert_float_dict(d, "gravity", 9.80665f, a);
    assert_int_equal(update_float_dict(d, "gravity", 1.62f), NO_ERROR);
    float v = 0.0f;
    assert_int_equal(get_float_dict_value(d, "gravity", &v), NO_ERROR);
    assert_true(v == 1.62f);
    assert_int_equal((int)float_dict_hash_size(d), 1);
    return_float_dict(d);
}
 
// ================================================================================
// Group 7: update_float_dict_n
// ================================================================================
 
static void test_fd_update_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal(update_float_dict_n(d, "k", 0, 1u), INVALID_ARG);
    return_float_dict(d);
}
 
static void test_fd_update_n_updates_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    const char* buf = "speed_max";
    insert_float_dict_n(d, buf, 5, 299792458.0f, a);   /* key = "speed" */
    assert_int_equal(update_float_dict_n(d, buf, 5, 0.0f), NO_ERROR);
    float v = 1.0f;
    assert_int_equal(get_float_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 0.0f);
    return_float_dict(d);
}
 
// ================================================================================
// Group 8: get_float_dict_value
// ================================================================================
 
static void test_fd_get_null_dict_returns_null_pointer(void** state) {
    (void)state;
    float v;
    assert_int_equal(get_float_dict_value(NULL, "k", &v), NULL_POINTER);
}
 
static void test_fd_get_null_key_returns_null_pointer(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    float v;
    assert_int_equal(get_float_dict_value(d, NULL, &v), NULL_POINTER);
    return_float_dict(d);
}
 
static void test_fd_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    insert_float_dict(d, "k", 1.0f, a);
    assert_int_equal(get_float_dict_value(d, "k", NULL), NULL_POINTER);
    return_float_dict(d);
}
 
static void test_fd_get_missing_key_returns_not_found(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    float v;
    assert_int_equal(get_float_dict_value(d, "missing", &v), NOT_FOUND);
    return_float_dict(d);
}
 
static void test_fd_get_retrieves_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    insert_float_dict(d, "pi",      3.14159265f,  a);
    insert_float_dict(d, "neg",    -2.71828183f,  a);
    insert_float_dict(d, "zero",    0.0f,         a);
    float v = 0.0f;
    assert_int_equal(get_float_dict_value(d, "pi", &v), NO_ERROR);
    assert_true(v == 3.14159265f);
    assert_int_equal(get_float_dict_value(d, "neg", &v), NO_ERROR);
    assert_true(v == -2.71828183f);
    assert_int_equal(get_float_dict_value(d, "zero", &v), NO_ERROR);
    assert_true(v == 0.0f);
    return_float_dict(d);
}
 
// ================================================================================
// Group 9: get_float_dict_value_n
// ================================================================================
 
static void test_fd_get_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    float v;
    assert_int_equal(get_float_dict_value_n(d, "k", 0, &v), INVALID_ARG);
    return_float_dict(d);
}
 
static void test_fd_get_n_retrieves_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    const char* buf = "speed_max";
    insert_float_dict_n(d, buf, 5, 299792458.0f, a);   /* key = "speed" */
    float v = 0.0f;
    assert_int_equal(get_float_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 299792458.0f);
    assert_int_equal(get_float_dict_value_n(d, buf, 9, &v), NOT_FOUND);
    return_float_dict(d);
}
 
// ================================================================================
// Group 10: get_float_dict_ptr
// ================================================================================
 
static void test_fd_ptr_null_dict_returns_null(void** state) {
    (void)state;
    assert_null(get_float_dict_ptr(NULL, "k"));
}
 
static void test_fd_ptr_null_key_returns_null(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    assert_null(get_float_dict_ptr(d, NULL));
    return_float_dict(d);
}
 
static void test_fd_ptr_missing_returns_null(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    assert_null(get_float_dict_ptr(d, "missing"));
    return_float_dict(d);
}
 
static void test_fd_ptr_points_to_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    insert_float_dict(d, "port", 8080u, a);
    const float_t* p = get_float_dict_ptr(d, "port");
    assert_non_null(p);
    assert_int_equal((int)*p, 8080);
    return_float_dict(d);
}
 
// ================================================================================
// Group 11: get_float_dict_ptr_n
// ================================================================================
 
static void test_fd_ptr_n_zero_len_returns_null(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    assert_null(get_float_dict_ptr_n(d, "k", 0));
    return_float_dict(d);
}
 
static void test_fd_ptr_n_points_to_bounded_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    const char* buf = "port_http";
    insert_float_dict_n(d, buf, 4, 8080u, a);   /* key = "port" */
    const float_t* p = get_float_dict_ptr_n(d, buf, 4);
    assert_non_null(p);
    assert_int_equal((int)*p, 8080);
    return_float_dict(d);
}
 
// ================================================================================
// Group 12: has_float_dict_key
// ================================================================================
 
static void test_fd_has_null_dict_returns_false(void** state) {
    (void)state;
    assert_false(has_float_dict_key(NULL, "k"));
}
 
static void test_fd_has_null_key_returns_false(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    assert_false(has_float_dict_key(d, NULL));
    return_float_dict(d);
}
 
static void test_fd_has_present_and_absent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    insert_float_dict(d, "present", 1.0f, a);
    assert_true(has_float_dict_key(d, "present"));
    assert_false(has_float_dict_key(d, "absent"));
    return_float_dict(d);
}
 
// ================================================================================
// Group 13: has_float_dict_key_n
// ================================================================================
 
static void test_fd_has_n_zero_len_returns_false(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    assert_false(has_float_dict_key_n(d, "k", 0));
    return_float_dict(d);
}
 
static void test_fd_has_n_distinguishes_by_length(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    insert_float_dict_n(d, "abc", 2, 1.0f, a);   /* key = "ab" */
    insert_float_dict_n(d, "abc", 3, 2.0f, a);   /* key = "abc" */
    assert_true(has_float_dict_key_n(d, "abc", 2));
    assert_true(has_float_dict_key_n(d, "abc", 3));
    assert_false(has_float_dict_key_n(d, "abc", 1));
    return_float_dict(d);
}
 
// ================================================================================
// Group 14: clear_float_dict
// ================================================================================
 
static void test_fd_clear_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_float_dict(NULL), NULL_POINTER);
}
 
static void test_fd_clear_resets_and_is_reusable(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    insert_float_dict(d, "a", 1.0f, a);
    insert_float_dict(d, "b", 2.0f, a);
    assert_int_equal(clear_float_dict(d), NO_ERROR);
    assert_int_equal((int)float_dict_hash_size(d), 0);
    assert_true(is_float_dict_empty(d));
    assert_int_equal(insert_float_dict(d, "a", 3.14159265f, a), NO_ERROR);
    float v = 0.0f;
    assert_int_equal(get_float_dict_value(d, "a", &v), NO_ERROR);
    assert_true(v == 3.14159265f);
    return_float_dict(d);
}
 
// ================================================================================
// Group 15: copy_float_dict
// ================================================================================
 
static void test_fd_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_expect_t r = copy_float_dict(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_fd_copy_is_independent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* orig = _make_float_dict(4);
    insert_float_dict(orig, "x", 3.14159265f, a);
    insert_float_dict(orig, "y", 9.80665f, a);
 
    float_dict_expect_t cr = copy_float_dict(orig, a);
    assert_true(cr.has_value);
 
    update_float_dict(orig, "x", FLT_MAX);
 
    float v = 0.0f;
    assert_int_equal(get_float_dict_value(cr.u.value, "x", &v), NO_ERROR);
    assert_true(v == 3.14159265f);
    assert_int_equal((int)float_dict_hash_size(cr.u.value), 2);
 
    return_float_dict(cr.u.value);
    return_float_dict(orig);
}
 
// ================================================================================
// Group 16: merge_float_dict
// ================================================================================
 
static void test_fd_merge_null_first_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    float_dict_expect_t r = merge_float_dict(NULL, d, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_float_dict(d);
}
 
static void test_fd_merge_no_overwrite_keeps_first_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* da = _make_float_dict(4);
    float_dict_t* db = _make_float_dict(4);
    insert_float_dict(da, "shared", 3.14159265f, a);
    insert_float_dict(db, "shared", 9.80665f, a);
    insert_float_dict(db, "new",    1.62f, a);
 
    float_dict_expect_t mr = merge_float_dict(da, db, false, a);
    assert_true(mr.has_value);
    float v = 0.0f;
    assert_int_equal(get_float_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_true(v == 3.14159265f);
    assert_int_equal(get_float_dict_value(mr.u.value, "new", &v), NO_ERROR);
    assert_true(v == 1.62f);
 
    return_float_dict(mr.u.value);
    return_float_dict(da);
    return_float_dict(db);
}
 
static void test_fd_merge_overwrite_uses_second_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* da = _make_float_dict(4);
    float_dict_t* db = _make_float_dict(4);
    insert_float_dict(da, "shared", 3.14159265f, a);
    insert_float_dict(db, "shared", 9.80665f, a);
 
    float_dict_expect_t mr = merge_float_dict(da, db, true, a);
    assert_true(mr.has_value);
    float v = 0.0f;
    assert_int_equal(get_float_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_true(v == 9.80665f);
 
    return_float_dict(mr.u.value);
    return_float_dict(da);
    return_float_dict(db);
}
 
// ================================================================================
// Group 17: foreach_float_dict
// ================================================================================
 
static void test_fd_foreach_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(foreach_float_dict(NULL, _fd_sum_iter, NULL),
                     NULL_POINTER);
}
 
static void test_fd_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    float_dict_t* d = _make_float_dict(4);
    assert_int_equal(foreach_float_dict(d, NULL, NULL), NULL_POINTER);
    return_float_dict(d);
}
 
static void test_fd_foreach_correct_sum(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    insert_float_dict(d, "a", 1.0f, a);
    insert_float_dict(d, "b", 2.0f, a);
    insert_float_dict(d, "c", 3.0f, a);
    _fd_iter_ctx_t ctx = { 0, 0.0 };
    assert_int_equal(foreach_float_dict(d, _fd_sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 3);
    assert_true(ctx.sum == 6.0);
    return_float_dict(d);
}
 
static void test_fd_foreach_delivers_negative_float(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    insert_float_dict(d, "pos",  1.5f, a);
    insert_float_dict(d, "neg", -2.5f, a);
    _fd_iter_ctx_t ctx = { 0, 0.0 };
    foreach_float_dict(d, _fd_sum_iter, &ctx);
    assert_int_equal(ctx.count, 2);
    /* 1.5 + (-2.5) == -1.0 */
    assert_true(ctx.sum == -1.0);
    return_float_dict(d);
}
 
// ================================================================================
// Group 18: introspection
// ================================================================================
 
static void test_fd_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)float_dict_size(NULL), 0);
}
 
static void test_fd_hash_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)float_dict_hash_size(NULL), 0);
}
 
static void test_fd_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)float_dict_alloc(NULL), 0);
}
 
static void test_fd_is_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_float_dict_empty(NULL));
}
 
static void test_fd_is_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_t* d = _make_float_dict(4);
    assert_true(is_float_dict_empty(d));
    insert_float_dict(d, "x", 1u, a);
    assert_false(is_float_dict_empty(d));
    pop_float_dict(d, "x", NULL);
    assert_true(is_float_dict_empty(d));
    return_float_dict(d);
}
 
// ================================================================================
// Group 19: growth stress
// ================================================================================
 
static void test_fd_growth_values_survive_resize(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_dict_expect_t r = init_float_dict(2, true, a);
    assert_true(r.has_value);
    float_dict_t* d = r.u.value;
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        float val = (float_t)(i * 300u);   /* exercises > 255 values */
        assert_int_equal(insert_float_dict(d, key, val, a), NO_ERROR);
    }
    assert_int_equal((int)float_dict_hash_size(d), 200);
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        float_t expected = (float_t)(i * 300u);
        float v = 0u;
        assert_int_equal(get_float_dict_value(d, key, &v), NO_ERROR);
        assert_true(v == expected);
    }
 
    return_float_dict(d);
}
 
// ================================================================================
// main
// ================================================================================
 
const struct CMUnitTest test_float_dict[] = {

    /* Group 1: init / return */
    cmocka_unit_test(test_fd_init_null_allocator_fails),
    cmocka_unit_test(test_fd_init_zero_capacity_fails),
    cmocka_unit_test(test_fd_init_data_size_is_four_bytes),
    cmocka_unit_test(test_fd_init_dtype_is_float),
    cmocka_unit_test(test_fd_return_null_is_safe),

    /* Group 2: insert */
    cmocka_unit_test(test_fd_insert_null_dict_returns_null_pointer),
    cmocka_unit_test(test_fd_insert_null_key_returns_null_pointer),
    cmocka_unit_test(test_fd_insert_duplicate_returns_invalid_arg),
    cmocka_unit_test(test_fd_insert_stores_zero),
    cmocka_unit_test(test_fd_insert_stores_float_extremes),
    cmocka_unit_test(test_fd_insert_stores_signed_float),

    /* Group 3: insert_n */
    cmocka_unit_test(test_fd_insert_n_null_key_returns_null_pointer),
    cmocka_unit_test(test_fd_insert_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_fd_insert_n_uses_only_specified_bytes),

    /* Group 4: pop */
    cmocka_unit_test(test_fd_pop_null_dict_returns_null_pointer),
    cmocka_unit_test(test_fd_pop_null_key_returns_null_pointer),
    cmocka_unit_test(test_fd_pop_missing_key_returns_not_found),
    cmocka_unit_test(test_fd_pop_returns_correct_value),
    cmocka_unit_test(test_fd_pop_null_out_value_discards_safely),

    /* Group 5: pop_n */
    cmocka_unit_test(test_fd_pop_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_fd_pop_n_removes_bounded_key),

    /* Group 6: update */
    cmocka_unit_test(test_fd_update_null_dict_returns_null_pointer),
    cmocka_unit_test(test_fd_update_null_key_returns_null_pointer),
    cmocka_unit_test(test_fd_update_missing_key_returns_not_found),
    cmocka_unit_test(test_fd_update_overwrites_value),

    /* Group 7: update_n */
    cmocka_unit_test(test_fd_update_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_fd_update_n_updates_bounded_key),

    /* Group 8: get_value */
    cmocka_unit_test(test_fd_get_null_dict_returns_null_pointer),
    cmocka_unit_test(test_fd_get_null_key_returns_null_pointer),
    cmocka_unit_test(test_fd_get_null_out_returns_null_pointer),
    cmocka_unit_test(test_fd_get_missing_key_returns_not_found),
    cmocka_unit_test(test_fd_get_retrieves_extremes),

    /* Group 9: get_value_n */
    cmocka_unit_test(test_fd_get_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_fd_get_n_retrieves_bounded_key),

    /* Group 10: get_ptr */
    cmocka_unit_test(test_fd_ptr_null_dict_returns_null),
    cmocka_unit_test(test_fd_ptr_null_key_returns_null),
    cmocka_unit_test(test_fd_ptr_missing_returns_null),
    cmocka_unit_test(test_fd_ptr_points_to_correct_value),

    /* Group 11: get_ptr_n */
    cmocka_unit_test(test_fd_ptr_n_zero_len_returns_null),
    cmocka_unit_test(test_fd_ptr_n_points_to_bounded_value),

    /* Group 12: has_key */
    cmocka_unit_test(test_fd_has_null_dict_returns_false),
    cmocka_unit_test(test_fd_has_null_key_returns_false),
    cmocka_unit_test(test_fd_has_present_and_absent),

    /* Group 13: has_key_n */
    cmocka_unit_test(test_fd_has_n_zero_len_returns_false),
    cmocka_unit_test(test_fd_has_n_distinguishes_by_length),

    /* Group 14: clear */
    cmocka_unit_test(test_fd_clear_null_returns_null_pointer),
    cmocka_unit_test(test_fd_clear_resets_and_is_reusable),

    /* Group 15: copy */
    cmocka_unit_test(test_fd_copy_null_src_fails),
    cmocka_unit_test(test_fd_copy_is_independent),

    /* Group 16: merge */
    cmocka_unit_test(test_fd_merge_null_first_fails),
    cmocka_unit_test(test_fd_merge_no_overwrite_keeps_first_value),
    cmocka_unit_test(test_fd_merge_overwrite_uses_second_value),

    /* Group 17: foreach */
    cmocka_unit_test(test_fd_foreach_null_dict_returns_null_pointer),
    cmocka_unit_test(test_fd_foreach_null_fn_returns_null_pointer),
    cmocka_unit_test(test_fd_foreach_correct_sum),
    cmocka_unit_test(test_fd_foreach_delivers_negative_float),

    /* Group 18: introspection */
    cmocka_unit_test(test_fd_size_null_returns_zero),
    cmocka_unit_test(test_fd_hash_size_null_returns_zero),
    cmocka_unit_test(test_fd_alloc_null_returns_zero),
    cmocka_unit_test(test_fd_is_empty_null_returns_true),
    cmocka_unit_test(test_fd_is_empty_reflects_contents),

    /* Group 19: growth stress */
    cmocka_unit_test(test_fd_growth_values_survive_resize),
};
 
const size_t test_float_dict_count = sizeof(test_float_dict) / sizeof(test_float_dict[0]);
// ================================================================================ 
// ================================================================================ 


static double_dict_t* _make_double_dict(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    double_dict_expect_t r = init_double_dict(cap, true, a);
    assert_true(r.has_value);
    return r.u.value;
}
 
typedef struct { int count; double sum; } _dd_iter_ctx_t;
 
static void _dd_sum_iter(const char* key, size_t key_len,
                          double value, void* ud) {
    (void)key; (void)key_len;
    _dd_iter_ctx_t* c = (_dd_iter_ctx_t*)ud;
    c->count++;
    c->sum += value;
}
 
// ================================================================================
// Group 1: init_double_dict / return_double_dict
// ================================================================================
 
static void test_dd_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    double_dict_expect_t r = init_double_dict(8, true, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_dd_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_expect_t r = init_double_dict(0, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_dd_init_data_size_is_eight_bytes(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal((int)dict_data_size(d), 8);
    return_double_dict(d);
}
 
static void test_dd_init_dtype_is_float(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal((int)d->dtype, (int)DOUBLE_TYPE);
    return_double_dict(d);
}
 
static void test_dd_return_null_is_safe(void** state) {
    (void)state;
    return_double_dict(NULL);
}
 
// ================================================================================
// Group 2: insert_double_dict
// ================================================================================
 
static void test_dd_insert_null_dict_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    assert_int_equal(insert_double_dict(NULL, "k", 1u, a), NULL_POINTER);
}
 
static void test_dd_insert_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal(insert_double_dict(d, NULL, 1u, a), NULL_POINTER);
    return_double_dict(d);
}
 
static void test_dd_insert_duplicate_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal(insert_double_dict(d, "k", 1u, a), NO_ERROR);
    assert_int_equal(insert_double_dict(d, "k", 2u, a), INVALID_ARG);
    assert_int_equal((int)double_dict_hash_size(d), 1);
    return_double_dict(d);
}
 
static void test_dd_insert_stores_zero(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal(insert_double_dict(d, "zero", 0.0, a), NO_ERROR);
    double v = 99.0;
    assert_int_equal(get_double_dict_value(d, "zero", &v), NO_ERROR);
    assert_true(v == 0.0);
    return_double_dict(d);
}
 
static void test_dd_insert_stores_double_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal(insert_double_dict(d, "max",  DBL_MAX, a), NO_ERROR);
    assert_int_equal(insert_double_dict(d, "min", -DBL_MAX, a), NO_ERROR);
    assert_int_equal(insert_double_dict(d, "tiny", DBL_MIN, a), NO_ERROR);
    double v = 0.0;
    assert_int_equal(get_double_dict_value(d, "max", &v), NO_ERROR);
    assert_true(v == DBL_MAX);
    assert_int_equal(get_double_dict_value(d, "min", &v), NO_ERROR);
    assert_true(v == -DBL_MAX);
    assert_int_equal(get_double_dict_value(d, "tiny", &v), NO_ERROR);
    assert_true(v == DBL_MIN);
    return_double_dict(d);
}
 
static void test_dd_insert_stores_signed_double(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal(insert_double_dict(d, "pi",      3.141592653589793, a), NO_ERROR);
    assert_int_equal(insert_double_dict(d, "gravity", 9.80665,           a), NO_ERROR);
    assert_int_equal(insert_double_dict(d, "neg",    -1.5,               a), NO_ERROR);
    double v = 0.0;
    assert_int_equal(get_double_dict_value(d, "pi", &v), NO_ERROR);
    assert_true(v == 3.141592653589793);
    assert_int_equal(get_double_dict_value(d, "gravity", &v), NO_ERROR);
    assert_true(v == 9.80665);
    assert_int_equal(get_double_dict_value(d, "neg", &v), NO_ERROR);
    assert_true(v == -1.5);
    return_double_dict(d);
}
 
// ================================================================================
// Group 3: insert_double_dict_n
// ================================================================================
 
static void test_dd_insert_n_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal(insert_double_dict_n(d, NULL, 3, 1.0, a), NULL_POINTER);
    return_double_dict(d);
}
 
static void test_dd_insert_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal(insert_double_dict_n(d, "k", 0, 1.0, a), INVALID_ARG);
    return_double_dict(d);
}
 
static void test_dd_insert_n_uses_only_specified_bytes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    /* "speed_max" — insert only "speed" (5 bytes) with value c (m/s) */
    const char* buf = "speed_max";
    assert_int_equal(insert_double_dict_n(d, buf, 5, 299792458.0, a), NO_ERROR);
    double v = 0.0;
    assert_int_equal(get_double_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 299792458.0);
    /* Full buffer key "speed_max" is different — not found */
    assert_int_equal(get_double_dict_value_n(d, buf, 9, &v), NOT_FOUND);
    /* Plain null-terminated lookup also works */
    assert_int_equal(get_double_dict_value(d, "speed", &v), NO_ERROR);
    assert_true(v == 299792458.0);
    return_double_dict(d);
}
 
// ================================================================================
// Group 4: pop_double_dict
// ================================================================================
 
static void test_dd_pop_null_dict_returns_null_pointer(void** state) {
    (void)state;
    double v;
    assert_int_equal(pop_double_dict(NULL, "k", &v), NULL_POINTER);
}
 
static void test_dd_pop_null_key_returns_null_pointer(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    double v;
    assert_int_equal(pop_double_dict(d, NULL, &v), NULL_POINTER);
    return_double_dict(d);
}
 
static void test_dd_pop_missing_key_returns_not_found(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal(pop_double_dict(d, "missing", NULL), NOT_FOUND);
    return_double_dict(d);
}
 
static void test_dd_pop_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    insert_double_dict(d, "gravity", 9.80665, a);
    double v = 0.0;
    assert_int_equal(pop_double_dict(d, "gravity", &v), NO_ERROR);
    assert_true(v == 9.80665);
    assert_int_equal((int)double_dict_hash_size(d), 0);
    return_double_dict(d);
}
 
static void test_dd_pop_null_out_value_discards_safely(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    insert_double_dict(d, "x", 1.0, a);
    assert_int_equal(pop_double_dict(d, "x", NULL), NO_ERROR);
    assert_int_equal((int)double_dict_hash_size(d), 0);
    return_double_dict(d);
}
 
// ================================================================================
// Group 5: pop_double_dict_n
// ================================================================================
 
static void test_dd_pop_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal(pop_double_dict_n(d, "k", 0, NULL), INVALID_ARG);
    return_double_dict(d);
}
 
static void test_dd_pop_n_removes_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    const char* buf = "speed_max";
    insert_double_dict_n(d, buf, 5, 299792458.0, a);   /* key = "speed" */
    double v = 0.0;
    assert_int_equal(pop_double_dict_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 299792458.0);
    assert_int_equal((int)double_dict_hash_size(d), 0);
    return_double_dict(d);
}
 
// ================================================================================
// Group 6: update_double_dict
// ================================================================================
 
static void test_dd_update_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(update_double_dict(NULL, "k", 1u), NULL_POINTER);
}
 
static void test_dd_update_null_key_returns_null_pointer(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal(update_double_dict(d, NULL, 1u), NULL_POINTER);
    return_double_dict(d);
}
 
static void test_dd_update_missing_key_returns_not_found(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal(update_double_dict(d, "missing", 1u), NOT_FOUND);
    return_double_dict(d);
}
 
static void test_dd_update_overwrites_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    insert_double_dict(d, "gravity", 9.80665, a);
    assert_int_equal(update_double_dict(d, "gravity", 1.62), NO_ERROR);
    double v = 0.0;
    assert_int_equal(get_double_dict_value(d, "gravity", &v), NO_ERROR);
    assert_true(v == 1.62);
    assert_int_equal((int)double_dict_hash_size(d), 1);
    return_double_dict(d);
}
 
// ================================================================================
// Group 7: update_double_dict_n
// ================================================================================
 
static void test_dd_update_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal(update_double_dict_n(d, "k", 0, 1u), INVALID_ARG);
    return_double_dict(d);
}
 
static void test_dd_update_n_updates_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    const char* buf = "speed_max";
    insert_double_dict_n(d, buf, 5, 299792458.0, a);   /* key = "speed" */
    assert_int_equal(update_double_dict_n(d, buf, 5, 0.0), NO_ERROR);
    double v = 1.0;
    assert_int_equal(get_double_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 0.0);
    return_double_dict(d);
}
 
// ================================================================================
// Group 8: get_double_dict_value
// ================================================================================
 
static void test_dd_get_null_dict_returns_null_pointer(void** state) {
    (void)state;
    double v;
    assert_int_equal(get_double_dict_value(NULL, "k", &v), NULL_POINTER);
}
 
static void test_dd_get_null_key_returns_null_pointer(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    double v;
    assert_int_equal(get_double_dict_value(d, NULL, &v), NULL_POINTER);
    return_double_dict(d);
}
 
static void test_dd_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    insert_double_dict(d, "k", 1.0, a);
    assert_int_equal(get_double_dict_value(d, "k", NULL), NULL_POINTER);
    return_double_dict(d);
}
 
static void test_dd_get_missing_key_returns_not_found(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    double v;
    assert_int_equal(get_double_dict_value(d, "missing", &v), NOT_FOUND);
    return_double_dict(d);
}
 
static void test_dd_get_retrieves_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    insert_double_dict(d, "pi",      3.141592653589793, a);
    insert_double_dict(d, "neg",    -2.718281828459045, a);
    insert_double_dict(d, "zero",    0.0,               a);
    double v = 0.0;
    assert_int_equal(get_double_dict_value(d, "pi", &v), NO_ERROR);
    assert_true(v == 3.141592653589793);
    assert_int_equal(get_double_dict_value(d, "neg", &v), NO_ERROR);
    assert_true(v == -2.718281828459045);
    assert_int_equal(get_double_dict_value(d, "zero", &v), NO_ERROR);
    assert_true(v == 0.0);
    return_double_dict(d);
}
 
// ================================================================================
// Group 9: get_double_dict_value_n
// ================================================================================
 
static void test_dd_get_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    double v;
    assert_int_equal(get_double_dict_value_n(d, "k", 0, &v), INVALID_ARG);
    return_double_dict(d);
}
 
static void test_dd_get_n_retrieves_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    const char* buf = "speed_max";
    insert_double_dict_n(d, buf, 5, 299792458.0, a);   /* key = "speed" */
    double v = 0.0;
    assert_int_equal(get_double_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 299792458.0);
    assert_int_equal(get_double_dict_value_n(d, buf, 9, &v), NOT_FOUND);
    return_double_dict(d);
}
 
// ================================================================================
// Group 10: get_double_dict_ptr
// ================================================================================
 
static void test_dd_ptr_null_dict_returns_null(void** state) {
    (void)state;
    assert_null(get_double_dict_ptr(NULL, "k"));
}
 
static void test_dd_ptr_null_key_returns_null(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    assert_null(get_double_dict_ptr(d, NULL));
    return_double_dict(d);
}
 
static void test_dd_ptr_missing_returns_null(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    assert_null(get_double_dict_ptr(d, "missing"));
    return_double_dict(d);
}
 
static void test_dd_ptr_points_to_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    insert_double_dict(d, "port", 8080u, a);
    const double* p = get_double_dict_ptr(d, "port");
    assert_non_null(p);
    assert_int_equal((int)*p, 8080);
    return_double_dict(d);
}
 
// ================================================================================
// Group 11: get_double_dict_ptr_n
// ================================================================================
 
static void test_dd_ptr_n_zero_len_returns_null(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    assert_null(get_double_dict_ptr_n(d, "k", 0));
    return_double_dict(d);
}
 
static void test_dd_ptr_n_points_to_bounded_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    const char* buf = "port_http";
    insert_double_dict_n(d, buf, 4, 8080u, a);   /* key = "port" */
    const double* p = get_double_dict_ptr_n(d, buf, 4);
    assert_non_null(p);
    assert_int_equal((int)*p, 8080);
    return_double_dict(d);
}
 
// ================================================================================
// Group 12: has_double_dict_key
// ================================================================================
 
static void test_dd_has_null_dict_returns_false(void** state) {
    (void)state;
    assert_false(has_double_dict_key(NULL, "k"));
}
 
static void test_dd_has_null_key_returns_false(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    assert_false(has_double_dict_key(d, NULL));
    return_double_dict(d);
}
 
static void test_dd_has_present_and_absent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    insert_double_dict(d, "present", 1.0, a);
    assert_true(has_double_dict_key(d, "present"));
    assert_false(has_double_dict_key(d, "absent"));
    return_double_dict(d);
}
 
// ================================================================================
// Group 13: has_double_dict_key_n
// ================================================================================
 
static void test_dd_has_n_zero_len_returns_false(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    assert_false(has_double_dict_key_n(d, "k", 0));
    return_double_dict(d);
}
 
static void test_dd_has_n_distinguishes_by_length(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    insert_double_dict_n(d, "abc", 2, 1.0, a);   /* key = "ab" */
    insert_double_dict_n(d, "abc", 3, 2.0, a);   /* key = "abc" */
    assert_true(has_double_dict_key_n(d, "abc", 2));
    assert_true(has_double_dict_key_n(d, "abc", 3));
    assert_false(has_double_dict_key_n(d, "abc", 1));
    return_double_dict(d);
}
 
// ================================================================================
// Group 14: clear_double_dict
// ================================================================================
 
static void test_dd_clear_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_double_dict(NULL), NULL_POINTER);
}
 
static void test_dd_clear_resets_and_is_reusable(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    insert_double_dict(d, "a", 1.0, a);
    insert_double_dict(d, "b", 2.0, a);
    assert_int_equal(clear_double_dict(d), NO_ERROR);
    assert_int_equal((int)double_dict_hash_size(d), 0);
    assert_true(is_double_dict_empty(d));
    assert_int_equal(insert_double_dict(d, "a", 3.141592653589793, a), NO_ERROR);
    double v = 0.0;
    assert_int_equal(get_double_dict_value(d, "a", &v), NO_ERROR);
    assert_true(v == 3.141592653589793);
    return_double_dict(d);
}
 
// ================================================================================
// Group 15: copy_double_dict
// ================================================================================
 
static void test_dd_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_expect_t r = copy_double_dict(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_dd_copy_is_independent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* orig = _make_double_dict(4);
    insert_double_dict(orig, "x", 3.141592653589793, a);
    insert_double_dict(orig, "y", 9.80665, a);
 
    double_dict_expect_t cr = copy_double_dict(orig, a);
    assert_true(cr.has_value);
 
    update_double_dict(orig, "x", DBL_MAX);
 
    double v = 0.0;
    assert_int_equal(get_double_dict_value(cr.u.value, "x", &v), NO_ERROR);
    assert_true(v == 3.141592653589793);
    assert_int_equal((int)double_dict_hash_size(cr.u.value), 2);
 
    return_double_dict(cr.u.value);
    return_double_dict(orig);
}
 
// ================================================================================
// Group 16: merge_double_dict
// ================================================================================
 
static void test_dd_merge_null_first_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    double_dict_expect_t r = merge_double_dict(NULL, d, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_double_dict(d);
}
 
static void test_dd_merge_no_overwrite_keeps_first_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* da = _make_double_dict(4);
    double_dict_t* db = _make_double_dict(4);
    insert_double_dict(da, "shared", 3.141592653589793, a);
    insert_double_dict(db, "shared", 9.80665, a);
    insert_double_dict(db, "new",    1.62, a);
 
    double_dict_expect_t mr = merge_double_dict(da, db, false, a);
    assert_true(mr.has_value);
    double v = 0.0;
    assert_int_equal(get_double_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_true(v == 3.141592653589793);
    assert_int_equal(get_double_dict_value(mr.u.value, "new", &v), NO_ERROR);
    assert_true(v == 1.62);
 
    return_double_dict(mr.u.value);
    return_double_dict(da);
    return_double_dict(db);
}
 
static void test_dd_merge_overwrite_uses_second_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* da = _make_double_dict(4);
    double_dict_t* db = _make_double_dict(4);
    insert_double_dict(da, "shared", 3.141592653589793, a);
    insert_double_dict(db, "shared", 9.80665, a);
 
    double_dict_expect_t mr = merge_double_dict(da, db, true, a);
    assert_true(mr.has_value);
    double v = 0.0;
    assert_int_equal(get_double_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_true(v == 9.80665);
 
    return_double_dict(mr.u.value);
    return_double_dict(da);
    return_double_dict(db);
}
 
// ================================================================================
// Group 17: foreach_double_dict
// ================================================================================
 
static void test_dd_foreach_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(foreach_double_dict(NULL, _dd_sum_iter, NULL),
                     NULL_POINTER);
}
 
static void test_dd_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    double_dict_t* d = _make_double_dict(4);
    assert_int_equal(foreach_double_dict(d, NULL, NULL), NULL_POINTER);
    return_double_dict(d);
}
 
static void test_dd_foreach_correct_sum(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    insert_double_dict(d, "a", 1.0, a);
    insert_double_dict(d, "b", 2.0, a);
    insert_double_dict(d, "c", 3.0, a);
    _dd_iter_ctx_t ctx = { 0, 0.0 };
    assert_int_equal(foreach_double_dict(d, _dd_sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 3);
    assert_true(ctx.sum == 6.0);
    return_double_dict(d);
}
 
static void test_dd_foreach_delivers_negative_double(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    insert_double_dict(d, "pos",  1.5, a);
    insert_double_dict(d, "neg", -2.5, a);
    _dd_iter_ctx_t ctx = { 0, 0.0 };
    foreach_double_dict(d, _dd_sum_iter, &ctx);
    assert_int_equal(ctx.count, 2);
    /* 1.5 + (-2.5) == -1.0 */
    assert_true(ctx.sum == -1.0);
    return_double_dict(d);
}
 
// ================================================================================
// Group 18: introspection
// ================================================================================
 
static void test_dd_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)double_dict_size(NULL), 0);
}
 
static void test_dd_hash_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)double_dict_hash_size(NULL), 0);
}
 
static void test_dd_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)double_dict_alloc(NULL), 0);
}
 
static void test_dd_is_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_double_dict_empty(NULL));
}
 
static void test_dd_is_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_t* d = _make_double_dict(4);
    assert_true(is_double_dict_empty(d));
    insert_double_dict(d, "x", 1u, a);
    assert_false(is_double_dict_empty(d));
    pop_double_dict(d, "x", NULL);
    assert_true(is_double_dict_empty(d));
    return_double_dict(d);
}
 
// ================================================================================
// Group 19: growth stress
// ================================================================================
 
static void test_dd_growth_values_survive_resize(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_dict_expect_t r = init_double_dict(2, true, a);
    assert_true(r.has_value);
    double_dict_t* d = r.u.value;
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        double val = (float_t)(i * 300u);   /* exercises > 255 values */
        assert_int_equal(insert_double_dict(d, key, val, a), NO_ERROR);
    }
    assert_int_equal((int)double_dict_hash_size(d), 200);
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        float_t expected = (float_t)(i * 300u);
        double v = 0u;
        assert_int_equal(get_double_dict_value(d, key, &v), NO_ERROR);
        assert_true(v == expected);
    }
 
    return_double_dict(d);
}
 
// ================================================================================
// main
// ================================================================================
 
const struct CMUnitTest test_double_dict[] = {

    /* Group 1: init / return */
    cmocka_unit_test(test_dd_init_null_allocator_fails),
    cmocka_unit_test(test_dd_init_zero_capacity_fails),
    cmocka_unit_test(test_dd_init_data_size_is_eight_bytes),
    cmocka_unit_test(test_dd_init_dtype_is_float),
    cmocka_unit_test(test_dd_return_null_is_safe),

    /* Group 2: insert */
    cmocka_unit_test(test_dd_insert_null_dict_returns_null_pointer),
    cmocka_unit_test(test_dd_insert_null_key_returns_null_pointer),
    cmocka_unit_test(test_dd_insert_duplicate_returns_invalid_arg),
    cmocka_unit_test(test_dd_insert_stores_zero),
    cmocka_unit_test(test_dd_insert_stores_double_extremes),
    cmocka_unit_test(test_dd_insert_stores_signed_double),

    /* Group 3: insert_n */
    cmocka_unit_test(test_dd_insert_n_null_key_returns_null_pointer),
    cmocka_unit_test(test_dd_insert_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_dd_insert_n_uses_only_specified_bytes),

    /* Group 4: pop */
    cmocka_unit_test(test_dd_pop_null_dict_returns_null_pointer),
    cmocka_unit_test(test_dd_pop_null_key_returns_null_pointer),
    cmocka_unit_test(test_dd_pop_missing_key_returns_not_found),
    cmocka_unit_test(test_dd_pop_returns_correct_value),
    cmocka_unit_test(test_dd_pop_null_out_value_discards_safely),

    /* Group 5: pop_n */
    cmocka_unit_test(test_dd_pop_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_dd_pop_n_removes_bounded_key),

    /* Group 6: update */
    cmocka_unit_test(test_dd_update_null_dict_returns_null_pointer),
    cmocka_unit_test(test_dd_update_null_key_returns_null_pointer),
    cmocka_unit_test(test_dd_update_missing_key_returns_not_found),
    cmocka_unit_test(test_dd_update_overwrites_value),

    /* Group 7: update_n */
    cmocka_unit_test(test_dd_update_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_dd_update_n_updates_bounded_key),

    /* Group 8: get_value */
    cmocka_unit_test(test_dd_get_null_dict_returns_null_pointer),
    cmocka_unit_test(test_dd_get_null_key_returns_null_pointer),
    cmocka_unit_test(test_dd_get_null_out_returns_null_pointer),
    cmocka_unit_test(test_dd_get_missing_key_returns_not_found),
    cmocka_unit_test(test_dd_get_retrieves_extremes),

    /* Group 9: get_value_n */
    cmocka_unit_test(test_dd_get_n_zero_len_returns_invalid_arg),
    cmocka_unit_test(test_dd_get_n_retrieves_bounded_key),

    /* Group 10: get_ptr */
    cmocka_unit_test(test_dd_ptr_null_dict_returns_null),
    cmocka_unit_test(test_dd_ptr_null_key_returns_null),
    cmocka_unit_test(test_dd_ptr_missing_returns_null),
    cmocka_unit_test(test_dd_ptr_points_to_correct_value),

    /* Group 11: get_ptr_n */
    cmocka_unit_test(test_dd_ptr_n_zero_len_returns_null),
    cmocka_unit_test(test_dd_ptr_n_points_to_bounded_value),

    /* Group 12: has_key */
    cmocka_unit_test(test_dd_has_null_dict_returns_false),
    cmocka_unit_test(test_dd_has_null_key_returns_false),
    cmocka_unit_test(test_dd_has_present_and_absent),

    /* Group 13: has_key_n */
    cmocka_unit_test(test_dd_has_n_zero_len_returns_false),
    cmocka_unit_test(test_dd_has_n_distinguishes_by_length),

    /* Group 14: clear */
    cmocka_unit_test(test_dd_clear_null_returns_null_pointer),
    cmocka_unit_test(test_dd_clear_resets_and_is_reusable),

    /* Group 15: copy */
    cmocka_unit_test(test_dd_copy_null_src_fails),
    cmocka_unit_test(test_dd_copy_is_independent),

    /* Group 16: merge */
    cmocka_unit_test(test_dd_merge_null_first_fails),
    cmocka_unit_test(test_dd_merge_no_overwrite_keeps_first_value),
    cmocka_unit_test(test_dd_merge_overwrite_uses_second_value),

    /* Group 17: foreach */
    cmocka_unit_test(test_dd_foreach_null_dict_returns_null_pointer),
    cmocka_unit_test(test_dd_foreach_null_fn_returns_null_pointer),
    cmocka_unit_test(test_dd_foreach_correct_sum),
    cmocka_unit_test(test_dd_foreach_delivers_negative_double),

    /* Group 18: introspection */
    cmocka_unit_test(test_dd_size_null_returns_zero),
    cmocka_unit_test(test_dd_hash_size_null_returns_zero),
    cmocka_unit_test(test_dd_alloc_null_returns_zero),
    cmocka_unit_test(test_dd_is_empty_null_returns_true),
    cmocka_unit_test(test_dd_is_empty_reflects_contents),

    /* Group 19: growth stress */
    cmocka_unit_test(test_dd_growth_values_survive_resize),

};
const size_t test_double_dict_count = sizeof(test_double_dict) / sizeof(test_double_dict[0]);
// ================================================================================ 
// ================================================================================ 

static ldouble_dict_t* _make_ldouble_dict(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_expect_t r = init_ldouble_dict(cap, true, a);
    assert_true(r.has_value);
    return r.u.value;
}
 
typedef struct { int count; long double sum; } _ld_iter_ctx_t;
 
static void _ld_sum_iter(const char* key, size_t key_len,
                          long double value, void* ud) {
    (void)key; (void)key_len;
    _ld_iter_ctx_t* c = (_ld_iter_ctx_t*)ud;
    c->count++;
    c->sum += value;
}
 
// ================================================================================
// Group 1: init_ldouble_dict / return_ldouble_dict
// ================================================================================
 
static void test_ld_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    ldouble_dict_expect_t r = init_ldouble_dict(8, true, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_ld_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_expect_t r = init_ldouble_dict(0, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_ld_init_data_size_is_sizeof_long_double(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal((int)dict_data_size(d), (int)sizeof(long double));
    return_ldouble_dict(d);
}
 
static void test_ld_init_dtype_is_float(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal((int)d->dtype, (int)LDOUBLE_TYPE);
    return_ldouble_dict(d);
}
 
static void test_ld_return_null_is_safe(void** state) {
    (void)state;
    return_ldouble_dict(NULL);
}
 
// ================================================================================
// Group 2: insert_ldouble_dict
// ================================================================================
 
static void test_ld_insert_null_dict_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    assert_int_equal(insert_ldouble_dict(NULL, "k", 1u, a), NULL_POINTER);
}
 
static void test_ld_insert_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal(insert_ldouble_dict(d, NULL, 1u, a), NULL_POINTER);
    return_ldouble_dict(d);
}
 
static void test_ld_insert_duplicate_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal(insert_ldouble_dict(d, "k", 1u, a), NO_ERROR);
    assert_int_equal(insert_ldouble_dict(d, "k", 2u, a), INVALID_ARG);
    assert_int_equal((int)ldouble_dict_hash_size(d), 1);
    return_ldouble_dict(d);
}
 
static void test_ld_insert_stores_zero(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal(insert_ldouble_dict(d, "zero", 0.0L, a), NO_ERROR);
    long double v = 99.0L;
    assert_int_equal(get_ldouble_dict_value(d, "zero", &v), NO_ERROR);
    assert_true(v == 0.0L);
    return_ldouble_dict(d);
}
 
static void test_ld_insert_stores_ldouble_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal(insert_ldouble_dict(d, "max",  LDBL_MAX, a), NO_ERROR);
    assert_int_equal(insert_ldouble_dict(d, "min", -LDBL_MAX, a), NO_ERROR);
    assert_int_equal(insert_ldouble_dict(d, "tiny", LDBL_MIN, a), NO_ERROR);
    long double v = 0.0L;
    assert_int_equal(get_ldouble_dict_value(d, "max", &v), NO_ERROR);
    assert_true(v == LDBL_MAX);
    assert_int_equal(get_ldouble_dict_value(d, "min", &v), NO_ERROR);
    assert_true(v == -LDBL_MAX);
    assert_int_equal(get_ldouble_dict_value(d, "tiny", &v), NO_ERROR);
    assert_true(v == LDBL_MIN);
    return_ldouble_dict(d);
}
 
static void test_ld_insert_stores_signed_ldouble(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal(insert_ldouble_dict(d, "pi",      3.14159265358979323846L, a), NO_ERROR);
    assert_int_equal(insert_ldouble_dict(d, "gravity", 9.80665L,                a), NO_ERROR);
    assert_int_equal(insert_ldouble_dict(d, "neg",    -1.5L,                    a), NO_ERROR);
    long double v = 0.0L;
    assert_int_equal(get_ldouble_dict_value(d, "pi", &v), NO_ERROR);
    assert_true(v == 3.14159265358979323846L);
    assert_int_equal(get_ldouble_dict_value(d, "gravity", &v), NO_ERROR);
    assert_true(v == 9.80665L);
    assert_int_equal(get_ldouble_dict_value(d, "neg", &v), NO_ERROR);
    assert_true(v == -1.5L);
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 3: insert_ldouble_dict_n
// ================================================================================
 
static void test_ld_insert_n_null_key_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal(insert_ldouble_dict_n(d, NULL, 3, 1.0, a), NULL_POINTER);
    return_ldouble_dict(d);
}
 
static void test_ld_insert_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal(insert_ldouble_dict_n(d, "k", 0, 1.0, a), INVALID_ARG);
    return_ldouble_dict(d);
}
 
static void test_ld_insert_n_uses_only_specified_bytes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    /* "speed_max" — insert only "speed" (5 bytes) with value c (m/s) */
    const char* buf = "speed_max";
    assert_int_equal(insert_ldouble_dict_n(d, buf, 5, 299792458.0, a), NO_ERROR);
    long double v = 0.0;
    assert_int_equal(get_ldouble_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 299792458.0);
    /* Full buffer key "speed_max" is different — not found */
    assert_int_equal(get_ldouble_dict_value_n(d, buf, 9, &v), NOT_FOUND);
    /* Plain null-terminated lookup also works */
    assert_int_equal(get_ldouble_dict_value(d, "speed", &v), NO_ERROR);
    assert_true(v == 299792458.0);
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 4: pop_ldouble_dict
// ================================================================================
 
static void test_ld_pop_null_dict_returns_null_pointer(void** state) {
    (void)state;
    long double v;
    assert_int_equal(pop_ldouble_dict(NULL, "k", &v), NULL_POINTER);
}
 
static void test_ld_pop_null_key_returns_null_pointer(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    long double v;
    assert_int_equal(pop_ldouble_dict(d, NULL, &v), NULL_POINTER);
    return_ldouble_dict(d);
}
 
static void test_ld_pop_missing_key_returns_not_found(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal(pop_ldouble_dict(d, "missing", NULL), NOT_FOUND);
    return_ldouble_dict(d);
}
 
static void test_ld_pop_returns_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    insert_ldouble_dict(d, "gravity", 9.80665, a);
    long double v = 0.0;
    assert_int_equal(pop_ldouble_dict(d, "gravity", &v), NO_ERROR);
    assert_true(v == 9.80665);
    assert_int_equal((int)ldouble_dict_hash_size(d), 0);
    return_ldouble_dict(d);
}
 
static void test_ld_pop_null_out_value_discards_safely(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    insert_ldouble_dict(d, "x", 1.0, a);
    assert_int_equal(pop_ldouble_dict(d, "x", NULL), NO_ERROR);
    assert_int_equal((int)ldouble_dict_hash_size(d), 0);
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 5: pop_ldouble_dict_n
// ================================================================================
 
static void test_ld_pop_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal(pop_ldouble_dict_n(d, "k", 0, NULL), INVALID_ARG);
    return_ldouble_dict(d);
}
 
static void test_ld_pop_n_removes_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    const char* buf = "speed_max";
    insert_ldouble_dict_n(d, buf, 5, 299792458.0, a);   /* key = "speed" */
    long double v = 0.0;
    assert_int_equal(pop_ldouble_dict_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 299792458.0);
    assert_int_equal((int)ldouble_dict_hash_size(d), 0);
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 6: update_ldouble_dict
// ================================================================================
 
static void test_ld_update_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(update_ldouble_dict(NULL, "k", 1u), NULL_POINTER);
}
 
static void test_ld_update_null_key_returns_null_pointer(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal(update_ldouble_dict(d, NULL, 1u), NULL_POINTER);
    return_ldouble_dict(d);
}
 
static void test_ld_update_missing_key_returns_not_found(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal(update_ldouble_dict(d, "missing", 1u), NOT_FOUND);
    return_ldouble_dict(d);
}
 
static void test_ld_update_overwrites_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    insert_ldouble_dict(d, "gravity", 9.80665, a);
    assert_int_equal(update_ldouble_dict(d, "gravity", 1.62), NO_ERROR);
    long double v = 0.0;
    assert_int_equal(get_ldouble_dict_value(d, "gravity", &v), NO_ERROR);
    assert_true(v == 1.62);
    assert_int_equal((int)ldouble_dict_hash_size(d), 1);
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 7: update_ldouble_dict_n
// ================================================================================
 
static void test_ld_update_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal(update_ldouble_dict_n(d, "k", 0, 1u), INVALID_ARG);
    return_ldouble_dict(d);
}
 
static void test_ld_update_n_updates_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    const char* buf = "speed_max";
    insert_ldouble_dict_n(d, buf, 5, 299792458.0, a);   /* key = "speed" */
    assert_int_equal(update_ldouble_dict_n(d, buf, 5, 0.0), NO_ERROR);
    long double v = 1.0;
    assert_int_equal(get_ldouble_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 0.0);
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 8: get_ldouble_dict_value
// ================================================================================
 
static void test_ld_get_null_dict_returns_null_pointer(void** state) {
    (void)state;
    long double v;
    assert_int_equal(get_ldouble_dict_value(NULL, "k", &v), NULL_POINTER);
}
 
static void test_ld_get_null_key_returns_null_pointer(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    long double v;
    assert_int_equal(get_ldouble_dict_value(d, NULL, &v), NULL_POINTER);
    return_ldouble_dict(d);
}
 
static void test_ld_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    insert_ldouble_dict(d, "k", 1.0, a);
    assert_int_equal(get_ldouble_dict_value(d, "k", NULL), NULL_POINTER);
    return_ldouble_dict(d);
}
 
static void test_ld_get_missing_key_returns_not_found(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    long double v;
    assert_int_equal(get_ldouble_dict_value(d, "missing", &v), NOT_FOUND);
    return_ldouble_dict(d);
}
 
static void test_ld_get_retrieves_extremes(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    insert_ldouble_dict(d, "pi",      3.14159265358979323846L,  a);
    insert_ldouble_dict(d, "neg",    -2.71828182845904523536L,  a);
    insert_ldouble_dict(d, "zero",    0.0L,                     a);
    long double v = 0.0L;
    assert_int_equal(get_ldouble_dict_value(d, "pi", &v), NO_ERROR);
    assert_true(v == 3.14159265358979323846L);
    assert_int_equal(get_ldouble_dict_value(d, "neg", &v), NO_ERROR);
    assert_true(v == -2.71828182845904523536L);
    assert_int_equal(get_ldouble_dict_value(d, "zero", &v), NO_ERROR);
    assert_true(v == 0.0L);
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 9: get_ldouble_dict_value_n
// ================================================================================
 
static void test_ld_get_n_zero_len_returns_invalid_arg(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    long double v;
    assert_int_equal(get_ldouble_dict_value_n(d, "k", 0, &v), INVALID_ARG);
    return_ldouble_dict(d);
}
 
static void test_ld_get_n_retrieves_bounded_key(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    const char* buf = "speed_max";
    insert_ldouble_dict_n(d, buf, 5, 299792458.0, a);   /* key = "speed" */
    long double v = 0.0;
    assert_int_equal(get_ldouble_dict_value_n(d, buf, 5, &v), NO_ERROR);
    assert_true(v == 299792458.0);
    assert_int_equal(get_ldouble_dict_value_n(d, buf, 9, &v), NOT_FOUND);
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 10: get_ldouble_dict_ptr
// ================================================================================
 
static void test_ld_ptr_null_dict_returns_null(void** state) {
    (void)state;
    assert_null(get_ldouble_dict_ptr(NULL, "k"));
}
 
static void test_ld_ptr_null_key_returns_null(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_null(get_ldouble_dict_ptr(d, NULL));
    return_ldouble_dict(d);
}
 
static void test_ld_ptr_missing_returns_null(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_null(get_ldouble_dict_ptr(d, "missing"));
    return_ldouble_dict(d);
}
 
static void test_ld_ptr_points_to_correct_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    insert_ldouble_dict(d, "port", 8080u, a);
    const long double* p = get_ldouble_dict_ptr(d, "port");
    assert_non_null(p);
    assert_int_equal((int)*p, 8080);
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 11: get_ldouble_dict_ptr_n
// ================================================================================
 
static void test_ld_ptr_n_zero_len_returns_null(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_null(get_ldouble_dict_ptr_n(d, "k", 0));
    return_ldouble_dict(d);
}
 
static void test_ld_ptr_n_points_to_bounded_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    const char* buf = "port_http";
    insert_ldouble_dict_n(d, buf, 4, 8080u, a);   /* key = "port" */
    const long double* p = get_ldouble_dict_ptr_n(d, buf, 4);
    assert_non_null(p);
    assert_int_equal((int)*p, 8080);
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 12: has_ldouble_dict_key
// ================================================================================
 
static void test_ld_has_null_dict_returns_false(void** state) {
    (void)state;
    assert_false(has_ldouble_dict_key(NULL, "k"));
}
 
static void test_ld_has_null_key_returns_false(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_false(has_ldouble_dict_key(d, NULL));
    return_ldouble_dict(d);
}
 
static void test_ld_has_present_and_absent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    insert_ldouble_dict(d, "present", 1.0, a);
    assert_true(has_ldouble_dict_key(d, "present"));
    assert_false(has_ldouble_dict_key(d, "absent"));
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 13: has_ldouble_dict_key_n
// ================================================================================
 
static void test_ld_has_n_zero_len_returns_false(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_false(has_ldouble_dict_key_n(d, "k", 0));
    return_ldouble_dict(d);
}
 
static void test_ld_has_n_distinguishes_by_length(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    insert_ldouble_dict_n(d, "abc", 2, 1.0L, a);   /* key = "ab" */
    insert_ldouble_dict_n(d, "abc", 3, 2.0L, a);   /* key = "abc" */
    assert_true(has_ldouble_dict_key_n(d, "abc", 2));
    assert_true(has_ldouble_dict_key_n(d, "abc", 3));
    assert_false(has_ldouble_dict_key_n(d, "abc", 1));
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 14: clear_ldouble_dict
// ================================================================================
 
static void test_ld_clear_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_ldouble_dict(NULL), NULL_POINTER);
}
 
static void test_ld_clear_resets_and_is_reusable(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    insert_ldouble_dict(d, "a", 1.0, a);
    insert_ldouble_dict(d, "b", 2.0, a);
    assert_int_equal(clear_ldouble_dict(d), NO_ERROR);
    assert_int_equal((int)ldouble_dict_hash_size(d), 0);
    assert_true(is_ldouble_dict_empty(d));
    assert_int_equal(insert_ldouble_dict(d, "a", 3.141592653589793, a), NO_ERROR);
    long double v = 0.0;
    assert_int_equal(get_ldouble_dict_value(d, "a", &v), NO_ERROR);
    assert_true(v == 3.141592653589793);
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 15: copy_ldouble_dict
// ================================================================================
 
static void test_ld_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_expect_t r = copy_ldouble_dict(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_ld_copy_is_independent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* orig = _make_ldouble_dict(4);
    insert_ldouble_dict(orig, "x", 3.141592653589793, a);
    insert_ldouble_dict(orig, "y", 9.80665, a);
 
    ldouble_dict_expect_t cr = copy_ldouble_dict(orig, a);
    assert_true(cr.has_value);
 
    update_ldouble_dict(orig, "x", LDBL_MAX);
 
    long double v = 0.0;
    assert_int_equal(get_ldouble_dict_value(cr.u.value, "x", &v), NO_ERROR);
    assert_true(v == 3.141592653589793);
    assert_int_equal((int)ldouble_dict_hash_size(cr.u.value), 2);
 
    return_ldouble_dict(cr.u.value);
    return_ldouble_dict(orig);
}
 
// ================================================================================
// Group 16: merge_ldouble_dict
// ================================================================================
 
static void test_ld_merge_null_first_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    ldouble_dict_expect_t r = merge_ldouble_dict(NULL, d, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_ldouble_dict(d);
}
 
static void test_ld_merge_no_overwrite_keeps_first_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* da = _make_ldouble_dict(4);
    ldouble_dict_t* db = _make_ldouble_dict(4);
    insert_ldouble_dict(da, "shared", 3.141592653589793, a);
    insert_ldouble_dict(db, "shared", 9.80665, a);
    insert_ldouble_dict(db, "new",    1.62, a);
 
    ldouble_dict_expect_t mr = merge_ldouble_dict(da, db, false, a);
    assert_true(mr.has_value);
    long double v = 0.0;
    assert_int_equal(get_ldouble_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_true(v == 3.141592653589793);
    assert_int_equal(get_ldouble_dict_value(mr.u.value, "new", &v), NO_ERROR);
    assert_true(v == 1.62);
 
    return_ldouble_dict(mr.u.value);
    return_ldouble_dict(da);
    return_ldouble_dict(db);
}
 
static void test_ld_merge_overwrite_uses_second_value(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* da = _make_ldouble_dict(4);
    ldouble_dict_t* db = _make_ldouble_dict(4);
    insert_ldouble_dict(da, "shared", 3.141592653589793, a);
    insert_ldouble_dict(db, "shared", 9.80665, a);
 
    ldouble_dict_expect_t mr = merge_ldouble_dict(da, db, true, a);
    assert_true(mr.has_value);
    long double v = 0.0;
    assert_int_equal(get_ldouble_dict_value(mr.u.value, "shared", &v), NO_ERROR);
    assert_true(v == 9.80665);
 
    return_ldouble_dict(mr.u.value);
    return_ldouble_dict(da);
    return_ldouble_dict(db);
}
 
// ================================================================================
// Group 17: foreach_ldouble_dict
// ================================================================================
 
static void test_ld_foreach_null_dict_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(foreach_ldouble_dict(NULL, _ld_sum_iter, NULL),
                     NULL_POINTER);
}
 
static void test_ld_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_int_equal(foreach_ldouble_dict(d, NULL, NULL), NULL_POINTER);
    return_ldouble_dict(d);
}
 
static void test_ld_foreach_correct_sum(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    insert_ldouble_dict(d, "a", 1.0, a);
    insert_ldouble_dict(d, "b", 2.0, a);
    insert_ldouble_dict(d, "c", 3.0, a);
    _ld_iter_ctx_t ctx = { 0, 0.0L };
    assert_int_equal(foreach_ldouble_dict(d, _ld_sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 3);
    assert_true(ctx.sum == 6.0);
    return_ldouble_dict(d);
}
 
static void test_ld_foreach_delivers_negative_ldouble(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    insert_ldouble_dict(d, "pos",  1.5L, a);
    insert_ldouble_dict(d, "neg", -2.5L, a);
    _ld_iter_ctx_t ctx = { 0, 0.0L };
    foreach_ldouble_dict(d, _ld_sum_iter, &ctx);
    assert_int_equal(ctx.count, 2);
    /* 1.5L + (-2.5L) == -1.0L */
    assert_true(ctx.sum == -1.0L);
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 18: introspection
// ================================================================================
 
static void test_ld_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)ldouble_dict_size(NULL), 0);
}
 
static void test_ld_hash_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)ldouble_dict_hash_size(NULL), 0);
}
 
static void test_ld_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)ldouble_dict_alloc(NULL), 0);
}
 
static void test_ld_is_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_ldouble_dict_empty(NULL));
}
 
static void test_ld_is_empty_reflects_contents(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_t* d = _make_ldouble_dict(4);
    assert_true(is_ldouble_dict_empty(d));
    insert_ldouble_dict(d, "x", 1u, a);
    assert_false(is_ldouble_dict_empty(d));
    pop_ldouble_dict(d, "x", NULL);
    assert_true(is_ldouble_dict_empty(d));
    return_ldouble_dict(d);
}
 
// ================================================================================
// Group 19: growth stress
// ================================================================================
 
static void test_ld_growth_values_survive_resize(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_dict_expect_t r = init_ldouble_dict(2, true, a);
    assert_true(r.has_value);
    ldouble_dict_t* d = r.u.value;
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        long double val = (float_t)(i * 300u);   /* exercises > 255 values */
        assert_int_equal(insert_ldouble_dict(d, key, val, a), NO_ERROR);
    }
    assert_int_equal((int)ldouble_dict_hash_size(d), 200);
 
    for (int i = 0; i < 200; i++) {
        char key[16];
        snprintf(key, sizeof(key), "k%d", i);
        float_t expected = (float_t)(i * 300u);
        long double v = 0u;
        assert_int_equal(get_ldouble_dict_value(d, key, &v), NO_ERROR);
        assert_true(v == expected);
    }
 
    return_ldouble_dict(d);
}
 
// ================================================================================
// main
// ================================================================================
 
    const struct CMUnitTest test_ldouble_dict[] = {
 
        /* Group 1: init / return */
        cmocka_unit_test(test_ld_init_null_allocator_fails),
        cmocka_unit_test(test_ld_init_zero_capacity_fails),
        cmocka_unit_test(test_ld_init_data_size_is_sizeof_long_double),
        cmocka_unit_test(test_ld_init_dtype_is_float),
        cmocka_unit_test(test_ld_return_null_is_safe),
 
        /* Group 2: insert */
        cmocka_unit_test(test_ld_insert_null_dict_returns_null_pointer),
        cmocka_unit_test(test_ld_insert_null_key_returns_null_pointer),
        cmocka_unit_test(test_ld_insert_duplicate_returns_invalid_arg),
        cmocka_unit_test(test_ld_insert_stores_zero),
        cmocka_unit_test(test_ld_insert_stores_ldouble_extremes),
        cmocka_unit_test(test_ld_insert_stores_signed_ldouble),
 
        /* Group 3: insert_n */
        cmocka_unit_test(test_ld_insert_n_null_key_returns_null_pointer),
        cmocka_unit_test(test_ld_insert_n_zero_len_returns_invalid_arg),
        cmocka_unit_test(test_ld_insert_n_uses_only_specified_bytes),
 
        /* Group 4: pop */
        cmocka_unit_test(test_ld_pop_null_dict_returns_null_pointer),
        cmocka_unit_test(test_ld_pop_null_key_returns_null_pointer),
        cmocka_unit_test(test_ld_pop_missing_key_returns_not_found),
        cmocka_unit_test(test_ld_pop_returns_correct_value),
        cmocka_unit_test(test_ld_pop_null_out_value_discards_safely),
 
        /* Group 5: pop_n */
        cmocka_unit_test(test_ld_pop_n_zero_len_returns_invalid_arg),
        cmocka_unit_test(test_ld_pop_n_removes_bounded_key),
 
        /* Group 6: update */
        cmocka_unit_test(test_ld_update_null_dict_returns_null_pointer),
        cmocka_unit_test(test_ld_update_null_key_returns_null_pointer),
        cmocka_unit_test(test_ld_update_missing_key_returns_not_found),
        cmocka_unit_test(test_ld_update_overwrites_value),
 
        /* Group 7: update_n */
        cmocka_unit_test(test_ld_update_n_zero_len_returns_invalid_arg),
        cmocka_unit_test(test_ld_update_n_updates_bounded_key),
 
        /* Group 8: get_value */
        cmocka_unit_test(test_ld_get_null_dict_returns_null_pointer),
        cmocka_unit_test(test_ld_get_null_key_returns_null_pointer),
        cmocka_unit_test(test_ld_get_null_out_returns_null_pointer),
        cmocka_unit_test(test_ld_get_missing_key_returns_not_found),
        cmocka_unit_test(test_ld_get_retrieves_extremes),
 
        /* Group 9: get_value_n */
        cmocka_unit_test(test_ld_get_n_zero_len_returns_invalid_arg),
        cmocka_unit_test(test_ld_get_n_retrieves_bounded_key),
 
        /* Group 10: get_ptr */
        cmocka_unit_test(test_ld_ptr_null_dict_returns_null),
        cmocka_unit_test(test_ld_ptr_null_key_returns_null),
        cmocka_unit_test(test_ld_ptr_missing_returns_null),
        cmocka_unit_test(test_ld_ptr_points_to_correct_value),
 
        /* Group 11: get_ptr_n */
        cmocka_unit_test(test_ld_ptr_n_zero_len_returns_null),
        cmocka_unit_test(test_ld_ptr_n_points_to_bounded_value),
 
        /* Group 12: has_key */
        cmocka_unit_test(test_ld_has_null_dict_returns_false),
        cmocka_unit_test(test_ld_has_null_key_returns_false),
        cmocka_unit_test(test_ld_has_present_and_absent),
 
        /* Group 13: has_key_n */
        cmocka_unit_test(test_ld_has_n_zero_len_returns_false),
        cmocka_unit_test(test_ld_has_n_distinguishes_by_length),
 
        /* Group 14: clear */
        cmocka_unit_test(test_ld_clear_null_returns_null_pointer),
        cmocka_unit_test(test_ld_clear_resets_and_is_reusable),
 
        /* Group 15: copy */
        cmocka_unit_test(test_ld_copy_null_src_fails),
        cmocka_unit_test(test_ld_copy_is_independent),
 
        /* Group 16: merge */
        cmocka_unit_test(test_ld_merge_null_first_fails),
        cmocka_unit_test(test_ld_merge_no_overwrite_keeps_first_value),
        cmocka_unit_test(test_ld_merge_overwrite_uses_second_value),
 
        /* Group 17: foreach */
        cmocka_unit_test(test_ld_foreach_null_dict_returns_null_pointer),
        cmocka_unit_test(test_ld_foreach_null_fn_returns_null_pointer),
        cmocka_unit_test(test_ld_foreach_correct_sum),
        cmocka_unit_test(test_ld_foreach_delivers_negative_ldouble),
 
        /* Group 18: introspection */
        cmocka_unit_test(test_ld_size_null_returns_zero),
        cmocka_unit_test(test_ld_hash_size_null_returns_zero),
        cmocka_unit_test(test_ld_alloc_null_returns_zero),
        cmocka_unit_test(test_ld_is_empty_null_returns_true),
        cmocka_unit_test(test_ld_is_empty_reflects_contents),
 
        /* Group 19: growth stress */
        cmocka_unit_test(test_ld_growth_values_survive_resize),
 
    };
const size_t test_ldouble_dict_count = sizeof(test_ldouble_dict) / sizeof(test_ldouble_dict[0]);
// ================================================================================
// ================================================================================
// eof
