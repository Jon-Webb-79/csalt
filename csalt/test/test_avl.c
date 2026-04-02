// ================================================================================
// ================================================================================
// - File:    test_avl.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    April 02, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_allocator.h"
#include "c_dtypes.h"
#include "c_error.h"
#include "c_tree.h"
 
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
// ================================================================================
// ================================================================================
// TESTS


/* Standard int32 ascending comparator */
static int cmp_int32(const void* a, const void* b) {
    int32_t va = *(const int32_t*)a;
    int32_t vb = *(const int32_t*)b;
    return (va > vb) - (va < vb);
}
 
/* Double ascending comparator */
static int cmp_double(const void* a, const void* b) {
    double va = *(const double*)a;
    double vb = *(const double*)b;
    return (va > vb) - (va < vb);
}
 
/* Build a no-duplicate, no-overflow int32 tree with given capacity. */
static avl_t* _make_tree(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = init_avl(cap, INT32_TYPE, false, false, a, cmp_int32);
    assert_true(r.has_value);
    return r.u.value;
}
 
/* Build a duplicate-allowed, no-overflow int32 tree. */
static avl_t* _make_dup_tree(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = init_avl(cap, INT32_TYPE, false, true, a, cmp_int32);
    assert_true(r.has_value);
    return r.u.value;
}
 
/* Build an overflow-enabled, no-duplicate int32 tree. */
static avl_t* _make_overflow_tree(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = init_avl(cap, INT32_TYPE, true, false, a, cmp_int32);
    assert_true(r.has_value);
    return r.u.value;
}
 
/* Insert a list of int32 values and assert every insert succeeds. */
static void _insert_all(avl_t* tree, const int32_t* vals, int n) {
    for (int i = 0; i < n; i++)
        assert_int_equal(avl_insert(tree, &vals[i]), NO_ERROR);
}
 
/* Iterator context: records visit count and accumulates sum. */
typedef struct { int count; int32_t sum; } _iter_ctx_t;
 
static void _sum_iter(const void* elem, void* ctx) {
    _iter_ctx_t* c = (_iter_ctx_t*)ctx;
    c->count++;
    c->sum += *(const int32_t*)elem;
}
 
/* Iterator context: verifies elements arrive in non-decreasing order. */
typedef struct { int32_t prev; bool ok; int count; } _order_ctx_t;
 
static void _order_iter(const void* elem, void* ctx) {
    _order_ctx_t* c = (_order_ctx_t*)ctx;
    int32_t v = *(const int32_t*)elem;
    if (v < c->prev) c->ok = false;
    c->prev = v;
    c->count++;
}
 
/* Iterator context: records whether a specific value was seen. */
typedef struct { int32_t target; bool found; } _find_ctx_t;
 
static void _find_iter(const void* elem, void* ctx) {
    _find_ctx_t* c = (_find_ctx_t*)ctx;
    if (*(const int32_t*)elem == c->target)
        c->found = true;
}
 
// ================================================================================
// Group 1: init_avl
// ================================================================================
 
static void test_avl_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    avl_expect_t r = init_avl(8, INT32_TYPE, false, false, bad, cmp_int32);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_avl_init_null_comparator_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = init_avl(8, INT32_TYPE, false, false, a, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_avl_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = init_avl(0, INT32_TYPE, false, false, a, cmp_int32);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_avl_init_unknown_dtype_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = init_avl(8, UNKNOWN_TYPE, false, false, a, cmp_int32);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_avl_init_returns_valid_tree(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = init_avl(16, INT32_TYPE, false, false, a, cmp_int32);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_int_equal((int)avl_size(r.u.value), 0);
    assert_true(avl_is_empty(r.u.value));
    assert_int_equal(avl_height(r.u.value), 0);
    return_avl(r.u.value);
}
 
static void test_avl_init_stores_dtype(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    assert_int_equal((int)tree->dtype, (int)INT32_TYPE);
    return_avl(tree);
}
 
static void test_avl_init_stores_comparator(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    assert_ptr_equal(tree->cmp, cmp_int32);
    return_avl(tree);
}
 
static void test_avl_init_allow_duplicates_false_stored(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    assert_false(tree->allow_duplicates);
    return_avl(tree);
}
 
static void test_avl_init_allow_duplicates_true_stored(void** state) {
    (void)state;
    avl_t* tree = _make_dup_tree(8);
    assert_true(tree->allow_duplicates);
    return_avl(tree);
}
 
static void test_avl_init_overflow_false_stored(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    assert_false(tree->overflow);
    return_avl(tree);
}
 
static void test_avl_init_overflow_true_stored(void** state) {
    (void)state;
    avl_t* tree = _make_overflow_tree(8);
    assert_true(tree->overflow);
    return_avl(tree);
}
 
// ================================================================================
// Group 2: return_avl
// ================================================================================
 
static void test_avl_return_null_is_safe(void** state) {
    (void)state;
    return_avl(NULL);   /* must not crash */
}
 
static void test_avl_return_empty_tree_does_not_crash(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    return_avl(tree);
}
 
static void test_avl_return_populated_tree_does_not_crash(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t vals[] = { 5, 3, 7, 1, 4 };
    _insert_all(tree, vals, 5);
    return_avl(tree);   /* must not crash or leak */
}
 
static void test_avl_return_overflow_tree_does_not_crash(void** state) {
    (void)state;
    avl_t* tree = _make_overflow_tree(2);
    for (int32_t i = 1; i <= 10; i++)
        avl_insert(tree, &i);
    return_avl(tree);
}
 
// ================================================================================
// Group 3: avl_insert
// ================================================================================
 
static void test_avl_insert_null_tree_returns_null_pointer(void** state) {
    (void)state;
    int32_t v = 1;
    assert_int_equal(avl_insert(NULL, &v), NULL_POINTER);
}
 
static void test_avl_insert_null_data_returns_null_pointer(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    assert_int_equal(avl_insert(tree, NULL), NULL_POINTER);
    return_avl(tree);
}
 
static void test_avl_insert_single_element_increments_size(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 42;
    assert_int_equal(avl_insert(tree, &v), NO_ERROR);
    assert_int_equal((int)avl_size(tree), 1);
    return_avl(tree);
}
 
static void test_avl_insert_duplicate_no_dup_returns_invalid_arg(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 10;
    assert_int_equal(avl_insert(tree, &v), NO_ERROR);
    assert_int_equal(avl_insert(tree, &v), INVALID_ARG);
    assert_int_equal((int)avl_size(tree), 1);
    return_avl(tree);
}
 
static void test_avl_insert_duplicate_allowed_increments_size(void** state) {
    (void)state;
    avl_t* tree = _make_dup_tree(8);
    int32_t v = 10;
    assert_int_equal(avl_insert(tree, &v), NO_ERROR);
    assert_int_equal(avl_insert(tree, &v), NO_ERROR);
    assert_int_equal((int)avl_size(tree), 2);
    return_avl(tree);
}
 
static void test_avl_insert_no_overflow_capacity_overflow(void** state) {
    (void)state;
    avl_t* tree = _make_tree(3);
    int32_t vals[] = { 1, 2, 3 };
    _insert_all(tree, vals, 3);
    int32_t v = 4;
    assert_int_equal(avl_insert(tree, &v), CAPACITY_OVERFLOW);
    assert_int_equal((int)avl_size(tree), 3);
    return_avl(tree);
}
 
static void test_avl_insert_overflow_grows_beyond_slab(void** state) {
    (void)state;
    avl_t* tree = _make_overflow_tree(2);
    for (int32_t i = 1; i <= 20; i++)
        assert_int_equal(avl_insert(tree, &i), NO_ERROR);
    assert_int_equal((int)avl_size(tree), 20);
    return_avl(tree);
}
 
static void test_avl_insert_maintains_sorted_order(void** state) {
    (void)state;
    avl_t* tree = _make_tree(16);
    int32_t vals[] = { 5, 3, 8, 1, 4, 7, 9, 2, 6 };
    _insert_all(tree, vals, 9);
 
    _order_ctx_t ctx = { INT32_MIN, true, 0 };
    avl_foreach(tree, _order_iter, &ctx);
    assert_true(ctx.ok);
    assert_int_equal(ctx.count, 9);
    return_avl(tree);
}
 
static void test_avl_insert_height_bounded_by_avl_invariant(void** state) {
    (void)state;
    /* For n nodes, AVL height <= 1.44 * log2(n+2).
       For 100 nodes that bound is approximately 10; we check <= 11 for margin. */
    avl_t* tree = _make_tree(128);
    for (int32_t i = 1; i <= 100; i++)
        avl_insert(tree, &i);
    assert_true(avl_height(tree) <= 11);
    return_avl(tree);
}
 
static void test_avl_insert_all_values_findable_after_insert(void** state) {
    (void)state;
    avl_t* tree = _make_tree(32);
    int32_t vals[] = { 15, 3, 22, 7, 18, 1, 9, 25, 12, 6 };
    _insert_all(tree, vals, 10);
    for (int i = 0; i < 10; i++)
        assert_true(avl_contains(tree, &vals[i]));
    return_avl(tree);
}
 
// ================================================================================
// Group 4: avl_remove
// ================================================================================
 
static void test_avl_remove_null_tree_returns_null_pointer(void** state) {
    (void)state;
    int32_t key = 1;
    assert_int_equal(avl_remove(NULL, &key, NULL), NULL_POINTER);
}
 
static void test_avl_remove_null_key_returns_null_pointer(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    assert_int_equal(avl_remove(tree, NULL, NULL), NULL_POINTER);
    return_avl(tree);
}
 
static void test_avl_remove_empty_tree_returns_empty(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t key = 1;
    assert_int_equal(avl_remove(tree, &key, NULL), EMPTY);
    return_avl(tree);
}
 
static void test_avl_remove_missing_key_returns_not_found(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 5;
    avl_insert(tree, &v);
    int32_t key = 99;
    assert_int_equal(avl_remove(tree, &key, NULL), NOT_FOUND);
    assert_int_equal((int)avl_size(tree), 1);
    return_avl(tree);
}
 
static void test_avl_remove_copies_element_to_out(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 42;
    avl_insert(tree, &v);
    int32_t out = 0;
    assert_int_equal(avl_remove(tree, &v, &out), NO_ERROR);
    assert_int_equal(out, 42);
    return_avl(tree);
}
 
static void test_avl_remove_null_out_discards_safely(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 7;
    avl_insert(tree, &v);
    assert_int_equal(avl_remove(tree, &v, NULL), NO_ERROR);
    assert_int_equal((int)avl_size(tree), 0);
    return_avl(tree);
}
 
static void test_avl_remove_decrements_size(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t vals[] = { 3, 1, 5 };
    _insert_all(tree, vals, 3);
    int32_t key = 1;
    avl_remove(tree, &key, NULL);
    assert_int_equal((int)avl_size(tree), 2);
    return_avl(tree);
}
 
static void test_avl_remove_leaf_node(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t vals[] = { 5, 3, 7 };
    _insert_all(tree, vals, 3);
    int32_t key = 3;   /* leaf */
    assert_int_equal(avl_remove(tree, &key, NULL), NO_ERROR);
    assert_false(avl_contains(tree, &key));
    assert_int_equal((int)avl_size(tree), 2);
    return_avl(tree);
}
 
static void test_avl_remove_node_with_one_child(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    /* Insert so that 3 has only a right child (4) */
    int32_t vals[] = { 5, 3, 7, 4 };
    _insert_all(tree, vals, 4);
    int32_t key = 3;
    assert_int_equal(avl_remove(tree, &key, NULL), NO_ERROR);
    assert_false(avl_contains(tree, &key));
    /* Remaining values must still be findable */
    int32_t v4 = 4, v5 = 5, v7 = 7;
    assert_true(avl_contains(tree, &v4));
    assert_true(avl_contains(tree, &v5));
    assert_true(avl_contains(tree, &v7));
    return_avl(tree);
}
 
static void test_avl_remove_node_with_two_children(void** state) {
    (void)state;
    avl_t* tree = _make_tree(16);
    int32_t vals[] = { 10, 5, 15, 3, 7, 12, 20 };
    _insert_all(tree, vals, 7);
    /* Remove root (two children) */
    int32_t key = 10;
    assert_int_equal(avl_remove(tree, &key, NULL), NO_ERROR);
    assert_false(avl_contains(tree, &key));
    assert_int_equal((int)avl_size(tree), 6);
    return_avl(tree);
}
 
static void test_avl_remove_root_of_single_element_tree(void** state) {
    (void)state;
    avl_t* tree = _make_tree(4);
    int32_t v = 99;
    avl_insert(tree, &v);
    assert_int_equal(avl_remove(tree, &v, NULL), NO_ERROR);
    assert_true(avl_is_empty(tree));
    assert_int_equal(avl_height(tree), 0);
    return_avl(tree);
}
 
static void test_avl_remove_maintains_sorted_order(void** state) {
    (void)state;
    avl_t* tree = _make_tree(16);
    int32_t vals[] = { 8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7 };
    _insert_all(tree, vals, 11);
 
    int32_t key = 4;
    avl_remove(tree, &key, NULL);
 
    _order_ctx_t ctx = { INT32_MIN, true, 0 };
    avl_foreach(tree, _order_iter, &ctx);
    assert_true(ctx.ok);
    assert_int_equal(ctx.count, 10);
    return_avl(tree);
}
 
static void test_avl_remove_height_stays_bounded_after_removals(void** state) {
    (void)state;
    avl_t* tree = _make_tree(128);
    for (int32_t i = 1; i <= 100; i++)
        avl_insert(tree, &i);
    /* Remove half the elements */
    for (int32_t i = 2; i <= 100; i += 2)
        avl_remove(tree, &i, NULL);
    assert_true(avl_height(tree) <= 11);
    return_avl(tree);
}
 
static void test_avl_remove_recycled_slot_reused_on_insert(void** state) {
    (void)state;
    /* Slab has exactly 4 slots; insert 4, remove 1, insert 1 more —
       the freed slot must be reused (no overflow needed). */
    avl_t* tree = _make_tree(4);
    int32_t vals[] = { 1, 2, 3, 4 };
    _insert_all(tree, vals, 4);
    int32_t key = 2;
    avl_remove(tree, &key, NULL);
    int32_t v = 5;
    assert_int_equal(avl_insert(tree, &v), NO_ERROR);
    assert_int_equal((int)avl_size(tree), 4);
    return_avl(tree);
}
 
// ================================================================================
// Group 5: avl_contains
// ================================================================================
 
static void test_avl_contains_null_tree_returns_false(void** state) {
    (void)state;
    int32_t v = 1;
    assert_false(avl_contains(NULL, &v));
}
 
static void test_avl_contains_null_key_returns_false(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    assert_false(avl_contains(tree, NULL));
    return_avl(tree);
}
 
static void test_avl_contains_empty_tree_returns_false(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 1;
    assert_false(avl_contains(tree, &v));
    return_avl(tree);
}
 
static void test_avl_contains_present_element_returns_true(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t vals[] = { 5, 3, 7 };
    _insert_all(tree, vals, 3);
    assert_true(avl_contains(tree, &vals[0]));
    assert_true(avl_contains(tree, &vals[1]));
    assert_true(avl_contains(tree, &vals[2]));
    return_avl(tree);
}
 
static void test_avl_contains_absent_element_returns_false(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t vals[] = { 5, 3, 7 };
    _insert_all(tree, vals, 3);
    int32_t absent = 99;
    assert_false(avl_contains(tree, &absent));
    return_avl(tree);
}
 
static void test_avl_contains_returns_false_after_removal(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 10;
    avl_insert(tree, &v);
    assert_true(avl_contains(tree, &v));
    avl_remove(tree, &v, NULL);
    assert_false(avl_contains(tree, &v));
    return_avl(tree);
}
 
// ================================================================================
// Group 6: avl_find
// ================================================================================
 
static void test_avl_find_null_tree_returns_null_pointer(void** state) {
    (void)state;
    int32_t key = 1, out;
    assert_int_equal(avl_find(NULL, &key, &out), NULL_POINTER);
}
 
static void test_avl_find_null_key_returns_null_pointer(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t out;
    assert_int_equal(avl_find(tree, NULL, &out), NULL_POINTER);
    return_avl(tree);
}
 
static void test_avl_find_null_out_returns_null_pointer(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t key = 1;
    assert_int_equal(avl_find(tree, &key, NULL), NULL_POINTER);
    return_avl(tree);
}
 
static void test_avl_find_empty_tree_returns_empty(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t key = 1, out;
    assert_int_equal(avl_find(tree, &key, &out), EMPTY);
    return_avl(tree);
}
 
static void test_avl_find_missing_key_returns_not_found(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 5;
    avl_insert(tree, &v);
    int32_t key = 99, out;
    assert_int_equal(avl_find(tree, &key, &out), NOT_FOUND);
    return_avl(tree);
}
 
static void test_avl_find_copies_correct_value(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t vals[] = { 10, 5, 15 };
    _insert_all(tree, vals, 3);
    int32_t key = 15, out = 0;
    assert_int_equal(avl_find(tree, &key, &out), NO_ERROR);
    assert_int_equal(out, 15);
    return_avl(tree);
}
 
static void test_avl_find_does_not_modify_tree(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 7;
    avl_insert(tree, &v);
    size_t size_before   = avl_size(tree);
    int    height_before = avl_height(tree);
    int32_t out;
    avl_find(tree, &v, &out);
    assert_int_equal((int)avl_size(tree),   (int)size_before);
    assert_int_equal(avl_height(tree), height_before);
    return_avl(tree);
}
 
// ================================================================================
// Group 7: avl_min and avl_max
// ================================================================================
 
static void test_avl_min_null_tree_returns_null_pointer(void** state) {
    (void)state;
    int32_t out;
    assert_int_equal(avl_min(NULL, &out), NULL_POINTER);
}
 
static void test_avl_min_null_out_returns_null_pointer(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    assert_int_equal(avl_min(tree, NULL), NULL_POINTER);
    return_avl(tree);
}
 
static void test_avl_min_empty_tree_returns_empty(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t out;
    assert_int_equal(avl_min(tree, &out), EMPTY);
    return_avl(tree);
}
 
static void test_avl_min_returns_correct_minimum(void** state) {
    (void)state;
    avl_t* tree = _make_tree(16);
    int32_t vals[] = { 8, 3, 15, 1, 5, 12, 20 };
    _insert_all(tree, vals, 7);
    int32_t out = 0;
    assert_int_equal(avl_min(tree, &out), NO_ERROR);
    assert_int_equal(out, 1);
    return_avl(tree);
}
 
static void test_avl_min_single_element(void** state) {
    (void)state;
    avl_t* tree = _make_tree(4);
    int32_t v = 42;
    avl_insert(tree, &v);
    int32_t out = 0;
    assert_int_equal(avl_min(tree, &out), NO_ERROR);
    assert_int_equal(out, 42);
    return_avl(tree);
}
 
static void test_avl_max_null_tree_returns_null_pointer(void** state) {
    (void)state;
    int32_t out;
    assert_int_equal(avl_max(NULL, &out), NULL_POINTER);
}
 
static void test_avl_max_null_out_returns_null_pointer(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    assert_int_equal(avl_max(tree, NULL), NULL_POINTER);
    return_avl(tree);
}
 
static void test_avl_max_empty_tree_returns_empty(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t out;
    assert_int_equal(avl_max(tree, &out), EMPTY);
    return_avl(tree);
}
 
static void test_avl_max_returns_correct_maximum(void** state) {
    (void)state;
    avl_t* tree = _make_tree(16);
    int32_t vals[] = { 8, 3, 15, 1, 5, 12, 20 };
    _insert_all(tree, vals, 7);
    int32_t out = 0;
    assert_int_equal(avl_max(tree, &out), NO_ERROR);
    assert_int_equal(out, 20);
    return_avl(tree);
}
 
static void test_avl_max_single_element(void** state) {
    (void)state;
    avl_t* tree = _make_tree(4);
    int32_t v = 99;
    avl_insert(tree, &v);
    int32_t out = 0;
    assert_int_equal(avl_max(tree, &out), NO_ERROR);
    assert_int_equal(out, 99);
    return_avl(tree);
}
 
static void test_avl_min_max_update_after_removal(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t vals[] = { 5, 1, 10 };
    _insert_all(tree, vals, 3);
 
    /* Remove min; new min should be 5 */
    int32_t key = 1;
    avl_remove(tree, &key, NULL);
    int32_t out = 0;
    avl_min(tree, &out);
    assert_int_equal(out, 5);
 
    /* Remove max; new max should be 5 */
    key = 10;
    avl_remove(tree, &key, NULL);
    avl_max(tree, &out);
    assert_int_equal(out, 5);
    return_avl(tree);
}
 
// ================================================================================
// Group 8: avl_foreach
// ================================================================================
 
static void test_avl_foreach_null_tree_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(avl_foreach(NULL, _sum_iter, NULL), NULL_POINTER);
}
 
static void test_avl_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 1;
    avl_insert(tree, &v);
    assert_int_equal(avl_foreach(tree, NULL, NULL), NULL_POINTER);
    return_avl(tree);
}
 
static void test_avl_foreach_empty_tree_returns_empty(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    assert_int_equal(avl_foreach(tree, _sum_iter, NULL), EMPTY);
    return_avl(tree);
}
 
static void test_avl_foreach_visits_all_elements(void** state) {
    (void)state;
    avl_t* tree = _make_tree(16);
    int32_t vals[] = { 3, 1, 4, 1, 5, 9, 2, 6 };
    /* Use only unique values for a no-dup tree */
    int32_t uvals[] = { 3, 1, 4, 5, 9, 2, 6 };
    _insert_all(tree, uvals, 7);
 
    _iter_ctx_t ctx = { 0, 0 };
    assert_int_equal(avl_foreach(tree, _sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 7);
    assert_int_equal(ctx.sum, 3 + 1 + 4 + 5 + 9 + 2 + 6);
    return_avl(tree);
}
 
static void test_avl_foreach_visits_in_sorted_order(void** state) {
    (void)state;
    avl_t* tree = _make_tree(32);
    int32_t vals[] = { 15, 3, 22, 7, 18, 1, 9, 25, 12, 6 };
    _insert_all(tree, vals, 10);
 
    _order_ctx_t ctx = { INT32_MIN, true, 0 };
    avl_foreach(tree, _order_iter, &ctx);
    assert_true(ctx.ok);
    assert_int_equal(ctx.count, 10);
    return_avl(tree);
}
 
static void test_avl_foreach_does_not_modify_tree(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t vals[] = { 5, 3, 7 };
    _insert_all(tree, vals, 3);
 
    size_t size_before   = avl_size(tree);
    int    height_before = avl_height(tree);
 
    _iter_ctx_t ctx = { 0, 0 };
    avl_foreach(tree, _sum_iter, &ctx);
 
    assert_int_equal((int)avl_size(tree),   (int)size_before);
    assert_int_equal(avl_height(tree), height_before);
    return_avl(tree);
}
 
static void test_avl_foreach_null_ctx_forwarded(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 42;
    avl_insert(tree, &v);
    _find_ctx_t ctx = { 42, false };
    assert_int_equal(avl_foreach(tree, _find_iter, &ctx), NO_ERROR);
    assert_true(ctx.found);
    return_avl(tree);
}
 
static void test_avl_foreach_sorted_order_after_removals(void** state) {
    (void)state;
    avl_t* tree = _make_tree(32);
    for (int32_t i = 1; i <= 20; i++)
        avl_insert(tree, &i);
    /* Remove every even number */
    for (int32_t i = 2; i <= 20; i += 2)
        avl_remove(tree, &i, NULL);
 
    _order_ctx_t ctx = { INT32_MIN, true, 0 };
    avl_foreach(tree, _order_iter, &ctx);
    assert_true(ctx.ok);
    assert_int_equal(ctx.count, 10);  /* 1,3,5,...,19 */
    return_avl(tree);
}
 
static void test_avl_foreach_duplicate_elements_all_visited(void** state) {
    (void)state;
    avl_t* tree = _make_dup_tree(16);
    int32_t v = 5;
    avl_insert(tree, &v);
    avl_insert(tree, &v);
    avl_insert(tree, &v);
 
    _iter_ctx_t ctx = { 0, 0 };
    avl_foreach(tree, _sum_iter, &ctx);
    assert_int_equal(ctx.count, 3);
    assert_int_equal(ctx.sum, 15);
    return_avl(tree);
}
 
// ================================================================================
// Group 9: avl_foreach_range
// ================================================================================
 
static void test_avl_range_null_tree_returns_null_pointer(void** state) {
    (void)state;
    int32_t lo = 1, hi = 5;
    assert_int_equal(avl_foreach_range(NULL, &lo, &hi, _sum_iter, NULL),
                     NULL_POINTER);
}
 
static void test_avl_range_null_low_returns_null_pointer(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 1, hi = 5;
    avl_insert(tree, &v);
    assert_int_equal(avl_foreach_range(tree, NULL, &hi, _sum_iter, NULL),
                     NULL_POINTER);
    return_avl(tree);
}
 
static void test_avl_range_null_high_returns_null_pointer(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 1, lo = 1;
    avl_insert(tree, &v);
    assert_int_equal(avl_foreach_range(tree, &lo, NULL, _sum_iter, NULL),
                     NULL_POINTER);
    return_avl(tree);
}
 
static void test_avl_range_null_fn_returns_null_pointer(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 1, lo = 1, hi = 5;
    avl_insert(tree, &v);
    assert_int_equal(avl_foreach_range(tree, &lo, &hi, NULL, NULL),
                     NULL_POINTER);
    return_avl(tree);
}
 
static void test_avl_range_empty_tree_returns_empty(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t lo = 1, hi = 5;
    assert_int_equal(avl_foreach_range(tree, &lo, &hi, _sum_iter, NULL),
                     EMPTY);
    return_avl(tree);
}
 
static void test_avl_range_inverted_range_returns_invalid_arg(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 5;
    avl_insert(tree, &v);
    int32_t lo = 10, hi = 1;   /* lo > hi */
    assert_int_equal(avl_foreach_range(tree, &lo, &hi, _sum_iter, NULL),
                     INVALID_ARG);
    return_avl(tree);
}
 
static void test_avl_range_visits_only_in_range_elements(void** state) {
    (void)state;
    avl_t* tree = _make_tree(32);
    for (int32_t i = 1; i <= 20; i++)
        avl_insert(tree, &i);
 
    int32_t lo = 5, hi = 10;
    _iter_ctx_t ctx = { 0, 0 };
    assert_int_equal(avl_foreach_range(tree, &lo, &hi, _sum_iter, &ctx),
                     NO_ERROR);
    assert_int_equal(ctx.count, 6);          /* 5,6,7,8,9,10 */
    assert_int_equal(ctx.sum, 5+6+7+8+9+10);
    return_avl(tree);
}
 
static void test_avl_range_visits_in_sorted_order(void** state) {
    (void)state;
    avl_t* tree = _make_tree(32);
    int32_t vals[] = { 15, 3, 22, 7, 18, 1, 9, 25, 12, 6 };
    _insert_all(tree, vals, 10);
 
    int32_t lo = 5, hi = 15;
    _order_ctx_t ctx = { INT32_MIN, true, 0 };
    avl_foreach_range(tree, &lo, &hi, _order_iter, &ctx);
    assert_true(ctx.ok);
    return_avl(tree);
}
 
static void test_avl_range_single_value_range(void** state) {
    (void)state;
    avl_t* tree = _make_tree(16);
    int32_t vals[] = { 1, 5, 10, 15, 20 };
    _insert_all(tree, vals, 5);
 
    int32_t lo = 10, hi = 10;
    _iter_ctx_t ctx = { 0, 0 };
    avl_foreach_range(tree, &lo, &hi, _sum_iter, &ctx);
    assert_int_equal(ctx.count, 1);
    assert_int_equal(ctx.sum, 10);
    return_avl(tree);
}
 
static void test_avl_range_no_elements_in_range(void** state) {
    (void)state;
    avl_t* tree = _make_tree(16);
    int32_t vals[] = { 1, 2, 3, 20, 21 };
    _insert_all(tree, vals, 5);
 
    /* Gap between 3 and 20 — range [10,15] matches nothing */
    int32_t lo = 10, hi = 15;
    _iter_ctx_t ctx = { 0, 0 };
    avl_foreach_range(tree, &lo, &hi, _sum_iter, &ctx);
    assert_int_equal(ctx.count, 0);
    return_avl(tree);
}
 
static void test_avl_range_full_tree_range(void** state) {
    (void)state;
    avl_t* tree = _make_tree(16);
    int32_t vals[] = { 5, 2, 8, 1, 3, 7, 9 };
    _insert_all(tree, vals, 7);
 
    int32_t lo = 1, hi = 9;
    _iter_ctx_t ctx = { 0, 0 };
    avl_foreach_range(tree, &lo, &hi, _sum_iter, &ctx);
    assert_int_equal(ctx.count, 7);
    return_avl(tree);
}
 
// ================================================================================
// Group 10: avl_size, avl_height, avl_is_empty
// ================================================================================
 
static void test_avl_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)avl_size(NULL), 0);
}
 
static void test_avl_size_empty_is_zero(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    assert_int_equal((int)avl_size(tree), 0);
    return_avl(tree);
}
 
static void test_avl_size_tracks_inserts_and_removals(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 1;
    avl_insert(tree, &v); assert_int_equal((int)avl_size(tree), 1);
    v = 2; avl_insert(tree, &v); assert_int_equal((int)avl_size(tree), 2);
    v = 1; avl_remove(tree, &v, NULL); assert_int_equal((int)avl_size(tree), 1);
    v = 2; avl_remove(tree, &v, NULL); assert_int_equal((int)avl_size(tree), 0);
    return_avl(tree);
}
 
static void test_avl_height_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal(avl_height(NULL), 0);
}
 
static void test_avl_height_empty_is_zero(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    assert_int_equal(avl_height(tree), 0);
    return_avl(tree);
}
 
static void test_avl_height_single_node_is_one(void** state) {
    (void)state;
    avl_t* tree = _make_tree(4);
    int32_t v = 5;
    avl_insert(tree, &v);
    assert_int_equal(avl_height(tree), 1);
    return_avl(tree);
}
 
static void test_avl_height_decreases_after_removals(void** state) {
    (void)state;
    avl_t* tree = _make_tree(16);
    for (int32_t i = 1; i <= 15; i++)
        avl_insert(tree, &i);
    int h_before = avl_height(tree);
 
    /* Remove enough nodes to reduce height */
    for (int32_t i = 1; i <= 12; i++)
        avl_remove(tree, &i, NULL);
 
    assert_true(avl_height(tree) <= h_before);
    return_avl(tree);
}
 
static void test_avl_is_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(avl_is_empty(NULL));
}
 
static void test_avl_is_empty_reflects_contents(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    assert_true(avl_is_empty(tree));
    int32_t v = 1;
    avl_insert(tree, &v);
    assert_false(avl_is_empty(tree));
    avl_remove(tree, &v, NULL);
    assert_true(avl_is_empty(tree));
    return_avl(tree);
}
 
// ================================================================================
// Group 11: copy_avl
// ================================================================================
 
static void test_avl_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = copy_avl(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_avl_copy_null_allocator_fails(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 1;
    avl_insert(tree, &v);
    allocator_vtable_t bad = { 0 };
    avl_expect_t r = copy_avl(tree, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_avl(tree);
}
 
static void test_avl_copy_empty_tree_produces_valid_copy(void** state) {
    (void)state;
    avl_t* src = _make_tree(8);
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = copy_avl(src, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_true(avl_is_empty(r.u.value));
    return_avl(r.u.value);
    return_avl(src);
}
 
static void test_avl_copy_has_same_size(void** state) {
    (void)state;
    avl_t* src = _make_tree(16);
    int32_t vals[] = { 8, 3, 12, 1, 5, 10, 15 };
    _insert_all(src, vals, 7);
 
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = copy_avl(src, a);
    assert_true(r.has_value);
    assert_int_equal((int)avl_size(r.u.value), (int)avl_size(src));
    return_avl(r.u.value);
    return_avl(src);
}
 
static void test_avl_copy_all_elements_present(void** state) {
    (void)state;
    avl_t* src = _make_tree(16);
    int32_t vals[] = { 8, 3, 12, 1, 5, 10, 15 };
    _insert_all(src, vals, 7);
 
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = copy_avl(src, a);
    assert_true(r.has_value);
    avl_t* dst = r.u.value;
 
    for (int i = 0; i < 7; i++)
        assert_true(avl_contains(dst, &vals[i]));
    return_avl(dst);
    return_avl(src);
}
 
static void test_avl_copy_is_sorted(void** state) {
    (void)state;
    avl_t* src = _make_tree(16);
    int32_t vals[] = { 8, 3, 12, 1, 5, 10, 15 };
    _insert_all(src, vals, 7);
 
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = copy_avl(src, a);
    assert_true(r.has_value);
 
    _order_ctx_t ctx = { INT32_MIN, true, 0 };
    avl_foreach(r.u.value, _order_iter, &ctx);
    assert_true(ctx.ok);
    assert_int_equal(ctx.count, 7);
    return_avl(r.u.value);
    return_avl(src);
}
 
static void test_avl_copy_is_independent_of_source(void** state) {
    (void)state;
    avl_t* src = _make_tree(16);
    int32_t vals[] = { 5, 3, 7 };
    _insert_all(src, vals, 3);
 
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = copy_avl(src, a);
    assert_true(r.has_value);
    avl_t* dst = r.u.value;
 
    /* Insert into copy — source must be unaffected */
    int32_t v = 99;
    avl_insert(dst, &v);
    assert_int_equal((int)avl_size(src), 3);
    assert_false(avl_contains(src, &v));
 
    /* Remove from copy — source must be unaffected */
    v = 5;
    avl_remove(dst, &v, NULL);
    assert_true(avl_contains(src, &v));
 
    return_avl(dst);
    return_avl(src);
}
 
static void test_avl_copy_shares_comparator(void** state) {
    (void)state;
    avl_t* src = _make_tree(8);
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = copy_avl(src, a);
    assert_true(r.has_value);
    assert_ptr_equal(r.u.value->cmp, src->cmp);
    return_avl(r.u.value);
    return_avl(src);
}
 
static void test_avl_copy_preserves_allow_duplicates(void** state) {
    (void)state;
    avl_t* src = _make_dup_tree(8);
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = copy_avl(src, a);
    assert_true(r.has_value);
    assert_true(r.u.value->allow_duplicates);
    return_avl(r.u.value);
    return_avl(src);
}
 
static void test_avl_copy_with_different_allocator(void** state) {
    (void)state;
    avl_t* src = _make_tree(8);
    int32_t vals[] = { 4, 2, 6 };
    _insert_all(src, vals, 3);
 
    /* Copy using a fresh heap_allocator — both must work independently */
    allocator_vtable_t a2 = heap_allocator();
    avl_expect_t r = copy_avl(src, a2);
    assert_true(r.has_value);
    assert_int_equal((int)avl_size(r.u.value), 3);
    return_avl(r.u.value);
    return_avl(src);
}
 
// ================================================================================
// Group 12: duplicate policy
// ================================================================================
 
static void test_avl_dup_policy_false_rejects_equal_elements(void** state) {
    (void)state;
    avl_t* tree = _make_tree(8);
    int32_t v = 10;
    assert_int_equal(avl_insert(tree, &v), NO_ERROR);
    assert_int_equal(avl_insert(tree, &v), INVALID_ARG);
    assert_int_equal(avl_insert(tree, &v), INVALID_ARG);
    assert_int_equal((int)avl_size(tree), 1);
    return_avl(tree);
}
 
static void test_avl_dup_policy_true_accepts_equal_elements(void** state) {
    (void)state;
    avl_t* tree = _make_dup_tree(16);
    int32_t v = 10;
    for (int i = 0; i < 5; i++)
        assert_int_equal(avl_insert(tree, &v), NO_ERROR);
    assert_int_equal((int)avl_size(tree), 5);
    return_avl(tree);
}
 
static void test_avl_dup_foreach_visits_all_duplicates_in_order(void** state) {
    (void)state;
    avl_t* tree = _make_dup_tree(16);
    int32_t vals[] = { 5, 3, 5, 7, 3, 5 };
    for (int i = 0; i < 6; i++)
        avl_insert(tree, &vals[i]);
 
    _order_ctx_t ctx = { INT32_MIN, true, 0 };
    avl_foreach(tree, _order_iter, &ctx);
    assert_true(ctx.ok);
    assert_int_equal(ctx.count, 6);
    return_avl(tree);
}
 
static void test_avl_dup_remove_removes_one_instance(void** state) {
    (void)state;
    avl_t* tree = _make_dup_tree(16);
    int32_t v = 7;
    avl_insert(tree, &v);
    avl_insert(tree, &v);
    avl_insert(tree, &v);
    assert_int_equal((int)avl_size(tree), 3);
 
    avl_remove(tree, &v, NULL);
    assert_int_equal((int)avl_size(tree), 2);
    /* Remaining instances must still be present */
    assert_true(avl_contains(tree, &v));
    return_avl(tree);
}
 
// ================================================================================
// Group 13: stress / integration
// ================================================================================
 
static void test_avl_stress_sequential_insert_sorted_output(void** state) {
    (void)state;
    avl_t* tree = _make_tree(256);
    for (int32_t i = 1; i <= 200; i++)
        assert_int_equal(avl_insert(tree, &i), NO_ERROR);
    assert_int_equal((int)avl_size(tree), 200);
 
    _order_ctx_t ctx = { INT32_MIN, true, 0 };
    avl_foreach(tree, _order_iter, &ctx);
    assert_true(ctx.ok);
    assert_int_equal(ctx.count, 200);
    assert_true(avl_height(tree) <= 11);
    return_avl(tree);
}
 
static void test_avl_stress_reverse_insert_sorted_output(void** state) {
    (void)state;
    avl_t* tree = _make_tree(256);
    for (int32_t i = 200; i >= 1; i--)
        assert_int_equal(avl_insert(tree, &i), NO_ERROR);
 
    _order_ctx_t ctx = { INT32_MIN, true, 0 };
    avl_foreach(tree, _order_iter, &ctx);
    assert_true(ctx.ok);
    assert_int_equal(ctx.count, 200);
    assert_true(avl_height(tree) <= 11);
    return_avl(tree);
}
 
static void test_avl_stress_interleaved_insert_remove(void** state) {
    (void)state;
    avl_t* tree = _make_tree(64);
 
    /* Insert 50, remove 25, insert 25 more — tree must remain valid */
    for (int32_t i = 1; i <= 50; i++)
        avl_insert(tree, &i);
    for (int32_t i = 1; i <= 50; i += 2)
        avl_remove(tree, &i, NULL);
    for (int32_t i = 51; i <= 75; i++)
        avl_insert(tree, &i);
 
    /* All even numbers 2-50 plus 51-75 must be present */
    assert_int_equal((int)avl_size(tree), 50);
 
    _order_ctx_t ctx = { INT32_MIN, true, 0 };
    avl_foreach(tree, _order_iter, &ctx);
    assert_true(ctx.ok);
    assert_int_equal(ctx.count, 50);
    return_avl(tree);
}
 
static void test_avl_stress_overflow_sorted_output(void** state) {
    (void)state;
    /* Slab holds only 4 nodes; the rest must overflow correctly */
    avl_t* tree = _make_overflow_tree(4);
    for (int32_t i = 100; i >= 1; i--)
        assert_int_equal(avl_insert(tree, &i), NO_ERROR);
 
    _order_ctx_t ctx = { INT32_MIN, true, 0 };
    avl_foreach(tree, _order_iter, &ctx);
    assert_true(ctx.ok);
    assert_int_equal(ctx.count, 100);
    return_avl(tree);
}
 
static void test_avl_stress_double_type(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = init_avl(64, DOUBLE_TYPE, false, false, a, cmp_double);
    assert_true(r.has_value);
    avl_t* tree = r.u.value;
 
    double vals[] = { 3.14, 2.71, 1.41, 1.73, 0.57 };
    for (int i = 0; i < 5; i++)
        assert_int_equal(avl_insert(tree, &vals[i]), NO_ERROR);
 
    double min_out = 0.0, max_out = 0.0;
    avl_min(tree, &min_out);
    avl_max(tree, &max_out);
    assert_true(min_out < max_out);
    assert_true(min_out == 0.57);
    assert_true(max_out == 3.14);
    return_avl(tree);
}
 
static void test_avl_stress_copy_large_tree(void** state) {
    (void)state;
    avl_t* src = _make_tree(256);
    for (int32_t i = 1; i <= 200; i++)
        avl_insert(src, &i);
 
    allocator_vtable_t a = heap_allocator();
    avl_expect_t r = copy_avl(src, a);
    assert_true(r.has_value);
    avl_t* dst = r.u.value;
 
    assert_int_equal((int)avl_size(dst), 200);
    _order_ctx_t ctx = { INT32_MIN, true, 0 };
    avl_foreach(dst, _order_iter, &ctx);
    assert_true(ctx.ok);
 
    return_avl(dst);
    return_avl(src);
}
 
// ================================================================================
// Test registry
// ================================================================================
 
const struct CMUnitTest test_avl[] = {
 
    /* Group 1: init_avl */
    cmocka_unit_test(test_avl_init_null_allocator_fails),
    cmocka_unit_test(test_avl_init_null_comparator_fails),
    cmocka_unit_test(test_avl_init_zero_capacity_fails),
    cmocka_unit_test(test_avl_init_unknown_dtype_fails),
    cmocka_unit_test(test_avl_init_returns_valid_tree),
    cmocka_unit_test(test_avl_init_stores_dtype),
    cmocka_unit_test(test_avl_init_stores_comparator),
    cmocka_unit_test(test_avl_init_allow_duplicates_false_stored),
    cmocka_unit_test(test_avl_init_allow_duplicates_true_stored),
    cmocka_unit_test(test_avl_init_overflow_false_stored),
    cmocka_unit_test(test_avl_init_overflow_true_stored),
 
    /* Group 2: return_avl */
    cmocka_unit_test(test_avl_return_null_is_safe),
    cmocka_unit_test(test_avl_return_empty_tree_does_not_crash),
    cmocka_unit_test(test_avl_return_populated_tree_does_not_crash),
    cmocka_unit_test(test_avl_return_overflow_tree_does_not_crash),
 
    /* Group 3: avl_insert */
    cmocka_unit_test(test_avl_insert_null_tree_returns_null_pointer),
    cmocka_unit_test(test_avl_insert_null_data_returns_null_pointer),
    cmocka_unit_test(test_avl_insert_single_element_increments_size),
    cmocka_unit_test(test_avl_insert_duplicate_no_dup_returns_invalid_arg),
    cmocka_unit_test(test_avl_insert_duplicate_allowed_increments_size),
    cmocka_unit_test(test_avl_insert_no_overflow_capacity_overflow),
    cmocka_unit_test(test_avl_insert_overflow_grows_beyond_slab),
    cmocka_unit_test(test_avl_insert_maintains_sorted_order),
    cmocka_unit_test(test_avl_insert_height_bounded_by_avl_invariant),
    cmocka_unit_test(test_avl_insert_all_values_findable_after_insert),
 
    /* Group 4: avl_remove */
    cmocka_unit_test(test_avl_remove_null_tree_returns_null_pointer),
    cmocka_unit_test(test_avl_remove_null_key_returns_null_pointer),
    cmocka_unit_test(test_avl_remove_empty_tree_returns_empty),
    cmocka_unit_test(test_avl_remove_missing_key_returns_not_found),
    cmocka_unit_test(test_avl_remove_copies_element_to_out),
    cmocka_unit_test(test_avl_remove_null_out_discards_safely),
    cmocka_unit_test(test_avl_remove_decrements_size),
    cmocka_unit_test(test_avl_remove_leaf_node),
    cmocka_unit_test(test_avl_remove_node_with_one_child),
    cmocka_unit_test(test_avl_remove_node_with_two_children),
    cmocka_unit_test(test_avl_remove_root_of_single_element_tree),
    cmocka_unit_test(test_avl_remove_maintains_sorted_order),
    cmocka_unit_test(test_avl_remove_height_stays_bounded_after_removals),
    cmocka_unit_test(test_avl_remove_recycled_slot_reused_on_insert),
 
    /* Group 5: avl_contains */
    cmocka_unit_test(test_avl_contains_null_tree_returns_false),
    cmocka_unit_test(test_avl_contains_null_key_returns_false),
    cmocka_unit_test(test_avl_contains_empty_tree_returns_false),
    cmocka_unit_test(test_avl_contains_present_element_returns_true),
    cmocka_unit_test(test_avl_contains_absent_element_returns_false),
    cmocka_unit_test(test_avl_contains_returns_false_after_removal),
 
    /* Group 6: avl_find */
    cmocka_unit_test(test_avl_find_null_tree_returns_null_pointer),
    cmocka_unit_test(test_avl_find_null_key_returns_null_pointer),
    cmocka_unit_test(test_avl_find_null_out_returns_null_pointer),
    cmocka_unit_test(test_avl_find_empty_tree_returns_empty),
    cmocka_unit_test(test_avl_find_missing_key_returns_not_found),
    cmocka_unit_test(test_avl_find_copies_correct_value),
    cmocka_unit_test(test_avl_find_does_not_modify_tree),
 
    /* Group 7: avl_min and avl_max */
    cmocka_unit_test(test_avl_min_null_tree_returns_null_pointer),
    cmocka_unit_test(test_avl_min_null_out_returns_null_pointer),
    cmocka_unit_test(test_avl_min_empty_tree_returns_empty),
    cmocka_unit_test(test_avl_min_returns_correct_minimum),
    cmocka_unit_test(test_avl_min_single_element),
    cmocka_unit_test(test_avl_max_null_tree_returns_null_pointer),
    cmocka_unit_test(test_avl_max_null_out_returns_null_pointer),
    cmocka_unit_test(test_avl_max_empty_tree_returns_empty),
    cmocka_unit_test(test_avl_max_returns_correct_maximum),
    cmocka_unit_test(test_avl_max_single_element),
    cmocka_unit_test(test_avl_min_max_update_after_removal),
 
    /* Group 8: avl_foreach */
    cmocka_unit_test(test_avl_foreach_null_tree_returns_null_pointer),
    cmocka_unit_test(test_avl_foreach_null_fn_returns_null_pointer),
    cmocka_unit_test(test_avl_foreach_empty_tree_returns_empty),
    cmocka_unit_test(test_avl_foreach_visits_all_elements),
    cmocka_unit_test(test_avl_foreach_visits_in_sorted_order),
    cmocka_unit_test(test_avl_foreach_does_not_modify_tree),
    cmocka_unit_test(test_avl_foreach_null_ctx_forwarded),
    cmocka_unit_test(test_avl_foreach_sorted_order_after_removals),
    cmocka_unit_test(test_avl_foreach_duplicate_elements_all_visited),
 
    /* Group 9: avl_foreach_range */
    cmocka_unit_test(test_avl_range_null_tree_returns_null_pointer),
    cmocka_unit_test(test_avl_range_null_low_returns_null_pointer),
    cmocka_unit_test(test_avl_range_null_high_returns_null_pointer),
    cmocka_unit_test(test_avl_range_null_fn_returns_null_pointer),
    cmocka_unit_test(test_avl_range_empty_tree_returns_empty),
    cmocka_unit_test(test_avl_range_inverted_range_returns_invalid_arg),
    cmocka_unit_test(test_avl_range_visits_only_in_range_elements),
    cmocka_unit_test(test_avl_range_visits_in_sorted_order),
    cmocka_unit_test(test_avl_range_single_value_range),
    cmocka_unit_test(test_avl_range_no_elements_in_range),
    cmocka_unit_test(test_avl_range_full_tree_range),
 
    /* Group 10: avl_size, avl_height, avl_is_empty */
    cmocka_unit_test(test_avl_size_null_returns_zero),
    cmocka_unit_test(test_avl_size_empty_is_zero),
    cmocka_unit_test(test_avl_size_tracks_inserts_and_removals),
    cmocka_unit_test(test_avl_height_null_returns_zero),
    cmocka_unit_test(test_avl_height_empty_is_zero),
    cmocka_unit_test(test_avl_height_single_node_is_one),
    cmocka_unit_test(test_avl_height_decreases_after_removals),
    cmocka_unit_test(test_avl_is_empty_null_returns_true),
    cmocka_unit_test(test_avl_is_empty_reflects_contents),
 
    /* Group 11: copy_avl */
    cmocka_unit_test(test_avl_copy_null_src_fails),
    cmocka_unit_test(test_avl_copy_null_allocator_fails),
    cmocka_unit_test(test_avl_copy_empty_tree_produces_valid_copy),
    cmocka_unit_test(test_avl_copy_has_same_size),
    cmocka_unit_test(test_avl_copy_all_elements_present),
    cmocka_unit_test(test_avl_copy_is_sorted),
    cmocka_unit_test(test_avl_copy_is_independent_of_source),
    cmocka_unit_test(test_avl_copy_shares_comparator),
    cmocka_unit_test(test_avl_copy_preserves_allow_duplicates),
    cmocka_unit_test(test_avl_copy_with_different_allocator),
 
    /* Group 12: duplicate policy */
    cmocka_unit_test(test_avl_dup_policy_false_rejects_equal_elements),
    cmocka_unit_test(test_avl_dup_policy_true_accepts_equal_elements),
    cmocka_unit_test(test_avl_dup_foreach_visits_all_duplicates_in_order),
    cmocka_unit_test(test_avl_dup_remove_removes_one_instance),
 
    /* Group 13: stress / integration */
    cmocka_unit_test(test_avl_stress_sequential_insert_sorted_output),
    cmocka_unit_test(test_avl_stress_reverse_insert_sorted_output),
    cmocka_unit_test(test_avl_stress_interleaved_insert_remove),
    cmocka_unit_test(test_avl_stress_overflow_sorted_output),
    cmocka_unit_test(test_avl_stress_double_type),
    cmocka_unit_test(test_avl_stress_copy_large_tree),
};
const size_t test_avl_count = sizeof(test_avl) / sizeof(test_avl[0]);
// ================================================================================
// ================================================================================
// eof
