// ================================================================================
// ================================================================================
// - File:    c_int8.c
// - Purpose: This file contains the implementations for funcitons related to 
//            int8_t data structures.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 27, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_int8.h"
#include "c_dtypes.h"

#include <inttypes.h>

   #if defined(__AVX512BW__)
   #  include "simd_avx512_int8.inl"
   #elif defined(__AVX2__)
   #  include "simd_avx2_int8.inl"
   #elif defined(__AVX__)
   #  include "simd_avx_int8.inl"
   #elif defined(__SSE4_1__)
   #  include "simd_sse41_int8.inl"
   #elif defined(__SSSE3__)
   #  include "simd_ssse3_int8.inl"
   #elif defined(__SSE2__)
   #  include "simd_sse2_int8.inl"
   #elif defined(__ARM_FEATURE_SVE2)
   #  include "simd_sve2_int8.inl"
   #elif defined(__ARM_FEATURE_SVE)
   #  include "simd_sve_int8.inl"
   #elif defined(__ARM_NEON)
   #  include "simd_neon_int8.inl"
   #else
   #  include "simd_scalar_int8.inl"
   #endif
// ================================================================================ 
// ================================================================================ 

int8_tensor_expect_t init_int8_tensor(uint8_t            ndim,
                                      const size_t*      shape,
                                      allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor(ndim, shape, INT8_TYPE, alloc_v);
    if (!r.has_value)
        return (int8_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(int8_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (int8_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    int8_tensor_t* a = (int8_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (int8_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

int8_tensor_expect_t init_int8_array(size_t             capacity,
                                       bool               growth,
                                       allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor_array(capacity, INT8_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (int8_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(int8_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (int8_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    int8_tensor_t* a = (int8_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (int8_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

void return_int8_tensor(int8_tensor_t* a) {
    if (a == NULL) return;

    /* Cache the allocator before return_tensor invalidates the tensor */
    allocator_vtable_t av = a->base->alloc_v;
    return_tensor(a->base);
    av.return_element(av.ctx, a);
}
// -------------------------------------------------------------------------------- 

int8_tensor_expect_t copy_int8_tensor(const int8_tensor_t* src,
                                        allocator_vtable_t*   alloc_v) {
    if (src == NULL)
        return (int8_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    /* Deep copy the underlying tensor — NULL alloc_v inherits src's allocator */
    tensor_expect_t r = copy_tensor(src->base, alloc_v);
    if (!r.has_value)
        return (int8_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the copied tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(int8_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (int8_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    int8_tensor_t* a = (int8_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (int8_tensor_expect_t){ .has_value = true, .u.value = a };
}
// ================================================================================ 
// ================================================================================ 
// UTILITY OPERATIONS

int8_tensor_expect_t slice_int8_tensor_array(const int8_tensor_t* src,
                                               size_t              start,
                                               size_t              end,
                                               allocator_vtable_t* alloc_v) {
    if (src == NULL)
        return (int8_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    tensor_expect_t r = slice_tensor_array(src->base, start, end, alloc_v);
    if (!r.has_value)
        return (int8_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the sliced tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(int8_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (int8_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    int8_tensor_t* a = (int8_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (int8_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

error_code_t int8_tensor_lsearch(const int8_tensor_t* t,
                                 size_t*               index,
                                 int8_t               value) {
    if (t == NULL || index == NULL) return NULL_POINTER;
    if (t->base->len == 0u)         return EMPTY;

    int8_t out = 0u;
    for (size_t i = 0u; i < t->base->len; i++) {
        get_int8_tensor_index(t, i, &out);
        if (out == value) {
            *index = i;
            return NO_ERROR;
        }
    }

    return NOT_FOUND;
}
// -------------------------------------------------------------------------------- 

error_code_t int8_tensor_bsearch(const int8_tensor_t* t,
                                  size_t*               index,
                                  int8_t               value) {
    if (t == NULL || index == NULL) return NULL_POINTER;
    if (t->base->len == 0u)         return EMPTY;

    size_t  low  = 0u;
    size_t  high = t->base->len - 1u;
    int8_t test = 0u;

    while (low <= high) {
        size_t mid = low + (high - low) / 2u;
        get_int8_tensor_index(t, mid, &test);

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
// -------------------------------------------------------------------------------- 

bracket_expect_t int8_tensor_bbsearch(const int8_tensor_t* t,
                                       int8_t               value) {
    if (t == NULL)
        return (bracket_expect_t){ .has_value = false,
                                   .u.error   = NULL_POINTER };
    if (t->base->len == 0u)
        return (bracket_expect_t){ .has_value = false,
                                   .u.error   = EMPTY };

    size_t  len   = t->base->len;
    int8_t first = 0u, last = 0u;
    get_int8_tensor_index(t, 0u,       &first);
    get_int8_tensor_index(t, len - 1u, &last);

    if (value < first)
        return (bracket_expect_t){ .has_value = false,
                                   .u.error   = BELOW_RANGE };
    if (value > last)
        return (bracket_expect_t){ .has_value = false,
                                   .u.error   = ABOVE_RANGE };

    size_t  low  = 0u;
    size_t  high = len - 1u;
    int8_t test = 0u;

    while (low <= high) {
        size_t mid = low + (high - low) / 2u;
        get_int8_tensor_index(t, mid, &test);

        if (test == value) {
            return (bracket_expect_t){ .has_value     = true,
                                       .u.value.lower = mid,
                                       .u.value.upper = mid };
        } else if (test < value) {
            low = mid + 1u;
        } else {
            if (mid == 0u) break;
            high = mid - 1u;
        }
    }

    return (bracket_expect_t){ .has_value     = true,
                               .u.value.lower = low - 1u,
                               .u.value.upper = low };
}
// -------------------------------------------------------------------------------- 

bool int8_tensors_equal(const int8_tensor_t* one,
                        const int8_tensor_t* two,
                        bool                 meta) {
    if (!one || !two)                         return false;
    if (!one->base->data || !two->base->data) return false;
    if (one->base->len  != two->base->len)    return false;
    if (one->base->ndim != two->base->ndim)   return false;

    if (meta) {
        for (uint8_t d = 0u; d < one->base->ndim; d++) {
            if (one->base->shape[d] != two->base->shape[d]) return false;
        }
        if (one->base->alloc  != two->base->alloc)  return false;
        if (one->base->mode   != two->base->mode)   return false;
        if (one->base->growth != two->base->growth) return false;
    }

    return memcmp(one->base->data,
                  two->base->data,
                  sizeof(int8_t) * one->base->len) == 0;
}
// -------------------------------------------------------------------------------- 

error_code_t min_int8_tensor(const int8_tensor_t* t, int8_t* value) {
    if (t == NULL || value == NULL)  return NULL_POINTER;
    if (t->base == NULL)            return NULL_POINTER;
    if (t->base->data == NULL)      return EMPTY;
    if (t->base->len == 0u)         return EMPTY;
 
    *value = INT8_MAX;
    return simd_min_int8((const int8_t*)t->base->data, t->base->len, value);
}
// ================================================================================
// ================================================================================
// eof
