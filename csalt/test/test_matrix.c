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

};

const size_t test_matrix_count = sizeof(test_matrix) / sizeof(test_matrix[0]);

// ================================================================================
// ================================================================================
// eof
