// ================================================================================
// ================================================================================
// - File:    c_int16.c
// - Purpose: This file contains the implementations for funcitons related to 
//            int16_t data structures.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 27, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#include "c_int16.h"
#include "c_dtypes.h"

#include <inttypes.h>
// ================================================================================ 
// ================================================================================ 

int16_tensor_expect_t init_int16_tensor(int16_t            ndim,
                                        const size_t*      shape,
                                        allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor(ndim, shape, INT16_TYPE, alloc_v);
    if (!r.has_value)
        return (int16_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(int16_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (int16_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    int16_tensor_t* a = (int16_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (int16_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

int16_tensor_expect_t init_int16_array(size_t             capacity,
                                       bool               growth,
                                       allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor_array(capacity, INT16_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (int16_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(int16_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (int16_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    int16_tensor_t* a = (int16_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (int16_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

void return_int16_tensor(int16_tensor_t* a) {
    if (a == NULL) return;

    /* Cache the allocator before return_tensor invalidates the tensor */
    allocator_vtable_t av = a->base->alloc_v;
    return_tensor(a->base);
    av.return_element(av.ctx, a);
}
// -------------------------------------------------------------------------------- 

int16_tensor_expect_t copy_int16_tensor(const int16_tensor_t* src,
                                        allocator_vtable_t*   alloc_v) {
    if (src == NULL)
        return (int16_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    /* Deep copy the underlying tensor — NULL alloc_v inherits src's allocator */
    tensor_expect_t r = copy_tensor(src->base, alloc_v);
    if (!r.has_value)
        return (int16_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the copied tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(int16_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (int16_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    int16_tensor_t* a = (int16_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (int16_tensor_expect_t){ .has_value = true, .u.value = a };
}
// ================================================================================ 
// ================================================================================ 
// UTILITY OPERATIONS

int16_tensor_expect_t slice_int16_tensor_array(const int16_tensor_t* src,
                                               size_t              start,
                                               size_t              end,
                                               allocator_vtable_t* alloc_v) {
    if (src == NULL)
        return (int16_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    tensor_expect_t r = slice_tensor_array(src->base, start, end, alloc_v);
    if (!r.has_value)
        return (int16_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the sliced tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(int16_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (int16_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    int16_tensor_t* a = (int16_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (int16_tensor_expect_t){ .has_value = true, .u.value = a };
}
// ================================================================================
// ================================================================================
// eof
