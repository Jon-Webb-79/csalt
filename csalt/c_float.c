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
//
#include "c_float.h"
#include "c_dtypes.h"

#if defined(__AVX512BW__)
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

float_tensor_expect_t init_float_tensor(uint8_t            ndim,
                                        const size_t*      shape,
                                        allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor(ndim, shape, FLOAT_TYPE, alloc_v);
    if (!r.has_value)
        return (float_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(float_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (float_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    float_tensor_t* a = (float_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (float_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

float_tensor_expect_t init_float_array(size_t             capacity,
                                       bool               growth,
                                       allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor_array(capacity, FLOAT_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (float_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(float_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (float_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    float_tensor_t* a = (float_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (float_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

void return_float_tensor(float_tensor_t* a) {
    if (a == NULL) return;

    /* Cache the allocator before return_tensor invalidates the tensor */
    allocator_vtable_t av = a->base->alloc_v;
    return_tensor(a->base);
    av.return_element(av.ctx, a);
}
// -------------------------------------------------------------------------------- 

float_tensor_expect_t copy_float_tensor(const float_tensor_t* src,
                                        allocator_vtable_t*   alloc_v) {
    if (src == NULL)
        return (float_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    /* Deep copy the underlying tensor — NULL alloc_v inherits src's allocator */
    tensor_expect_t r = copy_tensor(src->base, alloc_v);
    if (!r.has_value)
        return (float_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the copied tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(float_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (float_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    float_tensor_t* a = (float_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (float_tensor_expect_t){ .has_value = true, .u.value = a };
}
// ================================================================================ 
// ================================================================================ 
// UTILITY OPERATIONS

float_tensor_expect_t slice_float_tensor_array(const float_tensor_t* src,
                                               size_t              start,
                                               size_t              end,
                                               allocator_vtable_t* alloc_v) {
    if (src == NULL)
        return (float_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    tensor_expect_t r = slice_tensor_array(src->base, start, end, alloc_v);
    if (!r.has_value)
        return (float_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the sliced tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(float_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (float_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    float_tensor_t* a = (float_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (float_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

error_code_t float_tensor_lsearch(const float_tensor_t* t,
                                  size_t*               index,
                                  float                 value,
                                  float                 tolerance) {
    if (t == NULL || index == NULL) return NULL_POINTER;
    if (t->base->len == 0u)         return EMPTY;

    size_t result = simd_lsearch_float(
        (const float*)t->base->data,
        t->base->len,
        value,
        tolerance
    );

    if (result == SIZE_MAX) return NOT_FOUND;

    /* Verify the candidate satisfies the tolerance condition — confirms
     * correctness independently of the SIMD helper's sentinel convention. */
    float found_val = 0.0f;
    get_float_tensor_index(t, result, &found_val);
    float diff = found_val - value;
    if (diff < 0.0f) diff = -diff;
    if (diff > tolerance) return NOT_FOUND;

    *index = result;
    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

error_code_t float_tensor_bsearch(const float_tensor_t* t,
                                  size_t*               index,
                                  float                 value,
                                  float                 tolerance) {
    if (t == NULL || index == NULL) return NULL_POINTER;
    if (t->base->len == 0u)         return EMPTY;

    size_t len = t->base->len;

    /* Binary search for the leftmost position where an element could
     * be within tolerance — i.e. the insertion point of
     * (value - tolerance).  After the loop, low is the index of the
     * first element >= (value - tolerance). */
    float  lower_bound = value - tolerance;
    size_t low  = 0u;
    size_t high = len;   /* half-open upper bound */

    while (low < high) {
        size_t mid = low + (high - low) / 2u;
        float  elem = 0.0f;
        get_float_tensor_index(t, mid, &elem);

        if (elem < lower_bound) {
            low = mid + 1u;
        } else {
            high = mid;
        }
    }

    /* Linear scan forward from low: any element here could be within
     * tolerance.  Stop as soon as an element exceeds (value + tolerance)
     * since the array is sorted and no further elements can match. */
    float upper_bound = value + tolerance;

    for (size_t i = low; i < len; i++) {
        float elem = 0.0f;
        get_float_tensor_index(t, i, &elem);

        if (elem > upper_bound) break;

        float diff = elem - value;
        if (diff < 0.0f) diff = -diff;
        if (diff <= tolerance) {
            *index = i;
            return NO_ERROR;
        }
    }

    return NOT_FOUND;
}
// ================================================================================
// ================================================================================
// eof
