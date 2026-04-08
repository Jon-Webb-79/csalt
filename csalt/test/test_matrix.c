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

// ================================================================================
// Helpers
// ================================================================================

static int setup_dtype_registry(void** state) {
    (void)state;

    /* Safe to call repeatedly if your registry init is idempotent */
    assert_true(init_dtype_registry());
    return 0;
}

// ================================================================================
// Dense matrix init tests
// ================================================================================

static void test_init_dense_matrix_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_int_equal(mat->rows, 3u);
    assert_int_equal(mat->cols, 4u);
    assert_int_equal(mat->dtype, INT32_TYPE);
    assert_int_equal(mat->format, DENSE_MATRIX);

    assert_true(mat->data_size == sizeof(int32_t));
    assert_non_null(mat->rep.dense.data);

    /* Dense nnz is currently defined as rows * cols in your implementation */
    assert_int_equal(matrix_nnz(mat), 12u);
    assert_int_equal(matrix_rows(mat), 3u);
    assert_int_equal(matrix_cols(mat), 4u);
    assert_int_equal(matrix_dtype(mat), INT32_TYPE);
    assert_int_equal(matrix_format(mat), DENSE_MATRIX);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_init_dense_matrix_zero_initialized(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_dense_matrix(2u, 3u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            int32_t value = -1;
            assert_int_equal(get_matrix(mat, i, j, &value), NO_ERROR);
            assert_int_equal(value, 0);
        }
    }

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_init_dense_matrix_invalid_rows(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_dense_matrix(0u, 4u, INT32_TYPE, alloc);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_init_dense_matrix_invalid_cols(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_dense_matrix(3u, 0u, INT32_TYPE, alloc);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_init_dense_matrix_invalid_dtype(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_dense_matrix(3u, 4u, UNKNOWN_TYPE, alloc);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_init_dense_matrix_null_allocator(void** state) {
    (void)state;

    allocator_vtable_t alloc = {0};

    matrix_expect_t r = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// ================================================================================
// COO matrix init tests
// ================================================================================

static void test_init_coo_matrix_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_coo_matrix(5u, 6u, 8u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_int_equal(mat->rows, 5u);
    assert_int_equal(mat->cols, 6u);
    assert_int_equal(mat->dtype, INT32_TYPE);
    assert_int_equal(mat->format, COO_MATRIX);
    assert_true(mat->data_size == sizeof(int32_t));

    assert_int_equal(mat->rep.coo.nnz, 0u);
    assert_int_equal(mat->rep.coo.cap, 8u);
    assert_true(mat->rep.coo.growth);
    assert_true(mat->rep.coo.sorted);

    assert_non_null(mat->rep.coo.row_idx);
    assert_non_null(mat->rep.coo.col_idx);
    assert_non_null(mat->rep.coo.values);

    assert_int_equal(matrix_rows(mat), 5u);
    assert_int_equal(matrix_cols(mat), 6u);
    assert_int_equal(matrix_dtype(mat), INT32_TYPE);
    assert_int_equal(matrix_format(mat), COO_MATRIX);
    assert_int_equal(matrix_nnz(mat), 0u);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_init_coo_matrix_growth_disabled(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_coo_matrix(4u, 4u, 3u, DOUBLE_TYPE, false, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_false(mat->rep.coo.growth);
    assert_true(mat->rep.coo.sorted);
    assert_int_equal(mat->rep.coo.nnz, 0u);
    assert_int_equal(mat->rep.coo.cap, 3u);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_init_coo_matrix_invalid_rows(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_coo_matrix(0u, 4u, 8u, INT32_TYPE, true, alloc);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_init_coo_matrix_invalid_cols(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_coo_matrix(4u, 0u, 8u, INT32_TYPE, true, alloc);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_init_coo_matrix_invalid_capacity(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_coo_matrix(4u, 4u, 0u, INT32_TYPE, true, alloc);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_init_coo_matrix_invalid_dtype(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_coo_matrix(4u, 4u, 8u, UNKNOWN_TYPE, true, alloc);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

static void test_init_coo_matrix_null_allocator(void** state) {
    (void)state;

    allocator_vtable_t alloc = {0};

    matrix_expect_t r = init_coo_matrix(4u, 4u, 8u, INT32_TYPE, true, alloc);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// ================================================================================
// return_matrix smoke tests
// ================================================================================

static void test_return_dense_matrix_smoke(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_dense_matrix(10u, 10u, FLOAT_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    return_matrix(r.u.value);

    /* If we reach here without crashing, the smoke test passes */
    assert_true(true);
}

// --------------------------------------------------------------------------------

static void test_return_coo_matrix_smoke(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_coo_matrix(10u, 10u, 16u, FLOAT_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    return_matrix(r.u.value);

    /* If we reach here without crashing, the smoke test passes */
    assert_true(true);
}

// --------------------------------------------------------------------------------

static void test_return_null_matrix_is_safe(void** state) {
    (void)state;

    return_matrix(NULL);
    assert_true(true);
}
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Dense matrix get / set tests
// ================================================================================

static void test_dense_set_and_get_single_value(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    int32_t in = 42;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 1u, 2u, &in), NO_ERROR);
    assert_int_equal(get_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 42);

    assert_int_equal(matrix_rows(mat), 3u);
    assert_int_equal(matrix_cols(mat), 3u);
    assert_int_equal(matrix_dtype(mat), INT32_TYPE);
    assert_int_equal(matrix_format(mat), DENSE_MATRIX);
    assert_int_equal(matrix_nnz(mat), 9u);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_set_and_get_multiple_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 3u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    int32_t v00 = 11;
    int32_t v01 = 12;
    int32_t v10 = 21;
    int32_t v12 = 23;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 0u, 0u, &v00), NO_ERROR);
    assert_int_equal(set_matrix(mat, 0u, 1u, &v01), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 0u, &v10), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 2u, &v12), NO_ERROR);

    assert_int_equal(get_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 11);

    assert_int_equal(get_matrix(mat, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 12);

    assert_int_equal(get_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 21);

    assert_int_equal(get_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 23);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_get_unset_value_returns_zero(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(4u, 4u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    int32_t out = -1;
    assert_int_equal(get_matrix(mat, 3u, 3u, &out), NO_ERROR);
    assert_int_equal(out, 0);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_set_invalid_row(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;
    int32_t value = 7;

    assert_int_equal(set_matrix(mat, 2u, 0u, &value), INVALID_ARG);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_set_invalid_col(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;
    int32_t value = 7;

    assert_int_equal(set_matrix(mat, 0u, 2u, &value), INVALID_ARG);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_get_invalid_row(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;
    int32_t out = 0;

    assert_int_equal(get_matrix(mat, 2u, 1u, &out), INVALID_ARG);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_get_invalid_col(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;
    int32_t out = 0;

    assert_int_equal(get_matrix(mat, 1u, 2u, &out), INVALID_ARG);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_set_null_value(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_int_equal(set_matrix(mat, 0u, 0u, NULL), NULL_POINTER);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_dense_get_null_out(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_int_equal(get_matrix(mat, 0u, 0u, NULL), NULL_POINTER);

    return_matrix(mat);
}

// ================================================================================
// COO matrix get / set tests
// ================================================================================

static void test_coo_set_and_get_single_value(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(4u, 4u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    int32_t in = 77;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 2u, 3u, &in), NO_ERROR);
    assert_int_equal(get_matrix(mat, 2u, 3u, &out), NO_ERROR);
    assert_int_equal(out, 77);

    assert_int_equal(matrix_rows(mat), 4u);
    assert_int_equal(matrix_cols(mat), 4u);
    assert_int_equal(matrix_dtype(mat), INT32_TYPE);
    assert_int_equal(matrix_format(mat), COO_MATRIX);
    assert_int_equal(matrix_nnz(mat), 1u);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_get_missing_value_returns_zero(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(4u, 4u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    int32_t in = 33;
    int32_t out = -1;

    assert_int_equal(set_matrix(mat, 1u, 1u, &in), NO_ERROR);
    assert_int_equal(get_matrix(mat, 0u, 3u, &out), NO_ERROR);
    assert_int_equal(out, 0);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_set_and_get_multiple_unsorted_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(5u, 5u, 8u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    int32_t v1 = 101;
    int32_t v2 = 202;
    int32_t v3 = 303;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 4u, 4u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(mat, 0u, 2u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(mat, 3u, 1u, &v3), NO_ERROR);

    assert_false(mat->rep.coo.sorted);
    assert_int_equal(mat->rep.coo.nnz, 3u);

    assert_int_equal(get_matrix(mat, 4u, 4u, &out), NO_ERROR);
    assert_int_equal(out, 101);

    assert_int_equal(get_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 202);

    assert_int_equal(get_matrix(mat, 3u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 303);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_sort_then_get_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(5u, 5u, 8u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    int32_t v1 = 15;
    int32_t v2 = 25;
    int32_t v3 = 35;
    int32_t out = 0;

    assert_int_equal(push_back_coo_matrix(mat, 4u, 2u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 1u, 4u, &v2), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 1u, 1u, &v3), NO_ERROR);

    assert_false(mat->rep.coo.sorted);
    assert_int_equal(sort_coo_matrix(mat), NO_ERROR);
    assert_true(mat->rep.coo.sorted);

    assert_int_equal(get_matrix(mat, 4u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 15);

    assert_int_equal(get_matrix(mat, 1u, 4u, &out), NO_ERROR);
    assert_int_equal(out, 25);

    assert_int_equal(get_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 35);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_growth_updates_nnz_and_capacity(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(4u, 4u, 2u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    int32_t v1 = 1;
    int32_t v2 = 2;
    int32_t v3 = 3;

    assert_int_equal(mat->rep.coo.cap, 2u);

    assert_int_equal(push_back_coo_matrix(mat, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 1u, 1u, &v2), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 2u, 2u, &v3), NO_ERROR);

    assert_int_equal(mat->rep.coo.nnz, 3u);
    assert_true(mat->rep.coo.cap >= 3u);
    assert_int_equal(matrix_nnz(mat), 3u);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_push_back_capacity_overflow_when_growth_disabled(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(4u, 4u, 2u, INT32_TYPE, false, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    int32_t v1 = 1;
    int32_t v2 = 2;
    int32_t v3 = 3;

    assert_int_equal(push_back_coo_matrix(mat, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 1u, 1u, &v2), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 2u, 2u, &v3), CAPACITY_OVERFLOW);

    assert_int_equal(mat->rep.coo.nnz, 2u);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_set_invalid_row(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;
    int32_t value = 9;

    assert_int_equal(set_matrix(mat, 3u, 1u, &value), INVALID_ARG);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_set_invalid_col(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;
    int32_t value = 9;

    assert_int_equal(set_matrix(mat, 1u, 3u, &value), INVALID_ARG);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_get_invalid_row(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;
    int32_t out = 0;

    assert_int_equal(get_matrix(mat, 3u, 0u, &out), INVALID_ARG);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_get_invalid_col(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;
    int32_t out = 0;

    assert_int_equal(get_matrix(mat, 0u, 3u, &out), INVALID_ARG);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_set_null_value(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_int_equal(set_matrix(mat, 0u, 0u, NULL), NULL_POINTER);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_coo_get_null_out(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_int_equal(get_matrix(mat, 0u, 0u, NULL), NULL_POINTER);

    return_matrix(mat);
}
// ================================================================================
// COO reserve / sort tests
// ================================================================================

static void test_reserve_coo_matrix_grows_capacity(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(4u, 4u, 2u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_int_equal(mat->rep.coo.cap, 2u);
    assert_int_equal(reserve_coo_matrix(mat, 6u), NO_ERROR);
    assert_int_equal(mat->rep.coo.cap, 6u);
    assert_int_equal(mat->rep.coo.nnz, 0u);
    assert_true(mat->rep.coo.sorted);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_reserve_coo_matrix_noop_when_capacity_not_increased(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(4u, 4u, 5u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_int_equal(mat->rep.coo.cap, 5u);
    assert_int_equal(reserve_coo_matrix(mat, 5u), NO_ERROR);
    assert_int_equal(mat->rep.coo.cap, 5u);

    assert_int_equal(reserve_coo_matrix(mat, 3u), NO_ERROR);
    assert_int_equal(mat->rep.coo.cap, 5u);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_reserve_coo_matrix_preserves_existing_entries(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(5u, 5u, 2u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    int32_t v1 = 10;
    int32_t v2 = 20;
    int32_t out = 0;

    assert_int_equal(push_back_coo_matrix(mat, 1u, 1u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 3u, 4u, &v2), NO_ERROR);

    assert_int_equal(mat->rep.coo.nnz, 2u);
    assert_int_equal(mat->rep.coo.cap, 2u);

    assert_int_equal(reserve_coo_matrix(mat, 8u), NO_ERROR);
    assert_int_equal(mat->rep.coo.cap, 8u);
    assert_int_equal(mat->rep.coo.nnz, 2u);

    assert_int_equal(get_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10);

    assert_int_equal(get_matrix(mat, 3u, 4u, &out), NO_ERROR);
    assert_int_equal(out, 20);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_reserve_coo_matrix_null_matrix(void** state) {
    (void)state;

    assert_int_equal(reserve_coo_matrix(NULL, 10u), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_reserve_coo_matrix_invalid_for_dense_matrix(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_int_equal(reserve_coo_matrix(mat, 10u), ILLEGAL_STATE);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_sort_coo_matrix_on_empty_matrix(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(4u, 4u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_true(mat->rep.coo.sorted);
    assert_int_equal(sort_coo_matrix(mat), NO_ERROR);
    assert_true(mat->rep.coo.sorted);
    assert_int_equal(mat->rep.coo.nnz, 0u);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_sort_coo_matrix_on_single_entry(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(4u, 4u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;
    int32_t value = 99;
    int32_t out = 0;

    assert_int_equal(push_back_coo_matrix(mat, 2u, 3u, &value), NO_ERROR);
    assert_false(mat->rep.coo.sorted);

    assert_int_equal(sort_coo_matrix(mat), NO_ERROR);
    assert_true(mat->rep.coo.sorted);
    assert_int_equal(mat->rep.coo.nnz, 1u);

    assert_int_equal(get_matrix(mat, 2u, 3u, &out), NO_ERROR);
    assert_int_equal(out, 99);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_sort_coo_matrix_orders_by_row_then_column(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(6u, 6u, 8u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    int32_t v1 = 11;
    int32_t v2 = 22;
    int32_t v3 = 33;
    int32_t v4 = 44;

    assert_int_equal(push_back_coo_matrix(mat, 4u, 5u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 1u, 3u, &v2), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 1u, 1u, &v3), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 3u, 0u, &v4), NO_ERROR);

    assert_false(mat->rep.coo.sorted);
    assert_int_equal(mat->rep.coo.nnz, 4u);

    assert_int_equal(sort_coo_matrix(mat), NO_ERROR);
    assert_true(mat->rep.coo.sorted);

    assert_int_equal(mat->rep.coo.row_idx[0], 1u);
    assert_int_equal(mat->rep.coo.col_idx[0], 1u);

    assert_int_equal(mat->rep.coo.row_idx[1], 1u);
    assert_int_equal(mat->rep.coo.col_idx[1], 3u);

    assert_int_equal(mat->rep.coo.row_idx[2], 3u);
    assert_int_equal(mat->rep.coo.col_idx[2], 0u);

    assert_int_equal(mat->rep.coo.row_idx[3], 4u);
    assert_int_equal(mat->rep.coo.col_idx[3], 5u);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_sort_coo_matrix_preserves_values_with_indices(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(6u, 6u, 8u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;
    int32_t out = 0;

    int32_t v1 = 100;
    int32_t v2 = 200;
    int32_t v3 = 300;

    assert_int_equal(push_back_coo_matrix(mat, 5u, 5u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 0u, 1u, &v2), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 2u, 2u, &v3), NO_ERROR);

    assert_int_equal(sort_coo_matrix(mat), NO_ERROR);
    assert_true(mat->rep.coo.sorted);

    assert_int_equal(get_matrix(mat, 5u, 5u, &out), NO_ERROR);
    assert_int_equal(out, 100);

    assert_int_equal(get_matrix(mat, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 200);

    assert_int_equal(get_matrix(mat, 2u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 300);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_sort_coo_matrix_null_matrix(void** state) {
    (void)state;

    assert_int_equal(sort_coo_matrix(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

static void test_sort_coo_matrix_invalid_for_dense_matrix(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_int_equal(sort_coo_matrix(mat), ILLEGAL_STATE);

    return_matrix(mat);
}

// ================================================================================
// Storage bytes / format name / equality tests
// ================================================================================

static void test_matrix_storage_bytes_dense(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_int_equal(matrix_storage_bytes(mat), 3u * 4u * sizeof(int32_t));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_storage_bytes_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(5u, 6u, 8u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    size_t expected =
        (8u * sizeof(size_t)) +
        (8u * sizeof(size_t)) +
        (8u * sizeof(int32_t));

    assert_int_equal(matrix_storage_bytes(mat), expected);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_storage_bytes_null_matrix(void** state) {
    (void)state;

    assert_int_equal(matrix_storage_bytes(NULL), 0u);
}

// --------------------------------------------------------------------------------

static void test_matrix_format_name_dense(void** state) {
    (void)state;

    assert_string_equal(matrix_format_name(DENSE_MATRIX), "DENSE_MATRIX");
}

// --------------------------------------------------------------------------------

static void test_matrix_format_name_coo(void** state) {
    (void)state;

    assert_string_equal(matrix_format_name(COO_MATRIX), "COO_MATRIX");
}

// --------------------------------------------------------------------------------

static void test_matrix_format_name_csr(void** state) {
    (void)state;

    assert_string_equal(matrix_format_name(CSR_MATRIX), "CSR_MATRIX");
}

// --------------------------------------------------------------------------------

static void test_matrix_format_name_csc(void** state) {
    (void)state;

    assert_string_equal(matrix_format_name(CSC_MATRIX), "CSC_MATRIX");
}

// --------------------------------------------------------------------------------

static void test_matrix_format_name_unknown(void** state) {
    (void)state;

    assert_string_equal(matrix_format_name((matrix_format_t)99), "UNKNOWN_MATRIX_FORMAT");
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_dense_same_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* a = r1.u.value;
    matrix_t* b = r2.u.value;

    int32_t v00 = 1;
    int32_t v01 = 2;
    int32_t v10 = 3;
    int32_t v11 = 4;

    assert_int_equal(set_matrix(a, 0u, 0u, &v00), NO_ERROR);
    assert_int_equal(set_matrix(a, 0u, 1u, &v01), NO_ERROR);
    assert_int_equal(set_matrix(a, 1u, 0u, &v10), NO_ERROR);
    assert_int_equal(set_matrix(a, 1u, 1u, &v11), NO_ERROR);

    assert_int_equal(set_matrix(b, 0u, 0u, &v00), NO_ERROR);
    assert_int_equal(set_matrix(b, 0u, 1u, &v01), NO_ERROR);
    assert_int_equal(set_matrix(b, 1u, 0u, &v10), NO_ERROR);
    assert_int_equal(set_matrix(b, 1u, 1u, &v11), NO_ERROR);

    assert_true(matrix_equal(a, b));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_dense_different_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* a = r1.u.value;
    matrix_t* b = r2.u.value;

    int32_t va = 5;
    int32_t vb = 6;

    assert_int_equal(set_matrix(a, 1u, 1u, &va), NO_ERROR);
    assert_int_equal(set_matrix(b, 1u, 1u, &vb), NO_ERROR);

    assert_false(matrix_equal(a, b));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_dense_and_coo_same_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* dense = r1.u.value;
    matrix_t* coo   = r2.u.value;

    int32_t v1 = 10;
    int32_t v2 = 20;
    int32_t v3 = 30;

    assert_int_equal(set_matrix(dense, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 2u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 1u, &v3), NO_ERROR);

    assert_int_equal(push_back_coo_matrix(coo, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(coo, 1u, 2u, &v2), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(coo, 2u, 1u, &v3), NO_ERROR);

    assert_true(matrix_equal(dense, coo));
    assert_true(matrix_equal(coo, dense));

    return_matrix(dense);
    return_matrix(coo);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_dense_and_coo_different_values(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* dense = r1.u.value;
    matrix_t* coo   = r2.u.value;

    int32_t v1 = 10;
    int32_t v2 = 20;
    int32_t v3 = 30;
    int32_t wrong = 99;

    assert_int_equal(set_matrix(dense, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 2u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 1u, &v3), NO_ERROR);

    assert_int_equal(push_back_coo_matrix(coo, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(coo, 1u, 2u, &wrong), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(coo, 2u, 1u, &v3), NO_ERROR);

    assert_false(matrix_equal(dense, coo));
    assert_false(matrix_equal(coo, dense));

    return_matrix(dense);
    return_matrix(coo);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_same_shape_but_different_dtype(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(2u, 2u, FLOAT_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* a = r1.u.value;
    matrix_t* b = r2.u.value;

    assert_false(matrix_equal(a, b));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_different_shapes(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(2u, 3u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(3u, 2u, INT32_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* a = r1.u.value;
    matrix_t* b = r2.u.value;

    assert_false(matrix_equal(a, b));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_null_inputs(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_false(matrix_equal(NULL, mat));
    assert_false(matrix_equal(mat, NULL));
    assert_false(matrix_equal(NULL, NULL));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_equal_zero_initialized_dense_matrices(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* a = r1.u.value;
    matrix_t* b = r2.u.value;

    assert_true(matrix_equal(a, b));

    return_matrix(a);
    return_matrix(b);
}
// ================================================================================
// Shape / structure helper tests
// ================================================================================

static void test_matrix_has_same_shape_dense_dense_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(3u, 4u, DOUBLE_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* a = r1.u.value;
    matrix_t* b = r2.u.value;

    assert_true(matrix_has_same_shape(a, b));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_has_same_shape_dense_dense_false(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(4u, 3u, INT32_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* a = r1.u.value;
    matrix_t* b = r2.u.value;

    assert_false(matrix_has_same_shape(a, b));

    return_matrix(a);
    return_matrix(b);
}

// --------------------------------------------------------------------------------

static void test_matrix_has_same_shape_dense_coo_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(5u, 6u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_coo_matrix(5u, 6u, 8u, INT32_TYPE, true, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* dense = r1.u.value;
    matrix_t* coo   = r2.u.value;

    assert_true(matrix_has_same_shape(dense, coo));
    assert_true(matrix_has_same_shape(coo, dense));

    return_matrix(dense);
    return_matrix(coo);
}

// --------------------------------------------------------------------------------

static void test_matrix_has_same_shape_dense_coo_false(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(5u, 6u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_coo_matrix(6u, 5u, 8u, INT32_TYPE, true, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* dense = r1.u.value;
    matrix_t* coo   = r2.u.value;

    assert_false(matrix_has_same_shape(dense, coo));
    assert_false(matrix_has_same_shape(coo, dense));

    return_matrix(dense);
    return_matrix(coo);
}

// --------------------------------------------------------------------------------

static void test_matrix_has_same_shape_null_inputs(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_false(matrix_has_same_shape(NULL, mat));
    assert_false(matrix_has_same_shape(mat, NULL));
    assert_false(matrix_has_same_shape(NULL, NULL));

    return_matrix(mat);
}

// ================================================================================
// matrix_is_square tests
// ================================================================================

static void test_matrix_is_square_dense_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(4u, 4u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_true(matrix_is_square(mat));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_is_square_dense_false(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_false(matrix_is_square(mat));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_is_square_coo_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(5u, 5u, 8u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_true(matrix_is_square(mat));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_is_square_coo_false(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(5u, 6u, 8u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_false(matrix_is_square(mat));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_is_square_null_matrix(void** state) {
    (void)state;

    assert_false(matrix_is_square(NULL));
}

// ================================================================================
// matrix_is_sparse tests
// ================================================================================

static void test_matrix_is_sparse_dense_false(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_false(matrix_is_sparse(mat));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_is_sparse_coo_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    assert_true(matrix_is_sparse(mat));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_is_sparse_null_matrix(void** state) {
    (void)state;

    assert_false(matrix_is_sparse(NULL));
}
// ================================================================================
// Copy tests for CSR / CSC
// ================================================================================

static void test_copy_matrix_csr_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 10;
    int32_t v2 = 20;
    int32_t v3 = 30;

    assert_int_equal(set_matrix(dense, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 2u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 1u, &v3), NO_ERROR);

    matrix_expect_t csr_r = convert_matrix(dense, CSR_MATRIX, alloc);
    assert_true(csr_r.has_value);
    matrix_t* csr = csr_r.u.value;

    matrix_expect_t copy_r = copy_matrix(csr, alloc);
    assert_true(copy_r.has_value);
    matrix_t* copy = copy_r.u.value;

    assert_int_equal(copy->format, CSR_MATRIX);
    assert_int_equal(copy->rows, 3u);
    assert_int_equal(copy->cols, 3u);
    assert_int_equal(copy->rep.csr.nnz, csr->rep.csr.nnz);
    assert_true(matrix_equal(csr, copy));
    //
    return_matrix(dense);
    return_matrix(csr);
    return_matrix(copy);
}

// --------------------------------------------------------------------------------

static void test_copy_matrix_csc_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 5;
    int32_t v2 = 15;
    int32_t v3 = 25;

    assert_int_equal(set_matrix(dense, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 0u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 2u, &v3), NO_ERROR);

    matrix_expect_t csc_r = convert_matrix(dense, CSC_MATRIX, alloc);
    assert_true(csc_r.has_value);
    matrix_t* csc = csc_r.u.value;

    matrix_expect_t copy_r = copy_matrix(csc, alloc);
    assert_true(copy_r.has_value);
    matrix_t* copy = copy_r.u.value;

    assert_int_equal(copy->format, CSC_MATRIX);
    assert_int_equal(copy->rows, 3u);
    assert_int_equal(copy->cols, 3u);
    assert_int_equal(copy->rep.csc.nnz, csc->rep.csc.nnz);
    assert_true(matrix_equal(csc, copy));

    return_matrix(dense);
    return_matrix(csc);
    return_matrix(copy);
}

// ================================================================================
// Conversion tests
// ================================================================================

static void test_convert_dense_to_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 11;
    int32_t v2 = 22;
    int32_t v3 = 33;

    assert_int_equal(set_matrix(dense, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 3u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 1u, &v3), NO_ERROR);

    matrix_expect_t csr_r = convert_matrix(dense, CSR_MATRIX, alloc);
    assert_true(csr_r.has_value);
    matrix_t* csr = csr_r.u.value;

    assert_int_equal(csr->format, CSR_MATRIX);
    assert_int_equal(csr->rows, 3u);
    assert_int_equal(csr->cols, 4u);
    assert_int_equal(csr->rep.csr.nnz, 3u);

    assert_true(matrix_equal(dense, csr));

    return_matrix(dense);
    return_matrix(csr);
}

// --------------------------------------------------------------------------------

static void test_convert_dense_to_csc(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 7;
    int32_t v2 = 14;
    int32_t v3 = 21;

    assert_int_equal(set_matrix(dense, 0u, 2u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 0u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 3u, &v3), NO_ERROR);

    matrix_expect_t csc_r = convert_matrix(dense, CSC_MATRIX, alloc);
    assert_true(csc_r.has_value);
    matrix_t* csc = csc_r.u.value;

    assert_int_equal(csc->format, CSC_MATRIX);
    assert_int_equal(csc->rows, 3u);
    assert_int_equal(csc->cols, 4u);
    assert_int_equal(csc->rep.csc.nnz, 3u);

    assert_true(matrix_equal(dense, csc));

    return_matrix(dense);
    return_matrix(csc);
}

// --------------------------------------------------------------------------------

static void test_convert_coo_to_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t coo_r = init_coo_matrix(4u, 4u, 6u, INT32_TYPE, true, alloc);
    assert_true(coo_r.has_value);
    matrix_t* coo = coo_r.u.value;

    int32_t v1 = 1;
    int32_t v2 = 2;
    int32_t v3 = 3;

    assert_int_equal(push_back_coo_matrix(coo, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(coo, 2u, 3u, &v2), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(coo, 3u, 1u, &v3), NO_ERROR);

    matrix_expect_t csr_r = convert_matrix(coo, CSR_MATRIX, alloc);
    assert_true(csr_r.has_value);
    matrix_t* csr = csr_r.u.value;

    assert_int_equal(csr->format, CSR_MATRIX);
    assert_int_equal(csr->rep.csr.nnz, 3u);
    assert_true(matrix_equal(coo, csr));

    return_matrix(coo);
    return_matrix(csr);
}

// --------------------------------------------------------------------------------

static void test_convert_coo_to_csc(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t coo_r = init_coo_matrix(4u, 4u, 6u, INT32_TYPE, true, alloc);
    assert_true(coo_r.has_value);
    matrix_t* coo = coo_r.u.value;

    int32_t v1 = 4;
    int32_t v2 = 5;
    int32_t v3 = 6;

    assert_int_equal(push_back_coo_matrix(coo, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(coo, 1u, 3u, &v2), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(coo, 3u, 0u, &v3), NO_ERROR);

    matrix_expect_t csc_r = convert_matrix(coo, CSC_MATRIX, alloc);
    assert_true(csc_r.has_value);
    matrix_t* csc = csc_r.u.value;

    assert_int_equal(csc->format, CSC_MATRIX);
    assert_int_equal(csc->rep.csc.nnz, 3u);
    assert_true(matrix_equal(coo, csc));

    return_matrix(coo);
    return_matrix(csc);
}

// --------------------------------------------------------------------------------

static void test_convert_csr_to_dense(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 8;
    int32_t v2 = 16;
    int32_t v3 = 24;

    assert_int_equal(set_matrix(dense, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 2u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 1u, &v3), NO_ERROR);

    matrix_expect_t csr_r = convert_matrix(dense, CSR_MATRIX, alloc);
    assert_true(csr_r.has_value);
    matrix_t* csr = csr_r.u.value;

    matrix_expect_t back_r = convert_matrix(csr, DENSE_MATRIX, alloc);
    assert_true(back_r.has_value);
    matrix_t* back = back_r.u.value;

    assert_int_equal(back->format, DENSE_MATRIX);
    assert_true(matrix_equal(dense, back));

    return_matrix(dense);
    return_matrix(csr);
    return_matrix(back);
}

// --------------------------------------------------------------------------------

static void test_convert_csc_to_dense(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 12;
    int32_t v2 = 18;
    int32_t v3 = 27;

    assert_int_equal(set_matrix(dense, 0u, 2u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 0u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 2u, &v3), NO_ERROR);

    matrix_expect_t csc_r = convert_matrix(dense, CSC_MATRIX, alloc);
    assert_true(csc_r.has_value);
    matrix_t* csc = csc_r.u.value;

    matrix_expect_t back_r = convert_matrix(csc, DENSE_MATRIX, alloc);
    assert_true(back_r.has_value);
    matrix_t* back = back_r.u.value;

    assert_int_equal(back->format, DENSE_MATRIX);
    assert_true(matrix_equal(dense, back));

    return_matrix(dense);
    return_matrix(csc);
    return_matrix(back);
}

// --------------------------------------------------------------------------------

static void test_convert_csr_to_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(4u, 4u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 9;
    int32_t v2 = 19;
    int32_t v3 = 29;

    assert_int_equal(set_matrix(dense, 0u, 3u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 1u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 3u, 2u, &v3), NO_ERROR);

    matrix_expect_t csr_r = convert_matrix(dense, CSR_MATRIX, alloc);
    assert_true(csr_r.has_value);
    matrix_t* csr = csr_r.u.value;

    matrix_expect_t coo_r = convert_matrix(csr, COO_MATRIX, alloc);
    assert_true(coo_r.has_value);
    matrix_t* coo = coo_r.u.value;

    assert_int_equal(coo->format, COO_MATRIX);
    assert_true(matrix_equal(csr, coo));

    return_matrix(dense);
    return_matrix(csr);
    return_matrix(coo);
}

// --------------------------------------------------------------------------------

static void test_convert_csc_to_coo(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(4u, 4u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 13;
    int32_t v2 = 23;
    int32_t v3 = 33;

    assert_int_equal(set_matrix(dense, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 3u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 3u, 0u, &v3), NO_ERROR);

    matrix_expect_t csc_r = convert_matrix(dense, CSC_MATRIX, alloc);
    assert_true(csc_r.has_value);
    matrix_t* csc = csc_r.u.value;

    matrix_expect_t coo_r = convert_matrix(csc, COO_MATRIX, alloc);
    assert_true(coo_r.has_value);
    matrix_t* coo = coo_r.u.value;

    assert_int_equal(coo->format, COO_MATRIX);
    assert_true(matrix_equal(csc, coo));

    return_matrix(dense);
    return_matrix(csc);
    return_matrix(coo);
}

// --------------------------------------------------------------------------------

static void test_convert_csr_to_csc(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 2;
    int32_t v2 = 4;
    int32_t v3 = 6;

    assert_int_equal(set_matrix(dense, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 2u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 3u, &v3), NO_ERROR);

    matrix_expect_t csr_r = convert_matrix(dense, CSR_MATRIX, alloc);
    assert_true(csr_r.has_value);
    matrix_t* csr = csr_r.u.value;

    matrix_expect_t csc_r = convert_matrix(csr, CSC_MATRIX, alloc);
    assert_true(csc_r.has_value);
    matrix_t* csc = csc_r.u.value;

    assert_int_equal(csc->format, CSC_MATRIX);
    assert_true(matrix_equal(csr, csc));

    return_matrix(dense);
    return_matrix(csr);
    return_matrix(csc);
}

// --------------------------------------------------------------------------------

static void test_convert_csc_to_csr(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 3;
    int32_t v2 = 6;
    int32_t v3 = 9;

    assert_int_equal(set_matrix(dense, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 0u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 2u, &v3), NO_ERROR);

    matrix_expect_t csc_r = convert_matrix(dense, CSC_MATRIX, alloc);
    assert_true(csc_r.has_value);
    matrix_t* csc = csc_r.u.value;

    matrix_expect_t csr_r = convert_matrix(csc, CSR_MATRIX, alloc);
    assert_true(csr_r.has_value);
    matrix_t* csr = csr_r.u.value;

    assert_int_equal(csr->format, CSR_MATRIX);
    assert_true(matrix_equal(csc, csr));

    return_matrix(dense);
    return_matrix(csc);
    return_matrix(csr);
}
// ================================================================================
// Transpose tests
// ================================================================================

static void test_transpose_dense_rectangular_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t src_r = init_dense_matrix(2u, 3u, INT32_TYPE, alloc);
    assert_true(src_r.has_value);
    assert_non_null(src_r.u.value);

    matrix_t* src = src_r.u.value;

    int32_t v00 = 1;
    int32_t v01 = 2;
    int32_t v02 = 3;
    int32_t v10 = 4;
    int32_t v11 = 5;
    int32_t v12 = 6;
    int32_t out = 0;

    assert_int_equal(set_matrix(src, 0u, 0u, &v00), NO_ERROR);
    assert_int_equal(set_matrix(src, 0u, 1u, &v01), NO_ERROR);
    assert_int_equal(set_matrix(src, 0u, 2u, &v02), NO_ERROR);
    assert_int_equal(set_matrix(src, 1u, 0u, &v10), NO_ERROR);
    assert_int_equal(set_matrix(src, 1u, 1u, &v11), NO_ERROR);
    assert_int_equal(set_matrix(src, 1u, 2u, &v12), NO_ERROR);

    matrix_expect_t tr_r = transpose_matrix(src, alloc);
    assert_true(tr_r.has_value);
    assert_non_null(tr_r.u.value);

    matrix_t* tr = tr_r.u.value;

    assert_int_equal(tr->format, DENSE_MATRIX);
    assert_int_equal(tr->rows, 3u);
    assert_int_equal(tr->cols, 2u);
    assert_int_equal(tr->dtype, INT32_TYPE);

    assert_int_equal(get_matrix(tr, 0u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 1);
    assert_int_equal(get_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 2);
    assert_int_equal(get_matrix(tr, 2u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 3);

    assert_int_equal(get_matrix(tr, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 4);
    assert_int_equal(get_matrix(tr, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 5);
    assert_int_equal(get_matrix(tr, 2u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 6);

    return_matrix(src);
    return_matrix(tr);
}

// --------------------------------------------------------------------------------

static void test_transpose_dense_square_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t src_r = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);
    assert_true(src_r.has_value);
    assert_non_null(src_r.u.value);

    matrix_t* src = src_r.u.value;

    int32_t v1 = 10;
    int32_t v2 = 20;
    int32_t v3 = 30;
    int32_t v4 = 40;
    int32_t out = 0;

    assert_int_equal(set_matrix(src, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(src, 1u, 0u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(src, 1u, 2u, &v3), NO_ERROR);
    assert_int_equal(set_matrix(src, 2u, 2u, &v4), NO_ERROR);

    matrix_expect_t tr_r = transpose_matrix(src, alloc);
    assert_true(tr_r.has_value);
    assert_non_null(tr_r.u.value);

    matrix_t* tr = tr_r.u.value;

    assert_int_equal(tr->format, DENSE_MATRIX);
    assert_int_equal(tr->rows, 3u);
    assert_int_equal(tr->cols, 3u);

    assert_int_equal(get_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10);

    assert_int_equal(get_matrix(tr, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 20);

    assert_int_equal(get_matrix(tr, 2u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 30);

    assert_int_equal(get_matrix(tr, 2u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 40);

    return_matrix(src);
    return_matrix(tr);
}

// --------------------------------------------------------------------------------

static void test_transpose_dense_twice_returns_original(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t src_r = init_dense_matrix(3u, 2u, INT32_TYPE, alloc);
    assert_true(src_r.has_value);
    assert_non_null(src_r.u.value);

    matrix_t* src = src_r.u.value;

    int32_t v1 = 10;
    int32_t v2 = 20;
    int32_t v3 = 30;
    int32_t v4 = 40;

    assert_int_equal(set_matrix(src, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(src, 1u, 1u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(src, 2u, 0u, &v3), NO_ERROR);
    assert_int_equal(set_matrix(src, 2u, 1u, &v4), NO_ERROR);

    matrix_expect_t t1_r = transpose_matrix(src, alloc);
    assert_true(t1_r.has_value);
    assert_non_null(t1_r.u.value);

    matrix_t* t1 = t1_r.u.value;

    matrix_expect_t t2_r = transpose_matrix(t1, alloc);
    assert_true(t2_r.has_value);
    assert_non_null(t2_r.u.value);

    matrix_t* t2 = t2_r.u.value;

    assert_true(matrix_equal(src, t2));

    return_matrix(src);
    return_matrix(t1);
    return_matrix(t2);
}

// --------------------------------------------------------------------------------

static void test_transpose_coo_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t src_r = init_coo_matrix(3u, 4u, 6u, INT32_TYPE, true, alloc);
    assert_true(src_r.has_value);
    assert_non_null(src_r.u.value);

    matrix_t* src = src_r.u.value;

    int32_t v1 = 11;
    int32_t v2 = 22;
    int32_t v3 = 33;
    int32_t out = 0;

    assert_int_equal(push_back_coo_matrix(src, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(src, 1u, 3u, &v2), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(src, 2u, 0u, &v3), NO_ERROR);

    matrix_expect_t tr_r = transpose_matrix(src, alloc);
    assert_true(tr_r.has_value);
    assert_non_null(tr_r.u.value);

    matrix_t* tr = tr_r.u.value;

    assert_int_equal(tr->format, COO_MATRIX);
    assert_int_equal(tr->rows, 4u);
    assert_int_equal(tr->cols, 3u);
    assert_int_equal(tr->dtype, INT32_TYPE);
    assert_int_equal(matrix_nnz(tr), 3u);

    assert_int_equal(get_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 11);

    assert_int_equal(get_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 22);

    assert_int_equal(get_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 33);

    return_matrix(src);
    return_matrix(tr);
}

// --------------------------------------------------------------------------------

static void test_transpose_coo_twice_returns_original(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t src_r = init_coo_matrix(4u, 3u, 6u, INT32_TYPE, true, alloc);
    assert_true(src_r.has_value);
    assert_non_null(src_r.u.value);

    matrix_t* src = src_r.u.value;

    int32_t v1 = 5;
    int32_t v2 = 15;
    int32_t v3 = 25;

    assert_int_equal(push_back_coo_matrix(src, 0u, 2u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(src, 2u, 1u, &v2), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(src, 3u, 0u, &v3), NO_ERROR);

    matrix_expect_t t1_r = transpose_matrix(src, alloc);
    assert_true(t1_r.has_value);
    assert_non_null(t1_r.u.value);

    matrix_t* t1 = t1_r.u.value;

    matrix_expect_t t2_r = transpose_matrix(t1, alloc);
    assert_true(t2_r.has_value);
    assert_non_null(t2_r.u.value);

    matrix_t* t2 = t2_r.u.value;

    assert_true(matrix_equal(src, t2));

    return_matrix(src);
    return_matrix(t1);
    return_matrix(t2);
}

// --------------------------------------------------------------------------------

static void test_transpose_csr_returns_csr(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t dense_r = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    assert_non_null(dense_r.u.value);
    matrix_t* dense = dense_r.u.value;
    int32_t v1 = 7;
    int32_t v2 = 8;
    int32_t v3 = 9;
    int32_t out = 0;
    assert_int_equal(set_matrix(dense, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 3u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 0u, &v3), NO_ERROR);
    matrix_expect_t csr_r = convert_matrix(dense, CSR_MATRIX, alloc);
    assert_true(csr_r.has_value);
    assert_non_null(csr_r.u.value);
    matrix_t* csr = csr_r.u.value;
    matrix_expect_t tr_r = transpose_matrix(csr, alloc);
    assert_true(tr_r.has_value);
    assert_non_null(tr_r.u.value);
    matrix_t* tr = tr_r.u.value;
    assert_int_equal(tr->format, CSR_MATRIX);  /* CSR -> CSR, format preserved */
    assert_int_equal(tr->rows, 4u);
    assert_int_equal(tr->cols, 3u);
    assert_int_equal(tr->dtype, INT32_TYPE);
    assert_int_equal(get_matrix(tr, 1u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 7);
    assert_int_equal(get_matrix(tr, 3u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 8);
    assert_int_equal(get_matrix(tr, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 9);
    return_matrix(dense);
    return_matrix(csr);
    return_matrix(tr);
}
// --------------------------------------------------------------------------------

static void test_transpose_csr_twice_returns_original(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(4u, 3u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    assert_non_null(dense_r.u.value);

    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 2;
    int32_t v2 = 4;
    int32_t v3 = 6;

    assert_int_equal(set_matrix(dense, 0u, 2u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 1u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 3u, 0u, &v3), NO_ERROR);

    matrix_expect_t csr_r = convert_matrix(dense, CSR_MATRIX, alloc);
    assert_true(csr_r.has_value);
    assert_non_null(csr_r.u.value);

    matrix_t* csr = csr_r.u.value;

    matrix_expect_t t1_r = transpose_matrix(csr, alloc);
    assert_true(t1_r.has_value);
    assert_non_null(t1_r.u.value);

    matrix_t* t1 = t1_r.u.value;

    matrix_expect_t t2_r = transpose_matrix(t1, alloc);
    assert_true(t2_r.has_value);
    assert_non_null(t2_r.u.value);

    matrix_t* t2 = t2_r.u.value;

    assert_true(matrix_equal(csr, t2));

    return_matrix(dense);
    return_matrix(csr);
    return_matrix(t1);
    return_matrix(t2);
}

// --------------------------------------------------------------------------------

static void test_transpose_csc_returns_csc(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t dense_r = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    assert_non_null(dense_r.u.value);
    matrix_t* dense = dense_r.u.value;
    int32_t v1 = 12;
    int32_t v2 = 24;
    int32_t v3 = 36;
    int32_t out = 0;
    assert_int_equal(set_matrix(dense, 0u, 2u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 0u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 3u, &v3), NO_ERROR);
    matrix_expect_t csc_r = convert_matrix(dense, CSC_MATRIX, alloc);
    assert_true(csc_r.has_value);
    assert_non_null(csc_r.u.value);
    matrix_t* csc = csc_r.u.value;
    matrix_expect_t tr_r = transpose_matrix(csc, alloc);
    assert_true(tr_r.has_value);
    assert_non_null(tr_r.u.value);
    matrix_t* tr = tr_r.u.value;
    assert_int_equal(tr->format, CSC_MATRIX);  /* CSC -> CSC, format preserved */
    assert_int_equal(tr->rows, 4u);
    assert_int_equal(tr->cols, 3u);
    assert_int_equal(tr->dtype, INT32_TYPE);
    assert_int_equal(get_matrix(tr, 2u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 12);
    assert_int_equal(get_matrix(tr, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 24);
    assert_int_equal(get_matrix(tr, 3u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 36);
    return_matrix(dense);
    return_matrix(csc);
    return_matrix(tr);
}

// --------------------------------------------------------------------------------

static void test_transpose_csc_twice_returns_original(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(4u, 3u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    assert_non_null(dense_r.u.value);

    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 3;
    int32_t v2 = 6;
    int32_t v3 = 9;

    assert_int_equal(set_matrix(dense, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 2u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 3u, 0u, &v3), NO_ERROR);

    matrix_expect_t csc_r = convert_matrix(dense, CSC_MATRIX, alloc);
    assert_true(csc_r.has_value);
    assert_non_null(csc_r.u.value);

    matrix_t* csc = csc_r.u.value;

    matrix_expect_t t1_r = transpose_matrix(csc, alloc);
    assert_true(t1_r.has_value);
    assert_non_null(t1_r.u.value);

    matrix_t* t1 = t1_r.u.value;

    matrix_expect_t t2_r = transpose_matrix(t1, alloc);
    assert_true(t2_r.has_value);
    assert_non_null(t2_r.u.value);

    matrix_t* t2 = t2_r.u.value;

    assert_true(matrix_equal(csc, t2));

    return_matrix(dense);
    return_matrix(csc);
    return_matrix(t1);
    return_matrix(t2);
}

// --------------------------------------------------------------------------------

static void test_transpose_matrix_null_input(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = transpose_matrix(NULL, alloc);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// ================================================================================
// fill_matrix tests
// ================================================================================

static void test_fill_matrix_dense_nonzero_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    int32_t fill = 9;
    int32_t out = 0;

    assert_int_equal(fill_matrix(mat, &fill), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 2u; ++j) {
            assert_int_equal(get_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal(out, 9);
        }
    }

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_fill_matrix_dense_zero_calls_zero_behavior(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 3u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    int32_t v1 = 1;
    int32_t v2 = 2;
    int32_t zero = 0;
    int32_t out = -1;

    assert_int_equal(set_matrix(mat, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 2u, &v2), NO_ERROR);

    assert_int_equal(fill_matrix(mat, &zero), NO_ERROR);

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            assert_int_equal(get_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal(out, 0);
        }
    }

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_fill_matrix_sparse_nonzero_unavailable(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    int32_t value = 5;

    assert_int_equal(fill_matrix(mat, &value), OPERATION_UNAVAILABLE);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_fill_matrix_sparse_zero_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    int32_t v1 = 3;
    int32_t v2 = 6;
    int32_t zero = 0;
    int32_t out = -1;

    assert_int_equal(push_back_coo_matrix(mat, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 2u, 2u, &v2), NO_ERROR);
    assert_int_equal(mat->rep.coo.nnz, 2u);

    assert_int_equal(fill_matrix(mat, &zero), NO_ERROR);
    assert_int_equal(mat->rep.coo.nnz, 0u);

    assert_int_equal(get_matrix(mat, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 0);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_fill_matrix_null_inputs(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    int32_t value = 1;

    assert_int_equal(fill_matrix(NULL, &value), NULL_POINTER);
    assert_int_equal(fill_matrix(mat, NULL), NULL_POINTER);

    return_matrix(mat);
}

// ================================================================================
// is_zero_matrix tests
// ================================================================================

static void test_is_zero_matrix_dense_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    assert_true(is_zero_matrix(mat));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_is_zero_matrix_dense_false(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    int32_t value = 1;
    assert_int_equal(set_matrix(mat, 1u, 1u, &value), NO_ERROR);

    assert_false(is_zero_matrix(mat));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_is_zero_matrix_coo_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    assert_true(is_zero_matrix(mat));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_is_zero_matrix_coo_false(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    int32_t value = 4;
    assert_int_equal(push_back_coo_matrix(mat, 0u, 2u, &value), NO_ERROR);

    assert_false(is_zero_matrix(mat));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_is_zero_matrix_null_matrix(void** state) {
    (void)state;

    assert_true(is_zero_matrix(NULL));
}

// ================================================================================
// dtype / compatibility tests
// ================================================================================

static void test_matrix_has_same_dtype_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_coo_matrix(3u, 3u, 4u, INT32_TYPE, true, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(matrix_has_same_dtype(r1.u.value, r2.u.value));

    return_matrix(r1.u.value);
    return_matrix(r2.u.value);
}

// --------------------------------------------------------------------------------

static void test_matrix_has_same_dtype_false(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(2u, 2u, FLOAT_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_false(matrix_has_same_dtype(r1.u.value, r2.u.value));

    return_matrix(r1.u.value);
    return_matrix(r2.u.value);
}

// --------------------------------------------------------------------------------

static void test_matrix_has_same_dtype_null_inputs(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    assert_false(matrix_has_same_dtype(NULL, mat));
    assert_false(matrix_has_same_dtype(mat, NULL));
    assert_false(matrix_has_same_dtype(NULL, NULL));

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_matrix_is_add_compatible_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_coo_matrix(3u, 4u, 6u, INT32_TYPE, true, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(matrix_is_add_compatible(r1.u.value, r2.u.value));

    return_matrix(r1.u.value);
    return_matrix(r2.u.value);
}

// --------------------------------------------------------------------------------

static void test_matrix_is_add_compatible_false_shape(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(4u, 3u, INT32_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_false(matrix_is_add_compatible(r1.u.value, r2.u.value));

    return_matrix(r1.u.value);
    return_matrix(r2.u.value);
}

// --------------------------------------------------------------------------------

static void test_matrix_is_add_compatible_false_dtype(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(3u, 4u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(3u, 4u, DOUBLE_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_false(matrix_is_add_compatible(r1.u.value, r2.u.value));

    return_matrix(r1.u.value);
    return_matrix(r2.u.value);
}

// --------------------------------------------------------------------------------

static void test_matrix_is_multiply_compatible_true(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(2u, 3u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(3u, 5u, INT32_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_true(matrix_is_multiply_compatible(r1.u.value, r2.u.value));

    return_matrix(r1.u.value);
    return_matrix(r2.u.value);
}

// --------------------------------------------------------------------------------

static void test_matrix_is_multiply_compatible_false_shape(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(2u, 4u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(3u, 5u, INT32_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_false(matrix_is_multiply_compatible(r1.u.value, r2.u.value));

    return_matrix(r1.u.value);
    return_matrix(r2.u.value);
}

// --------------------------------------------------------------------------------

static void test_matrix_is_multiply_compatible_false_dtype(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(2u, 3u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(3u, 2u, FLOAT_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    assert_false(matrix_is_multiply_compatible(r1.u.value, r2.u.value));

    return_matrix(r1.u.value);
    return_matrix(r2.u.value);
}

// ================================================================================
// swap_matrix_rows tests
// ================================================================================

static void test_swap_matrix_rows_dense_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(3u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    int32_t v00 = 1, v01 = 2;
    int32_t v10 = 3, v11 = 4;
    int32_t v20 = 5, v21 = 6;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 0u, 0u, &v00), NO_ERROR);
    assert_int_equal(set_matrix(mat, 0u, 1u, &v01), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 0u, &v10), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 1u, &v11), NO_ERROR);
    assert_int_equal(set_matrix(mat, 2u, 0u, &v20), NO_ERROR);
    assert_int_equal(set_matrix(mat, 2u, 1u, &v21), NO_ERROR);

    assert_int_equal(swap_matrix_rows(mat, 0u, 2u), NO_ERROR);

    assert_int_equal(get_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 5);
    assert_int_equal(get_matrix(mat, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 6);

    assert_int_equal(get_matrix(mat, 2u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 1);
    assert_int_equal(get_matrix(mat, 2u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 2);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_swap_matrix_rows_coo_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(4u, 4u, 6u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    int32_t v1 = 10;
    int32_t v2 = 20;
    int32_t out = 0;

    assert_int_equal(push_back_coo_matrix(mat, 0u, 1u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 3u, 2u, &v2), NO_ERROR);

    assert_int_equal(swap_matrix_rows(mat, 0u, 3u), NO_ERROR);

    assert_int_equal(get_matrix(mat, 3u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10);

    assert_int_equal(get_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 20);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_swap_matrix_rows_same_index_noop(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    int32_t value = 7;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 1u, 1u, &value), NO_ERROR);
    assert_int_equal(swap_matrix_rows(mat, 1u, 1u), NO_ERROR);
    assert_int_equal(get_matrix(mat, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 7);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_swap_matrix_rows_invalid_row(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    assert_int_equal(swap_matrix_rows(mat, 0u, 2u), OUT_OF_BOUNDS);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_swap_matrix_rows_sparse_compressed_unavailable(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t value = 1;
    assert_int_equal(set_matrix(dense, 0u, 1u, &value), NO_ERROR);

    matrix_expect_t csr_r = convert_matrix(dense, CSR_MATRIX, alloc);
    assert_true(csr_r.has_value);
    matrix_t* csr = csr_r.u.value;

    matrix_expect_t csc_r = convert_matrix(dense, CSC_MATRIX, alloc);
    assert_true(csc_r.has_value);
    matrix_t* csc = csc_r.u.value;

    assert_int_equal(swap_matrix_rows(csr, 0u, 1u), OPERATION_UNAVAILABLE);
    assert_int_equal(swap_matrix_rows(csc, 0u, 1u), OPERATION_UNAVAILABLE);

    return_matrix(dense);
    return_matrix(csr);
    return_matrix(csc);
}

// ================================================================================
// swap_matrix_cols tests
// ================================================================================

static void test_swap_matrix_cols_dense_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 3u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    int32_t v00 = 1, v01 = 2, v02 = 3;
    int32_t v10 = 4, v11 = 5, v12 = 6;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 0u, 0u, &v00), NO_ERROR);
    assert_int_equal(set_matrix(mat, 0u, 1u, &v01), NO_ERROR);
    assert_int_equal(set_matrix(mat, 0u, 2u, &v02), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 0u, &v10), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 1u, &v11), NO_ERROR);
    assert_int_equal(set_matrix(mat, 1u, 2u, &v12), NO_ERROR);

    assert_int_equal(swap_matrix_cols(mat, 0u, 2u), NO_ERROR);

    assert_int_equal(get_matrix(mat, 0u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 3);
    assert_int_equal(get_matrix(mat, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 1);

    assert_int_equal(get_matrix(mat, 1u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 6);
    assert_int_equal(get_matrix(mat, 1u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 4);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_swap_matrix_cols_coo_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_coo_matrix(4u, 4u, 6u, INT32_TYPE, true, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    int32_t v1 = 10;
    int32_t v2 = 20;
    int32_t out = 0;

    assert_int_equal(push_back_coo_matrix(mat, 1u, 0u, &v1), NO_ERROR);
    assert_int_equal(push_back_coo_matrix(mat, 2u, 3u, &v2), NO_ERROR);

    assert_int_equal(swap_matrix_cols(mat, 0u, 3u), NO_ERROR);

    assert_int_equal(get_matrix(mat, 1u, 3u, &out), NO_ERROR);
    assert_int_equal(out, 10);

    assert_int_equal(get_matrix(mat, 2u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 20);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_swap_matrix_cols_same_index_noop(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    int32_t value = 8;
    int32_t out = 0;

    assert_int_equal(set_matrix(mat, 0u, 1u, &value), NO_ERROR);
    assert_int_equal(swap_matrix_cols(mat, 1u, 1u), NO_ERROR);
    assert_int_equal(get_matrix(mat, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 8);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_swap_matrix_cols_invalid_col(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();
    matrix_expect_t r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    matrix_t* mat = r.u.value;

    assert_int_equal(swap_matrix_cols(mat, 0u, 2u), OUT_OF_BOUNDS);

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_swap_matrix_cols_sparse_compressed_unavailable(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t value = 1;
    assert_int_equal(set_matrix(dense, 0u, 1u, &value), NO_ERROR);

    matrix_expect_t csr_r = convert_matrix(dense, CSR_MATRIX, alloc);
    assert_true(csr_r.has_value);
    matrix_t* csr = csr_r.u.value;

    matrix_expect_t csc_r = convert_matrix(dense, CSC_MATRIX, alloc);
    assert_true(csc_r.has_value);
    matrix_t* csc = csc_r.u.value;

    assert_int_equal(swap_matrix_cols(csr, 0u, 1u), OPERATION_UNAVAILABLE);
    assert_int_equal(swap_matrix_cols(csc, 0u, 1u), OPERATION_UNAVAILABLE);

    return_matrix(dense);
    return_matrix(csr);
    return_matrix(csc);
}

// ================================================================================
// init_identity_matrix tests
// ================================================================================

static void test_init_identity_matrix_int32_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_identity_matrix(4u, INT32_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;
    int32_t out = -1;

    assert_int_equal(mat->format, DENSE_MATRIX);
    assert_int_equal(mat->rows, 4u);
    assert_int_equal(mat->cols, 4u);
    assert_int_equal(mat->dtype, INT32_TYPE);

    for (size_t i = 0u; i < 4u; ++i) {
        for (size_t j = 0u; j < 4u; ++j) {
            assert_int_equal(get_matrix(mat, i, j, &out), NO_ERROR);
            if (i == j) {
                assert_int_equal(out, 1);
            } else {
                assert_int_equal(out, 0);
            }
        }
    }

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_init_identity_matrix_float_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_identity_matrix(3u, FLOAT_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;
    float out = -1.0f;

    for (size_t i = 0u; i < 3u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            assert_int_equal(get_matrix(mat, i, j, &out), NO_ERROR);
            if (i == j) {
                assert_true(out == 1.0f);
            } else {
                assert_true(out == 0.0f);
            }
        }
    }

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_init_identity_matrix_double_success(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_identity_matrix(2u, DOUBLE_TYPE, alloc);

    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;
    double out = -1.0;

    for (size_t i = 0u; i < 2u; ++i) {
        for (size_t j = 0u; j < 2u; ++j) {
            assert_int_equal(get_matrix(mat, i, j, &out), NO_ERROR);
            if (i == j) {
                assert_true(out == 1.0);
            } else {
                assert_true(out == 0.0);
            }
        }
    }

    return_matrix(mat);
}

// --------------------------------------------------------------------------------

static void test_init_identity_matrix_invalid_size(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_identity_matrix(0u, INT32_TYPE, alloc);

    assert_false(r.has_value);
}

// --------------------------------------------------------------------------------

static void test_init_identity_matrix_invalid_dtype(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_identity_matrix(3u, UNKNOWN_TYPE, alloc);

    assert_false(r.has_value);
}

// --------------------------------------------------------------------------------

static void test_init_identity_matrix_null_allocator(void** state) {
    (void)state;

    allocator_vtable_t alloc = {0};

    matrix_expect_t r = init_identity_matrix(3u, INT32_TYPE, alloc);

    assert_false(r.has_value);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// Callback-aware generic tests
// ================================================================================

static bool int32_equal_cb(const void* a, const void* b) {
    const int32_t* lhs = (const int32_t*)a;
    const int32_t* rhs = (const int32_t*)b;
    return (*lhs == *rhs);
}

// --------------------------------------------------------------------------------

static bool int32_zero_cb(const void* value) {
    const int32_t* v = (const int32_t*)value;
    return (*v == 0);
}

// --------------------------------------------------------------------------------

static bool abs_int32_equal_cb(const void* a, const void* b) {
    const int32_t* lhs = (const int32_t*)a;
    const int32_t* rhs = (const int32_t*)b;

    int32_t al = (*lhs < 0) ? -(*lhs) : *lhs;
    int32_t ar = (*rhs < 0) ? -(*rhs) : *rhs;
    return (al == ar);
}

// --------------------------------------------------------------------------------

static bool neg_one_is_zero_cb(const void* value) {
    const int32_t* v = (const int32_t*)value;
    return (*v == -1);
}


static bool zero_or_neg_one_is_zero_cb(const void* value) {
    const int32_t* v = (const int32_t*)value;
    return (*v == 0 || *v == -1);
}
// ================================================================================
// 1. matrix_equal_cmp NULL comparator matches matrix_equal
// ================================================================================

static void test_matrix_equal_cmp_null_callback_matches_matrix_equal(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* a = r1.u.value;
    matrix_t* b = r2.u.value;

    int32_t v00 = 1;
    int32_t v01 = 2;
    int32_t v10 = 3;
    int32_t v11 = 4;

    assert_int_equal(set_matrix(a, 0u, 0u, &v00), NO_ERROR);
    assert_int_equal(set_matrix(a, 0u, 1u, &v01), NO_ERROR);
    assert_int_equal(set_matrix(a, 1u, 0u, &v10), NO_ERROR);
    assert_int_equal(set_matrix(a, 1u, 1u, &v11), NO_ERROR);

    assert_int_equal(set_matrix(b, 0u, 0u, &v00), NO_ERROR);
    assert_int_equal(set_matrix(b, 0u, 1u, &v01), NO_ERROR);
    assert_int_equal(set_matrix(b, 1u, 0u, &v10), NO_ERROR);
    assert_int_equal(set_matrix(b, 1u, 1u, &v11), NO_ERROR);

    assert_true(matrix_equal(a, b));
    assert_true(matrix_equal_cmp(a, b, NULL));

    return_matrix(a);
    return_matrix(b);
}

// ================================================================================
// 2. matrix_equal_cmp uses custom comparator
// ================================================================================

static void test_matrix_equal_cmp_uses_custom_comparator(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(2u, 2u, INT32_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* a = r1.u.value;
    matrix_t* b = r2.u.value;

    int32_t a00 = -1;
    int32_t a01 = -2;
    int32_t a10 = -3;
    int32_t a11 = -4;

    int32_t b00 = 1;
    int32_t b01 = 2;
    int32_t b10 = 3;
    int32_t b11 = 4;

    assert_int_equal(set_matrix(a, 0u, 0u, &a00), NO_ERROR);
    assert_int_equal(set_matrix(a, 0u, 1u, &a01), NO_ERROR);
    assert_int_equal(set_matrix(a, 1u, 0u, &a10), NO_ERROR);
    assert_int_equal(set_matrix(a, 1u, 1u, &a11), NO_ERROR);

    assert_int_equal(set_matrix(b, 0u, 0u, &b00), NO_ERROR);
    assert_int_equal(set_matrix(b, 0u, 1u, &b01), NO_ERROR);
    assert_int_equal(set_matrix(b, 1u, 0u, &b10), NO_ERROR);
    assert_int_equal(set_matrix(b, 1u, 1u, &b11), NO_ERROR);

    assert_false(matrix_equal(a, b));
    assert_true(matrix_equal_cmp(a, b, abs_int32_equal_cb));

    return_matrix(a);
    return_matrix(b);
}

// ================================================================================
// 3. fill_matrix_zero NULL callback matches fill_matrix
// ================================================================================

static void test_fill_matrix_zero_null_callback_matches_fill_matrix(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r1 = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);
    matrix_expect_t r2 = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);

    assert_true(r1.has_value);
    assert_true(r2.has_value);

    matrix_t* a = r1.u.value;
    matrix_t* b = r2.u.value;

    int32_t value = 7;
    int32_t out = 0;

    assert_int_equal(fill_matrix(a, &value), NO_ERROR);
    assert_int_equal(fill_matrix_zero(b, &value, NULL), NO_ERROR);

    assert_true(matrix_equal(a, b));

    assert_int_equal(get_matrix(a, 2u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 7);

    assert_int_equal(get_matrix(b, 2u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 7);

    return_matrix(a);
    return_matrix(b);
}

// ================================================================================
// 4. fill_matrix_zero uses custom zero predicate
// ================================================================================

static void test_fill_matrix_zero_uses_custom_zero_predicate(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t r = init_dense_matrix(3u, 3u, INT32_TYPE, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);

    matrix_t* mat = r.u.value;

    int32_t seed = 9;
    int32_t semantic_zero = -1;
    int32_t out = 0;

    assert_int_equal(fill_matrix(mat, &seed), NO_ERROR);
    assert_false(matrix_equal_cmp(mat, NULL, NULL)); /* harmless sanity split */

    assert_int_equal(fill_matrix_zero(mat, &semantic_zero, neg_one_is_zero_cb), NO_ERROR);

    for (size_t i = 0u; i < 3u; ++i) {
        for (size_t j = 0u; j < 3u; ++j) {
            assert_int_equal(get_matrix(mat, i, j, &out), NO_ERROR);
            assert_int_equal(out, 0);
        }
    }

    return_matrix(mat);
}

// ================================================================================
// 5. convert_matrix_zero dense->CSR uses custom zero predicate
// ================================================================================

static void test_convert_matrix_zero_dense_to_csr_custom_zero_predicate(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(2u, 3u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 5;
    int32_t v2 = -1;   /* semantic zero under callback */
    int32_t v3 = 8;
    int32_t out = 0;

    assert_int_equal(set_matrix(dense, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 0u, 1u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 2u, &v3), NO_ERROR);

    matrix_expect_t csr_r = convert_matrix_zero(dense, CSR_MATRIX, alloc, zero_or_neg_one_is_zero_cb);
    assert_true(csr_r.has_value);
    matrix_t* csr = csr_r.u.value;
    
    assert_int_equal(csr->format, CSR_MATRIX);
    assert_int_equal(csr->rep.csr.nnz, 2u);

    assert_int_equal(get_matrix(csr, 0u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 5);

    assert_int_equal(get_matrix(csr, 0u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 0);

    assert_int_equal(get_matrix(csr, 1u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 8);

    return_matrix(dense);
    return_matrix(csr);
}

// ================================================================================
// 6. convert_matrix_zero dense->CSC uses custom zero predicate
// ================================================================================

static void test_convert_matrix_zero_dense_to_csc_custom_zero_predicate(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(3u, 2u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t v1 = -1;   /* semantic zero under callback */
    int32_t v2 = 4;
    int32_t v3 = 9;
    int32_t out = 0;

    assert_int_equal(set_matrix(dense, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 1u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 2u, 0u, &v3), NO_ERROR);

    matrix_expect_t csc_r = convert_matrix_zero(dense, CSC_MATRIX, alloc, zero_or_neg_one_is_zero_cb);
    assert_true(csc_r.has_value);
    matrix_t* csc = csc_r.u.value;

    assert_int_equal(csc->format, CSC_MATRIX);
    assert_int_equal(csc->rep.csc.nnz, 2u);

    assert_int_equal(get_matrix(csc, 0u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 0);

    assert_int_equal(get_matrix(csc, 1u, 1u, &out), NO_ERROR);
    assert_int_equal(out, 4);

    assert_int_equal(get_matrix(csc, 2u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 9);

    return_matrix(dense);
    return_matrix(csc);
}

// ================================================================================
// 7. convert_matrix_zero dense->COO uses custom zero predicate
// ================================================================================

static void test_convert_matrix_zero_dense_to_coo_custom_zero_predicate(void** state) {
    (void)state;

    allocator_vtable_t alloc = heap_allocator();

    matrix_expect_t dense_r = init_dense_matrix(2u, 4u, INT32_TYPE, alloc);
    assert_true(dense_r.has_value);
    matrix_t* dense = dense_r.u.value;

    int32_t v1 = 3;
    int32_t v2 = -1;   /* semantic zero under callback */
    int32_t v3 = 7;
    int32_t out = 0;

    assert_int_equal(set_matrix(dense, 0u, 0u, &v1), NO_ERROR);
    assert_int_equal(set_matrix(dense, 0u, 2u, &v2), NO_ERROR);
    assert_int_equal(set_matrix(dense, 1u, 3u, &v3), NO_ERROR);

    matrix_expect_t coo_r = convert_matrix_zero(dense, COO_MATRIX, alloc, zero_or_neg_one_is_zero_cb);
    assert_true(coo_r.has_value);
    matrix_t* coo = coo_r.u.value;

    assert_int_equal(coo->format, COO_MATRIX);
    assert_int_equal(coo->rep.coo.nnz, 2u);

    assert_int_equal(get_matrix(coo, 0u, 0u, &out), NO_ERROR);
    assert_int_equal(out, 3);

    assert_int_equal(get_matrix(coo, 0u, 2u, &out), NO_ERROR);
    assert_int_equal(out, 0);

    assert_int_equal(get_matrix(coo, 1u, 3u, &out), NO_ERROR);
    assert_int_equal(out, 7);

    return_matrix(dense);
    return_matrix(coo);
}
// ================================================================================
// Test runner
// ================================================================================

const struct CMUnitTest test_matrix[] = {
    cmocka_unit_test_setup(test_init_dense_matrix_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_dense_matrix_zero_initialized, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_dense_matrix_invalid_rows, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_dense_matrix_invalid_cols, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_dense_matrix_invalid_dtype, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_dense_matrix_null_allocator, setup_dtype_registry),

    cmocka_unit_test_setup(test_init_coo_matrix_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_coo_matrix_growth_disabled, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_coo_matrix_invalid_rows, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_coo_matrix_invalid_cols, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_coo_matrix_invalid_capacity, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_coo_matrix_invalid_dtype, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_coo_matrix_null_allocator, setup_dtype_registry),

    cmocka_unit_test_setup(test_return_dense_matrix_smoke, setup_dtype_registry),
    cmocka_unit_test_setup(test_return_coo_matrix_smoke, setup_dtype_registry),
    cmocka_unit_test(test_return_null_matrix_is_safe),

    cmocka_unit_test_setup(test_dense_set_and_get_single_value, setup_dtype_registry),
    cmocka_unit_test_setup(test_dense_set_and_get_multiple_values, setup_dtype_registry),
    cmocka_unit_test_setup(test_dense_get_unset_value_returns_zero, setup_dtype_registry),
    cmocka_unit_test_setup(test_dense_set_invalid_row, setup_dtype_registry),
    cmocka_unit_test_setup(test_dense_set_invalid_col, setup_dtype_registry),
    cmocka_unit_test_setup(test_dense_get_invalid_row, setup_dtype_registry),
    cmocka_unit_test_setup(test_dense_get_invalid_col, setup_dtype_registry),
    cmocka_unit_test_setup(test_dense_set_null_value, setup_dtype_registry),
    cmocka_unit_test_setup(test_dense_get_null_out, setup_dtype_registry),

    cmocka_unit_test_setup(test_coo_set_and_get_single_value, setup_dtype_registry),
    cmocka_unit_test_setup(test_coo_get_missing_value_returns_zero, setup_dtype_registry),
    cmocka_unit_test_setup(test_coo_set_and_get_multiple_unsorted_values, setup_dtype_registry),
    cmocka_unit_test_setup(test_coo_sort_then_get_values, setup_dtype_registry),
    cmocka_unit_test_setup(test_coo_growth_updates_nnz_and_capacity, setup_dtype_registry),
    cmocka_unit_test_setup(test_coo_push_back_capacity_overflow_when_growth_disabled, setup_dtype_registry),
    cmocka_unit_test_setup(test_coo_set_invalid_row, setup_dtype_registry),
    cmocka_unit_test_setup(test_coo_set_invalid_col, setup_dtype_registry),
    cmocka_unit_test_setup(test_coo_get_invalid_row, setup_dtype_registry),
    cmocka_unit_test_setup(test_coo_get_invalid_col, setup_dtype_registry),
    cmocka_unit_test_setup(test_coo_set_null_value, setup_dtype_registry),
    cmocka_unit_test_setup(test_coo_get_null_out, setup_dtype_registry),

    cmocka_unit_test_setup(test_reserve_coo_matrix_grows_capacity, setup_dtype_registry),
    cmocka_unit_test_setup(test_reserve_coo_matrix_noop_when_capacity_not_increased, setup_dtype_registry),
    cmocka_unit_test_setup(test_reserve_coo_matrix_preserves_existing_entries, setup_dtype_registry),
    cmocka_unit_test(test_reserve_coo_matrix_null_matrix),
    cmocka_unit_test_setup(test_reserve_coo_matrix_invalid_for_dense_matrix, setup_dtype_registry),

    cmocka_unit_test_setup(test_sort_coo_matrix_on_empty_matrix, setup_dtype_registry),
    cmocka_unit_test_setup(test_sort_coo_matrix_on_single_entry, setup_dtype_registry),
    cmocka_unit_test_setup(test_sort_coo_matrix_orders_by_row_then_column, setup_dtype_registry),
    cmocka_unit_test_setup(test_sort_coo_matrix_preserves_values_with_indices, setup_dtype_registry),
    cmocka_unit_test(test_sort_coo_matrix_null_matrix),
    cmocka_unit_test_setup(test_sort_coo_matrix_invalid_for_dense_matrix, setup_dtype_registry),

    cmocka_unit_test_setup(test_matrix_storage_bytes_dense, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_storage_bytes_coo, setup_dtype_registry),
    cmocka_unit_test(test_matrix_storage_bytes_null_matrix),

    cmocka_unit_test(test_matrix_format_name_dense),
    cmocka_unit_test(test_matrix_format_name_coo),
    cmocka_unit_test(test_matrix_format_name_csr),
    cmocka_unit_test(test_matrix_format_name_csc),
    cmocka_unit_test(test_matrix_format_name_unknown),

    cmocka_unit_test_setup(test_matrix_equal_dense_same_values, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_equal_dense_different_values, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_equal_dense_and_coo_same_values, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_equal_dense_and_coo_different_values, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_equal_same_shape_but_different_dtype, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_equal_different_shapes, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_equal_null_inputs, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_equal_zero_initialized_dense_matrices, setup_dtype_registry),

    cmocka_unit_test_setup(test_matrix_has_same_shape_dense_dense_true, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_has_same_shape_dense_dense_false, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_has_same_shape_dense_coo_true, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_has_same_shape_dense_coo_false, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_has_same_shape_null_inputs, setup_dtype_registry),

    cmocka_unit_test_setup(test_matrix_is_square_dense_true, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_is_square_dense_false, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_is_square_coo_true, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_is_square_coo_false, setup_dtype_registry),
    cmocka_unit_test(test_matrix_is_square_null_matrix),

    cmocka_unit_test_setup(test_matrix_is_sparse_dense_false, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_is_sparse_coo_true, setup_dtype_registry),
    cmocka_unit_test(test_matrix_is_sparse_null_matrix),

    cmocka_unit_test_setup(test_copy_matrix_csr_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_copy_matrix_csc_success, setup_dtype_registry),

    cmocka_unit_test_setup(test_convert_dense_to_csr, setup_dtype_registry),
    cmocka_unit_test_setup(test_convert_dense_to_csc, setup_dtype_registry),
    cmocka_unit_test_setup(test_convert_coo_to_csr, setup_dtype_registry),
    cmocka_unit_test_setup(test_convert_coo_to_csc, setup_dtype_registry),
    cmocka_unit_test_setup(test_convert_csr_to_dense, setup_dtype_registry),
    cmocka_unit_test_setup(test_convert_csc_to_dense, setup_dtype_registry),
    cmocka_unit_test_setup(test_convert_csr_to_coo, setup_dtype_registry),
    cmocka_unit_test_setup(test_convert_csc_to_coo, setup_dtype_registry),
    cmocka_unit_test_setup(test_convert_csr_to_csc, setup_dtype_registry),
    cmocka_unit_test_setup(test_convert_csc_to_csr, setup_dtype_registry),

    cmocka_unit_test_setup(test_transpose_dense_rectangular_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_transpose_dense_square_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_transpose_dense_twice_returns_original, setup_dtype_registry),

    cmocka_unit_test_setup(test_transpose_coo_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_transpose_coo_twice_returns_original, setup_dtype_registry),

    cmocka_unit_test_setup(test_transpose_csr_returns_csr, setup_dtype_registry),
    cmocka_unit_test_setup(test_transpose_csr_twice_returns_original, setup_dtype_registry),

    cmocka_unit_test_setup(test_transpose_csc_returns_csc, setup_dtype_registry),
    cmocka_unit_test_setup(test_transpose_csc_twice_returns_original, setup_dtype_registry),

    cmocka_unit_test(test_transpose_matrix_null_input),

    cmocka_unit_test_setup(test_fill_matrix_dense_nonzero_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_fill_matrix_dense_zero_calls_zero_behavior, setup_dtype_registry),
    cmocka_unit_test_setup(test_fill_matrix_sparse_nonzero_unavailable, setup_dtype_registry),
    cmocka_unit_test_setup(test_fill_matrix_sparse_zero_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_fill_matrix_null_inputs, setup_dtype_registry),

    cmocka_unit_test_setup(test_is_zero_matrix_dense_true, setup_dtype_registry),
    cmocka_unit_test_setup(test_is_zero_matrix_dense_false, setup_dtype_registry),
    cmocka_unit_test_setup(test_is_zero_matrix_coo_true, setup_dtype_registry),
    cmocka_unit_test_setup(test_is_zero_matrix_coo_false, setup_dtype_registry),
    cmocka_unit_test(test_is_zero_matrix_null_matrix),

    cmocka_unit_test_setup(test_matrix_has_same_dtype_true, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_has_same_dtype_false, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_has_same_dtype_null_inputs, setup_dtype_registry),

    cmocka_unit_test_setup(test_matrix_is_add_compatible_true, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_is_add_compatible_false_shape, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_is_add_compatible_false_dtype, setup_dtype_registry),

    cmocka_unit_test_setup(test_matrix_is_multiply_compatible_true, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_is_multiply_compatible_false_shape, setup_dtype_registry),
    cmocka_unit_test_setup(test_matrix_is_multiply_compatible_false_dtype, setup_dtype_registry),

    cmocka_unit_test_setup(test_swap_matrix_rows_dense_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_swap_matrix_rows_coo_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_swap_matrix_rows_same_index_noop, setup_dtype_registry),
    cmocka_unit_test_setup(test_swap_matrix_rows_invalid_row, setup_dtype_registry),
    cmocka_unit_test_setup(test_swap_matrix_rows_sparse_compressed_unavailable, setup_dtype_registry),

    cmocka_unit_test_setup(test_swap_matrix_cols_dense_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_swap_matrix_cols_coo_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_swap_matrix_cols_same_index_noop, setup_dtype_registry),
    cmocka_unit_test_setup(test_swap_matrix_cols_invalid_col, setup_dtype_registry),
    cmocka_unit_test_setup(test_swap_matrix_cols_sparse_compressed_unavailable, setup_dtype_registry),

    cmocka_unit_test_setup(test_init_identity_matrix_int32_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_identity_matrix_float_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_identity_matrix_double_success, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_identity_matrix_invalid_size, setup_dtype_registry),
    cmocka_unit_test_setup(test_init_identity_matrix_invalid_dtype, setup_dtype_registry),
    cmocka_unit_test(test_init_identity_matrix_null_allocator),

    cmocka_unit_test(test_matrix_equal_cmp_null_callback_matches_matrix_equal),
    cmocka_unit_test(test_matrix_equal_cmp_uses_custom_comparator),
    cmocka_unit_test(test_fill_matrix_zero_null_callback_matches_fill_matrix),
    cmocka_unit_test(test_fill_matrix_zero_uses_custom_zero_predicate),
    cmocka_unit_test(test_convert_matrix_zero_dense_to_csr_custom_zero_predicate),
    cmocka_unit_test(test_convert_matrix_zero_dense_to_csc_custom_zero_predicate),
    cmocka_unit_test(test_convert_matrix_zero_dense_to_coo_custom_zero_predicate),

};

const size_t test_matrix_count = sizeof(test_matrix) / sizeof(test_matrix[0]);
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
