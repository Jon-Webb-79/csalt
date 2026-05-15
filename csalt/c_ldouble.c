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

#include <math.h>

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
// -------------------------------------------------------------------------------- 

error_code_t ldouble_tensor_lsearch(const ldouble_tensor_t* t,
                                    size_t*                 index,
                                    long double             value,
                                    long double             tolerance) {
    if (t == NULL || index == NULL) return NULL_POINTER;
    if (t->base->len == 0u)         return EMPTY;

    const long double* data = (const long double*)t->base->data;
    size_t             len  = t->base->len;

    for (size_t i = 0u; i < len; i++) {
        if (fabsl(data[i] - value) <= tolerance) {
            *index = i;
            return NO_ERROR;
        }
    }

    return NOT_FOUND;
}
// -------------------------------------------------------------------------------- 

error_code_t ldouble_tensor_bsearch(const ldouble_tensor_t* t,
                                    size_t*                 index,
                                    long double             value,
                                    long double             tolerance) {
    if (t == NULL || index == NULL) return NULL_POINTER;
    if (t->base->len == 0u)         return EMPTY;

    size_t len = t->base->len;

    long double lower_bound = value - tolerance;
    size_t      low         = 0u;
    size_t      high        = len;

    while (low < high) {
        size_t      mid  = low + (high - low) / 2u;
        long double elem = 0.0L;
        get_ldouble_tensor_index(t, mid, &elem);
        if (elem < lower_bound) {
            low = mid + 1u;
        } else {
            high = mid;
        }
    }

    long double upper_bound = value + tolerance;

    for (size_t i = low; i < len; i++) {
        long double elem = 0.0L;
        get_ldouble_tensor_index(t, i, &elem);
        if (elem > upper_bound) break;
        long double diff = elem - value;
        if (diff < 0.0L) diff = -diff;
        if (diff <= tolerance) {
            *index = i;
            return NO_ERROR;
        }
    }

    return NOT_FOUND;
}
// -------------------------------------------------------------------------------- 

bracket_expect_t ldouble_tensor_bbsearch(const ldouble_tensor_t* t,
                                         long double             value,
                                         long double             tolerance) {
    if (t == NULL)
        return (bracket_expect_t){ .has_value = false,
                                   .u.error   = NULL_POINTER };
    if (t->base->len == 0u)
        return (bracket_expect_t){ .has_value = false,
                                   .u.error   = EMPTY };

    if (value != value)
        return (bracket_expect_t){ .has_value = false,
                                   .u.error   = DOMAIN_ERROR };

    size_t      len   = t->base->len;
    long double first = 0.0L, last = 0.0L;
    get_ldouble_tensor_index(t, 0u,       &first);
    get_ldouble_tensor_index(t, len - 1u, &last);

    if (value < first - tolerance)
        return (bracket_expect_t){ .has_value = false,
                                   .u.error   = BELOW_RANGE };
    if (value > last + tolerance)
        return (bracket_expect_t){ .has_value = false,
                                   .u.error   = ABOVE_RANGE };

    size_t      low  = 0u;
    size_t      high = len - 1u;
    long double test = 0.0L;

    while (low <= high) {
        size_t      mid  = low + (high - low) / 2u;
        get_ldouble_tensor_index(t, mid, &test);

        long double diff = test - value;
        if (diff < 0.0L) diff = -diff;

        if (diff <= tolerance) {
            while (mid > 0u) {
                long double prev = 0.0L;
                get_ldouble_tensor_index(t, mid - 1u, &prev);
                long double prev_diff = prev - value;
                if (prev_diff < 0.0L) prev_diff = -prev_diff;
                if (prev_diff > tolerance) break;
                mid--;
            }
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

    size_t lower = (low > 0u)  ? low - 1u : 0u;
    size_t upper = (low < len) ? low : len - 1u;
    return (bracket_expect_t){ .has_value     = true,
                               .u.value.lower = lower,
                               .u.value.upper = upper };
}
// -------------------------------------------------------------------------------- 

bool ldouble_tensors_equal(const ldouble_tensor_t* one,
                           const ldouble_tensor_t* two,
                           long double             tolerance,
                           bool                    meta) {
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

    const long double* a   = (const long double*)one->base->data;
    const long double* b   = (const long double*)two->base->data;
    size_t             len = one->base->len;

    /* Same pointer — trivially equal */
    if (a == b) return true;

    for (size_t i = 0u; i < len; i++) {
        long double diff = a[i] - b[i];
        if (diff != diff)      return false;   /* NaN check */
        if (diff < 0.0L)       diff = -diff;
        if (diff > tolerance)  return false;
    }
    return true;
}
// ================================================================================
// ================================================================================
// eof
