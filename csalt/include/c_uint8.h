// ================================================================================
// ================================================================================
// - File:    c_uint8.h
// - Purpose: This file contains prototypes for uint8_t data structures and 
//            associated functions
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 23, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here
#ifndef c_uint8_H
#define c_uint8_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "c_tensor.h"
#include "c_error.h"
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

/**
 * @brief Type-safe wrapper for a tensor containing uint8_t elements.
 *
 * Holds a pointer to the underlying generic tensor rather than embedding
 * it by value, which is required because tensor_t contains a flexible
 * array member and cannot legally appear as a struct member in C99/C11.
 * All generic tensor operations are reached through the base pointer.
 * Use the typed wrapper functions (init_uint8_array, push_back_uint8_tensor,
 * etc.) rather than accessing base directly.
 */
typedef struct {
    tensor_t* base; /**< Underlying generic tensor. Do not access directly. */
} uint8_tensor_t;

// --------------------------------------------------------------------------------

/**
 * @brief Expected-value return type for functions that produce a
 *        uint8_tensor_t.
 *
 * has_value must be checked before accessing either union member.
 *
 * @code{.c}
 * uint8_tensor_expect_t r = init_uint8_array(8, true, heap_allocator());
 * if (!r.has_value) { handle(r.u.error); }
 * uint8_tensor_t* a = r.u.value;
 * @endcode
 */
typedef struct {
    bool has_value;
    union {
        uint8_tensor_t* value;
        error_code_t    error;
    } u;
} uint8_tensor_expect_t;

// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialise a fixed-shape N-dimensional tensor of uint8_t elements.
 *
 * Allocates the underlying tensor_t via init_tensor and then allocates a
 * thin uint8_tensor_t wrapper struct through the same allocator. All slots
 * are zero-initialised at construction and considered live for the lifetime
 * of the tensor — len equals alloc equals the product of all shape
 * dimensions. Both allocations must be returned by calling
 * return_uint8_tensor when the tensor is no longer needed.
 *
 * @param ndim    Number of dimensions. Must be > 0 and <= 255.
 * @param shape   Array of ndim dimension sizes. Must not be NULL.
 *                Each shape[i] must be > 0.
 * @param alloc_v Allocator vtable for all memory operations.
 *                alloc_v.allocate must not be NULL.
 *
 * @return uint8_tensor_expect_t with has_value true on success.
 *         On failure has_value is false and u.error is set.
 *
 * @code{.c}
 * // Create a 3x4 matrix of uint8_t values
 * size_t shape[] = { 3u, 4u };
 * uint8_tensor_expect_t r = init_uint8_tensor(2u, shape, heap_allocator());
 * if (!r.has_value) { fprintf(stderr, "%s\n", error_to_string(r.u.error)); }
 * uint8_tensor_t* mat = r.u.value;
 *
 * // Write to element (1, 2)
 * size_t idx[] = { 1u, 2u };
 * set_uint8_tensor_nd_index(mat, idx, 42u);
 *
 * // Read it back
 * uint8_t val;
 * get_uint8_tensor_nd_index(mat, idx, &val);   // val == 42
 *
 * return_uint8_tensor(mat);
 * @endcode
 */
uint8_tensor_expect_t init_uint8_tensor(uint8_t            ndim,
                                        const size_t*      shape,
                                        allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/*
 * Allocator convention for uint8_tensor_t functions:
 *
 * init_uint8_array        — takes allocator_vtable_t by value. An allocator
 *                           is mandatory since there is no existing tensor to
 *                           inherit one from.
 *
 * copy_uint8_tensor       — takes allocator_vtable_t* (pointer). Pass NULL to
 * slice_uint8_tensor_array  inherit the allocator from the source tensor, or
 *                           a non-NULL pointer to use a different allocator.
 */
 
/**
 * @brief Initialise a dynamic 1-D array of uint8_t elements.
 *
 * Allocates the underlying tensor_t via init_tensor_array and then
 * allocates a thin uint8_tensor_t wrapper struct through the same
 * allocator. Both allocations must be returned by calling
 * return_uint8_tensor when the array is no longer needed.
 *
 * @param capacity  Number of elements to allocate. Must be > 0.
 * @param growth    If true, push operations may reallocate the buffer
 *                  when capacity is reached.
 * @param alloc_v   Allocator vtable for all memory operations.
 *                  alloc_v.allocate must not be NULL.
 *
 * @return uint8_tensor_expect_t with has_value true on success.
 *         On failure has_value is false and u.error is set.
 *
 * @code{.c}
 * uint8_tensor_expect_t r = init_uint8_array(16, true, heap_allocator());
 * if (!r.has_value) { fprintf(stderr, "%s\n", error_to_string(r.u.error)); }
 * uint8_tensor_t* arr = r.u.value;
 * push_back_uint8_tensor(arr, 42u);
 * return_uint8_tensor(arr);
 * @endcode
 */
uint8_tensor_expect_t init_uint8_array(size_t capacity,
                                       bool growth,
                                       allocator_vtable_t alloc_v);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a uint8_tensor_t and its underlying tensor to the allocator.
 *
 * Returns the data buffer and tensor_t struct via return_tensor, then
 * returns the wrapper struct itself. The allocator is read from the tensor
 * before return_tensor is called so it remains valid for the wrapper
 * deallocation. Silently ignored if t is NULL.
 *
 * @param t  Pointer to the wrapper to return. May be NULL.
 *
 * @code{.c}
 * uint8_tensor_expect_t r = init_uint8_array(8, false, heap_allocator());
 * uint8_tensor_t* arr = r.u.value;
 * // ... use arr ...
 * return_uint8_tensor(arr);
 * arr = NULL;   // guard against accidental reuse
 * @endcode
 */
void return_uint8_tensor(uint8_tensor_t* t);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a deep copy of a uint8_tensor_t.
 *
 * Copies the underlying tensor via copy_tensor then allocates a new
 * uint8_tensor_t wrapper struct. Both allocations are returned by calling
 * return_uint8_tensor on the returned pointer. On partial failure the
 * copied tensor_t is returned before propagating the error so no memory
 * is leaked. The copy is independent of src — mutations to one do not
 * affect the other.
 *
 * @param src     Pointer to the source uint8_tensor_t. Must not be NULL.
 * @param alloc_v Pointer to the allocator vtable for the new memory.
 *                Pass NULL to inherit the allocator from src.
 *
 * @return uint8_tensor_expect_t with has_value true on success.
 *         On failure has_value is false and u.error is set.
 *
 * @code{.c}
 * // Copy using src's own allocator
 * uint8_tensor_expect_t c = copy_uint8_tensor(arr, NULL);
 * if (!c.has_value) { fprintf(stderr, "%s\n", error_to_string(c.u.error)); }
 * uint8_tensor_t* copy = c.u.value;
 * // ...
 * return_uint8_tensor(copy);
 *
 * // Copy using a different allocator
 * allocator_vtable_t arena = arena_allocator(&my_arena);
 * uint8_tensor_expect_t c2 = copy_uint8_tensor(arr, &arena);
 * @endcode
 */
uint8_tensor_expect_t copy_uint8_tensor(const uint8_tensor_t* src,
                                        allocator_vtable_t* alloc_v);
// ================================================================================ 
// ================================================================================ 
// INTROSPECTION 

/**
 * @brief Return the number of populated elements in the array.
 *
 * For ARRAY_STRUCT tensors this is the number of elements pushed so far.
 * For TENSOR_STRUCT tensors this equals alloc (all slots are always live).
 * Returns 0 if t is NULL.
 *
 * @code{.c}
 * size_t n = uint8_tensor_size(arr);   // e.g. 3 after three push_back calls
 * @endcode
 */
static inline size_t uint8_tensor_size(const uint8_tensor_t* t) {
    if (t == NULL) return 0u;
    return tensor_size(t->base);
}

// --------------------------------------------------------------------------------

/**
 * @brief Return the allocated capacity of the array in elements.
 *
 * Returns 0 if t is NULL.
 *
 * @code{.c}
 * size_t cap = uint8_tensor_alloc(arr);   // e.g. 16 after init_uint8_array(16,...)
 * @endcode
 */
static inline size_t uint8_tensor_alloc(const uint8_tensor_t* t) {
    if (t == NULL) return 0u;
    return tensor_alloc(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the size of one element in bytes.
 *
 * Always returns sizeof(uint8_t) == 1 for a correctly initialised
 * uint8_tensor_t. Returns 0 if t is NULL.
 *
 * @code{.c}
 * size_t esz = uint8_tensor_data_size(arr);   // 1
 * @endcode
 */
static inline size_t uint8_tensor_data_size(const uint8_tensor_t* t) {
    if (t == NULL) return 0u;
    return tensor_data_size(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the dtype identifier of the array.
 *
 * Always returns UINT8_TYPE for a correctly initialised uint8_tensor_t.
 * Returns UNKNOWN_TYPE if t is NULL.
 *
 * @code{.c}
 * dtype_id_t id = uint8_tensor_dtype(arr);   // UINT8_TYPE
 * @endcode
 */
static inline dtype_id_t uint8_tensor_dtype(const uint8_tensor_t* t) {
    if (t == NULL) return UNKNOWN_TYPE;
    return tensor_dtype(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return true if the array contains no populated elements.
 *
 * Returns true if t is NULL.
 *
 * @code{.c}
 * uint8_tensor_expect_t r = init_uint8_array(8, false, heap_allocator());
 * uint8_tensor_t* arr = r.u.value;
 * bool empty = is_uint8_tensor_empty(arr);   // true before any push
 * push_back_uint8_tensor(arr, 1u);
 * empty = is_uint8_tensor_empty(arr);         // false after push
 * return_uint8_tensor(arr);
 * @endcode
 */
static inline bool is_uint8_tensor_empty(const uint8_tensor_t* t) {
    if (t == NULL) return true;
    return is_tensor_empty(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return true if the array is at full capacity.
 *
 * Returns true if t is NULL.
 *
 * @code{.c}
 * uint8_tensor_expect_t r = init_uint8_array(2, false, heap_allocator());
 * uint8_tensor_t* arr = r.u.value;
 * push_back_uint8_tensor(arr, 1u);
 * push_back_uint8_tensor(arr, 2u);
 * bool full = is_uint8_tensor_full(arr);   // true — len == alloc == 2
 * return_uint8_tensor(arr);
 * @endcode
 */
static inline bool is_uint8_tensor_full(const uint8_tensor_t* t) {
    if (t == NULL) return true;
    return is_tensor_full(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return true if ptr points to a valid element-aligned address
 *        within the live region of the array's data buffer.
 *
 * The pointer must be >= data, < data + len * data_size, and aligned to
 * an element boundary. Returns false if either argument is NULL.
 *
 * @code{.c}
 * uint8_tensor_expect_t r = init_uint8_array(4, false, heap_allocator());
 * uint8_tensor_t* arr = r.u.value;
 * push_back_uint8_tensor(arr, 10u);
 * const uint8_t* p = arr->base->data;
 * bool valid = is_uint8_tensor_ptr(arr, p);   // true — points to element 0
 * return_uint8_tensor(arr);
 * @endcode
 */
static inline bool is_uint8_tensor_ptr(const uint8_tensor_t* t, const uint8_t* ptr) {
    if (t == NULL || ptr == NULL) return false;
    return is_tensor_ptr(t->base, ptr);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the number of dimensions of the underlying tensor.
 *
 * Always returns 1 for a tensor initialised via init_uint8_array.
 * Returns 0 if t is NULL.
 *
 * @code{.c}
 * uint8_t ndim = uint8_tensor_ndim(arr);   // 1
 * @endcode
 */
static inline uint8_t uint8_tensor_ndim(const uint8_tensor_t* t) {
    if (t == NULL) return 0u;
    return tensor_ndim(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the size of a single dimension.
 *
 * For a 1-D array, dim must be 0. Returns 0 if t is NULL or dim is out
 * of range.
 *
 * @code{.c}
 * uint8_tensor_expect_t r = init_uint8_array(8, false, heap_allocator());
 * uint8_tensor_t* arr = r.u.value;
 * size_t d0 = uint8_tensor_shape_dim(arr, 0u);   // 8
 * return_uint8_tensor(arr);
 * @endcode
 */
static inline size_t uint8_tensor_shape_dim(const uint8_tensor_t* t, uint8_t dim) {
    if (t == NULL) return 0u;
    return tensor_shape_dim(t->base, dim);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Copy the shape of the tensor into a caller-provided buffer.
 *
 * Copies min(ndim, count) dimension sizes into out. Returns INVALID_ARG
 * if count < ndim (output truncated). Returns NULL_POINTER if t or out
 * is NULL.
 *
 * @code{.c}
 * size_t shape[1];
 * error_code_t err = uint8_tensor_shape(arr, shape, 1u);
 * // shape[0] now holds the single dimension size
 * @endcode
 */
static inline error_code_t uint8_tensor_shape(const uint8_tensor_t* t, size_t* out, uint8_t count) {
    if (t == NULL || out == NULL) return NULL_POINTER;
    return tensor_shape(t->base, out, count);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a read-only pointer to the internal shape array.
 *
 * The pointer is valid for the lifetime of the tensor and must not be
 * modified or freed. Returns NULL if t is NULL.
 *
 * @code{.c}
 * const size_t* sp = uint8_tensor_shape_ptr(arr);
 * if (sp) printf("dim0 = %zu\n", sp[0]);
 * @endcode
 */
static inline const size_t* uint8_tensor_shape_ptr(const uint8_tensor_t* t) {
    if (t == NULL) return NULL;
    return tensor_shape_ptr(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a read-only pointer to the internal strides array.
 *
 * Strides are in bytes. For a contiguous 1-D uint8_t array,
 * strides[0] == 1. The pointer is valid for the lifetime of the tensor
 * and must not be modified or freed. Returns NULL if t is NULL.
 *
 * @code{.c}
 * const size_t* strides = uint8_tensor_strides_ptr(arr);
 * if (strides) printf("stride0 = %zu\n", strides[0]);   // 1
 * @endcode
 */
static inline const size_t* uint8_tensor_strides_ptr(const uint8_tensor_t* t) {
    if (t == NULL) return NULL;
    return tensor_strides_ptr(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Write a human-readable shape string into a caller-provided buffer.
 *
 * Formats the shape as "(d0)" for a 1-D array. Intended for debugging
 * and logging only. Returns CAPACITY_OVERFLOW if the buffer is too small.
 * Returns NULL_POINTER if t or buf is NULL.
 *
 * @code{.c}
 * char buf[32];
 * error_code_t err = uint8_tensor_shape_str(arr, buf, sizeof(buf));
 * if (err == NO_ERROR) printf("shape = %s\n", buf);   // e.g. "(8)"
 * @endcode
 */
static inline error_code_t uint8_tensor_shape_str(const uint8_tensor_t* t, 
                                                  char* buf, 
                                                  size_t buf_len) {
    if (t == NULL || buf == NULL) return NULL_POINTER;
    return tensor_shape_str(t->base, buf, buf_len);
}
// ================================================================================ 
// ================================================================================ 
// UTILITY OPERATIONS 

/**
 * @brief Reset the array to empty without releasing its allocated buffer.
 *
 * Zeroes all bytes in the data buffer and sets len to 0 if the tensor is an
 * array. The allocated
 * capacity, shape, strides, and mode are preserved. Returns NULL_POINTER
 * if t is NULL.
 *
 * @code{.c}
 * push_back_uint8_tensor(arr, 1u);
 * push_back_uint8_tensor(arr, 2u);
 * clear_uint8_tensor(arr);
 * size_t n = uint8_tensor_size(arr);   // 0
 * @endcode
 */
static inline error_code_t clear_uint8_tensor(uint8_tensor_t* t) {
    if (t == NULL) return NULL_POINTER;
    return clear_tensor(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Append all elements of src to the back of dst.
 *
 * Copies src->len elements from src's data buffer into the next available
 * slots in dst's data buffer and increments dst->len accordingly. Both
 * tensors must be in ARRAY_STRUCT mode. If dst is full and growth is true
 * the buffer is reallocated once to cover all of src rather than
 * element-by-element. Returns NULL_POINTER if dst or src is NULL.
 *
 * @code{.c}
 * uint8_tensor_expect_t r1 = init_uint8_array(8, false, heap_allocator());
 * uint8_tensor_expect_t r2 = init_uint8_array(4, false, heap_allocator());
 * uint8_tensor_t* dst = r1.u.value;
 * uint8_tensor_t* src = r2.u.value;
 * push_back_uint8_tensor(dst, 1u);
 * push_back_uint8_tensor(src, 2u);
 * push_back_uint8_tensor(src, 3u);
 * concat_uint8_tensor_array(dst, src);   // dst = [1, 2, 3]
 * return_uint8_tensor(dst);
 * return_uint8_tensor(src);
 * @endcode
 */
static inline error_code_t concat_uint8_tensor_array(uint8_tensor_t* dst,
                                                     const uint8_tensor_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_tensor_array(dst->base, src->base, UINT8_TYPE);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a new array containing a copy of the elements in src at
 *        positions [start, end).
 *
 * The range is half-open: start is inclusive, end is exclusive. Both must
 * be <= src->len and start must be strictly less than end. The returned
 * array is independent of src — mutations to one do not affect the other.
 * The caller is responsible for calling return_uint8_tensor on the result.
 *
 * Pass NULL for alloc_v to inherit the allocator from src.
 *
 * @param src     Pointer to the source array. Must not be NULL.
 * @param start   Zero-based index of the first element to include.
 * @param end     Zero-based index one past the last element to include.
 * @param alloc_v Pointer to the allocator vtable. NULL inherits from src.
 *
 * @return uint8_tensor_expect_t with has_value true on success.
 *
 * @code{.c}
 * // arr = [10, 20, 30, 40, 50]
 * uint8_tensor_expect_t s = slice_uint8_tensor_array(arr, 1u, 4u, NULL);
 * if (!s.has_value) { fprintf(stderr, "%s\n", error_to_string(s.u.error)); }
 * uint8_tensor_t* sl = s.u.value;   // sl = [20, 30, 40]
 * return_uint8_tensor(sl);
 * @endcode
 */
uint8_tensor_expect_t slice_uint8_tensor_array(const uint8_tensor_t* src,
                                               size_t start,
                                               size_t end,
                                               allocator_vtable_t* alloc_v);
// ================================================================================ 
// ================================================================================ 
// ADD AND REMOVE DATA 

/**
 * @brief Overwrite the element at a flat index without changing len.
 *
 * For ARRAY_STRUCT mode the index must be < len. For TENSOR_STRUCT mode
 * the index must be < alloc. Returns NULL_POINTER if t is NULL,
 * OUT_OF_BOUNDS if the index is out of range.
 *
 * @code{.c}
 * push_back_uint8_tensor(arr, 0u);   // arr = [0]
 * set_uint8_tensor_index(arr, 0u, 99u);   // arr = [99]
 * @endcode
 */
static inline error_code_t set_uint8_tensor_index(uint8_tensor_t* t,
                                                  size_t index,
                                                  const uint8_t data) {
    if (t == NULL) return NULL_POINTER;
    return set_tensor_index(t->base, index, &data, UINT8_TYPE);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Overwrite the element at an N-dimensional index without changing len.
 *
 * Only valid for TENSOR_STRUCT mode. Returns ILLEGAL_STATE if called on
 * an ARRAY_STRUCT tensor. idx must point to an array of ndim indices.
 * Returns NULL_POINTER if t or idx is NULL, OUT_OF_BOUNDS if any index
 * exceeds its dimension bound.
 *
 * @code{.c}
 * // For a 3x3 uint8 matrix initialised via init_tensor
 * size_t idx[] = { 1u, 2u };
 * set_uint8_tensor_nd_index(mat, idx, 42u);   // mat[1][2] = 42
 * @endcode
 */
static inline error_code_t set_uint8_tensor_nd_index(uint8_tensor_t* t,
                                                     const size_t* idx,
                                                     uint8_t data) {
    if (t == NULL) return NULL_POINTER;
    return set_tensor_nd_index(t->base, idx, &data, UINT8_TYPE);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Append one element to the back of a dynamic 1-D array.
 *
 * Increments len by one after copying the value. If the array is full
 * and growth is true the buffer is reallocated automatically. Returns
 * CAPACITY_OVERFLOW if the array is full and growth is false, or if the
 * allocator does not support reallocation. Returns NULL_POINTER if t is
 * NULL, PRECONDITION_FAIL if mode != ARRAY_STRUCT.
 *
 * @code{.c}
 * push_back_uint8_tensor(arr, 10u);
 * push_back_uint8_tensor(arr, 20u);
 * // arr = [10, 20], len == 2
 * @endcode
 */
static inline error_code_t push_back_uint8_array(uint8_tensor_t* t,
                                                 uint8_t data) {
    if (t == NULL) return NULL_POINTER;
    return push_back_tensor(t->base, &data, UINT8_TYPE);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Prepend one element to the front of a dynamic 1-D array.
 *
 * Shifts all existing elements one slot toward the back then copies the
 * new value into slot 0. O(n) due to the shift. Returns NULL_POINTER if
 * t is NULL, PRECONDITION_FAIL if mode != ARRAY_STRUCT.
 *
 * @code{.c}
 * push_back_uint8_tensor(arr, 10u);
 * push_front_uint8_tensor(arr, 99u);
 * // arr = [99, 10]
 * @endcode
 */
static inline error_code_t push_front_uint8_array(uint8_tensor_t* t,
                                                  uint8_t data) {
    if (t == NULL) return NULL_POINTER;
    return push_front_tensor(t->base, &data, UINT8_TYPE);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Insert one element at an arbitrary index in a dynamic 1-D array.
 *
 * Shifts elements at positions [index, len) one slot toward the back then
 * copies the new value into the vacated slot. Valid index range is
 * [0, len]. index == 0 is equivalent to push_front, index == len is
 * equivalent to push_back. O(n) due to the shift. Returns NULL_POINTER if
 * t is NULL, OUT_OF_BOUNDS if index > len, PRECONDITION_FAIL if mode !=
 * ARRAY_STRUCT.
 *
 * @code{.c}
 * // arr = [10, 20, 30]
 * push_at_uint8_tensor(arr, 99u, 1u);
 * // arr = [10, 99, 20, 30]
 * @endcode
 */
static inline error_code_t push_at_uint8_array(uint8_tensor_t* t,
                                               uint8_t data,
                                               size_t index) {
    if (t == NULL) return NULL_POINTER;
    return push_at_tensor(t->base, &data, index, UINT8_TYPE);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Remove and optionally retrieve the last element of a dynamic
 *        1-D array.
 *
 * Decrements len by one. If out is non-NULL, copies the removed value
 * into the caller-provided buffer. Pass NULL for out to discard the
 * value. Returns NULL_POINTER if t is NULL, EMPTY if len == 0,
 * PRECONDITION_FAIL if mode != ARRAY_STRUCT.
 *
 * @code{.c}
 * // arr = [10, 20, 30]
 * uint8_t val;
 * pop_back_uint8_tensor(arr, &val);   // val == 30, arr = [10, 20]
 * pop_back_uint8_tensor(arr, NULL);   // discard, arr = [10]
 * @endcode
 */
static inline error_code_t pop_back_uint8_array(uint8_tensor_t* t,
                                                uint8_t* out) {
    if (t == NULL) return NULL_POINTER;
    return pop_back_tensor(t->base, out, UINT8_TYPE);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Remove and optionally retrieve the first element of a dynamic
 *        1-D array.
 *
 * Copies the first element into out (if non-NULL), then shifts all
 * remaining elements one slot toward the front and decrements len. O(n)
 * due to the shift. Returns NULL_POINTER if t is NULL, EMPTY if len == 0,
 * PRECONDITION_FAIL if mode != ARRAY_STRUCT.
 *
 * @code{.c}
 * // arr = [10, 20, 30]
 * uint8_t val;
 * pop_front_uint8_tensor(arr, &val);   // val == 10, arr = [20, 30]
 * @endcode
 */
static inline error_code_t pop_front_uint8_array(uint8_tensor_t* t,
                                                 uint8_t* out) {
    if (t == NULL) return NULL_POINTER;
    return pop_front_tensor(t->base, out, UINT8_TYPE);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Remove and optionally retrieve the element at an arbitrary index
 *        in a dynamic 1-D array.
 *
 * Copies the element at index into out (if non-NULL), shifts all elements
 * after index one slot toward the front, and decrements len. index == 0
 * delegates to pop_front, index == len - 1 delegates to pop_back. O(n)
 * due to the shift. Returns NULL_POINTER if t is NULL, EMPTY if len == 0,
 * OUT_OF_BOUNDS if index >= len, PRECONDITION_FAIL if mode != ARRAY_STRUCT.
 *
 * @code{.c}
 * // arr = [10, 20, 30, 40]
 * uint8_t val;
 * pop_at_uint8_tensor(arr, &val, 2u);   // val == 30, arr = [10, 20, 40]
 * @endcode
 */
static inline error_code_t pop_at_uint8_array(uint8_tensor_t* t,
                                              uint8_t* out,
                                              size_t index) {
    if (t == NULL) return NULL_POINTER;
    return pop_at_tensor(t->base, out, index, UINT8_TYPE);
}
// ================================================================================ 
// ================================================================================ 
// RETRIEVE DATA

/**
 * @brief Copy one element out of the array by flat index.
 *
 * For ARRAY_STRUCT mode the index must be < len. For TENSOR_STRUCT mode
 * the index must be < alloc. Returns NULL_POINTER if t or out is NULL,
 * OUT_OF_BOUNDS if the index is out of range.
 *
 * @code{.c}
 * // arr = [10, 20, 30]
 * uint8_t val;
 * get_uint8_tensor_index(arr, 1u, &val);   // val == 20
 * @endcode
 */
static inline error_code_t get_uint8_tensor_index(const uint8_tensor_t* t,
                                                  size_t index,
                                                  uint8_t* out) {
    if (t == NULL || out == NULL) return NULL_POINTER;
    return get_tensor_index(t->base, index, out, UINT8_TYPE);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Copy one element out of the tensor by N-dimensional index.
 *
 * Only valid for TENSOR_STRUCT mode. Returns ILLEGAL_STATE if called on
 * an ARRAY_STRUCT tensor. idx must point to an array of ndim indices.
 * Returns NULL_POINTER if t, idx, or out is NULL, OUT_OF_BOUNDS if any
 * index exceeds its dimension bound.
 *
 * @code{.c}
 * // For a 3x3 uint8 matrix
 * size_t idx[] = { 1u, 2u };
 * uint8_t val;
 * get_uint8_tensor_nd_index(mat, idx, &val);   // val = mat[1][2]
 * @endcode
 */
static inline error_code_t get_uint8_tensor_nd_index(const uint8_tensor_t* t,
                                                     const size_t* idx,
                                                     uint8_t* out) {
    if (t == NULL || idx == NULL || out == NULL) return NULL_POINTER;
    return get_tensor_nd_index(t->base, idx, out, UINT8_TYPE);
}
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_uint8_H */
// ================================================================================
// ================================================================================
// eof
