// ================================================================================
// ================================================================================
// - File:    dtypes.c
// - Purpose: This file contains implementations for functions that are used 
//            for an extensible tensor data structure
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 22, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_tensor.h"
// ================================================================================ 
// ================================================================================ 

// ================================================================================ 
// ================================================================================ 
// Internal growth strategy
//
// Tiered to avoid runaway allocation at large sizes while maintaining fast
// ramp-up at small sizes:
//   0 elements       -> 1
//   < 1024 elements  -> 2x
//   < 8192 elements  -> 1.5x  (alloc + alloc/2)
//   < 65536 elements -> 1.25x (alloc + alloc/4)
//   >= 65536 elements -> linear increment of 256
// ================================================================================

static size_t _compute_new_alloc(size_t current) {
    if (current == 0u)    return 1u;
    if (current < 1024u)  return current * 2u;
    if (current < 8192u)  return current + current / 2u;
    if (current < 65536u) return current + current / 4u;
    return current + 256u;
}

// ================================================================================
// Internal grow helper
//
// Reallocates the data buffer to new_alloc elements using the cached data_size.
// Returns NO_ERROR on success. On failure the array is left untouched.
// ================================================================================

static error_code_t _grow_array(tensor_t* t, size_t new_alloc) {
    /* Overflow guard: new_alloc * data_size must not wrap */
    if (new_alloc > SIZE_MAX / t->data_size) return LENGTH_OVERFLOW;

    size_t old_bytes = t->alloc * t->data_size;
    size_t new_bytes = new_alloc    * t->data_size;

    void_ptr_expect_t result = t->alloc_v.reallocate(
        t->alloc_v.ctx,
        t->data,
        old_bytes,
        new_bytes,
        false
    );

    if (result.has_value == false) return OUT_OF_MEMORY;

    t->data  = (uint8_t*)result.u.value;
    t->alloc = new_alloc;
    return NO_ERROR;
}

// ================================================================================
// Internal capacity helper
//
// Ensures at least one free slot exists, growing if permitted.
// Returns NO_ERROR when there is room to write, or an error code otherwise.
// ================================================================================

static error_code_t _ensure_capacity(tensor_t* t) {
    if (t->len < t->alloc) return NO_ERROR;

    if (t->growth == false)              return CAPACITY_OVERFLOW;
    if (t->alloc_v.reallocate == NULL)   return CAPACITY_OVERFLOW;

    size_t new_alloc = _compute_new_alloc(t->alloc);
    return _grow_array(t, new_alloc);
}
// ================================================================================ 
// ================================================================================ 
// INITIALIZATION AND TEARDOWN

tensor_expect_t init_tensor(uint8_t            ndim,
                            const size_t*      shape,
                            dtype_id_t         dtype,
                            allocator_vtable_t alloc_v) {

    // ---- input validation ------------------------------------------------
    if (alloc_v.allocate == NULL || shape == NULL)
        return (tensor_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (ndim == 0u)
        return (tensor_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (dtype == UNKNOWN_TYPE)
        return (tensor_expect_t){ .has_value = false, .u.error = INVALID_ARG };

    // ---- dtype registry --------------------------------------------------
    if (init_dtype_registry() == false)
        return (tensor_expect_t){ .has_value = false, .u.error = ILLEGAL_STATE };

    const dtype_t* desc = lookup_dtype(dtype);
    if (desc == NULL)
        return (tensor_expect_t){ .has_value = false, .u.error = TYPE_MISMATCH };

    size_t data_size = desc->data_size;

    // ---- compute total element count with overflow guard -----------------
    size_t len = 1u;
    for (size_t i = 0u; i < ndim; i++) {
        if (shape[i] == 0u)
            return (tensor_expect_t){ .has_value = false, .u.error = INVALID_ARG };
        if (len > SIZE_MAX / shape[i])
            return (tensor_expect_t){ .has_value = false, .u.error = LENGTH_OVERFLOW };
        len *= shape[i];
    }

    // ---- guard against large user-defined dtypes -------------------------
    if (len > SIZE_MAX / data_size)
        return (tensor_expect_t){ .has_value = false, .u.error = LENGTH_OVERFLOW };

    // ---- allocate struct + FAM (shape[] and strides[] inline) ------------
    size_t header_bytes = sizeof(tensor_t) + 2u * ndim * sizeof(size_t);

    void_ptr_expect_t struct_result = alloc_v.allocate(alloc_v.ctx, header_bytes, true);
    if (struct_result.has_value == false)
        return (tensor_expect_t){ .has_value = false, .u.error = BAD_ALLOC };

    tensor_t* t = (tensor_t*)struct_result.u.value;

    // ---- wire shape and strides pointers into the FAM --------------------
    t->shape   = t->meta;
    t->strides = t->meta + ndim;

    // ---- populate shape and compute C-order (row-major) strides ----------
    for (size_t i = 0u; i < ndim; i++)
        t->shape[i] = shape[i];

    t->strides[ndim - 1u] = data_size;
    for (size_t i = ndim - 1u; i > 0u; i--)
        t->strides[i - 1u] = t->strides[i] * t->shape[i];

    // ---- allocate and zero-initialise data buffer ------------------------
    // TENSOR_STRUCT: all slots are considered live at init time so the
    // buffer must be zeroed — there is no push_back mechanism to signal which
    // slots have been written.
    void_ptr_expect_t data_result = alloc_v.allocate(alloc_v.ctx, len * data_size, true);
    if (data_result.has_value == false) {
        alloc_v.return_element(alloc_v.ctx, t);
        return (tensor_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    // ---- populate remaining fields ---------------------------------------
    t->data      = (uint8_t*)data_result.u.value;
    t->len       = len;
    t->alloc     = len;
    t->data_size = data_size;
    t->dtype     = dtype;
    t->mode      = TENSOR_STRUCT;
    t->growth    = false;   /* ignored for TENSOR_STRUCT; set defensively */
    t->alloc_v   = alloc_v;
    t->ndim      = ndim;

    return (tensor_expect_t){ .has_value = true, .u.value = t };
}
// -------------------------------------------------------------------------------- 

void return_tensor(tensor_t* t) {
    if (t == NULL) return;

    // return the data buffer first, then the struct+FAM block
    // shape and strides live inside the FAM so they are freed with the struct
    if (t->data != NULL) {
        t->alloc_v.return_element(t->alloc_v.ctx, t->data);
        t->data = NULL;
    }

    t->alloc_v.return_element(t->alloc_v.ctx, t);
}

// -------------------------------------------------------------------------------- 

tensor_expect_t init_tensor_array(size_t             capacity,
                                  dtype_id_t         dtype,
                                  bool               growth,
                                  allocator_vtable_t alloc_v) {
    const size_t shape[] = { capacity };
    tensor_expect_t r = init_tensor(1u, shape, dtype, alloc_v);
    if (!r.has_value) return r;

    r.u.value->mode   = ARRAY_STRUCT;
    r.u.value->growth = growth;
    r.u.value->len    = 0u;

    return r;
}

// -------------------------------------------------------------------------------- 

tensor_expect_t copy_tensor(const tensor_t*     src,
                            allocator_vtable_t* alloc_v) {
    if (src == NULL)
        return (tensor_expect_t){ .has_value = false, .u.error = NULL_POINTER };

    /* Use src's own allocator when none is supplied */
    allocator_vtable_t av = (alloc_v != NULL) ? *alloc_v : src->alloc_v;

    /* Allocate new header + FAM block sized for src->ndim */
    size_t header_bytes = sizeof(tensor_t) + 2u * src->ndim * sizeof(size_t);

    void_ptr_expect_t struct_result = av.allocate(av.ctx, header_bytes, true);
    if (struct_result.has_value == false)
        return (tensor_expect_t){ .has_value = false, .u.error = BAD_ALLOC };

    tensor_t* dst = (tensor_t*)struct_result.u.value;

    /* Wire shape and strides into the new FAM then copy from src */
    dst->shape   = dst->meta;
    dst->strides = dst->meta + src->ndim;

    for (uint8_t i = 0u; i < src->ndim; i++) {
        dst->shape[i]   = src->shape[i];
        dst->strides[i] = src->strides[i];
    }

    /* Allocate and copy the data buffer */
    void_ptr_expect_t data_result = av.allocate(av.ctx,
                                                 src->alloc * src->data_size,
                                                 false);
    if (data_result.has_value == false) {
        av.return_element(av.ctx, dst);
        return (tensor_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    dst->data = (uint8_t*)data_result.u.value;
    memcpy(dst->data, src->data, src->len * src->data_size);

    /* Copy all scalar fields */
    dst->len       = src->len;
    dst->alloc     = src->alloc;
    dst->data_size = src->data_size;
    dst->dtype     = src->dtype;
    dst->mode      = src->mode;
    dst->growth    = src->growth;
    dst->ndim      = src->ndim;
    dst->alloc_v   = av;

    return (tensor_expect_t){ .has_value = true, .u.value = dst };
}
// ================================================================================ 
// ================================================================================ 
// INTROSPECTION 

bool is_tensor_ptr(const tensor_t* t, const void* ptr) {
    if (t == NULL || ptr == NULL)   return false;
    if (t->data == NULL)            return false;

    const uint8_t* p     = (const uint8_t*)ptr;
    const uint8_t* begin = t->data;
    const uint8_t* end   = t->data + t->len * t->data_size;

    if (p < begin || p >= end)                      return false;
    if ((size_t)(p - begin) % t->data_size != 0u)   return false;
    return true;
}

// --------------------------------------------------------------------------------

error_code_t tensor_shape(const tensor_t* t, size_t* out, uint8_t count) {
    if (t == NULL || out == NULL) return NULL_POINTER;

    uint8_t to_copy = count < t->ndim ? count : t->ndim;
    for (uint8_t i = 0u; i < to_copy; i++)
        out[i] = t->shape[i];

    return count < t->ndim ? INVALID_ARG : NO_ERROR;
}

// --------------------------------------------------------------------------------

error_code_t tensor_shape_str(const tensor_t* t, char* buf, size_t buf_len) {
    if (t == NULL || buf == NULL) return NULL_POINTER;

    // We need room for at least "()\0"
    if (buf_len < 3u) {
        if (buf_len > 0u) buf[0] = '\0';
        return CAPACITY_OVERFLOW;
    }

    size_t pos = 0u;
    buf[pos++] = '(';

    for (uint8_t i = 0u; i < t->ndim; i++) {
        // Format the dimension value into a temporary buffer.
        // A size_t on a 64-bit system is at most 20 decimal digits.
        char tmp[21];
        size_t val = t->shape[i];
        uint8_t tmp_len = 0u;

        // Build digits in reverse
        if (val == 0u) {
            tmp[tmp_len++] = '0';
        } else {
            while (val > 0u && tmp_len < 20u) {
                tmp[tmp_len++] = (char)('0' + (val % 10u));
                val /= 10u;
            }
        }

        // Write digits forward into buf
        for (uint8_t d = tmp_len; d > 0u; d--) {
            if (pos >= buf_len - 1u) {
                buf[pos] = '\0';
                return CAPACITY_OVERFLOW;
            }
            buf[pos++] = tmp[d - 1u];
        }

        // Write separator between dimensions
        if (i < t->ndim - 1u) {
            if (pos >= buf_len - 1u) {
                buf[pos] = '\0';
                return CAPACITY_OVERFLOW;
            }
            buf[pos++] = ',';
            if (pos >= buf_len - 1u) {
                buf[pos] = '\0';
                return CAPACITY_OVERFLOW;
            }
            buf[pos++] = ' ';
        }
    }

    // Write closing paren
    if (pos >= buf_len - 1u) {
        buf[pos] = '\0';
        return CAPACITY_OVERFLOW;
    }
    buf[pos++] = ')';
    buf[pos]   = '\0';

    return NO_ERROR;
}

// ================================================================================ 
// ================================================================================ 
// UTILITY OPERATIONS 

error_code_t clear_tensor(tensor_t* t) {
    if (t == NULL) return NULL_POINTER;

    memset(t->data, 0, t->alloc * t->data_size);

    /* DYNAMIC_1D: reset populated count to zero.
     * FIXED_SHAPE: len == alloc always; zeroing the buffer does not
     *              change the number of live slots. */
    if (t->mode == ARRAY_STRUCT)
        t->len = 0u;

    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

error_code_t concat_tensor_array(tensor_t*       dst,
                                 const tensor_t* src,
                                 dtype_id_t      dtype) {
    if (dst == NULL || src == NULL)  return NULL_POINTER;
    if (dst->mode != ARRAY_STRUCT)   return PRECONDITION_FAIL;
    if (src->mode != ARRAY_STRUCT)   return PRECONDITION_FAIL;
    if (dtype != dst->dtype)         return TYPE_MISMATCH;
    if (dst->dtype != src->dtype)    return TYPE_MISMATCH;
    if (src->len == 0u)              return NO_ERROR;
    if (src->len > SIZE_MAX - dst->len) return LENGTH_OVERFLOW;

    size_t needed = dst->len + src->len;

    if (needed > dst->alloc) {
        if (dst->growth == false)            return CAPACITY_OVERFLOW;
        if (dst->alloc_v.reallocate == NULL) return CAPACITY_OVERFLOW;
        size_t tiered    = _compute_new_alloc(dst->alloc);
        size_t new_alloc = (tiered > needed) ? tiered : needed;
        error_code_t err = _grow_array(dst, new_alloc);
        if (err != NO_ERROR) return err;
    }

    memcpy(dst->data + dst->len * dst->data_size,
           src->data,
           src->len * src->data_size);
    dst->len = needed;
    return NO_ERROR;
}

// -------------------------------------------------------------------------------- 

tensor_expect_t slice_tensor_array(const tensor_t*     src,
                                   size_t              start,
                                   size_t              end,
                                   allocator_vtable_t* alloc_v) {
    if (src == NULL)
        return (tensor_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (src->mode != ARRAY_STRUCT)
        return (tensor_expect_t){ .has_value = false, .u.error = PRECONDITION_FAIL };

    /* Default to src's own allocator when none is supplied */
    allocator_vtable_t av = (alloc_v != NULL) ? *alloc_v : src->alloc_v;

    /* Validate range — end is exclusive, start must be strictly less */
    if (start > src->len || end > src->len)
        return (tensor_expect_t){ .has_value = false, .u.error = OUT_OF_BOUNDS };
    if (start >= end)
        return (tensor_expect_t){ .has_value = false, .u.error = INVALID_ARG };

    size_t slice_len = end - start;

    /* Allocate header + FAM for a 1-D tensor */
    size_t header_bytes = sizeof(tensor_t) + 2u * sizeof(size_t);

    void_ptr_expect_t struct_result = av.allocate(av.ctx, header_bytes, true);
    if (struct_result.has_value == false)
        return (tensor_expect_t){ .has_value = false, .u.error = BAD_ALLOC };

    tensor_t* t = (tensor_t*)struct_result.u.value;

    /* Wire shape and strides into the FAM */
    t->shape      = t->meta;
    t->strides    = t->meta + 1u;
    t->shape[0]   = slice_len;
    t->strides[0] = src->data_size;

    /* Allocate and populate the data buffer */
    void_ptr_expect_t data_result = av.allocate(av.ctx,
                                                 slice_len * src->data_size,
                                                 false);
    if (data_result.has_value == false) {
        av.return_element(av.ctx, t);
        return (tensor_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    t->data = (uint8_t*)data_result.u.value;
    memcpy(t->data,
           src->data + start * src->data_size,
           slice_len * src->data_size);

    /* Populate scalar fields */
    t->len       = slice_len;
    t->alloc     = slice_len;
    t->data_size = src->data_size;
    t->dtype     = src->dtype;
    t->mode      = ARRAY_STRUCT;
    t->growth    = false;
    t->ndim      = 1u;
    t->alloc_v   = av;

    return (tensor_expect_t){ .has_value = true, .u.value = t };
}
// ================================================================================ 
// ================================================================================ 
// ADD AND REMOVE DATA

error_code_t set_tensor_index(tensor_t*   t,
                              size_t      index,
                              const void* data,
                              dtype_id_t  dtype) {
    if (t == NULL || data == NULL) return NULL_POINTER;
    if (dtype != t->dtype)         return TYPE_MISMATCH;

    /* Bound check depends on mode:
     *   DYNAMIC_1D:   only populated slots [0, len) are addressable
     *   FIXED_SHAPE:  all allocated slots  [0, alloc) are addressable */
    size_t limit = (t->mode == ARRAY_STRUCT) ? t->len : t->alloc;
    if (index >= limit) return OUT_OF_BOUNDS;

    memcpy(t->data + index * t->data_size, data, t->data_size);
    return NO_ERROR;
}

// --------------------------------------------------------------------------------

error_code_t set_tensor_nd_index(tensor_t*      t,
                                 const size_t*  idx,
                                 const void*    data,
                                 dtype_id_t     dtype) {
    if (t == NULL || idx == NULL || data == NULL) return NULL_POINTER;
    if (dtype != t->dtype)                        return TYPE_MISMATCH;

    /* N-D indexing is only meaningful for fixed-shape tensors.
     * A dynamic 1-D array has no row/column structure. */
    if (t->mode != TENSOR_STRUCT) return ILLEGAL_STATE;

    size_t offset = 0u;
    for (uint8_t i = 0u; i < t->ndim; i++) {
        if (idx[i] >= t->shape[i]) return OUT_OF_BOUNDS;
        offset += idx[i] * t->strides[i];
    }

    memcpy(t->data + offset, data, t->data_size);
    return NO_ERROR;
}

// -------------------------------------------------------------------------------- 

error_code_t push_back_tensor(tensor_t*    t,
                              const void* data,
                              dtype_id_t  dtype) {
    if (t == NULL || data == NULL) return NULL_POINTER;
    if (t->mode != ARRAY_STRUCT)   return PRECONDITION_FAIL;
    if (dtype != t->dtype)         return TYPE_MISMATCH;

    error_code_t err = _ensure_capacity(t);
    if (err != NO_ERROR) return err;

    memcpy(t->data + t->len * t->data_size, data, t->data_size);
    t->len++;
    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

error_code_t push_front_tensor(tensor_t*  t,
                               const void* data,
                               dtype_id_t  dtype) {
    if (t == NULL || data == NULL) return NULL_POINTER;
    if (t->mode != ARRAY_STRUCT)   return PRECONDITION_FAIL;
    if (dtype != t->dtype)         return TYPE_MISMATCH;

    /* Grow before shifting so the buffer pointer is stable during memmove */
    error_code_t err = _ensure_capacity(t);
    if (err != NO_ERROR) return err;

    if (t->len > 0u) {
        memmove(t->data + t->data_size,
                t->data,
                t->len * t->data_size);
}

    memcpy(t->data, data, t->data_size);
    t->len++;
    return NO_ERROR;
}

// --------------------------------------------------------------------------------

error_code_t push_at_tensor(tensor_t*    t,
                            const void* data,
                            size_t      index,
                            dtype_id_t  dtype) {
    if (t == NULL || data == NULL) return NULL_POINTER;
    if (t->mode != ARRAY_STRUCT)   return PRECONDITION_FAIL;
    if (dtype != t->dtype)         return TYPE_MISMATCH;
    if (index > t->len)            return OUT_OF_BOUNDS;

    if (index == 0u)         return push_front_tensor(t, data, dtype);
    if (index == t->len) return push_back_tensor(t, data, dtype);

    /* Grow before shifting so the buffer pointer is stable during memmove */
    error_code_t err = _ensure_capacity(t);
    if (err != NO_ERROR) return err;

    memmove(t->data + (index + 1u) * t->data_size,
            t->data + index * t->data_size,
            (t->len - index) * t->data_size);

    memcpy(t->data + index * t->data_size, data, t->data_size);
    t->len++;
    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

error_code_t pop_back_tensor(tensor_t*   t,
                             void*       out,
                             dtype_id_t  dtype) {
    if (t == NULL)             return NULL_POINTER;
    if (t->mode != ARRAY_STRUCT) return PRECONDITION_FAIL;
    if (dtype != t->dtype)     return TYPE_MISMATCH;
    if (t->len == 0u)          return EMPTY;

    t->len--;
    if (out != NULL)
        memcpy(out, t->data + t->len * t->data_size, t->data_size);

    return NO_ERROR;
}

// --------------------------------------------------------------------------------

error_code_t pop_front_tensor(tensor_t*   t,
                              void*       out,
                              dtype_id_t  dtype) {
    if (t == NULL)             return NULL_POINTER;
    if (t->mode != ARRAY_STRUCT) return PRECONDITION_FAIL;
    if (dtype != t->dtype)     return TYPE_MISMATCH;
    if (t->len == 0u)          return EMPTY;

    if (out != NULL)
        memcpy(out, t->data, t->data_size);

    t->len--;
    if (t->len > 0u) {
        memmove(t->data,
                t->data + t->data_size,
                t->len * t->data_size);
    }

    return NO_ERROR;
}

// --------------------------------------------------------------------------------

error_code_t pop_at_tensor(tensor_t*   t,
                           void*       out,
                           size_t      index,
                           dtype_id_t  dtype) {
    if (t == NULL)             return NULL_POINTER;
    if (t->mode != ARRAY_STRUCT) return PRECONDITION_FAIL;
    if (dtype != t->dtype)     return TYPE_MISMATCH;
    if (t->len == 0u)          return EMPTY;
    if (index >= t->len)       return OUT_OF_BOUNDS;

    if (index == 0u)              return pop_front_tensor(t, out, dtype);
    if (index == t->len - 1u)    return pop_back_tensor(t, out, dtype);

    if (out != NULL)
        memcpy(out, t->data + index * t->data_size, t->data_size);

    /* Compute shift count before decrementing len for clarity */
    size_t shift_count = t->len - index - 1u;
    t->len--;
    memmove(t->data + index * t->data_size,
            t->data + (index + 1u) * t->data_size,
            shift_count * t->data_size);

    return NO_ERROR;
}

// ================================================================================ 
// ================================================================================ 
// RETRIEVE DATA

error_code_t get_tensor_index(const tensor_t* t,
                              size_t          index,
                              void*           out,
                              dtype_id_t      dtype) {
    if (t == NULL || out == NULL) return NULL_POINTER;
    if (dtype != t->dtype)        return TYPE_MISMATCH;

    size_t limit = (t->mode == ARRAY_STRUCT) ? t->len : t->alloc;
    if (index >= limit) return OUT_OF_BOUNDS;

    memcpy(out, t->data + index * t->data_size, t->data_size);
    return NO_ERROR;
}

// --------------------------------------------------------------------------------

error_code_t get_tensor_nd_index(const tensor_t* t,
                                 const size_t*   idx,
                                 void*           out,
                                 dtype_id_t      dtype) {
    if (t == NULL || idx == NULL || out == NULL) return NULL_POINTER;
    if (dtype != t->dtype)                       return TYPE_MISMATCH;

    if (t->mode != TENSOR_STRUCT) return ILLEGAL_STATE;

    size_t offset = 0u;
    for (uint8_t i = 0u; i < t->ndim; i++) {
        if (idx[i] >= t->shape[i]) return OUT_OF_BOUNDS;
        offset += idx[i] * t->strides[i];
    }

    memcpy(out, t->data + offset, t->data_size);
    return NO_ERROR;
}

// ================================================================================
// ================================================================================
// eof
