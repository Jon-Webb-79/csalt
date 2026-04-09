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
// #include "c_float.h"
// #include "c_double.h"
// #include "c_ldouble.h"
// #include "c_uint8.h"
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

// --------------------------------------------------------------------------------

static void test_matrix_equal_cmp_null_matrices_returns_false(void** state) {
    (void)state;
    assert_false(matrix_equal_cmp(NULL, NULL, _int32_abs_equal));
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_cmp_with_null_comparator_falls_back_to_bytewise(void** state) {
    (void)state;

    matrix_t* a = _make_sample_dense_int32_matrix();
    matrix_t* b = _make_sample_dense_int32_matrix();

    assert_true(matrix_equal_cmp(a, b, NULL));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_cmp_abs_equal_accepts_negated_values(void** state) {
    (void)state;

    matrix_t* a = _make_dense_int32_matrix(2u, 2u);
    matrix_t* b = _make_dense_int32_matrix(2u, 2u);

    int32_t va = -5;
    int32_t vb =  5;
    int32_t vc = -9;
    int32_t vd =  9;

    assert_int_equal(set_matrix(a, 0u, 0u, &va), NO_ERROR);
    assert_int_equal(set_matrix(b, 0u, 0u, &vb), NO_ERROR);
    assert_int_equal(set_matrix(a, 1u, 1u, &vc), NO_ERROR);
    assert_int_equal(set_matrix(b, 1u, 1u, &vd), NO_ERROR);

    assert_false(matrix_equal(a, b));
    assert_true(matrix_equal_cmp(a, b, _int32_abs_equal));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_custom_struct_bytewise_equal_returns_true(void** state) {
    (void)state;

    matrix_t* a = _make_dense_record_matrix(2u, 2u);
    matrix_t* b = _make_dense_record_matrix(2u, 2u);

    test_record_t r1 = { 17, 3.5, 0xA5u };

    assert_int_equal(set_matrix(a, 1u, 0u, &r1), NO_ERROR);
    assert_int_equal(set_matrix(b, 1u, 0u, &r1), NO_ERROR);

    assert_true(matrix_equal(a, b));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_custom_struct_bytewise_different_returns_false(void** state) {
    (void)state;

    matrix_t* a = _make_dense_record_matrix(2u, 2u);
    matrix_t* b = _make_dense_record_matrix(2u, 2u);

    test_record_t r1 = { 17, 3.5, 0xA5u };
    test_record_t r2 = { 17, 9.5, 0x00u };   /* same id, different other fields */

    assert_int_equal(set_matrix(a, 1u, 0u, &r1), NO_ERROR);
    assert_int_equal(set_matrix(b, 1u, 0u, &r2), NO_ERROR);

    assert_false(matrix_equal(a, b));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_cmp_custom_struct_by_id_returns_true(void** state) {
    (void)state;

    matrix_t* a = _make_dense_record_matrix(2u, 2u);
    matrix_t* b = _make_dense_record_matrix(2u, 2u);

    test_record_t r1 = { 17, 3.5, 0xA5u };
    test_record_t r2 = { 17, 9.5, 0x00u };   /* same id */

    assert_int_equal(set_matrix(a, 1u, 0u, &r1), NO_ERROR);
    assert_int_equal(set_matrix(b, 1u, 0u, &r2), NO_ERROR);

    assert_true(matrix_equal_cmp(a, b, _record_equal_by_id));

    return_matrix(a);
    return_matrix(b);
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
    cmocka_unit_test(test_matrix_equal_cmp_null_matrices_returns_false),
    cmocka_unit_test(test_matrix_equal_cmp_with_null_comparator_falls_back_to_bytewise),
    cmocka_unit_test(test_matrix_equal_cmp_abs_equal_accepts_negated_values),
    cmocka_unit_test(test_matrix_equal_custom_struct_bytewise_equal_returns_true),
    cmocka_unit_test(test_matrix_equal_custom_struct_bytewise_different_returns_false),
    cmocka_unit_test(test_matrix_equal_cmp_custom_struct_by_id_returns_true),

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

/* Exact float equality for binary fractions used in tests. */
// #define ASSERT_FLOAT_EXACT(a, b) assert_true((a) == (b))
//  
// // ================================================================================
// // Group 1: init_float_dense_matrix
// // ================================================================================
//  
// static void test_dense_init_null_allocator_fails(void** state) {
//     (void)state;
//     allocator_vtable_t bad = { 0 };
//     float_matrix_expect_t r = init_float_dense_matrix(3, 4, bad);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_dense_init_zero_rows_fails(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(0, 4, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, INVALID_ARG);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_dense_init_zero_cols_fails(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(3, 0, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, INVALID_ARG);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_dense_init_returns_valid_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(3, 4, alloc);
//     assert_true(r.has_value);
//     assert_non_null(r.u.value);
//     assert_int_equal((int)float_matrix_rows(r.u.value), 3);
//     assert_int_equal((int)float_matrix_cols(r.u.value), 4);
//     assert_int_equal((int)float_matrix_format(r.u.value), DENSE_MATRIX);
//     return_float_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_dense_init_elements_are_zero(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     float val = -1.0f;
//     for (size_t i = 0; i < 2; ++i) {
//         for (size_t j = 0; j < 3; ++j) {
//             assert_int_equal(get_float_matrix(m, i, j, &val), NO_ERROR);
//             ASSERT_FLOAT_EXACT(val, 0.0f);
//         }
//     }
//  
//     return_float_matrix(m);
// }
//  
// // ================================================================================
// // Group 2: init_float_coo_matrix
// // ================================================================================
//  
// static void test_coo_init_returns_valid_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_coo_matrix(10, 10, 8, true, alloc);
//     assert_true(r.has_value);
//     assert_int_equal((int)float_matrix_rows(r.u.value), 10);
//     assert_int_equal((int)float_matrix_cols(r.u.value), 10);
//     assert_int_equal((int)float_matrix_format(r.u.value), COO_MATRIX);
//     assert_int_equal((int)float_matrix_nnz(r.u.value), 0);
//     return_float_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_coo_init_zero_capacity_fails(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_coo_matrix(5, 5, 0, true, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, INVALID_ARG);
// }
//  
// // ================================================================================
// // Group 3: return_float_matrix
// // ================================================================================
//  
// static void test_return_null_is_safe(void** state) {
//     (void)state;
//     return_float_matrix(NULL);
// }
//  
// // ================================================================================
// // Group 4: get/set_float_matrix (dense)
// // ================================================================================
//  
// static void test_get_null_matrix_returns_null_pointer(void** state) {
//     (void)state;
//     float val = 0.0f;
//     assert_int_equal(get_float_matrix(NULL, 0, 0, &val), NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_get_null_out_returns_null_pointer(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     assert_int_equal(get_float_matrix(r.u.value, 0, 0, NULL), NULL_POINTER);
//     return_float_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_set_null_matrix_returns_null_pointer(void** state) {
//     (void)state;
//     assert_int_equal(set_float_matrix(NULL, 0, 0, 1.0f), NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_set_and_get_round_trip(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     assert_int_equal(set_float_matrix(m, 0, 0, 1.5f), NO_ERROR);
//     assert_int_equal(set_float_matrix(m, 1, 2, -3.0f), NO_ERROR);
//     assert_int_equal(set_float_matrix(m, 2, 1, 42.0f), NO_ERROR);
//  
//     float val = 0.0f;
//     get_float_matrix(m, 0, 0, &val); ASSERT_FLOAT_EXACT(val, 1.5f);
//     get_float_matrix(m, 1, 2, &val); ASSERT_FLOAT_EXACT(val, -3.0f);
//     get_float_matrix(m, 2, 1, &val); ASSERT_FLOAT_EXACT(val, 42.0f);
//  
//     /* Unset element should still be zero */
//     get_float_matrix(m, 0, 1, &val); ASSERT_FLOAT_EXACT(val, 0.0f);
//  
//     return_float_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_set_out_of_bounds_returns_error(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//  
//     assert_int_equal(set_float_matrix(r.u.value, 2, 0, 1.0f), INVALID_ARG);
//     assert_int_equal(set_float_matrix(r.u.value, 0, 3, 1.0f), INVALID_ARG);
//  
//     return_float_matrix(r.u.value);
// }
//  
// // ================================================================================
// // Group 5: COO assembly (push_back, sort, reserve)
// // ================================================================================
//  
// static void test_coo_push_back_and_retrieve(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_coo_matrix(5, 5, 4, true, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     assert_int_equal(push_back_float_coo_matrix(m, 0, 2, 3.14f), NO_ERROR);
//     assert_int_equal(push_back_float_coo_matrix(m, 3, 4, -1.0f), NO_ERROR);
//     assert_int_equal((int)float_matrix_nnz(m), 2);
//  
//     float val = 0.0f;
//     get_float_matrix(m, 0, 2, &val); ASSERT_FLOAT_EXACT(val, 3.14f);
//     get_float_matrix(m, 3, 4, &val); ASSERT_FLOAT_EXACT(val, -1.0f);
//  
//     /* Unset entry returns zero */
//     get_float_matrix(m, 1, 1, &val); ASSERT_FLOAT_EXACT(val, 0.0f);
//  
//     return_float_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_coo_push_back_overwrites_existing(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_coo_matrix(3, 3, 4, true, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     push_back_float_coo_matrix(m, 1, 1, 10.0f);
//     push_back_float_coo_matrix(m, 1, 1, 20.0f);  /* overwrite */
//     assert_int_equal((int)float_matrix_nnz(m), 1);
//  
//     float val = 0.0f;
//     get_float_matrix(m, 1, 1, &val);
//     ASSERT_FLOAT_EXACT(val, 20.0f);
//  
//     return_float_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_coo_sort_orders_entries(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_coo_matrix(4, 4, 8, true, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     /* Insert out of order */
//     push_back_float_coo_matrix(m, 3, 0, 4.0f);
//     push_back_float_coo_matrix(m, 0, 1, 1.0f);
//     push_back_float_coo_matrix(m, 1, 2, 2.0f);
//  
//     assert_int_equal(sort_float_coo_matrix(m), NO_ERROR);
//  
//     /* Values should still be retrievable after sort */
//     float val = 0.0f;
//     get_float_matrix(m, 0, 1, &val); ASSERT_FLOAT_EXACT(val, 1.0f);
//     get_float_matrix(m, 1, 2, &val); ASSERT_FLOAT_EXACT(val, 2.0f);
//     get_float_matrix(m, 3, 0, &val); ASSERT_FLOAT_EXACT(val, 4.0f);
//  
//     return_float_matrix(m);
// }
//  
// // ================================================================================
// // Group 6: clear_float_matrix
// // ================================================================================
//  
// static void test_clear_dense_zeros_all_elements(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     set_float_matrix(m, 0, 0, 5.0f);
//     set_float_matrix(m, 1, 1, 9.0f);
//     assert_int_equal(clear_float_matrix(m), NO_ERROR);
//  
//     assert_true(is_float_matrix_zero(m));
//  
//     return_float_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_clear_coo_resets_nnz(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_coo_matrix(3, 3, 4, true, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     push_back_float_coo_matrix(m, 0, 0, 1.0f);
//     push_back_float_coo_matrix(m, 1, 1, 2.0f);
//     assert_int_equal(clear_float_matrix(m), NO_ERROR);
//     assert_int_equal((int)float_matrix_nnz(m), 0);
//  
//     return_float_matrix(m);
// }
//  
// // ================================================================================
// // Group 7: copy_float_matrix
// // ================================================================================
//  
// static void test_copy_null_returns_error(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = copy_float_matrix(NULL, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_copy_dense_produces_independent_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* src = r.u.value;
//  
//     set_float_matrix(src, 0, 0, 1.0f);
//     set_float_matrix(src, 1, 2, 7.0f);
//  
//     float_matrix_expect_t cr = copy_float_matrix(src, alloc);
//     assert_true(cr.has_value);
//     float_matrix_t* dst = cr.u.value;
//  
//     /* Mutating src must not affect copy */
//     set_float_matrix(src, 0, 0, 999.0f);
//  
//     float val = 0.0f;
//     get_float_matrix(dst, 0, 0, &val); ASSERT_FLOAT_EXACT(val, 1.0f);
//     get_float_matrix(dst, 1, 2, &val); ASSERT_FLOAT_EXACT(val, 7.0f);
//  
//     return_float_matrix(src);
//     return_float_matrix(dst);
// }
//  
// // ================================================================================
// // Group 8: fill_float_matrix (SIMD-accelerated)
// // ================================================================================
//  
// static void test_fill_null_returns_error(void** state) {
//     (void)state;
//     assert_int_equal(fill_float_matrix(NULL, 1.0f), NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_fill_dense_sets_all_elements(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(4, 5, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     assert_int_equal(fill_float_matrix(m, 3.5f), NO_ERROR);
//  
//     float val = 0.0f;
//     for (size_t i = 0; i < 4; ++i) {
//         for (size_t j = 0; j < 5; ++j) {
//             get_float_matrix(m, i, j, &val);
//             ASSERT_FLOAT_EXACT(val, 3.5f);
//         }
//     }
//  
//     return_float_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_fill_zero_clears_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     fill_float_matrix(m, 5.0f);
//     assert_int_equal(fill_float_matrix(m, 0.0f), NO_ERROR);
//     assert_true(is_float_matrix_zero(m));
//  
//     return_float_matrix(m);
// }
//  
// // ================================================================================
// // Group 9: is_float_matrix_zero (SIMD-accelerated)
// // ================================================================================
//  
// static void test_is_zero_null_returns_true(void** state) {
//     (void)state;
//     assert_true(is_float_matrix_zero(NULL));
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_is_zero_freshly_initialized_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(5, 5, alloc);
//     assert_true(r.has_value);
//     assert_true(is_float_matrix_zero(r.u.value));
//     return_float_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_is_zero_returns_false_after_set(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     set_float_matrix(m, 1, 1, 0.001f);
//     assert_false(is_float_matrix_zero(m));
//  
//     return_float_matrix(m);
// }
//  
// // ================================================================================
// // Group 10: float_matrix_equal (SIMD-accelerated)
// // ================================================================================
//  
// static void test_equal_null_returns_false(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//  
//     assert_false(float_matrix_equal(NULL, r.u.value));
//     assert_false(float_matrix_equal(r.u.value, NULL));
//  
//     return_float_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_equal_identical_matrices(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* a = r.u.value;
//  
//     set_float_matrix(a, 0, 0, 1.0f);
//     set_float_matrix(a, 1, 2, -4.5f);
//  
//     float_matrix_expect_t cr = copy_float_matrix(a, alloc);
//     assert_true(cr.has_value);
//  
//     assert_true(float_matrix_equal(a, cr.u.value));
//  
//     return_float_matrix(a);
//     return_float_matrix(cr.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_equal_different_values_returns_false(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t ra = init_float_dense_matrix(2, 2, alloc);
//     float_matrix_expect_t rb = init_float_dense_matrix(2, 2, alloc);
//     assert_true(ra.has_value);
//     assert_true(rb.has_value);
//  
//     set_float_matrix(ra.u.value, 0, 0, 1.0f);
//     set_float_matrix(rb.u.value, 0, 0, 2.0f);
//  
//     assert_false(float_matrix_equal(ra.u.value, rb.u.value));
//  
//     return_float_matrix(ra.u.value);
//     return_float_matrix(rb.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_equal_different_shape_returns_false(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t ra = init_float_dense_matrix(2, 3, alloc);
//     float_matrix_expect_t rb = init_float_dense_matrix(3, 2, alloc);
//     assert_true(ra.has_value);
//     assert_true(rb.has_value);
//  
//     assert_false(float_matrix_equal(ra.u.value, rb.u.value));
//  
//     return_float_matrix(ra.u.value);
//     return_float_matrix(rb.u.value);
// }
//  
// // ================================================================================
// // Group 11: transpose_float_matrix (SIMD-accelerated)
// // ================================================================================
//  
// static void test_transpose_null_returns_error(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = transpose_float_matrix(NULL, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_transpose_dense_swaps_dimensions(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     /* Fill with distinct values: m[i][j] = (i+1)*10 + j */
//     set_float_matrix(m, 0, 0, 10.0f);
//     set_float_matrix(m, 0, 1, 11.0f);
//     set_float_matrix(m, 0, 2, 12.0f);
//     set_float_matrix(m, 1, 0, 20.0f);
//     set_float_matrix(m, 1, 1, 21.0f);
//     set_float_matrix(m, 1, 2, 22.0f);
//  
//     float_matrix_expect_t tr = transpose_float_matrix(m, alloc);
//     assert_true(tr.has_value);
//     float_matrix_t* t = tr.u.value;
//  
//     assert_int_equal((int)float_matrix_rows(t), 3);
//     assert_int_equal((int)float_matrix_cols(t), 2);
//  
//     float val = 0.0f;
//     get_float_matrix(t, 0, 0, &val); ASSERT_FLOAT_EXACT(val, 10.0f);
//     get_float_matrix(t, 0, 1, &val); ASSERT_FLOAT_EXACT(val, 20.0f);
//     get_float_matrix(t, 1, 0, &val); ASSERT_FLOAT_EXACT(val, 11.0f);
//     get_float_matrix(t, 1, 1, &val); ASSERT_FLOAT_EXACT(val, 21.0f);
//     get_float_matrix(t, 2, 0, &val); ASSERT_FLOAT_EXACT(val, 12.0f);
//     get_float_matrix(t, 2, 1, &val); ASSERT_FLOAT_EXACT(val, 22.0f);
//  
//     return_float_matrix(m);
//     return_float_matrix(t);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_transpose_square_is_correct(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     set_float_matrix(m, 0, 1, 5.0f);
//     set_float_matrix(m, 1, 0, 7.0f);
//  
//     float_matrix_expect_t tr = transpose_float_matrix(m, alloc);
//     assert_true(tr.has_value);
//  
//     float val = 0.0f;
//     get_float_matrix(tr.u.value, 0, 1, &val); ASSERT_FLOAT_EXACT(val, 7.0f);
//     get_float_matrix(tr.u.value, 1, 0, &val); ASSERT_FLOAT_EXACT(val, 5.0f);
//  
//     return_float_matrix(m);
//     return_float_matrix(tr.u.value);
// }
//  
// // ================================================================================
// // Group 12: convert_float_matrix dense→CSR (SIMD-accelerated)
// // ================================================================================
//  
// static void test_convert_dense_to_csr_preserves_values(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     set_float_matrix(m, 0, 0, 1.0f);
//     set_float_matrix(m, 0, 2, 2.0f);
//     set_float_matrix(m, 1, 1, 3.0f);
//     set_float_matrix(m, 2, 0, 4.0f);
//     set_float_matrix(m, 2, 2, 5.0f);
//  
//     float_matrix_expect_t cr = convert_float_matrix(m, CSR_MATRIX, alloc);
//     assert_true(cr.has_value);
//     float_matrix_t* csr = cr.u.value;
//  
//     assert_int_equal((int)float_matrix_format(csr), CSR_MATRIX);
//     assert_int_equal((int)float_matrix_rows(csr), 3);
//     assert_int_equal((int)float_matrix_cols(csr), 3);
//     assert_int_equal((int)float_matrix_nnz(csr), 5);
//  
//     float val = 0.0f;
//     get_float_matrix(csr, 0, 0, &val); ASSERT_FLOAT_EXACT(val, 1.0f);
//     get_float_matrix(csr, 0, 2, &val); ASSERT_FLOAT_EXACT(val, 2.0f);
//     get_float_matrix(csr, 1, 1, &val); ASSERT_FLOAT_EXACT(val, 3.0f);
//     get_float_matrix(csr, 2, 0, &val); ASSERT_FLOAT_EXACT(val, 4.0f);
//     get_float_matrix(csr, 2, 2, &val); ASSERT_FLOAT_EXACT(val, 5.0f);
//  
//     /* Zero entries must read as zero */
//     get_float_matrix(csr, 0, 1, &val); ASSERT_FLOAT_EXACT(val, 0.0f);
//     get_float_matrix(csr, 1, 0, &val); ASSERT_FLOAT_EXACT(val, 0.0f);
//  
//     return_float_matrix(m);
//     return_float_matrix(csr);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_convert_all_zero_dense_to_csr(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//  
//     float_matrix_expect_t cr = convert_float_matrix(r.u.value, CSR_MATRIX, alloc);
//     assert_true(cr.has_value);
//     assert_int_equal((int)float_matrix_nnz(cr.u.value), 0);
//  
//     return_float_matrix(r.u.value);
//     return_float_matrix(cr.u.value);
// }
//  
// // ================================================================================
// // Group 13: Introspection
// // ================================================================================
//  
// static void test_introspection_null_returns_defaults(void** state) {
//     (void)state;
//     assert_int_equal((int)float_matrix_rows(NULL), 0);
//     assert_int_equal((int)float_matrix_cols(NULL), 0);
//     assert_int_equal((int)float_matrix_nnz(NULL), 0);
//     assert_int_equal((int)float_matrix_storage_bytes(NULL), 0);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_dense_nnz_equals_rows_times_cols(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(4, 5, alloc);
//     assert_true(r.has_value);
//     assert_int_equal((int)float_matrix_nnz(r.u.value), 20);
//     return_float_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_format_name_returns_string(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     assert_string_equal(float_matrix_format_name(r.u.value), "DENSE_MATRIX");
//     return_float_matrix(r.u.value);
// }
//  
// // ================================================================================
// // Group 14: Shape and compatibility queries
// // ================================================================================
//  
// static void test_same_shape_true_for_matching_dimensions(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t ra = init_float_dense_matrix(3, 4, alloc);
//     float_matrix_expect_t rb = init_float_dense_matrix(3, 4, alloc);
//     assert_true(ra.has_value && rb.has_value);
//     assert_true(float_matrix_has_same_shape(ra.u.value, rb.u.value));
//     return_float_matrix(ra.u.value);
//     return_float_matrix(rb.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_is_square(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t sq = init_float_dense_matrix(3, 3, alloc);
//     float_matrix_expect_t ns = init_float_dense_matrix(3, 4, alloc);
//     assert_true(sq.has_value && ns.has_value);
//     assert_true(float_matrix_is_square(sq.u.value));
//     assert_false(float_matrix_is_square(ns.u.value));
//     return_float_matrix(sq.u.value);
//     return_float_matrix(ns.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_is_sparse(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t d = init_float_dense_matrix(2, 2, alloc);
//     float_matrix_expect_t c = init_float_coo_matrix(2, 2, 4, true, alloc);
//     assert_true(d.has_value && c.has_value);
//     assert_false(float_matrix_is_sparse(d.u.value));
//     assert_true(float_matrix_is_sparse(c.u.value));
//     return_float_matrix(d.u.value);
//     return_float_matrix(c.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_add_compatible(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t ra = init_float_dense_matrix(3, 4, alloc);
//     float_matrix_expect_t rb = init_float_dense_matrix(3, 4, alloc);
//     float_matrix_expect_t rc = init_float_dense_matrix(4, 3, alloc);
//     assert_true(ra.has_value && rb.has_value && rc.has_value);
//  
//     assert_true(float_matrix_is_add_compatible(ra.u.value, rb.u.value));
//     assert_false(float_matrix_is_add_compatible(ra.u.value, rc.u.value));
//  
//     return_float_matrix(ra.u.value);
//     return_float_matrix(rb.u.value);
//     return_float_matrix(rc.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_multiply_compatible(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t ra = init_float_dense_matrix(2, 3, alloc);
//     float_matrix_expect_t rb = init_float_dense_matrix(3, 5, alloc);
//     float_matrix_expect_t rc = init_float_dense_matrix(4, 5, alloc);
//     assert_true(ra.has_value && rb.has_value && rc.has_value);
//  
//     assert_true(float_matrix_is_multiply_compatible(ra.u.value, rb.u.value));
//     assert_false(float_matrix_is_multiply_compatible(ra.u.value, rc.u.value));
//  
//     return_float_matrix(ra.u.value);
//     return_float_matrix(rb.u.value);
//     return_float_matrix(rc.u.value);
// }
//  
// // ================================================================================
// // Group 15: Row / column swaps
// // ================================================================================
//  
// static void test_swap_rows_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(3, 2, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     /* Row 0: [1, 2], Row 1: [3, 4], Row 2: [5, 6] */
//     set_float_matrix(m, 0, 0, 1.0f); set_float_matrix(m, 0, 1, 2.0f);
//     set_float_matrix(m, 1, 0, 3.0f); set_float_matrix(m, 1, 1, 4.0f);
//     set_float_matrix(m, 2, 0, 5.0f); set_float_matrix(m, 2, 1, 6.0f);
//  
//     assert_int_equal(swap_float_matrix_rows(m, 0, 2), NO_ERROR);
//  
//     float val = 0.0f;
//     get_float_matrix(m, 0, 0, &val); ASSERT_FLOAT_EXACT(val, 5.0f);
//     get_float_matrix(m, 0, 1, &val); ASSERT_FLOAT_EXACT(val, 6.0f);
//     get_float_matrix(m, 2, 0, &val); ASSERT_FLOAT_EXACT(val, 1.0f);
//     get_float_matrix(m, 2, 1, &val); ASSERT_FLOAT_EXACT(val, 2.0f);
//  
//     return_float_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_swap_cols_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     set_float_matrix(m, 0, 0, 1.0f); set_float_matrix(m, 0, 2, 3.0f);
//     set_float_matrix(m, 1, 0, 4.0f); set_float_matrix(m, 1, 2, 6.0f);
//  
//     assert_int_equal(swap_float_matrix_cols(m, 0, 2), NO_ERROR);
//  
//     float val = 0.0f;
//     get_float_matrix(m, 0, 0, &val); ASSERT_FLOAT_EXACT(val, 3.0f);
//     get_float_matrix(m, 0, 2, &val); ASSERT_FLOAT_EXACT(val, 1.0f);
//     get_float_matrix(m, 1, 0, &val); ASSERT_FLOAT_EXACT(val, 6.0f);
//     get_float_matrix(m, 1, 2, &val); ASSERT_FLOAT_EXACT(val, 4.0f);
//  
//     return_float_matrix(m);
// }
//  
// // ================================================================================
// // Group 16: Special constructors (identity, row/col vector)
// // ================================================================================
//  
// static void test_identity_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_identity_matrix(3, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     assert_int_equal((int)float_matrix_rows(m), 3);
//     assert_int_equal((int)float_matrix_cols(m), 3);
//  
//     float val = 0.0f;
//     for (size_t i = 0; i < 3; ++i) {
//         for (size_t j = 0; j < 3; ++j) {
//             get_float_matrix(m, i, j, &val);
//             ASSERT_FLOAT_EXACT(val, (i == j) ? 1.0f : 0.0f);
//         }
//     }
//  
//     return_float_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_row_vector_shape(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_row_vector(5, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     assert_int_equal((int)float_matrix_rows(m), 1);
//     assert_int_equal((int)float_matrix_cols(m), 5);
//     assert_true(float_matrix_is_row_vector(m));
//     assert_false(float_matrix_is_col_vector(m));
//     assert_true(float_matrix_is_vector(m));
//     assert_int_equal((int)float_matrix_vector_length(m), 5);
//  
//     return_float_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_col_vector_shape(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_col_vector(4, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     assert_int_equal((int)float_matrix_rows(m), 4);
//     assert_int_equal((int)float_matrix_cols(m), 1);
//     assert_false(float_matrix_is_row_vector(m));
//     assert_true(float_matrix_is_col_vector(m));
//     assert_true(float_matrix_is_vector(m));
//     assert_int_equal((int)float_matrix_vector_length(m), 4);
//  
//     return_float_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_non_vector_returns_zero_length(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(3, 4, alloc);
//     assert_true(r.has_value);
//  
//     assert_false(float_matrix_is_vector(r.u.value));
//     assert_int_equal((int)float_matrix_vector_length(r.u.value), 0);
//  
//     return_float_matrix(r.u.value);
// }
//  
// // ================================================================================
// // Group 17: Large matrix smoke tests (exercises SIMD body + tail paths)
// // ================================================================================
//  
// static void test_fill_large_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     /* 100×100 = 10000 elements — enough to exercise SIMD unrolled body
//        and scalar/masked tail for any vector width up to 512-bit. */
//     float_matrix_expect_t r = init_float_dense_matrix(100, 100, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     assert_int_equal(fill_float_matrix(m, -2.5f), NO_ERROR);
//  
//     float val = 0.0f;
//     get_float_matrix(m, 0, 0, &val);     ASSERT_FLOAT_EXACT(val, -2.5f);
//     get_float_matrix(m, 49, 50, &val);   ASSERT_FLOAT_EXACT(val, -2.5f);
//     get_float_matrix(m, 99, 99, &val);   ASSERT_FLOAT_EXACT(val, -2.5f);
//  
//     return_float_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_transpose_large_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     /* 17×19 — not a multiple of any SIMD width, exercises all remainder paths */
//     float_matrix_expect_t r = init_float_dense_matrix(17, 19, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     /* Fill with distinct values */
//     for (size_t i = 0; i < 17; ++i) {
//         for (size_t j = 0; j < 19; ++j) {
//             set_float_matrix(m, i, j, (float)(i * 19 + j));
//         }
//     }
//  
//     float_matrix_expect_t tr = transpose_float_matrix(m, alloc);
//     assert_true(tr.has_value);
//     float_matrix_t* t = tr.u.value;
//  
//     assert_int_equal((int)float_matrix_rows(t), 19);
//     assert_int_equal((int)float_matrix_cols(t), 17);
//  
//     /* Verify every element: t[j][i] == m[i][j] */
//     float vm = 0.0f, vt = 0.0f;
//     for (size_t i = 0; i < 17; ++i) {
//         for (size_t j = 0; j < 19; ++j) {
//             get_float_matrix(m, i, j, &vm);
//             get_float_matrix(t, j, i, &vt);
//             ASSERT_FLOAT_EXACT(vm, vt);
//         }
//     }
//  
//     return_float_matrix(m);
//     return_float_matrix(t);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_equal_large_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     float_matrix_expect_t r = init_float_dense_matrix(50, 50, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     fill_float_matrix(m, 7.0f);
//  
//     float_matrix_expect_t cr = copy_float_matrix(m, alloc);
//     assert_true(cr.has_value);
//  
//     assert_true(float_matrix_equal(m, cr.u.value));
//  
//     /* Flip one element near the end to test early-exit vs. full scan */
//     set_float_matrix(cr.u.value, 49, 49, 8.0f);
//     assert_false(float_matrix_equal(m, cr.u.value));
//  
//     return_float_matrix(m);
//     return_float_matrix(cr.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_convert_large_dense_to_csr(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     /* 20×20 with sparse population */
//     float_matrix_expect_t r = init_float_dense_matrix(20, 20, alloc);
//     assert_true(r.has_value);
//     float_matrix_t* m = r.u.value;
//  
//     /* Set only diagonal */
//     for (size_t i = 0; i < 20; ++i) {
//         set_float_matrix(m, i, i, (float)(i + 1));
//     }
//  
//     float_matrix_expect_t cr = convert_float_matrix(m, CSR_MATRIX, alloc);
//     assert_true(cr.has_value);
//     float_matrix_t* csr = cr.u.value;
//  
//     assert_int_equal((int)float_matrix_nnz(csr), 20);
//  
//     /* Verify diagonal values survive conversion */
//     float val = 0.0f;
//     for (size_t i = 0; i < 20; ++i) {
//         get_float_matrix(csr, i, i, &val);
//         ASSERT_FLOAT_EXACT(val, (float)(i + 1));
//     }
//  
//     /* Verify off-diagonal is zero */
//     get_float_matrix(csr, 0, 1, &val); ASSERT_FLOAT_EXACT(val, 0.0f);
//     get_float_matrix(csr, 19, 0, &val); ASSERT_FLOAT_EXACT(val, 0.0f);
//  
//     return_float_matrix(m);
//     return_float_matrix(csr);
// }
//  
// // ================================================================================
// // Test suite export
// // ================================================================================
//  
// const struct CMUnitTest test_float_matrix[] = {
//     /* Group 1: init dense */
//     cmocka_unit_test(test_dense_init_null_allocator_fails),
//     cmocka_unit_test(test_dense_init_zero_rows_fails),
//     cmocka_unit_test(test_dense_init_zero_cols_fails),
//     cmocka_unit_test(test_dense_init_returns_valid_matrix),
//     cmocka_unit_test(test_dense_init_elements_are_zero),
//     /* Group 2: init COO */
//     cmocka_unit_test(test_coo_init_returns_valid_matrix),
//     cmocka_unit_test(test_coo_init_zero_capacity_fails),
//     /* Group 3: return */
//     cmocka_unit_test(test_return_null_is_safe),
//     /* Group 4: get/set dense */
//     cmocka_unit_test(test_get_null_matrix_returns_null_pointer),
//     cmocka_unit_test(test_get_null_out_returns_null_pointer),
//     cmocka_unit_test(test_set_null_matrix_returns_null_pointer),
//     cmocka_unit_test(test_set_and_get_round_trip),
//     cmocka_unit_test(test_set_out_of_bounds_returns_error),
//     /* Group 5: COO assembly */
//     cmocka_unit_test(test_coo_push_back_and_retrieve),
//     cmocka_unit_test(test_coo_push_back_overwrites_existing),
//     cmocka_unit_test(test_coo_sort_orders_entries),
//     /* Group 6: clear */
//     cmocka_unit_test(test_clear_dense_zeros_all_elements),
//     cmocka_unit_test(test_clear_coo_resets_nnz),
//     /* Group 7: copy */
//     cmocka_unit_test(test_copy_null_returns_error),
//     cmocka_unit_test(test_copy_dense_produces_independent_matrix),
//     /* Group 8: fill (SIMD) */
//     cmocka_unit_test(test_fill_null_returns_error),
//     cmocka_unit_test(test_fill_dense_sets_all_elements),
//     cmocka_unit_test(test_fill_zero_clears_matrix),
//     /* Group 9: is_zero (SIMD) */
//     cmocka_unit_test(test_is_zero_null_returns_true),
//     cmocka_unit_test(test_is_zero_freshly_initialized_dense),
//     cmocka_unit_test(test_is_zero_returns_false_after_set),
//     /* Group 10: equal (SIMD) */
//     cmocka_unit_test(test_equal_null_returns_false),
//     cmocka_unit_test(test_equal_identical_matrices),
//     cmocka_unit_test(test_equal_different_values_returns_false),
//     cmocka_unit_test(test_equal_different_shape_returns_false),
//     /* Group 11: transpose (SIMD) */
//     cmocka_unit_test(test_transpose_null_returns_error),
//     cmocka_unit_test(test_transpose_dense_swaps_dimensions),
//     cmocka_unit_test(test_transpose_square_is_correct),
//     /* Group 12: convert dense→CSR (SIMD) */
//     cmocka_unit_test(test_convert_dense_to_csr_preserves_values),
//     cmocka_unit_test(test_convert_all_zero_dense_to_csr),
//     /* Group 13: introspection */
//     cmocka_unit_test(test_introspection_null_returns_defaults),
//     cmocka_unit_test(test_dense_nnz_equals_rows_times_cols),
//     cmocka_unit_test(test_format_name_returns_string),
//     /* Group 14: shape/compat */
//     cmocka_unit_test(test_same_shape_true_for_matching_dimensions),
//     cmocka_unit_test(test_is_square),
//     cmocka_unit_test(test_is_sparse),
//     cmocka_unit_test(test_add_compatible),
//     cmocka_unit_test(test_multiply_compatible),
//     /* Group 15: swaps */
//     cmocka_unit_test(test_swap_rows_dense),
//     cmocka_unit_test(test_swap_cols_dense),
//     /* Group 16: special constructors */
//     cmocka_unit_test(test_identity_matrix),
//     cmocka_unit_test(test_row_vector_shape),
//     cmocka_unit_test(test_col_vector_shape),
//     cmocka_unit_test(test_non_vector_returns_zero_length),
//     /* Group 17: large matrix smoke tests */
//     cmocka_unit_test(test_fill_large_matrix),
//     cmocka_unit_test(test_transpose_large_matrix),
//     cmocka_unit_test(test_equal_large_matrix),
//     cmocka_unit_test(test_convert_large_dense_to_csr),
// };
// const size_t test_float_matrix_count =
//     sizeof(test_float_matrix) / sizeof(test_float_matrix[0]);
// // ================================================================================ 
// // ================================================================================ 
//
// /* Exact double equality for binary fractions used in tests. */
// #define ASSERT_DOUBLE_EXACT(a, b) assert_true((a) == (b))
//  
// // ================================================================================
// // Group 1: init_double_dense_matrix
// // ================================================================================
//  
// static void test_double_dense_init_null_allocator_fails(void** state) {
//     (void)state;
//     allocator_vtable_t bad = { 0 };
//     double_matrix_expect_t r = init_double_dense_matrix(3, 4, bad);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_dense_init_zero_rows_fails(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(0, 4, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, INVALID_ARG);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_dense_init_zero_cols_fails(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(3, 0, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, INVALID_ARG);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_dense_init_returns_valid_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(3, 4, alloc);
//     assert_true(r.has_value);
//     assert_non_null(r.u.value);
//     assert_int_equal((int)double_matrix_rows(r.u.value), 3);
//     assert_int_equal((int)double_matrix_cols(r.u.value), 4);
//     assert_int_equal((int)double_matrix_format(r.u.value), DENSE_MATRIX);
//     return_double_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_dense_init_elements_are_zero(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     double val = -1.0;
//     for (size_t i = 0; i < 2; ++i) {
//         for (size_t j = 0; j < 3; ++j) {
//             assert_int_equal(get_double_matrix(m, i, j, &val), NO_ERROR);
//             ASSERT_DOUBLE_EXACT(val, 0.0);
//         }
//     }
//  
//     return_double_matrix(m);
// }
//  
// // ================================================================================
// // Group 2: init_double_coo_matrix
// // ================================================================================
//  
// static void test_double_coo_init_returns_valid_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_coo_matrix(10, 10, 8, true, alloc);
//     assert_true(r.has_value);
//     assert_int_equal((int)double_matrix_rows(r.u.value), 10);
//     assert_int_equal((int)double_matrix_cols(r.u.value), 10);
//     assert_int_equal((int)double_matrix_format(r.u.value), COO_MATRIX);
//     assert_int_equal((int)double_matrix_nnz(r.u.value), 0);
//     return_double_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_coo_init_zero_capacity_fails(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_coo_matrix(5, 5, 0, true, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, INVALID_ARG);
// }
//  
// // ================================================================================
// // Group 3: return_double_matrix
// // ================================================================================
//  
// static void test_double_return_null_is_safe(void** state) {
//     (void)state;
//     return_double_matrix(NULL);
// }
//  
// // ================================================================================
// // Group 4: get/set_double_matrix (dense)
// // ================================================================================
//  
// static void test_double_get_null_matrix_returns_null_pointer(void** state) {
//     (void)state;
//     double val = 0.0;
//     assert_int_equal(get_double_matrix(NULL, 0, 0, &val), NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_get_null_out_returns_null_pointer(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     assert_int_equal(get_double_matrix(r.u.value, 0, 0, NULL), NULL_POINTER);
//     return_double_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_set_null_matrix_returns_null_pointer(void** state) {
//     (void)state;
//     assert_int_equal(set_double_matrix(NULL, 0, 0, 1.0), NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_set_and_get_round_trip(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     assert_int_equal(set_double_matrix(m, 0, 0, 1.5), NO_ERROR);
//     assert_int_equal(set_double_matrix(m, 1, 2, -3.0), NO_ERROR);
//     assert_int_equal(set_double_matrix(m, 2, 1, 42.0), NO_ERROR);
//  
//     double val = 0.0;
//     get_double_matrix(m, 0, 0, &val); ASSERT_DOUBLE_EXACT(val, 1.5);
//     get_double_matrix(m, 1, 2, &val); ASSERT_DOUBLE_EXACT(val, -3.0);
//     get_double_matrix(m, 2, 1, &val); ASSERT_DOUBLE_EXACT(val, 42.0);
//  
//     /* Unset element should still be zero */
//     get_double_matrix(m, 0, 1, &val); ASSERT_DOUBLE_EXACT(val, 0.0);
//  
//     return_double_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_set_out_of_bounds_returns_error(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//  
//     assert_int_equal(set_double_matrix(r.u.value, 2, 0, 1.0), INVALID_ARG);
//     assert_int_equal(set_double_matrix(r.u.value, 0, 3, 1.0), INVALID_ARG);
//  
//     return_double_matrix(r.u.value);
// }
//  
// // ================================================================================
// // Group 5: COO assembly (push_back, sort, reserve)
// // ================================================================================
//  
// static void test_double_coo_push_back_and_retrieve(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_coo_matrix(5, 5, 4, true, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     assert_int_equal(push_back_double_coo_matrix(m, 0, 2, 3.14159265), NO_ERROR);
//     assert_int_equal(push_back_double_coo_matrix(m, 3, 4, -1.0), NO_ERROR);
//     assert_int_equal((int)double_matrix_nnz(m), 2);
//  
//     double val = 0.0;
//     get_double_matrix(m, 0, 2, &val); ASSERT_DOUBLE_EXACT(val, 3.14159265);
//     get_double_matrix(m, 3, 4, &val); ASSERT_DOUBLE_EXACT(val, -1.0);
//  
//     /* Unset entry returns zero */
//     get_double_matrix(m, 1, 1, &val); ASSERT_DOUBLE_EXACT(val, 0.0);
//  
//     return_double_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_coo_push_back_overwrites_existing(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_coo_matrix(3, 3, 4, true, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     push_back_double_coo_matrix(m, 1, 1, 10.0);
//     push_back_double_coo_matrix(m, 1, 1, 20.0);  /* overwrite */
//     assert_int_equal((int)double_matrix_nnz(m), 1);
//  
//     double val = 0.0;
//     get_double_matrix(m, 1, 1, &val);
//     ASSERT_DOUBLE_EXACT(val, 20.0);
//  
//     return_double_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_coo_sort_orders_entries(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_coo_matrix(4, 4, 8, true, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     /* Insert out of order */
//     push_back_double_coo_matrix(m, 3, 0, 4.0);
//     push_back_double_coo_matrix(m, 0, 1, 1.0);
//     push_back_double_coo_matrix(m, 1, 2, 2.0);
//  
//     assert_int_equal(sort_double_coo_matrix(m), NO_ERROR);
//  
//     /* Values should still be retrievable after sort */
//     double val = 0.0;
//     get_double_matrix(m, 0, 1, &val); ASSERT_DOUBLE_EXACT(val, 1.0);
//     get_double_matrix(m, 1, 2, &val); ASSERT_DOUBLE_EXACT(val, 2.0);
//     get_double_matrix(m, 3, 0, &val); ASSERT_DOUBLE_EXACT(val, 4.0);
//  
//     return_double_matrix(m);
// }
//  
// // ================================================================================
// // Group 6: clear_double_matrix
// // ================================================================================
//  
// static void test_double_clear_dense_zeros_all_elements(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     set_double_matrix(m, 0, 0, 5.0);
//     set_double_matrix(m, 1, 1, 9.0);
//     assert_int_equal(clear_double_matrix(m), NO_ERROR);
//  
//     assert_true(is_double_matrix_zero(m));
//  
//     return_double_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_clear_coo_resets_nnz(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_coo_matrix(3, 3, 4, true, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     push_back_double_coo_matrix(m, 0, 0, 1.0);
//     push_back_double_coo_matrix(m, 1, 1, 2.0);
//     assert_int_equal(clear_double_matrix(m), NO_ERROR);
//     assert_int_equal((int)double_matrix_nnz(m), 0);
//  
//     return_double_matrix(m);
// }
//  
// // ================================================================================
// // Group 7: copy_double_matrix
// // ================================================================================
//  
// static void test_double_copy_null_returns_error(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = copy_double_matrix(NULL, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_copy_dense_produces_independent_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* src = r.u.value;
//  
//     set_double_matrix(src, 0, 0, 1.0);
//     set_double_matrix(src, 1, 2, 7.0);
//  
//     double_matrix_expect_t cr = copy_double_matrix(src, alloc);
//     assert_true(cr.has_value);
//     double_matrix_t* dst = cr.u.value;
//  
//     /* Mutating src must not affect copy */
//     set_double_matrix(src, 0, 0, 999.0);
//  
//     double val = 0.0;
//     get_double_matrix(dst, 0, 0, &val); ASSERT_DOUBLE_EXACT(val, 1.0);
//     get_double_matrix(dst, 1, 2, &val); ASSERT_DOUBLE_EXACT(val, 7.0);
//  
//     return_double_matrix(src);
//     return_double_matrix(dst);
// }
//  
// // ================================================================================
// // Group 8: fill_double_matrix (SIMD-accelerated)
// // ================================================================================
//  
// static void test_double_fill_null_returns_error(void** state) {
//     (void)state;
//     assert_int_equal(fill_double_matrix(NULL, 1.0), NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_fill_dense_sets_all_elements(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(4, 5, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     assert_int_equal(fill_double_matrix(m, 3.5), NO_ERROR);
//  
//     double val = 0.0;
//     for (size_t i = 0; i < 4; ++i) {
//         for (size_t j = 0; j < 5; ++j) {
//             get_double_matrix(m, i, j, &val);
//             ASSERT_DOUBLE_EXACT(val, 3.5);
//         }
//     }
//  
//     return_double_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_fill_zero_clears_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     fill_double_matrix(m, 5.0);
//     assert_int_equal(fill_double_matrix(m, 0.0), NO_ERROR);
//     assert_true(is_double_matrix_zero(m));
//  
//     return_double_matrix(m);
// }
//  
// // ================================================================================
// // Group 9: is_double_matrix_zero (SIMD-accelerated)
// // ================================================================================
//  
// static void test_double_is_zero_null_returns_true(void** state) {
//     (void)state;
//     assert_true(is_double_matrix_zero(NULL));
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_is_zero_freshly_initialized_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(5, 5, alloc);
//     assert_true(r.has_value);
//     assert_true(is_double_matrix_zero(r.u.value));
//     return_double_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_is_zero_returns_false_after_set(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     set_double_matrix(m, 1, 1, 0.001);
//     assert_false(is_double_matrix_zero(m));
//  
//     return_double_matrix(m);
// }
//  
// // ================================================================================
// // Group 10: double_matrix_equal (SIMD-accelerated)
// // ================================================================================
//  
// static void test_double_equal_null_returns_false(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//  
//     assert_false(double_matrix_equal(NULL, r.u.value));
//     assert_false(double_matrix_equal(r.u.value, NULL));
//  
//     return_double_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_equal_identical_matrices(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* a = r.u.value;
//  
//     set_double_matrix(a, 0, 0, 1.0);
//     set_double_matrix(a, 1, 2, -4.5);
//  
//     double_matrix_expect_t cr = copy_double_matrix(a, alloc);
//     assert_true(cr.has_value);
//  
//     assert_true(double_matrix_equal(a, cr.u.value));
//  
//     return_double_matrix(a);
//     return_double_matrix(cr.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_equal_different_values_returns_false(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t ra = init_double_dense_matrix(2, 2, alloc);
//     double_matrix_expect_t rb = init_double_dense_matrix(2, 2, alloc);
//     assert_true(ra.has_value);
//     assert_true(rb.has_value);
//  
//     set_double_matrix(ra.u.value, 0, 0, 1.0);
//     set_double_matrix(rb.u.value, 0, 0, 2.0);
//  
//     assert_false(double_matrix_equal(ra.u.value, rb.u.value));
//  
//     return_double_matrix(ra.u.value);
//     return_double_matrix(rb.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_equal_different_shape_returns_false(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t ra = init_double_dense_matrix(2, 3, alloc);
//     double_matrix_expect_t rb = init_double_dense_matrix(3, 2, alloc);
//     assert_true(ra.has_value);
//     assert_true(rb.has_value);
//  
//     assert_false(double_matrix_equal(ra.u.value, rb.u.value));
//  
//     return_double_matrix(ra.u.value);
//     return_double_matrix(rb.u.value);
// }
//  
// // ================================================================================
// // Group 11: transpose_double_matrix (SIMD-accelerated)
// // ================================================================================
//  
// static void test_double_transpose_null_returns_error(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = transpose_double_matrix(NULL, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_transpose_dense_swaps_dimensions(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     /* Fill with distinct values: m[i][j] = (i+1)*10 + j */
//     set_double_matrix(m, 0, 0, 10.0);
//     set_double_matrix(m, 0, 1, 11.0);
//     set_double_matrix(m, 0, 2, 12.0);
//     set_double_matrix(m, 1, 0, 20.0);
//     set_double_matrix(m, 1, 1, 21.0);
//     set_double_matrix(m, 1, 2, 22.0);
//  
//     double_matrix_expect_t tr = transpose_double_matrix(m, alloc);
//     assert_true(tr.has_value);
//     double_matrix_t* t = tr.u.value;
//  
//     assert_int_equal((int)double_matrix_rows(t), 3);
//     assert_int_equal((int)double_matrix_cols(t), 2);
//  
//     double val = 0.0;
//     get_double_matrix(t, 0, 0, &val); ASSERT_DOUBLE_EXACT(val, 10.0);
//     get_double_matrix(t, 0, 1, &val); ASSERT_DOUBLE_EXACT(val, 20.0);
//     get_double_matrix(t, 1, 0, &val); ASSERT_DOUBLE_EXACT(val, 11.0);
//     get_double_matrix(t, 1, 1, &val); ASSERT_DOUBLE_EXACT(val, 21.0);
//     get_double_matrix(t, 2, 0, &val); ASSERT_DOUBLE_EXACT(val, 12.0);
//     get_double_matrix(t, 2, 1, &val); ASSERT_DOUBLE_EXACT(val, 22.0);
//  
//     return_double_matrix(m);
//     return_double_matrix(t);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_transpose_square_is_correct(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     set_double_matrix(m, 0, 1, 5.0);
//     set_double_matrix(m, 1, 0, 7.0);
//  
//     double_matrix_expect_t tr = transpose_double_matrix(m, alloc);
//     assert_true(tr.has_value);
//  
//     double val = 0.0;
//     get_double_matrix(tr.u.value, 0, 1, &val); ASSERT_DOUBLE_EXACT(val, 7.0);
//     get_double_matrix(tr.u.value, 1, 0, &val); ASSERT_DOUBLE_EXACT(val, 5.0);
//  
//     return_double_matrix(m);
//     return_double_matrix(tr.u.value);
// }
//  
// // ================================================================================
// // Group 12: convert_double_matrix dense→CSR (SIMD-accelerated)
// // ================================================================================
//  
// static void test_double_convert_dense_to_csr_preserves_values(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     set_double_matrix(m, 0, 0, 1.0);
//     set_double_matrix(m, 0, 2, 2.0);
//     set_double_matrix(m, 1, 1, 3.0);
//     set_double_matrix(m, 2, 0, 4.0);
//     set_double_matrix(m, 2, 2, 5.0);
//  
//     double_matrix_expect_t cr = convert_double_matrix(m, CSR_MATRIX, alloc);
//     assert_true(cr.has_value);
//     double_matrix_t* csr = cr.u.value;
//  
//     assert_int_equal((int)double_matrix_format(csr), CSR_MATRIX);
//     assert_int_equal((int)double_matrix_rows(csr), 3);
//     assert_int_equal((int)double_matrix_cols(csr), 3);
//     assert_int_equal((int)double_matrix_nnz(csr), 5);
//  
//     double val = 0.0;
//     get_double_matrix(csr, 0, 0, &val); ASSERT_DOUBLE_EXACT(val, 1.0);
//     get_double_matrix(csr, 0, 2, &val); ASSERT_DOUBLE_EXACT(val, 2.0);
//     get_double_matrix(csr, 1, 1, &val); ASSERT_DOUBLE_EXACT(val, 3.0);
//     get_double_matrix(csr, 2, 0, &val); ASSERT_DOUBLE_EXACT(val, 4.0);
//     get_double_matrix(csr, 2, 2, &val); ASSERT_DOUBLE_EXACT(val, 5.0);
//  
//     /* Zero entries must read as zero */
//     get_double_matrix(csr, 0, 1, &val); ASSERT_DOUBLE_EXACT(val, 0.0);
//     get_double_matrix(csr, 1, 0, &val); ASSERT_DOUBLE_EXACT(val, 0.0);
//  
//     return_double_matrix(m);
//     return_double_matrix(csr);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_convert_all_zero_dense_to_csr(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//  
//     double_matrix_expect_t cr = convert_double_matrix(r.u.value, CSR_MATRIX, alloc);
//     assert_true(cr.has_value);
//     assert_int_equal((int)double_matrix_nnz(cr.u.value), 0);
//  
//     return_double_matrix(r.u.value);
//     return_double_matrix(cr.u.value);
// }
//  
// // ================================================================================
// // Group 13: Introspection
// // ================================================================================
//  
// static void test_double_introspection_null_returns_defaults(void** state) {
//     (void)state;
//     assert_int_equal((int)double_matrix_rows(NULL), 0);
//     assert_int_equal((int)double_matrix_cols(NULL), 0);
//     assert_int_equal((int)double_matrix_nnz(NULL), 0);
//     assert_int_equal((int)double_matrix_storage_bytes(NULL), 0);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_dense_nnz_equals_rows_times_cols(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(4, 5, alloc);
//     assert_true(r.has_value);
//     assert_int_equal((int)double_matrix_nnz(r.u.value), 20);
//     return_double_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_format_name_returns_string(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     assert_string_equal(double_matrix_format_name(r.u.value), "DENSE_MATRIX");
//     return_double_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_storage_bytes_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(3, 4, alloc);
//     assert_true(r.has_value);
//     /* 3 * 4 * sizeof(double) = 3 * 4 * 8 = 96 */
//     assert_int_equal((int)double_matrix_storage_bytes(r.u.value), 96);
//     return_double_matrix(r.u.value);
// }
//  
// // ================================================================================
// // Group 14: Shape and compatibility queries
// // ================================================================================
//  
// static void test_double_same_shape_true_for_matching_dimensions(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t ra = init_double_dense_matrix(3, 4, alloc);
//     double_matrix_expect_t rb = init_double_dense_matrix(3, 4, alloc);
//     assert_true(ra.has_value && rb.has_value);
//     assert_true(double_matrix_has_same_shape(ra.u.value, rb.u.value));
//     return_double_matrix(ra.u.value);
//     return_double_matrix(rb.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_is_square(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t sq = init_double_dense_matrix(3, 3, alloc);
//     double_matrix_expect_t ns = init_double_dense_matrix(3, 4, alloc);
//     assert_true(sq.has_value && ns.has_value);
//     assert_true(double_matrix_is_square(sq.u.value));
//     assert_false(double_matrix_is_square(ns.u.value));
//     return_double_matrix(sq.u.value);
//     return_double_matrix(ns.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_is_sparse(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t d = init_double_dense_matrix(2, 2, alloc);
//     double_matrix_expect_t c = init_double_coo_matrix(2, 2, 4, true, alloc);
//     assert_true(d.has_value && c.has_value);
//     assert_false(double_matrix_is_sparse(d.u.value));
//     assert_true(double_matrix_is_sparse(c.u.value));
//     return_double_matrix(d.u.value);
//     return_double_matrix(c.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_add_compatible(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t ra = init_double_dense_matrix(3, 4, alloc);
//     double_matrix_expect_t rb = init_double_dense_matrix(3, 4, alloc);
//     double_matrix_expect_t rc = init_double_dense_matrix(4, 3, alloc);
//     assert_true(ra.has_value && rb.has_value && rc.has_value);
//  
//     assert_true(double_matrix_is_add_compatible(ra.u.value, rb.u.value));
//     assert_false(double_matrix_is_add_compatible(ra.u.value, rc.u.value));
//  
//     return_double_matrix(ra.u.value);
//     return_double_matrix(rb.u.value);
//     return_double_matrix(rc.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_multiply_compatible(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t ra = init_double_dense_matrix(2, 3, alloc);
//     double_matrix_expect_t rb = init_double_dense_matrix(3, 5, alloc);
//     double_matrix_expect_t rc = init_double_dense_matrix(4, 5, alloc);
//     assert_true(ra.has_value && rb.has_value && rc.has_value);
//  
//     assert_true(double_matrix_is_multiply_compatible(ra.u.value, rb.u.value));
//     assert_false(double_matrix_is_multiply_compatible(ra.u.value, rc.u.value));
//  
//     return_double_matrix(ra.u.value);
//     return_double_matrix(rb.u.value);
//     return_double_matrix(rc.u.value);
// }
//  
// // ================================================================================
// // Group 15: Row / column swaps
// // ================================================================================
//  
// static void test_double_swap_rows_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(3, 2, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     /* Row 0: [1, 2], Row 1: [3, 4], Row 2: [5, 6] */
//     set_double_matrix(m, 0, 0, 1.0); set_double_matrix(m, 0, 1, 2.0);
//     set_double_matrix(m, 1, 0, 3.0); set_double_matrix(m, 1, 1, 4.0);
//     set_double_matrix(m, 2, 0, 5.0); set_double_matrix(m, 2, 1, 6.0);
//  
//     assert_int_equal(swap_double_matrix_rows(m, 0, 2), NO_ERROR);
//  
//     double val = 0.0;
//     get_double_matrix(m, 0, 0, &val); ASSERT_DOUBLE_EXACT(val, 5.0);
//     get_double_matrix(m, 0, 1, &val); ASSERT_DOUBLE_EXACT(val, 6.0);
//     get_double_matrix(m, 2, 0, &val); ASSERT_DOUBLE_EXACT(val, 1.0);
//     get_double_matrix(m, 2, 1, &val); ASSERT_DOUBLE_EXACT(val, 2.0);
//  
//     return_double_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_swap_cols_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     set_double_matrix(m, 0, 0, 1.0); set_double_matrix(m, 0, 2, 3.0);
//     set_double_matrix(m, 1, 0, 4.0); set_double_matrix(m, 1, 2, 6.0);
//  
//     assert_int_equal(swap_double_matrix_cols(m, 0, 2), NO_ERROR);
//  
//     double val = 0.0;
//     get_double_matrix(m, 0, 0, &val); ASSERT_DOUBLE_EXACT(val, 3.0);
//     get_double_matrix(m, 0, 2, &val); ASSERT_DOUBLE_EXACT(val, 1.0);
//     get_double_matrix(m, 1, 0, &val); ASSERT_DOUBLE_EXACT(val, 6.0);
//     get_double_matrix(m, 1, 2, &val); ASSERT_DOUBLE_EXACT(val, 4.0);
//  
//     return_double_matrix(m);
// }
//  
// // ================================================================================
// // Group 16: Special constructors (identity, row/col vector)
// // ================================================================================
//  
// static void test_double_identity_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_identity_matrix(3, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     assert_int_equal((int)double_matrix_rows(m), 3);
//     assert_int_equal((int)double_matrix_cols(m), 3);
//  
//     double val = 0.0;
//     for (size_t i = 0; i < 3; ++i) {
//         for (size_t j = 0; j < 3; ++j) {
//             get_double_matrix(m, i, j, &val);
//             ASSERT_DOUBLE_EXACT(val, (i == j) ? 1.0 : 0.0);
//         }
//     }
//  
//     return_double_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_row_vector_shape(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_row_vector(5, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     assert_int_equal((int)double_matrix_rows(m), 1);
//     assert_int_equal((int)double_matrix_cols(m), 5);
//     assert_true(double_matrix_is_row_vector(m));
//     assert_false(double_matrix_is_col_vector(m));
//     assert_true(double_matrix_is_vector(m));
//     assert_int_equal((int)double_matrix_vector_length(m), 5);
//  
//     return_double_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_col_vector_shape(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_col_vector(4, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     assert_int_equal((int)double_matrix_rows(m), 4);
//     assert_int_equal((int)double_matrix_cols(m), 1);
//     assert_false(double_matrix_is_row_vector(m));
//     assert_true(double_matrix_is_col_vector(m));
//     assert_true(double_matrix_is_vector(m));
//     assert_int_equal((int)double_matrix_vector_length(m), 4);
//  
//     return_double_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_non_vector_returns_zero_length(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(3, 4, alloc);
//     assert_true(r.has_value);
//  
//     assert_false(double_matrix_is_vector(r.u.value));
//     assert_int_equal((int)double_matrix_vector_length(r.u.value), 0);
//  
//     return_double_matrix(r.u.value);
// }
//  
// // ================================================================================
// // Group 17: Large matrix smoke tests (exercises SIMD body + tail paths)
// // ================================================================================
//  
// static void test_double_fill_large_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     /* 100×100 = 10000 elements — exercises SIMD unrolled body and
//        scalar/masked tail for any vector width up to 512-bit. */
//     double_matrix_expect_t r = init_double_dense_matrix(100, 100, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     assert_int_equal(fill_double_matrix(m, -2.5), NO_ERROR);
//  
//     double val = 0.0;
//     get_double_matrix(m, 0, 0, &val);     ASSERT_DOUBLE_EXACT(val, -2.5);
//     get_double_matrix(m, 49, 50, &val);   ASSERT_DOUBLE_EXACT(val, -2.5);
//     get_double_matrix(m, 99, 99, &val);   ASSERT_DOUBLE_EXACT(val, -2.5);
//  
//     return_double_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_transpose_large_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     /* 17×19 — not a multiple of any SIMD width, exercises all remainder paths.
//        For doubles: not a multiple of 2 (SSE/NEON), 4 (AVX), or 8 (AVX-512). */
//     double_matrix_expect_t r = init_double_dense_matrix(17, 19, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     /* Fill with distinct values */
//     for (size_t i = 0; i < 17; ++i) {
//         for (size_t j = 0; j < 19; ++j) {
//             set_double_matrix(m, i, j, (double)(i * 19 + j));
//         }
//     }
//  
//     double_matrix_expect_t tr = transpose_double_matrix(m, alloc);
//     assert_true(tr.has_value);
//     double_matrix_t* t = tr.u.value;
//  
//     assert_int_equal((int)double_matrix_rows(t), 19);
//     assert_int_equal((int)double_matrix_cols(t), 17);
//  
//     /* Verify every element: t[j][i] == m[i][j] */
//     double vm = 0.0, vt = 0.0;
//     for (size_t i = 0; i < 17; ++i) {
//         for (size_t j = 0; j < 19; ++j) {
//             get_double_matrix(m, i, j, &vm);
//             get_double_matrix(t, j, i, &vt);
//             ASSERT_DOUBLE_EXACT(vm, vt);
//         }
//     }
//  
//     return_double_matrix(m);
//     return_double_matrix(t);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_equal_large_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(50, 50, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     fill_double_matrix(m, 7.0);
//  
//     double_matrix_expect_t cr = copy_double_matrix(m, alloc);
//     assert_true(cr.has_value);
//  
//     assert_true(double_matrix_equal(m, cr.u.value));
//  
//     /* Flip one element near the end to test early-exit vs. full scan */
//     set_double_matrix(cr.u.value, 49, 49, 8.0);
//     assert_false(double_matrix_equal(m, cr.u.value));
//  
//     return_double_matrix(m);
//     return_double_matrix(cr.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_double_convert_large_dense_to_csr(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     /* 20×20 with sparse population (diagonal only) */
//     double_matrix_expect_t r = init_double_dense_matrix(20, 20, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     /* Set only diagonal */
//     for (size_t i = 0; i < 20; ++i) {
//         set_double_matrix(m, i, i, (double)(i + 1));
//     }
//  
//     double_matrix_expect_t cr = convert_double_matrix(m, CSR_MATRIX, alloc);
//     assert_true(cr.has_value);
//     double_matrix_t* csr = cr.u.value;
//  
//     assert_int_equal((int)double_matrix_nnz(csr), 20);
//  
//     /* Verify diagonal values survive conversion */
//     double val = 0.0;
//     for (size_t i = 0; i < 20; ++i) {
//         get_double_matrix(csr, i, i, &val);
//         ASSERT_DOUBLE_EXACT(val, (double)(i + 1));
//     }
//  
//     /* Verify off-diagonal is zero */
//     get_double_matrix(csr, 0, 1, &val); ASSERT_DOUBLE_EXACT(val, 0.0);
//     get_double_matrix(csr, 19, 0, &val); ASSERT_DOUBLE_EXACT(val, 0.0);
//  
//     return_double_matrix(m);
//     return_double_matrix(csr);
// }
//  
// // ================================================================================
// // Group 18: Double-specific precision test
// // ================================================================================
//  
// static void test_double_preserves_full_precision(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     double_matrix_expect_t r = init_double_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     double_matrix_t* m = r.u.value;
//  
//     /* Values that require more than 24 mantissa bits (float precision)
//        to represent exactly.  If the implementation accidentally used
//        float anywhere, these would be truncated. */
//     double precise_a = 1.0000000000000002;   /* 1 + 2^-52 */
//     double precise_b = 9007199254740993.0;   /* 2^53 + 1  */
//  
//     set_double_matrix(m, 0, 0, precise_a);
//     set_double_matrix(m, 1, 1, precise_b);
//  
//     double val = 0.0;
//     get_double_matrix(m, 0, 0, &val); ASSERT_DOUBLE_EXACT(val, precise_a);
//     get_double_matrix(m, 1, 1, &val); ASSERT_DOUBLE_EXACT(val, precise_b);
//  
//     /* Verify through copy as well */
//     double_matrix_expect_t cr = copy_double_matrix(m, alloc);
//     assert_true(cr.has_value);
//  
//     get_double_matrix(cr.u.value, 0, 0, &val); ASSERT_DOUBLE_EXACT(val, precise_a);
//     get_double_matrix(cr.u.value, 1, 1, &val); ASSERT_DOUBLE_EXACT(val, precise_b);
//  
//     return_double_matrix(m);
//     return_double_matrix(cr.u.value);
// }
//  
// // ================================================================================
// // Test suite export
// // ================================================================================
//  
// const struct CMUnitTest test_double_matrix[] = {
//     /* Group 1: init dense */
//     cmocka_unit_test(test_double_dense_init_null_allocator_fails),
//     cmocka_unit_test(test_double_dense_init_zero_rows_fails),
//     cmocka_unit_test(test_double_dense_init_zero_cols_fails),
//     cmocka_unit_test(test_double_dense_init_returns_valid_matrix),
//     cmocka_unit_test(test_double_dense_init_elements_are_zero),
//     /* Group 2: init COO */
//     cmocka_unit_test(test_double_coo_init_returns_valid_matrix),
//     cmocka_unit_test(test_double_coo_init_zero_capacity_fails),
//     /* Group 3: return */
//     cmocka_unit_test(test_double_return_null_is_safe),
//     /* Group 4: get/set dense */
//     cmocka_unit_test(test_double_get_null_matrix_returns_null_pointer),
//     cmocka_unit_test(test_double_get_null_out_returns_null_pointer),
//     cmocka_unit_test(test_double_set_null_matrix_returns_null_pointer),
//     cmocka_unit_test(test_double_set_and_get_round_trip),
//     cmocka_unit_test(test_double_set_out_of_bounds_returns_error),
//     /* Group 5: COO assembly */
//     cmocka_unit_test(test_double_coo_push_back_and_retrieve),
//     cmocka_unit_test(test_double_coo_push_back_overwrites_existing),
//     cmocka_unit_test(test_double_coo_sort_orders_entries),
//     /* Group 6: clear */
//     cmocka_unit_test(test_double_clear_dense_zeros_all_elements),
//     cmocka_unit_test(test_double_clear_coo_resets_nnz),
//     /* Group 7: copy */
//     cmocka_unit_test(test_double_copy_null_returns_error),
//     cmocka_unit_test(test_double_copy_dense_produces_independent_matrix),
//     /* Group 8: fill (SIMD) */
//     cmocka_unit_test(test_double_fill_null_returns_error),
//     cmocka_unit_test(test_double_fill_dense_sets_all_elements),
//     cmocka_unit_test(test_double_fill_zero_clears_matrix),
//     /* Group 9: is_zero (SIMD) */
//     cmocka_unit_test(test_double_is_zero_null_returns_true),
//     cmocka_unit_test(test_double_is_zero_freshly_initialized_dense),
//     cmocka_unit_test(test_double_is_zero_returns_false_after_set),
//     /* Group 10: equal (SIMD) */
//     cmocka_unit_test(test_double_equal_null_returns_false),
//     cmocka_unit_test(test_double_equal_identical_matrices),
//     cmocka_unit_test(test_double_equal_different_values_returns_false),
//     cmocka_unit_test(test_double_equal_different_shape_returns_false),
//     /* Group 11: transpose (SIMD) */
//     cmocka_unit_test(test_double_transpose_null_returns_error),
//     cmocka_unit_test(test_double_transpose_dense_swaps_dimensions),
//     cmocka_unit_test(test_double_transpose_square_is_correct),
//     /* Group 12: convert dense→CSR (SIMD) */
//     cmocka_unit_test(test_double_convert_dense_to_csr_preserves_values),
//     cmocka_unit_test(test_double_convert_all_zero_dense_to_csr),
//     /* Group 13: introspection */
//     cmocka_unit_test(test_double_introspection_null_returns_defaults),
//     cmocka_unit_test(test_double_dense_nnz_equals_rows_times_cols),
//     cmocka_unit_test(test_double_format_name_returns_string),
//     cmocka_unit_test(test_double_storage_bytes_dense),
//     /* Group 14: shape/compat */
//     cmocka_unit_test(test_double_same_shape_true_for_matching_dimensions),
//     cmocka_unit_test(test_double_is_square),
//     cmocka_unit_test(test_double_is_sparse),
//     cmocka_unit_test(test_double_add_compatible),
//     cmocka_unit_test(test_double_multiply_compatible),
//     /* Group 15: swaps */
//     cmocka_unit_test(test_double_swap_rows_dense),
//     cmocka_unit_test(test_double_swap_cols_dense),
//     /* Group 16: special constructors */
//     cmocka_unit_test(test_double_identity_matrix),
//     cmocka_unit_test(test_double_row_vector_shape),
//     cmocka_unit_test(test_double_col_vector_shape),
//     cmocka_unit_test(test_double_non_vector_returns_zero_length),
//     /* Group 17: large matrix smoke tests */
//     cmocka_unit_test(test_double_fill_large_matrix),
//     cmocka_unit_test(test_double_transpose_large_matrix),
//     cmocka_unit_test(test_double_equal_large_matrix),
//     cmocka_unit_test(test_double_convert_large_dense_to_csr),
//     /* Group 18: double-specific precision */
//     cmocka_unit_test(test_double_preserves_full_precision),
// };
// const size_t test_double_matrix_count =
//     sizeof(test_double_matrix) / sizeof(test_double_matrix[0]);
// // ================================================================================ 
// // ================================================================================ 
//
//
// /* Exact long double equality for binary fractions used in tests. */
// #define ASSERT_LDOUBLE_EXACT(a, b) assert_true((a) == (b))
//  
// // ================================================================================
// // Group 1: init_ldouble_dense_matrix
// // ================================================================================
//  
// static void test_ldouble_dense_init_null_allocator_fails(void** state) {
//     (void)state;
//     allocator_vtable_t bad = { 0 };
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3, 4, bad);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_dense_init_zero_rows_fails(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(0, 4, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, INVALID_ARG);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_dense_init_zero_cols_fails(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3, 0, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, INVALID_ARG);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_dense_init_returns_valid_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3, 4, alloc);
//     assert_true(r.has_value);
//     assert_non_null(r.u.value);
//     assert_int_equal((int)ldouble_matrix_rows(r.u.value), 3);
//     assert_int_equal((int)ldouble_matrix_cols(r.u.value), 4);
//     assert_int_equal((int)ldouble_matrix_format(r.u.value), DENSE_MATRIX);
//     return_ldouble_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_dense_init_elements_are_zero(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     long double val = -1.0L;
//     for (size_t i = 0; i < 2; ++i) {
//         for (size_t j = 0; j < 3; ++j) {
//             assert_int_equal(get_ldouble_matrix(m, i, j, &val), NO_ERROR);
//             ASSERT_LDOUBLE_EXACT(val, 0.0L);
//         }
//     }
//  
//     return_ldouble_matrix(m);
// }
//  
// // ================================================================================
// // Group 2: init_ldouble_coo_matrix
// // ================================================================================
//  
// static void test_ldouble_coo_init_returns_valid_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_coo_matrix(10, 10, 8, true, alloc);
//     assert_true(r.has_value);
//     assert_int_equal((int)ldouble_matrix_rows(r.u.value), 10);
//     assert_int_equal((int)ldouble_matrix_cols(r.u.value), 10);
//     assert_int_equal((int)ldouble_matrix_format(r.u.value), COO_MATRIX);
//     assert_int_equal((int)ldouble_matrix_nnz(r.u.value), 0);
//     return_ldouble_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_coo_init_zero_capacity_fails(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_coo_matrix(5, 5, 0, true, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, INVALID_ARG);
// }
//  
// // ================================================================================
// // Group 3: return_ldouble_matrix
// // ================================================================================
//  
// static void test_ldouble_return_null_is_safe(void** state) {
//     (void)state;
//     return_ldouble_matrix(NULL);
// }
//  
// // ================================================================================
// // Group 4: get/set_ldouble_matrix (dense)
// // ================================================================================
//  
// static void test_ldouble_get_null_matrix_returns_null_pointer(void** state) {
//     (void)state;
//     long double val = 0.0L;
//     assert_int_equal(get_ldouble_matrix(NULL, 0, 0, &val), NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_get_null_out_returns_null_pointer(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     assert_int_equal(get_ldouble_matrix(r.u.value, 0, 0, NULL), NULL_POINTER);
//     return_ldouble_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_set_null_matrix_returns_null_pointer(void** state) {
//     (void)state;
//     assert_int_equal(set_ldouble_matrix(NULL, 0, 0, 1.0L), NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_set_and_get_round_trip(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     assert_int_equal(set_ldouble_matrix(m, 0, 0, 1.5L), NO_ERROR);
//     assert_int_equal(set_ldouble_matrix(m, 1, 2, -3.0L), NO_ERROR);
//     assert_int_equal(set_ldouble_matrix(m, 2, 1, 42.0L), NO_ERROR);
//  
//     long double val = 0.0L;
//     get_ldouble_matrix(m, 0, 0, &val); ASSERT_LDOUBLE_EXACT(val, 1.5L);
//     get_ldouble_matrix(m, 1, 2, &val); ASSERT_LDOUBLE_EXACT(val, -3.0L);
//     get_ldouble_matrix(m, 2, 1, &val); ASSERT_LDOUBLE_EXACT(val, 42.0L);
//  
//     /* Unset element should still be zero */
//     get_ldouble_matrix(m, 0, 1, &val); ASSERT_LDOUBLE_EXACT(val, 0.0L);
//  
//     return_ldouble_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_set_out_of_bounds_returns_error(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//  
//     assert_int_equal(set_ldouble_matrix(r.u.value, 2, 0, 1.0L), INVALID_ARG);
//     assert_int_equal(set_ldouble_matrix(r.u.value, 0, 3, 1.0L), INVALID_ARG);
//  
//     return_ldouble_matrix(r.u.value);
// }
//  
// // ================================================================================
// // Group 5: COO assembly (push_back, sort, reserve)
// // ================================================================================
//  
// static void test_ldouble_coo_push_back_and_retrieve(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_coo_matrix(5, 5, 4, true, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     assert_int_equal(push_back_ldouble_coo_matrix(m, 0, 2, 3.14159265358979L), NO_ERROR);
//     assert_int_equal(push_back_ldouble_coo_matrix(m, 3, 4, -1.0L), NO_ERROR);
//     assert_int_equal((int)ldouble_matrix_nnz(m), 2);
//  
//     long double val = 0.0L;
//     get_ldouble_matrix(m, 0, 2, &val); ASSERT_LDOUBLE_EXACT(val, 3.14159265358979L);
//     get_ldouble_matrix(m, 3, 4, &val); ASSERT_LDOUBLE_EXACT(val, -1.0L);
//  
//     /* Unset entry returns zero */
//     get_ldouble_matrix(m, 1, 1, &val); ASSERT_LDOUBLE_EXACT(val, 0.0L);
//  
//     return_ldouble_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_coo_push_back_overwrites_existing(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_coo_matrix(3, 3, 4, true, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     push_back_ldouble_coo_matrix(m, 1, 1, 10.0L);
//     push_back_ldouble_coo_matrix(m, 1, 1, 20.0L);  /* overwrite */
//     assert_int_equal((int)ldouble_matrix_nnz(m), 1);
//  
//     long double val = 0.0L;
//     get_ldouble_matrix(m, 1, 1, &val);
//     ASSERT_LDOUBLE_EXACT(val, 20.0L);
//  
//     return_ldouble_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_coo_sort_orders_entries(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_coo_matrix(4, 4, 8, true, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     /* Insert out of order */
//     push_back_ldouble_coo_matrix(m, 3, 0, 4.0L);
//     push_back_ldouble_coo_matrix(m, 0, 1, 1.0L);
//     push_back_ldouble_coo_matrix(m, 1, 2, 2.0L);
//  
//     assert_int_equal(sort_ldouble_coo_matrix(m), NO_ERROR);
//  
//     /* Values should still be retrievable after sort */
//     long double val = 0.0L;
//     get_ldouble_matrix(m, 0, 1, &val); ASSERT_LDOUBLE_EXACT(val, 1.0L);
//     get_ldouble_matrix(m, 1, 2, &val); ASSERT_LDOUBLE_EXACT(val, 2.0L);
//     get_ldouble_matrix(m, 3, 0, &val); ASSERT_LDOUBLE_EXACT(val, 4.0L);
//  
//     return_ldouble_matrix(m);
// }
//  
// // ================================================================================
// // Group 6: clear_ldouble_matrix
// // ================================================================================
//  
// static void test_ldouble_clear_dense_zeros_all_elements(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     set_ldouble_matrix(m, 0, 0, 5.0L);
//     set_ldouble_matrix(m, 1, 1, 9.0L);
//     assert_int_equal(clear_ldouble_matrix(m), NO_ERROR);
//  
//     assert_true(is_ldouble_matrix_zero(m));
//  
//     return_ldouble_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_clear_coo_resets_nnz(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_coo_matrix(3, 3, 4, true, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     push_back_ldouble_coo_matrix(m, 0, 0, 1.0L);
//     push_back_ldouble_coo_matrix(m, 1, 1, 2.0L);
//     assert_int_equal(clear_ldouble_matrix(m), NO_ERROR);
//     assert_int_equal((int)ldouble_matrix_nnz(m), 0);
//  
//     return_ldouble_matrix(m);
// }
//  
// // ================================================================================
// // Group 7: copy_ldouble_matrix
// // ================================================================================
//  
// static void test_ldouble_copy_null_returns_error(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = copy_ldouble_matrix(NULL, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_copy_dense_produces_independent_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* src = r.u.value;
//  
//     set_ldouble_matrix(src, 0, 0, 1.0L);
//     set_ldouble_matrix(src, 1, 2, 7.0L);
//  
//     ldouble_matrix_expect_t cr = copy_ldouble_matrix(src, alloc);
//     assert_true(cr.has_value);
//     ldouble_matrix_t* dst = cr.u.value;
//  
//     /* Mutating src must not affect copy */
//     set_ldouble_matrix(src, 0, 0, 999.0L);
//  
//     long double val = 0.0L;
//     get_ldouble_matrix(dst, 0, 0, &val); ASSERT_LDOUBLE_EXACT(val, 1.0L);
//     get_ldouble_matrix(dst, 1, 2, &val); ASSERT_LDOUBLE_EXACT(val, 7.0L);
//  
//     return_ldouble_matrix(src);
//     return_ldouble_matrix(dst);
// }
//  
// // ================================================================================
// // Group 8: fill_ldouble_matrix
// // ================================================================================
//  
// static void test_ldouble_fill_null_returns_error(void** state) {
//     (void)state;
//     assert_int_equal(fill_ldouble_matrix(NULL, 1.0L), NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_fill_dense_sets_all_elements(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(4, 5, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     assert_int_equal(fill_ldouble_matrix(m, 3.5L), NO_ERROR);
//  
//     long double val = 0.0L;
//     for (size_t i = 0; i < 4; ++i) {
//         for (size_t j = 0; j < 5; ++j) {
//             get_ldouble_matrix(m, i, j, &val);
//             ASSERT_LDOUBLE_EXACT(val, 3.5L);
//         }
//     }
//  
//     return_ldouble_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_fill_zero_clears_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     fill_ldouble_matrix(m, 5.0L);
//     assert_int_equal(fill_ldouble_matrix(m, 0.0L), NO_ERROR);
//     assert_true(is_ldouble_matrix_zero(m));
//  
//     return_ldouble_matrix(m);
// }
//  
// // ================================================================================
// // Group 9: is_ldouble_matrix_zero
// // ================================================================================
//  
// static void test_ldouble_is_zero_null_returns_true(void** state) {
//     (void)state;
//     assert_true(is_ldouble_matrix_zero(NULL));
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_is_zero_freshly_initialized_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(5, 5, alloc);
//     assert_true(r.has_value);
//     assert_true(is_ldouble_matrix_zero(r.u.value));
//     return_ldouble_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_is_zero_returns_false_after_set(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     set_ldouble_matrix(m, 1, 1, 0.001L);
//     assert_false(is_ldouble_matrix_zero(m));
//  
//     return_ldouble_matrix(m);
// }
//  
// // ================================================================================
// // Group 10: ldouble_matrix_equal
// // ================================================================================
//  
// static void test_ldouble_equal_null_returns_false(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//  
//     assert_false(ldouble_matrix_equal(NULL, r.u.value));
//     assert_false(ldouble_matrix_equal(r.u.value, NULL));
//  
//     return_ldouble_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_equal_identical_matrices(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* a = r.u.value;
//  
//     set_ldouble_matrix(a, 0, 0, 1.0L);
//     set_ldouble_matrix(a, 1, 2, -4.5L);
//  
//     ldouble_matrix_expect_t cr = copy_ldouble_matrix(a, alloc);
//     assert_true(cr.has_value);
//  
//     assert_true(ldouble_matrix_equal(a, cr.u.value));
//  
//     return_ldouble_matrix(a);
//     return_ldouble_matrix(cr.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_equal_different_values_returns_false(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t ra = init_ldouble_dense_matrix(2, 2, alloc);
//     ldouble_matrix_expect_t rb = init_ldouble_dense_matrix(2, 2, alloc);
//     assert_true(ra.has_value);
//     assert_true(rb.has_value);
//  
//     set_ldouble_matrix(ra.u.value, 0, 0, 1.0L);
//     set_ldouble_matrix(rb.u.value, 0, 0, 2.0L);
//  
//     assert_false(ldouble_matrix_equal(ra.u.value, rb.u.value));
//  
//     return_ldouble_matrix(ra.u.value);
//     return_ldouble_matrix(rb.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_equal_different_shape_returns_false(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t ra = init_ldouble_dense_matrix(2, 3, alloc);
//     ldouble_matrix_expect_t rb = init_ldouble_dense_matrix(3, 2, alloc);
//     assert_true(ra.has_value);
//     assert_true(rb.has_value);
//  
//     assert_false(ldouble_matrix_equal(ra.u.value, rb.u.value));
//  
//     return_ldouble_matrix(ra.u.value);
//     return_ldouble_matrix(rb.u.value);
// }
//  
// // ================================================================================
// // Group 11: transpose_ldouble_matrix
// // ================================================================================
//  
// static void test_ldouble_transpose_null_returns_error(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = transpose_ldouble_matrix(NULL, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_transpose_dense_swaps_dimensions(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     set_ldouble_matrix(m, 0, 0, 10.0L);
//     set_ldouble_matrix(m, 0, 1, 11.0L);
//     set_ldouble_matrix(m, 0, 2, 12.0L);
//     set_ldouble_matrix(m, 1, 0, 20.0L);
//     set_ldouble_matrix(m, 1, 1, 21.0L);
//     set_ldouble_matrix(m, 1, 2, 22.0L);
//  
//     ldouble_matrix_expect_t tr = transpose_ldouble_matrix(m, alloc);
//     assert_true(tr.has_value);
//     ldouble_matrix_t* t = tr.u.value;
//  
//     assert_int_equal((int)ldouble_matrix_rows(t), 3);
//     assert_int_equal((int)ldouble_matrix_cols(t), 2);
//  
//     long double val = 0.0L;
//     get_ldouble_matrix(t, 0, 0, &val); ASSERT_LDOUBLE_EXACT(val, 10.0L);
//     get_ldouble_matrix(t, 0, 1, &val); ASSERT_LDOUBLE_EXACT(val, 20.0L);
//     get_ldouble_matrix(t, 1, 0, &val); ASSERT_LDOUBLE_EXACT(val, 11.0L);
//     get_ldouble_matrix(t, 1, 1, &val); ASSERT_LDOUBLE_EXACT(val, 21.0L);
//     get_ldouble_matrix(t, 2, 0, &val); ASSERT_LDOUBLE_EXACT(val, 12.0L);
//     get_ldouble_matrix(t, 2, 1, &val); ASSERT_LDOUBLE_EXACT(val, 22.0L);
//  
//     return_ldouble_matrix(m);
//     return_ldouble_matrix(t);
// }
//  
// // ================================================================================
// // Group 12: convert_ldouble_matrix dense→CSR
// // ================================================================================
//  
// static void test_ldouble_convert_dense_to_csr_preserves_values(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     set_ldouble_matrix(m, 0, 0, 1.0L);
//     set_ldouble_matrix(m, 0, 2, 2.0L);
//     set_ldouble_matrix(m, 1, 1, 3.0L);
//     set_ldouble_matrix(m, 2, 0, 4.0L);
//     set_ldouble_matrix(m, 2, 2, 5.0L);
//  
//     ldouble_matrix_expect_t cr = convert_ldouble_matrix(m, CSR_MATRIX, alloc);
//     assert_true(cr.has_value);
//     ldouble_matrix_t* csr = cr.u.value;
//  
//     assert_int_equal((int)ldouble_matrix_format(csr), CSR_MATRIX);
//     assert_int_equal((int)ldouble_matrix_nnz(csr), 5);
//  
//     long double val = 0.0L;
//     get_ldouble_matrix(csr, 0, 0, &val); ASSERT_LDOUBLE_EXACT(val, 1.0L);
//     get_ldouble_matrix(csr, 0, 2, &val); ASSERT_LDOUBLE_EXACT(val, 2.0L);
//     get_ldouble_matrix(csr, 1, 1, &val); ASSERT_LDOUBLE_EXACT(val, 3.0L);
//     get_ldouble_matrix(csr, 2, 0, &val); ASSERT_LDOUBLE_EXACT(val, 4.0L);
//     get_ldouble_matrix(csr, 2, 2, &val); ASSERT_LDOUBLE_EXACT(val, 5.0L);
//  
//     /* Zero entries must read as zero */
//     get_ldouble_matrix(csr, 0, 1, &val); ASSERT_LDOUBLE_EXACT(val, 0.0L);
//  
//     return_ldouble_matrix(m);
//     return_ldouble_matrix(csr);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_convert_all_zero_dense_to_csr(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//  
//     ldouble_matrix_expect_t cr = convert_ldouble_matrix(r.u.value, CSR_MATRIX, alloc);
//     assert_true(cr.has_value);
//     assert_int_equal((int)ldouble_matrix_nnz(cr.u.value), 0);
//
//     return_ldouble_matrix(r.u.value);
//     return_ldouble_matrix(cr.u.value);
// }
//  
// // ================================================================================
// // Group 13: Introspection
// // ================================================================================
//  
// static void test_ldouble_introspection_null_returns_defaults(void** state) {
//     (void)state;
//     assert_int_equal((int)ldouble_matrix_rows(NULL), 0);
//     assert_int_equal((int)ldouble_matrix_cols(NULL), 0);
//     assert_int_equal((int)ldouble_matrix_nnz(NULL), 0);
//     assert_int_equal((int)ldouble_matrix_storage_bytes(NULL), 0);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_dense_nnz_equals_rows_times_cols(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(4, 5, alloc);
//     assert_true(r.has_value);
//     assert_int_equal((int)ldouble_matrix_nnz(r.u.value), 20);
//     return_ldouble_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_format_name_returns_string(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     assert_string_equal(ldouble_matrix_format_name(r.u.value), "DENSE_MATRIX");
//     return_ldouble_matrix(r.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_storage_bytes_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3, 4, alloc);
//     assert_true(r.has_value);
//     /* 3 * 4 * sizeof(long double) — platform-dependent */
//     size_t expected = 3u * 4u * sizeof(long double);
//     assert_int_equal((int)ldouble_matrix_storage_bytes(r.u.value), (int)expected);
//     return_ldouble_matrix(r.u.value);
// }
//  
// // ================================================================================
// // Group 14: Shape and compatibility queries
// // ================================================================================
//  
// static void test_ldouble_same_shape_true_for_matching_dimensions(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t ra = init_ldouble_dense_matrix(3, 4, alloc);
//     ldouble_matrix_expect_t rb = init_ldouble_dense_matrix(3, 4, alloc);
//     assert_true(ra.has_value && rb.has_value);
//     assert_true(ldouble_matrix_has_same_shape(ra.u.value, rb.u.value));
//     return_ldouble_matrix(ra.u.value);
//     return_ldouble_matrix(rb.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_is_square(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t sq = init_ldouble_dense_matrix(3, 3, alloc);
//     ldouble_matrix_expect_t ns = init_ldouble_dense_matrix(3, 4, alloc);
//     assert_true(sq.has_value && ns.has_value);
//     assert_true(ldouble_matrix_is_square(sq.u.value));
//     assert_false(ldouble_matrix_is_square(ns.u.value));
//     return_ldouble_matrix(sq.u.value);
//     return_ldouble_matrix(ns.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_is_sparse(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t d = init_ldouble_dense_matrix(2, 2, alloc);
//     ldouble_matrix_expect_t c = init_ldouble_coo_matrix(2, 2, 4, true, alloc);
//     assert_true(d.has_value && c.has_value);
//     assert_false(ldouble_matrix_is_sparse(d.u.value));
//     assert_true(ldouble_matrix_is_sparse(c.u.value));
//     return_ldouble_matrix(d.u.value);
//     return_ldouble_matrix(c.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_add_compatible(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t ra = init_ldouble_dense_matrix(3, 4, alloc);
//     ldouble_matrix_expect_t rb = init_ldouble_dense_matrix(3, 4, alloc);
//     ldouble_matrix_expect_t rc = init_ldouble_dense_matrix(4, 3, alloc);
//     assert_true(ra.has_value && rb.has_value && rc.has_value);
//  
//     assert_true(ldouble_matrix_is_add_compatible(ra.u.value, rb.u.value));
//     assert_false(ldouble_matrix_is_add_compatible(ra.u.value, rc.u.value));
//  
//     return_ldouble_matrix(ra.u.value);
//     return_ldouble_matrix(rb.u.value);
//     return_ldouble_matrix(rc.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_multiply_compatible(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t ra = init_ldouble_dense_matrix(2, 3, alloc);
//     ldouble_matrix_expect_t rb = init_ldouble_dense_matrix(3, 5, alloc);
//     ldouble_matrix_expect_t rc = init_ldouble_dense_matrix(4, 5, alloc);
//     assert_true(ra.has_value && rb.has_value && rc.has_value);
//  
//     assert_true(ldouble_matrix_is_multiply_compatible(ra.u.value, rb.u.value));
//     assert_false(ldouble_matrix_is_multiply_compatible(ra.u.value, rc.u.value));
//  
//     return_ldouble_matrix(ra.u.value);
//     return_ldouble_matrix(rb.u.value);
//     return_ldouble_matrix(rc.u.value);
// }
//  
// // ================================================================================
// // Group 15: Row / column swaps
// // ================================================================================
//  
// static void test_ldouble_swap_rows_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3, 2, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     set_ldouble_matrix(m, 0, 0, 1.0L); set_ldouble_matrix(m, 0, 1, 2.0L);
//     set_ldouble_matrix(m, 1, 0, 3.0L); set_ldouble_matrix(m, 1, 1, 4.0L);
//     set_ldouble_matrix(m, 2, 0, 5.0L); set_ldouble_matrix(m, 2, 1, 6.0L);
//  
//     assert_int_equal(swap_ldouble_matrix_rows(m, 0, 2), NO_ERROR);
//  
//     long double val = 0.0L;
//     get_ldouble_matrix(m, 0, 0, &val); ASSERT_LDOUBLE_EXACT(val, 5.0L);
//     get_ldouble_matrix(m, 0, 1, &val); ASSERT_LDOUBLE_EXACT(val, 6.0L);
//     get_ldouble_matrix(m, 2, 0, &val); ASSERT_LDOUBLE_EXACT(val, 1.0L);
//     get_ldouble_matrix(m, 2, 1, &val); ASSERT_LDOUBLE_EXACT(val, 2.0L);
//  
//     return_ldouble_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_swap_cols_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     set_ldouble_matrix(m, 0, 0, 1.0L); set_ldouble_matrix(m, 0, 2, 3.0L);
//     set_ldouble_matrix(m, 1, 0, 4.0L); set_ldouble_matrix(m, 1, 2, 6.0L);
//  
//     assert_int_equal(swap_ldouble_matrix_cols(m, 0, 2), NO_ERROR);
//  
//     long double val = 0.0L;
//     get_ldouble_matrix(m, 0, 0, &val); ASSERT_LDOUBLE_EXACT(val, 3.0L);
//     get_ldouble_matrix(m, 0, 2, &val); ASSERT_LDOUBLE_EXACT(val, 1.0L);
//     get_ldouble_matrix(m, 1, 0, &val); ASSERT_LDOUBLE_EXACT(val, 6.0L);
//     get_ldouble_matrix(m, 1, 2, &val); ASSERT_LDOUBLE_EXACT(val, 4.0L);
//  
//     return_ldouble_matrix(m);
// }
//  
// // ================================================================================
// // Group 16: Special constructors (identity, row/col vector)
// // ================================================================================
//  
// static void test_ldouble_identity_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_identity_matrix(3, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     assert_int_equal((int)ldouble_matrix_rows(m), 3);
//     assert_int_equal((int)ldouble_matrix_cols(m), 3);
//  
//     long double val = 0.0L;
//     for (size_t i = 0; i < 3; ++i) {
//         for (size_t j = 0; j < 3; ++j) {
//             get_ldouble_matrix(m, i, j, &val);
//             ASSERT_LDOUBLE_EXACT(val, (i == j) ? 1.0L : 0.0L);
//         }
//     }
//  
//     return_ldouble_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_row_vector_shape(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_row_vector(5, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     assert_int_equal((int)ldouble_matrix_rows(m), 1);
//     assert_int_equal((int)ldouble_matrix_cols(m), 5);
//     assert_true(ldouble_matrix_is_row_vector(m));
//     assert_false(ldouble_matrix_is_col_vector(m));
//     assert_true(ldouble_matrix_is_vector(m));
//     assert_int_equal((int)ldouble_matrix_vector_length(m), 5);
//  
//     return_ldouble_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_col_vector_shape(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_col_vector(4, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     assert_int_equal((int)ldouble_matrix_rows(m), 4);
//     assert_int_equal((int)ldouble_matrix_cols(m), 1);
//     assert_false(ldouble_matrix_is_row_vector(m));
//     assert_true(ldouble_matrix_is_col_vector(m));
//     assert_true(ldouble_matrix_is_vector(m));
//     assert_int_equal((int)ldouble_matrix_vector_length(m), 4);
//  
//     return_ldouble_matrix(m);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_non_vector_returns_zero_length(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3, 4, alloc);
//     assert_true(r.has_value);
//  
//     assert_false(ldouble_matrix_is_vector(r.u.value));
//     assert_int_equal((int)ldouble_matrix_vector_length(r.u.value), 0);
//  
//     return_ldouble_matrix(r.u.value);
// }
//  
// // ================================================================================
// // Group 17: Long-double-specific precision test
// // ================================================================================
//  
// static void test_ldouble_preserves_extended_precision(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     ldouble_matrix_t* m = r.u.value;
//  
//     /* Values that require more than 53 mantissa bits (double precision).
//        On x86 with 80-bit extended, long double has 64 mantissa bits.
//        If the implementation accidentally used double anywhere, these
//        would be truncated. */
//     long double precise_a = 1.0L + 1.0e-18L;
//     long double precise_b = 1234567890123456789.0L;
//  
//     set_ldouble_matrix(m, 0, 0, precise_a);
//     set_ldouble_matrix(m, 1, 1, precise_b);
//  
//     long double val = 0.0L;
//     get_ldouble_matrix(m, 0, 0, &val); ASSERT_LDOUBLE_EXACT(val, precise_a);
//     get_ldouble_matrix(m, 1, 1, &val); ASSERT_LDOUBLE_EXACT(val, precise_b);
//  
//     /* Verify through copy as well */
//     ldouble_matrix_expect_t cr = copy_ldouble_matrix(m, alloc);
//     assert_true(cr.has_value);
//  
//     get_ldouble_matrix(cr.u.value, 0, 0, &val); ASSERT_LDOUBLE_EXACT(val, precise_a);
//     get_ldouble_matrix(cr.u.value, 1, 1, &val); ASSERT_LDOUBLE_EXACT(val, precise_b);
//  
//     return_ldouble_matrix(m);
//     return_ldouble_matrix(cr.u.value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// static void test_ldouble_sizeof_matches_registration(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(1, 1, alloc);
//     assert_true(r.has_value);
//  
//     /* The matrix's data_size must match sizeof(long double) on this platform */
//     assert_int_equal((int)r.u.value->data_size, (int)sizeof(long double));
//  
//     return_ldouble_matrix(r.u.value);
// }
//  
// // ================================================================================
// // Test suite export
// // ================================================================================
//  
// const struct CMUnitTest test_ldouble_matrix[] = {
//     /* Group 1: init dense */
//     cmocka_unit_test(test_ldouble_dense_init_null_allocator_fails),
//     cmocka_unit_test(test_ldouble_dense_init_zero_rows_fails),
//     cmocka_unit_test(test_ldouble_dense_init_zero_cols_fails),
//     cmocka_unit_test(test_ldouble_dense_init_returns_valid_matrix),
//     cmocka_unit_test(test_ldouble_dense_init_elements_are_zero),
//     /* Group 2: init COO */
//     cmocka_unit_test(test_ldouble_coo_init_returns_valid_matrix),
//     cmocka_unit_test(test_ldouble_coo_init_zero_capacity_fails),
//     /* Group 3: return */
//     cmocka_unit_test(test_ldouble_return_null_is_safe),
//     /* Group 4: get/set dense */
//     cmocka_unit_test(test_ldouble_get_null_matrix_returns_null_pointer),
//     cmocka_unit_test(test_ldouble_get_null_out_returns_null_pointer),
//     cmocka_unit_test(test_ldouble_set_null_matrix_returns_null_pointer),
//     cmocka_unit_test(test_ldouble_set_and_get_round_trip),
//     cmocka_unit_test(test_ldouble_set_out_of_bounds_returns_error),
//     /* Group 5: COO assembly */
//     cmocka_unit_test(test_ldouble_coo_push_back_and_retrieve),
//     cmocka_unit_test(test_ldouble_coo_push_back_overwrites_existing),
//     cmocka_unit_test(test_ldouble_coo_sort_orders_entries),
//     /* Group 6: clear */
//     cmocka_unit_test(test_ldouble_clear_dense_zeros_all_elements),
//     cmocka_unit_test(test_ldouble_clear_coo_resets_nnz),
//     /* Group 7: copy */
//     cmocka_unit_test(test_ldouble_copy_null_returns_error),
//     cmocka_unit_test(test_ldouble_copy_dense_produces_independent_matrix),
//     /* Group 8: fill */
//     cmocka_unit_test(test_ldouble_fill_null_returns_error),
//     cmocka_unit_test(test_ldouble_fill_dense_sets_all_elements),
//     cmocka_unit_test(test_ldouble_fill_zero_clears_matrix),
//     /* Group 9: is_zero */
//     cmocka_unit_test(test_ldouble_is_zero_null_returns_true),
//     cmocka_unit_test(test_ldouble_is_zero_freshly_initialized_dense),
//     cmocka_unit_test(test_ldouble_is_zero_returns_false_after_set),
//     /* Group 10: equal */
//     cmocka_unit_test(test_ldouble_equal_null_returns_false),
//     cmocka_unit_test(test_ldouble_equal_identical_matrices),
//     cmocka_unit_test(test_ldouble_equal_different_values_returns_false),
//     cmocka_unit_test(test_ldouble_equal_different_shape_returns_false),
//     /* Group 11: transpose */
//     cmocka_unit_test(test_ldouble_transpose_null_returns_error),
//     cmocka_unit_test(test_ldouble_transpose_dense_swaps_dimensions),
//     /* Group 12: convert dense→CSR */
//     cmocka_unit_test(test_ldouble_convert_dense_to_csr_preserves_values),
//     cmocka_unit_test(test_ldouble_convert_all_zero_dense_to_csr),
//     /* Group 13: introspection */
//     cmocka_unit_test(test_ldouble_introspection_null_returns_defaults),
//     cmocka_unit_test(test_ldouble_dense_nnz_equals_rows_times_cols),
//     cmocka_unit_test(test_ldouble_format_name_returns_string),
//     cmocka_unit_test(test_ldouble_storage_bytes_dense),
//     /* Group 14: shape/compat */
//     cmocka_unit_test(test_ldouble_same_shape_true_for_matching_dimensions),
//     cmocka_unit_test(test_ldouble_is_square),
//     cmocka_unit_test(test_ldouble_is_sparse),
//     cmocka_unit_test(test_ldouble_add_compatible),
//     cmocka_unit_test(test_ldouble_multiply_compatible),
//     /* Group 15: swaps */
//     cmocka_unit_test(test_ldouble_swap_rows_dense),
//     cmocka_unit_test(test_ldouble_swap_cols_dense),
//     /* Group 16: special constructors */
//     cmocka_unit_test(test_ldouble_identity_matrix),
//     cmocka_unit_test(test_ldouble_row_vector_shape),
//     cmocka_unit_test(test_ldouble_col_vector_shape),
//     cmocka_unit_test(test_ldouble_non_vector_returns_zero_length),
//     /* Group 17: ldouble-specific precision */
//     cmocka_unit_test(test_ldouble_preserves_extended_precision),
//     cmocka_unit_test(test_ldouble_sizeof_matches_registration),
// };
// const size_t test_ldouble_matrix_count =
//     sizeof(test_ldouble_matrix) / sizeof(test_ldouble_matrix[0]);
// // ================================================================================ 
// // ================================================================================ 
//
// // ================================================================================
// // Group 1: init_uint8_dense_matrix
// // ================================================================================
//
// static void test_uint8_dense_init_null_allocator_fails(void** state) {
//     (void)state;
//     allocator_vtable_t bad = { 0 };
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(3, 4, bad);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_dense_init_zero_rows_fails(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(0, 4, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, INVALID_ARG);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_dense_init_zero_cols_fails(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(3, 0, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, INVALID_ARG);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_dense_init_returns_valid_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(3, 4, alloc);
//     assert_true(r.has_value);
//     assert_non_null(r.u.value);
//     assert_int_equal((int)uint8_matrix_rows(r.u.value), 3);
//     assert_int_equal((int)uint8_matrix_cols(r.u.value), 4);
//     assert_int_equal((int)uint8_matrix_format(r.u.value), DENSE_MATRIX);
//     return_uint8_matrix(r.u.value);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_dense_init_elements_are_zero(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     uint8_t val = 255u;
//     for (size_t i = 0; i < 2; ++i) {
//         for (size_t j = 0; j < 3; ++j) {
//             assert_int_equal(get_uint8_matrix(m, i, j, &val), NO_ERROR);
//             assert_int_equal((int)val, 0);
//         }
//     }
//
//     return_uint8_matrix(m);
// }
//
// // ================================================================================
// // Group 2: init_uint8_coo_matrix
// // ================================================================================
//
// static void test_uint8_coo_init_returns_valid_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_coo_matrix(10, 10, 8, true, alloc);
//     assert_true(r.has_value);
//     assert_int_equal((int)uint8_matrix_rows(r.u.value), 10);
//     assert_int_equal((int)uint8_matrix_cols(r.u.value), 10);
//     assert_int_equal((int)uint8_matrix_format(r.u.value), COO_MATRIX);
//     assert_int_equal((int)uint8_matrix_nnz(r.u.value), 0);
//     return_uint8_matrix(r.u.value);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_coo_init_zero_capacity_fails(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_coo_matrix(5, 5, 0, true, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, INVALID_ARG);
// }
//
// // ================================================================================
// // Group 3: return_uint8_matrix
// // ================================================================================
//
// static void test_return_uint8_null_is_safe(void** state) {
//     (void)state;
//     return_uint8_matrix(NULL);
// }
//
// // ================================================================================
// // Group 4: get/set_uint8_matrix (dense)
// // ================================================================================
//
// static void test_get_uint8_null_matrix_returns_null_pointer(void** state) {
//     (void)state;
//     uint8_t val = 0u;
//     assert_int_equal(get_uint8_matrix(NULL, 0, 0, &val), NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_get_uint8_null_out_returns_null_pointer(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     assert_int_equal(get_uint8_matrix(r.u.value, 0, 0, NULL), NULL_POINTER);
//     return_uint8_matrix(r.u.value);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_set_uint8_null_matrix_returns_null_pointer(void** state) {
//     (void)state;
//     assert_int_equal(set_uint8_matrix(NULL, 0, 0, 1u), NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_set_and_get_uint8_round_trip(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     assert_int_equal(set_uint8_matrix(m, 0, 0, 1u), NO_ERROR);
//     assert_int_equal(set_uint8_matrix(m, 1, 2, 3u), NO_ERROR);
//     assert_int_equal(set_uint8_matrix(m, 2, 1, 42u), NO_ERROR);
//
//     uint8_t val = 0u;
//     get_uint8_matrix(m, 0, 0, &val); assert_int_equal((int)val, 1);
//     get_uint8_matrix(m, 1, 2, &val); assert_int_equal((int)val, 3);
//     get_uint8_matrix(m, 2, 1, &val); assert_int_equal((int)val, 42);
//
//     get_uint8_matrix(m, 0, 1, &val); assert_int_equal((int)val, 0);
//
//     return_uint8_matrix(m);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_set_uint8_out_of_bounds_returns_error(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//
//     assert_int_equal(set_uint8_matrix(r.u.value, 2, 0, 1u), INVALID_ARG);
//     assert_int_equal(set_uint8_matrix(r.u.value, 0, 3, 1u), INVALID_ARG);
//
//     return_uint8_matrix(r.u.value);
// }
//
// // ================================================================================
// // Group 5: COO assembly (push_back, sort, reserve)
// // ================================================================================
//
// static void test_uint8_coo_push_back_and_retrieve(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_coo_matrix(5, 5, 4, true, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     assert_int_equal(push_back_uint8_coo_matrix(m, 0, 2, 3u), NO_ERROR);
//     assert_int_equal(push_back_uint8_coo_matrix(m, 3, 4, 9u), NO_ERROR);
//     assert_int_equal((int)uint8_matrix_nnz(m), 2);
//
//     uint8_t val = 0u;
//     get_uint8_matrix(m, 0, 2, &val); assert_int_equal((int)val, 3);
//     get_uint8_matrix(m, 3, 4, &val); assert_int_equal((int)val, 9);
//
//     get_uint8_matrix(m, 1, 1, &val); assert_int_equal((int)val, 0);
//
//     return_uint8_matrix(m);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_coo_push_back_overwrites_existing(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_coo_matrix(3, 3, 4, true, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     push_back_uint8_coo_matrix(m, 1, 1, 10u);
//     push_back_uint8_coo_matrix(m, 1, 1, 20u);
//     assert_int_equal((int)uint8_matrix_nnz(m), 1);
//
//     uint8_t val = 0u;
//     get_uint8_matrix(m, 1, 1, &val);
//     assert_int_equal((int)val, 20);
//
//     return_uint8_matrix(m);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_coo_sort_orders_entries(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_coo_matrix(4, 4, 8, true, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     push_back_uint8_coo_matrix(m, 3, 0, 4u);
//     push_back_uint8_coo_matrix(m, 0, 1, 1u);
//     push_back_uint8_coo_matrix(m, 1, 2, 2u);
//
//     assert_int_equal(sort_uint8_coo_matrix(m), NO_ERROR);
//
//     uint8_t val = 0u;
//     get_uint8_matrix(m, 0, 1, &val); assert_int_equal((int)val, 1);
//     get_uint8_matrix(m, 1, 2, &val); assert_int_equal((int)val, 2);
//     get_uint8_matrix(m, 3, 0, &val); assert_int_equal((int)val, 4);
//
//     return_uint8_matrix(m);
// }
//
// // ================================================================================
// // Group 6: clear_uint8_matrix
// // ================================================================================
//
// static void test_clear_uint8_dense_zeros_all_elements(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     set_uint8_matrix(m, 0, 0, 5u);
//     set_uint8_matrix(m, 1, 1, 9u);
//     assert_int_equal(clear_uint8_matrix(m), NO_ERROR);
//
//     assert_true(is_uint8_matrix_zero(m));
//
//     return_uint8_matrix(m);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_clear_uint8_coo_resets_nnz(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_coo_matrix(3, 3, 4, true, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     push_back_uint8_coo_matrix(m, 0, 0, 1u);
//     push_back_uint8_coo_matrix(m, 1, 1, 2u);
//     assert_int_equal(clear_uint8_matrix(m), NO_ERROR);
//     assert_int_equal((int)uint8_matrix_nnz(m), 0);
//
//     return_uint8_matrix(m);
// }
//
// // ================================================================================
// // Group 7: copy_uint8_matrix
// // ================================================================================
//
// static void test_copy_uint8_null_returns_error(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = copy_uint8_matrix(NULL, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_copy_uint8_dense_produces_independent_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* src = r.u.value;
//
//     set_uint8_matrix(src, 0, 0, 1u);
//     set_uint8_matrix(src, 1, 2, 7u);
//
//     uint8_matrix_expect_t cr = copy_uint8_matrix(src, alloc);
//     assert_true(cr.has_value);
//     uint8_matrix_t* dst = cr.u.value;
//
//     set_uint8_matrix(src, 0, 0, 99u);
//
//     uint8_t val = 0u;
//     get_uint8_matrix(dst, 0, 0, &val); assert_int_equal((int)val, 1);
//     get_uint8_matrix(dst, 1, 2, &val); assert_int_equal((int)val, 7);
//
//     return_uint8_matrix(src);
//     return_uint8_matrix(dst);
// }
//
// // ================================================================================
// // Group 8: fill_uint8_matrix (SIMD-accelerated)
// // ================================================================================
//
// static void test_fill_uint8_null_returns_error(void** state) {
//     (void)state;
//     assert_int_equal(fill_uint8_matrix(NULL, 1u), NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_fill_uint8_dense_sets_all_elements(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(4, 5, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     assert_int_equal(fill_uint8_matrix(m, 7u), NO_ERROR);
//
//     uint8_t val = 0u;
//     for (size_t i = 0; i < 4; ++i) {
//         for (size_t j = 0; j < 5; ++j) {
//             get_uint8_matrix(m, i, j, &val);
//             assert_int_equal((int)val, 7);
//         }
//     }
//
//     return_uint8_matrix(m);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_fill_uint8_zero_clears_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     fill_uint8_matrix(m, 5u);
//     assert_int_equal(fill_uint8_matrix(m, 0u), NO_ERROR);
//     assert_true(is_uint8_matrix_zero(m));
//
//     return_uint8_matrix(m);
// }
//
// // ================================================================================
// // Group 9: is_uint8_matrix_zero (SIMD-accelerated)
// // ================================================================================
//
// static void test_is_uint8_zero_null_returns_true(void** state) {
//     (void)state;
//     assert_true(is_uint8_matrix_zero(NULL));
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_is_uint8_zero_freshly_initialized_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(5, 5, alloc);
//     assert_true(r.has_value);
//     assert_true(is_uint8_matrix_zero(r.u.value));
//     return_uint8_matrix(r.u.value);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_is_uint8_zero_returns_false_after_set(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     set_uint8_matrix(m, 1, 1, 1u);
//     assert_false(is_uint8_matrix_zero(m));
//
//     return_uint8_matrix(m);
// }
//
// // ================================================================================
// // Group 10: uint8_matrix_equal (SIMD-accelerated)
// // ================================================================================
//
// static void test_uint8_equal_null_returns_false(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//
//     assert_false(uint8_matrix_equal(NULL, r.u.value));
//     assert_false(uint8_matrix_equal(r.u.value, NULL));
//
//     return_uint8_matrix(r.u.value);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_equal_identical_matrices(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* a = r.u.value;
//
//     set_uint8_matrix(a, 0, 0, 1u);
//     set_uint8_matrix(a, 1, 2, 45u);
//
//     uint8_matrix_expect_t cr = copy_uint8_matrix(a, alloc);
//     assert_true(cr.has_value);
//
//     assert_true(uint8_matrix_equal(a, cr.u.value));
//
//     return_uint8_matrix(a);
//     return_uint8_matrix(cr.u.value);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_equal_different_values_returns_false(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t ra = init_uint8_dense_matrix(2, 2, alloc);
//     uint8_matrix_expect_t rb = init_uint8_dense_matrix(2, 2, alloc);
//     assert_true(ra.has_value);
//     assert_true(rb.has_value);
//
//     set_uint8_matrix(ra.u.value, 0, 0, 1u);
//     set_uint8_matrix(rb.u.value, 0, 0, 2u);
//
//     assert_false(uint8_matrix_equal(ra.u.value, rb.u.value));
//
//     return_uint8_matrix(ra.u.value);
//     return_uint8_matrix(rb.u.value);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_equal_different_shape_returns_false(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t ra = init_uint8_dense_matrix(2, 3, alloc);
//     uint8_matrix_expect_t rb = init_uint8_dense_matrix(3, 2, alloc);
//     assert_true(ra.has_value);
//     assert_true(rb.has_value);
//
//     assert_false(uint8_matrix_equal(ra.u.value, rb.u.value));
//
//     return_uint8_matrix(ra.u.value);
//     return_uint8_matrix(rb.u.value);
// }
//
// // ================================================================================
// // Group 11: transpose_uint8_matrix (SIMD-accelerated)
// // ================================================================================
//
// static void test_transpose_uint8_null_returns_error(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = transpose_uint8_matrix(NULL, alloc);
//     assert_false(r.has_value);
//     assert_int_equal(r.u.error, NULL_POINTER);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_transpose_uint8_dense_swaps_dimensions(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     set_uint8_matrix(m, 0, 0, 10u);
//     set_uint8_matrix(m, 0, 1, 11u);
//     set_uint8_matrix(m, 0, 2, 12u);
//     set_uint8_matrix(m, 1, 0, 20u);
//     set_uint8_matrix(m, 1, 1, 21u);
//     set_uint8_matrix(m, 1, 2, 22u);
//
//     uint8_matrix_expect_t tr = transpose_uint8_matrix(m, alloc);
//     assert_true(tr.has_value);
//     uint8_matrix_t* t = tr.u.value;
//
//     assert_int_equal((int)uint8_matrix_rows(t), 3);
//     assert_int_equal((int)uint8_matrix_cols(t), 2);
//
//     uint8_t val = 0u;
//     get_uint8_matrix(t, 0, 0, &val); assert_int_equal((int)val, 10);
//     get_uint8_matrix(t, 0, 1, &val); assert_int_equal((int)val, 20);
//     get_uint8_matrix(t, 1, 0, &val); assert_int_equal((int)val, 11);
//     get_uint8_matrix(t, 1, 1, &val); assert_int_equal((int)val, 21);
//     get_uint8_matrix(t, 2, 0, &val); assert_int_equal((int)val, 12);
//     get_uint8_matrix(t, 2, 1, &val); assert_int_equal((int)val, 22);
//
//     return_uint8_matrix(m);
//     return_uint8_matrix(t);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_transpose_uint8_square_is_correct(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     set_uint8_matrix(m, 0, 1, 5u);
//     set_uint8_matrix(m, 1, 0, 7u);
//
//     uint8_matrix_expect_t tr = transpose_uint8_matrix(m, alloc);
//     assert_true(tr.has_value);
//
//     uint8_t val = 0u;
//     get_uint8_matrix(tr.u.value, 0, 1, &val); assert_int_equal((int)val, 7);
//     get_uint8_matrix(tr.u.value, 1, 0, &val); assert_int_equal((int)val, 5);
//
//     return_uint8_matrix(m);
//     return_uint8_matrix(tr.u.value);
// }
//
// // ================================================================================
// // Group 12: convert_uint8_matrix dense→CSR (SIMD-accelerated)
// // ================================================================================
//
// static void test_convert_uint8_dense_to_csr_preserves_values(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(3, 3, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     set_uint8_matrix(m, 0, 0, 1u);
//     set_uint8_matrix(m, 0, 2, 2u);
//     set_uint8_matrix(m, 1, 1, 3u);
//     set_uint8_matrix(m, 2, 0, 4u);
//     set_uint8_matrix(m, 2, 2, 5u);
//
//     uint8_matrix_expect_t cr = convert_uint8_matrix(m, CSR_MATRIX, alloc);
//     assert_true(cr.has_value);
//     uint8_matrix_t* csr = cr.u.value;
//
//     assert_int_equal((int)uint8_matrix_format(csr), CSR_MATRIX);
//     assert_int_equal((int)uint8_matrix_rows(csr), 3);
//     assert_int_equal((int)uint8_matrix_cols(csr), 3);
//     assert_int_equal((int)uint8_matrix_nnz(csr), 5);
//
//     uint8_t val = 0u;
//     get_uint8_matrix(csr, 0, 0, &val); assert_int_equal((int)val, 1);
//     get_uint8_matrix(csr, 0, 2, &val); assert_int_equal((int)val, 2);
//     get_uint8_matrix(csr, 1, 1, &val); assert_int_equal((int)val, 3);
//     get_uint8_matrix(csr, 2, 0, &val); assert_int_equal((int)val, 4);
//     get_uint8_matrix(csr, 2, 2, &val); assert_int_equal((int)val, 5);
//
//     get_uint8_matrix(csr, 0, 1, &val); assert_int_equal((int)val, 0);
//     get_uint8_matrix(csr, 1, 0, &val); assert_int_equal((int)val, 0);
//
//     return_uint8_matrix(m);
//     return_uint8_matrix(csr);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_convert_uint8_all_zero_dense_to_csr(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//
//     uint8_matrix_expect_t cr = convert_uint8_matrix(r.u.value, CSR_MATRIX, alloc);
//     assert_true(cr.has_value);
//     assert_int_equal((int)uint8_matrix_nnz(cr.u.value), 0);
//
//     return_uint8_matrix(r.u.value);
//     return_uint8_matrix(cr.u.value);
// }
//
// // ================================================================================
// // Group 13: Introspection
// // ================================================================================
//
// static void test_uint8_introspection_null_returns_defaults(void** state) {
//     (void)state;
//     assert_int_equal((int)uint8_matrix_rows(NULL), 0);
//     assert_int_equal((int)uint8_matrix_cols(NULL), 0);
//     assert_int_equal((int)uint8_matrix_nnz(NULL), 0);
//     assert_int_equal((int)uint8_matrix_storage_bytes(NULL), 0);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_dense_nnz_equals_rows_times_cols(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(4, 5, alloc);
//     assert_true(r.has_value);
//     assert_int_equal((int)uint8_matrix_nnz(r.u.value), 20);
//     return_uint8_matrix(r.u.value);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_format_name_returns_string(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(2, 2, alloc);
//     assert_true(r.has_value);
//     assert_string_equal(uint8_matrix_format_name(r.u.value), "DENSE_MATRIX");
//     return_uint8_matrix(r.u.value);
// }
//
// // ================================================================================
// // Group 14: Shape and compatibility queries
// // ================================================================================
//
// static void test_uint8_same_shape_true_for_matching_dimensions(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t ra = init_uint8_dense_matrix(3, 4, alloc);
//     uint8_matrix_expect_t rb = init_uint8_dense_matrix(3, 4, alloc);
//     assert_true(ra.has_value && rb.has_value);
//     assert_true(uint8_matrix_has_same_shape(ra.u.value, rb.u.value));
//     return_uint8_matrix(ra.u.value);
//     return_uint8_matrix(rb.u.value);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_is_square(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t sq = init_uint8_dense_matrix(3, 3, alloc);
//     uint8_matrix_expect_t ns = init_uint8_dense_matrix(3, 4, alloc);
//     assert_true(sq.has_value && ns.has_value);
//     assert_true(uint8_matrix_is_square(sq.u.value));
//     assert_false(uint8_matrix_is_square(ns.u.value));
//     return_uint8_matrix(sq.u.value);
//     return_uint8_matrix(ns.u.value);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_is_sparse(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t d = init_uint8_dense_matrix(2, 2, alloc);
//     uint8_matrix_expect_t c = init_uint8_coo_matrix(2, 2, 4, true, alloc);
//     assert_true(d.has_value && c.has_value);
//     assert_false(uint8_matrix_is_sparse(d.u.value));
//     assert_true(uint8_matrix_is_sparse(c.u.value));
//     return_uint8_matrix(d.u.value);
//     return_uint8_matrix(c.u.value);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_add_compatible(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t ra = init_uint8_dense_matrix(3, 4, alloc);
//     uint8_matrix_expect_t rb = init_uint8_dense_matrix(3, 4, alloc);
//     uint8_matrix_expect_t rc = init_uint8_dense_matrix(4, 3, alloc);
//     assert_true(ra.has_value && rb.has_value && rc.has_value);
//
//     assert_true(uint8_matrix_is_add_compatible(ra.u.value, rb.u.value));
//     assert_false(uint8_matrix_is_add_compatible(ra.u.value, rc.u.value));
//
//     return_uint8_matrix(ra.u.value);
//     return_uint8_matrix(rb.u.value);
//     return_uint8_matrix(rc.u.value);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_multiply_compatible(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t ra = init_uint8_dense_matrix(2, 3, alloc);
//     uint8_matrix_expect_t rb = init_uint8_dense_matrix(3, 5, alloc);
//     uint8_matrix_expect_t rc = init_uint8_dense_matrix(4, 5, alloc);
//     assert_true(ra.has_value && rb.has_value && rc.has_value);
//
//     assert_true(uint8_matrix_is_multiply_compatible(ra.u.value, rb.u.value));
//     assert_false(uint8_matrix_is_multiply_compatible(ra.u.value, rc.u.value));
//
//     return_uint8_matrix(ra.u.value);
//     return_uint8_matrix(rb.u.value);
//     return_uint8_matrix(rc.u.value);
// }
//
// // ================================================================================
// // Group 15: Row / column swaps
// // ================================================================================
//
// static void test_swap_uint8_rows_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(3, 2, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     set_uint8_matrix(m, 0, 0, 1u); set_uint8_matrix(m, 0, 1, 2u);
//     set_uint8_matrix(m, 1, 0, 3u); set_uint8_matrix(m, 1, 1, 4u);
//     set_uint8_matrix(m, 2, 0, 5u); set_uint8_matrix(m, 2, 1, 6u);
//
//     assert_int_equal(swap_uint8_matrix_rows(m, 0, 2), NO_ERROR);
//
//     uint8_t val = 0u;
//     get_uint8_matrix(m, 0, 0, &val); assert_int_equal((int)val, 5);
//     get_uint8_matrix(m, 0, 1, &val); assert_int_equal((int)val, 6);
//     get_uint8_matrix(m, 2, 0, &val); assert_int_equal((int)val, 1);
//     get_uint8_matrix(m, 2, 1, &val); assert_int_equal((int)val, 2);
//
//     return_uint8_matrix(m);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_swap_uint8_cols_dense(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(2, 3, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     set_uint8_matrix(m, 0, 0, 1u); set_uint8_matrix(m, 0, 2, 3u);
//     set_uint8_matrix(m, 1, 0, 4u); set_uint8_matrix(m, 1, 2, 6u);
//
//     assert_int_equal(swap_uint8_matrix_cols(m, 0, 2), NO_ERROR);
//
//     uint8_t val = 0u;
//     get_uint8_matrix(m, 0, 0, &val); assert_int_equal((int)val, 3);
//     get_uint8_matrix(m, 0, 2, &val); assert_int_equal((int)val, 1);
//     get_uint8_matrix(m, 1, 0, &val); assert_int_equal((int)val, 6);
//     get_uint8_matrix(m, 1, 2, &val); assert_int_equal((int)val, 4);
//
//     return_uint8_matrix(m);
// }
//
// // ================================================================================
// // Group 16: Special constructors (identity, row/col vector)
// // ================================================================================
//
// static void test_uint8_identity_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_identity_matrix(3, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     assert_int_equal((int)uint8_matrix_rows(m), 3);
//     assert_int_equal((int)uint8_matrix_cols(m), 3);
//
//     uint8_t val = 0u;
//     for (size_t i = 0; i < 3; ++i) {
//         for (size_t j = 0; j < 3; ++j) {
//             get_uint8_matrix(m, i, j, &val);
//             assert_int_equal((int)val, (i == j) ? 1 : 0);
//         }
//     }
//
//     return_uint8_matrix(m);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_row_vector_shape(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_row_vector(5, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     assert_int_equal((int)uint8_matrix_rows(m), 1);
//     assert_int_equal((int)uint8_matrix_cols(m), 5);
//     assert_true(uint8_matrix_is_row_vector(m));
//     assert_false(uint8_matrix_is_col_vector(m));
//     assert_true(uint8_matrix_is_vector(m));
//     assert_int_equal((int)uint8_matrix_vector_length(m), 5);
//
//     return_uint8_matrix(m);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_col_vector_shape(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_col_vector(4, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     assert_int_equal((int)uint8_matrix_rows(m), 4);
//     assert_int_equal((int)uint8_matrix_cols(m), 1);
//     assert_false(uint8_matrix_is_row_vector(m));
//     assert_true(uint8_matrix_is_col_vector(m));
//     assert_true(uint8_matrix_is_vector(m));
//     assert_int_equal((int)uint8_matrix_vector_length(m), 4);
//
//     return_uint8_matrix(m);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_non_vector_returns_zero_length(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(3, 4, alloc);
//     assert_true(r.has_value);
//
//     assert_false(uint8_matrix_is_vector(r.u.value));
//     assert_int_equal((int)uint8_matrix_vector_length(r.u.value), 0);
//
//     return_uint8_matrix(r.u.value);
// }
//
// // ================================================================================
// // Group 17: Large matrix smoke tests
// // ================================================================================
//
// static void test_fill_uint8_large_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(100, 100, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     assert_int_equal(fill_uint8_matrix(m, 17u), NO_ERROR);
//
//     uint8_t val = 0u;
//     get_uint8_matrix(m, 0, 0, &val);   assert_int_equal((int)val, 17);
//     get_uint8_matrix(m, 49, 50, &val); assert_int_equal((int)val, 17);
//     get_uint8_matrix(m, 99, 99, &val); assert_int_equal((int)val, 17);
//
//     return_uint8_matrix(m);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_transpose_uint8_large_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(17, 19, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     for (size_t i = 0; i < 17; ++i) {
//         for (size_t j = 0; j < 19; ++j) {
//             set_uint8_matrix(m, i, j, (uint8_t)((i * 19u + j) % 251u));
//         }
//     }
//
//     uint8_matrix_expect_t tr = transpose_uint8_matrix(m, alloc);
//     assert_true(tr.has_value);
//     uint8_matrix_t* t = tr.u.value;
//
//     assert_int_equal((int)uint8_matrix_rows(t), 19);
//     assert_int_equal((int)uint8_matrix_cols(t), 17);
//
//     uint8_t vm = 0u, vt = 0u;
//     for (size_t i = 0; i < 17; ++i) {
//         for (size_t j = 0; j < 19; ++j) {
//             get_uint8_matrix(m, i, j, &vm);
//             get_uint8_matrix(t, j, i, &vt);
//             assert_int_equal((int)vm, (int)vt);
//         }
//     }
//
//     return_uint8_matrix(m);
//     return_uint8_matrix(t);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_uint8_equal_large_matrix(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(50, 50, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     fill_uint8_matrix(m, 7u);
//
//     uint8_matrix_expect_t cr = copy_uint8_matrix(m, alloc);
//     assert_true(cr.has_value);
//
//     assert_true(uint8_matrix_equal(m, cr.u.value));
//
//     set_uint8_matrix(cr.u.value, 49, 49, 8u);
//     assert_false(uint8_matrix_equal(m, cr.u.value));
//
//     return_uint8_matrix(m);
//     return_uint8_matrix(cr.u.value);
// }
//
// // --------------------------------------------------------------------------------
//
// static void test_convert_uint8_large_dense_to_csr(void** state) {
//     (void)state;
//     allocator_vtable_t alloc = heap_allocator();
//     uint8_matrix_expect_t r = init_uint8_dense_matrix(20, 20, alloc);
//     assert_true(r.has_value);
//     uint8_matrix_t* m = r.u.value;
//
//     for (size_t i = 0; i < 20; ++i) {
//         set_uint8_matrix(m, i, i, (uint8_t)(i + 1u));
//     }
//
//     uint8_matrix_expect_t cr = convert_uint8_matrix(m, CSR_MATRIX, alloc);
//     assert_true(cr.has_value);
//     uint8_matrix_t* csr = cr.u.value;
//
//     assert_int_equal((int)uint8_matrix_nnz(csr), 20);
//
//     uint8_t val = 0u;
//     for (size_t i = 0; i < 20; ++i) {
//         get_uint8_matrix(csr, i, i, &val);
//         assert_int_equal((int)val, (int)(i + 1u));
//     }
//
//     get_uint8_matrix(csr, 0, 1, &val);  assert_int_equal((int)val, 0);
//     get_uint8_matrix(csr, 19, 0, &val); assert_int_equal((int)val, 0);
//
//     return_uint8_matrix(m);
//     return_uint8_matrix(csr);
// }
//
// // ================================================================================
// // Test suite export
// // ================================================================================
//
// const struct CMUnitTest test_uint8_matrix[] = {
//     /* Group 1: init dense */
//     cmocka_unit_test(test_uint8_dense_init_null_allocator_fails),
//     cmocka_unit_test(test_uint8_dense_init_zero_rows_fails),
//     cmocka_unit_test(test_uint8_dense_init_zero_cols_fails),
//     cmocka_unit_test(test_uint8_dense_init_returns_valid_matrix),
//     cmocka_unit_test(test_uint8_dense_init_elements_are_zero),
//
//     /* Group 2: init COO */
//     cmocka_unit_test(test_uint8_coo_init_returns_valid_matrix),
//     cmocka_unit_test(test_uint8_coo_init_zero_capacity_fails),
//
//     /* Group 3: return */
//     cmocka_unit_test(test_return_uint8_null_is_safe),
//
//     /* Group 4: get/set dense */
//     cmocka_unit_test(test_get_uint8_null_matrix_returns_null_pointer),
//     cmocka_unit_test(test_get_uint8_null_out_returns_null_pointer),
//     cmocka_unit_test(test_set_uint8_null_matrix_returns_null_pointer),
//     cmocka_unit_test(test_set_and_get_uint8_round_trip),
//     cmocka_unit_test(test_set_uint8_out_of_bounds_returns_error),
//
//     /* Group 5: COO assembly */
//     cmocka_unit_test(test_uint8_coo_push_back_and_retrieve),
//     cmocka_unit_test(test_uint8_coo_push_back_overwrites_existing),
//     cmocka_unit_test(test_uint8_coo_sort_orders_entries),
//
//     /* Group 6: clear */
//     cmocka_unit_test(test_clear_uint8_dense_zeros_all_elements),
//     cmocka_unit_test(test_clear_uint8_coo_resets_nnz),
//
//     /* Group 7: copy */
//     cmocka_unit_test(test_copy_uint8_null_returns_error),
//     cmocka_unit_test(test_copy_uint8_dense_produces_independent_matrix),
//
//     /* Group 8: fill */
//     cmocka_unit_test(test_fill_uint8_null_returns_error),
//     cmocka_unit_test(test_fill_uint8_dense_sets_all_elements),
//     cmocka_unit_test(test_fill_uint8_zero_clears_matrix),
//
//     /* Group 9: is_zero */
//     cmocka_unit_test(test_is_uint8_zero_null_returns_true),
//     cmocka_unit_test(test_is_uint8_zero_freshly_initialized_dense),
//     cmocka_unit_test(test_is_uint8_zero_returns_false_after_set),
//
//     /* Group 10: equal */
//     cmocka_unit_test(test_uint8_equal_null_returns_false),
//     cmocka_unit_test(test_uint8_equal_identical_matrices),
//     cmocka_unit_test(test_uint8_equal_different_values_returns_false),
//     cmocka_unit_test(test_uint8_equal_different_shape_returns_false),
//
//     /* Group 11: transpose */
//     cmocka_unit_test(test_transpose_uint8_null_returns_error),
//     cmocka_unit_test(test_transpose_uint8_dense_swaps_dimensions),
//     cmocka_unit_test(test_transpose_uint8_square_is_correct),
//
//     /* Group 12: convert dense→CSR */
//     cmocka_unit_test(test_convert_uint8_dense_to_csr_preserves_values),
//     cmocka_unit_test(test_convert_uint8_all_zero_dense_to_csr),
//
//     /* Group 13: introspection */
//     cmocka_unit_test(test_uint8_introspection_null_returns_defaults),
//     cmocka_unit_test(test_uint8_dense_nnz_equals_rows_times_cols),
//     cmocka_unit_test(test_uint8_format_name_returns_string),
//
//     /* Group 14: shape/compat */
//     cmocka_unit_test(test_uint8_same_shape_true_for_matching_dimensions),
//     cmocka_unit_test(test_uint8_is_square),
//     cmocka_unit_test(test_uint8_is_sparse),
//     cmocka_unit_test(test_uint8_add_compatible),
//     cmocka_unit_test(test_uint8_multiply_compatible),
//
//     /* Group 15: swaps */
//     cmocka_unit_test(test_swap_uint8_rows_dense),
//     cmocka_unit_test(test_swap_uint8_cols_dense),
//
//     /* Group 16: special constructors */
//     cmocka_unit_test(test_uint8_identity_matrix),
//     cmocka_unit_test(test_uint8_row_vector_shape),
//     cmocka_unit_test(test_uint8_col_vector_shape),
//     cmocka_unit_test(test_uint8_non_vector_returns_zero_length),
//
//     /* Group 17: large matrix smoke tests */
//     cmocka_unit_test(test_fill_uint8_large_matrix),
//     cmocka_unit_test(test_transpose_uint8_large_matrix),
//     cmocka_unit_test(test_uint8_equal_large_matrix),
//     cmocka_unit_test(test_convert_uint8_large_dense_to_csr),
// };
//
// const size_t test_uint8_matrix_count =
//     sizeof(test_uint8_matrix) / sizeof(test_uint8_matrix[0]);
// ================================================================================
// ================================================================================
// eof
