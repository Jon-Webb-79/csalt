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
#include "c_error.h"

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

typedef enum {
    TENSOR_STRUCT,   /* N-D fixed capacity; all slots zero-initialised;
                      len == alloc == product of shape at all times     */

    ARRAY_STRUCT     /* 1-D only; len tracks populated count;
                      growth flag controls automatic reallocation       */
} tensor_mode_t;

typedef struct {
    uint8_t*           data;        /* type-erased element buffer              */
    size_t             len;         /* FIXED:   == alloc (all slots live)
                                       DYNAMIC: populated element count        */
    size_t             alloc;       /* allocated capacity in elements          */
    size_t             data_size;   /* bytes per element, cached from dtype    */
    dtype_id_t         dtype;       /* runtime type identity                   */
    tensor_mode_t      mode;        /* FIXED_SHAPE or DYNAMIC_1D               */
    bool               growth;      /* DYNAMIC_1D only: auto-grow when full    */
    allocator_vtable_t alloc_v;     /* allocator for all memory operations     */
    uint8_t            ndim;        /* number of dimensions (max 255)          */
    size_t*            shape;       /* points to meta[0]                       */
    size_t*            strides;     /* points to meta[ndim]                    */
    size_t             meta[];      /* FAM: shape[0..ndim-1], strides[0..ndim-1] */
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

/**
 * @brief Initialize a new fixed-shape tensor.
 *
 * Allocates the tensor_t struct (including the inline FAM block holding
 * shape and strides) and its data buffer through the provided allocator
 * vtable. The dtype must be registered in the dtype registry before
 * calling this function. data_size is resolved from the registry once at
 * init and cached on the struct — no further registry lookups are
 * performed during the lifetime of the tensor.
 *
 * The tensor is created in TENSOR_FIXED_SHAPE mode. All allocated slots
 * are considered live at construction time, so the data buffer is
 * zero-initialised. len is set equal to alloc, which equals the product
 * of all shape dimensions. growth is set to false. To create a
 * TENSOR_DYNAMIC_1D tensor, initialise with ndim == 1 and then set
 * mode and growth via the typed wrapper init function (e.g. init_array).
 *
 * Strides are computed in C-order (row-major):
 *   strides[ndim - 1] = data_size
 *   strides[i]        = strides[i + 1] * shape[i + 1]   for i < ndim - 1
 *
 * @param ndim     Number of dimensions. Must be > 0. Values above a few
 *                 dozen are unusual in practice; the field is uint8_t so
 *                 the maximum is 255.
 * @param shape    Array of ndim dimension sizes. Must not be NULL.
 *                 Each shape[i] must be > 0. The product of all shape
 *                 dimensions must not overflow size_t.
 * @param dtype    Type identifier. Must not be UNKNOWN_TYPE and must be
 *                 registered in the dtype registry before this call.
 * @param alloc_v  Allocator vtable used for all memory operations.
 *                 alloc_v.allocate must not be NULL.
 *
 * @return tensor_expect_t with has_value true and a valid tensor_t* on
 *         success. On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER    if alloc_v.allocate is NULL or shape is NULL
 *         - INVALID_ARG     if ndim is 0 or any shape[i] is 0
 *         - INVALID_ARG     if dtype is UNKNOWN_TYPE
 *         - ILLEGAL_STATE   if the dtype registry could not be initialised
 *         - TYPE_MISMATCH   if dtype is not registered in the dtype registry
 *         - LENGTH_OVERFLOW if the product of shape dimensions overflows
 *                           size_t, or if len * data_size overflows size_t
 *         - BAD_ALLOC       if the allocator fails to allocate the struct
 *                           and FAM metadata block
 *         - OUT_OF_MEMORY   if the allocator fails to allocate the data
 *                           buffer
 */
tensor_expect_t init_tensor(uint8_t            ndim,
                            const size_t*      shape,
                            dtype_id_t         dtype,
                            allocator_vtable_t alloc_v);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a tensor's memory back to its allocator.
 *
 * Returns the data buffer and the tensor_t struct (including the inline
 * FAM block holding shape and strides) to the allocator via
 * return_element. Because shape and strides point into the FAM which is
 * contiguous with the struct header, both are freed by the single
 * return_element call on the struct — there is no separate call needed
 * for shape or strides.
 *
 * This does NOT free or destroy the allocator itself. After this call
 * the pointer must not be used.
 *
 * @param t  Pointer to the tensor to return. Silently ignored if NULL.
 */
void return_tensor(tensor_t* t);

// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a new dynamic 1-D tensor operating in ARRAY_STRUCT mode.
 *
 * Convenience wrapper around init_tensor that constructs a single-dimension
 * tensor and configures it for dynamic array semantics. The underlying
 * allocation is identical to a rank-1 init_tensor call — one header+FAM
 * block and one data buffer — but the mode, growth flag, and len are set
 * to reflect ARRAY_STRUCT invariants:
 *
 *   - mode  = ARRAY_STRUCT  (1-D dynamic array semantics)
 *   - len   = 0             (no elements populated at construction)
 *   - alloc = capacity      (total allocated capacity in elements)
 *   - growth controls whether push operations may trigger reallocation
 *     when len reaches alloc
 *
 * The data buffer is zero-initialised at construction. The dtype must be
 * registered in the dtype registry before calling this function. For
 * type-safe access prefer a typed wrapper such as init_float_array which
 * fixes the dtype at compile time and hides it from every call site.
 *
 * @param capacity  Number of elements to allocate. Must be > 0.
 * @param dtype     Type identifier. Must not be UNKNOWN_TYPE and must be
 *                  registered in the dtype registry before this call.
 * @param growth    If true, push operations will automatically reallocate
 *                  the data buffer when len reaches alloc. If false, pushing
 *                  onto a full array returns CAPACITY_OVERFLOW.
 * @param alloc_v   Allocator vtable used for all memory operations.
 *                  alloc_v.allocate must not be NULL.
 *
 * @return tensor_expect_t with has_value true and a valid tensor_t* on
 *         success. On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER    if alloc_v.allocate is NULL
 *         - INVALID_ARG     if capacity is 0 or dtype is UNKNOWN_TYPE
 *         - ILLEGAL_STATE   if the dtype registry could not be initialised
 *         - TYPE_MISMATCH   if dtype is not registered in the dtype registry
 *         - BAD_ALLOC       if the allocator fails to allocate the struct
 *                           and FAM metadata block
 *         - OUT_OF_MEMORY   if the allocator fails to allocate the data buffer
 */
tensor_expect_t init_tensor_array(size_t             capacity,
                                  dtype_id_t         dtype,
                                  bool               growth,
                                  allocator_vtable_t alloc_v);
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
// INTROSPECTION

/**
 * @brief Return the total number of elements currently stored in the tensor.
 *
 * This is the product of all dimension sizes, not the allocated capacity.
 *
 * @param t  Pointer to the tensor.
 * @return Total element count, or 0 if t is NULL.
 */
static inline size_t tensor_size(const tensor_t* t) {
    if (t == NULL) return 0u;
    return t->len;
}

// --------------------------------------------------------------------------------

/**
 * @brief Return the allocated capacity of the tensor's data buffer in elements.
 *
 * @param t  Pointer to the tensor.
 * @return Allocated capacity in elements, or 0 if t is NULL.
 */
static inline size_t tensor_alloc(const tensor_t* t) {
    if (t == NULL) return 0u;
    return t->alloc;
}

// --------------------------------------------------------------------------------

/**
 * @brief Return the size of one element in bytes.
 *
 * @param t  Pointer to the tensor.
 * @return Element size in bytes, or 0 if t is NULL.
 */
static inline size_t tensor_data_size(const tensor_t* t) {
    if (t == NULL) return 0u;
    return t->data_size;
}

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
static inline dtype_id_t tensor_dtype(const tensor_t* t) {
    if (t == NULL) return UNKNOWN_TYPE;
    return t->dtype;
}

// --------------------------------------------------------------------------------

/**
 * @brief Return true if the tensor contains no elements.
 *
 * @param t  Pointer to the tensor.
 * @return true if len == 0 or t is NULL, false otherwise.
 */
static inline bool is_tensor_empty(const tensor_t* t) {
    if (t == NULL) return true;
    return t->len == 0u;
}

// --------------------------------------------------------------------------------

/**
 * @brief Return true if the tensor is at full capacity.
 *
 * @param t  Pointer to the tensor.
 * @return true if len == alloc or t is NULL, false otherwise.
 */
static inline bool is_tensor_full(const tensor_t* t) {
    if (t == NULL) return true;
    return t->len == t->alloc;
}

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
static inline uint8_t tensor_ndim(const tensor_t* t) {
    if (t == NULL) return 0u;
    return t->ndim;
}

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
static inline size_t tensor_shape_dim(const tensor_t* t, uint8_t dim) {
    if (t == NULL) return 0u;
    if (dim >= t->ndim) return 0u;
    return t->shape[dim];
}

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
static inline const size_t* tensor_shape_ptr(const tensor_t* t) {
    if (t == NULL) return NULL;
    return t->shape;
}

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
static inline const size_t* tensor_strides_ptr(const tensor_t* t) {
    if (t == NULL) return NULL;
    return t->strides;
}
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
 * @brief Append all elements of src to the back of dst.
 *
 * Copies exactly src->len * src->data_size bytes from src's data buffer
 * into the first available slot in dst's data buffer and increments
 * dst->len by src->len. Both tensors must be in ARRAY_STRUCT mode and
 * must share the same dtype.
 *
 * Rather than appending elements one at a time, the function computes the
 * total capacity needed upfront and performs at most one reallocation
 * before the copy. When growth is required the new allocation is the
 * larger of the tiered growth target (_compute_new_alloc) and the exact
 * capacity needed, so that small concatenations still benefit from the
 * tiered growth headroom and large concatenations are never undersized.
 *
 * If src is empty (src->len == 0) the function returns NO_ERROR without
 * modifying dst.
 *
 * @param dst    Pointer to the destination tensor. Must not be NULL.
 *               Must have mode == ARRAY_STRUCT.
 *               Must have the same dtype as src.
 *               If dst is full and growth is true, its data buffer will
 *               be reallocated to accommodate all of src's elements.
 * @param src    Pointer to the source tensor. Must not be NULL.
 *               Must have mode == ARRAY_STRUCT.
 *               Must have the same dtype as dst.
 *               src and dst must not refer to the same tensor.
 * @param dtype  Type identifier. Must match dst->dtype and src->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if dst or src is NULL
 *         - PRECONDITION_FAIL if dst->mode != ARRAY_STRUCT or
 *                             src->mode != ARRAY_STRUCT
 *         - TYPE_MISMATCH     if dtype != dst->dtype or
 *                             dst->dtype != src->dtype
 *         - LENGTH_OVERFLOW   if dst->len + src->len overflows size_t
 *         - CAPACITY_OVERFLOW if the combined length exceeds dst->alloc
 *                             and growth is false, or if the allocator
 *                             does not support reallocation
 *         - OUT_OF_MEMORY     if reallocation fails
 */
error_code_t concat_tensor_array(tensor_t*       dst,
                                 const tensor_t* src,
                                 dtype_id_t      dtype);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a new dynamic 1-D tensor containing a copy of the elements
 *        in src at positions [start, end).
 *
 * Allocates a new tensor_t and data buffer, copies exactly
 * (end - start) * src->data_size bytes from src's buffer starting at
 * position start, and returns the result as an independent ARRAY_STRUCT
 * tensor. The slice is a deep copy — mutations to the slice do not affect
 * src and vice versa.
 *
 * The slice is always returned with growth == false and alloc == len.
 * The caller may enable growth after construction if dynamic behaviour
 * is needed.
 *
 * The range [start, end) is half-open: start is inclusive, end is
 * exclusive. Both start and end must be <= src->len, and start must be
 * strictly less than end. A zero-length slice (start == end) is
 * considered invalid and returns INVALID_ARG.
 *
 * If alloc_v.allocate is NULL the allocator is inherited from src,
 * so the slice is managed by the same allocator as the source tensor.
 * The caller is responsible for calling return_tensor on the returned
 * tensor when it is no longer needed.
 *
 * @param src     Pointer to the source tensor. Must not be NULL.
 *                Must have mode == ARRAY_STRUCT.
 * @param start   Zero-based index of the first element to include.
 *                Must be < end and <= src->len.
 * @param end     Zero-based index one past the last element to include.
 *                Must be > start and <= src->len.
 * @param alloc_v Allocator vtable for the new tensor's memory. If
 *                alloc_v.allocate is NULL the allocator is inherited
 *                from src.
 *
 * @return tensor_expect_t with has_value true and a valid tensor_t* on
 *         success. On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER      if src is NULL
 *         - PRECONDITION_FAIL if src->mode != ARRAY_STRUCT
 *         - OUT_OF_BOUNDS     if start > src->len or end > src->len
 *         - INVALID_ARG       if start >= end
 *         - BAD_ALLOC         if the allocator fails to allocate the
 *                             struct and FAM metadata block
 *         - OUT_OF_MEMORY     if the allocator fails to allocate the
 *                             data buffer
 */
tensor_expect_t slice_tensor_array(const tensor_t*    src,
                                   size_t             start,
                                   size_t             end,
                                   allocator_vtable_t alloc_v);
// ================================================================================ 
// ================================================================================ 
// ADD AND REMOVE DATA

/**
 * @brief Overwrite the element at a flat index without changing len or alloc.
 *
 * Copies exactly data_size bytes from data into the slot at index in the
 * tensor's internal buffer. The flat index addresses elements in the order
 * they are laid out in memory — for a C-order tensor this is row-major order.
 *
 * The bound check depends on mode:
 *   TENSOR_FIXED_SHAPE: index must be < t->alloc. All allocated slots are
 *                       always live regardless of len, so this function
 *                       remains valid after clear_tensor.
 *   TENSOR_DYNAMIC_1D: index must be < t->len. Only populated slots are
 *                       addressable; slots in [len, alloc) are off-limits
 *                       until exposed by a push operation.
 *
 * @param t      Pointer to the target tensor. Must not be NULL.
 * @param index  Zero-based flat index of the slot to overwrite.
 *               FIXED_SHAPE: must be < t->alloc.
 *               DYNAMIC_1D:  must be < t->len.
 * @param data   Pointer to the replacement value. Must not be NULL.
 *               Must point to at least t->data_size bytes.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if t or data is NULL
 *         - TYPE_MISMATCH if dtype != t->dtype
 *         - OUT_OF_BOUNDS if index >= t->alloc (FIXED_SHAPE) or
 *                                   index >= t->len  (DYNAMIC_1D)
 */
error_code_t set_tensor_index(tensor_t*   t,
                              size_t      index,
                              const void* data,
                              dtype_id_t  dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Overwrite the element at an N-dimensional index without changing
 *        len or alloc.
 *
 * Resolves the N-dimensional index into a flat byte offset using the
 * tensor's stride array, then copies exactly data_size bytes from data
 * into that slot. This is the natural access pattern for matrix and
 * higher-dimensional tensor operations and is consistent with the
 * C-order (row-major) strides computed at init time.
 *
 * This function is only valid for TENSOR_FIXED_SHAPE tensors. A
 * TENSOR_DYNAMIC_1D tensor has no row or column structure and must be
 * accessed via set_tensor_index instead.
 *
 * @param t      Pointer to the target tensor. Must not be NULL.
 * @param idx    Array of t->ndim indices, one per dimension. Must not be
 *               NULL. idx[i] must be < t->shape[i] for all i in
 *               [0, t->ndim).
 * @param data   Pointer to the replacement value. Must not be NULL.
 *               Must point to at least t->data_size bytes.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if t, idx, or data is NULL
 *         - TYPE_MISMATCH if dtype != t->dtype
 *         - ILLEGAL_STATE if t->mode != TENSOR_FIXED_SHAPE
 *         - OUT_OF_BOUNDS if any idx[i] >= t->shape[i]
 */
error_code_t set_tensor_nd_index(tensor_t*      t,
                                 const size_t*  idx,
                                 const void*    data,
                                 dtype_id_t     dtype);
// -------------------------------------------------------------------------------- 

/**
 * @brief Append one element to the back of a dynamic 1-D tensor.
 *
 * Copies exactly data_size bytes from data into the next available slot
 * at index len, then increments len by one. If the tensor is full and
 * growth is true, the data buffer is reallocated before the copy using
 * the tiered growth strategy (_compute_new_alloc). If growth is false
 * and the tensor is full, no data is written and CAPACITY_OVERFLOW is
 * returned.
 *
 * This is an O(1) amortised operation when growth is enabled.
 *
 * @param t      Pointer to the target tensor. Must not be NULL.
 *               Must have mode == ARRAY_STRUCT.
 * @param data   Pointer to the value to append. Must not be NULL.
 *               Must point to at least t->data_size bytes.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if t or data is NULL
 *         - PRECONDITION_FAIL if t->mode != ARRAY_STRUCT
 *         - TYPE_MISMATCH     if dtype != t->dtype
 *         - CAPACITY_OVERFLOW if the tensor is full and growth is false,
 *                             or if the allocator does not support
 *                             reallocation
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *         - OUT_OF_MEMORY     if reallocation fails
 */
error_code_t push_back_tensor(tensor_t*   t,
                              const void* data,
                              dtype_id_t  dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Prepend one element to the front of a dynamic 1-D tensor.
 *
 * Shifts all existing elements one slot toward the back via memmove, then
 * copies exactly data_size bytes from data into slot 0 and increments len
 * by one. If the tensor is full and growth is true, the data buffer is
 * reallocated before the shift. The reallocation is performed first so
 * that the buffer pointer is stable during the memmove.
 *
 * This is an O(n) operation due to the element shift.
 *
 * @param t      Pointer to the target tensor. Must not be NULL.
 *               Must have mode == ARRAY_STRUCT.
 * @param data   Pointer to the value to prepend. Must not be NULL.
 *               Must point to at least t->data_size bytes.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if t or data is NULL
 *         - PRECONDITION_FAIL if t->mode != ARRAY_STRUCT
 *         - TYPE_MISMATCH     if dtype != t->dtype
 *         - CAPACITY_OVERFLOW if the tensor is full and growth is false,
 *                             or if the allocator does not support
 *                             reallocation
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *         - OUT_OF_MEMORY     if reallocation fails
 */
error_code_t push_front_tensor(tensor_t*   t,
                               const void* data,
                               dtype_id_t  dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Insert one element at an arbitrary index in a dynamic 1-D tensor.
 *
 * Inserts data at the given index by shifting all elements at positions
 * [index, len) one slot toward the back via memmove, then copying exactly
 * data_size bytes from data into the vacated slot and incrementing len by
 * one. The reallocation is performed before the shift so that the buffer
 * pointer is stable during the memmove.
 *
 * As fast paths, index == 0 delegates to push_front_tensor and
 * index == len delegates to push_back_tensor, avoiding an unnecessary
 * memmove in both cases.
 *
 * Valid index range is [0, len] inclusive. Passing index == len is
 * equivalent to push_back_tensor. Passing index > len returns
 * OUT_OF_BOUNDS without modifying the tensor.
 *
 * This is an O(n) operation due to the element shift.
 *
 * @param t      Pointer to the target tensor. Must not be NULL.
 *               Must have mode == ARRAY_STRUCT.
 * @param data   Pointer to the value to insert. Must not be NULL.
 *               Must point to at least t->data_size bytes.
 * @param index  Zero-based position at which to insert. Must be <= t->len.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if t or data is NULL
 *         - PRECONDITION_FAIL if t->mode != ARRAY_STRUCT
 *         - TYPE_MISMATCH     if dtype != t->dtype
 *         - OUT_OF_BOUNDS     if index > t->len
 *         - CAPACITY_OVERFLOW if the tensor is full and growth is false,
 *                             or if the allocator does not support
 *                             reallocation
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *         - OUT_OF_MEMORY     if reallocation fails
 */
error_code_t push_at_tensor(tensor_t*   t,
                            const void* data,
                            size_t      index,
                            dtype_id_t  dtype);

// -------------------------------------------------------------------------------- 

/**
 * @brief Remove and optionally retrieve the last element of a dynamic
 *        1-D tensor.
 *
 * Decrements len by one, exposing the vacated slot as spare capacity.
 * If out is non-NULL, copies exactly data_size bytes from the removed
 * slot into the caller-provided buffer before the slot is considered
 * free. The element bytes remain in the buffer until overwritten by a
 * subsequent push — the caller must not rely on them after this call.
 *
 * This is an O(1) operation.
 *
 * @param t      Pointer to the target tensor. Must not be NULL.
 *               Must have mode == ARRAY_STRUCT.
 * @param out    Caller-provided buffer to receive the removed element.
 *               May be NULL if the value is not needed. When non-NULL,
 *               must point to at least t->data_size bytes.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if t is NULL
 *         - PRECONDITION_FAIL if t->mode != ARRAY_STRUCT
 *         - TYPE_MISMATCH     if dtype != t->dtype
 *         - EMPTY             if t->len == 0
 */
error_code_t pop_back_tensor(tensor_t*   t,
                             void*       out,
                             dtype_id_t  dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally retrieve the first element of a dynamic
 *        1-D tensor.
 *
 * Copies exactly data_size bytes from slot 0 into out (if non-NULL),
 * then shifts all remaining elements one slot toward the front via
 * memmove and decrements len by one. The shift is skipped when len
 * reaches zero after the removal.
 *
 * This is an O(n) operation due to the element shift.
 *
 * @param t      Pointer to the target tensor. Must not be NULL.
 *               Must have mode == ARRAY_STRUCT.
 * @param out    Caller-provided buffer to receive the removed element.
 *               May be NULL if the value is not needed. When non-NULL,
 *               must point to at least t->data_size bytes.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if t is NULL
 *         - PRECONDITION_FAIL if t->mode != ARRAY_STRUCT
 *         - TYPE_MISMATCH     if dtype != t->dtype
 *         - EMPTY             if t->len == 0
 */
error_code_t pop_front_tensor(tensor_t*   t,
                              void*       out,
                              dtype_id_t  dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally retrieve the element at an arbitrary index
 *        in a dynamic 1-D tensor.
 *
 * Copies exactly data_size bytes from the slot at index into out (if
 * non-NULL), then shifts all elements at positions (index, len) one slot
 * toward the front via memmove and decrements len by one.
 *
 * As fast paths, index == 0 delegates to pop_front_tensor and
 * index == len - 1 delegates to pop_back_tensor, avoiding an unnecessary
 * memmove in both cases.
 *
 * Valid index range is [0, len). Passing index >= len returns
 * OUT_OF_BOUNDS without modifying the tensor.
 *
 * This is an O(n) operation due to the element shift.
 *
 * @param t      Pointer to the target tensor. Must not be NULL.
 *               Must have mode == ARRAY_STRUCT.
 * @param out    Caller-provided buffer to receive the removed element.
 *               May be NULL if the value is not needed. When non-NULL,
 *               must point to at least t->data_size bytes.
 * @param index  Zero-based position of the element to remove.
 *               Must be < t->len.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if t is NULL
 *         - PRECONDITION_FAIL if t->mode != ARRAY_STRUCT
 *         - TYPE_MISMATCH     if dtype != t->dtype
 *         - EMPTY             if t->len == 0
 *         - OUT_OF_BOUNDS     if index >= t->len
 */
error_code_t pop_at_tensor(tensor_t*   t,
                           void*       out,
                           size_t      index,
                           dtype_id_t  dtype);
// ================================================================================ 
// ================================================================================ 
// RETRIEVE DATA

/**
 * @brief Copy one element out of the tensor by flat index.
 *
 * Copies exactly t->data_size bytes from the tensor's internal buffer at
 * the given flat index into the caller-provided output buffer. The flat
 * index addresses elements in the order they are laid out in memory —
 * for a C-order tensor this is row-major order. The tensor is not
 * modified.
 *
 * The bound check depends on mode:
 *   TENSOR_FIXED_SHAPE: index must be < t->alloc. All allocated slots are
 *                       always live regardless of len, so this function
 *                       remains valid after clear_tensor.
 *   TENSOR_DYNAMIC_1D: index must be < t->len. Only populated slots are
 *                       addressable; slots in [len, alloc) are off-limits
 *                       until exposed by a push operation.
 *
 * @param t      Pointer to the source tensor. Must not be NULL.
 * @param index  Zero-based flat index of the element to retrieve.
 *               FIXED_SHAPE: must be < t->alloc.
 *               DYNAMIC_1D:  must be < t->len.
 * @param out    Caller-provided buffer to copy the element into.
 *               Must not be NULL. Must be at least t->data_size bytes.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if t or out is NULL
 *         - TYPE_MISMATCH if dtype != t->dtype
 *         - OUT_OF_BOUNDS if index >= t->alloc (FIXED_SHAPE) or
 *                                   index >= t->len  (DYNAMIC_1D)
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
 * pattern for matrix and higher-dimensional tensor operations and is
 * consistent with the C-order (row-major) strides computed at init time.
 * The tensor is not modified.
 *
 * This function is only valid for TENSOR_FIXED_SHAPE tensors. A
 * TENSOR_DYNAMIC_1D tensor has no row or column structure and must be
 * accessed via get_tensor_index instead.
 *
 * @param t      Pointer to the source tensor. Must not be NULL.
 * @param idx    Array of t->ndim indices, one per dimension. Must not be
 *               NULL. idx[i] must be < t->shape[i] for all i in
 *               [0, t->ndim).
 * @param out    Caller-provided buffer to copy the element into.
 *               Must not be NULL. Must be at least t->data_size bytes.
 * @param dtype  Type identifier. Must match t->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if t, idx, or out is NULL
 *         - TYPE_MISMATCH if dtype != t->dtype
 *         - ILLEGAL_STATE if t->mode != TENSOR_FIXED_SHAPE
 *         - OUT_OF_BOUNDS if any idx[i] >= t->shape[i]
 */
error_code_t get_tensor_nd_index(const tensor_t* t,
                                 const size_t*   idx,
                                 void*           out,
                                 dtype_id_t      dtype);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* tensor_H */
// ================================================================================
// ================================================================================
// eof
