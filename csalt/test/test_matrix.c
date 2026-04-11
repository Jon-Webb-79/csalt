// ================================================================================
// ================================================================================
// - File:    test_matrix.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    April 03, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "c_matrix.h"
#include "c_allocator.h"
#include "c_dtypes.h"
#include "c_error.h"
#include "c_float.h"
#include "c_double.h"
#include "c_ldouble.h"
#include "c_uint8.h"
#include "c_int8.h"
#include "c_uint16.h"
#include "c_int16.h"
#include "c_uint32.h"
#include "c_int32.h"
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Helpers for set/get tests
// ================================================================================

static matrix_t* _make_dense_int32_matrix(size_t rows, size_t cols) {
    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = init_dense_matrix(rows, cols, INT32_TYPE, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

// --------------------------------------------------------------------------------

static matrix_t* _make_coo_int32_matrix(size_t rows,
                                        size_t cols,
                                        size_t cap,
                                        bool   growth) {
    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = init_coo_matrix(rows, cols, cap, INT32_TYPE, growth, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

// --------------------------------------------------------------------------------

typedef struct {
    int32_t id;
    double  value;
    uint8_t flags;
} test_record_t;

// Use a test-local custom dtype id that should not collide with builtins.
#define TEST_RECORD_TYPE (USER_BASE_TYPE + 10u)
// --------------------------------------------------------------------------------

static void _ensure_test_record_dtype_registered(void) {
    const dtype_t* found = lookup_dtype(TEST_RECORD_TYPE);
    if (found != NULL) {
        return;
    }

    dtype_t desc = {
        .id        = TEST_RECORD_TYPE,
        .data_size = sizeof(test_record_t),
        .name      = "test_record_t"
    };

    assert_true(ensure_dtype_registered(&desc));
}

// --------------------------------------------------------------------------------

static matrix_t* _make_dense_record_matrix(size_t rows, size_t cols) {
    allocator_vtable_t a = heap_allocator();

    _ensure_test_record_dtype_registered();

    matrix_expect_t r = init_dense_matrix(rows, cols, TEST_RECORD_TYPE, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

// --------------------------------------------------------------------------------

static matrix_t* _make_coo_record_matrix(size_t rows,
                                         size_t cols,
                                         size_t cap,
                                         bool   growth) {
    allocator_vtable_t a = heap_allocator();

    _ensure_test_record_dtype_registered();

    matrix_expect_t r = init_coo_matrix(rows, cols, cap, TEST_RECORD_TYPE, growth, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

// ================================================================================
// Helpers for conversion tests
// ================================================================================

static matrix_t* _make_sample_dense_int32_matrix(void) {
    matrix_t* mat = _make_dense_int32_matrix(3u, 4u);

    int32_t a = 10;
    int32_t b = 20;
    int32_t c = 30;
    int32_t d = 40;

    assert_int_equal(set_matrix(mat, 0u, 1u, &a), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 3u, &b), NO_ERROR);
    assert_int_equal(set_matrix(mat, 2u, 0u, &c), NO_ERROR);
    assert_int_equal(set_matrix(mat, 2u, 2u, &d), NO_ERROR);

    return mat;
}

// --------------------------------------------------------------------------------

static matrix_t* _make_sample_coo_int32_matrix(void) {
    matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, true);

    int32_t a = 10;
    int32_t b = 20;
    int32_t c = 30;
    int32_t d = 40;

    assert_int_equal(set_matrix(mat, 0u, 1u, &a), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 3u, &b), NO_ERROR);
    assert_int_equal(set_matrix(mat, 2u, 0u, &c), NO_ERROR);
    assert_int_equal(set_matrix(mat, 2u, 2u, &d), NO_ERROR);

    return mat;
}

// --------------------------------------------------------------------------------

static bool _zero_or_neg_one_int32(const void* value) {
    const int32_t* x = (const int32_t*)value;
    return (*x == 0 || *x == -1);
}

// --------------------------------------------------------------------------------

static bool _record_zero_by_id(const void* value) {
    const test_record_t* rec = (const test_record_t*)value;
    return rec->id == 0;
}

// ================================================================================
// Helpers for equality / transpose tests
// ================================================================================

static bool _int32_abs_equal(const void* a, const void* b) {
    const int32_t* x = (const int32_t*)a;
    const int32_t* y = (const int32_t*)b;

    int32_t ax = (*x < 0) ? -(*x) : *x;
    int32_t ay = (*y < 0) ? -(*y) : *y;

    return ax == ay;
}

// --------------------------------------------------------------------------------

static bool _record_equal_by_id(const void* a, const void* b) {
    const test_record_t* x = (const test_record_t*)a;
    const test_record_t* y = (const test_record_t*)b;
    return x->id == y->id;
}
// ================================================================================
// Group 1: init_dense_matrix
// ================================================================================

static void test_dense_init_null_allocator_fails(void** state) {
    (void)state;

    allocator_vtable_t bad = { 0 };
    matrix_expect_t r = init_dense_matrix(3u, 4u, INT32_TYPE, bad);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_dense_init_zero_rows_fails(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = init_dense_matrix(0u, 4u, INT32_TYPE, a);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_dense_init_zero_cols_fails(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = init_dense_matrix(3u, 0u, INT32_TYPE, a);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_dense_init_unknown_dtype_fails(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = init_dense_matrix(3u, 4u, UNKNOWN_TYPE, a);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_dense_init_returns_valid_matrix(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = init_dense_matrix(3u, 4u, INT32_TYPE, a);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    return_matrix(r.u.value);
}

// --------------------------------------------------------------------------------

static void test_dense_init_stores_rows(void** state) {
    (void)state;

    matrix_t* mat = _make_dense_int32_matrix(3u, 4u);
    assert_int_equal((int)matrix_rows(mat), 3);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_init_stores_cols(void** state) {
    (void)state;

    matrix_t* mat = _make_dense_int32_matrix(3u, 4u);
    assert_int_equal((int)matrix_cols(mat), 4);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_init_stores_dtype(void** state) {
    (void)state;

    matrix_t* mat = _make_dense_int32_matrix(3u, 4u);
    assert_int_equal((int)matrix_dtype(mat), (int)INT32_TYPE);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_init_stores_data_size(void** state) {
    (void)state;

    matrix_t* mat = _make_dense_int32_matrix(3u, 4u);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(int32_t));
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_init_stores_format(void** state) {
    (void)state;

    matrix_t* mat = _make_dense_int32_matrix(3u, 4u);
    assert_int_equal((int)matrix_format(mat), (int)DENSE_MATRIX);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_init_nnz_equals_rows_times_cols(void** state) {
    (void)state;

    matrix_t* mat = _make_dense_int32_matrix(3u, 4u);
    assert_int_equal((int)matrix_nnz(mat), 12);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_init_is_zero_initialized(void** state) {
    (void)state;

    matrix_t* mat = _make_dense_int32_matrix(2u, 3u);

    int32_t out = -1;
    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = -1;
            assert_int_equal(get_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal(out, 0);
        }
    }

    assert_true(is_zero_matrix(mat));
    return_matrix(mat);
}

// ================================================================================
// Group 2: init_coo_matrix
// ================================================================================

static void test_coo_init_null_allocator_fails(void** state) {
    (void)state;

    allocator_vtable_t bad = { 0 };
    matrix_expect_t r = init_coo_matrix(3u, 4u, 8u, INT32_TYPE, true, bad);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_coo_init_zero_rows_fails(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = init_coo_matrix(0u, 4u, 8u, INT32_TYPE, true, a);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_coo_init_zero_cols_fails(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 0u, 8u, INT32_TYPE, true, a);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_coo_init_zero_capacity_fails(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 4u, 0u, INT32_TYPE, true, a);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_coo_init_unknown_dtype_fails(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 4u, 8u, UNKNOWN_TYPE, true, a);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_coo_init_returns_valid_matrix(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 4u, 8u, INT32_TYPE, true, a);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    return_matrix(r.u.value);
}

// --------------------------------------------------------------------------------

static void test_coo_init_stores_rows(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, true);
    assert_int_equal((int)matrix_rows(mat), 3);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_init_stores_cols(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, true);
    assert_int_equal((int)matrix_cols(mat), 4);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_init_stores_dtype(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, true);
    assert_int_equal((int)matrix_dtype(mat), (int)INT32_TYPE);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_init_stores_data_size(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, true);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(int32_t));
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_init_stores_format(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, true);
    assert_int_equal((int)matrix_format(mat), (int)COO_MATRIX);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_init_starts_with_zero_nnz(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, true);
    assert_int_equal((int)matrix_nnz(mat), 0);
    assert_true(is_zero_matrix(mat));
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_init_unset_entries_read_back_as_zero(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, true);

    int32_t out = -1;
    assert_int_equal(get_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 0);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_init_growth_flag_is_stored(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, true);
    assert_true(mat->rep.coo.growth);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_init_capacity_is_stored(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, false);
    assert_int_equal((int)mat->rep.coo.cap, 8);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_init_sorted_starts_true(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, false);
    assert_true(mat->rep.coo.sorted);
    return_matrix(mat);
}

// ================================================================================
// Group 3: null-safe introspection
// ================================================================================

static void test_matrix_rows_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)matrix_rows(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_matrix_cols_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)matrix_cols(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_matrix_data_size_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)matrix_data_size(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_matrix_dtype_null_returns_unknown_type(void** state) {
    (void)state;
    assert_int_equal((int)matrix_dtype(NULL), (int)UNKNOWN_TYPE);
}

// --------------------------------------------------------------------------------

static void test_matrix_format_null_returns_dense_default(void** state) {
    (void)state;
    assert_int_equal((int)matrix_format(NULL), (int)DENSE_MATRIX);
}

// --------------------------------------------------------------------------------

static void test_matrix_nnz_null_returns_zero(void** state) {
    (void)state;
    assert_int_equal((int)matrix_nnz(NULL), 0);
}

// --------------------------------------------------------------------------------

static void test_is_zero_matrix_null_returns_true(void** state) {
    (void)state;
    assert_true(is_zero_matrix(NULL));
}

// ================================================================================
// Group 4: return_matrix
// ================================================================================

static void test_return_matrix_null_is_safe(void** state) {
    (void)state;
    return_matrix(NULL);
}

// --------------------------------------------------------------------------------

static void test_return_dense_matrix_does_not_crash(void** state) {
    (void)state;

    matrix_t* mat = _make_dense_int32_matrix(3u, 4u);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_return_coo_matrix_does_not_crash(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, true);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_return_populated_dense_matrix_does_not_crash(void** state) {
    (void)state;

    matrix_t* mat = _make_dense_int32_matrix(2u, 2u);
    int32_t v = 42;

    assert_int_equal(set_matrix(mat, 0u, 0u, &v), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 1u, &v), NO_ERROR);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_return_populated_coo_matrix_does_not_crash(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(3u, 3u, 4u, true);
    int32_t v1 = 10;
    int32_t v2 = 20;

    assert_int_equal(push_back_coo_matrix(mat, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 2u, 2u, &v2), NO_ERROR);

    return_matrix(mat);
}
// ================================================================================ 
// ================================================================================ 
// Group 5: dense set/get with primitive types
// ================================================================================ 
// ================================================================================ 

static void test_matrix_set_dense_null_matrix_returns_null_pointer(void** state) {
    (void)state;
    int32_t v = 7;
    assert_int_equal(set_matrix(NULL, 0u, 0u, &v), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_dense_null_value_returns_null_pointer(void** state) {
    (void)state;
    matrix_t* mat = _make_dense_int32_matrix(2u, 2u);
    assert_int_equal(set_matrix(mat, 0u, 0u, NULL), NULL_POINTER);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_dense_out_of_bounds_row_returns_invalid_arg(void** state) {
    (void)state;
    matrix_t* mat = _make_dense_int32_matrix(2u, 2u);
    int32_t v = 7;
    assert_int_equal(set_matrix(mat, 2u, 0u, &v), INVALID_ARG);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_dense_out_of_bounds_col_returns_invalid_arg(void** state) {
    (void)state;
    matrix_t* mat = _make_dense_int32_matrix(2u, 2u);
    int32_t v = 7;
    assert_int_equal(set_matrix(mat, 0u, 2u, &v), INVALID_ARG);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_get_null_matrix_returns_null_pointer(void** state) {
    (void)state;
    int32_t out = 0;
    assert_int_equal(get_matrix(NULL, 0u, 0u, &out), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_matrix_get_null_out_returns_null_pointer(void** state) {
    (void)state;
    matrix_t* mat = _make_dense_int32_matrix(2u, 2u);
    assert_int_equal(get_matrix(mat, 0u, 0u, NULL), NULL_POINTER);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_get_dense_out_of_bounds_row_returns_invalid_arg(void** state) {
    (void)state;
    matrix_t* mat = _make_dense_int32_matrix(2u, 2u);
    int32_t out = 0;
    assert_int_equal(get_matrix(mat, 2u, 0u, &out), INVALID_ARG);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_get_dense_out_of_bounds_col_returns_invalid_arg(void** state) {
    (void)state;
    matrix_t* mat = _make_dense_int32_matrix(2u, 2u);
    int32_t out = 0;
    assert_int_equal(get_matrix(mat, 0u, 2u, &out), INVALID_ARG);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_get_dense_int32_single_value(void** state) {
    (void)state;
    matrix_t* mat = _make_dense_int32_matrix(3u, 3u);

    int32_t in = 42;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 1u, 2u, &in), NO_ERROR);
    assert_int_equal(get_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 42);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_get_dense_int32_multiple_values(void** state) {
    (void)state;
    matrix_t* mat = _make_dense_int32_matrix(2u, 3u);

    int32_t a = 11, b = 22, c = 33;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 0u, 0u, &a), NO_ERROR);
    assert_int_equal(set_matrix(mat, 0u, 2u, &b), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 1u, &c), NO_ERROR);

    assert_int_equal(get_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 11);

    assert_int_equal(get_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 22);

    assert_int_equal(get_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 33);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_dense_overwrites_existing_value(void** state) {
    (void)state;
    matrix_t* mat = _make_dense_int32_matrix(2u, 2u);

    int32_t v1 = 5;
    int32_t v2 = 99;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 1u, 1u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 1u, &v2), NO_ERROR);

    assert_int_equal(get_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_get_dense_unwritten_entry_returns_zero(void** state) {
    (void)state;
    matrix_t* mat = _make_dense_int32_matrix(2u, 2u);

    int32_t out = -1;
    assert_int_equal(get_matrix(mat, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 0);

    return_matrix(mat);
}
// ================================================================================ 
// ================================================================================ 
// Group 6: COO set/get with primitive types
// ================================================================================ 
// ================================================================================ 

static void test_matrix_set_coo_null_matrix_returns_null_pointer(void** state) {
    (void)state;
    int32_t v = 7;
    assert_int_equal(set_matrix(NULL, 0u, 0u, &v), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_coo_null_value_returns_null_pointer(void** state) {
    (void)state;
    matrix_t* mat = _make_coo_int32_matrix(3u, 3u, 4u, true);
    assert_int_equal(set_matrix(mat, 0u, 0u, NULL), NULL_POINTER);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_push_back_coo_null_matrix_returns_null_pointer(void** state) {
    (void)state;
    int32_t v = 7;
    assert_int_equal(push_back_coo_matrix(NULL, 0u, 0u, &v), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_matrix_push_back_coo_null_value_returns_null_pointer(void** state) {
    (void)state;
    matrix_t* mat = _make_coo_int32_matrix(3u, 3u, 4u, true);
    assert_int_equal(push_back_coo_matrix(mat, 0u, 0u, NULL), NULL_POINTER);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_coo_out_of_bounds_row_returns_invalid_arg(void** state) {
    (void)state;
    matrix_t* mat = _make_coo_int32_matrix(2u, 2u, 4u, true);
    int32_t v = 10;
    assert_int_equal(set_matrix(mat, 2u, 0u, &v), INVALID_ARG);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_coo_out_of_bounds_col_returns_invalid_arg(void** state) {
    (void)state;
    matrix_t* mat = _make_coo_int32_matrix(2u, 2u, 4u, true);
    int32_t v = 10;
    assert_int_equal(set_matrix(mat, 0u, 2u, &v), INVALID_ARG);
    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_get_coo_unwritten_entry_returns_zero(void** state) {
    (void)state;
    matrix_t* mat = _make_coo_int32_matrix(3u, 3u, 4u, true);

    int32_t out = -1;
    assert_int_equal(get_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 0);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_get_coo_int32_single_value(void** state) {
    (void)state;
    matrix_t* mat = _make_coo_int32_matrix(3u, 3u, 4u, true);

    int32_t in = 123;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 2u, 1u, &in), NO_ERROR);
    assert_int_equal(get_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 123);
    assert_int_equal((int)matrix_nnz(mat), 1);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_push_back_get_coo_int32_single_value(void** state) {
    (void)state;
    matrix_t* mat = _make_coo_int32_matrix(3u, 3u, 4u, true);

    int32_t in = 456;
    int32_t out = 0;

    assert_int_equal(push_back_coo_matrix(mat, 0u, 2u, &in), NO_ERROR);
    assert_int_equal(get_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 456);
    assert_int_equal((int)matrix_nnz(mat), 1);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_get_coo_multiple_values(void** state) {
    (void)state;
    matrix_t* mat = _make_coo_int32_matrix(4u, 4u, 4u, true);

    int32_t a = 10, b = 20, c = 30;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 0u, 1u, &a), NO_ERROR);
    assert_int_equal(set_matrix(mat, 3u, 3u, &b), NO_ERROR);
    assert_int_equal(set_matrix(mat, 2u, 0u, &c), NO_ERROR);

    assert_int_equal(get_matrix(mat, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10);

    assert_int_equal(get_matrix(mat, 3u, 3u, &out), NO_ERROR);
    assert_int_equal(out, 20);

    assert_int_equal(get_matrix(mat, 2u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 30);

    assert_int_equal((int)matrix_nnz(mat), 3);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_coo_overwrite_existing_entry_does_not_increase_nnz(void** state) {
    (void)state;
    matrix_t* mat = _make_coo_int32_matrix(3u, 3u, 4u, true);

    int32_t v1 = 5;
    int32_t v2 = 99;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 1u, 1u, &v1), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(set_matrix(mat, 1u, 1u, &v2), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(get_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_push_back_coo_capacity_overflow_when_growth_disabled(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 3u, 2u, INT32_TYPE, false, a);
    assert_true(r.has_value);

    matrix_t* mat = r.u.value;
    int32_t v1 = 1, v2 = 2, v3 = 3;

    assert_int_equal(push_back_coo_matrix(mat, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 1u, 1u, &v2), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 2u, 2u, &v3), CAPACITY_OVERFLOW);
    assert_int_equal((int)matrix_nnz(mat), 2);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_push_back_coo_growth_expands_capacity(void** state) {
    (void)state;
    matrix_t* mat = _make_coo_int32_matrix(4u, 4u, 2u, true);

    int32_t v1 = 1, v2 = 2, v3 = 3;

    assert_int_equal(push_back_coo_matrix(mat, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 1u, 1u, &v2), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 2u, 2u, &v3), NO_ERROR);

    assert_true(mat->rep.coo.cap >= 3u);
    assert_int_equal((int)matrix_nnz(mat), 3);

    return_matrix(mat);
}
// ================================================================================ 
// Group 7: set/get with custom struct dtype
// ================================================================================ 

static void test_matrix_set_get_dense_custom_struct(void** state) {
    (void)state;
    matrix_t* mat = _make_dense_record_matrix(2u, 2u);

    test_record_t in = { 17, 3.5, 0xA5u };
    test_record_t out = { 0, 0.0, 0u };

    assert_int_equal(set_matrix(mat, 1u, 0u, &in), NO_ERROR);
    assert_int_equal(get_matrix(mat, 1u, 0u, &out), NO_ERROR);

    assert_int_equal(out.id, 17);
    assert_true(out.value == 3.5);
    // assert_int_equal(out.flags, 0xA5u);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_dense_custom_struct_overwrites_existing_value(void** state) {
    (void)state;
    matrix_t* mat = _make_dense_record_matrix(2u, 2u);

    test_record_t v1 = { 1, 1.25, 0x11u };
    test_record_t v2 = { 2, 9.75, 0x22u };
    test_record_t out = { 0, 0.0, 0u };

    assert_int_equal(set_matrix(mat, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(mat, 0u, 1u, &v2), NO_ERROR);
    assert_int_equal(get_matrix(mat, 0u, 1u, &out), NO_ERROR);

    assert_int_equal(out.id, 2);
    assert_true(out.value == 9.75);
    assert_int_equal(out.flags, 0x22u);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_get_coo_custom_struct(void** state) {
    (void)state;
    matrix_t* mat = _make_coo_record_matrix(3u, 3u, 4u, true);

    test_record_t in = { 25, 8.125, 0x5Au };
    test_record_t out = { 0, 0.0, 0u };

    assert_int_equal(set_matrix(mat, 2u, 2u, &in), NO_ERROR);
    assert_int_equal(get_matrix(mat, 2u, 2u, &out), NO_ERROR);

    assert_int_equal(out.id, 25);
    assert_true(out.value == 8.125);
    assert_int_equal(out.flags, 0x5Au);
    assert_int_equal((int)matrix_nnz(mat), 1);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_set_coo_custom_struct_overwrite_does_not_increase_nnz(void** state) {
    (void)state;
    matrix_t* mat = _make_coo_record_matrix(3u, 3u, 4u, true);

    test_record_t v1 = { 10, 1.0, 0x01u };
    test_record_t v2 = { 11, 2.0, 0x02u };
    test_record_t out = { 0, 0.0, 0u };

    assert_int_equal(set_matrix(mat, 1u, 2u, &v1), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(set_matrix(mat, 1u, 2u, &v2), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(get_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal(out.id, 11);
    assert_true(out.value == 2.0);
    assert_int_equal(out.flags, 0x02u);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_get_coo_unwritten_custom_struct_returns_zero_bytes(void** state) {
    (void)state;
    matrix_t* mat = _make_coo_record_matrix(3u, 3u, 4u, true);

    test_record_t out = { -1, -1.0, 0xFFu };
    assert_int_equal(get_matrix(mat, 0u, 1u, &out), NO_ERROR);

    assert_int_equal(out.id, 0);
    assert_true(out.value == 0.0);
    assert_int_equal(out.flags, 0u);

    return_matrix(mat);
}
// ================================================================================ 
// Group 8: clear_matrix
// ================================================================================ 

static void test_clear_matrix_null_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(clear_matrix(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_clear_dense_int32_zeroes_all_entries(void** state) {
    (void)state;

    matrix_t* mat = _make_dense_int32_matrix(2u, 3u);

    int32_t a = 11, b = 22, c = 33;
    int32_t out = -1;

    assert_int_equal(set_matrix(mat, 0u, 0u, &a), NO_ERROR);
    assert_int_equal(set_matrix(mat, 0u, 2u, &b), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 1u, &c), NO_ERROR);

    assert_false(is_zero_matrix(mat));

    assert_int_equal(clear_matrix(mat), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = -1;
            assert_int_equal(get_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal(out, 0);
        }
    }

    assert_true(is_zero_matrix(mat));
    assert_int_equal((int)matrix_rows(mat), 2);
    assert_int_equal((int)matrix_cols(mat), 3);
    assert_int_equal((int)matrix_dtype(mat), (int)INT32_TYPE);
    assert_int_equal((int)matrix_format(mat), (int)DENSE_MATRIX);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_clear_dense_custom_struct_zeroes_all_bytes(void** state) {
    (void)state;

    matrix_t* mat = _make_dense_record_matrix(2u, 2u);

    test_record_t in1 = { 17, 3.5, 0xA5u };
    test_record_t in2 = { 99, 8.25, 0x5Au };
    test_record_t out = { -1, -1.0, 0xFFu };

    assert_int_equal(set_matrix(mat, 0u, 0u, &in1), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 1u, &in2), NO_ERROR);

    assert_int_equal(clear_matrix(mat), NO_ERROR);

    assert_int_equal(get_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal(out.id, 0);
    assert_true(out.value == 0.0);
    assert_int_equal(out.flags, 0u);

    out = (test_record_t){ -1, -1.0, 0xFFu };
    assert_int_equal(get_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out.id, 0);
    assert_true(out.value == 0.0);
    assert_int_equal(out.flags, 0u);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_clear_dense_twice_is_stable(void** state) {
    (void)state;

    matrix_t* mat = _make_dense_int32_matrix(2u, 2u);
    int32_t v = 7;
    int32_t out = -1;

    assert_int_equal(set_matrix(mat, 1u, 1u, &v), NO_ERROR);

    assert_int_equal(clear_matrix(mat), NO_ERROR);
    assert_int_equal(clear_matrix(mat), NO_ERROR);

    assert_int_equal(get_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 0);
    assert_true(is_zero_matrix(mat));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_clear_coo_resets_nnz_and_sorted(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(4u, 4u, 8u, true);

    int32_t a = 10, b = 20, c = 30;

    assert_int_equal(push_back_coo_matrix(mat, 3u, 3u, &a), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 0u, 1u, &b), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 2u, 0u, &c), NO_ERROR);

    /* push_back may leave the matrix unsorted depending on insertion order */
    assert_int_equal((int)matrix_nnz(mat), 3);

    assert_int_equal(clear_matrix(mat), NO_ERROR);

    assert_int_equal((int)matrix_nnz(mat), 0);
    assert_true(mat->rep.coo.sorted);
    assert_true(is_zero_matrix(mat));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_clear_coo_makes_previous_entries_read_back_as_zero(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(3u, 3u, 4u, true);

    int32_t v = 42;
    int32_t out = -1;

    assert_int_equal(set_matrix(mat, 2u, 1u, &v), NO_ERROR);
    assert_int_equal(get_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 42);

    assert_int_equal(clear_matrix(mat), NO_ERROR);

    out = -1;
    assert_int_equal(get_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 0);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_clear_coo_preserves_capacity_growth_shape_dtype(void** state) {
    (void)state;

    matrix_t* mat = _make_coo_int32_matrix(5u, 6u, 7u, true);
    int32_t v = 1;

    assert_int_equal(push_back_coo_matrix(mat, 1u, 2u, &v), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 4u, 5u, &v), NO_ERROR);

    assert_int_equal(clear_matrix(mat), NO_ERROR);

    assert_int_equal((int)matrix_rows(mat), 5);
    assert_int_equal((int)matrix_cols(mat), 6);
    assert_int_equal((int)matrix_dtype(mat), (int)INT32_TYPE);
    assert_int_equal((int)matrix_format(mat), (int)COO_MATRIX);
    assert_int_equal((int)mat->rep.coo.cap, 7);
    assert_true(mat->rep.coo.growth);

    return_matrix(mat);
}
// ================================================================================ 
// Group 9: copy_matrix
// ================================================================================ 

static void test_copy_matrix_null_src_fails(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = copy_matrix(NULL, a);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_copy_dense_null_allocator_fails(void** state) {
    (void)state;

    matrix_t* src = _make_dense_int32_matrix(2u, 2u);
    allocator_vtable_t bad = { 0 };

    matrix_expect_t r = copy_matrix(src, bad);
    assert_false(r.has_value);

    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_dense_returns_valid_matrix(void** state) {
    (void)state;

    matrix_t* src = _make_dense_int32_matrix(2u, 3u);
    allocator_vtable_t a = heap_allocator();

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);

    return_matrix(r.u.value);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_dense_preserves_shape_dtype_and_format(void** state) {
    (void)state;

    matrix_t* src = _make_dense_int32_matrix(2u, 3u);
    allocator_vtable_t a = heap_allocator();

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_rows(dst), 2);
    assert_int_equal((int)matrix_cols(dst), 3);
    assert_int_equal((int)matrix_dtype(dst), (int)INT32_TYPE);
    assert_int_equal((int)matrix_format(dst), (int)DENSE_MATRIX);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_dense_preserves_values(void** state) {
    (void)state;

    matrix_t* src = _make_dense_int32_matrix(2u, 3u);
    allocator_vtable_t a = heap_allocator();

    int32_t a1 = 10, a2 = 20, a3 = 30;
    int32_t out = 0;

    assert_int_equal(set_matrix(src, 0u, 0u, &a1), NO_ERROR);
    assert_int_equal(set_matrix(src, 0u, 2u, &a2), NO_ERROR);
    assert_int_equal(set_matrix(src, 1u, 1u, &a3), NO_ERROR);

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal(get_matrix(dst, 0u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10);

    assert_int_equal(get_matrix(dst, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 20);

    assert_int_equal(get_matrix(dst, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 30);

    assert_true(matrix_equal(src, dst));

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_dense_is_independent_of_source(void** state) {
    (void)state;

    matrix_t* src = _make_dense_int32_matrix(2u, 2u);
    allocator_vtable_t a = heap_allocator();

    int32_t v1 = 5;
    int32_t v2 = 99;
    int32_t out = 0;

    assert_int_equal(set_matrix(src, 1u, 1u, &v1), NO_ERROR);

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal(set_matrix(dst, 1u, 1u, &v2), NO_ERROR);

    assert_int_equal(get_matrix(src, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 5);

    assert_int_equal(get_matrix(dst, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_dense_source_mutation_does_not_affect_copy(void** state) {
    (void)state;

    matrix_t* src = _make_dense_int32_matrix(2u, 2u);
    allocator_vtable_t a = heap_allocator();

    int32_t v1 = 7;
    int32_t v2 = 44;
    int32_t out = 0;

    assert_int_equal(set_matrix(src, 0u, 1u, &v1), NO_ERROR);

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal(set_matrix(src, 0u, 1u, &v2), NO_ERROR);

    assert_int_equal(get_matrix(src, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 44);

    assert_int_equal(get_matrix(dst, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 7);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_dense_custom_struct_preserves_values(void** state) {
    (void)state;

    matrix_t* src = _make_dense_record_matrix(2u, 2u);
    allocator_vtable_t a = heap_allocator();

    test_record_t in = { 17, 3.5, 0xA5u };
    test_record_t out = { 0, 0.0, 0u };

    assert_int_equal(set_matrix(src, 1u, 0u, &in), NO_ERROR);

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_data_size(dst), (int)sizeof(test_record_t));
    assert_int_equal(get_matrix(dst, 1u, 0u, &out), NO_ERROR);
    assert_int_equal(out.id, 17);
    assert_true(out.value == 3.5);
    assert_int_equal(out.flags, 0xA5u);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_coo_returns_valid_matrix(void** state) {
    (void)state;

    matrix_t* src = _make_coo_int32_matrix(3u, 4u, 8u, true);
    allocator_vtable_t a = heap_allocator();

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);

    return_matrix(r.u.value);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_coo_preserves_shape_dtype_format_nnz(void** state) {
    (void)state;

    matrix_t* src = _make_coo_int32_matrix(3u, 4u, 8u, true);
    allocator_vtable_t a = heap_allocator();

    int32_t v1 = 10, v2 = 20;
    assert_int_equal(set_matrix(src, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(src, 2u, 3u, &v2), NO_ERROR);

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_rows(dst), 3);
    assert_int_equal((int)matrix_cols(dst), 4);
    assert_int_equal((int)matrix_dtype(dst), (int)INT32_TYPE);
    assert_int_equal((int)matrix_format(dst), (int)COO_MATRIX);
    assert_int_equal((int)matrix_nnz(dst), 2);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_coo_preserves_capacity_growth_and_sorted(void** state) {
    (void)state;

    matrix_t* src = _make_coo_int32_matrix(4u, 4u, 6u, true);
    allocator_vtable_t a = heap_allocator();

    int32_t v1 = 1, v2 = 2, v3 = 3;
    assert_int_equal(push_back_coo_matrix(src, 3u, 3u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(src, 0u, 1u, &v2), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(src, 2u, 0u, &v3), NO_ERROR);

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)dst->rep.coo.cap, 6);
    assert_true(dst->rep.coo.growth == src->rep.coo.growth);
    assert_true(dst->rep.coo.sorted == src->rep.coo.sorted);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_coo_preserves_logical_values(void** state) {
    (void)state;

    matrix_t* src = _make_coo_int32_matrix(4u, 4u, 6u, true);
    allocator_vtable_t a = heap_allocator();
    int32_t out = 0;

    int32_t v1 = 10, v2 = 20, v3 = 30;
    assert_int_equal(set_matrix(src, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(src, 3u, 3u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(src, 2u, 0u, &v3), NO_ERROR);

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal(get_matrix(dst, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10);

    assert_int_equal(get_matrix(dst, 3u, 3u, &out), NO_ERROR);
    assert_int_equal(out, 20);

    assert_int_equal(get_matrix(dst, 2u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 30);

    assert_true(matrix_equal(src, dst));

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_coo_is_independent_of_source(void** state) {
    (void)state;

    matrix_t* src = _make_coo_int32_matrix(3u, 3u, 4u, true);
    allocator_vtable_t a = heap_allocator();

    int32_t v1 = 5;
    int32_t v2 = 77;
    int32_t out = 0;

    assert_int_equal(set_matrix(src, 1u, 1u, &v1), NO_ERROR);

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal(set_matrix(dst, 1u, 1u, &v2), NO_ERROR);

    assert_int_equal(get_matrix(src, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 5);

    assert_int_equal(get_matrix(dst, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 77);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_coo_source_mutation_does_not_affect_copy(void** state) {
    (void)state;

    matrix_t* src = _make_coo_int32_matrix(3u, 3u, 4u, true);
    allocator_vtable_t a = heap_allocator();

    int32_t v1 = 8;
    int32_t v2 = 99;
    int32_t out = 0;

    assert_int_equal(set_matrix(src, 0u, 2u, &v1), NO_ERROR);

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal(set_matrix(src, 0u, 2u, &v2), NO_ERROR);

    assert_int_equal(get_matrix(src, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 99);

    assert_int_equal(get_matrix(dst, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 8);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_coo_custom_struct_preserves_values(void** state) {
    (void)state;

    matrix_t* src = _make_coo_record_matrix(3u, 3u, 4u, true);
    allocator_vtable_t a = heap_allocator();

    test_record_t in = { 25, 8.125, 0x5Au };
    test_record_t out = { 0, 0.0, 0u };

    assert_int_equal(set_matrix(src, 2u, 2u, &in), NO_ERROR);

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_data_size(dst), (int)sizeof(test_record_t));
    assert_int_equal(get_matrix(dst, 2u, 2u, &out), NO_ERROR);
    assert_int_equal(out.id, 25);
    assert_true(out.value == 8.125);
    assert_int_equal(out.flags, 0x5Au);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_zero_dense_matrix_remains_zero(void** state) {
    (void)state;

    matrix_t* src = _make_dense_int32_matrix(3u, 3u);
    allocator_vtable_t a = heap_allocator();

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_true(is_zero_matrix(src));
    assert_true(is_zero_matrix(dst));
    assert_true(matrix_equal(src, dst));

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_empty_coo_matrix_remains_empty(void** state) {
    (void)state;

    matrix_t* src = _make_coo_int32_matrix(3u, 3u, 5u, true);
    allocator_vtable_t a = heap_allocator();

    matrix_expect_t r = copy_matrix(src, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_nnz(dst), 0);
    assert_true(is_zero_matrix(dst));
    assert_int_equal((int)dst->rep.coo.cap, 5);
    assert_true(dst->rep.coo.growth);

    return_matrix(dst);
    return_matrix(src);
}
// ================================================================================ 
// Group 10: convert_matrix / convert_matrix_zero dispatcher behavior 
// ================================================================================ 

static void test_convert_matrix_null_src_fails(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = convert_matrix(NULL, COO_MATRIX, a);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_convert_matrix_zero_null_src_fails(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_expect_t r = convert_matrix_zero(NULL, CSR_MATRIX, a, _zero_or_neg_one_int32);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_convert_matrix_same_format_dense_behaves_like_copy(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_sample_dense_int32_matrix();

    matrix_expect_t r = convert_matrix(src, DENSE_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)DENSE_MATRIX);
    assert_true(matrix_equal(src, dst));
    assert_ptr_not_equal(src, dst);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_matrix_same_format_coo_behaves_like_copy(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_sample_coo_int32_matrix();

    matrix_expect_t r = convert_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)COO_MATRIX);
    assert_true(matrix_equal(src, dst));
    assert_ptr_not_equal(src, dst);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_matrix_invalid_target_returns_illegal_state(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_sample_dense_int32_matrix();

    matrix_expect_t r = convert_matrix(src, (matrix_format_t)99, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, ILLEGAL_STATE);

    return_matrix(src);
}
// ================================================================================ 
// Group 11: dense -> sparse conversions
// ================================================================================ 

static void test_convert_dense_to_coo_preserves_shape_dtype_and_values(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_sample_dense_int32_matrix();

    matrix_expect_t r = convert_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)COO_MATRIX);
    assert_int_equal((int)matrix_rows(dst), 3);
    assert_int_equal((int)matrix_cols(dst), 4);
    assert_int_equal((int)matrix_dtype(dst), (int)INT32_TYPE);
    assert_int_equal((int)matrix_nnz(dst), 4);
    assert_true(matrix_equal(src, dst));

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_dense_to_coo_result_is_sorted(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_sample_dense_int32_matrix();

    matrix_expect_t r = convert_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;
    assert_true(dst->rep.coo.sorted);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_dense_to_csr_preserves_shape_dtype_and_values(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_sample_dense_int32_matrix();

    matrix_expect_t r = convert_matrix(src, CSR_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)CSR_MATRIX);
    assert_int_equal((int)matrix_rows(dst), 3);
    assert_int_equal((int)matrix_cols(dst), 4);
    assert_int_equal((int)matrix_dtype(dst), (int)INT32_TYPE);
    assert_int_equal((int)matrix_nnz(dst), 4);
    assert_true(matrix_equal(src, dst));

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_dense_to_csc_preserves_shape_dtype_and_values(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_sample_dense_int32_matrix();

    matrix_expect_t r = convert_matrix(src, CSC_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)CSC_MATRIX);
    assert_int_equal((int)matrix_rows(dst), 3);
    assert_int_equal((int)matrix_cols(dst), 4);
    assert_int_equal((int)matrix_dtype(dst), (int)INT32_TYPE);
    assert_int_equal((int)matrix_nnz(dst), 4);
    assert_true(matrix_equal(src, dst));

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_zero_dense_to_csr_omits_neg_one_when_callback_used(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_dense_int32_matrix(2u, 3u);

    int32_t a1 = 5;
    int32_t a2 = -1;
    int32_t a3 = 8;

    assert_int_equal(set_matrix(src, 0u, 0u, &a1), NO_ERROR);
    assert_int_equal(set_matrix(src, 0u, 1u, &a2), NO_ERROR);
    assert_int_equal(set_matrix(src, 1u, 2u, &a3), NO_ERROR);

    matrix_expect_t r = convert_matrix_zero(src, CSR_MATRIX, a, _zero_or_neg_one_int32);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    /* -1 is treated as zero by the callback, so only 2 entries are stored. */
    assert_int_equal((int)matrix_nnz(dst), 2);

    /* Logical retrieval still returns 0 for absent sparse entries. */
    int32_t out = 0;
    assert_int_equal(get_matrix(dst, 0u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 5);

    assert_int_equal(get_matrix(dst, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 0);

    assert_int_equal(get_matrix(dst, 1u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 8);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_zero_dense_to_coo_matches_convert_matrix_when_callback_null(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_sample_dense_int32_matrix();

    matrix_expect_t r1 = convert_matrix(src, COO_MATRIX, a);
    matrix_expect_t r2 = convert_matrix_zero(src, COO_MATRIX, a, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);
    assert_true(matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_matrix(r2.u.value);
    return_matrix(r1.u.value);
    return_matrix(src);
}
// ================================================================================ 
// Group 11: sparse -> dense conversions
// ================================================================================ 

static void test_convert_coo_to_dense_preserves_values(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_sample_coo_int32_matrix();

    matrix_expect_t r = convert_matrix(src, DENSE_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)DENSE_MATRIX);
    assert_true(matrix_equal(src, dst));

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_csr_to_dense_preserves_values(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* dense = _make_sample_dense_int32_matrix();

    matrix_expect_t tmp = convert_matrix(dense, CSR_MATRIX, a);
    assert_true(tmp.has_value);

    matrix_t* src = tmp.u.value;

    matrix_expect_t r = convert_matrix(src, DENSE_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)DENSE_MATRIX);
    assert_true(matrix_equal(dense, dst));

    return_matrix(dst);
    return_matrix(src);
    return_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_convert_csc_to_dense_preserves_values(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* dense = _make_sample_dense_int32_matrix();

    matrix_expect_t tmp = convert_matrix(dense, CSC_MATRIX, a);
    assert_true(tmp.has_value);

    matrix_t* src = tmp.u.value;

    matrix_expect_t r = convert_matrix(src, DENSE_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)DENSE_MATRIX);
    assert_true(matrix_equal(dense, dst));

    return_matrix(dst);
    return_matrix(src);
    return_matrix(dense);
}
// ================================================================================ 
// Group 12: sparse -> sparse conversions
// ================================================================================ 

static void test_convert_coo_to_csr_preserves_values(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_sample_coo_int32_matrix();

    matrix_expect_t r = convert_matrix(src, CSR_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)CSR_MATRIX);
    assert_int_equal((int)matrix_nnz(dst), 4);
    assert_true(matrix_equal(src, dst));

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_coo_to_csc_preserves_values(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_sample_coo_int32_matrix();

    matrix_expect_t r = convert_matrix(src, CSC_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)CSC_MATRIX);
    assert_int_equal((int)matrix_nnz(dst), 4);
    assert_true(matrix_equal(src, dst));

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_csr_to_coo_preserves_values(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* dense = _make_sample_dense_int32_matrix();

    matrix_expect_t tmp = convert_matrix(dense, CSR_MATRIX, a);
    assert_true(tmp.has_value);
    matrix_t* src = tmp.u.value;

    matrix_expect_t r = convert_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)COO_MATRIX);
    assert_true(matrix_equal(dense, dst));

    return_matrix(dst);
    return_matrix(src);
    return_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_convert_csr_to_csc_preserves_values(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* dense = _make_sample_dense_int32_matrix();

    matrix_expect_t tmp = convert_matrix(dense, CSR_MATRIX, a);
    assert_true(tmp.has_value);
    matrix_t* src = tmp.u.value;

    matrix_expect_t r = convert_matrix(src, CSC_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)CSC_MATRIX);
    assert_true(matrix_equal(dense, dst));

    return_matrix(dst);
    return_matrix(src);
    return_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_convert_csc_to_coo_preserves_values(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* dense = _make_sample_dense_int32_matrix();

    matrix_expect_t tmp = convert_matrix(dense, CSC_MATRIX, a);
    assert_true(tmp.has_value);
    matrix_t* src = tmp.u.value;

    matrix_expect_t r = convert_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)COO_MATRIX);
    assert_true(matrix_equal(dense, dst));

    return_matrix(dst);
    return_matrix(src);
    return_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_convert_csc_to_csr_preserves_values(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* dense = _make_sample_dense_int32_matrix();

    matrix_expect_t tmp = convert_matrix(dense, CSC_MATRIX, a);
    assert_true(tmp.has_value);
    matrix_t* src = tmp.u.value;

    matrix_expect_t r = convert_matrix(src, CSR_MATRIX, a);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)CSR_MATRIX);
    assert_true(matrix_equal(dense, dst));

    return_matrix(dst);
    return_matrix(src);
    return_matrix(dense);
}
// ================================================================================ 
// Group 13: convert_matrix_zero callback ignored dense->sparse
// ================================================================================ 

static void test_convert_zero_sparse_to_dense_ignores_callback(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_sample_coo_int32_matrix();

    matrix_expect_t r1 = convert_matrix(src, DENSE_MATRIX, a);
    matrix_expect_t r2 = convert_matrix_zero(src, DENSE_MATRIX, a, _zero_or_neg_one_int32);

    assert_true(r1.has_value);
    assert_true(r2.has_value);
    assert_true(matrix_equal(r1.u.value, r2.u.value));

    return_matrix(r2.u.value);
    return_matrix(r1.u.value);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_zero_sparse_to_sparse_ignores_callback(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_sample_coo_int32_matrix();

    matrix_expect_t r1 = convert_matrix(src, CSR_MATRIX, a);
    matrix_expect_t r2 = convert_matrix_zero(src, CSR_MATRIX, a, _zero_or_neg_one_int32);

    assert_true(r1.has_value);
    assert_true(r2.has_value);
    assert_true(matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_matrix(r2.u.value);
    return_matrix(r1.u.value);
    return_matrix(src);
}
// ================================================================================ 
// Group 14: custom struct conversion with semantic zero callback
// ================================================================================ 

static void test_convert_zero_dense_record_to_coo_uses_semantic_zero(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_dense_record_matrix(2u, 3u);

    test_record_t z1 = { 0, 99.0, 0xAAu };   /* semantic zero by callback */
    test_record_t n1 = { 5,  1.5, 0x01u };
    test_record_t n2 = { 7,  2.5, 0x02u };

    assert_int_equal(set_matrix(src, 0u, 0u, &z1), NO_ERROR);
    assert_int_equal(set_matrix(src, 0u, 2u, &n1), NO_ERROR);
    assert_int_equal(set_matrix(src, 1u, 1u, &n2), NO_ERROR);

    matrix_expect_t r = convert_matrix_zero(src, COO_MATRIX, a, _record_zero_by_id);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    /* Only the id!=0 entries should be stored. */
    assert_int_equal((int)matrix_nnz(dst), 2);

    test_record_t out = { 0, 0.0, 0u };

    assert_int_equal(get_matrix(dst, 0u, 0u, &out), NO_ERROR);
    assert_int_equal(out.id, 0);

    assert_int_equal(get_matrix(dst, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out.id, 5);
    assert_true(out.value == 1.5);
    assert_int_equal(out.flags, 0x01u);

    assert_int_equal(get_matrix(dst, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out.id, 7);
    assert_true(out.value == 2.5);
    assert_int_equal(out.flags, 0x02u);

    return_matrix(dst);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_zero_dense_record_to_csr_uses_semantic_zero(void** state) {
    (void)state;

    allocator_vtable_t a = heap_allocator();
    matrix_t* src = _make_dense_record_matrix(2u, 2u);

    test_record_t z = { 0, 1.0, 0x10u };
    test_record_t n = { 9, 4.0, 0x20u };

    assert_int_equal(set_matrix(src, 0u, 1u, &z), NO_ERROR);
    assert_int_equal(set_matrix(src, 1u, 0u, &n), NO_ERROR);

    matrix_expect_t r = convert_matrix_zero(src, CSR_MATRIX, a, _record_zero_by_id);
    assert_true(r.has_value);

    matrix_t* dst = r.u.value;

    assert_int_equal((int)matrix_format(dst), (int)CSR_MATRIX);
    assert_int_equal((int)matrix_nnz(dst), 1);

    test_record_t out = { 0, 0.0, 0u };

    assert_int_equal(get_matrix(dst, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out.id, 0);

    assert_int_equal(get_matrix(dst, 1u, 0u, &out), NO_ERROR);
    assert_int_equal(out.id, 9);
    assert_true(out.value == 4.0);
    assert_int_equal(out.flags, 0x20u);

    return_matrix(dst);
    return_matrix(src);
}
// ================================================================================ 
// Group 15: matrix_equality / matrix_equal_cmp
// ================================================================================ 

static void test_matrix_equal_null_null_returns_false(void** state) {
    (void)state;
    assert_false(matrix_equal(NULL, NULL));
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_null_left_returns_false(void** state) {
    (void)state;
    matrix_t* b = _make_dense_int32_matrix(2u, 2u);
    assert_false(matrix_equal(NULL, b));
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_null_right_returns_false(void** state) {
    (void)state;
    matrix_t* a = _make_dense_int32_matrix(2u, 2u);
    assert_false(matrix_equal(a, NULL));
    return_matrix(a);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_same_dense_zero_matrices_returns_true(void** state) {
    (void)state;

    matrix_t* a = _make_dense_int32_matrix(2u, 3u);
    matrix_t* b = _make_dense_int32_matrix(2u, 3u);

    assert_true(matrix_equal(a, b));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_dense_same_values_returns_true(void** state) {
    (void)state;

    matrix_t* a = _make_sample_dense_int32_matrix();
    matrix_t* b = _make_sample_dense_int32_matrix();

    assert_true(matrix_equal(a, b));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_dense_different_values_returns_false(void** state) {
    (void)state;

    matrix_t* a = _make_sample_dense_int32_matrix();
    matrix_t* b = _make_sample_dense_int32_matrix();

    int32_t v = 999;
    assert_int_equal(set_matrix(b, 2u, 2u, &v), NO_ERROR);

    assert_false(matrix_equal(a, b));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_different_shape_returns_false(void** state) {
    (void)state;

    matrix_t* a = _make_dense_int32_matrix(2u, 3u);
    matrix_t* b = _make_dense_int32_matrix(3u, 2u);

    assert_false(matrix_equal(a, b));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_different_dtype_returns_false(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t ai = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);
    matrix_expect_t af = init_dense_matrix(2u, 2u, FLOAT_TYPE, alloc);

    assert_true(ai.has_value);
    assert_true(af.has_value);

    assert_false(matrix_equal(ai.u.value, af.u.value));

    return_matrix(ai.u.value);
    return_matrix(af.u.value);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_dense_and_coo_same_values_returns_true(void** state) {
    (void)state;

    matrix_t* dense = _make_sample_dense_int32_matrix();
    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = convert_matrix(dense, COO_MATRIX, alloc);
    assert_true(r.has_value);

    matrix_t* coo = r.u.value;

    assert_true(matrix_equal(dense, coo));
    assert_true(matrix_equal(coo, dense));

    return_matrix(coo);
    return_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_dense_and_csr_same_values_returns_true(void** state) {
    (void)state;

    matrix_t* dense = _make_sample_dense_int32_matrix();
    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = convert_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);

    matrix_t* csr = r.u.value;

    assert_true(matrix_equal(dense, csr));
    assert_true(matrix_equal(csr, dense));

    return_matrix(csr);
    return_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_dense_and_csc_same_values_returns_true(void** state) {
    (void)state;

    matrix_t* dense = _make_sample_dense_int32_matrix();
    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = convert_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);

    matrix_t* csc = r.u.value;

    assert_true(matrix_equal(dense, csc));
    assert_true(matrix_equal(csc, dense));

    return_matrix(csc);
    return_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_coo_and_csr_same_values_returns_true(void** state) {
    (void)state;

    matrix_t* dense = _make_sample_dense_int32_matrix();
    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t rc = convert_matrix(dense, COO_MATRIX, alloc);
    matrix_expect_t rr = convert_matrix(dense, CSR_MATRIX, alloc);

    assert_true(rc.has_value);
    assert_true(rr.has_value);

    assert_true(matrix_equal(rc.u.value, rr.u.value));

    return_matrix(rr.u.value);
    return_matrix(rc.u.value);
    return_matrix(dense);
}

// ================================================================================ 
// Group 16: transpose_matrix
// ================================================================================ 

static void test_transpose_matrix_null_src_fails(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = transpose_matrix(NULL, alloc);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_transpose_dense_swaps_shape(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_t* src = _make_sample_dense_int32_matrix();

    matrix_expect_t r = transpose_matrix(src, alloc);
    assert_true(r.has_value);

    matrix_t* tr = r.u.value;

    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);
    assert_int_equal((int)matrix_dtype(tr), (int)INT32_TYPE);
    assert_int_equal((int)matrix_format(tr), (int)DENSE_MATRIX);

    return_matrix(tr);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_transpose_dense_moves_values_to_transposed_coordinates(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_t* src = _make_sample_dense_int32_matrix();

    matrix_expect_t r = transpose_matrix(src, alloc);
    assert_true(r.has_value);

    matrix_t* tr = r.u.value;
    int32_t out = 0;

    assert_int_equal(get_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10);

    assert_int_equal(get_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 20);

    assert_int_equal(get_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 30);

    assert_int_equal(get_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 40);

    return_matrix(tr);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_transpose_dense_twice_returns_logically_equal_matrix(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_t* src = _make_sample_dense_int32_matrix();

    matrix_expect_t r1 = transpose_matrix(src, alloc);
    assert_true(r1.has_value);

    matrix_expect_t r2 = transpose_matrix(r1.u.value, alloc);
    assert_true(r2.has_value);

    assert_true(matrix_equal(src, r2.u.value));

    return_matrix(r2.u.value);
    return_matrix(r1.u.value);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_transpose_dense_does_not_modify_source(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_t* src = _make_sample_dense_int32_matrix();

    matrix_expect_t copy_r = copy_matrix(src, alloc);
    assert_true(copy_r.has_value);

    matrix_expect_t tr_r = transpose_matrix(src, alloc);
    assert_true(tr_r.has_value);

    assert_true(matrix_equal(src, copy_r.u.value));

    return_matrix(tr_r.u.value);
    return_matrix(copy_r.u.value);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_transpose_coo_swaps_shape_and_preserves_format(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_t* src = _make_sample_coo_int32_matrix();

    matrix_expect_t r = transpose_matrix(src, alloc);
    assert_true(r.has_value);

    matrix_t* tr = r.u.value;

    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);
    assert_int_equal((int)matrix_format(tr), (int)COO_MATRIX);
    assert_int_equal((int)matrix_nnz(tr), 4);

    return_matrix(tr);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_transpose_coo_moves_values_to_transposed_coordinates(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_t* src = _make_sample_coo_int32_matrix();

    matrix_expect_t r = transpose_matrix(src, alloc);
    assert_true(r.has_value);

    matrix_t* tr = r.u.value;
    int32_t out = 0;

    assert_int_equal(get_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10);

    assert_int_equal(get_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 20);

    assert_int_equal(get_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 30);

    assert_int_equal(get_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 40);

    return_matrix(tr);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_transpose_coo_twice_returns_logically_equal_matrix(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_t* src = _make_sample_coo_int32_matrix();

    matrix_expect_t r1 = transpose_matrix(src, alloc);
    assert_true(r1.has_value);

    matrix_expect_t r2 = transpose_matrix(r1.u.value, alloc);
    assert_true(r2.has_value);

    assert_true(matrix_equal(src, r2.u.value));

    return_matrix(r2.u.value);
    return_matrix(r1.u.value);
    return_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_transpose_csr_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_t* dense = _make_sample_dense_int32_matrix();

    matrix_expect_t src_r = convert_matrix(dense, CSR_MATRIX, alloc);
    assert_true(src_r.has_value);

    matrix_expect_t tr_r = transpose_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    matrix_t* tr = tr_r.u.value;
    int32_t out = 0;

    assert_int_equal((int)matrix_format(tr), (int)CSR_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10);

    assert_int_equal(get_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 20);

    assert_int_equal(get_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 30);

    assert_int_equal(get_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 40);

    return_matrix(tr);
    return_matrix(src_r.u.value);
    return_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_transpose_csc_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_t* dense = _make_sample_dense_int32_matrix();

    matrix_expect_t src_r = convert_matrix(dense, CSC_MATRIX, alloc);
    assert_true(src_r.has_value);

    matrix_expect_t tr_r = transpose_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    matrix_t* tr = tr_r.u.value;
    int32_t out = 0;

    assert_int_equal((int)matrix_format(tr), (int)CSC_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10);

    assert_int_equal(get_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 20);

    assert_int_equal(get_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 30);

    assert_int_equal(get_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 40);

    return_matrix(tr);
    return_matrix(src_r.u.value);
    return_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_transpose_dense_custom_struct_preserves_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_t* src = _make_dense_record_matrix(2u, 3u);

    test_record_t v1 = { 11, 1.25, 0x10u };
    test_record_t v2 = { 22, 2.50, 0x20u };
    test_record_t out = { 0, 0.0, 0u };

    assert_int_equal(set_matrix(src, 0u, 2u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(src, 1u, 0u, &v2), NO_ERROR);

    matrix_expect_t r = transpose_matrix(src, alloc);
    assert_true(r.has_value);

    matrix_t* tr = r.u.value;

    assert_int_equal((int)matrix_rows(tr), 3);
    assert_int_equal((int)matrix_cols(tr), 2);
    assert_int_equal((int)matrix_data_size(tr), (int)sizeof(test_record_t));

    assert_int_equal(get_matrix(tr, 2u, 0u, &out), NO_ERROR);
    assert_int_equal(out.id, 11);
    assert_true(out.value == 1.25);
    assert_int_equal(out.flags, 0x10u);

    assert_int_equal(get_matrix(tr, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out.id, 22);
    assert_true(out.value == 2.50);
    assert_int_equal(out.flags, 0x20u);

    return_matrix(tr);
    return_matrix(src);
}
// ================================================================================
// Test registry
// ================================================================================

const struct CMUnitTest test_matrix[] = {

    /* Group 1: init_dense_matrix */
    cmocka_unit_test(test_dense_init_null_allocator_fails),
    cmocka_unit_test(test_dense_init_zero_rows_fails),
    cmocka_unit_test(test_dense_init_zero_cols_fails),
    cmocka_unit_test(test_dense_init_unknown_dtype_fails),
    cmocka_unit_test(test_dense_init_returns_valid_matrix),
    cmocka_unit_test(test_dense_init_stores_rows),
    cmocka_unit_test(test_dense_init_stores_cols),
    cmocka_unit_test(test_dense_init_stores_dtype),
    cmocka_unit_test(test_dense_init_stores_data_size),
    cmocka_unit_test(test_dense_init_stores_format),
    cmocka_unit_test(test_dense_init_nnz_equals_rows_times_cols),
    cmocka_unit_test(test_dense_init_is_zero_initialized),

    /* Group 2: init_coo_matrix */
    cmocka_unit_test(test_coo_init_null_allocator_fails),
    cmocka_unit_test(test_coo_init_zero_rows_fails),
    cmocka_unit_test(test_coo_init_zero_cols_fails),
    cmocka_unit_test(test_coo_init_zero_capacity_fails),
    cmocka_unit_test(test_coo_init_unknown_dtype_fails),
    cmocka_unit_test(test_coo_init_returns_valid_matrix),
    cmocka_unit_test(test_coo_init_stores_rows),
    cmocka_unit_test(test_coo_init_stores_cols),
    cmocka_unit_test(test_coo_init_stores_dtype),
    cmocka_unit_test(test_coo_init_stores_data_size),
    cmocka_unit_test(test_coo_init_stores_format),
    cmocka_unit_test(test_coo_init_starts_with_zero_nnz),
    cmocka_unit_test(test_coo_init_unset_entries_read_back_as_zero),
    cmocka_unit_test(test_coo_init_growth_flag_is_stored),
    cmocka_unit_test(test_coo_init_capacity_is_stored),
    cmocka_unit_test(test_coo_init_sorted_starts_true),

    /* Group 3: null-safe introspection */
    cmocka_unit_test(test_matrix_rows_null_returns_zero),
    cmocka_unit_test(test_matrix_cols_null_returns_zero),
    cmocka_unit_test(test_matrix_data_size_null_returns_zero),
    cmocka_unit_test(test_matrix_dtype_null_returns_unknown_type),
    cmocka_unit_test(test_matrix_format_null_returns_dense_default),
    cmocka_unit_test(test_matrix_nnz_null_returns_zero),
    cmocka_unit_test(test_is_zero_matrix_null_returns_true),

    /* Group 4: return_matrix */
    cmocka_unit_test(test_return_matrix_null_is_safe),
    cmocka_unit_test(test_return_dense_matrix_does_not_crash),
    cmocka_unit_test(test_return_coo_matrix_does_not_crash),
    cmocka_unit_test(test_return_populated_dense_matrix_does_not_crash),
    cmocka_unit_test(test_return_populated_coo_matrix_does_not_crash),

    /* Group 5: Dense set/get primitive */
    cmocka_unit_test(test_matrix_set_dense_null_matrix_returns_null_pointer),
    cmocka_unit_test(test_matrix_set_dense_null_value_returns_null_pointer),
    cmocka_unit_test(test_matrix_set_dense_out_of_bounds_row_returns_invalid_arg),
    cmocka_unit_test(test_matrix_set_dense_out_of_bounds_col_returns_invalid_arg),
    cmocka_unit_test(test_matrix_get_null_matrix_returns_null_pointer),
    cmocka_unit_test(test_matrix_get_null_out_returns_null_pointer),
    cmocka_unit_test(test_matrix_get_dense_out_of_bounds_row_returns_invalid_arg),
    cmocka_unit_test(test_matrix_get_dense_out_of_bounds_col_returns_invalid_arg),
    cmocka_unit_test(test_matrix_set_get_dense_int32_single_value),
    cmocka_unit_test(test_matrix_set_get_dense_int32_multiple_values),
    cmocka_unit_test(test_matrix_set_dense_overwrites_existing_value),
    cmocka_unit_test(test_matrix_get_dense_unwritten_entry_returns_zero),

    /* Group 6: COO set/get primitive */
    cmocka_unit_test(test_matrix_set_coo_null_matrix_returns_null_pointer),
    cmocka_unit_test(test_matrix_set_coo_null_value_returns_null_pointer),
    cmocka_unit_test(test_matrix_push_back_coo_null_matrix_returns_null_pointer),
    cmocka_unit_test(test_matrix_push_back_coo_null_value_returns_null_pointer),
    cmocka_unit_test(test_matrix_set_coo_out_of_bounds_row_returns_invalid_arg),
    cmocka_unit_test(test_matrix_set_coo_out_of_bounds_col_returns_invalid_arg),
    cmocka_unit_test(test_matrix_get_coo_unwritten_entry_returns_zero),
    cmocka_unit_test(test_matrix_set_get_coo_int32_single_value),
    cmocka_unit_test(test_matrix_push_back_get_coo_int32_single_value),
    cmocka_unit_test(test_matrix_set_get_coo_multiple_values),
    cmocka_unit_test(test_matrix_set_coo_overwrite_existing_entry_does_not_increase_nnz),
    cmocka_unit_test(test_matrix_push_back_coo_capacity_overflow_when_growth_disabled),
    cmocka_unit_test(test_matrix_push_back_coo_growth_expands_capacity),

    /* Group 7: Custom struct dtype */
    cmocka_unit_test(test_matrix_set_get_dense_custom_struct),
    cmocka_unit_test(test_matrix_set_dense_custom_struct_overwrites_existing_value),
    cmocka_unit_test(test_matrix_set_get_coo_custom_struct),
    cmocka_unit_test(test_matrix_set_coo_custom_struct_overwrite_does_not_increase_nnz),
    cmocka_unit_test(test_matrix_get_coo_unwritten_custom_struct_returns_zero_bytes),

    /* Group 8: clear_matrix */
    cmocka_unit_test(test_clear_matrix_null_returns_null_pointer),
    cmocka_unit_test(test_clear_dense_int32_zeroes_all_entries),
    cmocka_unit_test(test_clear_dense_custom_struct_zeroes_all_bytes),
    cmocka_unit_test(test_clear_dense_twice_is_stable),
    cmocka_unit_test(test_clear_coo_resets_nnz_and_sorted),
    cmocka_unit_test(test_clear_coo_makes_previous_entries_read_back_as_zero),
    cmocka_unit_test(test_clear_coo_preserves_capacity_growth_shape_dtype),

    /* Group 9: copy_matrix */
    cmocka_unit_test(test_copy_matrix_null_src_fails),
    cmocka_unit_test(test_copy_dense_null_allocator_fails),
    cmocka_unit_test(test_copy_dense_returns_valid_matrix),
    cmocka_unit_test(test_copy_dense_preserves_shape_dtype_and_format),
    cmocka_unit_test(test_copy_dense_preserves_values),
    cmocka_unit_test(test_copy_dense_is_independent_of_source),
    cmocka_unit_test(test_copy_dense_source_mutation_does_not_affect_copy),
    cmocka_unit_test(test_copy_dense_custom_struct_preserves_values),
    cmocka_unit_test(test_copy_coo_returns_valid_matrix),
    cmocka_unit_test(test_copy_coo_preserves_shape_dtype_format_nnz),
    cmocka_unit_test(test_copy_coo_preserves_capacity_growth_and_sorted),
    cmocka_unit_test(test_copy_coo_preserves_logical_values),
    cmocka_unit_test(test_copy_coo_is_independent_of_source),
    cmocka_unit_test(test_copy_coo_source_mutation_does_not_affect_copy),
    cmocka_unit_test(test_copy_coo_custom_struct_preserves_values),
    cmocka_unit_test(test_copy_zero_dense_matrix_remains_zero),
    cmocka_unit_test(test_copy_empty_coo_matrix_remains_empty),

    /* Group 10: convert_matrix / convert_matrix_zero dispatcher */
    cmocka_unit_test(test_convert_matrix_null_src_fails),
    cmocka_unit_test(test_convert_matrix_zero_null_src_fails),
    cmocka_unit_test(test_convert_matrix_same_format_dense_behaves_like_copy),
    cmocka_unit_test(test_convert_matrix_same_format_coo_behaves_like_copy),
    cmocka_unit_test(test_convert_matrix_invalid_target_returns_illegal_state),

    /* Group 11: dense -> sparse */
    cmocka_unit_test(test_convert_dense_to_coo_preserves_shape_dtype_and_values),
    cmocka_unit_test(test_convert_dense_to_coo_result_is_sorted),
    cmocka_unit_test(test_convert_dense_to_csr_preserves_shape_dtype_and_values),
    cmocka_unit_test(test_convert_dense_to_csc_preserves_shape_dtype_and_values),
    cmocka_unit_test(test_convert_zero_dense_to_csr_omits_neg_one_when_callback_used),
    cmocka_unit_test(test_convert_zero_dense_to_coo_matches_convert_matrix_when_callback_null),

    /* Group 12: sparse -> dense */
    cmocka_unit_test(test_convert_coo_to_dense_preserves_values),
    cmocka_unit_test(test_convert_csr_to_dense_preserves_values),
    cmocka_unit_test(test_convert_csc_to_dense_preserves_values),

    /* Group 13: sparse -> sparse */
    cmocka_unit_test(test_convert_coo_to_csr_preserves_values),
    cmocka_unit_test(test_convert_coo_to_csc_preserves_values),
    cmocka_unit_test(test_convert_csr_to_coo_preserves_values),
    cmocka_unit_test(test_convert_csr_to_csc_preserves_values),
    cmocka_unit_test(test_convert_csc_to_coo_preserves_values),
    cmocka_unit_test(test_convert_csc_to_csr_preserves_values),

    /* Group 14: callback ignored outside dense -> sparse */
    cmocka_unit_test(test_convert_zero_sparse_to_dense_ignores_callback),
    cmocka_unit_test(test_convert_zero_sparse_to_sparse_ignores_callback),
    cmocka_unit_test(test_convert_zero_dense_record_to_coo_uses_semantic_zero),
    cmocka_unit_test(test_convert_zero_dense_record_to_csr_uses_semantic_zero),

    /* group 16: matrix_equal / matrix_equal_cmp */
    cmocka_unit_test(test_matrix_equal_null_null_returns_false),
    cmocka_unit_test(test_matrix_equal_null_left_returns_false),
    cmocka_unit_test(test_matrix_equal_null_right_returns_false),
    cmocka_unit_test(test_matrix_equal_same_dense_zero_matrices_returns_true),
    cmocka_unit_test(test_matrix_equal_dense_same_values_returns_true),
    cmocka_unit_test(test_matrix_equal_dense_different_values_returns_false),
    cmocka_unit_test(test_matrix_equal_different_shape_returns_false),
    cmocka_unit_test(test_matrix_equal_different_dtype_returns_false),
    cmocka_unit_test(test_matrix_equal_dense_and_coo_same_values_returns_true),
    cmocka_unit_test(test_matrix_equal_dense_and_csr_same_values_returns_true),
    cmocka_unit_test(test_matrix_equal_dense_and_csc_same_values_returns_true),
    cmocka_unit_test(test_matrix_equal_coo_and_csr_same_values_returns_true),

    /* Group 17: transpose_matrix */
    cmocka_unit_test(test_transpose_matrix_null_src_fails),
    cmocka_unit_test(test_transpose_dense_swaps_shape),
    cmocka_unit_test(test_transpose_dense_moves_values_to_transposed_coordinates),
    cmocka_unit_test(test_transpose_dense_twice_returns_logically_equal_matrix),
    cmocka_unit_test(test_transpose_dense_does_not_modify_source),
    cmocka_unit_test(test_transpose_coo_swaps_shape_and_preserves_format),
    cmocka_unit_test(test_transpose_coo_moves_values_to_transposed_coordinates),
    cmocka_unit_test(test_transpose_coo_twice_returns_logically_equal_matrix),
    cmocka_unit_test(test_transpose_csr_preserves_logical_values),
    cmocka_unit_test(test_transpose_csc_preserves_logical_values),
    cmocka_unit_test(test_transpose_dense_custom_struct_preserves_values),
};

const size_t test_matrix_count =
    sizeof(test_matrix) / sizeof(test_matrix[0]);
// ================================================================================ 
// ================================================================================ 

/* =============================================================================
 * Helpers
 * ========================================================================== */

static float_matrix_t* _make_dense_float_matrix(size_t rows, size_t cols) {
    allocator_vtable_t a = heap_allocator();
    float_matrix_expect_t r = init_float_dense_matrix(rows, cols, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

static float_matrix_t* _make_coo_float_matrix(size_t rows,
                                              size_t cols,
                                              size_t cap,
                                              bool   growth) {
    allocator_vtable_t a = heap_allocator();
    float_matrix_expect_t r = init_float_coo_matrix(rows, cols, cap, growth, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

static float_matrix_t* _make_sample_dense_float_matrix(void) {
    float_matrix_t* mat = _make_dense_float_matrix(3u, 4u);

    assert_int_equal(set_float_matrix(mat, 0u, 1u, 10.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(mat, 1u, 3u, 20.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(mat, 2u, 0u, 30.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(mat, 2u, 2u, 40.0f), NO_ERROR);

    return mat;
}

static float_matrix_t* _make_sample_coo_float_matrix(void) {
    float_matrix_t* mat = _make_coo_float_matrix(3u, 4u, 8u, true);

    assert_int_equal(set_float_matrix(mat, 0u, 1u, 10.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(mat, 1u, 3u, 20.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(mat, 2u, 0u, 30.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(mat, 2u, 2u, 40.0f), NO_ERROR);

    return mat;
}

static bool _float_abs_equal(float a, float b) {
    return fabsf(fabsf(a) - fabsf(b)) < 1e-6f;
}

static bool _float_zero_or_neg_one(float v) {
    return (v == 0.0f) || (v == -1.0f);
}

/* =============================================================================
 * Group 1: init_float_dense_matrix
 * ========================================================================== */

static void test_float_dense_init_zero_rows_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    float_matrix_expect_t r = init_float_dense_matrix(0u, 4u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_float_dense_init_zero_cols_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    float_matrix_expect_t r = init_float_dense_matrix(3u, 0u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_float_dense_init_returns_valid_matrix(void** state) {
    (void)state;

    float_matrix_t* mat = _make_dense_float_matrix(3u, 4u);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)FLOAT_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(float));
    assert_int_equal((int)matrix_format(mat), (int)DENSE_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 12);

    return_float_matrix(mat);
}

static void test_float_dense_init_is_zero_initialized(void** state) {
    (void)state;

    float_matrix_t* mat = _make_dense_float_matrix(2u, 3u);
    float out = -1.0f;

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = -1.0f;
            assert_int_equal(get_float_matrix(mat, i, j, &out), NO_ERROR);
            assert_true(out == 0.0f);
        }
    }

    assert_true(float_matrix_is_zero(mat));
    return_float_matrix(mat);
}

/* =============================================================================
 * Group 2: init_float_coo_matrix
 * ========================================================================== */

static void test_float_coo_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    float_matrix_expect_t r = init_float_coo_matrix(3u, 4u, 0u, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_float_coo_init_returns_valid_matrix(void** state) {
    (void)state;

    float_matrix_t* mat = _make_coo_float_matrix(3u, 4u, 8u, true);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)FLOAT_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(float));
    assert_int_equal((int)matrix_format(mat), (int)COO_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 0);
    assert_true(mat->rep.coo.growth);
    assert_true(mat->rep.coo.sorted);

    return_float_matrix(mat);
}

static void test_float_coo_unset_entry_reads_back_as_zero(void** state) {
    (void)state;

    float_matrix_t* mat = _make_coo_float_matrix(3u, 4u, 8u, true);
    float out = -1.0f;

    assert_int_equal(get_float_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_true(out == 0.0f);

    return_float_matrix(mat);
}

/* =============================================================================
 * Group 3: dense set/get
 * ========================================================================== */

static void test_float_set_dense_null_matrix_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_float_matrix(NULL, 0u, 0u, 7.0f), NULL_POINTER);
}

static void test_float_get_dense_null_out_returns_null_pointer(void** state) {
    (void)state;
    float_matrix_t* mat = _make_dense_float_matrix(2u, 2u);

    assert_int_equal(get_float_matrix(mat, 0u, 0u, NULL), NULL_POINTER);
    return_float_matrix(mat);
}

static void test_float_set_get_dense_single_value(void** state) {
    (void)state;
    float_matrix_t* mat = _make_dense_float_matrix(3u, 3u);

    float out = 0.0f;
    assert_int_equal(set_float_matrix(mat, 1u, 2u, 42.5f), NO_ERROR);
    assert_int_equal(get_float_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_true(out == 42.5f);

    return_float_matrix(mat);
}

static void test_float_set_dense_overwrites_existing_value(void** state) {
    (void)state;
    float_matrix_t* mat = _make_dense_float_matrix(2u, 2u);

    float out = 0.0f;
    assert_int_equal(set_float_matrix(mat, 1u, 1u, 5.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(mat, 1u, 1u, 99.0f), NO_ERROR);
    assert_int_equal(get_float_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_true(out == 99.0f);

    return_float_matrix(mat);
}

/* =============================================================================
 * Group 4: COO set/get
 * ========================================================================== */

static void test_float_set_get_coo_single_value(void** state) {
    (void)state;
    float_matrix_t* mat = _make_coo_float_matrix(3u, 3u, 4u, true);

    float out = 0.0f;
    assert_int_equal(set_float_matrix(mat, 2u, 1u, 123.25f), NO_ERROR);
    assert_int_equal(get_float_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_true(out == 123.25f);
    assert_int_equal((int)matrix_nnz(mat), 1);

    return_float_matrix(mat);
}

static void test_float_push_back_coo_overwrite_does_not_increase_nnz(void** state) {
    (void)state;
    float_matrix_t* mat = _make_coo_float_matrix(3u, 3u, 4u, true);

    float out = 0.0f;
    assert_int_equal(push_back_float_coo_matrix(mat, 1u, 1u, 5.0f), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(push_back_float_coo_matrix(mat, 1u, 1u, 99.0f), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(get_float_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_true(out == 99.0f);

    return_float_matrix(mat);
}

static void test_float_push_back_coo_capacity_overflow_when_growth_disabled(void** state) {
    (void)state;
    float_matrix_t* mat = _make_coo_float_matrix(3u, 3u, 2u, false);

    assert_int_equal(push_back_float_coo_matrix(mat, 0u, 0u, 1.0f), NO_ERROR);
    assert_int_equal(push_back_float_coo_matrix(mat, 1u, 1u, 2.0f), NO_ERROR);
    assert_int_equal(push_back_float_coo_matrix(mat, 2u, 2u, 3.0f), CAPACITY_OVERFLOW);

    return_float_matrix(mat);
}

/* =============================================================================
 * Group 5: clear / copy
 * ========================================================================== */

static void test_clear_float_dense_zeroes_all_entries(void** state) {
    (void)state;
    float_matrix_t* mat = _make_dense_float_matrix(2u, 3u);
    float out = -1.0f;

    assert_int_equal(set_float_matrix(mat, 0u, 0u, 11.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(mat, 0u, 2u, 22.0f), NO_ERROR);
    assert_false(float_matrix_is_zero(mat));

    assert_int_equal(clear_float_matrix(mat), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = -1.0f;
            assert_int_equal(get_float_matrix(mat, i, j, &out), NO_ERROR);
            assert_true(out == 0.0f);
        }
    }

    assert_true(float_matrix_is_zero(mat));
    return_float_matrix(mat);
}

static void test_copy_float_dense_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_matrix_t* src = _make_sample_dense_float_matrix();

    float_matrix_expect_t r = copy_float_matrix(src, a);
    assert_true(r.has_value);
    assert_true(float_matrix_equal(src, r.u.value));

    return_float_matrix(r.u.value);
    return_float_matrix(src);
}

static void test_copy_float_coo_is_independent_of_source(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_matrix_t* src = _make_coo_float_matrix(3u, 3u, 4u, true);

    assert_int_equal(set_float_matrix(src, 1u, 1u, 5.0f), NO_ERROR);

    float_matrix_expect_t r = copy_float_matrix(src, a);
    assert_true(r.has_value);

    assert_int_equal(set_float_matrix(r.u.value, 1u, 1u, 77.0f), NO_ERROR);

    float src_out = 0.0f;
    float dst_out = 0.0f;

    assert_int_equal(get_float_matrix(src, 1u, 1u, &src_out), NO_ERROR);
    assert_int_equal(get_float_matrix(r.u.value, 1u, 1u, &dst_out), NO_ERROR);

    assert_true(src_out == 5.0f);
    assert_true(dst_out == 77.0f);

    return_float_matrix(r.u.value);
    return_float_matrix(src);
}

/* =============================================================================
 * Group 6: convert / convert_zero
 * ========================================================================== */

static void test_convert_float_dense_to_coo_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_matrix_t* src = _make_sample_dense_float_matrix();

    float_matrix_expect_t r = convert_float_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)COO_MATRIX);
    assert_true(float_matrix_equal(src, r.u.value));

    return_float_matrix(r.u.value);
    return_float_matrix(src);
}

static void test_convert_float_dense_to_csr_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_matrix_t* src = _make_sample_dense_float_matrix();

    float_matrix_expect_t r = convert_float_matrix(src, CSR_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)CSR_MATRIX);
    assert_true(float_matrix_equal(src, r.u.value));

    return_float_matrix(r.u.value);
    return_float_matrix(src);
}

static void test_convert_float_zero_dense_to_csr_omits_neg_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_matrix_t* src = _make_dense_float_matrix(2u, 3u);

    assert_int_equal(set_float_matrix(src, 0u, 0u, 5.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(src, 0u, 1u, -1.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(src, 1u, 2u, 8.0f), NO_ERROR);

    float_matrix_expect_t r =
        convert_float_matrix_zero(src, CSR_MATRIX, a, _float_zero_or_neg_one);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_nnz(r.u.value), 2);

    return_float_matrix(r.u.value);
    return_float_matrix(src);
}

/* =============================================================================
 * Group 7: transpose / fill
 * ========================================================================== */

static void test_transpose_float_dense_moves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    float_matrix_t* src = _make_sample_dense_float_matrix();

    float_matrix_expect_t r = transpose_float_matrix(src, a);
    assert_true(r.has_value);

    float out = 0.0f;
    assert_int_equal(get_float_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 10.0f);

    assert_int_equal(get_float_matrix(r.u.value, 3u, 1u, &out), NO_ERROR);
    assert_true(out == 20.0f);

    return_float_matrix(r.u.value);
    return_float_matrix(src);
}

static void test_fill_float_dense_sets_every_entry(void** state) {
    (void)state;
    float_matrix_t* mat = _make_dense_float_matrix(2u, 3u);
    float out = 0.0f;

    assert_int_equal(fill_float_matrix(mat, 7.5f), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            assert_int_equal(get_float_matrix(mat, i, j, &out), NO_ERROR);
            assert_true(out == 7.5f);
        }
    }

    return_float_matrix(mat);
}

static void test_fill_float_zero_equivalent_to_clear(void** state) {
    (void)state;
    float_matrix_t* mat = _make_dense_float_matrix(2u, 2u);

    assert_int_equal(set_float_matrix(mat, 1u, 1u, 9.0f), NO_ERROR);
    assert_false(float_matrix_is_zero(mat));

    assert_int_equal(fill_float_matrix(mat, 0.0f), NO_ERROR);
    assert_true(float_matrix_is_zero(mat));

    return_float_matrix(mat);
}

/* =============================================================================
 * Group 8: equality / compatibility
 * ========================================================================== */

static void test_float_matrix_equal_dense_same_values_returns_true(void** state) {
    (void)state;

    float_matrix_t* a = _make_sample_dense_float_matrix();
    float_matrix_t* b = _make_sample_dense_float_matrix();

    assert_true(float_matrix_equal(a, b));

    return_float_matrix(a);
    return_float_matrix(b);
}

static void test_float_matrix_shape_compatibility_helpers(void** state) {
    (void)state;

    float_matrix_t* a = _make_dense_float_matrix(2u, 3u);
    float_matrix_t* b = _make_dense_float_matrix(2u, 3u);
    float_matrix_t* c = _make_dense_float_matrix(3u, 4u);

    assert_true(float_matrix_has_same_shape(a, b));
    assert_true(float_matrix_is_add_compatible(a, b));
    assert_true(float_matrix_is_multiply_compatible(a, c));
    assert_false(float_matrix_is_square(a));

    return_float_matrix(a);
    return_float_matrix(b);
    return_float_matrix(c);
}

/* =============================================================================
 * Group 9: swaps / constructors / vector queries
 * ========================================================================== */

static void test_swap_float_matrix_rows_dense(void** state) {
    (void)state;
    float_matrix_t* mat = _make_dense_float_matrix(2u, 2u);
    float out = 0.0f;

    assert_int_equal(set_float_matrix(mat, 0u, 0u, 1.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(mat, 1u, 0u, 2.0f), NO_ERROR);

    assert_int_equal(swap_float_matrix_rows(mat, 0u, 1u), NO_ERROR);

    assert_int_equal(get_float_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_true(out == 2.0f);

    assert_int_equal(get_float_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 1.0f);

    return_float_matrix(mat);
}

static void test_init_float_identity_matrix_sets_diagonal_to_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    float_matrix_expect_t r = init_float_identity_matrix(3u, a);
    assert_true(r.has_value);

    float out = 0.0f;
    assert_int_equal(get_float_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_true(out == 1.0f);

    assert_int_equal(get_float_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_true(out == 0.0f);

    return_float_matrix(r.u.value);
}

static void test_float_row_and_col_vector_queries(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    float_matrix_expect_t row = init_float_row_vector(5u, a);
    float_matrix_expect_t col = init_float_col_vector(7u, a);

    assert_true(row.has_value);
    assert_true(col.has_value);

    assert_true(float_matrix_is_row_vector(row.u.value));
    assert_true(float_matrix_is_vector(row.u.value));
    assert_int_equal((int)float_matrix_vector_length(row.u.value), 5);

    assert_true(float_matrix_is_col_vector(col.u.value));
    assert_true(float_matrix_is_vector(col.u.value));
    assert_int_equal((int)float_matrix_vector_length(col.u.value), 7);

    return_float_matrix(row.u.value);
    return_float_matrix(col.u.value);
}
// ================================================================================
// Group 10: Additional float matrix tests
// ================================================================================

static void test_transpose_float_coo_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    float_matrix_t* src = _make_sample_coo_float_matrix();

    float_matrix_expect_t r = transpose_float_matrix(src, alloc);
    assert_true(r.has_value);

    float_matrix_t* tr = r.u.value;
    float out = 0.0f;

    assert_int_equal((int)matrix_format(tr), (int)COO_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_float_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 10.0f);

    assert_int_equal(get_float_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_true(out == 20.0f);

    assert_int_equal(get_float_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_true(out == 30.0f);

    assert_int_equal(get_float_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_true(out == 40.0f);

    return_float_matrix(tr);
    return_float_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_transpose_float_csr_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    float_matrix_t* dense = _make_sample_dense_float_matrix();

    float_matrix_expect_t src_r = convert_float_matrix(dense, CSR_MATRIX, alloc);
    assert_true(src_r.has_value);

    float_matrix_expect_t tr_r = transpose_float_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    float_matrix_t* tr = tr_r.u.value;
    float out = 0.0f;

    assert_int_equal((int)matrix_format(tr), (int)CSR_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_float_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 10.0f);

    assert_int_equal(get_float_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_true(out == 20.0f);

    assert_int_equal(get_float_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_true(out == 30.0f);

    assert_int_equal(get_float_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_true(out == 40.0f);

    return_float_matrix(tr);
    return_float_matrix(src_r.u.value);
    return_float_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_transpose_float_csc_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    float_matrix_t* dense = _make_sample_dense_float_matrix();

    float_matrix_expect_t src_r = convert_float_matrix(dense, CSC_MATRIX, alloc);
    assert_true(src_r.has_value);

    float_matrix_expect_t tr_r = transpose_float_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    float_matrix_t* tr = tr_r.u.value;
    float out = 0.0f;

    assert_int_equal((int)matrix_format(tr), (int)CSC_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_float_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 10.0f);

    assert_int_equal(get_float_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_true(out == 20.0f);

    assert_int_equal(get_float_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_true(out == 30.0f);

    assert_int_equal(get_float_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_true(out == 40.0f);

    return_float_matrix(tr);
    return_float_matrix(src_r.u.value);
    return_float_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_swap_float_matrix_cols_dense(void** state) {
    (void)state;

    float_matrix_t* mat = _make_dense_float_matrix(2u, 3u);
    float out = 0.0f;

    assert_int_equal(set_float_matrix(mat, 0u, 0u, 1.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(mat, 0u, 2u, 3.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(mat, 1u, 0u, 4.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(mat, 1u, 2u, 6.0f), NO_ERROR);

    assert_int_equal(swap_float_matrix_cols(mat, 0u, 2u), NO_ERROR);

    assert_int_equal(get_float_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_true(out == 3.0f);

    assert_int_equal(get_float_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_true(out == 1.0f);

    assert_int_equal(get_float_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 6.0f);

    assert_int_equal(get_float_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_true(out == 4.0f);

    return_float_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_float_matrix_is_sparse_dense_returns_false(void** state) {
    (void)state;

    float_matrix_t* mat = _make_dense_float_matrix(3u, 3u);
    assert_false(float_matrix_is_sparse(mat));

    return_float_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_float_matrix_is_sparse_coo_returns_true(void** state) {
    (void)state;

    float_matrix_t* mat = _make_coo_float_matrix(3u, 3u, 4u, true);
    assert_true(float_matrix_is_sparse(mat));

    return_float_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_float_matrix_is_sparse_csr_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    float_matrix_t* dense = _make_sample_dense_float_matrix();

    float_matrix_expect_t r = convert_float_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(float_matrix_is_sparse(r.u.value));

    return_float_matrix(r.u.value);
    return_float_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_float_matrix_is_sparse_csc_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    float_matrix_t* dense = _make_sample_dense_float_matrix();

    float_matrix_expect_t r = convert_float_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(float_matrix_is_sparse(r.u.value));

    return_float_matrix(r.u.value);
    return_float_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_float_matrix_equal_dense_and_coo_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    float_matrix_t* dense = _make_sample_dense_float_matrix();

    float_matrix_expect_t r = convert_float_matrix(dense, COO_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(float_matrix_equal(dense, r.u.value));
    assert_true(float_matrix_equal(r.u.value, dense));

    return_float_matrix(r.u.value);
    return_float_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_float_matrix_equal_dense_and_csr_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    float_matrix_t* dense = _make_sample_dense_float_matrix();

    float_matrix_expect_t r = convert_float_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(float_matrix_equal(dense, r.u.value));
    assert_true(float_matrix_equal(r.u.value, dense));

    return_float_matrix(r.u.value);
    return_float_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_float_matrix_equal_dense_and_csc_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    float_matrix_t* dense = _make_sample_dense_float_matrix();

    float_matrix_expect_t r = convert_float_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(float_matrix_equal(dense, r.u.value));
    assert_true(float_matrix_equal(r.u.value, dense));

    return_float_matrix(r.u.value);
    return_float_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_convert_float_matrix_zero_null_callback_matches_convert_float_matrix_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    float_matrix_t* src = _make_sample_dense_float_matrix();

    float_matrix_expect_t r1 = convert_float_matrix(src, COO_MATRIX, alloc);
    float_matrix_expect_t r2 = convert_float_matrix_zero(src, COO_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(float_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_float_matrix(r2.u.value);
    return_float_matrix(r1.u.value);
    return_float_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_float_matrix_zero_null_callback_matches_convert_float_matrix_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    float_matrix_t* src = _make_sample_dense_float_matrix();

    float_matrix_expect_t r1 = convert_float_matrix(src, CSR_MATRIX, alloc);
    float_matrix_expect_t r2 = convert_float_matrix_zero(src, CSR_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(float_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_float_matrix(r2.u.value);
    return_float_matrix(r1.u.value);
    return_float_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_float_matrix_zero_null_callback_matches_convert_float_matrix_csc(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    float_matrix_t* src = _make_sample_dense_float_matrix();

    float_matrix_expect_t r1 = convert_float_matrix(src, CSC_MATRIX, alloc);
    float_matrix_expect_t r2 = convert_float_matrix_zero(src, CSC_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(float_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_float_matrix(r2.u.value);
    return_float_matrix(r1.u.value);
    return_float_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_float_matrix_zero_treats_negative_zero_as_zero_in_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    float_matrix_t* src = _make_dense_float_matrix(2u, 2u);

    float neg_zero = -0.0f;
    float one = 1.0f;

    assert_int_equal(set_float_matrix(src, 0u, 0u, neg_zero), NO_ERROR);
    assert_int_equal(set_float_matrix(src, 1u, 1u, one), NO_ERROR);

    float_matrix_expect_t r = convert_float_matrix_zero(src, COO_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    /* -0.0f should be treated as semantic zero and omitted */
    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    float out = 123.0f;
    assert_int_equal(get_float_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_true(out == 0.0f);

    assert_int_equal(get_float_matrix(r.u.value, 1u, 1u, &out), NO_ERROR);
    assert_true(out == 1.0f);

    return_float_matrix(r.u.value);
    return_float_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_float_matrix_zero_treats_negative_zero_as_zero_in_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    float_matrix_t* src = _make_dense_float_matrix(2u, 2u);

    assert_int_equal(set_float_matrix(src, 0u, 1u, -0.0f), NO_ERROR);
    assert_int_equal(set_float_matrix(src, 1u, 0u, 2.0f), NO_ERROR);

    float_matrix_expect_t r = convert_float_matrix_zero(src, CSR_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    float out = 0.0f;
    assert_int_equal(get_float_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_true(out == 0.0f);

    assert_int_equal(get_float_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 2.0f);

    return_float_matrix(r.u.value);
    return_float_matrix(src);
}

/* =============================================================================
 * Registry
 * ========================================================================== */

const struct CMUnitTest test_float_matrix[] = {
    /* Group 1 */
    cmocka_unit_test(test_float_dense_init_zero_rows_fails),
    cmocka_unit_test(test_float_dense_init_zero_cols_fails),
    cmocka_unit_test(test_float_dense_init_returns_valid_matrix),
    cmocka_unit_test(test_float_dense_init_is_zero_initialized),

    /* Group 2 */
    cmocka_unit_test(test_float_coo_init_zero_capacity_fails),
    cmocka_unit_test(test_float_coo_init_returns_valid_matrix),
    cmocka_unit_test(test_float_coo_unset_entry_reads_back_as_zero),

    /* Group 3 */
    cmocka_unit_test(test_float_set_dense_null_matrix_returns_null_pointer),
    cmocka_unit_test(test_float_get_dense_null_out_returns_null_pointer),
    cmocka_unit_test(test_float_set_get_dense_single_value),
    cmocka_unit_test(test_float_set_dense_overwrites_existing_value),

    /* Group 4 */
    cmocka_unit_test(test_float_set_get_coo_single_value),
    cmocka_unit_test(test_float_push_back_coo_overwrite_does_not_increase_nnz),
    cmocka_unit_test(test_float_push_back_coo_capacity_overflow_when_growth_disabled),

    /* Group 5 */
    cmocka_unit_test(test_clear_float_dense_zeroes_all_entries),
    cmocka_unit_test(test_copy_float_dense_preserves_values),
    cmocka_unit_test(test_copy_float_coo_is_independent_of_source),

    /* Group 6 */
    cmocka_unit_test(test_convert_float_dense_to_coo_preserves_values),
    cmocka_unit_test(test_convert_float_dense_to_csr_preserves_values),
    cmocka_unit_test(test_convert_float_zero_dense_to_csr_omits_neg_one),

    /* Group 7 */
    cmocka_unit_test(test_transpose_float_dense_moves_values),
    cmocka_unit_test(test_fill_float_dense_sets_every_entry),
    cmocka_unit_test(test_fill_float_zero_equivalent_to_clear),

    /* Group 8 */
    cmocka_unit_test(test_float_matrix_equal_dense_same_values_returns_true),
    cmocka_unit_test(test_float_matrix_shape_compatibility_helpers),

    /* Group 9 */
    cmocka_unit_test(test_swap_float_matrix_rows_dense),
    cmocka_unit_test(test_init_float_identity_matrix_sets_diagonal_to_one),
    cmocka_unit_test(test_float_row_and_col_vector_queries),

    /* Group 10 */
    cmocka_unit_test(test_transpose_float_coo_preserves_logical_values),
    cmocka_unit_test(test_transpose_float_csr_preserves_logical_values),
    cmocka_unit_test(test_transpose_float_csc_preserves_logical_values),

    cmocka_unit_test(test_swap_float_matrix_cols_dense),

    cmocka_unit_test(test_float_matrix_is_sparse_dense_returns_false),
    cmocka_unit_test(test_float_matrix_is_sparse_coo_returns_true),
    cmocka_unit_test(test_float_matrix_is_sparse_csr_returns_true),
    cmocka_unit_test(test_float_matrix_is_sparse_csc_returns_true),

    cmocka_unit_test(test_float_matrix_equal_dense_and_coo_same_values_returns_true),
    cmocka_unit_test(test_float_matrix_equal_dense_and_csr_same_values_returns_true),
    cmocka_unit_test(test_float_matrix_equal_dense_and_csc_same_values_returns_true),

    cmocka_unit_test(test_convert_float_matrix_zero_null_callback_matches_convert_float_matrix_coo),
    cmocka_unit_test(test_convert_float_matrix_zero_null_callback_matches_convert_float_matrix_csr),
    cmocka_unit_test(test_convert_float_matrix_zero_null_callback_matches_convert_float_matrix_csc),

    cmocka_unit_test(test_convert_float_matrix_zero_treats_negative_zero_as_zero_in_coo),
    cmocka_unit_test(test_convert_float_matrix_zero_treats_negative_zero_as_zero_in_csr),
};

const size_t test_float_matrix_count =
    sizeof(test_float_matrix) / sizeof(test_float_matrix[0]);
// ================================================================================ 
// ================================================================================ 

/* =============================================================================
 * Helpers
 * ========================================================================== */

static double_matrix_t* _make_dense_double_matrix(size_t rows, size_t cols) {
    allocator_vtable_t a = heap_allocator();
    double_matrix_expect_t r = init_double_dense_matrix(rows, cols, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

static double_matrix_t* _make_coo_double_matrix(size_t rows,
                                              size_t cols,
                                              size_t cap,
                                              bool   growth) {
    allocator_vtable_t a = heap_allocator();
    double_matrix_expect_t r = init_double_coo_matrix(rows, cols, cap, growth, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

static double_matrix_t* _make_sample_dense_double_matrix(void) {
    double_matrix_t* mat = _make_dense_double_matrix(3u, 4u);

    assert_int_equal(set_double_matrix(mat, 0u, 1u, 10.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(mat, 1u, 3u, 20.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(mat, 2u, 0u, 30.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(mat, 2u, 2u, 40.0f), NO_ERROR);

    return mat;
}

static double_matrix_t* _make_sample_coo_double_matrix(void) {
    double_matrix_t* mat = _make_coo_double_matrix(3u, 4u, 8u, true);

    assert_int_equal(set_double_matrix(mat, 0u, 1u, 10.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(mat, 1u, 3u, 20.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(mat, 2u, 0u, 30.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(mat, 2u, 2u, 40.0f), NO_ERROR);

    return mat;
}

static bool _double_abs_equal(double a, double b) {
    return fabsf(fabsf(a) - fabsf(b)) < 1e-6f;
}

static bool _double_zero_or_neg_one(double v) {
    return (v == 0.0f) || (v == -1.0f);
}

/* =============================================================================
 * Group 1: init_double_dense_matrix
 * ========================================================================== */

static void test_double_dense_init_zero_rows_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    double_matrix_expect_t r = init_double_dense_matrix(0u, 4u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_double_dense_init_zero_cols_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    double_matrix_expect_t r = init_double_dense_matrix(3u, 0u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_double_dense_init_returns_valid_matrix(void** state) {
    (void)state;

    double_matrix_t* mat = _make_dense_double_matrix(3u, 4u);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)DOUBLE_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(double));
    assert_int_equal((int)matrix_format(mat), (int)DENSE_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 12);

    return_double_matrix(mat);
}

static void test_double_dense_init_is_zero_initialized(void** state) {
    (void)state;

    double_matrix_t* mat = _make_dense_double_matrix(2u, 3u);
    double out = -1.0f;

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = -1.0f;
            assert_int_equal(get_double_matrix(mat, i, j, &out), NO_ERROR);
            assert_true(out == 0.0f);
        }
    }

    assert_true(double_matrix_is_zero(mat));
    return_double_matrix(mat);
}

/* =============================================================================
 * Group 2: init_double_coo_matrix
 * ========================================================================== */

static void test_double_coo_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    double_matrix_expect_t r = init_double_coo_matrix(3u, 4u, 0u, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_double_coo_init_returns_valid_matrix(void** state) {
    (void)state;

    double_matrix_t* mat = _make_coo_double_matrix(3u, 4u, 8u, true);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)DOUBLE_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(double));
    assert_int_equal((int)matrix_format(mat), (int)COO_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 0);
    assert_true(mat->rep.coo.growth);
    assert_true(mat->rep.coo.sorted);

    return_double_matrix(mat);
}

static void test_double_coo_unset_entry_reads_back_as_zero(void** state) {
    (void)state;

    double_matrix_t* mat = _make_coo_double_matrix(3u, 4u, 8u, true);
    double out = -1.0f;

    assert_int_equal(get_double_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_true(out == 0.0f);

    return_double_matrix(mat);
}

/* =============================================================================
 * Group 3: dense set/get
 * ========================================================================== */

static void test_double_set_dense_null_matrix_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_double_matrix(NULL, 0u, 0u, 7.0f), NULL_POINTER);
}

static void test_double_get_dense_null_out_returns_null_pointer(void** state) {
    (void)state;
    double_matrix_t* mat = _make_dense_double_matrix(2u, 2u);

    assert_int_equal(get_double_matrix(mat, 0u, 0u, NULL), NULL_POINTER);
    return_double_matrix(mat);
}

static void test_double_set_get_dense_single_value(void** state) {
    (void)state;
    double_matrix_t* mat = _make_dense_double_matrix(3u, 3u);

    double out = 0.0f;
    assert_int_equal(set_double_matrix(mat, 1u, 2u, 42.5f), NO_ERROR);
    assert_int_equal(get_double_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_true(out == 42.5f);

    return_double_matrix(mat);
}

static void test_double_set_dense_overwrites_existing_value(void** state) {
    (void)state;
    double_matrix_t* mat = _make_dense_double_matrix(2u, 2u);

    double out = 0.0f;
    assert_int_equal(set_double_matrix(mat, 1u, 1u, 5.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(mat, 1u, 1u, 99.0f), NO_ERROR);
    assert_int_equal(get_double_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_true(out == 99.0f);

    return_double_matrix(mat);
}

/* =============================================================================
 * Group 4: COO set/get
 * ========================================================================== */

static void test_double_set_get_coo_single_value(void** state) {
    (void)state;
    double_matrix_t* mat = _make_coo_double_matrix(3u, 3u, 4u, true);

    double out = 0.0f;
    assert_int_equal(set_double_matrix(mat, 2u, 1u, 123.25f), NO_ERROR);
    assert_int_equal(get_double_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_true(out == 123.25f);
    assert_int_equal((int)matrix_nnz(mat), 1);

    return_double_matrix(mat);
}

static void test_double_push_back_coo_overwrite_does_not_increase_nnz(void** state) {
    (void)state;
    double_matrix_t* mat = _make_coo_double_matrix(3u, 3u, 4u, true);

    double out = 0.0f;
    assert_int_equal(push_back_double_coo_matrix(mat, 1u, 1u, 5.0f), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(push_back_double_coo_matrix(mat, 1u, 1u, 99.0f), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(get_double_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_true(out == 99.0f);

    return_double_matrix(mat);
}

static void test_double_push_back_coo_capacity_overflow_when_growth_disabled(void** state) {
    (void)state;
    double_matrix_t* mat = _make_coo_double_matrix(3u, 3u, 2u, false);

    assert_int_equal(push_back_double_coo_matrix(mat, 0u, 0u, 1.0f), NO_ERROR);
    assert_int_equal(push_back_double_coo_matrix(mat, 1u, 1u, 2.0f), NO_ERROR);
    assert_int_equal(push_back_double_coo_matrix(mat, 2u, 2u, 3.0f), CAPACITY_OVERFLOW);

    return_double_matrix(mat);
}

/* =============================================================================
 * Group 5: clear / copy
 * ========================================================================== */

static void test_clear_double_dense_zeroes_all_entries(void** state) {
    (void)state;
    double_matrix_t* mat = _make_dense_double_matrix(2u, 3u);
    double out = -1.0f;

    assert_int_equal(set_double_matrix(mat, 0u, 0u, 11.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(mat, 0u, 2u, 22.0f), NO_ERROR);
    assert_false(double_matrix_is_zero(mat));

    assert_int_equal(clear_double_matrix(mat), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = -1.0f;
            assert_int_equal(get_double_matrix(mat, i, j, &out), NO_ERROR);
            assert_true(out == 0.0f);
        }
    }

    assert_true(double_matrix_is_zero(mat));
    return_double_matrix(mat);
}

static void test_copy_double_dense_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_matrix_t* src = _make_sample_dense_double_matrix();

    double_matrix_expect_t r = copy_double_matrix(src, a);
    assert_true(r.has_value);
    assert_true(double_matrix_equal(src, r.u.value));

    return_double_matrix(r.u.value);
    return_double_matrix(src);
}

static void test_copy_double_coo_is_independent_of_source(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_matrix_t* src = _make_coo_double_matrix(3u, 3u, 4u, true);

    assert_int_equal(set_double_matrix(src, 1u, 1u, 5.0f), NO_ERROR);

    double_matrix_expect_t r = copy_double_matrix(src, a);
    assert_true(r.has_value);

    assert_int_equal(set_double_matrix(r.u.value, 1u, 1u, 77.0f), NO_ERROR);

    double src_out = 0.0f;
    double dst_out = 0.0f;

    assert_int_equal(get_double_matrix(src, 1u, 1u, &src_out), NO_ERROR);
    assert_int_equal(get_double_matrix(r.u.value, 1u, 1u, &dst_out), NO_ERROR);

    assert_true(src_out == 5.0f);
    assert_true(dst_out == 77.0f);

    return_double_matrix(r.u.value);
    return_double_matrix(src);
}

/* =============================================================================
 * Group 6: convert / convert_zero
 * ========================================================================== */

static void test_convert_double_dense_to_coo_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_matrix_t* src = _make_sample_dense_double_matrix();

    double_matrix_expect_t r = convert_double_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)COO_MATRIX);
    assert_true(double_matrix_equal(src, r.u.value));

    return_double_matrix(r.u.value);
    return_double_matrix(src);
}

static void test_convert_double_dense_to_csr_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_matrix_t* src = _make_sample_dense_double_matrix();

    double_matrix_expect_t r = convert_double_matrix(src, CSR_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)CSR_MATRIX);
    assert_true(double_matrix_equal(src, r.u.value));

    return_double_matrix(r.u.value);
    return_double_matrix(src);
}

static void test_convert_double_zero_dense_to_csr_omits_neg_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_matrix_t* src = _make_dense_double_matrix(2u, 3u);

    assert_int_equal(set_double_matrix(src, 0u, 0u, 5.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(src, 0u, 1u, -1.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(src, 1u, 2u, 8.0f), NO_ERROR);

    double_matrix_expect_t r =
        convert_double_matrix_zero(src, CSR_MATRIX, a, _double_zero_or_neg_one);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_nnz(r.u.value), 2);

    return_double_matrix(r.u.value);
    return_double_matrix(src);
}

/* =============================================================================
 * Group 7: transpose / fill
 * ========================================================================== */

static void test_transpose_double_dense_moves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    double_matrix_t* src = _make_sample_dense_double_matrix();

    double_matrix_expect_t r = transpose_double_matrix(src, a);
    assert_true(r.has_value);

    double out = 0.0f;
    assert_int_equal(get_double_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 10.0f);

    assert_int_equal(get_double_matrix(r.u.value, 3u, 1u, &out), NO_ERROR);
    assert_true(out == 20.0f);

    return_double_matrix(r.u.value);
    return_double_matrix(src);
}

static void test_fill_double_dense_sets_every_entry(void** state) {
    (void)state;
    double_matrix_t* mat = _make_dense_double_matrix(2u, 3u);
    double out = 0.0f;

    assert_int_equal(fill_double_matrix(mat, 7.5f), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            assert_int_equal(get_double_matrix(mat, i, j, &out), NO_ERROR);
            assert_true(out == 7.5f);
        }
    }

    return_double_matrix(mat);
}

static void test_fill_double_zero_equivalent_to_clear(void** state) {
    (void)state;
    double_matrix_t* mat = _make_dense_double_matrix(2u, 2u);

    assert_int_equal(set_double_matrix(mat, 1u, 1u, 9.0f), NO_ERROR);
    assert_false(double_matrix_is_zero(mat));

    assert_int_equal(fill_double_matrix(mat, 0.0f), NO_ERROR);
    assert_true(double_matrix_is_zero(mat));

    return_double_matrix(mat);
}

/* =============================================================================
 * Group 8: equality / compatibility
 * ========================================================================== */

static void test_double_matrix_equal_dense_same_values_returns_true(void** state) {
    (void)state;

    double_matrix_t* a = _make_sample_dense_double_matrix();
    double_matrix_t* b = _make_sample_dense_double_matrix();

    assert_true(double_matrix_equal(a, b));

    return_double_matrix(a);
    return_double_matrix(b);
}

static void test_double_matrix_shape_compatibility_helpers(void** state) {
    (void)state;

    double_matrix_t* a = _make_dense_double_matrix(2u, 3u);
    double_matrix_t* b = _make_dense_double_matrix(2u, 3u);
    double_matrix_t* c = _make_dense_double_matrix(3u, 4u);

    assert_true(double_matrix_has_same_shape(a, b));
    assert_true(double_matrix_is_add_compatible(a, b));
    assert_true(double_matrix_is_multiply_compatible(a, c));
    assert_false(double_matrix_is_square(a));

    return_double_matrix(a);
    return_double_matrix(b);
    return_double_matrix(c);
}

/* =============================================================================
 * Group 9: swaps / constructors / vector queries
 * ========================================================================== */

static void test_swap_double_matrix_rows_dense(void** state) {
    (void)state;
    double_matrix_t* mat = _make_dense_double_matrix(2u, 2u);
    double out = 0.0f;

    assert_int_equal(set_double_matrix(mat, 0u, 0u, 1.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(mat, 1u, 0u, 2.0f), NO_ERROR);

    assert_int_equal(swap_double_matrix_rows(mat, 0u, 1u), NO_ERROR);

    assert_int_equal(get_double_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_true(out == 2.0f);

    assert_int_equal(get_double_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 1.0f);

    return_double_matrix(mat);
}

static void test_init_double_identity_matrix_sets_diagonal_to_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    double_matrix_expect_t r = init_double_identity_matrix(3u, a);
    assert_true(r.has_value);

    double out = 0.0f;
    assert_int_equal(get_double_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_true(out == 1.0f);

    assert_int_equal(get_double_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_true(out == 0.0f);

    return_double_matrix(r.u.value);
}

static void test_double_row_and_col_vector_queries(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    double_matrix_expect_t row = init_double_row_vector(5u, a);
    double_matrix_expect_t col = init_double_col_vector(7u, a);

    assert_true(row.has_value);
    assert_true(col.has_value);

    assert_true(double_matrix_is_row_vector(row.u.value));
    assert_true(double_matrix_is_vector(row.u.value));
    assert_int_equal((int)double_matrix_vector_length(row.u.value), 5);

    assert_true(double_matrix_is_col_vector(col.u.value));
    assert_true(double_matrix_is_vector(col.u.value));
    assert_int_equal((int)double_matrix_vector_length(col.u.value), 7);

    return_double_matrix(row.u.value);
    return_double_matrix(col.u.value);
}
// ================================================================================
// Group 10: Additional double matrix tests
// ================================================================================

static void test_transpose_double_coo_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    double_matrix_t* src = _make_sample_coo_double_matrix();

    double_matrix_expect_t r = transpose_double_matrix(src, alloc);
    assert_true(r.has_value);

    double_matrix_t* tr = r.u.value;
    double out = 0.0f;

    assert_int_equal((int)matrix_format(tr), (int)COO_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_double_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 10.0f);

    assert_int_equal(get_double_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_true(out == 20.0f);

    assert_int_equal(get_double_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_true(out == 30.0f);

    assert_int_equal(get_double_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_true(out == 40.0f);

    return_double_matrix(tr);
    return_double_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_transpose_double_csr_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    double_matrix_t* dense = _make_sample_dense_double_matrix();

    double_matrix_expect_t src_r = convert_double_matrix(dense, CSR_MATRIX, alloc);
    assert_true(src_r.has_value);

    double_matrix_expect_t tr_r = transpose_double_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    double_matrix_t* tr = tr_r.u.value;
    double out = 0.0f;

    assert_int_equal((int)matrix_format(tr), (int)CSR_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_double_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 10.0f);

    assert_int_equal(get_double_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_true(out == 20.0f);

    assert_int_equal(get_double_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_true(out == 30.0f);

    assert_int_equal(get_double_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_true(out == 40.0f);

    return_double_matrix(tr);
    return_double_matrix(src_r.u.value);
    return_double_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_transpose_double_csc_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    double_matrix_t* dense = _make_sample_dense_double_matrix();

    double_matrix_expect_t src_r = convert_double_matrix(dense, CSC_MATRIX, alloc);
    assert_true(src_r.has_value);

    double_matrix_expect_t tr_r = transpose_double_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    double_matrix_t* tr = tr_r.u.value;
    double out = 0.0f;

    assert_int_equal((int)matrix_format(tr), (int)CSC_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_double_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 10.0f);

    assert_int_equal(get_double_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_true(out == 20.0f);

    assert_int_equal(get_double_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_true(out == 30.0f);

    assert_int_equal(get_double_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_true(out == 40.0f);

    return_double_matrix(tr);
    return_double_matrix(src_r.u.value);
    return_double_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_swap_double_matrix_cols_dense(void** state) {
    (void)state;

    double_matrix_t* mat = _make_dense_double_matrix(2u, 3u);
    double out = 0.0f;

    assert_int_equal(set_double_matrix(mat, 0u, 0u, 1.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(mat, 0u, 2u, 3.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(mat, 1u, 0u, 4.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(mat, 1u, 2u, 6.0f), NO_ERROR);

    assert_int_equal(swap_double_matrix_cols(mat, 0u, 2u), NO_ERROR);

    assert_int_equal(get_double_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_true(out == 3.0f);

    assert_int_equal(get_double_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_true(out == 1.0f);

    assert_int_equal(get_double_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 6.0f);

    assert_int_equal(get_double_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_true(out == 4.0f);

    return_double_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_double_matrix_is_sparse_dense_returns_false(void** state) {
    (void)state;

    double_matrix_t* mat = _make_dense_double_matrix(3u, 3u);
    assert_false(double_matrix_is_sparse(mat));

    return_double_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_double_matrix_is_sparse_coo_returns_true(void** state) {
    (void)state;

    double_matrix_t* mat = _make_coo_double_matrix(3u, 3u, 4u, true);
    assert_true(double_matrix_is_sparse(mat));

    return_double_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_double_matrix_is_sparse_csr_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    double_matrix_t* dense = _make_sample_dense_double_matrix();

    double_matrix_expect_t r = convert_double_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(double_matrix_is_sparse(r.u.value));

    return_double_matrix(r.u.value);
    return_double_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_double_matrix_is_sparse_csc_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    double_matrix_t* dense = _make_sample_dense_double_matrix();

    double_matrix_expect_t r = convert_double_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(double_matrix_is_sparse(r.u.value));

    return_double_matrix(r.u.value);
    return_double_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_double_matrix_equal_dense_and_coo_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    double_matrix_t* dense = _make_sample_dense_double_matrix();

    double_matrix_expect_t r = convert_double_matrix(dense, COO_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(double_matrix_equal(dense, r.u.value));
    assert_true(double_matrix_equal(r.u.value, dense));

    return_double_matrix(r.u.value);
    return_double_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_double_matrix_equal_dense_and_csr_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    double_matrix_t* dense = _make_sample_dense_double_matrix();

    double_matrix_expect_t r = convert_double_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(double_matrix_equal(dense, r.u.value));
    assert_true(double_matrix_equal(r.u.value, dense));

    return_double_matrix(r.u.value);
    return_double_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_double_matrix_equal_dense_and_csc_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    double_matrix_t* dense = _make_sample_dense_double_matrix();

    double_matrix_expect_t r = convert_double_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(double_matrix_equal(dense, r.u.value));
    assert_true(double_matrix_equal(r.u.value, dense));

    return_double_matrix(r.u.value);
    return_double_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_convert_double_matrix_zero_null_callback_matches_convert_double_matrix_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    double_matrix_t* src = _make_sample_dense_double_matrix();

    double_matrix_expect_t r1 = convert_double_matrix(src, COO_MATRIX, alloc);
    double_matrix_expect_t r2 = convert_double_matrix_zero(src, COO_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(double_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_double_matrix(r2.u.value);
    return_double_matrix(r1.u.value);
    return_double_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_double_matrix_zero_null_callback_matches_convert_double_matrix_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    double_matrix_t* src = _make_sample_dense_double_matrix();

    double_matrix_expect_t r1 = convert_double_matrix(src, CSR_MATRIX, alloc);
    double_matrix_expect_t r2 = convert_double_matrix_zero(src, CSR_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(double_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_double_matrix(r2.u.value);
    return_double_matrix(r1.u.value);
    return_double_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_double_matrix_zero_null_callback_matches_convert_double_matrix_csc(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    double_matrix_t* src = _make_sample_dense_double_matrix();

    double_matrix_expect_t r1 = convert_double_matrix(src, CSC_MATRIX, alloc);
    double_matrix_expect_t r2 = convert_double_matrix_zero(src, CSC_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(double_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_double_matrix(r2.u.value);
    return_double_matrix(r1.u.value);
    return_double_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_double_matrix_zero_treats_negative_zero_as_zero_in_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    double_matrix_t* src = _make_dense_double_matrix(2u, 2u);

    double neg_zero = -0.0f;
    double one = 1.0f;

    assert_int_equal(set_double_matrix(src, 0u, 0u, neg_zero), NO_ERROR);
    assert_int_equal(set_double_matrix(src, 1u, 1u, one), NO_ERROR);

    double_matrix_expect_t r = convert_double_matrix_zero(src, COO_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    /* -0.0f should be treated as semantic zero and omitted */
    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    double out = 123.0f;
    assert_int_equal(get_double_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_true(out == 0.0f);

    assert_int_equal(get_double_matrix(r.u.value, 1u, 1u, &out), NO_ERROR);
    assert_true(out == 1.0f);

    return_double_matrix(r.u.value);
    return_double_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_double_matrix_zero_treats_negative_zero_as_zero_in_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    double_matrix_t* src = _make_dense_double_matrix(2u, 2u);

    assert_int_equal(set_double_matrix(src, 0u, 1u, -0.0f), NO_ERROR);
    assert_int_equal(set_double_matrix(src, 1u, 0u, 2.0f), NO_ERROR);

    double_matrix_expect_t r = convert_double_matrix_zero(src, CSR_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    double out = 0.0f;
    assert_int_equal(get_double_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_true(out == 0.0f);

    assert_int_equal(get_double_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 2.0f);

    return_double_matrix(r.u.value);
    return_double_matrix(src);
}

/* =============================================================================
 * Registry
 * ========================================================================== */

const struct CMUnitTest test_double_matrix[] = {
    /* Group 1 */
    cmocka_unit_test(test_double_dense_init_zero_rows_fails),
    cmocka_unit_test(test_double_dense_init_zero_cols_fails),
    cmocka_unit_test(test_double_dense_init_returns_valid_matrix),
    cmocka_unit_test(test_double_dense_init_is_zero_initialized),

    /* Group 2 */
    cmocka_unit_test(test_double_coo_init_zero_capacity_fails),
    cmocka_unit_test(test_double_coo_init_returns_valid_matrix),
    cmocka_unit_test(test_double_coo_unset_entry_reads_back_as_zero),

    /* Group 3 */
    cmocka_unit_test(test_double_set_dense_null_matrix_returns_null_pointer),
    cmocka_unit_test(test_double_get_dense_null_out_returns_null_pointer),
    cmocka_unit_test(test_double_set_get_dense_single_value),
    cmocka_unit_test(test_double_set_dense_overwrites_existing_value),

    /* Group 4 */
    cmocka_unit_test(test_double_set_get_coo_single_value),
    cmocka_unit_test(test_double_push_back_coo_overwrite_does_not_increase_nnz),
    cmocka_unit_test(test_double_push_back_coo_capacity_overflow_when_growth_disabled),

    /* Group 5 */
    cmocka_unit_test(test_clear_double_dense_zeroes_all_entries),
    cmocka_unit_test(test_copy_double_dense_preserves_values),
    cmocka_unit_test(test_copy_double_coo_is_independent_of_source),

    /* Group 6 */
    cmocka_unit_test(test_convert_double_dense_to_coo_preserves_values),
    cmocka_unit_test(test_convert_double_dense_to_csr_preserves_values),
    cmocka_unit_test(test_convert_double_zero_dense_to_csr_omits_neg_one),

    /* Group 7 */
    cmocka_unit_test(test_transpose_double_dense_moves_values),
    cmocka_unit_test(test_fill_double_dense_sets_every_entry),
    cmocka_unit_test(test_fill_double_zero_equivalent_to_clear),

    /* Group 8 */
    cmocka_unit_test(test_double_matrix_equal_dense_same_values_returns_true),
    cmocka_unit_test(test_double_matrix_shape_compatibility_helpers),

    /* Group 9 */
    cmocka_unit_test(test_swap_double_matrix_rows_dense),
    cmocka_unit_test(test_init_double_identity_matrix_sets_diagonal_to_one),
    cmocka_unit_test(test_double_row_and_col_vector_queries),

    /* Group 10 */
    cmocka_unit_test(test_transpose_double_coo_preserves_logical_values),
    cmocka_unit_test(test_transpose_double_csr_preserves_logical_values),
    cmocka_unit_test(test_transpose_double_csc_preserves_logical_values),

    cmocka_unit_test(test_swap_double_matrix_cols_dense),

    cmocka_unit_test(test_double_matrix_is_sparse_dense_returns_false),
    cmocka_unit_test(test_double_matrix_is_sparse_coo_returns_true),
    cmocka_unit_test(test_double_matrix_is_sparse_csr_returns_true),
    cmocka_unit_test(test_double_matrix_is_sparse_csc_returns_true),

    cmocka_unit_test(test_double_matrix_equal_dense_and_coo_same_values_returns_true),
    cmocka_unit_test(test_double_matrix_equal_dense_and_csr_same_values_returns_true),
    cmocka_unit_test(test_double_matrix_equal_dense_and_csc_same_values_returns_true),

    cmocka_unit_test(test_convert_double_matrix_zero_null_callback_matches_convert_double_matrix_coo),
    cmocka_unit_test(test_convert_double_matrix_zero_null_callback_matches_convert_double_matrix_csr),
    cmocka_unit_test(test_convert_double_matrix_zero_null_callback_matches_convert_double_matrix_csc),

    cmocka_unit_test(test_convert_double_matrix_zero_treats_negative_zero_as_zero_in_coo),
    cmocka_unit_test(test_convert_double_matrix_zero_treats_negative_zero_as_zero_in_csr),
};

const size_t test_double_matrix_count =
    sizeof(test_double_matrix) / sizeof(test_double_matrix[0]);
// ================================================================================ 
// ================================================================================ 

/* =============================================================================
 * Helpers
 * ========================================================================== */

static ldouble_matrix_t* _make_dense_ldouble_matrix(size_t rows, size_t cols) {
    allocator_vtable_t a = heap_allocator();
    ldouble_matrix_expect_t r = init_ldouble_dense_matrix(rows, cols, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

static ldouble_matrix_t* _make_coo_ldouble_matrix(size_t rows,
                                              size_t cols,
                                              size_t cap,
                                              bool   growth) {
    allocator_vtable_t a = heap_allocator();
    ldouble_matrix_expect_t r = init_ldouble_coo_matrix(rows, cols, cap, growth, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

static ldouble_matrix_t* _make_sample_dense_ldouble_matrix(void) {
    ldouble_matrix_t* mat = _make_dense_ldouble_matrix(3u, 4u);

    assert_int_equal(set_ldouble_matrix(mat, 0u, 1u, 10.0L), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(mat, 1u, 3u, 20.0L), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(mat, 2u, 0u, 30.0L), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(mat, 2u, 2u, 40.0L), NO_ERROR);

    return mat;
}

static ldouble_matrix_t* _make_sample_coo_ldouble_matrix(void) {
    ldouble_matrix_t* mat = _make_coo_ldouble_matrix(3u, 4u, 8u, true);

    assert_int_equal(set_ldouble_matrix(mat, 0u, 1u, 10.0L), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(mat, 1u, 3u, 20.0L), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(mat, 2u, 0u, 30.0L), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(mat, 2u, 2u, 40.0L), NO_ERROR);

    return mat;
}

static bool _ldouble_abs_equal(long double a, long double b) {
    return fabsf(fabsf(a) - fabsf(b)) < 1e-6f;
}

static bool _ldouble_zero_or_neg_one(long double v) {
    return (v == 0.0L) || (v == -1.0L);
}

/* =============================================================================
 * Group 1: init_ldouble_dense_matrix
 * ========================================================================== */

static void test_ldouble_dense_init_zero_rows_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    ldouble_matrix_expect_t r = init_ldouble_dense_matrix(0u, 4u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_ldouble_dense_init_zero_cols_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3u, 0u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_ldouble_dense_init_returns_valid_matrix(void** state) {
    (void)state;

    ldouble_matrix_t* mat = _make_dense_ldouble_matrix(3u, 4u);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)LDOUBLE_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(long double));
    assert_int_equal((int)matrix_format(mat), (int)DENSE_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 12);

    return_ldouble_matrix(mat);
}

static void test_ldouble_dense_init_is_zero_initialized(void** state) {
    (void)state;

    ldouble_matrix_t* mat = _make_dense_ldouble_matrix(2u, 3u);
    long double out = -1.0L;

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = -1.0f;
            assert_int_equal(get_ldouble_matrix(mat, i, j, &out), NO_ERROR);
            assert_true(out == 0.0f);
        }
    }

    assert_true(ldouble_matrix_is_zero(mat));
    return_ldouble_matrix(mat);
}

/* =============================================================================
 * Group 2: init_ldouble_coo_matrix
 * ========================================================================== */

static void test_ldouble_coo_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    ldouble_matrix_expect_t r = init_ldouble_coo_matrix(3u, 4u, 0u, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_ldouble_coo_init_returns_valid_matrix(void** state) {
    (void)state;

    ldouble_matrix_t* mat = _make_coo_ldouble_matrix(3u, 4u, 8u, true);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)LDOUBLE_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(long double));
    assert_int_equal((int)matrix_format(mat), (int)COO_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 0);
    assert_true(mat->rep.coo.growth);
    assert_true(mat->rep.coo.sorted);

    return_ldouble_matrix(mat);
}

static void test_ldouble_coo_unset_entry_reads_back_as_zero(void** state) {
    (void)state;

    ldouble_matrix_t* mat = _make_coo_ldouble_matrix(3u, 4u, 8u, true);
    long double out = -1.0f;

    assert_int_equal(get_ldouble_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_true(out == 0.0f);

    return_ldouble_matrix(mat);
}

/* =============================================================================
 * Group 3: dense set/get
 * ========================================================================== */

static void test_ldouble_set_dense_null_matrix_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_ldouble_matrix(NULL, 0u, 0u, 7.0f), NULL_POINTER);
}

static void test_ldouble_get_dense_null_out_returns_null_pointer(void** state) {
    (void)state;
    ldouble_matrix_t* mat = _make_dense_ldouble_matrix(2u, 2u);

    assert_int_equal(get_ldouble_matrix(mat, 0u, 0u, NULL), NULL_POINTER);
    return_ldouble_matrix(mat);
}

static void test_ldouble_set_get_dense_single_value(void** state) {
    (void)state;
    ldouble_matrix_t* mat = _make_dense_ldouble_matrix(3u, 3u);

    long double out = 0.0L;
    assert_int_equal(set_ldouble_matrix(mat, 1u, 2u, 42.5L), NO_ERROR);
    assert_int_equal(get_ldouble_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_true(out == 42.5L);

    return_ldouble_matrix(mat);
}

static void test_ldouble_set_dense_overwrites_existing_value(void** state) {
    (void)state;
    ldouble_matrix_t* mat = _make_dense_ldouble_matrix(2u, 2u);

    long double out = 0.0L;
    assert_int_equal(set_ldouble_matrix(mat, 1u, 1u, 5.0f), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(mat, 1u, 1u, 99.0f), NO_ERROR);
    assert_int_equal(get_ldouble_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_true(out == 99.0f);

    return_ldouble_matrix(mat);
}

/* =============================================================================
 * Group 4: COO set/get
 * ========================================================================== */

static void test_ldouble_set_get_coo_single_value(void** state) {
    (void)state;
    ldouble_matrix_t* mat = _make_coo_ldouble_matrix(3u, 3u, 4u, true);

    long double out = 0.0L;
    assert_int_equal(set_ldouble_matrix(mat, 2u, 1u, 123.25L), NO_ERROR);
    assert_int_equal(get_ldouble_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_true(out == 123.25L);
    assert_int_equal((int)matrix_nnz(mat), 1);

    return_ldouble_matrix(mat);
}

static void test_ldouble_push_back_coo_overwrite_does_not_increase_nnz(void** state) {
    (void)state;
    ldouble_matrix_t* mat = _make_coo_ldouble_matrix(3u, 3u, 4u, true);

    long double out = 0.0L;
    assert_int_equal(push_back_ldouble_coo_matrix(mat, 1u, 1u, 5.0L), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(push_back_ldouble_coo_matrix(mat, 1u, 1u, 99.0L), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(get_ldouble_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_true(out == 99.0L);

    return_ldouble_matrix(mat);
}

static void test_ldouble_push_back_coo_capacity_overflow_when_growth_disabled(void** state) {
    (void)state;
    ldouble_matrix_t* mat = _make_coo_ldouble_matrix(3u, 3u, 2u, false);

    assert_int_equal(push_back_ldouble_coo_matrix(mat, 0u, 0u, 1.0L), NO_ERROR);
    assert_int_equal(push_back_ldouble_coo_matrix(mat, 1u, 1u, 2.0L), NO_ERROR);
    assert_int_equal(push_back_ldouble_coo_matrix(mat, 2u, 2u, 3.0L), CAPACITY_OVERFLOW);

    return_ldouble_matrix(mat);
}

/* =============================================================================
 * Group 5: clear / copy
 * ========================================================================== */

static void test_clear_ldouble_dense_zeroes_all_entries(void** state) {
    (void)state;
    ldouble_matrix_t* mat = _make_dense_ldouble_matrix(2u, 3u);
    long double out = -1.0L;

    assert_int_equal(set_ldouble_matrix(mat, 0u, 0u, 11.0L), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(mat, 0u, 2u, 22.0L), NO_ERROR);
    assert_false(ldouble_matrix_is_zero(mat));

    assert_int_equal(clear_ldouble_matrix(mat), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = -1.0f;
            assert_int_equal(get_ldouble_matrix(mat, i, j, &out), NO_ERROR);
            assert_true(out == 0.0f);
        }
    }

    assert_true(ldouble_matrix_is_zero(mat));
    return_ldouble_matrix(mat);
}

static void test_copy_ldouble_dense_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_matrix_t* src = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t r = copy_ldouble_matrix(src, a);
    assert_true(r.has_value);
    assert_true(ldouble_matrix_equal(src, r.u.value));

    return_ldouble_matrix(r.u.value);
    return_ldouble_matrix(src);
}

static void test_copy_ldouble_coo_is_independent_of_source(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_matrix_t* src = _make_coo_ldouble_matrix(3u, 3u, 4u, true);

    assert_int_equal(set_ldouble_matrix(src, 1u, 1u, 5.0L), NO_ERROR);

    ldouble_matrix_expect_t r = copy_ldouble_matrix(src, a);
    assert_true(r.has_value);

    assert_int_equal(set_ldouble_matrix(r.u.value, 1u, 1u, 77.0L), NO_ERROR);

    long double src_out = 0.0L;
    long double dst_out = 0.0L;

    assert_int_equal(get_ldouble_matrix(src, 1u, 1u, &src_out), NO_ERROR);
    assert_int_equal(get_ldouble_matrix(r.u.value, 1u, 1u, &dst_out), NO_ERROR);

    assert_true(src_out == 5.0L);
    assert_true(dst_out == 77.0L);

    return_ldouble_matrix(r.u.value);
    return_ldouble_matrix(src);
}

/* =============================================================================
 * Group 6: convert / convert_zero
 * ========================================================================== */

static void test_convert_ldouble_dense_to_coo_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_matrix_t* src = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t r = convert_ldouble_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)COO_MATRIX);
    assert_true(ldouble_matrix_equal(src, r.u.value));

    return_ldouble_matrix(r.u.value);
    return_ldouble_matrix(src);
}

static void test_convert_ldouble_dense_to_csr_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_matrix_t* src = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t r = convert_ldouble_matrix(src, CSR_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)CSR_MATRIX);
    assert_true(ldouble_matrix_equal(src, r.u.value));

    return_ldouble_matrix(r.u.value);
    return_ldouble_matrix(src);
}

static void test_convert_ldouble_zero_dense_to_csr_omits_neg_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_matrix_t* src = _make_dense_ldouble_matrix(2u, 3u);

    assert_int_equal(set_ldouble_matrix(src, 0u, 0u, 5.0L), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(src, 0u, 1u, -1.0L), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(src, 1u, 2u, 8.0L), NO_ERROR);

    ldouble_matrix_expect_t r =
        convert_ldouble_matrix_zero(src, CSR_MATRIX, a, _ldouble_zero_or_neg_one);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_nnz(r.u.value), 2);

    return_ldouble_matrix(r.u.value);
    return_ldouble_matrix(src);
}

/* =============================================================================
 * Group 7: transpose / fill
 * ========================================================================== */

static void test_transpose_ldouble_dense_moves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    ldouble_matrix_t* src = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t r = transpose_ldouble_matrix(src, a);
    assert_true(r.has_value);

    long double out = 0.0L;
    assert_int_equal(get_ldouble_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 10.0L);

    assert_int_equal(get_ldouble_matrix(r.u.value, 3u, 1u, &out), NO_ERROR);
    assert_true(out == 20.0L);

    return_ldouble_matrix(r.u.value);
    return_ldouble_matrix(src);
}

static void test_fill_ldouble_dense_sets_every_entry(void** state) {
    (void)state;
    ldouble_matrix_t* mat = _make_dense_ldouble_matrix(2u, 3u);
    long double out = 0.0L;

    assert_int_equal(fill_ldouble_matrix(mat, 7.5L), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            assert_int_equal(get_ldouble_matrix(mat, i, j, &out), NO_ERROR);
            assert_true(out == 7.5L);
        }
    }

    return_ldouble_matrix(mat);
}

static void test_fill_ldouble_zero_equivalent_to_clear(void** state) {
    (void)state;
    ldouble_matrix_t* mat = _make_dense_ldouble_matrix(2u, 2u);

    assert_int_equal(set_ldouble_matrix(mat, 1u, 1u, 9.0L), NO_ERROR);
    assert_false(ldouble_matrix_is_zero(mat));

    assert_int_equal(fill_ldouble_matrix(mat, 0.0L), NO_ERROR);
    assert_true(ldouble_matrix_is_zero(mat));

    return_ldouble_matrix(mat);
}

/* =============================================================================
 * Group 8: equality / compatibility
 * ========================================================================== */

static void test_ldouble_matrix_equal_dense_same_values_returns_true(void** state) {
    (void)state;

    ldouble_matrix_t* a = _make_sample_dense_ldouble_matrix();
    ldouble_matrix_t* b = _make_sample_dense_ldouble_matrix();

    assert_true(ldouble_matrix_equal(a, b));

    return_ldouble_matrix(a);
    return_ldouble_matrix(b);
}

static void test_ldouble_matrix_shape_compatibility_helpers(void** state) {
    (void)state;

    ldouble_matrix_t* a = _make_dense_ldouble_matrix(2u, 3u);
    ldouble_matrix_t* b = _make_dense_ldouble_matrix(2u, 3u);
    ldouble_matrix_t* c = _make_dense_ldouble_matrix(3u, 4u);

    assert_true(ldouble_matrix_has_same_shape(a, b));
    assert_true(ldouble_matrix_is_add_compatible(a, b));
    assert_true(ldouble_matrix_is_multiply_compatible(a, c));
    assert_false(ldouble_matrix_is_square(a));

    return_ldouble_matrix(a);
    return_ldouble_matrix(b);
    return_ldouble_matrix(c);
}

/* =============================================================================
 * Group 9: swaps / constructors / vector queries
 * ========================================================================== */

static void test_swap_ldouble_matrix_rows_dense(void** state) {
    (void)state;
    ldouble_matrix_t* mat = _make_dense_ldouble_matrix(2u, 2u);
    long double out = 0.0L;

    assert_int_equal(set_ldouble_matrix(mat, 0u, 0u, 1.0L), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(mat, 1u, 0u, 2.0L), NO_ERROR);

    assert_int_equal(swap_ldouble_matrix_rows(mat, 0u, 1u), NO_ERROR);

    assert_int_equal(get_ldouble_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_true(out == 2.0L);

    assert_int_equal(get_ldouble_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 1.0L);

    return_ldouble_matrix(mat);
}

static void test_init_ldouble_identity_matrix_sets_diagonal_to_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    ldouble_matrix_expect_t r = init_ldouble_identity_matrix(3u, a);
    assert_true(r.has_value);

    long double out = 0.0L;
    assert_int_equal(get_ldouble_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_true(out == 1.0L);

    assert_int_equal(get_ldouble_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_true(out == 0.0L);

    return_ldouble_matrix(r.u.value);
}

static void test_ldouble_row_and_col_vector_queries(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    ldouble_matrix_expect_t row = init_ldouble_row_vector(5u, a);
    ldouble_matrix_expect_t col = init_ldouble_col_vector(7u, a);

    assert_true(row.has_value);
    assert_true(col.has_value);

    assert_true(ldouble_matrix_is_row_vector(row.u.value));
    assert_true(ldouble_matrix_is_vector(row.u.value));
    assert_int_equal((int)ldouble_matrix_vector_length(row.u.value), 5);

    assert_true(ldouble_matrix_is_col_vector(col.u.value));
    assert_true(ldouble_matrix_is_vector(col.u.value));
    assert_int_equal((int)ldouble_matrix_vector_length(col.u.value), 7);

    return_ldouble_matrix(row.u.value);
    return_ldouble_matrix(col.u.value);
}
// ================================================================================
// Group 10: Additional ldouble matrix tests
// ================================================================================

static void test_transpose_ldouble_coo_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    ldouble_matrix_t* src = _make_sample_coo_ldouble_matrix();

    ldouble_matrix_expect_t r = transpose_ldouble_matrix(src, alloc);
    assert_true(r.has_value);

    ldouble_matrix_t* tr = r.u.value;
    long double out = 0.0L;

    assert_int_equal((int)matrix_format(tr), (int)COO_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_ldouble_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 10.0L);

    assert_int_equal(get_ldouble_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_true(out == 20.0L);

    assert_int_equal(get_ldouble_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_true(out == 30.0L);

    assert_int_equal(get_ldouble_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_true(out == 40.0L);

    return_ldouble_matrix(tr);
    return_ldouble_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_transpose_ldouble_csr_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    ldouble_matrix_t* dense = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t src_r = convert_ldouble_matrix(dense, CSR_MATRIX, alloc);
    assert_true(src_r.has_value);

    ldouble_matrix_expect_t tr_r = transpose_ldouble_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    ldouble_matrix_t* tr = tr_r.u.value;
    long double out = 0.0L;

    assert_int_equal((int)matrix_format(tr), (int)CSR_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_ldouble_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 10.0L);

    assert_int_equal(get_ldouble_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_true(out == 20.0L);

    assert_int_equal(get_ldouble_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_true(out == 30.0L);

    assert_int_equal(get_ldouble_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_true(out == 40.0L);

    return_ldouble_matrix(tr);
    return_ldouble_matrix(src_r.u.value);
    return_ldouble_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_transpose_ldouble_csc_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    ldouble_matrix_t* dense = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t src_r = convert_ldouble_matrix(dense, CSC_MATRIX, alloc);
    assert_true(src_r.has_value);

    ldouble_matrix_expect_t tr_r = transpose_ldouble_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    ldouble_matrix_t* tr = tr_r.u.value;
    long double out = 0.0L;

    assert_int_equal((int)matrix_format(tr), (int)CSC_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_ldouble_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 10.0L);

    assert_int_equal(get_ldouble_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_true(out == 20.0L);

    assert_int_equal(get_ldouble_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_true(out == 30.0L);

    assert_int_equal(get_ldouble_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_true(out == 40.0L);

    return_ldouble_matrix(tr);
    return_ldouble_matrix(src_r.u.value);
    return_ldouble_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_swap_ldouble_matrix_cols_dense(void** state) {
    (void)state;

    ldouble_matrix_t* mat = _make_dense_ldouble_matrix(2u, 3u);
    long double out = 0.0L;

    assert_int_equal(set_ldouble_matrix(mat, 0u, 0u, 1.0f), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(mat, 0u, 2u, 3.0f), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(mat, 1u, 0u, 4.0f), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(mat, 1u, 2u, 6.0f), NO_ERROR);

    assert_int_equal(swap_ldouble_matrix_cols(mat, 0u, 2u), NO_ERROR);

    assert_int_equal(get_ldouble_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_true(out == 3.0L);

    assert_int_equal(get_ldouble_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_true(out == 1.0L);

    assert_int_equal(get_ldouble_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 6.0L);

    assert_int_equal(get_ldouble_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_true(out == 4.0L);

    return_ldouble_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_ldouble_matrix_is_sparse_dense_returns_false(void** state) {
    (void)state;

    ldouble_matrix_t* mat = _make_dense_ldouble_matrix(3u, 3u);
    assert_false(ldouble_matrix_is_sparse(mat));

    return_ldouble_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_ldouble_matrix_is_sparse_coo_returns_true(void** state) {
    (void)state;

    ldouble_matrix_t* mat = _make_coo_ldouble_matrix(3u, 3u, 4u, true);
    assert_true(ldouble_matrix_is_sparse(mat));

    return_ldouble_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_ldouble_matrix_is_sparse_csr_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    ldouble_matrix_t* dense = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t r = convert_ldouble_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(ldouble_matrix_is_sparse(r.u.value));

    return_ldouble_matrix(r.u.value);
    return_ldouble_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_ldouble_matrix_is_sparse_csc_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    ldouble_matrix_t* dense = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t r = convert_ldouble_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(ldouble_matrix_is_sparse(r.u.value));

    return_ldouble_matrix(r.u.value);
    return_ldouble_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_ldouble_matrix_equal_dense_and_coo_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    ldouble_matrix_t* dense = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t r = convert_ldouble_matrix(dense, COO_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(ldouble_matrix_equal(dense, r.u.value));
    assert_true(ldouble_matrix_equal(r.u.value, dense));

    return_ldouble_matrix(r.u.value);
    return_ldouble_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_ldouble_matrix_equal_dense_and_csr_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    ldouble_matrix_t* dense = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t r = convert_ldouble_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(ldouble_matrix_equal(dense, r.u.value));
    assert_true(ldouble_matrix_equal(r.u.value, dense));

    return_ldouble_matrix(r.u.value);
    return_ldouble_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_ldouble_matrix_equal_dense_and_csc_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    ldouble_matrix_t* dense = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t r = convert_ldouble_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(ldouble_matrix_equal(dense, r.u.value));
    assert_true(ldouble_matrix_equal(r.u.value, dense));

    return_ldouble_matrix(r.u.value);
    return_ldouble_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_convert_ldouble_matrix_zero_null_callback_matches_convert_ldouble_matrix_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    ldouble_matrix_t* src = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t r1 = convert_ldouble_matrix(src, COO_MATRIX, alloc);
    ldouble_matrix_expect_t r2 = convert_ldouble_matrix_zero(src, COO_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(ldouble_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_ldouble_matrix(r2.u.value);
    return_ldouble_matrix(r1.u.value);
    return_ldouble_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_ldouble_matrix_zero_null_callback_matches_convert_ldouble_matrix_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    ldouble_matrix_t* src = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t r1 = convert_ldouble_matrix(src, CSR_MATRIX, alloc);
    ldouble_matrix_expect_t r2 = convert_ldouble_matrix_zero(src, CSR_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(ldouble_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_ldouble_matrix(r2.u.value);
    return_ldouble_matrix(r1.u.value);
    return_ldouble_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_ldouble_matrix_zero_null_callback_matches_convert_ldouble_matrix_csc(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    ldouble_matrix_t* src = _make_sample_dense_ldouble_matrix();

    ldouble_matrix_expect_t r1 = convert_ldouble_matrix(src, CSC_MATRIX, alloc);
    ldouble_matrix_expect_t r2 = convert_ldouble_matrix_zero(src, CSC_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(ldouble_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_ldouble_matrix(r2.u.value);
    return_ldouble_matrix(r1.u.value);
    return_ldouble_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_ldouble_matrix_zero_treats_negative_zero_as_zero_in_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    ldouble_matrix_t* src = _make_dense_ldouble_matrix(2u, 2u);

    long double neg_zero = -0.0L;
    long double one = 1.0L;

    assert_int_equal(set_ldouble_matrix(src, 0u, 0u, neg_zero), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(src, 1u, 1u, one), NO_ERROR);

    ldouble_matrix_expect_t r = convert_ldouble_matrix_zero(src, COO_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    /* -0.0f should be treated as semantic zero and omitted */
    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    long double out = 123.0L;
    assert_int_equal(get_ldouble_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_true(out == 0.0L);

    assert_int_equal(get_ldouble_matrix(r.u.value, 1u, 1u, &out), NO_ERROR);
    assert_true(out == 1.0L);

    return_ldouble_matrix(r.u.value);
    return_ldouble_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_ldouble_matrix_zero_treats_negative_zero_as_zero_in_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    ldouble_matrix_t* src = _make_dense_ldouble_matrix(2u, 2u);

    assert_int_equal(set_ldouble_matrix(src, 0u, 1u, -0.0L), NO_ERROR);
    assert_int_equal(set_ldouble_matrix(src, 1u, 0u, 2.0L), NO_ERROR);

    ldouble_matrix_expect_t r = convert_ldouble_matrix_zero(src, CSR_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    long double out = 0.0L;
    assert_int_equal(get_ldouble_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_true(out == 0.0L);

    assert_int_equal(get_ldouble_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_true(out == 2.0L);

    return_ldouble_matrix(r.u.value);
    return_ldouble_matrix(src);
}

/* =============================================================================
 * Registry
 * ========================================================================== */

const struct CMUnitTest test_ldouble_matrix[] = {
    /* Group 1 */
    cmocka_unit_test(test_ldouble_dense_init_zero_rows_fails),
    cmocka_unit_test(test_ldouble_dense_init_zero_cols_fails),
    cmocka_unit_test(test_ldouble_dense_init_returns_valid_matrix),
    cmocka_unit_test(test_ldouble_dense_init_is_zero_initialized),

    /* Group 2 */
    cmocka_unit_test(test_ldouble_coo_init_zero_capacity_fails),
    cmocka_unit_test(test_ldouble_coo_init_returns_valid_matrix),
    cmocka_unit_test(test_ldouble_coo_unset_entry_reads_back_as_zero),

    /* Group 3 */
    cmocka_unit_test(test_ldouble_set_dense_null_matrix_returns_null_pointer),
    cmocka_unit_test(test_ldouble_get_dense_null_out_returns_null_pointer),
    cmocka_unit_test(test_ldouble_set_get_dense_single_value),
    cmocka_unit_test(test_ldouble_set_dense_overwrites_existing_value),

    /* Group 4 */
    cmocka_unit_test(test_ldouble_set_get_coo_single_value),
    cmocka_unit_test(test_ldouble_push_back_coo_overwrite_does_not_increase_nnz),
    cmocka_unit_test(test_ldouble_push_back_coo_capacity_overflow_when_growth_disabled),

    /* Group 5 */
    cmocka_unit_test(test_clear_ldouble_dense_zeroes_all_entries),
    cmocka_unit_test(test_copy_ldouble_dense_preserves_values),
    cmocka_unit_test(test_copy_ldouble_coo_is_independent_of_source),

    /* Group 6 */
    cmocka_unit_test(test_convert_ldouble_dense_to_coo_preserves_values),
    cmocka_unit_test(test_convert_ldouble_dense_to_csr_preserves_values),
    cmocka_unit_test(test_convert_ldouble_zero_dense_to_csr_omits_neg_one),

    /* Group 7 */
    cmocka_unit_test(test_transpose_ldouble_dense_moves_values),
    cmocka_unit_test(test_fill_ldouble_dense_sets_every_entry),
    cmocka_unit_test(test_fill_ldouble_zero_equivalent_to_clear),

    /* Group 8 */
    cmocka_unit_test(test_ldouble_matrix_equal_dense_same_values_returns_true),
    cmocka_unit_test(test_ldouble_matrix_shape_compatibility_helpers),

    /* Group 9 */
    cmocka_unit_test(test_swap_ldouble_matrix_rows_dense),
    cmocka_unit_test(test_init_ldouble_identity_matrix_sets_diagonal_to_one),
    cmocka_unit_test(test_ldouble_row_and_col_vector_queries),

    /* Group 10 */
    cmocka_unit_test(test_transpose_ldouble_coo_preserves_logical_values),
    cmocka_unit_test(test_transpose_ldouble_csr_preserves_logical_values),
    cmocka_unit_test(test_transpose_ldouble_csc_preserves_logical_values),

    cmocka_unit_test(test_swap_ldouble_matrix_cols_dense),

    cmocka_unit_test(test_ldouble_matrix_is_sparse_dense_returns_false),
    cmocka_unit_test(test_ldouble_matrix_is_sparse_coo_returns_true),
    cmocka_unit_test(test_ldouble_matrix_is_sparse_csr_returns_true),
    cmocka_unit_test(test_ldouble_matrix_is_sparse_csc_returns_true),

    cmocka_unit_test(test_ldouble_matrix_equal_dense_and_coo_same_values_returns_true),
    cmocka_unit_test(test_ldouble_matrix_equal_dense_and_csr_same_values_returns_true),
    cmocka_unit_test(test_ldouble_matrix_equal_dense_and_csc_same_values_returns_true),

    cmocka_unit_test(test_convert_ldouble_matrix_zero_null_callback_matches_convert_ldouble_matrix_coo),
    cmocka_unit_test(test_convert_ldouble_matrix_zero_null_callback_matches_convert_ldouble_matrix_csr),
    cmocka_unit_test(test_convert_ldouble_matrix_zero_null_callback_matches_convert_ldouble_matrix_csc),

    cmocka_unit_test(test_convert_ldouble_matrix_zero_treats_negative_zero_as_zero_in_coo),
    cmocka_unit_test(test_convert_ldouble_matrix_zero_treats_negative_zero_as_zero_in_csr),
};

const size_t test_ldouble_matrix_count =
    sizeof(test_ldouble_matrix) / sizeof(test_ldouble_matrix[0]);
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Helpers
// ================================================================================

static uint8_matrix_t* _make_dense_uint8_matrix(size_t rows, size_t cols) {
    allocator_vtable_t a = heap_allocator();
    uint8_matrix_expect_t r = init_uint8_dense_matrix(rows, cols, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

static uint8_matrix_t* _make_coo_uint8_matrix(size_t rows,
                                              size_t cols,
                                              size_t cap,
                                              bool   growth) {
    allocator_vtable_t a = heap_allocator();
    uint8_matrix_expect_t r = init_uint8_coo_matrix(rows, cols, cap, growth, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

static uint8_matrix_t* _make_sample_dense_uint8_matrix(void) {
    uint8_matrix_t* mat = _make_dense_uint8_matrix(3u, 4u);

    assert_int_equal(set_uint8_matrix(mat, 0u, 1u, (uint8_t)10u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(mat, 1u, 3u, (uint8_t)20u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(mat, 2u, 0u, (uint8_t)30u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(mat, 2u, 2u, (uint8_t)40u), NO_ERROR);

    return mat;
}

static uint8_matrix_t* _make_sample_coo_uint8_matrix(void) {
    uint8_matrix_t* mat = _make_coo_uint8_matrix(3u, 4u, 8u, true);

    assert_int_equal(set_uint8_matrix(mat, 0u, 1u, (uint8_t)10u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(mat, 1u, 3u, (uint8_t)20u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(mat, 2u, 0u, (uint8_t)30u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(mat, 2u, 2u, (uint8_t)40u), NO_ERROR);

    return mat;
}

static bool _uint8_equal_mod_10(uint8_t a, uint8_t b) {
    return (uint8_t)(a % 10u) == (uint8_t)(b % 10u);
}

static bool _uint8_zero_or_ff(uint8_t v) {
    return (v == (uint8_t)0u) || (v == (uint8_t)255u);
}

// ================================================================================
// Group 1: init_uint8_dense_matrix
// ================================================================================

static void test_uint8_dense_init_zero_rows_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint8_matrix_expect_t r = init_uint8_dense_matrix(0u, 4u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_uint8_dense_init_zero_cols_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint8_matrix_expect_t r = init_uint8_dense_matrix(3u, 0u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_uint8_dense_init_returns_valid_matrix(void** state) {
    (void)state;

    uint8_matrix_t* mat = _make_dense_uint8_matrix(3u, 4u);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)UINT8_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(uint8_t));
    assert_int_equal((int)matrix_format(mat), (int)DENSE_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 12);

    return_uint8_matrix(mat);
}

static void test_uint8_dense_init_is_zero_initialized(void** state) {
    (void)state;

    uint8_matrix_t* mat = _make_dense_uint8_matrix(2u, 3u);
    uint8_t out = 255u;

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = 255u;
            assert_int_equal(get_uint8_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 0);
        }
    }

    assert_true(uint8_matrix_is_zero(mat));
    return_uint8_matrix(mat);
}

// ================================================================================
// Group 2: init_uint8_coo_matrix
// ================================================================================

static void test_uint8_coo_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint8_matrix_expect_t r = init_uint8_coo_matrix(3u, 4u, 0u, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_uint8_coo_init_returns_valid_matrix(void** state) {
    (void)state;

    uint8_matrix_t* mat = _make_coo_uint8_matrix(3u, 4u, 8u, true);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)UINT8_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(uint8_t));
    assert_int_equal((int)matrix_format(mat), (int)COO_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 0);
    assert_true(mat->rep.coo.growth);
    assert_true(mat->rep.coo.sorted);

    return_uint8_matrix(mat);
}

static void test_uint8_coo_unset_entry_reads_back_as_zero(void** state) {
    (void)state;

    uint8_matrix_t* mat = _make_coo_uint8_matrix(3u, 4u, 8u, true);
    uint8_t out = 255u;

    assert_int_equal(get_uint8_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_uint8_matrix(mat);
}

// ================================================================================
// Group 3: dense set/get
// ================================================================================

static void test_uint8_set_dense_null_matrix_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_uint8_matrix(NULL, 0u, 0u, (uint8_t)7u), NULL_POINTER);
}

static void test_uint8_get_dense_null_out_returns_null_pointer(void** state) {
    (void)state;
    uint8_matrix_t* mat = _make_dense_uint8_matrix(2u, 2u);

    assert_int_equal(get_uint8_matrix(mat, 0u, 0u, NULL), NULL_POINTER);
    return_uint8_matrix(mat);
}

static void test_uint8_set_get_dense_single_value(void** state) {
    (void)state;
    uint8_matrix_t* mat = _make_dense_uint8_matrix(3u, 3u);

    uint8_t out = 0u;
    assert_int_equal(set_uint8_matrix(mat, 1u, 2u, (uint8_t)42u), NO_ERROR);
    assert_int_equal(get_uint8_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 42);

    return_uint8_matrix(mat);
}

static void test_uint8_set_dense_overwrites_existing_value(void** state) {
    (void)state;
    uint8_matrix_t* mat = _make_dense_uint8_matrix(2u, 2u);

    uint8_t out = 0u;
    assert_int_equal(set_uint8_matrix(mat, 1u, 1u, (uint8_t)5u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(mat, 1u, 1u, (uint8_t)99u), NO_ERROR);
    assert_int_equal(get_uint8_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 99);

    return_uint8_matrix(mat);
}

// ================================================================================
// Group 4: COO set/get
// ================================================================================

static void test_uint8_set_get_coo_single_value(void** state) {
    (void)state;
    uint8_matrix_t* mat = _make_coo_uint8_matrix(3u, 3u, 4u, true);

    uint8_t out = 0u;
    assert_int_equal(set_uint8_matrix(mat, 2u, 1u, (uint8_t)123u), NO_ERROR);
    assert_int_equal(get_uint8_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 123);
    assert_int_equal((int)matrix_nnz(mat), 1);

    return_uint8_matrix(mat);
}

static void test_uint8_push_back_coo_overwrite_does_not_increase_nnz(void** state) {
    (void)state;
    uint8_matrix_t* mat = _make_coo_uint8_matrix(3u, 3u, 4u, true);

    uint8_t out = 0u;
    assert_int_equal(push_back_uint8_coo_matrix(mat, 1u, 1u, (uint8_t)5u), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(push_back_uint8_coo_matrix(mat, 1u, 1u, (uint8_t)99u), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(get_uint8_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 99);

    return_uint8_matrix(mat);
}

static void test_uint8_push_back_coo_capacity_overflow_when_growth_disabled(void** state) {
    (void)state;
    uint8_matrix_t* mat = _make_coo_uint8_matrix(3u, 3u, 2u, false);

    assert_int_equal(push_back_uint8_coo_matrix(mat, 0u, 0u, (uint8_t)1u), NO_ERROR);
    assert_int_equal(push_back_uint8_coo_matrix(mat, 1u, 1u, (uint8_t)2u), NO_ERROR);
    assert_int_equal(push_back_uint8_coo_matrix(mat, 2u, 2u, (uint8_t)3u), CAPACITY_OVERFLOW);

    return_uint8_matrix(mat);
}

// ================================================================================
// Group 5: clear / copy
// ================================================================================

static void test_clear_uint8_dense_zeroes_all_entries(void** state) {
    (void)state;
    uint8_matrix_t* mat = _make_dense_uint8_matrix(2u, 3u);
    uint8_t out = 255u;

    assert_int_equal(set_uint8_matrix(mat, 0u, 0u, (uint8_t)11u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(mat, 0u, 2u, (uint8_t)22u), NO_ERROR);
    assert_false(uint8_matrix_is_zero(mat));

    assert_int_equal(clear_uint8_matrix(mat), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = 255u;
            assert_int_equal(get_uint8_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 0);
        }
    }

    assert_true(uint8_matrix_is_zero(mat));
    return_uint8_matrix(mat);
}

static void test_copy_uint8_dense_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_matrix_t* src = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t r = copy_uint8_matrix(src, a);
    assert_true(r.has_value);
    assert_true(uint8_matrix_equal(src, r.u.value));

    return_uint8_matrix(r.u.value);
    return_uint8_matrix(src);
}

static void test_copy_uint8_coo_is_independent_of_source(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_matrix_t* src = _make_coo_uint8_matrix(3u, 3u, 4u, true);

    assert_int_equal(set_uint8_matrix(src, 1u, 1u, (uint8_t)5u), NO_ERROR);

    uint8_matrix_expect_t r = copy_uint8_matrix(src, a);
    assert_true(r.has_value);

    assert_int_equal(set_uint8_matrix(r.u.value, 1u, 1u, (uint8_t)77u), NO_ERROR);

    uint8_t src_out = 0u;
    uint8_t dst_out = 0u;

    assert_int_equal(get_uint8_matrix(src, 1u, 1u, &src_out), NO_ERROR);
    assert_int_equal(get_uint8_matrix(r.u.value, 1u, 1u, &dst_out), NO_ERROR);

    assert_int_equal((int)src_out, 5);
    assert_int_equal((int)dst_out, 77);

    return_uint8_matrix(r.u.value);
    return_uint8_matrix(src);
}

// ================================================================================
// Group 6: convert / convert_zero
// ================================================================================

static void test_convert_uint8_dense_to_coo_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_matrix_t* src = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t r = convert_uint8_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)COO_MATRIX);
    assert_true(uint8_matrix_equal(src, r.u.value));

    return_uint8_matrix(r.u.value);
    return_uint8_matrix(src);
}

static void test_convert_uint8_dense_to_csr_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_matrix_t* src = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t r = convert_uint8_matrix(src, CSR_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)CSR_MATRIX);
    assert_true(uint8_matrix_equal(src, r.u.value));

    return_uint8_matrix(r.u.value);
    return_uint8_matrix(src);
}

static void test_convert_uint8_zero_dense_to_csr_omits_ff(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_matrix_t* src = _make_dense_uint8_matrix(2u, 3u);

    assert_int_equal(set_uint8_matrix(src, 0u, 0u, (uint8_t)5u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(src, 0u, 1u, (uint8_t)255u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(src, 1u, 2u, (uint8_t)8u), NO_ERROR);

    uint8_matrix_expect_t r =
        convert_uint8_matrix_zero(src, CSR_MATRIX, a, _uint8_zero_or_ff);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_nnz(r.u.value), 2);

    return_uint8_matrix(r.u.value);
    return_uint8_matrix(src);
}

// ================================================================================
// Group 7: transpose / fill
// ================================================================================

static void test_transpose_uint8_dense_moves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint8_matrix_t* src = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t r = transpose_uint8_matrix(src, a);
    assert_true(r.has_value);

    uint8_t out = 0u;
    assert_int_equal(get_uint8_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_uint8_matrix(r.u.value, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    return_uint8_matrix(r.u.value);
    return_uint8_matrix(src);
}

static void test_fill_uint8_dense_sets_every_entry(void** state) {
    (void)state;
    uint8_matrix_t* mat = _make_dense_uint8_matrix(2u, 3u);
    uint8_t out = 0u;

    assert_int_equal(fill_uint8_matrix(mat, (uint8_t)7u), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            assert_int_equal(get_uint8_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 7);
        }
    }

    return_uint8_matrix(mat);
}

static void test_fill_uint8_zero_equivalent_to_clear(void** state) {
    (void)state;
    uint8_matrix_t* mat = _make_dense_uint8_matrix(2u, 2u);

    assert_int_equal(set_uint8_matrix(mat, 1u, 1u, (uint8_t)9u), NO_ERROR);
    assert_false(uint8_matrix_is_zero(mat));

    assert_int_equal(fill_uint8_matrix(mat, (uint8_t)0u), NO_ERROR);
    assert_true(uint8_matrix_is_zero(mat));

    return_uint8_matrix(mat);
}

// ================================================================================
// Group 8: equality / compatibility
// ================================================================================

static void test_uint8_matrix_equal_dense_same_values_returns_true(void** state) {
    (void)state;

    uint8_matrix_t* a = _make_sample_dense_uint8_matrix();
    uint8_matrix_t* b = _make_sample_dense_uint8_matrix();

    assert_true(uint8_matrix_equal(a, b));

    return_uint8_matrix(a);
    return_uint8_matrix(b);
}

static void test_uint8_matrix_shape_compatibility_helpers(void** state) {
    (void)state;

    uint8_matrix_t* a = _make_dense_uint8_matrix(2u, 3u);
    uint8_matrix_t* b = _make_dense_uint8_matrix(2u, 3u);
    uint8_matrix_t* c = _make_dense_uint8_matrix(3u, 4u);

    assert_true(uint8_matrix_has_same_shape(a, b));
    assert_true(uint8_matrix_is_add_compatible(a, b));
    assert_true(uint8_matrix_is_multiply_compatible(a, c));
    assert_false(uint8_matrix_is_square(a));

    return_uint8_matrix(a);
    return_uint8_matrix(b);
    return_uint8_matrix(c);
}

// ================================================================================
// Group 9: swaps / constructors / vector queries
// ================================================================================

static void test_swap_uint8_matrix_rows_dense(void** state) {
    (void)state;
    uint8_matrix_t* mat = _make_dense_uint8_matrix(2u, 2u);
    uint8_t out = 0u;

    assert_int_equal(set_uint8_matrix(mat, 0u, 0u, (uint8_t)1u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(mat, 1u, 0u, (uint8_t)2u), NO_ERROR);

    assert_int_equal(swap_uint8_matrix_rows(mat, 0u, 1u), NO_ERROR);

    assert_int_equal(get_uint8_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 2);

    assert_int_equal(get_uint8_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    return_uint8_matrix(mat);
}

static void test_init_uint8_identity_matrix_sets_diagonal_to_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint8_matrix_expect_t r = init_uint8_identity_matrix(3u, a);
    assert_true(r.has_value);

    uint8_t out = 0u;
    assert_int_equal(get_uint8_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    assert_int_equal(get_uint8_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_uint8_matrix(r.u.value);
}

static void test_uint8_row_and_col_vector_queries(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint8_matrix_expect_t row = init_uint8_row_vector(5u, a);
    uint8_matrix_expect_t col = init_uint8_col_vector(7u, a);

    assert_true(row.has_value);
    assert_true(col.has_value);

    assert_true(uint8_matrix_is_row_vector(row.u.value));
    assert_true(uint8_matrix_is_vector(row.u.value));
    assert_int_equal((int)uint8_matrix_vector_length(row.u.value), 5);

    assert_true(uint8_matrix_is_col_vector(col.u.value));
    assert_true(uint8_matrix_is_vector(col.u.value));
    assert_int_equal((int)uint8_matrix_vector_length(col.u.value), 7);

    return_uint8_matrix(row.u.value);
    return_uint8_matrix(col.u.value);
}

// ================================================================================
// Group 10: Additional uint8 matrix tests
// ================================================================================

static void test_transpose_uint8_coo_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint8_matrix_t* src = _make_sample_coo_uint8_matrix();

    uint8_matrix_expect_t r = transpose_uint8_matrix(src, alloc);
    assert_true(r.has_value);

    uint8_matrix_t* tr = r.u.value;
    uint8_t out = 0u;

    assert_int_equal((int)matrix_format(tr), (int)COO_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_uint8_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_uint8_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_uint8_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_uint8_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_uint8_matrix(tr);
    return_uint8_matrix(src);
}

static void test_transpose_uint8_csr_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint8_matrix_t* dense = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t src_r = convert_uint8_matrix(dense, CSR_MATRIX, alloc);
    assert_true(src_r.has_value);

    uint8_matrix_expect_t tr_r = transpose_uint8_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    uint8_matrix_t* tr = tr_r.u.value;
    uint8_t out = 0u;

    assert_int_equal((int)matrix_format(tr), (int)CSR_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_uint8_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_uint8_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_uint8_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_uint8_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_uint8_matrix(tr);
    return_uint8_matrix(src_r.u.value);
    return_uint8_matrix(dense);
}

static void test_transpose_uint8_csc_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint8_matrix_t* dense = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t src_r = convert_uint8_matrix(dense, CSC_MATRIX, alloc);
    assert_true(src_r.has_value);

    uint8_matrix_expect_t tr_r = transpose_uint8_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    uint8_matrix_t* tr = tr_r.u.value;
    uint8_t out = 0u;

    assert_int_equal((int)matrix_format(tr), (int)CSC_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_uint8_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_uint8_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_uint8_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_uint8_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_uint8_matrix(tr);
    return_uint8_matrix(src_r.u.value);
    return_uint8_matrix(dense);
}

static void test_swap_uint8_matrix_cols_dense(void** state) {
    (void)state;

    uint8_matrix_t* mat = _make_dense_uint8_matrix(2u, 3u);
    uint8_t out = 0u;

    assert_int_equal(set_uint8_matrix(mat, 0u, 0u, (uint8_t)1u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(mat, 0u, 2u, (uint8_t)3u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(mat, 1u, 0u, (uint8_t)4u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(mat, 1u, 2u, (uint8_t)6u), NO_ERROR);

    assert_int_equal(swap_uint8_matrix_cols(mat, 0u, 2u), NO_ERROR);

    assert_int_equal(get_uint8_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 3);

    assert_int_equal(get_uint8_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    assert_int_equal(get_uint8_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 6);

    assert_int_equal(get_uint8_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 4);

    return_uint8_matrix(mat);
}

static void test_uint8_matrix_is_sparse_dense_returns_false(void** state) {
    (void)state;

    uint8_matrix_t* mat = _make_dense_uint8_matrix(3u, 3u);
    assert_false(uint8_matrix_is_sparse(mat));

    return_uint8_matrix(mat);
}

static void test_uint8_matrix_is_sparse_coo_returns_true(void** state) {
    (void)state;

    uint8_matrix_t* mat = _make_coo_uint8_matrix(3u, 3u, 4u, true);
    assert_true(uint8_matrix_is_sparse(mat));

    return_uint8_matrix(mat);
}

static void test_uint8_matrix_is_sparse_csr_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint8_matrix_t* dense = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t r = convert_uint8_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(uint8_matrix_is_sparse(r.u.value));

    return_uint8_matrix(r.u.value);
    return_uint8_matrix(dense);
}

static void test_uint8_matrix_is_sparse_csc_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint8_matrix_t* dense = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t r = convert_uint8_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(uint8_matrix_is_sparse(r.u.value));

    return_uint8_matrix(r.u.value);
    return_uint8_matrix(dense);
}

static void test_uint8_matrix_equal_dense_and_coo_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint8_matrix_t* dense = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t r = convert_uint8_matrix(dense, COO_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(uint8_matrix_equal(dense, r.u.value));
    assert_true(uint8_matrix_equal(r.u.value, dense));

    return_uint8_matrix(r.u.value);
    return_uint8_matrix(dense);
}

static void test_uint8_matrix_equal_dense_and_csr_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint8_matrix_t* dense = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t r = convert_uint8_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(uint8_matrix_equal(dense, r.u.value));
    assert_true(uint8_matrix_equal(r.u.value, dense));

    return_uint8_matrix(r.u.value);
    return_uint8_matrix(dense);
}

static void test_uint8_matrix_equal_dense_and_csc_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint8_matrix_t* dense = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t r = convert_uint8_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(uint8_matrix_equal(dense, r.u.value));
    assert_true(uint8_matrix_equal(r.u.value, dense));

    return_uint8_matrix(r.u.value);
    return_uint8_matrix(dense);
}

static void test_convert_uint8_matrix_zero_null_callback_matches_convert_uint8_matrix_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint8_matrix_t* src = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t r1 = convert_uint8_matrix(src, COO_MATRIX, alloc);
    uint8_matrix_expect_t r2 = convert_uint8_matrix_zero(src, COO_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(uint8_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_uint8_matrix(r2.u.value);
    return_uint8_matrix(r1.u.value);
    return_uint8_matrix(src);
}

static void test_convert_uint8_matrix_zero_null_callback_matches_convert_uint8_matrix_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint8_matrix_t* src = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t r1 = convert_uint8_matrix(src, CSR_MATRIX, alloc);
    uint8_matrix_expect_t r2 = convert_uint8_matrix_zero(src, CSR_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(uint8_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_uint8_matrix(r2.u.value);
    return_uint8_matrix(r1.u.value);
    return_uint8_matrix(src);
}

static void test_convert_uint8_matrix_zero_null_callback_matches_convert_uint8_matrix_csc(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint8_matrix_t* src = _make_sample_dense_uint8_matrix();

    uint8_matrix_expect_t r1 = convert_uint8_matrix(src, CSC_MATRIX, alloc);
    uint8_matrix_expect_t r2 = convert_uint8_matrix_zero(src, CSC_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(uint8_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_uint8_matrix(r2.u.value);
    return_uint8_matrix(r1.u.value);
    return_uint8_matrix(src);
}

static void test_convert_uint8_matrix_zero_default_omits_zero_in_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint8_matrix_t* src = _make_dense_uint8_matrix(2u, 2u);

    assert_int_equal(set_uint8_matrix(src, 0u, 0u, (uint8_t)0u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(src, 1u, 1u, (uint8_t)1u), NO_ERROR);

    uint8_matrix_expect_t r = convert_uint8_matrix_zero(src, COO_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    uint8_t out = 123u;
    assert_int_equal(get_uint8_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    assert_int_equal(get_uint8_matrix(r.u.value, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    return_uint8_matrix(r.u.value);
    return_uint8_matrix(src);
}

static void test_convert_uint8_matrix_zero_default_omits_zero_in_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint8_matrix_t* src = _make_dense_uint8_matrix(2u, 2u);

    assert_int_equal(set_uint8_matrix(src, 0u, 1u, (uint8_t)0u), NO_ERROR);
    assert_int_equal(set_uint8_matrix(src, 1u, 0u, (uint8_t)2u), NO_ERROR);

    uint8_matrix_expect_t r = convert_uint8_matrix_zero(src, CSR_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    uint8_t out = 0u;
    assert_int_equal(get_uint8_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    assert_int_equal(get_uint8_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 2);

    return_uint8_matrix(r.u.value);
    return_uint8_matrix(src);
}

// ================================================================================
// Registry
// ================================================================================

const struct CMUnitTest test_uint8_matrix[] = {
    /* Group 1 */
    cmocka_unit_test(test_uint8_dense_init_zero_rows_fails),
    cmocka_unit_test(test_uint8_dense_init_zero_cols_fails),
    cmocka_unit_test(test_uint8_dense_init_returns_valid_matrix),
    cmocka_unit_test(test_uint8_dense_init_is_zero_initialized),

    /* Group 2 */
    cmocka_unit_test(test_uint8_coo_init_zero_capacity_fails),
    cmocka_unit_test(test_uint8_coo_init_returns_valid_matrix),
    cmocka_unit_test(test_uint8_coo_unset_entry_reads_back_as_zero),

    /* Group 3 */
    cmocka_unit_test(test_uint8_set_dense_null_matrix_returns_null_pointer),
    cmocka_unit_test(test_uint8_get_dense_null_out_returns_null_pointer),
    cmocka_unit_test(test_uint8_set_get_dense_single_value),
    cmocka_unit_test(test_uint8_set_dense_overwrites_existing_value),

    /* Group 4 */
    cmocka_unit_test(test_uint8_set_get_coo_single_value),
    cmocka_unit_test(test_uint8_push_back_coo_overwrite_does_not_increase_nnz),
    cmocka_unit_test(test_uint8_push_back_coo_capacity_overflow_when_growth_disabled),

    /* Group 5 */
    cmocka_unit_test(test_clear_uint8_dense_zeroes_all_entries),
    cmocka_unit_test(test_copy_uint8_dense_preserves_values),
    cmocka_unit_test(test_copy_uint8_coo_is_independent_of_source),

    /* Group 6 */
    cmocka_unit_test(test_convert_uint8_dense_to_coo_preserves_values),
    cmocka_unit_test(test_convert_uint8_dense_to_csr_preserves_values),
    cmocka_unit_test(test_convert_uint8_zero_dense_to_csr_omits_ff),

    /* Group 7 */
    cmocka_unit_test(test_transpose_uint8_dense_moves_values),
    cmocka_unit_test(test_fill_uint8_dense_sets_every_entry),
    cmocka_unit_test(test_fill_uint8_zero_equivalent_to_clear),

    /* Group 8 */
    cmocka_unit_test(test_uint8_matrix_equal_dense_same_values_returns_true),
    cmocka_unit_test(test_uint8_matrix_shape_compatibility_helpers),

    /* Group 9 */
    cmocka_unit_test(test_swap_uint8_matrix_rows_dense),
    cmocka_unit_test(test_init_uint8_identity_matrix_sets_diagonal_to_one),
    cmocka_unit_test(test_uint8_row_and_col_vector_queries),

    /* Group 10 */
    cmocka_unit_test(test_transpose_uint8_coo_preserves_logical_values),
    cmocka_unit_test(test_transpose_uint8_csr_preserves_logical_values),
    cmocka_unit_test(test_transpose_uint8_csc_preserves_logical_values),

    cmocka_unit_test(test_swap_uint8_matrix_cols_dense),

    cmocka_unit_test(test_uint8_matrix_is_sparse_dense_returns_false),
    cmocka_unit_test(test_uint8_matrix_is_sparse_coo_returns_true),
    cmocka_unit_test(test_uint8_matrix_is_sparse_csr_returns_true),
    cmocka_unit_test(test_uint8_matrix_is_sparse_csc_returns_true),

    cmocka_unit_test(test_uint8_matrix_equal_dense_and_coo_same_values_returns_true),
    cmocka_unit_test(test_uint8_matrix_equal_dense_and_csr_same_values_returns_true),
    cmocka_unit_test(test_uint8_matrix_equal_dense_and_csc_same_values_returns_true),

    cmocka_unit_test(test_convert_uint8_matrix_zero_null_callback_matches_convert_uint8_matrix_coo),
    cmocka_unit_test(test_convert_uint8_matrix_zero_null_callback_matches_convert_uint8_matrix_csr),
    cmocka_unit_test(test_convert_uint8_matrix_zero_null_callback_matches_convert_uint8_matrix_csc),

    cmocka_unit_test(test_convert_uint8_matrix_zero_default_omits_zero_in_coo),
    cmocka_unit_test(test_convert_uint8_matrix_zero_default_omits_zero_in_csr),
};

const size_t test_uint8_matrix_count =
    sizeof(test_uint8_matrix) / sizeof(test_uint8_matrix[0]);
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Helpers
// ================================================================================

static int8_matrix_t* _make_dense_int8_matrix(size_t rows, size_t cols) {
    allocator_vtable_t a = heap_allocator();
    int8_matrix_expect_t r = init_int8_dense_matrix(rows, cols, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

// --------------------------------------------------------------------------------

static int8_matrix_t* _make_coo_int8_matrix(size_t rows,
                                            size_t cols,
                                            size_t cap,
                                            bool   growth) {
    allocator_vtable_t a = heap_allocator();
    int8_matrix_expect_t r = init_int8_coo_matrix(rows, cols, cap, growth, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

// --------------------------------------------------------------------------------

static int8_matrix_t* _make_sample_dense_int8_matrix(void) {
    int8_matrix_t* mat = _make_dense_int8_matrix(3u, 4u);

    assert_int_equal(set_int8_matrix(mat, 0u, 1u, (int8_t)10), NO_ERROR);
    assert_int_equal(set_int8_matrix(mat, 1u, 3u, (int8_t)20), NO_ERROR);
    assert_int_equal(set_int8_matrix(mat, 2u, 0u, (int8_t)30), NO_ERROR);
    assert_int_equal(set_int8_matrix(mat, 2u, 2u, (int8_t)40), NO_ERROR);

    return mat;
}

// --------------------------------------------------------------------------------

static int8_matrix_t* _make_sample_coo_int8_matrix(void) {
    int8_matrix_t* mat = _make_coo_int8_matrix(3u, 4u, 8u, true);

    assert_int_equal(set_int8_matrix(mat, 0u, 1u, (int8_t)10), NO_ERROR);
    assert_int_equal(set_int8_matrix(mat, 1u, 3u, (int8_t)20), NO_ERROR);
    assert_int_equal(set_int8_matrix(mat, 2u, 0u, (int8_t)30), NO_ERROR);
    assert_int_equal(set_int8_matrix(mat, 2u, 2u, (int8_t)40), NO_ERROR);

    return mat;
}

// --------------------------------------------------------------------------------

static bool _int8_equal_abs(int8_t a, int8_t b) {
    int aa = (a < 0) ? -(int)a : (int)a;
    int bb = (b < 0) ? -(int)b : (int)b;
    return aa == bb;
}

// --------------------------------------------------------------------------------

static bool _int8_zero_or_neg_one(int8_t v) {
    return (v == (int8_t)0) || (v == (int8_t)-1);
}

/* =============================================================================
 * Group 1: init_int8_dense_matrix
 * ========================================================================== */

static void test_int8_dense_init_zero_rows_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int8_matrix_expect_t r = init_int8_dense_matrix(0u, 4u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_int8_dense_init_zero_cols_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int8_matrix_expect_t r = init_int8_dense_matrix(3u, 0u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_int8_dense_init_returns_valid_matrix(void** state) {
    (void)state;

    int8_matrix_t* mat = _make_dense_int8_matrix(3u, 4u);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)INT8_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(int8_t));
    assert_int_equal((int)matrix_format(mat), (int)DENSE_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 12);

    return_int8_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int8_dense_init_is_zero_initialized(void** state) {
    (void)state;

    int8_matrix_t* mat = _make_dense_int8_matrix(2u, 3u);
    int8_t out = (int8_t)-99;

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = (int8_t)-99;
            assert_int_equal(get_int8_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 0);
        }
    }

    assert_true(int8_matrix_is_zero(mat));
    return_int8_matrix(mat);
}

/* =============================================================================
 * Group 2: init_int8_coo_matrix
 * ========================================================================== */

static void test_int8_coo_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int8_matrix_expect_t r = init_int8_coo_matrix(3u, 4u, 0u, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_int8_coo_init_returns_valid_matrix(void** state) {
    (void)state;

    int8_matrix_t* mat = _make_coo_int8_matrix(3u, 4u, 8u, true);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)INT8_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(int8_t));
    assert_int_equal((int)matrix_format(mat), (int)COO_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 0);
    assert_true(mat->rep.coo.growth);
    assert_true(mat->rep.coo.sorted);

    return_int8_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int8_coo_unset_entry_reads_back_as_zero(void** state) {
    (void)state;

    int8_matrix_t* mat = _make_coo_int8_matrix(3u, 4u, 8u, true);
    int8_t out = (int8_t)-99;

    assert_int_equal(get_int8_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_int8_matrix(mat);
}

/* =============================================================================
 * Group 3: dense set/get
 * ========================================================================== */

static void test_int8_set_dense_null_matrix_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_int8_matrix(NULL, 0u, 0u, (int8_t)7), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int8_get_dense_null_out_returns_null_pointer(void** state) {
    (void)state;
    int8_matrix_t* mat = _make_dense_int8_matrix(2u, 2u);

    assert_int_equal(get_int8_matrix(mat, 0u, 0u, NULL), NULL_POINTER);
    return_int8_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int8_set_get_dense_single_value(void** state) {
    (void)state;
    int8_matrix_t* mat = _make_dense_int8_matrix(3u, 3u);

    int8_t out = 0;
    assert_int_equal(set_int8_matrix(mat, 1u, 2u, (int8_t)42), NO_ERROR);
    assert_int_equal(get_int8_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 42);

    return_int8_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int8_set_dense_overwrites_existing_value(void** state) {
    (void)state;
    int8_matrix_t* mat = _make_dense_int8_matrix(2u, 2u);

    int8_t out = 0;
    assert_int_equal(set_int8_matrix(mat, 1u, 1u, (int8_t)5), NO_ERROR);
    assert_int_equal(set_int8_matrix(mat, 1u, 1u, (int8_t)-7), NO_ERROR);
    assert_int_equal(get_int8_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, -7);

    return_int8_matrix(mat);
}

/* =============================================================================
 * Group 4: COO set/get
 * ========================================================================== */

static void test_int8_set_get_coo_single_value(void** state) {
    (void)state;
    int8_matrix_t* mat = _make_coo_int8_matrix(3u, 3u, 4u, true);

    int8_t out = 0;
    assert_int_equal(set_int8_matrix(mat, 2u, 1u, (int8_t)-23), NO_ERROR);
    assert_int_equal(get_int8_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, -23);
    assert_int_equal((int)matrix_nnz(mat), 1);

    return_int8_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int8_push_back_coo_overwrite_does_not_increase_nnz(void** state) {
    (void)state;
    int8_matrix_t* mat = _make_coo_int8_matrix(3u, 3u, 4u, true);

    int8_t out = 0;
    assert_int_equal(push_back_int8_coo_matrix(mat, 1u, 1u, (int8_t)5), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(push_back_int8_coo_matrix(mat, 1u, 1u, (int8_t)-9), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(get_int8_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, -9);

    return_int8_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int8_push_back_coo_capacity_overflow_when_growth_disabled(void** state) {
    (void)state;
    int8_matrix_t* mat = _make_coo_int8_matrix(3u, 3u, 2u, false);

    assert_int_equal(push_back_int8_coo_matrix(mat, 0u, 0u, (int8_t)1), NO_ERROR);
    assert_int_equal(push_back_int8_coo_matrix(mat, 1u, 1u, (int8_t)-2), NO_ERROR);
    assert_int_equal(push_back_int8_coo_matrix(mat, 2u, 2u, (int8_t)3), CAPACITY_OVERFLOW);

    return_int8_matrix(mat);
}

/* =============================================================================
 * Group 5: clear / copy
 * ========================================================================== */

static void test_clear_int8_dense_zeroes_all_entries(void** state) {
    (void)state;
    int8_matrix_t* mat = _make_dense_int8_matrix(2u, 3u);
    int8_t out = (int8_t)-99;

    assert_int_equal(set_int8_matrix(mat, 0u, 0u, (int8_t)11), NO_ERROR);
    assert_int_equal(set_int8_matrix(mat, 0u, 2u, (int8_t)-22), NO_ERROR);
    assert_false(int8_matrix_is_zero(mat));

    assert_int_equal(clear_int8_matrix(mat), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = (int8_t)-99;
            assert_int_equal(get_int8_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 0);
        }
    }

    assert_true(int8_matrix_is_zero(mat));
    return_int8_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_copy_int8_dense_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_matrix_t* src = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t r = copy_int8_matrix(src, a);
    assert_true(r.has_value);
    assert_true(int8_matrix_equal(src, r.u.value));

    return_int8_matrix(r.u.value);
    return_int8_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_int8_coo_is_independent_of_source(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_matrix_t* src = _make_coo_int8_matrix(3u, 3u, 4u, true);

    assert_int_equal(set_int8_matrix(src, 1u, 1u, (int8_t)5), NO_ERROR);

    int8_matrix_expect_t r = copy_int8_matrix(src, a);
    assert_true(r.has_value);

    assert_int_equal(set_int8_matrix(r.u.value, 1u, 1u, (int8_t)-77), NO_ERROR);

    int8_t src_out = 0;
    int8_t dst_out = 0;

    assert_int_equal(get_int8_matrix(src, 1u, 1u, &src_out), NO_ERROR);
    assert_int_equal(get_int8_matrix(r.u.value, 1u, 1u, &dst_out), NO_ERROR);

    assert_int_equal((int)src_out, 5);
    assert_int_equal((int)dst_out, -77);

    return_int8_matrix(r.u.value);
    return_int8_matrix(src);
}

/* =============================================================================
 * Group 6: convert / convert_zero
 * ========================================================================== */

static void test_convert_int8_dense_to_coo_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_matrix_t* src = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t r = convert_int8_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)COO_MATRIX);
    assert_true(int8_matrix_equal(src, r.u.value));

    return_int8_matrix(r.u.value);
    return_int8_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int8_dense_to_csr_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_matrix_t* src = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t r = convert_int8_matrix(src, CSR_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)CSR_MATRIX);
    assert_true(int8_matrix_equal(src, r.u.value));

    return_int8_matrix(r.u.value);
    return_int8_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int8_zero_dense_to_csr_omits_neg_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_matrix_t* src = _make_dense_int8_matrix(2u, 3u);

    assert_int_equal(set_int8_matrix(src, 0u, 0u, (int8_t)5), NO_ERROR);
    assert_int_equal(set_int8_matrix(src, 0u, 1u, (int8_t)-1), NO_ERROR);
    assert_int_equal(set_int8_matrix(src, 1u, 2u, (int8_t)8), NO_ERROR);

    int8_matrix_expect_t r =
        convert_int8_matrix_zero(src, CSR_MATRIX, a, _int8_zero_or_neg_one);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_nnz(r.u.value), 2);

    return_int8_matrix(r.u.value);
    return_int8_matrix(src);
}

/* =============================================================================
 * Group 7: transpose / fill
 * ========================================================================== */

static void test_transpose_int8_dense_moves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int8_matrix_t* src = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t r = transpose_int8_matrix(src, a);
    assert_true(r.has_value);

    int8_t out = 0;
    assert_int_equal(get_int8_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_int8_matrix(r.u.value, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    return_int8_matrix(r.u.value);
    return_int8_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_fill_int8_dense_sets_every_entry(void** state) {
    (void)state;
    int8_matrix_t* mat = _make_dense_int8_matrix(2u, 3u);
    int8_t out = 0;

    assert_int_equal(fill_int8_matrix(mat, (int8_t)-7), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            assert_int_equal(get_int8_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, -7);
        }
    }

    return_int8_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_fill_int8_zero_equivalent_to_clear(void** state) {
    (void)state;
    int8_matrix_t* mat = _make_dense_int8_matrix(2u, 2u);

    assert_int_equal(set_int8_matrix(mat, 1u, 1u, (int8_t)9), NO_ERROR);
    assert_false(int8_matrix_is_zero(mat));

    assert_int_equal(fill_int8_matrix(mat, (int8_t)0), NO_ERROR);
    assert_true(int8_matrix_is_zero(mat));

    return_int8_matrix(mat);
}

/* =============================================================================
 * Group 8: equality / compatibility
 * ========================================================================== */

static void test_int8_matrix_equal_dense_same_values_returns_true(void** state) {
    (void)state;

    int8_matrix_t* a = _make_sample_dense_int8_matrix();
    int8_matrix_t* b = _make_sample_dense_int8_matrix();

    assert_true(int8_matrix_equal(a, b));

    return_int8_matrix(a);
    return_int8_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_int8_matrix_shape_compatibility_helpers(void** state) {
    (void)state;

    int8_matrix_t* a = _make_dense_int8_matrix(2u, 3u);
    int8_matrix_t* b = _make_dense_int8_matrix(2u, 3u);
    int8_matrix_t* c = _make_dense_int8_matrix(3u, 4u);

    assert_true(int8_matrix_has_same_shape(a, b));
    assert_true(int8_matrix_is_add_compatible(a, b));
    assert_true(int8_matrix_is_multiply_compatible(a, c));
    assert_false(int8_matrix_is_square(a));

    return_int8_matrix(a);
    return_int8_matrix(b);
    return_int8_matrix(c);
}

/* =============================================================================
 * Group 9: swaps / constructors / vector queries
 * ========================================================================== */

static void test_swap_int8_matrix_rows_dense(void** state) {
    (void)state;
    int8_matrix_t* mat = _make_dense_int8_matrix(2u, 2u);
    int8_t out = 0;

    assert_int_equal(set_int8_matrix(mat, 0u, 0u, (int8_t)1), NO_ERROR);
    assert_int_equal(set_int8_matrix(mat, 1u, 0u, (int8_t)-2), NO_ERROR);

    assert_int_equal(swap_int8_matrix_rows(mat, 0u, 1u), NO_ERROR);

    assert_int_equal(get_int8_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, -2);

    assert_int_equal(get_int8_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    return_int8_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_init_int8_identity_matrix_sets_diagonal_to_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int8_matrix_expect_t r = init_int8_identity_matrix(3u, a);
    assert_true(r.has_value);

    int8_t out = 0;
    assert_int_equal(get_int8_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    assert_int_equal(get_int8_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_int8_matrix(r.u.value);
}

// --------------------------------------------------------------------------------

static void test_int8_row_and_col_vector_queries(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int8_matrix_expect_t row = init_int8_row_vector(5u, a);
    int8_matrix_expect_t col = init_int8_col_vector(7u, a);

    assert_true(row.has_value);
    assert_true(col.has_value);

    assert_true(int8_matrix_is_row_vector(row.u.value));
    assert_true(int8_matrix_is_vector(row.u.value));
    assert_int_equal((int)int8_matrix_vector_length(row.u.value), 5);

    assert_true(int8_matrix_is_col_vector(col.u.value));
    assert_true(int8_matrix_is_vector(col.u.value));
    assert_int_equal((int)int8_matrix_vector_length(col.u.value), 7);

    return_int8_matrix(row.u.value);
    return_int8_matrix(col.u.value);
}

/* =============================================================================
 * Group 10: Additional int8 matrix tests
 * ========================================================================== */

static void test_transpose_int8_coo_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int8_matrix_t* src = _make_sample_coo_int8_matrix();

    int8_matrix_expect_t r = transpose_int8_matrix(src, alloc);
    assert_true(r.has_value);

    int8_matrix_t* tr = r.u.value;
    int8_t out = 0;

    assert_int_equal((int)matrix_format(tr), (int)COO_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_int8_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_int8_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_int8_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_int8_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_int8_matrix(tr);
    return_int8_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_transpose_int8_csr_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int8_matrix_t* dense = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t src_r = convert_int8_matrix(dense, CSR_MATRIX, alloc);
    assert_true(src_r.has_value);

    int8_matrix_expect_t tr_r = transpose_int8_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    int8_matrix_t* tr = tr_r.u.value;
    int8_t out = 0;

    assert_int_equal((int)matrix_format(tr), (int)CSR_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_int8_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_int8_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_int8_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_int8_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_int8_matrix(tr);
    return_int8_matrix(src_r.u.value);
    return_int8_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_transpose_int8_csc_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int8_matrix_t* dense = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t src_r = convert_int8_matrix(dense, CSC_MATRIX, alloc);
    assert_true(src_r.has_value);

    int8_matrix_expect_t tr_r = transpose_int8_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    int8_matrix_t* tr = tr_r.u.value;
    int8_t out = 0;

    assert_int_equal((int)matrix_format(tr), (int)CSC_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_int8_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_int8_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_int8_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_int8_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_int8_matrix(tr);
    return_int8_matrix(src_r.u.value);
    return_int8_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_swap_int8_matrix_cols_dense(void** state) {
    (void)state;

    int8_matrix_t* mat = _make_dense_int8_matrix(2u, 3u);
    int8_t out = 0;

    assert_int_equal(set_int8_matrix(mat, 0u, 0u, (int8_t)1), NO_ERROR);
    assert_int_equal(set_int8_matrix(mat, 0u, 2u, (int8_t)-3), NO_ERROR);
    assert_int_equal(set_int8_matrix(mat, 1u, 0u, (int8_t)4), NO_ERROR);
    assert_int_equal(set_int8_matrix(mat, 1u, 2u, (int8_t)-6), NO_ERROR);

    assert_int_equal(swap_int8_matrix_cols(mat, 0u, 2u), NO_ERROR);

    assert_int_equal(get_int8_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, -3);

    assert_int_equal(get_int8_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    assert_int_equal(get_int8_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, -6);

    assert_int_equal(get_int8_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 4);

    return_int8_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int8_matrix_is_sparse_dense_returns_false(void** state) {
    (void)state;

    int8_matrix_t* mat = _make_dense_int8_matrix(3u, 3u);
    assert_false(int8_matrix_is_sparse(mat));

    return_int8_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int8_matrix_is_sparse_coo_returns_true(void** state) {
    (void)state;

    int8_matrix_t* mat = _make_coo_int8_matrix(3u, 3u, 4u, true);
    assert_true(int8_matrix_is_sparse(mat));

    return_int8_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int8_matrix_is_sparse_csr_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int8_matrix_t* dense = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t r = convert_int8_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(int8_matrix_is_sparse(r.u.value));

    return_int8_matrix(r.u.value);
    return_int8_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_int8_matrix_is_sparse_csc_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int8_matrix_t* dense = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t r = convert_int8_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(int8_matrix_is_sparse(r.u.value));

    return_int8_matrix(r.u.value);
    return_int8_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_int8_matrix_equal_dense_and_coo_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int8_matrix_t* dense = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t r = convert_int8_matrix(dense, COO_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(int8_matrix_equal(dense, r.u.value));
    assert_true(int8_matrix_equal(r.u.value, dense));

    return_int8_matrix(r.u.value);
    return_int8_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_int8_matrix_equal_dense_and_csr_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int8_matrix_t* dense = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t r = convert_int8_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(int8_matrix_equal(dense, r.u.value));
    assert_true(int8_matrix_equal(r.u.value, dense));

    return_int8_matrix(r.u.value);
    return_int8_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_int8_matrix_equal_dense_and_csc_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int8_matrix_t* dense = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t r = convert_int8_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(int8_matrix_equal(dense, r.u.value));
    assert_true(int8_matrix_equal(r.u.value, dense));

    return_int8_matrix(r.u.value);
    return_int8_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_convert_int8_matrix_zero_null_callback_matches_convert_int8_matrix_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int8_matrix_t* src = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t r1 = convert_int8_matrix(src, COO_MATRIX, alloc);
    int8_matrix_expect_t r2 = convert_int8_matrix_zero(src, COO_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(int8_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_int8_matrix(r2.u.value);
    return_int8_matrix(r1.u.value);
    return_int8_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int8_matrix_zero_null_callback_matches_convert_int8_matrix_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int8_matrix_t* src = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t r1 = convert_int8_matrix(src, CSR_MATRIX, alloc);
    int8_matrix_expect_t r2 = convert_int8_matrix_zero(src, CSR_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(int8_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_int8_matrix(r2.u.value);
    return_int8_matrix(r1.u.value);
    return_int8_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int8_matrix_zero_null_callback_matches_convert_int8_matrix_csc(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int8_matrix_t* src = _make_sample_dense_int8_matrix();

    int8_matrix_expect_t r1 = convert_int8_matrix(src, CSC_MATRIX, alloc);
    int8_matrix_expect_t r2 = convert_int8_matrix_zero(src, CSC_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(int8_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_int8_matrix(r2.u.value);
    return_int8_matrix(r1.u.value);
    return_int8_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int8_matrix_zero_default_omits_zero_in_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int8_matrix_t* src = _make_dense_int8_matrix(2u, 2u);

    assert_int_equal(set_int8_matrix(src, 0u, 0u, (int8_t)0), NO_ERROR);
    assert_int_equal(set_int8_matrix(src, 1u, 1u, (int8_t)1), NO_ERROR);

    int8_matrix_expect_t r = convert_int8_matrix_zero(src, COO_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    int8_t out = (int8_t)99;
    assert_int_equal(get_int8_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    assert_int_equal(get_int8_matrix(r.u.value, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    return_int8_matrix(r.u.value);
    return_int8_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int8_matrix_zero_default_omits_zero_in_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int8_matrix_t* src = _make_dense_int8_matrix(2u, 2u);

    assert_int_equal(set_int8_matrix(src, 0u, 1u, (int8_t)0), NO_ERROR);
    assert_int_equal(set_int8_matrix(src, 1u, 0u, (int8_t)-2), NO_ERROR);

    int8_matrix_expect_t r = convert_int8_matrix_zero(src, CSR_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    int8_t out = 0;
    assert_int_equal(get_int8_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    assert_int_equal(get_int8_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, -2);

    return_int8_matrix(r.u.value);
    return_int8_matrix(src);
}

/* =============================================================================
 * Registry
 * ========================================================================== */

const struct CMUnitTest test_int8_matrix[] = {
    /* Group 1 */
    cmocka_unit_test(test_int8_dense_init_zero_rows_fails),
    cmocka_unit_test(test_int8_dense_init_zero_cols_fails),
    cmocka_unit_test(test_int8_dense_init_returns_valid_matrix),
    cmocka_unit_test(test_int8_dense_init_is_zero_initialized),

    /* Group 2 */
    cmocka_unit_test(test_int8_coo_init_zero_capacity_fails),
    cmocka_unit_test(test_int8_coo_init_returns_valid_matrix),
    cmocka_unit_test(test_int8_coo_unset_entry_reads_back_as_zero),

    /* Group 3 */
    cmocka_unit_test(test_int8_set_dense_null_matrix_returns_null_pointer),
    cmocka_unit_test(test_int8_get_dense_null_out_returns_null_pointer),
    cmocka_unit_test(test_int8_set_get_dense_single_value),
    cmocka_unit_test(test_int8_set_dense_overwrites_existing_value),

    /* Group 4 */
    cmocka_unit_test(test_int8_set_get_coo_single_value),
    cmocka_unit_test(test_int8_push_back_coo_overwrite_does_not_increase_nnz),
    cmocka_unit_test(test_int8_push_back_coo_capacity_overflow_when_growth_disabled),

    /* Group 5 */
    cmocka_unit_test(test_clear_int8_dense_zeroes_all_entries),
    cmocka_unit_test(test_copy_int8_dense_preserves_values),
    cmocka_unit_test(test_copy_int8_coo_is_independent_of_source),

    /* Group 6 */
    cmocka_unit_test(test_convert_int8_dense_to_coo_preserves_values),
    cmocka_unit_test(test_convert_int8_dense_to_csr_preserves_values),
    cmocka_unit_test(test_convert_int8_zero_dense_to_csr_omits_neg_one),

    /* Group 7 */
    cmocka_unit_test(test_transpose_int8_dense_moves_values),
    cmocka_unit_test(test_fill_int8_dense_sets_every_entry),
    cmocka_unit_test(test_fill_int8_zero_equivalent_to_clear),

    /* Group 8 */
    cmocka_unit_test(test_int8_matrix_equal_dense_same_values_returns_true),
    cmocka_unit_test(test_int8_matrix_shape_compatibility_helpers),

    /* Group 9 */
    cmocka_unit_test(test_swap_int8_matrix_rows_dense),
    cmocka_unit_test(test_init_int8_identity_matrix_sets_diagonal_to_one),
    cmocka_unit_test(test_int8_row_and_col_vector_queries),

    /* Group 10 */
    cmocka_unit_test(test_transpose_int8_coo_preserves_logical_values),
    cmocka_unit_test(test_transpose_int8_csr_preserves_logical_values),
    cmocka_unit_test(test_transpose_int8_csc_preserves_logical_values),

    cmocka_unit_test(test_swap_int8_matrix_cols_dense),

    cmocka_unit_test(test_int8_matrix_is_sparse_dense_returns_false),
    cmocka_unit_test(test_int8_matrix_is_sparse_coo_returns_true),
    cmocka_unit_test(test_int8_matrix_is_sparse_csr_returns_true),
    cmocka_unit_test(test_int8_matrix_is_sparse_csc_returns_true),

    cmocka_unit_test(test_int8_matrix_equal_dense_and_coo_same_values_returns_true),
    cmocka_unit_test(test_int8_matrix_equal_dense_and_csr_same_values_returns_true),
    cmocka_unit_test(test_int8_matrix_equal_dense_and_csc_same_values_returns_true),

    cmocka_unit_test(test_convert_int8_matrix_zero_null_callback_matches_convert_int8_matrix_coo),
    cmocka_unit_test(test_convert_int8_matrix_zero_null_callback_matches_convert_int8_matrix_csr),
    cmocka_unit_test(test_convert_int8_matrix_zero_null_callback_matches_convert_int8_matrix_csc),

    cmocka_unit_test(test_convert_int8_matrix_zero_default_omits_zero_in_coo),
    cmocka_unit_test(test_convert_int8_matrix_zero_default_omits_zero_in_csr),
};

const size_t test_int8_matrix_count =
    sizeof(test_int8_matrix) / sizeof(test_int8_matrix[0]);
// ================================================================================ 
// ================================================================================ 

static uint16_matrix_t* _make_dense_uint16_matrix(size_t rows, size_t cols) {
    allocator_vtable_t a = heap_allocator();
    uint16_matrix_expect_t r = init_uint16_dense_matrix(rows, cols, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

static uint16_matrix_t* _make_coo_uint16_matrix(size_t rows,
                                              size_t cols,
                                              size_t cap,
                                              bool   growth) {
    allocator_vtable_t a = heap_allocator();
    uint16_matrix_expect_t r = init_uint16_coo_matrix(rows, cols, cap, growth, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

static uint16_matrix_t* _make_sample_dense_uint16_matrix(void) {
    uint16_matrix_t* mat = _make_dense_uint16_matrix(3u, 4u);

    assert_int_equal(set_uint16_matrix(mat, 0u, 1u, (uint16_t)10u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(mat, 1u, 3u, (uint16_t)20u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(mat, 2u, 0u, (uint16_t)30u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(mat, 2u, 2u, (uint16_t)40u), NO_ERROR);

    return mat;
}

static uint16_matrix_t* _make_sample_coo_uint16_matrix(void) {
    uint16_matrix_t* mat = _make_coo_uint16_matrix(3u, 4u, 8u, true);

    assert_int_equal(set_uint16_matrix(mat, 0u, 1u, (uint16_t)10u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(mat, 1u, 3u, (uint16_t)20u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(mat, 2u, 0u, (uint16_t)30u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(mat, 2u, 2u, (uint16_t)40u), NO_ERROR);

    return mat;
}

static bool _uint16_equal_mod_10(uint16_t a, uint16_t b) {
    return (uint16_t)(a % 10u) == (uint16_t)(b % 10u);
}

static bool _uint16_zero_or_ff(uint16_t v) {
    return (v == (uint16_t)0u) || (v == (uint16_t)255u);
}

// ================================================================================
// Group 1: init_uint16_dense_matrix
// ================================================================================

static void test_uint16_dense_init_zero_rows_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint16_matrix_expect_t r = init_uint16_dense_matrix(0u, 4u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_uint16_dense_init_zero_cols_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint16_matrix_expect_t r = init_uint16_dense_matrix(3u, 0u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_uint16_dense_init_returns_valid_matrix(void** state) {
    (void)state;

    uint16_matrix_t* mat = _make_dense_uint16_matrix(3u, 4u);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)UINT16_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(uint16_t));
    assert_int_equal((int)matrix_format(mat), (int)DENSE_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 12);

    return_uint16_matrix(mat);
}

static void test_uint16_dense_init_is_zero_initialized(void** state) {
    (void)state;

    uint16_matrix_t* mat = _make_dense_uint16_matrix(2u, 3u);
    uint16_t out = 255u;

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = 255u;
            assert_int_equal(get_uint16_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 0);
        }
    }

    assert_true(uint16_matrix_is_zero(mat));
    return_uint16_matrix(mat);
}

// ================================================================================
// Group 2: init_uint16_coo_matrix
// ================================================================================

static void test_uint16_coo_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint16_matrix_expect_t r = init_uint16_coo_matrix(3u, 4u, 0u, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_uint16_coo_init_returns_valid_matrix(void** state) {
    (void)state;

    uint16_matrix_t* mat = _make_coo_uint16_matrix(3u, 4u, 8u, true);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)UINT16_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(uint16_t));
    assert_int_equal((int)matrix_format(mat), (int)COO_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 0);
    assert_true(mat->rep.coo.growth);
    assert_true(mat->rep.coo.sorted);

    return_uint16_matrix(mat);
}

static void test_uint16_coo_unset_entry_reads_back_as_zero(void** state) {
    (void)state;

    uint16_matrix_t* mat = _make_coo_uint16_matrix(3u, 4u, 8u, true);
    uint16_t out = 255u;

    assert_int_equal(get_uint16_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_uint16_matrix(mat);
}

// ================================================================================
// Group 3: dense set/get
// ================================================================================

static void test_uint16_set_dense_null_matrix_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_uint16_matrix(NULL, 0u, 0u, (uint16_t)7u), NULL_POINTER);
}

static void test_uint16_get_dense_null_out_returns_null_pointer(void** state) {
    (void)state;
    uint16_matrix_t* mat = _make_dense_uint16_matrix(2u, 2u);

    assert_int_equal(get_uint16_matrix(mat, 0u, 0u, NULL), NULL_POINTER);
    return_uint16_matrix(mat);
}

static void test_uint16_set_get_dense_single_value(void** state) {
    (void)state;
    uint16_matrix_t* mat = _make_dense_uint16_matrix(3u, 3u);

    uint16_t out = 0u;
    assert_int_equal(set_uint16_matrix(mat, 1u, 2u, (uint16_t)42u), NO_ERROR);
    assert_int_equal(get_uint16_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 42);

    return_uint16_matrix(mat);
}

static void test_uint16_set_dense_overwrites_existing_value(void** state) {
    (void)state;
    uint16_matrix_t* mat = _make_dense_uint16_matrix(2u, 2u);

    uint16_t out = 0u;
    assert_int_equal(set_uint16_matrix(mat, 1u, 1u, (uint16_t)5u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(mat, 1u, 1u, (uint16_t)99u), NO_ERROR);
    assert_int_equal(get_uint16_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 99);

    return_uint16_matrix(mat);
}

// ================================================================================
// Group 4: COO set/get
// ================================================================================

static void test_uint16_set_get_coo_single_value(void** state) {
    (void)state;
    uint16_matrix_t* mat = _make_coo_uint16_matrix(3u, 3u, 4u, true);

    uint16_t out = 0u;
    assert_int_equal(set_uint16_matrix(mat, 2u, 1u, (uint16_t)123u), NO_ERROR);
    assert_int_equal(get_uint16_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 123);
    assert_int_equal((int)matrix_nnz(mat), 1);

    return_uint16_matrix(mat);
}

static void test_uint16_push_back_coo_overwrite_does_not_increase_nnz(void** state) {
    (void)state;
    uint16_matrix_t* mat = _make_coo_uint16_matrix(3u, 3u, 4u, true);

    uint16_t out = 0u;
    assert_int_equal(push_back_uint16_coo_matrix(mat, 1u, 1u, (uint16_t)5u), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(push_back_uint16_coo_matrix(mat, 1u, 1u, (uint16_t)99u), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(get_uint16_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 99);

    return_uint16_matrix(mat);
}

static void test_uint16_push_back_coo_capacity_overflow_when_growth_disabled(void** state) {
    (void)state;
    uint16_matrix_t* mat = _make_coo_uint16_matrix(3u, 3u, 2u, false);

    assert_int_equal(push_back_uint16_coo_matrix(mat, 0u, 0u, (uint16_t)1u), NO_ERROR);
    assert_int_equal(push_back_uint16_coo_matrix(mat, 1u, 1u, (uint16_t)2u), NO_ERROR);
    assert_int_equal(push_back_uint16_coo_matrix(mat, 2u, 2u, (uint16_t)3u), CAPACITY_OVERFLOW);

    return_uint16_matrix(mat);
}

// ================================================================================
// Group 5: clear / copy
// ================================================================================

static void test_clear_uint16_dense_zeroes_all_entries(void** state) {
    (void)state;
    uint16_matrix_t* mat = _make_dense_uint16_matrix(2u, 3u);
    uint16_t out = 255u;

    assert_int_equal(set_uint16_matrix(mat, 0u, 0u, (uint16_t)11u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(mat, 0u, 2u, (uint16_t)22u), NO_ERROR);
    assert_false(uint16_matrix_is_zero(mat));

    assert_int_equal(clear_uint16_matrix(mat), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = 255u;
            assert_int_equal(get_uint16_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 0);
        }
    }

    assert_true(uint16_matrix_is_zero(mat));
    return_uint16_matrix(mat);
}

static void test_copy_uint16_dense_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_matrix_t* src = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t r = copy_uint16_matrix(src, a);
    assert_true(r.has_value);
    assert_true(uint16_matrix_equal(src, r.u.value));

    return_uint16_matrix(r.u.value);
    return_uint16_matrix(src);
}

static void test_copy_uint16_coo_is_independent_of_source(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_matrix_t* src = _make_coo_uint16_matrix(3u, 3u, 4u, true);

    assert_int_equal(set_uint16_matrix(src, 1u, 1u, (uint16_t)5u), NO_ERROR);

    uint16_matrix_expect_t r = copy_uint16_matrix(src, a);
    assert_true(r.has_value);

    assert_int_equal(set_uint16_matrix(r.u.value, 1u, 1u, (uint16_t)77u), NO_ERROR);

    uint16_t src_out = 0u;
    uint16_t dst_out = 0u;

    assert_int_equal(get_uint16_matrix(src, 1u, 1u, &src_out), NO_ERROR);
    assert_int_equal(get_uint16_matrix(r.u.value, 1u, 1u, &dst_out), NO_ERROR);

    assert_int_equal((int)src_out, 5);
    assert_int_equal((int)dst_out, 77);

    return_uint16_matrix(r.u.value);
    return_uint16_matrix(src);
}

// ================================================================================
// Group 6: convert / convert_zero
// ================================================================================

static void test_convert_uint16_dense_to_coo_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_matrix_t* src = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t r = convert_uint16_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)COO_MATRIX);
    assert_true(uint16_matrix_equal(src, r.u.value));

    return_uint16_matrix(r.u.value);
    return_uint16_matrix(src);
}

static void test_convert_uint16_dense_to_csr_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_matrix_t* src = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t r = convert_uint16_matrix(src, CSR_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)CSR_MATRIX);
    assert_true(uint16_matrix_equal(src, r.u.value));

    return_uint16_matrix(r.u.value);
    return_uint16_matrix(src);
}

static void test_convert_uint16_zero_dense_to_csr_omits_ff(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_matrix_t* src = _make_dense_uint16_matrix(2u, 3u);

    assert_int_equal(set_uint16_matrix(src, 0u, 0u, (uint16_t)5u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(src, 0u, 1u, (uint16_t)255u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(src, 1u, 2u, (uint16_t)8u), NO_ERROR);

    uint16_matrix_expect_t r =
        convert_uint16_matrix_zero(src, CSR_MATRIX, a, _uint16_zero_or_ff);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_nnz(r.u.value), 2);

    return_uint16_matrix(r.u.value);
    return_uint16_matrix(src);
}

// ================================================================================
// Group 7: transpose / fill
// ================================================================================

static void test_transpose_uint16_dense_moves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint16_matrix_t* src = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t r = transpose_uint16_matrix(src, a);
    assert_true(r.has_value);

    uint16_t out = 0u;
    assert_int_equal(get_uint16_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_uint16_matrix(r.u.value, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    return_uint16_matrix(r.u.value);
    return_uint16_matrix(src);
}

static void test_fill_uint16_dense_sets_every_entry(void** state) {
    (void)state;
    uint16_matrix_t* mat = _make_dense_uint16_matrix(2u, 3u);
    uint16_t out = 0u;

    assert_int_equal(fill_uint16_matrix(mat, (uint16_t)7u), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            assert_int_equal(get_uint16_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 7);
        }
    }

    return_uint16_matrix(mat);
}

static void test_fill_uint16_zero_equivalent_to_clear(void** state) {
    (void)state;
    uint16_matrix_t* mat = _make_dense_uint16_matrix(2u, 2u);

    assert_int_equal(set_uint16_matrix(mat, 1u, 1u, (uint16_t)9u), NO_ERROR);
    assert_false(uint16_matrix_is_zero(mat));

    assert_int_equal(fill_uint16_matrix(mat, (uint16_t)0u), NO_ERROR);
    assert_true(uint16_matrix_is_zero(mat));

    return_uint16_matrix(mat);
}

// ================================================================================
// Group 8: equality / compatibility
// ================================================================================

static void test_uint16_matrix_equal_dense_same_values_returns_true(void** state) {
    (void)state;

    uint16_matrix_t* a = _make_sample_dense_uint16_matrix();
    uint16_matrix_t* b = _make_sample_dense_uint16_matrix();

    assert_true(uint16_matrix_equal(a, b));

    return_uint16_matrix(a);
    return_uint16_matrix(b);
}

static void test_uint16_matrix_shape_compatibility_helpers(void** state) {
    (void)state;

    uint16_matrix_t* a = _make_dense_uint16_matrix(2u, 3u);
    uint16_matrix_t* b = _make_dense_uint16_matrix(2u, 3u);
    uint16_matrix_t* c = _make_dense_uint16_matrix(3u, 4u);

    assert_true(uint16_matrix_has_same_shape(a, b));
    assert_true(uint16_matrix_is_add_compatible(a, b));
    assert_true(uint16_matrix_is_multiply_compatible(a, c));
    assert_false(uint16_matrix_is_square(a));

    return_uint16_matrix(a);
    return_uint16_matrix(b);
    return_uint16_matrix(c);
}

// ================================================================================
// Group 9: swaps / constructors / vector queries
// ================================================================================

static void test_swap_uint16_matrix_rows_dense(void** state) {
    (void)state;
    uint16_matrix_t* mat = _make_dense_uint16_matrix(2u, 2u);
    uint16_t out = 0u;

    assert_int_equal(set_uint16_matrix(mat, 0u, 0u, (uint16_t)1u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(mat, 1u, 0u, (uint16_t)2u), NO_ERROR);

    assert_int_equal(swap_uint16_matrix_rows(mat, 0u, 1u), NO_ERROR);

    assert_int_equal(get_uint16_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 2);

    assert_int_equal(get_uint16_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    return_uint16_matrix(mat);
}

static void test_init_uint16_identity_matrix_sets_diagonal_to_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint16_matrix_expect_t r = init_uint16_identity_matrix(3u, a);
    assert_true(r.has_value);

    uint16_t out = 0u;
    assert_int_equal(get_uint16_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    assert_int_equal(get_uint16_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_uint16_matrix(r.u.value);
}

static void test_uint16_row_and_col_vector_queries(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint16_matrix_expect_t row = init_uint16_row_vector(5u, a);
    uint16_matrix_expect_t col = init_uint16_col_vector(7u, a);

    assert_true(row.has_value);
    assert_true(col.has_value);

    assert_true(uint16_matrix_is_row_vector(row.u.value));
    assert_true(uint16_matrix_is_vector(row.u.value));
    assert_int_equal((int)uint16_matrix_vector_length(row.u.value), 5);

    assert_true(uint16_matrix_is_col_vector(col.u.value));
    assert_true(uint16_matrix_is_vector(col.u.value));
    assert_int_equal((int)uint16_matrix_vector_length(col.u.value), 7);

    return_uint16_matrix(row.u.value);
    return_uint16_matrix(col.u.value);
}

// ================================================================================
// Group 10: Additional uint16 matrix tests
// ================================================================================

static void test_transpose_uint16_coo_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint16_matrix_t* src = _make_sample_coo_uint16_matrix();

    uint16_matrix_expect_t r = transpose_uint16_matrix(src, alloc);
    assert_true(r.has_value);

    uint16_matrix_t* tr = r.u.value;
    uint16_t out = 0u;

    assert_int_equal((int)matrix_format(tr), (int)COO_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_uint16_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_uint16_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_uint16_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_uint16_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_uint16_matrix(tr);
    return_uint16_matrix(src);
}

static void test_transpose_uint16_csr_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint16_matrix_t* dense = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t src_r = convert_uint16_matrix(dense, CSR_MATRIX, alloc);
    assert_true(src_r.has_value);

    uint16_matrix_expect_t tr_r = transpose_uint16_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    uint16_matrix_t* tr = tr_r.u.value;
    uint16_t out = 0u;

    assert_int_equal((int)matrix_format(tr), (int)CSR_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_uint16_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_uint16_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_uint16_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_uint16_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_uint16_matrix(tr);
    return_uint16_matrix(src_r.u.value);
    return_uint16_matrix(dense);
}

static void test_transpose_uint16_csc_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint16_matrix_t* dense = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t src_r = convert_uint16_matrix(dense, CSC_MATRIX, alloc);
    assert_true(src_r.has_value);

    uint16_matrix_expect_t tr_r = transpose_uint16_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    uint16_matrix_t* tr = tr_r.u.value;
    uint16_t out = 0u;

    assert_int_equal((int)matrix_format(tr), (int)CSC_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_uint16_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_uint16_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_uint16_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_uint16_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_uint16_matrix(tr);
    return_uint16_matrix(src_r.u.value);
    return_uint16_matrix(dense);
}

static void test_swap_uint16_matrix_cols_dense(void** state) {
    (void)state;

    uint16_matrix_t* mat = _make_dense_uint16_matrix(2u, 3u);
    uint16_t out = 0u;

    assert_int_equal(set_uint16_matrix(mat, 0u, 0u, (uint16_t)1u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(mat, 0u, 2u, (uint16_t)3u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(mat, 1u, 0u, (uint16_t)4u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(mat, 1u, 2u, (uint16_t)6u), NO_ERROR);

    assert_int_equal(swap_uint16_matrix_cols(mat, 0u, 2u), NO_ERROR);

    assert_int_equal(get_uint16_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 3);

    assert_int_equal(get_uint16_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    assert_int_equal(get_uint16_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 6);

    assert_int_equal(get_uint16_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 4);

    return_uint16_matrix(mat);
}

static void test_uint16_matrix_is_sparse_dense_returns_false(void** state) {
    (void)state;

    uint16_matrix_t* mat = _make_dense_uint16_matrix(3u, 3u);
    assert_false(uint16_matrix_is_sparse(mat));

    return_uint16_matrix(mat);
}

static void test_uint16_matrix_is_sparse_coo_returns_true(void** state) {
    (void)state;

    uint16_matrix_t* mat = _make_coo_uint16_matrix(3u, 3u, 4u, true);
    assert_true(uint16_matrix_is_sparse(mat));

    return_uint16_matrix(mat);
}

static void test_uint16_matrix_is_sparse_csr_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint16_matrix_t* dense = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t r = convert_uint16_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(uint16_matrix_is_sparse(r.u.value));

    return_uint16_matrix(r.u.value);
    return_uint16_matrix(dense);
}

static void test_uint16_matrix_is_sparse_csc_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint16_matrix_t* dense = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t r = convert_uint16_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(uint16_matrix_is_sparse(r.u.value));

    return_uint16_matrix(r.u.value);
    return_uint16_matrix(dense);
}

static void test_uint16_matrix_equal_dense_and_coo_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint16_matrix_t* dense = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t r = convert_uint16_matrix(dense, COO_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(uint16_matrix_equal(dense, r.u.value));
    assert_true(uint16_matrix_equal(r.u.value, dense));

    return_uint16_matrix(r.u.value);
    return_uint16_matrix(dense);
}

static void test_uint16_matrix_equal_dense_and_csr_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint16_matrix_t* dense = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t r = convert_uint16_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(uint16_matrix_equal(dense, r.u.value));
    assert_true(uint16_matrix_equal(r.u.value, dense));

    return_uint16_matrix(r.u.value);
    return_uint16_matrix(dense);
}

static void test_uint16_matrix_equal_dense_and_csc_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint16_matrix_t* dense = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t r = convert_uint16_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(uint16_matrix_equal(dense, r.u.value));
    assert_true(uint16_matrix_equal(r.u.value, dense));

    return_uint16_matrix(r.u.value);
    return_uint16_matrix(dense);
}

static void test_convert_uint16_matrix_zero_null_callback_matches_convert_uint16_matrix_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint16_matrix_t* src = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t r1 = convert_uint16_matrix(src, COO_MATRIX, alloc);
    uint16_matrix_expect_t r2 = convert_uint16_matrix_zero(src, COO_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(uint16_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_uint16_matrix(r2.u.value);
    return_uint16_matrix(r1.u.value);
    return_uint16_matrix(src);
}

static void test_convert_uint16_matrix_zero_null_callback_matches_convert_uint16_matrix_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint16_matrix_t* src = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t r1 = convert_uint16_matrix(src, CSR_MATRIX, alloc);
    uint16_matrix_expect_t r2 = convert_uint16_matrix_zero(src, CSR_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(uint16_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_uint16_matrix(r2.u.value);
    return_uint16_matrix(r1.u.value);
    return_uint16_matrix(src);
}

static void test_convert_uint16_matrix_zero_null_callback_matches_convert_uint16_matrix_csc(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint16_matrix_t* src = _make_sample_dense_uint16_matrix();

    uint16_matrix_expect_t r1 = convert_uint16_matrix(src, CSC_MATRIX, alloc);
    uint16_matrix_expect_t r2 = convert_uint16_matrix_zero(src, CSC_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(uint16_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_uint16_matrix(r2.u.value);
    return_uint16_matrix(r1.u.value);
    return_uint16_matrix(src);
}

static void test_convert_uint16_matrix_zero_default_omits_zero_in_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint16_matrix_t* src = _make_dense_uint16_matrix(2u, 2u);

    assert_int_equal(set_uint16_matrix(src, 0u, 0u, (uint16_t)0u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(src, 1u, 1u, (uint16_t)1u), NO_ERROR);

    uint16_matrix_expect_t r = convert_uint16_matrix_zero(src, COO_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    uint16_t out = 123u;
    assert_int_equal(get_uint16_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    assert_int_equal(get_uint16_matrix(r.u.value, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    return_uint16_matrix(r.u.value);
    return_uint16_matrix(src);
}

static void test_convert_uint16_matrix_zero_default_omits_zero_in_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint16_matrix_t* src = _make_dense_uint16_matrix(2u, 2u);

    assert_int_equal(set_uint16_matrix(src, 0u, 1u, (uint16_t)0u), NO_ERROR);
    assert_int_equal(set_uint16_matrix(src, 1u, 0u, (uint16_t)2u), NO_ERROR);

    uint16_matrix_expect_t r = convert_uint16_matrix_zero(src, CSR_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    uint16_t out = 0u;
    assert_int_equal(get_uint16_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    assert_int_equal(get_uint16_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 2);

    return_uint16_matrix(r.u.value);
    return_uint16_matrix(src);
}

// ================================================================================
// Registry
// ================================================================================

const struct CMUnitTest test_uint16_matrix[] = {
    /* Group 1 */
    cmocka_unit_test(test_uint16_dense_init_zero_rows_fails),
    cmocka_unit_test(test_uint16_dense_init_zero_cols_fails),
    cmocka_unit_test(test_uint16_dense_init_returns_valid_matrix),
    cmocka_unit_test(test_uint16_dense_init_is_zero_initialized),

    /* Group 2 */
    cmocka_unit_test(test_uint16_coo_init_zero_capacity_fails),
    cmocka_unit_test(test_uint16_coo_init_returns_valid_matrix),
    cmocka_unit_test(test_uint16_coo_unset_entry_reads_back_as_zero),

    /* Group 3 */
    cmocka_unit_test(test_uint16_set_dense_null_matrix_returns_null_pointer),
    cmocka_unit_test(test_uint16_get_dense_null_out_returns_null_pointer),
    cmocka_unit_test(test_uint16_set_get_dense_single_value),
    cmocka_unit_test(test_uint16_set_dense_overwrites_existing_value),

    /* Group 4 */
    cmocka_unit_test(test_uint16_set_get_coo_single_value),
    cmocka_unit_test(test_uint16_push_back_coo_overwrite_does_not_increase_nnz),
    cmocka_unit_test(test_uint16_push_back_coo_capacity_overflow_when_growth_disabled),

    /* Group 5 */
    cmocka_unit_test(test_clear_uint16_dense_zeroes_all_entries),
    cmocka_unit_test(test_copy_uint16_dense_preserves_values),
    cmocka_unit_test(test_copy_uint16_coo_is_independent_of_source),

    /* Group 6 */
    cmocka_unit_test(test_convert_uint16_dense_to_coo_preserves_values),
    cmocka_unit_test(test_convert_uint16_dense_to_csr_preserves_values),
    cmocka_unit_test(test_convert_uint16_zero_dense_to_csr_omits_ff),

    /* Group 7 */
    cmocka_unit_test(test_transpose_uint16_dense_moves_values),
    cmocka_unit_test(test_fill_uint16_dense_sets_every_entry),
    cmocka_unit_test(test_fill_uint16_zero_equivalent_to_clear),

    /* Group 8 */
    cmocka_unit_test(test_uint16_matrix_equal_dense_same_values_returns_true),
    cmocka_unit_test(test_uint16_matrix_shape_compatibility_helpers),

    /* Group 9 */
    cmocka_unit_test(test_swap_uint16_matrix_rows_dense),
    cmocka_unit_test(test_init_uint16_identity_matrix_sets_diagonal_to_one),
    cmocka_unit_test(test_uint16_row_and_col_vector_queries),

    /* Group 10 */
    cmocka_unit_test(test_transpose_uint16_coo_preserves_logical_values),
    cmocka_unit_test(test_transpose_uint16_csr_preserves_logical_values),
    cmocka_unit_test(test_transpose_uint16_csc_preserves_logical_values),

    cmocka_unit_test(test_swap_uint16_matrix_cols_dense),

    cmocka_unit_test(test_uint16_matrix_is_sparse_dense_returns_false),
    cmocka_unit_test(test_uint16_matrix_is_sparse_coo_returns_true),
    cmocka_unit_test(test_uint16_matrix_is_sparse_csr_returns_true),
    cmocka_unit_test(test_uint16_matrix_is_sparse_csc_returns_true),

    cmocka_unit_test(test_uint16_matrix_equal_dense_and_coo_same_values_returns_true),
    cmocka_unit_test(test_uint16_matrix_equal_dense_and_csr_same_values_returns_true),
    cmocka_unit_test(test_uint16_matrix_equal_dense_and_csc_same_values_returns_true),

    cmocka_unit_test(test_convert_uint16_matrix_zero_null_callback_matches_convert_uint16_matrix_coo),
    cmocka_unit_test(test_convert_uint16_matrix_zero_null_callback_matches_convert_uint16_matrix_csr),
    cmocka_unit_test(test_convert_uint16_matrix_zero_null_callback_matches_convert_uint16_matrix_csc),

    cmocka_unit_test(test_convert_uint16_matrix_zero_default_omits_zero_in_coo),
    cmocka_unit_test(test_convert_uint16_matrix_zero_default_omits_zero_in_csr),
};

const size_t test_uint16_matrix_count =
    sizeof(test_uint16_matrix) / sizeof(test_uint16_matrix[0]);
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Helpers
// ================================================================================

static int16_matrix_t* _make_dense_int16_matrix(size_t rows, size_t cols) {
    allocator_vtable_t a = heap_allocator();
    int16_matrix_expect_t r = init_int16_dense_matrix(rows, cols, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

// --------------------------------------------------------------------------------

static int16_matrix_t* _make_coo_int16_matrix(size_t rows,
                                            size_t cols,
                                            size_t cap,
                                            bool   growth) {
    allocator_vtable_t a = heap_allocator();
    int16_matrix_expect_t r = init_int16_coo_matrix(rows, cols, cap, growth, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

// --------------------------------------------------------------------------------

static int16_matrix_t* _make_sample_dense_int16_matrix(void) {
    int16_matrix_t* mat = _make_dense_int16_matrix(3u, 4u);

    assert_int_equal(set_int16_matrix(mat, 0u, 1u, (int16_t)10), NO_ERROR);
    assert_int_equal(set_int16_matrix(mat, 1u, 3u, (int16_t)20), NO_ERROR);
    assert_int_equal(set_int16_matrix(mat, 2u, 0u, (int16_t)30), NO_ERROR);
    assert_int_equal(set_int16_matrix(mat, 2u, 2u, (int16_t)40), NO_ERROR);

    return mat;
}

// --------------------------------------------------------------------------------

static int16_matrix_t* _make_sample_coo_int16_matrix(void) {
    int16_matrix_t* mat = _make_coo_int16_matrix(3u, 4u, 8u, true);

    assert_int_equal(set_int16_matrix(mat, 0u, 1u, (int16_t)10), NO_ERROR);
    assert_int_equal(set_int16_matrix(mat, 1u, 3u, (int16_t)20), NO_ERROR);
    assert_int_equal(set_int16_matrix(mat, 2u, 0u, (int16_t)30), NO_ERROR);
    assert_int_equal(set_int16_matrix(mat, 2u, 2u, (int16_t)40), NO_ERROR);

    return mat;
}

// --------------------------------------------------------------------------------

static bool _int16_equal_abs(int16_t a, int16_t b) {
    int aa = (a < 0) ? -(int)a : (int)a;
    int bb = (b < 0) ? -(int)b : (int)b;
    return aa == bb;
}

// --------------------------------------------------------------------------------

static bool _int16_zero_or_neg_one(int16_t v) {
    return (v == (int16_t)0) || (v == (int16_t)-1);
}

/* =============================================================================
 * Group 1: init_int16_dense_matrix
 * ========================================================================== */

static void test_int16_dense_init_zero_rows_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int16_matrix_expect_t r = init_int16_dense_matrix(0u, 4u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_int16_dense_init_zero_cols_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int16_matrix_expect_t r = init_int16_dense_matrix(3u, 0u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_int16_dense_init_returns_valid_matrix(void** state) {
    (void)state;

    int16_matrix_t* mat = _make_dense_int16_matrix(3u, 4u);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)INT16_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(int16_t));
    assert_int_equal((int)matrix_format(mat), (int)DENSE_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 12);

    return_int16_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int16_dense_init_is_zero_initialized(void** state) {
    (void)state;

    int16_matrix_t* mat = _make_dense_int16_matrix(2u, 3u);
    int16_t out = (int16_t)-99;

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = (int16_t)-99;
            assert_int_equal(get_int16_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 0);
        }
    }

    assert_true(int16_matrix_is_zero(mat));
    return_int16_matrix(mat);
}

/* =============================================================================
 * Group 2: init_int16_coo_matrix
 * ========================================================================== */

static void test_int16_coo_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int16_matrix_expect_t r = init_int16_coo_matrix(3u, 4u, 0u, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_int16_coo_init_returns_valid_matrix(void** state) {
    (void)state;

    int16_matrix_t* mat = _make_coo_int16_matrix(3u, 4u, 8u, true);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)INT16_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(int16_t));
    assert_int_equal((int)matrix_format(mat), (int)COO_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 0);
    assert_true(mat->rep.coo.growth);
    assert_true(mat->rep.coo.sorted);

    return_int16_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int16_coo_unset_entry_reads_back_as_zero(void** state) {
    (void)state;

    int16_matrix_t* mat = _make_coo_int16_matrix(3u, 4u, 8u, true);
    int16_t out = (int16_t)-99;

    assert_int_equal(get_int16_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_int16_matrix(mat);
}

/* =============================================================================
 * Group 3: dense set/get
 * ========================================================================== */

static void test_int16_set_dense_null_matrix_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_int16_matrix(NULL, 0u, 0u, (int16_t)7), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int16_get_dense_null_out_returns_null_pointer(void** state) {
    (void)state;
    int16_matrix_t* mat = _make_dense_int16_matrix(2u, 2u);

    assert_int_equal(get_int16_matrix(mat, 0u, 0u, NULL), NULL_POINTER);
    return_int16_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int16_set_get_dense_single_value(void** state) {
    (void)state;
    int16_matrix_t* mat = _make_dense_int16_matrix(3u, 3u);

    int16_t out = 0;
    assert_int_equal(set_int16_matrix(mat, 1u, 2u, (int16_t)42), NO_ERROR);
    assert_int_equal(get_int16_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 42);

    return_int16_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int16_set_dense_overwrites_existing_value(void** state) {
    (void)state;
    int16_matrix_t* mat = _make_dense_int16_matrix(2u, 2u);

    int16_t out = 0;
    assert_int_equal(set_int16_matrix(mat, 1u, 1u, (int16_t)5), NO_ERROR);
    assert_int_equal(set_int16_matrix(mat, 1u, 1u, (int16_t)-7), NO_ERROR);
    assert_int_equal(get_int16_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, -7);

    return_int16_matrix(mat);
}

/* =============================================================================
 * Group 4: COO set/get
 * ========================================================================== */

static void test_int16_set_get_coo_single_value(void** state) {
    (void)state;
    int16_matrix_t* mat = _make_coo_int16_matrix(3u, 3u, 4u, true);

    int16_t out = 0;
    assert_int_equal(set_int16_matrix(mat, 2u, 1u, (int16_t)-23), NO_ERROR);
    assert_int_equal(get_int16_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, -23);
    assert_int_equal((int)matrix_nnz(mat), 1);

    return_int16_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int16_push_back_coo_overwrite_does_not_increase_nnz(void** state) {
    (void)state;
    int16_matrix_t* mat = _make_coo_int16_matrix(3u, 3u, 4u, true);

    int16_t out = 0;
    assert_int_equal(push_back_int16_coo_matrix(mat, 1u, 1u, (int16_t)5), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(push_back_int16_coo_matrix(mat, 1u, 1u, (int16_t)-9), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(get_int16_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, -9);

    return_int16_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int16_push_back_coo_capacity_overflow_when_growth_disabled(void** state) {
    (void)state;
    int16_matrix_t* mat = _make_coo_int16_matrix(3u, 3u, 2u, false);

    assert_int_equal(push_back_int16_coo_matrix(mat, 0u, 0u, (int16_t)1), NO_ERROR);
    assert_int_equal(push_back_int16_coo_matrix(mat, 1u, 1u, (int16_t)-2), NO_ERROR);
    assert_int_equal(push_back_int16_coo_matrix(mat, 2u, 2u, (int16_t)3), CAPACITY_OVERFLOW);

    return_int16_matrix(mat);
}

/* =============================================================================
 * Group 5: clear / copy
 * ========================================================================== */

static void test_clear_int16_dense_zeroes_all_entries(void** state) {
    (void)state;
    int16_matrix_t* mat = _make_dense_int16_matrix(2u, 3u);
    int16_t out = (int16_t)-99;

    assert_int_equal(set_int16_matrix(mat, 0u, 0u, (int16_t)11), NO_ERROR);
    assert_int_equal(set_int16_matrix(mat, 0u, 2u, (int16_t)-22), NO_ERROR);
    assert_false(int16_matrix_is_zero(mat));

    assert_int_equal(clear_int16_matrix(mat), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = (int16_t)-99;
            assert_int_equal(get_int16_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 0);
        }
    }

    assert_true(int16_matrix_is_zero(mat));
    return_int16_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_copy_int16_dense_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_matrix_t* src = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t r = copy_int16_matrix(src, a);
    assert_true(r.has_value);
    assert_true(int16_matrix_equal(src, r.u.value));

    return_int16_matrix(r.u.value);
    return_int16_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_int16_coo_is_independent_of_source(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_matrix_t* src = _make_coo_int16_matrix(3u, 3u, 4u, true);

    assert_int_equal(set_int16_matrix(src, 1u, 1u, (int16_t)5), NO_ERROR);

    int16_matrix_expect_t r = copy_int16_matrix(src, a);
    assert_true(r.has_value);

    assert_int_equal(set_int16_matrix(r.u.value, 1u, 1u, (int16_t)-77), NO_ERROR);

    int16_t src_out = 0;
    int16_t dst_out = 0;

    assert_int_equal(get_int16_matrix(src, 1u, 1u, &src_out), NO_ERROR);
    assert_int_equal(get_int16_matrix(r.u.value, 1u, 1u, &dst_out), NO_ERROR);

    assert_int_equal((int)src_out, 5);
    assert_int_equal((int)dst_out, -77);

    return_int16_matrix(r.u.value);
    return_int16_matrix(src);
}

/* =============================================================================
 * Group 6: convert / convert_zero
 * ========================================================================== */

static void test_convert_int16_dense_to_coo_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_matrix_t* src = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t r = convert_int16_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)COO_MATRIX);
    assert_true(int16_matrix_equal(src, r.u.value));

    return_int16_matrix(r.u.value);
    return_int16_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int16_dense_to_csr_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_matrix_t* src = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t r = convert_int16_matrix(src, CSR_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)CSR_MATRIX);
    assert_true(int16_matrix_equal(src, r.u.value));

    return_int16_matrix(r.u.value);
    return_int16_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int16_zero_dense_to_csr_omits_neg_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_matrix_t* src = _make_dense_int16_matrix(2u, 3u);

    assert_int_equal(set_int16_matrix(src, 0u, 0u, (int16_t)5), NO_ERROR);
    assert_int_equal(set_int16_matrix(src, 0u, 1u, (int16_t)-1), NO_ERROR);
    assert_int_equal(set_int16_matrix(src, 1u, 2u, (int16_t)8), NO_ERROR);

    int16_matrix_expect_t r =
        convert_int16_matrix_zero(src, CSR_MATRIX, a, _int16_zero_or_neg_one);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_nnz(r.u.value), 2);

    return_int16_matrix(r.u.value);
    return_int16_matrix(src);
}

/* =============================================================================
 * Group 7: transpose / fill
 * ========================================================================== */

static void test_transpose_int16_dense_moves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int16_matrix_t* src = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t r = transpose_int16_matrix(src, a);
    assert_true(r.has_value);

    int16_t out = 0;
    assert_int_equal(get_int16_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_int16_matrix(r.u.value, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    return_int16_matrix(r.u.value);
    return_int16_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_fill_int16_dense_sets_every_entry(void** state) {
    (void)state;
    int16_matrix_t* mat = _make_dense_int16_matrix(2u, 3u);
    int16_t out = 0;

    assert_int_equal(fill_int16_matrix(mat, (int16_t)-7), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            assert_int_equal(get_int16_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, -7);
        }
    }

    return_int16_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_fill_int16_zero_equivalent_to_clear(void** state) {
    (void)state;
    int16_matrix_t* mat = _make_dense_int16_matrix(2u, 2u);

    assert_int_equal(set_int16_matrix(mat, 1u, 1u, (int16_t)9), NO_ERROR);
    assert_false(int16_matrix_is_zero(mat));

    assert_int_equal(fill_int16_matrix(mat, (int16_t)0), NO_ERROR);
    assert_true(int16_matrix_is_zero(mat));

    return_int16_matrix(mat);
}

/* =============================================================================
 * Group 8: equality / compatibility
 * ========================================================================== */

static void test_int16_matrix_equal_dense_same_values_returns_true(void** state) {
    (void)state;

    int16_matrix_t* a = _make_sample_dense_int16_matrix();
    int16_matrix_t* b = _make_sample_dense_int16_matrix();

    assert_true(int16_matrix_equal(a, b));

    return_int16_matrix(a);
    return_int16_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_int16_matrix_shape_compatibility_helpers(void** state) {
    (void)state;

    int16_matrix_t* a = _make_dense_int16_matrix(2u, 3u);
    int16_matrix_t* b = _make_dense_int16_matrix(2u, 3u);
    int16_matrix_t* c = _make_dense_int16_matrix(3u, 4u);

    assert_true(int16_matrix_has_same_shape(a, b));
    assert_true(int16_matrix_is_add_compatible(a, b));
    assert_true(int16_matrix_is_multiply_compatible(a, c));
    assert_false(int16_matrix_is_square(a));

    return_int16_matrix(a);
    return_int16_matrix(b);
    return_int16_matrix(c);
}

/* =============================================================================
 * Group 9: swaps / constructors / vector queries
 * ========================================================================== */

static void test_swap_int16_matrix_rows_dense(void** state) {
    (void)state;
    int16_matrix_t* mat = _make_dense_int16_matrix(2u, 2u);
    int16_t out = 0;

    assert_int_equal(set_int16_matrix(mat, 0u, 0u, (int16_t)1), NO_ERROR);
    assert_int_equal(set_int16_matrix(mat, 1u, 0u, (int16_t)-2), NO_ERROR);

    assert_int_equal(swap_int16_matrix_rows(mat, 0u, 1u), NO_ERROR);

    assert_int_equal(get_int16_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, -2);

    assert_int_equal(get_int16_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    return_int16_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_init_int16_identity_matrix_sets_diagonal_to_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int16_matrix_expect_t r = init_int16_identity_matrix(3u, a);
    assert_true(r.has_value);

    int16_t out = 0;
    assert_int_equal(get_int16_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    assert_int_equal(get_int16_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_int16_matrix(r.u.value);
}

// --------------------------------------------------------------------------------

static void test_int16_row_and_col_vector_queries(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int16_matrix_expect_t row = init_int16_row_vector(5u, a);
    int16_matrix_expect_t col = init_int16_col_vector(7u, a);

    assert_true(row.has_value);
    assert_true(col.has_value);

    assert_true(int16_matrix_is_row_vector(row.u.value));
    assert_true(int16_matrix_is_vector(row.u.value));
    assert_int_equal((int)int16_matrix_vector_length(row.u.value), 5);

    assert_true(int16_matrix_is_col_vector(col.u.value));
    assert_true(int16_matrix_is_vector(col.u.value));
    assert_int_equal((int)int16_matrix_vector_length(col.u.value), 7);

    return_int16_matrix(row.u.value);
    return_int16_matrix(col.u.value);
}

/* =============================================================================
 * Group 10: Additional int16 matrix tests
 * ========================================================================== */

static void test_transpose_int16_coo_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int16_matrix_t* src = _make_sample_coo_int16_matrix();

    int16_matrix_expect_t r = transpose_int16_matrix(src, alloc);
    assert_true(r.has_value);

    int16_matrix_t* tr = r.u.value;
    int16_t out = 0;

    assert_int_equal((int)matrix_format(tr), (int)COO_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_int16_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_int16_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_int16_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_int16_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_int16_matrix(tr);
    return_int16_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_transpose_int16_csr_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int16_matrix_t* dense = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t src_r = convert_int16_matrix(dense, CSR_MATRIX, alloc);
    assert_true(src_r.has_value);

    int16_matrix_expect_t tr_r = transpose_int16_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    int16_matrix_t* tr = tr_r.u.value;
    int16_t out = 0;

    assert_int_equal((int)matrix_format(tr), (int)CSR_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_int16_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_int16_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_int16_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_int16_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_int16_matrix(tr);
    return_int16_matrix(src_r.u.value);
    return_int16_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_transpose_int16_csc_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int16_matrix_t* dense = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t src_r = convert_int16_matrix(dense, CSC_MATRIX, alloc);
    assert_true(src_r.has_value);

    int16_matrix_expect_t tr_r = transpose_int16_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    int16_matrix_t* tr = tr_r.u.value;
    int16_t out = 0;

    assert_int_equal((int)matrix_format(tr), (int)CSC_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_int16_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_int16_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_int16_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_int16_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_int16_matrix(tr);
    return_int16_matrix(src_r.u.value);
    return_int16_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_swap_int16_matrix_cols_dense(void** state) {
    (void)state;

    int16_matrix_t* mat = _make_dense_int16_matrix(2u, 3u);
    int16_t out = 0;

    assert_int_equal(set_int16_matrix(mat, 0u, 0u, (int16_t)1), NO_ERROR);
    assert_int_equal(set_int16_matrix(mat, 0u, 2u, (int16_t)-3), NO_ERROR);
    assert_int_equal(set_int16_matrix(mat, 1u, 0u, (int16_t)4), NO_ERROR);
    assert_int_equal(set_int16_matrix(mat, 1u, 2u, (int16_t)-6), NO_ERROR);

    assert_int_equal(swap_int16_matrix_cols(mat, 0u, 2u), NO_ERROR);

    assert_int_equal(get_int16_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, -3);

    assert_int_equal(get_int16_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    assert_int_equal(get_int16_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, -6);

    assert_int_equal(get_int16_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 4);

    return_int16_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int16_matrix_is_sparse_dense_returns_false(void** state) {
    (void)state;

    int16_matrix_t* mat = _make_dense_int16_matrix(3u, 3u);
    assert_false(int16_matrix_is_sparse(mat));

    return_int16_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int16_matrix_is_sparse_coo_returns_true(void** state) {
    (void)state;

    int16_matrix_t* mat = _make_coo_int16_matrix(3u, 3u, 4u, true);
    assert_true(int16_matrix_is_sparse(mat));

    return_int16_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int16_matrix_is_sparse_csr_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int16_matrix_t* dense = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t r = convert_int16_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(int16_matrix_is_sparse(r.u.value));

    return_int16_matrix(r.u.value);
    return_int16_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_int16_matrix_is_sparse_csc_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int16_matrix_t* dense = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t r = convert_int16_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(int16_matrix_is_sparse(r.u.value));

    return_int16_matrix(r.u.value);
    return_int16_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_int16_matrix_equal_dense_and_coo_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int16_matrix_t* dense = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t r = convert_int16_matrix(dense, COO_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(int16_matrix_equal(dense, r.u.value));
    assert_true(int16_matrix_equal(r.u.value, dense));

    return_int16_matrix(r.u.value);
    return_int16_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_int16_matrix_equal_dense_and_csr_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int16_matrix_t* dense = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t r = convert_int16_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(int16_matrix_equal(dense, r.u.value));
    assert_true(int16_matrix_equal(r.u.value, dense));

    return_int16_matrix(r.u.value);
    return_int16_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_int16_matrix_equal_dense_and_csc_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int16_matrix_t* dense = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t r = convert_int16_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(int16_matrix_equal(dense, r.u.value));
    assert_true(int16_matrix_equal(r.u.value, dense));

    return_int16_matrix(r.u.value);
    return_int16_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_convert_int16_matrix_zero_null_callback_matches_convert_int16_matrix_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int16_matrix_t* src = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t r1 = convert_int16_matrix(src, COO_MATRIX, alloc);
    int16_matrix_expect_t r2 = convert_int16_matrix_zero(src, COO_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(int16_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_int16_matrix(r2.u.value);
    return_int16_matrix(r1.u.value);
    return_int16_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int16_matrix_zero_null_callback_matches_convert_int16_matrix_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int16_matrix_t* src = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t r1 = convert_int16_matrix(src, CSR_MATRIX, alloc);
    int16_matrix_expect_t r2 = convert_int16_matrix_zero(src, CSR_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(int16_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_int16_matrix(r2.u.value);
    return_int16_matrix(r1.u.value);
    return_int16_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int16_matrix_zero_null_callback_matches_convert_int16_matrix_csc(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int16_matrix_t* src = _make_sample_dense_int16_matrix();

    int16_matrix_expect_t r1 = convert_int16_matrix(src, CSC_MATRIX, alloc);
    int16_matrix_expect_t r2 = convert_int16_matrix_zero(src, CSC_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(int16_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_int16_matrix(r2.u.value);
    return_int16_matrix(r1.u.value);
    return_int16_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int16_matrix_zero_default_omits_zero_in_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int16_matrix_t* src = _make_dense_int16_matrix(2u, 2u);

    assert_int_equal(set_int16_matrix(src, 0u, 0u, (int16_t)0), NO_ERROR);
    assert_int_equal(set_int16_matrix(src, 1u, 1u, (int16_t)1), NO_ERROR);

    int16_matrix_expect_t r = convert_int16_matrix_zero(src, COO_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    int16_t out = (int16_t)99;
    assert_int_equal(get_int16_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    assert_int_equal(get_int16_matrix(r.u.value, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    return_int16_matrix(r.u.value);
    return_int16_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int16_matrix_zero_default_omits_zero_in_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int16_matrix_t* src = _make_dense_int16_matrix(2u, 2u);

    assert_int_equal(set_int16_matrix(src, 0u, 1u, (int16_t)0), NO_ERROR);
    assert_int_equal(set_int16_matrix(src, 1u, 0u, (int16_t)-2), NO_ERROR);

    int16_matrix_expect_t r = convert_int16_matrix_zero(src, CSR_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    int16_t out = 0;
    assert_int_equal(get_int16_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    assert_int_equal(get_int16_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, -2);

    return_int16_matrix(r.u.value);
    return_int16_matrix(src);
}

/* =============================================================================
 * Registry
 * ========================================================================== */

const struct CMUnitTest test_int16_matrix[] = {
    /* Group 1 */
    cmocka_unit_test(test_int16_dense_init_zero_rows_fails),
    cmocka_unit_test(test_int16_dense_init_zero_cols_fails),
    cmocka_unit_test(test_int16_dense_init_returns_valid_matrix),
    cmocka_unit_test(test_int16_dense_init_is_zero_initialized),

    /* Group 2 */
    cmocka_unit_test(test_int16_coo_init_zero_capacity_fails),
    cmocka_unit_test(test_int16_coo_init_returns_valid_matrix),
    cmocka_unit_test(test_int16_coo_unset_entry_reads_back_as_zero),

    /* Group 3 */
    cmocka_unit_test(test_int16_set_dense_null_matrix_returns_null_pointer),
    cmocka_unit_test(test_int16_get_dense_null_out_returns_null_pointer),
    cmocka_unit_test(test_int16_set_get_dense_single_value),
    cmocka_unit_test(test_int16_set_dense_overwrites_existing_value),

    /* Group 4 */
    cmocka_unit_test(test_int16_set_get_coo_single_value),
    cmocka_unit_test(test_int16_push_back_coo_overwrite_does_not_increase_nnz),
    cmocka_unit_test(test_int16_push_back_coo_capacity_overflow_when_growth_disabled),

    /* Group 5 */
    cmocka_unit_test(test_clear_int16_dense_zeroes_all_entries),
    cmocka_unit_test(test_copy_int16_dense_preserves_values),
    cmocka_unit_test(test_copy_int16_coo_is_independent_of_source),

    /* Group 6 */
    cmocka_unit_test(test_convert_int16_dense_to_coo_preserves_values),
    cmocka_unit_test(test_convert_int16_dense_to_csr_preserves_values),
    cmocka_unit_test(test_convert_int16_zero_dense_to_csr_omits_neg_one),

    /* Group 7 */
    cmocka_unit_test(test_transpose_int16_dense_moves_values),
    cmocka_unit_test(test_fill_int16_dense_sets_every_entry),
    cmocka_unit_test(test_fill_int16_zero_equivalent_to_clear),

    /* Group 8 */
    cmocka_unit_test(test_int16_matrix_equal_dense_same_values_returns_true),
    cmocka_unit_test(test_int16_matrix_shape_compatibility_helpers),

    /* Group 9 */
    cmocka_unit_test(test_swap_int16_matrix_rows_dense),
    cmocka_unit_test(test_init_int16_identity_matrix_sets_diagonal_to_one),
    cmocka_unit_test(test_int16_row_and_col_vector_queries),

    /* Group 10 */
    cmocka_unit_test(test_transpose_int16_coo_preserves_logical_values),
    cmocka_unit_test(test_transpose_int16_csr_preserves_logical_values),
    cmocka_unit_test(test_transpose_int16_csc_preserves_logical_values),

    cmocka_unit_test(test_swap_int16_matrix_cols_dense),

    cmocka_unit_test(test_int16_matrix_is_sparse_dense_returns_false),
    cmocka_unit_test(test_int16_matrix_is_sparse_coo_returns_true),
    cmocka_unit_test(test_int16_matrix_is_sparse_csr_returns_true),
    cmocka_unit_test(test_int16_matrix_is_sparse_csc_returns_true),

    cmocka_unit_test(test_int16_matrix_equal_dense_and_coo_same_values_returns_true),
    cmocka_unit_test(test_int16_matrix_equal_dense_and_csr_same_values_returns_true),
    cmocka_unit_test(test_int16_matrix_equal_dense_and_csc_same_values_returns_true),

    cmocka_unit_test(test_convert_int16_matrix_zero_null_callback_matches_convert_int16_matrix_coo),
    cmocka_unit_test(test_convert_int16_matrix_zero_null_callback_matches_convert_int16_matrix_csr),
    cmocka_unit_test(test_convert_int16_matrix_zero_null_callback_matches_convert_int16_matrix_csc),

    cmocka_unit_test(test_convert_int16_matrix_zero_default_omits_zero_in_coo),
    cmocka_unit_test(test_convert_int16_matrix_zero_default_omits_zero_in_csr),
};

const size_t test_int16_matrix_count =
    sizeof(test_int16_matrix) / sizeof(test_int16_matrix[0]);
// ================================================================================ 
// ================================================================================ 

static uint32_matrix_t* _make_dense_uint32_matrix(size_t rows, size_t cols) {
    allocator_vtable_t a = heap_allocator();
    uint32_matrix_expect_t r = init_uint32_dense_matrix(rows, cols, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

static uint32_matrix_t* _make_coo_uint32_matrix(size_t rows,
                                              size_t cols,
                                              size_t cap,
                                              bool   growth) {
    allocator_vtable_t a = heap_allocator();
    uint32_matrix_expect_t r = init_uint32_coo_matrix(rows, cols, cap, growth, a);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    return r.u.value;
}

static uint32_matrix_t* _make_sample_dense_uint32_matrix(void) {
    uint32_matrix_t* mat = _make_dense_uint32_matrix(3u, 4u);

    assert_int_equal(set_uint32_matrix(mat, 0u, 1u, (uint32_t)10u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(mat, 1u, 3u, (uint32_t)20u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(mat, 2u, 0u, (uint32_t)30u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(mat, 2u, 2u, (uint32_t)40u), NO_ERROR);

    return mat;
}

static uint32_matrix_t* _make_sample_coo_uint32_matrix(void) {
    uint32_matrix_t* mat = _make_coo_uint32_matrix(3u, 4u, 8u, true);

    assert_int_equal(set_uint32_matrix(mat, 0u, 1u, (uint32_t)10u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(mat, 1u, 3u, (uint32_t)20u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(mat, 2u, 0u, (uint32_t)30u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(mat, 2u, 2u, (uint32_t)40u), NO_ERROR);

    return mat;
}

static bool _uint32_equal_mod_10(uint32_t a, uint32_t b) {
    return (uint32_t)(a % 10u) == (uint32_t)(b % 10u);
}

static bool _uint32_zero_or_ff(uint32_t v) {
    return (v == (uint32_t)0u) || (v == (uint32_t)255u);
}

// ================================================================================
// Group 1: init_uint32_dense_matrix
// ================================================================================

static void test_uint32_dense_init_zero_rows_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint32_matrix_expect_t r = init_uint32_dense_matrix(0u, 4u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_uint32_dense_init_zero_cols_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint32_matrix_expect_t r = init_uint32_dense_matrix(3u, 0u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_uint32_dense_init_returns_valid_matrix(void** state) {
    (void)state;

    uint32_matrix_t* mat = _make_dense_uint32_matrix(3u, 4u);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)UINT32_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(uint32_t));
    assert_int_equal((int)matrix_format(mat), (int)DENSE_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 12);

    return_uint32_matrix(mat);
}

static void test_uint32_dense_init_is_zero_initialized(void** state) {
    (void)state;

    uint32_matrix_t* mat = _make_dense_uint32_matrix(2u, 3u);
    uint32_t out = 255u;

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = 255u;
            assert_int_equal(get_uint32_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 0);
        }
    }

    assert_true(uint32_matrix_is_zero(mat));
    return_uint32_matrix(mat);
}

// ================================================================================
// Group 2: init_uint32_coo_matrix
// ================================================================================

static void test_uint32_coo_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint32_matrix_expect_t r = init_uint32_coo_matrix(3u, 4u, 0u, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

static void test_uint32_coo_init_returns_valid_matrix(void** state) {
    (void)state;

    uint32_matrix_t* mat = _make_coo_uint32_matrix(3u, 4u, 8u, true);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)UINT32_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(uint32_t));
    assert_int_equal((int)matrix_format(mat), (int)COO_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 0);
    assert_true(mat->rep.coo.growth);
    assert_true(mat->rep.coo.sorted);

    return_uint32_matrix(mat);
}

static void test_uint32_coo_unset_entry_reads_back_as_zero(void** state) {
    (void)state;

    uint32_matrix_t* mat = _make_coo_uint32_matrix(3u, 4u, 8u, true);
    uint32_t out = 255u;

    assert_int_equal(get_uint32_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_uint32_matrix(mat);
}

// ================================================================================
// Group 3: dense set/get
// ================================================================================

static void test_uint32_set_dense_null_matrix_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_uint32_matrix(NULL, 0u, 0u, (uint32_t)7u), NULL_POINTER);
}

static void test_uint32_get_dense_null_out_returns_null_pointer(void** state) {
    (void)state;
    uint32_matrix_t* mat = _make_dense_uint32_matrix(2u, 2u);

    assert_int_equal(get_uint32_matrix(mat, 0u, 0u, NULL), NULL_POINTER);
    return_uint32_matrix(mat);
}

static void test_uint32_set_get_dense_single_value(void** state) {
    (void)state;
    uint32_matrix_t* mat = _make_dense_uint32_matrix(3u, 3u);

    uint32_t out = 0u;
    assert_int_equal(set_uint32_matrix(mat, 1u, 2u, (uint32_t)42u), NO_ERROR);
    assert_int_equal(get_uint32_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 42);

    return_uint32_matrix(mat);
}

static void test_uint32_set_dense_overwrites_existing_value(void** state) {
    (void)state;
    uint32_matrix_t* mat = _make_dense_uint32_matrix(2u, 2u);

    uint32_t out = 0u;
    assert_int_equal(set_uint32_matrix(mat, 1u, 1u, (uint32_t)5u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(mat, 1u, 1u, (uint32_t)99u), NO_ERROR);
    assert_int_equal(get_uint32_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 99);

    return_uint32_matrix(mat);
}

// ================================================================================
// Group 4: COO set/get
// ================================================================================

static void test_uint32_set_get_coo_single_value(void** state) {
    (void)state;
    uint32_matrix_t* mat = _make_coo_uint32_matrix(3u, 3u, 4u, true);

    uint32_t out = 0u;
    assert_int_equal(set_uint32_matrix(mat, 2u, 1u, (uint32_t)123u), NO_ERROR);
    assert_int_equal(get_uint32_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 123);
    assert_int_equal((int)matrix_nnz(mat), 1);

    return_uint32_matrix(mat);
}

static void test_uint32_push_back_coo_overwrite_does_not_increase_nnz(void** state) {
    (void)state;
    uint32_matrix_t* mat = _make_coo_uint32_matrix(3u, 3u, 4u, true);

    uint32_t out = 0u;
    assert_int_equal(push_back_uint32_coo_matrix(mat, 1u, 1u, (uint32_t)5u), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(push_back_uint32_coo_matrix(mat, 1u, 1u, (uint32_t)99u), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(get_uint32_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 99);

    return_uint32_matrix(mat);
}

static void test_uint32_push_back_coo_capacity_overflow_when_growth_disabled(void** state) {
    (void)state;
    uint32_matrix_t* mat = _make_coo_uint32_matrix(3u, 3u, 2u, false);

    assert_int_equal(push_back_uint32_coo_matrix(mat, 0u, 0u, (uint32_t)1u), NO_ERROR);
    assert_int_equal(push_back_uint32_coo_matrix(mat, 1u, 1u, (uint32_t)2u), NO_ERROR);
    assert_int_equal(push_back_uint32_coo_matrix(mat, 2u, 2u, (uint32_t)3u), CAPACITY_OVERFLOW);

    return_uint32_matrix(mat);
}

// ================================================================================
// Group 5: clear / copy
// ================================================================================

static void test_clear_uint32_dense_zeroes_all_entries(void** state) {
    (void)state;
    uint32_matrix_t* mat = _make_dense_uint32_matrix(2u, 3u);
    uint32_t out = 255u;

    assert_int_equal(set_uint32_matrix(mat, 0u, 0u, (uint32_t)11u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(mat, 0u, 2u, (uint32_t)22u), NO_ERROR);
    assert_false(uint32_matrix_is_zero(mat));

    assert_int_equal(clear_uint32_matrix(mat), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = 255u;
            assert_int_equal(get_uint32_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 0);
        }
    }

    assert_true(uint32_matrix_is_zero(mat));
    return_uint32_matrix(mat);
}

static void test_copy_uint32_dense_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_matrix_t* src = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t r = copy_uint32_matrix(src, a);
    assert_true(r.has_value);
    assert_true(uint32_matrix_equal(src, r.u.value));

    return_uint32_matrix(r.u.value);
    return_uint32_matrix(src);
}

static void test_copy_uint32_coo_is_independent_of_source(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_matrix_t* src = _make_coo_uint32_matrix(3u, 3u, 4u, true);

    assert_int_equal(set_uint32_matrix(src, 1u, 1u, (uint32_t)5u), NO_ERROR);

    uint32_matrix_expect_t r = copy_uint32_matrix(src, a);
    assert_true(r.has_value);

    assert_int_equal(set_uint32_matrix(r.u.value, 1u, 1u, (uint32_t)77u), NO_ERROR);

    uint32_t src_out = 0u;
    uint32_t dst_out = 0u;

    assert_int_equal(get_uint32_matrix(src, 1u, 1u, &src_out), NO_ERROR);
    assert_int_equal(get_uint32_matrix(r.u.value, 1u, 1u, &dst_out), NO_ERROR);

    assert_int_equal((int)src_out, 5);
    assert_int_equal((int)dst_out, 77);

    return_uint32_matrix(r.u.value);
    return_uint32_matrix(src);
}

// ================================================================================
// Group 6: convert / convert_zero
// ================================================================================

static void test_convert_uint32_dense_to_coo_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_matrix_t* src = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t r = convert_uint32_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)COO_MATRIX);
    assert_true(uint32_matrix_equal(src, r.u.value));

    return_uint32_matrix(r.u.value);
    return_uint32_matrix(src);
}

static void test_convert_uint32_dense_to_csr_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_matrix_t* src = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t r = convert_uint32_matrix(src, CSR_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)CSR_MATRIX);
    assert_true(uint32_matrix_equal(src, r.u.value));

    return_uint32_matrix(r.u.value);
    return_uint32_matrix(src);
}

static void test_convert_uint32_zero_dense_to_csr_omits_ff(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_matrix_t* src = _make_dense_uint32_matrix(2u, 3u);

    assert_int_equal(set_uint32_matrix(src, 0u, 0u, (uint32_t)5u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(src, 0u, 1u, (uint32_t)255u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(src, 1u, 2u, (uint32_t)8u), NO_ERROR);

    uint32_matrix_expect_t r =
        convert_uint32_matrix_zero(src, CSR_MATRIX, a, _uint32_zero_or_ff);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_nnz(r.u.value), 2);

    return_uint32_matrix(r.u.value);
    return_uint32_matrix(src);
}

// ================================================================================
// Group 7: transpose / fill
// ================================================================================

static void test_transpose_uint32_dense_moves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    uint32_matrix_t* src = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t r = transpose_uint32_matrix(src, a);
    assert_true(r.has_value);

    uint32_t out = 0u;
    assert_int_equal(get_uint32_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_uint32_matrix(r.u.value, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    return_uint32_matrix(r.u.value);
    return_uint32_matrix(src);
}

static void test_fill_uint32_dense_sets_every_entry(void** state) {
    (void)state;
    uint32_matrix_t* mat = _make_dense_uint32_matrix(2u, 3u);
    uint32_t out = 0u;

    assert_int_equal(fill_uint32_matrix(mat, (uint32_t)7u), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            assert_int_equal(get_uint32_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 7);
        }
    }

    return_uint32_matrix(mat);
}

static void test_fill_uint32_zero_equivalent_to_clear(void** state) {
    (void)state;
    uint32_matrix_t* mat = _make_dense_uint32_matrix(2u, 2u);

    assert_int_equal(set_uint32_matrix(mat, 1u, 1u, (uint32_t)9u), NO_ERROR);
    assert_false(uint32_matrix_is_zero(mat));

    assert_int_equal(fill_uint32_matrix(mat, (uint32_t)0u), NO_ERROR);
    assert_true(uint32_matrix_is_zero(mat));

    return_uint32_matrix(mat);
}

// ================================================================================
// Group 8: equality / compatibility
// ================================================================================

static void test_uint32_matrix_equal_dense_same_values_returns_true(void** state) {
    (void)state;

    uint32_matrix_t* a = _make_sample_dense_uint32_matrix();
    uint32_matrix_t* b = _make_sample_dense_uint32_matrix();

    assert_true(uint32_matrix_equal(a, b));

    return_uint32_matrix(a);
    return_uint32_matrix(b);
}

static void test_uint32_matrix_shape_compatibility_helpers(void** state) {
    (void)state;

    uint32_matrix_t* a = _make_dense_uint32_matrix(2u, 3u);
    uint32_matrix_t* b = _make_dense_uint32_matrix(2u, 3u);
    uint32_matrix_t* c = _make_dense_uint32_matrix(3u, 4u);

    assert_true(uint32_matrix_has_same_shape(a, b));
    assert_true(uint32_matrix_is_add_compatible(a, b));
    assert_true(uint32_matrix_is_multiply_compatible(a, c));
    assert_false(uint32_matrix_is_square(a));

    return_uint32_matrix(a);
    return_uint32_matrix(b);
    return_uint32_matrix(c);
}

// ================================================================================
// Group 9: swaps / constructors / vector queries
// ================================================================================

static void test_swap_uint32_matrix_rows_dense(void** state) {
    (void)state;
    uint32_matrix_t* mat = _make_dense_uint32_matrix(2u, 2u);
    uint32_t out = 0u;

    assert_int_equal(set_uint32_matrix(mat, 0u, 0u, (uint32_t)1u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(mat, 1u, 0u, (uint32_t)2u), NO_ERROR);

    assert_int_equal(swap_uint32_matrix_rows(mat, 0u, 1u), NO_ERROR);

    assert_int_equal(get_uint32_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 2);

    assert_int_equal(get_uint32_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    return_uint32_matrix(mat);
}

static void test_init_uint32_identity_matrix_sets_diagonal_to_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint32_matrix_expect_t r = init_uint32_identity_matrix(3u, a);
    assert_true(r.has_value);

    uint32_t out = 0u;
    assert_int_equal(get_uint32_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    assert_int_equal(get_uint32_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_uint32_matrix(r.u.value);
}

static void test_uint32_row_and_col_vector_queries(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    uint32_matrix_expect_t row = init_uint32_row_vector(5u, a);
    uint32_matrix_expect_t col = init_uint32_col_vector(7u, a);

    assert_true(row.has_value);
    assert_true(col.has_value);

    assert_true(uint32_matrix_is_row_vector(row.u.value));
    assert_true(uint32_matrix_is_vector(row.u.value));
    assert_int_equal((int)uint32_matrix_vector_length(row.u.value), 5);

    assert_true(uint32_matrix_is_col_vector(col.u.value));
    assert_true(uint32_matrix_is_vector(col.u.value));
    assert_int_equal((int)uint32_matrix_vector_length(col.u.value), 7);

    return_uint32_matrix(row.u.value);
    return_uint32_matrix(col.u.value);
}

// ================================================================================
// Group 10: Additional uint32 matrix tests
// ================================================================================

static void test_transpose_uint32_coo_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint32_matrix_t* src = _make_sample_coo_uint32_matrix();

    uint32_matrix_expect_t r = transpose_uint32_matrix(src, alloc);
    assert_true(r.has_value);

    uint32_matrix_t* tr = r.u.value;
    uint32_t out = 0u;

    assert_int_equal((int)matrix_format(tr), (int)COO_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_uint32_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_uint32_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_uint32_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_uint32_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_uint32_matrix(tr);
    return_uint32_matrix(src);
}

static void test_transpose_uint32_csr_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint32_matrix_t* dense = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t src_r = convert_uint32_matrix(dense, CSR_MATRIX, alloc);
    assert_true(src_r.has_value);

    uint32_matrix_expect_t tr_r = transpose_uint32_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    uint32_matrix_t* tr = tr_r.u.value;
    uint32_t out = 0u;

    assert_int_equal((int)matrix_format(tr), (int)CSR_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_uint32_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_uint32_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_uint32_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_uint32_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_uint32_matrix(tr);
    return_uint32_matrix(src_r.u.value);
    return_uint32_matrix(dense);
}

static void test_transpose_uint32_csc_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint32_matrix_t* dense = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t src_r = convert_uint32_matrix(dense, CSC_MATRIX, alloc);
    assert_true(src_r.has_value);

    uint32_matrix_expect_t tr_r = transpose_uint32_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    uint32_matrix_t* tr = tr_r.u.value;
    uint32_t out = 0u;

    assert_int_equal((int)matrix_format(tr), (int)CSC_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_uint32_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_uint32_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_uint32_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_uint32_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_uint32_matrix(tr);
    return_uint32_matrix(src_r.u.value);
    return_uint32_matrix(dense);
}

static void test_swap_uint32_matrix_cols_dense(void** state) {
    (void)state;

    uint32_matrix_t* mat = _make_dense_uint32_matrix(2u, 3u);
    uint32_t out = 0u;

    assert_int_equal(set_uint32_matrix(mat, 0u, 0u, (uint32_t)1u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(mat, 0u, 2u, (uint32_t)3u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(mat, 1u, 0u, (uint32_t)4u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(mat, 1u, 2u, (uint32_t)6u), NO_ERROR);

    assert_int_equal(swap_uint32_matrix_cols(mat, 0u, 2u), NO_ERROR);

    assert_int_equal(get_uint32_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 3);

    assert_int_equal(get_uint32_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    assert_int_equal(get_uint32_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 6);

    assert_int_equal(get_uint32_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 4);

    return_uint32_matrix(mat);
}

static void test_uint32_matrix_is_sparse_dense_returns_false(void** state) {
    (void)state;

    uint32_matrix_t* mat = _make_dense_uint32_matrix(3u, 3u);
    assert_false(uint32_matrix_is_sparse(mat));

    return_uint32_matrix(mat);
}

static void test_uint32_matrix_is_sparse_coo_returns_true(void** state) {
    (void)state;

    uint32_matrix_t* mat = _make_coo_uint32_matrix(3u, 3u, 4u, true);
    assert_true(uint32_matrix_is_sparse(mat));

    return_uint32_matrix(mat);
}

static void test_uint32_matrix_is_sparse_csr_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint32_matrix_t* dense = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t r = convert_uint32_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(uint32_matrix_is_sparse(r.u.value));

    return_uint32_matrix(r.u.value);
    return_uint32_matrix(dense);
}

static void test_uint32_matrix_is_sparse_csc_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint32_matrix_t* dense = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t r = convert_uint32_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(uint32_matrix_is_sparse(r.u.value));

    return_uint32_matrix(r.u.value);
    return_uint32_matrix(dense);
}

static void test_uint32_matrix_equal_dense_and_coo_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint32_matrix_t* dense = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t r = convert_uint32_matrix(dense, COO_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(uint32_matrix_equal(dense, r.u.value));
    assert_true(uint32_matrix_equal(r.u.value, dense));

    return_uint32_matrix(r.u.value);
    return_uint32_matrix(dense);
}

static void test_uint32_matrix_equal_dense_and_csr_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint32_matrix_t* dense = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t r = convert_uint32_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(uint32_matrix_equal(dense, r.u.value));
    assert_true(uint32_matrix_equal(r.u.value, dense));

    return_uint32_matrix(r.u.value);
    return_uint32_matrix(dense);
}

static void test_uint32_matrix_equal_dense_and_csc_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint32_matrix_t* dense = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t r = convert_uint32_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(uint32_matrix_equal(dense, r.u.value));
    assert_true(uint32_matrix_equal(r.u.value, dense));

    return_uint32_matrix(r.u.value);
    return_uint32_matrix(dense);
}

static void test_convert_uint32_matrix_zero_null_callback_matches_convert_uint32_matrix_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint32_matrix_t* src = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t r1 = convert_uint32_matrix(src, COO_MATRIX, alloc);
    uint32_matrix_expect_t r2 = convert_uint32_matrix_zero(src, COO_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(uint32_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_uint32_matrix(r2.u.value);
    return_uint32_matrix(r1.u.value);
    return_uint32_matrix(src);
}

static void test_convert_uint32_matrix_zero_null_callback_matches_convert_uint32_matrix_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint32_matrix_t* src = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t r1 = convert_uint32_matrix(src, CSR_MATRIX, alloc);
    uint32_matrix_expect_t r2 = convert_uint32_matrix_zero(src, CSR_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(uint32_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_uint32_matrix(r2.u.value);
    return_uint32_matrix(r1.u.value);
    return_uint32_matrix(src);
}

static void test_convert_uint32_matrix_zero_null_callback_matches_convert_uint32_matrix_csc(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint32_matrix_t* src = _make_sample_dense_uint32_matrix();

    uint32_matrix_expect_t r1 = convert_uint32_matrix(src, CSC_MATRIX, alloc);
    uint32_matrix_expect_t r2 = convert_uint32_matrix_zero(src, CSC_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(uint32_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_uint32_matrix(r2.u.value);
    return_uint32_matrix(r1.u.value);
    return_uint32_matrix(src);
}

static void test_convert_uint32_matrix_zero_default_omits_zero_in_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint32_matrix_t* src = _make_dense_uint32_matrix(2u, 2u);

    assert_int_equal(set_uint32_matrix(src, 0u, 0u, (uint32_t)0u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(src, 1u, 1u, (uint32_t)1u), NO_ERROR);

    uint32_matrix_expect_t r = convert_uint32_matrix_zero(src, COO_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    uint32_t out = 123u;
    assert_int_equal(get_uint32_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    assert_int_equal(get_uint32_matrix(r.u.value, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    return_uint32_matrix(r.u.value);
    return_uint32_matrix(src);
}

static void test_convert_uint32_matrix_zero_default_omits_zero_in_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    uint32_matrix_t* src = _make_dense_uint32_matrix(2u, 2u);

    assert_int_equal(set_uint32_matrix(src, 0u, 1u, (uint32_t)0u), NO_ERROR);
    assert_int_equal(set_uint32_matrix(src, 1u, 0u, (uint32_t)2u), NO_ERROR);

    uint32_matrix_expect_t r = convert_uint32_matrix_zero(src, CSR_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    uint32_t out = 0u;
    assert_int_equal(get_uint32_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    assert_int_equal(get_uint32_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 2);

    return_uint32_matrix(r.u.value);
    return_uint32_matrix(src);
}

// ================================================================================
// Registry
// ================================================================================

const struct CMUnitTest test_uint32_matrix[] = {
    /* Group 1 */
    cmocka_unit_test(test_uint32_dense_init_zero_rows_fails),
    cmocka_unit_test(test_uint32_dense_init_zero_cols_fails),
    cmocka_unit_test(test_uint32_dense_init_returns_valid_matrix),
    cmocka_unit_test(test_uint32_dense_init_is_zero_initialized),

    /* Group 2 */
    cmocka_unit_test(test_uint32_coo_init_zero_capacity_fails),
    cmocka_unit_test(test_uint32_coo_init_returns_valid_matrix),
    cmocka_unit_test(test_uint32_coo_unset_entry_reads_back_as_zero),

    /* Group 3 */
    cmocka_unit_test(test_uint32_set_dense_null_matrix_returns_null_pointer),
    cmocka_unit_test(test_uint32_get_dense_null_out_returns_null_pointer),
    cmocka_unit_test(test_uint32_set_get_dense_single_value),
    cmocka_unit_test(test_uint32_set_dense_overwrites_existing_value),

    /* Group 4 */
    cmocka_unit_test(test_uint32_set_get_coo_single_value),
    cmocka_unit_test(test_uint32_push_back_coo_overwrite_does_not_increase_nnz),
    cmocka_unit_test(test_uint32_push_back_coo_capacity_overflow_when_growth_disabled),

    /* Group 5 */
    cmocka_unit_test(test_clear_uint32_dense_zeroes_all_entries),
    cmocka_unit_test(test_copy_uint32_dense_preserves_values),
    cmocka_unit_test(test_copy_uint32_coo_is_independent_of_source),

    /* Group 6 */
    cmocka_unit_test(test_convert_uint32_dense_to_coo_preserves_values),
    cmocka_unit_test(test_convert_uint32_dense_to_csr_preserves_values),
    cmocka_unit_test(test_convert_uint32_zero_dense_to_csr_omits_ff),

    /* Group 7 */
    cmocka_unit_test(test_transpose_uint32_dense_moves_values),
    cmocka_unit_test(test_fill_uint32_dense_sets_every_entry),
    cmocka_unit_test(test_fill_uint32_zero_equivalent_to_clear),

    /* Group 8 */
    cmocka_unit_test(test_uint32_matrix_equal_dense_same_values_returns_true),
    cmocka_unit_test(test_uint32_matrix_shape_compatibility_helpers),

    /* Group 9 */
    cmocka_unit_test(test_swap_uint32_matrix_rows_dense),
    cmocka_unit_test(test_init_uint32_identity_matrix_sets_diagonal_to_one),
    cmocka_unit_test(test_uint32_row_and_col_vector_queries),

    /* Group 10 */
    cmocka_unit_test(test_transpose_uint32_coo_preserves_logical_values),
    cmocka_unit_test(test_transpose_uint32_csr_preserves_logical_values),
    cmocka_unit_test(test_transpose_uint32_csc_preserves_logical_values),

    cmocka_unit_test(test_swap_uint32_matrix_cols_dense),

    cmocka_unit_test(test_uint32_matrix_is_sparse_dense_returns_false),
    cmocka_unit_test(test_uint32_matrix_is_sparse_coo_returns_true),
    cmocka_unit_test(test_uint32_matrix_is_sparse_csr_returns_true),
    cmocka_unit_test(test_uint32_matrix_is_sparse_csc_returns_true),

    cmocka_unit_test(test_uint32_matrix_equal_dense_and_coo_same_values_returns_true),
    cmocka_unit_test(test_uint32_matrix_equal_dense_and_csr_same_values_returns_true),
    cmocka_unit_test(test_uint32_matrix_equal_dense_and_csc_same_values_returns_true),

    cmocka_unit_test(test_convert_uint32_matrix_zero_null_callback_matches_convert_uint32_matrix_coo),
    cmocka_unit_test(test_convert_uint32_matrix_zero_null_callback_matches_convert_uint32_matrix_csr),
    cmocka_unit_test(test_convert_uint32_matrix_zero_null_callback_matches_convert_uint32_matrix_csc),

    cmocka_unit_test(test_convert_uint32_matrix_zero_default_omits_zero_in_coo),
    cmocka_unit_test(test_convert_uint32_matrix_zero_default_omits_zero_in_csr),
};

const size_t test_uint32_matrix_count =
    sizeof(test_uint32_matrix) / sizeof(test_uint32_matrix[0]);
// ================================================================================ 
// ================================================================================ 

// static int32_matrix_t* _make_dense_int32_matrix(size_t rows, size_t cols) {
//     allocator_vtable_t a = heap_allocator();
//     int32_matrix_expect_t r = init_int32_dense_matrix(rows, cols, a);
//     assert_true(r.has_value);
//     assert_non_null(r.u.value);
//     return r.u.value;
// }
//
// // --------------------------------------------------------------------------------
//
// static int32_matrix_t* _make_coo_int32_matrix(size_t rows,
//                                             size_t cols,
//                                             size_t cap,
//                                             bool   growth) {
//     allocator_vtable_t a = heap_allocator();
//     int32_matrix_expect_t r = init_int32_coo_matrix(rows, cols, cap, growth, a);
//     assert_true(r.has_value);
//     assert_non_null(r.u.value);
//     return r.u.value;
// }

// --------------------------------------------------------------------------------

// static int32_matrix_t* _make_sample_dense_int32_matrix(void) {
//     int32_matrix_t* mat = _make_dense_int32_matrix(3u, 4u);
//
//     assert_int_equal(set_int32_matrix(mat, 0u, 1u, (int32_t)10), NO_ERROR);
//     assert_int_equal(set_int32_matrix(mat, 1u, 3u, (int32_t)20), NO_ERROR);
//     assert_int_equal(set_int32_matrix(mat, 2u, 0u, (int32_t)30), NO_ERROR);
//     assert_int_equal(set_int32_matrix(mat, 2u, 2u, (int32_t)40), NO_ERROR);
//
//     return mat;
// }
//
// // --------------------------------------------------------------------------------
//
// static int32_matrix_t* _make_sample_coo_int32_matrix(void) {
//     int32_matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, true);
//
//     assert_int_equal(set_int32_matrix(mat, 0u, 1u, (int32_t)10), NO_ERROR);
//     assert_int_equal(set_int32_matrix(mat, 1u, 3u, (int32_t)20), NO_ERROR);
//     assert_int_equal(set_int32_matrix(mat, 2u, 0u, (int32_t)30), NO_ERROR);
//     assert_int_equal(set_int32_matrix(mat, 2u, 2u, (int32_t)40), NO_ERROR);
//
//     return mat;
// }

// --------------------------------------------------------------------------------

static bool _int32_equal_abs(int32_t a, int32_t b) {
    int aa = (a < 0) ? -(int)a : (int)a;
    int bb = (b < 0) ? -(int)b : (int)b;
    return aa == bb;
}

// --------------------------------------------------------------------------------

static bool _int32_zero_or_neg_one(int32_t v) {
    return (v == (int32_t)0) || (v == (int32_t)-1);
}

/* =============================================================================
 * Group 1: init_int32_dense_matrix
 * ========================================================================== */

static void test_int32_dense_init_zero_rows_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int32_matrix_expect_t r = init_int32_dense_matrix(0u, 4u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_int32_dense_init_zero_cols_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int32_matrix_expect_t r = init_int32_dense_matrix(3u, 0u, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_int32_dense_init_returns_valid_matrix(void** state) {
    (void)state;

    int32_matrix_t* mat = _make_dense_int32_matrix(3u, 4u);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)INT32_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(int32_t));
    assert_int_equal((int)matrix_format(mat), (int)DENSE_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 12);

    return_int32_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int32_dense_init_is_zero_initialized(void** state) {
    (void)state;

    int32_matrix_t* mat = _make_dense_int32_matrix(2u, 3u);
    int32_t out = (int32_t)-99;

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = (int32_t)-99;
            assert_int_equal(get_int32_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 0);
        }
    }

    assert_true(int32_matrix_is_zero(mat));
    return_int32_matrix(mat);
}

/* =============================================================================
 * Group 2: init_int32_coo_matrix
 * ========================================================================== */

static void test_int32_coo_init_zero_capacity_fails(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int32_matrix_expect_t r = init_int32_coo_matrix(3u, 4u, 0u, true, a);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_int32_coo_init_returns_valid_matrix(void** state) {
    (void)state;

    int32_matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, true);

    assert_int_equal((int)matrix_rows(mat), 3);
    assert_int_equal((int)matrix_cols(mat), 4);
    assert_int_equal((int)matrix_dtype(mat), (int)INT32_TYPE);
    assert_int_equal((int)matrix_data_size(mat), (int)sizeof(int32_t));
    assert_int_equal((int)matrix_format(mat), (int)COO_MATRIX);
    assert_int_equal((int)matrix_nnz(mat), 0);
    assert_true(mat->rep.coo.growth);
    assert_true(mat->rep.coo.sorted);

    return_int32_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int32_coo_unset_entry_reads_back_as_zero(void** state) {
    (void)state;

    int32_matrix_t* mat = _make_coo_int32_matrix(3u, 4u, 8u, true);
    int32_t out = (int32_t)-99;

    assert_int_equal(get_int32_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_int32_matrix(mat);
}

/* =============================================================================
 * Group 3: dense set/get
 * ========================================================================== */

static void test_int32_set_dense_null_matrix_returns_null_pointer(void** state) {
    (void)state;
    assert_int_equal(set_int32_matrix(NULL, 0u, 0u, (int32_t)7), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_int32_get_dense_null_out_returns_null_pointer(void** state) {
    (void)state;
    int32_matrix_t* mat = _make_dense_int32_matrix(2u, 2u);

    assert_int_equal(get_int32_matrix(mat, 0u, 0u, NULL), NULL_POINTER);
    return_int32_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int32_set_get_dense_single_value(void** state) {
    (void)state;
    int32_matrix_t* mat = _make_dense_int32_matrix(3u, 3u);

    int32_t out = 0;
    assert_int_equal(set_int32_matrix(mat, 1u, 2u, (int32_t)42), NO_ERROR);
    assert_int_equal(get_int32_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 42);

    return_int32_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int32_set_dense_overwrites_existing_value(void** state) {
    (void)state;
    int32_matrix_t* mat = _make_dense_int32_matrix(2u, 2u);

    int32_t out = 0;
    assert_int_equal(set_int32_matrix(mat, 1u, 1u, (int32_t)5), NO_ERROR);
    assert_int_equal(set_int32_matrix(mat, 1u, 1u, (int32_t)-7), NO_ERROR);
    assert_int_equal(get_int32_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, -7);

    return_int32_matrix(mat);
}

/* =============================================================================
 * Group 4: COO set/get
 * ========================================================================== */

static void test_int32_set_get_coo_single_value(void** state) {
    (void)state;
    int32_matrix_t* mat = _make_coo_int32_matrix(3u, 3u, 4u, true);

    int32_t out = 0;
    assert_int_equal(set_int32_matrix(mat, 2u, 1u, (int32_t)-23), NO_ERROR);
    assert_int_equal(get_int32_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, -23);
    assert_int_equal((int)matrix_nnz(mat), 1);

    return_int32_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int32_push_back_coo_overwrite_does_not_increase_nnz(void** state) {
    (void)state;
    int32_matrix_t* mat = _make_coo_int32_matrix(3u, 3u, 4u, true);

    int32_t out = 0;
    assert_int_equal(push_back_int32_coo_matrix(mat, 1u, 1u, (int32_t)5), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(push_back_int32_coo_matrix(mat, 1u, 1u, (int32_t)-9), NO_ERROR);
    assert_int_equal((int)matrix_nnz(mat), 1);

    assert_int_equal(get_int32_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, -9);

    return_int32_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int32_push_back_coo_capacity_overflow_when_growth_disabled(void** state) {
    (void)state;
    int32_matrix_t* mat = _make_coo_int32_matrix(3u, 3u, 2u, false);

    assert_int_equal(push_back_int32_coo_matrix(mat, 0u, 0u, (int32_t)1), NO_ERROR);
    assert_int_equal(push_back_int32_coo_matrix(mat, 1u, 1u, (int32_t)-2), NO_ERROR);
    assert_int_equal(push_back_int32_coo_matrix(mat, 2u, 2u, (int32_t)3), CAPACITY_OVERFLOW);

    return_int32_matrix(mat);
}

/* =============================================================================
 * Group 5: clear / copy
 * ========================================================================== */

static void test_clear_int32_dense_zeroes_all_entries(void** state) {
    (void)state;
    int32_matrix_t* mat = _make_dense_int32_matrix(2u, 3u);
    int32_t out = (int32_t)-99;

    assert_int_equal(set_int32_matrix(mat, 0u, 0u, (int32_t)11), NO_ERROR);
    assert_int_equal(set_int32_matrix(mat, 0u, 2u, (int32_t)-22), NO_ERROR);
    assert_false(int32_matrix_is_zero(mat));

    assert_int_equal(clear_int32_matrix(mat), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            out = (int32_t)-99;
            assert_int_equal(get_int32_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, 0);
        }
    }

    assert_true(int32_matrix_is_zero(mat));
    return_int32_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_copy_int32_dense_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_matrix_t* src = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t r = copy_int32_matrix(src, a);
    assert_true(r.has_value);
    assert_true(int32_matrix_equal(src, r.u.value));

    return_int32_matrix(r.u.value);
    return_int32_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_copy_int32_coo_is_independent_of_source(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_matrix_t* src = _make_coo_int32_matrix(3u, 3u, 4u, true);

    assert_int_equal(set_int32_matrix(src, 1u, 1u, (int32_t)5), NO_ERROR);

    int32_matrix_expect_t r = copy_int32_matrix(src, a);
    assert_true(r.has_value);

    assert_int_equal(set_int32_matrix(r.u.value, 1u, 1u, (int32_t)-77), NO_ERROR);

    int32_t src_out = 0;
    int32_t dst_out = 0;

    assert_int_equal(get_int32_matrix(src, 1u, 1u, &src_out), NO_ERROR);
    assert_int_equal(get_int32_matrix(r.u.value, 1u, 1u, &dst_out), NO_ERROR);

    assert_int_equal((int)src_out, 5);
    assert_int_equal((int)dst_out, -77);

    return_int32_matrix(r.u.value);
    return_int32_matrix(src);
}

/* =============================================================================
 * Group 6: convert / convert_zero
 * ========================================================================== */

static void test_convert_int32_dense_to_coo_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_matrix_t* src = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t r = convert_int32_matrix(src, COO_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)COO_MATRIX);
    assert_true(int32_matrix_equal(src, r.u.value));

    return_int32_matrix(r.u.value);
    return_int32_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int32_dense_to_csr_preserves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_matrix_t* src = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t r = convert_int32_matrix(src, CSR_MATRIX, a);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_format(r.u.value), (int)CSR_MATRIX);
    assert_true(int32_matrix_equal(src, r.u.value));

    return_int32_matrix(r.u.value);
    return_int32_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int32_zero_dense_to_csr_omits_neg_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_matrix_t* src = _make_dense_int32_matrix(2u, 3u);

    assert_int_equal(set_int32_matrix(src, 0u, 0u, (int32_t)5), NO_ERROR);
    assert_int_equal(set_int32_matrix(src, 0u, 1u, (int32_t)-1), NO_ERROR);
    assert_int_equal(set_int32_matrix(src, 1u, 2u, (int32_t)8), NO_ERROR);

    int32_matrix_expect_t r =
        convert_int32_matrix_zero(src, CSR_MATRIX, a, _int32_zero_or_neg_one);
    assert_true(r.has_value);
    assert_int_equal((int)matrix_nnz(r.u.value), 2);

    return_int32_matrix(r.u.value);
    return_int32_matrix(src);
}

/* =============================================================================
 * Group 7: transpose / fill
 * ========================================================================== */

static void test_transpose_int32_dense_moves_values(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();
    int32_matrix_t* src = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t r = transpose_int32_matrix(src, a);
    assert_true(r.has_value);

    int32_t out = 0;
    assert_int_equal(get_int32_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_int32_matrix(r.u.value, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    return_int32_matrix(r.u.value);
    return_int32_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_fill_int32_dense_sets_every_entry(void** state) {
    (void)state;
    int32_matrix_t* mat = _make_dense_int32_matrix(2u, 3u);
    int32_t out = 0;

    assert_int_equal(fill_int32_matrix(mat, (int32_t)-7), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            assert_int_equal(get_int32_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal((int)out, -7);
        }
    }

    return_int32_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_fill_int32_zero_equivalent_to_clear(void** state) {
    (void)state;
    int32_matrix_t* mat = _make_dense_int32_matrix(2u, 2u);

    assert_int_equal(set_int32_matrix(mat, 1u, 1u, (int32_t)9), NO_ERROR);
    assert_false(int32_matrix_is_zero(mat));

    assert_int_equal(fill_int32_matrix(mat, (int32_t)0), NO_ERROR);
    assert_true(int32_matrix_is_zero(mat));

    return_int32_matrix(mat);
}

/* =============================================================================
 * Group 8: equality / compatibility
 * ========================================================================== */

static void test_int32_matrix_equal_dense_same_values_returns_true(void** state) {
    (void)state;

    int32_matrix_t* a = _make_sample_dense_int32_matrix();
    int32_matrix_t* b = _make_sample_dense_int32_matrix();

    assert_true(int32_matrix_equal(a, b));

    return_int32_matrix(a);
    return_int32_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_int32_matrix_shape_compatibility_helpers(void** state) {
    (void)state;

    int32_matrix_t* a = _make_dense_int32_matrix(2u, 3u);
    int32_matrix_t* b = _make_dense_int32_matrix(2u, 3u);
    int32_matrix_t* c = _make_dense_int32_matrix(3u, 4u);

    assert_true(int32_matrix_has_same_shape(a, b));
    assert_true(int32_matrix_is_add_compatible(a, b));
    assert_true(int32_matrix_is_multiply_compatible(a, c));
    assert_false(int32_matrix_is_square(a));

    return_int32_matrix(a);
    return_int32_matrix(b);
    return_int32_matrix(c);
}

/* =============================================================================
 * Group 9: swaps / constructors / vector queries
 * ========================================================================== */

static void test_swap_int32_matrix_rows_dense(void** state) {
    (void)state;
    int32_matrix_t* mat = _make_dense_int32_matrix(2u, 2u);
    int32_t out = 0;

    assert_int_equal(set_int32_matrix(mat, 0u, 0u, (int32_t)1), NO_ERROR);
    assert_int_equal(set_int32_matrix(mat, 1u, 0u, (int32_t)-2), NO_ERROR);

    assert_int_equal(swap_int32_matrix_rows(mat, 0u, 1u), NO_ERROR);

    assert_int_equal(get_int32_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, -2);

    assert_int_equal(get_int32_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    return_int32_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_init_int32_identity_matrix_sets_diagonal_to_one(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int32_matrix_expect_t r = init_int32_identity_matrix(3u, a);
    assert_true(r.has_value);

    int32_t out = 0;
    assert_int_equal(get_int32_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    assert_int_equal(get_int32_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    return_int32_matrix(r.u.value);
}

// --------------------------------------------------------------------------------

static void test_int32_row_and_col_vector_queries(void** state) {
    (void)state;
    allocator_vtable_t a = heap_allocator();

    int32_matrix_expect_t row = init_int32_row_vector(5u, a);
    int32_matrix_expect_t col = init_int32_col_vector(7u, a);

    assert_true(row.has_value);
    assert_true(col.has_value);

    assert_true(int32_matrix_is_row_vector(row.u.value));
    assert_true(int32_matrix_is_vector(row.u.value));
    assert_int_equal((int)int32_matrix_vector_length(row.u.value), 5);

    assert_true(int32_matrix_is_col_vector(col.u.value));
    assert_true(int32_matrix_is_vector(col.u.value));
    assert_int_equal((int)int32_matrix_vector_length(col.u.value), 7);

    return_int32_matrix(row.u.value);
    return_int32_matrix(col.u.value);
}

/* =============================================================================
 * Group 10: Additional int32 matrix tests
 * ========================================================================== */

static void test_transpose_int32_coo_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int32_matrix_t* src = _make_sample_coo_int32_matrix();

    int32_matrix_expect_t r = transpose_int32_matrix(src, alloc);
    assert_true(r.has_value);

    int32_matrix_t* tr = r.u.value;
    int32_t out = 0;

    assert_int_equal((int)matrix_format(tr), (int)COO_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_int32_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_int32_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_int32_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_int32_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_int32_matrix(tr);
    return_int32_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_transpose_int32_csr_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int32_matrix_t* dense = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t src_r = convert_int32_matrix(dense, CSR_MATRIX, alloc);
    assert_true(src_r.has_value);

    int32_matrix_expect_t tr_r = transpose_int32_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    int32_matrix_t* tr = tr_r.u.value;
    int32_t out = 0;

    assert_int_equal((int)matrix_format(tr), (int)CSR_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_int32_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_int32_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_int32_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_int32_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_int32_matrix(tr);
    return_int32_matrix(src_r.u.value);
    return_int32_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_transpose_int32_csc_preserves_logical_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int32_matrix_t* dense = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t src_r = convert_int32_matrix(dense, CSC_MATRIX, alloc);
    assert_true(src_r.has_value);

    int32_matrix_expect_t tr_r = transpose_int32_matrix(src_r.u.value, alloc);
    assert_true(tr_r.has_value);

    int32_matrix_t* tr = tr_r.u.value;
    int32_t out = 0;

    assert_int_equal((int)matrix_format(tr), (int)CSC_MATRIX);
    assert_int_equal((int)matrix_rows(tr), 4);
    assert_int_equal((int)matrix_cols(tr), 3);

    assert_int_equal(get_int32_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 10);

    assert_int_equal(get_int32_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 20);

    assert_int_equal(get_int32_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 30);

    assert_int_equal(get_int32_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 40);

    return_int32_matrix(tr);
    return_int32_matrix(src_r.u.value);
    return_int32_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_swap_int32_matrix_cols_dense(void** state) {
    (void)state;

    int32_matrix_t* mat = _make_dense_int32_matrix(2u, 3u);
    int32_t out = 0;

    assert_int_equal(set_int32_matrix(mat, 0u, 0u, (int32_t)1), NO_ERROR);
    assert_int_equal(set_int32_matrix(mat, 0u, 2u, (int32_t)-3), NO_ERROR);
    assert_int_equal(set_int32_matrix(mat, 1u, 0u, (int32_t)4), NO_ERROR);
    assert_int_equal(set_int32_matrix(mat, 1u, 2u, (int32_t)-6), NO_ERROR);

    assert_int_equal(swap_int32_matrix_cols(mat, 0u, 2u), NO_ERROR);

    assert_int_equal(get_int32_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, -3);

    assert_int_equal(get_int32_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    assert_int_equal(get_int32_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, -6);

    assert_int_equal(get_int32_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal((int)out, 4);

    return_int32_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int32_matrix_is_sparse_dense_returns_false(void** state) {
    (void)state;

    int32_matrix_t* mat = _make_dense_int32_matrix(3u, 3u);
    assert_false(int32_matrix_is_sparse(mat));

    return_int32_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int32_matrix_is_sparse_coo_returns_true(void** state) {
    (void)state;

    int32_matrix_t* mat = _make_coo_int32_matrix(3u, 3u, 4u, true);
    assert_true(int32_matrix_is_sparse(mat));

    return_int32_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_int32_matrix_is_sparse_csr_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int32_matrix_t* dense = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t r = convert_int32_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(int32_matrix_is_sparse(r.u.value));

    return_int32_matrix(r.u.value);
    return_int32_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_int32_matrix_is_sparse_csc_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int32_matrix_t* dense = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t r = convert_int32_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);
    assert_true(int32_matrix_is_sparse(r.u.value));

    return_int32_matrix(r.u.value);
    return_int32_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_int32_matrix_equal_dense_and_coo_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int32_matrix_t* dense = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t r = convert_int32_matrix(dense, COO_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(int32_matrix_equal(dense, r.u.value));
    assert_true(int32_matrix_equal(r.u.value, dense));

    return_int32_matrix(r.u.value);
    return_int32_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_int32_matrix_equal_dense_and_csr_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int32_matrix_t* dense = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t r = convert_int32_matrix(dense, CSR_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(int32_matrix_equal(dense, r.u.value));
    assert_true(int32_matrix_equal(r.u.value, dense));

    return_int32_matrix(r.u.value);
    return_int32_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_int32_matrix_equal_dense_and_csc_same_values_returns_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int32_matrix_t* dense = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t r = convert_int32_matrix(dense, CSC_MATRIX, alloc);
    assert_true(r.has_value);

    assert_true(int32_matrix_equal(dense, r.u.value));
    assert_true(int32_matrix_equal(r.u.value, dense));

    return_int32_matrix(r.u.value);
    return_int32_matrix(dense);
}

// --------------------------------------------------------------------------------

static void test_convert_int32_matrix_zero_null_callback_matches_convert_int32_matrix_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int32_matrix_t* src = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t r1 = convert_int32_matrix(src, COO_MATRIX, alloc);
    int32_matrix_expect_t r2 = convert_int32_matrix_zero(src, COO_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(int32_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_int32_matrix(r2.u.value);
    return_int32_matrix(r1.u.value);
    return_int32_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int32_matrix_zero_null_callback_matches_convert_int32_matrix_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int32_matrix_t* src = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t r1 = convert_int32_matrix(src, CSR_MATRIX, alloc);
    int32_matrix_expect_t r2 = convert_int32_matrix_zero(src, CSR_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(int32_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_int32_matrix(r2.u.value);
    return_int32_matrix(r1.u.value);
    return_int32_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int32_matrix_zero_null_callback_matches_convert_int32_matrix_csc(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int32_matrix_t* src = _make_sample_dense_int32_matrix();

    int32_matrix_expect_t r1 = convert_int32_matrix(src, CSC_MATRIX, alloc);
    int32_matrix_expect_t r2 = convert_int32_matrix_zero(src, CSC_MATRIX, alloc, NULL);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(int32_matrix_equal(r1.u.value, r2.u.value));
    assert_int_equal((int)matrix_nnz(r1.u.value), (int)matrix_nnz(r2.u.value));

    return_int32_matrix(r2.u.value);
    return_int32_matrix(r1.u.value);
    return_int32_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int32_matrix_zero_default_omits_zero_in_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int32_matrix_t* src = _make_dense_int32_matrix(2u, 2u);

    assert_int_equal(set_int32_matrix(src, 0u, 0u, (int32_t)0), NO_ERROR);
    assert_int_equal(set_int32_matrix(src, 1u, 1u, (int32_t)1), NO_ERROR);

    int32_matrix_expect_t r = convert_int32_matrix_zero(src, COO_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    int32_t out = (int32_t)99;
    assert_int_equal(get_int32_matrix(r.u.value, 0u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    assert_int_equal(get_int32_matrix(r.u.value, 1u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 1);

    return_int32_matrix(r.u.value);
    return_int32_matrix(src);
}

// --------------------------------------------------------------------------------

static void test_convert_int32_matrix_zero_default_omits_zero_in_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    int32_matrix_t* src = _make_dense_int32_matrix(2u, 2u);

    assert_int_equal(set_int32_matrix(src, 0u, 1u, (int32_t)0), NO_ERROR);
    assert_int_equal(set_int32_matrix(src, 1u, 0u, (int32_t)-2), NO_ERROR);

    int32_matrix_expect_t r = convert_int32_matrix_zero(src, CSR_MATRIX, alloc, NULL);
    assert_true(r.has_value);

    assert_int_equal((int)matrix_nnz(r.u.value), 1);

    int32_t out = 0;
    assert_int_equal(get_int32_matrix(r.u.value, 0u, 1u, &out), NO_ERROR);
    assert_int_equal((int)out, 0);

    assert_int_equal(get_int32_matrix(r.u.value, 1u, 0u, &out), NO_ERROR);
    assert_int_equal((int)out, -2);

    return_int32_matrix(r.u.value);
    return_int32_matrix(src);
}

/* =============================================================================
 * Registry
 * ========================================================================== */

const struct CMUnitTest test_int32_matrix[] = {
    /* Group 1 */
    cmocka_unit_test(test_int32_dense_init_zero_rows_fails),
    cmocka_unit_test(test_int32_dense_init_zero_cols_fails),
    cmocka_unit_test(test_int32_dense_init_returns_valid_matrix),
    cmocka_unit_test(test_int32_dense_init_is_zero_initialized),

    /* Group 2 */
    cmocka_unit_test(test_int32_coo_init_zero_capacity_fails),
    cmocka_unit_test(test_int32_coo_init_returns_valid_matrix),
    cmocka_unit_test(test_int32_coo_unset_entry_reads_back_as_zero),

    /* Group 3 */
    cmocka_unit_test(test_int32_set_dense_null_matrix_returns_null_pointer),
    cmocka_unit_test(test_int32_get_dense_null_out_returns_null_pointer),
    cmocka_unit_test(test_int32_set_get_dense_single_value),
    cmocka_unit_test(test_int32_set_dense_overwrites_existing_value),

    /* Group 4 */
    cmocka_unit_test(test_int32_set_get_coo_single_value),
    cmocka_unit_test(test_int32_push_back_coo_overwrite_does_not_increase_nnz),
    cmocka_unit_test(test_int32_push_back_coo_capacity_overflow_when_growth_disabled),

    /* Group 5 */
    cmocka_unit_test(test_clear_int32_dense_zeroes_all_entries),
    cmocka_unit_test(test_copy_int32_dense_preserves_values),
    cmocka_unit_test(test_copy_int32_coo_is_independent_of_source),

    /* Group 6 */
    cmocka_unit_test(test_convert_int32_dense_to_coo_preserves_values),
    cmocka_unit_test(test_convert_int32_dense_to_csr_preserves_values),
    cmocka_unit_test(test_convert_int32_zero_dense_to_csr_omits_neg_one),

    /* Group 7 */
    cmocka_unit_test(test_transpose_int32_dense_moves_values),
    cmocka_unit_test(test_fill_int32_dense_sets_every_entry),
    cmocka_unit_test(test_fill_int32_zero_equivalent_to_clear),

    /* Group 8 */
    cmocka_unit_test(test_int32_matrix_equal_dense_same_values_returns_true),
    cmocka_unit_test(test_int32_matrix_shape_compatibility_helpers),

    /* Group 9 */
    cmocka_unit_test(test_swap_int32_matrix_rows_dense),
    cmocka_unit_test(test_init_int32_identity_matrix_sets_diagonal_to_one),
    cmocka_unit_test(test_int32_row_and_col_vector_queries),

    /* Group 10 */
    cmocka_unit_test(test_transpose_int32_coo_preserves_logical_values),
    cmocka_unit_test(test_transpose_int32_csr_preserves_logical_values),
    cmocka_unit_test(test_transpose_int32_csc_preserves_logical_values),

    cmocka_unit_test(test_swap_int32_matrix_cols_dense),

    cmocka_unit_test(test_int32_matrix_is_sparse_dense_returns_false),
    cmocka_unit_test(test_int32_matrix_is_sparse_coo_returns_true),
    cmocka_unit_test(test_int32_matrix_is_sparse_csr_returns_true),
    cmocka_unit_test(test_int32_matrix_is_sparse_csc_returns_true),

    cmocka_unit_test(test_int32_matrix_equal_dense_and_coo_same_values_returns_true),
    cmocka_unit_test(test_int32_matrix_equal_dense_and_csr_same_values_returns_true),
    cmocka_unit_test(test_int32_matrix_equal_dense_and_csc_same_values_returns_true),

    cmocka_unit_test(test_convert_int32_matrix_zero_null_callback_matches_convert_int32_matrix_coo),
    cmocka_unit_test(test_convert_int32_matrix_zero_null_callback_matches_convert_int32_matrix_csr),
    cmocka_unit_test(test_convert_int32_matrix_zero_null_callback_matches_convert_int32_matrix_csc),

    cmocka_unit_test(test_convert_int32_matrix_zero_default_omits_zero_in_coo),
    cmocka_unit_test(test_convert_int32_matrix_zero_default_omits_zero_in_csr),
};

const size_t test_int32_matrix_count =
    sizeof(test_int32_matrix) / sizeof(test_int32_matrix[0]);
// ================================================================================
// ================================================================================
// eof
