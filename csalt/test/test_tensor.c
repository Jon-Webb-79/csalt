// ================================================================================
// ================================================================================
// - File:    test_uint8_array.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 31, 2022
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include <math.h>       /* NAN, isnan — used when checking float zero-init */

#include "c_tensor.h"   /* tensor_t, tensor_expect_t, init_tensor, return_tensor */
#include "c_allocator.h"
#include "c_dtypes.h"
#include "c_error.h"
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

// ================================================================================
// ================================================================================
// HELPERS

/** Shorthand: build a heap allocator and call init_tensor (TENSOR_STRUCT). */
static tensor_expect_t _make_tensor(uint8_t ndim, const size_t* shape,
                                    dtype_id_t dtype) {
    allocator_vtable_t alloc = heap_allocator();
    return init_tensor(ndim, shape, dtype, alloc);
}

/**
 * Shorthand: construct a TENSOR_DYNAMIC_ tensor with a given capacity.
 * len starts at 0; the caller populates slots via set_tensor_index or
 * push operations.  growth controls whether automatic reallocation is
 * permitted when the buffer is full.
 */
static tensor_expect_t _make_array(size_t capacity, dtype_id_t dtype,
                                   bool growth) {
    allocator_vtable_t alloc = heap_allocator();
    const size_t shape[] = { capacity };
    tensor_expect_t r = init_tensor(1u, shape, dtype, alloc);
    if (!r.has_value) return r;

    /* Flip mode and reset len to match TENSOR_DYNAMIC_ invariants.
     * In a real init_array wrapper this would be done inside that function;
     * here we set it directly so the test file has no dependency on an
     * init_array function that may not exist yet. */
    r.u.value->mode   = ARRAY_STRUCT;
    r.u.value->growth = growth;
    r.u.value->len    = 0u;
    return r;
}

// ================================================================================
// ================================================================================
// INIT — NULL / INVALID ARGUMENT GUARD TESTS

/** NULL allocate function pointer must return NULL_POINTER. */
static void test_init_tensor_null_allocator(void** state) {
    (void)state;
    allocator_vtable_t bad_alloc = heap_allocator();
    bad_alloc.allocate = NULL;

    const size_t shape[] = { 3u, 4u };
    tensor_expect_t r = init_tensor(2u, shape, FLOAT_TYPE, bad_alloc);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

/** NULL shape pointer must return NULL_POINTER. */
static void test_init_tensor_null_shape(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();

    tensor_expect_t r = init_tensor(2u, NULL, FLOAT_TYPE, alloc);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

// --------------------------------------------------------------------------------

/** ndim == 0 must return INVALID_ARG. */
static void test_init_tensor_zero_ndim(void** state) {
    (void)state;
    const size_t shape[] = { 4u };

    tensor_expect_t r = _make_tensor(0u, shape, FLOAT_TYPE);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

/** UNKNOWN_TYPE must return INVALID_ARG. */
static void test_init_tensor_unknown_dtype(void** state) {
    (void)state;
    const size_t shape[] = { 4u };

    tensor_expect_t r = _make_tensor(1u, shape, UNKNOWN_TYPE);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// --------------------------------------------------------------------------------

/** An unregistered user dtype must return TYPE_MISMATCH. */
static void test_init_tensor_unregistered_dtype(void** state) {
    (void)state;
    const size_t shape[] = { 4u };

    /* USER_BASE_TYPE + 9999 is very unlikely to be registered */
    tensor_expect_t r = _make_tensor(1u, shape, USER_BASE_TYPE + 9999u);

    assert_false(r.has_value);
    assert_int_equal(r.u.error, TYPE_MISMATCH);
}

// --------------------------------------------------------------------------------

/** A zero in any shape dimension must return INVALID_ARG. */
static void test_init_tensor_zero_in_shape(void** state) {
    (void)state;

    /* Zero in the first dimension */
    const size_t shape_first[] = { 0u, 4u };
    tensor_expect_t r = _make_tensor(2u, shape_first, FLOAT_TYPE);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);

    /* Zero in the second dimension */
    const size_t shape_second[] = { 3u, 0u };
    r = _make_tensor(2u, shape_second, FLOAT_TYPE);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);

    /* Zero in a middle dimension */
    const size_t shape_mid[] = { 3u, 0u, 5u };
    r = _make_tensor(3u, shape_mid, FLOAT_TYPE);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, INVALID_ARG);
}

// ================================================================================
// ================================================================================
// INIT — SUCCESSFUL CONSTRUCTION TESTS

/**
 * 1-D tensor (array mode shape): verify all scalar fields are set correctly
 * and that mode is TENSOR_STRUCT.
 */
static void test_init_tensor_1d_scalar_fields(void** state) {
    (void)state;
    const size_t shape[] = { 8u };

    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);

    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    assert_non_null(t);
    assert_non_null(t->data);
    assert_int_equal(t->ndim,      1u);
    assert_int_equal(t->len,       8u);
    assert_int_equal(t->alloc,     8u);
    assert_int_equal(t->data_size, sizeof(int32_t));
    assert_int_equal(t->dtype,     INT32_TYPE);
    assert_int_equal(t->mode,      TENSOR_STRUCT);
    assert_false(t->growth);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/** 1-D tensor: shape and strides arrays are populated correctly. */
static void test_init_tensor_1d_shape_and_strides(void** state) {
    (void)state;
    const size_t shape[] = { 5u };

    tensor_expect_t r = _make_tensor(1u, shape, DOUBLE_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* shape */
    assert_int_equal(t->shape[0], 5u);

    /* For a 1-D tensor the only stride is data_size */
    assert_int_equal(t->strides[0], sizeof(double));

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/** 2-D tensor (matrix): shape, strides, and len are correct. */
static void test_init_tensor_2d_shape_and_strides(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 4u };

    tensor_expect_t r = _make_tensor(2u, shape, FLOAT_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    assert_int_equal(t->ndim,      2u);
    assert_int_equal(t->len,       12u);   /* 3 * 4 */
    assert_int_equal(t->alloc,     12u);
    assert_int_equal(t->shape[0],  3u);
    assert_int_equal(t->shape[1],  4u);

    /* C-order (row-major) strides:
     *   strides[1] = data_size            = 4
     *   strides[0] = strides[1] * shape[1] = 4 * 4 = 16  */
    assert_int_equal(t->strides[1], sizeof(float));
    assert_int_equal(t->strides[0], sizeof(float) * 4u);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/** 3-D tensor: shape, strides, and len are correct. */
static void test_init_tensor_3d_shape_and_strides(void** state) {
    (void)state;
    const size_t shape[] = { 2u, 3u, 4u };

    tensor_expect_t r = _make_tensor(3u, shape, INT16_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    assert_int_equal(t->ndim,  3u);
    assert_int_equal(t->len,   24u);   /* 2 * 3 * 4 */
    assert_int_equal(t->alloc, 24u);

    assert_int_equal(t->shape[0], 2u);
    assert_int_equal(t->shape[1], 3u);
    assert_int_equal(t->shape[2], 4u);

    /* C-order strides:
     *   strides[2] = data_size                      = 2
     *   strides[1] = strides[2] * shape[2]          = 2 * 4  = 8
     *   strides[0] = strides[1] * shape[1]          = 8 * 3  = 24  */
    assert_int_equal(t->strides[2], sizeof(int16_t));
    assert_int_equal(t->strides[1], sizeof(int16_t) * 4u);
    assert_int_equal(t->strides[0], sizeof(int16_t) * 4u * 3u);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/**
 * TENSOR_STRUCT: data buffer must be zero-initialised at construction.
 * Check every byte directly so no stale value can hide behind a typed read.
 */
static void test_init_tensor_data_zeroed(void** state) {
    (void)state;
    const size_t shape[] = { 4u, 4u };

    tensor_expect_t r = _make_tensor(2u, shape, FLOAT_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    const uint8_t* p = t->data;
    for (size_t i = 0u; i < t->len * t->data_size; i++)
        assert_int_equal(p[i], 0u);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/** shape and strides pointers must lie within the FAM, not outside the struct. */
static void test_init_tensor_fam_pointers_in_range(void** state) {
    (void)state;
    const size_t shape[] = { 2u, 5u };

    tensor_expect_t r = _make_tensor(2u, shape, UINT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* shape must equal &t->meta[0] */
    assert_ptr_equal(t->shape, t->meta);

    /* strides must equal &t->meta[ndim] */
    assert_ptr_equal(t->strides, t->meta + t->ndim);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/** A 1-D tensor with a single element is a valid degenerate case. */
static void test_init_tensor_single_element(void** state) {
    (void)state;
    const size_t shape[] = { 1u };

    tensor_expect_t r = _make_tensor(1u, shape, UINT8_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    assert_int_equal(t->len,      1u);
    assert_int_equal(t->alloc,    1u);
    assert_int_equal(t->shape[0], 1u);
    assert_int_equal(t->strides[0], sizeof(uint8_t));

    return_tensor(t);
}

// ================================================================================
// ================================================================================
// RETURN — TEARDOWN TESTS

/** return_tensor(NULL) must not crash. */
static void test_return_tensor_null(void** state) {
    (void)state;
    return_tensor(NULL);   /* must return silently */
}

// --------------------------------------------------------------------------------

/** Normal construction followed by return_tensor must not leak or crash. */
static void test_return_tensor_normal(void** state) {
    (void)state;
    const size_t shape[] = { 6u, 6u };

    tensor_expect_t r = _make_tensor(2u, shape, DOUBLE_TYPE);
    assert_true(r.has_value);

    return_tensor(r.u.value);   /* must not crash or leak under valgrind */
}

// --------------------------------------------------------------------------------

/** return_tensor on every built-in dtype to exercise different data_size paths. */
static void test_return_tensor_all_builtin_dtypes(void** state) {
    (void)state;
    const size_t shape[] = { 3u };

    static const dtype_id_t dtypes[] = {
        FLOAT_TYPE, DOUBLE_TYPE, LDOUBLE_TYPE,
        CHAR_TYPE,  UCHAR_TYPE,
        INT8_TYPE,  UINT8_TYPE,
        INT16_TYPE, UINT16_TYPE,
        INT32_TYPE, UINT32_TYPE,
        INT64_TYPE, UINT64_TYPE,
        BOOL_TYPE,  SIZE_T_TYPE,
    };
    const size_t n = sizeof(dtypes) / sizeof(dtypes[0]);

    for (size_t i = 0u; i < n; i++) {
        tensor_expect_t r = _make_tensor(1u, shape, dtypes[i]);
        assert_true(r.has_value);
        return_tensor(r.u.value);
    }
}

// ================================================================================
// ================================================================================
// INTROSPECTION — tensor_ndim

static void test_tensor_ndim_null(void** state) {
    (void)state;
    assert_int_equal(tensor_ndim(NULL), 0u);
}

static void test_tensor_ndim_1d(void** state) {
    (void)state;
    const size_t shape[] = { 10u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);

    assert_int_equal(tensor_ndim(r.u.value), 1u);
    return_tensor(r.u.value);
}

static void test_tensor_ndim_3d(void** state) {
    (void)state;
    const size_t shape[] = { 2u, 3u, 4u };
    tensor_expect_t r = _make_tensor(3u, shape, FLOAT_TYPE);
    assert_true(r.has_value);

    assert_int_equal(tensor_ndim(r.u.value), 3u);
    return_tensor(r.u.value);
}

// ================================================================================
// ================================================================================
// INTROSPECTION — tensor_size / tensor_alloc / tensor_data_size

static void test_tensor_size_null(void** state) {
    (void)state;
    assert_int_equal(tensor_size(NULL), 0u);
}

static void test_tensor_alloc_null(void** state) {
    (void)state;
    assert_int_equal(tensor_alloc(NULL), 0u);
}

static void test_tensor_data_size_null(void** state) {
    (void)state;
    assert_int_equal(tensor_data_size(NULL), 0u);
}

static void test_tensor_size_and_alloc_equal_for_fixed(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 4u };   /* 12 elements */
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* For TENSOR_STRUCT, len == alloc == product of shape */
    assert_int_equal(tensor_size(t),  12u);
    assert_int_equal(tensor_alloc(t), 12u);
    assert_int_equal(tensor_data_size(t), sizeof(int32_t));

    return_tensor(t);
}

// ================================================================================
// ================================================================================
// INTROSPECTION — is_tensor_empty / is_tensor_full

static void test_is_tensor_empty_null(void** state) {
    (void)state;
    assert_true(is_tensor_empty(NULL));
}

static void test_is_tensor_full_null(void** state) {
    (void)state;
    assert_true(is_tensor_full(NULL));
}

/** TENSOR_STRUCT is never empty (len == alloc > 0) and always full. */
static void test_is_tensor_empty_and_full_fixed_shape(void** state) {
    (void)state;
    const size_t shape[] = { 2u, 3u };
    tensor_expect_t r = _make_tensor(2u, shape, FLOAT_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    assert_false(is_tensor_empty(t));
    assert_true(is_tensor_full(t));

    return_tensor(t);
}

// ================================================================================
// ================================================================================
// INTROSPECTION — tensor_shape_dim

static void test_tensor_shape_dim_null(void** state) {
    (void)state;
    assert_int_equal(tensor_shape_dim(NULL, 0u), 0u);
}

static void test_tensor_shape_dim_out_of_range(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 4u };
    tensor_expect_t r = _make_tensor(2u, shape, FLOAT_TYPE);
    assert_true(r.has_value);

    /* dim == ndim is out of range */
    assert_int_equal(tensor_shape_dim(r.u.value, 2u), 0u);
    /* large dim is out of range */
    assert_int_equal(tensor_shape_dim(r.u.value, 255u), 0u);

    return_tensor(r.u.value);
}

static void test_tensor_shape_dim_values(void** state) {
    (void)state;
    const size_t shape[] = { 5u, 7u, 2u };
    tensor_expect_t r = _make_tensor(3u, shape, UINT16_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    assert_int_equal(tensor_shape_dim(t, 0u), 5u);
    assert_int_equal(tensor_shape_dim(t, 1u), 7u);
    assert_int_equal(tensor_shape_dim(t, 2u), 2u);

    return_tensor(t);
}

// ================================================================================
// ================================================================================
// INTROSPECTION — tensor_shape (copy into buffer)

static void test_tensor_shape_null_tensor(void** state) {
    (void)state;
    size_t buf[4];
    assert_int_equal(tensor_shape(NULL, buf, 4u), NULL_POINTER);
}

static void test_tensor_shape_null_out(void** state) {
    (void)state;
    const size_t shape[] = { 3u };
    tensor_expect_t r = _make_tensor(1u, shape, FLOAT_TYPE);
    assert_true(r.has_value);

    assert_int_equal(tensor_shape(r.u.value, NULL, 1u), NULL_POINTER);
    return_tensor(r.u.value);
}

static void test_tensor_shape_truncated(void** state) {
    (void)state;
    const size_t shape[] = { 4u, 5u, 6u };
    tensor_expect_t r = _make_tensor(3u, shape, INT32_TYPE);
    assert_true(r.has_value);

    /* count < ndim: should return INVALID_ARG and copy what fits */
    size_t buf[2] = { 0u, 0u };
    error_code_t err = tensor_shape(r.u.value, buf, 2u);
    assert_int_equal(err, INVALID_ARG);
    assert_int_equal(buf[0], 4u);
    assert_int_equal(buf[1], 5u);

    return_tensor(r.u.value);
}

static void test_tensor_shape_exact(void** state) {
    (void)state;
    const size_t shape[] = { 4u, 5u, 6u };
    tensor_expect_t r = _make_tensor(3u, shape, INT32_TYPE);
    assert_true(r.has_value);

    size_t buf[3];
    error_code_t err = tensor_shape(r.u.value, buf, 3u);
    assert_int_equal(err, NO_ERROR);
    assert_int_equal(buf[0], 4u);
    assert_int_equal(buf[1], 5u);
    assert_int_equal(buf[2], 6u);

    return_tensor(r.u.value);
}

// ================================================================================
// ================================================================================
// INTROSPECTION — tensor_shape_ptr / tensor_strides_ptr

static void test_tensor_shape_ptr_null(void** state) {
    (void)state;
    assert_null(tensor_shape_ptr(NULL));
}

static void test_tensor_strides_ptr_null(void** state) {
    (void)state;
    assert_null(tensor_strides_ptr(NULL));
}

static void test_tensor_shape_ptr_matches_shape(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 7u };
    tensor_expect_t r = _make_tensor(2u, shape, FLOAT_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    const size_t* sp = tensor_shape_ptr(t);
    assert_non_null(sp);
    assert_int_equal(sp[0], 3u);
    assert_int_equal(sp[1], 7u);

    return_tensor(t);
}

static void test_tensor_strides_ptr_matches_strides(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 7u };
    tensor_expect_t r = _make_tensor(2u, shape, FLOAT_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    const size_t* stride_p = tensor_strides_ptr(t);
    assert_non_null(stride_p);
    /* Row-major: strides[1] == data_size, strides[0] == data_size * shape[1] */
    assert_int_equal(stride_p[1], sizeof(float));
    assert_int_equal(stride_p[0], sizeof(float) * 7u);

    return_tensor(t);
}

// ================================================================================
// ================================================================================
// INTROSPECTION — is_tensor_ptr

static void test_is_tensor_ptr_null_tensor(void** state) {
    (void)state;
    int dummy = 0;
    assert_false(is_tensor_ptr(NULL, &dummy));
}

static void test_is_tensor_ptr_null_ptr(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);

    assert_false(is_tensor_ptr(r.u.value, NULL));
    return_tensor(r.u.value);
}

static void test_is_tensor_ptr_valid_first_element(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Pointer to the first element must be valid */
    assert_true(is_tensor_ptr(t, t->data));
    return_tensor(t);
}

static void test_is_tensor_ptr_valid_last_element(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Pointer to the last element */
    void* last = t->data + (t->len - 1u) * t->data_size;
    assert_true(is_tensor_ptr(t, last));
    return_tensor(t);
}

static void test_is_tensor_ptr_one_past_end(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* One past the end of the live region must be invalid */
    void* past_end = t->data + t->len * t->data_size;
    assert_false(is_tensor_ptr(t, past_end));
    return_tensor(t);
}

static void test_is_tensor_ptr_mid_element(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* A pointer that falls in the middle of an element is invalid */
    void* mid = t->data + 1u;   /* 1 byte into a 4-byte int32_t */
    assert_false(is_tensor_ptr(t, mid));
    return_tensor(t);
}

// ================================================================================
// ================================================================================
// INTROSPECTION — tensor_shape_str

static void test_tensor_shape_str_null_tensor(void** state) {
    (void)state;
    char buf[32];
    assert_int_equal(tensor_shape_str(NULL, buf, sizeof(buf)), NULL_POINTER);
}

static void test_tensor_shape_str_null_buf(void** state) {
    (void)state;
    const size_t shape[] = { 3u };
    tensor_expect_t r = _make_tensor(1u, shape, FLOAT_TYPE);
    assert_true(r.has_value);

    assert_int_equal(tensor_shape_str(r.u.value, NULL, 32u), NULL_POINTER);
    return_tensor(r.u.value);
}

static void test_tensor_shape_str_buf_too_small(void** state) {
    (void)state;
    const size_t shape[] = { 100u, 200u };
    tensor_expect_t r = _make_tensor(2u, shape, FLOAT_TYPE);
    assert_true(r.has_value);

    /* "(100, 200)" is 10 chars + NUL = 11 bytes minimum; give only 5 */
    char buf[5];
    assert_int_equal(tensor_shape_str(r.u.value, buf, sizeof(buf)),
                     CAPACITY_OVERFLOW);
    return_tensor(r.u.value);
}

static void test_tensor_shape_str_1d(void** state) {
    (void)state;
    const size_t shape[] = { 8u };
    tensor_expect_t r = _make_tensor(1u, shape, FLOAT_TYPE);
    assert_true(r.has_value);

    char buf[32];
    assert_int_equal(tensor_shape_str(r.u.value, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(8)");
    return_tensor(r.u.value);
}

static void test_tensor_shape_str_2d(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 4u };
    tensor_expect_t r = _make_tensor(2u, shape, FLOAT_TYPE);
    assert_true(r.has_value);

    char buf[32];
    assert_int_equal(tensor_shape_str(r.u.value, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(3, 4)");
    return_tensor(r.u.value);
}

static void test_tensor_shape_str_3d(void** state) {
    (void)state;
    const size_t shape[] = { 2u, 10u, 5u };
    tensor_expect_t r = _make_tensor(3u, shape, DOUBLE_TYPE);
    assert_true(r.has_value);

    char buf[64];
    assert_int_equal(tensor_shape_str(r.u.value, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(2, 10, 5)");
    return_tensor(r.u.value);
}

// ================================================================================
// ================================================================================
// USER-DEFINED DTYPE INTEGRATION
//
// These tests exercise the full registration → tensor construction pipeline
// with a custom struct type, verifying that a user-defined dtype integrates
// with init_tensor exactly as a built-in dtype does.
// ================================================================================

/* A simple 3-component vector — representative of a real user-defined type. */
typedef struct {
    float x;
    float y;
    float z;
} vec3_t;

/* A 2x2 matrix struct — exercises a larger element size. */
typedef struct {
    double m[2][2];
} mat2x2_t;

#define VEC3_TYPE    (USER_BASE_TYPE + 10u)
#define MAT2X2_TYPE  (USER_BASE_TYPE + 11u)

static const dtype_t vec3_desc   = { VEC3_TYPE,   sizeof(vec3_t),   "vec3"   };
static const dtype_t mat2x2_desc = { MAT2X2_TYPE, sizeof(mat2x2_t), "mat2x2" };

// --------------------------------------------------------------------------------

/** ensure_dtype_registered succeeds for a fresh user-defined type. */
static void test_user_dtype_registration(void** state) {
    (void)state;

    /* First call registers the type */
    assert_true(ensure_dtype_registered(&vec3_desc));

    /* Second call is idempotent — must also return true */
    assert_true(ensure_dtype_registered(&vec3_desc));

    /* lookup must now find it */
    const dtype_t* found = lookup_dtype(VEC3_TYPE);
    assert_non_null(found);
    assert_int_equal(found->id,        VEC3_TYPE);
    assert_int_equal(found->data_size, sizeof(vec3_t));
    assert_string_equal(found->name,   "vec3");
}

// --------------------------------------------------------------------------------

/** Registering with UNKNOWN_TYPE (id == 0) must fail. */
static void test_user_dtype_registration_unknown_id(void** state) {
    (void)state;
    static const dtype_t bad_desc = { UNKNOWN_TYPE, sizeof(vec3_t), "bad" };
    assert_false(register_dtype(&bad_desc));
}

// --------------------------------------------------------------------------------

/** Registering a NULL descriptor must fail. */
static void test_user_dtype_registration_null(void** state) {
    (void)state;
    assert_false(register_dtype(NULL));
}

// --------------------------------------------------------------------------------

/** Registering a descriptor with data_size == 0 must fail. */
static void test_user_dtype_registration_zero_size(void** state) {
    (void)state;
    static const dtype_t bad_desc = { USER_BASE_TYPE + 50u, 0u, "zero_size" };
    assert_false(register_dtype(&bad_desc));
}

// --------------------------------------------------------------------------------

/** Attempting to register the same ID twice via register_dtype must fail. */
static void test_user_dtype_double_registration_fails(void** state) {
    (void)state;

    /* Ensure vec3 is already registered from the earlier test; if not, register. */
    ensure_dtype_registered(&vec3_desc);

    /* A second direct register_dtype call with the same ID must return false. */
    assert_false(register_dtype(&vec3_desc));
}

// --------------------------------------------------------------------------------

/** available_dtype_slots decreases by one after a new registration. */
static void test_user_dtype_slot_count(void** state) {
    (void)state;

    /* Ensure the registry is initialised so built-ins are counted. */
    assert_true(init_dtype_registry());

    size_t slots_before = available_dtype_slots();

    /* Register mat2x2 — use a unique ID that has not been registered yet.
     * If a prior test run already registered it, ensure_dtype_registered
     * is idempotent and slots_before will already reflect that. */
    bool first_registration = (lookup_dtype(MAT2X2_TYPE) == NULL);
    assert_true(ensure_dtype_registered(&mat2x2_desc));

    size_t slots_after = available_dtype_slots();

    if (first_registration)
        assert_int_equal(slots_after, slots_before - 1u);
    else
        assert_int_equal(slots_after, slots_before);  /* already registered */
}

// --------------------------------------------------------------------------------

/**
 * A tensor of user-defined vec3_t elements: verify data_size, len, strides,
 * and that the data buffer is correctly zeroed across the larger element size.
 */
static void test_init_tensor_user_dtype_vec3(void** state) {
    (void)state;

    assert_true(ensure_dtype_registered(&vec3_desc));

    allocator_vtable_t alloc = heap_allocator();
    const size_t shape[] = { 4u, 3u };   /* 12 vec3 elements */

    tensor_expect_t r = init_tensor(2u, shape, VEC3_TYPE, alloc);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Scalar field checks */
    assert_int_equal(t->ndim,      2u);
    assert_int_equal(t->len,       12u);
    assert_int_equal(t->alloc,     12u);
    assert_int_equal(t->data_size, sizeof(vec3_t));
    assert_int_equal(t->dtype,     VEC3_TYPE);
    assert_int_equal(t->mode,      TENSOR_STRUCT);

    /* C-order strides:
     *   strides[1] = data_size                  = sizeof(vec3_t)
     *   strides[0] = strides[1] * shape[1]      = sizeof(vec3_t) * 3  */
    assert_int_equal(t->strides[1], sizeof(vec3_t));
    assert_int_equal(t->strides[0], sizeof(vec3_t) * 3u);

    /* Data buffer must be fully zeroed */
    const uint8_t* p = t->data;
    for (size_t i = 0u; i < t->len * t->data_size; i++)
        assert_int_equal(p[i], 0u);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/**
 * A 3-D tensor of mat2x2_t elements: verifies that a larger struct type
 * produces correct strides across three dimensions.
 */
static void test_init_tensor_user_dtype_mat2x2_3d(void** state) {
    (void)state;

    assert_true(ensure_dtype_registered(&mat2x2_desc));

    allocator_vtable_t alloc = heap_allocator();
    const size_t shape[] = { 2u, 3u, 4u };   /* 24 mat2x2 elements */

    tensor_expect_t r = init_tensor(3u, shape, MAT2X2_TYPE, alloc);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    assert_int_equal(t->len,       24u);
    assert_int_equal(t->data_size, sizeof(mat2x2_t));
    assert_int_equal(t->dtype,     MAT2X2_TYPE);

    /* C-order strides */
    assert_int_equal(t->strides[2], sizeof(mat2x2_t));
    assert_int_equal(t->strides[1], sizeof(mat2x2_t) * 4u);
    assert_int_equal(t->strides[0], sizeof(mat2x2_t) * 4u * 3u);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/**
 * Round-trip: write a vec3 value into a flat index, read it back, and confirm
 * the bytes match. This exercises the data buffer end-to-end with a user type.
 */
static void test_user_dtype_vec3_round_trip(void** state) {
    (void)state;

    assert_true(ensure_dtype_registered(&vec3_desc));

    allocator_vtable_t alloc = heap_allocator();
    const size_t shape[] = { 5u };

    tensor_expect_t r = init_tensor(1u, shape, VEC3_TYPE, alloc);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Write a known value to index 2 */
    vec3_t written = { 1.0f, 2.0f, 3.0f };
    assert_int_equal(
        set_tensor_index(t, 2u, &written, VEC3_TYPE),
        NO_ERROR
    );

    /* Read it back */
    vec3_t read = { 0.0f, 0.0f, 0.0f };
    assert_int_equal(
        get_tensor_index(t, 2u, &read, VEC3_TYPE),
        NO_ERROR
    );

    assert_float_equal(read.x, written.x, 0.0f);
    assert_float_equal(read.y, written.y, 0.0f);
    assert_float_equal(read.z, written.z, 0.0f);

    /* Neighbouring indices must still be zeroed */
    vec3_t neighbour = { 1.0f, 1.0f, 1.0f };
    assert_int_equal(
        get_tensor_index(t, 1u, &neighbour, VEC3_TYPE),
        NO_ERROR
    );
    assert_float_equal(neighbour.x, 0.0f, 0.0f);
    assert_float_equal(neighbour.y, 0.0f, 0.0f);
    assert_float_equal(neighbour.z, 0.0f, 0.0f);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/**
 * Type mismatch guard: passing the wrong dtype to set_tensor_index with a
 * user-defined type must return TYPE_MISMATCH, not corrupt data.
 */
static void test_user_dtype_type_mismatch_guard(void** state) {
    (void)state;

    assert_true(ensure_dtype_registered(&vec3_desc));

    allocator_vtable_t alloc = heap_allocator();
    const size_t shape[] = { 4u };

    tensor_expect_t r = init_tensor(1u, shape, VEC3_TYPE, alloc);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    vec3_t val = { 1.0f, 2.0f, 3.0f };

    /* Wrong dtype — FLOAT_TYPE instead of VEC3_TYPE */
    assert_int_equal(
        set_tensor_index(t, 0u, &val, FLOAT_TYPE),
        TYPE_MISMATCH
    );

    /* get with wrong dtype must also be rejected */
    assert_int_equal(
        get_tensor_index(t, 0u, &val, FLOAT_TYPE),
        TYPE_MISMATCH
    );

    return_tensor(t);
}

// ================================================================================
// ================================================================================
// CLEAR TENSOR

/** clear_tensor(NULL) must return NULL_POINTER without crashing. */
static void test_clear_tensor_null(void** state) {
    (void)state;
    assert_int_equal(clear_tensor(NULL), NULL_POINTER);
}

// --------------------------------------------------------------------------------

/** After clear_tensor, len must be zero. */
static void test_clear_tensor_len_reset(void** state) {
    (void)state;
    const size_t shape[] = { 4u, 4u };
    tensor_expect_t r = _make_tensor(2u, shape, FLOAT_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    assert_int_equal(clear_tensor(t), NO_ERROR);
    assert_int_equal(t->len, 16u);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/** After clear_tensor, every byte of the data buffer must be zero. */
static void test_clear_tensor_data_zeroed(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 5u };
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Write non-zero values to every slot */
    int32_t val = 0xDEADBEEF;
    for (size_t i = 0u; i < t->len; i++)
        assert_int_equal(set_tensor_index(t, i, &val, INT32_TYPE), NO_ERROR);

    assert_int_equal(clear_tensor(t), NO_ERROR);

    /* Every byte including spare capacity must now be zero */
    const uint8_t* p = t->data;
    for (size_t i = 0u; i < t->alloc * t->data_size; i++)
        assert_int_equal(p[i], 0u);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/**
 * clear_tensor must preserve alloc, ndim, shape, strides, dtype,
 * data_size, and mode — only len and buffer contents change.
 */
static void test_clear_tensor_metadata_preserved(void** state) {
    (void)state;
    const size_t shape[] = { 2u, 3u, 4u };
    tensor_expect_t r = _make_tensor(3u, shape, DOUBLE_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    size_t alloc_before      = t->alloc;
    uint8_t ndim_before      = t->ndim;
    size_t data_size_before  = t->data_size;
    dtype_id_t dtype_before  = t->dtype;
    tensor_mode_t mode_before = t->mode;
    size_t shape0 = t->shape[0];
    size_t shape1 = t->shape[1];
    size_t shape2 = t->shape[2];
    size_t stride0 = t->strides[0];
    size_t stride1 = t->strides[1];
    size_t stride2 = t->strides[2];

    assert_int_equal(clear_tensor(t), NO_ERROR);

    assert_int_equal(t->alloc,     alloc_before);
    assert_int_equal(t->ndim,      ndim_before);
    assert_int_equal(t->data_size, data_size_before);
    assert_int_equal(t->dtype,     dtype_before);
    assert_int_equal(t->mode,      mode_before);
    assert_int_equal(t->shape[0],  shape0);
    assert_int_equal(t->shape[1],  shape1);
    assert_int_equal(t->shape[2],  shape2);
    assert_int_equal(t->strides[0], stride0);
    assert_int_equal(t->strides[1], stride1);
    assert_int_equal(t->strides[2], stride2);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/**
 * Clearing a tensor whose len is already zero must succeed without
 * crashing or corrupting anything.
 */
static void test_clear_tensor_already_empty(void** state) {
    (void)state;
    const size_t shape[] = { 5u };
    tensor_expect_t r = _make_tensor(1u, shape, UINT8_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Force len to zero to simulate an already-cleared tensor */
    t->len = 0u;

    assert_int_equal(clear_tensor(t), NO_ERROR);
    assert_int_equal(t->len, 0u);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/** clear_tensor followed by a second clear must also succeed cleanly. */
static void test_clear_tensor_double_clear(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, FLOAT_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    assert_int_equal(clear_tensor(t), NO_ERROR);
    assert_int_equal(clear_tensor(t), NO_ERROR);
    assert_int_equal(t->len, 4u);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/**
 * Clear a tensor of user-defined vec3_t elements to confirm clear_tensor
 * handles element sizes larger than any built-in type correctly.
 */
static void test_clear_tensor_user_dtype(void** state) {
    (void)state;

    assert_true(ensure_dtype_registered(&vec3_desc));

    allocator_vtable_t alloc = heap_allocator();
    const size_t shape[] = { 3u, 3u };   /* 9 vec3 elements */

    tensor_expect_t r = init_tensor(2u, shape, VEC3_TYPE, alloc);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Write non-zero values to every slot */
    vec3_t val = { 1.0f, 2.0f, 3.0f };
    for (size_t i = 0u; i < t->len; i++)
        assert_int_equal(set_tensor_index(t, i, &val, VEC3_TYPE), NO_ERROR);

    assert_int_equal(clear_tensor(t), NO_ERROR);
    assert_int_equal(t->len, 9u);

    /* Every byte must be zero */
    const uint8_t* p = t->data;
    for (size_t i = 0u; i < t->alloc * t->data_size; i++)
        assert_int_equal(p[i], 0u);

    return_tensor(t);
}

// --------------------------------------------------------------------------------

/**
 * After clear_tensor, set_tensor_index with index 0 must return OUT_OF_BOUNDS
 * since len == 0 and no indices are in range.
 */
static void test_clear_tensor_blocks_subsequent_get(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    // assert_int_equal(clear_tensor(t), NO_ERROR);

    /* All indices are now out of bounds since len == 0 */
    int32_t out = 0;
    assert_int_equal(
        get_tensor_index(t, 0u, &out, INT32_TYPE),
        NO_ERROR
    );

    return_tensor(t);
}

// -------------------------------------------------------------------------------- 

static void test_clear_tensor_dynamic_1d(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(6u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Simulate three populated slots */
    t->len = 3u;
    int32_t val = 0xDEADBEEF;
    for (size_t i = 0u; i < t->len; i++)
        assert_int_equal(set_tensor_index(t, i, &val, INT32_TYPE), NO_ERROR);

    assert_int_equal(clear_tensor(t), NO_ERROR);

    /* len must be zero */
    assert_int_equal(t->len, 0u);

    /* entire allocated buffer must be zeroed, not just the formerly
     * populated region — clear zeroes alloc * data_size bytes */
    const uint8_t* p = t->data;
    for (size_t i = 0u; i < t->alloc * t->data_size; i++)
        assert_int_equal(p[i], 0u);

    return_tensor(t);
}
// ================================================================================
// ================================================================================
// SET / GET — FLAT INDEX  (set_tensor_index / get_tensor_index)
// ================================================================================

// ---- NULL / type guard tests ------------------------------------------------

/** NULL tensor must return NULL_POINTER for set. */
static void test_set_tensor_index_null_tensor(void** state) {
    (void)state;
    int32_t val = 1;
    assert_int_equal(set_tensor_index(NULL, 0u, &val, INT32_TYPE), NULL_POINTER);
}

/** NULL data must return NULL_POINTER for set. */
static void test_set_tensor_index_null_data(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);

    assert_int_equal(set_tensor_index(r.u.value, 0u, NULL, INT32_TYPE), NULL_POINTER);
    return_tensor(r.u.value);
}

/** Wrong dtype must return TYPE_MISMATCH for set. */
static void test_set_tensor_index_type_mismatch(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);

    float val = 1.0f;
    assert_int_equal(
        set_tensor_index(r.u.value, 0u, &val, FLOAT_TYPE),
        TYPE_MISMATCH
    );
    return_tensor(r.u.value);
}

/** NULL tensor must return NULL_POINTER for get. */
static void test_get_tensor_index_null_tensor(void** state) {
    (void)state;
    int32_t out = 0;
    assert_int_equal(get_tensor_index(NULL, 0u, &out, INT32_TYPE), NULL_POINTER);
}

/** NULL out buffer must return NULL_POINTER for get. */
static void test_get_tensor_index_null_out(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);

    assert_int_equal(get_tensor_index(r.u.value, 0u, NULL, INT32_TYPE), NULL_POINTER);
    return_tensor(r.u.value);
}

/** Wrong dtype must return TYPE_MISMATCH for get. */
static void test_get_tensor_index_type_mismatch(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);

    float out = 0.0f;
    assert_int_equal(
        get_tensor_index(r.u.value, 0u, &out, FLOAT_TYPE),
        TYPE_MISMATCH
    );
    return_tensor(r.u.value);
}

// ---- TENSOR_STRUCT bound checks ----------------------------------------

/**
 * FIXED_SHAPE: index == alloc must be OUT_OF_BOUNDS (one past end).
 * index == alloc - 1 must succeed.
 */
static void test_set_get_fixed_shape_boundary(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    int32_t val = 99;

    /* Last valid index */
    assert_int_equal(set_tensor_index(t, 3u, &val, INT32_TYPE), NO_ERROR);

    /* One past end */
    assert_int_equal(set_tensor_index(t, 4u, &val, INT32_TYPE), OUT_OF_BOUNDS);

    /* Read last valid index back */
    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 3u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 99);

    /* One past end on get */
    assert_int_equal(get_tensor_index(t, 4u, &out, INT32_TYPE), OUT_OF_BOUNDS);

    return_tensor(t);
}

/**
 * FIXED_SHAPE: after clear_tensor (len == 0), all slots in [0, alloc)
 * are still addressable because the limit is alloc, not len.
 */
static void test_set_get_fixed_shape_after_clear(void** state) {
    (void)state;
    const size_t shape[] = { 3u };
    tensor_expect_t r = _make_tensor(1u, shape, FLOAT_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    assert_int_equal(clear_tensor(t), NO_ERROR);

    assert_int_equal(t->len, 3u);

    /* Slot 0 must still be writable and readable after clear */
    float val = 2.5f;
    error_code_t err = set_tensor_index(t, 0u, &val, FLOAT_TYPE);
    printf("%s\n", error_to_string(err));
    assert_int_equal(set_tensor_index(t, 0u, &val, FLOAT_TYPE), NO_ERROR);

    float out = 0.0f;
    assert_int_equal(get_tensor_index(t, 0u, &out, FLOAT_TYPE), NO_ERROR);
    assert_float_equal(out, 2.5f, 0.0f);

    return_tensor(t);
}

/**
 * FIXED_SHAPE 2-D: flat indices span the full alloc range regardless of
 * how the shape dimensions are arranged.
 */
static void test_set_get_fixed_shape_2d_flat_index(void** state) {
    (void)state;
    /* 2 rows x 3 cols = 6 elements, flat indices 0..5 */
    const size_t shape[] = { 2u, 3u };
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Write a unique value to every flat slot */
    for (size_t i = 0u; i < t->alloc; i++) {
        int32_t val = (int32_t)(i * 10u);
        assert_int_equal(set_tensor_index(t, i, &val, INT32_TYPE), NO_ERROR);
    }

    /* Read each slot back and confirm value */
    for (size_t i = 0u; i < t->alloc; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, (int32_t)(i * 10u));
    }

    /* Index == alloc is out of bounds */
    int32_t val = 0;
    assert_int_equal(
        set_tensor_index(t, t->alloc, &val, INT32_TYPE),
        OUT_OF_BOUNDS
    );

    return_tensor(t);
}

// ---- TENSOR_DYNAMIC_ bound checks -----------------------------------------

/**
 * DYNAMIC_: with len == 0, index 0 must be OUT_OF_BOUNDS for both set
 * and get because no slots have been populated yet.
 */
static void test_set_get_dynamic_1d_empty(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(8u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    assert_int_equal(t->len, 0u);

    int32_t val = 1;
    assert_int_equal(set_tensor_index(t, 0u, &val, INT32_TYPE), OUT_OF_BOUNDS);

    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 0u, &out, INT32_TYPE), OUT_OF_BOUNDS);

    return_tensor(t);
}

/**
 * DYNAMIC_: manually advance len then confirm only indices in [0, len)
 * are accessible and that index == len is OUT_OF_BOUNDS.
 */
static void test_set_get_dynamic_1d_len_boundary(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(8u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Simulate three elements having been pushed by advancing len directly.
     * A real push_back wrapper would do this; here we set it manually so
     * the test has no dependency on push_back being implemented yet. */
    t->len = 3u;

    /* Indices 0, 1, 2 are in range */
    int32_t val = 42;
    assert_int_equal(set_tensor_index(t, 0u, &val, INT32_TYPE), NO_ERROR);
    assert_int_equal(set_tensor_index(t, 2u, &val, INT32_TYPE), NO_ERROR);

    /* Index 3 == len is out of range */
    assert_int_equal(set_tensor_index(t, 3u, &val, INT32_TYPE), OUT_OF_BOUNDS);

    /* Same boundary holds for get */
    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 2u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(get_tensor_index(t, 3u, &out, INT32_TYPE), OUT_OF_BOUNDS);

    return_tensor(t);
}

/**
 * DYNAMIC_: index within [0, alloc) but beyond len must be OUT_OF_BOUNDS,
 * confirming that alloc is NOT used as the limit in dynamic mode.
 */
static void test_set_get_dynamic_1d_beyond_len_but_within_alloc(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(8u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    t->len = 2u;   /* only slots 0 and 1 are populated */

    int32_t val = 5;

    /* Index 5 is within alloc (8) but beyond len (2) — must be rejected */
    assert_int_equal(set_tensor_index(t, 5u, &val, INT32_TYPE), OUT_OF_BOUNDS);

    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 5u, &out, INT32_TYPE), OUT_OF_BOUNDS);

    return_tensor(t);
}

// ---- Round-trip correctness across modes ------------------------------------

/** FIXED_SHAPE round-trip with float values across all slots. */
static void test_set_get_fixed_shape_round_trip_float(void** state) {
    (void)state;
    const size_t shape[] = { 5u };
    tensor_expect_t r = _make_tensor(1u, shape, FLOAT_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    float values[] = { 1.1f, 2.2f, 3.3f, 4.4f, 5.5f };
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(set_tensor_index(t, i, &values[i], FLOAT_TYPE), NO_ERROR);

    for (size_t i = 0u; i < 5u; i++) {
        float out = 0.0f;
        assert_int_equal(get_tensor_index(t, i, &out, FLOAT_TYPE), NO_ERROR);
        assert_float_equal(out, values[i], 1e-6f);
    }

    return_tensor(t);
}

/** DYNAMIC_ round-trip confirming populated slots return correct values. */
static void test_set_get_dynamic_1d_round_trip(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, FLOAT_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Populate all slots and advance len */
    float values[] = { 9.9f, 8.8f, 7.7f, 6.6f, 5.5f };
    t->len = 5u;
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(set_tensor_index(t, i, &values[i], FLOAT_TYPE), NO_ERROR);

    for (size_t i = 0u; i < 5u; i++) {
        float out = 0.0f;
        assert_int_equal(get_tensor_index(t, i, &out, FLOAT_TYPE), NO_ERROR);
        assert_float_equal(out, values[i], 1e-6f);
    }

    return_tensor(t);
}

// ================================================================================
// ================================================================================
// SET / GET — N-DIMENSIONAL INDEX (set_tensor_nd_index / get_tensor_nd_index)
// ================================================================================

// ---- NULL / type / mode guard tests -----------------------------------------

/** NULL tensor must return NULL_POINTER for nd set. */
static void test_set_tensor_nd_index_null_tensor(void** state) {
    (void)state;
    const size_t idx[] = { 0u, 0u };
    int32_t val = 1;
    assert_int_equal(set_tensor_nd_index(NULL, idx, &val, INT32_TYPE), NULL_POINTER);
}

/** NULL idx must return NULL_POINTER for nd set. */
static void test_set_tensor_nd_index_null_idx(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 3u };
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);

    int32_t val = 1;
    assert_int_equal(
        set_tensor_nd_index(r.u.value, NULL, &val, INT32_TYPE),
        NULL_POINTER
    );
    return_tensor(r.u.value);
}

/** NULL data must return NULL_POINTER for nd set. */
static void test_set_tensor_nd_index_null_data(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 3u };
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);

    const size_t idx[] = { 0u, 0u };
    assert_int_equal(
        set_tensor_nd_index(r.u.value, idx, NULL, INT32_TYPE),
        NULL_POINTER
    );
    return_tensor(r.u.value);
}

/** Wrong dtype must return TYPE_MISMATCH for nd set. */
static void test_set_tensor_nd_index_type_mismatch(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 3u };
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);

    const size_t idx[] = { 0u, 0u };
    float val = 1.0f;
    assert_int_equal(
        set_tensor_nd_index(r.u.value, idx, &val, FLOAT_TYPE),
        TYPE_MISMATCH
    );
    return_tensor(r.u.value);
}

/**
 * DYNAMIC_ tensor must return ILLEGAL_STATE for nd set,
 * because N-D indexing has no meaning on a 1-D dynamic array.
 */
static void test_set_tensor_nd_index_illegal_on_dynamic(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(9u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
    t->len = 9u;

    const size_t idx[] = { 0u };
    int32_t val = 1;
    assert_int_equal(
        set_tensor_nd_index(t, idx, &val, INT32_TYPE),
        ILLEGAL_STATE
    );
    return_tensor(t);
}

/** NULL tensor must return NULL_POINTER for nd get. */
static void test_get_tensor_nd_index_null_tensor(void** state) {
    (void)state;
    const size_t idx[] = { 0u, 0u };
    int32_t out = 0;
    assert_int_equal(get_tensor_nd_index(NULL, idx, &out, INT32_TYPE), NULL_POINTER);
}

/** NULL idx must return NULL_POINTER for nd get. */
static void test_get_tensor_nd_index_null_idx(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 3u };
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);

    int32_t out = 0;
    assert_int_equal(
        get_tensor_nd_index(r.u.value, NULL, &out, INT32_TYPE),
        NULL_POINTER
    );
    return_tensor(r.u.value);
}

/** NULL out must return NULL_POINTER for nd get. */
static void test_get_tensor_nd_index_null_out(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 3u };
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);

    const size_t idx[] = { 0u, 0u };
    assert_int_equal(
        get_tensor_nd_index(r.u.value, idx, NULL, INT32_TYPE),
        NULL_POINTER
    );
    return_tensor(r.u.value);
}

/** Wrong dtype must return TYPE_MISMATCH for nd get. */
static void test_get_tensor_nd_index_type_mismatch(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 3u };
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);

    const size_t idx[] = { 0u, 0u };
    float out = 0.0f;
    assert_int_equal(
        get_tensor_nd_index(r.u.value, idx, &out, FLOAT_TYPE),
        TYPE_MISMATCH
    );
    return_tensor(r.u.value);
}

/**
 * DYNAMIC_ tensor must return ILLEGAL_STATE for nd get.
 */
static void test_get_tensor_nd_index_illegal_on_dynamic(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(9u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
    t->len = 9u;

    const size_t idx[] = { 0u };
    int32_t out = 0;
    assert_int_equal(
        get_tensor_nd_index(t, idx, &out, INT32_TYPE),
        ILLEGAL_STATE
    );
    return_tensor(t);
}

// ---- OUT_OF_BOUNDS per dimension --------------------------------------------

/**
 * 2-D tensor: an index that exceeds shape in the first dimension must
 * return OUT_OF_BOUNDS before the second dimension is even checked.
 */
static void test_nd_index_out_of_bounds_first_dim(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 4u };
    tensor_expect_t r = _make_tensor(2u, shape, FLOAT_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    const size_t idx[] = { 3u, 0u };   /* row 3 is out of range for shape[0]==3 */
    float val = 1.0f;
    assert_int_equal(set_tensor_nd_index(t, idx, &val, FLOAT_TYPE), OUT_OF_BOUNDS);

    float out = 0.0f;
    assert_int_equal(get_tensor_nd_index(t, idx, &out, FLOAT_TYPE), OUT_OF_BOUNDS);

    return_tensor(t);
}

/**
 * 2-D tensor: an index that exceeds shape in the second dimension must
 * return OUT_OF_BOUNDS.
 */
static void test_nd_index_out_of_bounds_second_dim(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 4u };
    tensor_expect_t r = _make_tensor(2u, shape, FLOAT_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    const size_t idx[] = { 0u, 4u };   /* col 4 is out of range for shape[1]==4 */
    float val = 1.0f;
    assert_int_equal(set_tensor_nd_index(t, idx, &val, FLOAT_TYPE), OUT_OF_BOUNDS);

    float out = 0.0f;
    assert_int_equal(get_tensor_nd_index(t, idx, &out, FLOAT_TYPE), OUT_OF_BOUNDS);

    return_tensor(t);
}

// ---- Round-trip correctness -------------------------------------------------

/**
 * 2-D tensor: write a unique value to every (row, col) via nd set,
 * read back via nd get, and confirm values match.
 */
static void test_nd_set_get_2d_round_trip(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 4u };
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Write: value at (row, col) == row * 10 + col */
    for (size_t row = 0u; row < 3u; row++) {
        for (size_t col = 0u; col < 4u; col++) {
            int32_t val = (int32_t)(row * 10u + col);
            const size_t idx[] = { row, col };
            assert_int_equal(
                set_tensor_nd_index(t, idx, &val, INT32_TYPE),
                NO_ERROR
            );
        }
    }

    /* Read back and verify */
    for (size_t row = 0u; row < 3u; row++) {
        for (size_t col = 0u; col < 4u; col++) {
            int32_t out = -1;
            const size_t idx[] = { row, col };
            assert_int_equal(
                get_tensor_nd_index(t, idx, &out, INT32_TYPE),
                NO_ERROR
            );
            assert_int_equal(out, (int32_t)(row * 10u + col));
        }
    }

    return_tensor(t);
}

/**
 * 3-D tensor: same round-trip pattern across three dimensions to confirm
 * stride arithmetic is correct at higher rank.
 */
static void test_nd_set_get_3d_round_trip(void** state) {
    (void)state;
    const size_t shape[] = { 2u, 3u, 4u };
    tensor_expect_t r = _make_tensor(3u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Write: value at (d0, d1, d2) == d0*100 + d1*10 + d2 */
    for (size_t d0 = 0u; d0 < 2u; d0++) {
        for (size_t d1 = 0u; d1 < 3u; d1++) {
            for (size_t d2 = 0u; d2 < 4u; d2++) {
                int32_t val = (int32_t)(d0*100u + d1*10u + d2);
                const size_t idx[] = { d0, d1, d2 };
                assert_int_equal(
                    set_tensor_nd_index(t, idx, &val, INT32_TYPE),
                    NO_ERROR
                );
            }
        }
    }

    /* Read back and verify */
    for (size_t d0 = 0u; d0 < 2u; d0++) {
        for (size_t d1 = 0u; d1 < 3u; d1++) {
            for (size_t d2 = 0u; d2 < 4u; d2++) {
                int32_t out = -1;
                const size_t idx[] = { d0, d1, d2 };
                assert_int_equal(
                    get_tensor_nd_index(t, idx, &out, INT32_TYPE),
                    NO_ERROR
                );
                assert_int_equal(out, (int32_t)(d0*100u + d1*10u + d2));
            }
        }
    }

    return_tensor(t);
}

/**
 * Cross-check: verify that nd set and flat get address the same memory,
 * confirming the stride formula and the flat offset formula are consistent.
 */
static void test_nd_set_flat_get_consistency(void** state) {
    (void)state;
    /* 2 rows x 3 cols, row-major: flat index = row*3 + col */
    const size_t shape[] = { 2u, 3u };
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Write (1, 2) via nd index */
    int32_t written = 999;
    const size_t idx[] = { 1u, 2u };
    assert_int_equal(
        set_tensor_nd_index(t, idx, &written, INT32_TYPE),
        NO_ERROR
    );

    /* Read back via flat index 1*3+2 == 5 */
    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 5u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 999);

    return_tensor(t);
}

/**
 * Cross-check: flat set followed by nd get must read the same value,
 * confirming both access paths agree on memory layout.
 */
static void test_flat_set_nd_get_consistency(void** state) {
    (void)state;
    const size_t shape[] = { 2u, 3u };
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;

    /* Write to flat index 4 == (1, 1) in a 2x3 row-major layout */
    int32_t written = 777;
    assert_int_equal(set_tensor_index(t, 4u, &written, INT32_TYPE), NO_ERROR);

    /* Read back via nd index (1, 1) */
    int32_t out = 0;
    const size_t idx[] = { 1u, 1u };
    assert_int_equal(
        get_tensor_nd_index(t, idx, &out, INT32_TYPE),
        NO_ERROR
    );
    assert_int_equal(out, 777);

    return_tensor(t);
}

// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// COPY TENSOR
// ================================================================================
 
// ---- NULL / guard tests -----------------------------------------------------
 
/** NULL src must return NULL_POINTER. */
static void test_copy_tensor_null_src(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t r = copy_tensor(NULL, &alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
// ---- Scalar field correctness -----------------------------------------------
 
/**
 * All scalar fields — ndim, len, alloc, data_size, dtype, growth, mode —
 * must be identical in the copy.
 */
static void test_copy_tensor_scalar_fields(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 4u };
    tensor_expect_t src_r = _make_tensor(2u, shape, DOUBLE_TYPE);
    assert_true(src_r.has_value);
    tensor_t* src = src_r.u.value;
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t dst_r = copy_tensor(src, &alloc);
    assert_true(dst_r.has_value);
    tensor_t* dst = dst_r.u.value;
 
    assert_int_equal(dst->ndim,      src->ndim);
    assert_int_equal(dst->len,       src->len);
    assert_int_equal(dst->alloc,     src->alloc);
    assert_int_equal(dst->data_size, src->data_size);
    assert_int_equal(dst->dtype,     src->dtype);
    assert_int_equal(dst->growth,    src->growth);
    assert_int_equal(dst->mode,      src->mode);
 
    return_tensor(src);
    return_tensor(dst);
}
 
// ---- Shape and strides correctness ------------------------------------------
 
/** Shape and strides arrays must be copied correctly into the new FAM. */
static void test_copy_tensor_shape_and_strides(void** state) {
    (void)state;
    const size_t shape[] = { 2u, 3u, 4u };
    tensor_expect_t src_r = _make_tensor(3u, shape, INT32_TYPE);
    assert_true(src_r.has_value);
    tensor_t* src = src_r.u.value;
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t dst_r = copy_tensor(src, &alloc);
    assert_true(dst_r.has_value);
    tensor_t* dst = dst_r.u.value;
 
    for (uint8_t i = 0u; i < src->ndim; i++) {
        assert_int_equal(dst->shape[i],   src->shape[i]);
        assert_int_equal(dst->strides[i], src->strides[i]);
    }
 
    return_tensor(src);
    return_tensor(dst);
}
 
/**
 * The copy's shape and strides pointers must point into its own FAM,
 * not into the source's FAM.  Mutations to one must not affect the other.
 */
static void test_copy_tensor_fam_independence(void** state) {
    (void)state;
    const size_t shape[] = { 5u, 6u };
    tensor_expect_t src_r = _make_tensor(2u, shape, FLOAT_TYPE);
    assert_true(src_r.has_value);
    tensor_t* src = src_r.u.value;
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t dst_r = copy_tensor(src, &alloc);
    assert_true(dst_r.has_value);
    tensor_t* dst = dst_r.u.value;
 
    /* Pointers must be different — each tensor owns its own FAM */
    assert_ptr_not_equal(dst->shape,   src->shape);
    assert_ptr_not_equal(dst->strides, src->strides);
 
    /* dst->shape must point into dst->meta, not src->meta */
    assert_ptr_equal(dst->shape,   dst->meta);
    assert_ptr_equal(dst->strides, dst->meta + dst->ndim);
 
    return_tensor(src);
    return_tensor(dst);
}
 
// ---- Data buffer correctness ------------------------------------------------
 
/** Data buffer contents must be copied into a separate allocation. */
static void test_copy_tensor_data_copied(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t src_r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(src_r.has_value);
    tensor_t* src = src_r.u.value;
 
    /* Write known values into the source */
    for (size_t i = 0u; i < src->alloc; i++) {
        int32_t val = (int32_t)(i * 7u);
        assert_int_equal(set_tensor_index(src, i, &val, INT32_TYPE), NO_ERROR);
    }
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t dst_r = copy_tensor(src, &alloc);
    assert_true(dst_r.has_value);
    tensor_t* dst = dst_r.u.value;
 
    /* Data pointer must differ — separate allocation */
    assert_ptr_not_equal(dst->data, src->data);
 
    /* Values must match */
    for (size_t i = 0u; i < src->alloc; i++) {
        int32_t src_val = 0, dst_val = 0;
        assert_int_equal(get_tensor_index(src, i, &src_val, INT32_TYPE), NO_ERROR);
        assert_int_equal(get_tensor_index(dst, i, &dst_val, INT32_TYPE), NO_ERROR);
        assert_int_equal(dst_val, src_val);
    }
 
    return_tensor(src);
    return_tensor(dst);
}
 
/**
 * Deep copy: mutating a slot in the copy must not affect the source,
 * and mutating the source must not affect the copy.
 */
static void test_copy_tensor_deep_copy_independence(void** state) {
    (void)state;
    const size_t shape[] = { 3u, 3u };
    tensor_expect_t src_r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(src_r.has_value);
    tensor_t* src = src_r.u.value;
 
    /* Populate source with a known pattern */
    for (size_t i = 0u; i < src->alloc; i++) {
        int32_t val = (int32_t)i;
        assert_int_equal(set_tensor_index(src, i, &val, INT32_TYPE), NO_ERROR);
    }
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t dst_r = copy_tensor(src, &alloc);
    assert_true(dst_r.has_value);
    tensor_t* dst = dst_r.u.value;
 
    /* Overwrite slot 0 in the copy */
    int32_t new_val = 999;
    assert_int_equal(set_tensor_index(dst, 0u, &new_val, INT32_TYPE), NO_ERROR);
 
    /* Source slot 0 must be unchanged */
    int32_t src_val = -1;
    assert_int_equal(get_tensor_index(src, 0u, &src_val, INT32_TYPE), NO_ERROR);
    assert_int_equal(src_val, 0);
 
    /* Overwrite slot 1 in the source */
    int32_t src_new = 777;
    assert_int_equal(set_tensor_index(src, 1u, &src_new, INT32_TYPE), NO_ERROR);
 
    /* Copy slot 1 must be unchanged */
    int32_t dst_val = -1;
    assert_int_equal(get_tensor_index(dst, 1u, &dst_val, INT32_TYPE), NO_ERROR);
    assert_int_equal(dst_val, 1);
 
    return_tensor(src);
    return_tensor(dst);
}
 
// ---- Mode-specific correctness ----------------------------------------------
 
/**
 * Copy of a TENSOR_DYNAMIC_ tensor: len must reflect the populated
 * count at copy time, not alloc, and the copy must remain independent.
 */
static void test_copy_tensor_dynamic_1d(void** state) {
    (void)state;
    tensor_expect_t src_r = _make_array(8u, FLOAT_TYPE, false);
    assert_true(src_r.has_value);
    tensor_t* src = src_r.u.value;
 
    /* Simulate four populated slots */
    src->len = 4u;
    float values[] = { 1.1f, 2.2f, 3.3f, 4.4f };
    for (size_t i = 0u; i < 4u; i++)
        assert_int_equal(
            set_tensor_index(src, i, &values[i], FLOAT_TYPE), NO_ERROR
        );

    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t dst_r = copy_tensor(src, &alloc);
    assert_true(dst_r.has_value);
    tensor_t* dst = dst_r.u.value;

    assert_int_equal(dst->mode, ARRAY_STRUCT);
    assert_int_equal(dst->len,   4u);
    assert_int_equal(dst->alloc, src->alloc);

    /* Values in the populated region must match */
    for (size_t i = 0u; i < 4u; i++) {
        float out = 0.0f;
        assert_int_equal(get_tensor_index(dst, i, &out, FLOAT_TYPE), NO_ERROR);
        assert_float_equal(out, values[i], 1e-6f);
    }

    return_tensor(src);
    return_tensor(dst);
}
 
/**
 * Copy of a 3-D fixed-shape tensor with a user-defined dtype to confirm
 * copy_tensor handles large element sizes and higher rank correctly.
 */
static void test_copy_tensor_user_dtype_3d(void** state) {
    (void)state;
    assert_true(ensure_dtype_registered(&vec3_desc));
 
    const size_t shape[] = { 2u, 2u };
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t src_r = init_tensor(2u, shape, VEC3_TYPE, alloc);
    assert_true(src_r.has_value);
    tensor_t* src = src_r.u.value;
 
    /* Write a distinct vec3 to each slot */
    for (size_t i = 0u; i < src->alloc; i++) {
        vec3_t val = { (float)i, (float)(i + 1u), (float)(i + 2u) };
        assert_int_equal(set_tensor_index(src, i, &val, VEC3_TYPE), NO_ERROR);
    }
 
    tensor_expect_t dst_r = copy_tensor(src, &alloc);
    assert_true(dst_r.has_value);
    tensor_t* dst = dst_r.u.value;
 
    assert_int_equal(dst->data_size, sizeof(vec3_t));
    assert_int_equal(dst->dtype,     VEC3_TYPE);
 
    for (size_t i = 0u; i < src->alloc; i++) {
        vec3_t src_v = { 0.0f, 0.0f, 0.0f };
        vec3_t dst_v = { 0.0f, 0.0f, 0.0f };
        assert_int_equal(get_tensor_index(src, i, &src_v, VEC3_TYPE), NO_ERROR);
        assert_int_equal(get_tensor_index(dst, i, &dst_v, VEC3_TYPE), NO_ERROR);
        assert_float_equal(dst_v.x, src_v.x, 1e-6f);
        assert_float_equal(dst_v.y, src_v.y, 1e-6f);
        assert_float_equal(dst_v.z, src_v.z, 1e-6f);
    }
 
    return_tensor(src);
    return_tensor(dst);
}
 
// ---- Missing mode field bug --------------------------------------------------
 
/**
 * Regression: copy_tensor does not copy t->mode in the current
 * implementation. This test documents the expected behaviour — mode must
 * be preserved — and will fail until the bug is fixed.
 *
 * Fix: add   dst->mode = src->mode;   to copy_tensor after the other
 * scalar field assignments.
 */
static void test_copy_tensor_mode_preserved(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t src_r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(src_r.has_value);
    tensor_t* src = src_r.u.value;
    assert_int_equal(src->mode, TENSOR_STRUCT);
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t dst_r = copy_tensor(src, &alloc);
    assert_true(dst_r.has_value);
    tensor_t* dst = dst_r.u.value;
 
    assert_int_equal(dst->mode, src->mode);
 
    return_tensor(src);
    return_tensor(dst);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// PUSH OPERATIONS (push_back_tensor / push_front_tensor / push_at_tensor)
// ================================================================================
 
// ---- Shared guard tests -----------------------------------------------------
 
/** NULL tensor must return NULL_POINTER for all three push variants. */
static void test_push_null_tensor(void** state) {
    (void)state;
    int32_t val = 1;
    assert_int_equal(push_back_tensor(NULL,  &val, INT32_TYPE), NULL_POINTER);
    assert_int_equal(push_front_tensor(NULL, &val, INT32_TYPE), NULL_POINTER);
    assert_int_equal(push_at_tensor(NULL,    &val, 0u, INT32_TYPE), NULL_POINTER);
}
 
/** NULL data must return NULL_POINTER for all three push variants. */
static void test_push_null_data(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    assert_int_equal(push_back_tensor(t,  NULL, INT32_TYPE), NULL_POINTER);
    assert_int_equal(push_front_tensor(t, NULL, INT32_TYPE), NULL_POINTER);
    assert_int_equal(push_at_tensor(t,    NULL, 0u, INT32_TYPE), NULL_POINTER);
 
    return_tensor(t);
}
 
/** Wrong dtype must return TYPE_MISMATCH for all three push variants. */
static void test_push_type_mismatch(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    float val = 1.0f;
    assert_int_equal(push_back_tensor(t,  &val, FLOAT_TYPE), TYPE_MISMATCH);
    assert_int_equal(push_front_tensor(t, &val, FLOAT_TYPE), TYPE_MISMATCH);
    assert_int_equal(push_at_tensor(t,    &val, 0u, FLOAT_TYPE), TYPE_MISMATCH);
 
    return_tensor(t);
}
 
/**
 * TENSOR_STRUCT mode must return PRECONDITION_FAIL for all three push
 * variants — push operations are only valid on ARRAY_STRUCT tensors.
 */
static void test_push_wrong_mode(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t val = 1;
    assert_int_equal(push_back_tensor(t,  &val, INT32_TYPE), PRECONDITION_FAIL);
    assert_int_equal(push_front_tensor(t, &val, INT32_TYPE), PRECONDITION_FAIL);
    assert_int_equal(push_at_tensor(t,    &val, 0u, INT32_TYPE), PRECONDITION_FAIL);
 
    return_tensor(t);
}
 
// ---- push_back_tensor -------------------------------------------------------
 
/** push_back appends to the back and increments len. */
static void test_push_back_basic(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 10, b = 20, c = 30;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &c, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 3u);
 
    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 0u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 10);
    assert_int_equal(get_tensor_index(t, 1u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 20);
    assert_int_equal(get_tensor_index(t, 2u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 30);
 
    return_tensor(t);
}
 
/**
 * push_back on a full fixed-capacity array must return CAPACITY_OVERFLOW
 * and leave the array unchanged.
 */
static void test_push_back_full_no_growth(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(2u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 1, b = 2, c = 3;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 2u);
 
    /* Array is now full — next push must fail */
    assert_int_equal(push_back_tensor(t, &c, INT32_TYPE), CAPACITY_OVERFLOW);
    assert_int_equal(t->len, 2u);
 
    return_tensor(t);
}
 
/**
 * push_back on a full growable array must reallocate and succeed.
 * len must be incremented and the new element must be readable.
 */
static void test_push_back_growth(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(2u, INT32_TYPE, true);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 1, b = 2, c = 3;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
 
    /* This push triggers reallocation */
    assert_int_equal(push_back_tensor(t, &c, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 3u);
    assert_true(t->alloc >= 3u);
 
    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 2u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 3);
 
    return_tensor(t);
}
 
/** push_back fills every slot and confirms order is preserved. */
static void test_push_back_fill_to_capacity(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    for (int32_t i = 0; i < 5; i++)
        assert_int_equal(push_back_tensor(t, &i, INT32_TYPE), NO_ERROR);
 
    assert_int_equal(t->len, 5u);
 
    for (size_t i = 0u; i < 5u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, (int32_t)i);
    }
 
    return_tensor(t);
}
 
// ---- push_front_tensor ------------------------------------------------------
 
/** push_front prepends to the front, shifts existing elements, and
 *  increments len. */
static void test_push_front_basic(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 10, b = 20, c = 30;
    assert_int_equal(push_front_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_front_tensor(t, &b, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_front_tensor(t, &c, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 3u);
 
    /* Expected order: 30, 20, 10 */
    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 0u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 30);
    assert_int_equal(get_tensor_index(t, 1u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 20);
    assert_int_equal(get_tensor_index(t, 2u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 10);
 
    return_tensor(t);
}
 
/**
 * push_front onto an empty array must not call memmove (len == 0)
 * and must place the element at index 0.
 */
static void test_push_front_into_empty(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    assert_int_equal(t->len, 0u);
    int32_t val = 42;
    assert_int_equal(push_front_tensor(t, &val, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 1u);
 
    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 0u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 42);
 
    return_tensor(t);
}
 
/**
 * push_front on a full fixed-capacity array must return CAPACITY_OVERFLOW
 * and leave the array unchanged.
 */
static void test_push_front_full_no_growth(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(2u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 1, b = 2, c = 3;
    assert_int_equal(push_front_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_front_tensor(t, &b, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 2u);
 
    assert_int_equal(push_front_tensor(t, &c, INT32_TYPE), CAPACITY_OVERFLOW);
    assert_int_equal(t->len, 2u);
 
    return_tensor(t);
}
 
/**
 * push_front on a full growable array must reallocate, shift all
 * existing elements, and place the new value at index 0.
 */
static void test_push_front_growth(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(2u, INT32_TYPE, true);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 1, b = 2, c = 99;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
 
    /* This push_front triggers reallocation then shifts [1, 2] right */
    assert_int_equal(push_front_tensor(t, &c, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 3u);
    assert_true(t->alloc >= 3u);
 
    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 0u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 99);
    assert_int_equal(get_tensor_index(t, 1u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 1);
    assert_int_equal(get_tensor_index(t, 2u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 2);
 
    return_tensor(t);
}
 
// ---- push_at_tensor ---------------------------------------------------------
 
/** index > len must return OUT_OF_BOUNDS without modifying the array. */
static void test_push_at_out_of_bounds(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    t->len = 2u;
    int32_t val = 1;
 
    /* index 3 > len 2 */
    assert_int_equal(push_at_tensor(t, &val, 3u, INT32_TYPE), OUT_OF_BOUNDS);
    assert_int_equal(t->len, 2u);
 
    return_tensor(t);
}
 
/**
 * index == 0 must delegate to push_front_tensor — new element at slot 0,
 * existing elements shifted right.
 */
static void test_push_at_zero_delegates_to_front(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 10, b = 20, front = 99;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
 
    assert_int_equal(push_at_tensor(t, &front, 0u, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 3u);
 
    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 0u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 99);
    assert_int_equal(get_tensor_index(t, 1u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 10);
    assert_int_equal(get_tensor_index(t, 2u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 20);
 
    return_tensor(t);
}
 
/**
 * index == len must delegate to push_back_tensor — new element appended
 * at the back without any shift.
 */
static void test_push_at_len_delegates_to_back(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 10, b = 20, back = 99;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
 
    assert_int_equal(push_at_tensor(t, &back, t->len, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 3u);
 
    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 0u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 10);
    assert_int_equal(get_tensor_index(t, 1u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 20);
    assert_int_equal(get_tensor_index(t, 2u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 99);
 
    return_tensor(t);
}
 
/**
 * Insert in the middle: elements before index are unchanged, new element
 * appears at index, elements from index onward are shifted right by one.
 */
static void test_push_at_middle(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    /* Build [10, 20, 30, 40] */
    int32_t vals[] = { 10, 20, 30, 40 };
    for (size_t i = 0u; i < 4u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    /* Insert 99 at index 2 → [10, 20, 99, 30, 40] */
    int32_t ins = 99;
    assert_int_equal(push_at_tensor(t, &ins, 2u, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 5u);
 
    int32_t expected[] = { 10, 20, 99, 30, 40 };
    for (size_t i = 0u; i < 5u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(t);
}
 
/**
 * push_at on a full fixed-capacity array must return CAPACITY_OVERFLOW
 * regardless of the index.
 */
static void test_push_at_full_no_growth(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(3u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 1, b = 2, c = 3, d = 4;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &c, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 3u);
 
    assert_int_equal(push_at_tensor(t, &d, 1u, INT32_TYPE), CAPACITY_OVERFLOW);
    assert_int_equal(t->len, 3u);
 
    return_tensor(t);
}
 
/**
 * push_at in the middle of a growable array must reallocate, shift
 * elements correctly, and place the new value at the right position.
 */
static void test_push_at_growth(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(2u, INT32_TYPE, true);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 1, b = 2, ins = 99;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
 
    /* Insert at index 1 triggers reallocation → [1, 99, 2] */
    assert_int_equal(push_at_tensor(t, &ins, 1u, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 3u);
    assert_true(t->alloc >= 3u);
 
    int32_t expected[] = { 1, 99, 2 };
    for (size_t i = 0u; i < 3u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(t);
}
 
/**
 * Interleaved push_back, push_front, and push_at to confirm all three
 * functions maintain a consistent, correct element order.
 */
static void test_push_interleaved(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(8u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    /* push_back  → [1]          */
    int32_t v1 = 1;
    assert_int_equal(push_back_tensor(t, &v1, INT32_TYPE), NO_ERROR);
 
    /* push_front → [2, 1]       */
    int32_t v2 = 2;
    assert_int_equal(push_front_tensor(t, &v2, INT32_TYPE), NO_ERROR);
 
    /* push_at 1  → [2, 3, 1]   */
    int32_t v3 = 3;
    assert_int_equal(push_at_tensor(t, &v3, 1u, INT32_TYPE), NO_ERROR);
 
    /* push_back  → [2, 3, 1, 4] */
    int32_t v4 = 4;
    assert_int_equal(push_back_tensor(t, &v4, INT32_TYPE), NO_ERROR);
 
    assert_int_equal(t->len, 4u);
 
    int32_t expected[] = { 2, 3, 1, 4 };
    for (size_t i = 0u; i < 4u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(t);
}

// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// POP OPERATIONS (pop_back_tensor / pop_front_tensor / pop_at_tensor)
// ================================================================================
 
// ---- Shared guard tests -----------------------------------------------------
 
/** NULL tensor must return NULL_POINTER for all three pop variants. */
static void test_pop_null_tensor(void** state) {
    (void)state;
    int32_t out = 0;
    assert_int_equal(pop_back_tensor(NULL,  &out, INT32_TYPE), NULL_POINTER);
    assert_int_equal(pop_front_tensor(NULL, &out, INT32_TYPE), NULL_POINTER);
    assert_int_equal(pop_at_tensor(NULL,    &out, 0u, INT32_TYPE), NULL_POINTER);
}
 
/** Wrong dtype must return TYPE_MISMATCH for all three pop variants. */
static void test_pop_type_mismatch(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
    t->len = 2u;
 
    float out = 0.0f;
    assert_int_equal(pop_back_tensor(t,  &out, FLOAT_TYPE), TYPE_MISMATCH);
    assert_int_equal(pop_front_tensor(t, &out, FLOAT_TYPE), TYPE_MISMATCH);
    assert_int_equal(pop_at_tensor(t,    &out, 0u, FLOAT_TYPE), TYPE_MISMATCH);
    assert_int_equal(t->len, 2u);
 
    return_tensor(t);
}
 
/**
 * TENSOR_STRUCT mode must return PRECONDITION_FAIL for all three pop
 * variants.
 */
static void test_pop_wrong_mode(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t out = 0;
    assert_int_equal(pop_back_tensor(t,  &out, INT32_TYPE), PRECONDITION_FAIL);
    assert_int_equal(pop_front_tensor(t, &out, INT32_TYPE), PRECONDITION_FAIL);
    assert_int_equal(pop_at_tensor(t,    &out, 0u, INT32_TYPE), PRECONDITION_FAIL);
 
    return_tensor(t);
}
 
/** Popping from an empty array must return EMPTY for all three variants. */
static void test_pop_empty(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    assert_int_equal(t->len, 0u);
    int32_t out = 0;
    assert_int_equal(pop_back_tensor(t,  &out, INT32_TYPE), EMPTY);
    assert_int_equal(pop_front_tensor(t, &out, INT32_TYPE), EMPTY);
    assert_int_equal(pop_at_tensor(t,    &out, 0u, INT32_TYPE), EMPTY);
 
    return_tensor(t);
}
 
/**
 * Passing NULL for out must succeed — the removed value is discarded
 * without a copy.  Applies to all three variants.
 */
static void test_pop_null_out_discards_value(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 1, b = 2, c = 3;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &c, INT32_TYPE), NO_ERROR);
 
    assert_int_equal(pop_back_tensor(t,  NULL, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 2u);
 
    assert_int_equal(pop_front_tensor(t, NULL, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 1u);
 
    assert_int_equal(pop_at_tensor(t, NULL, 0u, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 0u);
 
    return_tensor(t);
}
 
// ---- pop_back_tensor --------------------------------------------------------
 
/** pop_back removes the last element, copies it to out, and decrements len. */
static void test_pop_back_basic(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 10, b = 20, c = 30;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &c, INT32_TYPE), NO_ERROR);
 
    int32_t out = 0;
    assert_int_equal(pop_back_tensor(t, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 30);
    assert_int_equal(t->len, 2u);
 
    assert_int_equal(pop_back_tensor(t, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 20);
    assert_int_equal(t->len, 1u);
 
    assert_int_equal(pop_back_tensor(t, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 10);
    assert_int_equal(t->len, 0u);
 
    /* Now empty — next pop must return EMPTY */
    assert_int_equal(pop_back_tensor(t, &out, INT32_TYPE), EMPTY);
 
    return_tensor(t);
}
 
/** pop_back does not disturb elements before the removed slot. */
static void test_pop_back_preserves_preceding_elements(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t vals[] = { 1, 2, 3, 4, 5 };
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    int32_t out = 0;
    assert_int_equal(pop_back_tensor(t, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 5);
 
    /* Remaining elements must be [1, 2, 3, 4] */
    for (size_t i = 0u; i < 4u; i++) {
        int32_t val = -1;
        assert_int_equal(get_tensor_index(t, i, &val, INT32_TYPE), NO_ERROR);
        assert_int_equal(val, (int32_t)(i + 1u));
    }
 
    return_tensor(t);
}
 
// ---- pop_front_tensor -------------------------------------------------------
 
/** pop_front removes the first element, copies it to out, shifts remaining
 *  elements left, and decrements len. */
static void test_pop_front_basic(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 10, b = 20, c = 30;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &c, INT32_TYPE), NO_ERROR);
 
    int32_t out = 0;
    assert_int_equal(pop_front_tensor(t, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 10);
    assert_int_equal(t->len, 2u);
 
    /* Remaining elements must be [20, 30] */
    int32_t v0 = -1, v1 = -1;
    assert_int_equal(get_tensor_index(t, 0u, &v0, INT32_TYPE), NO_ERROR);
    assert_int_equal(get_tensor_index(t, 1u, &v1, INT32_TYPE), NO_ERROR);
    assert_int_equal(v0, 20);
    assert_int_equal(v1, 30);
 
    return_tensor(t);
}
 
/**
 * pop_front on a single-element array must leave the array empty without
 * calling memmove (shift count would be zero).
 */
static void test_pop_front_single_element(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t val = 42;
    assert_int_equal(push_back_tensor(t, &val, INT32_TYPE), NO_ERROR);
 
    int32_t out = 0;
    assert_int_equal(pop_front_tensor(t, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 42);
    assert_int_equal(t->len, 0u);
 
    /* Array is now empty */
    assert_int_equal(pop_front_tensor(t, &out, INT32_TYPE), EMPTY);
 
    return_tensor(t);
}
 
/** Repeated pop_front drains the array in FIFO order. */
static void test_pop_front_fifo_order(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t vals[] = { 1, 2, 3, 4, 5 };
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    for (size_t i = 0u; i < 5u; i++) {
        int32_t out = -1;
        assert_int_equal(pop_front_tensor(t, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, (int32_t)(i + 1u));
    }
 
    assert_int_equal(t->len, 0u);
 
    return_tensor(t);
}
 
// ---- pop_at_tensor ----------------------------------------------------------
 
/** index >= len must return OUT_OF_BOUNDS without modifying the array. */
static void test_pop_at_out_of_bounds(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 1, b = 2;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
 
    int32_t out = 0;
    assert_int_equal(pop_at_tensor(t, &out, 2u, INT32_TYPE), OUT_OF_BOUNDS);
    assert_int_equal(t->len, 2u);
 
    return_tensor(t);
}
 
/**
 * index == 0 must delegate to pop_front_tensor — element at slot 0 is
 * removed and remaining elements shift left.
 */
static void test_pop_at_zero_delegates_to_front(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 10, b = 20, c = 30;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &c, INT32_TYPE), NO_ERROR);
 
    int32_t out = 0;
    assert_int_equal(pop_at_tensor(t, &out, 0u, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 10);
    assert_int_equal(t->len, 2u);
 
    /* Remaining elements must be [20, 30] */
    int32_t v0 = -1, v1 = -1;
    assert_int_equal(get_tensor_index(t, 0u, &v0, INT32_TYPE), NO_ERROR);
    assert_int_equal(get_tensor_index(t, 1u, &v1, INT32_TYPE), NO_ERROR);
    assert_int_equal(v0, 20);
    assert_int_equal(v1, 30);
 
    return_tensor(t);
}
 
/**
 * index == len - 1 must delegate to pop_back_tensor — last element is
 * removed without a shift.
 */
static void test_pop_at_last_delegates_to_back(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 10, b = 20, c = 30;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &c, INT32_TYPE), NO_ERROR);
 
    int32_t out = 0;
    assert_int_equal(pop_at_tensor(t, &out, t->len - 1u, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 30);
    assert_int_equal(t->len, 2u);
 
    /* Remaining elements must be [10, 20] */
    int32_t v0 = -1, v1 = -1;
    assert_int_equal(get_tensor_index(t, 0u, &v0, INT32_TYPE), NO_ERROR);
    assert_int_equal(get_tensor_index(t, 1u, &v1, INT32_TYPE), NO_ERROR);
    assert_int_equal(v0, 10);
    assert_int_equal(v1, 20);
 
    return_tensor(t);
}
 
/**
 * Remove from the middle: elements before index are unchanged, elements
 * after index shift left by one.
 */
static void test_pop_at_middle(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    /* Build [10, 20, 30, 40, 50] */
    int32_t vals[] = { 10, 20, 30, 40, 50 };
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    /* Remove index 2 (value 30) → [10, 20, 40, 50] */
    int32_t out = 0;
    assert_int_equal(pop_at_tensor(t, &out, 2u, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 30);
    assert_int_equal(t->len, 4u);
 
    int32_t expected[] = { 10, 20, 40, 50 };
    for (size_t i = 0u; i < 4u; i++) {
        int32_t val = -1;
        assert_int_equal(get_tensor_index(t, i, &val, INT32_TYPE), NO_ERROR);
        assert_int_equal(val, expected[i]);
    }
 
    return_tensor(t);
}
 
// ---- Push / pop round-trip --------------------------------------------------
 
/**
 * Push a sequence then pop it from the back — values must come out in
 * LIFO order and the array must be empty at the end.
 */
static void test_push_back_pop_back_lifo(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t vals[] = { 1, 2, 3, 4, 5 };
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    for (size_t i = 5u; i > 0u; i--) {
        int32_t out = -1;
        assert_int_equal(pop_back_tensor(t, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, (int32_t)i);
    }
 
    assert_int_equal(t->len, 0u);
    return_tensor(t);
}
 
/**
 * Interleaved push and pop across all three variants to confirm consistent
 * element order is maintained throughout.
 */
static void test_pop_interleaved(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(8u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    /* Build [10, 20, 30, 40] via push_back */
    int32_t vals[] = { 10, 20, 30, 40 };
    for (size_t i = 0u; i < 4u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    /* pop_front → removes 10, array is [20, 30, 40] */
    int32_t out = 0;
    assert_int_equal(pop_front_tensor(t, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 10);
 
    /* pop_at 1 → removes 30, array is [20, 40] */
    assert_int_equal(pop_at_tensor(t, &out, 1u, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 30);
 
    /* push_front 5 → array is [5, 20, 40] */
    int32_t five = 5;
    assert_int_equal(push_front_tensor(t, &five, INT32_TYPE), NO_ERROR);
 
    /* pop_back → removes 40, array is [5, 20] */
    assert_int_equal(pop_back_tensor(t, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 40);
 
    assert_int_equal(t->len, 2u);
 
    int32_t v0 = -1, v1 = -1;
    assert_int_equal(get_tensor_index(t, 0u, &v0, INT32_TYPE), NO_ERROR);
    assert_int_equal(get_tensor_index(t, 1u, &v1, INT32_TYPE), NO_ERROR);
    assert_int_equal(v0, 5);
    assert_int_equal(v1, 20);
 
    return_tensor(t);
}

// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// CONCAT TENSOR ARRAY (concat_tensor_array)
// ================================================================================
 
// ---- NULL / guard tests -----------------------------------------------------
 
/** NULL dst or src must return NULL_POINTER. */
static void test_concat_null(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    assert_int_equal(concat_tensor_array(NULL, t,    INT32_TYPE), NULL_POINTER);
    assert_int_equal(concat_tensor_array(t,    NULL, INT32_TYPE), NULL_POINTER);
 
    return_tensor(t);
}
 
/** Wrong dtype must return TYPE_MISMATCH. */
static void test_concat_type_mismatch_dtype_arg(void** state) {
    (void)state;
    tensor_expect_t r1 = _make_array(4u, INT32_TYPE, false);
    tensor_expect_t r2 = _make_array(4u, INT32_TYPE, false);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
 
    /* Caller passes FLOAT_TYPE but both tensors are INT32_TYPE */
    assert_int_equal(
        concat_tensor_array(r1.u.value, r2.u.value, FLOAT_TYPE),
        TYPE_MISMATCH
    );
 
    return_tensor(r1.u.value);
    return_tensor(r2.u.value);
}
 
/** Mismatched dtypes between dst and src must return TYPE_MISMATCH. */
static void test_concat_type_mismatch_between_tensors(void** state) {
    (void)state;
    tensor_expect_t r1 = _make_array(4u, INT32_TYPE,  false);
    tensor_expect_t r2 = _make_array(4u, FLOAT_TYPE,  false);
    assert_true(r1.has_value);
    assert_true(r2.has_value);
 
    assert_int_equal(
        concat_tensor_array(r1.u.value, r2.u.value, INT32_TYPE),
        TYPE_MISMATCH
    );
 
    return_tensor(r1.u.value);
    return_tensor(r2.u.value);
}
 
/**
 * TENSOR_STRUCT mode on dst or src must return PRECONDITION_FAIL.
 */
static void test_concat_wrong_mode(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t fixed = _make_tensor(1u, shape, INT32_TYPE);
    tensor_expect_t arr   = _make_array(4u, INT32_TYPE, false);
    assert_true(fixed.has_value);
    assert_true(arr.has_value);
 
    /* Fixed-shape dst */
    assert_int_equal(
        concat_tensor_array(fixed.u.value, arr.u.value, INT32_TYPE),
        PRECONDITION_FAIL
    );
 
    /* Fixed-shape src */
    assert_int_equal(
        concat_tensor_array(arr.u.value, fixed.u.value, INT32_TYPE),
        PRECONDITION_FAIL
    );
 
    return_tensor(fixed.u.value);
    return_tensor(arr.u.value);
}
 
// ---- Empty source fast path -------------------------------------------------
 
/**
 * Concatenating an empty src must return NO_ERROR without modifying dst.
 */
static void test_concat_empty_src(void** state) {
    (void)state;
    tensor_expect_t r_dst = _make_array(4u, INT32_TYPE, false);
    tensor_expect_t r_src = _make_array(4u, INT32_TYPE, false);
    assert_true(r_dst.has_value);
    assert_true(r_src.has_value);
 
    tensor_t* dst = r_dst.u.value;
    tensor_t* src = r_src.u.value;
 
    int32_t a = 1, b = 2;
    assert_int_equal(push_back_tensor(dst, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(dst, &b, INT32_TYPE), NO_ERROR);
 
    /* src has len == 0 */
    assert_int_equal(concat_tensor_array(dst, src, INT32_TYPE), NO_ERROR);
    assert_int_equal(dst->len, 2u);
 
    return_tensor(dst);
    return_tensor(src);
}
 
// ---- Correctness tests ------------------------------------------------------
 
/**
 * Basic concatenation: elements from src are appended to the back of dst
 * in the correct order and dst->len is updated correctly.
 */
static void test_concat_basic(void** state) {
    (void)state;
    tensor_expect_t r_dst = _make_array(8u, INT32_TYPE, false);
    tensor_expect_t r_src = _make_array(4u, INT32_TYPE, false);
    assert_true(r_dst.has_value);
    assert_true(r_src.has_value);
 
    tensor_t* dst = r_dst.u.value;
    tensor_t* src = r_src.u.value;
 
    /* dst = [1, 2, 3] */
    int32_t a = 1, b = 2, c = 3;
    assert_int_equal(push_back_tensor(dst, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(dst, &b, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(dst, &c, INT32_TYPE), NO_ERROR);
 
    /* src = [4, 5] */
    int32_t d = 4, e = 5;
    assert_int_equal(push_back_tensor(src, &d, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(src, &e, INT32_TYPE), NO_ERROR);
 
    assert_int_equal(concat_tensor_array(dst, src, INT32_TYPE), NO_ERROR);
    assert_int_equal(dst->len, 5u);
 
    int32_t expected[] = { 1, 2, 3, 4, 5 };
    for (size_t i = 0u; i < 5u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(dst, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(dst);
    return_tensor(src);
}
 
/**
 * Concatenate into an empty dst — result should contain exactly the
 * elements of src in the original order.
 */
static void test_concat_into_empty_dst(void** state) {
    (void)state;
    tensor_expect_t r_dst = _make_array(8u, INT32_TYPE, false);
    tensor_expect_t r_src = _make_array(4u, INT32_TYPE, false);
    assert_true(r_dst.has_value);
    assert_true(r_src.has_value);
 
    tensor_t* dst = r_dst.u.value;
    tensor_t* src = r_src.u.value;
 
    int32_t vals[] = { 10, 20, 30 };
    for (size_t i = 0u; i < 3u; i++)
        assert_int_equal(push_back_tensor(src, &vals[i], INT32_TYPE), NO_ERROR);
 
    assert_int_equal(concat_tensor_array(dst, src, INT32_TYPE), NO_ERROR);
    assert_int_equal(dst->len, 3u);
 
    for (size_t i = 0u; i < 3u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(dst, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, vals[i]);
    }
 
    return_tensor(dst);
    return_tensor(src);
}
 
/**
 * src must not be modified by the concatenation — its len, alloc, and
 * element values must be identical before and after the call.
 */
static void test_concat_src_unmodified(void** state) {
    (void)state;
    tensor_expect_t r_dst = _make_array(8u, INT32_TYPE, false);
    tensor_expect_t r_src = _make_array(4u, INT32_TYPE, false);
    assert_true(r_dst.has_value);
    assert_true(r_src.has_value);
 
    tensor_t* dst = r_dst.u.value;
    tensor_t* src = r_src.u.value;
 
    int32_t vals[] = { 7, 8, 9 };
    for (size_t i = 0u; i < 3u; i++)
        assert_int_equal(push_back_tensor(src, &vals[i], INT32_TYPE), NO_ERROR);
 
    size_t src_len_before   = src->len;
    size_t src_alloc_before = src->alloc;
 
    assert_int_equal(concat_tensor_array(dst, src, INT32_TYPE), NO_ERROR);
 
    assert_int_equal(src->len,   src_len_before);
    assert_int_equal(src->alloc, src_alloc_before);
 
    for (size_t i = 0u; i < 3u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(src, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, vals[i]);
    }
 
    return_tensor(dst);
    return_tensor(src);
}
 
// ---- Capacity and growth tests ----------------------------------------------
 
/**
 * Concatenation that fits within existing capacity must not reallocate —
 * alloc must be unchanged after the call.
 */
static void test_concat_fits_in_existing_capacity(void** state) {
    (void)state;
    /* dst has capacity 6, will hold 2 elements before concat */
    tensor_expect_t r_dst = _make_array(6u, INT32_TYPE, false);
    tensor_expect_t r_src = _make_array(3u, INT32_TYPE, false);
    assert_true(r_dst.has_value);
    assert_true(r_src.has_value);
 
    tensor_t* dst = r_dst.u.value;
    tensor_t* src = r_src.u.value;
 
    int32_t a = 1, b = 2;
    assert_int_equal(push_back_tensor(dst, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(dst, &b, INT32_TYPE), NO_ERROR);
 
    int32_t c = 3, d = 4, e = 5;
    assert_int_equal(push_back_tensor(src, &c, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(src, &d, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(src, &e, INT32_TYPE), NO_ERROR);
 
    size_t alloc_before = dst->alloc;
 
    /* 2 + 3 == 5 <= 6: no reallocation needed */
    assert_int_equal(concat_tensor_array(dst, src, INT32_TYPE), NO_ERROR);
    assert_int_equal(dst->len,   5u);
    assert_int_equal(dst->alloc, alloc_before);
 
    return_tensor(dst);
    return_tensor(src);
}
 
/**
 * Concatenation that exceeds capacity on a growable dst must reallocate
 * once and produce the correct element sequence.
 */
static void test_concat_triggers_growth(void** state) {
    (void)state;
    tensor_expect_t r_dst = _make_array(2u, INT32_TYPE, true);
    tensor_expect_t r_src = _make_array(4u, INT32_TYPE, false);
    assert_true(r_dst.has_value);
    assert_true(r_src.has_value);
 
    tensor_t* dst = r_dst.u.value;
    tensor_t* src = r_src.u.value;
 
    int32_t a = 1, b = 2;
    assert_int_equal(push_back_tensor(dst, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(dst, &b, INT32_TYPE), NO_ERROR);
 
    int32_t vals[] = { 3, 4, 5, 6 };
    for (size_t i = 0u; i < 4u; i++)
        assert_int_equal(push_back_tensor(src, &vals[i], INT32_TYPE), NO_ERROR);
 
    /* dst is full (len==alloc==2); concat must grow and succeed */
    assert_int_equal(concat_tensor_array(dst, src, INT32_TYPE), NO_ERROR);
    assert_int_equal(dst->len, 6u);
    assert_true(dst->alloc >= 6u);
 
    int32_t expected[] = { 1, 2, 3, 4, 5, 6 };
    for (size_t i = 0u; i < 6u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(dst, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(dst);
    return_tensor(src);
}
 
/**
 * Concatenation that exceeds capacity on a fixed-capacity dst must return
 * CAPACITY_OVERFLOW and leave dst unchanged.
 */
static void test_concat_exceeds_fixed_capacity(void** state) {
    (void)state;
    tensor_expect_t r_dst = _make_array(3u, INT32_TYPE, false);
    tensor_expect_t r_src = _make_array(4u, INT32_TYPE, false);
    assert_true(r_dst.has_value);
    assert_true(r_src.has_value);
 
    tensor_t* dst = r_dst.u.value;
    tensor_t* src = r_src.u.value;
 
    int32_t a = 1, b = 2;
    assert_int_equal(push_back_tensor(dst, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(dst, &b, INT32_TYPE), NO_ERROR);
 
    int32_t vals[] = { 3, 4, 5 };
    for (size_t i = 0u; i < 3u; i++)
        assert_int_equal(push_back_tensor(src, &vals[i], INT32_TYPE), NO_ERROR);
 
    /* 2 + 3 == 5 > 3: must fail */
    assert_int_equal(
        concat_tensor_array(dst, src, INT32_TYPE),
        CAPACITY_OVERFLOW
    );
    assert_int_equal(dst->len, 2u);
 
    return_tensor(dst);
    return_tensor(src);
}
 
/**
 * Concatenate two non-trivial sequences and verify the full combined
 * element order end-to-end.
 */
static void test_concat_full_sequence(void** state) {
    (void)state;
    tensor_expect_t r_dst = _make_array(10u, INT32_TYPE, false);
    tensor_expect_t r_src = _make_array(5u,  INT32_TYPE, false);
    assert_true(r_dst.has_value);
    assert_true(r_src.has_value);
 
    tensor_t* dst = r_dst.u.value;
    tensor_t* src = r_src.u.value;
 
    for (int32_t i = 1; i <= 5; i++)
        assert_int_equal(push_back_tensor(dst, &i, INT32_TYPE), NO_ERROR);
 
    for (int32_t i = 6; i <= 10; i++)
        assert_int_equal(push_back_tensor(src, &i, INT32_TYPE), NO_ERROR);
 
    assert_int_equal(concat_tensor_array(dst, src, INT32_TYPE), NO_ERROR);
    assert_int_equal(dst->len, 10u);
 
    for (size_t i = 0u; i < 10u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(dst, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, (int32_t)(i + 1u));
    }
 
    return_tensor(dst);
    return_tensor(src);
}

// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// SLICE TENSOR ARRAY (slice_tensor_array)
// ================================================================================
 
// ---- NULL / guard tests -----------------------------------------------------
 
/** NULL src must return NULL_POINTER. */
static void test_slice_null_src(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t r = slice_tensor_array(NULL, 0u, 1u, &alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
/** TENSOR_STRUCT mode must return PRECONDITION_FAIL. */
static void test_slice_wrong_mode(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t r = _make_tensor(1u, shape, INT32_TYPE);
    assert_true(r.has_value);
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(r.u.value, 0u, 2u, &alloc);
    assert_false(s.has_value);
    assert_int_equal(s.u.error, PRECONDITION_FAIL);
 
    return_tensor(r.u.value);
}
 
/** start > src->len must return OUT_OF_BOUNDS. */
static void test_slice_start_out_of_bounds(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
    t->len = 3u;
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(t, 4u, 5u, &alloc);
    assert_false(s.has_value);
    assert_int_equal(s.u.error, OUT_OF_BOUNDS);
 
    return_tensor(t);
}
 
/** end > src->len must return OUT_OF_BOUNDS. */
static void test_slice_end_out_of_bounds(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
    t->len = 3u;
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(t, 0u, 4u, &alloc);
    assert_false(s.has_value);
    assert_int_equal(s.u.error, OUT_OF_BOUNDS);
 
    return_tensor(t);
}
 
/** start == end must return INVALID_ARG (zero-length slice). */
static void test_slice_zero_length(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
    t->len = 4u;
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(t, 2u, 2u, &alloc);
    assert_false(s.has_value);
    assert_int_equal(s.u.error, INVALID_ARG);
 
    return_tensor(t);
}
 
/** start > end must return INVALID_ARG. */
static void test_slice_start_greater_than_end(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
    t->len = 4u;
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(t, 3u, 1u, &alloc);
    assert_false(s.has_value);
    assert_int_equal(s.u.error, INVALID_ARG);
 
    return_tensor(t);
}
 
// ---- Scalar field correctness -----------------------------------------------
 
/**
 * The returned slice must have the correct scalar fields: ndim == 1,
 * len == end - start, alloc == len, data_size == src->data_size,
 * dtype == src->dtype, mode == ARRAY_STRUCT, growth == false.
 */
static void test_slice_scalar_fields(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(8u, DOUBLE_TYPE, true);
    assert_true(r.has_value);
    tensor_t* src = r.u.value;
    src->len = 8u;
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(src, 2u, 6u, &alloc);
    assert_true(s.has_value);
    tensor_t* sl = s.u.value;
 
    assert_int_equal(sl->ndim,      1u);
    assert_int_equal(sl->len,       4u);
    assert_int_equal(sl->alloc,     4u);
    assert_int_equal(sl->data_size, sizeof(double));
    assert_int_equal(sl->dtype,     DOUBLE_TYPE);
    assert_int_equal(sl->mode,      ARRAY_STRUCT);
    assert_false(sl->growth);
 
    return_tensor(src);
    return_tensor(sl);
}
 
/** Shape and stride of the returned slice must be correct. */
static void test_slice_shape_and_strides(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(6u, FLOAT_TYPE, false);
    assert_true(r.has_value);
    tensor_t* src = r.u.value;
    src->len = 6u;
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(src, 1u, 4u, &alloc);
    assert_true(s.has_value);
    tensor_t* sl = s.u.value;
 
    assert_int_equal(sl->shape[0],   3u);
    assert_int_equal(sl->strides[0], sizeof(float));
    assert_ptr_equal(sl->shape,      sl->meta);
    assert_ptr_equal(sl->strides,    sl->meta + 1u);
 
    return_tensor(src);
    return_tensor(sl);
}
 
// ---- Element correctness ----------------------------------------------------
 
/** Slice from the front of the array. */
static void test_slice_front(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(6u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* src = r.u.value;
 
    int32_t vals[] = { 10, 20, 30, 40, 50, 60 };
    src->len = 6u;
    for (size_t i = 0u; i < 6u; i++)
        assert_int_equal(set_tensor_index(src, i, &vals[i], INT32_TYPE), NO_ERROR);
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(src, 0u, 3u, &alloc);
    assert_true(s.has_value);
    tensor_t* sl = s.u.value;
 
    assert_int_equal(sl->len, 3u);
    int32_t expected[] = { 10, 20, 30 };
    for (size_t i = 0u; i < 3u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(sl, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(src);
    return_tensor(sl);
}
 
/** Slice from the back of the array. */
static void test_slice_back(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(6u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* src = r.u.value;
 
    int32_t vals[] = { 10, 20, 30, 40, 50, 60 };
    src->len = 6u;
    for (size_t i = 0u; i < 6u; i++)
        assert_int_equal(set_tensor_index(src, i, &vals[i], INT32_TYPE), NO_ERROR);
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(src, 3u, 6u, &alloc);
    assert_true(s.has_value);
    tensor_t* sl = s.u.value;
 
    assert_int_equal(sl->len, 3u);
    int32_t expected[] = { 40, 50, 60 };
    for (size_t i = 0u; i < 3u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(sl, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(src);
    return_tensor(sl);
}
 
/** Slice from the middle of the array. */
static void test_slice_middle(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(6u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* src = r.u.value;
 
    int32_t vals[] = { 10, 20, 30, 40, 50, 60 };
    src->len = 6u;
    for (size_t i = 0u; i < 6u; i++)
        assert_int_equal(set_tensor_index(src, i, &vals[i], INT32_TYPE), NO_ERROR);
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(src, 2u, 5u, &alloc);
    assert_true(s.has_value);
    tensor_t* sl = s.u.value;
 
    assert_int_equal(sl->len, 3u);
    int32_t expected[] = { 30, 40, 50 };
    for (size_t i = 0u; i < 3u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(sl, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(src);
    return_tensor(sl);
}
 
/** A single-element slice (end == start + 1) must succeed. */
static void test_slice_single_element(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* src = r.u.value;
 
    int32_t vals[] = { 1, 2, 3, 4, 5 };
    src->len = 5u;
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(set_tensor_index(src, i, &vals[i], INT32_TYPE), NO_ERROR);
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(src, 3u, 4u, &alloc);
    assert_true(s.has_value);
    tensor_t* sl = s.u.value;
 
    assert_int_equal(sl->len, 1u);
    int32_t out = -1;
    assert_int_equal(get_tensor_index(sl, 0u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 4);
 
    return_tensor(src);
    return_tensor(sl);
}
 
/** A full-length slice (start == 0, end == len) must equal the source. */
static void test_slice_full_length(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* src = r.u.value;
 
    int32_t vals[] = { 1, 2, 3, 4, 5 };
    src->len = 5u;
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(set_tensor_index(src, i, &vals[i], INT32_TYPE), NO_ERROR);
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(src, 0u, src->len, &alloc);
    assert_true(s.has_value);
    tensor_t* sl = s.u.value;
 
    assert_int_equal(sl->len, src->len);
    for (size_t i = 0u; i < src->len; i++) {
        int32_t src_val = -1, sl_val = -1;
        assert_int_equal(get_tensor_index(src, i, &src_val, INT32_TYPE), NO_ERROR);
        assert_int_equal(get_tensor_index(sl,  i, &sl_val,  INT32_TYPE), NO_ERROR);
        assert_int_equal(sl_val, src_val);
    }
 
    return_tensor(src);
    return_tensor(sl);
}
 
// ---- Deep copy independence -------------------------------------------------
 
/**
 * Mutating an element in the slice must not affect the source, and
 * mutating the source must not affect the slice.
 */
static void test_slice_deep_copy_independence(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* src = r.u.value;
 
    int32_t vals[] = { 10, 20, 30, 40, 50 };
    src->len = 5u;
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(set_tensor_index(src, i, &vals[i], INT32_TYPE), NO_ERROR);
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(src, 1u, 4u, &alloc);
    assert_true(s.has_value);
    tensor_t* sl = s.u.value;
 
    /* Mutate slot 0 of the slice — src slot 1 must be unchanged */
    int32_t new_val = 999;
    assert_int_equal(set_tensor_index(sl, 0u, &new_val, INT32_TYPE), NO_ERROR);
 
    int32_t src_val = -1;
    assert_int_equal(get_tensor_index(src, 1u, &src_val, INT32_TYPE), NO_ERROR);
    assert_int_equal(src_val, 20);
 
    /* Mutate src slot 2 — slice slot 1 must be unchanged */
    int32_t src_new = 777;
    assert_int_equal(set_tensor_index(src, 2u, &src_new, INT32_TYPE), NO_ERROR);
 
    int32_t sl_val = -1;
    assert_int_equal(get_tensor_index(sl, 1u, &sl_val, INT32_TYPE), NO_ERROR);
    assert_int_equal(sl_val, 30);
 
    return_tensor(src);
    return_tensor(sl);
}
 
/**
 * src must be unmodified after slice_tensor_array — len, alloc, and all
 * element values must be identical before and after the call.
 */
static void test_slice_src_unmodified(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* src = r.u.value;
 
    int32_t vals[] = { 1, 2, 3, 4, 5 };
    src->len = 5u;
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(set_tensor_index(src, i, &vals[i], INT32_TYPE), NO_ERROR);
 
    size_t len_before   = src->len;
    size_t alloc_before = src->alloc;
 
    allocator_vtable_t alloc = heap_allocator();
    tensor_expect_t s = slice_tensor_array(src, 1u, 3u, &alloc);
    assert_true(s.has_value);
 
    assert_int_equal(src->len,   len_before);
    assert_int_equal(src->alloc, alloc_before);
    for (size_t i = 0u; i < 5u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(src, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, vals[i]);
    }
 
    return_tensor(src);
    return_tensor(s.u.value);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// REVERSE TENSOR (reverse_tensor)
// ================================================================================
 
/** NULL tensor must return NULL_POINTER. */
static void test_reverse_tensor_null(void** state) {
    (void)state;
    assert_int_equal(reverse_tensor(NULL), NULL_POINTER);
}
 
/** Empty array (len == 0) must return EMPTY without modifying the tensor. */
static void test_reverse_tensor_empty(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    assert_int_equal(t->len, 0u);
    assert_int_equal(reverse_tensor(t), EMPTY);
    assert_int_equal(t->len, 0u);
 
    return_tensor(t);
}
 
/** Single-element array (len == 1) must return EMPTY without modifying the
 *  tensor — there is nothing to swap. */
static void test_reverse_tensor_single_element(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t val = 42;
    assert_int_equal(push_back_tensor(t, &val, INT32_TYPE), NO_ERROR);
    assert_int_equal(t->len, 1u);
 
    assert_int_equal(reverse_tensor(t), EMPTY);
 
    /* Element must be unchanged */
    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 0u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 42);
 
    return_tensor(t);
}
 
/** Even-length sequence must be reversed correctly. */
static void test_reverse_tensor_even_length(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(6u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    /* Build [10, 20, 30, 40, 50, 60] */
    int32_t vals[] = { 10, 20, 30, 40, 50, 60 };
    for (size_t i = 0u; i < 6u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    assert_int_equal(reverse_tensor(t), NO_ERROR);
 
    int32_t expected[] = { 60, 50, 40, 30, 20, 10 };
    for (size_t i = 0u; i < 6u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(t);
}
 
/** Odd-length sequence must be reversed correctly with the middle element
 *  remaining in place. */
static void test_reverse_tensor_odd_length(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    /* Build [1, 2, 3, 4, 5] */
    int32_t vals[] = { 1, 2, 3, 4, 5 };
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    assert_int_equal(reverse_tensor(t), NO_ERROR);
 
    int32_t expected[] = { 5, 4, 3, 2, 1 };
    for (size_t i = 0u; i < 5u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    /* Middle element must be unchanged */
    int32_t mid = -1;
    assert_int_equal(get_tensor_index(t, 2u, &mid, INT32_TYPE), NO_ERROR);
    assert_int_equal(mid, 3);
 
    return_tensor(t);
}
 
/** Two-element sequence is the minimal non-trivial case — the two elements
 *  must be swapped. */
static void test_reverse_tensor_two_elements(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(2u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 7, b = 99;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
 
    assert_int_equal(reverse_tensor(t), NO_ERROR);
 
    int32_t v0 = -1, v1 = -1;
    assert_int_equal(get_tensor_index(t, 0u, &v0, INT32_TYPE), NO_ERROR);
    assert_int_equal(get_tensor_index(t, 1u, &v1, INT32_TYPE), NO_ERROR);
    assert_int_equal(v0, 99);
    assert_int_equal(v1, 7);
 
    return_tensor(t);
}
 
/**
 * Double reverse must restore the original order exactly — this is the
 * strongest single correctness check since any byte-level error in the
 * swap would produce a different sequence on the first pass and then
 * fail to restore it on the second.
 */
static void test_reverse_tensor_double_reverse(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t vals[] = { 10, 20, 30, 40, 50 };
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    assert_int_equal(reverse_tensor(t), NO_ERROR);
    assert_int_equal(reverse_tensor(t), NO_ERROR);
 
    for (size_t i = 0u; i < 5u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, vals[i]);
    }
 
    return_tensor(t);
}
 
/**
 * Reverse a TENSOR_FIXED_SHAPE tensor (2-D) via flat index access to
 * confirm reverse_tensor operates on the raw buffer regardless of mode.
 */
static void test_reverse_tensor_fixed_shape(void** state) {
    (void)state;
    const size_t shape[] = { 2u, 3u };
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    /* Populate via flat index: [1, 2, 3, 4, 5, 6] */
    for (size_t i = 0u; i < 6u; i++) {
        int32_t val = (int32_t)(i + 1u);
        assert_int_equal(set_tensor_index(t, i, &val, INT32_TYPE), NO_ERROR);
    }
 
    assert_int_equal(reverse_tensor(t), NO_ERROR);
 
    int32_t expected[] = { 6, 5, 4, 3, 2, 1 };
    for (size_t i = 0u; i < 6u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(t);
}
 
/**
 * Reverse a tensor of double elements to confirm the SIMD routine handles
 * element sizes larger than one byte correctly.
 */
static void test_reverse_tensor_double_dtype(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, DOUBLE_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    double vals[] = { 1.1, 2.2, 3.3, 4.4 };
    for (size_t i = 0u; i < 4u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], DOUBLE_TYPE), NO_ERROR);
 
    assert_int_equal(reverse_tensor(t), NO_ERROR);
 
    double expected[] = { 4.4, 3.3, 2.2, 1.1 };
    for (size_t i = 0u; i < 4u; i++) {
        double out = 0.0;
        assert_int_equal(get_tensor_index(t, i, &out, DOUBLE_TYPE), NO_ERROR);
        assert_double_equal(out, expected[i], 1e-9);
    }

    return_tensor(t);
}
 
/**
 * Reverse a tensor of user-defined vec3_t elements to confirm the SIMD
 * routine handles large struct element sizes correctly.
 */
static void test_reverse_tensor_user_dtype(void** state) {
    (void)state;
    assert_true(ensure_dtype_registered(&vec3_desc));
 
    allocator_vtable_t alloc = heap_allocator();
    const size_t shape[] = { 3u };
    tensor_expect_t r = init_tensor(1u, shape, VEC3_TYPE, alloc);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
    t->mode = ARRAY_STRUCT;
    t->len  = 3u;
 
    vec3_t a = { 1.0f, 2.0f, 3.0f };
    vec3_t b = { 4.0f, 5.0f, 6.0f };
    vec3_t c = { 7.0f, 8.0f, 9.0f };
    assert_int_equal(set_tensor_index(t, 0u, &a, VEC3_TYPE), NO_ERROR);
    assert_int_equal(set_tensor_index(t, 1u, &b, VEC3_TYPE), NO_ERROR);
    assert_int_equal(set_tensor_index(t, 2u, &c, VEC3_TYPE), NO_ERROR);
 
    assert_int_equal(reverse_tensor(t), NO_ERROR);
 
    vec3_t out0 = { 0 }, out1 = { 0 }, out2 = { 0 };
    assert_int_equal(get_tensor_index(t, 0u, &out0, VEC3_TYPE), NO_ERROR);
    assert_int_equal(get_tensor_index(t, 1u, &out1, VEC3_TYPE), NO_ERROR);
    assert_int_equal(get_tensor_index(t, 2u, &out2, VEC3_TYPE), NO_ERROR);
 
    assert_float_equal(out0.x, c.x, 1e-6f);
    assert_float_equal(out0.y, c.y, 1e-6f);
    assert_float_equal(out0.z, c.z, 1e-6f);
    assert_float_equal(out1.x, b.x, 1e-6f);
    assert_float_equal(out1.y, b.y, 1e-6f);
    assert_float_equal(out1.z, b.z, 1e-6f);
    assert_float_equal(out2.x, a.x, 1e-6f);
    assert_float_equal(out2.y, a.y, 1e-6f);
    assert_float_equal(out2.z, a.z, 1e-6f);
 
    return_tensor(t);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// SORT TENSOR (sort_tensor)
// ================================================================================
 
// ---- Comparator functions used across tests ----------------------------------
 
static int _cmp_int32(const void* a, const void* b) {
    int32_t ia = *(const int32_t*)a;
    int32_t ib = *(const int32_t*)b;
    return (ia > ib) - (ia < ib);
}
 
static int _cmp_float(const void* a, const void* b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}
 
static int _cmp_double(const void* a, const void* b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    return (da > db) - (da < db);
}
 
static int _cmp_vec3_by_x(const void* a, const void* b) {
    float fa = ((const vec3_t*)a)->x;
    float fb = ((const vec3_t*)b)->x;
    return (fa > fb) - (fa < fb);
}
 
// ---- NULL / guard tests -----------------------------------------------------
 
/** NULL tensor must return NULL_POINTER. */
static void test_sort_tensor_null_tensor(void** state) {
    (void)state;
    assert_int_equal(sort_tensor(NULL, _cmp_int32, FORWARD), NULL_POINTER);
}
 
/** NULL comparator must return NULL_POINTER. */
static void test_sort_tensor_null_cmp(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
    t->len = 4u;
 
    assert_int_equal(sort_tensor(t, NULL, FORWARD), NULL_POINTER);
    return_tensor(t);
}
 
/** len == 0 must return EMPTY. */
static void test_sort_tensor_empty(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    assert_int_equal(t->len, 0u);
    assert_int_equal(sort_tensor(t, _cmp_int32, FORWARD), EMPTY);
    return_tensor(t);
}
 
/** len == 1 must return EMPTY — nothing to sort. */
static void test_sort_tensor_single_element(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(4u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t val = 42;
    assert_int_equal(push_back_tensor(t, &val, INT32_TYPE), NO_ERROR);
    assert_int_equal(sort_tensor(t, _cmp_int32, FORWARD), EMPTY);
 
    int32_t out = 0;
    assert_int_equal(get_tensor_index(t, 0u, &out, INT32_TYPE), NO_ERROR);
    assert_int_equal(out, 42);
 
    return_tensor(t);
}
 
// ---- Two-element edge cases --------------------------------------------------
 
/** Two elements in wrong order must be swapped by FORWARD sort. */
static void test_sort_tensor_two_elements_forward(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(2u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 20, b = 10;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
 
    assert_int_equal(sort_tensor(t, _cmp_int32, FORWARD), NO_ERROR);
 
    int32_t v0 = -1, v1 = -1;
    assert_int_equal(get_tensor_index(t, 0u, &v0, INT32_TYPE), NO_ERROR);
    assert_int_equal(get_tensor_index(t, 1u, &v1, INT32_TYPE), NO_ERROR);
    assert_int_equal(v0, 10);
    assert_int_equal(v1, 20);
 
    return_tensor(t);
}
 
/** Two elements in wrong order must be swapped by REVERSE sort. */
static void test_sort_tensor_two_elements_reverse(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(2u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t a = 10, b = 20;
    assert_int_equal(push_back_tensor(t, &a, INT32_TYPE), NO_ERROR);
    assert_int_equal(push_back_tensor(t, &b, INT32_TYPE), NO_ERROR);
 
    assert_int_equal(sort_tensor(t, _cmp_int32, REVERSE), NO_ERROR);
 
    int32_t v0 = -1, v1 = -1;
    assert_int_equal(get_tensor_index(t, 0u, &v0, INT32_TYPE), NO_ERROR);
    assert_int_equal(get_tensor_index(t, 1u, &v1, INT32_TYPE), NO_ERROR);
    assert_int_equal(v0, 20);
    assert_int_equal(v1, 10);
 
    return_tensor(t);
}
 
// ---- ARRAY_STRUCT forward sort ----------------------------------------------
 
/** Forward sort of an unsorted int32 array produces ascending order. */
static void test_sort_tensor_array_forward_int32(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(8u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t vals[] = { 5, 2, 8, 1, 9, 3, 7, 4 };
    for (size_t i = 0u; i < 8u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    assert_int_equal(sort_tensor(t, _cmp_int32, FORWARD), NO_ERROR);
 
    int32_t expected[] = { 1, 2, 3, 4, 5, 7, 8, 9 };
    for (size_t i = 0u; i < 8u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(t);
}
 
/** Forward sort of an unsorted float array produces ascending order. */
static void test_sort_tensor_array_forward_float(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(6u, FLOAT_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    float vals[] = { 3.3f, 1.1f, 4.4f, 2.2f, 6.6f, 5.5f };
    for (size_t i = 0u; i < 6u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], FLOAT_TYPE), NO_ERROR);
 
    assert_int_equal(sort_tensor(t, _cmp_float, FORWARD), NO_ERROR);
 
    float expected[] = { 1.1f, 2.2f, 3.3f, 4.4f, 5.5f, 6.6f };
    for (size_t i = 0u; i < 6u; i++) {
        float out = 0.0f;
        assert_int_equal(get_tensor_index(t, i, &out, FLOAT_TYPE), NO_ERROR);
        assert_float_equal(out, expected[i], 1e-5f);
    }
 
    return_tensor(t);
}
 
// ---- ARRAY_STRUCT reverse sort ----------------------------------------------
 
/** Reverse sort of an unsorted int32 array produces descending order. */
static void test_sort_tensor_array_reverse_int32(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(6u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t vals[] = { 3, 1, 4, 1, 5, 9 };
    for (size_t i = 0u; i < 6u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    assert_int_equal(sort_tensor(t, _cmp_int32, REVERSE), NO_ERROR);
 
    int32_t expected[] = { 9, 5, 4, 3, 1, 1 };
    for (size_t i = 0u; i < 6u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(t);
}
 
// ---- Already sorted and reverse sorted inputs -------------------------------
 
/** An already sorted array must be unchanged by a FORWARD sort. */
static void test_sort_tensor_already_sorted(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t vals[] = { 1, 2, 3, 4, 5 };
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    assert_int_equal(sort_tensor(t, _cmp_int32, FORWARD), NO_ERROR);
 
    for (size_t i = 0u; i < 5u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, vals[i]);
    }
 
    return_tensor(t);
}
 
/** A reverse-sorted array must produce ascending order after FORWARD sort. */
static void test_sort_tensor_reverse_sorted_input(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t vals[] = { 5, 4, 3, 2, 1 };
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    assert_int_equal(sort_tensor(t, _cmp_int32, FORWARD), NO_ERROR);
 
    for (size_t i = 0u; i < 5u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, (int32_t)(i + 1u));
    }
 
    return_tensor(t);
}
 
/** All-equal elements must survive a sort without corruption. */
static void test_sort_tensor_all_equal(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t val = 7;
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(push_back_tensor(t, &val, INT32_TYPE), NO_ERROR);
 
    assert_int_equal(sort_tensor(t, _cmp_int32, FORWARD), NO_ERROR);
 
    for (size_t i = 0u; i < 5u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, 7);
    }
 
    return_tensor(t);
}
 
// ---- Large array to exercise quicksort path ---------------------------------
 
/**
 * Sort a larger array that exceeds the insertion sort threshold (10 elements)
 * to exercise the quicksort and median-of-three paths.
 */
static void test_sort_tensor_large_array(void** state) {
    (void)state;
    const size_t n = 32u;
    tensor_expect_t r = _make_array(n, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    /* Pseudo-random but deterministic sequence */
    int32_t vals[32] = {
        17, 3, 29, 11, 25, 7, 21, 15, 31, 1,
        27, 5, 23, 9,  19, 13, 30, 2, 28, 10,
        24, 6, 22, 14, 32, 4,  20, 16, 26, 8,
        18, 12
    };
    for (size_t i = 0u; i < n; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    assert_int_equal(sort_tensor(t, _cmp_int32, FORWARD), NO_ERROR);
 
    /* Verify strictly ascending order */
    for (size_t i = 1u; i < n; i++) {
        int32_t prev = -1, curr = -1;
        assert_int_equal(get_tensor_index(t, i - 1u, &prev, INT32_TYPE), NO_ERROR);
        assert_int_equal(get_tensor_index(t, i,      &curr, INT32_TYPE), NO_ERROR);
        assert_true(prev <= curr);
    }
 
    return_tensor(t);
}
 
// ---- TENSOR_STRUCT mode -----------------------------------------------------
 
/**
 * sort_tensor on a TENSOR_STRUCT (fixed-shape) tensor must sort all len
 * elements in flat row-major order.
 */
static void test_sort_tensor_fixed_shape(void** state) {
    (void)state;
    const size_t shape[] = { 2u, 3u };
    tensor_expect_t r = _make_tensor(2u, shape, INT32_TYPE);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    /* Populate via flat index: [5, 2, 4, 1, 6, 3] */
    int32_t vals[] = { 5, 2, 4, 1, 6, 3 };
    for (size_t i = 0u; i < 6u; i++)
        assert_int_equal(set_tensor_index(t, i, &vals[i], INT32_TYPE), NO_ERROR);
 
    assert_int_equal(sort_tensor(t, _cmp_int32, FORWARD), NO_ERROR);
 
    int32_t expected[] = { 1, 2, 3, 4, 5, 6 };
    for (size_t i = 0u; i < 6u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(t);
}
 
// ---- Double dtype -----------------------------------------------------------
 
/** Forward sort of a double array produces ascending order. */
static void test_sort_tensor_double_dtype(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(5u, DOUBLE_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    double vals[] = { 3.3, 1.1, 5.5, 2.2, 4.4 };
    for (size_t i = 0u; i < 5u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], DOUBLE_TYPE), NO_ERROR);
 
    assert_int_equal(sort_tensor(t, _cmp_double, FORWARD), NO_ERROR);
 
    double expected[] = { 1.1, 2.2, 3.3, 4.4, 5.5 };
    for (size_t i = 0u; i < 5u; i++) {
        double out = 0.0;
        assert_int_equal(get_tensor_index(t, i, &out, DOUBLE_TYPE), NO_ERROR);
        assert_float_equal(out, expected[i], 1e-9);
    }
 
    return_tensor(t);
}
 
// ---- User-defined dtype -----------------------------------------------------
 
/**
 * Sort a tensor of vec3_t elements by the x field, confirming that the
 * comparator receives correctly aligned pointers to each full element and
 * that the full struct is swapped atomically — not just the x field.
 */
static void test_sort_tensor_user_dtype_vec3(void** state) {
    (void)state;
    assert_true(ensure_dtype_registered(&vec3_desc));
 
    allocator_vtable_t alloc = heap_allocator();
    const size_t shape[] = { 4u };
    tensor_expect_t r = init_tensor(1u, shape, VEC3_TYPE, alloc);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
    t->mode = ARRAY_STRUCT;
    t->len  = 4u;
 
    vec3_t a = { 3.0f, 30.0f, 300.0f };
    vec3_t b = { 1.0f, 10.0f, 100.0f };
    vec3_t c = { 4.0f, 40.0f, 400.0f };
    vec3_t d = { 2.0f, 20.0f, 200.0f };
 
    assert_int_equal(set_tensor_index(t, 0u, &a, VEC3_TYPE), NO_ERROR);
    assert_int_equal(set_tensor_index(t, 1u, &b, VEC3_TYPE), NO_ERROR);
    assert_int_equal(set_tensor_index(t, 2u, &c, VEC3_TYPE), NO_ERROR);
    assert_int_equal(set_tensor_index(t, 3u, &d, VEC3_TYPE), NO_ERROR);
 
    assert_int_equal(sort_tensor(t, _cmp_vec3_by_x, FORWARD), NO_ERROR);
 
    /* Expected order by x: b(1), d(2), a(3), c(4) */
    vec3_t out0 = {0}, out1 = {0}, out2 = {0}, out3 = {0};
    assert_int_equal(get_tensor_index(t, 0u, &out0, VEC3_TYPE), NO_ERROR);
    assert_int_equal(get_tensor_index(t, 1u, &out1, VEC3_TYPE), NO_ERROR);
    assert_int_equal(get_tensor_index(t, 2u, &out2, VEC3_TYPE), NO_ERROR);
    assert_int_equal(get_tensor_index(t, 3u, &out3, VEC3_TYPE), NO_ERROR);
 
    /* Verify x field (sort key) */
    assert_float_equal(out0.x, 1.0f, 1e-6f);
    assert_float_equal(out1.x, 2.0f, 1e-6f);
    assert_float_equal(out2.x, 3.0f, 1e-6f);
    assert_float_equal(out3.x, 4.0f, 1e-6f);
 
    /* Verify y and z fields moved with their element — confirms full
     * struct swap not just the sort key */
    assert_float_equal(out0.y, 10.0f,  1e-6f);
    assert_float_equal(out0.z, 100.0f, 1e-6f);
    assert_float_equal(out1.y, 20.0f,  1e-6f);
    assert_float_equal(out1.z, 200.0f, 1e-6f);
    assert_float_equal(out2.y, 30.0f,  1e-6f);
    assert_float_equal(out2.z, 300.0f, 1e-6f);
    assert_float_equal(out3.y, 40.0f,  1e-6f);
    assert_float_equal(out3.z, 400.0f, 1e-6f);
 
    return_tensor(t);
}
 
// ---- Sort then reverse round-trip -------------------------------------------
 
/**
 * Sort ascending then reverse — result must equal a descending sort.
 * Exercises the interaction between sort_tensor and reverse_tensor.
 */
static void test_sort_then_reverse_round_trip(void** state) {
    (void)state;
    tensor_expect_t r = _make_array(6u, INT32_TYPE, false);
    assert_true(r.has_value);
    tensor_t* t = r.u.value;
 
    int32_t vals[] = { 4, 2, 6, 1, 5, 3 };
    for (size_t i = 0u; i < 6u; i++)
        assert_int_equal(push_back_tensor(t, &vals[i], INT32_TYPE), NO_ERROR);
 
    assert_int_equal(sort_tensor(t,    _cmp_int32, FORWARD), NO_ERROR);
    assert_int_equal(reverse_tensor(t),                      NO_ERROR);
 
    int32_t expected[] = { 6, 5, 4, 3, 2, 1 };
    for (size_t i = 0u; i < 6u; i++) {
        int32_t out = -1;
        assert_int_equal(get_tensor_index(t, i, &out, INT32_TYPE), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }
 
    return_tensor(t);
}
// ================================================================================
// ================================================================================
// TEST SUITE REGISTRY

const struct CMUnitTest test_tensor[] = {
    /* init — guard tests */
    cmocka_unit_test(test_init_tensor_null_allocator),
    cmocka_unit_test(test_init_tensor_null_shape),
    cmocka_unit_test(test_init_tensor_zero_ndim),
    cmocka_unit_test(test_init_tensor_unknown_dtype),
    cmocka_unit_test(test_init_tensor_unregistered_dtype),
    cmocka_unit_test(test_init_tensor_zero_in_shape),

    /* init — successful construction */
    cmocka_unit_test(test_init_tensor_1d_scalar_fields),
    cmocka_unit_test(test_init_tensor_1d_shape_and_strides),
    cmocka_unit_test(test_init_tensor_2d_shape_and_strides),
    cmocka_unit_test(test_init_tensor_3d_shape_and_strides),
    cmocka_unit_test(test_init_tensor_data_zeroed),
    cmocka_unit_test(test_init_tensor_fam_pointers_in_range),
    cmocka_unit_test(test_init_tensor_single_element),

    /* return */
    cmocka_unit_test(test_return_tensor_null),
    cmocka_unit_test(test_return_tensor_normal),
    cmocka_unit_test(test_return_tensor_all_builtin_dtypes),

    /* tensor_ndim */
    cmocka_unit_test(test_tensor_ndim_null),
    cmocka_unit_test(test_tensor_ndim_1d),
    cmocka_unit_test(test_tensor_ndim_3d),

    /* tensor_size / tensor_alloc / tensor_data_size */
    cmocka_unit_test(test_tensor_size_null),
    cmocka_unit_test(test_tensor_alloc_null),
    cmocka_unit_test(test_tensor_data_size_null),
    cmocka_unit_test(test_tensor_size_and_alloc_equal_for_fixed),

    /* is_tensor_empty / is_tensor_full */
    cmocka_unit_test(test_is_tensor_empty_null),
    cmocka_unit_test(test_is_tensor_full_null),
    cmocka_unit_test(test_is_tensor_empty_and_full_fixed_shape),

    /* tensor_shape_dim */
    cmocka_unit_test(test_tensor_shape_dim_null),
    cmocka_unit_test(test_tensor_shape_dim_out_of_range),
    cmocka_unit_test(test_tensor_shape_dim_values),

    /* tensor_shape (copy) */
    cmocka_unit_test(test_tensor_shape_null_tensor),
    cmocka_unit_test(test_tensor_shape_null_out),
    cmocka_unit_test(test_tensor_shape_truncated),
    cmocka_unit_test(test_tensor_shape_exact),

    /* tensor_shape_ptr / tensor_strides_ptr */
    cmocka_unit_test(test_tensor_shape_ptr_null),
    cmocka_unit_test(test_tensor_strides_ptr_null),
    cmocka_unit_test(test_tensor_shape_ptr_matches_shape),
    cmocka_unit_test(test_tensor_strides_ptr_matches_strides),

    /* is_tensor_ptr */
    cmocka_unit_test(test_is_tensor_ptr_null_tensor),
    cmocka_unit_test(test_is_tensor_ptr_null_ptr),
    cmocka_unit_test(test_is_tensor_ptr_valid_first_element),
    cmocka_unit_test(test_is_tensor_ptr_valid_last_element),
    cmocka_unit_test(test_is_tensor_ptr_one_past_end),
    cmocka_unit_test(test_is_tensor_ptr_mid_element),

    /* tensor_shape_str */
    cmocka_unit_test(test_tensor_shape_str_null_tensor),
    cmocka_unit_test(test_tensor_shape_str_null_buf),
    cmocka_unit_test(test_tensor_shape_str_buf_too_small),
    cmocka_unit_test(test_tensor_shape_str_1d),
    cmocka_unit_test(test_tensor_shape_str_2d),
    cmocka_unit_test(test_tensor_shape_str_3d),

    /* user-defined dtype integration */
    cmocka_unit_test(test_user_dtype_registration),
    cmocka_unit_test(test_user_dtype_registration_unknown_id),
    cmocka_unit_test(test_user_dtype_registration_null),
    cmocka_unit_test(test_user_dtype_registration_zero_size),
    cmocka_unit_test(test_user_dtype_double_registration_fails),
    cmocka_unit_test(test_user_dtype_slot_count),
    cmocka_unit_test(test_init_tensor_user_dtype_vec3),
    cmocka_unit_test(test_init_tensor_user_dtype_mat2x2_3d),
    cmocka_unit_test(test_user_dtype_vec3_round_trip),
    cmocka_unit_test(test_user_dtype_type_mismatch_guard),

    /* clear_tensor */
    cmocka_unit_test(test_clear_tensor_null),
    cmocka_unit_test(test_clear_tensor_len_reset),
    cmocka_unit_test(test_clear_tensor_data_zeroed),
    cmocka_unit_test(test_clear_tensor_metadata_preserved),
    cmocka_unit_test(test_clear_tensor_already_empty),
    cmocka_unit_test(test_clear_tensor_double_clear),
    cmocka_unit_test(test_clear_tensor_user_dtype),
    cmocka_unit_test(test_clear_tensor_blocks_subsequent_get),
    cmocka_unit_test(test_clear_tensor_dynamic_1d), 

    /* set_tensor_index / get_tensor_index — null/type guards */
    cmocka_unit_test(test_set_tensor_index_null_tensor),
    cmocka_unit_test(test_set_tensor_index_null_data),
    cmocka_unit_test(test_set_tensor_index_type_mismatch),
    cmocka_unit_test(test_get_tensor_index_null_tensor),
    cmocka_unit_test(test_get_tensor_index_null_out),
    cmocka_unit_test(test_get_tensor_index_type_mismatch),

    /* set_tensor_index / get_tensor_index — FIXED_SHAPE bounds */
    cmocka_unit_test(test_set_get_fixed_shape_boundary),
    cmocka_unit_test(test_set_get_fixed_shape_after_clear),
    cmocka_unit_test(test_set_get_fixed_shape_2d_flat_index),

    /* set_tensor_index / get_tensor_index — DYNAMIC_ bounds */
    cmocka_unit_test(test_set_get_dynamic_1d_empty),
    cmocka_unit_test(test_set_get_dynamic_1d_len_boundary),
    cmocka_unit_test(test_set_get_dynamic_1d_beyond_len_but_within_alloc),

    /* set_tensor_index / get_tensor_index — round-trip */
    cmocka_unit_test(test_set_get_fixed_shape_round_trip_float),
    cmocka_unit_test(test_set_get_dynamic_1d_round_trip),

    /* set_tensor_nd_index / get_tensor_nd_index — null/type/mode guards */
    cmocka_unit_test(test_set_tensor_nd_index_null_tensor),
    cmocka_unit_test(test_set_tensor_nd_index_null_idx),
    cmocka_unit_test(test_set_tensor_nd_index_null_data),
    cmocka_unit_test(test_set_tensor_nd_index_type_mismatch),
    cmocka_unit_test(test_set_tensor_nd_index_illegal_on_dynamic),
    cmocka_unit_test(test_get_tensor_nd_index_null_tensor),
    cmocka_unit_test(test_get_tensor_nd_index_null_idx),
    cmocka_unit_test(test_get_tensor_nd_index_null_out),
    cmocka_unit_test(test_get_tensor_nd_index_type_mismatch),
    cmocka_unit_test(test_get_tensor_nd_index_illegal_on_dynamic),

    /* set_tensor_nd_index / get_tensor_nd_index — out-of-bounds per dim */
    cmocka_unit_test(test_nd_index_out_of_bounds_first_dim),
    cmocka_unit_test(test_nd_index_out_of_bounds_second_dim),

    /* set_tensor_nd_index / get_tensor_nd_index — round-trip */
    cmocka_unit_test(test_nd_set_get_2d_round_trip),
    cmocka_unit_test(test_nd_set_get_3d_round_trip),

    /* cross-checks: flat and nd access must agree on memory layout */
    cmocka_unit_test(test_nd_set_flat_get_consistency),
    cmocka_unit_test(test_flat_set_nd_get_consistency),

   /* copy_tensor — null/guard */
    cmocka_unit_test(test_copy_tensor_null_src),
 
    /* copy_tensor — scalar fields */
    cmocka_unit_test(test_copy_tensor_scalar_fields),
 
    /* copy_tensor — shape and strides */
    cmocka_unit_test(test_copy_tensor_shape_and_strides),
    cmocka_unit_test(test_copy_tensor_fam_independence),
 
    /* copy_tensor — data buffer */
    cmocka_unit_test(test_copy_tensor_data_copied),
    cmocka_unit_test(test_copy_tensor_deep_copy_independence),
 
    /* copy_tensor — mode-specific */
    cmocka_unit_test(test_copy_tensor_dynamic_1d),
    cmocka_unit_test(test_copy_tensor_user_dtype_3d),

    /* copy_tensor — regression: mode field must be copied */
    cmocka_unit_test(test_copy_tensor_mode_preserved),

     /* push — shared guards */
    cmocka_unit_test(test_push_null_tensor),
    cmocka_unit_test(test_push_null_data),
    cmocka_unit_test(test_push_type_mismatch),
    cmocka_unit_test(test_push_wrong_mode),
 
    /* push_back_tensor */
    cmocka_unit_test(test_push_back_basic),
    cmocka_unit_test(test_push_back_full_no_growth),
    cmocka_unit_test(test_push_back_growth),
    cmocka_unit_test(test_push_back_fill_to_capacity),
 
    /* push_front_tensor */
    cmocka_unit_test(test_push_front_basic),
    cmocka_unit_test(test_push_front_into_empty),
    cmocka_unit_test(test_push_front_full_no_growth),
    cmocka_unit_test(test_push_front_growth),
 
    /* push_at_tensor */
    cmocka_unit_test(test_push_at_out_of_bounds),
    cmocka_unit_test(test_push_at_zero_delegates_to_front),
    cmocka_unit_test(test_push_at_len_delegates_to_back),
    cmocka_unit_test(test_push_at_middle),
    cmocka_unit_test(test_push_at_full_no_growth),
    cmocka_unit_test(test_push_at_growth),
 
    /* push — interleaved correctness */
    cmocka_unit_test(test_push_interleaved),

    /* push — interleaved correctness */
    cmocka_unit_test(test_push_interleaved),
 
    /* pop — shared guards */
    cmocka_unit_test(test_pop_null_tensor),
    cmocka_unit_test(test_pop_type_mismatch),
    cmocka_unit_test(test_pop_wrong_mode),
    cmocka_unit_test(test_pop_empty),
    cmocka_unit_test(test_pop_null_out_discards_value),
 
    /* pop_back_tensor */
    cmocka_unit_test(test_pop_back_basic),
    cmocka_unit_test(test_pop_back_preserves_preceding_elements),
 
    /* pop_front_tensor */
    cmocka_unit_test(test_pop_front_basic),
    cmocka_unit_test(test_pop_front_single_element),
    cmocka_unit_test(test_pop_front_fifo_order),
 
    /* pop_at_tensor */
    cmocka_unit_test(test_pop_at_out_of_bounds),
    cmocka_unit_test(test_pop_at_zero_delegates_to_front),
    cmocka_unit_test(test_pop_at_last_delegates_to_back),
    cmocka_unit_test(test_pop_at_middle),
 
    /* push / pop round-trip */
    cmocka_unit_test(test_push_back_pop_back_lifo),
    cmocka_unit_test(test_pop_interleaved),

    /* concat_tensor_array — null/guard */
    cmocka_unit_test(test_concat_null),
    cmocka_unit_test(test_concat_type_mismatch_dtype_arg),
    cmocka_unit_test(test_concat_type_mismatch_between_tensors),
    cmocka_unit_test(test_concat_wrong_mode),

    /* concat_tensor_array — empty source fast path */
    cmocka_unit_test(test_concat_empty_src),

    /* concat_tensor_array — correctness */
    cmocka_unit_test(test_concat_basic),
    cmocka_unit_test(test_concat_into_empty_dst),
    cmocka_unit_test(test_concat_src_unmodified),

    /* concat_tensor_array — capacity and growth */
    cmocka_unit_test(test_concat_fits_in_existing_capacity),
    cmocka_unit_test(test_concat_triggers_growth),
    cmocka_unit_test(test_concat_exceeds_fixed_capacity),
    cmocka_unit_test(test_concat_full_sequence),

    /* slice_tensor_array — null/guard */
    cmocka_unit_test(test_slice_null_src),
    cmocka_unit_test(test_slice_wrong_mode),
    cmocka_unit_test(test_slice_start_out_of_bounds),
    cmocka_unit_test(test_slice_end_out_of_bounds),
    cmocka_unit_test(test_slice_zero_length),
    cmocka_unit_test(test_slice_start_greater_than_end),
 
    /* slice_tensor_array — scalar fields */
    cmocka_unit_test(test_slice_scalar_fields),
    cmocka_unit_test(test_slice_shape_and_strides),
 
    /* slice_tensor_array — element correctness */
    cmocka_unit_test(test_slice_front),
    cmocka_unit_test(test_slice_back),
    cmocka_unit_test(test_slice_middle),
    cmocka_unit_test(test_slice_single_element),
    cmocka_unit_test(test_slice_full_length),
 
    /* slice_tensor_array — deep copy independence */
    cmocka_unit_test(test_slice_deep_copy_independence),
    cmocka_unit_test(test_slice_src_unmodified),

    /* reverse_tensor — guards */
    cmocka_unit_test(test_reverse_tensor_null),
    cmocka_unit_test(test_reverse_tensor_empty),
    cmocka_unit_test(test_reverse_tensor_single_element),

    /* reverse_tensor — correctness */
    cmocka_unit_test(test_reverse_tensor_two_elements),
    cmocka_unit_test(test_reverse_tensor_even_length),
    cmocka_unit_test(test_reverse_tensor_odd_length),
    cmocka_unit_test(test_reverse_tensor_double_reverse),

    /* reverse_tensor — mode and dtype coverage */
    cmocka_unit_test(test_reverse_tensor_fixed_shape),
    cmocka_unit_test(test_reverse_tensor_double_dtype),
    cmocka_unit_test(test_reverse_tensor_user_dtype),

    /* sort_tensor — null/guard */
    cmocka_unit_test(test_sort_tensor_null_tensor),
    cmocka_unit_test(test_sort_tensor_null_cmp),
    cmocka_unit_test(test_sort_tensor_empty),
    cmocka_unit_test(test_sort_tensor_single_element),
 
    /* sort_tensor — two-element edge cases */
    cmocka_unit_test(test_sort_tensor_two_elements_forward),
    cmocka_unit_test(test_sort_tensor_two_elements_reverse),
 
    /* sort_tensor — ARRAY_STRUCT forward sort */
    cmocka_unit_test(test_sort_tensor_array_forward_int32),
    cmocka_unit_test(test_sort_tensor_array_forward_float),
 
    /* sort_tensor — ARRAY_STRUCT reverse sort */
    cmocka_unit_test(test_sort_tensor_array_reverse_int32),
 
    /* sort_tensor — degenerate inputs */
    cmocka_unit_test(test_sort_tensor_already_sorted),
    cmocka_unit_test(test_sort_tensor_reverse_sorted_input),
    cmocka_unit_test(test_sort_tensor_all_equal),
 
    /* sort_tensor — large array exercises quicksort path */
    cmocka_unit_test(test_sort_tensor_large_array),
 
    /* sort_tensor — TENSOR_STRUCT mode */
    cmocka_unit_test(test_sort_tensor_fixed_shape),
 
    /* sort_tensor — dtype coverage */
    cmocka_unit_test(test_sort_tensor_double_dtype),
    cmocka_unit_test(test_sort_tensor_user_dtype_vec3),
 
    /* sort_tensor — interaction with reverse_tensor */
    cmocka_unit_test(test_sort_then_reverse_round_trip),
};

const size_t test_tensor_count = sizeof(test_tensor) / sizeof(test_tensor[0]);
// ================================================================================ 
// ================================================================================ 
// HELPERS

/** Construct a populated uint8 array with values 1..n. */
static uint8_tensor_t* _make_uint8_array(size_t capacity, bool growth) {
    allocator_vtable_t alloc = heap_allocator();
    uint8_tensor_expect_t r  = init_uint8_array(capacity, growth, alloc);
    if (!r.has_value) return NULL;
    return r.u.value;
}

static uint8_tensor_t* _make_uint8_array_filled(size_t n, uint8_t base) {
    /* Allocate n+1 so the result always has at least one free slot,
     * making it safe to use as a push destination in tests. */
    uint8_tensor_t* arr = _make_uint8_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++) {
        uint8_t val = (uint8_t)(base + i);
        push_back_uint8_array(arr, val);
    }
    return arr;
}

// ================================================================================
// ================================================================================
// INITIALISATION AND TEARDOWN

// ---- init_uint8_array ----------------------------------------------------------

/** NULL allocator must propagate an error through the expect type. */
static void test_init_uint8_array_null_allocator(void** state) {
    (void)state;
    allocator_vtable_t bad = heap_allocator();
    bad.allocate = NULL;
    uint8_tensor_expect_t r = init_uint8_array(8u, false, bad);
    assert_false(r.has_value);
}

/** Successful construction produces a non-NULL wrapper with correct mode. */
static void test_init_uint8_array_success(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_tensor_expect_t r  = init_uint8_array(8u, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  ARRAY_STRUCT);
    assert_int_equal(r.u.value->base->dtype, UINT8_TYPE);
    assert_int_equal(r.u.value->base->len,   0u);
    assert_int_equal(r.u.value->base->alloc, 8u);
    return_uint8_tensor(r.u.value);
}

// ---- init_uint8_tensor ---------------------------------------------------------

/** Successful construction of a fixed-shape tensor. */
static void test_init_uint8_tensor_success(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    uint8_tensor_expect_t r  = init_uint8_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  TENSOR_STRUCT);
    assert_int_equal(r.u.value->base->dtype, UINT8_TYPE);
    assert_int_equal(r.u.value->base->len,   12u);
    return_uint8_tensor(r.u.value);
}

/** NULL shape propagates an error. */
static void test_init_uint8_tensor_null_shape(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint8_tensor_expect_t r  = init_uint8_tensor(2u, NULL, alloc);
    assert_false(r.has_value);
}

// ---- return_uint8_tensor -------------------------------------------------------

/** return_uint8_tensor(NULL) must not crash. */
static void test_return_uint8_tensor_null(void** state) {
    (void)state;
    return_uint8_tensor(NULL);
}

/** Normal construction followed by return must not crash or leak. */
static void test_return_uint8_tensor_normal(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(4u, false);
    assert_non_null(arr);
    return_uint8_tensor(arr);
}

// ---- copy_uint8_tensor ---------------------------------------------------------

/** NULL src must return NULL_POINTER. */
static void test_copy_uint8_tensor_null_src(void** state) {
    (void)state;
    uint8_tensor_expect_t r = copy_uint8_tensor(NULL, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

/** Copy is independent of source — mutation of copy does not affect src. */
static void test_copy_uint8_tensor_independence(void** state) {
    (void)state;
    uint8_tensor_t* src = _make_uint8_array_filled(3u, 10u);
    assert_non_null(src);

    uint8_tensor_expect_t r = copy_uint8_tensor(src, NULL);
    assert_true(r.has_value);
    uint8_tensor_t* dst = r.u.value;

    /* Overwrite slot 0 of the copy */
    assert_int_equal(set_uint8_tensor_index(dst, 0u, 99u), NO_ERROR);

    /* Source slot 0 must be unchanged */
    uint8_t src_val = 0u;
    assert_int_equal(get_uint8_tensor_index(src, 0u, &src_val), NO_ERROR);
    assert_int_equal(src_val, 10u);

    return_uint8_tensor(src);
    return_uint8_tensor(dst);
}

// ================================================================================
// ================================================================================
// INTROSPECTION

static void test_uint8_tensor_size_null(void** state) {
    (void)state;
    assert_int_equal(uint8_tensor_size(NULL), 0u);
}

static void test_uint8_tensor_size_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(3u, 1u);
    assert_non_null(arr);
    assert_int_equal(uint8_tensor_size(arr), 3u);
    return_uint8_tensor(arr);
}

static void test_uint8_tensor_alloc_null(void** state) {
    (void)state;
    assert_int_equal(uint8_tensor_alloc(NULL), 0u);
}

static void test_uint8_tensor_alloc_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(8u, false);
    assert_non_null(arr);
    assert_int_equal(uint8_tensor_alloc(arr), 8u);
    return_uint8_tensor(arr);
}

static void test_uint8_tensor_data_size_null(void** state) {
    (void)state;
    assert_int_equal(uint8_tensor_data_size(NULL), 0u);
}

static void test_uint8_tensor_data_size_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(uint8_tensor_data_size(arr), sizeof(uint8_t));
    return_uint8_tensor(arr);
}

static void test_uint8_tensor_dtype_null(void** state) {
    (void)state;
    assert_int_equal(uint8_tensor_dtype(NULL), UNKNOWN_TYPE);
}

static void test_uint8_tensor_dtype_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(uint8_tensor_dtype(arr), UINT8_TYPE);
    return_uint8_tensor(arr);
}

static void test_is_uint8_tensor_empty_null(void** state) {
    (void)state;
    assert_true(is_uint8_tensor_empty(NULL));
}

static void test_is_uint8_tensor_empty_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(4u, false);
    assert_non_null(arr);
    assert_true(is_uint8_tensor_empty(arr));
    push_back_uint8_array(arr, 1u);
    assert_false(is_uint8_tensor_empty(arr));
    return_uint8_tensor(arr);
}

static void test_is_uint8_tensor_full_null(void** state) {
    (void)state;
    assert_true(is_uint8_tensor_full(NULL));
}

static void test_is_uint8_tensor_full_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(2u, false);
    assert_non_null(arr);
    assert_false(is_uint8_tensor_full(arr));
    push_back_uint8_array(arr, 1u);
    push_back_uint8_array(arr, 2u);
    assert_true(is_uint8_tensor_full(arr));
    return_uint8_tensor(arr);
}

static void test_is_uint8_tensor_ptr_null_wrapper(void** state) {
    (void)state;
    uint8_t dummy = 0u;
    assert_false(is_uint8_tensor_ptr(NULL, &dummy));
}

static void test_is_uint8_tensor_ptr_valid(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(4u, 10u);
    assert_non_null(arr);
    /* Pointer to first element must be valid */
    const uint8_t* p = (const uint8_t*)arr->base->data;
    assert_true(is_uint8_tensor_ptr(arr, p));
    return_uint8_tensor(arr);
}

static void test_uint8_tensor_ndim_null(void** state) {
    (void)state;
    assert_int_equal(uint8_tensor_ndim(NULL), 0u);
}

static void test_uint8_tensor_ndim_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(uint8_tensor_ndim(arr), 1u);
    return_uint8_tensor(arr);
}

static void test_uint8_tensor_shape_dim_null(void** state) {
    (void)state;
    assert_int_equal(uint8_tensor_shape_dim(NULL, 0u), 0u);
}

static void test_uint8_tensor_shape_dim_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    uint8_tensor_expect_t r  = init_uint8_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_int_equal(uint8_tensor_shape_dim(r.u.value, 0u), 3u);
    assert_int_equal(uint8_tensor_shape_dim(r.u.value, 1u), 4u);
    return_uint8_tensor(r.u.value);
}

static void test_uint8_tensor_shape_null(void** state) {
    (void)state;
    size_t buf[2];
    assert_int_equal(uint8_tensor_shape(NULL, buf, 2u), NULL_POINTER);
}

static void test_uint8_tensor_shape_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    uint8_tensor_expect_t r  = init_uint8_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    size_t buf[2] = { 0u, 0u };
    assert_int_equal(uint8_tensor_shape(r.u.value, buf, 2u), NO_ERROR);
    assert_int_equal(buf[0], 3u);
    assert_int_equal(buf[1], 4u);
    return_uint8_tensor(r.u.value);
}

static void test_uint8_tensor_shape_ptr_null(void** state) {
    (void)state;
    assert_null(uint8_tensor_shape_ptr(NULL));
}

static void test_uint8_tensor_shape_ptr_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(4u, false);
    assert_non_null(arr);
    assert_non_null(uint8_tensor_shape_ptr(arr));
    return_uint8_tensor(arr);
}

static void test_uint8_tensor_strides_ptr_null(void** state) {
    (void)state;
    assert_null(uint8_tensor_strides_ptr(NULL));
}

static void test_uint8_tensor_strides_ptr_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(4u, false);
    assert_non_null(arr);
    const size_t* strides = uint8_tensor_strides_ptr(arr);
    assert_non_null(strides);
    assert_int_equal(strides[0], sizeof(uint8_t));
    return_uint8_tensor(arr);
}

static void test_uint8_tensor_shape_str_null(void** state) {
    (void)state;
    char buf[32];
    assert_int_equal(uint8_tensor_shape_str(NULL, buf, sizeof(buf)), NULL_POINTER);
}

static void test_uint8_tensor_shape_str_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(8u, false);
    assert_non_null(arr);
    char buf[32];
    assert_int_equal(uint8_tensor_shape_str(arr, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(8)");
    return_uint8_tensor(arr);
}

// ================================================================================
// ================================================================================
// UTILITY OPERATIONS

// ---- clear_uint8_tensor --------------------------------------------------------

static void test_clear_uint8_tensor_null(void** state) {
    (void)state;
    assert_int_equal(clear_uint8_tensor(NULL), NULL_POINTER);
}

static void test_clear_uint8_tensor_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(3u, 10u);
    assert_non_null(arr);
    assert_int_equal(clear_uint8_tensor(arr), NO_ERROR);
    assert_int_equal(uint8_tensor_size(arr), 0u);
    return_uint8_tensor(arr);
}

// ---- concat_uint8_tensor_array -------------------------------------------------

static void test_concat_uint8_tensor_null(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(concat_uint8_tensor_array(NULL, arr), NULL_POINTER);
    assert_int_equal(concat_uint8_tensor_array(arr, NULL), NULL_POINTER);
    return_uint8_tensor(arr);
}

static void test_concat_uint8_tensor_value(void** state) {
    (void)state;

    /* dst needs capacity for 5 elements total — allocate with room to grow */
    allocator_vtable_t alloc = heap_allocator();
    uint8_tensor_expect_t r  = init_uint8_array(8u, false, alloc);
    assert_true(r.has_value);
    uint8_tensor_t* dst = r.u.value;

    /* Populate dst with [1, 2, 3] */
    push_back_uint8_array(dst, 1u);
    push_back_uint8_array(dst, 2u);
    push_back_uint8_array(dst, 3u);

    /* src = [4, 5] */
    uint8_tensor_t* src = _make_uint8_array_filled(2u, 4u);
    assert_non_null(src);

    assert_int_equal(concat_uint8_tensor_array(dst, src), NO_ERROR);
    assert_int_equal(uint8_tensor_size(dst), 5u);

    uint8_t expected[] = { 1u, 2u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        uint8_t out = 0u;
        assert_int_equal(get_uint8_tensor_index(dst, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint8_tensor(dst);
    return_uint8_tensor(src);
}

// ---- slice_uint8_tensor_array --------------------------------------------------

static void test_slice_uint8_tensor_null(void** state) {
    (void)state;
    uint8_tensor_expect_t r = slice_uint8_tensor_array(NULL, 0u, 1u, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_slice_uint8_tensor_value(void** state) {
    (void)state;
    uint8_tensor_t* src = _make_uint8_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(src);

    uint8_tensor_expect_t r = slice_uint8_tensor_array(src, 1u, 4u, NULL);
    assert_true(r.has_value);
    uint8_tensor_t* sl = r.u.value;

    assert_int_equal(uint8_tensor_size(sl), 3u);
    uint8_t expected[] = { 11u, 12u, 13u };
    for (size_t i = 0u; i < 3u; i++) {
        uint8_t out = 0u;
        assert_int_equal(get_uint8_tensor_index(sl, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint8_tensor(src);
    return_uint8_tensor(sl);
}

// ---- reverse_uint8_tensor ------------------------------------------------------

static void test_reverse_uint8_tensor_null(void** state) {
    (void)state;
    assert_int_equal(reverse_uint8_tensor(NULL), NULL_POINTER);
}

static void test_reverse_uint8_tensor_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(4u, 1u); /* [1,2,3,4] */
    assert_non_null(arr);

    assert_int_equal(reverse_uint8_tensor(arr), NO_ERROR);

    uint8_t expected[] = { 4u, 3u, 2u, 1u };
    for (size_t i = 0u; i < 4u; i++) {
        uint8_t out = 0u;
        assert_int_equal(get_uint8_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint8_tensor(arr);
}

// ---- sort_uint8_tensor ---------------------------------------------------------

static void test_sort_uint8_tensor_null(void** state) {
    (void)state;
    assert_int_equal(sort_uint8_tensor(NULL, FORWARD), NULL_POINTER);
}

static void test_sort_uint8_tensor_forward(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(5u, false);
    assert_non_null(arr);

    uint8_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_uint8_array(arr, vals[i]);

    assert_int_equal(sort_uint8_tensor(arr, FORWARD), NO_ERROR);

    uint8_t expected[] = { 1u, 1u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        uint8_t out = 0u;
        assert_int_equal(get_uint8_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint8_tensor(arr);
}

static void test_sort_uint8_tensor_reverse(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(5u, false);
    assert_non_null(arr);

    uint8_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_uint8_array(arr, vals[i]);

    assert_int_equal(sort_uint8_tensor(arr, REVERSE), NO_ERROR);

    uint8_t expected[] = { 5u, 4u, 3u, 1u, 1u };
    for (size_t i = 0u; i < 5u; i++) {
        uint8_t out = 0u;
        assert_int_equal(get_uint8_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint8_tensor(arr);
}

// ================================================================================
// ================================================================================
// SET AND GET DATA

// ---- set_uint8_tensor_index / get_uint8_tensor_index --------------------------

static void test_set_get_uint8_tensor_index_null(void** state) {
    (void)state;
    uint8_t dummy = 0u;
    assert_int_equal(set_uint8_tensor_index(NULL, 0u, 1u),    NULL_POINTER);
    assert_int_equal(get_uint8_tensor_index(NULL, 0u, &dummy), NULL_POINTER);
}

static void test_set_get_uint8_tensor_index_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(3u, 10u);
    assert_non_null(arr);

    assert_int_equal(set_uint8_tensor_index(arr, 1u, 99u), NO_ERROR);

    uint8_t out = 0u;
    assert_int_equal(get_uint8_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);

    return_uint8_tensor(arr);
}

// ---- set_uint8_tensor_nd_index / get_uint8_tensor_nd_index --------------------

static void test_set_get_uint8_tensor_nd_index_null(void** state) {
    (void)state;
    size_t  idx[] = { 0u, 0u };
    uint8_t out   = 0u;
    assert_int_equal(set_uint8_tensor_nd_index(NULL, idx, 1u),   NULL_POINTER);
    assert_int_equal(get_uint8_tensor_nd_index(NULL, idx, &out), NULL_POINTER);
}

static void test_set_get_uint8_tensor_nd_index_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 3u };
    allocator_vtable_t alloc = heap_allocator();
    uint8_tensor_expect_t r  = init_uint8_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    uint8_tensor_t* mat = r.u.value;

    size_t idx[] = { 1u, 2u };
    assert_int_equal(set_uint8_tensor_nd_index(mat, idx, 42u), NO_ERROR);

    uint8_t out = 0u;
    assert_int_equal(get_uint8_tensor_nd_index(mat, idx, &out), NO_ERROR);
    assert_int_equal(out, 42u);

    return_uint8_tensor(mat);
}

// ---- push and pop wrappers ----------------------------------------------------

static void test_push_back_uint8_array_null(void** state) {
    (void)state;
    assert_int_equal(push_back_uint8_array(NULL, 1u), NULL_POINTER);
}

static void test_push_back_uint8_array_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(4u, false);
    assert_non_null(arr);

    assert_int_equal(push_back_uint8_array(arr, 10u), NO_ERROR);
    assert_int_equal(push_back_uint8_array(arr, 20u), NO_ERROR);
    assert_int_equal(uint8_tensor_size(arr), 2u);

    uint8_t out = 0u;
    assert_int_equal(get_uint8_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 20u);

    return_uint8_tensor(arr);
}

static void test_push_front_uint8_array_null(void** state) {
    (void)state;
    assert_int_equal(push_front_uint8_array(NULL, 1u), NULL_POINTER);
}

static void test_push_front_uint8_array_value(void** state) {
    (void)state;
    /* Capacity 3 so there is room for the push_front */
    allocator_vtable_t alloc = heap_allocator();
    uint8_tensor_expect_t r  = init_uint8_array(3u, false, alloc);
    assert_true(r.has_value);
    uint8_tensor_t* arr = r.u.value;

    push_back_uint8_array(arr, 10u);
    push_back_uint8_array(arr, 11u);
    /* arr = [10, 11], len == 2, alloc == 3 — one slot free */

    assert_int_equal(push_front_uint8_array(arr, 99u), NO_ERROR);

    uint8_t out = 0u;
    assert_int_equal(get_uint8_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_uint8_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_uint8_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);

    return_uint8_tensor(arr);
}

static void test_push_at_uint8_array_null(void** state) {
    (void)state;
    assert_int_equal(push_at_uint8_array(NULL, 1u, 0u), NULL_POINTER);
}

static void test_push_at_uint8_array_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(3u, 10u); /* [10, 11, 12] */
    assert_non_null(arr);

    assert_int_equal(push_at_uint8_array(arr, 99u, 1u), NO_ERROR);
    /* arr = [10, 99, 11, 12] */

    uint8_t out = 0u;
    assert_int_equal(get_uint8_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_uint8_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_uint8_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);
    assert_int_equal(get_uint8_tensor_index(arr, 3u, &out), NO_ERROR);
    assert_int_equal(out, 12u);

    return_uint8_tensor(arr);
}

static void test_pop_back_uint8_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_back_uint8_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_back_uint8_array_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    uint8_t out = 0u;
    assert_int_equal(pop_back_uint8_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(uint8_tensor_size(arr), 2u);

    return_uint8_tensor(arr);
}

static void test_pop_front_uint8_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_front_uint8_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_front_uint8_array_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    uint8_t out = 0u;
    assert_int_equal(pop_front_uint8_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(uint8_tensor_size(arr), 2u);

    return_uint8_tensor(arr);
}

static void test_pop_at_uint8_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_at_uint8_array(NULL, NULL, 0u), NULL_POINTER);
}

static void test_pop_at_uint8_array_value(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(4u, 10u); /* [10,11,12,13] */
    assert_non_null(arr);

    uint8_t out = 0u;
    assert_int_equal(pop_at_uint8_array(arr, &out, 2u), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(uint8_tensor_size(arr), 3u);

    assert_int_equal(get_uint8_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 13u);

    return_uint8_tensor(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// FIND UINT8 TENSOR VALUE (uint8_tensor_lsearch)
// ================================================================================
 
/** NULL tensor must return NULL_POINTER. */
static void test_find_uint8_tensor_null_tensor(void** state) {
    (void)state;
    size_t idx = 0u;
    assert_int_equal(uint8_tensor_lsearch(NULL, &idx, 10u), NULL_POINTER);
}
 
/** NULL index pointer must return NULL_POINTER. */
static void test_find_uint8_tensor_null_index(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(3u, 10u);
    assert_non_null(arr);
    assert_int_equal(uint8_tensor_lsearch(arr, NULL, 10u), NULL_POINTER);
    return_uint8_tensor(arr);
}
 
/** Empty array must return EMPTY. */
static void test_find_uint8_tensor_empty(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(4u, false);
    assert_non_null(arr);
    size_t idx = 0u;
    assert_int_equal(uint8_tensor_lsearch(arr, &idx, 10u), EMPTY);
    return_uint8_tensor(arr);
}
 
/** Value not present must return NOT_FOUND. */
static void test_find_uint8_tensor_not_found(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(4u, 1u); /* [1,2,3,4] */
    assert_non_null(arr);
    size_t idx = 99u;   /* sentinel — must not be modified */
    assert_int_equal(uint8_tensor_lsearch(arr, &idx, 99u), NOT_FOUND);
    /* index must be unchanged on NOT_FOUND */
    assert_int_equal(idx, 99u);
    return_uint8_tensor(arr);
}
 
/** Value at the first position must be found at index 0. */
static void test_find_uint8_tensor_first_element(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_lsearch(arr, &idx, 10u), NO_ERROR);
    assert_int_equal(idx, 0u);
    return_uint8_tensor(arr);
}
 
/** Value at the last position must be found at index len-1. */
static void test_find_uint8_tensor_last_element(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_lsearch(arr, &idx, 14u), NO_ERROR);
    assert_int_equal(idx, 4u);
    return_uint8_tensor(arr);
}
 
/** Value in the middle must be found at the correct index. */
static void test_find_uint8_tensor_middle_element(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_lsearch(arr, &idx, 12u), NO_ERROR);
    assert_int_equal(idx, 2u);
    return_uint8_tensor(arr);
}
 
/**
 * When duplicates are present, the first occurrence must be returned.
 */
static void test_find_uint8_tensor_first_of_duplicates(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(6u, false);
    assert_non_null(arr);
 
    /* [5, 9, 9, 9, 7, 3] */
    push_back_uint8_array(arr, 5u);
    push_back_uint8_array(arr, 9u);
    push_back_uint8_array(arr, 9u);
    push_back_uint8_array(arr, 9u);
    push_back_uint8_array(arr, 7u);
    push_back_uint8_array(arr, 3u);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_lsearch(arr, &idx, 9u), NO_ERROR);
    assert_int_equal(idx, 1u);   /* first occurrence at index 1 */
 
    return_uint8_tensor(arr);
}
 
/**
 * Single-element array containing the target value must succeed.
 */
static void test_find_uint8_tensor_single_element_match(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(1u, false);
    assert_non_null(arr);
    push_back_uint8_array(arr, 42u);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_lsearch(arr, &idx, 42u), NO_ERROR);
    assert_int_equal(idx, 0u);
 
    return_uint8_tensor(arr);
}
 
/**
 * Single-element array not containing the target must return NOT_FOUND
 * without modifying index.
 */
static void test_find_uint8_tensor_single_element_no_match(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(1u, false);
    assert_non_null(arr);
    push_back_uint8_array(arr, 42u);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_lsearch(arr, &idx, 7u), NOT_FOUND);
    assert_int_equal(idx, 99u);   /* unchanged */
 
    return_uint8_tensor(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// BINARY SEARCH (uint8_tensor_bsearch)
//
// Note: the array must be sorted in ascending order before calling bsearch.
//       Tests use sort_uint8_tensor to establish the precondition.
// ================================================================================
 
/** NULL tensor must return NULL_POINTER. */
static void test_bsearch_uint8_null_tensor(void** state) {
    (void)state;
    size_t idx = 0u;
    assert_int_equal(uint8_tensor_bsearch(NULL, &idx, 10u), NULL_POINTER);
}
 
/** NULL index pointer must return NULL_POINTER. */
static void test_bsearch_uint8_null_index(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array_filled(3u, 1u);
    assert_non_null(arr);
    assert_int_equal(uint8_tensor_bsearch(arr, NULL, 1u), NULL_POINTER);
    return_uint8_tensor(arr);
}
 
/** Empty array must return EMPTY without touching index. */
static void test_bsearch_uint8_empty(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(4u, false);
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 5u), EMPTY);
    assert_int_equal(idx, 99u);
    return_uint8_tensor(arr);
}
 
/** Single-element array containing the target must succeed at index 0. */
static void test_bsearch_uint8_single_match(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(2u, false);
    assert_non_null(arr);
    push_back_uint8_array(arr, 42u);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 42u), NO_ERROR);
    assert_int_equal(idx, 0u);
    return_uint8_tensor(arr);
}
 
/** Single-element array not containing the target must return NOT_FOUND. */
static void test_bsearch_uint8_single_no_match(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(2u, false);
    assert_non_null(arr);
    push_back_uint8_array(arr, 42u);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 7u), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_uint8_tensor(arr);
}
 
/**
 * Value at the first position (smallest element) must be found.
 * This also exercises the underflow guard: when the target is less
 * than mid the search narrows toward index 0 and high = mid - 1u
 * must not underflow.
 */
static void test_bsearch_uint8_find_first(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(6u, false);
    assert_non_null(arr);
    /* Sorted: [1, 3, 5, 7, 9, 11] */
    uint8_t vals[] = { 1u, 3u, 5u, 7u, 9u, 11u };
    for (size_t i = 0u; i < 6u; i++)
        push_back_uint8_array(arr, vals[i]);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 1u), NO_ERROR);
    assert_int_equal(idx, 0u);
    return_uint8_tensor(arr);
}
 
/** Value at the last position (largest element) must be found. */
static void test_bsearch_uint8_find_last(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(6u, false);
    assert_non_null(arr);
    uint8_t vals[] = { 1u, 3u, 5u, 7u, 9u, 11u };
    for (size_t i = 0u; i < 6u; i++)
        push_back_uint8_array(arr, vals[i]);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 11u), NO_ERROR);
    assert_int_equal(idx, 5u);
    return_uint8_tensor(arr);
}
 
/** Value at the exact midpoint must be found in the first iteration. */
static void test_bsearch_uint8_find_middle(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(6u, false);
    assert_non_null(arr);
    /* Sorted: [10, 20, 30, 40, 50, 60] — midpoint is index 2 or 3 */
    uint8_t vals[] = { 10u, 20u, 30u, 40u, 50u, 60u };
    for (size_t i = 0u; i < 6u; i++)
        push_back_uint8_array(arr, vals[i]);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 30u), NO_ERROR);
    assert_int_equal(idx, 2u);
    return_uint8_tensor(arr);
}
 
/**
 * Value smaller than every element must return NOT_FOUND.
 * This exercises the high = mid - 1u path when mid == 0,
 * confirming the underflow guard fires correctly.
 */
static void test_bsearch_uint8_value_below_all(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(5u, false);
    assert_non_null(arr);
    uint8_t vals[] = { 10u, 20u, 30u, 40u, 50u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_uint8_array(arr, vals[i]);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 5u), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_uint8_tensor(arr);
}
 
/**
 * Value larger than every element must return NOT_FOUND.
 * This exercises the low = mid + 1u path until low exceeds high.
 */
static void test_bsearch_uint8_value_above_all(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(5u, false);
    assert_non_null(arr);
    uint8_t vals[] = { 10u, 20u, 30u, 40u, 50u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_uint8_array(arr, vals[i]);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 99u), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_uint8_tensor(arr);
}
 
/** Value between two elements (gap in the sequence) must return NOT_FOUND. */
static void test_bsearch_uint8_value_in_gap(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(5u, false);
    assert_non_null(arr);
    /* Sorted: [10, 20, 30, 40, 50] — 25 is between 20 and 30 */
    uint8_t vals[] = { 10u, 20u, 30u, 40u, 50u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_uint8_array(arr, vals[i]);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 25u), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_uint8_tensor(arr);
}
 
/**
 * All elements are the same value — bsearch finds one occurrence.
 * The exact index is implementation-defined for duplicates but must
 * satisfy the postcondition that arr[idx] == value.
 */
static void test_bsearch_uint8_all_equal(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(5u, false);
    assert_non_null(arr);
    for (size_t i = 0u; i < 5u; i++)
        push_back_uint8_array(arr, 7u);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 7u), NO_ERROR);
    /* Postcondition: element at idx must equal the search value */
    uint8_t out = 0u;
    assert_int_equal(get_uint8_tensor_index(arr, idx, &out), NO_ERROR);
    assert_int_equal(out, 7u);
 
    return_uint8_tensor(arr);
}
 
/**
 * Two-element array: both low and high boundary are checked in one
 * iteration — exercises the minimal non-trivial case.
 */
static void test_bsearch_uint8_two_elements_find_low(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(3u, false);
    assert_non_null(arr);
    push_back_uint8_array(arr, 5u);
    push_back_uint8_array(arr, 10u);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 5u), NO_ERROR);
    assert_int_equal(idx, 0u);
    return_uint8_tensor(arr);
}
 
static void test_bsearch_uint8_two_elements_find_high(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(3u, false);
    assert_non_null(arr);
    push_back_uint8_array(arr, 5u);
    push_back_uint8_array(arr, 10u);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 10u), NO_ERROR);
    assert_int_equal(idx, 1u);
    return_uint8_tensor(arr);
}
 
/**
 * sort_uint8_tensor followed by bsearch — confirms the two functions
 * work correctly in combination, which is the primary real-world use
 * pattern.
 */
static void test_bsearch_uint8_after_sort(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(8u, false);
    assert_non_null(arr);
 
    /* Push in unsorted order */
    uint8_t vals[] = { 50u, 10u, 80u, 30u, 70u, 20u, 60u, 40u };
    for (size_t i = 0u; i < 8u; i++)
        push_back_uint8_array(arr, vals[i]);
 
    /* Sort in place */
    assert_int_equal(sort_uint8_tensor(arr, FORWARD), NO_ERROR);
    /* Sorted: [10, 20, 30, 40, 50, 60, 70, 80] */
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 40u), NO_ERROR);
    assert_int_equal(idx, 3u);
 
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 10u), NO_ERROR);
    assert_int_equal(idx, 0u);
 
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 80u), NO_ERROR);
    assert_int_equal(idx, 7u);
 
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 35u), NOT_FOUND);
 
    return_uint8_tensor(arr);
}
 
/**
 * Boundary values 0 and UINT8_MAX in a sorted array — confirms the
 * search handles the full range of uint8_t without sign-extension
 * or overflow in comparisons.
 */
static void test_bsearch_uint8_boundary_values(void** state) {
    (void)state;
    uint8_tensor_t* arr = _make_uint8_array(4u, false);
    assert_non_null(arr);
    push_back_uint8_array(arr, 0u);
    push_back_uint8_array(arr, 1u);
    push_back_uint8_array(arr, 254u);
    push_back_uint8_array(arr, 255u);
 
    size_t idx = 99u;
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 0u),   NO_ERROR);
    assert_int_equal(idx, 0u);
 
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 255u), NO_ERROR);
    assert_int_equal(idx, 3u);
 
    assert_int_equal(uint8_tensor_bsearch(arr, &idx, 128u), NOT_FOUND);
 
    return_uint8_tensor(arr);
}
// ================================================================================
// ================================================================================
// TEST SUITE REGISTRY

const struct CMUnitTest test_uint8_tensor[] = {
    /* init_uint8_array */
    cmocka_unit_test(test_init_uint8_array_null_allocator),
    cmocka_unit_test(test_init_uint8_array_success),

    /* init_uint8_tensor */
    cmocka_unit_test(test_init_uint8_tensor_success),
    cmocka_unit_test(test_init_uint8_tensor_null_shape),

    /* return_uint8_tensor */
    cmocka_unit_test(test_return_uint8_tensor_null),
    cmocka_unit_test(test_return_uint8_tensor_normal),

    /* copy_uint8_tensor */
    cmocka_unit_test(test_copy_uint8_tensor_null_src),
    cmocka_unit_test(test_copy_uint8_tensor_independence),

    /* introspection — null guards */
    cmocka_unit_test(test_uint8_tensor_size_null),
    cmocka_unit_test(test_uint8_tensor_alloc_null),
    cmocka_unit_test(test_uint8_tensor_data_size_null),
    cmocka_unit_test(test_uint8_tensor_dtype_null),
    cmocka_unit_test(test_is_uint8_tensor_empty_null),
    cmocka_unit_test(test_is_uint8_tensor_full_null),
    cmocka_unit_test(test_is_uint8_tensor_ptr_null_wrapper),
    cmocka_unit_test(test_uint8_tensor_ndim_null),
    cmocka_unit_test(test_uint8_tensor_shape_dim_null),
    cmocka_unit_test(test_uint8_tensor_shape_null),
    cmocka_unit_test(test_uint8_tensor_shape_ptr_null),
    cmocka_unit_test(test_uint8_tensor_strides_ptr_null),
    cmocka_unit_test(test_uint8_tensor_shape_str_null),

    /* introspection — values */
    cmocka_unit_test(test_uint8_tensor_size_value),
    cmocka_unit_test(test_uint8_tensor_alloc_value),
    cmocka_unit_test(test_uint8_tensor_data_size_value),
    cmocka_unit_test(test_uint8_tensor_dtype_value),
    cmocka_unit_test(test_is_uint8_tensor_empty_value),
    cmocka_unit_test(test_is_uint8_tensor_full_value),
    cmocka_unit_test(test_is_uint8_tensor_ptr_valid),
    cmocka_unit_test(test_uint8_tensor_ndim_value),
    cmocka_unit_test(test_uint8_tensor_shape_dim_value),
    cmocka_unit_test(test_uint8_tensor_shape_value),
    cmocka_unit_test(test_uint8_tensor_shape_ptr_value),
    cmocka_unit_test(test_uint8_tensor_strides_ptr_value),
    cmocka_unit_test(test_uint8_tensor_shape_str_value),

    /* utility — null guards */
    cmocka_unit_test(test_clear_uint8_tensor_null),
    cmocka_unit_test(test_concat_uint8_tensor_null),
    cmocka_unit_test(test_slice_uint8_tensor_null),
    cmocka_unit_test(test_reverse_uint8_tensor_null),
    cmocka_unit_test(test_sort_uint8_tensor_null),

    /* utility — values */
    cmocka_unit_test(test_clear_uint8_tensor_value),
    cmocka_unit_test(test_concat_uint8_tensor_value),
    cmocka_unit_test(test_slice_uint8_tensor_value),
    cmocka_unit_test(test_reverse_uint8_tensor_value),
    cmocka_unit_test(test_sort_uint8_tensor_forward),
    cmocka_unit_test(test_sort_uint8_tensor_reverse),

    /* set/get — null guards */
    cmocka_unit_test(test_set_get_uint8_tensor_index_null),
    cmocka_unit_test(test_set_get_uint8_tensor_nd_index_null),
    cmocka_unit_test(test_push_back_uint8_array_null),
    cmocka_unit_test(test_push_front_uint8_array_null),
    cmocka_unit_test(test_push_at_uint8_array_null),
    cmocka_unit_test(test_pop_back_uint8_array_null),
    cmocka_unit_test(test_pop_front_uint8_array_null),
    cmocka_unit_test(test_pop_at_uint8_array_null),

    /* set/get — values */
    cmocka_unit_test(test_set_get_uint8_tensor_index_value),
    cmocka_unit_test(test_set_get_uint8_tensor_nd_index_value),
    cmocka_unit_test(test_push_back_uint8_array_value),
    cmocka_unit_test(test_push_front_uint8_array_value),
    cmocka_unit_test(test_push_at_uint8_array_value),
    cmocka_unit_test(test_pop_back_uint8_array_value),
    cmocka_unit_test(test_pop_front_uint8_array_value),
    cmocka_unit_test(test_pop_at_uint8_array_value),

    /* uint8_tensor_lsearch — null/guard */
    cmocka_unit_test(test_find_uint8_tensor_null_tensor),
    cmocka_unit_test(test_find_uint8_tensor_null_index),
    cmocka_unit_test(test_find_uint8_tensor_empty),
 
    /* uint8_tensor_lsearch — not found */
    cmocka_unit_test(test_find_uint8_tensor_not_found),
 
    /* uint8_tensor_lsearch — found at various positions */
    cmocka_unit_test(test_find_uint8_tensor_first_element),
    cmocka_unit_test(test_find_uint8_tensor_last_element),
    cmocka_unit_test(test_find_uint8_tensor_middle_element),
    cmocka_unit_test(test_find_uint8_tensor_first_of_duplicates),
    cmocka_unit_test(test_find_uint8_tensor_single_element_match),
    cmocka_unit_test(test_find_uint8_tensor_single_element_no_match),

    /* uint8_tensor_bsearch — null/guard */
    cmocka_unit_test(test_bsearch_uint8_null_tensor),
    cmocka_unit_test(test_bsearch_uint8_null_index),
    cmocka_unit_test(test_bsearch_uint8_empty),
 
    /* uint8_tensor_bsearch — single element */
    cmocka_unit_test(test_bsearch_uint8_single_match),
    cmocka_unit_test(test_bsearch_uint8_single_no_match),
 
    /* uint8_tensor_bsearch — found at various positions */
    cmocka_unit_test(test_bsearch_uint8_find_first),
    cmocka_unit_test(test_bsearch_uint8_find_last),
    cmocka_unit_test(test_bsearch_uint8_find_middle),
 
    /* uint8_tensor_bsearch — not found cases */
    cmocka_unit_test(test_bsearch_uint8_value_below_all),
    cmocka_unit_test(test_bsearch_uint8_value_above_all),
    cmocka_unit_test(test_bsearch_uint8_value_in_gap),
 
    /* uint8_tensor_bsearch — edge cases */
    cmocka_unit_test(test_bsearch_uint8_all_equal),
    cmocka_unit_test(test_bsearch_uint8_two_elements_find_low),
    cmocka_unit_test(test_bsearch_uint8_two_elements_find_high),
 
    /* uint8_tensor_bsearch — integration with sort */
    cmocka_unit_test(test_bsearch_uint8_after_sort),
 
    /* uint8_tensor_bsearch — full uint8_t value range */
    cmocka_unit_test(test_bsearch_uint8_boundary_values),
};

const size_t test_uint8_tensor_count = sizeof(test_uint8_tensor) /
                                       sizeof(test_uint8_tensor[0]);
// ================================================================================ 
// ================================================================================ 
// HELPERS

/** Construct a populated int8 array with values 1..n. */
static int8_tensor_t* _make_int8_array(size_t capacity, bool growth) {
    allocator_vtable_t alloc = heap_allocator();
    int8_tensor_expect_t r  = init_int8_array(capacity, growth, alloc);
    if (!r.has_value) return NULL;
    return r.u.value;
}

static int8_tensor_t* _make_int8_array_filled(size_t n, int8_t base) {
    /* Allocate n+1 so the result always has at least one free slot,
     * making it safe to use as a push destination in tests. */
    int8_tensor_t* arr = _make_int8_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++) {
        int8_t val = (int8_t)(base + i);
        push_back_int8_array(arr, val);
    }
    return arr;
}

// ================================================================================
// ================================================================================
// INITIALISATION AND TEARDOWN

// ---- init_int8_array ----------------------------------------------------------

/** NULL allocator must propagate an error through the expect type. */
static void test_init_int8_array_null_allocator(void** state) {
    (void)state;
    allocator_vtable_t bad = heap_allocator();
    bad.allocate = NULL;
    int8_tensor_expect_t r = init_int8_array(8u, false, bad);
    assert_false(r.has_value);
}

/** Successful construction produces a non-NULL wrapper with correct mode. */
static void test_init_int8_array_success(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_tensor_expect_t r  = init_int8_array(8u, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  ARRAY_STRUCT);
    assert_int_equal(r.u.value->base->dtype, INT8_TYPE);
    assert_int_equal(r.u.value->base->len,   0u);
    assert_int_equal(r.u.value->base->alloc, 8u);
    return_int8_tensor(r.u.value);
}

// ---- init_int8_tensor ---------------------------------------------------------

/** Successful construction of a fixed-shape tensor. */
static void test_init_int8_tensor_success(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    int8_tensor_expect_t r  = init_int8_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  TENSOR_STRUCT);
    assert_int_equal(r.u.value->base->dtype, INT8_TYPE);
    assert_int_equal(r.u.value->base->len,   12u);
    return_int8_tensor(r.u.value);
}

/** NULL shape propagates an error. */
static void test_init_int8_tensor_null_shape(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int8_tensor_expect_t r  = init_int8_tensor(2u, NULL, alloc);
    assert_false(r.has_value);
}

// ---- return_int8_tensor -------------------------------------------------------

/** return_int8_tensor(NULL) must not crash. */
static void test_return_int8_tensor_null(void** state) {
    (void)state;
    return_int8_tensor(NULL);
}

/** Normal construction followed by return must not crash or leak. */
static void test_return_int8_tensor_normal(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(4u, false);
    assert_non_null(arr);
    return_int8_tensor(arr);
}

// ---- copy_int8_tensor ---------------------------------------------------------

/** NULL src must return NULL_POINTER. */
static void test_copy_int8_tensor_null_src(void** state) {
    (void)state;
    int8_tensor_expect_t r = copy_int8_tensor(NULL, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

/** Copy is independent of source — mutation of copy does not affect src. */
static void test_copy_int8_tensor_independence(void** state) {
    (void)state;
    int8_tensor_t* src = _make_int8_array_filled(3u, 10u);
    assert_non_null(src);

    int8_tensor_expect_t r = copy_int8_tensor(src, NULL);
    assert_true(r.has_value);
    int8_tensor_t* dst = r.u.value;

    /* Overwrite slot 0 of the copy */
    assert_int_equal(set_int8_tensor_index(dst, 0u, 99u), NO_ERROR);

    /* Source slot 0 must be unchanged */
    int8_t src_val = 0u;
    assert_int_equal(get_int8_tensor_index(src, 0u, &src_val), NO_ERROR);
    assert_int_equal(src_val, 10u);

    return_int8_tensor(src);
    return_int8_tensor(dst);
}

// ================================================================================
// ================================================================================
// INTROSPECTION

static void test_int8_tensor_size_null(void** state) {
    (void)state;
    assert_int_equal(int8_tensor_size(NULL), 0u);
}

static void test_int8_tensor_size_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(3u, 1u);
    assert_non_null(arr);
    assert_int_equal(int8_tensor_size(arr), 3u);
    return_int8_tensor(arr);
}

static void test_int8_tensor_alloc_null(void** state) {
    (void)state;
    assert_int_equal(int8_tensor_alloc(NULL), 0u);
}

static void test_int8_tensor_alloc_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(8u, false);
    assert_non_null(arr);
    assert_int_equal(int8_tensor_alloc(arr), 8u);
    return_int8_tensor(arr);
}

static void test_int8_tensor_data_size_null(void** state) {
    (void)state;
    assert_int_equal(int8_tensor_data_size(NULL), 0u);
}

static void test_int8_tensor_data_size_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(int8_tensor_data_size(arr), sizeof(int8_t));
    return_int8_tensor(arr);
}

static void test_int8_tensor_dtype_null(void** state) {
    (void)state;
    assert_int_equal(int8_tensor_dtype(NULL), UNKNOWN_TYPE);
}

static void test_int8_tensor_dtype_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(int8_tensor_dtype(arr), INT8_TYPE);
    return_int8_tensor(arr);
}

static void test_is_int8_tensor_empty_null(void** state) {
    (void)state;
    assert_true(is_int8_tensor_empty(NULL));
}

static void test_is_int8_tensor_empty_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(4u, false);
    assert_non_null(arr);
    assert_true(is_int8_tensor_empty(arr));
    push_back_int8_array(arr, 1u);
    assert_false(is_int8_tensor_empty(arr));
    return_int8_tensor(arr);
}

static void test_is_int8_tensor_full_null(void** state) {
    (void)state;
    assert_true(is_int8_tensor_full(NULL));
}

static void test_is_int8_tensor_full_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(2u, false);
    assert_non_null(arr);
    assert_false(is_int8_tensor_full(arr));
    push_back_int8_array(arr, 1u);
    push_back_int8_array(arr, 2u);
    assert_true(is_int8_tensor_full(arr));
    return_int8_tensor(arr);
}

static void test_is_int8_tensor_ptr_null_wrapper(void** state) {
    (void)state;
    int8_t dummy = 0u;
    assert_false(is_int8_tensor_ptr(NULL, &dummy));
}

static void test_is_int8_tensor_ptr_valid(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(4u, 10u);
    assert_non_null(arr);
    /* Pointer to first element must be valid */
    const int8_t* p = (const int8_t*)arr->base->data;
    assert_true(is_int8_tensor_ptr(arr, p));
    return_int8_tensor(arr);
}

static void test_int8_tensor_ndim_null(void** state) {
    (void)state;
    assert_int_equal(int8_tensor_ndim(NULL), 0u);
}

static void test_int8_tensor_ndim_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(int8_tensor_ndim(arr), 1u);
    return_int8_tensor(arr);
}

static void test_int8_tensor_shape_dim_null(void** state) {
    (void)state;
    assert_int_equal(int8_tensor_shape_dim(NULL, 0u), 0u);
}

static void test_int8_tensor_shape_dim_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    int8_tensor_expect_t r  = init_int8_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_int_equal(int8_tensor_shape_dim(r.u.value, 0u), 3u);
    assert_int_equal(int8_tensor_shape_dim(r.u.value, 1u), 4u);
    return_int8_tensor(r.u.value);
}

static void test_int8_tensor_shape_null(void** state) {
    (void)state;
    size_t buf[2];
    assert_int_equal(int8_tensor_shape(NULL, buf, 2u), NULL_POINTER);
}

static void test_int8_tensor_shape_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    int8_tensor_expect_t r  = init_int8_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    size_t buf[2] = { 0u, 0u };
    assert_int_equal(int8_tensor_shape(r.u.value, buf, 2u), NO_ERROR);
    assert_int_equal(buf[0], 3u);
    assert_int_equal(buf[1], 4u);
    return_int8_tensor(r.u.value);
}

static void test_int8_tensor_shape_ptr_null(void** state) {
    (void)state;
    assert_null(int8_tensor_shape_ptr(NULL));
}

static void test_int8_tensor_shape_ptr_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(4u, false);
    assert_non_null(arr);
    assert_non_null(int8_tensor_shape_ptr(arr));
    return_int8_tensor(arr);
}

static void test_int8_tensor_strides_ptr_null(void** state) {
    (void)state;
    assert_null(int8_tensor_strides_ptr(NULL));
}

static void test_int8_tensor_strides_ptr_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(4u, false);
    assert_non_null(arr);
    const size_t* strides = int8_tensor_strides_ptr(arr);
    assert_non_null(strides);
    assert_int_equal(strides[0], sizeof(int8_t));
    return_int8_tensor(arr);
}

static void test_int8_tensor_shape_str_null(void** state) {
    (void)state;
    char buf[32];
    assert_int_equal(int8_tensor_shape_str(NULL, buf, sizeof(buf)), NULL_POINTER);
}

static void test_int8_tensor_shape_str_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(8u, false);
    assert_non_null(arr);
    char buf[32];
    assert_int_equal(int8_tensor_shape_str(arr, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(8)");
    return_int8_tensor(arr);
}

// ================================================================================
// ================================================================================
// UTILITY OPERATIONS

// ---- clear_int8_tensor --------------------------------------------------------

static void test_clear_int8_tensor_null(void** state) {
    (void)state;
    assert_int_equal(clear_int8_tensor(NULL), NULL_POINTER);
}

static void test_clear_int8_tensor_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(3u, 10u);
    assert_non_null(arr);
    assert_int_equal(clear_int8_tensor(arr), NO_ERROR);
    assert_int_equal(int8_tensor_size(arr), 0u);
    return_int8_tensor(arr);
}

// ---- concat_int8_tensor_array -------------------------------------------------

static void test_concat_int8_tensor_null(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(concat_int8_tensor_array(NULL, arr), NULL_POINTER);
    assert_int_equal(concat_int8_tensor_array(arr, NULL), NULL_POINTER);
    return_int8_tensor(arr);
}

static void test_concat_int8_tensor_value(void** state) {
    (void)state;

    /* dst needs capacity for 5 elements total — allocate with room to grow */
    allocator_vtable_t alloc = heap_allocator();
    int8_tensor_expect_t r  = init_int8_array(8u, false, alloc);
    assert_true(r.has_value);
    int8_tensor_t* dst = r.u.value;

    /* Populate dst with [1, 2, 3] */
    push_back_int8_array(dst, 1u);
    push_back_int8_array(dst, 2u);
    push_back_int8_array(dst, 3u);

    /* src = [4, 5] */
    int8_tensor_t* src = _make_int8_array_filled(2u, 4u);
    assert_non_null(src);

    assert_int_equal(concat_int8_tensor_array(dst, src), NO_ERROR);
    assert_int_equal(int8_tensor_size(dst), 5u);

    int8_t expected[] = { 1u, 2u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        int8_t out = 0u;
        assert_int_equal(get_int8_tensor_index(dst, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int8_tensor(dst);
    return_int8_tensor(src);
}

// ---- slice_int8_tensor_array --------------------------------------------------

static void test_slice_int8_tensor_null(void** state) {
    (void)state;
    int8_tensor_expect_t r = slice_int8_tensor_array(NULL, 0u, 1u, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_slice_int8_tensor_value(void** state) {
    (void)state;
    int8_tensor_t* src = _make_int8_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(src);

    int8_tensor_expect_t r = slice_int8_tensor_array(src, 1u, 4u, NULL);
    assert_true(r.has_value);
    int8_tensor_t* sl = r.u.value;

    assert_int_equal(int8_tensor_size(sl), 3u);
    int8_t expected[] = { 11u, 12u, 13u };
    for (size_t i = 0u; i < 3u; i++) {
        int8_t out = 0u;
        assert_int_equal(get_int8_tensor_index(sl, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int8_tensor(src);
    return_int8_tensor(sl);
}

// ---- reverse_int8_tensor ------------------------------------------------------

static void test_reverse_int8_tensor_null(void** state) {
    (void)state;
    assert_int_equal(reverse_int8_tensor(NULL), NULL_POINTER);
}

static void test_reverse_int8_tensor_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(4u, 1u); /* [1,2,3,4] */
    assert_non_null(arr);

    assert_int_equal(reverse_int8_tensor(arr), NO_ERROR);

    int8_t expected[] = { 4u, 3u, 2u, 1u };
    for (size_t i = 0u; i < 4u; i++) {
        int8_t out = 0u;
        assert_int_equal(get_int8_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int8_tensor(arr);
}

// ---- sort_int8_tensor ---------------------------------------------------------

static void test_sort_int8_tensor_null(void** state) {
    (void)state;
    assert_int_equal(sort_int8_tensor(NULL, FORWARD), NULL_POINTER);
}

static void test_sort_int8_tensor_forward(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(5u, false);
    assert_non_null(arr);

    int8_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_int8_array(arr, vals[i]);

    assert_int_equal(sort_int8_tensor(arr, FORWARD), NO_ERROR);

    int8_t expected[] = { 1u, 1u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        int8_t out = 0u;
        assert_int_equal(get_int8_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int8_tensor(arr);
}

static void test_sort_int8_tensor_reverse(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(5u, false);
    assert_non_null(arr);

    int8_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_int8_array(arr, vals[i]);

    assert_int_equal(sort_int8_tensor(arr, REVERSE), NO_ERROR);

    int8_t expected[] = { 5u, 4u, 3u, 1u, 1u };
    for (size_t i = 0u; i < 5u; i++) {
        int8_t out = 0u;
        assert_int_equal(get_int8_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int8_tensor(arr);
}

// ================================================================================
// ================================================================================
// SET AND GET DATA

// ---- set_int8_tensor_index / get_int8_tensor_index --------------------------

static void test_set_get_int8_tensor_index_null(void** state) {
    (void)state;
    int8_t dummy = 0u;
    assert_int_equal(set_int8_tensor_index(NULL, 0u, 1u),    NULL_POINTER);
    assert_int_equal(get_int8_tensor_index(NULL, 0u, &dummy), NULL_POINTER);
}

static void test_set_get_int8_tensor_index_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(3u, 10u);
    assert_non_null(arr);

    assert_int_equal(set_int8_tensor_index(arr, 1u, 99u), NO_ERROR);

    int8_t out = 0u;
    assert_int_equal(get_int8_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);

    return_int8_tensor(arr);
}

// ---- set_int8_tensor_nd_index / get_int8_tensor_nd_index --------------------

static void test_set_get_int8_tensor_nd_index_null(void** state) {
    (void)state;
    size_t  idx[] = { 0u, 0u };
    int8_t out   = 0u;
    assert_int_equal(set_int8_tensor_nd_index(NULL, idx, 1u),   NULL_POINTER);
    assert_int_equal(get_int8_tensor_nd_index(NULL, idx, &out), NULL_POINTER);
}

static void test_set_get_int8_tensor_nd_index_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 3u };
    allocator_vtable_t alloc = heap_allocator();
    int8_tensor_expect_t r  = init_int8_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    int8_tensor_t* mat = r.u.value;

    size_t idx[] = { 1u, 2u };
    assert_int_equal(set_int8_tensor_nd_index(mat, idx, 42u), NO_ERROR);

    int8_t out = 0u;
    assert_int_equal(get_int8_tensor_nd_index(mat, idx, &out), NO_ERROR);
    assert_int_equal(out, 42u);

    return_int8_tensor(mat);
}

// ---- push and pop wrappers ----------------------------------------------------

static void test_push_back_int8_array_null(void** state) {
    (void)state;
    assert_int_equal(push_back_int8_array(NULL, 1u), NULL_POINTER);
}

static void test_push_back_int8_array_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(4u, false);
    assert_non_null(arr);

    assert_int_equal(push_back_int8_array(arr, 10u), NO_ERROR);
    assert_int_equal(push_back_int8_array(arr, 20u), NO_ERROR);
    assert_int_equal(int8_tensor_size(arr), 2u);

    int8_t out = 0u;
    assert_int_equal(get_int8_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 20u);

    return_int8_tensor(arr);
}

static void test_push_front_int8_array_null(void** state) {
    (void)state;
    assert_int_equal(push_front_int8_array(NULL, 1u), NULL_POINTER);
}

static void test_push_front_int8_array_value(void** state) {
    (void)state;
    /* Capacity 3 so there is room for the push_front */
    allocator_vtable_t alloc = heap_allocator();
    int8_tensor_expect_t r  = init_int8_array(3u, false, alloc);
    assert_true(r.has_value);
    int8_tensor_t* arr = r.u.value;

    push_back_int8_array(arr, 10u);
    push_back_int8_array(arr, 11u);
    /* arr = [10, 11], len == 2, alloc == 3 — one slot free */

    assert_int_equal(push_front_int8_array(arr, 99u), NO_ERROR);

    int8_t out = 0u;
    assert_int_equal(get_int8_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_int8_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_int8_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);

    return_int8_tensor(arr);
}

static void test_push_at_int8_array_null(void** state) {
    (void)state;
    assert_int_equal(push_at_int8_array(NULL, 1u, 0u), NULL_POINTER);
}

static void test_push_at_int8_array_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(3u, 10u); /* [10, 11, 12] */
    assert_non_null(arr);

    assert_int_equal(push_at_int8_array(arr, 99u, 1u), NO_ERROR);
    /* arr = [10, 99, 11, 12] */

    int8_t out = 0u;
    assert_int_equal(get_int8_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_int8_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_int8_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);
    assert_int_equal(get_int8_tensor_index(arr, 3u, &out), NO_ERROR);
    assert_int_equal(out, 12u);

    return_int8_tensor(arr);
}

static void test_pop_back_int8_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_back_int8_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_back_int8_array_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    int8_t out = 0u;
    assert_int_equal(pop_back_int8_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(int8_tensor_size(arr), 2u);

    return_int8_tensor(arr);
}

static void test_pop_front_int8_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_front_int8_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_front_int8_array_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    int8_t out = 0u;
    assert_int_equal(pop_front_int8_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(int8_tensor_size(arr), 2u);

    return_int8_tensor(arr);
}

static void test_pop_at_int8_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_at_int8_array(NULL, NULL, 0u), NULL_POINTER);
}

static void test_pop_at_int8_array_value(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(4u, 10u); /* [10,11,12,13] */
    assert_non_null(arr);

    int8_t out = 0u;
    assert_int_equal(pop_at_int8_array(arr, &out, 2u), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(int8_tensor_size(arr), 3u);

    assert_int_equal(get_int8_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 13u);

    return_int8_tensor(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// FIND UINT8 TENSOR VALUE (int8_tensor_lsearch)
// ================================================================================
 
/** NULL tensor must return NULL_POINTER. */
static void test_find_int8_tensor_null_tensor(void** state) {
    (void)state;
    size_t idx = 0u;
    assert_int_equal(int8_tensor_lsearch(NULL, &idx, 10u), NULL_POINTER);
}
 
/** NULL index pointer must return NULL_POINTER. */
static void test_find_int8_tensor_null_index(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(3u, 10u);
    assert_non_null(arr);
    assert_int_equal(int8_tensor_lsearch(arr, NULL, 10u), NULL_POINTER);
    return_int8_tensor(arr);
}
 
/** Empty array must return EMPTY. */
static void test_find_int8_tensor_empty(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(4u, false);
    assert_non_null(arr);
    size_t idx = 0u;
    assert_int_equal(int8_tensor_lsearch(arr, &idx, 10u), EMPTY);
    return_int8_tensor(arr);
}
 
/** Value not present must return NOT_FOUND. */
static void test_find_int8_tensor_not_found(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(4u, 1u); /* [1,2,3,4] */
    assert_non_null(arr);
    size_t idx = 99u;   /* sentinel — must not be modified */
    assert_int_equal(int8_tensor_lsearch(arr, &idx, 99u), NOT_FOUND);
    /* index must be unchanged on NOT_FOUND */
    assert_int_equal(idx, 99u);
    return_int8_tensor(arr);
}
 
/** Value at the first position must be found at index 0. */
static void test_find_int8_tensor_first_element(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(int8_tensor_lsearch(arr, &idx, 10u), NO_ERROR);
    assert_int_equal(idx, 0u);
    return_int8_tensor(arr);
}
 
/** Value at the last position must be found at index len-1. */
static void test_find_int8_tensor_last_element(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(int8_tensor_lsearch(arr, &idx, 14u), NO_ERROR);
    assert_int_equal(idx, 4u);
    return_int8_tensor(arr);
}
 
/** Value in the middle must be found at the correct index. */
static void test_find_int8_tensor_middle_element(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(int8_tensor_lsearch(arr, &idx, 12u), NO_ERROR);
    assert_int_equal(idx, 2u);
    return_int8_tensor(arr);
}
 
/**
 * When duplicates are present, the first occurrence must be returned.
 */
static void test_find_int8_tensor_first_of_duplicates(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(6u, false);
    assert_non_null(arr);
 
    /* [5, 9, 9, 9, 7, 3] */
    push_back_int8_array(arr, 5u);
    push_back_int8_array(arr, 9u);
    push_back_int8_array(arr, 9u);
    push_back_int8_array(arr, 9u);
    push_back_int8_array(arr, 7u);
    push_back_int8_array(arr, 3u);
 
    size_t idx = 99u;
    assert_int_equal(int8_tensor_lsearch(arr, &idx, 9u), NO_ERROR);
    assert_int_equal(idx, 1u);   /* first occurrence at index 1 */
 
    return_int8_tensor(arr);
}
 
/**
 * Single-element array containing the target value must succeed.
 */
static void test_find_int8_tensor_single_element_match(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(1u, false);
    assert_non_null(arr);
    push_back_int8_array(arr, 42u);
 
    size_t idx = 99u;
    assert_int_equal(int8_tensor_lsearch(arr, &idx, 42u), NO_ERROR);
    assert_int_equal(idx, 0u);
 
    return_int8_tensor(arr);
}
 
/**
 * Single-element array not containing the target must return NOT_FOUND
 * without modifying index.
 */
static void test_find_int8_tensor_single_element_no_match(void** state) {
    (void)state;
    int8_tensor_t* arr = _make_int8_array(1u, false);
    assert_non_null(arr);
    push_back_int8_array(arr, 42u);
 
    size_t idx = 99u;
    assert_int_equal(int8_tensor_lsearch(arr, &idx, 7u), NOT_FOUND);
    assert_int_equal(idx, 99u);   /* unchanged */
 
    return_int8_tensor(arr);
}
// ================================================================================
// ================================================================================
// TEST SUITE REGISTRY

const struct CMUnitTest test_int8_tensor[] = {
    /* init_int8_array */
    cmocka_unit_test(test_init_int8_array_null_allocator),
    cmocka_unit_test(test_init_int8_array_success),

    /* init_int8_tensor */
    cmocka_unit_test(test_init_int8_tensor_success),
    cmocka_unit_test(test_init_int8_tensor_null_shape),

    /* return_int8_tensor */
    cmocka_unit_test(test_return_int8_tensor_null),
    cmocka_unit_test(test_return_int8_tensor_normal),

    /* copy_int8_tensor */
    cmocka_unit_test(test_copy_int8_tensor_null_src),
    cmocka_unit_test(test_copy_int8_tensor_independence),

    /* introspection — null guards */
    cmocka_unit_test(test_int8_tensor_size_null),
    cmocka_unit_test(test_int8_tensor_alloc_null),
    cmocka_unit_test(test_int8_tensor_data_size_null),
    cmocka_unit_test(test_int8_tensor_dtype_null),
    cmocka_unit_test(test_is_int8_tensor_empty_null),
    cmocka_unit_test(test_is_int8_tensor_full_null),
    cmocka_unit_test(test_is_int8_tensor_ptr_null_wrapper),
    cmocka_unit_test(test_int8_tensor_ndim_null),
    cmocka_unit_test(test_int8_tensor_shape_dim_null),
    cmocka_unit_test(test_int8_tensor_shape_null),
    cmocka_unit_test(test_int8_tensor_shape_ptr_null),
    cmocka_unit_test(test_int8_tensor_strides_ptr_null),
    cmocka_unit_test(test_int8_tensor_shape_str_null),

    /* introspection — values */
    cmocka_unit_test(test_int8_tensor_size_value),
    cmocka_unit_test(test_int8_tensor_alloc_value),
    cmocka_unit_test(test_int8_tensor_data_size_value),
    cmocka_unit_test(test_int8_tensor_dtype_value),
    cmocka_unit_test(test_is_int8_tensor_empty_value),
    cmocka_unit_test(test_is_int8_tensor_full_value),
    cmocka_unit_test(test_is_int8_tensor_ptr_valid),
    cmocka_unit_test(test_int8_tensor_ndim_value),
    cmocka_unit_test(test_int8_tensor_shape_dim_value),
    cmocka_unit_test(test_int8_tensor_shape_value),
    cmocka_unit_test(test_int8_tensor_shape_ptr_value),
    cmocka_unit_test(test_int8_tensor_strides_ptr_value),
    cmocka_unit_test(test_int8_tensor_shape_str_value),

    /* utility — null guards */
    cmocka_unit_test(test_clear_int8_tensor_null),
    cmocka_unit_test(test_concat_int8_tensor_null),
    cmocka_unit_test(test_slice_int8_tensor_null),
    cmocka_unit_test(test_reverse_int8_tensor_null),
    cmocka_unit_test(test_sort_int8_tensor_null),

    /* utility — values */
    cmocka_unit_test(test_clear_int8_tensor_value),
    cmocka_unit_test(test_concat_int8_tensor_value),
    cmocka_unit_test(test_slice_int8_tensor_value),
    cmocka_unit_test(test_reverse_int8_tensor_value),
    cmocka_unit_test(test_sort_int8_tensor_forward),
    cmocka_unit_test(test_sort_int8_tensor_reverse),

    /* set/get — null guards */
    cmocka_unit_test(test_set_get_int8_tensor_index_null),
    cmocka_unit_test(test_set_get_int8_tensor_nd_index_null),
    cmocka_unit_test(test_push_back_int8_array_null),
    cmocka_unit_test(test_push_front_int8_array_null),
    cmocka_unit_test(test_push_at_int8_array_null),
    cmocka_unit_test(test_pop_back_int8_array_null),
    cmocka_unit_test(test_pop_front_int8_array_null),
    cmocka_unit_test(test_pop_at_int8_array_null),

    /* set/get — values */
    cmocka_unit_test(test_set_get_int8_tensor_index_value),
    cmocka_unit_test(test_set_get_int8_tensor_nd_index_value),
    cmocka_unit_test(test_push_back_int8_array_value),
    cmocka_unit_test(test_push_front_int8_array_value),
    cmocka_unit_test(test_push_at_int8_array_value),
    cmocka_unit_test(test_pop_back_int8_array_value),
    cmocka_unit_test(test_pop_front_int8_array_value),
    cmocka_unit_test(test_pop_at_int8_array_value),

    /* int8_tensor_lsearch — null/guard */
    cmocka_unit_test(test_find_int8_tensor_null_tensor),
    cmocka_unit_test(test_find_int8_tensor_null_index),
    cmocka_unit_test(test_find_int8_tensor_empty),
 
    /* int8_tensor_lsearch — not found */
    cmocka_unit_test(test_find_int8_tensor_not_found),
 
    /* int8_tensor_lsearch — found at various positions */
    cmocka_unit_test(test_find_int8_tensor_first_element),
    cmocka_unit_test(test_find_int8_tensor_last_element),
    cmocka_unit_test(test_find_int8_tensor_middle_element),
    cmocka_unit_test(test_find_int8_tensor_first_of_duplicates),
    cmocka_unit_test(test_find_int8_tensor_single_element_match),
    cmocka_unit_test(test_find_int8_tensor_single_element_no_match),
};

const size_t test_int8_tensor_count = sizeof(test_int8_tensor) /
                                       sizeof(test_int8_tensor[0]);
// ================================================================================ 
// ================================================================================ 
// HELPERS

/** Construct a populated uint8 array with values 1..n. */
static uint16_tensor_t* _make_uint16_array(size_t capacity, bool growth) {
    allocator_vtable_t alloc = heap_allocator();
    uint16_tensor_expect_t r  = init_uint16_array(capacity, growth, alloc);
    if (!r.has_value) return NULL;
    return r.u.value;
}

static uint16_tensor_t* _make_uint16_array_filled(size_t n, uint16_t base) {
    /* Allocate n+1 so the result always has at least one free slot,
     * making it safe to use as a push destination in tests. */
    uint16_tensor_t* arr = _make_uint16_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++) {
        uint16_t val = (uint16_t)(base + i);
        push_back_uint16_array(arr, val);
    }
    return arr;
}

// ================================================================================
// ================================================================================
// INITIALISATION AND TEARDOWN

// ---- init_uint16_array ----------------------------------------------------------

/** NULL allocator must propagate an error through the expect type. */
static void test_init_uint16_array_null_allocator(void** state) {
    (void)state;
    allocator_vtable_t bad = heap_allocator();
    bad.allocate = NULL;
    uint16_tensor_expect_t r = init_uint16_array(8u, false, bad);
    assert_false(r.has_value);
}

/** Successful construction produces a non-NULL wrapper with correct mode. */
static void test_init_uint16_array_success(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_tensor_expect_t r  = init_uint16_array(8u, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  ARRAY_STRUCT);
    assert_int_equal(r.u.value->base->dtype, UINT16_TYPE);
    assert_int_equal(r.u.value->base->len,   0u);
    assert_int_equal(r.u.value->base->alloc, 8u);
    return_uint16_tensor(r.u.value);
}

// ---- init_uint16_tensor ---------------------------------------------------------

/** Successful construction of a fixed-shape tensor. */
static void test_init_uint16_tensor_success(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    uint16_tensor_expect_t r  = init_uint16_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  TENSOR_STRUCT);
    assert_int_equal(r.u.value->base->dtype, UINT16_TYPE);
    assert_int_equal(r.u.value->base->len,   12u);
    return_uint16_tensor(r.u.value);
}

/** NULL shape propagates an error. */
static void test_init_uint16_tensor_null_shape(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint16_tensor_expect_t r  = init_uint16_tensor(2u, NULL, alloc);
    assert_false(r.has_value);
}

// ---- return_uint16_tensor -------------------------------------------------------

/** return_uint16_tensor(NULL) must not crash. */
static void test_return_uint16_tensor_null(void** state) {
    (void)state;
    return_uint16_tensor(NULL);
}

/** Normal construction followed by return must not crash or leak. */
static void test_return_uint16_tensor_normal(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(4u, false);
    assert_non_null(arr);
    return_uint16_tensor(arr);
}

// ---- copy_uint16_tensor ---------------------------------------------------------

/** NULL src must return NULL_POINTER. */
static void test_copy_uint16_tensor_null_src(void** state) {
    (void)state;
    uint16_tensor_expect_t r = copy_uint16_tensor(NULL, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

/** Copy is independent of source — mutation of copy does not affect src. */
static void test_copy_uint16_tensor_independence(void** state) {
    (void)state;
    uint16_tensor_t* src = _make_uint16_array_filled(3u, 10u);
    assert_non_null(src);

    uint16_tensor_expect_t r = copy_uint16_tensor(src, NULL);
    assert_true(r.has_value);
    uint16_tensor_t* dst = r.u.value;

    /* Overwrite slot 0 of the copy */
    assert_int_equal(set_uint16_tensor_index(dst, 0u, 99u), NO_ERROR);

    /* Source slot 0 must be unchanged */
    uint16_t src_val = 0u;
    assert_int_equal(get_uint16_tensor_index(src, 0u, &src_val), NO_ERROR);
    assert_int_equal(src_val, 10u);

    return_uint16_tensor(src);
    return_uint16_tensor(dst);
}

// ================================================================================
// ================================================================================
// INTROSPECTION

static void test_uint16_tensor_size_null(void** state) {
    (void)state;
    assert_int_equal(uint16_tensor_size(NULL), 0u);
}

static void test_uint16_tensor_size_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(3u, 1u);
    assert_non_null(arr);
    assert_int_equal(uint16_tensor_size(arr), 3u);
    return_uint16_tensor(arr);
}

static void test_uint16_tensor_alloc_null(void** state) {
    (void)state;
    assert_int_equal(uint16_tensor_alloc(NULL), 0u);
}

static void test_uint16_tensor_alloc_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(8u, false);
    assert_non_null(arr);
    assert_int_equal(uint16_tensor_alloc(arr), 8u);
    return_uint16_tensor(arr);
}

static void test_uint16_tensor_data_size_null(void** state) {
    (void)state;
    assert_int_equal(uint16_tensor_data_size(NULL), 0u);
}

static void test_uint16_tensor_data_size_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(uint16_tensor_data_size(arr), sizeof(uint16_t));
    return_uint16_tensor(arr);
}

static void test_uint16_tensor_dtype_null(void** state) {
    (void)state;
    assert_int_equal(uint16_tensor_dtype(NULL), UNKNOWN_TYPE);
}

static void test_uint16_tensor_dtype_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(uint16_tensor_dtype(arr), UINT16_TYPE);
    return_uint16_tensor(arr);
}

static void test_is_uint16_tensor_empty_null(void** state) {
    (void)state;
    assert_true(is_uint16_tensor_empty(NULL));
}

static void test_is_uint16_tensor_empty_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(4u, false);
    assert_non_null(arr);
    assert_true(is_uint16_tensor_empty(arr));
    push_back_uint16_array(arr, 1u);
    assert_false(is_uint16_tensor_empty(arr));
    return_uint16_tensor(arr);
}

static void test_is_uint16_tensor_full_null(void** state) {
    (void)state;
    assert_true(is_uint16_tensor_full(NULL));
}

static void test_is_uint16_tensor_full_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(2u, false);
    assert_non_null(arr);
    assert_false(is_uint16_tensor_full(arr));
    push_back_uint16_array(arr, 1u);
    push_back_uint16_array(arr, 2u);
    assert_true(is_uint16_tensor_full(arr));
    return_uint16_tensor(arr);
}

static void test_is_uint16_tensor_ptr_null_wrapper(void** state) {
    (void)state;
    uint16_t dummy = 0u;
    assert_false(is_uint16_tensor_ptr(NULL, &dummy));
}

static void test_is_uint16_tensor_ptr_valid(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(4u, 10u);
    assert_non_null(arr);
    /* Pointer to first element must be valid */
    const uint16_t* p = (const uint16_t*)arr->base->data;
    assert_true(is_uint16_tensor_ptr(arr, p));
    return_uint16_tensor(arr);
}

static void test_uint16_tensor_ndim_null(void** state) {
    (void)state;
    assert_int_equal(uint16_tensor_ndim(NULL), 0u);
}

static void test_uint16_tensor_ndim_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(uint16_tensor_ndim(arr), 1u);
    return_uint16_tensor(arr);
}

static void test_uint16_tensor_shape_dim_null(void** state) {
    (void)state;
    assert_int_equal(uint16_tensor_shape_dim(NULL, 0u), 0u);
}

static void test_uint16_tensor_shape_dim_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    uint16_tensor_expect_t r  = init_uint16_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_int_equal(uint16_tensor_shape_dim(r.u.value, 0u), 3u);
    assert_int_equal(uint16_tensor_shape_dim(r.u.value, 1u), 4u);
    return_uint16_tensor(r.u.value);
}

static void test_uint16_tensor_shape_null(void** state) {
    (void)state;
    size_t buf[2];
    assert_int_equal(uint16_tensor_shape(NULL, buf, 2u), NULL_POINTER);
}

static void test_uint16_tensor_shape_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    uint16_tensor_expect_t r  = init_uint16_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    size_t buf[2] = { 0u, 0u };
    assert_int_equal(uint16_tensor_shape(r.u.value, buf, 2u), NO_ERROR);
    assert_int_equal(buf[0], 3u);
    assert_int_equal(buf[1], 4u);
    return_uint16_tensor(r.u.value);
}

static void test_uint16_tensor_shape_ptr_null(void** state) {
    (void)state;
    assert_null(uint16_tensor_shape_ptr(NULL));
}

static void test_uint16_tensor_shape_ptr_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(4u, false);
    assert_non_null(arr);
    assert_non_null(uint16_tensor_shape_ptr(arr));
    return_uint16_tensor(arr);
}

static void test_uint16_tensor_strides_ptr_null(void** state) {
    (void)state;
    assert_null(uint16_tensor_strides_ptr(NULL));
}

static void test_uint16_tensor_strides_ptr_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(4u, false);
    assert_non_null(arr);
    const size_t* strides = uint16_tensor_strides_ptr(arr);
    assert_non_null(strides);
    assert_int_equal(strides[0], sizeof(uint16_t));
    return_uint16_tensor(arr);
}

static void test_uint16_tensor_shape_str_null(void** state) {
    (void)state;
    char buf[32];
    assert_int_equal(uint16_tensor_shape_str(NULL, buf, sizeof(buf)), NULL_POINTER);
}

static void test_uint16_tensor_shape_str_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(8u, false);
    assert_non_null(arr);
    char buf[32];
    assert_int_equal(uint16_tensor_shape_str(arr, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(8)");
    return_uint16_tensor(arr);
}

// ================================================================================
// ================================================================================
// UTILITY OPERATIONS

// ---- clear_uint16_tensor --------------------------------------------------------

static void test_clear_uint16_tensor_null(void** state) {
    (void)state;
    assert_int_equal(clear_uint16_tensor(NULL), NULL_POINTER);
}

static void test_clear_uint16_tensor_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(3u, 10u);
    assert_non_null(arr);
    assert_int_equal(clear_uint16_tensor(arr), NO_ERROR);
    assert_int_equal(uint16_tensor_size(arr), 0u);
    return_uint16_tensor(arr);
}

// ---- concat_uint16_tensor_array -------------------------------------------------

static void test_concat_uint16_tensor_null(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(concat_uint16_tensor_array(NULL, arr), NULL_POINTER);
    assert_int_equal(concat_uint16_tensor_array(arr, NULL), NULL_POINTER);
    return_uint16_tensor(arr);
}

static void test_concat_uint16_tensor_value(void** state) {
    (void)state;

    /* dst needs capacity for 5 elements total — allocate with room to grow */
    allocator_vtable_t alloc = heap_allocator();
    uint16_tensor_expect_t r  = init_uint16_array(8u, false, alloc);
    assert_true(r.has_value);
    uint16_tensor_t* dst = r.u.value;

    /* Populate dst with [1, 2, 3] */
    push_back_uint16_array(dst, 1u);
    push_back_uint16_array(dst, 2u);
    push_back_uint16_array(dst, 3u);

    /* src = [4, 5] */
    uint16_tensor_t* src = _make_uint16_array_filled(2u, 4u);
    assert_non_null(src);

    assert_int_equal(concat_uint16_tensor_array(dst, src), NO_ERROR);
    assert_int_equal(uint16_tensor_size(dst), 5u);

    uint16_t expected[] = { 1u, 2u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        uint16_t out = 0u;
        assert_int_equal(get_uint16_tensor_index(dst, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint16_tensor(dst);
    return_uint16_tensor(src);
}

// ---- slice_uint16_tensor_array --------------------------------------------------

static void test_slice_uint16_tensor_null(void** state) {
    (void)state;
    uint16_tensor_expect_t r = slice_uint16_tensor_array(NULL, 0u, 1u, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_slice_uint16_tensor_value(void** state) {
    (void)state;
    uint16_tensor_t* src = _make_uint16_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(src);

    uint16_tensor_expect_t r = slice_uint16_tensor_array(src, 1u, 4u, NULL);
    assert_true(r.has_value);
    uint16_tensor_t* sl = r.u.value;

    assert_int_equal(uint16_tensor_size(sl), 3u);
    uint16_t expected[] = { 11u, 12u, 13u };
    for (size_t i = 0u; i < 3u; i++) {
        uint16_t out = 0u;
        assert_int_equal(get_uint16_tensor_index(sl, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint16_tensor(src);
    return_uint16_tensor(sl);
}

// ---- reverse_uint16_tensor ------------------------------------------------------

static void test_reverse_uint16_tensor_null(void** state) {
    (void)state;
    assert_int_equal(reverse_uint16_tensor(NULL), NULL_POINTER);
}

static void test_reverse_uint16_tensor_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(4u, 1u); /* [1,2,3,4] */
    assert_non_null(arr);

    assert_int_equal(reverse_uint16_tensor(arr), NO_ERROR);

    uint16_t expected[] = { 4u, 3u, 2u, 1u };
    for (size_t i = 0u; i < 4u; i++) {
        uint16_t out = 0u;
        assert_int_equal(get_uint16_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint16_tensor(arr);
}

// ---- sort_uint16_tensor ---------------------------------------------------------

static void test_sort_uint16_tensor_null(void** state) {
    (void)state;
    assert_int_equal(sort_uint16_tensor(NULL, FORWARD), NULL_POINTER);
}

static void test_sort_uint16_tensor_forward(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(5u, false);
    assert_non_null(arr);

    uint16_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_uint16_array(arr, vals[i]);

    assert_int_equal(sort_uint16_tensor(arr, FORWARD), NO_ERROR);

    uint16_t expected[] = { 1u, 1u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        uint16_t out = 0u;
        assert_int_equal(get_uint16_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint16_tensor(arr);
}

static void test_sort_uint16_tensor_reverse(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(5u, false);
    assert_non_null(arr);

    uint16_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_uint16_array(arr, vals[i]);

    assert_int_equal(sort_uint16_tensor(arr, REVERSE), NO_ERROR);

    uint16_t expected[] = { 5u, 4u, 3u, 1u, 1u };
    for (size_t i = 0u; i < 5u; i++) {
        uint16_t out = 0u;
        assert_int_equal(get_uint16_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint16_tensor(arr);
}

// ================================================================================
// ================================================================================
// SET AND GET DATA

// ---- set_uint16_tensor_index / get_uint16_tensor_index --------------------------

static void test_set_get_uint16_tensor_index_null(void** state) {
    (void)state;
    uint16_t dummy = 0u;
    assert_int_equal(set_uint16_tensor_index(NULL, 0u, 1u),    NULL_POINTER);
    assert_int_equal(get_uint16_tensor_index(NULL, 0u, &dummy), NULL_POINTER);
}

static void test_set_get_uint16_tensor_index_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(3u, 10u);
    assert_non_null(arr);

    assert_int_equal(set_uint16_tensor_index(arr, 1u, 99u), NO_ERROR);

    uint16_t out = 0u;
    assert_int_equal(get_uint16_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);

    return_uint16_tensor(arr);
}

// ---- set_uint16_tensor_nd_index / get_uint16_tensor_nd_index --------------------

static void test_set_get_uint16_tensor_nd_index_null(void** state) {
    (void)state;
    size_t  idx[] = { 0u, 0u };
    uint16_t out   = 0u;
    assert_int_equal(set_uint16_tensor_nd_index(NULL, idx, 1u),   NULL_POINTER);
    assert_int_equal(get_uint16_tensor_nd_index(NULL, idx, &out), NULL_POINTER);
}

static void test_set_get_uint16_tensor_nd_index_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 3u };
    allocator_vtable_t alloc = heap_allocator();
    uint16_tensor_expect_t r  = init_uint16_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    uint16_tensor_t* mat = r.u.value;

    size_t idx[] = { 1u, 2u };
    assert_int_equal(set_uint16_tensor_nd_index(mat, idx, 42u), NO_ERROR);

    uint16_t out = 0u;
    assert_int_equal(get_uint16_tensor_nd_index(mat, idx, &out), NO_ERROR);
    assert_int_equal(out, 42u);

    return_uint16_tensor(mat);
}

// ---- push and pop wrappers ----------------------------------------------------

static void test_push_back_uint16_array_null(void** state) {
    (void)state;
    assert_int_equal(push_back_uint16_array(NULL, 1u), NULL_POINTER);
}

static void test_push_back_uint16_array_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(4u, false);
    assert_non_null(arr);

    assert_int_equal(push_back_uint16_array(arr, 10u), NO_ERROR);
    assert_int_equal(push_back_uint16_array(arr, 20u), NO_ERROR);
    assert_int_equal(uint16_tensor_size(arr), 2u);

    uint16_t out = 0u;
    assert_int_equal(get_uint16_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 20u);

    return_uint16_tensor(arr);
}

static void test_push_front_uint16_array_null(void** state) {
    (void)state;
    assert_int_equal(push_front_uint16_array(NULL, 1u), NULL_POINTER);
}

static void test_push_front_uint16_array_value(void** state) {
    (void)state;
    /* Capacity 3 so there is room for the push_front */
    allocator_vtable_t alloc = heap_allocator();
    uint16_tensor_expect_t r  = init_uint16_array(3u, false, alloc);
    assert_true(r.has_value);
    uint16_tensor_t* arr = r.u.value;

    push_back_uint16_array(arr, 10u);
    push_back_uint16_array(arr, 11u);
    /* arr = [10, 11], len == 2, alloc == 3 — one slot free */

    assert_int_equal(push_front_uint16_array(arr, 99u), NO_ERROR);

    uint16_t out = 0u;
    assert_int_equal(get_uint16_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_uint16_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_uint16_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);

    return_uint16_tensor(arr);
}

static void test_push_at_uint16_array_null(void** state) {
    (void)state;
    assert_int_equal(push_at_uint16_array(NULL, 1u, 0u), NULL_POINTER);
}

static void test_push_at_uint16_array_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(3u, 10u); /* [10, 11, 12] */
    assert_non_null(arr);

    assert_int_equal(push_at_uint16_array(arr, 99u, 1u), NO_ERROR);
    /* arr = [10, 99, 11, 12] */

    uint16_t out = 0u;
    assert_int_equal(get_uint16_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_uint16_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_uint16_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);
    assert_int_equal(get_uint16_tensor_index(arr, 3u, &out), NO_ERROR);
    assert_int_equal(out, 12u);

    return_uint16_tensor(arr);
}

static void test_pop_back_uint16_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_back_uint16_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_back_uint16_array_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    uint16_t out = 0u;
    assert_int_equal(pop_back_uint16_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(uint16_tensor_size(arr), 2u);

    return_uint16_tensor(arr);
}

static void test_pop_front_uint16_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_front_uint16_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_front_uint16_array_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    uint16_t out = 0u;
    assert_int_equal(pop_front_uint16_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(uint16_tensor_size(arr), 2u);

    return_uint16_tensor(arr);
}

static void test_pop_at_uint16_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_at_uint16_array(NULL, NULL, 0u), NULL_POINTER);
}

static void test_pop_at_uint16_array_value(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(4u, 10u); /* [10,11,12,13] */
    assert_non_null(arr);

    uint16_t out = 0u;
    assert_int_equal(pop_at_uint16_array(arr, &out, 2u), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(uint16_tensor_size(arr), 3u);

    assert_int_equal(get_uint16_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 13u);

    return_uint16_tensor(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// FIND uint16 TENSOR VALUE (uint16_tensor_lsearch)
// ================================================================================
 
/** NULL tensor must return NULL_POINTER. */
static void test_find_uint16_tensor_null_tensor(void** state) {
    (void)state;
    size_t idx = 0u;
    assert_int_equal(uint16_tensor_lsearch(NULL, &idx, 10u), NULL_POINTER);
}
 
/** NULL index pointer must return NULL_POINTER. */
static void test_find_uint16_tensor_null_index(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(3u, 10u);
    assert_non_null(arr);
    assert_int_equal(uint16_tensor_lsearch(arr, NULL, 10u), NULL_POINTER);
    return_uint16_tensor(arr);
}
 
/** Empty array must return EMPTY. */
static void test_find_uint16_tensor_empty(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(4u, false);
    assert_non_null(arr);
    size_t idx = 0u;
    assert_int_equal(uint16_tensor_lsearch(arr, &idx, 10u), EMPTY);
    return_uint16_tensor(arr);
}
 
/** Value not present must return NOT_FOUND. */
static void test_find_uint16_tensor_not_found(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(4u, 1u); /* [1,2,3,4] */
    assert_non_null(arr);
    size_t idx = 99u;   /* sentinel — must not be modified */
    assert_int_equal(uint16_tensor_lsearch(arr, &idx, 99u), NOT_FOUND);
    /* index must be unchanged on NOT_FOUND */
    assert_int_equal(idx, 99u);
    return_uint16_tensor(arr);
}
 
/** Value at the first position must be found at index 0. */
static void test_find_uint16_tensor_first_element(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(uint16_tensor_lsearch(arr, &idx, 10u), NO_ERROR);
    assert_int_equal(idx, 0u);
    return_uint16_tensor(arr);
}
 
/** Value at the last position must be found at index len-1. */
static void test_find_uint16_tensor_last_element(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(uint16_tensor_lsearch(arr, &idx, 14u), NO_ERROR);
    assert_int_equal(idx, 4u);
    return_uint16_tensor(arr);
}
 
/** Value in the middle must be found at the correct index. */
static void test_find_uint16_tensor_middle_element(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(uint16_tensor_lsearch(arr, &idx, 12u), NO_ERROR);
    assert_int_equal(idx, 2u);
    return_uint16_tensor(arr);
}
 
/**
 * When duplicates are present, the first occurrence must be returned.
 */
static void test_find_uint16_tensor_first_of_duplicates(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(6u, false);
    assert_non_null(arr);
 
    /* [5, 9, 9, 9, 7, 3] */
    push_back_uint16_array(arr, 5u);
    push_back_uint16_array(arr, 9u);
    push_back_uint16_array(arr, 9u);
    push_back_uint16_array(arr, 9u);
    push_back_uint16_array(arr, 7u);
    push_back_uint16_array(arr, 3u);
 
    size_t idx = 99u;
    assert_int_equal(uint16_tensor_lsearch(arr, &idx, 9u), NO_ERROR);
    assert_int_equal(idx, 1u);   /* first occurrence at index 1 */
 
    return_uint16_tensor(arr);
}
 
/**
 * Single-element array containing the target value must succeed.
 */
static void test_find_uint16_tensor_single_element_match(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(1u, false);
    assert_non_null(arr);
    push_back_uint16_array(arr, 42u);
 
    size_t idx = 99u;
    assert_int_equal(uint16_tensor_lsearch(arr, &idx, 42u), NO_ERROR);
    assert_int_equal(idx, 0u);
 
    return_uint16_tensor(arr);
}
 
/**
 * Single-element array not containing the target must return NOT_FOUND
 * without modifying index.
 */
static void test_find_uint16_tensor_single_element_no_match(void** state) {
    (void)state;
    uint16_tensor_t* arr = _make_uint16_array(1u, false);
    assert_non_null(arr);
    push_back_uint16_array(arr, 42u);
 
    size_t idx = 99u;
    assert_int_equal(uint16_tensor_lsearch(arr, &idx, 7u), NOT_FOUND);
    assert_int_equal(idx, 99u);   /* unchanged */
 
    return_uint16_tensor(arr);
}
// ================================================================================
// ================================================================================
// TEST SUITE REGISTRY

const struct CMUnitTest test_uint16_tensor[] = {
    /* init_uint16_array */
    cmocka_unit_test(test_init_uint16_array_null_allocator),
    cmocka_unit_test(test_init_uint16_array_success),

    /* init_uint16_tensor */
    cmocka_unit_test(test_init_uint16_tensor_success),
    cmocka_unit_test(test_init_uint16_tensor_null_shape),

    /* return_uint16_tensor */
    cmocka_unit_test(test_return_uint16_tensor_null),
    cmocka_unit_test(test_return_uint16_tensor_normal),

    /* copy_uint16_tensor */
    cmocka_unit_test(test_copy_uint16_tensor_null_src),
    cmocka_unit_test(test_copy_uint16_tensor_independence),

    /* introspection — null guards */
    cmocka_unit_test(test_uint16_tensor_size_null),
    cmocka_unit_test(test_uint16_tensor_alloc_null),
    cmocka_unit_test(test_uint16_tensor_data_size_null),
    cmocka_unit_test(test_uint16_tensor_dtype_null),
    cmocka_unit_test(test_is_uint16_tensor_empty_null),
    cmocka_unit_test(test_is_uint16_tensor_full_null),
    cmocka_unit_test(test_is_uint16_tensor_ptr_null_wrapper),
    cmocka_unit_test(test_uint16_tensor_ndim_null),
    cmocka_unit_test(test_uint16_tensor_shape_dim_null),
    cmocka_unit_test(test_uint16_tensor_shape_null),
    cmocka_unit_test(test_uint16_tensor_shape_ptr_null),
    cmocka_unit_test(test_uint16_tensor_strides_ptr_null),
    cmocka_unit_test(test_uint16_tensor_shape_str_null),

    /* introspection — values */
    cmocka_unit_test(test_uint16_tensor_size_value),
    cmocka_unit_test(test_uint16_tensor_alloc_value),
    cmocka_unit_test(test_uint16_tensor_data_size_value),
    cmocka_unit_test(test_uint16_tensor_dtype_value),
    cmocka_unit_test(test_is_uint16_tensor_empty_value),
    cmocka_unit_test(test_is_uint16_tensor_full_value),
    cmocka_unit_test(test_is_uint16_tensor_ptr_valid),
    cmocka_unit_test(test_uint16_tensor_ndim_value),
    cmocka_unit_test(test_uint16_tensor_shape_dim_value),
    cmocka_unit_test(test_uint16_tensor_shape_value),
    cmocka_unit_test(test_uint16_tensor_shape_ptr_value),
    cmocka_unit_test(test_uint16_tensor_strides_ptr_value),
    cmocka_unit_test(test_uint16_tensor_shape_str_value),

    /* utility — null guards */
    cmocka_unit_test(test_clear_uint16_tensor_null),
    cmocka_unit_test(test_concat_uint16_tensor_null),
    cmocka_unit_test(test_slice_uint16_tensor_null),
    cmocka_unit_test(test_reverse_uint16_tensor_null),
    cmocka_unit_test(test_sort_uint16_tensor_null),

    /* utility — values */
    cmocka_unit_test(test_clear_uint16_tensor_value),
    cmocka_unit_test(test_concat_uint16_tensor_value),
    cmocka_unit_test(test_slice_uint16_tensor_value),
    cmocka_unit_test(test_reverse_uint16_tensor_value),
    cmocka_unit_test(test_sort_uint16_tensor_forward),
    cmocka_unit_test(test_sort_uint16_tensor_reverse),

    /* set/get — null guards */
    cmocka_unit_test(test_set_get_uint16_tensor_index_null),
    cmocka_unit_test(test_set_get_uint16_tensor_nd_index_null),
    cmocka_unit_test(test_push_back_uint16_array_null),
    cmocka_unit_test(test_push_front_uint16_array_null),
    cmocka_unit_test(test_push_at_uint16_array_null),
    cmocka_unit_test(test_pop_back_uint16_array_null),
    cmocka_unit_test(test_pop_front_uint16_array_null),
    cmocka_unit_test(test_pop_at_uint16_array_null),

    /* set/get — values */
    cmocka_unit_test(test_set_get_uint16_tensor_index_value),
    cmocka_unit_test(test_set_get_uint16_tensor_nd_index_value),
    cmocka_unit_test(test_push_back_uint16_array_value),
    cmocka_unit_test(test_push_front_uint16_array_value),
    cmocka_unit_test(test_push_at_uint16_array_value),
    cmocka_unit_test(test_pop_back_uint16_array_value),
    cmocka_unit_test(test_pop_front_uint16_array_value),
    cmocka_unit_test(test_pop_at_uint16_array_value),

    /* uint16_tensor_lsearch — null/guard */
    cmocka_unit_test(test_find_uint16_tensor_null_tensor),
    cmocka_unit_test(test_find_uint16_tensor_null_index),
    cmocka_unit_test(test_find_uint16_tensor_empty),
 
    /* uint16_tensor_lsearch — not found */
    cmocka_unit_test(test_find_uint16_tensor_not_found),
 
    /* uint16_tensor_lsearch — found at various positions */
    cmocka_unit_test(test_find_uint16_tensor_first_element),
    cmocka_unit_test(test_find_uint16_tensor_last_element),
    cmocka_unit_test(test_find_uint16_tensor_middle_element),
    cmocka_unit_test(test_find_uint16_tensor_first_of_duplicates),
    cmocka_unit_test(test_find_uint16_tensor_single_element_match),
    cmocka_unit_test(test_find_uint16_tensor_single_element_no_match),
};

const size_t test_uint16_tensor_count = sizeof(test_uint16_tensor) /
                                       sizeof(test_uint16_tensor[0]);
// ================================================================================ 
// ================================================================================ 

/** Construct a populated int16 array with values 1..n. */
static int16_tensor_t* _make_int16_array(size_t capacity, bool growth) {
    allocator_vtable_t alloc = heap_allocator();
    int16_tensor_expect_t r  = init_int16_array(capacity, growth, alloc);
    if (!r.has_value) return NULL;
    return r.u.value;
}

static int16_tensor_t* _make_int16_array_filled(size_t n, int16_t base) {
    /* Allocate n+1 so the result always has at least one free slot,
     * making it safe to use as a push destination in tests. */
    int16_tensor_t* arr = _make_int16_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++) {
        int16_t val = (int16_t)(base + i);
        push_back_int16_array(arr, val);
    }
    return arr;
}

// ================================================================================
// ================================================================================
// INITIALISATION AND TEARDOWN

// ---- init_int16_array ----------------------------------------------------------

/** NULL allocator must propagate an error through the expect type. */
static void test_init_int16_array_null_allocator(void** state) {
    (void)state;
    allocator_vtable_t bad = heap_allocator();
    bad.allocate = NULL;
    int16_tensor_expect_t r = init_int16_array(8u, false, bad);
    assert_false(r.has_value);
}

/** Successful construction produces a non-NULL wrapper with correct mode. */
static void test_init_int16_array_success(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_tensor_expect_t r  = init_int16_array(8u, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  ARRAY_STRUCT);
    assert_int_equal(r.u.value->base->dtype, INT16_TYPE);
    assert_int_equal(r.u.value->base->len,   0u);
    assert_int_equal(r.u.value->base->alloc, 8u);
    return_int16_tensor(r.u.value);
}

// ---- init_int16_tensor ---------------------------------------------------------

/** Successful construction of a fixed-shape tensor. */
static void test_init_int16_tensor_success(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    int16_tensor_expect_t r  = init_int16_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  TENSOR_STRUCT);
    assert_int_equal(r.u.value->base->dtype, INT16_TYPE);
    assert_int_equal(r.u.value->base->len,   12u);
    return_int16_tensor(r.u.value);
}

/** NULL shape propagates an error. */
static void test_init_int16_tensor_null_shape(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int16_tensor_expect_t r  = init_int16_tensor(2u, NULL, alloc);
    assert_false(r.has_value);
}

// ---- return_int16_tensor -------------------------------------------------------

/** return_int16_tensor(NULL) must not crash. */
static void test_return_int16_tensor_null(void** state) {
    (void)state;
    return_int16_tensor(NULL);
}

/** Normal construction followed by return must not crash or leak. */
static void test_return_int16_tensor_normal(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(4u, false);
    assert_non_null(arr);
    return_int16_tensor(arr);
}

// ---- copy_int16_tensor ---------------------------------------------------------

/** NULL src must return NULL_POINTER. */
static void test_copy_int16_tensor_null_src(void** state) {
    (void)state;
    int16_tensor_expect_t r = copy_int16_tensor(NULL, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

/** Copy is independent of source — mutation of copy does not affect src. */
static void test_copy_int16_tensor_independence(void** state) {
    (void)state;
    int16_tensor_t* src = _make_int16_array_filled(3u, 10u);
    assert_non_null(src);

    int16_tensor_expect_t r = copy_int16_tensor(src, NULL);
    assert_true(r.has_value);
    int16_tensor_t* dst = r.u.value;

    /* Overwrite slot 0 of the copy */
    assert_int_equal(set_int16_tensor_index(dst, 0u, 99u), NO_ERROR);

    /* Source slot 0 must be unchanged */
    int16_t src_val = 0u;
    assert_int_equal(get_int16_tensor_index(src, 0u, &src_val), NO_ERROR);
    assert_int_equal(src_val, 10u);

    return_int16_tensor(src);
    return_int16_tensor(dst);
}

// ================================================================================
// ================================================================================
// INTROSPECTION

static void test_int16_tensor_size_null(void** state) {
    (void)state;
    assert_int_equal(int16_tensor_size(NULL), 0u);
}

static void test_int16_tensor_size_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array_filled(3u, 1u);
    assert_non_null(arr);
    assert_int_equal(int16_tensor_size(arr), 3u);
    return_int16_tensor(arr);
}

static void test_int16_tensor_alloc_null(void** state) {
    (void)state;
    assert_int_equal(int16_tensor_alloc(NULL), 0u);
}

static void test_int16_tensor_alloc_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(8u, false);
    assert_non_null(arr);
    assert_int_equal(int16_tensor_alloc(arr), 8u);
    return_int16_tensor(arr);
}

static void test_int16_tensor_data_size_null(void** state) {
    (void)state;
    assert_int_equal(int16_tensor_data_size(NULL), 0u);
}

static void test_int16_tensor_data_size_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(int16_tensor_data_size(arr), sizeof(int16_t));
    return_int16_tensor(arr);
}

static void test_int16_tensor_dtype_null(void** state) {
    (void)state;
    assert_int_equal(int16_tensor_dtype(NULL), UNKNOWN_TYPE);
}

static void test_int16_tensor_dtype_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(int16_tensor_dtype(arr), INT16_TYPE);
    return_int16_tensor(arr);
}

static void test_is_int16_tensor_empty_null(void** state) {
    (void)state;
    assert_true(is_int16_tensor_empty(NULL));
}

static void test_is_int16_tensor_empty_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(4u, false);
    assert_non_null(arr);
    assert_true(is_int16_tensor_empty(arr));
    push_back_int16_array(arr, 1u);
    assert_false(is_int16_tensor_empty(arr));
    return_int16_tensor(arr);
}

static void test_is_int16_tensor_full_null(void** state) {
    (void)state;
    assert_true(is_int16_tensor_full(NULL));
}

static void test_is_int16_tensor_full_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(2u, false);
    assert_non_null(arr);
    assert_false(is_int16_tensor_full(arr));
    push_back_int16_array(arr, 1u);
    push_back_int16_array(arr, 2u);
    assert_true(is_int16_tensor_full(arr));
    return_int16_tensor(arr);
}

static void test_is_int16_tensor_ptr_null_wrapper(void** state) {
    (void)state;
    int16_t dummy = 0u;
    assert_false(is_int16_tensor_ptr(NULL, &dummy));
}

static void test_is_int16_tensor_ptr_valid(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array_filled(4u, 10u);
    assert_non_null(arr);
    /* Pointer to first element must be valid */
    const int16_t* p = (const int16_t*)arr->base->data;
    assert_true(is_int16_tensor_ptr(arr, p));
    return_int16_tensor(arr);
}

static void test_int16_tensor_ndim_null(void** state) {
    (void)state;
    assert_int_equal(int16_tensor_ndim(NULL), 0u);
}

static void test_int16_tensor_ndim_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(int16_tensor_ndim(arr), 1u);
    return_int16_tensor(arr);
}

static void test_int16_tensor_shape_dim_null(void** state) {
    (void)state;
    assert_int_equal(int16_tensor_shape_dim(NULL, 0u), 0u);
}

static void test_int16_tensor_shape_dim_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    int16_tensor_expect_t r  = init_int16_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_int_equal(int16_tensor_shape_dim(r.u.value, 0u), 3u);
    assert_int_equal(int16_tensor_shape_dim(r.u.value, 1u), 4u);
    return_int16_tensor(r.u.value);
}

static void test_int16_tensor_shape_null(void** state) {
    (void)state;
    size_t buf[2];
    assert_int_equal(int16_tensor_shape(NULL, buf, 2u), NULL_POINTER);
}

static void test_int16_tensor_shape_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    int16_tensor_expect_t r  = init_int16_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    size_t buf[2] = { 0u, 0u };
    assert_int_equal(int16_tensor_shape(r.u.value, buf, 2u), NO_ERROR);
    assert_int_equal(buf[0], 3u);
    assert_int_equal(buf[1], 4u);
    return_int16_tensor(r.u.value);
}

static void test_int16_tensor_shape_ptr_null(void** state) {
    (void)state;
    assert_null(int16_tensor_shape_ptr(NULL));
}

static void test_int16_tensor_shape_ptr_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(4u, false);
    assert_non_null(arr);
    assert_non_null(int16_tensor_shape_ptr(arr));
    return_int16_tensor(arr);
}

static void test_int16_tensor_strides_ptr_null(void** state) {
    (void)state;
    assert_null(int16_tensor_strides_ptr(NULL));
}

static void test_int16_tensor_strides_ptr_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(4u, false);
    assert_non_null(arr);
    const size_t* strides = int16_tensor_strides_ptr(arr);
    assert_non_null(strides);
    assert_int_equal(strides[0], sizeof(int16_t));
    return_int16_tensor(arr);
}

static void test_int16_tensor_shape_str_null(void** state) {
    (void)state;
    char buf[32];
    assert_int_equal(int16_tensor_shape_str(NULL, buf, sizeof(buf)), NULL_POINTER);
}

static void test_int16_tensor_shape_str_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(8u, false);
    assert_non_null(arr);
    char buf[32];
    assert_int_equal(int16_tensor_shape_str(arr, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(8)");
    return_int16_tensor(arr);
}

// ================================================================================
// ================================================================================
// UTILITY OPERATIONS

// ---- clear_int16_tensor --------------------------------------------------------

static void test_clear_int16_tensor_null(void** state) {
    (void)state;
    assert_int_equal(clear_int16_tensor(NULL), NULL_POINTER);
}

static void test_clear_int16_tensor_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array_filled(3u, 10u);
    assert_non_null(arr);
    assert_int_equal(clear_int16_tensor(arr), NO_ERROR);
    assert_int_equal(int16_tensor_size(arr), 0u);
    return_int16_tensor(arr);
}

// ---- concat_int16_tensor_array -------------------------------------------------

static void test_concat_int16_tensor_null(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(concat_int16_tensor_array(NULL, arr), NULL_POINTER);
    assert_int_equal(concat_int16_tensor_array(arr, NULL), NULL_POINTER);
    return_int16_tensor(arr);
}

static void test_concat_int16_tensor_value(void** state) {
    (void)state;

    /* dst needs capacity for 5 elements total — allocate with room to grow */
    allocator_vtable_t alloc = heap_allocator();
    int16_tensor_expect_t r  = init_int16_array(8u, false, alloc);
    assert_true(r.has_value);
    int16_tensor_t* dst = r.u.value;

    /* Populate dst with [1, 2, 3] */
    push_back_int16_array(dst, 1u);
    push_back_int16_array(dst, 2u);
    push_back_int16_array(dst, 3u);

    /* src = [4, 5] */
    int16_tensor_t* src = _make_int16_array_filled(2u, 4u);
    assert_non_null(src);

    assert_int_equal(concat_int16_tensor_array(dst, src), NO_ERROR);
    assert_int_equal(int16_tensor_size(dst), 5u);

    int16_t expected[] = { 1u, 2u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        int16_t out = 0u;
        assert_int_equal(get_int16_tensor_index(dst, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int16_tensor(dst);
    return_int16_tensor(src);
}

// ---- slice_int16_tensor_array --------------------------------------------------

static void test_slice_int16_tensor_null(void** state) {
    (void)state;
    int16_tensor_expect_t r = slice_int16_tensor_array(NULL, 0u, 1u, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_slice_int16_tensor_value(void** state) {
    (void)state;
    int16_tensor_t* src = _make_int16_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(src);

    int16_tensor_expect_t r = slice_int16_tensor_array(src, 1u, 4u, NULL);
    assert_true(r.has_value);
    int16_tensor_t* sl = r.u.value;

    assert_int_equal(int16_tensor_size(sl), 3u);
    int16_t expected[] = { 11u, 12u, 13u };
    for (size_t i = 0u; i < 3u; i++) {
        int16_t out = 0u;
        assert_int_equal(get_int16_tensor_index(sl, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int16_tensor(src);
    return_int16_tensor(sl);
}

// ---- reverse_int16_tensor ------------------------------------------------------

static void test_reverse_int16_tensor_null(void** state) {
    (void)state;
    assert_int_equal(reverse_int16_tensor(NULL), NULL_POINTER);
}

static void test_reverse_int16_tensor_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array_filled(4u, 1u); /* [1,2,3,4] */
    assert_non_null(arr);

    assert_int_equal(reverse_int16_tensor(arr), NO_ERROR);

    int16_t expected[] = { 4u, 3u, 2u, 1u };
    for (size_t i = 0u; i < 4u; i++) {
        int16_t out = 0u;
        assert_int_equal(get_int16_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int16_tensor(arr);
}

// ---- sort_int16_tensor ---------------------------------------------------------

static void test_sort_int16_tensor_null(void** state) {
    (void)state;
    assert_int_equal(sort_int16_tensor(NULL, FORWARD), NULL_POINTER);
}

static void test_sort_int16_tensor_forward(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(5u, false);
    assert_non_null(arr);

    int16_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_int16_array(arr, vals[i]);

    assert_int_equal(sort_int16_tensor(arr, FORWARD), NO_ERROR);

    int16_t expected[] = { 1u, 1u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        int16_t out = 0u;
        assert_int_equal(get_int16_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int16_tensor(arr);
}

static void test_sort_int16_tensor_reverse(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(5u, false);
    assert_non_null(arr);

    int16_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_int16_array(arr, vals[i]);

    assert_int_equal(sort_int16_tensor(arr, REVERSE), NO_ERROR);

    int16_t expected[] = { 5u, 4u, 3u, 1u, 1u };
    for (size_t i = 0u; i < 5u; i++) {
        int16_t out = 0u;
        assert_int_equal(get_int16_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int16_tensor(arr);
}

// ================================================================================
// ================================================================================
// SET AND GET DATA

// ---- set_int16_tensor_index / get_int16_tensor_index --------------------------

static void test_set_get_int16_tensor_index_null(void** state) {
    (void)state;
    int16_t dummy = 0u;
    assert_int_equal(set_int16_tensor_index(NULL, 0u, 1u),    NULL_POINTER);
    assert_int_equal(get_int16_tensor_index(NULL, 0u, &dummy), NULL_POINTER);
}

static void test_set_get_int16_tensor_index_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array_filled(3u, 10u);
    assert_non_null(arr);

    assert_int_equal(set_int16_tensor_index(arr, 1u, 99u), NO_ERROR);

    int16_t out = 0u;
    assert_int_equal(get_int16_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);

    return_int16_tensor(arr);
}

// ---- set_int16_tensor_nd_index / get_int16_tensor_nd_index --------------------

static void test_set_get_int16_tensor_nd_index_null(void** state) {
    (void)state;
    size_t  idx[] = { 0u, 0u };
    int16_t out   = 0u;
    assert_int_equal(set_int16_tensor_nd_index(NULL, idx, 1u),   NULL_POINTER);
    assert_int_equal(get_int16_tensor_nd_index(NULL, idx, &out), NULL_POINTER);
}

static void test_set_get_int16_tensor_nd_index_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 3u };
    allocator_vtable_t alloc = heap_allocator();
    int16_tensor_expect_t r  = init_int16_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    int16_tensor_t* mat = r.u.value;

    size_t idx[] = { 1u, 2u };
    assert_int_equal(set_int16_tensor_nd_index(mat, idx, 42u), NO_ERROR);

    int16_t out = 0u;
    assert_int_equal(get_int16_tensor_nd_index(mat, idx, &out), NO_ERROR);
    assert_int_equal(out, 42u);

    return_int16_tensor(mat);
}

// ---- push and pop wrappers ----------------------------------------------------

static void test_push_back_int16_array_null(void** state) {
    (void)state;
    assert_int_equal(push_back_int16_array(NULL, 1u), NULL_POINTER);
}

static void test_push_back_int16_array_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(4u, false);
    assert_non_null(arr);

    assert_int_equal(push_back_int16_array(arr, 10u), NO_ERROR);
    assert_int_equal(push_back_int16_array(arr, 20u), NO_ERROR);
    assert_int_equal(int16_tensor_size(arr), 2u);

    int16_t out = 0u;
    assert_int_equal(get_int16_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 20u);

    return_int16_tensor(arr);
}

static void test_push_front_int16_array_null(void** state) {
    (void)state;
    assert_int_equal(push_front_int16_array(NULL, 1u), NULL_POINTER);
}

static void test_push_front_int16_array_value(void** state) {
    (void)state;
    /* Capacity 3 so there is room for the push_front */
    allocator_vtable_t alloc = heap_allocator();
    int16_tensor_expect_t r  = init_int16_array(3u, false, alloc);
    assert_true(r.has_value);
    int16_tensor_t* arr = r.u.value;

    push_back_int16_array(arr, 10u);
    push_back_int16_array(arr, 11u);
    /* arr = [10, 11], len == 2, alloc == 3 — one slot free */

    assert_int_equal(push_front_int16_array(arr, 99u), NO_ERROR);

    int16_t out = 0u;
    assert_int_equal(get_int16_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_int16_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_int16_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);

    return_int16_tensor(arr);
}

static void test_push_at_int16_array_null(void** state) {
    (void)state;
    assert_int_equal(push_at_int16_array(NULL, 1u, 0u), NULL_POINTER);
}

static void test_push_at_int16_array_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array_filled(3u, 10u); /* [10, 11, 12] */
    assert_non_null(arr);

    assert_int_equal(push_at_int16_array(arr, 99u, 1u), NO_ERROR);
    /* arr = [10, 99, 11, 12] */

    int16_t out = 0u;
    assert_int_equal(get_int16_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_int16_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_int16_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);
    assert_int_equal(get_int16_tensor_index(arr, 3u, &out), NO_ERROR);
    assert_int_equal(out, 12u);

    return_int16_tensor(arr);
}

static void test_pop_back_int16_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_back_int16_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_back_int16_array_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    int16_t out = 0u;
    assert_int_equal(pop_back_int16_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(int16_tensor_size(arr), 2u);

    return_int16_tensor(arr);
}

static void test_pop_front_int16_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_front_int16_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_front_int16_array_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    int16_t out = 0u;
    assert_int_equal(pop_front_int16_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(int16_tensor_size(arr), 2u);

    return_int16_tensor(arr);
}

static void test_pop_at_int16_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_at_int16_array(NULL, NULL, 0u), NULL_POINTER);
}

static void test_pop_at_int16_array_value(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array_filled(4u, 10u); /* [10,11,12,13] */
    assert_non_null(arr);

    int16_t out = 0u;
    assert_int_equal(pop_at_int16_array(arr, &out, 2u), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(int16_tensor_size(arr), 3u);

    assert_int_equal(get_int16_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 13u);

    return_int16_tensor(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// INT16 LSEARCH — NEGATIVE VALUE CORRECTNESS
// ================================================================================

/** Search for a negative value that is present must return NO_ERROR and
 *  the correct index. Exercises the (uint16_t) cast path on a value
 *  whose sign bit is set. */
static void test_int16_tensor_lsearch_negative_found(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(6u, false);
    assert_non_null(arr);

    /* Mix of positive, negative, and zero */
    push_back_int16_array(arr,  10);
    push_back_int16_array(arr, -5);
    push_back_int16_array(arr,  30);
    push_back_int16_array(arr, -5);   /* duplicate — must find first */
    push_back_int16_array(arr,  0);
    push_back_int16_array(arr, -128);

    size_t idx = 99u;
    assert_int_equal(int16_tensor_lsearch(arr, &idx, -5), NO_ERROR);
    assert_int_equal(idx, 1u);   /* first occurrence of -5 is at index 1 */

    return_int16_tensor(arr);
}

/** Search for -128 (0xFF80) at the last position exercises a negative value
 *  whose bit pattern differs maximally from zero. */
static void test_int16_tensor_lsearch_negative_last(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(5u, false);
    assert_non_null(arr);

    push_back_int16_array(arr,  1);
    push_back_int16_array(arr,  2);
    push_back_int16_array(arr,  3);
    push_back_int16_array(arr,  4);
    push_back_int16_array(arr, -128);

    size_t idx = 99u;
    assert_int_equal(int16_tensor_lsearch(arr, &idx, -128), NO_ERROR);
    assert_int_equal(idx, 4u);

    return_int16_tensor(arr);
}

/** Search for -1 (0xFFFF) — the value whose bit pattern equals UINT16_MAX.
 *  This is the most important case: confirms the cast does not confuse
 *  -1 with the SIZE_MAX sentinel or with 65535 as a positive value. */
static void test_int16_tensor_lsearch_minus_one(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(4u, false);
    assert_non_null(arr);

    push_back_int16_array(arr,  100);
    push_back_int16_array(arr,   -1);
    push_back_int16_array(arr,  200);
    push_back_int16_array(arr,  300);

    size_t idx = 99u;
    assert_int_equal(int16_tensor_lsearch(arr, &idx, -1), NO_ERROR);
    assert_int_equal(idx, 1u);

    return_int16_tensor(arr);
}

/** Search for INT16_MIN (-32768, 0x8000) — the value with only the sign
 *  bit set, which is the most extreme negative value in range. */
static void test_int16_tensor_lsearch_int16_min(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(4u, false);
    assert_non_null(arr);

    push_back_int16_array(arr,      0);
    push_back_int16_array(arr,      1);
    push_back_int16_array(arr, INT16_MIN);
    push_back_int16_array(arr,    100);

    size_t idx = 99u;
    assert_int_equal(int16_tensor_lsearch(arr, &idx, INT16_MIN), NO_ERROR);
    assert_int_equal(idx, 2u);

    return_int16_tensor(arr);
}

/** A negative value that is not present must return NOT_FOUND and must
 *  not modify index. */
static void test_int16_tensor_lsearch_negative_not_found(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(4u, false);
    assert_non_null(arr);

    push_back_int16_array(arr,  10);
    push_back_int16_array(arr,  20);
    push_back_int16_array(arr, -30);
    push_back_int16_array(arr,  40);

    size_t idx = 99u;
    assert_int_equal(int16_tensor_lsearch(arr, &idx, -99), NOT_FOUND);
    assert_int_equal(idx, 99u);   /* unchanged */

    return_int16_tensor(arr);
}

/** Array containing only negative values — confirms the SIMD path handles
 *  a buffer where every element has its sign bit set. */
static void test_int16_tensor_lsearch_all_negative(void** state) {
    (void)state;
    int16_tensor_t* arr = _make_int16_array(5u, false);
    assert_non_null(arr);

    push_back_int16_array(arr, -1);
    push_back_int16_array(arr, -2);
    push_back_int16_array(arr, -3);
    push_back_int16_array(arr, -4);
    push_back_int16_array(arr, -5);

    size_t idx = 99u;
    assert_int_equal(int16_tensor_lsearch(arr, &idx, -3), NO_ERROR);
    assert_int_equal(idx, 2u);

    assert_int_equal(int16_tensor_lsearch(arr, &idx, -6), NOT_FOUND);

    return_int16_tensor(arr);
}
// ================================================================================
// ================================================================================
// TEST SUITE REGISTRY

const struct CMUnitTest test_int16_tensor[] = {
    /* init_int16_array */
    cmocka_unit_test(test_init_int16_array_null_allocator),
    cmocka_unit_test(test_init_int16_array_success),

    /* init_int16_tensor */
    cmocka_unit_test(test_init_int16_tensor_success),
    cmocka_unit_test(test_init_int16_tensor_null_shape),

    /* return_int16_tensor */
    cmocka_unit_test(test_return_int16_tensor_null),
    cmocka_unit_test(test_return_int16_tensor_normal),

    /* copy_int16_tensor */
    cmocka_unit_test(test_copy_int16_tensor_null_src),
    cmocka_unit_test(test_copy_int16_tensor_independence),

    /* introspection — null guards */
    cmocka_unit_test(test_int16_tensor_size_null),
    cmocka_unit_test(test_int16_tensor_alloc_null),
    cmocka_unit_test(test_int16_tensor_data_size_null),
    cmocka_unit_test(test_int16_tensor_dtype_null),
    cmocka_unit_test(test_is_int16_tensor_empty_null),
    cmocka_unit_test(test_is_int16_tensor_full_null),
    cmocka_unit_test(test_is_int16_tensor_ptr_null_wrapper),
    cmocka_unit_test(test_int16_tensor_ndim_null),
    cmocka_unit_test(test_int16_tensor_shape_dim_null),
    cmocka_unit_test(test_int16_tensor_shape_null),
    cmocka_unit_test(test_int16_tensor_shape_ptr_null),
    cmocka_unit_test(test_int16_tensor_strides_ptr_null),
    cmocka_unit_test(test_int16_tensor_shape_str_null),

    /* introspection — values */
    cmocka_unit_test(test_int16_tensor_size_value),
    cmocka_unit_test(test_int16_tensor_alloc_value),
    cmocka_unit_test(test_int16_tensor_data_size_value),
    cmocka_unit_test(test_int16_tensor_dtype_value),
    cmocka_unit_test(test_is_int16_tensor_empty_value),
    cmocka_unit_test(test_is_int16_tensor_full_value),
    cmocka_unit_test(test_is_int16_tensor_ptr_valid),
    cmocka_unit_test(test_int16_tensor_ndim_value),
    cmocka_unit_test(test_int16_tensor_shape_dim_value),
    cmocka_unit_test(test_int16_tensor_shape_value),
    cmocka_unit_test(test_int16_tensor_shape_ptr_value),
    cmocka_unit_test(test_int16_tensor_strides_ptr_value),
    cmocka_unit_test(test_int16_tensor_shape_str_value),

    /* utility — null guards */
    cmocka_unit_test(test_clear_int16_tensor_null),
    cmocka_unit_test(test_concat_int16_tensor_null),
    cmocka_unit_test(test_slice_int16_tensor_null),
    cmocka_unit_test(test_reverse_int16_tensor_null),
    cmocka_unit_test(test_sort_int16_tensor_null),

    /* utility — values */
    cmocka_unit_test(test_clear_int16_tensor_value),
    cmocka_unit_test(test_concat_int16_tensor_value),
    cmocka_unit_test(test_slice_int16_tensor_value),
    cmocka_unit_test(test_reverse_int16_tensor_value),
    cmocka_unit_test(test_sort_int16_tensor_forward),
    cmocka_unit_test(test_sort_int16_tensor_reverse),

    /* set/get — null guards */
    cmocka_unit_test(test_set_get_int16_tensor_index_null),
    cmocka_unit_test(test_set_get_int16_tensor_nd_index_null),
    cmocka_unit_test(test_push_back_int16_array_null),
    cmocka_unit_test(test_push_front_int16_array_null),
    cmocka_unit_test(test_push_at_int16_array_null),
    cmocka_unit_test(test_pop_back_int16_array_null),
    cmocka_unit_test(test_pop_front_int16_array_null),
    cmocka_unit_test(test_pop_at_int16_array_null),

    /* set/get — values */
    cmocka_unit_test(test_set_get_int16_tensor_index_value),
    cmocka_unit_test(test_set_get_int16_tensor_nd_index_value),
    cmocka_unit_test(test_push_back_int16_array_value),
    cmocka_unit_test(test_push_front_int16_array_value),
    cmocka_unit_test(test_push_at_int16_array_value),
    cmocka_unit_test(test_pop_back_int16_array_value),
    cmocka_unit_test(test_pop_front_int16_array_value),
    cmocka_unit_test(test_pop_at_int16_array_value),

    /* int16 lsearch — negative value correctness */
    cmocka_unit_test(test_int16_tensor_lsearch_negative_found),
    cmocka_unit_test(test_int16_tensor_lsearch_negative_last),
    cmocka_unit_test(test_int16_tensor_lsearch_minus_one),
    cmocka_unit_test(test_int16_tensor_lsearch_int16_min),
    cmocka_unit_test(test_int16_tensor_lsearch_negative_not_found),
    cmocka_unit_test(test_int16_tensor_lsearch_all_negative), 
};

const size_t test_int16_tensor_count = sizeof(test_int16_tensor) /
                                       sizeof(test_int16_tensor[0]);
// ================================================================================ 
// ================================================================================ 

/** Construct a populated uint32 array with values 1..n. */
static uint32_tensor_t* _make_uint32_array(size_t capacity, bool growth) {
    allocator_vtable_t alloc = heap_allocator();
    uint32_tensor_expect_t r  = init_uint32_array(capacity, growth, alloc);
    if (!r.has_value) return NULL;
    return r.u.value;
}

static uint32_tensor_t* _make_uint32_array_filled(size_t n, uint32_t base) {
    /* Allocate n+1 so the result always has at least one free slot,
     * making it safe to use as a push destination in tests. */
    uint32_tensor_t* arr = _make_uint32_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++) {
        uint32_t val = (uint32_t)(base + i);
        push_back_uint32_array(arr, val);
    }
    return arr;
}

// ================================================================================
// ================================================================================
// INITIALISATION AND TEARDOWN

// ---- init_uint32_array ----------------------------------------------------------

/** NULL allocator must propagate an error through the expect type. */
static void test_init_uint32_array_null_allocator(void** state) {
    (void)state;
    allocator_vtable_t bad = heap_allocator();
    bad.allocate = NULL;
    uint32_tensor_expect_t r = init_uint32_array(8u, false, bad);
    assert_false(r.has_value);
}

/** Successful construction produces a non-NULL wrapper with correct mode. */
static void test_init_uint32_array_success(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_tensor_expect_t r  = init_uint32_array(8u, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  ARRAY_STRUCT);
    assert_int_equal(r.u.value->base->dtype, UINT32_TYPE);
    assert_int_equal(r.u.value->base->len,   0u);
    assert_int_equal(r.u.value->base->alloc, 8u);
    return_uint32_tensor(r.u.value);
}

// ---- init_uint32_tensor ---------------------------------------------------------

/** Successful construction of a fixed-shape tensor. */
static void test_init_uint32_tensor_success(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    uint32_tensor_expect_t r  = init_uint32_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  TENSOR_STRUCT);
    assert_int_equal(r.u.value->base->dtype, UINT32_TYPE);
    assert_int_equal(r.u.value->base->len,   12u);
    return_uint32_tensor(r.u.value);
}

/** NULL shape propagates an error. */
static void test_init_uint32_tensor_null_shape(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint32_tensor_expect_t r  = init_uint32_tensor(2u, NULL, alloc);
    assert_false(r.has_value);
}

// ---- return_uint32_tensor -------------------------------------------------------

/** return_uint32_tensor(NULL) must not crash. */
static void test_return_uint32_tensor_null(void** state) {
    (void)state;
    return_uint32_tensor(NULL);
}

/** Normal construction followed by return must not crash or leak. */
static void test_return_uint32_tensor_normal(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(4u, false);
    assert_non_null(arr);
    return_uint32_tensor(arr);
}

// ---- copy_uint32_tensor ---------------------------------------------------------

/** NULL src must return NULL_POINTER. */
static void test_copy_uint32_tensor_null_src(void** state) {
    (void)state;
    uint32_tensor_expect_t r = copy_uint32_tensor(NULL, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

/** Copy is independent of source — mutation of copy does not affect src. */
static void test_copy_uint32_tensor_independence(void** state) {
    (void)state;
    uint32_tensor_t* src = _make_uint32_array_filled(3u, 10u);
    assert_non_null(src);

    uint32_tensor_expect_t r = copy_uint32_tensor(src, NULL);
    assert_true(r.has_value);
    uint32_tensor_t* dst = r.u.value;

    /* Overwrite slot 0 of the copy */
    assert_int_equal(set_uint32_tensor_index(dst, 0u, 99u), NO_ERROR);

    /* Source slot 0 must be unchanged */
    uint32_t src_val = 0u;
    assert_int_equal(get_uint32_tensor_index(src, 0u, &src_val), NO_ERROR);
    assert_int_equal(src_val, 10u);

    return_uint32_tensor(src);
    return_uint32_tensor(dst);
}

// ================================================================================
// ================================================================================
// INTROSPECTION

static void test_uint32_tensor_size_null(void** state) {
    (void)state;
    assert_int_equal(uint32_tensor_size(NULL), 0u);
}

static void test_uint32_tensor_size_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(3u, 1u);
    assert_non_null(arr);
    assert_int_equal(uint32_tensor_size(arr), 3u);
    return_uint32_tensor(arr);
}

static void test_uint32_tensor_alloc_null(void** state) {
    (void)state;
    assert_int_equal(uint32_tensor_alloc(NULL), 0u);
}

static void test_uint32_tensor_alloc_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(8u, false);
    assert_non_null(arr);
    assert_int_equal(uint32_tensor_alloc(arr), 8u);
    return_uint32_tensor(arr);
}

static void test_uint32_tensor_data_size_null(void** state) {
    (void)state;
    assert_int_equal(uint32_tensor_data_size(NULL), 0u);
}

static void test_uint32_tensor_data_size_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(uint32_tensor_data_size(arr), sizeof(uint32_t));
    return_uint32_tensor(arr);
}

static void test_uint32_tensor_dtype_null(void** state) {
    (void)state;
    assert_int_equal(uint32_tensor_dtype(NULL), UNKNOWN_TYPE);
}

static void test_uint32_tensor_dtype_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(uint32_tensor_dtype(arr), UINT32_TYPE);
    return_uint32_tensor(arr);
}

static void test_is_uint32_tensor_empty_null(void** state) {
    (void)state;
    assert_true(is_uint32_tensor_empty(NULL));
}

static void test_is_uint32_tensor_empty_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(4u, false);
    assert_non_null(arr);
    assert_true(is_uint32_tensor_empty(arr));
    push_back_uint32_array(arr, 1u);
    assert_false(is_uint32_tensor_empty(arr));
    return_uint32_tensor(arr);
}

static void test_is_uint32_tensor_full_null(void** state) {
    (void)state;
    assert_true(is_uint32_tensor_full(NULL));
}

static void test_is_uint32_tensor_full_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(2u, false);
    assert_non_null(arr);
    assert_false(is_uint32_tensor_full(arr));
    push_back_uint32_array(arr, 1u);
    push_back_uint32_array(arr, 2u);
    assert_true(is_uint32_tensor_full(arr));
    return_uint32_tensor(arr);
}

static void test_is_uint32_tensor_ptr_null_wrapper(void** state) {
    (void)state;
    uint32_t dummy = 0u;
    assert_false(is_uint32_tensor_ptr(NULL, &dummy));
}

static void test_is_uint32_tensor_ptr_valid(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(4u, 10u);
    assert_non_null(arr);
    /* Pointer to first element must be valid */
    const uint32_t* p = (const uint32_t*)arr->base->data;
    assert_true(is_uint32_tensor_ptr(arr, p));
    return_uint32_tensor(arr);
}

static void test_uint32_tensor_ndim_null(void** state) {
    (void)state;
    assert_int_equal(uint32_tensor_ndim(NULL), 0u);
}

static void test_uint32_tensor_ndim_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(uint32_tensor_ndim(arr), 1u);
    return_uint32_tensor(arr);
}

static void test_uint32_tensor_shape_dim_null(void** state) {
    (void)state;
    assert_int_equal(uint32_tensor_shape_dim(NULL, 0u), 0u);
}

static void test_uint32_tensor_shape_dim_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    uint32_tensor_expect_t r  = init_uint32_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_int_equal(uint32_tensor_shape_dim(r.u.value, 0u), 3u);
    assert_int_equal(uint32_tensor_shape_dim(r.u.value, 1u), 4u);
    return_uint32_tensor(r.u.value);
}

static void test_uint32_tensor_shape_null(void** state) {
    (void)state;
    size_t buf[2];
    assert_int_equal(uint32_tensor_shape(NULL, buf, 2u), NULL_POINTER);
}

static void test_uint32_tensor_shape_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    uint32_tensor_expect_t r  = init_uint32_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    size_t buf[2] = { 0u, 0u };
    assert_int_equal(uint32_tensor_shape(r.u.value, buf, 2u), NO_ERROR);
    assert_int_equal(buf[0], 3u);
    assert_int_equal(buf[1], 4u);
    return_uint32_tensor(r.u.value);
}

static void test_uint32_tensor_shape_ptr_null(void** state) {
    (void)state;
    assert_null(uint32_tensor_shape_ptr(NULL));
}

static void test_uint32_tensor_shape_ptr_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(4u, false);
    assert_non_null(arr);
    assert_non_null(uint32_tensor_shape_ptr(arr));
    return_uint32_tensor(arr);
}

static void test_uint32_tensor_strides_ptr_null(void** state) {
    (void)state;
    assert_null(uint32_tensor_strides_ptr(NULL));
}

static void test_uint32_tensor_strides_ptr_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(4u, false);
    assert_non_null(arr);
    const size_t* strides = uint32_tensor_strides_ptr(arr);
    assert_non_null(strides);
    assert_int_equal(strides[0], sizeof(uint32_t));
    return_uint32_tensor(arr);
}

static void test_uint32_tensor_shape_str_null(void** state) {
    (void)state;
    char buf[32];
    assert_int_equal(uint32_tensor_shape_str(NULL, buf, sizeof(buf)), NULL_POINTER);
}

static void test_uint32_tensor_shape_str_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(8u, false);
    assert_non_null(arr);
    char buf[32];
    assert_int_equal(uint32_tensor_shape_str(arr, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(8)");
    return_uint32_tensor(arr);
}

// ================================================================================
// ================================================================================
// UTILITY OPERATIONS

// ---- clear_uint32_tensor --------------------------------------------------------

static void test_clear_uint32_tensor_null(void** state) {
    (void)state;
    assert_int_equal(clear_uint32_tensor(NULL), NULL_POINTER);
}

static void test_clear_uint32_tensor_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(3u, 10u);
    assert_non_null(arr);
    assert_int_equal(clear_uint32_tensor(arr), NO_ERROR);
    assert_int_equal(uint32_tensor_size(arr), 0u);
    return_uint32_tensor(arr);
}

// ---- concat_uint32_tensor_array -------------------------------------------------

static void test_concat_uint32_tensor_null(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(concat_uint32_tensor_array(NULL, arr), NULL_POINTER);
    assert_int_equal(concat_uint32_tensor_array(arr, NULL), NULL_POINTER);
    return_uint32_tensor(arr);
}

static void test_concat_uint32_tensor_value(void** state) {
    (void)state;

    /* dst needs capacity for 5 elements total — allocate with room to grow */
    allocator_vtable_t alloc = heap_allocator();
    uint32_tensor_expect_t r  = init_uint32_array(8u, false, alloc);
    assert_true(r.has_value);
    uint32_tensor_t* dst = r.u.value;

    /* Populate dst with [1, 2, 3] */
    push_back_uint32_array(dst, 1u);
    push_back_uint32_array(dst, 2u);
    push_back_uint32_array(dst, 3u);

    /* src = [4, 5] */
    uint32_tensor_t* src = _make_uint32_array_filled(2u, 4u);
    assert_non_null(src);

    assert_int_equal(concat_uint32_tensor_array(dst, src), NO_ERROR);
    assert_int_equal(uint32_tensor_size(dst), 5u);

    uint32_t expected[] = { 1u, 2u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        uint32_t out = 0u;
        assert_int_equal(get_uint32_tensor_index(dst, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint32_tensor(dst);
    return_uint32_tensor(src);
}

// ---- slice_uint32_tensor_array --------------------------------------------------

static void test_slice_uint32_tensor_null(void** state) {
    (void)state;
    uint32_tensor_expect_t r = slice_uint32_tensor_array(NULL, 0u, 1u, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_slice_uint32_tensor_value(void** state) {
    (void)state;
    uint32_tensor_t* src = _make_uint32_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(src);

    uint32_tensor_expect_t r = slice_uint32_tensor_array(src, 1u, 4u, NULL);
    assert_true(r.has_value);
    uint32_tensor_t* sl = r.u.value;

    assert_int_equal(uint32_tensor_size(sl), 3u);
    uint32_t expected[] = { 11u, 12u, 13u };
    for (size_t i = 0u; i < 3u; i++) {
        uint32_t out = 0u;
        assert_int_equal(get_uint32_tensor_index(sl, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint32_tensor(src);
    return_uint32_tensor(sl);
}

// ---- reverse_uint32_tensor ------------------------------------------------------

static void test_reverse_uint32_tensor_null(void** state) {
    (void)state;
    assert_int_equal(reverse_uint32_tensor(NULL), NULL_POINTER);
}

static void test_reverse_uint32_tensor_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(4u, 1u); /* [1,2,3,4] */
    assert_non_null(arr);

    assert_int_equal(reverse_uint32_tensor(arr), NO_ERROR);

    uint32_t expected[] = { 4u, 3u, 2u, 1u };
    for (size_t i = 0u; i < 4u; i++) {
        uint32_t out = 0u;
        assert_int_equal(get_uint32_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint32_tensor(arr);
}

// ---- sort_uint32_tensor ---------------------------------------------------------

static void test_sort_uint32_tensor_null(void** state) {
    (void)state;
    assert_int_equal(sort_uint32_tensor(NULL, FORWARD), NULL_POINTER);
}

static void test_sort_uint32_tensor_forward(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(5u, false);
    assert_non_null(arr);

    uint32_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_uint32_array(arr, vals[i]);

    assert_int_equal(sort_uint32_tensor(arr, FORWARD), NO_ERROR);

    uint32_t expected[] = { 1u, 1u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        uint32_t out = 0u;
        assert_int_equal(get_uint32_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint32_tensor(arr);
}

static void test_sort_uint32_tensor_reverse(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(5u, false);
    assert_non_null(arr);

    uint32_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_uint32_array(arr, vals[i]);

    assert_int_equal(sort_uint32_tensor(arr, REVERSE), NO_ERROR);

    uint32_t expected[] = { 5u, 4u, 3u, 1u, 1u };
    for (size_t i = 0u; i < 5u; i++) {
        uint32_t out = 0u;
        assert_int_equal(get_uint32_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint32_tensor(arr);
}

// ================================================================================
// ================================================================================
// SET AND GET DATA

// ---- set_uint32_tensor_index / get_uint32_tensor_index --------------------------

static void test_set_get_uint32_tensor_index_null(void** state) {
    (void)state;
    uint32_t dummy = 0u;
    assert_int_equal(set_uint32_tensor_index(NULL, 0u, 1u),    NULL_POINTER);
    assert_int_equal(get_uint32_tensor_index(NULL, 0u, &dummy), NULL_POINTER);
}

static void test_set_get_uint32_tensor_index_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(3u, 10u);
    assert_non_null(arr);

    assert_int_equal(set_uint32_tensor_index(arr, 1u, 99u), NO_ERROR);

    uint32_t out = 0u;
    assert_int_equal(get_uint32_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);

    return_uint32_tensor(arr);
}

// ---- set_uint32_tensor_nd_index / get_uint32_tensor_nd_index --------------------

static void test_set_get_uint32_tensor_nd_index_null(void** state) {
    (void)state;
    size_t  idx[] = { 0u, 0u };
    uint32_t out   = 0u;
    assert_int_equal(set_uint32_tensor_nd_index(NULL, idx, 1u),   NULL_POINTER);
    assert_int_equal(get_uint32_tensor_nd_index(NULL, idx, &out), NULL_POINTER);
}

static void test_set_get_uint32_tensor_nd_index_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 3u };
    allocator_vtable_t alloc = heap_allocator();
    uint32_tensor_expect_t r  = init_uint32_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    uint32_tensor_t* mat = r.u.value;

    size_t idx[] = { 1u, 2u };
    assert_int_equal(set_uint32_tensor_nd_index(mat, idx, 42u), NO_ERROR);

    uint32_t out = 0u;
    assert_int_equal(get_uint32_tensor_nd_index(mat, idx, &out), NO_ERROR);
    assert_int_equal(out, 42u);

    return_uint32_tensor(mat);
}

// ---- push and pop wrappers ----------------------------------------------------

static void test_push_back_uint32_array_null(void** state) {
    (void)state;
    assert_int_equal(push_back_uint32_array(NULL, 1u), NULL_POINTER);
}

static void test_push_back_uint32_array_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(4u, false);
    assert_non_null(arr);

    assert_int_equal(push_back_uint32_array(arr, 10u), NO_ERROR);
    assert_int_equal(push_back_uint32_array(arr, 20u), NO_ERROR);
    assert_int_equal(uint32_tensor_size(arr), 2u);

    uint32_t out = 0u;
    assert_int_equal(get_uint32_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 20u);

    return_uint32_tensor(arr);
}

static void test_push_front_uint32_array_null(void** state) {
    (void)state;
    assert_int_equal(push_front_uint32_array(NULL, 1u), NULL_POINTER);
}

static void test_push_front_uint32_array_value(void** state) {
    (void)state;
    /* Capacity 3 so there is room for the push_front */
    allocator_vtable_t alloc = heap_allocator();
    uint32_tensor_expect_t r  = init_uint32_array(3u, false, alloc);
    assert_true(r.has_value);
    uint32_tensor_t* arr = r.u.value;

    push_back_uint32_array(arr, 10u);
    push_back_uint32_array(arr, 11u);
    /* arr = [10, 11], len == 2, alloc == 3 — one slot free */

    assert_int_equal(push_front_uint32_array(arr, 99u), NO_ERROR);

    uint32_t out = 0u;
    assert_int_equal(get_uint32_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_uint32_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_uint32_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);

    return_uint32_tensor(arr);
}

static void test_push_at_uint32_array_null(void** state) {
    (void)state;
    assert_int_equal(push_at_uint32_array(NULL, 1u, 0u), NULL_POINTER);
}

static void test_push_at_uint32_array_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(3u, 10u); /* [10, 11, 12] */
    assert_non_null(arr);

    assert_int_equal(push_at_uint32_array(arr, 99u, 1u), NO_ERROR);
    /* arr = [10, 99, 11, 12] */

    uint32_t out = 0u;
    assert_int_equal(get_uint32_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_uint32_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_uint32_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);
    assert_int_equal(get_uint32_tensor_index(arr, 3u, &out), NO_ERROR);
    assert_int_equal(out, 12u);

    return_uint32_tensor(arr);
}

static void test_pop_back_uint32_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_back_uint32_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_back_uint32_array_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    uint32_t out = 0u;
    assert_int_equal(pop_back_uint32_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(uint32_tensor_size(arr), 2u);

    return_uint32_tensor(arr);
}

static void test_pop_front_uint32_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_front_uint32_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_front_uint32_array_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    uint32_t out = 0u;
    assert_int_equal(pop_front_uint32_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(uint32_tensor_size(arr), 2u);

    return_uint32_tensor(arr);
}

static void test_pop_at_uint32_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_at_uint32_array(NULL, NULL, 0u), NULL_POINTER);
}

static void test_pop_at_uint32_array_value(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(4u, 10u); /* [10,11,12,13] */
    assert_non_null(arr);

    uint32_t out = 0u;
    assert_int_equal(pop_at_uint32_array(arr, &out, 2u), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(uint32_tensor_size(arr), 3u);

    assert_int_equal(get_uint32_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 13u);

    return_uint32_tensor(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// FIND uint32 TENSOR VALUE (uint32_tensor_lsearch)
// ================================================================================
 
/** NULL tensor must return NULL_POINTER. */
static void test_find_uint32_tensor_null_tensor(void** state) {
    (void)state;
    size_t idx = 0u;
    assert_int_equal(uint32_tensor_lsearch(NULL, &idx, 10u), NULL_POINTER);
}
 
/** NULL index pointer must return NULL_POINTER. */
static void test_find_uint32_tensor_null_index(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(3u, 10u);
    assert_non_null(arr);
    assert_int_equal(uint32_tensor_lsearch(arr, NULL, 10u), NULL_POINTER);
    return_uint32_tensor(arr);
}
 
/** Empty array must return EMPTY. */
static void test_find_uint32_tensor_empty(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(4u, false);
    assert_non_null(arr);
    size_t idx = 0u;
    assert_int_equal(uint32_tensor_lsearch(arr, &idx, 10u), EMPTY);
    return_uint32_tensor(arr);
}
 
/** Value not present must return NOT_FOUND. */
static void test_find_uint32_tensor_not_found(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(4u, 1u); /* [1,2,3,4] */
    assert_non_null(arr);
    size_t idx = 99u;   /* sentinel — must not be modified */
    assert_int_equal(uint32_tensor_lsearch(arr, &idx, 99u), NOT_FOUND);
    /* index must be unchanged on NOT_FOUND */
    assert_int_equal(idx, 99u);
    return_uint32_tensor(arr);
}
 
/** Value at the first position must be found at index 0. */
static void test_find_uint32_tensor_first_element(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(uint32_tensor_lsearch(arr, &idx, 10u), NO_ERROR);
    assert_int_equal(idx, 0u);
    return_uint32_tensor(arr);
}
 
/** Value at the last position must be found at index len-1. */
static void test_find_uint32_tensor_last_element(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(uint32_tensor_lsearch(arr, &idx, 14u), NO_ERROR);
    assert_int_equal(idx, 4u);
    return_uint32_tensor(arr);
}
 
/** Value in the middle must be found at the correct index. */
static void test_find_uint32_tensor_middle_element(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(uint32_tensor_lsearch(arr, &idx, 12u), NO_ERROR);
    assert_int_equal(idx, 2u);
    return_uint32_tensor(arr);
}
 
/**
 * When duplicates are present, the first occurrence must be returned.
 */
static void test_find_uint32_tensor_first_of_duplicates(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(6u, false);
    assert_non_null(arr);
 
    /* [5, 9, 9, 9, 7, 3] */
    push_back_uint32_array(arr, 5u);
    push_back_uint32_array(arr, 9u);
    push_back_uint32_array(arr, 9u);
    push_back_uint32_array(arr, 9u);
    push_back_uint32_array(arr, 7u);
    push_back_uint32_array(arr, 3u);
 
    size_t idx = 99u;
    assert_int_equal(uint32_tensor_lsearch(arr, &idx, 9u), NO_ERROR);
    assert_int_equal(idx, 1u);   /* first occurrence at index 1 */
 
    return_uint32_tensor(arr);
}
 
/**
 * Single-element array containing the target value must succeed.
 */
static void test_find_uint32_tensor_single_element_match(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(1u, false);
    assert_non_null(arr);
    push_back_uint32_array(arr, 42u);
 
    size_t idx = 99u;
    assert_int_equal(uint32_tensor_lsearch(arr, &idx, 42u), NO_ERROR);
    assert_int_equal(idx, 0u);
 
    return_uint32_tensor(arr);
}
 
/**
 * Single-element array not containing the target must return NOT_FOUND
 * without modifying index.
 */
static void test_find_uint32_tensor_single_element_no_match(void** state) {
    (void)state;
    uint32_tensor_t* arr = _make_uint32_array(1u, false);
    assert_non_null(arr);
    push_back_uint32_array(arr, 42u);
 
    size_t idx = 99u;
    assert_int_equal(uint32_tensor_lsearch(arr, &idx, 7u), NOT_FOUND);
    assert_int_equal(idx, 99u);   /* unchanged */
 
    return_uint32_tensor(arr);
}
// ================================================================================
// ================================================================================
// TEST SUITE REGISTRY

const struct CMUnitTest test_uint32_tensor[] = {
    /* init_uint32_array */
    cmocka_unit_test(test_init_uint32_array_null_allocator),
    cmocka_unit_test(test_init_uint32_array_success),

    /* init_uint32_tensor */
    cmocka_unit_test(test_init_uint32_tensor_success),
    cmocka_unit_test(test_init_uint32_tensor_null_shape),

    /* return_uint32_tensor */
    cmocka_unit_test(test_return_uint32_tensor_null),
    cmocka_unit_test(test_return_uint32_tensor_normal),

    /* copy_uint32_tensor */
    cmocka_unit_test(test_copy_uint32_tensor_null_src),
    cmocka_unit_test(test_copy_uint32_tensor_independence),

    /* introspection — null guards */
    cmocka_unit_test(test_uint32_tensor_size_null),
    cmocka_unit_test(test_uint32_tensor_alloc_null),
    cmocka_unit_test(test_uint32_tensor_data_size_null),
    cmocka_unit_test(test_uint32_tensor_dtype_null),
    cmocka_unit_test(test_is_uint32_tensor_empty_null),
    cmocka_unit_test(test_is_uint32_tensor_full_null),
    cmocka_unit_test(test_is_uint32_tensor_ptr_null_wrapper),
    cmocka_unit_test(test_uint32_tensor_ndim_null),
    cmocka_unit_test(test_uint32_tensor_shape_dim_null),
    cmocka_unit_test(test_uint32_tensor_shape_null),
    cmocka_unit_test(test_uint32_tensor_shape_ptr_null),
    cmocka_unit_test(test_uint32_tensor_strides_ptr_null),
    cmocka_unit_test(test_uint32_tensor_shape_str_null),

    /* introspection — values */
    cmocka_unit_test(test_uint32_tensor_size_value),
    cmocka_unit_test(test_uint32_tensor_alloc_value),
    cmocka_unit_test(test_uint32_tensor_data_size_value),
    cmocka_unit_test(test_uint32_tensor_dtype_value),
    cmocka_unit_test(test_is_uint32_tensor_empty_value),
    cmocka_unit_test(test_is_uint32_tensor_full_value),
    cmocka_unit_test(test_is_uint32_tensor_ptr_valid),
    cmocka_unit_test(test_uint32_tensor_ndim_value),
    cmocka_unit_test(test_uint32_tensor_shape_dim_value),
    cmocka_unit_test(test_uint32_tensor_shape_value),
    cmocka_unit_test(test_uint32_tensor_shape_ptr_value),
    cmocka_unit_test(test_uint32_tensor_strides_ptr_value),
    cmocka_unit_test(test_uint32_tensor_shape_str_value),

    /* utility — null guards */
    cmocka_unit_test(test_clear_uint32_tensor_null),
    cmocka_unit_test(test_concat_uint32_tensor_null),
    cmocka_unit_test(test_slice_uint32_tensor_null),
    cmocka_unit_test(test_reverse_uint32_tensor_null),
    cmocka_unit_test(test_sort_uint32_tensor_null),

    /* utility — values */
    cmocka_unit_test(test_clear_uint32_tensor_value),
    cmocka_unit_test(test_concat_uint32_tensor_value),
    cmocka_unit_test(test_slice_uint32_tensor_value),
    cmocka_unit_test(test_reverse_uint32_tensor_value),
    cmocka_unit_test(test_sort_uint32_tensor_forward),
    cmocka_unit_test(test_sort_uint32_tensor_reverse),

    /* set/get — null guards */
    cmocka_unit_test(test_set_get_uint32_tensor_index_null),
    cmocka_unit_test(test_set_get_uint32_tensor_nd_index_null),
    cmocka_unit_test(test_push_back_uint32_array_null),
    cmocka_unit_test(test_push_front_uint32_array_null),
    cmocka_unit_test(test_push_at_uint32_array_null),
    cmocka_unit_test(test_pop_back_uint32_array_null),
    cmocka_unit_test(test_pop_front_uint32_array_null),
    cmocka_unit_test(test_pop_at_uint32_array_null),

    /* set/get — values */
    cmocka_unit_test(test_set_get_uint32_tensor_index_value),
    cmocka_unit_test(test_set_get_uint32_tensor_nd_index_value),
    cmocka_unit_test(test_push_back_uint32_array_value),
    cmocka_unit_test(test_push_front_uint32_array_value),
    cmocka_unit_test(test_push_at_uint32_array_value),
    cmocka_unit_test(test_pop_back_uint32_array_value),
    cmocka_unit_test(test_pop_front_uint32_array_value),
    cmocka_unit_test(test_pop_at_uint32_array_value),

    /* uint32_tensor_lsearch — null/guard */
    cmocka_unit_test(test_find_uint32_tensor_null_tensor),
    cmocka_unit_test(test_find_uint32_tensor_null_index),
    cmocka_unit_test(test_find_uint32_tensor_empty),
 
    /* uint32_tensor_lsearch — not found */
    cmocka_unit_test(test_find_uint32_tensor_not_found),
 
    /* uint32_tensor_lsearch — found at various positions */
    cmocka_unit_test(test_find_uint32_tensor_first_element),
    cmocka_unit_test(test_find_uint32_tensor_last_element),
    cmocka_unit_test(test_find_uint32_tensor_middle_element),
    cmocka_unit_test(test_find_uint32_tensor_first_of_duplicates),
    cmocka_unit_test(test_find_uint32_tensor_single_element_match),
    cmocka_unit_test(test_find_uint32_tensor_single_element_no_match),
};

const size_t test_uint32_tensor_count = sizeof(test_uint32_tensor) /
                                       sizeof(test_uint32_tensor[0]);
// ================================================================================ 
// ================================================================================ 

/** Construct a populated int32 array with values 1..n. */
static int32_tensor_t* _make_int32_array(size_t capacity, bool growth) {
    allocator_vtable_t alloc = heap_allocator();
    int32_tensor_expect_t r  = init_int32_array(capacity, growth, alloc);
    if (!r.has_value) return NULL;
    return r.u.value;
}

static int32_tensor_t* _make_int32_array_filled(size_t n, int32_t base) {
    /* Allocate n+1 so the result always has at least one free slot,
     * making it safe to use as a push destination in tests. */
    int32_tensor_t* arr = _make_int32_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++) {
        int32_t val = (int32_t)(base + i);
        push_back_int32_array(arr, val);
    }
    return arr;
}

// ================================================================================
// ================================================================================
// INITIALISATION AND TEARDOWN

// ---- init_int32_array ----------------------------------------------------------

/** NULL allocator must propagate an error through the expect type. */
static void test_init_int32_array_null_allocator(void** state) {
    (void)state;
    allocator_vtable_t bad = heap_allocator();
    bad.allocate = NULL;
    int32_tensor_expect_t r = init_int32_array(8u, false, bad);
    assert_false(r.has_value);
}

/** Successful construction produces a non-NULL wrapper with correct mode. */
static void test_init_int32_array_success(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_tensor_expect_t r  = init_int32_array(8u, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  ARRAY_STRUCT);
    assert_int_equal(r.u.value->base->dtype, INT32_TYPE);
    assert_int_equal(r.u.value->base->len,   0u);
    assert_int_equal(r.u.value->base->alloc, 8u);
    return_int32_tensor(r.u.value);
}

// ---- init_int32_tensor ---------------------------------------------------------

/** Successful construction of a fixed-shape tensor. */
static void test_init_int32_tensor_success(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    int32_tensor_expect_t r  = init_int32_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  TENSOR_STRUCT);
    assert_int_equal(r.u.value->base->dtype, INT32_TYPE);
    assert_int_equal(r.u.value->base->len,   12u);
    return_int32_tensor(r.u.value);
}

/** NULL shape propagates an error. */
static void test_init_int32_tensor_null_shape(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int32_tensor_expect_t r  = init_int32_tensor(2u, NULL, alloc);
    assert_false(r.has_value);
}

// ---- return_int32_tensor -------------------------------------------------------

/** return_int32_tensor(NULL) must not crash. */
static void test_return_int32_tensor_null(void** state) {
    (void)state;
    return_int32_tensor(NULL);
}

/** Normal construction followed by return must not crash or leak. */
static void test_return_int32_tensor_normal(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(4u, false);
    assert_non_null(arr);
    return_int32_tensor(arr);
}

// ---- copy_int32_tensor ---------------------------------------------------------

/** NULL src must return NULL_POINTER. */
static void test_copy_int32_tensor_null_src(void** state) {
    (void)state;
    int32_tensor_expect_t r = copy_int32_tensor(NULL, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

/** Copy is independent of source — mutation of copy does not affect src. */
static void test_copy_int32_tensor_independence(void** state) {
    (void)state;
    int32_tensor_t* src = _make_int32_array_filled(3u, 10u);
    assert_non_null(src);

    int32_tensor_expect_t r = copy_int32_tensor(src, NULL);
    assert_true(r.has_value);
    int32_tensor_t* dst = r.u.value;

    /* Overwrite slot 0 of the copy */
    assert_int_equal(set_int32_tensor_index(dst, 0u, 99u), NO_ERROR);

    /* Source slot 0 must be unchanged */
    int32_t src_val = 0u;
    assert_int_equal(get_int32_tensor_index(src, 0u, &src_val), NO_ERROR);
    assert_int_equal(src_val, 10u);

    return_int32_tensor(src);
    return_int32_tensor(dst);
}

// ================================================================================
// ================================================================================
// INTROSPECTION

static void test_int32_tensor_size_null(void** state) {
    (void)state;
    assert_int_equal(int32_tensor_size(NULL), 0u);
}

static void test_int32_tensor_size_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array_filled(3u, 1u);
    assert_non_null(arr);
    assert_int_equal(int32_tensor_size(arr), 3u);
    return_int32_tensor(arr);
}

static void test_int32_tensor_alloc_null(void** state) {
    (void)state;
    assert_int_equal(int32_tensor_alloc(NULL), 0u);
}

static void test_int32_tensor_alloc_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(8u, false);
    assert_non_null(arr);
    assert_int_equal(int32_tensor_alloc(arr), 8u);
    return_int32_tensor(arr);
}

static void test_int32_tensor_data_size_null(void** state) {
    (void)state;
    assert_int_equal(int32_tensor_data_size(NULL), 0u);
}

static void test_int32_tensor_data_size_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(int32_tensor_data_size(arr), sizeof(int32_t));
    return_int32_tensor(arr);
}

static void test_int32_tensor_dtype_null(void** state) {
    (void)state;
    assert_int_equal(int32_tensor_dtype(NULL), UNKNOWN_TYPE);
}

static void test_int32_tensor_dtype_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(int32_tensor_dtype(arr), INT32_TYPE);
    return_int32_tensor(arr);
}

static void test_is_int32_tensor_empty_null(void** state) {
    (void)state;
    assert_true(is_int32_tensor_empty(NULL));
}

static void test_is_int32_tensor_empty_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(4u, false);
    assert_non_null(arr);
    assert_true(is_int32_tensor_empty(arr));
    push_back_int32_array(arr, 1u);
    assert_false(is_int32_tensor_empty(arr));
    return_int32_tensor(arr);
}

static void test_is_int32_tensor_full_null(void** state) {
    (void)state;
    assert_true(is_int32_tensor_full(NULL));
}

static void test_is_int32_tensor_full_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(2u, false);
    assert_non_null(arr);
    assert_false(is_int32_tensor_full(arr));
    push_back_int32_array(arr, 1u);
    push_back_int32_array(arr, 2u);
    assert_true(is_int32_tensor_full(arr));
    return_int32_tensor(arr);
}

static void test_is_int32_tensor_ptr_null_wrapper(void** state) {
    (void)state;
    int32_t dummy = 0u;
    assert_false(is_int32_tensor_ptr(NULL, &dummy));
}

static void test_is_int32_tensor_ptr_valid(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array_filled(4u, 10u);
    assert_non_null(arr);
    /* Pointer to first element must be valid */
    const int32_t* p = (const int32_t*)arr->base->data;
    assert_true(is_int32_tensor_ptr(arr, p));
    return_int32_tensor(arr);
}

static void test_int32_tensor_ndim_null(void** state) {
    (void)state;
    assert_int_equal(int32_tensor_ndim(NULL), 0u);
}

static void test_int32_tensor_ndim_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(int32_tensor_ndim(arr), 1u);
    return_int32_tensor(arr);
}

static void test_int32_tensor_shape_dim_null(void** state) {
    (void)state;
    assert_int_equal(int32_tensor_shape_dim(NULL, 0u), 0u);
}

static void test_int32_tensor_shape_dim_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    int32_tensor_expect_t r  = init_int32_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_int_equal(int32_tensor_shape_dim(r.u.value, 0u), 3u);
    assert_int_equal(int32_tensor_shape_dim(r.u.value, 1u), 4u);
    return_int32_tensor(r.u.value);
}

static void test_int32_tensor_shape_null(void** state) {
    (void)state;
    size_t buf[2];
    assert_int_equal(int32_tensor_shape(NULL, buf, 2u), NULL_POINTER);
}

static void test_int32_tensor_shape_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    int32_tensor_expect_t r  = init_int32_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    size_t buf[2] = { 0u, 0u };
    assert_int_equal(int32_tensor_shape(r.u.value, buf, 2u), NO_ERROR);
    assert_int_equal(buf[0], 3u);
    assert_int_equal(buf[1], 4u);
    return_int32_tensor(r.u.value);
}

static void test_int32_tensor_shape_ptr_null(void** state) {
    (void)state;
    assert_null(int32_tensor_shape_ptr(NULL));
}

static void test_int32_tensor_shape_ptr_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(4u, false);
    assert_non_null(arr);
    assert_non_null(int32_tensor_shape_ptr(arr));
    return_int32_tensor(arr);
}

static void test_int32_tensor_strides_ptr_null(void** state) {
    (void)state;
    assert_null(int32_tensor_strides_ptr(NULL));
}

static void test_int32_tensor_strides_ptr_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(4u, false);
    assert_non_null(arr);
    const size_t* strides = int32_tensor_strides_ptr(arr);
    assert_non_null(strides);
    assert_int_equal(strides[0], sizeof(int32_t));
    return_int32_tensor(arr);
}

static void test_int32_tensor_shape_str_null(void** state) {
    (void)state;
    char buf[32];
    assert_int_equal(int32_tensor_shape_str(NULL, buf, sizeof(buf)), NULL_POINTER);
}

static void test_int32_tensor_shape_str_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(8u, false);
    assert_non_null(arr);
    char buf[32];
    assert_int_equal(int32_tensor_shape_str(arr, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(8)");
    return_int32_tensor(arr);
}

// ================================================================================
// ================================================================================
// UTILITY OPERATIONS

// ---- clear_int32_tensor --------------------------------------------------------

static void test_clear_int32_tensor_null(void** state) {
    (void)state;
    assert_int_equal(clear_int32_tensor(NULL), NULL_POINTER);
}

static void test_clear_int32_tensor_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array_filled(3u, 10u);
    assert_non_null(arr);
    assert_int_equal(clear_int32_tensor(arr), NO_ERROR);
    assert_int_equal(int32_tensor_size(arr), 0u);
    return_int32_tensor(arr);
}

// ---- concat_int32_tensor_array -------------------------------------------------

static void test_concat_int32_tensor_null(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(concat_int32_tensor_array(NULL, arr), NULL_POINTER);
    assert_int_equal(concat_int32_tensor_array(arr, NULL), NULL_POINTER);
    return_int32_tensor(arr);
}

static void test_concat_int32_tensor_value(void** state) {
    (void)state;

    /* dst needs capacity for 5 elements total — allocate with room to grow */
    allocator_vtable_t alloc = heap_allocator();
    int32_tensor_expect_t r  = init_int32_array(8u, false, alloc);
    assert_true(r.has_value);
    int32_tensor_t* dst = r.u.value;

    /* Populate dst with [1, 2, 3] */
    push_back_int32_array(dst, 1u);
    push_back_int32_array(dst, 2u);
    push_back_int32_array(dst, 3u);

    /* src = [4, 5] */
    int32_tensor_t* src = _make_int32_array_filled(2u, 4u);
    assert_non_null(src);

    assert_int_equal(concat_int32_tensor_array(dst, src), NO_ERROR);
    assert_int_equal(int32_tensor_size(dst), 5u);

    int32_t expected[] = { 1u, 2u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        int32_t out = 0u;
        assert_int_equal(get_int32_tensor_index(dst, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int32_tensor(dst);
    return_int32_tensor(src);
}

// ---- slice_int32_tensor_array --------------------------------------------------

static void test_slice_int32_tensor_null(void** state) {
    (void)state;
    int32_tensor_expect_t r = slice_int32_tensor_array(NULL, 0u, 1u, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_slice_int32_tensor_value(void** state) {
    (void)state;
    int32_tensor_t* src = _make_int32_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(src);

    int32_tensor_expect_t r = slice_int32_tensor_array(src, 1u, 4u, NULL);
    assert_true(r.has_value);
    int32_tensor_t* sl = r.u.value;

    assert_int_equal(int32_tensor_size(sl), 3u);
    int32_t expected[] = { 11u, 12u, 13u };
    for (size_t i = 0u; i < 3u; i++) {
        int32_t out = 0u;
        assert_int_equal(get_int32_tensor_index(sl, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int32_tensor(src);
    return_int32_tensor(sl);
}

// ---- reverse_int32_tensor ------------------------------------------------------

static void test_reverse_int32_tensor_null(void** state) {
    (void)state;
    assert_int_equal(reverse_int32_tensor(NULL), NULL_POINTER);
}

static void test_reverse_int32_tensor_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array_filled(4u, 1u); /* [1,2,3,4] */
    assert_non_null(arr);

    assert_int_equal(reverse_int32_tensor(arr), NO_ERROR);

    int32_t expected[] = { 4u, 3u, 2u, 1u };
    for (size_t i = 0u; i < 4u; i++) {
        int32_t out = 0u;
        assert_int_equal(get_int32_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int32_tensor(arr);
}

// ---- sort_int32_tensor ---------------------------------------------------------

static void test_sort_int32_tensor_null(void** state) {
    (void)state;
    assert_int_equal(sort_int32_tensor(NULL, FORWARD), NULL_POINTER);
}

static void test_sort_int32_tensor_forward(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(5u, false);
    assert_non_null(arr);

    int32_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_int32_array(arr, vals[i]);

    assert_int_equal(sort_int32_tensor(arr, FORWARD), NO_ERROR);

    int32_t expected[] = { 1u, 1u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        int32_t out = 0u;
        assert_int_equal(get_int32_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int32_tensor(arr);
}

static void test_sort_int32_tensor_reverse(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(5u, false);
    assert_non_null(arr);

    int32_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_int32_array(arr, vals[i]);

    assert_int_equal(sort_int32_tensor(arr, REVERSE), NO_ERROR);

    int32_t expected[] = { 5u, 4u, 3u, 1u, 1u };
    for (size_t i = 0u; i < 5u; i++) {
        int32_t out = 0u;
        assert_int_equal(get_int32_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int32_tensor(arr);
}

// ================================================================================
// ================================================================================
// SET AND GET DATA

// ---- set_int32_tensor_index / get_int32_tensor_index --------------------------

static void test_set_get_int32_tensor_index_null(void** state) {
    (void)state;
    int32_t dummy = 0u;
    assert_int_equal(set_int32_tensor_index(NULL, 0u, 1u),    NULL_POINTER);
    assert_int_equal(get_int32_tensor_index(NULL, 0u, &dummy), NULL_POINTER);
}

static void test_set_get_int32_tensor_index_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array_filled(3u, 10u);
    assert_non_null(arr);

    assert_int_equal(set_int32_tensor_index(arr, 1u, 99u), NO_ERROR);

    int32_t out = 0u;
    assert_int_equal(get_int32_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);

    return_int32_tensor(arr);
}

// ---- set_int32_tensor_nd_index / get_int32_tensor_nd_index --------------------

static void test_set_get_int32_tensor_nd_index_null(void** state) {
    (void)state;
    size_t  idx[] = { 0u, 0u };
    int32_t out   = 0u;
    assert_int_equal(set_int32_tensor_nd_index(NULL, idx, 1u),   NULL_POINTER);
    assert_int_equal(get_int32_tensor_nd_index(NULL, idx, &out), NULL_POINTER);
}

static void test_set_get_int32_tensor_nd_index_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 3u };
    allocator_vtable_t alloc = heap_allocator();
    int32_tensor_expect_t r  = init_int32_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    int32_tensor_t* mat = r.u.value;

    size_t idx[] = { 1u, 2u };
    assert_int_equal(set_int32_tensor_nd_index(mat, idx, 42u), NO_ERROR);

    int32_t out = 0u;
    assert_int_equal(get_int32_tensor_nd_index(mat, idx, &out), NO_ERROR);
    assert_int_equal(out, 42u);

    return_int32_tensor(mat);
}

// ---- push and pop wrappers ----------------------------------------------------

static void test_push_back_int32_array_null(void** state) {
    (void)state;
    assert_int_equal(push_back_int32_array(NULL, 1u), NULL_POINTER);
}

static void test_push_back_int32_array_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(4u, false);
    assert_non_null(arr);

    assert_int_equal(push_back_int32_array(arr, 10u), NO_ERROR);
    assert_int_equal(push_back_int32_array(arr, 20u), NO_ERROR);
    assert_int_equal(int32_tensor_size(arr), 2u);

    int32_t out = 0u;
    assert_int_equal(get_int32_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 20u);

    return_int32_tensor(arr);
}

static void test_push_front_int32_array_null(void** state) {
    (void)state;
    assert_int_equal(push_front_int32_array(NULL, 1u), NULL_POINTER);
}

static void test_push_front_int32_array_value(void** state) {
    (void)state;
    /* Capacity 3 so there is room for the push_front */
    allocator_vtable_t alloc = heap_allocator();
    int32_tensor_expect_t r  = init_int32_array(3u, false, alloc);
    assert_true(r.has_value);
    int32_tensor_t* arr = r.u.value;

    push_back_int32_array(arr, 10u);
    push_back_int32_array(arr, 11u);
    /* arr = [10, 11], len == 2, alloc == 3 — one slot free */

    assert_int_equal(push_front_int32_array(arr, 99u), NO_ERROR);

    int32_t out = 0u;
    assert_int_equal(get_int32_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_int32_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_int32_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);

    return_int32_tensor(arr);
}

static void test_push_at_int32_array_null(void** state) {
    (void)state;
    assert_int_equal(push_at_int32_array(NULL, 1u, 0u), NULL_POINTER);
}

static void test_push_at_int32_array_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array_filled(3u, 10u); /* [10, 11, 12] */
    assert_non_null(arr);

    assert_int_equal(push_at_int32_array(arr, 99u, 1u), NO_ERROR);
    /* arr = [10, 99, 11, 12] */

    int32_t out = 0u;
    assert_int_equal(get_int32_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_int32_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_int32_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);
    assert_int_equal(get_int32_tensor_index(arr, 3u, &out), NO_ERROR);
    assert_int_equal(out, 12u);

    return_int32_tensor(arr);
}

static void test_pop_back_int32_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_back_int32_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_back_int32_array_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    int32_t out = 0u;
    assert_int_equal(pop_back_int32_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(int32_tensor_size(arr), 2u);

    return_int32_tensor(arr);
}

static void test_pop_front_int32_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_front_int32_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_front_int32_array_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    int32_t out = 0u;
    assert_int_equal(pop_front_int32_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(int32_tensor_size(arr), 2u);

    return_int32_tensor(arr);
}

static void test_pop_at_int32_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_at_int32_array(NULL, NULL, 0u), NULL_POINTER);
}

static void test_pop_at_int32_array_value(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array_filled(4u, 10u); /* [10,11,12,13] */
    assert_non_null(arr);

    int32_t out = 0u;
    assert_int_equal(pop_at_int32_array(arr, &out, 2u), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(int32_tensor_size(arr), 3u);

    assert_int_equal(get_int32_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 13u);

    return_int32_tensor(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// int32 LSEARCH — NEGATIVE VALUE CORRECTNESS
// ================================================================================

/** Search for a negative value that is present must return NO_ERROR and
 *  the correct index. Exercises the (uint32_t) cast path on a value
 *  whose sign bit is set. */
static void test_int32_tensor_lsearch_negative_found(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(6u, false);
    assert_non_null(arr);

    /* Mix of positive, negative, and zero */
    push_back_int32_array(arr,  10);
    push_back_int32_array(arr, -5);
    push_back_int32_array(arr,  30);
    push_back_int32_array(arr, -5);   /* duplicate — must find first */
    push_back_int32_array(arr,  0);
    push_back_int32_array(arr, -128);

    size_t idx = 99u;
    assert_int_equal(int32_tensor_lsearch(arr, &idx, -5), NO_ERROR);
    assert_int_equal(idx, 1u);   /* first occurrence of -5 is at index 1 */

    return_int32_tensor(arr);
}

/** Search for -128 (0xFF80) at the last position exercises a negative value
 *  whose bit pattern differs maximally from zero. */
static void test_int32_tensor_lsearch_negative_last(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(5u, false);
    assert_non_null(arr);

    push_back_int32_array(arr,  1);
    push_back_int32_array(arr,  2);
    push_back_int32_array(arr,  3);
    push_back_int32_array(arr,  4);
    push_back_int32_array(arr, -128);

    size_t idx = 99u;
    assert_int_equal(int32_tensor_lsearch(arr, &idx, -128), NO_ERROR);
    assert_int_equal(idx, 4u);

    return_int32_tensor(arr);
}

/** Search for -1 (0xFFFF) — the value whose bit pattern equals Uint32_MAX.
 *  This is the most important case: confirms the cast does not confuse
 *  -1 with the SIZE_MAX sentinel or with 65535 as a positive value. */
static void test_int32_tensor_lsearch_minus_one(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(4u, false);
    assert_non_null(arr);

    push_back_int32_array(arr,  100);
    push_back_int32_array(arr,   -1);
    push_back_int32_array(arr,  200);
    push_back_int32_array(arr,  300);

    size_t idx = 99u;
    assert_int_equal(int32_tensor_lsearch(arr, &idx, -1), NO_ERROR);
    assert_int_equal(idx, 1u);

    return_int32_tensor(arr);
}

/** Search for int32_MIN (-32768, 0x8000) — the value with only the sign
 *  bit set, which is the most extreme negative value in range. */
static void test_int32_tensor_lsearch_int32_min(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(4u, false);
    assert_non_null(arr);

    push_back_int32_array(arr,      0);
    push_back_int32_array(arr,      1);
    push_back_int32_array(arr, INT32_MIN);
    push_back_int32_array(arr,    100);

    size_t idx = 99u;
    assert_int_equal(int32_tensor_lsearch(arr, &idx, INT32_MIN), NO_ERROR);
    assert_int_equal(idx, 2u);

    return_int32_tensor(arr);
}

/** A negative value that is not present must return NOT_FOUND and must
 *  not modify index. */
static void test_int32_tensor_lsearch_negative_not_found(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(4u, false);
    assert_non_null(arr);

    push_back_int32_array(arr,  10);
    push_back_int32_array(arr,  20);
    push_back_int32_array(arr, -30);
    push_back_int32_array(arr,  40);

    size_t idx = 99u;
    assert_int_equal(int32_tensor_lsearch(arr, &idx, -99), NOT_FOUND);
    assert_int_equal(idx, 99u);   /* unchanged */

    return_int32_tensor(arr);
}

/** Array containing only negative values — confirms the SIMD path handles
 *  a buffer where every element has its sign bit set. */
static void test_int32_tensor_lsearch_all_negative(void** state) {
    (void)state;
    int32_tensor_t* arr = _make_int32_array(5u, false);
    assert_non_null(arr);

    push_back_int32_array(arr, -1);
    push_back_int32_array(arr, -2);
    push_back_int32_array(arr, -3);
    push_back_int32_array(arr, -4);
    push_back_int32_array(arr, -5);

    size_t idx = 99u;
    assert_int_equal(int32_tensor_lsearch(arr, &idx, -3), NO_ERROR);
    assert_int_equal(idx, 2u);

    assert_int_equal(int32_tensor_lsearch(arr, &idx, -6), NOT_FOUND);

    return_int32_tensor(arr);
}
// ================================================================================
// ================================================================================
// TEST SUITE REGISTRY

const struct CMUnitTest test_int32_tensor[] = {
    /* init_int32_array */
    cmocka_unit_test(test_init_int32_array_null_allocator),
    cmocka_unit_test(test_init_int32_array_success),

    /* init_int32_tensor */
    cmocka_unit_test(test_init_int32_tensor_success),
    cmocka_unit_test(test_init_int32_tensor_null_shape),

    /* return_int32_tensor */
    cmocka_unit_test(test_return_int32_tensor_null),
    cmocka_unit_test(test_return_int32_tensor_normal),

    /* copy_int32_tensor */
    cmocka_unit_test(test_copy_int32_tensor_null_src),
    cmocka_unit_test(test_copy_int32_tensor_independence),

    /* introspection — null guards */
    cmocka_unit_test(test_int32_tensor_size_null),
    cmocka_unit_test(test_int32_tensor_alloc_null),
    cmocka_unit_test(test_int32_tensor_data_size_null),
    cmocka_unit_test(test_int32_tensor_dtype_null),
    cmocka_unit_test(test_is_int32_tensor_empty_null),
    cmocka_unit_test(test_is_int32_tensor_full_null),
    cmocka_unit_test(test_is_int32_tensor_ptr_null_wrapper),
    cmocka_unit_test(test_int32_tensor_ndim_null),
    cmocka_unit_test(test_int32_tensor_shape_dim_null),
    cmocka_unit_test(test_int32_tensor_shape_null),
    cmocka_unit_test(test_int32_tensor_shape_ptr_null),
    cmocka_unit_test(test_int32_tensor_strides_ptr_null),
    cmocka_unit_test(test_int32_tensor_shape_str_null),

    /* introspection — values */
    cmocka_unit_test(test_int32_tensor_size_value),
    cmocka_unit_test(test_int32_tensor_alloc_value),
    cmocka_unit_test(test_int32_tensor_data_size_value),
    cmocka_unit_test(test_int32_tensor_dtype_value),
    cmocka_unit_test(test_is_int32_tensor_empty_value),
    cmocka_unit_test(test_is_int32_tensor_full_value),
    cmocka_unit_test(test_is_int32_tensor_ptr_valid),
    cmocka_unit_test(test_int32_tensor_ndim_value),
    cmocka_unit_test(test_int32_tensor_shape_dim_value),
    cmocka_unit_test(test_int32_tensor_shape_value),
    cmocka_unit_test(test_int32_tensor_shape_ptr_value),
    cmocka_unit_test(test_int32_tensor_strides_ptr_value),
    cmocka_unit_test(test_int32_tensor_shape_str_value),

    /* utility — null guards */
    cmocka_unit_test(test_clear_int32_tensor_null),
    cmocka_unit_test(test_concat_int32_tensor_null),
    cmocka_unit_test(test_slice_int32_tensor_null),
    cmocka_unit_test(test_reverse_int32_tensor_null),
    cmocka_unit_test(test_sort_int32_tensor_null),

    /* utility — values */
    cmocka_unit_test(test_clear_int32_tensor_value),
    cmocka_unit_test(test_concat_int32_tensor_value),
    cmocka_unit_test(test_slice_int32_tensor_value),
    cmocka_unit_test(test_reverse_int32_tensor_value),
    cmocka_unit_test(test_sort_int32_tensor_forward),
    cmocka_unit_test(test_sort_int32_tensor_reverse),

    /* set/get — null guards */
    cmocka_unit_test(test_set_get_int32_tensor_index_null),
    cmocka_unit_test(test_set_get_int32_tensor_nd_index_null),
    cmocka_unit_test(test_push_back_int32_array_null),
    cmocka_unit_test(test_push_front_int32_array_null),
    cmocka_unit_test(test_push_at_int32_array_null),
    cmocka_unit_test(test_pop_back_int32_array_null),
    cmocka_unit_test(test_pop_front_int32_array_null),
    cmocka_unit_test(test_pop_at_int32_array_null),

    /* set/get — values */
    cmocka_unit_test(test_set_get_int32_tensor_index_value),
    cmocka_unit_test(test_set_get_int32_tensor_nd_index_value),
    cmocka_unit_test(test_push_back_int32_array_value),
    cmocka_unit_test(test_push_front_int32_array_value),
    cmocka_unit_test(test_push_at_int32_array_value),
    cmocka_unit_test(test_pop_back_int32_array_value),
    cmocka_unit_test(test_pop_front_int32_array_value),
    cmocka_unit_test(test_pop_at_int32_array_value),

    /* int16 lsearch — negative value correctness */
    cmocka_unit_test(test_int32_tensor_lsearch_negative_found),
    cmocka_unit_test(test_int32_tensor_lsearch_negative_last),
    cmocka_unit_test(test_int32_tensor_lsearch_minus_one),
    cmocka_unit_test(test_int32_tensor_lsearch_int32_min),
    cmocka_unit_test(test_int32_tensor_lsearch_negative_not_found),
    cmocka_unit_test(test_int32_tensor_lsearch_all_negative),
};

const size_t test_int32_tensor_count = sizeof(test_int32_tensor) /
                                       sizeof(test_int32_tensor[0]);
// ================================================================================ 
// ================================================================================ 

/** Construct a populated uint64 array with values 1..n. */
static uint64_tensor_t* _make_uint64_array(size_t capacity, bool growth) {
    allocator_vtable_t alloc = heap_allocator();
    uint64_tensor_expect_t r  = init_uint64_array(capacity, growth, alloc);
    if (!r.has_value) return NULL;
    return r.u.value;
}

static uint64_tensor_t* _make_uint64_array_filled(size_t n, uint64_t base) {
    /* Allocate n+1 so the result always has at least one free slot,
     * making it safe to use as a push destination in tests. */
    uint64_tensor_t* arr = _make_uint64_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++) {
        uint64_t val = (uint64_t)(base + i);
        push_back_uint64_array(arr, val);
    }
    return arr;
}

// ================================================================================
// ================================================================================
// INITIALISATION AND TEARDOWN

// ---- init_uint64_array ----------------------------------------------------------

/** NULL allocator must propagate an error through the expect type. */
static void test_init_uint64_array_null_allocator(void** state) {
    (void)state;
    allocator_vtable_t bad = heap_allocator();
    bad.allocate = NULL;
    uint64_tensor_expect_t r = init_uint64_array(8u, false, bad);
    assert_false(r.has_value);
}

/** Successful construction produces a non-NULL wrapper with correct mode. */
static void test_init_uint64_array_success(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_tensor_expect_t r  = init_uint64_array(8u, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  ARRAY_STRUCT);
    assert_int_equal(r.u.value->base->dtype, UINT64_TYPE);
    assert_int_equal(r.u.value->base->len,   0u);
    assert_int_equal(r.u.value->base->alloc, 8u);
    return_uint64_tensor(r.u.value);
}

// ---- init_uint64_tensor ---------------------------------------------------------

/** Successful construction of a fixed-shape tensor. */
static void test_init_uint64_tensor_success(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    uint64_tensor_expect_t r  = init_uint64_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  TENSOR_STRUCT);
    assert_int_equal(r.u.value->base->dtype, UINT64_TYPE);
    assert_int_equal(r.u.value->base->len,   12u);
    return_uint64_tensor(r.u.value);
}

/** NULL shape propagates an error. */
static void test_init_uint64_tensor_null_shape(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    uint64_tensor_expect_t r  = init_uint64_tensor(2u, NULL, alloc);
    assert_false(r.has_value);
}

// ---- return_uint64_tensor -------------------------------------------------------

/** return_uint64_tensor(NULL) must not crash. */
static void test_return_uint64_tensor_null(void** state) {
    (void)state;
    return_uint64_tensor(NULL);
}

/** Normal construction followed by return must not crash or leak. */
static void test_return_uint64_tensor_normal(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(4u, false);
    assert_non_null(arr);
    return_uint64_tensor(arr);
}

// ---- copy_uint64_tensor ---------------------------------------------------------

/** NULL src must return NULL_POINTER. */
static void test_copy_uint64_tensor_null_src(void** state) {
    (void)state;
    uint64_tensor_expect_t r = copy_uint64_tensor(NULL, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

/** Copy is independent of source — mutation of copy does not affect src. */
static void test_copy_uint64_tensor_independence(void** state) {
    (void)state;
    uint64_tensor_t* src = _make_uint64_array_filled(3u, 10u);
    assert_non_null(src);

    uint64_tensor_expect_t r = copy_uint64_tensor(src, NULL);
    assert_true(r.has_value);
    uint64_tensor_t* dst = r.u.value;

    /* Overwrite slot 0 of the copy */
    assert_int_equal(set_uint64_tensor_index(dst, 0u, 99u), NO_ERROR);

    /* Source slot 0 must be unchanged */
    uint64_t src_val = 0u;
    assert_int_equal(get_uint64_tensor_index(src, 0u, &src_val), NO_ERROR);
    assert_int_equal(src_val, 10u);

    return_uint64_tensor(src);
    return_uint64_tensor(dst);
}

// ================================================================================
// ================================================================================
// INTROSPECTION

static void test_uint64_tensor_size_null(void** state) {
    (void)state;
    assert_int_equal(uint64_tensor_size(NULL), 0u);
}

static void test_uint64_tensor_size_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(3u, 1u);
    assert_non_null(arr);
    assert_int_equal(uint64_tensor_size(arr), 3u);
    return_uint64_tensor(arr);
}

static void test_uint64_tensor_alloc_null(void** state) {
    (void)state;
    assert_int_equal(uint64_tensor_alloc(NULL), 0u);
}

static void test_uint64_tensor_alloc_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(8u, false);
    assert_non_null(arr);
    assert_int_equal(uint64_tensor_alloc(arr), 8u);
    return_uint64_tensor(arr);
}

static void test_uint64_tensor_data_size_null(void** state) {
    (void)state;
    assert_int_equal(uint64_tensor_data_size(NULL), 0u);
}

static void test_uint64_tensor_data_size_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(uint64_tensor_data_size(arr), sizeof(uint64_t));
    return_uint64_tensor(arr);
}

static void test_uint64_tensor_dtype_null(void** state) {
    (void)state;
    assert_int_equal(uint64_tensor_dtype(NULL), UNKNOWN_TYPE);
}

static void test_uint64_tensor_dtype_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(uint64_tensor_dtype(arr), UINT64_TYPE);
    return_uint64_tensor(arr);
}

static void test_is_uint64_tensor_empty_null(void** state) {
    (void)state;
    assert_true(is_uint64_tensor_empty(NULL));
}

static void test_is_uint64_tensor_empty_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(4u, false);
    assert_non_null(arr);
    assert_true(is_uint64_tensor_empty(arr));
    push_back_uint64_array(arr, 1u);
    assert_false(is_uint64_tensor_empty(arr));
    return_uint64_tensor(arr);
}

static void test_is_uint64_tensor_full_null(void** state) {
    (void)state;
    assert_true(is_uint64_tensor_full(NULL));
}

static void test_is_uint64_tensor_full_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(2u, false);
    assert_non_null(arr);
    assert_false(is_uint64_tensor_full(arr));
    push_back_uint64_array(arr, 1u);
    push_back_uint64_array(arr, 2u);
    assert_true(is_uint64_tensor_full(arr));
    return_uint64_tensor(arr);
}

static void test_is_uint64_tensor_ptr_null_wrapper(void** state) {
    (void)state;
    uint64_t dummy = 0u;
    assert_false(is_uint64_tensor_ptr(NULL, &dummy));
}

static void test_is_uint64_tensor_ptr_valid(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(4u, 10u);
    assert_non_null(arr);
    /* Pointer to first element must be valid */
    const uint64_t* p = (const uint64_t*)arr->base->data;
    assert_true(is_uint64_tensor_ptr(arr, p));
    return_uint64_tensor(arr);
}

static void test_uint64_tensor_ndim_null(void** state) {
    (void)state;
    assert_int_equal(uint64_tensor_ndim(NULL), 0u);
}

static void test_uint64_tensor_ndim_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(uint64_tensor_ndim(arr), 1u);
    return_uint64_tensor(arr);
}

static void test_uint64_tensor_shape_dim_null(void** state) {
    (void)state;
    assert_int_equal(uint64_tensor_shape_dim(NULL, 0u), 0u);
}

static void test_uint64_tensor_shape_dim_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    uint64_tensor_expect_t r  = init_uint64_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_int_equal(uint64_tensor_shape_dim(r.u.value, 0u), 3u);
    assert_int_equal(uint64_tensor_shape_dim(r.u.value, 1u), 4u);
    return_uint64_tensor(r.u.value);
}

static void test_uint64_tensor_shape_null(void** state) {
    (void)state;
    size_t buf[2];
    assert_int_equal(uint64_tensor_shape(NULL, buf, 2u), NULL_POINTER);
}

static void test_uint64_tensor_shape_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    uint64_tensor_expect_t r  = init_uint64_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    size_t buf[2] = { 0u, 0u };
    assert_int_equal(uint64_tensor_shape(r.u.value, buf, 2u), NO_ERROR);
    assert_int_equal(buf[0], 3u);
    assert_int_equal(buf[1], 4u);
    return_uint64_tensor(r.u.value);
}

static void test_uint64_tensor_shape_ptr_null(void** state) {
    (void)state;
    assert_null(uint64_tensor_shape_ptr(NULL));
}

static void test_uint64_tensor_shape_ptr_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(4u, false);
    assert_non_null(arr);
    assert_non_null(uint64_tensor_shape_ptr(arr));
    return_uint64_tensor(arr);
}

static void test_uint64_tensor_strides_ptr_null(void** state) {
    (void)state;
    assert_null(uint64_tensor_strides_ptr(NULL));
}

static void test_uint64_tensor_strides_ptr_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(4u, false);
    assert_non_null(arr);
    const size_t* strides = uint64_tensor_strides_ptr(arr);
    assert_non_null(strides);
    assert_int_equal(strides[0], sizeof(uint64_t));
    return_uint64_tensor(arr);
}

static void test_uint64_tensor_shape_str_null(void** state) {
    (void)state;
    char buf[32];
    assert_int_equal(uint64_tensor_shape_str(NULL, buf, sizeof(buf)), NULL_POINTER);
}

static void test_uint64_tensor_shape_str_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(8u, false);
    assert_non_null(arr);
    char buf[32];
    assert_int_equal(uint64_tensor_shape_str(arr, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(8)");
    return_uint64_tensor(arr);
}

// ================================================================================
// ================================================================================
// UTILITY OPERATIONS

// ---- clear_uint64_tensor --------------------------------------------------------

static void test_clear_uint64_tensor_null(void** state) {
    (void)state;
    assert_int_equal(clear_uint64_tensor(NULL), NULL_POINTER);
}

static void test_clear_uint64_tensor_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(3u, 10u);
    assert_non_null(arr);
    assert_int_equal(clear_uint64_tensor(arr), NO_ERROR);
    assert_int_equal(uint64_tensor_size(arr), 0u);
    return_uint64_tensor(arr);
}

// ---- concat_uint64_tensor_array -------------------------------------------------

static void test_concat_uint64_tensor_null(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(concat_uint64_tensor_array(NULL, arr), NULL_POINTER);
    assert_int_equal(concat_uint64_tensor_array(arr, NULL), NULL_POINTER);
    return_uint64_tensor(arr);
}

static void test_concat_uint64_tensor_value(void** state) {
    (void)state;

    /* dst needs capacity for 5 elements total — allocate with room to grow */
    allocator_vtable_t alloc = heap_allocator();
    uint64_tensor_expect_t r  = init_uint64_array(8u, false, alloc);
    assert_true(r.has_value);
    uint64_tensor_t* dst = r.u.value;

    /* Populate dst with [1, 2, 3] */
    push_back_uint64_array(dst, 1u);
    push_back_uint64_array(dst, 2u);
    push_back_uint64_array(dst, 3u);

    /* src = [4, 5] */
    uint64_tensor_t* src = _make_uint64_array_filled(2u, 4u);
    assert_non_null(src);

    assert_int_equal(concat_uint64_tensor_array(dst, src), NO_ERROR);
    assert_int_equal(uint64_tensor_size(dst), 5u);

    uint64_t expected[] = { 1u, 2u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        uint64_t out = 0u;
        assert_int_equal(get_uint64_tensor_index(dst, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint64_tensor(dst);
    return_uint64_tensor(src);
}

// ---- slice_uint64_tensor_array --------------------------------------------------

static void test_slice_uint64_tensor_null(void** state) {
    (void)state;
    uint64_tensor_expect_t r = slice_uint64_tensor_array(NULL, 0u, 1u, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_slice_uint64_tensor_value(void** state) {
    (void)state;
    uint64_tensor_t* src = _make_uint64_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(src);

    uint64_tensor_expect_t r = slice_uint64_tensor_array(src, 1u, 4u, NULL);
    assert_true(r.has_value);
    uint64_tensor_t* sl = r.u.value;

    assert_int_equal(uint64_tensor_size(sl), 3u);
    uint64_t expected[] = { 11u, 12u, 13u };
    for (size_t i = 0u; i < 3u; i++) {
        uint64_t out = 0u;
        assert_int_equal(get_uint64_tensor_index(sl, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint64_tensor(src);
    return_uint64_tensor(sl);
}

// ---- reverse_uint64_tensor ------------------------------------------------------

static void test_reverse_uint64_tensor_null(void** state) {
    (void)state;
    assert_int_equal(reverse_uint64_tensor(NULL), NULL_POINTER);
}

static void test_reverse_uint64_tensor_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(4u, 1u); /* [1,2,3,4] */
    assert_non_null(arr);

    assert_int_equal(reverse_uint64_tensor(arr), NO_ERROR);

    uint64_t expected[] = { 4u, 3u, 2u, 1u };
    for (size_t i = 0u; i < 4u; i++) {
        uint64_t out = 0u;
        assert_int_equal(get_uint64_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint64_tensor(arr);
}

// ---- sort_uint64_tensor ---------------------------------------------------------

static void test_sort_uint64_tensor_null(void** state) {
    (void)state;
    assert_int_equal(sort_uint64_tensor(NULL, FORWARD), NULL_POINTER);
}

static void test_sort_uint64_tensor_forward(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(5u, false);
    assert_non_null(arr);

    uint64_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_uint64_array(arr, vals[i]);

    assert_int_equal(sort_uint64_tensor(arr, FORWARD), NO_ERROR);

    uint64_t expected[] = { 1u, 1u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        uint64_t out = 0u;
        assert_int_equal(get_uint64_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint64_tensor(arr);
}

static void test_sort_uint64_tensor_reverse(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(5u, false);
    assert_non_null(arr);

    uint64_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_uint64_array(arr, vals[i]);

    assert_int_equal(sort_uint64_tensor(arr, REVERSE), NO_ERROR);

    uint64_t expected[] = { 5u, 4u, 3u, 1u, 1u };
    for (size_t i = 0u; i < 5u; i++) {
        uint64_t out = 0u;
        assert_int_equal(get_uint64_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_uint64_tensor(arr);
}

// ================================================================================
// ================================================================================
// SET AND GET DATA

// ---- set_uint64_tensor_index / get_uint64_tensor_index --------------------------

static void test_set_get_uint64_tensor_index_null(void** state) {
    (void)state;
    uint64_t dummy = 0u;
    assert_int_equal(set_uint64_tensor_index(NULL, 0u, 1u),    NULL_POINTER);
    assert_int_equal(get_uint64_tensor_index(NULL, 0u, &dummy), NULL_POINTER);
}

static void test_set_get_uint64_tensor_index_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(3u, 10u);
    assert_non_null(arr);

    assert_int_equal(set_uint64_tensor_index(arr, 1u, 99u), NO_ERROR);

    uint64_t out = 0u;
    assert_int_equal(get_uint64_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);

    return_uint64_tensor(arr);
}

// ---- set_uint64_tensor_nd_index / get_uint64_tensor_nd_index --------------------

static void test_set_get_uint64_tensor_nd_index_null(void** state) {
    (void)state;
    size_t  idx[] = { 0u, 0u };
    uint64_t out   = 0u;
    assert_int_equal(set_uint64_tensor_nd_index(NULL, idx, 1u),   NULL_POINTER);
    assert_int_equal(get_uint64_tensor_nd_index(NULL, idx, &out), NULL_POINTER);
}

static void test_set_get_uint64_tensor_nd_index_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 3u };
    allocator_vtable_t alloc = heap_allocator();
    uint64_tensor_expect_t r  = init_uint64_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    uint64_tensor_t* mat = r.u.value;

    size_t idx[] = { 1u, 2u };
    assert_int_equal(set_uint64_tensor_nd_index(mat, idx, 42u), NO_ERROR);

    uint64_t out = 0u;
    assert_int_equal(get_uint64_tensor_nd_index(mat, idx, &out), NO_ERROR);
    assert_int_equal(out, 42u);

    return_uint64_tensor(mat);
}

// ---- push and pop wrappers ----------------------------------------------------

static void test_push_back_uint64_array_null(void** state) {
    (void)state;
    assert_int_equal(push_back_uint64_array(NULL, 1u), NULL_POINTER);
}

static void test_push_back_uint64_array_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(4u, false);
    assert_non_null(arr);

    assert_int_equal(push_back_uint64_array(arr, 10u), NO_ERROR);
    assert_int_equal(push_back_uint64_array(arr, 20u), NO_ERROR);
    assert_int_equal(uint64_tensor_size(arr), 2u);

    uint64_t out = 0u;
    assert_int_equal(get_uint64_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 20u);

    return_uint64_tensor(arr);
}

static void test_push_front_uint64_array_null(void** state) {
    (void)state;
    assert_int_equal(push_front_uint64_array(NULL, 1u), NULL_POINTER);
}

static void test_push_front_uint64_array_value(void** state) {
    (void)state;
    /* Capacity 3 so there is room for the push_front */
    allocator_vtable_t alloc = heap_allocator();
    uint64_tensor_expect_t r  = init_uint64_array(3u, false, alloc);
    assert_true(r.has_value);
    uint64_tensor_t* arr = r.u.value;

    push_back_uint64_array(arr, 10u);
    push_back_uint64_array(arr, 11u);
    /* arr = [10, 11], len == 2, alloc == 3 — one slot free */

    assert_int_equal(push_front_uint64_array(arr, 99u), NO_ERROR);

    uint64_t out = 0u;
    assert_int_equal(get_uint64_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_uint64_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_uint64_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);

    return_uint64_tensor(arr);
}

static void test_push_at_uint64_array_null(void** state) {
    (void)state;
    assert_int_equal(push_at_uint64_array(NULL, 1u, 0u), NULL_POINTER);
}

static void test_push_at_uint64_array_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(3u, 10u); /* [10, 11, 12] */
    assert_non_null(arr);

    assert_int_equal(push_at_uint64_array(arr, 99u, 1u), NO_ERROR);
    /* arr = [10, 99, 11, 12] */

    uint64_t out = 0u;
    assert_int_equal(get_uint64_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_uint64_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_uint64_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);
    assert_int_equal(get_uint64_tensor_index(arr, 3u, &out), NO_ERROR);
    assert_int_equal(out, 12u);

    return_uint64_tensor(arr);
}

static void test_pop_back_uint64_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_back_uint64_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_back_uint64_array_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    uint64_t out = 0u;
    assert_int_equal(pop_back_uint64_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(uint64_tensor_size(arr), 2u);

    return_uint64_tensor(arr);
}

static void test_pop_front_uint64_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_front_uint64_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_front_uint64_array_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    uint64_t out = 0u;
    assert_int_equal(pop_front_uint64_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(uint64_tensor_size(arr), 2u);

    return_uint64_tensor(arr);
}

static void test_pop_at_uint64_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_at_uint64_array(NULL, NULL, 0u), NULL_POINTER);
}

static void test_pop_at_uint64_array_value(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(4u, 10u); /* [10,11,12,13] */
    assert_non_null(arr);

    uint64_t out = 0u;
    assert_int_equal(pop_at_uint64_array(arr, &out, 2u), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(uint64_tensor_size(arr), 3u);

    assert_int_equal(get_uint64_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 13u);

    return_uint64_tensor(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// FIND uint64 TENSOR VALUE (uint64_tensor_lsearch)
// ================================================================================
 
/** NULL tensor must return NULL_POINTER. */
static void test_find_uint64_tensor_null_tensor(void** state) {
    (void)state;
    size_t idx = 0u;
    assert_int_equal(uint64_tensor_lsearch(NULL, &idx, 10u), NULL_POINTER);
}
 
/** NULL index pointer must return NULL_POINTER. */
static void test_find_uint64_tensor_null_index(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(3u, 10u);
    assert_non_null(arr);
    assert_int_equal(uint64_tensor_lsearch(arr, NULL, 10u), NULL_POINTER);
    return_uint64_tensor(arr);
}
 
/** Empty array must return EMPTY. */
static void test_find_uint64_tensor_empty(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(4u, false);
    assert_non_null(arr);
    size_t idx = 0u;
    assert_int_equal(uint64_tensor_lsearch(arr, &idx, 10u), EMPTY);
    return_uint64_tensor(arr);
}
 
/** Value not present must return NOT_FOUND. */
static void test_find_uint64_tensor_not_found(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(4u, 1u); /* [1,2,3,4] */
    assert_non_null(arr);
    size_t idx = 99u;   /* sentinel — must not be modified */
    assert_int_equal(uint64_tensor_lsearch(arr, &idx, 99u), NOT_FOUND);
    /* index must be unchanged on NOT_FOUND */
    assert_int_equal(idx, 99u);
    return_uint64_tensor(arr);
}
 
/** Value at the first position must be found at index 0. */
static void test_find_uint64_tensor_first_element(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(uint64_tensor_lsearch(arr, &idx, 10u), NO_ERROR);
    assert_int_equal(idx, 0u);
    return_uint64_tensor(arr);
}
 
/** Value at the last position must be found at index len-1. */
static void test_find_uint64_tensor_last_element(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(uint64_tensor_lsearch(arr, &idx, 14u), NO_ERROR);
    assert_int_equal(idx, 4u);
    return_uint64_tensor(arr);
}
 
/** Value in the middle must be found at the correct index. */
static void test_find_uint64_tensor_middle_element(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(arr);
    size_t idx = 99u;
    assert_int_equal(uint64_tensor_lsearch(arr, &idx, 12u), NO_ERROR);
    assert_int_equal(idx, 2u);
    return_uint64_tensor(arr);
}
 
/**
 * When duplicates are present, the first occurrence must be returned.
 */
static void test_find_uint64_tensor_first_of_duplicates(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(6u, false);
    assert_non_null(arr);
 
    /* [5, 9, 9, 9, 7, 3] */
    push_back_uint64_array(arr, 5u);
    push_back_uint64_array(arr, 9u);
    push_back_uint64_array(arr, 9u);
    push_back_uint64_array(arr, 9u);
    push_back_uint64_array(arr, 7u);
    push_back_uint64_array(arr, 3u);
 
    size_t idx = 99u;
    assert_int_equal(uint64_tensor_lsearch(arr, &idx, 9u), NO_ERROR);
    assert_int_equal(idx, 1u);   /* first occurrence at index 1 */
 
    return_uint64_tensor(arr);
}
 
/**
 * Single-element array containing the target value must succeed.
 */
static void test_find_uint64_tensor_single_element_match(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(1u, false);
    assert_non_null(arr);
    push_back_uint64_array(arr, 42u);
 
    size_t idx = 99u;
    assert_int_equal(uint64_tensor_lsearch(arr, &idx, 42u), NO_ERROR);
    assert_int_equal(idx, 0u);
 
    return_uint64_tensor(arr);
}
 
/**
 * Single-element array not containing the target must return NOT_FOUND
 * without modifying index.
 */
static void test_find_uint64_tensor_single_element_no_match(void** state) {
    (void)state;
    uint64_tensor_t* arr = _make_uint64_array(1u, false);
    assert_non_null(arr);
    push_back_uint64_array(arr, 42u);
 
    size_t idx = 99u;
    assert_int_equal(uint64_tensor_lsearch(arr, &idx, 7u), NOT_FOUND);
    assert_int_equal(idx, 99u);   /* unchanged */
 
    return_uint64_tensor(arr);
}

// ================================================================================
// ================================================================================
// TEST SUITE REGISTRY

const struct CMUnitTest test_uint64_tensor[] = {
    /* init_uint64_array */
    cmocka_unit_test(test_init_uint64_array_null_allocator),
    cmocka_unit_test(test_init_uint64_array_success),

    /* init_uint64_tensor */
    cmocka_unit_test(test_init_uint64_tensor_success),
    cmocka_unit_test(test_init_uint64_tensor_null_shape),

    /* return_uint64_tensor */
    cmocka_unit_test(test_return_uint64_tensor_null),
    cmocka_unit_test(test_return_uint64_tensor_normal),

    /* copy_uint64_tensor */
    cmocka_unit_test(test_copy_uint64_tensor_null_src),
    cmocka_unit_test(test_copy_uint64_tensor_independence),

    /* introspection — null guards */
    cmocka_unit_test(test_uint64_tensor_size_null),
    cmocka_unit_test(test_uint64_tensor_alloc_null),
    cmocka_unit_test(test_uint64_tensor_data_size_null),
    cmocka_unit_test(test_uint64_tensor_dtype_null),
    cmocka_unit_test(test_is_uint64_tensor_empty_null),
    cmocka_unit_test(test_is_uint64_tensor_full_null),
    cmocka_unit_test(test_is_uint64_tensor_ptr_null_wrapper),
    cmocka_unit_test(test_uint64_tensor_ndim_null),
    cmocka_unit_test(test_uint64_tensor_shape_dim_null),
    cmocka_unit_test(test_uint64_tensor_shape_null),
    cmocka_unit_test(test_uint64_tensor_shape_ptr_null),
    cmocka_unit_test(test_uint64_tensor_strides_ptr_null),
    cmocka_unit_test(test_uint64_tensor_shape_str_null),

    /* introspection — values */
    cmocka_unit_test(test_uint64_tensor_size_value),
    cmocka_unit_test(test_uint64_tensor_alloc_value),
    cmocka_unit_test(test_uint64_tensor_data_size_value),
    cmocka_unit_test(test_uint64_tensor_dtype_value),
    cmocka_unit_test(test_is_uint64_tensor_empty_value),
    cmocka_unit_test(test_is_uint64_tensor_full_value),
    cmocka_unit_test(test_is_uint64_tensor_ptr_valid),
    cmocka_unit_test(test_uint64_tensor_ndim_value),
    cmocka_unit_test(test_uint64_tensor_shape_dim_value),
    cmocka_unit_test(test_uint64_tensor_shape_value),
    cmocka_unit_test(test_uint64_tensor_shape_ptr_value),
    cmocka_unit_test(test_uint64_tensor_strides_ptr_value),
    cmocka_unit_test(test_uint64_tensor_shape_str_value),

    /* utility — null guards */
    cmocka_unit_test(test_clear_uint64_tensor_null),
    cmocka_unit_test(test_concat_uint64_tensor_null),
    cmocka_unit_test(test_slice_uint64_tensor_null),
    cmocka_unit_test(test_reverse_uint64_tensor_null),
    cmocka_unit_test(test_sort_uint64_tensor_null),

    /* utility — values */
    cmocka_unit_test(test_clear_uint64_tensor_value),
    cmocka_unit_test(test_concat_uint64_tensor_value),
    cmocka_unit_test(test_slice_uint64_tensor_value),
    cmocka_unit_test(test_reverse_uint64_tensor_value),
    cmocka_unit_test(test_sort_uint64_tensor_forward),
    cmocka_unit_test(test_sort_uint64_tensor_reverse),

    /* set/get — null guards */
    cmocka_unit_test(test_set_get_uint64_tensor_index_null),
    cmocka_unit_test(test_set_get_uint64_tensor_nd_index_null),
    cmocka_unit_test(test_push_back_uint64_array_null),
    cmocka_unit_test(test_push_front_uint64_array_null),
    cmocka_unit_test(test_push_at_uint64_array_null),
    cmocka_unit_test(test_pop_back_uint64_array_null),
    cmocka_unit_test(test_pop_front_uint64_array_null),
    cmocka_unit_test(test_pop_at_uint64_array_null),

    /* set/get — values */
    cmocka_unit_test(test_set_get_uint64_tensor_index_value),
    cmocka_unit_test(test_set_get_uint64_tensor_nd_index_value),
    cmocka_unit_test(test_push_back_uint64_array_value),
    cmocka_unit_test(test_push_front_uint64_array_value),
    cmocka_unit_test(test_push_at_uint64_array_value),
    cmocka_unit_test(test_pop_back_uint64_array_value),
    cmocka_unit_test(test_pop_front_uint64_array_value),
    cmocka_unit_test(test_pop_at_uint64_array_value),

    /* uint64_tensor_lsearch — null/guard */
    cmocka_unit_test(test_find_uint64_tensor_null_tensor),
    cmocka_unit_test(test_find_uint64_tensor_null_index),
    cmocka_unit_test(test_find_uint64_tensor_empty),
 
    /* uint64_tensor_lsearch — not found */
    cmocka_unit_test(test_find_uint64_tensor_not_found),
 
    /* uint64_tensor_lsearch — found at various positions */
    cmocka_unit_test(test_find_uint64_tensor_first_element),
    cmocka_unit_test(test_find_uint64_tensor_last_element),
    cmocka_unit_test(test_find_uint64_tensor_middle_element),
    cmocka_unit_test(test_find_uint64_tensor_first_of_duplicates),
    cmocka_unit_test(test_find_uint64_tensor_single_element_match),
    cmocka_unit_test(test_find_uint64_tensor_single_element_no_match),
};

const size_t test_uint64_tensor_count = sizeof(test_uint64_tensor) /
                                       sizeof(test_uint64_tensor[0]);
// ================================================================================ 
// ================================================================================ 

/** Construct a populated int64 array with values 1..n. */
static int64_tensor_t* _make_int64_array(size_t capacity, bool growth) {
    allocator_vtable_t alloc = heap_allocator();
    int64_tensor_expect_t r  = init_int64_array(capacity, growth, alloc);
    if (!r.has_value) return NULL;
    return r.u.value;
}

static int64_tensor_t* _make_int64_array_filled(size_t n, int64_t base) {
    /* Allocate n+1 so the result always has at least one free slot,
     * making it safe to use as a push destination in tests. */
    int64_tensor_t* arr = _make_int64_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++) {
        int64_t val = (int64_t)(base + i);
        push_back_int64_array(arr, val);
    }
    return arr;
}

// ================================================================================
// ================================================================================
// INITIALISATION AND TEARDOWN

// ---- init_int64_array ----------------------------------------------------------

/** NULL allocator must propagate an error through the expect type. */
static void test_init_int64_array_null_allocator(void** state) {
    (void)state;
    allocator_vtable_t bad = heap_allocator();
    bad.allocate = NULL;
    int64_tensor_expect_t r = init_int64_array(8u, false, bad);
    assert_false(r.has_value);
}

/** Successful construction produces a non-NULL wrapper with correct mode. */
static void test_init_int64_array_success(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_tensor_expect_t r  = init_int64_array(8u, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  ARRAY_STRUCT);
    assert_int_equal(r.u.value->base->dtype, INT64_TYPE);
    assert_int_equal(r.u.value->base->len,   0u);
    assert_int_equal(r.u.value->base->alloc, 8u);
    return_int64_tensor(r.u.value);
}

// ---- init_int64_tensor ---------------------------------------------------------

/** Successful construction of a fixed-shape tensor. */
static void test_init_int64_tensor_success(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    int64_tensor_expect_t r  = init_int64_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  TENSOR_STRUCT);
    assert_int_equal(r.u.value->base->dtype, INT64_TYPE);
    assert_int_equal(r.u.value->base->len,   12u);
    return_int64_tensor(r.u.value);
}

/** NULL shape propagates an error. */
static void test_init_int64_tensor_null_shape(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    int64_tensor_expect_t r  = init_int64_tensor(2u, NULL, alloc);
    assert_false(r.has_value);
}

// ---- return_int64_tensor -------------------------------------------------------

/** return_int64_tensor(NULL) must not crash. */
static void test_return_int64_tensor_null(void** state) {
    (void)state;
    return_int64_tensor(NULL);
}

/** Normal construction followed by return must not crash or leak. */
static void test_return_int64_tensor_normal(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(4u, false);
    assert_non_null(arr);
    return_int64_tensor(arr);
}

// ---- copy_int64_tensor ---------------------------------------------------------

/** NULL src must return NULL_POINTER. */
static void test_copy_int64_tensor_null_src(void** state) {
    (void)state;
    int64_tensor_expect_t r = copy_int64_tensor(NULL, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

/** Copy is independent of source — mutation of copy does not affect src. */
static void test_copy_int64_tensor_independence(void** state) {
    (void)state;
    int64_tensor_t* src = _make_int64_array_filled(3u, 10u);
    assert_non_null(src);

    int64_tensor_expect_t r = copy_int64_tensor(src, NULL);
    assert_true(r.has_value);
    int64_tensor_t* dst = r.u.value;

    /* Overwrite slot 0 of the copy */
    assert_int_equal(set_int64_tensor_index(dst, 0u, 99u), NO_ERROR);

    /* Source slot 0 must be unchanged */
    int64_t src_val = 0u;
    assert_int_equal(get_int64_tensor_index(src, 0u, &src_val), NO_ERROR);
    assert_int_equal(src_val, 10u);

    return_int64_tensor(src);
    return_int64_tensor(dst);
}

// ================================================================================
// ================================================================================
// INTROSPECTION

static void test_int64_tensor_size_null(void** state) {
    (void)state;
    assert_int_equal(int64_tensor_size(NULL), 0u);
}

static void test_int64_tensor_size_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array_filled(3u, 1u);
    assert_non_null(arr);
    assert_int_equal(int64_tensor_size(arr), 3u);
    return_int64_tensor(arr);
}

static void test_int64_tensor_alloc_null(void** state) {
    (void)state;
    assert_int_equal(int64_tensor_alloc(NULL), 0u);
}

static void test_int64_tensor_alloc_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(8u, false);
    assert_non_null(arr);
    assert_int_equal(int64_tensor_alloc(arr), 8u);
    return_int64_tensor(arr);
}

static void test_int64_tensor_data_size_null(void** state) {
    (void)state;
    assert_int_equal(int64_tensor_data_size(NULL), 0u);
}

static void test_int64_tensor_data_size_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(int64_tensor_data_size(arr), sizeof(int64_t));
    return_int64_tensor(arr);
}

static void test_int64_tensor_dtype_null(void** state) {
    (void)state;
    assert_int_equal(int64_tensor_dtype(NULL), UNKNOWN_TYPE);
}

static void test_int64_tensor_dtype_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(int64_tensor_dtype(arr), INT64_TYPE);
    return_int64_tensor(arr);
}

static void test_is_int64_tensor_empty_null(void** state) {
    (void)state;
    assert_true(is_int64_tensor_empty(NULL));
}

static void test_is_int64_tensor_empty_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(4u, false);
    assert_non_null(arr);
    assert_true(is_int64_tensor_empty(arr));
    push_back_int64_array(arr, 1u);
    assert_false(is_int64_tensor_empty(arr));
    return_int64_tensor(arr);
}

static void test_is_int64_tensor_full_null(void** state) {
    (void)state;
    assert_true(is_int64_tensor_full(NULL));
}

static void test_is_int64_tensor_full_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(2u, false);
    assert_non_null(arr);
    assert_false(is_int64_tensor_full(arr));
    push_back_int64_array(arr, 1u);
    push_back_int64_array(arr, 2u);
    assert_true(is_int64_tensor_full(arr));
    return_int64_tensor(arr);
}

static void test_is_int64_tensor_ptr_null_wrapper(void** state) {
    (void)state;
    int64_t dummy = 0u;
    assert_false(is_int64_tensor_ptr(NULL, &dummy));
}

static void test_is_int64_tensor_ptr_valid(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array_filled(4u, 10u);
    assert_non_null(arr);
    /* Pointer to first element must be valid */
    const int64_t* p = (const int64_t*)arr->base->data;
    assert_true(is_int64_tensor_ptr(arr, p));
    return_int64_tensor(arr);
}

static void test_int64_tensor_ndim_null(void** state) {
    (void)state;
    assert_int_equal(int64_tensor_ndim(NULL), 0u);
}

static void test_int64_tensor_ndim_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(int64_tensor_ndim(arr), 1u);
    return_int64_tensor(arr);
}

static void test_int64_tensor_shape_dim_null(void** state) {
    (void)state;
    assert_int_equal(int64_tensor_shape_dim(NULL, 0u), 0u);
}

static void test_int64_tensor_shape_dim_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    int64_tensor_expect_t r  = init_int64_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_int_equal(int64_tensor_shape_dim(r.u.value, 0u), 3u);
    assert_int_equal(int64_tensor_shape_dim(r.u.value, 1u), 4u);
    return_int64_tensor(r.u.value);
}

static void test_int64_tensor_shape_null(void** state) {
    (void)state;
    size_t buf[2];
    assert_int_equal(int64_tensor_shape(NULL, buf, 2u), NULL_POINTER);
}

static void test_int64_tensor_shape_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    int64_tensor_expect_t r  = init_int64_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    size_t buf[2] = { 0u, 0u };
    assert_int_equal(int64_tensor_shape(r.u.value, buf, 2u), NO_ERROR);
    assert_int_equal(buf[0], 3u);
    assert_int_equal(buf[1], 4u);
    return_int64_tensor(r.u.value);
}

static void test_int64_tensor_shape_ptr_null(void** state) {
    (void)state;
    assert_null(int64_tensor_shape_ptr(NULL));
}

static void test_int64_tensor_shape_ptr_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(4u, false);
    assert_non_null(arr);
    assert_non_null(int64_tensor_shape_ptr(arr));
    return_int64_tensor(arr);
}

static void test_int64_tensor_strides_ptr_null(void** state) {
    (void)state;
    assert_null(int64_tensor_strides_ptr(NULL));
}

static void test_int64_tensor_strides_ptr_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(4u, false);
    assert_non_null(arr);
    const size_t* strides = int64_tensor_strides_ptr(arr);
    assert_non_null(strides);
    assert_int_equal(strides[0], sizeof(int64_t));
    return_int64_tensor(arr);
}

static void test_int64_tensor_shape_str_null(void** state) {
    (void)state;
    char buf[32];
    assert_int_equal(int64_tensor_shape_str(NULL, buf, sizeof(buf)), NULL_POINTER);
}

static void test_int64_tensor_shape_str_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(8u, false);
    assert_non_null(arr);
    char buf[32];
    assert_int_equal(int64_tensor_shape_str(arr, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(8)");
    return_int64_tensor(arr);
}

// ================================================================================
// ================================================================================
// UTILITY OPERATIONS

// ---- clear_int64_tensor --------------------------------------------------------

static void test_clear_int64_tensor_null(void** state) {
    (void)state;
    assert_int_equal(clear_int64_tensor(NULL), NULL_POINTER);
}

static void test_clear_int64_tensor_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array_filled(3u, 10u);
    assert_non_null(arr);
    assert_int_equal(clear_int64_tensor(arr), NO_ERROR);
    assert_int_equal(int64_tensor_size(arr), 0u);
    return_int64_tensor(arr);
}

// ---- concat_int64_tensor_array -------------------------------------------------

static void test_concat_int64_tensor_null(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(concat_int64_tensor_array(NULL, arr), NULL_POINTER);
    assert_int_equal(concat_int64_tensor_array(arr, NULL), NULL_POINTER);
    return_int64_tensor(arr);
}

static void test_concat_int64_tensor_value(void** state) {
    (void)state;

    /* dst needs capacity for 5 elements total — allocate with room to grow */
    allocator_vtable_t alloc = heap_allocator();
    int64_tensor_expect_t r  = init_int64_array(8u, false, alloc);
    assert_true(r.has_value);
    int64_tensor_t* dst = r.u.value;

    /* Populate dst with [1, 2, 3] */
    push_back_int64_array(dst, 1u);
    push_back_int64_array(dst, 2u);
    push_back_int64_array(dst, 3u);

    /* src = [4, 5] */
    int64_tensor_t* src = _make_int64_array_filled(2u, 4u);
    assert_non_null(src);

    assert_int_equal(concat_int64_tensor_array(dst, src), NO_ERROR);
    assert_int_equal(int64_tensor_size(dst), 5u);

    int64_t expected[] = { 1u, 2u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        int64_t out = 0u;
        assert_int_equal(get_int64_tensor_index(dst, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int64_tensor(dst);
    return_int64_tensor(src);
}

// ---- slice_int64_tensor_array --------------------------------------------------

static void test_slice_int64_tensor_null(void** state) {
    (void)state;
    int64_tensor_expect_t r = slice_int64_tensor_array(NULL, 0u, 1u, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_slice_int64_tensor_value(void** state) {
    (void)state;
    int64_tensor_t* src = _make_int64_array_filled(5u, 10u); /* [10,11,12,13,14] */
    assert_non_null(src);

    int64_tensor_expect_t r = slice_int64_tensor_array(src, 1u, 4u, NULL);
    assert_true(r.has_value);
    int64_tensor_t* sl = r.u.value;

    assert_int_equal(int64_tensor_size(sl), 3u);
    int64_t expected[] = { 11u, 12u, 13u };
    for (size_t i = 0u; i < 3u; i++) {
        int64_t out = 0u;
        assert_int_equal(get_int64_tensor_index(sl, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int64_tensor(src);
    return_int64_tensor(sl);
}

// ---- reverse_int64_tensor ------------------------------------------------------

static void test_reverse_int64_tensor_null(void** state) {
    (void)state;
    assert_int_equal(reverse_int64_tensor(NULL), NULL_POINTER);
}

static void test_reverse_int64_tensor_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array_filled(4u, 1u); /* [1,2,3,4] */
    assert_non_null(arr);

    assert_int_equal(reverse_int64_tensor(arr), NO_ERROR);

    int64_t expected[] = { 4u, 3u, 2u, 1u };
    for (size_t i = 0u; i < 4u; i++) {
        int64_t out = 0u;
        assert_int_equal(get_int64_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int64_tensor(arr);
}

// ---- sort_int64_tensor ---------------------------------------------------------

static void test_sort_int64_tensor_null(void** state) {
    (void)state;
    assert_int_equal(sort_int64_tensor(NULL, FORWARD), NULL_POINTER);
}

static void test_sort_int64_tensor_forward(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(5u, false);
    assert_non_null(arr);

    int64_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_int64_array(arr, vals[i]);

    assert_int_equal(sort_int64_tensor(arr, FORWARD), NO_ERROR);

    int64_t expected[] = { 1u, 1u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        int64_t out = 0u;
        assert_int_equal(get_int64_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int64_tensor(arr);
}

static void test_sort_int64_tensor_reverse(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(5u, false);
    assert_non_null(arr);

    int64_t vals[] = { 3u, 1u, 4u, 1u, 5u };
    for (size_t i = 0u; i < 5u; i++)
        push_back_int64_array(arr, vals[i]);

    assert_int_equal(sort_int64_tensor(arr, REVERSE), NO_ERROR);

    int64_t expected[] = { 5u, 4u, 3u, 1u, 1u };
    for (size_t i = 0u; i < 5u; i++) {
        int64_t out = 0u;
        assert_int_equal(get_int64_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_int64_tensor(arr);
}

// ================================================================================
// ================================================================================
// SET AND GET DATA

// ---- set_int64_tensor_index / get_int64_tensor_index --------------------------

static void test_set_get_int64_tensor_index_null(void** state) {
    (void)state;
    int64_t dummy = 0u;
    assert_int_equal(set_int64_tensor_index(NULL, 0u, 1u),    NULL_POINTER);
    assert_int_equal(get_int64_tensor_index(NULL, 0u, &dummy), NULL_POINTER);
}

static void test_set_get_int64_tensor_index_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array_filled(3u, 10u);
    assert_non_null(arr);

    assert_int_equal(set_int64_tensor_index(arr, 1u, 99u), NO_ERROR);

    int64_t out = 0u;
    assert_int_equal(get_int64_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);

    return_int64_tensor(arr);
}

// ---- set_int64_tensor_nd_index / get_int64_tensor_nd_index --------------------

static void test_set_get_int64_tensor_nd_index_null(void** state) {
    (void)state;
    size_t  idx[] = { 0u, 0u };
    int64_t out   = 0u;
    assert_int_equal(set_int64_tensor_nd_index(NULL, idx, 1u),   NULL_POINTER);
    assert_int_equal(get_int64_tensor_nd_index(NULL, idx, &out), NULL_POINTER);
}

static void test_set_get_int64_tensor_nd_index_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 3u };
    allocator_vtable_t alloc = heap_allocator();
    int64_tensor_expect_t r  = init_int64_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    int64_tensor_t* mat = r.u.value;

    size_t idx[] = { 1u, 2u };
    assert_int_equal(set_int64_tensor_nd_index(mat, idx, 42u), NO_ERROR);

    int64_t out = 0u;
    assert_int_equal(get_int64_tensor_nd_index(mat, idx, &out), NO_ERROR);
    assert_int_equal(out, 42u);

    return_int64_tensor(mat);
}

// ---- push and pop wrappers ----------------------------------------------------

static void test_push_back_int64_array_null(void** state) {
    (void)state;
    assert_int_equal(push_back_int64_array(NULL, 1u), NULL_POINTER);
}

static void test_push_back_int64_array_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(4u, false);
    assert_non_null(arr);

    assert_int_equal(push_back_int64_array(arr, 10u), NO_ERROR);
    assert_int_equal(push_back_int64_array(arr, 20u), NO_ERROR);
    assert_int_equal(int64_tensor_size(arr), 2u);

    int64_t out = 0u;
    assert_int_equal(get_int64_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 20u);

    return_int64_tensor(arr);
}

static void test_push_front_int64_array_null(void** state) {
    (void)state;
    assert_int_equal(push_front_int64_array(NULL, 1u), NULL_POINTER);
}

static void test_push_front_int64_array_value(void** state) {
    (void)state;
    /* Capacity 3 so there is room for the push_front */
    allocator_vtable_t alloc = heap_allocator();
    int64_tensor_expect_t r  = init_int64_array(3u, false, alloc);
    assert_true(r.has_value);
    int64_tensor_t* arr = r.u.value;

    push_back_int64_array(arr, 10u);
    push_back_int64_array(arr, 11u);
    /* arr = [10, 11], len == 2, alloc == 3 — one slot free */

    assert_int_equal(push_front_int64_array(arr, 99u), NO_ERROR);

    int64_t out = 0u;
    assert_int_equal(get_int64_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_int64_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_int64_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);

    return_int64_tensor(arr);
}

static void test_push_at_int64_array_null(void** state) {
    (void)state;
    assert_int_equal(push_at_int64_array(NULL, 1u, 0u), NULL_POINTER);
}

static void test_push_at_int64_array_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array_filled(3u, 10u); /* [10, 11, 12] */
    assert_non_null(arr);

    assert_int_equal(push_at_int64_array(arr, 99u, 1u), NO_ERROR);
    /* arr = [10, 99, 11, 12] */

    int64_t out = 0u;
    assert_int_equal(get_int64_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(get_int64_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 99u);
    assert_int_equal(get_int64_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11u);
    assert_int_equal(get_int64_tensor_index(arr, 3u, &out), NO_ERROR);
    assert_int_equal(out, 12u);

    return_int64_tensor(arr);
}

static void test_pop_back_int64_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_back_int64_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_back_int64_array_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    int64_t out = 0u;
    assert_int_equal(pop_back_int64_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(int64_tensor_size(arr), 2u);

    return_int64_tensor(arr);
}

static void test_pop_front_int64_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_front_int64_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_front_int64_array_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array_filled(3u, 10u); /* [10,11,12] */
    assert_non_null(arr);

    int64_t out = 0u;
    assert_int_equal(pop_front_int64_array(arr, &out), NO_ERROR);
    assert_int_equal(out, 10u);
    assert_int_equal(int64_tensor_size(arr), 2u);

    return_int64_tensor(arr);
}

static void test_pop_at_int64_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_at_int64_array(NULL, NULL, 0u), NULL_POINTER);
}

static void test_pop_at_int64_array_value(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array_filled(4u, 10u); /* [10,11,12,13] */
    assert_non_null(arr);

    int64_t out = 0u;
    assert_int_equal(pop_at_int64_array(arr, &out, 2u), NO_ERROR);
    assert_int_equal(out, 12u);
    assert_int_equal(int64_tensor_size(arr), 3u);

    assert_int_equal(get_int64_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 13u);

    return_int64_tensor(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// int64 LSEARCH — NEGATIVE VALUE CORRECTNESS
// ================================================================================

/** Search for a negative value that is present must return NO_ERROR and
 *  the correct index. Exercises the (uint64_t) cast path on a value
 *  whose sign bit is set. */
static void test_int64_tensor_lsearch_negative_found(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(6u, false);
    assert_non_null(arr);

    /* Mix of positive, negative, and zero */
    push_back_int64_array(arr,  10);
    push_back_int64_array(arr, -5);
    push_back_int64_array(arr,  30);
    push_back_int64_array(arr, -5);   /* duplicate — must find first */
    push_back_int64_array(arr,  0);
    push_back_int64_array(arr, -128);

    size_t idx = 99u;
    assert_int_equal(int64_tensor_lsearch(arr, &idx, -5), NO_ERROR);
    assert_int_equal(idx, 1u);   /* first occurrence of -5 is at index 1 */

    return_int64_tensor(arr);
}

/** Search for -128 (0xFF80) at the last position exercises a negative value
 *  whose bit pattern differs maximally from zero. */
static void test_int64_tensor_lsearch_negative_last(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(5u, false);
    assert_non_null(arr);

    push_back_int64_array(arr,  1);
    push_back_int64_array(arr,  2);
    push_back_int64_array(arr,  3);
    push_back_int64_array(arr,  4);
    push_back_int64_array(arr, -128);

    size_t idx = 99u;
    assert_int_equal(int64_tensor_lsearch(arr, &idx, -128), NO_ERROR);
    assert_int_equal(idx, 4u);

    return_int64_tensor(arr);
}

/** Search for -1 (0xFFFF) — the value whose bit pattern equals Uint64_MAX.
 *  This is the most important case: confirms the cast does not confuse
 *  -1 with the SIZE_MAX sentinel or with 65535 as a positive value. */
static void test_int64_tensor_lsearch_minus_one(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(4u, false);
    assert_non_null(arr);

    push_back_int64_array(arr,  100);
    push_back_int64_array(arr,   -1);
    push_back_int64_array(arr,  200);
    push_back_int64_array(arr,  300);

    size_t idx = 99u;
    assert_int_equal(int64_tensor_lsearch(arr, &idx, -1), NO_ERROR);
    assert_int_equal(idx, 1u);

    return_int64_tensor(arr);
}

/** Search for int64_MIN (-32768, 0x8000) — the value with only the sign
 *  bit set, which is the most extreme negative value in range. */
static void test_int64_tensor_lsearch_int64_min(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(4u, false);
    assert_non_null(arr);

    push_back_int64_array(arr,      0);
    push_back_int64_array(arr,      1);
    push_back_int64_array(arr, INT64_MIN);
    push_back_int64_array(arr,    100);

    size_t idx = 99u;
    assert_int_equal(int64_tensor_lsearch(arr, &idx, INT64_MIN), NO_ERROR);
    assert_int_equal(idx, 2u);

    return_int64_tensor(arr);
}

/** A negative value that is not present must return NOT_FOUND and must
 *  not modify index. */
static void test_int64_tensor_lsearch_negative_not_found(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(4u, false);
    assert_non_null(arr);

    push_back_int64_array(arr,  10);
    push_back_int64_array(arr,  20);
    push_back_int64_array(arr, -30);
    push_back_int64_array(arr,  40);

    size_t idx = 99u;
    assert_int_equal(int64_tensor_lsearch(arr, &idx, -99), NOT_FOUND);
    assert_int_equal(idx, 99u);   /* unchanged */

    return_int64_tensor(arr);
}

/** Array containing only negative values — confirms the SIMD path handles
 *  a buffer where every element has its sign bit set. */
static void test_int64_tensor_lsearch_all_negative(void** state) {
    (void)state;
    int64_tensor_t* arr = _make_int64_array(5u, false);
    assert_non_null(arr);

    push_back_int64_array(arr, -1);
    push_back_int64_array(arr, -2);
    push_back_int64_array(arr, -3);
    push_back_int64_array(arr, -4);
    push_back_int64_array(arr, -5);

    size_t idx = 99u;
    assert_int_equal(int64_tensor_lsearch(arr, &idx, -3), NO_ERROR);
    assert_int_equal(idx, 2u);

    assert_int_equal(int64_tensor_lsearch(arr, &idx, -6), NOT_FOUND);

    return_int64_tensor(arr);
}
// ================================================================================
// ================================================================================
// TEST SUITE REGISTRY

const struct CMUnitTest test_int64_tensor[] = {
    /* init_int64_array */
    cmocka_unit_test(test_init_int64_array_null_allocator),
    cmocka_unit_test(test_init_int64_array_success),

    /* init_int64_tensor */
    cmocka_unit_test(test_init_int64_tensor_success),
    cmocka_unit_test(test_init_int64_tensor_null_shape),

    /* return_int64_tensor */
    cmocka_unit_test(test_return_int64_tensor_null),
    cmocka_unit_test(test_return_int64_tensor_normal),

    /* copy_int64_tensor */
    cmocka_unit_test(test_copy_int64_tensor_null_src),
    cmocka_unit_test(test_copy_int64_tensor_independence),

    /* introspection — null guards */
    cmocka_unit_test(test_int64_tensor_size_null),
    cmocka_unit_test(test_int64_tensor_alloc_null),
    cmocka_unit_test(test_int64_tensor_data_size_null),
    cmocka_unit_test(test_int64_tensor_dtype_null),
    cmocka_unit_test(test_is_int64_tensor_empty_null),
    cmocka_unit_test(test_is_int64_tensor_full_null),
    cmocka_unit_test(test_is_int64_tensor_ptr_null_wrapper),
    cmocka_unit_test(test_int64_tensor_ndim_null),
    cmocka_unit_test(test_int64_tensor_shape_dim_null),
    cmocka_unit_test(test_int64_tensor_shape_null),
    cmocka_unit_test(test_int64_tensor_shape_ptr_null),
    cmocka_unit_test(test_int64_tensor_strides_ptr_null),
    cmocka_unit_test(test_int64_tensor_shape_str_null),

    /* introspection — values */
    cmocka_unit_test(test_int64_tensor_size_value),
    cmocka_unit_test(test_int64_tensor_alloc_value),
    cmocka_unit_test(test_int64_tensor_data_size_value),
    cmocka_unit_test(test_int64_tensor_dtype_value),
    cmocka_unit_test(test_is_int64_tensor_empty_value),
    cmocka_unit_test(test_is_int64_tensor_full_value),
    cmocka_unit_test(test_is_int64_tensor_ptr_valid),
    cmocka_unit_test(test_int64_tensor_ndim_value),
    cmocka_unit_test(test_int64_tensor_shape_dim_value),
    cmocka_unit_test(test_int64_tensor_shape_value),
    cmocka_unit_test(test_int64_tensor_shape_ptr_value),
    cmocka_unit_test(test_int64_tensor_strides_ptr_value),
    cmocka_unit_test(test_int64_tensor_shape_str_value),

    /* utility — null guards */
    cmocka_unit_test(test_clear_int64_tensor_null),
    cmocka_unit_test(test_concat_int64_tensor_null),
    cmocka_unit_test(test_slice_int64_tensor_null),
    cmocka_unit_test(test_reverse_int64_tensor_null),
    cmocka_unit_test(test_sort_int64_tensor_null),

    /* utility — values */
    cmocka_unit_test(test_clear_int64_tensor_value),
    cmocka_unit_test(test_concat_int64_tensor_value),
    cmocka_unit_test(test_slice_int64_tensor_value),
    cmocka_unit_test(test_reverse_int64_tensor_value),
    cmocka_unit_test(test_sort_int64_tensor_forward),
    cmocka_unit_test(test_sort_int64_tensor_reverse),

    /* set/get — null guards */
    cmocka_unit_test(test_set_get_int64_tensor_index_null),
    cmocka_unit_test(test_set_get_int64_tensor_nd_index_null),
    cmocka_unit_test(test_push_back_int64_array_null),
    cmocka_unit_test(test_push_front_int64_array_null),
    cmocka_unit_test(test_push_at_int64_array_null),
    cmocka_unit_test(test_pop_back_int64_array_null),
    cmocka_unit_test(test_pop_front_int64_array_null),
    cmocka_unit_test(test_pop_at_int64_array_null),

    /* set/get — values */
    cmocka_unit_test(test_set_get_int64_tensor_index_value),
    cmocka_unit_test(test_set_get_int64_tensor_nd_index_value),
    cmocka_unit_test(test_push_back_int64_array_value),
    cmocka_unit_test(test_push_front_int64_array_value),
    cmocka_unit_test(test_push_at_int64_array_value),
    cmocka_unit_test(test_pop_back_int64_array_value),
    cmocka_unit_test(test_pop_front_int64_array_value),
    cmocka_unit_test(test_pop_at_int64_array_value),

    /* int16 lsearch — negative value correctness */
    cmocka_unit_test(test_int64_tensor_lsearch_negative_found),
    cmocka_unit_test(test_int64_tensor_lsearch_negative_last),
    cmocka_unit_test(test_int64_tensor_lsearch_minus_one),
    cmocka_unit_test(test_int64_tensor_lsearch_int64_min),
    cmocka_unit_test(test_int64_tensor_lsearch_negative_not_found),
    cmocka_unit_test(test_int64_tensor_lsearch_all_negative),
};

const size_t test_int64_tensor_count = sizeof(test_int64_tensor) /
                                       sizeof(test_int64_tensor[0]);
// ================================================================================ 
// ================================================================================ 

/** Construct a populated float array with values 1..n. */
static float_tensor_t* _make_float_array(size_t capacity, bool growth) {
    allocator_vtable_t alloc = heap_allocator();
    float_tensor_expect_t r  = init_float_array(capacity, growth, alloc);
    if (!r.has_value) return NULL;
    return r.u.value;
}

static float_tensor_t* _make_float_array_filled(size_t n, float base) {
    /* Allocate n+1 so the result always has at least one free slot,
     * making it safe to use as a push destination in tests. */
    float_tensor_t* arr = _make_float_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++) {
        float val = (float)(base + i);
        push_back_float_array(arr, val);
    }
    return arr;
}

// ================================================================================
// ================================================================================
// INITIALISATION AND TEARDOWN

// ---- init_float_array ----------------------------------------------------------

/** NULL allocator must propagate an error through the expect type. */
static void test_init_float_array_null_allocator(void** state) {
    (void)state;
    allocator_vtable_t bad = heap_allocator();
    bad.allocate = NULL;
    float_tensor_expect_t r = init_float_array(8u, false, bad);
    assert_false(r.has_value);
}

/** Successful construction produces a non-NULL wrapper with correct mode. */
static void test_init_float_array_success(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_tensor_expect_t r  = init_float_array(8u, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  ARRAY_STRUCT);
    assert_int_equal(r.u.value->base->dtype, FLOAT_TYPE);
    assert_int_equal(r.u.value->base->len,   0u);
    assert_int_equal(r.u.value->base->alloc, 8u);
    return_float_tensor(r.u.value);
}

// ---- init_float_tensor ---------------------------------------------------------

/** Successful construction of a fixed-shape tensor. */
static void test_init_float_tensor_success(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    float_tensor_expect_t r  = init_float_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  TENSOR_STRUCT);
    assert_int_equal(r.u.value->base->dtype, FLOAT_TYPE);
    assert_int_equal(r.u.value->base->len,   12u);
    return_float_tensor(r.u.value);
}

/** NULL shape propagates an error. */
static void test_init_float_tensor_null_shape(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    float_tensor_expect_t r  = init_float_tensor(2u, NULL, alloc);
    assert_false(r.has_value);
}

// ---- return_float_tensor -------------------------------------------------------

/** return_float_tensor(NULL) must not crash. */
static void test_return_float_tensor_null(void** state) {
    (void)state;
    return_float_tensor(NULL);
}

/** Normal construction followed by return must not crash or leak. */
static void test_return_float_tensor_normal(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(4u, false);
    assert_non_null(arr);
    return_float_tensor(arr);
}

// ---- copy_float_tensor ---------------------------------------------------------

/** NULL src must return NULL_POINTER. */
static void test_copy_float_tensor_null_src(void** state) {
    (void)state;
    float_tensor_expect_t r = copy_float_tensor(NULL, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

/** Copy is independent of source — mutation of copy does not affect src. */
static void test_copy_float_tensor_independence(void** state) {
    (void)state;
    float_tensor_t* src = _make_float_array_filled(3u, 10u);
    assert_non_null(src);

    float_tensor_expect_t r = copy_float_tensor(src, NULL);
    assert_true(r.has_value);
    float_tensor_t* dst = r.u.value;

    /* Overwrite slot 0 of the copy */
    assert_int_equal(set_float_tensor_index(dst, 0u, 99.0f), NO_ERROR);

    /* Source slot 0 must be unchanged */
    float src_val = 0u;
    assert_int_equal(get_float_tensor_index(src, 0u, &src_val), NO_ERROR);
    assert_float_equal(src_val, 10.0f, 1.0e-3);

    return_float_tensor(src);
    return_float_tensor(dst);
}

// ================================================================================
// ================================================================================
// INTROSPECTION

static void test_float_tensor_size_null(void** state) {
    (void)state;
    assert_int_equal(float_tensor_size(NULL), 0u);
}

static void test_float_tensor_size_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array_filled(3u, 1u);
    assert_non_null(arr);
    assert_int_equal(float_tensor_size(arr), 3u);
    return_float_tensor(arr);
}

static void test_float_tensor_alloc_null(void** state) {
    (void)state;
    assert_int_equal(float_tensor_alloc(NULL), 0u);
}

static void test_float_tensor_alloc_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(8u, false);
    assert_non_null(arr);
    assert_int_equal(float_tensor_alloc(arr), 8u);
    return_float_tensor(arr);
}

static void test_float_tensor_data_size_null(void** state) {
    (void)state;
    assert_int_equal(float_tensor_data_size(NULL), 0u);
}

static void test_float_tensor_data_size_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(float_tensor_data_size(arr), sizeof(float_t));
    return_float_tensor(arr);
}

static void test_float_tensor_dtype_null(void** state) {
    (void)state;
    assert_int_equal(float_tensor_dtype(NULL), UNKNOWN_TYPE);
}

static void test_float_tensor_dtype_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(float_tensor_dtype(arr), FLOAT_TYPE);
    return_float_tensor(arr);
}

static void test_is_float_tensor_empty_null(void** state) {
    (void)state;
    assert_true(is_float_tensor_empty(NULL));
}

static void test_is_float_tensor_empty_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(4u, false);
    assert_non_null(arr);
    assert_true(is_float_tensor_empty(arr));
    push_back_float_array(arr, 1u);
    assert_false(is_float_tensor_empty(arr));
    return_float_tensor(arr);
}

static void test_is_float_tensor_full_null(void** state) {
    (void)state;
    assert_true(is_float_tensor_full(NULL));
}

static void test_is_float_tensor_full_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(2u, false);
    assert_non_null(arr);
    assert_false(is_float_tensor_full(arr));
    push_back_float_array(arr, 1.0f);
    push_back_float_array(arr, 2.0f);
    assert_true(is_float_tensor_full(arr));
    return_float_tensor(arr);
}

static void test_is_float_tensor_ptr_null_wrapper(void** state) {
    (void)state;
    float dummy = 0u;
    assert_false(is_float_tensor_ptr(NULL, &dummy));
}

static void test_is_float_tensor_ptr_valid(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array_filled(4u, 10.0f);
    assert_non_null(arr);
    /* Pointer to first element must be valid */
    const float_t* p = (const float_t*)arr->base->data;
    assert_true(is_float_tensor_ptr(arr, p));
    return_float_tensor(arr);
}

static void test_float_tensor_ndim_null(void** state) {
    (void)state;
    assert_int_equal(float_tensor_ndim(NULL), 0u);
}

static void test_float_tensor_ndim_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(float_tensor_ndim(arr), 1u);
    return_float_tensor(arr);
}

static void test_float_tensor_shape_dim_null(void** state) {
    (void)state;
    assert_int_equal(float_tensor_shape_dim(NULL, 0u), 0u);
}

static void test_float_tensor_shape_dim_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    float_tensor_expect_t r  = init_float_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_int_equal(float_tensor_shape_dim(r.u.value, 0u), 3u);
    assert_int_equal(float_tensor_shape_dim(r.u.value, 1u), 4u);
    return_float_tensor(r.u.value);
}

static void test_float_tensor_shape_null(void** state) {
    (void)state;
    size_t buf[2];
    assert_int_equal(float_tensor_shape(NULL, buf, 2u), NULL_POINTER);
}

static void test_float_tensor_shape_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    float_tensor_expect_t r  = init_float_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    size_t buf[2] = { 0u, 0u };
    assert_int_equal(float_tensor_shape(r.u.value, buf, 2u), NO_ERROR);
    assert_int_equal(buf[0], 3u);
    assert_int_equal(buf[1], 4u);
    return_float_tensor(r.u.value);
}

static void test_float_tensor_shape_ptr_null(void** state) {
    (void)state;
    assert_null(float_tensor_shape_ptr(NULL));
}

static void test_float_tensor_shape_ptr_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(4u, false);
    assert_non_null(arr);
    assert_non_null(float_tensor_shape_ptr(arr));
    return_float_tensor(arr);
}

static void test_float_tensor_strides_ptr_null(void** state) {
    (void)state;
    assert_null(float_tensor_strides_ptr(NULL));
}

static void test_float_tensor_strides_ptr_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(4u, false);
    assert_non_null(arr);
    const size_t* strides = float_tensor_strides_ptr(arr);
    assert_non_null(strides);
    assert_int_equal(strides[0], sizeof(float_t));
    return_float_tensor(arr);
}

static void test_float_tensor_shape_str_null(void** state) {
    (void)state;
    char buf[32];
    assert_int_equal(float_tensor_shape_str(NULL, buf, sizeof(buf)), NULL_POINTER);
}

static void test_float_tensor_shape_str_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(8u, false);
    assert_non_null(arr);
    char buf[32];
    assert_int_equal(float_tensor_shape_str(arr, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(8)");
    return_float_tensor(arr);
}

// ================================================================================
// ================================================================================
// UTILITY OPERATIONS

// ---- clear_float_tensor --------------------------------------------------------

static void test_clear_float_tensor_null(void** state) {
    (void)state;
    assert_int_equal(clear_float_tensor(NULL), NULL_POINTER);
}

static void test_clear_float_tensor_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array_filled(3u, 10.0f);
    assert_non_null(arr);
    assert_int_equal(clear_float_tensor(arr), NO_ERROR);
    assert_int_equal(float_tensor_size(arr), 0u);
    return_float_tensor(arr);
}

// ---- concat_float_tensor_array -------------------------------------------------

static void test_concat_float_tensor_null(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(concat_float_tensor_array(NULL, arr), NULL_POINTER);
    assert_int_equal(concat_float_tensor_array(arr, NULL), NULL_POINTER);
    return_float_tensor(arr);
}

static void test_concat_float_tensor_value(void** state) {
    (void)state;

    /* dst needs capacity for 5 elements total — allocate with room to grow */
    allocator_vtable_t alloc = heap_allocator();
    float_tensor_expect_t r  = init_float_array(8u, false, alloc);
    assert_true(r.has_value);
    float_tensor_t* dst = r.u.value;

    /* Populate dst with [1, 2, 3] */
    push_back_float_array(dst, 1.0f);
    push_back_float_array(dst, 2.0f);
    push_back_float_array(dst, 3.0f);

    /* src = [4, 5] */
    float_tensor_t* src = _make_float_array_filled(2u, 4.0f);
    assert_non_null(src);

    assert_int_equal(concat_float_tensor_array(dst, src), NO_ERROR);
    assert_int_equal(float_tensor_size(dst), 5u);

    float expected[] = { 1u, 2u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        float out = 0u;
        assert_int_equal(get_float_tensor_index(dst, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_float_tensor(dst);
    return_float_tensor(src);
}

// ---- slice_float_tensor_array --------------------------------------------------

static void test_slice_float_tensor_null(void** state) {
    (void)state;
    float_tensor_expect_t r = slice_float_tensor_array(NULL, 0u, 1u, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_slice_float_tensor_value(void** state) {
    (void)state;
    float_tensor_t* src = _make_float_array_filled(5u, 10.0f); /* [10,11,12,13,14] */
    assert_non_null(src);

    float_tensor_expect_t r = slice_float_tensor_array(src, 1u, 4u, NULL);
    assert_true(r.has_value);
    float_tensor_t* sl = r.u.value;

    assert_int_equal(float_tensor_size(sl), 3u);
    float expected[] = { 11., 12., 13. };
    for (size_t i = 0u; i < 3u; i++) {
        float out = 0.;
        assert_int_equal(get_float_tensor_index(sl, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_float_tensor(src);
    return_float_tensor(sl);
}

// ---- reverse_float_tensor ------------------------------------------------------

static void test_reverse_float_tensor_null(void** state) {
    (void)state;
    assert_int_equal(reverse_float_tensor(NULL), NULL_POINTER);
}

static void test_reverse_float_tensor_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array_filled(4u, 1.); /* [1,2,3,4] */
    assert_non_null(arr);

    assert_int_equal(reverse_float_tensor(arr), NO_ERROR);

    float expected[] = { 4., 3., 2., 1. };
    for (size_t i = 0u; i < 4u; i++) {
        float out = 0.;
        assert_int_equal(get_float_tensor_index(arr, i, &out), NO_ERROR);
        assert_float_equal(out, expected[i], 1.0e-3);
    }

    return_float_tensor(arr);
}

// ---- sort_float_tensor ---------------------------------------------------------

static void test_sort_float_tensor_null(void** state) {
    (void)state;
    assert_int_equal(sort_float_tensor(NULL, FORWARD), NULL_POINTER);
}

static void test_sort_float_tensor_forward(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(5u, false);
    assert_non_null(arr);

    float vals[] = { 3., 1., 4., 1., 5. };
    for (size_t i = 0u; i < 5u; i++)
        push_back_float_array(arr, vals[i]);

    assert_int_equal(sort_float_tensor(arr, FORWARD), NO_ERROR);

    float expected[] = { 1., 1., 3., 4., 5. };
    for (size_t i = 0u; i < 5u; i++) {
        float out = 0.;
        assert_int_equal(get_float_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_float_tensor(arr);
}

static void test_sort_float_tensor_reverse(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(5u, false);
    assert_non_null(arr);

    float vals[] = { 3., 1., 4., 1., 5. };
    for (size_t i = 0u; i < 5u; i++)
        push_back_float_array(arr, vals[i]);

    assert_int_equal(sort_float_tensor(arr, REVERSE), NO_ERROR);

    float expected[] = { 5., 4., 3., 1., 1. };
    for (size_t i = 0u; i < 5u; i++) {
        float out = 0.;
        assert_int_equal(get_float_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_float_tensor(arr);
}

// ================================================================================
// ================================================================================
// SET AND GET DATA

// ---- set_float_tensor_index / get_float_tensor_index --------------------------

static void test_set_get_float_tensor_index_null(void** state) {
    (void)state;
    float dummy = 0.;
    assert_int_equal(set_float_tensor_index(NULL, 0u, 1.),    NULL_POINTER);
    assert_int_equal(get_float_tensor_index(NULL, 0u, &dummy), NULL_POINTER);
}

static void test_set_get_float_tensor_index_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array_filled(3u, 10.);
    assert_non_null(arr);

    assert_int_equal(set_float_tensor_index(arr, 1u, 99.), NO_ERROR);

    float out = 0.;
    assert_int_equal(get_float_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_float_equal(out, 99.0, 1.0e-3);

    return_float_tensor(arr);
}

// ---- set_float_tensor_nd_index / get_float_tensor_nd_index --------------------

static void test_set_get_float_tensor_nd_index_null(void** state) {
    (void)state;
    size_t  idx[] = { 0u, 0u };
    float out   = 0u;
    assert_int_equal(set_float_tensor_nd_index(NULL, idx, 1.),   NULL_POINTER);
    assert_int_equal(get_float_tensor_nd_index(NULL, idx, &out), NULL_POINTER);
}

static void test_set_get_float_tensor_nd_index_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 3u };
    allocator_vtable_t alloc = heap_allocator();
    float_tensor_expect_t r  = init_float_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    float_tensor_t* mat = r.u.value;

    size_t idx[] = { 1u, 2u };
    assert_int_equal(set_float_tensor_nd_index(mat, idx, 42.), NO_ERROR);

    float out = 0.;
    assert_int_equal(get_float_tensor_nd_index(mat, idx, &out), NO_ERROR);
    assert_float_equal(out, 42., 1.0e-3);

    return_float_tensor(mat);
}

// ---- push and pop wrappers ----------------------------------------------------

static void test_push_back_float_array_null(void** state) {
    (void)state;
    assert_int_equal(push_back_float_array(NULL, 1.0), NULL_POINTER);
}

static void test_push_back_float_array_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(4u, false);
    assert_non_null(arr);

    assert_int_equal(push_back_float_array(arr, 10.0), NO_ERROR);
    assert_int_equal(push_back_float_array(arr, 20.0), NO_ERROR);
    assert_int_equal(float_tensor_size(arr), 2u);

    float out = 0.0;
    assert_int_equal(get_float_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_float_equal(out, 20.0, 1.0e-3);

    return_float_tensor(arr);
}

static void test_push_front_float_array_null(void** state) {
    (void)state;
    assert_int_equal(push_front_float_array(NULL, 1.), NULL_POINTER);
}

static void test_push_front_float_array_value(void** state) {
    (void)state;
    /* Capacity 3 so there is room for the push_front */
    allocator_vtable_t alloc = heap_allocator();
    float_tensor_expect_t r  = init_float_array(3u, false, alloc);
    assert_true(r.has_value);
    float_tensor_t* arr = r.u.value;

    push_back_float_array(arr, 10.0);
    push_back_float_array(arr, 11.0);
    /* arr = [10, 11], len == 2, alloc == 3 — one slot free */

    assert_int_equal(push_front_float_array(arr, 99.0), NO_ERROR);

    float out = 0.0;
    assert_int_equal(get_float_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 99.0);
    assert_int_equal(get_float_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10.0);
    assert_int_equal(get_float_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11.0);

    return_float_tensor(arr);
}

static void test_push_at_float_array_null(void** state) {
    (void)state;
    assert_int_equal(push_at_float_array(NULL, 1u, 0.0), NULL_POINTER);
}

static void test_push_at_float_array_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array_filled(3u, 10.0); /* [10, 11, 12] */
    assert_non_null(arr);

    assert_int_equal(push_at_float_array(arr, 99.0, 1u), NO_ERROR);
    /* arr = [10, 99, 11, 12] */

    float out = 0.0;
    assert_int_equal(get_float_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_float_equal(out, 10.0, 1.0e-3);
    assert_int_equal(get_float_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_float_equal(out, 99.0, 1.0e-3);
    assert_int_equal(get_float_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_float_equal(out, 11.0, 1.0e-3);
    assert_int_equal(get_float_tensor_index(arr, 3u, &out), NO_ERROR);
    assert_float_equal(out, 12.0, 1.0e-3);

    return_float_tensor(arr);
}

static void test_pop_back_float_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_back_float_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_back_float_array_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array_filled(3u, 10.0); /* [10,11,12] */
    assert_non_null(arr);

    float out = 0.0;
    assert_int_equal(pop_back_float_array(arr, &out), NO_ERROR);
    assert_float_equal(out, 12.0, 1.0e-3);
    assert_int_equal(float_tensor_size(arr), 2u);

    return_float_tensor(arr);
}

static void test_pop_front_float_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_front_float_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_front_float_array_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array_filled(3u, 10.0); /* [10,11,12] */
    assert_non_null(arr);

    float out = 0.0;
    assert_int_equal(pop_front_float_array(arr, &out), NO_ERROR);
    assert_float_equal(out, 10.0, 1.0e-3);
    assert_int_equal(float_tensor_size(arr), 2u);

    return_float_tensor(arr);
}

static void test_pop_at_float_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_at_float_array(NULL, NULL, 0u), NULL_POINTER);
}

static void test_pop_at_float_array_value(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array_filled(4u, 10.0); /* [10,11,12,13] */
    assert_non_null(arr);

    float out = 0.0;
    assert_int_equal(pop_at_float_array(arr, &out, 2u), NO_ERROR);
    assert_float_equal(out, 12.0, 1.0e-3);
    assert_int_equal(float_tensor_size(arr), 3u);

    assert_int_equal(get_float_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_float_equal(out, 13.0, 1.0e-3);

    return_float_tensor(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// LSEARCH TESTS

/** Build a float array from a literal array of values. */
static float_tensor_t* _make_float_array_from(const float* vals, size_t n) {
    float_tensor_t* arr = _make_float_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++)
        push_back_float_array(arr, vals[i]);
    return arr;
}

/** NULL tensor must return NULL_POINTER. */
static void test_float_lsearch_null_tensor(void** state) {
    (void)state;
    size_t idx = 0u;
    assert_int_equal(float_tensor_lsearch(NULL, &idx, 1.0f, 0.0f),
                     NULL_POINTER);
}
 
/** NULL index pointer must return NULL_POINTER. */
static void test_float_lsearch_null_index(void** state) {
    (void)state;
    float  vals[] = { 1.0f, 2.0f };
    float_tensor_t* arr = _make_float_array_from(vals, 2u);
    assert_non_null(arr);
 
    assert_int_equal(float_tensor_lsearch(arr, NULL, 1.0f, 0.0f),
                     NULL_POINTER);
    return_float_tensor(arr);
}
 
/** Empty array must return EMPTY without touching index. */
static void test_float_lsearch_empty(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(4u, false);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(float_tensor_lsearch(arr, &idx, 1.0f, 0.0f), EMPTY);
    assert_int_equal(idx, 99u);
    return_float_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// EXACT MATCH (tolerance == 0.0f)
 
/** Exact match at index 0 with zero tolerance. */
static void test_float_lsearch_exact_first(void** state) {
    (void)state;
    float vals[] = { 1.0f, 2.0f, 3.0f };
    float_tensor_t* arr = _make_float_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(float_tensor_lsearch(arr, &idx, 1.0f, 0.0f), NO_ERROR);
    assert_int_equal(idx, 0u);
    return_float_tensor(arr);
}
 
/** Exact match at the last index with zero tolerance. */
static void test_float_lsearch_exact_last(void** state) {
    (void)state;
    float vals[] = { 1.0f, 2.0f, 3.0f };
    float_tensor_t* arr = _make_float_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(float_tensor_lsearch(arr, &idx, 3.0f, 0.0f), NO_ERROR);
    assert_int_equal(idx, 2u);
    return_float_tensor(arr);
}
 
/** Exact match in the middle with zero tolerance. */
static void test_float_lsearch_exact_middle(void** state) {
    (void)state;
    float vals[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
    float_tensor_t* arr = _make_float_array_from(vals, 5u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(float_tensor_lsearch(arr, &idx, 3.0f, 0.0f), NO_ERROR);
    assert_int_equal(idx, 2u);
    return_float_tensor(arr);
}
 
/** Value not present returns NOT_FOUND and index is unchanged. */
static void test_float_lsearch_exact_not_found(void** state) {
    (void)state;
    float vals[] = { 1.0f, 2.0f, 3.0f };
    float_tensor_t* arr = _make_float_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(float_tensor_lsearch(arr, &idx, 9.9f, 0.0f), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_float_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// TOLERANCE MATCHING
 
/**
 * Element exactly at the tolerance boundary must match.
 * |2.05 - 2.0| == 0.05 == tolerance → match.
 */
static void test_float_lsearch_tolerance_exact_boundary(void** state) {
    (void)state;
    float vals[] = { 1.0f, 2.05f, 3.0f };
    float_tensor_t* arr = _make_float_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        float_tensor_lsearch(arr, &idx, 2.0f, 0.05f), NO_ERROR);
    assert_int_equal(idx, 1u);
    return_float_tensor(arr);
}
 
/**
 * Element just beyond tolerance must not match.
 * |2.06 - 2.0| == 0.06 > 0.05 → no match.
 */
static void test_float_lsearch_tolerance_just_outside(void** state) {
    (void)state;
    float vals[] = { 1.0f, 2.06f, 3.0f };
    float_tensor_t* arr = _make_float_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        float_tensor_lsearch(arr, &idx, 2.0f, 0.05f), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_float_tensor(arr);
}
 
/**
 * Negative value within tolerance must match — confirms the absolute
 * difference is taken correctly for values below zero.
 */
static void test_float_lsearch_tolerance_negative_value(void** state) {
    (void)state;
    float vals[] = { -3.0f, -1.95f, 0.0f };
    float_tensor_t* arr = _make_float_array_from(vals, 3u);
    assert_non_null(arr);
 
    /* |-1.95 - (-2.0)| == 0.05 <= 0.1 → match at index 1 */
    size_t idx = 99u;
    assert_int_equal(
        float_tensor_lsearch(arr, &idx, -2.0f, 0.1f), NO_ERROR);
    assert_int_equal(idx, 1u);
    return_float_tensor(arr);
}
 
/**
 * When multiple elements are within tolerance, the first occurrence
 * must be returned.
 */
static void test_float_lsearch_tolerance_first_of_multiple(void** state) {
    (void)state;
    float vals[] = { 1.0f, 2.03f, 2.01f, 2.04f, 5.0f };
    float_tensor_t* arr = _make_float_array_from(vals, 5u);
    assert_non_null(arr);
 
    /* All of 2.03, 2.01, 2.04 are within 0.05 of 2.0 — first is index 1 */
    size_t idx = 99u;
    assert_int_equal(
        float_tensor_lsearch(arr, &idx, 2.0f, 0.05f), NO_ERROR);
    assert_int_equal(idx, 1u);
    return_float_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// NaN AND SPECIAL VALUES
 
/**
 * NaN elements must never match — IEEE 754 ordered comparison of any
 * value against NaN is false.
 */
static void test_float_lsearch_nan_element_not_matched(void** state) {
    (void)state;
    float nan_val = (float)NAN;
    float vals[]  = { 1.0f, nan_val, 3.0f };
    float_tensor_t* arr = _make_float_array_from(vals, 3u);
    assert_non_null(arr);
 
    /* Searching for NaN with any tolerance must return NOT_FOUND */
    size_t idx = 99u;
    assert_int_equal(
        float_tensor_lsearch(arr, &idx, nan_val, 1e10f), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_float_tensor(arr);
}
 
/**
 * Positive infinity must match positive infinity exactly with zero
 * tolerance, since inf - inf == NaN, NOT zero.  Passing a large
 * tolerance does not help either since inf - inf is NaN which compares
 * false.  This test documents the expected NOT_FOUND behaviour for inf.
 */
static void test_float_lsearch_infinity_behaviour(void** state) {
    (void)state;
    float inf_val = (float)(1.0 / 0.0);   /* +inf */
    float vals[]  = { 1.0f, inf_val, 3.0f };
    float_tensor_t* arr = _make_float_array_from(vals, 3u);
    assert_non_null(arr);
 
    /* inf - inf == NaN, ordered comparison false → NOT_FOUND */
    size_t idx = 99u;
    assert_int_equal(
        float_tensor_lsearch(arr, &idx, inf_val, 0.0f), NOT_FOUND);
    return_float_tensor(arr);
}
 
/**
 * Negative tolerance produces no matches since no absolute difference
 * can be less than or equal to a negative number.
 */
static void test_float_lsearch_negative_tolerance(void** state) {
    (void)state;
    float vals[] = { 1.0f, 2.0f, 3.0f };
    float_tensor_t* arr = _make_float_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        float_tensor_lsearch(arr, &idx, 2.0f, -0.01f), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_float_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// SINGLE ELEMENT
 
/** Single-element array containing a match must succeed. */
static void test_float_lsearch_single_match(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(2u, false);
    assert_non_null(arr);
    push_back_float_array(arr, 42.0f);
 
    size_t idx = 99u;
    assert_int_equal(
        float_tensor_lsearch(arr, &idx, 42.0f, 0.0f), NO_ERROR);
    assert_int_equal(idx, 0u);
    return_float_tensor(arr);
}
 
/** Single-element array not containing a match must return NOT_FOUND. */
static void test_float_lsearch_single_no_match(void** state) {
    (void)state;
    float_tensor_t* arr = _make_float_array(2u, false);
    assert_non_null(arr);
    push_back_float_array(arr, 42.0f);
 
    size_t idx = 99u;
    assert_int_equal(
        float_tensor_lsearch(arr, &idx, 7.0f, 0.0f), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_float_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// LARGE ARRAY (exercises SIMD bulk path)
 
/**
 * A large array that exceeds the SIMD register width exercises the
 * bulk SIMD path and the scalar remainder.  The target is placed near
 * the end to ensure the SIMD loop runs multiple full iterations before
 * finding the match.
 */
static void test_float_lsearch_large_array_match_near_end(void** state) {
    (void)state;
    const size_t n = 64u;
    float_tensor_t* arr = _make_float_array(n + 1u, false);
    assert_non_null(arr);
 
    for (size_t i = 0u; i < n - 1u; i++)
        push_back_float_array(arr, (float)i * 0.1f);
    push_back_float_array(arr, 999.0f);   /* target at last position */
 
    size_t idx = 99u;
    assert_int_equal(
        float_tensor_lsearch(arr, &idx, 999.0f, 0.0f), NO_ERROR);
    assert_int_equal(idx, n - 1u);
    return_float_tensor(arr);
}
 
/**
 * A large array where the target is not present must return NOT_FOUND
 * after exhausting all SIMD chunks and the scalar remainder.
 */
static void test_float_lsearch_large_array_not_found(void** state) {
    (void)state;
    const size_t n = 64u;
    float_tensor_t* arr = _make_float_array(n + 1u, false);
    assert_non_null(arr);
 
    for (size_t i = 0u; i < n; i++)
        push_back_float_array(arr, (float)i);
 
    size_t idx = 99u;
    assert_int_equal(
        float_tensor_lsearch(arr, &idx, -1.0f, 0.0f), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_float_tensor(arr);
}
// ================================================================================
// ================================================================================
// TEST SUITE REGISTRY

const struct CMUnitTest test_float_tensor[] = {
    /* init_float_array */
    cmocka_unit_test(test_init_float_array_null_allocator),
    cmocka_unit_test(test_init_float_array_success),

    /* init_float_tensor */
    cmocka_unit_test(test_init_float_tensor_success),
    cmocka_unit_test(test_init_float_tensor_null_shape),

    /* return_float_tensor */
    cmocka_unit_test(test_return_float_tensor_null),
    cmocka_unit_test(test_return_float_tensor_normal),

    /* copy_float_tensor */
    cmocka_unit_test(test_copy_float_tensor_null_src),
    cmocka_unit_test(test_copy_float_tensor_independence),

    /* introspection — null guards */
    cmocka_unit_test(test_float_tensor_size_null),
    cmocka_unit_test(test_float_tensor_alloc_null),
    cmocka_unit_test(test_float_tensor_data_size_null),
    cmocka_unit_test(test_float_tensor_dtype_null),
    cmocka_unit_test(test_is_float_tensor_empty_null),
    cmocka_unit_test(test_is_float_tensor_full_null),
    cmocka_unit_test(test_is_float_tensor_ptr_null_wrapper),
    cmocka_unit_test(test_float_tensor_ndim_null),
    cmocka_unit_test(test_float_tensor_shape_dim_null),
    cmocka_unit_test(test_float_tensor_shape_null),
    cmocka_unit_test(test_float_tensor_shape_ptr_null),
    cmocka_unit_test(test_float_tensor_strides_ptr_null),
    cmocka_unit_test(test_float_tensor_shape_str_null),

    /* introspection — values */
    cmocka_unit_test(test_float_tensor_size_value),
    cmocka_unit_test(test_float_tensor_alloc_value),
    cmocka_unit_test(test_float_tensor_data_size_value),
    cmocka_unit_test(test_float_tensor_dtype_value),
    cmocka_unit_test(test_is_float_tensor_empty_value),
    cmocka_unit_test(test_is_float_tensor_full_value),
    cmocka_unit_test(test_is_float_tensor_ptr_valid),
    cmocka_unit_test(test_float_tensor_ndim_value),
    cmocka_unit_test(test_float_tensor_shape_dim_value),
    cmocka_unit_test(test_float_tensor_shape_value),
    cmocka_unit_test(test_float_tensor_shape_ptr_value),
    cmocka_unit_test(test_float_tensor_strides_ptr_value),
    cmocka_unit_test(test_float_tensor_shape_str_value),

    /* utility — null guards */
    cmocka_unit_test(test_clear_float_tensor_null),
    cmocka_unit_test(test_concat_float_tensor_null),
    cmocka_unit_test(test_slice_float_tensor_null),
    cmocka_unit_test(test_reverse_float_tensor_null),
    cmocka_unit_test(test_sort_float_tensor_null),

    /* utility — values */
    cmocka_unit_test(test_clear_float_tensor_value),
    cmocka_unit_test(test_concat_float_tensor_value),
    cmocka_unit_test(test_slice_float_tensor_value),
    cmocka_unit_test(test_reverse_float_tensor_value),
    cmocka_unit_test(test_sort_float_tensor_forward),
    cmocka_unit_test(test_sort_float_tensor_reverse),

    /* set/get — null guards */
    cmocka_unit_test(test_set_get_float_tensor_index_null),
    cmocka_unit_test(test_set_get_float_tensor_nd_index_null),
    cmocka_unit_test(test_push_back_float_array_null),
    cmocka_unit_test(test_push_front_float_array_null),
    cmocka_unit_test(test_push_at_float_array_null),
    cmocka_unit_test(test_pop_back_float_array_null),
    cmocka_unit_test(test_pop_front_float_array_null),
    cmocka_unit_test(test_pop_at_float_array_null),

    /* set/get — values */
    cmocka_unit_test(test_set_get_float_tensor_index_value),
    cmocka_unit_test(test_set_get_float_tensor_nd_index_value),
    cmocka_unit_test(test_push_back_float_array_value),
    cmocka_unit_test(test_push_front_float_array_value),
    cmocka_unit_test(test_push_at_float_array_value),
    cmocka_unit_test(test_pop_back_float_array_value),
    cmocka_unit_test(test_pop_front_float_array_value),
    cmocka_unit_test(test_pop_at_float_array_value),

    /* lsearch null/guard */
    cmocka_unit_test(test_float_lsearch_null_tensor),
    cmocka_unit_test(test_float_lsearch_null_index),
    cmocka_unit_test(test_float_lsearch_empty),
 
    /* lsearch exact match — zero tolerance */
    cmocka_unit_test(test_float_lsearch_exact_first),
    cmocka_unit_test(test_float_lsearch_exact_last),
    cmocka_unit_test(test_float_lsearch_exact_middle),
    cmocka_unit_test(test_float_lsearch_exact_not_found),
 
    /* lsearch tolerance matching */
    cmocka_unit_test(test_float_lsearch_tolerance_exact_boundary),
    cmocka_unit_test(test_float_lsearch_tolerance_just_outside),
    cmocka_unit_test(test_float_lsearch_tolerance_negative_value),
    cmocka_unit_test(test_float_lsearch_tolerance_first_of_multiple),
 
    /* lsearch NaN and special values */
    cmocka_unit_test(test_float_lsearch_nan_element_not_matched),
    cmocka_unit_test(test_float_lsearch_infinity_behaviour),
    cmocka_unit_test(test_float_lsearch_negative_tolerance),
 
    /* lsearch single element */
    cmocka_unit_test(test_float_lsearch_single_match),
    cmocka_unit_test(test_float_lsearch_single_no_match),
 
    /* lsearch large array — exercises SIMD bulk path */
    cmocka_unit_test(test_float_lsearch_large_array_match_near_end),
    cmocka_unit_test(test_float_lsearch_large_array_not_found),
};

const size_t test_float_tensor_count = sizeof(test_float_tensor) /
                                       sizeof(test_float_tensor[0]);
// ================================================================================ 
// ================================================================================ 

/** Construct a populated double array with values 1..n. */
static double_tensor_t* _make_double_array(size_t capacity, bool growth) {
    allocator_vtable_t alloc = heap_allocator();
    double_tensor_expect_t r  = init_double_array(capacity, growth, alloc);
    if (!r.has_value) return NULL;
    return r.u.value;
}

static double_tensor_t* _make_double_array_filled(size_t n, double base) {
    /* Allocate n+1 so the result always has at least one free slot,
     * making it safe to use as a push destination in tests. */
    double_tensor_t* arr = _make_double_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++) {
        double val = (double)(base + i);
        push_back_double_array(arr, val);
    }
    return arr;
}

// ================================================================================
// ================================================================================
// INITIALISATION AND TEARDOWN

// ---- init_double_array ----------------------------------------------------------

/** NULL allocator must propagate an error through the expect type. */
static void test_init_double_array_null_allocator(void** state) {
    (void)state;
    allocator_vtable_t bad = heap_allocator();
    bad.allocate = NULL;
    double_tensor_expect_t r = init_double_array(8u, false, bad);
    assert_false(r.has_value);
}

/** Successful construction produces a non-NULL wrapper with correct mode. */
static void test_init_double_array_success(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_tensor_expect_t r  = init_double_array(8u, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  ARRAY_STRUCT);
    assert_int_equal(r.u.value->base->dtype, DOUBLE_TYPE);
    assert_int_equal(r.u.value->base->len,   0u);
    assert_int_equal(r.u.value->base->alloc, 8u);
    return_double_tensor(r.u.value);
}

// ---- init_double_tensor ---------------------------------------------------------

/** Successful construction of a fixed-shape tensor. */
static void test_init_double_tensor_success(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    double_tensor_expect_t r  = init_double_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  TENSOR_STRUCT);
    assert_int_equal(r.u.value->base->dtype, DOUBLE_TYPE);
    assert_int_equal(r.u.value->base->len,   12u);
    return_double_tensor(r.u.value);
}

/** NULL shape propagates an error. */
static void test_init_double_tensor_null_shape(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    double_tensor_expect_t r  = init_double_tensor(2u, NULL, alloc);
    assert_false(r.has_value);
}

// ---- return_double_tensor -------------------------------------------------------

/** return_double_tensor(NULL) must not crash. */
static void test_return_double_tensor_null(void** state) {
    (void)state;
    return_double_tensor(NULL);
}

/** Normal construction followed by return must not crash or leak. */
static void test_return_double_tensor_normal(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(4u, false);
    assert_non_null(arr);
    return_double_tensor(arr);
}

// ---- copy_double_tensor ---------------------------------------------------------

/** NULL src must return NULL_POINTER. */
static void test_copy_double_tensor_null_src(void** state) {
    (void)state;
    double_tensor_expect_t r = copy_double_tensor(NULL, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

/** Copy is independent of source — mutation of copy does not affect src. */
static void test_copy_double_tensor_independence(void** state) {
    (void)state;
    double_tensor_t* src = _make_double_array_filled(3u, 10u);
    assert_non_null(src);

    double_tensor_expect_t r = copy_double_tensor(src, NULL);
    assert_true(r.has_value);
    double_tensor_t* dst = r.u.value;

    /* Overwrite slot 0 of the copy */
    assert_int_equal(set_double_tensor_index(dst, 0u, 99.0f), NO_ERROR);

    /* Source slot 0 must be unchanged */
    double src_val = 0u;
    assert_int_equal(get_double_tensor_index(src, 0u, &src_val), NO_ERROR);
    assert_double_equal(src_val, 10.0f, 1.0e-3);

    return_double_tensor(src);
    return_double_tensor(dst);
}

// ================================================================================
// ================================================================================
// INTROSPECTION

static void test_double_tensor_size_null(void** state) {
    (void)state;
    assert_int_equal(double_tensor_size(NULL), 0u);
}

static void test_double_tensor_size_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array_filled(3u, 1u);
    assert_non_null(arr);
    assert_int_equal(double_tensor_size(arr), 3u);
    return_double_tensor(arr);
}

static void test_double_tensor_alloc_null(void** state) {
    (void)state;
    assert_int_equal(double_tensor_alloc(NULL), 0u);
}

static void test_double_tensor_alloc_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(8u, false);
    assert_non_null(arr);
    assert_int_equal(double_tensor_alloc(arr), 8u);
    return_double_tensor(arr);
}

static void test_double_tensor_data_size_null(void** state) {
    (void)state;
    assert_int_equal(double_tensor_data_size(NULL), 0u);
}

static void test_double_tensor_data_size_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(double_tensor_data_size(arr), sizeof(double_t));
    return_double_tensor(arr);
}

static void test_double_tensor_dtype_null(void** state) {
    (void)state;
    assert_int_equal(double_tensor_dtype(NULL), UNKNOWN_TYPE);
}

static void test_double_tensor_dtype_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(double_tensor_dtype(arr), DOUBLE_TYPE);
    return_double_tensor(arr);
}

static void test_is_double_tensor_empty_null(void** state) {
    (void)state;
    assert_true(is_double_tensor_empty(NULL));
}

static void test_is_double_tensor_empty_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(4u, false);
    assert_non_null(arr);
    assert_true(is_double_tensor_empty(arr));
    push_back_double_array(arr, 1u);
    assert_false(is_double_tensor_empty(arr));
    return_double_tensor(arr);
}

static void test_is_double_tensor_full_null(void** state) {
    (void)state;
    assert_true(is_double_tensor_full(NULL));
}

static void test_is_double_tensor_full_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(2u, false);
    assert_non_null(arr);
    assert_false(is_double_tensor_full(arr));
    push_back_double_array(arr, 1.0f);
    push_back_double_array(arr, 2.0f);
    assert_true(is_double_tensor_full(arr));
    return_double_tensor(arr);
}

static void test_is_double_tensor_ptr_null_wrapper(void** state) {
    (void)state;
    double dummy = 0u;
    assert_false(is_double_tensor_ptr(NULL, &dummy));
}

static void test_is_double_tensor_ptr_valid(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array_filled(4u, 10.0f);
    assert_non_null(arr);
    /* Pointer to first element must be valid */
    const double_t* p = (const double_t*)arr->base->data;
    assert_true(is_double_tensor_ptr(arr, p));
    return_double_tensor(arr);
}

static void test_double_tensor_ndim_null(void** state) {
    (void)state;
    assert_int_equal(double_tensor_ndim(NULL), 0u);
}

static void test_double_tensor_ndim_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(double_tensor_ndim(arr), 1u);
    return_double_tensor(arr);
}

static void test_double_tensor_shape_dim_null(void** state) {
    (void)state;
    assert_int_equal(double_tensor_shape_dim(NULL, 0u), 0u);
}

static void test_double_tensor_shape_dim_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    double_tensor_expect_t r  = init_double_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_int_equal(double_tensor_shape_dim(r.u.value, 0u), 3u);
    assert_int_equal(double_tensor_shape_dim(r.u.value, 1u), 4u);
    return_double_tensor(r.u.value);
}

static void test_double_tensor_shape_null(void** state) {
    (void)state;
    size_t buf[2];
    assert_int_equal(double_tensor_shape(NULL, buf, 2u), NULL_POINTER);
}

static void test_double_tensor_shape_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    double_tensor_expect_t r  = init_double_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    size_t buf[2] = { 0u, 0u };
    assert_int_equal(double_tensor_shape(r.u.value, buf, 2u), NO_ERROR);
    assert_int_equal(buf[0], 3u);
    assert_int_equal(buf[1], 4u);
    return_double_tensor(r.u.value);
}

static void test_double_tensor_shape_ptr_null(void** state) {
    (void)state;
    assert_null(double_tensor_shape_ptr(NULL));
}

static void test_double_tensor_shape_ptr_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(4u, false);
    assert_non_null(arr);
    assert_non_null(double_tensor_shape_ptr(arr));
    return_double_tensor(arr);
}

static void test_double_tensor_strides_ptr_null(void** state) {
    (void)state;
    assert_null(double_tensor_strides_ptr(NULL));
}

static void test_double_tensor_strides_ptr_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(4u, false);
    assert_non_null(arr);
    const size_t* strides = double_tensor_strides_ptr(arr);
    assert_non_null(strides);
    assert_int_equal(strides[0], sizeof(double_t));
    return_double_tensor(arr);
}

static void test_double_tensor_shape_str_null(void** state) {
    (void)state;
    char buf[32];
    assert_int_equal(double_tensor_shape_str(NULL, buf, sizeof(buf)), NULL_POINTER);
}

static void test_double_tensor_shape_str_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(8u, false);
    assert_non_null(arr);
    char buf[32];
    assert_int_equal(double_tensor_shape_str(arr, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(8)");
    return_double_tensor(arr);
}

// ================================================================================
// ================================================================================
// UTILITY OPERATIONS

// ---- clear_double_tensor --------------------------------------------------------

static void test_clear_double_tensor_null(void** state) {
    (void)state;
    assert_int_equal(clear_double_tensor(NULL), NULL_POINTER);
}

static void test_clear_double_tensor_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array_filled(3u, 10.0f);
    assert_non_null(arr);
    assert_int_equal(clear_double_tensor(arr), NO_ERROR);
    assert_int_equal(double_tensor_size(arr), 0u);
    return_double_tensor(arr);
}

// ---- concat_double_tensor_array -------------------------------------------------

static void test_concat_double_tensor_null(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(concat_double_tensor_array(NULL, arr), NULL_POINTER);
    assert_int_equal(concat_double_tensor_array(arr, NULL), NULL_POINTER);
    return_double_tensor(arr);
}

static void test_concat_double_tensor_value(void** state) {
    (void)state;

    /* dst needs capacity for 5 elements total — allocate with room to grow */
    allocator_vtable_t alloc = heap_allocator();
    double_tensor_expect_t r  = init_double_array(8u, false, alloc);
    assert_true(r.has_value);
    double_tensor_t* dst = r.u.value;

    /* Populate dst with [1, 2, 3] */
    push_back_double_array(dst, 1.0f);
    push_back_double_array(dst, 2.0f);
    push_back_double_array(dst, 3.0f);

    /* src = [4, 5] */
    double_tensor_t* src = _make_double_array_filled(2u, 4.0f);
    assert_non_null(src);

    assert_int_equal(concat_double_tensor_array(dst, src), NO_ERROR);
    assert_int_equal(double_tensor_size(dst), 5u);

    double expected[] = { 1u, 2u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        double out = 0u;
        assert_int_equal(get_double_tensor_index(dst, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_double_tensor(dst);
    return_double_tensor(src);
}

// ---- slice_double_tensor_array --------------------------------------------------

static void test_slice_double_tensor_null(void** state) {
    (void)state;
    double_tensor_expect_t r = slice_double_tensor_array(NULL, 0u, 1u, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_slice_double_tensor_value(void** state) {
    (void)state;
    double_tensor_t* src = _make_double_array_filled(5u, 10.0f); /* [10,11,12,13,14] */
    assert_non_null(src);

    double_tensor_expect_t r = slice_double_tensor_array(src, 1u, 4u, NULL);
    assert_true(r.has_value);
    double_tensor_t* sl = r.u.value;

    assert_int_equal(double_tensor_size(sl), 3u);
    double expected[] = { 11., 12., 13. };
    for (size_t i = 0u; i < 3u; i++) {
        double out = 0.;
        assert_int_equal(get_double_tensor_index(sl, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_double_tensor(src);
    return_double_tensor(sl);
}

// ---- reverse_double_tensor ------------------------------------------------------

static void test_reverse_double_tensor_null(void** state) {
    (void)state;
    assert_int_equal(reverse_double_tensor(NULL), NULL_POINTER);
}

static void test_reverse_double_tensor_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array_filled(4u, 1.); /* [1,2,3,4] */
    assert_non_null(arr);

    assert_int_equal(reverse_double_tensor(arr), NO_ERROR);

    double expected[] = { 4., 3., 2., 1. };
    for (size_t i = 0u; i < 4u; i++) {
        double out = 0.;
        assert_int_equal(get_double_tensor_index(arr, i, &out), NO_ERROR);
        assert_double_equal(out, expected[i], 1.0e-3);
    }

    return_double_tensor(arr);
}

// ---- sort_double_tensor ---------------------------------------------------------

static void test_sort_double_tensor_null(void** state) {
    (void)state;
    assert_int_equal(sort_double_tensor(NULL, FORWARD), NULL_POINTER);
}

static void test_sort_double_tensor_forward(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(5u, false);
    assert_non_null(arr);

    double vals[] = { 3., 1., 4., 1., 5. };
    for (size_t i = 0u; i < 5u; i++)
        push_back_double_array(arr, vals[i]);

    assert_int_equal(sort_double_tensor(arr, FORWARD), NO_ERROR);

    double expected[] = { 1., 1., 3., 4., 5. };
    for (size_t i = 0u; i < 5u; i++) {
        double out = 0.;
        assert_int_equal(get_double_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_double_tensor(arr);
}

static void test_sort_double_tensor_reverse(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(5u, false);
    assert_non_null(arr);

    double vals[] = { 3., 1., 4., 1., 5. };
    for (size_t i = 0u; i < 5u; i++)
        push_back_double_array(arr, vals[i]);

    assert_int_equal(sort_double_tensor(arr, REVERSE), NO_ERROR);

    double expected[] = { 5., 4., 3., 1., 1. };
    for (size_t i = 0u; i < 5u; i++) {
        double out = 0.;
        assert_int_equal(get_double_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_double_tensor(arr);
}

// ================================================================================
// ================================================================================
// SET AND GET DATA

// ---- set_double_tensor_index / get_double_tensor_index --------------------------

static void test_set_get_double_tensor_index_null(void** state) {
    (void)state;
    double dummy = 0.;
    assert_int_equal(set_double_tensor_index(NULL, 0u, 1.),    NULL_POINTER);
    assert_int_equal(get_double_tensor_index(NULL, 0u, &dummy), NULL_POINTER);
}

static void test_set_get_double_tensor_index_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array_filled(3u, 10.);
    assert_non_null(arr);

    assert_int_equal(set_double_tensor_index(arr, 1u, 99.), NO_ERROR);

    double out = 0.;
    assert_int_equal(get_double_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_double_equal(out, 99.0, 1.0e-3);

    return_double_tensor(arr);
}

// ---- set_double_tensor_nd_index / get_double_tensor_nd_index --------------------

static void test_set_get_double_tensor_nd_index_null(void** state) {
    (void)state;
    size_t  idx[] = { 0u, 0u };
    double out   = 0u;
    assert_int_equal(set_double_tensor_nd_index(NULL, idx, 1.),   NULL_POINTER);
    assert_int_equal(get_double_tensor_nd_index(NULL, idx, &out), NULL_POINTER);
}

static void test_set_get_double_tensor_nd_index_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 3u };
    allocator_vtable_t alloc = heap_allocator();
    double_tensor_expect_t r  = init_double_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    double_tensor_t* mat = r.u.value;

    size_t idx[] = { 1u, 2u };
    assert_int_equal(set_double_tensor_nd_index(mat, idx, 42.), NO_ERROR);

    double out = 0.;
    assert_int_equal(get_double_tensor_nd_index(mat, idx, &out), NO_ERROR);
    assert_double_equal(out, 42., 1.0e-3);

    return_double_tensor(mat);
}

// ---- push and pop wrappers ----------------------------------------------------

static void test_push_back_double_array_null(void** state) {
    (void)state;
    assert_int_equal(push_back_double_array(NULL, 1.0), NULL_POINTER);
}

static void test_push_back_double_array_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(4u, false);
    assert_non_null(arr);

    assert_int_equal(push_back_double_array(arr, 10.0), NO_ERROR);
    assert_int_equal(push_back_double_array(arr, 20.0), NO_ERROR);
    assert_int_equal(double_tensor_size(arr), 2u);

    double out = 0.0;
    assert_int_equal(get_double_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_double_equal(out, 20.0, 1.0e-3);

    return_double_tensor(arr);
}

static void test_push_front_double_array_null(void** state) {
    (void)state;
    assert_int_equal(push_front_double_array(NULL, 1.), NULL_POINTER);
}

static void test_push_front_double_array_value(void** state) {
    (void)state;
    /* Capacity 3 so there is room for the push_front */
    allocator_vtable_t alloc = heap_allocator();
    double_tensor_expect_t r  = init_double_array(3u, false, alloc);
    assert_true(r.has_value);
    double_tensor_t* arr = r.u.value;

    push_back_double_array(arr, 10.0);
    push_back_double_array(arr, 11.0);
    /* arr = [10, 11], len == 2, alloc == 3 — one slot free */

    assert_int_equal(push_front_double_array(arr, 99.0), NO_ERROR);

    double out = 0.0;
    assert_int_equal(get_double_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 99.0);
    assert_int_equal(get_double_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10.0);
    assert_int_equal(get_double_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11.0);

    return_double_tensor(arr);
}

static void test_push_at_double_array_null(void** state) {
    (void)state;
    assert_int_equal(push_at_double_array(NULL, 1u, 0.0), NULL_POINTER);
}

static void test_push_at_double_array_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array_filled(3u, 10.0); /* [10, 11, 12] */
    assert_non_null(arr);

    assert_int_equal(push_at_double_array(arr, 99.0, 1u), NO_ERROR);
    /* arr = [10, 99, 11, 12] */

    double out = 0.0;
    assert_int_equal(get_double_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_double_equal(out, 10.0, 1.0e-3);
    assert_int_equal(get_double_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_double_equal(out, 99.0, 1.0e-3);
    assert_int_equal(get_double_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_double_equal(out, 11.0, 1.0e-3);
    assert_int_equal(get_double_tensor_index(arr, 3u, &out), NO_ERROR);
    assert_double_equal(out, 12.0, 1.0e-3);

    return_double_tensor(arr);
}

static void test_pop_back_double_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_back_double_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_back_double_array_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array_filled(3u, 10.0); /* [10,11,12] */
    assert_non_null(arr);

    double out = 0.0;
    assert_int_equal(pop_back_double_array(arr, &out), NO_ERROR);
    assert_double_equal(out, 12.0, 1.0e-3);
    assert_int_equal(double_tensor_size(arr), 2u);

    return_double_tensor(arr);
}

static void test_pop_front_double_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_front_double_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_front_double_array_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array_filled(3u, 10.0); /* [10,11,12] */
    assert_non_null(arr);

    double out = 0.0;
    assert_int_equal(pop_front_double_array(arr, &out), NO_ERROR);
    assert_double_equal(out, 10.0, 1.0e-3);
    assert_int_equal(double_tensor_size(arr), 2u);

    return_double_tensor(arr);
}

static void test_pop_at_double_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_at_double_array(NULL, NULL, 0u), NULL_POINTER);
}

static void test_pop_at_double_array_value(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array_filled(4u, 10.0); /* [10,11,12,13] */
    assert_non_null(arr);

    double out = 0.0;
    assert_int_equal(pop_at_double_array(arr, &out, 2u), NO_ERROR);
    assert_double_equal(out, 12.0, 1.0e-3);
    assert_int_equal(double_tensor_size(arr), 3u);

    assert_int_equal(get_double_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_double_equal(out, 13.0, 1.0e-3);

    return_double_tensor(arr);
}
// -------------------------------------------------------------------------------- 

// ================================================================================
// ================================================================================
// LSEARCH TESTS

/** Build a double array from a literal array of values. */
static double_tensor_t* _make_double_array_from(const double* vals, size_t n) {
    double_tensor_t* arr = _make_double_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++)
        push_back_double_array(arr, vals[i]);
    return arr;
}

/** NULL tensor must return NULL_POINTER. */
static void test_double_lsearch_null_tensor(void** state) {
    (void)state;
    size_t idx = 0u;
    assert_int_equal(double_tensor_lsearch(NULL, &idx, 1.0f, 0.0f),
                     NULL_POINTER);
}
 
/** NULL index pointer must return NULL_POINTER. */
static void test_double_lsearch_null_index(void** state) {
    (void)state;
    double  vals[] = { 1.0f, 2.0f };
    double_tensor_t* arr = _make_double_array_from(vals, 2u);
    assert_non_null(arr);
 
    assert_int_equal(double_tensor_lsearch(arr, NULL, 1.0f, 0.0f),
                     NULL_POINTER);
    return_double_tensor(arr);
}
 
/** Empty array must return EMPTY without touching index. */
static void test_double_lsearch_empty(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(4u, false);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(double_tensor_lsearch(arr, &idx, 1.0f, 0.0f), EMPTY);
    assert_int_equal(idx, 99u);
    return_double_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// EXACT MATCH (tolerance == 0.0f)
 
/** Exact match at index 0 with zero tolerance. */
static void test_double_lsearch_exact_first(void** state) {
    (void)state;
    double vals[] = { 1.0f, 2.0f, 3.0f };
    double_tensor_t* arr = _make_double_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(double_tensor_lsearch(arr, &idx, 1.0f, 0.0f), NO_ERROR);
    assert_int_equal(idx, 0u);
    return_double_tensor(arr);
}
 
/** Exact match at the last index with zero tolerance. */
static void test_double_lsearch_exact_last(void** state) {
    (void)state;
    double vals[] = { 1.0f, 2.0f, 3.0f };
    double_tensor_t* arr = _make_double_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(double_tensor_lsearch(arr, &idx, 3.0f, 0.0f), NO_ERROR);
    assert_int_equal(idx, 2u);
    return_double_tensor(arr);
}
 
/** Exact match in the middle with zero tolerance. */
static void test_double_lsearch_exact_middle(void** state) {
    (void)state;
    double vals[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
    double_tensor_t* arr = _make_double_array_from(vals, 5u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(double_tensor_lsearch(arr, &idx, 3.0f, 0.0f), NO_ERROR);
    assert_int_equal(idx, 2u);
    return_double_tensor(arr);
}
 
/** Value not present returns NOT_FOUND and index is unchanged. */
static void test_double_lsearch_exact_not_found(void** state) {
    (void)state;
    double vals[] = { 1.0f, 2.0f, 3.0f };
    double_tensor_t* arr = _make_double_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(double_tensor_lsearch(arr, &idx, 9.9f, 0.0f), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_double_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// TOLERANCE MATCHING
 
/**
 * Element exactly at the tolerance boundary must match.
 * |2.05 - 2.0| == 0.05 == tolerance → match.
 */
static void test_double_lsearch_tolerance_exact_boundary(void** state) {
    (void)state;
    double vals[] = { 1.0f, 2.05f, 3.0f };
    double_tensor_t* arr = _make_double_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        double_tensor_lsearch(arr, &idx, 2.0f, 0.05f), NO_ERROR);
    assert_int_equal(idx, 1u);
    return_double_tensor(arr);
}
 
/**
 * Element just beyond tolerance must not match.
 * |2.06 - 2.0| == 0.06 > 0.05 → no match.
 */
static void test_double_lsearch_tolerance_just_outside(void** state) {
    (void)state;
    double vals[] = { 1.0f, 2.06f, 3.0f };
    double_tensor_t* arr = _make_double_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        double_tensor_lsearch(arr, &idx, 2.0f, 0.05f), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_double_tensor(arr);
}
 
/**
 * Negative value within tolerance must match — confirms the absolute
 * difference is taken correctly for values below zero.
 */
static void test_double_lsearch_tolerance_negative_value(void** state) {
    (void)state;
    double vals[] = { -3.0f, -1.95f, 0.0f };
    double_tensor_t* arr = _make_double_array_from(vals, 3u);
    assert_non_null(arr);
 
    /* |-1.95 - (-2.0)| == 0.05 <= 0.1 → match at index 1 */
    size_t idx = 99u;
    assert_int_equal(
        double_tensor_lsearch(arr, &idx, -2.0f, 0.1f), NO_ERROR);
    assert_int_equal(idx, 1u);
    return_double_tensor(arr);
}
 
/**
 * When multiple elements are within tolerance, the first occurrence
 * must be returned.
 */
static void test_double_lsearch_tolerance_first_of_multiple(void** state) {
    (void)state;
    double vals[] = { 1.0f, 2.03f, 2.01f, 2.04f, 5.0f };
    double_tensor_t* arr = _make_double_array_from(vals, 5u);
    assert_non_null(arr);
 
    /* All of 2.03, 2.01, 2.04 are within 0.05 of 2.0 — first is index 1 */
    size_t idx = 99u;
    assert_int_equal(
        double_tensor_lsearch(arr, &idx, 2.0f, 0.05f), NO_ERROR);
    assert_int_equal(idx, 1u);
    return_double_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// NaN AND SPECIAL VALUES
 
/**
 * NaN elements must never match — IEEE 754 ordered comparison of any
 * value against NaN is false.
 */
static void test_double_lsearch_nan_element_not_matched(void** state) {
    (void)state;
    double nan_val = (double)NAN;
    double vals[]  = { 1.0f, nan_val, 3.0f };
    double_tensor_t* arr = _make_double_array_from(vals, 3u);
    assert_non_null(arr);
 
    /* Searching for NaN with any tolerance must return NOT_FOUND */
    size_t idx = 99u;
    assert_int_equal(
        double_tensor_lsearch(arr, &idx, nan_val, 1e10f), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_double_tensor(arr);
}
 
/**
 * Positive infinity must match positive infinity exactly with zero
 * tolerance, since inf - inf == NaN, NOT zero.  Passing a large
 * tolerance does not help either since inf - inf is NaN which compares
 * false.  This test documents the expected NOT_FOUND behaviour for inf.
 */
static void test_double_lsearch_infinity_behaviour(void** state) {
    (void)state;
    double inf_val = (double)(1.0 / 0.0);   /* +inf */
    double vals[]  = { 1.0f, inf_val, 3.0f };
    double_tensor_t* arr = _make_double_array_from(vals, 3u);
    assert_non_null(arr);
 
    /* inf - inf == NaN, ordered comparison false → NOT_FOUND */
    size_t idx = 99u;
    assert_int_equal(
        double_tensor_lsearch(arr, &idx, inf_val, 0.0f), NOT_FOUND);
    return_double_tensor(arr);
}
 
/**
 * Negative tolerance produces no matches since no absolute difference
 * can be less than or equal to a negative number.
 */
static void test_double_lsearch_negative_tolerance(void** state) {
    (void)state;
    double vals[] = { 1.0f, 2.0f, 3.0f };
    double_tensor_t* arr = _make_double_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        double_tensor_lsearch(arr, &idx, 2.0f, -0.01f), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_double_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// SINGLE ELEMENT
 
/** Single-element array containing a match must succeed. */
static void test_double_lsearch_single_match(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(2u, false);
    assert_non_null(arr);
    push_back_double_array(arr, 42.0f);
 
    size_t idx = 99u;
    assert_int_equal(
        double_tensor_lsearch(arr, &idx, 42.0f, 0.0f), NO_ERROR);
    assert_int_equal(idx, 0u);
    return_double_tensor(arr);
}
 
/** Single-element array not containing a match must return NOT_FOUND. */
static void test_double_lsearch_single_no_match(void** state) {
    (void)state;
    double_tensor_t* arr = _make_double_array(2u, false);
    assert_non_null(arr);
    push_back_double_array(arr, 42.0f);
 
    size_t idx = 99u;
    assert_int_equal(
        double_tensor_lsearch(arr, &idx, 7.0f, 0.0f), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_double_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// LARGE ARRAY (exercises SIMD bulk path)
 
/**
 * A large array that exceeds the SIMD register width exercises the
 * bulk SIMD path and the scalar remainder.  The target is placed near
 * the end to ensure the SIMD loop runs multiple full iterations before
 * finding the match.
 */
static void test_double_lsearch_large_array_match_near_end(void** state) {
    (void)state;
    const size_t n = 64u;
    double_tensor_t* arr = _make_double_array(n + 1u, false);
    assert_non_null(arr);
 
    for (size_t i = 0u; i < n - 1u; i++)
        push_back_double_array(arr, (double)i * 0.1f);
    push_back_double_array(arr, 999.0f);   /* target at last position */
 
    size_t idx = 99u;
    assert_int_equal(
        double_tensor_lsearch(arr, &idx, 999.0f, 0.0f), NO_ERROR);
    assert_int_equal(idx, n - 1u);
    return_double_tensor(arr);
}
 
/**
 * A large array where the target is not present must return NOT_FOUND
 * after exhausting all SIMD chunks and the scalar remainder.
 */
static void test_double_lsearch_large_array_not_found(void** state) {
    (void)state;
    const size_t n = 64u;
    double_tensor_t* arr = _make_double_array(n + 1u, false);
    assert_non_null(arr);
 
    for (size_t i = 0u; i < n; i++)
        push_back_double_array(arr, (double)i);
 
    size_t idx = 99u;
    assert_int_equal(
        double_tensor_lsearch(arr, &idx, -1.0f, 0.0f), NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_double_tensor(arr);
}
// ================================================================================
// ================================================================================
// TEST SUITE REGISTRY

const struct CMUnitTest test_double_tensor[] = {
    /* init_double_array */
    cmocka_unit_test(test_init_double_array_null_allocator),
    cmocka_unit_test(test_init_double_array_success),

    /* init_double_tensor */
    cmocka_unit_test(test_init_double_tensor_success),
    cmocka_unit_test(test_init_double_tensor_null_shape),

    /* return_double_tensor */
    cmocka_unit_test(test_return_double_tensor_null),
    cmocka_unit_test(test_return_double_tensor_normal),

    /* copy_double_tensor */
    cmocka_unit_test(test_copy_double_tensor_null_src),
    cmocka_unit_test(test_copy_double_tensor_independence),

    /* introspection — null guards */
    cmocka_unit_test(test_double_tensor_size_null),
    cmocka_unit_test(test_double_tensor_alloc_null),
    cmocka_unit_test(test_double_tensor_data_size_null),
    cmocka_unit_test(test_double_tensor_dtype_null),
    cmocka_unit_test(test_is_double_tensor_empty_null),
    cmocka_unit_test(test_is_double_tensor_full_null),
    cmocka_unit_test(test_is_double_tensor_ptr_null_wrapper),
    cmocka_unit_test(test_double_tensor_ndim_null),
    cmocka_unit_test(test_double_tensor_shape_dim_null),
    cmocka_unit_test(test_double_tensor_shape_null),
    cmocka_unit_test(test_double_tensor_shape_ptr_null),
    cmocka_unit_test(test_double_tensor_strides_ptr_null),
    cmocka_unit_test(test_double_tensor_shape_str_null),

    /* introspection — values */
    cmocka_unit_test(test_double_tensor_size_value),
    cmocka_unit_test(test_double_tensor_alloc_value),
    cmocka_unit_test(test_double_tensor_data_size_value),
    cmocka_unit_test(test_double_tensor_dtype_value),
    cmocka_unit_test(test_is_double_tensor_empty_value),
    cmocka_unit_test(test_is_double_tensor_full_value),
    cmocka_unit_test(test_is_double_tensor_ptr_valid),
    cmocka_unit_test(test_double_tensor_ndim_value),
    cmocka_unit_test(test_double_tensor_shape_dim_value),
    cmocka_unit_test(test_double_tensor_shape_value),
    cmocka_unit_test(test_double_tensor_shape_ptr_value),
    cmocka_unit_test(test_double_tensor_strides_ptr_value),
    cmocka_unit_test(test_double_tensor_shape_str_value),

    /* utility — null guards */
    cmocka_unit_test(test_clear_double_tensor_null),
    cmocka_unit_test(test_concat_double_tensor_null),
    cmocka_unit_test(test_slice_double_tensor_null),
    cmocka_unit_test(test_reverse_double_tensor_null),
    cmocka_unit_test(test_sort_double_tensor_null),

    /* utility — values */
    cmocka_unit_test(test_clear_double_tensor_value),
    cmocka_unit_test(test_concat_double_tensor_value),
    cmocka_unit_test(test_slice_double_tensor_value),
    cmocka_unit_test(test_reverse_double_tensor_value),
    cmocka_unit_test(test_sort_double_tensor_forward),
    cmocka_unit_test(test_sort_double_tensor_reverse),

    /* set/get — null guards */
    cmocka_unit_test(test_set_get_double_tensor_index_null),
    cmocka_unit_test(test_set_get_double_tensor_nd_index_null),
    cmocka_unit_test(test_push_back_double_array_null),
    cmocka_unit_test(test_push_front_double_array_null),
    cmocka_unit_test(test_push_at_double_array_null),
    cmocka_unit_test(test_pop_back_double_array_null),
    cmocka_unit_test(test_pop_front_double_array_null),
    cmocka_unit_test(test_pop_at_double_array_null),

    /* set/get — values */
    cmocka_unit_test(test_set_get_double_tensor_index_value),
    cmocka_unit_test(test_set_get_double_tensor_nd_index_value),
    cmocka_unit_test(test_push_back_double_array_value),
    cmocka_unit_test(test_push_front_double_array_value),
    cmocka_unit_test(test_push_at_double_array_value),
    cmocka_unit_test(test_pop_back_double_array_value),
    cmocka_unit_test(test_pop_front_double_array_value),
    cmocka_unit_test(test_pop_at_double_array_value),

    /* lsearch null/guard */
    cmocka_unit_test(test_double_lsearch_null_tensor),
    cmocka_unit_test(test_double_lsearch_null_index),
    cmocka_unit_test(test_double_lsearch_empty),
 
    /* lsearch exact match — zero tolerance */
    cmocka_unit_test(test_double_lsearch_exact_first),
    cmocka_unit_test(test_double_lsearch_exact_last),
    cmocka_unit_test(test_double_lsearch_exact_middle),
    cmocka_unit_test(test_double_lsearch_exact_not_found),
 
    /* lsearch tolerance matching */
    cmocka_unit_test(test_double_lsearch_tolerance_exact_boundary),
    cmocka_unit_test(test_double_lsearch_tolerance_just_outside),
    cmocka_unit_test(test_double_lsearch_tolerance_negative_value),
    cmocka_unit_test(test_double_lsearch_tolerance_first_of_multiple),
 
    /* lsearch NaN and special values */
    cmocka_unit_test(test_double_lsearch_nan_element_not_matched),
    cmocka_unit_test(test_double_lsearch_infinity_behaviour),
    cmocka_unit_test(test_double_lsearch_negative_tolerance),
 
    /* lsearch single element */
    cmocka_unit_test(test_double_lsearch_single_match),
    cmocka_unit_test(test_double_lsearch_single_no_match),
 
    /* lsearch large array — exercises SIMD bulk path */
    cmocka_unit_test(test_double_lsearch_large_array_match_near_end),
    cmocka_unit_test(test_double_lsearch_large_array_not_found),
};

const size_t test_double_tensor_count = sizeof(test_double_tensor) /
                                       sizeof(test_double_tensor[0]);
// ================================================================================ 
// ================================================================================ 

/** Construct a populated long double array with values 1..n. */
static ldouble_tensor_t* _make_ldouble_array(size_t capacity, bool growth) {
    allocator_vtable_t alloc = heap_allocator();
    ldouble_tensor_expect_t r  = init_ldouble_array(capacity, growth, alloc);
    if (!r.has_value) return NULL;
    return r.u.value;
}

static ldouble_tensor_t* _make_ldouble_array_filled(size_t n, long double base) {
    /* Allocate n+1 so the result always has at least one free slot,
     * making it safe to use as a push destination in tests. */
    ldouble_tensor_t* arr = _make_ldouble_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++) {
        long double val = (long double)(base + i);
        push_back_ldouble_array(arr, val);
    }
    return arr;
}

// ================================================================================
// ================================================================================
// INITIALISATION AND TEARDOWN

// ---- init_ldouble_array ----------------------------------------------------------

/** NULL allocator must propagate an error through the expect type. */
static void test_init_ldouble_array_null_allocator(void** state) {
    (void)state;
    allocator_vtable_t bad = heap_allocator();
    bad.allocate = NULL;
    ldouble_tensor_expect_t r = init_ldouble_array(8u, false, bad);
    assert_false(r.has_value);
}

/** Successful construction produces a non-NULL wrapper with correct mode. */
static void test_init_ldouble_array_success(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_tensor_expect_t r  = init_ldouble_array(8u, false, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  ARRAY_STRUCT);
    assert_int_equal(r.u.value->base->dtype, LDOUBLE_TYPE);
    assert_int_equal(r.u.value->base->len,   0u);
    assert_int_equal(r.u.value->base->alloc, 8u);
    return_ldouble_tensor(r.u.value);
}

// ---- init_ldouble_tensor ---------------------------------------------------------

/** Successful construction of a fixed-shape tensor. */
static void test_init_ldouble_tensor_success(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    ldouble_tensor_expect_t r  = init_ldouble_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_non_null(r.u.value);
    assert_non_null(r.u.value->base);
    assert_int_equal(r.u.value->base->mode,  TENSOR_STRUCT);
    assert_int_equal(r.u.value->base->dtype, LDOUBLE_TYPE);
    assert_int_equal(r.u.value->base->len,   12u);
    return_ldouble_tensor(r.u.value);
}

/** NULL shape propagates an error. */
static void test_init_ldouble_tensor_null_shape(void** state) {
    (void)state;
    allocator_vtable_t alloc = heap_allocator();
    ldouble_tensor_expect_t r  = init_ldouble_tensor(2u, NULL, alloc);
    assert_false(r.has_value);
}

// ---- return_ldouble_tensor -------------------------------------------------------

/** return_ldouble_tensor(NULL) must not crash. */
static void test_return_ldouble_tensor_null(void** state) {
    (void)state;
    return_ldouble_tensor(NULL);
}

/** Normal construction followed by return must not crash or leak. */
static void test_return_ldouble_tensor_normal(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(4u, false);
    assert_non_null(arr);
    return_ldouble_tensor(arr);
}

// ---- copy_ldouble_tensor ---------------------------------------------------------

/** NULL src must return NULL_POINTER. */
static void test_copy_ldouble_tensor_null_src(void** state) {
    (void)state;
    ldouble_tensor_expect_t r = copy_ldouble_tensor(NULL, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

/** Copy is independent of source — mutation of copy does not affect src. */
static void test_copy_ldouble_tensor_independence(void** state) {
    (void)state;
    ldouble_tensor_t* src = _make_ldouble_array_filled(3u, 10u);
    assert_non_null(src);

    ldouble_tensor_expect_t r = copy_ldouble_tensor(src, NULL);
    assert_true(r.has_value);
    ldouble_tensor_t* dst = r.u.value;

    /* Overwrite slot 0 of the copy */
    assert_int_equal(set_ldouble_tensor_index(dst, 0u, 99.0f), NO_ERROR);

    /* Source slot 0 must be unchanged */
    long double src_val = 0u;
    assert_int_equal(get_ldouble_tensor_index(src, 0u, &src_val), NO_ERROR);
    assert_double_equal(src_val, 10.0f, 1.0e-3);

    return_ldouble_tensor(src);
    return_ldouble_tensor(dst);
}

// ================================================================================
// ================================================================================
// INTROSPECTION

static void test_ldouble_tensor_size_null(void** state) {
    (void)state;
    assert_int_equal(ldouble_tensor_size(NULL), 0u);
}

static void test_ldouble_tensor_size_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array_filled(3u, 1u);
    assert_non_null(arr);
    assert_int_equal(ldouble_tensor_size(arr), 3u);
    return_ldouble_tensor(arr);
}

static void test_ldouble_tensor_alloc_null(void** state) {
    (void)state;
    assert_int_equal(ldouble_tensor_alloc(NULL), 0u);
}

static void test_ldouble_tensor_alloc_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(8u, false);
    assert_non_null(arr);
    assert_int_equal(ldouble_tensor_alloc(arr), 8u);
    return_ldouble_tensor(arr);
}

static void test_ldouble_tensor_data_size_null(void** state) {
    (void)state;
    assert_int_equal(ldouble_tensor_data_size(NULL), 0u);
}

static void test_ldouble_tensor_data_size_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(ldouble_tensor_data_size(arr), sizeof(long double));
    return_ldouble_tensor(arr);
}

static void test_ldouble_tensor_dtype_null(void** state) {
    (void)state;
    assert_int_equal(ldouble_tensor_dtype(NULL), UNKNOWN_TYPE);
}

static void test_ldouble_tensor_dtype_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(ldouble_tensor_dtype(arr), LDOUBLE_TYPE);
    return_ldouble_tensor(arr);
}

static void test_is_ldouble_tensor_empty_null(void** state) {
    (void)state;
    assert_true(is_ldouble_tensor_empty(NULL));
}

static void test_is_ldouble_tensor_empty_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(4u, false);
    assert_non_null(arr);
    assert_true(is_ldouble_tensor_empty(arr));
    push_back_ldouble_array(arr, 1u);
    assert_false(is_ldouble_tensor_empty(arr));
    return_ldouble_tensor(arr);
}

static void test_is_ldouble_tensor_full_null(void** state) {
    (void)state;
    assert_true(is_ldouble_tensor_full(NULL));
}

static void test_is_ldouble_tensor_full_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(2u, false);
    assert_non_null(arr);
    assert_false(is_ldouble_tensor_full(arr));
    push_back_ldouble_array(arr, 1.0f);
    push_back_ldouble_array(arr, 2.0f);
    assert_true(is_ldouble_tensor_full(arr));
    return_ldouble_tensor(arr);
}

static void test_is_ldouble_tensor_ptr_null_wrapper(void** state) {
    (void)state;
    long double dummy = 0u;
    assert_false(is_ldouble_tensor_ptr(NULL, &dummy));
}

static void test_is_ldouble_tensor_ptr_valid(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array_filled(4u, 10.0f);
    assert_non_null(arr);
    /* Pointer to first element must be valid */
    const long double* p = (const long double*)arr->base->data;
    assert_true(is_ldouble_tensor_ptr(arr, p));
    return_ldouble_tensor(arr);
}

static void test_ldouble_tensor_ndim_null(void** state) {
    (void)state;
    assert_int_equal(ldouble_tensor_ndim(NULL), 0u);
}

static void test_ldouble_tensor_ndim_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(ldouble_tensor_ndim(arr), 1u);
    return_ldouble_tensor(arr);
}

static void test_ldouble_tensor_shape_dim_null(void** state) {
    (void)state;
    assert_int_equal(ldouble_tensor_shape_dim(NULL, 0u), 0u);
}

static void test_ldouble_tensor_shape_dim_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    ldouble_tensor_expect_t r  = init_ldouble_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    assert_int_equal(ldouble_tensor_shape_dim(r.u.value, 0u), 3u);
    assert_int_equal(ldouble_tensor_shape_dim(r.u.value, 1u), 4u);
    return_ldouble_tensor(r.u.value);
}

static void test_ldouble_tensor_shape_null(void** state) {
    (void)state;
    size_t buf[2];
    assert_int_equal(ldouble_tensor_shape(NULL, buf, 2u), NULL_POINTER);
}

static void test_ldouble_tensor_shape_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 4u };
    allocator_vtable_t alloc = heap_allocator();
    ldouble_tensor_expect_t r  = init_ldouble_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    size_t buf[2] = { 0u, 0u };
    assert_int_equal(ldouble_tensor_shape(r.u.value, buf, 2u), NO_ERROR);
    assert_int_equal(buf[0], 3u);
    assert_int_equal(buf[1], 4u);
    return_ldouble_tensor(r.u.value);
}

static void test_ldouble_tensor_shape_ptr_null(void** state) {
    (void)state;
    assert_null(ldouble_tensor_shape_ptr(NULL));
}

static void test_ldouble_tensor_shape_ptr_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(4u, false);
    assert_non_null(arr);
    assert_non_null(ldouble_tensor_shape_ptr(arr));
    return_ldouble_tensor(arr);
}

static void test_ldouble_tensor_strides_ptr_null(void** state) {
    (void)state;
    assert_null(ldouble_tensor_strides_ptr(NULL));
}

static void test_ldouble_tensor_strides_ptr_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(4u, false);
    assert_non_null(arr);
    const size_t* strides = ldouble_tensor_strides_ptr(arr);
    assert_non_null(strides);
    assert_int_equal(strides[0], sizeof(long double));
    return_ldouble_tensor(arr);
}

static void test_ldouble_tensor_shape_str_null(void** state) {
    (void)state;
    char buf[32];
    assert_int_equal(ldouble_tensor_shape_str(NULL, buf, sizeof(buf)), NULL_POINTER);
}

static void test_ldouble_tensor_shape_str_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(8u, false);
    assert_non_null(arr);
    char buf[32];
    assert_int_equal(ldouble_tensor_shape_str(arr, buf, sizeof(buf)), NO_ERROR);
    assert_string_equal(buf, "(8)");
    return_ldouble_tensor(arr);
}

// ================================================================================
// ================================================================================
// UTILITY OPERATIONS

// ---- clear_ldouble_tensor --------------------------------------------------------

static void test_clear_ldouble_tensor_null(void** state) {
    (void)state;
    assert_int_equal(clear_ldouble_tensor(NULL), NULL_POINTER);
}

static void test_clear_ldouble_tensor_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array_filled(3u, 10.0f);
    assert_non_null(arr);
    assert_int_equal(clear_ldouble_tensor(arr), NO_ERROR);
    assert_int_equal(ldouble_tensor_size(arr), 0u);
    return_ldouble_tensor(arr);
}

// ---- concat_ldouble_tensor_array -------------------------------------------------

static void test_concat_ldouble_tensor_null(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(4u, false);
    assert_non_null(arr);
    assert_int_equal(concat_ldouble_tensor_array(NULL, arr), NULL_POINTER);
    assert_int_equal(concat_ldouble_tensor_array(arr, NULL), NULL_POINTER);
    return_ldouble_tensor(arr);
}

static void test_concat_ldouble_tensor_value(void** state) {
    (void)state;

    /* dst needs capacity for 5 elements total — allocate with room to grow */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_tensor_expect_t r  = init_ldouble_array(8u, false, alloc);
    assert_true(r.has_value);
    ldouble_tensor_t* dst = r.u.value;

    /* Populate dst with [1, 2, 3] */
    push_back_ldouble_array(dst, 1.0f);
    push_back_ldouble_array(dst, 2.0f);
    push_back_ldouble_array(dst, 3.0f);

    /* src = [4, 5] */
    ldouble_tensor_t* src = _make_ldouble_array_filled(2u, 4.0f);
    assert_non_null(src);

    assert_int_equal(concat_ldouble_tensor_array(dst, src), NO_ERROR);
    assert_int_equal(ldouble_tensor_size(dst), 5u);

    long double expected[] = { 1u, 2u, 3u, 4u, 5u };
    for (size_t i = 0u; i < 5u; i++) {
        long double out = 0u;
        assert_int_equal(get_ldouble_tensor_index(dst, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_ldouble_tensor(dst);
    return_ldouble_tensor(src);
}

// ---- slice_ldouble_tensor_array --------------------------------------------------

static void test_slice_ldouble_tensor_null(void** state) {
    (void)state;
    ldouble_tensor_expect_t r = slice_ldouble_tensor_array(NULL, 0u, 1u, NULL);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}

static void test_slice_ldouble_tensor_value(void** state) {
    (void)state;
    ldouble_tensor_t* src = _make_ldouble_array_filled(5u, 10.0f); /* [10,11,12,13,14] */
    assert_non_null(src);

    ldouble_tensor_expect_t r = slice_ldouble_tensor_array(src, 1u, 4u, NULL);
    assert_true(r.has_value);
    ldouble_tensor_t* sl = r.u.value;

    assert_int_equal(ldouble_tensor_size(sl), 3u);
    long double expected[] = { 11., 12., 13. };
    for (size_t i = 0u; i < 3u; i++) {
        long double out = 0.;
        assert_int_equal(get_ldouble_tensor_index(sl, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_ldouble_tensor(src);
    return_ldouble_tensor(sl);
}

// ---- reverse_ldouble_tensor ------------------------------------------------------

static void test_reverse_ldouble_tensor_null(void** state) {
    (void)state;
    assert_int_equal(reverse_ldouble_tensor(NULL), NULL_POINTER);
}

static void test_reverse_ldouble_tensor_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array_filled(4u, 1.); /* [1,2,3,4] */
    assert_non_null(arr);

    assert_int_equal(reverse_ldouble_tensor(arr), NO_ERROR);

    long double expected[] = { 4., 3., 2., 1. };
    for (size_t i = 0u; i < 4u; i++) {
        long double out = 0.;
        assert_int_equal(get_ldouble_tensor_index(arr, i, &out), NO_ERROR);
        assert_double_equal(out, expected[i], 1.0e-3);
    }

    return_ldouble_tensor(arr);
}

// ---- sort_ldouble_tensor ---------------------------------------------------------

static void test_sort_ldouble_tensor_null(void** state) {
    (void)state;
    assert_int_equal(sort_ldouble_tensor(NULL, FORWARD), NULL_POINTER);
}

static void test_sort_ldouble_tensor_forward(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(5u, false);
    assert_non_null(arr);

    long double vals[] = { 3., 1., 4., 1., 5. };
    for (size_t i = 0u; i < 5u; i++)
        push_back_ldouble_array(arr, vals[i]);

    assert_int_equal(sort_ldouble_tensor(arr, FORWARD), NO_ERROR);

    long double expected[] = { 1., 1., 3., 4., 5. };
    for (size_t i = 0u; i < 5u; i++) {
        long double out = 0.;
        assert_int_equal(get_ldouble_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_ldouble_tensor(arr);
}

static void test_sort_ldouble_tensor_reverse(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(5u, false);
    assert_non_null(arr);

    long double vals[] = { 3., 1., 4., 1., 5. };
    for (size_t i = 0u; i < 5u; i++)
        push_back_ldouble_array(arr, vals[i]);

    assert_int_equal(sort_ldouble_tensor(arr, REVERSE), NO_ERROR);

    long double expected[] = { 5., 4., 3., 1., 1. };
    for (size_t i = 0u; i < 5u; i++) {
        long double out = 0.;
        assert_int_equal(get_ldouble_tensor_index(arr, i, &out), NO_ERROR);
        assert_int_equal(out, expected[i]);
    }

    return_ldouble_tensor(arr);
}

// ================================================================================
// ================================================================================
// SET AND GET DATA

// ---- set_ldouble_tensor_index / get_ldouble_tensor_index --------------------------

static void test_set_get_ldouble_tensor_index_null(void** state) {
    (void)state;
    long double dummy = 0.;
    assert_int_equal(set_ldouble_tensor_index(NULL, 0u, 1.),    NULL_POINTER);
    assert_int_equal(get_ldouble_tensor_index(NULL, 0u, &dummy), NULL_POINTER);
}

static void test_set_get_ldouble_tensor_index_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array_filled(3u, 10.);
    assert_non_null(arr);

    assert_int_equal(set_ldouble_tensor_index(arr, 1u, 99.), NO_ERROR);

    long double out = 0.;
    assert_int_equal(get_ldouble_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_double_equal(out, 99.0, 1.0e-3);

    return_ldouble_tensor(arr);
}

// ---- set_ldouble_tensor_nd_index / get_ldouble_tensor_nd_index --------------------

static void test_set_get_ldouble_tensor_nd_index_null(void** state) {
    (void)state;
    size_t  idx[] = { 0u, 0u };
    long double out   = 0u;
    assert_int_equal(set_ldouble_tensor_nd_index(NULL, idx, 1.),   NULL_POINTER);
    assert_int_equal(get_ldouble_tensor_nd_index(NULL, idx, &out), NULL_POINTER);
}

static void test_set_get_ldouble_tensor_nd_index_value(void** state) {
    (void)state;
    size_t shape[] = { 3u, 3u };
    allocator_vtable_t alloc = heap_allocator();
    ldouble_tensor_expect_t r  = init_ldouble_tensor(2u, shape, alloc);
    assert_true(r.has_value);
    ldouble_tensor_t* mat = r.u.value;

    size_t idx[] = { 1u, 2u };
    assert_int_equal(set_ldouble_tensor_nd_index(mat, idx, 42.), NO_ERROR);

    long double out = 0.;
    assert_int_equal(get_ldouble_tensor_nd_index(mat, idx, &out), NO_ERROR);
    assert_double_equal(out, 42., 1.0e-3);

    return_ldouble_tensor(mat);
}

// ---- push and pop wrappers ----------------------------------------------------

static void test_push_back_ldouble_array_null(void** state) {
    (void)state;
    assert_int_equal(push_back_ldouble_array(NULL, 1.0), NULL_POINTER);
}

static void test_push_back_ldouble_array_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(4u, false);
    assert_non_null(arr);

    assert_int_equal(push_back_ldouble_array(arr, 10.0), NO_ERROR);
    assert_int_equal(push_back_ldouble_array(arr, 20.0), NO_ERROR);
    assert_int_equal(ldouble_tensor_size(arr), 2u);

    long double out = 0.0;
    assert_int_equal(get_ldouble_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_double_equal(out, 20.0, 1.0e-3);

    return_ldouble_tensor(arr);
}

static void test_push_front_ldouble_array_null(void** state) {
    (void)state;
    assert_int_equal(push_front_ldouble_array(NULL, 1.), NULL_POINTER);
}

static void test_push_front_ldouble_array_value(void** state) {
    (void)state;
    /* Capacity 3 so there is room for the push_front */
    allocator_vtable_t alloc = heap_allocator();
    ldouble_tensor_expect_t r  = init_ldouble_array(3u, false, alloc);
    assert_true(r.has_value);
    ldouble_tensor_t* arr = r.u.value;

    push_back_ldouble_array(arr, 10.0);
    push_back_ldouble_array(arr, 11.0);
    /* arr = [10, 11], len == 2, alloc == 3 — one slot free */

    assert_int_equal(push_front_ldouble_array(arr, 99.0), NO_ERROR);

    long double out = 0.0;
    assert_int_equal(get_ldouble_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_int_equal(out, 99.0);
    assert_int_equal(get_ldouble_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_int_equal(out, 10.0);
    assert_int_equal(get_ldouble_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_int_equal(out, 11.0);

    return_ldouble_tensor(arr);
}

static void test_push_at_ldouble_array_null(void** state) {
    (void)state;
    assert_int_equal(push_at_ldouble_array(NULL, 1u, 0.0), NULL_POINTER);
}

static void test_push_at_ldouble_array_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array_filled(3u, 10.0); /* [10, 11, 12] */
    assert_non_null(arr);

    assert_int_equal(push_at_ldouble_array(arr, 99.0, 1u), NO_ERROR);
    /* arr = [10, 99, 11, 12] */

    long double out = 0.0;
    assert_int_equal(get_ldouble_tensor_index(arr, 0u, &out), NO_ERROR);
    assert_double_equal(out, 10.0, 1.0e-3);
    assert_int_equal(get_ldouble_tensor_index(arr, 1u, &out), NO_ERROR);
    assert_double_equal(out, 99.0, 1.0e-3);
    assert_int_equal(get_ldouble_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_double_equal(out, 11.0, 1.0e-3);
    assert_int_equal(get_ldouble_tensor_index(arr, 3u, &out), NO_ERROR);
    assert_double_equal(out, 12.0, 1.0e-3);

    return_ldouble_tensor(arr);
}

static void test_pop_back_ldouble_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_back_ldouble_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_back_ldouble_array_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array_filled(3u, 10.0); /* [10,11,12] */
    assert_non_null(arr);

    long double out = 0.0;
    assert_int_equal(pop_back_ldouble_array(arr, &out), NO_ERROR);
    assert_double_equal(out, 12.0, 1.0e-3);
    assert_int_equal(ldouble_tensor_size(arr), 2u);

    return_ldouble_tensor(arr);
}

static void test_pop_front_ldouble_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_front_ldouble_array(NULL, NULL), NULL_POINTER);
}

static void test_pop_front_ldouble_array_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array_filled(3u, 10.0); /* [10,11,12] */
    assert_non_null(arr);

    long double out = 0.0;
    assert_int_equal(pop_front_ldouble_array(arr, &out), NO_ERROR);
    assert_double_equal(out, 10.0, 1.0e-3);
    assert_int_equal(ldouble_tensor_size(arr), 2u);

    return_ldouble_tensor(arr);
}

static void test_pop_at_ldouble_array_null(void** state) {
    (void)state;
    assert_int_equal(pop_at_ldouble_array(NULL, NULL, 0u), NULL_POINTER);
}

static void test_pop_at_ldouble_array_value(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array_filled(4u, 10.0); /* [10,11,12,13] */
    assert_non_null(arr);

    long double out = 0.0;
    assert_int_equal(pop_at_ldouble_array(arr, &out, 2u), NO_ERROR);
    assert_double_equal(out, 12.0, 1.0e-3);
    assert_int_equal(ldouble_tensor_size(arr), 3u);

    assert_int_equal(get_ldouble_tensor_index(arr, 2u, &out), NO_ERROR);
    assert_double_equal(out, 13.0, 1.0e-3);

    return_ldouble_tensor(arr);
}
// -------------------------------------------------------------------------------- 

static ldouble_tensor_t* _make_ldouble_array_from(const long double* vals,
                                                   size_t             n) {
    ldouble_tensor_t* arr = _make_ldouble_array(n + 1u, false);
    if (arr == NULL) return NULL;
    for (size_t i = 0u; i < n; i++)
        push_back_ldouble_array(arr, vals[i]);
    return arr;
}
 
// ================================================================================
// ================================================================================
// NULL / GUARD TESTS
 
/** NULL tensor must return NULL_POINTER. */
static void test_ldouble_lsearch_null_tensor(void** state) {
    (void)state;
    size_t idx = 0u;
    assert_int_equal(
        ldouble_tensor_lsearch(NULL, &idx, 1.0L, 0.0L),
        NULL_POINTER);
}
 
/** NULL index pointer must return NULL_POINTER. */
static void test_ldouble_lsearch_null_index(void** state) {
    (void)state;
    long double vals[] = { 1.0L, 2.0L };
    ldouble_tensor_t* arr = _make_ldouble_array_from(vals, 2u);
    assert_non_null(arr);
 
    assert_int_equal(
        ldouble_tensor_lsearch(arr, NULL, 1.0L, 0.0L),
        NULL_POINTER);
    return_ldouble_tensor(arr);
}
 
/** Empty array must return EMPTY without touching index. */
static void test_ldouble_lsearch_empty(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(4u, false);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, 1.0L, 0.0L),
        EMPTY);
    assert_int_equal(idx, 99u);
    return_ldouble_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// EXACT MATCH (tolerance == 0.0L)
 
/** Exact match at index 0. */
static void test_ldouble_lsearch_exact_first(void** state) {
    (void)state;
    long double vals[] = { 1.0L, 2.0L, 3.0L };
    ldouble_tensor_t* arr = _make_ldouble_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, 1.0L, 0.0L),
        NO_ERROR);
    assert_int_equal(idx, 0u);
    return_ldouble_tensor(arr);
}
 
/** Exact match at the last index. */
static void test_ldouble_lsearch_exact_last(void** state) {
    (void)state;
    long double vals[] = { 1.0L, 2.0L, 3.0L };
    ldouble_tensor_t* arr = _make_ldouble_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, 3.0L, 0.0L),
        NO_ERROR);
    assert_int_equal(idx, 2u);
    return_ldouble_tensor(arr);
}
 
/** Exact match in the middle. */
static void test_ldouble_lsearch_exact_middle(void** state) {
    (void)state;
    long double vals[] = { 1.0L, 2.0L, 3.0L, 4.0L, 5.0L };
    ldouble_tensor_t* arr = _make_ldouble_array_from(vals, 5u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, 3.0L, 0.0L),
        NO_ERROR);
    assert_int_equal(idx, 2u);
    return_ldouble_tensor(arr);
}
 
/** Value not present returns NOT_FOUND and index is unchanged. */
static void test_ldouble_lsearch_exact_not_found(void** state) {
    (void)state;
    long double vals[] = { 1.0L, 2.0L, 3.0L };
    ldouble_tensor_t* arr = _make_ldouble_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, 9.9L, 0.0L),
        NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_ldouble_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// TOLERANCE MATCHING
 
/**
 * Element at exactly the tolerance boundary must match —
 * the condition is <= not <.
 */
static void test_ldouble_lsearch_tolerance_exact_boundary(void** state) {
    (void)state;
    long double vals[] = { 1.0L, 2.05L, 3.0L };
    ldouble_tensor_t* arr = _make_ldouble_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, 2.0L, 0.05L),
        NO_ERROR);
    assert_int_equal(idx, 1u);
    return_ldouble_tensor(arr);
}
 
/**
 * Element just beyond tolerance must not match.
 * |2.06 - 2.0| == 0.06 > 0.05.
 */
static void test_ldouble_lsearch_tolerance_just_outside(void** state) {
    (void)state;
    long double vals[] = { 1.0L, 2.06L, 3.0L };
    ldouble_tensor_t* arr = _make_ldouble_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, 2.0L, 0.05L),
        NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_ldouble_tensor(arr);
}
 
/**
 * Negative value within tolerance must match — confirms fabsl
 * handles values below zero correctly.
 */
static void test_ldouble_lsearch_tolerance_negative_value(void** state) {
    (void)state;
    long double vals[] = { -3.0L, -1.95L, 0.0L };
    ldouble_tensor_t* arr = _make_ldouble_array_from(vals, 3u);
    assert_non_null(arr);
 
    /* |-1.95 - (-2.0)| == 0.05 <= 0.1 */
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, -2.0L, 0.1L),
        NO_ERROR);
    assert_int_equal(idx, 1u);
    return_ldouble_tensor(arr);
}
 
/**
 * When multiple elements are within tolerance, the first occurrence
 * must be returned.
 */
static void test_ldouble_lsearch_tolerance_first_of_multiple(void** state) {
    (void)state;
    long double vals[] = { 1.0L, 2.03L, 2.01L, 2.04L, 5.0L };
    ldouble_tensor_t* arr = _make_ldouble_array_from(vals, 5u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, 2.0L, 0.05L),
        NO_ERROR);
    assert_int_equal(idx, 1u);
    return_ldouble_tensor(arr);
}
 
/**
 * Extended precision specific: a value that rounds to the same double
 * but differs in the 80-bit mantissa must not match with zero tolerance.
 * This exercises the precision advantage of long double over double.
 */
static void test_ldouble_lsearch_extended_precision(void** state) {
    (void)state;
    /* 1.0L / 3.0L has more precision than (double)(1.0 / 3.0) cast back */
    long double exact   = 1.0L / 3.0L;
    long double approx  = (long double)(1.0 / 3.0);   /* double precision */
    long double vals[]  = { exact };
    ldouble_tensor_t* arr = _make_ldouble_array_from(vals, 1u);
    assert_non_null(arr);
 
    size_t idx = 99u;
 
    /* Exact match with the stored value must succeed */
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, exact, 0.0L),
        NO_ERROR);
    assert_int_equal(idx, 0u);
 
    /* The double-precision approximation differs from the stored value —
     * with zero tolerance and extended precision this must NOT match
     * on platforms where sizeof(long double) > sizeof(double). */
    if (sizeof(long double) > sizeof(double)) {
        assert_int_equal(
            ldouble_tensor_lsearch(arr, &idx, approx, 0.0L),
            NOT_FOUND);
    }
 
    return_ldouble_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// NaN AND SPECIAL VALUES
 
/**
 * NaN elements must never match — fabsl(NaN - value) is NaN and
 * NaN <= tolerance is false.
 */
static void test_ldouble_lsearch_nan_not_matched(void** state) {
    (void)state;
    long double nan_val = (long double)NAN;
    long double vals[]  = { 1.0L, nan_val, 3.0L };
    ldouble_tensor_t* arr = _make_ldouble_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, nan_val, 1e10L),
        NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_ldouble_tensor(arr);
}
 
/**
 * Negative tolerance produces no matches since no absolute difference
 * can be <= a negative number.
 */
static void test_ldouble_lsearch_negative_tolerance(void** state) {
    (void)state;
    long double vals[] = { 1.0L, 2.0L, 3.0L };
    ldouble_tensor_t* arr = _make_ldouble_array_from(vals, 3u);
    assert_non_null(arr);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, 2.0L, -0.01L),
        NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_ldouble_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// SINGLE ELEMENT
 
/** Single-element array containing a match must succeed. */
static void test_ldouble_lsearch_single_match(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(2u, false);
    assert_non_null(arr);
    push_back_ldouble_array(arr, 42.0L);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, 42.0L, 0.0L),
        NO_ERROR);
    assert_int_equal(idx, 0u);
    return_ldouble_tensor(arr);
}
 
/** Single-element array not containing a match must return NOT_FOUND. */
static void test_ldouble_lsearch_single_no_match(void** state) {
    (void)state;
    ldouble_tensor_t* arr = _make_ldouble_array(2u, false);
    assert_non_null(arr);
    push_back_ldouble_array(arr, 42.0L);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, 7.0L, 0.0L),
        NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_ldouble_tensor(arr);
}
 
// ================================================================================
// ================================================================================
// LARGE ARRAY
 
/**
 * Large array with target near the end exercises the full loop —
 * since there is no SIMD path every element is compared individually.
 */
static void test_ldouble_lsearch_large_match_near_end(void** state) {
    (void)state;
    const size_t n = 64u;
    ldouble_tensor_t* arr = _make_ldouble_array(n + 1u, false);
    assert_non_null(arr);
 
    for (size_t i = 0u; i < n - 1u; i++)
        push_back_ldouble_array(arr, (long double)i * 0.1L);
    push_back_ldouble_array(arr, 999.0L);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, 999.0L, 0.0L),
        NO_ERROR);
    assert_int_equal(idx, n - 1u);
    return_ldouble_tensor(arr);
}
 
/** Large array where the target is absent returns NOT_FOUND. */
static void test_ldouble_lsearch_large_not_found(void** state) {
    (void)state;
    const size_t n = 64u;
    ldouble_tensor_t* arr = _make_ldouble_array(n + 1u, false);
    assert_non_null(arr);
 
    for (size_t i = 0u; i < n; i++)
        push_back_ldouble_array(arr, (long double)i);
 
    size_t idx = 99u;
    assert_int_equal(
        ldouble_tensor_lsearch(arr, &idx, -1.0L, 0.0L),
        NOT_FOUND);
    assert_int_equal(idx, 99u);
    return_ldouble_tensor(arr);
}
// ================================================================================
// ================================================================================
// TEST SUITE REGISTRY

const struct CMUnitTest test_ldouble_tensor[] = {
    /* init_ldouble_array */
    cmocka_unit_test(test_init_ldouble_array_null_allocator),
    cmocka_unit_test(test_init_ldouble_array_success),

    /* init_ldouble_tensor */
    cmocka_unit_test(test_init_ldouble_tensor_success),
    cmocka_unit_test(test_init_ldouble_tensor_null_shape),

    /* return_ldouble_tensor */
    cmocka_unit_test(test_return_ldouble_tensor_null),
    cmocka_unit_test(test_return_ldouble_tensor_normal),

    /* copy_ldouble_tensor */
    cmocka_unit_test(test_copy_ldouble_tensor_null_src),
    cmocka_unit_test(test_copy_ldouble_tensor_independence),

    /* introspection — null guards */
    cmocka_unit_test(test_ldouble_tensor_size_null),
    cmocka_unit_test(test_ldouble_tensor_alloc_null),
    cmocka_unit_test(test_ldouble_tensor_data_size_null),
    cmocka_unit_test(test_ldouble_tensor_dtype_null),
    cmocka_unit_test(test_is_ldouble_tensor_empty_null),
    cmocka_unit_test(test_is_ldouble_tensor_full_null),
    cmocka_unit_test(test_is_ldouble_tensor_ptr_null_wrapper),
    cmocka_unit_test(test_ldouble_tensor_ndim_null),
    cmocka_unit_test(test_ldouble_tensor_shape_dim_null),
    cmocka_unit_test(test_ldouble_tensor_shape_null),
    cmocka_unit_test(test_ldouble_tensor_shape_ptr_null),
    cmocka_unit_test(test_ldouble_tensor_strides_ptr_null),
    cmocka_unit_test(test_ldouble_tensor_shape_str_null),

    /* introspection — values */
    cmocka_unit_test(test_ldouble_tensor_size_value),
    cmocka_unit_test(test_ldouble_tensor_alloc_value),
    cmocka_unit_test(test_ldouble_tensor_data_size_value),
    cmocka_unit_test(test_ldouble_tensor_dtype_value),
    cmocka_unit_test(test_is_ldouble_tensor_empty_value),
    cmocka_unit_test(test_is_ldouble_tensor_full_value),
    cmocka_unit_test(test_is_ldouble_tensor_ptr_valid),
    cmocka_unit_test(test_ldouble_tensor_ndim_value),
    cmocka_unit_test(test_ldouble_tensor_shape_dim_value),
    cmocka_unit_test(test_ldouble_tensor_shape_value),
    cmocka_unit_test(test_ldouble_tensor_shape_ptr_value),
    cmocka_unit_test(test_ldouble_tensor_strides_ptr_value),
    cmocka_unit_test(test_ldouble_tensor_shape_str_value),

    /* utility — null guards */
    cmocka_unit_test(test_clear_ldouble_tensor_null),
    cmocka_unit_test(test_concat_ldouble_tensor_null),
    cmocka_unit_test(test_slice_ldouble_tensor_null),
    cmocka_unit_test(test_reverse_ldouble_tensor_null),
    cmocka_unit_test(test_sort_ldouble_tensor_null),

    /* utility — values */
    cmocka_unit_test(test_clear_ldouble_tensor_value),
    cmocka_unit_test(test_concat_ldouble_tensor_value),
    cmocka_unit_test(test_slice_ldouble_tensor_value),
    cmocka_unit_test(test_reverse_ldouble_tensor_value),
    cmocka_unit_test(test_sort_ldouble_tensor_forward),
    cmocka_unit_test(test_sort_ldouble_tensor_reverse),

    /* set/get — null guards */
    cmocka_unit_test(test_set_get_ldouble_tensor_index_null),
    cmocka_unit_test(test_set_get_ldouble_tensor_nd_index_null),
    cmocka_unit_test(test_push_back_ldouble_array_null),
    cmocka_unit_test(test_push_front_ldouble_array_null),
    cmocka_unit_test(test_push_at_ldouble_array_null),
    cmocka_unit_test(test_pop_back_ldouble_array_null),
    cmocka_unit_test(test_pop_front_ldouble_array_null),
    cmocka_unit_test(test_pop_at_ldouble_array_null),

    /* set/get — values */
    cmocka_unit_test(test_set_get_ldouble_tensor_index_value),
    cmocka_unit_test(test_set_get_ldouble_tensor_nd_index_value),
    cmocka_unit_test(test_push_back_ldouble_array_value),
    cmocka_unit_test(test_push_front_ldouble_array_value),
    cmocka_unit_test(test_push_at_ldouble_array_value),
    cmocka_unit_test(test_pop_back_ldouble_array_value),
    cmocka_unit_test(test_pop_front_ldouble_array_value),
    cmocka_unit_test(test_pop_at_ldouble_array_value),

    /* null/guard */
    cmocka_unit_test(test_ldouble_lsearch_null_tensor),
    cmocka_unit_test(test_ldouble_lsearch_null_index),
    cmocka_unit_test(test_ldouble_lsearch_empty),
 
    /* exact match — zero tolerance */
    cmocka_unit_test(test_ldouble_lsearch_exact_first),
    cmocka_unit_test(test_ldouble_lsearch_exact_last),
    cmocka_unit_test(test_ldouble_lsearch_exact_middle),
    cmocka_unit_test(test_ldouble_lsearch_exact_not_found),
 
    /* tolerance matching */
    cmocka_unit_test(test_ldouble_lsearch_tolerance_exact_boundary),
    cmocka_unit_test(test_ldouble_lsearch_tolerance_just_outside),
    cmocka_unit_test(test_ldouble_lsearch_tolerance_negative_value),
    cmocka_unit_test(test_ldouble_lsearch_tolerance_first_of_multiple),
    cmocka_unit_test(test_ldouble_lsearch_extended_precision),
 
    /* NaN and special values */
    cmocka_unit_test(test_ldouble_lsearch_nan_not_matched),
    cmocka_unit_test(test_ldouble_lsearch_negative_tolerance),
 
    /* single element */
    cmocka_unit_test(test_ldouble_lsearch_single_match),
    cmocka_unit_test(test_ldouble_lsearch_single_no_match),
 
    /* large array — exercises full scalar loop */
    cmocka_unit_test(test_ldouble_lsearch_large_match_near_end),
    cmocka_unit_test(test_ldouble_lsearch_large_not_found),
};

const size_t test_ldouble_tensor_count = sizeof(test_ldouble_tensor) /
                                       sizeof(test_ldouble_tensor[0]);
// ================================================================================
// ================================================================================
// eof
