// ================================================================================
// ================================================================================
// - File:    c_ldouble.c
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

#include "c_ldouble.h"
#include "c_dtypes.h"
// ================================================================================ 
// ================================================================================ 

ldouble_tensor_expect_t init_ldouble_tensor(uint8_t            ndim,
                                            const size_t*      shape,
                                            allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor(ndim, shape, LDOUBLE_TYPE, alloc_v);
    if (!r.has_value)
        return (ldouble_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(ldouble_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (ldouble_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    ldouble_tensor_t* a = (ldouble_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (ldouble_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

ldouble_tensor_expect_t init_ldouble_array(size_t             capacity,
                                           bool               growth,
                                           allocator_vtable_t alloc_v) {
    /* Allocate the underlying tensor */
    tensor_expect_t r = init_tensor_array(capacity, LDOUBLE_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (ldouble_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = alloc_v.allocate(alloc_v.ctx,
                                             sizeof(ldouble_tensor_t),
                                             true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (ldouble_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    ldouble_tensor_t* a = (ldouble_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (ldouble_tensor_expect_t){ .has_value = true, .u.value = a };
}
// -------------------------------------------------------------------------------- 

void return_ldouble_tensor(ldouble_tensor_t* a) {
    if (a == NULL) return;

    /* Cache the allocator before return_tensor invalidates the tensor */
    allocator_vtable_t av = a->base->alloc_v;
    return_tensor(a->base);
    av.return_element(av.ctx, a);
}
// -------------------------------------------------------------------------------- 

ldouble_tensor_expect_t copy_ldouble_tensor(const ldouble_tensor_t* src,
                                            allocator_vtable_t*   alloc_v) {
    if (src == NULL)
        return (ldouble_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    /* Deep copy the underlying tensor — NULL alloc_v inherits src's allocator */
    tensor_expect_t r = copy_tensor(src->base, alloc_v);
    if (!r.has_value)
        return (ldouble_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the copied tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(ldouble_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (ldouble_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    ldouble_tensor_t* a = (ldouble_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (ldouble_tensor_expect_t){ .has_value = true, .u.value = a };
}
// ================================================================================ 
// ================================================================================ 
// UTILITY OPERATIONS

ldouble_tensor_expect_t slice_ldouble_tensor_array(const ldouble_tensor_t* src,
                                                   size_t              start,
                                                   size_t              end,
                                                   allocator_vtable_t* alloc_v) {
    if (src == NULL)
        return (ldouble_tensor_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };

    tensor_expect_t r = slice_tensor_array(src->base, start, end, alloc_v);
    if (!r.has_value)
        return (ldouble_tensor_expect_t){ .has_value = false,
                                        .u.error   = r.u.error };

    /* Read the allocator actually used from the sliced tensor so the wrapper
     * struct lands on the same allocator regardless of whether alloc_v
     * was NULL or not. */
    allocator_vtable_t av = r.u.value->alloc_v;

    /* Allocate the thin wrapper struct */
    void_ptr_expect_t wr = av.allocate(av.ctx, sizeof(ldouble_tensor_t), true);
    if (!wr.has_value) {
        return_tensor(r.u.value);
        return (ldouble_tensor_expect_t){ .has_value = false,
                                        .u.error   = BAD_ALLOC };
    }

    ldouble_tensor_t* a = (ldouble_tensor_t*)wr.u.value;
    a->base = r.u.value;

    return (ldouble_tensor_expect_t){ .has_value = true, .u.value = a };
}
// ================================================================================
// ================================================================================
// eof
