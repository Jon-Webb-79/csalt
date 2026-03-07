// ================================================================================
// ================================================================================
// - File:    c_float.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 06, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_float.h"

#if defined(__AVX512F__)
#  include "simd_avx512_float.inl"
#elif defined(__AVX2__)
#  include "simd_avx2_float.inl"
#elif defined(__AVX__)
#  include "simd_avx_float.inl"
#elif defined(__SSE4_1__)
#  include "simd_sse41_float.inl"
#elif defined(__SSSE3__)
#  include "simd_sse3_float.inl"
#elif defined(__SSE2__)
#  include "simd_sse2_float.inl"
#elif defined(__ARM_FEATURE_SVE2)
#  include "simd_sve2_float.inl"
#elif defined(__ARM_FEATURE_SVE)
#  include "simd_sve_float.inl"
#elif defined(__ARM_NEON)
#  include "simd_neon_float.inl"
#else
#  include "simd_scalar_float.inl"
#endif
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Internal helper: unwrap a raw array_expect_t into a float_array_expect_t.
// The array_t* stored inside the array_expect_t is reinterpreted as the base
// of a float_array_t since float_array_t contains array_t as its first member.
// ================================================================================

static inline float_array_expect_t _wrap_expect(array_expect_t e) {
    if (e.has_value) {
        return (float_array_expect_t){
            .has_value = true,
            .u.value   = (float_array_t*)e.u.value
        };
    }
    return (float_array_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

float_array_expect_t init_float_array(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v) {
    return _wrap_expect(init_array(capacity, FLOAT_TYPE, growth, alloc_v));
}

// --------------------------------------------------------------------------------

void return_float_array(float_array_t* array) {
    if (array == NULL) return;
    return_array(&array->base);
}

// ================================================================================
// Push operations
// ================================================================================

error_code_t push_back_float_array(float_array_t* array, float value) {
    if (array == NULL) return NULL_POINTER;
    return push_back_array(&array->base, &value, FLOAT_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_front_float_array(float_array_t* array, float value) {
    if (array == NULL) return NULL_POINTER;
    return push_front_array(&array->base, &value, FLOAT_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_at_float_array(float_array_t* array,
                                 size_t         index,
                                 float          value) {
    if (array == NULL) return NULL_POINTER;
    return push_at_array(&array->base, &value, index, FLOAT_TYPE);
}

// ================================================================================
// Get operation
// ================================================================================

error_code_t get_float_array_index(const float_array_t* array,
                                   size_t               index,
                                   float*               out) {
    if (array == NULL || out == NULL) return NULL_POINTER;
    return get_array_index(&array->base, index, out, FLOAT_TYPE);
}

// ================================================================================
// Pop operations
// ================================================================================

error_code_t pop_back_float_array(float_array_t* array, float* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_back_array(&array->base, out, FLOAT_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_front_float_array(float_array_t* array, float* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_front_array(&array->base, out, FLOAT_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_any_float_array(float_array_t* array,
                                 float*         out,
                                 size_t         index) {
    if (array == NULL) return NULL_POINTER;
    return pop_any_array(&array->base, out, index, FLOAT_TYPE);
}

// ================================================================================
// Utility operations
// ================================================================================

error_code_t clear_float_array(float_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return clear_array(&array->base);
}

// --------------------------------------------------------------------------------

error_code_t set_float_array_index(float_array_t* array,
                                   size_t         index,
                                   float          value) {
    if (array == NULL) return NULL_POINTER;
    return set_array_index(&array->base, index, &value, FLOAT_TYPE);
}

// --------------------------------------------------------------------------------

float_array_expect_t copy_float_array(const float_array_t* src,
                                      allocator_vtable_t   alloc_v) {
    if (src == NULL) {
        return (float_array_expect_t){ .has_value = false,
                                       .u.error   = NULL_POINTER };
    }
    return _wrap_expect(copy_array(&src->base, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t concat_float_array(float_array_t*       dst,
                                const float_array_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_array(&dst->base, &src->base);
}

// --------------------------------------------------------------------------------

float_array_expect_t slice_float_array(const float_array_t* src,
                                       size_t               start,
                                       size_t               end,
                                       allocator_vtable_t   alloc_v) {
    if (src == NULL) {
        return (float_array_expect_t){ .has_value = false,
                                       .u.error   = NULL_POINTER };
    }
    return _wrap_expect(slice_array(&src->base, start, end, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t reverse_float_array(float_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return reverse_array(&array->base);
}

// --------------------------------------------------------------------------------

/*
 * Comparator for float. The three-way idiom (va > vb) - (va < vb) is correct
 * for all finite values and signed zeros under IEEE 754. NaN comparisons
 * produce 0 for both sub-expressions, yielding 0 (equal), which gives
 * unspecified sort order — callers must ensure no NaN are present.
 *
 * Subtraction is not used because for floats it can produce -0.0f for equal
 * values and does not return an int, requiring a cast that loses the sign for
 * subnormal differences smaller than INT_MIN.
 */
static int _cmp_float(const void* a, const void* b) {
    float va = *(const float*)a;
    float vb = *(const float*)b;
    return (va > vb) - (va < vb);
}

error_code_t sort_float_array(float_array_t* array, direction_t dir) {
    if (array == NULL) return NULL_POINTER;
    return sort_array(&array->base, _cmp_float, dir);
}

// ================================================================================
// Reduction
// ================================================================================

float_expect_t float_array_min(const float_array_t* array) {
    if (array == NULL)
        return (float_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_min(&array->base, _cmp_float, FLOAT_TYPE);
    if (!idx.has_value)
        return (float_expect_t){ .has_value = false, .u.error = idx.u.error };
    float val = 0.0f;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, FLOAT_TYPE);
    if (err != NO_ERROR)
        return (float_expect_t){ .has_value = false, .u.error = err };
    return (float_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

float_expect_t float_array_max(const float_array_t* array) {
    if (array == NULL)
        return (float_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_max(&array->base, _cmp_float, FLOAT_TYPE);
    if (!idx.has_value)
        return (float_expect_t){ .has_value = false, .u.error = idx.u.error };
    float val = 0.0f;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, FLOAT_TYPE);
    if (err != NO_ERROR)
        return (float_expect_t){ .has_value = false, .u.error = err };
    return (float_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

static void _add_float(void* accum, const void* element) {
    *(float*)accum += *(const float*)element;
}

float_expect_t float_array_sum(const float_array_t* array) {
    if (array == NULL)
        return (float_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    float total = 0.0f;
    error_code_t err = array_sum(&array->base, &total, _add_float, FLOAT_TYPE);
    if (err != NO_ERROR)
        return (float_expect_t){ .has_value = false, .u.error = err };
    return (float_expect_t){ .has_value = true, .u.value = total };
}

// --------------------------------------------------------------------------------

float_array_expect_t cumulative_float_array(const float_array_t* src,
                                            allocator_vtable_t   alloc) {
    if (src == NULL)
        return (float_array_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    array_expect_t r = cumulative_array(&src->base, _add_float, alloc, FLOAT_TYPE);
    if (!r.has_value)
        return (float_array_expect_t){ .has_value = false, .u.error = r.u.error };
    return (float_array_expect_t){ .has_value = true,
                                   .u.value   = (float_array_t*)r.u.value };
}

// ================================================================================
// Search
// ================================================================================

size_expect_t float_array_contains(const float_array_t* array,
                                   float                value,
                                   size_t               start,
                                   size_t               end,
                                   float                abs_tol,
                                   float                rel_tol) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (abs_tol < 0.0f || rel_tol < 0.0f)
        return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (start >= end)
        return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (end > array->base.len)
        return (size_expect_t){ .has_value = false, .u.error = OUT_OF_BOUNDS };

    const float* data = (const float*)array->base.data;
    size_t idx = simd_contains_float(data, start, end, value, abs_tol, rel_tol);

    if (idx == SIZE_MAX)
        return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
    return (size_expect_t){ .has_value = true, .u.value = idx };
}

// --------------------------------------------------------------------------------

size_expect_t float_array_binary_search(float_array_t* array,
                                        float          value,
                                        bool           sort,
                                        float          abs_tol,
                                        float          rel_tol) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (abs_tol < 0.0f || rel_tol < 0.0f)
        return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (array->base.len == 0u)
        return (size_expect_t){ .has_value = false, .u.error = EMPTY };

    if (sort)
        sort_array(&array->base, _cmp_float, FORWARD);

    const float* data = (const float*)array->base.data;
    size_t idx = simd_binary_search_float(data, array->base.len, value,
                                          abs_tol, rel_tol, _cmp_float);
    if (idx == SIZE_MAX)
        return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
    return (size_expect_t){ .has_value = true, .u.value = idx };
}

// --------------------------------------------------------------------------------

bracket_expect_t float_array_binary_bracket(float_array_t* array,
                                            float          value,
                                            bool           sort) {
    if (array == NULL)
        return (bracket_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_bracket_array(&array->base, &value, _cmp_float, sort,
                                FLOAT_TYPE);
}

// ================================================================================
// Introspection
// ================================================================================

size_t float_array_size(const float_array_t* array) {
    if (array == NULL) return 0u;
    return array_size(&array->base);
}

// --------------------------------------------------------------------------------

size_t float_array_alloc(const float_array_t* array) {
    if (array == NULL) return 0u;
    return array_alloc(&array->base);
}

// --------------------------------------------------------------------------------

size_t float_array_data_size(const float_array_t* array) {
    if (array == NULL) return 0u;
    return array_data_size(&array->base);
}

// --------------------------------------------------------------------------------

bool is_float_array_empty(const float_array_t* array) {
    if (array == NULL) return true;
    return is_array_empty(&array->base);
}

// --------------------------------------------------------------------------------

bool is_float_array_full(const float_array_t* array) {
    if (array == NULL) return true;
    return is_array_full(&array->base);
}

// --------------------------------------------------------------------------------

bool is_float_array_ptr(const float_array_t* array, const float* ptr) {
    if (array == NULL) return false;
    return is_array_ptr(&array->base, ptr);
}
// ================================================================================
// ================================================================================
// eof
