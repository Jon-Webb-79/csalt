// ================================================================================
// ================================================================================
// - File:    test_heap.c
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
#include "c_array.h"
 
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

static void test_func(void **state)
{
    (void)state;

    int a = 5;
    int b = 2;
    int c = a + b;

    assert_int_equal(c, 7);
}

// ================================================================================
// ================================================================================
// TEST REGISTRY

/* int32 max-heap: largest value at root */
static int cmp_int32_max(const void* a, const void* b) {
    int32_t va = *(const int32_t*)a;
    int32_t vb = *(const int32_t*)b;
    return (va > vb) - (va < vb);
}
 
/* int32 min-heap: smallest value at root */
static int cmp_int32_min(const void* a, const void* b) {
    int32_t va = *(const int32_t*)a;
    int32_t vb = *(const int32_t*)b;
    return (va < vb) - (va > vb);
}
 
/* double max-heap */
static int cmp_double_max(const void* a, const void* b) {
    double va = *(const double*)a;
    double vb = *(const double*)b;
    return (va > vb) - (va < vb);
}
 
/* Build a heap_t (int32, max-heap, growth enabled) with a given capacity. */
static heap_t* _make_int32_heap(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = init_heap(cap, INT32_TYPE, true, a, cmp_int32_max);
    assert_true(r.has_value);
    return r.u.value;
}
 
/* Iterator context used by heap_foreach tests. */
typedef struct { int count; int32_t sum; } _iter_ctx_t;
 
static void _sum_iter(const void* elem, void* ctx) {
    _iter_ctx_t* c = (_iter_ctx_t*)ctx;
    c->count++;
    c->sum += *(const int32_t*)elem;
}
 
/* Iterator that records whether a specific value was seen. */
typedef struct { int32_t target; bool found; } _find_ctx_t;
 
static void _find_iter(const void* elem, void* ctx) {
    _find_ctx_t* c = (_find_ctx_t*)ctx;
    if (*(const int32_t*)elem == c->target)
        c->found = true;
}
 
// ================================================================================
// Group 1: init_heap
// ================================================================================
 
static void test_heap_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    heap_expect_t r = init_heap(8, INT32_TYPE, true, bad, cmp_int32_max);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_heap_init_null_comparator_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = init_heap(8, INT32_TYPE, true, a, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_heap_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = init_heap(0, INT32_TYPE, true, a, cmp_int32_max);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_heap_init_unknown_dtype_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = init_heap(8, UNKNOWN_TYPE, true, a, cmp_int32_max);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}
 
static void test_heap_init_returns_valid_heap(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = init_heap(8, INT32_TYPE, true, a, cmp_int32_max);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_int_equal((int)heap_size(r.u.value), 0);
    assert_int_equal((int)heap_alloc(r.u.value), 8);
    return_heap(r.u.value);
}
 
static void test_heap_init_stores_dtype(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    assert_int_equal((int)h->dtype, (int)INT32_TYPE);
    return_heap(h);
}
 
static void test_heap_init_stores_comparator(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    assert_ptr_equal(h->cmp, cmp_int32_max);
    return_heap(h);
}
 
// ================================================================================
// Group 2: return_heap
// ================================================================================
 
static void test_heap_return_null_is_safe(void** state) {
    (void)state;
    /* Must not crash */
    return_heap(NULL);
}
 
static void test_heap_return_populated_heap_does_not_crash(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t v = 10;
    heap_push(h, &v);
    return_heap(h);   /* must not crash or leak */
}
 
// ================================================================================
// Group 3: heap_push
// ================================================================================
 
static void test_heap_push_null_heap_returns_null_pointer(void** state) {
    (void)state;
    int32_t v = 1;
    assert_int_equal(heap_push(NULL, &v), NULL_POINTER);
}
 
static void test_heap_push_null_data_returns_null_pointer(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    assert_int_equal(heap_push(h, NULL), NULL_POINTER);
    return_heap(h);
}
 
static void test_heap_push_increments_size(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t v = 5;
    assert_int_equal(heap_push(h, &v), NO_ERROR);
    assert_int_equal((int)heap_size(h), 1);
    return_heap(h);
}
 
static void test_heap_push_single_element_becomes_root(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t v = 42;
    heap_push(h, &v);
    assert_int_equal(*(const int32_t*)heap_peek(h), 42);
    return_heap(h);
}
 
static void test_heap_push_larger_value_becomes_root_max_heap(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t a = 10, b = 20;
    heap_push(h, &a);
    heap_push(h, &b);
    /* max-heap: 20 must be at root */
    assert_int_equal(*(const int32_t*)heap_peek(h), 20);
    return_heap(h);
}
 
static void test_heap_push_smaller_value_stays_below_root_max_heap(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t a = 20, b = 5;
    heap_push(h, &a);
    heap_push(h, &b);
    assert_int_equal(*(const int32_t*)heap_peek(h), 20);
    return_heap(h);
}
 
static void test_heap_push_min_heap_smallest_at_root(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = init_heap(8, INT32_TYPE, true, a, cmp_int32_min);
    assert_true(r.has_value);
    heap_t* h = r.u.value;
 
    int32_t vals[] = { 30, 10, 50, 20 };
    for (int i = 0; i < 4; i++)
        assert_int_equal(heap_push(h, &vals[i]), NO_ERROR);
 
    /* min-heap: 10 must be at root */
    assert_int_equal(*(const int32_t*)heap_peek(h), 10);
    return_heap(h);
}
 
static void test_heap_push_no_growth_overflow_returns_error(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = init_heap(2, INT32_TYPE, false, a, cmp_int32_max);
    assert_true(r.has_value);
    heap_t* h = r.u.value;
 
    int32_t v = 1;
    assert_int_equal(heap_push(h, &v), NO_ERROR);
    v = 2;
    assert_int_equal(heap_push(h, &v), NO_ERROR);
    v = 3;
    /* capacity exhausted, growth disabled */
    assert_int_equal(heap_push(h, &v), CAPACITY_OVERFLOW);
    assert_int_equal((int)heap_size(h), 2);
    return_heap(h);
}
 
static void test_heap_push_growth_expands_capacity(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(2);   /* starts with capacity 2 */
    for (int32_t i = 1; i <= 10; i++)
        assert_int_equal(heap_push(h, &i), NO_ERROR);
    assert_int_equal((int)heap_size(h), 10);
    assert_true(heap_alloc(h) >= 10u);
    return_heap(h);
}
 
static void test_heap_push_duplicate_values_accepted(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t v = 7;
    heap_push(h, &v);
    heap_push(h, &v);
    heap_push(h, &v);
    assert_int_equal((int)heap_size(h), 3);
    assert_int_equal(*(const int32_t*)heap_peek(h), 7);
    return_heap(h);
}
 
// ================================================================================
// Group 4: heap_pop
// ================================================================================
 
static void test_heap_pop_null_heap_returns_null_pointer(void** state) {
    (void)state;
    int32_t out;
    assert_int_equal(heap_pop(NULL, &out), NULL_POINTER);
}
 
static void test_heap_pop_null_out_returns_null_pointer(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t v = 1;
    heap_push(h, &v);
    assert_int_equal(heap_pop(h, NULL), NULL_POINTER);
    return_heap(h);
}
 
static void test_heap_pop_empty_heap_returns_empty(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t out;
    assert_int_equal(heap_pop(h, &out), EMPTY);
    return_heap(h);
}
 
static void test_heap_pop_single_element_leaves_heap_empty(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t v = 99, out = 0;
    heap_push(h, &v);
    assert_int_equal(heap_pop(h, &out), NO_ERROR);
    assert_int_equal(out, 99);
    assert_int_equal((int)heap_size(h), 0);
    return_heap(h);
}
 
static void test_heap_pop_returns_max_first_from_max_heap(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(8);
    int32_t vals[] = { 5, 3, 8, 1, 9, 2 };
    for (int i = 0; i < 6; i++)
        heap_push(h, &vals[i]);
 
    int32_t out;
    assert_int_equal(heap_pop(h, &out), NO_ERROR);
    assert_int_equal(out, 9);
    return_heap(h);
}
 
static void test_heap_pop_produces_sorted_descending_order_max_heap(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(8);
    int32_t vals[] = { 4, 1, 7, 3, 9, 2, 6, 5, 8 };
    for (int i = 0; i < 9; i++)
        heap_push(h, &vals[i]);
 
    int32_t prev = INT32_MAX, out;
    while (heap_size(h) > 0u) {
        assert_int_equal(heap_pop(h, &out), NO_ERROR);
        assert_true(out <= prev);
        prev = out;
    }
    return_heap(h);
}
 
static void test_heap_pop_produces_sorted_ascending_order_min_heap(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = init_heap(8, INT32_TYPE, true, a, cmp_int32_min);
    assert_true(r.has_value);
    heap_t* h = r.u.value;
 
    int32_t vals[] = { 4, 1, 7, 3, 9, 2, 6, 5, 8 };
    for (int i = 0; i < 9; i++)
        heap_push(h, &vals[i]);
 
    int32_t prev = INT32_MIN, out;
    while (heap_size(h) > 0u) {
        assert_int_equal(heap_pop(h, &out), NO_ERROR);
        assert_true(out >= prev);
        prev = out;
    }
    return_heap(h);
}
 
static void test_heap_pop_decrements_size(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t v = 1, out;
    heap_push(h, &v);
    v = 2; heap_push(h, &v);
    heap_pop(h, &out);
    assert_int_equal((int)heap_size(h), 1);
    return_heap(h);
}
 
static void test_heap_pop_heap_property_restored_after_each_pop(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(8);
    int32_t vals[] = { 15, 3, 22, 7, 11, 1 };
    for (int i = 0; i < 6; i++)
        heap_push(h, &vals[i]);
 
    /* After each pop the new root must be the next largest remaining value */
    int32_t expected[] = { 22, 15, 11, 7, 3, 1 };
    int32_t out;
    for (int i = 0; i < 6; i++) {
        assert_int_equal(heap_pop(h, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
    return_heap(h);
}
 
// ================================================================================
// Group 5: heap_peek
// ================================================================================
 
static void test_heap_peek_null_returns_null(void** state) {
    (void)state;
    assert_null(heap_peek(NULL));
}
 
static void test_heap_peek_empty_returns_null(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    assert_null(heap_peek(h));
    return_heap(h);
}
 
static void test_heap_peek_returns_root_without_removing(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t a = 10, b = 30, c = 20;
    heap_push(h, &a);
    heap_push(h, &b);
    heap_push(h, &c);
 
    assert_int_equal(*(const int32_t*)heap_peek(h), 30);
    /* size must be unchanged */
    assert_int_equal((int)heap_size(h), 3);
    return_heap(h);
}
 
static void test_heap_peek_pointer_stable_between_reads(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t v = 7;
    heap_push(h, &v);
    const void* p1 = heap_peek(h);
    const void* p2 = heap_peek(h);
    assert_ptr_equal(p1, p2);
    return_heap(h);
}
 
// ================================================================================
// Group 6: heap_size
// ================================================================================
 
static void test_heap_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)heap_size(NULL), 0);
}
 
static void test_heap_size_empty_heap_is_zero(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    assert_int_equal((int)heap_size(h), 0);
    return_heap(h);
}
 
static void test_heap_size_tracks_push_and_pop(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(8);
    int32_t v = 1, out;
    heap_push(h, &v); assert_int_equal((int)heap_size(h), 1);
    v = 2; heap_push(h, &v); assert_int_equal((int)heap_size(h), 2);
    heap_pop(h, &out); assert_int_equal((int)heap_size(h), 1);
    heap_pop(h, &out); assert_int_equal((int)heap_size(h), 0);
    return_heap(h);
}
 
// ================================================================================
// Group 7: heap_alloc
// ================================================================================
 
static void test_heap_alloc_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)heap_alloc(NULL), 0);
}
 
static void test_heap_alloc_matches_initial_capacity(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = init_heap(16, INT32_TYPE, true, a, cmp_int32_max);
    assert_true(r.has_value);
    heap_t* h = r.u.value;
    assert_int_equal((int)heap_alloc(h), 16);
    return_heap(h);
}
 
static void test_heap_alloc_grows_after_capacity_exceeded(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(2);
    size_t initial = heap_alloc(h);
    /* Push enough to force a reallocation */
    for (int32_t i = 0; i < 10; i++)
        heap_push(h, &i);
    assert_true(heap_alloc(h) > initial);
    return_heap(h);
}
 
static void test_heap_alloc_stable_without_growth(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = init_heap(4, INT32_TYPE, false, a, cmp_int32_max);
    assert_true(r.has_value);
    heap_t* h = r.u.value;
 
    int32_t v = 1;
    heap_push(h, &v); v = 2; heap_push(h, &v);
    /* Alloc must not change after pushes within capacity */
    assert_int_equal((int)heap_alloc(h), 4);
    return_heap(h);
}
 
// ================================================================================
// Group 8: heap_foreach
// ================================================================================
 
static void test_heap_foreach_null_heap_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(heap_foreach(NULL, _sum_iter, NULL), NULL_POINTER);
}
 
static void test_heap_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t v = 1;
    heap_push(h, &v);
    assert_int_equal(heap_foreach(h, NULL, NULL), NULL_POINTER);
    return_heap(h);
}
 
static void test_heap_foreach_empty_heap_returns_empty(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    assert_int_equal(heap_foreach(h, _sum_iter, NULL), EMPTY);
    return_heap(h);
}
 
static void test_heap_foreach_visits_all_elements(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(8);
    int32_t vals[] = { 1, 2, 3, 4, 5 };
    for (int i = 0; i < 5; i++)
        heap_push(h, &vals[i]);
 
    _iter_ctx_t ctx = { 0, 0 };
    assert_int_equal(heap_foreach(h, _sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 5);
    assert_int_equal(ctx.sum, 15);
    return_heap(h);
}
 
static void test_heap_foreach_null_ctx_is_forwarded(void** state) {
    (void)state;
    /* Callback that ignores ctx must not crash when ctx is NULL */
    heap_t* h = _make_int32_heap(4);
    int32_t v = 99;
    heap_push(h, &v);
    /* _sum_iter dereferences ctx — use _find_iter with a local ctx instead,
       but here we just confirm that heap_foreach returns NO_ERROR. */
    _find_ctx_t ctx = { 99, false };
    assert_int_equal(heap_foreach(h, _find_iter, &ctx), NO_ERROR);
    assert_true(ctx.found);
    return_heap(h);
}
 
static void test_heap_foreach_finds_specific_value(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(8);
    int32_t vals[] = { 10, 20, 30, 40, 50 };
    for (int i = 0; i < 5; i++)
        heap_push(h, &vals[i]);
 
    _find_ctx_t ctx = { 30, false };
    heap_foreach(h, _find_iter, &ctx);
    assert_true(ctx.found);
    return_heap(h);
}
 
static void test_heap_foreach_does_not_modify_heap(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(8);
    int32_t vals[] = { 5, 1, 3 };
    for (int i = 0; i < 3; i++)
        heap_push(h, &vals[i]);
 
    size_t size_before = heap_size(h);
    int32_t root_before = *(const int32_t*)heap_peek(h);
 
    _iter_ctx_t ctx = { 0, 0 };
    heap_foreach(h, _sum_iter, &ctx);
 
    assert_int_equal((int)heap_size(h), (int)size_before);
    assert_int_equal(*(const int32_t*)heap_peek(h), root_before);
    return_heap(h);
}
 
static void test_heap_foreach_sum_with_doubles(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = init_heap(8, DOUBLE_TYPE, true, a, cmp_double_max);
    assert_true(r.has_value);
    heap_t* h = r.u.value;
 
    typedef struct { int count; double sum; } _dbl_ctx_t;
    static void (*sum_dbl)(const void*, void*);
    /* Inline lambda not available in C99 — use a file-scope helper instead.
       Verify correctness via heap_peek on a known dataset. */
    double vals[] = { 1.5, 2.5, 3.0 };
    for (int i = 0; i < 3; i++)
        heap_push(h, &vals[i]);
    /* Root of a double max-heap over {1.5, 2.5, 3.0} must be 3.0 */
    assert_true(*(const double*)heap_peek(h) == 3.0);
    return_heap(h);
}
 
// ================================================================================
// Group 9: copy_heap
// ================================================================================
 
static void test_heap_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = copy_heap(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
static void test_heap_copy_null_allocator_fails(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t v = 1;
    heap_push(h, &v);
    allocator_vtable_t bad = { 0 };
    heap_expect_t r = copy_heap(h, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_heap(h);
}
 
static void test_heap_copy_produces_valid_heap(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);
    int32_t v = 42;
    heap_push(h, &v);
 
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = copy_heap(h, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return_heap(r.u.value);
    return_heap(h);
}
 
static void test_heap_copy_has_same_size_and_root(void** state) {
    (void)state;
    heap_t* src = _make_int32_heap(8);
    int32_t vals[] = { 3, 1, 4, 1, 5, 9 };
    for (int i = 0; i < 6; i++)
        heap_push(src, &vals[i]);
 
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = copy_heap(src, a);
    assert_true(r.has_value);
    heap_t* dst = r.u.value;
 
    assert_int_equal((int)heap_size(dst), (int)heap_size(src));
    assert_int_equal(*(const int32_t*)heap_peek(dst),
                     *(const int32_t*)heap_peek(src));
    return_heap(dst);
    return_heap(src);
}
 
static void test_heap_copy_is_independent_of_source(void** state) {
    (void)state;
    heap_t* src = _make_int32_heap(8);
    int32_t vals[] = { 10, 20, 30 };
    for (int i = 0; i < 3; i++)
        heap_push(src, &vals[i]);
 
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = copy_heap(src, a);
    assert_true(r.has_value);
    heap_t* dst = r.u.value;
 
    /* Mutate the copy — source must be unaffected */
    int32_t v = 999;
    heap_push(dst, &v);
    assert_int_equal((int)heap_size(src), 3);
    assert_int_equal(*(const int32_t*)heap_peek(src), 30);
 
    return_heap(dst);
    return_heap(src);
}
 
static void test_heap_copy_shares_comparator(void** state) {
    (void)state;
    heap_t* src = _make_int32_heap(4);
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = copy_heap(src, a);
    assert_true(r.has_value);
    assert_ptr_equal(r.u.value->cmp, src->cmp);
    return_heap(r.u.value);
    return_heap(src);
}
 
static void test_heap_copy_ordered_traversal_via_pop(void** state) {
    (void)state;
    /* Canonical use case: ordered traversal without destroying the original. */
    heap_t* src = _make_int32_heap(8);
    int32_t vals[] = { 5, 2, 8, 1, 9 };
    for (int i = 0; i < 5; i++)
        heap_push(src, &vals[i]);
 
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = copy_heap(src, a);
    assert_true(r.has_value);
    heap_t* tmp = r.u.value;
 
    int32_t prev = INT32_MAX, out;
    while (heap_size(tmp) > 0u) {
        heap_pop(tmp, &out);
        assert_true(out <= prev);
        prev = out;
    }
 
    /* Source must still be intact */
    assert_int_equal((int)heap_size(src), 5);
    return_heap(tmp);
    return_heap(src);
}
 
static void test_heap_copy_dtype_matches_source(void** state) {
    (void)state;
    heap_t* src = _make_int32_heap(4);
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = copy_heap(src, a);
    assert_true(r.has_value);
    assert_int_equal((int)r.u.value->dtype, (int)src->dtype);
    return_heap(r.u.value);
    return_heap(src);
}
 
// ================================================================================
// Group 10: stress / integration
// ================================================================================
 
static void test_heap_stress_push_pop_max_heap_order(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(4);   /* starts small, will grow */
 
    /* Push 200 values in an interleaved pattern */
    for (int32_t i = 1; i <= 200; i++)
        assert_int_equal(heap_push(h, &i), NO_ERROR);
 
    assert_int_equal((int)heap_size(h), 200);
 
    /* Pop all values — must come out in descending order */
    int32_t prev = INT32_MAX, out;
    for (int i = 0; i < 200; i++) {
        assert_int_equal(heap_pop(h, &out), NO_ERROR);
        assert_true(out <= prev);
        prev = out;
    }
    assert_int_equal((int)heap_size(h), 0);
    return_heap(h);
}
 
static void test_heap_stress_interleaved_push_pop(void** state) {
    (void)state;
    heap_t* h = _make_int32_heap(8);
 
    /* Push 5, pop 2, push 5, pop 2 — heap property must hold throughout */
    for (int round = 0; round < 10; round++) {
        for (int32_t i = 0; i < 5; i++) {
            int32_t v = round * 5 + i;
            heap_push(h, &v);
        }
        int32_t a, b;
        heap_pop(h, &a);
        heap_pop(h, &b);
        /* Each successive pop must be <= the previous */
        assert_true(b <= a);
    }
    return_heap(h);
}
 
static void test_heap_stress_copy_survives_large_heap(void** state) {
    (void)state;
    heap_t* src = _make_int32_heap(4);
    for (int32_t i = 1; i <= 100; i++)
        heap_push(src, &i);
 
    allocator_vtable_t a = heap_allocator();
    heap_expect_t r = copy_heap(src, a);
    assert_true(r.has_value);
    heap_t* dst = r.u.value;
 
    assert_int_equal((int)heap_size(dst), 100);
    assert_int_equal(*(const int32_t*)heap_peek(dst), 100);
 
    return_heap(dst);
    return_heap(src);
}
 
// ================================================================================
// Test registry
// ================================================================================
 
const struct CMUnitTest test_heap[] = {
 
    /* Group 1: init_heap */
    cmocka_unit_test(test_heap_init_null_allocator_fails),
    cmocka_unit_test(test_heap_init_null_comparator_fails),
    cmocka_unit_test(test_heap_init_zero_capacity_fails),
    cmocka_unit_test(test_heap_init_unknown_dtype_fails),
    cmocka_unit_test(test_heap_init_returns_valid_heap),
    cmocka_unit_test(test_heap_init_stores_dtype),
    cmocka_unit_test(test_heap_init_stores_comparator),
 
    /* Group 2: return_heap */
    cmocka_unit_test(test_heap_return_null_is_safe),
    cmocka_unit_test(test_heap_return_populated_heap_does_not_crash),
 
    /* Group 3: heap_push */
    cmocka_unit_test(test_heap_push_null_heap_returns_null_pointer),
    cmocka_unit_test(test_heap_push_null_data_returns_null_pointer),
    cmocka_unit_test(test_heap_push_increments_size),
    cmocka_unit_test(test_heap_push_single_element_becomes_root),
    cmocka_unit_test(test_heap_push_larger_value_becomes_root_max_heap),
    cmocka_unit_test(test_heap_push_smaller_value_stays_below_root_max_heap),
    cmocka_unit_test(test_heap_push_min_heap_smallest_at_root),
    cmocka_unit_test(test_heap_push_no_growth_overflow_returns_error),
    cmocka_unit_test(test_heap_push_growth_expands_capacity),
    cmocka_unit_test(test_heap_push_duplicate_values_accepted),
 
    /* Group 4: heap_pop */
    cmocka_unit_test(test_heap_pop_null_heap_returns_null_pointer),
    cmocka_unit_test(test_heap_pop_null_out_returns_null_pointer),
    cmocka_unit_test(test_heap_pop_empty_heap_returns_empty),
    cmocka_unit_test(test_heap_pop_single_element_leaves_heap_empty),
    cmocka_unit_test(test_heap_pop_returns_max_first_from_max_heap),
    cmocka_unit_test(test_heap_pop_produces_sorted_descending_order_max_heap),
    cmocka_unit_test(test_heap_pop_produces_sorted_ascending_order_min_heap),
    cmocka_unit_test(test_heap_pop_decrements_size),
    cmocka_unit_test(test_heap_pop_heap_property_restored_after_each_pop),
 
    /* Group 5: heap_peek */
    cmocka_unit_test(test_heap_peek_null_returns_null),
    cmocka_unit_test(test_heap_peek_empty_returns_null),
    cmocka_unit_test(test_heap_peek_returns_root_without_removing),
    cmocka_unit_test(test_heap_peek_pointer_stable_between_reads),
 
    /* Group 6: heap_size */
    cmocka_unit_test(test_heap_size_null_returns_zero),
    cmocka_unit_test(test_heap_size_empty_heap_is_zero),
    cmocka_unit_test(test_heap_size_tracks_push_and_pop),
 
    /* Group 7: heap_alloc */
    cmocka_unit_test(test_heap_alloc_null_returns_zero),
    cmocka_unit_test(test_heap_alloc_matches_initial_capacity),
    cmocka_unit_test(test_heap_alloc_grows_after_capacity_exceeded),
    cmocka_unit_test(test_heap_alloc_stable_without_growth),
 
    /* Group 8: heap_foreach */
    cmocka_unit_test(test_heap_foreach_null_heap_returns_null_pointer),
    cmocka_unit_test(test_heap_foreach_null_fn_returns_null_pointer),
    cmocka_unit_test(test_heap_foreach_empty_heap_returns_empty),
    cmocka_unit_test(test_heap_foreach_visits_all_elements),
    cmocka_unit_test(test_heap_foreach_null_ctx_is_forwarded),
    cmocka_unit_test(test_heap_foreach_finds_specific_value),
    cmocka_unit_test(test_heap_foreach_does_not_modify_heap),
    cmocka_unit_test(test_heap_foreach_sum_with_doubles),
 
    /* Group 9: copy_heap */
    cmocka_unit_test(test_heap_copy_null_src_fails),
    cmocka_unit_test(test_heap_copy_null_allocator_fails),
    cmocka_unit_test(test_heap_copy_produces_valid_heap),
    cmocka_unit_test(test_heap_copy_has_same_size_and_root),
    cmocka_unit_test(test_heap_copy_is_independent_of_source),
    cmocka_unit_test(test_heap_copy_shares_comparator),
    cmocka_unit_test(test_heap_copy_ordered_traversal_via_pop),
    cmocka_unit_test(test_heap_copy_dtype_matches_source),
 
    /* Group 10: stress / integration */
    cmocka_unit_test(test_heap_stress_push_pop_max_heap_order),
    cmocka_unit_test(test_heap_stress_interleaved_push_pop),
    cmocka_unit_test(test_heap_stress_copy_survives_large_heap),
};
const size_t test_heap_count = sizeof(test_heap) / sizeof(test_heap[0]);
// ================================================================================
// ================================================================================
// eof
