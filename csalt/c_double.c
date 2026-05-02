// ================================================================================
// ================================================================================
// - File:    c_double.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 08, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_double.h"
#include "c_dtypes.h"

#if defined(__AVX512BW__)
#  include "simd_avx512_double.inl"
#elif defined(__AVX2__)
#  include "simd_avx2_double.inl"
#elif defined(__AVX__)
#  include "simd_avx_double.inl"
#elif defined(__SSE4_1__)
#  include "simd_sse41_double.inl"
#elif defined(__SSSE3__)
#  include "simd_sse3_double.inl"
#elif defined(__SSE2__)
#  include "simd_sse2_double.inl"
#elif defined(__ARM_FEATURE_SVE2)
#  include "simd_sve2_double.inl"
#elif defined(__ARM_FEATURE_SVE)
#  include "simd_sve_double.inl"
#elif defined(__ARM_NEON)
#  include "simd_neon_double.inl"
#else
#  include "simd_scalar_double.inl"
#endif

// ================================================================================ 
// ================================================================================ 

double_tensor_expect_t init_double_tensor(uint8_t            ndim,
                                        const size_t*      shape,
                                        allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor(ndim, shape, DOUBLE_TYPE, alloc_v);
    if (!r.has_value)
        return (double_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(double_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (double_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    double_tensor_t* a = (double_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (double_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

double_tensor_expect_t init_double_array(size_t             capacity,
                                       bool               growth,
                                       allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor_array(capacity, DOUBLE_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (double_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(double_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (double_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    double_tensor_t* a = (double_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (double_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

void return_double_tensor(double_tensor_t* a) {
    if (a == NULL) return;

    /* Cache the allocator before return_tensor invalidates the tensor */
    allocator_vtable_t av = a->base->alloc_v;
    return_tensor(a->base);
    av.return_element(av.ctx, a);
}
// -------------------------------------------------------------------------------- 

double_tensor_expect_t copy_double_tensor(const double_tensor_t* src,
                                        allocator_vtable_t*   alloc_v) {
    if (src == NULL)
        return (double_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    /* Deep copy the underlying tensor — NULL alloc_v inherits src's allocator */
    tensor_expect_t r = copy_tensor(src->base, alloc_v);
    if (!r.has_value)
        return (double_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the copied tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(double_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (double_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    double_tensor_t* a = (double_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (double_tensor_expect_t){ .has_value = true, .u.value = a };
}
// ================================================================================ 
// ================================================================================ 
// UTILITY OPERATIONS

double_tensor_expect_t slice_double_tensor_array(const double_tensor_t* src,
                                               size_t              start,
                                               size_t              end,
                                               allocator_vtable_t* alloc_v) {
    if (src == NULL)
        return (double_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    tensor_expect_t r = slice_tensor_array(src->base, start, end, alloc_v);
    if (!r.has_value)
        return (double_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the sliced tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(double_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (double_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    double_tensor_t* a = (double_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (double_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

error_code_t double_tensor_lsearch(const double_tensor_t* t,
                                  size_t*               index,
                                  double                 value,
                                  double                 tolerance) {
    if (t == NULL || index == NULL) return NULL_POINTER;
    if (t->base->len == 0u)         return EMPTY;

    size_t result = simd_lsearch_double(
        (const double*)t->base->data,
        t->base->len,
        value,
        tolerance
    );

    if (result == SIZE_MAX) return NOT_FOUND;

    /* Verify the candidate satisfies the tolerance condition — confirms
     * correctness independently of the SIMD helper's sentinel convention. */
    double found_val = 0.0f;
    get_double_tensor_index(t, result, &found_val);
    double diff = found_val - value;
    if (diff < 0.0f) diff = -diff;
    if (diff > tolerance) return NOT_FOUND;

    *index = result;
    return NO_ERROR;
}
// ================================================================================
// ================================================================================
// eof
