// ================================================================================
// ================================================================================
// - File:    c_ldouble.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 08, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_ldouble_H
#define c_ldouble_H

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
 * @brief Type-safe wrapper for a tensor containing long double elements.
 *
 * Holds a pointer to the underlying generic tensor rather than embedding
 * it by value, which is required because tensor_t contains a flexible
 * array member and cannot legally appear as a struct member in C99/C11.
 * All generic tensor operations are reached through the base pointer.
 * Use the typed wrapper functions (init_ldouble_array, push_back_ldouble_tensor,
 * etc.) rather than accessing base directly.
 */
typedef struct {
    tensor_t* base; /**< Underlying generic tensor. Do not access directly. */
} ldouble_tensor_t;

// --------------------------------------------------------------------------------

/**
 * @brief Expected-value return type for functions that produce a
 *        ldouble_tensor_t.
 *
 * has_value must be checked before accessing either union member.
 *
 * @code{.c}
 * ldouble_tensor_expect_t r = init_ldouble_array(8, true, heap_allocator());
 * if (!r.has_value) { handle(r.u.error); }
 * ldouble_tensor_t* a = r.u.value;
 * @endcode
 */
typedef struct {
    bool has_value;
    union {
        ldouble_tensor_t* value;
        error_code_t    error;
    } u;
} ldouble_tensor_expect_t;

// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialise a fixed-shape N-dimensional tensor of long double elements.
 *
 * Allocates the underlying tensor_t via init_tensor and then allocates a
 * thin ldouble_tensor_t wrapper struct through the same allocator. All slots
 * are zero-initialised at construction and considered live for the lifetime
 * of the tensor — len equals alloc equals the product of all shape
 * dimensions. Both allocations must be returned by calling
 * return_ldouble_tensor when the tensor is no longer needed.
 *
 * @param ndim    Number of dimensions. Must be > 0 and <= 255.
 * @param shape   Array of ndim dimension sizes. Must not be NULL.
 *                Each shape[i] must be > 0.
 * @param alloc_v Allocator vtable for all memory operations.
 *                alloc_v.allocate must not be NULL.
 *
 * @return ldouble_tensor_expect_t with has_value true on success.
 *         On failure has_value is false and u.error is set.
 *
 * @code{.c}
 * // Create a 3x4 matrix of long double values
 * size_t shape[] = { 3u, 4u };
 * ldouble_tensor_expect_t r = init_ldouble_tensor(2u, shape, heap_allocator());
 * if (!r.has_value) { fprintf(stderr, "%s\n", error_to_string(r.u.error)); }
 * ldouble_tensor_t* mat = r.u.value;
 *
 * // Write to element (1, 2)
 * size_t idx[] = { 1u, 2u };
 * set_ldouble_tensor_nd_index(mat, idx, 42.0f);
 *
 * // Read it back
 * long double val;
 * get_ldouble_tensor_nd_index(mat, idx, &val);   // val == 42.00
 *
 * return_ldouble_tensor(mat);
 * @endcode
 */
ldouble_tensor_expect_t init_ldouble_tensor(uint8_t            ndim,
                                            const size_t*      shape,
                                            allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/*
 * Allocator convention for ldouble_tensor_t functions:
 *
 * init_ldouble_array        — takes allocator_vtable_t by value. An allocator
 *                           is mandatory since there is no existing tensor to
 *                           inherit one from.
 *
 * copy_ldouble_tensor       — takes allocator_vtable_t* (pointer). Pass NULL to
 * slice_ldouble_tensor_array  inherit the allocator from the source tensor, or
 *                           a non-NULL pointer to use a different allocator.
 */
 
/**
 * @brief Initialise a dynamic 1-D array of long double elements.
 *
 * Allocates the underlying tensor_t via init_tensor_array and then
 * allocates a thin ldouble_tensor_t wrapper struct through the same
 * allocator. Both allocations must be returned by calling
 * return_ldouble_tensor when the array is no longer needed.
 *
 * @param capacity  Number of elements to allocate. Must be > 0.
 * @param growth    If true, push operations may reallocate the buffer
 *                  when capacity is reached.
 * @param alloc_v   Allocator vtable for all memory operations.
 *                  alloc_v.allocate must not be NULL.
 *
 * @return ldouble_tensor_expect_t with has_value true on success.
 *         On failure has_value is false and u.error is set.
 *
 * @code{.c}
 * ldouble_tensor_expect_t r = init_ldouble_array(16, true, heap_allocator());
 * if (!r.has_value) { fprintf(stderr, "%s\n", error_to_string(r.u.error)); }
 * ldouble_tensor_t* arr = r.u.value;
 * push_back_ldouble_tensor(arr, 42.0f);
 * return_ldouble_tensor(arr);
 * @endcode
 */
ldouble_tensor_expect_t init_ldouble_array(size_t capacity,
                                           bool growth,
                                           allocator_vtable_t alloc_v);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a ldouble_tensor_t and its underlying tensor to the allocator.
 *
 * Returns the data buffer and tensor_t struct via return_tensor, then
 * returns the wrapper struct itself. The allocator is read from the tensor
 * before return_tensor is called so it remains valid for the wrapper
 * deallocation. Silently ignored if t is NULL.
 *
 * @param t  Pointer to the wrapper to return. May be NULL.
 *
 * @code{.c}
 * ldouble_tensor_expect_t r = init_ldouble_array(8, false, heap_allocator());
 * ldouble_tensor_t* arr = r.u.value;
 * // ... use arr ...
 * return_ldouble_tensor(arr);
 * arr = NULL;   // guard against accidental reuse
 * @endcode
 */
void return_ldouble_tensor(ldouble_tensor_t* t);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a deep copy of a ldouble_tensor_t.
 *
 * Copies the underlying tensor via copy_tensor then allocates a new
 * ldouble_tensor_t wrapper struct. Both allocations are returned by calling
 * return_ldouble_tensor on the returned pointer. On partial failure the
 * copied tensor_t is returned before propagating the error so no memory
 * is leaked. The copy is independent of src — mutations to one do not
 * affect the other.
 *
 * @param src     Pointer to the source ldouble_tensor_t. Must not be NULL.
 * @param alloc_v Pointer to the allocator vtable for the new memory.
 *                Pass NULL to inherit the allocator from src.
 *
 * @return ldouble_tensor_expect_t with has_value true on success.
 *         On failure has_value is false and u.error is set.
 *
 * @code{.c}
 * // Copy using src's own allocator
 * ldouble_tensor_expect_t c = copy_ldouble_tensor(arr, NULL);
 * if (!c.has_value) { fprintf(stderr, "%s\n", error_to_string(c.u.error)); }
 * ldouble_tensor_t* copy = c.u.value;
 * // ...
 * return_ldouble_tensor(copy);
 *
 * // Copy using a different allocator
 * allocator_vtable_t arena = arena_allocator(&my_arena);
 * ldouble_tensor_expect_t c2 = copy_ldouble_tensor(arr, &arena);
 * @endcode
 */
ldouble_tensor_expect_t copy_ldouble_tensor(const ldouble_tensor_t* src,
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
 * size_t n = ldouble_tensor_size(arr);   // e.g. 3 after three push_back calls
 * @endcode
 */
static inline size_t ldouble_tensor_size(const ldouble_tensor_t* t) {
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
 * size_t cap = ldouble_tensor_alloc(arr);   // e.g. 16 after init_ldouble_array(16,...)
 * @endcode
 */
static inline size_t ldouble_tensor_alloc(const ldouble_tensor_t* t) {
    if (t == NULL) return 0u;
    return tensor_alloc(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the size of one element in bytes.
 *
 * Always returns sizeof(ldouble) == 1 for a correctly initialised
 * ldouble_tensor_t. Returns 0 if t is NULL.
 *
 * @code{.c}
 * size_t esz = ldouble_tensor_data_size(arr);   // 1
 * @endcode
 */
static inline size_t ldouble_tensor_data_size(const ldouble_tensor_t* t) {
    if (t == NULL) return 0u;
    return tensor_data_size(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the dtype identifier of the array.
 *
 * Always returns LDOUBLE_TYPE for a correctly initialised ldouble_tensor_t.
 * Returns UNKNOWN_TYPE if t is NULL.
 *
 * @code{.c}
 * dtype_id_t id = ldouble_tensor_dtype(arr);   // LDOUBLE_TYPE
 * @endcode
 */
static inline dtype_id_t ldouble_tensor_dtype(const ldouble_tensor_t* t) {
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
 * ldouble_tensor_expect_t r = init_ldouble_array(8, false, heap_allocator());
 * ldouble_tensor_t* arr = r.u.value;
 * bool empty = is_ldouble_tensor_empty(arr);   // true before any push
 * push_back_ldouble_tensor(arr, 1);
 * empty = is_ldouble_tensor_empty(arr);         // false after push
 * return_ldouble_tensor(arr);
 * @endcode
 */
static inline bool is_ldouble_tensor_empty(const ldouble_tensor_t* t) {
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
 * ldouble_tensor_expect_t r = init_ldouble_array(2, false, heap_allocator());
 * ldouble_tensor_t* arr = r.u.value;
 * push_back_ldouble_tensor(arr, 1.0f);
 * push_back_ldouble_tensor(arr, 2.0f);
 * bool full = is_ldouble_tensor_full(arr);   // true — len == alloc == 2
 * return_ldouble_tensor(arr);
 * @endcode
 */
static inline bool is_ldouble_tensor_full(const ldouble_tensor_t* t) {
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
 * ldouble_tensor_expect_t r = init_ldouble_array(4, false, heap_allocator());
 * ldouble_tensor_t* arr = r.u.value;
 * push_back_ldouble_tensor(arr, 10.0f);
 * const ldouble* p = arr->base->data;
 * bool valid = is_ldouble_tensor_ptr(arr, p);   // true — points to element 0
 * return_ldouble_tensor(arr);
 * @endcode
 */
static inline bool is_ldouble_tensor_ptr(const ldouble_tensor_t* t, const long double* ptr) {
    if (t == NULL || ptr == NULL) return false;
    return is_tensor_ptr(t->base, ptr);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the number of dimensions of the underlying tensor.
 *
 * Always returns 1 for a tensor initialised via init_ldouble_array.
 * Returns 0 if t is NULL.
 *
 * @code{.c}
 * uint8_t ndim = ldouble_tensor_ndim(arr);   // 1
 * @endcode
 */
static inline uint8_t ldouble_tensor_ndim(const ldouble_tensor_t* t) {
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
 * ldouble_tensor_expect_t r = init_ldouble_array(8, false, heap_allocator());
 * ldouble_tensor_t* arr = r.u.value;
 * size_t d0 = ldouble_tensor_shape_dim(arr, 0u);   // 8
 * return_ldouble_tensor(arr);
 * @endcode
 */
static inline size_t ldouble_tensor_shape_dim(const ldouble_tensor_t* t, long double dim) {
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
 * error_code_t err = ldouble_tensor_shape(arr, shape, 1u);
 * // shape[0] now holds the single dimension size
 * @endcode
 */
static inline error_code_t ldouble_tensor_shape(const ldouble_tensor_t* t, size_t* out, long double count) {
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
 * const size_t* sp = ldouble_tensor_shape_ptr(arr);
 * if (sp) printf("dim0 = %zu\n", sp[0]);
 * @endcode
 */
static inline const size_t* ldouble_tensor_shape_ptr(const ldouble_tensor_t* t) {
    if (t == NULL) return NULL;
    return tensor_shape_ptr(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a read-only pointer to the internal strides array.
 *
 * Strides are in bytes. For a contiguous 1-D long double array,
 * strides[0] == 1. The pointer is valid for the lifetime of the tensor
 * and must not be modified or freed. Returns NULL if t is NULL.
 *
 * @code{.c}
 * const size_t* strides = ldouble_tensor_strides_ptr(arr);
 * if (strides) printf("stride0 = %zu\n", strides[0]);   // 1
 * @endcode
 */
static inline const size_t* ldouble_tensor_strides_ptr(const ldouble_tensor_t* t) {
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
 * error_code_t err = ldouble_tensor_shape_str(arr, buf, sizeof(buf));
 * if (err == NO_ERROR) printf("shape = %s\n", buf);   // e.g. "(8)"
 * @endcode
 */
static inline error_code_t ldouble_tensor_shape_str(const ldouble_tensor_t* t, 
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
 * push_back_ldouble_tensor(arr, 1.0f);
 * push_back_ldouble_tensor(arr, 2.0f);
 * clear_ldouble_tensor(arr);
 * size_t n = ldouble_tensor_size(arr);   // 0
 * @endcode
 */
static inline error_code_t clear_ldouble_tensor(ldouble_tensor_t* t) {
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
 * ldouble_tensor_expect_t r1 = init_ldouble_array(8, false, heap_allocator());
 * ldouble_tensor_expect_t r2 = init_ldouble_array(4, false, heap_allocator());
 * ldouble_tensor_t* dst = r1.u.value;
 * ldouble_tensor_t* src = r2.u.value;
 * push_back_ldouble_tensor(dst, 1.0f);
 * push_back_ldouble_tensor(src, 2.0f);
 * push_back_ldouble_tensor(src, 3.0f);
 * concat_ldouble_tensor_array(dst, src);   // dst = [1, 2, 3]
 * return_ldouble_tensor(dst);
 * return_ldouble_tensor(src);
 * @endcode
 */
static inline error_code_t concat_ldouble_tensor_array(ldouble_tensor_t* dst,
                                                       const ldouble_tensor_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_tensor_array(dst->base, src->base, LDOUBLE_TYPE);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a new array containing a copy of the elements in src at
 *        positions [start, end).
 *
 * The range is half-open: start is inclusive, end is exclusive. Both must
 * be <= src->len and start must be strictly less than end. The returned
 * array is independent of src — mutations to one do not affect the other.
 * The caller is responsible for calling return_ldouble_tensor on the result.
 *
 * Pass NULL for alloc_v to inherit the allocator from src.
 *
 * @param src     Pointer to the source array. Must not be NULL.
 * @param start   Zero-based index of the first element to include.
 * @param end     Zero-based index one past the last element to include.
 * @param alloc_v Pointer to the allocator vtable. NULL inherits from src.
 *
 * @return ldouble_tensor_expect_t with has_value true on success.
 *
 * @code{.c}
 * // arr = [10, 20, 30, 40, 50]
 * ldouble_tensor_expect_t s = slice_ldouble_tensor_array(arr, 1u, 4u, NULL);
 * if (!s.has_value) { fprintf(stderr, "%s\n", error_to_string(s.u.error)); }
 * ldouble_tensor_t* sl = s.u.value;   // sl = [20, 30, 40]
 * return_ldouble_tensor(sl);
 * @endcode
 */
ldouble_tensor_expect_t slice_ldouble_tensor_array(const ldouble_tensor_t* src,
                                                   size_t start,
                                                   size_t end,
                                                   allocator_vtable_t* alloc_v);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reverse the populated elements of a ldouble_t tensor in place.
 *
 * Reverses the order of all elements in the live region of the tensor's
 * data buffer using a SIMD-accelerated byte-swap routine. The operation
 * is mode-agnostic — for ARRAY_STRUCT tensors it reverses the len
 * populated elements, and for TENSOR_STRUCT tensors it reverses all alloc
 * elements in flat row-major order.
 *
 * The tensor is modified in place. No allocation is performed and the
 * allocator is not consulted.
 *
 * Returns EMPTY rather than NO_ERROR when len < 2 because a zero or
 * single-element sequence has no meaningful reversal. The tensor is
 * left untouched in this case.
 *
 * @param t  Pointer to the target tensor. Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if t is NULL
 *         - EMPTY        if t->base->len < 2
 *
 * @code
 * // arr = [1, 2, 3, 4, 5]
 * error_code_t err = reverse_ldouble_tensor(arr);
 * // arr = [5, 4, 3, 2, 1]
 * @endcode
 */
static inline error_code_t reverse_ldouble_tensor(ldouble_tensor_t* t) {
    if (t == NULL) return NULL_POINTER;
    return reverse_tensor(t->base);
}
// -------------------------------------------------------------------------------- 

static int ldouble_cmp(const void* a, const void* b) {
    long double ua = *(const long double*)a;
    long double ub = *(const long double*)b;
    return (ua > ub) - (ua < ub);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Sort the populated elements of a long double tensor in place.
 *
 * Sorts the len elements in the tensor's data buffer using an iterative
 * quicksort with median-of-three pivot selection and an insertion sort
 * fallback for partitions of fewer than 10 elements. The ldouble
 * comparator is supplied internally so the caller only needs to specify
 * the sort direction.
 *
 * The sort operates on the flat element sequence in memory — for an
 * ARRAY_STRUCT tensor this covers only the len populated elements, and
 * for a TENSOR_STRUCT tensor this covers all alloc elements in flat
 * row-major order.
 *
 * @param t    Pointer to the target tensor. Must not be NULL.
 * @param dir  FORWARD for ascending order, REVERSE for descending order.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if t is NULL
 *         - EMPTY        if t->base->len < 2
 *
 * @code
 * // arr = [3, 1, 4, 1, 5, 9, 2, 6]
 * error_code_t err = sort_ldouble_tensor(arr, FORWARD);
 * // arr = [1, 1, 2, 3, 4, 5, 6, 9]
 *
 * err = sort_ldouble_tensor(arr, REVERSE);
 * // arr = [9, 6, 5, 4, 3, 2, 1, 1]
 * @endcode
 */
static inline error_code_t sort_ldouble_tensor(ldouble_tensor_t* t,
                                               direction_t dir) {
    if (t == NULL) return NULL_POINTER;
    return sort_tensor(t->base, ldouble_cmp, dir);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Search a long double tensor for the first element within
 *        tolerance of a target value.
 *
 * Performs a scalar linear scan of the populated elements in the
 * tensor's data buffer from index 0 to len - 1.  An element matches
 * when fabsl(data[i] - value) <= tolerance.  On the first match,
 * writes the zero-based index of the matching element into *index and
 * returns NO_ERROR.  If no match is found the function returns
 * NOT_FOUND and *index is left unchanged.
 *
 * No SIMD acceleration is available for long double — the size of
 * long double is platform-dependent (80-bit extended precision on
 * x86, 128-bit on AArch64, or 64-bit on MSVC) and no mainstream
 * SIMD ISA provides a corresponding lane type.  The scalar
 * implementation is the reference and the only path for this type.
 *
 * NaN behaviour: fabsl(NaN - value) is NaN, and NaN <= tolerance is
 * false for any finite tolerance, so NaN elements are never matched.
 * Passing tolerance < 0.0L produces no matches since no absolute
 * difference can be less than or equal to a negative number.
 *
 * The search covers only the populated region [0, len) — for
 * ARRAY_STRUCT tensors this is the number of elements pushed so far,
 * and for TENSOR_STRUCT tensors this is alloc (all slots are always
 * live).  When multiple elements are within tolerance the index of
 * the first occurrence is returned.
 *
 * @param t          Pointer to the source tensor. Must not be NULL.
 * @param index      Pointer to a size_t that receives the index of the
 *                   first matching element on success. Must not be NULL.
 *                   Unchanged if no element is within tolerance.
 * @param value      The long double value to search for.
 * @param tolerance  Maximum allowed absolute difference for a match.
 *                   Pass 0.0L for exact equality on non-NaN values.
 *                   Must be >= 0.0L for meaningful results.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if t or index is NULL
 *         - EMPTY        if t->base->len == 0
 *         - NOT_FOUND    if no element satisfies
 *                        fabsl(data[i] - value) <= tolerance
 *
 * @code{.c}
 * ldouble_tensor_expect_t r = init_ldouble_array(8, false, heap_allocator());
 * ldouble_tensor_t* arr = r.u.value;
 *
 * push_back_ldouble_array(arr, 1.0L);
 * push_back_ldouble_array(arr, 2.0L);
 * push_back_ldouble_array(arr, 3.0L);
 *
 * size_t idx = 0u;
 *
 * // Exact match
 * error_code_t err = ldouble_tensor_lsearch(arr, &idx, 2.0L, 0.0L);
 * // err == NO_ERROR, idx == 1
 *
 * // Tolerance match
 * err = ldouble_tensor_lsearch(arr, &idx, 1.95L, 0.1L);
 * // err == NO_ERROR, idx == 1  (|2.0 - 1.95| == 0.05 <= 0.1)
 *
 * // No match
 * err = ldouble_tensor_lsearch(arr, &idx, 9.9L, 0.01L);
 * // err == NOT_FOUND, idx unchanged
 *
 * return_ldouble_tensor(arr);
 * @endcode
 */
error_code_t ldouble_tensor_lsearch(const ldouble_tensor_t* t,
                                    size_t*                 index,
                                    long double             value,
                                    long double             tolerance);
// -------------------------------------------------------------------------------- 

/**
 * @brief Binary search a sorted long double tensor for an element within
 *        tolerance of a target value.
 *
 * Performs an iterative binary search over the populated elements of
 * the tensor's data buffer to locate the region where elements within
 * tolerance of value would reside, then performs a short linear scan
 * within that region to find the first matching element.
 *
 * The tensor must be sorted in ascending order before calling this
 * function — the result is undefined if the data is unsorted.  Use
 * sort_ldouble_tensor before calling if the sort order is not guaranteed.
 *
 * An element matches when absolute value (data[i] - value) <= tolerance.  The
 * binary search locates the insertion point of (value - tolerance) in
 * O(log n) and then scans forward from that point until an element
 * within tolerance is found or elements exceed (value + tolerance).
 * This scan is O(k) where k is the number of elements within tolerance,
 * which is typically very small.
 *
 * NaN behaviour: if value or any element is NaN the comparison is
 * unordered and that element is never matched.  Passing tolerance < 0.0f
 * produces no matches since no absolute difference can satisfy
 * fabsf(diff) <= negative_number.
 *
 * When duplicates are within tolerance the index of the first such
 * element in the sorted sequence is returned.
 *
 * @param t          Pointer to the source tensor. Must not be NULL.
 *                   Must be sorted in ascending order.
 * @param index      Pointer to a size_t that receives the zero-based
 *                   index of the first matching element on success.
 *                   Must not be NULL. Unchanged if no match is found.
 * @param value      The ldouble value to search for.
 * @param tolerance  Maximum allowed absolute difference for a match.
 *                   Pass 0.0f for exact equality on non-NaN values.
 *                   Must be >= 0.0f for meaningful results.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if t or index is NULL
 *         - EMPTY        if t->base->len == 0
 *         - NOT_FOUND    if no element satisfies
 *                        fabsf(data[i] - value) <= tolerance
 *
 * @code{.c}
 * ldouble_tensor_expect_t r = init_ldouble_array(8, false, heap_allocator());
 * ldouble_tensor_t* arr = r.u.value;
 *
 * push_back_ldouble_array(arr, 1.0L);
 * push_back_ldouble_array(arr, 2.0L);
 * push_back_ldouble_array(arr, 3.0L);
 * push_back_ldouble_array(arr, 4.0L);
 *
 * sort_ldouble_tensor(arr, FORWARD);
 * // arr = [1.0, 2.0, 3.0, 4.0]
 *
 * size_t idx = 0u;
 *
 * // Exact match
 * error_code_t err = ldouble_tensor_bsearch(arr, &idx, 3.0L, 0.0L);
 * // err == NO_ERROR, idx == 2
 *
 * // Tolerance match
 * err = ldouble_tensor_bsearch(arr, &idx, 2.05L, 0.1L);
 * // err == NO_ERROR, idx == 1  (|2.0 - 2.05| == 0.05 <= 0.1)
 *
 * // No match
 * err = ldouble_tensor_bsearch(arr, &idx, 2.5L, 0.1L);
 * // err == NOT_FOUND, idx unchanged
 *
 * return_ldouble_tensor(arr);
 * @endcode
 */
error_code_t ldouble_tensor_bsearch(const ldouble_tensor_t* t,
                                    size_t*               index,
                                    long double           value,
                                    long double           tolerance);
// -------------------------------------------------------------------------------- 

/**
 * @brief Bracketed binary search of a sorted long double tensor with tolerance.
 *
 * Locates the pair of adjacent indices that bracket value in a sorted
 * ascending tensor.  An element is considered an exact match when
 * fabsl(data[i] - value) <= tolerance.  When an exact match is found
 * lower == upper == index of the first matching element in sorted order.
 * When no exact match exists but value lies within the range of the
 * array, lower and upper are the indices of the largest element below
 * value and the smallest element above value respectively.
 *
 * The tensor must be sorted in ascending order before calling this
 * function — the result is undefined if the data is unsorted.
 *
 * Since no SIMD acceleration is available for long double the search
 * is performed entirely in scalar code.  The tolerance widens the
 * out-of-range checks in the same way as the float and double versions:
 * value is considered below range only when value < data[0] - tolerance,
 * and above range only when value > data[len-1] + tolerance.
 *
 * When multiple elements are within tolerance the left-scan ensures
 * lower == upper always points to the first (lowest index) match in
 * the sorted sequence.
 *
 * On x86 platforms long double is 80-bit extended precision, giving
 * approximately 18-19 significant decimal digits.  The tolerance
 * comparison is performed at full long double precision throughout —
 * no narrowing to double occurs at any point.
 *
 * NaN behaviour: a NaN value is rejected immediately with DOMAIN_ERROR
 * before any comparison is performed, since NaN comparisons are
 * unordered under IEEE 754 and would produce undefined bracket indices
 * if allowed through.
 *
 * @param t          Pointer to the source tensor. Must not be NULL.
 *                   Must be sorted in ascending order.
 * @param value      The long double value to bracket.
 * @param tolerance  Maximum absolute difference for an exact match.
 *                   Pass 0.0L for exact bit-pattern equality on non-NaN
 *                   values.  Negative tolerance produces no exact matches
 *                   and falls through to the bracket path.
 *
 * @return bracket_expect_t:
 *         - has_value true  → exact match or bracket found, u.value
 *                             holds lower and upper indices.
 *                             lower == upper on an exact match.
 *         - has_value false → u.error is one of:
 *             NULL_POINTER  if t is NULL
 *             EMPTY         if t->base->len == 0
 *             DOMAIN_ERROR  if value is NaN
 *             BELOW_RANGE   if value < data[0] - tolerance
 *             ABOVE_RANGE   if value > data[len-1] + tolerance
 *
 * @code{.c}
 * // arr = [-3.0L, -1.0L, 0.0L, 1.0L, 3.0L]
 * bracket_expect_t r;
 *
 * // Exact match within tolerance
 * r = ldouble_tensor_bbsearch(arr, 0.05L, 0.1L);
 * // r.has_value == true, r.u.value.lower == 2, r.u.value.upper == 2
 *
 * // Bracketed
 * r = ldouble_tensor_bbsearch(arr, 0.5L, 0.1L);
 * // r.has_value == true, r.u.value.lower == 2, r.u.value.upper == 3
 * // data[2]==0.0L <= 0.5L <= data[3]==1.0L
 *
 * // Above range
 * r = ldouble_tensor_bbsearch(arr, 4.0L, 0.1L);
 * // r.has_value == false, r.u.error == ABOVE_RANGE
 *
 * // NaN rejected
 * r = ldouble_tensor_bbsearch(arr, (long double)NAN, 0.1L);
 * // r.has_value == false, r.u.error == DOMAIN_ERROR
 *
 * return_ldouble_tensor(arr);
 * @endcode
 */
bracket_expect_t ldouble_tensor_bbsearch(const ldouble_tensor_t* t,
                                         long double             value,
                                         long double             tolerance);
// -------------------------------------------------------------------------------- 

/**
 * @brief Compare two long double tensors for equality within a tolerance.
 *
 * Compares the populated data buffers of two long double tensors element
 * by element using a scalar absolute-difference check.  Element i is
 * considered equal when fabsl(one[i] - two[i]) <= tolerance.  All
 * elements must satisfy this condition for the function to return true.
 *
 * The comparison is always performed on the live region [0, len) — for
 * ARRAY_STRUCT tensors this is the number of populated elements and for
 * TENSOR_STRUCT tensors this is alloc (all slots are always live).
 *
 * No SIMD acceleration is available for long double — the size of long
 * double is platform-dependent (80-bit extended precision on x86,
 * 128-bit on AArch64, or 64-bit on MSVC) and no mainstream SIMD ISA
 * provides a corresponding lane type.  The comparison is performed
 * entirely in scalar code at full long double precision throughout —
 * no narrowing to double occurs at any point.
 *
 * The meta flag controls whether structural metadata is included in
 * the comparison in addition to the element-wise tolerance check:
 *
 *   meta == false:
 *     Only ndim and the data buffer contents are compared within
 *     tolerance.  Two tensors are considered equal if they have the
 *     same number of dimensions and every element satisfies the
 *     tolerance condition.
 *
 *   meta == true:
 *     In addition to the checks above, shape, alloc, mode, and growth
 *     must all match exactly.
 *
 * The allocator vtable is never compared.
 *
 * NaN behaviour: if any element in either tensor is NaN then
 * one[i] - two[i] is NaN, and the portable NaN test diff != diff
 * fires, returning false immediately regardless of tolerance.  Passing
 * tolerance < 0.0L produces no matches since no absolute difference
 * satisfies fabsl(diff) <= negative_number.
 *
 * @param one        Pointer to the first tensor.  Must not be NULL.
 * @param two        Pointer to the second tensor.  Must not be NULL.
 * @param tolerance  Maximum allowed absolute difference per element.
 *                   Pass 0.0L for exact bit-pattern equality on non-NaN
 *                   values.
 * @param meta       If true, include shape, alloc, mode, and growth in
 *                   the comparison.  If false, compare only ndim and
 *                   element values within tolerance.
 *
 * @return true  if all elements are within tolerance and all checked
 *               metadata matches, or if one and two are the same pointer.
 * @return false if either pointer is NULL, either base data pointer is
 *               NULL, lengths differ, ndim differs, any element exceeds
 *               tolerance, or any checked metadata field differs.
 *
 * @code{.c}
 * ldouble_tensor_t* a = init_ldouble_array(4, false, heap_allocator()).u.value;
 * ldouble_tensor_t* b = init_ldouble_array(4, false, heap_allocator()).u.value;
 *
 * push_back_ldouble_array(a, 1.0L);
 * push_back_ldouble_array(a, 2.0L);
 * push_back_ldouble_array(b, 1.001L);
 * push_back_ldouble_array(b, 2.001L);
 *
 * // Within tolerance
 * bool eq = ldouble_tensors_equal(a, b, 0.01L, false);   // true
 *
 * // Outside tolerance
 * eq = ldouble_tensors_equal(a, b, 0.0001L, false);       // false
 *
 * return_ldouble_tensor(a);
 * return_ldouble_tensor(b);
 * @endcode
 */
bool ldouble_tensors_equal(const ldouble_tensor_t* one,
                           const ldouble_tensor_t* two,
                           long double             tolerance,
                           bool                    meta);
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
 * push_back_ldouble_tensor(arr, 0.0f);   // arr = [0]
 * set_ldouble_tensor_index(arr, 0u, 99.0f);   // arr = [99]
 * @endcode
 */
static inline error_code_t set_ldouble_tensor_index(ldouble_tensor_t* t,
                                                    size_t index,
                                                    long double data) {
    if (t == NULL) return NULL_POINTER;
    return set_tensor_index(t->base, index, &data, LDOUBLE_TYPE);
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
 * // For a 3x3 long double matrix initialised via init_tensor
 * size_t idx[] = { 1u, 2u };
 * set_ldouble_tensor_nd_index(mat, idx, 42.0f);   // mat[1][2] = 42
 * @endcode
 */
static inline error_code_t set_ldouble_tensor_nd_index(ldouble_tensor_t* t,
                                                       const size_t* idx,
                                                       long double data) {
    if (t == NULL) return NULL_POINTER;
    return set_tensor_nd_index(t->base, idx, &data, LDOUBLE_TYPE);
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
 * push_back_ldouble_tensor(arr, 10.0f);
 * push_back_ldouble_tensor(arr, 20.0f);
 * // arr = [10, 20], len == 2
 * @endcode
 */
static inline error_code_t push_back_ldouble_array(ldouble_tensor_t* t,
                                                   long double data) {
    if (t == NULL) return NULL_POINTER;
    return push_back_tensor(t->base, &data, LDOUBLE_TYPE);
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
 * push_back_ldouble_tensor(arr, 10.0f);
 * push_front_ldouble_tensor(arr, 99.0f);
 * // arr = [99, 10]
 * @endcode
 */
static inline error_code_t push_front_ldouble_array(ldouble_tensor_t* t,
                                                    long double data) {
    if (t == NULL) return NULL_POINTER;
    return push_front_tensor(t->base, &data, LDOUBLE_TYPE);
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
 * push_at_ldouble_tensor(arr, 99.0f, 1);
 * // arr = [10, 99, 20, 30]
 * @endcode
 */
static inline error_code_t push_at_ldouble_array(ldouble_tensor_t* t,
                                                 long double data,
                                                 size_t index) {
    if (t == NULL) return NULL_POINTER;
    return push_at_tensor(t->base, &data, index, LDOUBLE_TYPE);
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
 * long double val;
 * pop_back_ldouble_tensor(arr, &val);   // val == 30, arr = [10, 20]
 * pop_back_ldouble_tensor(arr, NULL);   // discard, arr = [10]
 * @endcode
 */
static inline error_code_t pop_back_ldouble_array(ldouble_tensor_t* t,
                                                  long double* out) {
    if (t == NULL) return NULL_POINTER;
    return pop_back_tensor(t->base, out, LDOUBLE_TYPE);
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
 * long double val;
 * pop_front_ldouble_tensor(arr, &val);   // val == 10, arr = [20, 30]
 * @endcode
 */
static inline error_code_t pop_front_ldouble_array(ldouble_tensor_t* t,
                                                   long double* out) {
    if (t == NULL) return NULL_POINTER;
    return pop_front_tensor(t->base, out, LDOUBLE_TYPE);
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
 * long double val;
 * pop_at_ldouble_tensor(arr, &val, 2u);   // val == 30, arr = [10, 20, 40]
 * @endcode
 */
static inline error_code_t pop_at_ldouble_array(ldouble_tensor_t* t,
                                                long double* out,
                                                size_t index) {
    if (t == NULL) return NULL_POINTER;
    return pop_at_tensor(t->base, out, index, LDOUBLE_TYPE);
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
 * long double val;
 * get_ldouble_tensor_index(arr, 1u, &val);   // val == 20
 * @endcode
 */
static inline error_code_t get_ldouble_tensor_index(const ldouble_tensor_t* t,
                                                    size_t index,
                                                    long double* out) {
    if (t == NULL || out == NULL) return NULL_POINTER;
    return get_tensor_index(t->base, index, out, LDOUBLE_TYPE);
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
 * // For a 3x3 long double matrix
 * size_t idx[] = { 1u, 2u };
 * long double val;
 * get_ldouble_tensor_nd_index(mat, idx, &val);   // val = mat[1][2]
 * @endcode
 */
static inline error_code_t get_ldouble_tensor_nd_index(const ldouble_tensor_t* t,
                                                       const size_t* idx,
                                                       long double* out) {
    if (t == NULL || idx == NULL || out == NULL) return NULL_POINTER;
    return get_tensor_nd_index(t->base, idx, out, LDOUBLE_TYPE);
}
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_ldouble_H */
// ================================================================================
// ================================================================================
// eof
