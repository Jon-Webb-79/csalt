// ================================================================================
// ================================================================================
// - File:    c_uint8.c
// - Purpose: This file contains the implementations for funcitons related to 
//            uint8_t data structures.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 23, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_uint8.h"
#include "c_dtypes.h"

#include <inttypes.h>
// ================================================================================ 
// ================================================================================ 

uint8_tensor_expect_t init_uint8_tensor(uint8_t            ndim,
                                        const size_t*      shape,
                                        allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor(ndim, shape, UINT8_TYPE, alloc_v);
    if (!r.has_value)
        return (uint8_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(uint8_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint8_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint8_tensor_t* a = (uint8_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint8_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

uint8_tensor_expect_t init_uint8_array(size_t             capacity,
                                       bool               growth,
                                       allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor_array(capacity, UINT8_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (uint8_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(uint8_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint8_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint8_tensor_t* a = (uint8_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint8_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

void return_uint8_tensor(uint8_tensor_t* a) {
    if (a == NULL) return;

    /* Cache the allocator before return_tensor invalidates the tensor */
    allocator_vtable_t av = a->base->alloc_v;
    return_tensor(a->base);
    av.return_element(av.ctx, a);
}
// -------------------------------------------------------------------------------- 

uint8_tensor_expect_t copy_uint8_tensor(const uint8_tensor_t* src,
                                        allocator_vtable_t*   alloc_v) {
    if (src == NULL)
        return (uint8_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    /* Deep copy the underlying tensor — NULL alloc_v inherits src's allocator */
    tensor_expect_t r = copy_tensor(src->base, alloc_v);
    if (!r.has_value)
        return (uint8_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the copied tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(uint8_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint8_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint8_tensor_t* a = (uint8_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint8_tensor_expect_t){ .has_value = true, .u.value = a };
}
// ================================================================================ 
// ================================================================================ 
// UTILITY OPERATIONS

uint8_tensor_expect_t slice_uint8_tensor_array(const uint8_tensor_t* src,
                                               size_t              start,
                                               size_t              end,
                                               allocator_vtable_t* alloc_v) {
    if (src == NULL)
        return (uint8_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    tensor_expect_t r = slice_tensor_array(src->base, start, end, alloc_v);
    if (!r.has_value)
        return (uint8_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the sliced tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(uint8_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (uint8_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    uint8_tensor_t* a = (uint8_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (uint8_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

error_code_t uint8_tensor_lsearch(const uint8_tensor_t* t,
                                  size_t*               index,
                                  uint8_t               value) {
    if (t == NULL || index == NULL) return NULL_POINTER;
    if (t->base->len == 0u)         return EMPTY;

    uint8_t out = 0u;
    for (size_t i = 0u; i < t->base->len; i++) {
        get_uint8_tensor_index(t, i, &out);
        if (out == value) {
            *index = i;
            return NO_ERROR;
        }
    }

    return NOT_FOUND;
}
// -------------------------------------------------------------------------------- 

error_code_t uint8_tensor_bsearch(const uint8_tensor_t* t,
                                  size_t*               index,
                                  uint8_t               value) {
    if (t == NULL || index == NULL) return NULL_POINTER;
    if (t->base->len == 0u)         return EMPTY;

    size_t  low  = 0u;
    size_t  high = t->base->len - 1u;
    uint8_t test = 0u;

    while (low <= high) {
        size_t mid = low + (high - low) / 2u;
        get_uint8_tensor_index(t, mid, &test);

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

bracket_expect_t uint8_tensor_bbsearch(const uint8_tensor_t* t,
                                       uint8_t               value) {
    if (t == NULL)
        return (bracket_expect_t){ .has_value = false,
                                   .u.error   = NULL_POINTER };
    if (t->base->len == 0u)
        return (bracket_expect_t){ .has_value = false,
                                   .u.error   = EMPTY };

    size_t  len   = t->base->len;
    uint8_t first = 0u, last = 0u;
    get_uint8_tensor_index(t, 0u,       &first);
    get_uint8_tensor_index(t, len - 1u, &last);

    if (value < first)
        return (bracket_expect_t){ .has_value = false,
                                   .u.error   = BELOW_RANGE };
    if (value > last)
        return (bracket_expect_t){ .has_value = false,
                                   .u.error   = ABOVE_RANGE };

    size_t  low  = 0u;
    size_t  high = len - 1u;
    uint8_t test = 0u;

    while (low <= high) {
        size_t mid = low + (high - low) / 2u;
        get_uint8_tensor_index(t, mid, &test);

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

// ================================================================================
// ================================================================================
// eof
