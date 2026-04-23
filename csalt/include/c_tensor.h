// ================================================================================
// ================================================================================
// - File:    dtypes.h
// - Purpose: This file contains prototypes for a extensible tensor data structure
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 22, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here
#ifndef tensor_H
#define tensor_H

#include "c_dtypes.h"
#include "c_allocator.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
// ================================================================================ 
// ================================================================================ 

#ifdef __cplusplus
extern "C" {
#endif 
// ================================================================================ 
// ================================================================================ 
// STRUCTS AND DERIVED DATA TYPES 

typedef struct {
    uint8_t*           data;        // type-erased element buffer
    size_t             len;         // total number of elements (product of shape)
    size_t             alloc;       // allocated capacity in elements
    size_t             data_size;   // bytes per element, cached from dtype
    dtype_id_t         dtype;       // runtime type identity
    bool               growth;      // whether data buffer may grow
    allocator_vtable_t alloc_v;     // allocator for all memory ops
    uint32_t           ndim;        // number of dimensions
    size_t*            shape;       // points into meta[] — ndim elements
    size_t*            strides;     // points into meta[] — ndim elements, in bytes
    size_t             meta[];      // FAM: 2*ndim elements, shape then strides
} tensor_t;
// -------------------------------------------------------------------------------- 

// In c_tensor.h
typedef struct {
    bool has_value;
    union {
        tensor_t*    value;
        error_code_t error;
    } u;
} tensor_expect_t;
// ================================================================================ 
// ================================================================================ 
// INITIALIZATION AND TEARDOWN


tensor_expect_t init_tensor(uint8_t            ndim,
                            const size_t*      shape,
                            dtype_id_t         dtype,
                            bool               growth,
                            allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

void return_tensor(tensor_t* t);

// ================================================================================ 
// ================================================================================ 
// INTROSPECTION

/**
 * @brief Return the total number of elements currently stored in the tensor.
 *
 * This is the product of all dimension sizes, not the allocated capacity.
 *
 * @param t  Pointer to the tensor.
 * @return Total element count, or 0 if t is NULL.
 */
size_t tensor_size(const tensor_t* t);

// --------------------------------------------------------------------------------

/**
 * @brief Return the allocated capacity of the tensor's data buffer in elements.
 *
 * @param t  Pointer to the tensor.
 * @return Allocated capacity in elements, or 0 if t is NULL.
 */
size_t tensor_alloc(const tensor_t* t);

// --------------------------------------------------------------------------------

/**
 * @brief Return the size of one element in bytes.
 *
 * @param t  Pointer to the tensor.
 * @return Element size in bytes, or 0 if t is NULL.
 */
size_t tensor_data_size(const tensor_t* t);

// -------------------------------------------------------------------------------- 

/**
 * @brief Return the dtype_id_t of the tensor.
 *
 * Convenience getter for the type identifier fixed at init time. Equivalent
 * to reading array->dtype directly but consistent with the rest of the API.
 *
 * @param array  Pointer to the array.
 * @return The dtype_id_t of the array, or UNKNOWN_TYPE if array is NULL.
 */
dtype_id_t tensor_type(const tensor_t* t);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if the tensor contains no elements.
 *
 * @param t  Pointer to the tensor.
 * @return true if len == 0 or t is NULL, false otherwise.
 */
bool is_tensor_empty(const tensor_t* t);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if the tensor is at full capacity.
 *
 * @param t  Pointer to the tensor.
 * @return true if len == alloc or t is NULL, false otherwise.
 */
bool is_tensor_full(const tensor_t* t);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if ptr points to a valid element-aligned address within
 *        the live region of the tensor's data buffer.
 *
 * Conditions for validity:
 *   1. t and ptr are non-NULL.
 *   2. ptr >= t->data.
 *   3. ptr <  t->data + t->len * t->data_size.
 *   4. (ptr - t->data) is an exact multiple of t->data_size.
 *
 * @param t    Pointer to the tensor.
 * @param ptr  Pointer to test.
 * @return true if ptr is valid, false otherwise.
 */
bool is_tensor_ptr(const tensor_t* t, const void* ptr);

// --------------------------------------------------------------------------------

/**
 * @brief Return the number of dimensions of the tensor.
 *
 * @param t  Pointer to the tensor.
 * @return Number of dimensions, or 0 if t is NULL.
 */
uint8_t tensor_ndim(const tensor_t* t);

// --------------------------------------------------------------------------------

/**
 * @brief Return the size of a single dimension.
 *
 * The most common access pattern — equivalent to asking "how many rows"
 * or "how many columns" without retrieving the full shape array.
 *
 * @param t    Pointer to the tensor.
 * @param dim  Zero-based dimension index. Must be < t->ndim.
 * @return Size of the requested dimension, or 0 if t is NULL or dim is
 *         out of range.
 */
size_t tensor_shape_dim(const tensor_t* t, uint8_t dim);

// --------------------------------------------------------------------------------

/**
 * @brief Copy the shape of the tensor into a caller-provided buffer.
 *
 * Copies min(t->ndim, count) dimension sizes into out. If count < t->ndim
 * only the first count dimensions are copied and INVALID_ARG is returned
 * so the caller knows the output was truncated.
 *
 * @param t      Pointer to the tensor.
 * @param out    Caller-provided buffer to receive the shape. Must not be NULL.
 *               Must hold at least count elements.
 * @param count  Number of elements out can hold. Should be >= t->ndim.
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if t or out is NULL
 *         - INVALID_ARG   if count < t->ndim (output truncated)
 */
error_code_t tensor_shape(const tensor_t* t, size_t* out, uint8_t count);

// --------------------------------------------------------------------------------

/**
 * @brief Return a read-only pointer directly into the tensor's internal
 *        shape array.
 *
 * The returned pointer is valid only for the lifetime of the tensor and
 * must not be modified or freed by the caller. Prefer tensor_shape() when
 * the caller needs to store or modify the shape independently.
 *
 * @param t  Pointer to the tensor.
 * @return Pointer to the first element of the shape array, or NULL if t
 *         is NULL.
 */
const size_t* tensor_shape_ptr(const tensor_t* t);

// --------------------------------------------------------------------------------

/**
 * @brief Return a read-only pointer directly into the tensor's internal
 *        strides array.
 *
 * Strides are in bytes. stride[i] is the number of bytes to advance in
 * the data buffer to move one step along dimension i. For a contiguous
 * C-order tensor, strides[ndim-1] == data_size and each outer stride is
 * the product of all inner dimension sizes times data_size.
 *
 * The returned pointer is valid only for the lifetime of the tensor and
 * must not be modified or freed by the caller.
 *
 * @param t  Pointer to the tensor.
 * @return Pointer to the first element of the strides array, or NULL if
 *         t is NULL.
 */
const size_t* tensor_strides_ptr(const tensor_t* t);

// --------------------------------------------------------------------------------

/**
 * @brief Write a human-readable shape string into a caller-provided buffer.
 *
 * Formats the shape as "(d0, d1, ..., dN)" into buf. Intended for
 * debugging and logging only — do not parse the result programmatically,
 * use tensor_shape() or tensor_shape_dim() instead.
 *
 * If the formatted string would exceed buf_len bytes (including the null
 * terminator) the output is truncated and CAPACITY_OVERFLOW is returned.
 *
 * @param t        Pointer to the tensor.
 * @param buf      Caller-provided character buffer. Must not be NULL.
 * @param buf_len  Size of buf in bytes. Should be large enough to hold
 *                 the formatted string.
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if t or buf is NULL
 *         - CAPACITY_OVERFLOW if the formatted string was truncated
 */
error_code_t tensor_shape_str(const tensor_t* t, char* buf, size_t buf_len);

// ================================================================================ 
// ================================================================================ 
// UTILITY OPERATIONS

/**
 * @brief Reset the tensor to empty without releasing its allocated buffer.
 *
 * Sets len to zero so the tensor can be reused from scratch. The allocated
 * capacity, data buffer, shape, strides, and ndim are all preserved. All
 * previously stored element bytes are zeroed so no stale data remains
 * readable in the buffer.
 *
 * @param t  Pointer to the target tensor. Must not be NULL.
 *
 * @return NO_ERROR on success, or:
 *         - NULL_POINTER if t is NULL
 */
error_code_t clear_tensor(tensor_t* t);

// --------------------------------------------------------------------------------

/**
 * @brief Overwrite the element at a flat index without changing len.
 *
 * Copies exactly data_size bytes from data into the slot at index, replacing
 * the current value. The flat index addresses elements in the order they are
 * laid out in the data buffer — for a C-order tensor this is row-major order.
 * The tensor length is unchanged.
 *
 * @param t      Pointer to the target tensor. Must not be NULL.
 * @param index  Zero-based flat index of the slot to overwrite. Must be < t->len.
 * @param data   Pointer to the replacement value. Must not be NULL.
 *               Must point to at least t->data_size bytes.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if t or data is NULL
 *         - TYPE_MISMATCH if dtype != t->dtype
 *         - OUT_OF_BOUNDS if index >= t->len
 */
error_code_t set_tensor_index(tensor_t*   t,
                              size_t      index,
                              const void* data,
                              dtype_id_t  dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Overwrite the element at an N-dimensional index without changing len.
 *
 * Resolves the N-dimensional index into a flat byte offset using the tensor's
 * stride array, then copies exactly data_size bytes from data into that slot.
 * This is the natural access pattern for matrix and higher-dimensional tensor
 * operations. The tensor length is unchanged.
 *
 * @param t      Pointer to the target tensor. Must not be NULL.
 * @param idx    Array of ndim indices, one per dimension. Must not be NULL.
 *               idx[i] must be < t->shape[i] for all i.
 * @param data   Pointer to the replacement value. Must not be NULL.
 *               Must point to at least t->data_size bytes.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if t, idx, or data is NULL
 *         - TYPE_MISMATCH if dtype != t->dtype
 *         - OUT_OF_BOUNDS if any idx[i] >= t->shape[i]
 */
error_code_t set_tensor_nd_index(tensor_t*      t,
                                 const size_t*  idx,
                                 const void*    data,
                                 dtype_id_t     dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Copy one element out of the tensor by flat index.
 *
 * Copies exactly t->data_size bytes from the tensor's internal buffer at
 * the given flat index into the caller-provided output buffer. The flat
 * index addresses elements in the order they are laid out in the data
 * buffer — for a C-order tensor this is row-major order.
 *
 * @param t      Pointer to the source tensor. Must not be NULL.
 * @param index  Zero-based flat index of the element to retrieve.
 *               Must be < t->len.
 * @param out    Caller-provided buffer to copy the element into.
 *               Must not be NULL. Must be at least t->data_size bytes.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if t or out is NULL
 *         - TYPE_MISMATCH if dtype != t->dtype
 *         - OUT_OF_BOUNDS if index >= t->len
 */
error_code_t get_tensor_index(const tensor_t* t,
                              size_t          index,
                              void*           out,
                              dtype_id_t      dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Copy one element out of the tensor by N-dimensional index.
 *
 * Resolves the N-dimensional index into a flat byte offset using the
 * tensor's stride array, then copies exactly data_size bytes from that
 * slot into the caller-provided output buffer. This is the natural access
 * pattern for matrix and higher-dimensional tensor operations.
 *
 * @param t      Pointer to the source tensor. Must not be NULL.
 * @param idx    Array of ndim indices, one per dimension. Must not be NULL.
 *               idx[i] must be < t->shape[i] for all i.
 * @param out    Caller-provided buffer to copy the element into.
 *               Must not be NULL. Must be at least t->data_size bytes.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if t, idx, or out is NULL
 *         - TYPE_MISMATCH if dtype != t->dtype
 *         - OUT_OF_BOUNDS if any idx[i] >= t->shape[i]
 */
error_code_t get_tensor_nd_index(const tensor_t* t,
                                 const size_t*   idx,
                                 void*           out,
                                 dtype_id_t      dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Create a deep copy of src into a newly initialized tensor.
 *
 * Allocates a new tensor_t (including its FAM metadata block) and a new
 * data buffer through alloc_v, then copies all len elements and the full
 * shape and strides arrays from src. The copy has the same dtype, ndim,
 * shape, strides, data_size, and growth setting as src. The caller is
 * responsible for calling return_tensor on the returned tensor when done.
 * The copy may use a different allocator than src.
 *
 * @param src      Pointer to the source tensor. Must not be NULL.
 * @param alloc_v  Allocator vtable to use for the new tensor's memory.
 *                 alloc_v.allocate must not be NULL.
 *
 * @return tensor_expect_t with has_value true and a valid tensor_t* on success.
 *         On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER  if src is NULL or alloc_v.allocate is NULL
 *         - BAD_ALLOC     if the allocator fails to allocate the new header
 *         - OUT_OF_MEMORY if the allocator fails to allocate the new data buffer
 */
tensor_expect_t copy_tensor(const tensor_t*    src,
                            allocator_vtable_t alloc_v);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* tensor_H */
// ================================================================================
// ================================================================================
// eof
