// ================================================================================
// ================================================================================
// - File:    c_uint32.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 27, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_uint32.h"
#include "c_dtypes.h"

#include <inttypes.h>

#if defined(__AVX512BW__)
#  include "simd_avx512_uint32.inl"
#elif defined(__AVX2__)
#  include "simd_avx2_uint32.inl"
#elif defined(__AVX__)
#  include "simd_avx_uint32.inl"
#elif defined(__SSE4_1__)
#  include "simd_sse41_uint32.inl"
#elif defined(__SSSE3__)
#  include "simd_sse3_uint32.inl"
#elif defined(__SSE2__)
#  include "simd_sse2_uint32.inl"
#elif defined(__ARM_FEATURE_SVE2)
#  include "simd_sve2_uint32.inl"
#elif defined(__ARM_FEATURE_SVE)
#  include "simd_sve_uint32.inl"
#elif defined(__ARM_NEON)
#  include "simd_neon_uint32.inl"
#else
#  include "simd_scalar_uint32.inl"
#endif
// ================================================================================ 
// ================================================================================ 

uint32_tensor_expect_t init_uint32_tensor(uint8_t            ndim,
                                          const size_t*      shape,
                                          allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor(ndim, shape, UINT32_TYPE, alloc_v);
    if (!r.has_value)
        return (uint32_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(uint32_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint32_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint32_tensor_t* a = (uint32_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint32_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

uint32_tensor_expect_t init_uint32_array(size_t             capacity,
                                       bool               growth,
                                       allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor_array(capacity, UINT32_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (uint32_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(uint32_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint32_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint32_tensor_t* a = (uint32_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint32_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

void return_uint32_tensor(uint32_tensor_t* a) {
    if (a == NULL) return;

    /* Cache the allocator before return_tensor invalidates the tensor */
    allocator_vtable_t av = a->base->alloc_v;
    return_tensor(a->base);
    av.return_element(av.ctx, a);
}
// -------------------------------------------------------------------------------- 

uint32_tensor_expect_t copy_uint32_tensor(const uint32_tensor_t* src,
                                        allocator_vtable_t*   alloc_v) {
    if (src == NULL)
        return (uint32_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    /* Deep copy the underlying tensor — NULL alloc_v inherits src's allocator */
    tensor_expect_t r = copy_tensor(src->base, alloc_v);
    if (!r.has_value)
        return (uint32_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the copied tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(uint32_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint32_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint32_tensor_t* a = (uint32_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint32_tensor_expect_t){ .has_value = true, .u.value = a };
}
// ================================================================================ 
// ================================================================================ 
// UTILITY OPERATIONS

uint32_tensor_expect_t slice_uint32_tensor_array(const uint32_tensor_t* src,
                                               size_t              start,
                                               size_t              end,
                                               allocator_vtable_t* alloc_v) {
    if (src == NULL)
        return (uint32_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    tensor_expect_t r = slice_tensor_array(src->base, start, end, alloc_v);
    if (!r.has_value)
        return (uint32_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the sliced tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(uint32_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint32_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint32_tensor_t* a = (uint32_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint32_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

error_code_t uint32_tensor_lsearch(const uint32_tensor_t* t,
                                   size_t*                index,
                                   uint32_t               value) {
    if (t == NULL || index == NULL) return NULL_POINTER;
    if (t->base->len == 0u)         return EMPTY;

    size_t result = simd_lsearch_uint32(
        (const uint32_t*)t->base->data,
        t->base->len,
        value
    );

    /* Verify the candidate index rather than trusting the SIZE_MAX
     * sentinel alone — this guards against any platform-specific
     * edge case and makes correctness explicit. */
    if (result == SIZE_MAX) return NOT_FOUND;

    uint32_t found_val = 0u;
    get_uint32_tensor_index(t, result, &found_val);
    if (found_val != value) return NOT_FOUND;

    *index = result;
    return NO_ERROR;
}
// ================================================================================
// ================================================================================
// eof
