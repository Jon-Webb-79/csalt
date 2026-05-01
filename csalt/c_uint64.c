// ================================================================================
// ================================================================================
// - File:    c_uint64.c
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

#include "c_uint64.h"
#include "c_dtypes.h"

#include <inttypes.h>

#if defined(__AVX512BW__)
#  include "simd_avx512_uint64.inl"
#elif defined(__AVX2__)
#  include "simd_avx2_uint64.inl"
#elif defined(__AVX__)
#  include "simd_avx_uint64.inl"
#elif defined(__SSE4_1__)
#  include "simd_sse41_uint64.inl"
#elif defined(__SSSE3__)
#  include "simd_sse3_uint64.inl"
#elif defined(__SSE2__)
#  include "simd_sse2_uint64.inl"
#elif defined(__ARM_FEATURE_SVE2)
#  include "simd_sve2_uint64.inl"
#elif defined(__ARM_FEATURE_SVE)
#  include "simd_sve_uint64.inl"
#elif defined(__ARM_NEON)
#  include "simd_neon_uint64.inl"
#else
#  include "simd_scalar_uint64.inl"
#endif
// ================================================================================ 
// ================================================================================ 

uint64_tensor_expect_t init_uint64_tensor(uint8_t            ndim,
                                          const size_t*      shape,
                                          allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor(ndim, shape, UINT64_TYPE, alloc_v);
    if (!r.has_value)
        return (uint64_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(uint64_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint64_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint64_tensor_t* a = (uint64_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint64_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

uint64_tensor_expect_t init_uint64_array(size_t             capacity,
                                       bool               growth,
                                       allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor_array(capacity, UINT64_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (uint64_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(uint64_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint64_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint64_tensor_t* a = (uint64_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint64_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

void return_uint64_tensor(uint64_tensor_t* a) {
    if (a == NULL) return;

    /* Cache the allocator before return_tensor invalidates the tensor */
    allocator_vtable_t av = a->base->alloc_v;
    return_tensor(a->base);
    av.return_element(av.ctx, a);
}
// -------------------------------------------------------------------------------- 

uint64_tensor_expect_t copy_uint64_tensor(const uint64_tensor_t* src,
                                        allocator_vtable_t*   alloc_v) {
    if (src == NULL)
        return (uint64_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    /* Deep copy the underlying tensor — NULL alloc_v inherits src's allocator */
    tensor_expect_t r = copy_tensor(src->base, alloc_v);
    if (!r.has_value)
        return (uint64_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the copied tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(uint64_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint64_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint64_tensor_t* a = (uint64_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint64_tensor_expect_t){ .has_value = true, .u.value = a };
}
// ================================================================================ 
// ================================================================================ 
// UTILITY OPERATIONS

uint64_tensor_expect_t slice_uint64_tensor_array(const uint64_tensor_t* src,
                                               size_t              start,
                                               size_t              end,
                                               allocator_vtable_t* alloc_v) {
    if (src == NULL)
        return (uint64_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    tensor_expect_t r = slice_tensor_array(src->base, start, end, alloc_v);
    if (!r.has_value)
        return (uint64_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the sliced tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(uint64_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint64_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint64_tensor_t* a = (uint64_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint64_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

error_code_t uint64_tensor_lsearch(const uint64_tensor_t* t,
                                   size_t*                index,
                                   uint64_t               value) {
    if (t == NULL || index == NULL) return NULL_POINTER;
    if (t->base->len == 0u)         return EMPTY;

    size_t result = simd_lsearch_uint64(
        (const uint64_t*)t->base->data,
        t->base->len,
        value
    );

    /* Verify the candidate index rather than trusting the SIZE_MAX
     * sentinel alone — this guards against any platform-specific
     * edge case and makes correctness explicit. */
    if (result == SIZE_MAX) return NOT_FOUND;

    uint64_t found_val = 0u;
    get_uint64_tensor_index(t, result, &found_val);
    if (found_val != value) return NOT_FOUND;

    *index = result;
    return NO_ERROR;
}
// ================================================================================
// ================================================================================
// eof
