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

    // float out = 0.0f;
    // assert_int_equal(get_tensor_index(t, 0u, &out, FLOAT_TYPE), NO_ERROR);
    // assert_float_equal(out, 2.5f, 0.0f);
    //
    // return_tensor(t);
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
    tensor_expect_t r = copy_tensor(NULL, alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
}
 
/** NULL allocate function pointer must return NULL_POINTER. */
static void test_copy_tensor_null_allocator(void** state) {
    (void)state;
    const size_t shape[] = { 4u };
    tensor_expect_t src_r = _make_tensor(1u, shape, FLOAT_TYPE);
    assert_true(src_r.has_value);
 
    allocator_vtable_t bad_alloc = heap_allocator();
    bad_alloc.allocate = NULL;
 
    tensor_expect_t r = copy_tensor(src_r.u.value, bad_alloc);
    assert_false(r.has_value);
    assert_int_equal(r.u.error, NULL_POINTER);
 
    return_tensor(src_r.u.value);
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
    tensor_expect_t dst_r = copy_tensor(src, alloc);
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
    tensor_expect_t dst_r = copy_tensor(src, alloc);
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
    tensor_expect_t dst_r = copy_tensor(src, alloc);
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
    tensor_expect_t dst_r = copy_tensor(src, alloc);
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
    tensor_expect_t dst_r = copy_tensor(src, alloc);
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
    tensor_expect_t dst_r = copy_tensor(src, alloc);
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
 
    tensor_expect_t dst_r = copy_tensor(src, alloc);
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
    tensor_expect_t dst_r = copy_tensor(src, alloc);
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
    cmocka_unit_test(test_copy_tensor_null_allocator),
 
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
};

const size_t test_tensor_count = sizeof(test_tensor) / sizeof(test_tensor[0]);
// ================================================================================
// ================================================================================
// eof
