// ================================================================================
// ================================================================================
// - File:    test_slist.c
// - Purpose: Unit tests for slist_t — a singly-linked list with a pre-allocated
//            node slab.  Tests are written through a thin entry_slist_t wrapper
//            that mirrors how a real caller would use the generic engine with
//            their own struct type.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 30, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_allocator.h"
#include "c_dtypes.h"
#include "c_error.h"
#include "c_list.h"

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>
// ================================================================================
// ================================================================================

// ================================================================================
// Wrapper type
// ================================================================================

#define ENTRY_TYPE (USER_BASE_TYPE + 1u)

/* entry_t represents a realistic struct payload — e.g. a task queue entry. */
typedef struct {
    int   id;
    float value;
    char  label[16];
} entry_t;

/* Thin typedef alias — exactly as a real caller would write. */
typedef slist_t entry_slist_t;

static inline slist_expect_t init_entry_slist(size_t cap,
                                               bool   allow_overflow,
                                               allocator_vtable_t a) {
    static const dtype_t entry_desc = {
        ENTRY_TYPE, sizeof(entry_t), "entry"
    };
    if (ensure_dtype_registered(&entry_desc) == false) {
        return (slist_expect_t){.has_value = false, .u.error = INVALID_ARG};
    };
    return init_slist(cap, sizeof(entry_t), ENTRY_TYPE, allow_overflow, a);
}

static inline void return_entry_slist(entry_slist_t* l) {
    return_slist(l);
}

static inline error_code_t push_back_entry(entry_slist_t* l, const entry_t* e) {
    return push_back_slist(l, e);
}

static inline error_code_t push_front_entry(entry_slist_t* l, const entry_t* e) {
    return push_front_slist(l, e);
}

static inline error_code_t push_at_entry(entry_slist_t* l, size_t idx,
                                          const entry_t* e) {
    return push_at_slist(l, idx, e);
}

static inline error_code_t pop_front_entry(entry_slist_t* l, entry_t* out) {
    return pop_front_slist(l, out);
}

static inline error_code_t pop_back_entry(entry_slist_t* l, entry_t* out) {
    return pop_back_slist(l, out);
}

static inline error_code_t pop_at_entry(entry_slist_t* l, size_t idx,
                                         entry_t* out) {
    return pop_at_slist(l, idx, out);
}

static inline error_code_t get_entry(const entry_slist_t* l, size_t idx,
                                      entry_t* out) {
    return get_slist(l, idx, out);
}

static inline error_code_t peek_front_entry(const entry_slist_t* l,
                                             entry_t* out) {
    return peek_front_slist(l, out);
}

static inline error_code_t peek_back_entry(const entry_slist_t* l,
                                            entry_t* out) {
    return peek_back_slist(l, out);
}

static inline slist_index_expect_t contains_entry(const entry_slist_t* l,
                                                    const entry_t* needle) {
    return contains_slist(l, needle);
}

static inline error_code_t clear_entry_slist(entry_slist_t* l) {
    return clear_slist(l);
}

static inline slist_expect_t copy_entry_slist(const entry_slist_t* src,
                                               allocator_vtable_t a) {
    return copy_slist(src, a);
}

static inline error_code_t reverse_entry_slist(entry_slist_t* l) {
    return reverse_slist(l);
}

static inline error_code_t concat_entry_slist(entry_slist_t* dst,
                                               const entry_slist_t* src) {
    return concat_slist(dst, src);
}

static inline error_code_t foreach_entry_slist(const entry_slist_t* l,
                                                slist_iter_fn fn,
                                                void* user_data) {
    return foreach_slist(l, fn, user_data);
}

// ================================================================================
// Helpers
// ================================================================================

static entry_slist_t* _make_list(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    slist_expect_t r = init_entry_slist(cap, false, a);
    assert_true(r.has_value);
    return r.u.value;
}

static entry_slist_t* _make_overflow_list(size_t cap) {
    allocator_vtable_t a = heap_allocator();
    slist_expect_t r = init_entry_slist(cap, true, a);
    assert_true(r.has_value);
    return r.u.value;
}

static entry_t _make_entry(int id, float value, const char* label) {
    entry_t e;
    e.id    = id;
    e.value = value;
    memset(e.label, 0, sizeof(e.label));
    strncpy(e.label, label, sizeof(e.label) - 1u);
    return e;
}

/* foreach accumulator — sums id fields across all visited nodes. */
typedef struct { int count; int id_sum; } _sl_iter_ctx_t;

static void _sl_sum_iter(const void* val, size_t index, void* ud) {
    (void)index;
    const entry_t*    e = (const entry_t*)val;
    _sl_iter_ctx_t*   c = (_sl_iter_ctx_t*)ud;
    c->count++;
    c->id_sum += e->id;
}

// ================================================================================
// Group 1: init_slist / return_slist
// ================================================================================

static void test_sl_init_null_allocator_fails(void** state) {
    (void)state;
    allocator_vtable_t bad = { 0 };
    slist_expect_t r = init_entry_slist(8, false, bad);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_sl_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    slist_expect_t r = init_entry_slist(0, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_sl_init_zero_data_size_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    slist_expect_t r = init_slist(8, 0, ENTRY_TYPE, false, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_sl_init_returns_empty_list(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(8);
    assert_true(is_slist_empty(l));
    assert_int_equal((int)slist_size(l), 0);
    assert_int_equal((int)slist_slab_alloc(l), 8);
    assert_int_equal((int)slist_slab_used(l), 0);
    return_entry_slist(l);
}

static void test_sl_init_data_size_is_sizeof_entry(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    assert_int_equal((int)slist_data_size(l), (int)sizeof(entry_t));
    return_entry_slist(l);
}

static void test_sl_return_null_is_safe(void** state) {
    (void)state;
    return_entry_slist(NULL);   /* must not crash */
}

// ================================================================================
// Group 2: push_back / push_front
// ================================================================================

static void test_sl_push_back_null_list_returns_null_pointer(void** state) {
    (void)state;
    entry_t e = _make_entry(1, 1.0f, "a");
    assert_int_equal(push_back_entry(NULL, &e), NULL_POINTER);
}

static void test_sl_push_back_null_value_returns_null_pointer(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    assert_int_equal(push_back_slist(l, NULL), NULL_POINTER);
    return_entry_slist(l);
}

static void test_sl_push_back_increments_len(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t e = _make_entry(1, 1.0f, "task_a");
    assert_int_equal(push_back_entry(l, &e), NO_ERROR);
    assert_int_equal((int)slist_size(l), 1);
    assert_false(is_slist_empty(l));
    return_entry_slist(l);
}

static void test_sl_push_back_order_is_preserved(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(1, 1.0f, "first");
    entry_t b = _make_entry(2, 2.0f, "second");
    entry_t c = _make_entry(3, 3.0f, "third");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    push_back_entry(l, &c);
    entry_t out = {0};
    get_entry(l, 0, &out); assert_int_equal(out.id, 1);
    get_entry(l, 1, &out); assert_int_equal(out.id, 2);
    get_entry(l, 2, &out); assert_int_equal(out.id, 3);
    return_entry_slist(l);
}

static void test_sl_push_front_places_at_head(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(1, 1.0f, "first");
    entry_t b = _make_entry(2, 2.0f, "new_head");
    push_back_entry(l, &a);
    push_front_entry(l, &b);
    entry_t out = {0};
    get_entry(l, 0, &out);
    assert_int_equal(out.id, 2);
    assert_string_equal(out.label, "new_head");
    return_entry_slist(l);
}

static void test_sl_push_back_stores_struct_correctly(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t e = _make_entry(42, 3.14f, "pi_task");
    push_back_entry(l, &e);
    entry_t out = {0};
    assert_int_equal(get_entry(l, 0, &out), NO_ERROR);
    assert_int_equal(out.id, 42);
    assert_float_equal(out.value, 3.14f, 1e-6f);
    assert_string_equal(out.label, "pi_task");
    return_entry_slist(l);
}

// ================================================================================
// Group 3: push_at
// ================================================================================

static void test_sl_push_at_null_list_returns_null_pointer(void** state) {
    (void)state;
    entry_t e = _make_entry(1, 1.0f, "a");
    assert_int_equal(push_at_slist(NULL, 0, &e), NULL_POINTER);
}

static void test_sl_push_at_out_of_range_returns_invalid_arg(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t e = _make_entry(1, 1.0f, "a");
    assert_int_equal(push_at_entry(l, 1, &e), INVALID_ARG);
    return_entry_slist(l);
}

static void test_sl_push_at_zero_delegates_to_push_front(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    push_back_entry(l, &a);
    push_at_entry(l, 0, &b);
    entry_t out = {0};
    get_entry(l, 0, &out);
    assert_int_equal(out.id, 2);
    return_entry_slist(l);
}

static void test_sl_push_at_len_delegates_to_push_back(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    push_back_entry(l, &a);
    push_at_entry(l, 1, &b);
    entry_t out = {0};
    get_entry(l, 1, &out);
    assert_int_equal(out.id, 2);
    return_entry_slist(l);
}

static void test_sl_push_at_mid_inserts_correctly(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(8);
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(3, 3.0f, "c");
    entry_t c = _make_entry(2, 2.0f, "b");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    push_at_entry(l, 1, &c);   /* insert between a and b */
    assert_int_equal((int)slist_size(l), 3);
    entry_t out = {0};
    get_entry(l, 0, &out); assert_int_equal(out.id, 1);
    get_entry(l, 1, &out); assert_int_equal(out.id, 2);
    get_entry(l, 2, &out); assert_int_equal(out.id, 3);
    return_entry_slist(l);
}

// ================================================================================
// Group 4: pop_front / pop_back / pop_at
// ================================================================================

static void test_sl_pop_front_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(pop_front_slist(NULL, NULL), NULL_POINTER);
}

static void test_sl_pop_front_empty_returns_not_found(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    assert_int_equal(pop_front_entry(l, NULL), NOT_FOUND);
    return_entry_slist(l);
}

static void test_sl_pop_front_returns_correct_value(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(10, 1.0f, "head");
    entry_t b = _make_entry(20, 2.0f, "tail");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    entry_t out = {0};
    assert_int_equal(pop_front_entry(l, &out), NO_ERROR);
    assert_int_equal(out.id, 10);
    assert_int_equal((int)slist_size(l), 1);
    return_entry_slist(l);
}

static void test_sl_pop_front_null_out_discards_safely(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t e = _make_entry(1, 1.0f, "a");
    push_back_entry(l, &e);
    assert_int_equal(pop_front_entry(l, NULL), NO_ERROR);
    assert_true(is_slist_empty(l));
    return_entry_slist(l);
}

static void test_sl_pop_back_returns_correct_value(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(10, 1.0f, "head");
    entry_t b = _make_entry(20, 2.0f, "tail");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    entry_t out = {0};
    assert_int_equal(pop_back_entry(l, &out), NO_ERROR);
    assert_int_equal(out.id, 20);
    assert_int_equal((int)slist_size(l), 1);
    return_entry_slist(l);
}

static void test_sl_pop_back_empty_returns_not_found(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    assert_int_equal(pop_back_entry(l, NULL), NOT_FOUND);
    return_entry_slist(l);
}

static void test_sl_pop_back_single_node_leaves_empty(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t e = _make_entry(1, 1.0f, "only");
    push_back_entry(l, &e);
    assert_int_equal(pop_back_entry(l, NULL), NO_ERROR);
    assert_true(is_slist_empty(l));
    assert_null(l->head);
    assert_null(l->tail);
    return_entry_slist(l);
}

static void test_sl_pop_at_out_of_range_returns_invalid_arg(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t e = _make_entry(1, 1.0f, "a");
    push_back_entry(l, &e);
    assert_int_equal(pop_at_entry(l, 1, NULL), INVALID_ARG);
    return_entry_slist(l);
}

static void test_sl_pop_at_mid_removes_correct_node(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(8);
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    entry_t c = _make_entry(3, 3.0f, "c");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    push_back_entry(l, &c);
    entry_t out = {0};
    assert_int_equal(pop_at_entry(l, 1, &out), NO_ERROR);
    assert_int_equal(out.id, 2);
    assert_int_equal((int)slist_size(l), 2);
    get_entry(l, 0, &out); assert_int_equal(out.id, 1);
    get_entry(l, 1, &out); assert_int_equal(out.id, 3);
    return_entry_slist(l);
}

// ================================================================================
// Group 5: get / peek
// ================================================================================

static void test_sl_get_null_list_returns_null_pointer(void** state) {
    (void)state;
    entry_t out = {0};
    assert_int_equal(get_slist(NULL, 0, &out), NULL_POINTER);
}

static void test_sl_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t e = _make_entry(1, 1.0f, "a");
    push_back_entry(l, &e);
    assert_int_equal(get_slist(l, 0, NULL), NULL_POINTER);
    return_entry_slist(l);
}

static void test_sl_get_out_of_range_returns_not_found(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t out = {0};
    assert_int_equal(get_entry(l, 0, &out), NOT_FOUND);
    return_entry_slist(l);
}

static void test_sl_get_retrieves_correct_struct(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(7, 7.7f, "seven");
    entry_t b = _make_entry(8, 8.8f, "eight");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    entry_t out = {0};
    assert_int_equal(get_entry(l, 1, &out), NO_ERROR);
    assert_int_equal(out.id, 8);
    assert_float_equal(out.value, 8.8f, 1e-6f);
    assert_string_equal(out.label, "eight");
    return_entry_slist(l);
}

static void test_sl_peek_front_null_returns_null_pointer(void** state) {
    (void)state;
    entry_t out = {0};
    assert_int_equal(peek_front_slist(NULL, &out), NULL_POINTER);
}

static void test_sl_peek_front_empty_returns_not_found(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t out = {0};
    assert_int_equal(peek_front_entry(l, &out), NOT_FOUND);
    return_entry_slist(l);
}

static void test_sl_peek_front_does_not_remove_node(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t e = _make_entry(99, 9.9f, "peek");
    push_back_entry(l, &e);
    entry_t out = {0};
    assert_int_equal(peek_front_entry(l, &out), NO_ERROR);
    assert_int_equal(out.id, 99);
    assert_int_equal((int)slist_size(l), 1);   /* node must still be present */
    return_entry_slist(l);
}

static void test_sl_peek_back_does_not_remove_node(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    entry_t out = {0};
    assert_int_equal(peek_back_entry(l, &out), NO_ERROR);
    assert_int_equal(out.id, 2);
    assert_int_equal((int)slist_size(l), 2);
    return_entry_slist(l);
}

// ================================================================================
// Group 6: slab / overflow behaviour
// ================================================================================

static void test_sl_slab_full_deny_returns_capacity_overflow(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(2);   /* capacity 2, overflow disabled */
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    entry_t c = _make_entry(3, 3.0f, "c");
    assert_int_equal(push_back_entry(l, &a), NO_ERROR);
    assert_int_equal(push_back_entry(l, &b), NO_ERROR);
    assert_int_equal(push_back_entry(l, &c), CAPACITY_OVERFLOW);
    assert_int_equal((int)slist_size(l), 2);
    return_entry_slist(l);
}

static void test_sl_overflow_allowed_accepts_extra_nodes(void** state) {
    (void)state;
    entry_slist_t* l = _make_overflow_list(2);   /* overflow enabled */
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    entry_t c = _make_entry(3, 3.0f, "c");
    assert_int_equal(push_back_entry(l, &a), NO_ERROR);
    assert_int_equal(push_back_entry(l, &b), NO_ERROR);
    assert_int_equal(push_back_entry(l, &c), NO_ERROR);   /* overflow node */
    assert_int_equal((int)slist_size(l), 3);
    assert_true(slist_in_overflow(l));
    assert_int_equal((int)slist_overflow_count(l), 1);
    entry_t out = {0};
    get_entry(l, 2, &out);
    assert_int_equal(out.id, 3);
    return_entry_slist(l);
}

static void test_sl_slab_remaining_decrements_on_push(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    assert_int_equal((int)slist_slab_remaining(l), 4);
    entry_t e = _make_entry(1, 1.0f, "a");
    push_back_entry(l, &e);
    assert_int_equal((int)slist_slab_remaining(l), 3);
    return_entry_slist(l);
}

static void test_sl_is_slab_full_when_cap_reached(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(2);
    assert_false(is_slist_slab_full(l));
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    assert_true(is_slist_slab_full(l));
    return_entry_slist(l);
}

// ================================================================================
// Group 7: clear_slist
// ================================================================================

static void test_sl_clear_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_slist(NULL), NULL_POINTER);
}

static void test_sl_clear_resets_len_and_slab(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    assert_int_equal(clear_entry_slist(l), NO_ERROR);
    assert_int_equal((int)slist_size(l), 0);
    assert_int_equal((int)slist_slab_used(l), 0);
    assert_true(is_slist_empty(l));
    return_entry_slist(l);
}

static void test_sl_clear_list_is_reusable(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(1, 1.0f, "a");
    push_back_entry(l, &a);
    assert_int_equal(clear_entry_slist(l), NO_ERROR);
    entry_t b = _make_entry(99, 9.9f, "new");
    assert_int_equal(push_back_entry(l, &b), NO_ERROR);
    assert_int_equal((int)slist_size(l), 1);
    entry_t out = {0};
    get_entry(l, 0, &out);
    assert_int_equal(out.id, 99);
    return_entry_slist(l);
}

static void test_sl_clear_overflow_nodes_are_freed(void** state) {
    (void)state;
    entry_slist_t* l = _make_overflow_list(2);
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    entry_t c = _make_entry(3, 3.0f, "c");   /* overflow */
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    push_back_entry(l, &c);
    assert_true(slist_in_overflow(l));
    assert_int_equal(clear_entry_slist(l), NO_ERROR);
    assert_false(slist_in_overflow(l));
    assert_int_equal((int)slist_size(l), 0);
    return_entry_slist(l);
}

// ================================================================================
// Group 8: copy_slist
// ================================================================================

static void test_sl_copy_null_src_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    slist_expect_t r = copy_entry_slist(NULL, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_sl_copy_produces_equal_contents(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    entry_slist_t* src = _make_list(4);
    entry_t x = _make_entry(1, 1.1f, "x");
    entry_t y = _make_entry(2, 2.2f, "y");
    push_back_entry(src, &x);
    push_back_entry(src, &y);
    slist_expect_t r = copy_entry_slist(src, a);
    assert_true(r.has_value);
    entry_slist_t* dst = r.u.value;
    assert_int_equal((int)slist_size(dst), 2);
    entry_t out = {0};
    get_entry(dst, 0, &out); assert_int_equal(out.id, 1);
    get_entry(dst, 1, &out); assert_int_equal(out.id, 2);
    return_entry_slist(src);
    return_entry_slist(dst);
}

static void test_sl_copy_is_independent(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    entry_slist_t* src = _make_list(4);
    entry_t x = _make_entry(1, 1.0f, "x");
    push_back_entry(src, &x);
    slist_expect_t r = copy_entry_slist(src, a);
    assert_true(r.has_value);
    entry_slist_t* dst = r.u.value;
    /* Mutate src — dst must not be affected. */
    entry_t y = _make_entry(99, 9.9f, "mutated");
    push_back_entry(src, &y);
    assert_int_equal((int)slist_size(dst), 1);
    return_entry_slist(src);
    return_entry_slist(dst);
}

// ================================================================================
// Group 9: contains_slist
// ================================================================================

static void test_sl_contains_null_list_returns_null_pointer(void** state) {
    (void)state;
    entry_t e = _make_entry(1, 1.0f, "a");
    slist_index_expect_t r = contains_slist(NULL, &e);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_sl_contains_null_needle_returns_null_pointer(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    slist_index_expect_t r = contains_slist(l, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
    return_entry_slist(l);
}

static void test_sl_contains_absent_returns_not_found(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    push_back_entry(l, &a);
    slist_index_expect_t r = contains_entry(l, &b);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NOT_FOUND);
    return_entry_slist(l);
}

static void test_sl_contains_returns_correct_index(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    entry_t c = _make_entry(3, 3.0f, "c");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    push_back_entry(l, &c);
    slist_index_expect_t r = contains_entry(l, &b);
    assert_true(r.has_value);
    assert_int_equal((int)r.u.value, 1);
    return_entry_slist(l);
}

static void test_sl_contains_finds_first_match(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(8);
    entry_t a = _make_entry(5, 5.0f, "dup");
    entry_t b = _make_entry(5, 5.0f, "dup");   /* identical by memcmp */
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    slist_index_expect_t r = contains_entry(l, &a);
    assert_true(r.has_value);
    assert_int_equal((int)r.u.value, 0);   /* must return first occurrence */
    return_entry_slist(l);
}

// ================================================================================
// Group 10: reverse_slist
// ================================================================================

static void test_sl_reverse_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(reverse_slist(NULL), NULL_POINTER);
}

static void test_sl_reverse_empty_is_safe(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    assert_int_equal(reverse_entry_slist(l), NO_ERROR);
    assert_true(is_slist_empty(l));
    return_entry_slist(l);
}

static void test_sl_reverse_single_node_unchanged(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t e = _make_entry(1, 1.0f, "only");
    push_back_entry(l, &e);
    assert_int_equal(reverse_entry_slist(l), NO_ERROR);
    entry_t out = {0};
    get_entry(l, 0, &out);
    assert_int_equal(out.id, 1);
    return_entry_slist(l);
}

static void test_sl_reverse_three_nodes(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    entry_t c = _make_entry(3, 3.0f, "c");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    push_back_entry(l, &c);
    assert_int_equal(reverse_entry_slist(l), NO_ERROR);
    entry_t out = {0};
    get_entry(l, 0, &out); assert_int_equal(out.id, 3);
    get_entry(l, 1, &out); assert_int_equal(out.id, 2);
    get_entry(l, 2, &out); assert_int_equal(out.id, 1);
    return_entry_slist(l);
}

static void test_sl_reverse_updates_head_and_tail(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(1, 1.0f, "head");
    entry_t b = _make_entry(2, 2.0f, "tail");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    reverse_entry_slist(l);
    entry_t out = {0};
    peek_front_entry(l, &out); assert_int_equal(out.id, 2);
    peek_back_entry(l, &out);  assert_int_equal(out.id, 1);
    return_entry_slist(l);
}

// ================================================================================
// Group 11: concat_slist
// ================================================================================

static void test_sl_concat_null_dst_returns_null_pointer(void** state) {
    (void)state;
    entry_slist_t* src = _make_list(4);
    assert_int_equal(concat_entry_slist(NULL, src), NULL_POINTER);
    return_entry_slist(src);
}

static void test_sl_concat_null_src_returns_null_pointer(void** state) {
    (void)state;
    entry_slist_t* dst = _make_list(4);
    assert_int_equal(concat_entry_slist(dst, NULL), NULL_POINTER);
    return_entry_slist(dst);
}

static void test_sl_concat_data_size_mismatch_returns_invalid_arg(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    slist_expect_t r1 = init_slist(4, sizeof(entry_t),      ENTRY_TYPE, false, a);
    slist_expect_t r2 = init_slist(4, sizeof(entry_t) + 1u, ENTRY_TYPE, false, a);
    assert_true(r1.has_value && r2.has_value);
    assert_int_equal(concat_slist(r1.u.value, r2.u.value), INVALID_ARG);
    return_slist(r1.u.value);
    return_slist(r2.u.value);
}

static void test_sl_concat_appends_all_nodes(void** state) {
    (void)state;
    entry_slist_t* dst = _make_overflow_list(4);
    entry_slist_t* src = _make_list(4);
    entry_t a1 = _make_entry(1, 1.0f, "a");
    entry_t a2 = _make_entry(2, 2.0f, "b");
    entry_t b1 = _make_entry(3, 3.0f, "c");
    entry_t b2 = _make_entry(4, 4.0f, "d");
    push_back_entry(dst, &a1);
    push_back_entry(dst, &a2);
    push_back_entry(src, &b1);
    push_back_entry(src, &b2);
    assert_int_equal(concat_entry_slist(dst, src), NO_ERROR);
    assert_int_equal((int)slist_size(dst), 4);
    entry_t out = {0};
    get_entry(dst, 2, &out); assert_int_equal(out.id, 3);
    get_entry(dst, 3, &out); assert_int_equal(out.id, 4);
    return_entry_slist(dst);
    return_entry_slist(src);
}

static void test_sl_concat_src_is_unchanged(void** state) {
    (void)state;
    entry_slist_t* dst = _make_overflow_list(4);
    entry_slist_t* src = _make_list(4);
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    push_back_entry(src, &a);
    push_back_entry(src, &b);
    concat_entry_slist(dst, src);
    assert_int_equal((int)slist_size(src), 2);   /* src must be unchanged */
    return_entry_slist(dst);
    return_entry_slist(src);
}

// ================================================================================
// Group 12: foreach_slist
// ================================================================================

static void test_sl_foreach_null_list_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(foreach_slist(NULL, _sl_sum_iter, NULL), NULL_POINTER);
}

static void test_sl_foreach_null_fn_returns_null_pointer(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    assert_int_equal(foreach_slist(l, NULL, NULL), NULL_POINTER);
    return_entry_slist(l);
}

static void test_sl_foreach_visits_all_nodes(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(10, 1.0f, "a");
    entry_t b = _make_entry(20, 2.0f, "b");
    entry_t c = _make_entry(30, 3.0f, "c");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    push_back_entry(l, &c);
    _sl_iter_ctx_t ctx = { 0, 0 };
    assert_int_equal(foreach_entry_slist(l, _sl_sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 3);
    assert_int_equal(ctx.id_sum, 60);
    return_entry_slist(l);
}

static void test_sl_foreach_empty_list_calls_fn_zero_times(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    _sl_iter_ctx_t ctx = { 0, 0 };
    assert_int_equal(foreach_entry_slist(l, _sl_sum_iter, &ctx), NO_ERROR);
    assert_int_equal(ctx.count, 0);
    return_entry_slist(l);
}

static void test_sl_foreach_delivers_correct_index(void** state) {
    (void)state;
    /* Record the index at which id == 2 is visited. */
    typedef struct { int target_id; size_t found_index; bool found; } find_ctx_t;
    find_ctx_t ctx = { .target_id = 2, .found = false };

    void find_by_id(const void* v, size_t idx, void* ud) {
        const entry_t* e = (const entry_t*)v;
        find_ctx_t*    c = (find_ctx_t*)ud;
        if (e->id == c->target_id) { c->found_index = idx; c->found = true; }
    }

    entry_slist_t* l = _make_list(4);
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    entry_t c = _make_entry(3, 3.0f, "c");
    push_back_entry(l, &a);
    push_back_entry(l, &b);
    push_back_entry(l, &c);
    foreach_entry_slist(l, find_by_id, &ctx);
    assert_true(ctx.found);
    assert_int_equal((int)ctx.found_index, 1);
    return_entry_slist(l);
}

// ================================================================================
// Group 13: introspection
// ================================================================================

static void test_sl_len_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)slist_size(NULL), 0);
}

static void test_sl_slab_cap_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)slist_slab_alloc(NULL), 0);
}

static void test_sl_slab_used_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)slist_slab_used(NULL), 0);
}

static void test_sl_slab_remaining_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)slist_slab_remaining(NULL), 0);
}

static void test_sl_overflow_count_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)slist_overflow_count(NULL), 0);
}

static void test_sl_data_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)slist_data_size(NULL), 0);
}

static void test_sl_is_empty_null_returns_true(void** state) {
    (void)state;
    assert_true(is_slist_empty(NULL));
}

static void test_sl_is_slab_empty_reflects_state(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(4);
    assert_true(is_slist_slab_empty(l));
    entry_t e = _make_entry(1, 1.0f, "a");
    push_back_entry(l, &e);
    assert_false(is_slist_slab_empty(l));
    return_entry_slist(l);
}

static void test_sl_is_slab_full_reflects_state(void** state) {
    (void)state;
    entry_slist_t* l = _make_list(1);
    assert_false(is_slist_slab_full(l));
    entry_t e = _make_entry(1, 1.0f, "a");
    push_back_entry(l, &e);
    assert_true(is_slist_slab_full(l));
    return_entry_slist(l);
}

static void test_sl_in_overflow_reflects_state(void** state) {
    (void)state;
    entry_slist_t* l = _make_overflow_list(1);
    assert_false(slist_in_overflow(l));
    entry_t a = _make_entry(1, 1.0f, "a");
    entry_t b = _make_entry(2, 2.0f, "b");
    push_back_entry(l, &a);
    push_back_entry(l, &b);   /* overflow */
    assert_true(slist_in_overflow(l));
    assert_int_equal((int)slist_overflow_count(l), 1);
    return_entry_slist(l);
}

// ================================================================================
// Group 14: stress
// ================================================================================

static void test_sl_stress_push_pop_round_trip(void** state) {
    (void)state;
    entry_slist_t* l = _make_overflow_list(16);
    const int N = 64;
    for (int i = 0; i < N; i++) {
        char label[16];
        snprintf(label, sizeof(label), "e%d", i);
        entry_t e = _make_entry(i, (float)i, label);
        assert_int_equal(push_back_entry(l, &e), NO_ERROR);
    }
    assert_int_equal((int)slist_size(l), N);
    for (int i = 0; i < N; i++) {
        entry_t out = {0};
        assert_int_equal(pop_front_entry(l, &out), NO_ERROR);
        assert_int_equal(out.id, i);
    }
    assert_true(is_slist_empty(l));
    return_entry_slist(l);
}

static void test_sl_stress_foreach_sum_matches_expected(void** state) {
    (void)state;
    entry_slist_t* l = _make_overflow_list(8);
    const int N           = 32;
    int       expected_sum = 0;
    for (int i = 1; i <= N; i++) {
        entry_t e = _make_entry(i, (float)i, "x");
        push_back_entry(l, &e);
        expected_sum += i;
    }
    _sl_iter_ctx_t ctx = { 0, 0 };
    foreach_entry_slist(l, _sl_sum_iter, &ctx);
    assert_int_equal(ctx.count, N);
    assert_int_equal(ctx.id_sum, expected_sum);
    return_entry_slist(l);
}

// ================================================================================
// ================================================================================

    const struct CMUnitTest test_slist[] = {

        /* Group 1: init / return */
        cmocka_unit_test(test_sl_init_null_allocator_fails),
        cmocka_unit_test(test_sl_init_zero_capacity_fails),
        cmocka_unit_test(test_sl_init_zero_data_size_fails),
        cmocka_unit_test(test_sl_init_returns_empty_list),
        cmocka_unit_test(test_sl_init_data_size_is_sizeof_entry),
        cmocka_unit_test(test_sl_return_null_is_safe),

        /* Group 2: push_back / push_front */
        cmocka_unit_test(test_sl_push_back_null_list_returns_null_pointer),
        cmocka_unit_test(test_sl_push_back_null_value_returns_null_pointer),
        cmocka_unit_test(test_sl_push_back_increments_len),
        cmocka_unit_test(test_sl_push_back_order_is_preserved),
        cmocka_unit_test(test_sl_push_front_places_at_head),
        cmocka_unit_test(test_sl_push_back_stores_struct_correctly),

        /* Group 3: push_at */
        cmocka_unit_test(test_sl_push_at_null_list_returns_null_pointer),
        cmocka_unit_test(test_sl_push_at_out_of_range_returns_invalid_arg),
        cmocka_unit_test(test_sl_push_at_zero_delegates_to_push_front),
        cmocka_unit_test(test_sl_push_at_len_delegates_to_push_back),
        cmocka_unit_test(test_sl_push_at_mid_inserts_correctly),

        /* Group 4: pop_front / pop_back / pop_at */
        cmocka_unit_test(test_sl_pop_front_null_returns_null_pointer),
        cmocka_unit_test(test_sl_pop_front_empty_returns_not_found),
        cmocka_unit_test(test_sl_pop_front_returns_correct_value),
        cmocka_unit_test(test_sl_pop_front_null_out_discards_safely),
        cmocka_unit_test(test_sl_pop_back_returns_correct_value),
        cmocka_unit_test(test_sl_pop_back_empty_returns_not_found),
        cmocka_unit_test(test_sl_pop_back_single_node_leaves_empty),
        cmocka_unit_test(test_sl_pop_at_out_of_range_returns_invalid_arg),
        cmocka_unit_test(test_sl_pop_at_mid_removes_correct_node),

        /* Group 5: get / peek */
        cmocka_unit_test(test_sl_get_null_list_returns_null_pointer),
        cmocka_unit_test(test_sl_get_null_out_returns_null_pointer),
        cmocka_unit_test(test_sl_get_out_of_range_returns_not_found),
        cmocka_unit_test(test_sl_get_retrieves_correct_struct),
        cmocka_unit_test(test_sl_peek_front_null_returns_null_pointer),
        cmocka_unit_test(test_sl_peek_front_empty_returns_not_found),
        cmocka_unit_test(test_sl_peek_front_does_not_remove_node),
        cmocka_unit_test(test_sl_peek_back_does_not_remove_node),

        /* Group 6: slab / overflow */
        cmocka_unit_test(test_sl_slab_full_deny_returns_capacity_overflow),
        cmocka_unit_test(test_sl_overflow_allowed_accepts_extra_nodes),
        cmocka_unit_test(test_sl_slab_remaining_decrements_on_push),
        cmocka_unit_test(test_sl_is_slab_full_when_cap_reached),

        /* Group 7: clear */
        cmocka_unit_test(test_sl_clear_null_returns_null_pointer),
        cmocka_unit_test(test_sl_clear_resets_len_and_slab),
        cmocka_unit_test(test_sl_clear_list_is_reusable),
        cmocka_unit_test(test_sl_clear_overflow_nodes_are_freed),

        /* Group 8: copy */
        cmocka_unit_test(test_sl_copy_null_src_fails),
        cmocka_unit_test(test_sl_copy_produces_equal_contents),
        cmocka_unit_test(test_sl_copy_is_independent),

        /* Group 9: contains */
        cmocka_unit_test(test_sl_contains_null_list_returns_null_pointer),
        cmocka_unit_test(test_sl_contains_null_needle_returns_null_pointer),
        cmocka_unit_test(test_sl_contains_absent_returns_not_found),
        cmocka_unit_test(test_sl_contains_returns_correct_index),
        cmocka_unit_test(test_sl_contains_finds_first_match),

        /* Group 10: reverse */
        cmocka_unit_test(test_sl_reverse_null_returns_null_pointer),
        cmocka_unit_test(test_sl_reverse_empty_is_safe),
        cmocka_unit_test(test_sl_reverse_single_node_unchanged),
        cmocka_unit_test(test_sl_reverse_three_nodes),
        cmocka_unit_test(test_sl_reverse_updates_head_and_tail),

        /* Group 11: concat */
        cmocka_unit_test(test_sl_concat_null_dst_returns_null_pointer),
        cmocka_unit_test(test_sl_concat_null_src_returns_null_pointer),
        cmocka_unit_test(test_sl_concat_data_size_mismatch_returns_invalid_arg),
        cmocka_unit_test(test_sl_concat_appends_all_nodes),
        cmocka_unit_test(test_sl_concat_src_is_unchanged),

        /* Group 12: foreach */
        cmocka_unit_test(test_sl_foreach_null_list_returns_null_pointer),
        cmocka_unit_test(test_sl_foreach_null_fn_returns_null_pointer),
        cmocka_unit_test(test_sl_foreach_visits_all_nodes),
        cmocka_unit_test(test_sl_foreach_empty_list_calls_fn_zero_times),
        cmocka_unit_test(test_sl_foreach_delivers_correct_index),

        /* Group 13: introspection */
        cmocka_unit_test(test_sl_len_null_returns_zero),
        cmocka_unit_test(test_sl_slab_cap_null_returns_zero),
        cmocka_unit_test(test_sl_slab_used_null_returns_zero),
        cmocka_unit_test(test_sl_slab_remaining_null_returns_zero),
        cmocka_unit_test(test_sl_overflow_count_null_returns_zero),
        cmocka_unit_test(test_sl_data_size_null_returns_zero),
        cmocka_unit_test(test_sl_is_empty_null_returns_true),
        cmocka_unit_test(test_sl_is_slab_empty_reflects_state),
        cmocka_unit_test(test_sl_is_slab_full_reflects_state),
        cmocka_unit_test(test_sl_in_overflow_reflects_state),

        /* Group 14: stress */
        cmocka_unit_test(test_sl_stress_push_pop_round_trip),
        cmocka_unit_test(test_sl_stress_foreach_sum_matches_expected),

    };
const size_t test_slist_count = sizeof(test_slist) / sizeof(test_slist[0]);
// ================================================================================
// ================================================================================
// eof
