// ================================================================================
// ================================================================================
// - File:    c_uint16.c
// - Purpose: This file contains the implementations for funcitons related to 
//            uint16_t data structures.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 27, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_uint16.h"
#include "c_dtypes.h"

#include <inttypes.h>

#if defined(__AVX512BW__)
#  include "simd_avx512_uint16.inl"
#elif defined(__AVX2__)
#  include "simd_avx2_uint16.inl"
#elif defined(__AVX__)
#  include "simd_avx_uint16.inl"
#elif defined(__SSE4_1__)
#  include "simd_sse41_uint16.inl"
#elif defined(__SSSE3__)
#  include "simd_sse3_uint16.inl"
#elif defined(__SSE2__)
#  include "simd_sse2_uint16.inl"
#elif defined(__ARM_FEATURE_SVE2)
#  include "simd_sve2_uint16.inl"
#elif defined(__ARM_FEATURE_SVE)
#  include "simd_sve_uint16.inl"
#elif defined(__ARM_NEON)
#  include "simd_neon_uint16.inl"
#else
#  include "simd_scalar_uint16.inl"
#endif
// ================================================================================ 
// ================================================================================ 

uint16_tensor_expect_t init_uint16_tensor(uint8_t            ndim,
                                          const size_t*      shape,
                                          allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor(ndim, shape, UINT16_TYPE, alloc_v);
    if (!r.has_value)
        return (uint16_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(uint16_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint16_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint16_tensor_t* a = (uint16_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint16_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

uint16_tensor_expect_t init_uint16_array(size_t             capacity,
                                       bool               growth,
                                       allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor_array(capacity, UINT16_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (uint16_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(uint16_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint16_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint16_tensor_t* a = (uint16_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint16_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

void return_uint16_tensor(uint16_tensor_t* a) {
    if (a == NULL) return;

    /* Cache the allocator before return_tensor invalidates the tensor */
    allocator_vtable_t av = a->base->alloc_v;
    return_tensor(a->base);
    av.return_element(av.ctx, a);
}
// -------------------------------------------------------------------------------- 

uint16_tensor_expect_t copy_uint16_tensor(const uint16_tensor_t* src,
                                        allocator_vtable_t*   alloc_v) {
    if (src == NULL)
        return (uint16_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    /* Deep copy the underlying tensor — NULL alloc_v inherits src's allocator */
    tensor_expect_t r = copy_tensor(src->base, alloc_v);
    if (!r.has_value)
        return (uint16_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the copied tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(uint16_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint16_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint16_tensor_t* a = (uint16_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint16_tensor_expect_t){ .has_value = true, .u.value = a };
}
// ================================================================================ 
// ================================================================================ 
// UTILITY OPERATIONS

uint16_tensor_expect_t slice_uint16_tensor_array(const uint16_tensor_t* src,
                                               size_t              start,
                                               size_t              end,
                                               allocator_vtable_t* alloc_v) {
    if (src == NULL)
        return (uint16_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    tensor_expect_t r = slice_tensor_array(src->base, start, end, alloc_v);
    if (!r.has_value)
        return (uint16_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the sliced tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(uint16_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint16_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint16_tensor_t* a = (uint16_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint16_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

error_code_t uint16_tensor_lsearch(const uint16_tensor_t* t,
                                   size_t*                index,
                                   uint16_t               value) {
    if (t == NULL || index == NULL) return NULL_POINTER;
    if (t->base->len == 0u)         return EMPTY;

    size_t result = simd_lsearch_uint16(
        (const uint16_t*)t->base->data,
        t->base->len,
        value
    );

    /* Verify the candidate index rather than trusting the SIZE_MAX
     * sentinel alone — this guards against any platform-specific
     * edge case and makes correctness explicit. */
    if (result == SIZE_MAX) return NOT_FOUND;

    uint16_t found_val = 0u;
    get_uint16_tensor_index(t, result, &found_val);
    if (found_val != value) return NOT_FOUND;

    *index = result;
    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

error_code_t uint16_tensor_bsearch(const uint16_tensor_t* t,
                                  size_t*               index,
                                  uint16_t               value) {
    if (t == NULL || index == NULL) return NULL_POINTER;
    if (t->base->len == 0u)         return EMPTY;

    size_t  low  = 0u;
    size_t  high = t->base->len - 1u;
    uint16_t test = 0u;

    while (low <= high) {
        size_t mid = low + (high - low) / 2u;
        get_uint16_tensor_index(t, mid, &test);

        if (test == value) {
            *index = mid;
            return NO_ERROR;
        } else if (test < value) {
            low = mid + 1u;
        } else {
            if (mid == 0u) break;   /* guard: high = mid-1 would underflow */
            high = mid - 1u;
        }
    }

    return NOT_FOUND;
}
// ================================================================================
// ================================================================================
// eof
