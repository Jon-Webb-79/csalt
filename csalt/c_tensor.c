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
// INITIALIZATION AND TEARDOWN

tensor_expect_t init_tensor(uint8_t            ndim,
                            const size_t*      shape,
                            dtype_id_t         dtype,
                            bool               growth,
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

    // ---- allocate data buffer --------------------------------------------
    void_ptr_expect_t data_result = alloc_v.allocate(alloc_v.ctx, len * data_size, false);
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
    t->growth    = growth;
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
// ================================================================================ 
// ================================================================================ 
// INTROSPECTION 

size_t tensor_size(const tensor_t* t) {
    if (t == NULL) return 0u;
    return t->len;
}

// --------------------------------------------------------------------------------

size_t tensor_alloc(const tensor_t* t) {
    if (t == NULL) return 0u;
    return t->alloc;
}

// --------------------------------------------------------------------------------

size_t tensor_data_size(const tensor_t* t) {
    if (t == NULL) return 0u;
    return t->data_size;
}
// -------------------------------------------------------------------------------- 

dtype_id_t tensor_type(const tensor_t* t) {
    if (t == NULL) return UNKNOWN_TYPE;
    return t->dtype;
}
// --------------------------------------------------------------------------------

bool is_tensor_empty(const tensor_t* t) {
    if (t == NULL) return true;
    return t->len == 0u;
}

// --------------------------------------------------------------------------------

bool is_tensor_full(const tensor_t* t) {
    if (t == NULL) return true;
    return t->len == t->alloc;
}

// --------------------------------------------------------------------------------

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

uint8_t tensor_ndim(const tensor_t* t) {
    if (t == NULL) return 0u;
    return t->ndim;
}

// --------------------------------------------------------------------------------

size_t tensor_shape_dim(const tensor_t* t, uint8_t dim) {
    if (t == NULL)        return 0u;
    if (dim >= t->ndim)   return 0u;
    return t->shape[dim];
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

const size_t* tensor_shape_ptr(const tensor_t* t) {
    if (t == NULL) return NULL;
    return t->shape;
}

// --------------------------------------------------------------------------------

const size_t* tensor_strides_ptr(const tensor_t* t) {
    if (t == NULL) return NULL;
    return t->strides;
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
    t->len = 0u;
    return NO_ERROR;
}

// --------------------------------------------------------------------------------

error_code_t set_tensor_index(tensor_t*   t,
                              size_t      index,
                              const void* data,
                              dtype_id_t  dtype) {
    if (t == NULL || data == NULL) return NULL_POINTER;
    if (dtype != t->dtype)         return TYPE_MISMATCH;
    if (index >= t->len)           return OUT_OF_BOUNDS;

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

    // Validate each dimension index and compute byte offset via strides
    size_t offset = 0u;
    for (uint8_t i = 0u; i < t->ndim; i++) {
        if (idx[i] >= t->shape[i]) return OUT_OF_BOUNDS;
        offset += idx[i] * t->strides[i];
    }

    memcpy(t->data + offset, data, t->data_size);
    return NO_ERROR;
}

// --------------------------------------------------------------------------------

error_code_t get_tensor_index(const tensor_t* t,
                              size_t          index,
                              void*           out,
                              dtype_id_t      dtype) {
    if (t == NULL || out == NULL) return NULL_POINTER;
    if (dtype != t->dtype)        return TYPE_MISMATCH;
    if (index >= t->len)          return OUT_OF_BOUNDS;

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

    // Validate each dimension index and compute byte offset via strides
    size_t offset = 0u;
    for (uint8_t i = 0u; i < t->ndim; i++) {
        if (idx[i] >= t->shape[i]) return OUT_OF_BOUNDS;
        offset += idx[i] * t->strides[i];
    }

    memcpy(out, t->data + offset, t->data_size);
    return NO_ERROR;
}

// --------------------------------------------------------------------------------

tensor_expect_t copy_tensor(const tensor_t*    src,
                            allocator_vtable_t alloc_v) {
    if (src == NULL || alloc_v.allocate == NULL)
        return (tensor_expect_t){ .has_value = false, .u.error = NULL_POINTER };

    // Allocate new header + FAM block sized for src->ndim
    size_t header_bytes = sizeof(tensor_t) + 2u * src->ndim * sizeof(size_t);

    void_ptr_expect_t struct_result = alloc_v.allocate(alloc_v.ctx, header_bytes, true);
    if (struct_result.has_value == false)
        return (tensor_expect_t){ .has_value = false, .u.error = BAD_ALLOC };

    tensor_t* dst = (tensor_t*)struct_result.u.value;

    // Wire shape and strides into the new FAM then copy from src
    dst->shape   = dst->meta;
    dst->strides = dst->meta + src->ndim;

    for (uint8_t i = 0u; i < src->ndim; i++) {
        dst->shape[i]   = src->shape[i];
        dst->strides[i] = src->strides[i];
    }

    // Allocate and copy the data buffer
    void_ptr_expect_t data_result = alloc_v.allocate(alloc_v.ctx,
                                                      src->alloc * src->data_size,
                                                      false);
    if (data_result.has_value == false) {
        alloc_v.return_element(alloc_v.ctx, dst);
        return (tensor_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    dst->data = (uint8_t*)data_result.u.value;
    memcpy(dst->data, src->data, src->len * src->data_size);

    // Copy all scalar fields
    dst->len       = src->len;
    dst->alloc     = src->alloc;
    dst->data_size = src->data_size;
    dst->dtype     = src->dtype;
    dst->growth    = src->growth;
    dst->ndim      = src->ndim;
    dst->alloc_v   = alloc_v;

    return (tensor_expect_t){ .has_value = true, .u.value = dst };
}
// ================================================================================
// ================================================================================
// eof
