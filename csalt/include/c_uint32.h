// ================================================================================
// ================================================================================
// - File:    c_uint32.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 27, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_uint32_H
#define c_uint32_H

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
 * @brief Type-safe wrapper for a tensor containing uint32_t elements.
 *
 * Holds a pointer to the underlying generic tensor rather than embedding
 * it by value, which is required because tensor_t contains a flexible
 * array member and cannot legally appear as a struct member in C99/C11.
 * All generic tensor operations are reached through the base pointer.
 * Use the typed wrapper functions (init_uint32_array, push_back_uint32_tensor,
 * etc.) rather than accessing base directly.
 */
typedef struct {
    tensor_t* base; /**< Underlying generic tensor. Do not access directly. */
} uint32_tensor_t;

// --------------------------------------------------------------------------------

/**
 * @brief Expected-value return type for functions that produce a
 *        uint32_tensor_t.
 *
 * has_value must be checked before accessing either union member.
 *
 * @code{.c}
 * uint32_tensor_expect_t r = init_uint32_array(8, true, heap_allocator());
 * if (!r.has_value) { handle(r.u.error); }
 * uint32_tensor_t* a = r.u.value;
 * @endcode
 */
typedef struct {
    bool has_value;
    union {
        uint32_tensor_t* value;
        error_code_t    error;
    } u;
} uint32_tensor_expect_t;

// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialise a fixed-shape N-dimensional tensor of uint32_t elements.
 *
 * Allocates the underlying tensor_t via init_tensor and then allocates a
 * thin uint32_tensor_t wrapper struct through the same allocator. All slots
 * are zero-initialised at construction and considered live for the lifetime
 * of the tensor — len equals alloc equals the product of all shape
 * dimensions. Both allocations must be returned by calling
 * return_uint32_tensor when the tensor is no longer needed.
 *
 * @param ndim    Number of dimensions. Must be > 0 and <= 255.
 * @param shape   Array of ndim dimension sizes. Must not be NULL.
 *                Each shape[i] must be > 0.
 * @param alloc_v Allocator vtable for all memory operations.
 *                alloc_v.allocate must not be NULL.
 *
 * @return uint32_tensor_expect_t with has_value true on success.
 *         On failure has_value is false and u.error is set.
 *
 * @code{.c}
 * // Create a 3x4 matrix of uint32_t values
 * size_t shape[] = { 3u, 4u };
 * uint32_tensor_expect_t r = init_uint32_tensor(2u, shape, heap_allocator());
 * if (!r.has_value) { fprintf(stderr, "%s\n", error_to_string(r.u.error)); }
 * uint32_tensor_t* mat = r.u.value;
 *
 * // Write to element (1, 2)
 * size_t idx[] = { 1u, 2u };
 * set_uint32_tensor_nd_index(mat, idx, 42u);
 *
 * // Read it back
 * uint32_t val;
 * get_uint32_tensor_nd_index(mat, idx, &val);   // val == 42
 *
 * return_uint32_tensor(mat);
 * @endcode
 */
uint32_tensor_expect_t init_uint32_tensor(uint8_t            ndim,
                                          const size_t*      shape,
                                          allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/*
 * Allocator convention for uint32_tensor_t functions:
 *
 * init_uint32_array        — takes allocator_vtable_t by value. An allocator
 *                           is mandatory since there is no existing tensor to
 *                           inherit one from.
 *
 * copy_uint32_tensor       — takes allocator_vtable_t* (pointer). Pass NULL to
 * slice_uint32_tensor_array  inherit the allocator from the source tensor, or
 *                           a non-NULL pointer to use a different allocator.
 */
 
/**
 * @brief Initialise a dynamic 1-D array of uint32_t elements.
 *
 * Allocates the underlying tensor_t via init_tensor_array and then
 * allocates a thin uint32_tensor_t wrapper struct through the same
 * allocator. Both allocations must be returned by calling
 * return_uint32_tensor when the array is no longer needed.
 *
 * @param capacity  Number of elements to allocate. Must be > 0.
 * @param growth    If true, push operations may reallocate the buffer
 *                  when capacity is reached.
 * @param alloc_v   Allocator vtable for all memory operations.
 *                  alloc_v.allocate must not be NULL.
 *
 * @return uint32_tensor_expect_t with has_value true on success.
 *         On failure has_value is false and u.error is set.
 *
 * @code{.c}
 * uint32_tensor_expect_t r = init_uint32_array(16, true, heap_allocator());
 * if (!r.has_value) { fprintf(stderr, "%s\n", error_to_string(r.u.error)); }
 * uint32_tensor_t* arr = r.u.value;
 * push_back_uint32_tensor(arr, 42u);
 * return_uint32_tensor(arr);
 * @endcode
 */
uint32_tensor_expect_t init_uint32_array(size_t capacity,
                                       bool growth,
                                       allocator_vtable_t alloc_v);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a uint32_tensor_t and its underlying tensor to the allocator.
 *
 * Returns the data buffer and tensor_t struct via return_tensor, then
 * returns the wrapper struct itself. The allocator is read from the tensor
 * before return_tensor is called so it remains valid for the wrapper
 * deallocation. Silently ignored if t is NULL.
 *
 * @param t  Pointer to the wrapper to return. May be NULL.
 *
 * @code{.c}
 * uint32_tensor_expect_t r = init_uint32_array(8, false, heap_allocator());
 * uint32_tensor_t* arr = r.u.value;
 * // ... use arr ...
 * return_uint32_tensor(arr);
 * arr = NULL;   // guard against accidental reuse
 * @endcode
 */
void return_uint32_tensor(uint32_tensor_t* t);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a deep copy of a uint32_tensor_t.
 *
 * Copies the underlying tensor via copy_tensor then allocates a new
 * uint32_tensor_t wrapper struct. Both allocations are returned by calling
 * return_uint32_tensor on the returned pointer. On partial failure the
 * copied tensor_t is returned before propagating the error so no memory
 * is leaked. The copy is independent of src — mutations to one do not
 * affect the other.
 *
 * @param src     Pointer to the source uint32_tensor_t. Must not be NULL.
 * @param alloc_v Pointer to the allocator vtable for the new memory.
 *                Pass NULL to inherit the allocator from src.
 *
 * @return uint32_tensor_expect_t with has_value true on success.
 *         On failure has_value is false and u.error is set.
 *
 * @code{.c}
 * // Copy using src's own allocator
 * uint32_tensor_expect_t c = copy_uint32_tensor(arr, NULL);
 * if (!c.has_value) { fprintf(stderr, "%s\n", error_to_string(c.u.error)); }
 * uint32_tensor_t* copy = c.u.value;
 * // ...
 * return_uint32_tensor(copy);
 *
 * // Copy using a different allocator
 * allocator_vtable_t arena = arena_allocator(&my_arena);
 * uint32_tensor_expect_t c2 = copy_uint32_tensor(arr, &arena);
 * @endcode
 */
uint32_tensor_expect_t copy_uint32_tensor(const uint32_tensor_t* src,
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
 * size_t n = uint32_tensor_size(arr);   // e.g. 3 after three push_back calls
 * @endcode
 */
static inline size_t uint32_tensor_size(const uint32_tensor_t* t) {
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
 * size_t cap = uint32_tensor_alloc(arr);   // e.g. 16 after init_uint32_array(16,...)
 * @endcode
 */
static inline size_t uint32_tensor_alloc(const uint32_tensor_t* t) {
    if (t == NULL) return 0u;
    return tensor_alloc(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the size of one element in bytes.
 *
 * Always returns sizeof(uint32_t) == 1 for a correctly initialised
 * uint32_tensor_t. Returns 0 if t is NULL.
 *
 * @code{.c}
 * size_t esz = uint32_tensor_data_size(arr);   // 1
 * @endcode
 */
static inline size_t uint32_tensor_data_size(const uint32_tensor_t* t) {
    if (t == NULL) return 0u;
    return tensor_data_size(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the dtype identifier of the array.
 *
 * Always returns UINT32_TYPE for a correctly initialised uint32_tensor_t.
 * Returns UNKNOWN_TYPE if t is NULL.
 *
 * @code{.c}
 * dtype_id_t id = uint32_tensor_dtype(arr);   // UINT32_TYPE
 * @endcode
 */
static inline dtype_id_t uint32_tensor_dtype(const uint32_tensor_t* t) {
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
 * uint32_tensor_expect_t r = init_uint32_array(8, false, heap_allocator());
 * uint32_tensor_t* arr = r.u.value;
 * bool empty = is_uint32_tensor_empty(arr);   // true before any push
 * push_back_uint32_tensor(arr, 1u);
 * empty = is_uint32_tensor_empty(arr);         // false after push
 * return_uint32_tensor(arr);
 * @endcode
 */
static inline bool is_uint32_tensor_empty(const uint32_tensor_t* t) {
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
 * uint32_tensor_expect_t r = init_uint32_array(2, false, heap_allocator());
 * uint32_tensor_t* arr = r.u.value;
 * push_back_uint32_tensor(arr, 1u);
 * push_back_uint32_tensor(arr, 2u);
 * bool full = is_uint32_tensor_full(arr);   // true — len == alloc == 2
 * return_uint32_tensor(arr);
 * @endcode
 */
static inline bool is_uint32_tensor_full(const uint32_tensor_t* t) {
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
 * uint32_tensor_expect_t r = init_uint32_array(4, false, heap_allocator());
 * uint32_tensor_t* arr = r.u.value;
 * push_back_uint32_tensor(arr, 10u);
 * const uint32_t* p = arr->base->data;
 * bool valid = is_uint32_tensor_ptr(arr, p);   // true — points to element 0
 * return_uint32_tensor(arr);
 * @endcode
 */
static inline bool is_uint32_tensor_ptr(const uint32_tensor_t* t, const uint32_t* ptr) {
    if (t == NULL || ptr == NULL) return false;
    return is_tensor_ptr(t->base, ptr);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the number of dimensions of the underlying tensor.
 *
 * Always returns 1 for a tensor initialised via init_uint32_array.
 * Returns 0 if t is NULL.
 *
 * @code{.c}
 * uint8_t ndim = uint32_tensor_ndim(arr);   // 1
 * @endcode
 */
static inline uint8_t uint32_tensor_ndim(const uint32_tensor_t* t) {
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
 * uint32_tensor_expect_t r = init_uint32_array(8, false, heap_allocator());
 * uint32_tensor_t* arr = r.u.value;
 * size_t d0 = uint32_tensor_shape_dim(arr, 0u);   // 8
 * return_uint32_tensor(arr);
 * @endcode
 */
static inline size_t uint32_tensor_shape_dim(const uint32_tensor_t* t, uint32_t dim) {
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
 * error_code_t err = uint32_tensor_shape(arr, shape, 1u);
 * // shape[0] now holds the single dimension size
 * @endcode
 */
static inline error_code_t uint32_tensor_shape(const uint32_tensor_t* t, size_t* out, uint32_t count) {
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
 * const size_t* sp = uint32_tensor_shape_ptr(arr);
 * if (sp) printf("dim0 = %zu\n", sp[0]);
 * @endcode
 */
static inline const size_t* uint32_tensor_shape_ptr(const uint32_tensor_t* t) {
    if (t == NULL) return NULL;
    return tensor_shape_ptr(t->base);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a read-only pointer to the internal strides array.
 *
 * Strides are in bytes. For a contiguous 1-D uint32_t array,
 * strides[0] == 1. The pointer is valid for the lifetime of the tensor
 * and must not be modified or freed. Returns NULL if t is NULL.
 *
 * @code{.c}
 * const size_t* strides = uint32_tensor_strides_ptr(arr);
 * if (strides) printf("stride0 = %zu\n", strides[0]);   // 1
 * @endcode
 */
static inline const size_t* uint32_tensor_strides_ptr(const uint32_tensor_t* t) {
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
 * error_code_t err = uint32_tensor_shape_str(arr, buf, sizeof(buf));
 * if (err == NO_ERROR) printf("shape = %s\n", buf);   // e.g. "(8)"
 * @endcode
 */
static inline error_code_t uint32_tensor_shape_str(const uint32_tensor_t* t, 
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
 * push_back_uint32_tensor(arr, 1u);
 * push_back_uint32_tensor(arr, 2u);
 * clear_uint32_tensor(arr);
 * size_t n = uint32_tensor_size(arr);   // 0
 * @endcode
 */
static inline error_code_t clear_uint32_tensor(uint32_tensor_t* t) {
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
 * uint32_tensor_expect_t r1 = init_uint32_array(8, false, heap_allocator());
 * uint32_tensor_expect_t r2 = init_uint32_array(4, false, heap_allocator());
 * uint32_tensor_t* dst = r1.u.value;
 * uint32_tensor_t* src = r2.u.value;
 * push_back_uint32_tensor(dst, 1u);
 * push_back_uint32_tensor(src, 2u);
 * push_back_uint32_tensor(src, 3u);
 * concat_uint32_tensor_array(dst, src);   // dst = [1, 2, 3]
 * return_uint32_tensor(dst);
 * return_uint32_tensor(src);
 * @endcode
 */
static inline error_code_t concat_uint32_tensor_array(uint32_tensor_t* dst,
                                                     const uint32_tensor_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_tensor_array(dst->base, src->base, UINT32_TYPE);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a new array containing a copy of the elements in src at
 *        positions [start, end).
 *
 * The range is half-open: start is inclusive, end is exclusive. Both must
 * be <= src->len and start must be strictly less than end. The returned
 * array is independent of src — mutations to one do not affect the other.
 * The caller is responsible for calling return_uint32_tensor on the result.
 *
 * Pass NULL for alloc_v to inherit the allocator from src.
 *
 * @param src     Pointer to the source array. Must not be NULL.
 * @param start   Zero-based index of the first element to include.
 * @param end     Zero-based index one past the last element to include.
 * @param alloc_v Pointer to the allocator vtable. NULL inherits from src.
 *
 * @return uint32_tensor_expect_t with has_value true on success.
 *
 * @code{.c}
 * // arr = [10, 20, 30, 40, 50]
 * uint32_tensor_expect_t s = slice_uint32_tensor_array(arr, 1u, 4u, NULL);
 * if (!s.has_value) { fprintf(stderr, "%s\n", error_to_string(s.u.error)); }
 * uint32_tensor_t* sl = s.u.value;   // sl = [20, 30, 40]
 * return_uint32_tensor(sl);
 * @endcode
 */
uint32_tensor_expect_t slice_uint32_tensor_array(const uint32_tensor_t* src,
                                               size_t start,
                                               size_t end,
                                               allocator_vtable_t* alloc_v);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reverse the populated elements of a uint32_t tensor in place.
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
 * error_code_t err = reverse_uint32_tensor(arr);
 * // arr = [5, 4, 3, 2, 1]
 * @endcode
 */
static inline error_code_t reverse_uint32_tensor(uint32_tensor_t* t) {
    if (t == NULL) return NULL_POINTER;
    return reverse_tensor(t->base);
}
// -------------------------------------------------------------------------------- 

static int uint32_cmp(const void* a, const void* b) {
    uint32_t ua = *(const uint32_t*)a;
    uint32_t ub = *(const uint32_t*)b;
    return (ua > ub) - (ua < ub);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Sort the populated elements of a uint32_t tensor in place.
 *
 * Sorts the len elements in the tensor's data buffer using an iterative
 * quicksort with median-of-three pivot selection and an insertion sort
 * fallback for partitions of fewer than 10 elements. The uint32_t
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
 * error_code_t err = sort_uint32_tensor(arr, FORWARD);
 * // arr = [1, 1, 2, 3, 4, 5, 6, 9]
 *
 * err = sort_uint32_tensor(arr, REVERSE);
 * // arr = [9, 6, 5, 4, 3, 2, 1, 1]
 * @endcode
 */
static inline error_code_t sort_uint32_tensor(uint32_tensor_t* t,
                                             direction_t dir) {
    if (t == NULL) return NULL_POINTER;
    return sort_tensor(t->base, uint32_cmp, dir);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Search a uint32_t tensor for the first occurrence of a value.
 *
 * Performs a SIMD-accelerated linear scan of the populated elements in
 * the tensor's data buffer from index 0 to len - 1. On the first match,
 * writes the zero-based index of the matching element into *index and
 * returns NO_ERROR. If no match is found the function returns NOT_FOUND
 * and *index is left unchanged.
 *
 * The internal SIMD helper uses SIZE_MAX as a sentinel for not-found.
 * After the search returns a candidate index the function verifies the
 * element at that index against value before reporting success, guarding
 * against any platform where SIZE_MAX could theoretically collide with a
 * valid result and ensuring correctness is not dependent on the sentinel
 * convention of the underlying helper.
 *
 * The search covers only the populated region [0, len) — for
 * ARRAY_STRUCT tensors this is the number of elements pushed so far,
 * and for TENSOR_STRUCT tensors this is alloc (all slots are always
 * live). When duplicates are present the index of the first occurrence
 * is returned.
 *
 * @param t      Pointer to the source tensor. Must not be NULL.
 * @param index  Pointer to a size_t that receives the index of the
 *               first matching element on success. Must not be NULL.
 *               Unchanged if the value is not found.
 * @param value  The uint32_t value to search for.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if t or index is NULL
 *         - EMPTY        if t->base->len == 0
 *         - NOT_FOUND    if value is not present in the tensor
 *
 * @code{.c}
 * uint32_tensor_expect_t r = init_uint32_array(8, false, heap_allocator());
 * uint32_tensor_t* arr = r.u.value;
 *
 * push_back_uint32_array(arr, 100u);
 * push_back_uint32_array(arr, 200u);
 * push_back_uint32_array(arr, 300u);
 * push_back_uint32_array(arr, 200u);
 * // arr = [100, 200, 300, 200]
 *
 * size_t idx = 0u;
 * error_code_t err = find_uint32_tensor_value(arr, &idx, 200u);
 * // err == NO_ERROR, idx == 1  (first occurrence)
 *
 * err = find_uint32_tensor_value(arr, &idx, 999u);
 * // err == NOT_FOUND, idx unchanged
 *
 * return_uint32_tensor(arr);
 * @endcode
 */
error_code_t uint32_tensor_lsearch(const uint32_tensor_t* t,
                                   size_t*                index,
                                   uint32_t               value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Binary search a sorted uint32_t tensor for a target value.
 *
 * Performs an iterative binary search over the populated elements of
 * the tensor's data buffer.  The tensor must be sorted in ascending
 * order before calling this function — the result is undefined if the
 * data is unsorted.  Use sort_uint32_tensor before calling if the
 * sort order is not guaranteed.
 *
 * When duplicates are present binary search does not guarantee which
 * occurrence is returned — only that the returned index satisfies
 * data[*index] == value.  Use uint32_tensor_lsearch if the first
 * occurrence is required.
 *
 * An underflow guard prevents size_t wraparound when the target is
 * smaller than every element: if the midpoint reaches index 0 and the
 * element there does not match, the loop exits cleanly rather than
 * decrementing high below zero.
 *
 * @param t      Pointer to the source tensor. Must not be NULL.
 *               The tensor must be sorted in ascending order.
 * @param index  Pointer to a size_t that receives the zero-based index
 *               of the matching element on success. Must not be NULL.
 *               Unchanged if the value is not found.
 * @param value  The uint32_t value to search for.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if t or index is NULL
 *         - EMPTY        if t->base->len == 0
 *         - NOT_FOUND    if value is not present in the tensor
 *
 * @code{.c}
 * uint32_tensor_expect_t r = init_uint32_array(8, false, heap_allocator());
 * uint32_tensor_t* arr = r.u.value;
 *
 * push_back_uint32_array(arr, 10u);
 * push_back_uint32_array(arr, 40u);
 * push_back_uint32_array(arr, 20u);
 * push_back_uint32_array(arr, 30u);
 *
 * // Sort before searching
 * sort_uint32_tensor(arr, FORWARD);
 * // arr = [10, 20, 30, 40]
 *
 * size_t idx = 0u;
 * error_code_t err = uint32_tensor_bsearch(arr, &idx, 30u);
 * // err == NO_ERROR, idx == 2
 *
 * err = uint32_tensor_bsearch(arr, &idx, 25u);
 * // err == NOT_FOUND, idx unchanged
 *
 * return_uint32_tensor(arr);
 * @endcode
 */
error_code_t uint32_tensor_bsearch(const uint32_tensor_t* t,
                                   size_t* index,
                                   uint32_t value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Bracketed binary search of a sorted uint32_t tensor.
 *
 * Locates the pair of adjacent indices that bracket value in a sorted
 * ascending tensor.  Unlike uint32_tensor_bsearch which returns
 * NOT_FOUND when an exact match is absent, this function always returns
 * the nearest enclosing indices, making it suitable for interpolation,
 * range queries, and nearest-neighbour lookups where the value is
 * unlikely to exist exactly in the array.
 *
 * The tensor must be sorted in ascending order before calling this
 * function — the result is undefined if the data is unsorted.
 *
 * Return cases:
 *
 *   Exact match:
 *     has_value == true, lower == upper == index of matching element.
 *
 *   Value bracketed:
 *     has_value == true, lower is the index of the largest element
 *     <= value, upper is the index of the smallest element >= value.
 *
 *   Value below all elements (value < data[0]):
 *     has_value == false, error == BELOW_RANGE,
 *     u.value.lower == 0, u.value.upper == 0.
 *     The caller can read the lower bound via u.value.upper.
 *
 *   Value above all elements (value > data[len-1]):
 *     has_value == false, error == ABOVE_RANGE,
 *     u.value.lower == len-1, u.value.upper == len-1.
 *     The caller can read the upper bound via u.value.lower.
 *
 * @param t      Pointer to the source tensor. Must not be NULL.
 *               Must be sorted in ascending order.
 * @param value  The uint32_t value to bracket.
 *
 * @return bracket_expect_t:
 *         - has_value true  → exact match or bracket found, u.value
 *                             holds lower and upper indices.
 *         - has_value false → u.error is one of:
 *             NULL_POINTER  if t is NULL
 *             EMPTY         if t->base->len == 0
 *             BELOW_RANGE   if value < data[0];  u.value.upper == 0
 *             ABOVE_RANGE   if value > data[len-1]; u.value.lower == len-1
 *
 * @code{.c}
 * // arr = [10, 20, 30, 40, 50]
 * bracket_expect_t r;
 *
 * // Exact match
 * r = uint32_tensor_bbsearch(arr, 30u);
 * // r.has_value == true, r.u.value.lower == 2, r.u.value.upper == 2
 *
 * // Bracketed
 * r = uint32_tensor_bbsearch(arr, 25u);
 * // r.has_value == true, r.u.value.lower == 1, r.u.value.upper == 2
 * // data[1]==20 <= 25 <= data[2]==30
 *
 * // Below range
 * r = uint32_tensor_bbsearch(arr, 5u);
 * // r.has_value == false, r.u.error == BELOW_RANGE
 * // r.u.value.upper == 0  (nearest element is data[0]==10)
 *
 * // Above range
 * r = uint32_tensor_bbsearch(arr, 99u);
 * // r.has_value == false, r.u.error == ABOVE_RANGE
 * // r.u.value.lower == 4  (nearest element is data[4]==50)
 * @endcode
 */
bracket_expect_t uint32_tensor_bbsearch(const uint32_tensor_t* t,
                                       uint32_t               value);
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
 * push_back_uint32_tensor(arr, 0u);   // arr = [0]
 * set_uint32_tensor_index(arr, 0u, 99u);   // arr = [99]
 * @endcode
 */
static inline error_code_t set_uint32_tensor_index(uint32_tensor_t* t,
                                                  size_t index,
                                                  uint32_t data) {
    if (t == NULL) return NULL_POINTER;
    return set_tensor_index(t->base, index, &data, UINT32_TYPE);
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
 * // For a 3x3 uint32 matrix initialised via init_tensor
 * size_t idx[] = { 1u, 2u };
 * set_uint32_tensor_nd_index(mat, idx, 42u);   // mat[1][2] = 42
 * @endcode
 */
static inline error_code_t set_uint32_tensor_nd_index(uint32_tensor_t* t,
                                                     const size_t* idx,
                                                     uint32_t data) {
    if (t == NULL) return NULL_POINTER;
    return set_tensor_nd_index(t->base, idx, &data, UINT32_TYPE);
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
 * push_back_uint32_tensor(arr, 10u);
 * push_back_uint32_tensor(arr, 20u);
 * // arr = [10, 20], len == 2
 * @endcode
 */
static inline error_code_t push_back_uint32_array(uint32_tensor_t* t,
                                                 uint32_t data) {
    if (t == NULL) return NULL_POINTER;
    return push_back_tensor(t->base, &data, UINT32_TYPE);
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
 * push_back_uint32_tensor(arr, 10u);
 * push_front_uint32_tensor(arr, 99u);
 * // arr = [99, 10]
 * @endcode
 */
static inline error_code_t push_front_uint32_array(uint32_tensor_t* t,
                                                  uint32_t data) {
    if (t == NULL) return NULL_POINTER;
    return push_front_tensor(t->base, &data, UINT32_TYPE);
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
 * push_at_uint32_tensor(arr, 99u, 1u);
 * // arr = [10, 99, 20, 30]
 * @endcode
 */
static inline error_code_t push_at_uint32_array(uint32_tensor_t* t,
                                               uint32_t data,
                                               size_t index) {
    if (t == NULL) return NULL_POINTER;
    return push_at_tensor(t->base, &data, index, UINT32_TYPE);
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
 * uint32_t val;
 * pop_back_uint32_tensor(arr, &val);   // val == 30, arr = [10, 20]
 * pop_back_uint32_tensor(arr, NULL);   // discard, arr = [10]
 * @endcode
 */
static inline error_code_t pop_back_uint32_array(uint32_tensor_t* t,
                                                uint32_t* out) {
    if (t == NULL) return NULL_POINTER;
    return pop_back_tensor(t->base, out, UINT32_TYPE);
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
 * uint32_t val;
 * pop_front_uint32_tensor(arr, &val);   // val == 10, arr = [20, 30]
 * @endcode
 */
static inline error_code_t pop_front_uint32_array(uint32_tensor_t* t,
                                                 uint32_t* out) {
    if (t == NULL) return NULL_POINTER;
    return pop_front_tensor(t->base, out, UINT32_TYPE);
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
 * uint32_t val;
 * pop_at_uint32_tensor(arr, &val, 2u);   // val == 30, arr = [10, 20, 40]
 * @endcode
 */
static inline error_code_t pop_at_uint32_array(uint32_tensor_t* t,
                                              uint32_t* out,
                                              size_t index) {
    if (t == NULL) return NULL_POINTER;
    return pop_at_tensor(t->base, out, index, UINT32_TYPE);
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
 * uint32_t val;
 * get_uint32_tensor_index(arr, 1u, &val);   // val == 20
 * @endcode
 */
static inline error_code_t get_uint32_tensor_index(const uint32_tensor_t* t,
                                                  size_t index,
                                                  uint32_t* out) {
    if (t == NULL || out == NULL) return NULL_POINTER;
    return get_tensor_index(t->base, index, out, UINT32_TYPE);
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
 * // For a 3x3 uint32 matrix
 * size_t idx[] = { 1u, 2u };
 * uint32_t val;
 * get_uint32_tensor_nd_index(mat, idx, &val);   // val = mat[1][2]
 * @endcode
 */
static inline error_code_t get_uint32_tensor_nd_index(const uint32_tensor_t* t,
                                                     const size_t* idx,
                                                     uint32_t* out) {
    if (t == NULL || idx == NULL || out == NULL) return NULL_POINTER;
    return get_tensor_nd_index(t->base, idx, out, UINT32_TYPE);
}
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_uint32_H */
// ================================================================================
// ================================================================================
// eof
