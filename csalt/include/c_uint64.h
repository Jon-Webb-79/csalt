// ================================================================================
// ================================================================================
// - File:    c_uint64.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 06, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_uint64_H
#define c_uint64_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "c_array.h"
#include "c_dict.h"
#include "c_matrix.h"
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

/**
 * @brief A type-safe dynamic array of uint64_t elements.
 *
 * Wraps array_t with the dtype fixed to UINT64_TYPE at initialization.
 * All operations enforce UINT64_TYPE at the call site so the caller never
 * needs to pass a dtype_id_t argument. The underlying array_t is embedded
 * by value so the struct can be stack- or heap-allocated by the caller.
 */
typedef struct {
    array_t base; /**< Underlying generic array. Do not modify directly. */
} uint64_array_t;

// ================================================================================
// Expected type
// ================================================================================

/**
 * @brief Expected return type for uint64_array_t initialization and copy/slice.
 */
typedef struct {
    bool has_value;
    union {
        uint64_array_t* value;
        error_code_t    error;
    } u;
} uint64_array_expect_t;

// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialize a new heap-allocated uint64_array_t.
 *
 * Allocates both the array_t struct and its internal data buffer through
 * alloc_v. The element type is fixed to UINT64_TYPE and the element size is
 * cached as 8 bytes. The array starts empty (len == 0) with the full capacity
 * reserved. When growth is true the data buffer will be reallocated
 * automatically whenever a push operation would exceed the current capacity;
 * when false any push beyond capacity returns CAPACITY_OVERFLOW without
 * modifying the array. The caller is responsible for calling
 * return_uint64_array when the array is no longer needed.
 *
 * @param capacity  Initial number of elements the buffer can hold.
 *                  Must be > 0.
 * @param growth    Pass true to allow the buffer to grow automatically on
 *                  overflow, false to use a fixed-capacity buffer.
 * @param alloc_v   Allocator vtable used for all internal memory operations.
 *                  alloc_v.allocate must not be NULL.
 *
 * @return uint64_array_expect_t with has_value true and u.value pointing to
 *         the initialized array on success. On failure has_value is false
 *         and u.error is one of:
 *         - NULL_POINTER    if alloc_v.allocate is NULL
 *         - INVALID_ARG     if capacity is 0
 *         - ILLEGAL_STATE   if the dtype registry could not be initialized
 *         - LENGTH_OVERFLOW if capacity would overflow size_t
 *         - BAD_ALLOC       if the struct allocation fails
 *         - OUT_OF_MEMORY   if the data buffer allocation fails
 *
 * @code
 *     allocator_vtable_t alloc = heap_allocator();
 *
 *     // Fixed-capacity buffer of 32 elements, no automatic growth.
 *     uint64_array_expect_t result = init_uint64_array(32, false, alloc);
 *     if (!result.has_value) {
 *         fprintf(stderr, "init failed: %d\n", result.u.error);
 *         return;
 *     }
 *     uint64_array_t* arr = result.u.value;
 *
 *     push_back_uint64_array(arr, 1000000u);
 *     push_back_uint64_array(arr, 2000000u);
 *     push_back_uint64_array(arr, 3000000u);
 *     // arr now contains [1000000, 2000000, 3000000], len == 3, alloc == 32.
 *
 *     return_uint64_array(arr);
 * @endcode
 */
uint64_array_expect_t init_uint64_array(size_t             capacity,
                                        bool               growth,
                                        allocator_vtable_t alloc_v);

// --------------------------------------------------------------------------------

/**
 * @brief Release all memory owned by a uint64_array_t back to its allocator.
 *
 * Frees the internal data buffer and then the array_t struct itself using
 * the allocator vtable that was supplied at initialization. After this call
 * the pointer is invalid and must not be dereferenced. Passing NULL is safe
 * and has no effect, making it suitable for use in cleanup paths without a
 * prior NULL check.
 *
 * @param array  Pointer to the array to release. May be NULL.
 *
 * @code
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint64_array_expect_t result = init_uint64_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint64_array_t* arr = result.u.value;
 *
 *     push_back_uint64_array(arr, 100000u);
 *
 *     // Release all memory. arr must not be used after this point.
 *     return_uint64_array(arr);
 *     arr = NULL;  // Optional but recommended to prevent accidental reuse.
 *
 *     // Passing NULL is always safe.
 *     return_uint64_array(NULL);
 * @endcode
 */
void return_uint64_array(uint64_array_t* array);

// ================================================================================
// Push operations
// ================================================================================

/**
 * @brief Append one uint64_t to the end of the array.
 *
 * Copies value into the next available slot at index array->base.len and
 * increments len by one. If the array is full and growth is true the data
 * buffer is reallocated before the value is written; if growth is false the
 * call fails immediately with CAPACITY_OVERFLOW and the array is left
 * unmodified. This is an O(1) amortised operation when growth is true and
 * strictly O(1) when the buffer has spare capacity.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param value  The uint64_t value to append.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint64_array_expect_t result = init_uint64_array(4, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint64_array_t* arr = result.u.value;
 *
 *     push_back_uint64_array(arr, 1000u);
 *     push_back_uint64_array(arr, 2000u);
 *     push_back_uint64_array(arr, 3000u);
 *     // arr contains [1000, 2000, 3000], len == 3.
 *
 *     uint64_t out = 0;
 *     get_uint64_array_index(arr, 2, &out);  // out == 3000
 *
 *     return_uint64_array(arr);
 * @endcode
 */
error_code_t push_back_uint64_array(uint64_array_t* array, uint64_t value);

// --------------------------------------------------------------------------------

/**
 * @brief Prepend one uint64_t to the front of the array.
 *
 * Shifts all existing elements one position toward the back using memmove,
 * writes value into index 0, and increments len by one. If the array is full
 * and growth is true the data buffer is reallocated before the shift; if
 * growth is false the call fails immediately with CAPACITY_OVERFLOW and the
 * array is left unmodified. Because every existing element must be moved this
 * is an O(n) operation — prefer push_back_uint64_array for performance-sensitive
 * append-heavy workloads.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param value  The uint64_t value to prepend.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint64_array_expect_t result = init_uint64_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint64_array_t* arr = result.u.value;
 *
 *     push_back_uint64_array(arr, 2000u);
 *     push_back_uint64_array(arr, 3000u);
 *     push_front_uint64_array(arr, 1000u);
 *     // arr contains [1000, 2000, 3000].
 *
 *     return_uint64_array(arr);
 * @endcode
 */
error_code_t push_front_uint64_array(uint64_array_t* array, uint64_t value);

// --------------------------------------------------------------------------------

/**
 * @brief Insert one uint64_t at an arbitrary index.
 *
 * Shifts all elements at positions >= index one slot toward the back using
 * memmove, writes value at index, and increments len by one. If the array is
 * full and growth is true the data buffer is reallocated before the shift;
 * if growth is false the call fails immediately with CAPACITY_OVERFLOW. Passing
 * index == 0 is equivalent to push_front_uint64_array; passing index == len
 * is equivalent to push_back_uint64_array. This is an O(n) operation in the
 * general case.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param index  Zero-based insertion position. Must be <= array->base.len.
 * @param value  The uint64_t value to insert.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - OUT_OF_BOUNDS     if index > array->base.len
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint64_array_expect_t result = init_uint64_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint64_array_t* arr = result.u.value;
 *
 *     push_back_uint64_array(arr, 1000u);
 *     push_back_uint64_array(arr, 3000u);
 *     push_at_uint64_array(arr, 1, 2000u);
 *     // arr contains [1000, 2000, 3000].
 *
 *     return_uint64_array(arr);
 * @endcode
 */
error_code_t push_at_uint64_array(uint64_array_t* array,
                                  size_t          index,
                                  uint64_t        value);

// ================================================================================
// Get operation
// ================================================================================

/**
 * @brief Copy the element at index into *out.
 *
 * Performs a bounds check and copies the element at the given zero-based index
 * into the location pointed to by out. The array is not modified. The call is
 * O(1).
 *
 * @param array  Pointer to the array to read. Must not be NULL.
 * @param index  Zero-based index of the element to retrieve. Must be
 *               < array->base.len.
 * @param out    Pointer to a uint64_t that receives the value. Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array or out is NULL
 *         - OUT_OF_BOUNDS if index >= array->base.len
 *
 * @code
 *     uint64_t val = 0;
 *     get_uint64_array_index(arr, 0, &val);  // val == first element
 * @endcode
 */
error_code_t get_uint64_array_index(const uint64_array_t* array,
                                    size_t                index,
                                    uint64_t*             out);

// ================================================================================
// Pop operations
// ================================================================================

/**
 * @brief Remove and optionally return the last element of the array.
 *
 * Decrements len by one. If out is non-NULL the removed element is copied
 * into *out before the length is reduced. If out is NULL the element is
 * discarded. The data buffer is never reallocated or zeroed. The call is O(1).
 *
 * @param array  Pointer to the array. Must not be NULL.
 * @param out    Pointer to a uint64_t that receives the removed value, or NULL
 *               to discard it.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *
 * @code
 *     uint64_t val = 0;
 *     pop_back_uint64_array(arr, &val);   // removes last; val holds its value
 *     pop_back_uint64_array(arr, NULL);   // removes last; value discarded
 * @endcode
 */
error_code_t pop_back_uint64_array(uint64_array_t* array, uint64_t* out);

// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally return the first element of the array.
 *
 * Copies the element at index 0 into *out (if out is non-NULL), then shifts
 * all remaining elements one position toward the front using memmove and
 * decrements len by one. The call is O(n).
 *
 * @param array  Pointer to the array. Must not be NULL.
 * @param out    Pointer to a uint64_t that receives the removed value, or NULL
 *               to discard it.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *
 * @code
 *     uint64_t val = 0;
 *     pop_front_uint64_array(arr, &val);  // removes first; val holds its value
 * @endcode
 */
error_code_t pop_front_uint64_array(uint64_array_t* array, uint64_t* out);

// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally return the element at an arbitrary index.
 *
 * Copies the element at index into *out (if out is non-NULL), then shifts all
 * elements after index one position toward the front using memmove and
 * decrements len by one. Passing index == 0 is equivalent to
 * pop_front_uint64_array; passing index == len - 1 is equivalent to
 * pop_back_uint64_array. The call is O(n) in the general case.
 *
 * @param array  Pointer to the array. Must not be NULL.
 * @param out    Pointer to a uint64_t that receives the removed value, or NULL
 *               to discard it.
 * @param index  Zero-based index of the element to remove. Must be
 *               < array->base.len.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *         - OUT_OF_BOUNDS if index >= array->base.len
 *
 * @code
 *     uint64_t val = 0;
 *     pop_any_uint64_array(arr, &val, 2);  // removes element at index 2
 * @endcode
 */
error_code_t pop_any_uint64_array(uint64_array_t* array,
                                  uint64_t*       out,
                                  size_t          index);

// ================================================================================
// Utility operations
// ================================================================================

/**
 * @brief Reset the array to an empty state without releasing its buffer.
 *
 * Sets len to 0 without freeing or zeroing the data buffer. The allocated
 * capacity is preserved. Subsequent push operations will overwrite the
 * existing memory. The call is O(1).
 *
 * @param array  Pointer to the array. Must not be NULL.
 *
 * @return NO_ERROR on success, or NULL_POINTER if array is NULL.
 *
 * @code
 *     clear_uint64_array(arr);
 *     // uint64_array_size(arr) == 0, uint64_array_alloc(arr) unchanged.
 * @endcode
 */
error_code_t clear_uint64_array(uint64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Overwrite the element at index with a new value.
 *
 * Performs a bounds check and copies value into the element slot at index.
 * The array length is not changed. The call is O(1).
 *
 * @param array  Pointer to the array. Must not be NULL.
 * @param index  Zero-based index of the element to overwrite. Must be
 *               < array->base.len.
 * @param value  The new uint64_t value to store at index.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - OUT_OF_BOUNDS if index >= array->base.len
 *
 * @code
 *     set_uint64_array_index(arr, 1, 9999u);  // overwrites element at index 1
 * @endcode
 */
error_code_t set_uint64_array_index(uint64_array_t* array,
                                    size_t          index,
                                    uint64_t        value);

// --------------------------------------------------------------------------------

/**
 * @brief Allocate an independent deep copy of an array.
 *
 * Creates a new uint64_array_t with the same elements, length, capacity, dtype,
 * and growth flag as src. Both the new struct and its data buffer are allocated
 * through alloc_v. Subsequent modifications to either array do not affect the
 * other. The caller is responsible for calling return_uint64_array on the
 * returned array when it is no longer needed.
 *
 * @param src     Pointer to the source array. Must not be NULL.
 * @param alloc_v Allocator vtable for the new array. alloc_v.allocate must
 *                not be NULL.
 *
 * @return uint64_array_expect_t with has_value true and u.value pointing to
 *         the new array on success. On failure has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if src is NULL
 *         - BAD_ALLOC     if the struct allocation fails
 *         - OUT_OF_MEMORY if the data buffer allocation fails
 *
 * @code
 *     uint64_array_expect_t cr = copy_uint64_array(src, alloc);
 *     if (!cr.has_value) { return; }
 *     uint64_array_t* dst = cr.u.value;
 *     // dst is independent of src.
 *     return_uint64_array(dst);
 * @endcode
 */
uint64_array_expect_t copy_uint64_array(const uint64_array_t* src,
                                        allocator_vtable_t    alloc_v);

// --------------------------------------------------------------------------------

/**
 * @brief Append all elements of src to the end of dst.
 *
 * Copies every element of src into dst in order, starting at index
 * dst->base.len and growing dst if necessary. src is not modified. If dst
 * does not have sufficient spare capacity and growth is false, the call fails
 * with CAPACITY_OVERFLOW and dst is left unmodified.
 *
 * @param dst  Pointer to the destination array. Must not be NULL.
 * @param src  Pointer to the source array. Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if dst or src is NULL
 *         - CAPACITY_OVERFLOW if dst cannot hold the extra elements
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *
 * @code
 *     concat_uint64_array(dst, src);
 *     // dst now ends with all elements that were in src.
 * @endcode
 */
error_code_t concat_uint64_array(uint64_array_t*       dst,
                                 const uint64_array_t* src);

// --------------------------------------------------------------------------------

/**
 * @brief Allocate a new array containing a sub-range of src.
 *
 * Copies the elements at indices [start, end) from src into a freshly
 * allocated uint64_array_t of capacity end - start. The source array is not
 * modified. The caller is responsible for calling return_uint64_array on the
 * returned array.
 *
 * @param src     Pointer to the source array. Must not be NULL.
 * @param start   Zero-based index of the first element to include (inclusive).
 * @param end     Zero-based index one past the last element to include
 *                (exclusive). Must be > start and <= src->base.len.
 * @param alloc_v Allocator for the new array.
 *
 * @return uint64_array_expect_t with has_value true and u.value pointing to
 *         the new array on success. On failure has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if src is NULL
 *         - INVALID_ARG   if start >= end
 *         - OUT_OF_BOUNDS if end > src->base.len
 *         - BAD_ALLOC     if the struct allocation fails
 *         - OUT_OF_MEMORY if the data buffer allocation fails
 *
 * @code
 *     // src: [10, 20, 30, 40, 50]
 *     uint64_array_expect_t sr = slice_uint64_array(src, 1, 4, alloc);
 *     // sr.u.value contains [20, 30, 40].
 *     return_uint64_array(sr.u.value);
 * @endcode
 */
uint64_array_expect_t slice_uint64_array(const uint64_array_t* src,
                                         size_t                start,
                                         size_t                end,
                                         allocator_vtable_t    alloc_v);

// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of all elements in place.
 *
 * Swaps elements symmetrically from both ends toward the middle. The array
 * length and capacity are unchanged. The call is O(n).
 *
 * @param array  Pointer to the array. Must not be NULL.
 *
 * @return NO_ERROR on success, or NULL_POINTER if array is NULL.
 *
 * @code
 *     // arr: [1, 2, 3, 4, 5]
 *     reverse_uint64_array(arr);
 *     // arr: [5, 4, 3, 2, 1]
 * @endcode
 */
error_code_t reverse_uint64_array(uint64_array_t* array);

// ================================================================================
// Sort
// ================================================================================

/**
 * @brief Sort all elements in place in ascending or descending order.
 *
 * Uses the generic sort_array path with an unsigned 64-bit comparator. The
 * comparator uses the three-way idiom to avoid overflow for large values near
 * UINT64_MAX. The sort is not guaranteed to be stable. The call is O(n log n)
 * on average.
 *
 * @param array  Pointer to the array to sort. Must not be NULL.
 * @param dir    FORWARD for ascending (smallest first), REVERSE for descending.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *
 * @code
 *     // arr: [30, 10, 20]
 *     sort_uint64_array(arr, FORWARD);
 *     // arr: [10, 20, 30]
 *
 *     sort_uint64_array(arr, REVERSE);
 *     // arr: [30, 20, 10]
 * @endcode
 */
error_code_t sort_uint64_array(uint64_array_t* array, direction_t dir);

// ================================================================================
// Reduction
// ================================================================================

/**
 * @brief Find the minimum value in the array.
 *
 * Scans all elements using an unsigned 64-bit comparison and returns the
 * smallest value found. When two or more elements share the minimum value the
 * value of the first occurrence is returned, consistent with the behaviour of
 * uint64_array_contains. The array is not modified. The scan dispatches to
 * the best SIMD horizontal-reduction path available at compile time and falls
 * back to a scalar loop on unsupported targets.
 *
 * @param array  Pointer to the array to scan. Must not be NULL and must
 *               contain at least one element.
 *
 * @return uint64_expect_t with has_value true and u.value == the minimum
 *         element on success. On failure, has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if array->base.len == 0
 *
 * @code
 *     push_back_uint64_array(arr, 300u);
 *     push_back_uint64_array(arr, 100u);
 *     push_back_uint64_array(arr, 200u);
 *
 *     uint64_expect_t r = uint64_array_min(arr);
 *     // r.has_value == true, r.u.value == 100.
 * @endcode
 */
uint64_expect_t uint64_array_min(const uint64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Find the maximum value in the array.
 *
 * Scans all elements using an unsigned 64-bit comparison and returns the
 * largest value found. When two or more elements share the maximum value the
 * value of the first occurrence is returned, consistent with the behaviour of
 * uint64_array_contains. The array is not modified. The scan dispatches to
 * the best SIMD horizontal-reduction path available at compile time and falls
 * back to a scalar loop on unsupported targets.
 *
 * @param array  Pointer to the array to scan. Must not be NULL and must
 *               contain at least one element.
 *
 * @return uint64_expect_t with has_value true and u.value == the maximum
 *         element on success. On failure, has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if array->base.len == 0
 *
 * @code
 *     uint64_expect_t r = uint64_array_max(arr);
 *     // r.has_value == true, r.u.value == 300.
 * @endcode
 */
uint64_expect_t uint64_array_max(const uint64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Sum all elements and return the result as a uint64_t.
 *
 * Iterates over every element and accumulates the total in a uint64_t. The
 * inner loop dispatches to the best SIMD load path available at compile time.
 *
 * Overflow: the accumulator is a uint64_t. If the true sum exceeds UINT64_MAX
 * the result wraps modulo 2^64. The caller is responsible for ensuring the
 * array is small enough that overflow will not occur.
 *
 * @param array  Pointer to the array to sum. Must not be NULL.
 *
 * @return uint64_expect_t with has_value true and u.value == the sum on
 *         success. On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if array->base.len == 0
 *
 * @code
 *     push_back_uint64_array(arr, 1000000u);
 *     push_back_uint64_array(arr, 2000000u);
 *     push_back_uint64_array(arr, 3000000u);
 *
 *     uint64_expect_t r = uint64_array_sum(arr);
 *     // r.has_value == true, r.u.value == 6000000.
 * @endcode
 */
uint64_expect_t uint64_array_sum(const uint64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Compute the prefix sum of a uint64 array into a new uint64 array.
 *
 * Allocates a new uint64_array_t of the same length as @p src and fills it
 * with the inclusive prefix sum: output[0] == src[0], and output[i] ==
 * output[i-1] + src[i] for every subsequent index. @p src is not modified.
 *
 * Overflow: each element is accumulated in a uint64_t. If the running total
 * exceeds UINT64_MAX it wraps modulo 2^64. The caller is responsible for
 * ensuring the values remain within range.
 *
 * The output array is owned by the caller and must be freed with
 * return_uint64_array when no longer needed.
 *
 * @param src    Source array to scan. Must not be NULL and must contain at
 *               least one element.
 * @param alloc  Allocator used for the output array.
 *
 * @return uint64_array_expect_t with has_value true and u.value pointing to
 *         the newly allocated output array on success. On failure, has_value
 *         is false and u.error is one of:
 *         - NULL_POINTER  if src is NULL
 *         - EMPTY         if src contains no elements
 *         - BAD_ALLOC     if the output struct allocation failed
 *         - OUT_OF_MEMORY if the output data buffer allocation failed
 *
 * @code
 *     push_back_uint64_array(src, 1000u);
 *     push_back_uint64_array(src, 2000u);
 *     push_back_uint64_array(src, 3000u);
 *     push_back_uint64_array(src, 4000u);
 *     // src contains [1000, 2000, 3000, 4000].
 *
 *     uint64_array_expect_t cr = cumulative_uint64_array(src, alloc);
 *     // cr.u.value contains [1000, 3000, 6000, 10000].
 *
 *     return_uint64_array(src);
 *     return_uint64_array(cr.u.value);
 * @endcode
 */
uint64_array_expect_t cumulative_uint64_array(const uint64_array_t* src,
                                              allocator_vtable_t    alloc);

// ================================================================================
// Search
// ================================================================================

/**
 * @brief Search for the first occurrence of value in the range [start, end).
 *
 * Scans elements at indices [start, end) using the best SIMD instruction set
 * available at compile time. The search performs an exact bitwise comparison —
 * the first element whose stored value equals value is returned. If the value
 * appears more than once only the lowest index within the range is returned.
 * The array is not modified.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param value  The uint64_t value to search for.
 * @param start  Zero-based index of the first element to examine (inclusive).
 * @param end    Zero-based index one past the last element to examine
 *               (exclusive). Must be > start and <= array->base.len.
 *
 * @return size_expect_t with has_value true and u.value holding the zero-based
 *         index of the first match on success. On failure has_value is false
 *         and u.error is one of:
 *         - NULL_POINTER  if array is NULL
 *         - INVALID_ARG   if start >= end
 *         - OUT_OF_BOUNDS if end > array->base.len
 *         - NOT_FOUND     if no element in [start, end) equals value
 *
 * @code
 *     // arr: [100, 300, 200, 300, 400]
 *     size_expect_t r = uint64_array_contains(arr, 300u, 0, 5);
 *     // r.has_value == true, r.u.value == 1  (first occurrence).
 *
 *     r = uint64_array_contains(arr, 300u, 2, 5);
 *     // r.has_value == true, r.u.value == 3  (first in [2,5)).
 * @endcode
 */
size_expect_t uint64_array_contains(const uint64_array_t* array,
                                    uint64_t              value,
                                    size_t                start,
                                    size_t                end);

// --------------------------------------------------------------------------------

/**
 * @brief Binary-search for a value in the array.
 *
 * If sort is true the array is sorted in ascending order before the search.
 * If sort is false the array must already be sorted in ascending order;
 * results are undefined for unsorted input. On success the index of the
 * first occurrence of value is returned.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param value  The uint64_t value to find.
 * @param sort   Pass true to sort before searching, false if already sorted.
 *
 * @return size_expect_t with has_value true and u.value == the zero-based
 *         index of the first match on success. On failure has_value is false
 *         and u.error is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *         - NOT_FOUND     if value is not present
 *
 * @code
 *     // arr (unsorted): [300, 100, 200]
 *     size_expect_t r = uint64_array_binary_search(arr, 200u, true);
 *     // arr is now [100, 200, 300]; r.u.value == 1.
 * @endcode
 */
size_expect_t uint64_array_binary_search(uint64_array_t* array,
                                         uint64_t        value,
                                         bool            sort);

// --------------------------------------------------------------------------------

/**
 * @brief Find the bracketing indices for a value in a sorted array.
 *
 * If sort is true the array is sorted in ascending order before the search.
 * Returns the lower bound (first index whose element >= value) and the upper
 * bound (last index whose element <= value). When value exists exactly in the
 * array lower == upper and both point to the first occurrence.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param value  The uint64_t value to bracket.
 * @param sort   Pass true to sort before searching, false if already sorted.
 *
 * @return bracket_expect_t with has_value true on success and:
 *           u.value.lower — index of the first element >= value
 *           u.value.upper — index of the last  element <= value
 *         On failure has_value is false and u.error is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *         - NOT_FOUND     if value is outside [min, max] of the array
 *
 * @code
 *     // arr: [1000, 3000, 5000, 7000]
 *     bracket_expect_t r = uint64_array_binary_bracket(arr, 3000u, false);
 *     // r.u.value.lower == 1, r.u.value.upper == 1  (exact match).
 *
 *     r = uint64_array_binary_bracket(arr, 4000u, false);
 *     // r.u.value.lower == 2  (5000), r.u.value.upper == 1  (3000).
 * @endcode
 */
bracket_expect_t uint64_array_binary_bracket(uint64_array_t* array,
                                             uint64_t        value,
                                             bool            sort);

// ================================================================================
// Introspection
// ================================================================================

/**
 * @brief Return the number of elements currently stored in the array.
 *
 * Reads array->base.len. The call is O(1) and does not modify the array.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return The current element count, or 0 if array is NULL.
 *
 * @code
 *     push_back_uint64_array(arr, 1000u);
 *     push_back_uint64_array(arr, 2000u);
 *     // uint64_array_size(arr) == 2.
 * @endcode
 */
size_t uint64_array_size(const uint64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return the number of elements the buffer can currently hold.
 *
 * Reads array->base.alloc. The call is O(1) and does not modify the array.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return The allocated capacity in elements, or 0 if array is NULL.
 *
 * @code
 *     // uint64_array_alloc(arr) == 8 after init with capacity 8.
 * @endcode
 */
size_t uint64_array_alloc(const uint64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return the size in bytes of one element in the array.
 *
 * Reads array->base.data_size, which is fixed to 8 for all uint64_array_t
 * instances — a uint64_t is always eight bytes. The call is O(1) and does not
 * modify the array.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return 8 when array is non-NULL, or 0 if array is NULL.
 *
 * @code
 *     size_t elem_bytes = uint64_array_data_size(arr);  // always 8
 * @endcode
 */
size_t uint64_array_data_size(const uint64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if the array currently contains no elements.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return true if array is NULL or array->base.len == 0, false otherwise.
 *
 * @code
 *     // is_uint64_array_empty(arr) == true immediately after init.
 *     push_back_uint64_array(arr, 1000u);
 *     // is_uint64_array_empty(arr) == false.
 * @endcode
 */
bool is_uint64_array_empty(const uint64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if the array has no remaining spare capacity.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return true if array is NULL or array->base.len == array->base.alloc,
 *         false otherwise.
 *
 * @code
 *     push_back_uint64_array(arr, 1000u);
 *     push_back_uint64_array(arr, 2000u);
 *     // is_uint64_array_full(arr) == false (one slot remains).
 *
 *     push_back_uint64_array(arr, 3000u);
 *     // is_uint64_array_full(arr) == true, size == alloc == 3.
 * @endcode
 */
bool is_uint64_array_full(const uint64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if ptr points to a valid element within the live region
 *        of the array's data buffer.
 *
 * A pointer is considered valid if array and ptr are both non-NULL,
 * ptr >= array->base.data, and ptr < array->base.data + array->base.len * 8.
 * Spare capacity beyond len is not considered valid. The call is O(1) and
 * does not modify the array.
 *
 * @param array  Pointer to the array to check against. May be NULL.
 * @param ptr    Pointer to test. May be NULL.
 *
 * @return true only if both arguments are non-NULL and ptr falls within the
 *         live element region. Returns false in all other cases.
 *
 * @code
 *     const uint64_t* first = (const uint64_t*)arr->base.data;
 *     is_uint64_array_ptr(arr, first);       // true
 *     is_uint64_array_ptr(arr, first + 2);   // true if len >= 3
 *     is_uint64_array_ptr(arr, first + len); // false — spare capacity
 *     is_uint64_array_ptr(arr, NULL);        // false
 * @endcode
 */
bool is_uint64_array_ptr(const uint64_array_t* array, const uint64_t* ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Print a uint64 array in bracketed form with line wrapping.
 *
 * Prints the array in the form:
 *
 *     [ 1, 5, 2, 3, 6 ]
 *
 * If appending the next value would cause the current line to exceed
 * 70 columns, printing continues on the next line. Continuation lines
 * are indented by two spaces.
 *
 * @param array   Array to print. Must not be NULL.
 * @param stream  Output stream to write to. Must not be NULL.
 *
 * @return NO_ERROR on success, or NULL_POINTER if array or stream is NULL.
 *
 * @code{.c}
 *     print_uint64_array(arr, stdout);
 * @endcode
 */
error_code_t print_uint64_array(const uint64_array_t* array, FILE* stream);
// ================================================================================ 
// ================================================================================ 

/**
 * @brief A dictionary that maps null-terminated C-string keys to uint64_t values.
 *
 * This is a typedef alias for @ref dict_t with the convention that
 * @c data_size is always @c sizeof(uint64_t) (8 bytes) and @c dtype is
 * always @c UINT64_TYPE.  Do not use the raw @ref dict_t API directly on a
 * @c uint64_dict_t — always go through the functions declared in this header
 * so that the type constraints are enforced.
 *
 * @code
 *     allocator_vtable_t a = heap_allocator();
 *     uint64_dict_expect_t r = init_uint64_dict(16, true, a);
 *     if (!r.has_value) { // handle r.u.error }
 *     uint64_dict_t* d = r.u.value;
 *
 *     insert_uint64_dict(d, "width",  1920u, a);
 *     insert_uint64_dict(d, "height", 1080u, a);
 *     insert_uint64_dict(d, "depth",    32u, a);
 *
 *     uint64_t v;
 *     get_uint64_dict_value(d, "width", &v);   // v == 1920
 *
 *     return_uint64_dict(d);
 * @endcode
 */
typedef dict_t uint64_dict_t;
 
/**
 * @brief Expected return type for @ref init_uint64_dict and
 *        @ref copy_uint64_dict.
 *
 * Check @c has_value before using @c u.value.  On failure @c u.error
 * contains the relevant @ref error_code_t.
 */
typedef struct {
    bool has_value;
    union {
        uint64_dict_t* value;
        error_code_t   error;
    } u;
} uint64_dict_expect_t;
 
// ================================================================================
// Iterator type
// ================================================================================
 
/**
 * @brief Typed iterator callback for @ref foreach_uint64_dict.
 *
 * @param key       Null-terminated C-string key (points into the dict's
 *                  internal storage — do not free or store beyond the
 *                  callback).
 * @param key_len   Length of @p key in bytes, excluding the null terminator.
 * @param value     The uint64_t value associated with @p key.
 * @param user_data Caller-supplied context pointer passed unchanged from
 *                  @ref foreach_uint64_dict; may be NULL.
 *
 * @code
 *     static void print_entry(const char* key, size_t key_len,
 *                             uint64_t value, void* ud) {
 *         (void)ud;
 *         printf("  %.*s => %u\n", (int)key_len, key, (unsigned)value);
 *     }
 *     foreach_uint64_dict(d, print_entry, NULL);
 * @endcode
 */
typedef void (*uint64_dict_iter_fn)(const char* key,
                                    size_t      key_len,
                                    uint64_t    value,
                                    void*       user_data);
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
/**
 * @brief Allocate and initialise a new uint64_dict_t.
 *
 * The underlying @ref dict_t is created with @c data_size = sizeof(uint64_t)
 * and @c dtype = UINT64_TYPE.  The bucket count is rounded up to the next
 * power of two internally.
 *
 * @param capacity  Initial bucket count.  Must be > 0.
 * @param growth    If true the table resizes automatically when the load
 *                  factor exceeds 0.75.
 * @param alloc_v   Allocator for all internal memory.  @c alloc_v.allocate
 *                  must not be NULL.
 *
 * @return @ref uint64_dict_expect_t with @c has_value true on success.
 *         On failure @c u.error is one of NULL_POINTER, INVALID_ARG, or
 *         OUT_OF_MEMORY.
 *
 * @code
 *     allocator_vtable_t a = heap_allocator();
 *
 *     uint64_dict_expect_t r = init_uint64_dict(8, true, a);
 *     if (!r.has_value) {
 *         fprintf(stderr, "init failed: %d\n", r.u.error);
 *         return;
 *     }
 *     uint64_dict_t* d = r.u.value;
 *
 *     insert_uint64_dict(d, "port",  8080u, a);
 *     insert_uint64_dict(d, "range", 1024u, a);
 *
 *     return_uint64_dict(d);
 * @endcode
 */
uint64_dict_expect_t init_uint64_dict(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Release all memory owned by a uint64_dict_t.
 *
 * Frees every node (key copy + value buffer), the bucket array, and the
 * dict struct itself.  Passing NULL is safe and performs no action.
 *
 * @param dict  Dictionary to release.
 *
 * @code
 *     uint64_dict_t* d = init_uint64_dict(8, true, a).u.value;
 *     insert_uint64_dict(d, "x", 1000u, a);
 *     // ... use the dict ...
 *     return_uint64_dict(d);   // d must not be used after this point
 * @endcode
 */
void return_uint64_dict(uint64_dict_t* dict);
 
// ================================================================================
// Insert
// ================================================================================
 
/**
 * @brief Insert a null-terminated string key with a uint64_t value.
 *
 * The key length is measured with @c strlen(key).  The key bytes are copied
 * into the dict's own storage — the caller may free or reuse @p key
 * immediately after this call returns.
 *
 * If the key already exists the insertion is rejected and INVALID_ARG is
 * returned.  To overwrite an existing value use @ref update_uint64_dict.
 *
 * @param dict    Must not be NULL.
 * @param key     Null-terminated C-string key.  Must not be NULL.
 * @param value   The uint64_t value to store.
 * @param alloc_v Allocator used to allocate the new node.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG (duplicate key),
 *         CAPACITY_OVERFLOW (growth == false and table is full), or
 *         OUT_OF_MEMORY.
 *
 * @code
 *     uint64_dict_t* d = init_uint64_dict(8, true, a).u.value;
 *
 *     insert_uint64_dict(d, "width",  1920u, a);   // OK
 *     insert_uint64_dict(d, "height", 1080u, a);   // OK
 *
 *     // Duplicate key — returns INVALID_ARG, dict is unchanged
 *     error_code_t err = insert_uint64_dict(d, "width", 800u, a);
 *     assert(err == INVALID_ARG);
 *
 *     return_uint64_dict(d);
 * @endcode
 */
error_code_t insert_uint64_dict(uint64_dict_t*     dict,
                                 const char*        key,
                                 uint64_t           value,
                                 allocator_vtable_t alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Insert a bounded key (explicit length) with a uint64_t value.
 *
 * Identical to @ref insert_uint64_dict but the key length is supplied by the
 * caller rather than measured with @c strlen.  Useful when the key is a
 * sub-string of a larger buffer or when the length is already known.
 *
 * @param dict    Must not be NULL.
 * @param key     Pointer to the first byte of the key.  Must not be NULL.
 * @param key_len Number of bytes in the key.  Must be > 0.
 * @param value   The uint64_t value to store.
 * @param alloc_v Allocator used to allocate the new node.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, CAPACITY_OVERFLOW, or
 *         OUT_OF_MEMORY.
 *
 * @code
 *     // Key is a sub-string: "width_max", take only "width" (5 bytes)
 *     const char* buf = "width_max";
 *     insert_uint64_dict_n(d, buf, 5, 1920u, a);
 *
 *     uint64_t v;
 *     get_uint64_dict_value_n(d, buf, 5, &v);   // v == 1920
 * @endcode
 */
error_code_t insert_uint64_dict_n(uint64_dict_t*     dict,
                                   const char*        key,
                                   size_t             key_len,
                                   uint64_t           value,
                                   allocator_vtable_t alloc_v);
 
// ================================================================================
// Pop (remove and retrieve)
// ================================================================================
 
/**
 * @brief Remove the entry for a null-terminated key and return its value.
 *
 * If the key is found the node is freed and the stored value is written into
 * @p out_value (if non-NULL).  If the key is not found @p out_value is not
 * written.
 *
 * @param dict       Must not be NULL.
 * @param key        Null-terminated C-string key.  Must not be NULL.
 * @param out_value  Destination for the removed value, or NULL to discard.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND.
 *
 * @code
 *     uint64_dict_t* d = init_uint64_dict(8, true, a).u.value;
 *     insert_uint64_dict(d, "port", 8080u, a);
 *
 *     uint64_t removed;
 *     error_code_t err = pop_uint64_dict(d, "port", &removed);
 *     assert(err == NO_ERROR && removed == 8080u);
 *
 *     // Key is gone — next pop returns NOT_FOUND
 *     err = pop_uint64_dict(d, "port", NULL);
 *     assert(err == NOT_FOUND);
 *
 *     return_uint64_dict(d);
 * @endcode
 */
error_code_t pop_uint64_dict(uint64_dict_t* dict,
                              const char*    key,
                              uint64_t*      out_value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Remove the entry for a bounded key and return its value.
 *
 * Identical to @ref pop_uint64_dict but the key length is supplied explicitly.
 *
 * @param dict       Must not be NULL.
 * @param key        Pointer to the key bytes.  Must not be NULL.
 * @param key_len    Number of bytes in the key.  Must be > 0.
 * @param out_value  Destination for the removed value, or NULL to discard.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, or NOT_FOUND.
 *
 * @code
 *     const char* buf = "port_http";
 *     insert_uint64_dict_n(d, buf, 4, 8080u, a);   // key = "port"
 *
 *     uint64_t v;
 *     pop_uint64_dict_n(d, buf, 4, &v);             // removes "port", v == 8080
 * @endcode
 */
error_code_t pop_uint64_dict_n(uint64_dict_t* dict,
                                const char*    key,
                                size_t         key_len,
                                uint64_t*      out_value);
 
// ================================================================================
// Update
// ================================================================================
 
/**
 * @brief Overwrite the value of an existing null-terminated key.
 *
 * No allocation is performed.  If the key does not exist NOT_FOUND is
 * returned and the dict is unchanged.  To insert a new entry use
 * @ref insert_uint64_dict.
 *
 * @param dict   Must not be NULL.
 * @param key    Null-terminated C-string key.  Must not be NULL.
 * @param value  New uint64_t value to store.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND.
 *
 * @code
 *     uint64_dict_t* d = init_uint64_dict(8, true, a).u.value;
 *     insert_uint64_dict(d, "port", 8080u, a);
 *
 *     update_uint64_dict(d, "port", 443u);
 *
 *     uint64_t v;
 *     get_uint64_dict_value(d, "port", &v);
 *     assert(v == 443u);
 *
 *     return_uint64_dict(d);
 * @endcode
 */
error_code_t update_uint64_dict(uint64_dict_t* dict,
                                 const char*    key,
                                 uint64_t       value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Overwrite the value of an existing bounded key.
 *
 * Identical to @ref update_uint64_dict but the key length is supplied
 * explicitly.
 *
 * @param dict     Must not be NULL.
 * @param key      Pointer to the key bytes.  Must not be NULL.
 * @param key_len  Number of bytes in the key.  Must be > 0.
 * @param value    New uint64_t value to store.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, or NOT_FOUND.
 *
 * @code
 *     const char* buf = "port_http";
 *     insert_uint64_dict_n(d, buf, 4, 8080u, a);   // key = "port"
 *     update_uint64_dict_n(d, buf, 4, 443u);        // update "port" to 443
 * @endcode
 */
error_code_t update_uint64_dict_n(uint64_dict_t* dict,
                                   const char*    key,
                                   size_t         key_len,
                                   uint64_t       value);
 
// ================================================================================
// Lookup
// ================================================================================
 
/**
 * @brief Copy the value for a null-terminated key into a caller-supplied
 *        variable.
 *
 * @param dict       Must not be NULL.
 * @param key        Null-terminated C-string key.  Must not be NULL.
 * @param out_value  Destination for the retrieved value.  Must not be NULL.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND.
 *
 * @code
 *     uint64_dict_t* d = init_uint64_dict(8, true, a).u.value;
 *     insert_uint64_dict(d, "width", 1920u, a);
 *
 *     uint64_t v;
 *     if (get_uint64_dict_value(d, "width", &v) == NO_ERROR)
 *         printf("width = %u\n", (unsigned)v);
 *
 *     return_uint64_dict(d);
 * @endcode
 */
error_code_t get_uint64_dict_value(const uint64_dict_t* dict,
                                    const char*          key,
                                    uint64_t*            out_value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Copy the value for a bounded key into a caller-supplied variable.
 *
 * Identical to @ref get_uint64_dict_value but the key length is supplied
 * explicitly.
 *
 * @param dict       Must not be NULL.
 * @param key        Pointer to the key bytes.  Must not be NULL.
 * @param key_len    Number of bytes in the key.  Must be > 0.
 * @param out_value  Destination for the retrieved value.  Must not be NULL.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, or NOT_FOUND.
 *
 * @code
 *     const char* buf = "width_max";
 *     insert_uint64_dict_n(d, buf, 5, 1920u, a);   // key = "width"
 *
 *     uint64_t v;
 *     get_uint64_dict_value_n(d, buf, 5, &v);       // v == 1920
 * @endcode
 */
error_code_t get_uint64_dict_value_n(const uint64_dict_t* dict,
                                      const char*          key,
                                      size_t               key_len,
                                      uint64_t*            out_value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Return a read-only pointer directly into the node's value buffer for
 *        a null-terminated key.
 *
 * The pointer is valid until the next mutation of the dict.  The caller must
 * not free it or write through it.
 *
 * @param dict  Must not be NULL.
 * @param key   Null-terminated C-string key.  Must not be NULL.
 *
 * @return Pointer to the stored uint64_t on success, NULL if not found or on
 *         error.
 *
 * @code
 *     insert_uint64_dict(d, "port", 8080u, a);
 *
 *     const uint64_t* p = get_uint64_dict_ptr(d, "port");
 *     if (p) printf("port = %u\n", (unsigned)*p);
 *     // Do not store p — it may be invalidated by the next insert or pop.
 * @endcode
 */
const uint64_t* get_uint64_dict_ptr(const uint64_dict_t* dict, const char* key);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Return a read-only pointer directly into the node's value buffer for
 *        a bounded key.
 *
 * Identical to @ref get_uint64_dict_ptr but the key length is supplied
 * explicitly.
 *
 * @param dict     Must not be NULL.
 * @param key      Pointer to the key bytes.  Must not be NULL.
 * @param key_len  Number of bytes in the key.  Must be > 0.
 *
 * @return Pointer to the stored uint64_t on success, NULL if not found or on
 *         error.
 *
 * @code
 *     const char* buf = "port_http";
 *     insert_uint64_dict_n(d, buf, 4, 8080u, a);   // key = "port"
 *
 *     const uint64_t* p = get_uint64_dict_ptr_n(d, buf, 4);
 *     if (p) printf("port = %u\n", (unsigned)*p);
 * @endcode
 */
const uint64_t* get_uint64_dict_ptr_n(const uint64_dict_t* dict,
                                       const char*          key,
                                       size_t               key_len);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Test whether a null-terminated key exists in the dict.
 *
 * @param dict  Must not be NULL.
 * @param key   Null-terminated C-string key.  Must not be NULL.
 *
 * @return true if the key exists, false otherwise (including on error).
 *
 * @code
 *     insert_uint64_dict(d, "active", 1u, a);
 *
 *     if (has_uint64_dict_key(d, "active"))
 *         printf("active is set\n");
 * @endcode
 */
bool has_uint64_dict_key(const uint64_dict_t* dict, const char* key);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Test whether a bounded key exists in the dict.
 *
 * Identical to @ref has_uint64_dict_key but the key length is supplied
 * explicitly.
 *
 * @param dict     Must not be NULL.
 * @param key      Pointer to the key bytes.  Must not be NULL.
 * @param key_len  Number of bytes in the key.  Must be > 0.
 *
 * @return true if the key exists, false otherwise.
 *
 * @code
 *     const char* buf = "active_flag";
 *     insert_uint64_dict_n(d, buf, 6, 1u, a);   // key = "active"
 *     assert(has_uint64_dict_key_n(d, buf, 6));
 * @endcode
 */
bool has_uint64_dict_key_n(const uint64_dict_t* dict,
                            const char*          key,
                            size_t               key_len);
 
// ================================================================================
// Utility
// ================================================================================
 
/**
 * @brief Remove all entries without freeing the dict or its bucket array.
 *
 * All nodes are freed via the dict's stored allocator.  The bucket array is
 * retained and zeroed, ready for reuse.  @c len and @c hash_size are reset
 * to 0.
 *
 * @param dict  Must not be NULL.
 *
 * @return NO_ERROR or NULL_POINTER.
 *
 * @code
 *     insert_uint64_dict(d, "a", 100u, a);
 *     insert_uint64_dict(d, "b", 200u, a);
 *     assert(uint64_dict_hash_size(d) == 2);
 *
 *     clear_uint64_dict(d);
 *     assert(uint64_dict_hash_size(d) == 0);
 *
 *     // The dict is still usable after clear
 *     insert_uint64_dict(d, "c", 300u, a);
 * @endcode
 */
error_code_t clear_uint64_dict(uint64_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Allocate a deep copy of a uint64_dict_t.
 *
 * All nodes are copied into a fresh bucket array of the same capacity.  The
 * new dict uses @p alloc_v for all of its allocations.
 *
 * @param src     Must not be NULL.
 * @param alloc_v Allocator for the new dict and its nodes.
 *
 * @return @ref uint64_dict_expect_t with @c has_value true on success.
 *
 * @code
 *     uint64_dict_t* orig = init_uint64_dict(8, true, a).u.value;
 *     insert_uint64_dict(orig, "x", 1000u, a);
 *     insert_uint64_dict(orig, "y", 2000u, a);
 *
 *     uint64_dict_expect_t cr = copy_uint64_dict(orig, a);
 *     assert(cr.has_value);
 *     uint64_dict_t* copy = cr.u.value;
 *
 *     // The copy is independent — mutating orig does not affect copy
 *     update_uint64_dict(orig, "x", 9999u);
 *     uint64_t v;
 *     get_uint64_dict_value(copy, "x", &v);
 *     assert(v == 1000u);
 *
 *     return_uint64_dict(copy);
 *     return_uint64_dict(orig);
 * @endcode
 */
uint64_dict_expect_t copy_uint64_dict(const uint64_dict_t* src,
                                      allocator_vtable_t   alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Merge two uint64_dict_t instances into a new dict.
 *
 * All entries from @p a are inserted first.  Entries from @p b are then
 * processed:
 * - If the key does not exist in the result it is inserted.
 * - If the key already exists and @p overwrite is true the value from @p b
 *   replaces the value from @p a.
 * - If the key already exists and @p overwrite is false the value from @p a
 *   is kept.
 *
 * Neither source dict is modified.
 *
 * @param a          First source dict.  Must not be NULL.
 * @param b          Second source dict.  Must not be NULL.
 * @param overwrite  If true, @p b's values win on key conflicts.
 * @param alloc_v    Allocator for the new dict.
 *
 * @return @ref uint64_dict_expect_t with @c has_value true on success.
 *
 * @code
 *     uint64_dict_t* da = init_uint64_dict(8, true, a).u.value;
 *     uint64_dict_t* db = init_uint64_dict(8, true, a).u.value;
 *
 *     insert_uint64_dict(da, "shared", 100u, a);
 *     insert_uint64_dict(da, "only_a",   1u, a);
 *     insert_uint64_dict(db, "shared", 999u, a);
 *     insert_uint64_dict(db, "only_b",   2u, a);
 *
 *     // Merge without overwrite — "shared" keeps da's value of 100
 *     uint64_dict_expect_t mr = merge_uint64_dict(da, db, false, a);
 *     uint64_t v;
 *     get_uint64_dict_value(mr.u.value, "shared", &v);
 *     assert(v == 100u);
 *
 *     return_uint64_dict(mr.u.value);
 *     return_uint64_dict(da);
 *     return_uint64_dict(db);
 * @endcode
 */
uint64_dict_expect_t merge_uint64_dict(const uint64_dict_t* a,
                                       const uint64_dict_t* b,
                                       bool                 overwrite,
                                       allocator_vtable_t   alloc_v);
 
// ================================================================================
// Iteration
// ================================================================================
 
/**
 * @brief Call @p fn once for every entry in the dict.
 *
 * Traversal order follows bucket order, which is not guaranteed to match
 * insertion order.  The callback receives the key as a null-terminated
 * C-string pointer (into internal storage), its length, and the uint64_t
 * value.  The callback must not insert or remove entries during traversal.
 *
 * @param dict       Must not be NULL.
 * @param fn         Typed callback.  Must not be NULL.
 * @param user_data  Passed unchanged to @p fn; may be NULL.
 *
 * @return NO_ERROR or NULL_POINTER.
 *
 * @code
 *     static void print_entry(const char* key, size_t key_len,
 *                             uint64_t value, void* ud) {
 *         (void)key_len; (void)ud;
 *         printf("  %s = %u\n", key, (unsigned)value);
 *     }
 *
 *     uint64_dict_t* d = init_uint64_dict(8, true, a).u.value;
 *     insert_uint64_dict(d, "width",  1920u, a);
 *     insert_uint64_dict(d, "height", 1080u, a);
 *
 *     foreach_uint64_dict(d, print_entry, NULL);
 *     // Output (order may vary):
 *     //   width = 1920
 *     //   height = 1080
 *
 *     return_uint64_dict(d);
 * @endcode
 */
error_code_t foreach_uint64_dict(const uint64_dict_t* dict,
                                  uint64_dict_iter_fn  fn,
                                  void*                user_data);
 
// ================================================================================
// Introspection
// ================================================================================
 
/**
 * @brief Number of occupied buckets (chains with at least one entry).
 *
 * Returns 0 if @p dict is NULL.
 */
size_t uint64_dict_size(const uint64_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Total number of key-value pairs stored.
 *
 * Returns 0 if @p dict is NULL.
 */
size_t uint64_dict_hash_size(const uint64_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Number of buckets currently allocated.
 *
 * Returns 0 if @p dict is NULL.
 */
size_t uint64_dict_alloc(const uint64_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief true if @p dict is NULL or contains no entries.
 */
bool is_uint64_dict_empty(const uint64_dict_t* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Print a uint64_dict_t in a JSON-like key-value format with line wrapping.
 *
 * Values are printed using uint64_t formatting.
 *
 * Example:
 *
 *     { "big": 18446744073709551615 }
 *
 * Wrapping occurs at 70 columns.
 *
 * @param dict    Pointer to dictionary. Must not be NULL.
 * @param stream  Output stream. Must not be NULL.
 *
 * @return NO_ERROR or:
 *         - NULL_POINTER
 *         - error from foreach_uint64_dict
 *
 * @note Output order is not guaranteed.
 */
error_code_t print_uint64_dict(const uint64_dict_t* dict, FILE* stream);
// ================================================================================ 
// ================================================================================ 

// ================================================================================ 
// ================================================================================ 
// uint64_matrix_t — type-safe uint64 wrapper around matrix_t
//
// This section provides a uint64-specialised interface to the generic matrix_t
// type.  uint64_matrix_t is a typedef for matrix_t; all functions below fix
// the dtype to uint64_TYPE and accept / return uint64 values directly so that
// callers never need to pass a dtype_id_t or void* value.
//
// The wrapper covers every public function in c_matrix.h that either:
//   (a) takes a dtype_id_t argument (replaced with uint64_TYPE), or
//   (b) takes or returns void* element pointers (replaced with uint64 / uint64*).
//
// Functions that are already type-agnostic (matrix_rows, matrix_cols,
// matrix_is_square, matrix_has_same_shape, etc.) are wrapped with a
// uint64_matrix_t* signature for consistency, but simply delegate.
// ================================================================================ 
// ================================================================================ 

typedef matrix_t uint64_matrix_t;

typedef struct {
    bool has_value;
    union {
        uint64_matrix_t* value;
        error_code_t    error;
    } u;
} uint64_matrix_expect_t;

/* =============================================================================
 * Comparator / Predicate Function Types
 * ========================================================================== */

/**
 * @brief Predicate function to determine semantic zero for uint64 values.
 *
 * This function defines what values should be treated as "zero" during
 * dense-to-sparse conversion operations. It is primarily used by:
 *
 * - convert_uint64_matrix_zero()
 *
 * @param value The uint64 value to evaluate.
 *
 * @return true if the value should be treated as zero, false otherwise.
 *
 * @note If NULL is passed, the default behavior is:
 *       value == 0.0f
 *
 * @example
 * Treat both 0.0 and -1.0 as zero:
 * @code{.c}
 * bool zero_or_neg_one(uint64 v) {
 *     return (v == 0u) || (v == 3u);
 * }
 * @endcode
 */
typedef bool (*uint64_zero_fn)(uint64_t value);

// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialize a dense uint64 matrix with zero-initialized storage.
 *
 * Creates a dense matrix of shape @p rows by @p cols with the dtype fixed to
 * uint64_TYPE. All elements are initialised to zero.
 *
 * @param rows     Number of matrix rows. Must be > 0.
 * @param cols     Number of matrix columns. Must be > 0.
 * @param alloc_v  Allocator for the matrix header and data buffer.
 *
 * @return uint64_matrix_expect_t with has_value true on success.
 *
 * @code{.c}
 *     allocator_vtable_t a = heap_allocator();
 *     uint64_matrix_expect_t r = init_uint64_dense_matrix(3, 4, a);
 *     if (!r.has_value) { fprintf(stderr, "%d\n", r.u.error); return; }
 *     uint64_matrix_t* m = r.u.value;
 *
 *     set_uint64_matrix(m, 0, 0, 1u);
 *     set_uint64_matrix(m, 1, 2, 5u);
 *
 *     return_uint64_matrix(m);
 * @endcode
 */
uint64_matrix_expect_t init_uint64_dense_matrix(size_t             rows,
                                              size_t             cols,
                                              allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize an empty COO sparse uint64 matrix.
 *
 * Creates a coordinate-list sparse matrix with storage for up to @p capacity
 * entries. The dtype is fixed to uint64_TYPE.
 *
 * @param rows      Number of matrix rows. Must be > 0.
 * @param cols      Number of matrix columns. Must be > 0.
 * @param capacity  Initial entry capacity. Must be > 0.
 * @param growth    If true, the COO storage may grow when full.
 * @param alloc_v   Allocator for the matrix header and COO buffers.
 *
 * @return uint64_matrix_expect_t with has_value true on success.
 *
 * @code{.c}
 *     allocator_vtable_t a = heap_allocator();
 *     uint64_matrix_expect_t r = init_uint64_coo_matrix(100, 100, 16, true, a);
 *     uint64_matrix_t* m = r.u.value;
 *
 *     push_back_uint64_coo_matrix(m, 0, 5, 3u);
 *     push_back_uint64_coo_matrix(m, 42, 99, 7u);
 *
 *     return_uint64_matrix(m);
 * @endcode
 */
uint64_matrix_expect_t init_uint64_coo_matrix(size_t             rows,
                                            size_t             cols,
                                            size_t             capacity,
                                            bool               growth,
                                            allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Release all storage owned by a uint64 matrix.
 *
 * Frees internal buffers and the matrix object itself. Passing NULL is safe.
 *
 * @param mat  Matrix to destroy, or NULL.
 */
void return_uint64_matrix(uint64_matrix_t* mat);

// ================================================================================
// Element access
// ================================================================================

/**
 * @brief Read a uint64 element at the given row and column.
 *
 * For sparse formats, missing entries are returned as 0.0f.
 *
 * @param mat  Matrix to read from.
 * @param row  Zero-based row index.
 * @param col  Zero-based column index.
 * @param out  Receives the uint64 value. Must not be NULL.
 *
 * @return NO_ERROR on success, or NULL_POINTER / INVALID_ARG / OUT_OF_BOUNDS.
 *
 * @code
 *     uint64_t v = 0u;
 *     get_uint64_matrix(m, 1, 2, &v);
 * @endcode
 */
error_code_t get_uint64_matrix(const uint64_matrix_t* mat,
                              size_t                row,
                              size_t                col,
                              uint64_t*              out);

// -------------------------------------------------------------------------------- 

/**
 * @brief Write a uint64 element at the given row and column.
 *
 * Dense and COO matrices are supported. CSR/CSC return ILLEGAL_STATE.
 *
 * @param mat    Matrix to modify.
 * @param row    Zero-based row index.
 * @param col    Zero-based column index.
 * @param value  The uint64 value to store.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 *
 * @code
 *     set_uint64_matrix(m, 0, 0, 42u);
 * @endcode
 */
error_code_t set_uint64_matrix(uint64_matrix_t* mat,
                              size_t          row,
                              size_t          col,
                              uint64_t         value);

// ================================================================================
// COO assembly helpers
// ================================================================================

/**
 * @brief Reserve additional entry capacity for a uint64 COO matrix.
 *
 * @param mat       COO matrix to grow.
 * @param capacity  Requested minimum entry capacity.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t reserve_uint64_coo_matrix(uint64_matrix_t* mat,
                                      size_t          capacity);

// -------------------------------------------------------------------------------- 

/**
 * @brief Append or overwrite a uint64 entry in a COO matrix.
 *
 * If (row, col) already exists its value is overwritten; otherwise a new
 * entry is appended. The matrix is marked unsorted after insertion.
 *
 * @param mat    COO matrix to append to.
 * @param row    Zero-based row index.
 * @param col    Zero-based column index.
 * @param value  The uint64 value to insert.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 *
 * @code
 *     push_back_uint64_coo_matrix(m, 0, 5, 3u);
 * @endcode
 */
error_code_t push_back_uint64_coo_matrix(uint64_matrix_t* mat,
                                        size_t          row,
                                        size_t          col,
                                        uint64_t         value);

// -------------------------------------------------------------------------------- 

/**
 * @brief Sort COO entries by (row, col).
 *
 * @param mat  COO matrix to sort.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t sort_uint64_coo_matrix(uint64_matrix_t* mat);

// ================================================================================
// Lifecycle / structural operations
// ================================================================================

/**
 * @brief Clear a uint64 matrix, preserving its allocated storage.
 *
 * Dense matrices are zeroed. COO matrices reset to zero logical entries.
 *
 * @param mat  Matrix to clear.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t clear_uint64_matrix(uint64_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Deep-copy a uint64 matrix.
 *
 * @param src      Matrix to copy.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return uint64_matrix_expect_t with has_value true on success.
 */
uint64_matrix_expect_t copy_uint64_matrix(const uint64_matrix_t* src,
                                        allocator_vtable_t    alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Convert a uint64 matrix to a different storage format.
 *
 * Produces a new matrix with the same logical values in the target format.
 *
 * @param src      Matrix to convert.
 * @param target   Desired destination storage format. Can be any value in 
 * ``matrix_format_t`` enum, (i.e. ``DENSE_MATRIX``, ``COO_MATRIX``, 
 * ``CSR_MATRIX``, ``CSC__MATRIX``)
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return uint64_matrix_expect_t with has_value true on success.
 *
 * @code
 *     uint64_matrix_expect_t csr =
 *         convert_uint64_matrix(dense_m, CSR_MATRIX, a);
 * @endcode
 */
uint64_matrix_expect_t convert_uint64_matrix(const uint64_matrix_t* src,
                                           matrix_format_t       target,
                                           allocator_vtable_t    alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Compute the transpose of a uint64 matrix.
 *
 * Dense stays dense, COO stays COO, CSR transposes to CSR, CSC to CSC.
 *
 * @param src      Matrix to transpose.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return uint64_matrix_expect_t with has_value true on success.
 */
uint64_matrix_expect_t transpose_uint64_matrix(const uint64_matrix_t* src,
                                             allocator_vtable_t    alloc_v);

// ================================================================================
// Fill and zero
// ================================================================================

/**
 * @brief Fill a uint64 matrix with a single repeated value.
 *
 * Nonzero fill is supported only for dense matrices. Supplying 0.0f resets
 * the matrix through clear_uint64_matrix.
 *
 * @param mat    Matrix to fill.
 * @param value  The uint64 fill value.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 *
 * @code
 *     fill_uint64_matrix(m, 1u);   // every element becomes 1.0
 *     fill_uint64_matrix(m, 0u);   // equivalent to clear
 * @endcode
 */
error_code_t fill_uint64_matrix(uint64_matrix_t* mat,
                               uint64_t         value);
// ================================================================================
// Shape and compatibility queries
// ================================================================================

/**
 * @brief Test whether two uint64 matrices have identical shape.
 */
bool uint64_matrix_has_same_shape(const uint64_matrix_t* a,
                                 const uint64_matrix_t* b);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a uint64 matrix is square.
 */
bool uint64_matrix_is_square(const uint64_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a uint64 matrix uses a sparse representation.
 */
bool uint64_matrix_is_sparse(const uint64_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a uint64 matrix is logically all zeros.
 */
bool uint64_matrix_is_zero(const uint64_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Compare two uint64 matrices for numerical equality.
 *
 * This function determines whether two matrices are logically equal based on
 * their numerical values, independent of their internal storage format.
 *
 * Equality is defined using standard uint64ing-point comparison semantics:
 *
 * Matrices must:
 * - be non-NULL
 * - have the same dimensions (rows and columns)
 * - have dtype == uint64_TYPE
 *
 * The comparison is **format-agnostic**, meaning matrices with different
 * storage formats (e.g., dense vs COO vs CSR vs CSC) are considered equal
 * if they represent the same logical values.
 *
 * Performance optimizations:
 * - Dense × Dense comparisons use SIMD-accelerated numerical comparison.
 * - COO × COO comparisons use a fast path when both matrices are sorted,
 *   comparing index arrays and values directly.
 * - All other cases fall back to element-wise comparison using logical
 *   matrix access.
 *
 * @param a First matrix (may be NULL).
 * @param b Second matrix (may be NULL).
 *
 * @return true if the matrices are numerically equal, false otherwise.
 *
 * @note This function performs numerical comparison only. For custom
 *       comparison rules (e.g., tolerance-based equality or sign-sensitive
 *       comparisons), use :c:func:`uint64_matrix_equal_cmp`.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * uint64_matrix_expect_t r1 = init_uint64_dense_matrix(2, 2, alloc);
 * uint64_matrix_expect_t r2 = init_uint64_dense_matrix(2, 2, alloc);
 *
 * uint64_matrix_t* a = r1.u.value;
 * uint64_matrix_t* b = r2.u.value;
 *
 * set_uint64_matrix(a, 0, 0, 4u);
 * set_uint64_matrix(b, 0, 0,  0u);
 *
 * assert_true(uint64_matrix_equal(a, b)); // false
 *
 * return_uint64_matrix(a);
 * return_uint64_matrix(b);
 * @endcode
 *
 * @code{.c}
 * // Equality across different storage formats
 *
 * uint64_matrix_expect_t dense_r = init_uint64_dense_matrix(2, 2, alloc);
 * uint64_matrix_t* dense = dense_r.u.value;
 *
 * set_uint64_matrix(dense, 0, 1, 3u);
 *
 * uint64_matrix_expect_t csr_r =
 *     convert_uint64_matrix(dense, CSR_MATRIX, alloc);
 *
 * uint64_matrix_t* csr = csr_r.u.value;
 *
 * // Same logical values → equal
 * assert_true(uint64_matrix_equal(dense, csr)); // true
 *
 * return_uint64_matrix(csr);
 * return_uint64_matrix(dense);
 * @endcode
 */
bool uint64_matrix_equal(const uint64_matrix_t* a,
                        const uint64_matrix_t* b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether two uint64 matrices are compatible for addition.
 *
 * Requires equal shape and equal dtype.
 */
bool uint64_matrix_is_add_compatible(const uint64_matrix_t* a,
                                    const uint64_matrix_t* b);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether two uint64 matrices are compatible for multiplication.
 *
 * Requires a->cols == b->rows and equal dtype.
 */
bool uint64_matrix_is_multiply_compatible(const uint64_matrix_t* a,
                                         const uint64_matrix_t* b);

// ================================================================================
// Row / column swaps
// ================================================================================

/**
 * @brief Swap two rows of a uint64 matrix in place.
 *
 * Dense and COO matrices are supported. CSR/CSC return OPERATION_UNAVAILABLE.
 *
 * @param mat  Matrix to modify.
 * @param r1   First zero-based row index.
 * @param r2   Second zero-based row index.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t swap_uint64_matrix_rows(uint64_matrix_t* mat,
                                    size_t          r1,
                                    size_t          r2);

// -------------------------------------------------------------------------------- 

/**
 * @brief Swap two columns of a uint64 matrix in place.
 *
 * Dense and COO matrices are supported. CSR/CSC return OPERATION_UNAVAILABLE.
 *
 * @param mat  Matrix to modify.
 * @param c1   First zero-based column index.
 * @param c2   Second zero-based column index.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t swap_uint64_matrix_cols(uint64_matrix_t* mat,
                                    size_t          c1,
                                    size_t          c2);

// ================================================================================
// Special matrix constructors
// ================================================================================

/**
 * @brief Initialize a dense uint64 identity matrix.
 *
 * Creates an @p n by @p n dense matrix with 1.0f on the diagonal and 0.0f
 * elsewhere.
 *
 * @param n        Matrix order (rows == cols).
 * @param alloc_v  Allocator for the matrix.
 *
 * @return uint64_matrix_expect_t with has_value true on success.
 *
 * @code
 *     uint64_matrix_expect_t r = init_uint64_identity_matrix(3, a);
 *     // 3x3 identity matrix.
 * @endcode
 */
uint64_matrix_expect_t init_uint64_identity_matrix(size_t             n,
                                                 allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a dense uint64 row vector (1 x length).
 *
 * @param length   Vector length (number of columns).
 * @param alloc_v  Allocator for the matrix.
 *
 * @return uint64_matrix_expect_t with has_value true on success.
 */
uint64_matrix_expect_t init_uint64_row_vector(size_t             length,
                                            allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a dense uint64 column vector (length x 1).
 *
 * @param length   Vector length (number of rows).
 * @param alloc_v  Allocator for the matrix.
 *
 * @return uint64_matrix_expect_t with has_value true on success.
 */
uint64_matrix_expect_t init_uint64_col_vector(size_t             length,
                                            allocator_vtable_t alloc_v);

// ================================================================================
// Vector shape queries
// ================================================================================

/**
 * @brief Test whether a uint64 matrix has row-vector shape (1 x N).
 */
bool uint64_matrix_is_row_vector(const uint64_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a uint64 matrix has column-vector shape (N x 1).
 */
bool uint64_matrix_is_col_vector(const uint64_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a uint64 matrix has vector shape (row or column).
 */
bool uint64_matrix_is_vector(const uint64_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Return the logical length of a vector-shaped uint64 matrix.
 *
 * For row vectors returns cols; for column vectors returns rows.
 * Non-vector matrices return 0.
 */
size_t uint64_matrix_vector_length(const uint64_matrix_t* mat);

// ================================================================================
// Conversion with semantic zero callback
// ================================================================================

/**
 * @brief Convert a uint64 matrix to a different storage format using an
 *        optional semantic zero-test.
 *
 * This function performs the same structural conversion as
 * :c:func:`convert_uint64_matrix`, but allows the caller to define what
 * uint64 values should be treated as zero during dense-to-sparse conversion.
 *
 * The @p is_zero callback is used only when:
 *   - @p src is a dense matrix, and
 *   - @p target is COO_MATRIX, CSR_MATRIX, or CSC_MATRIX.
 *
 * In all other cases, this function behaves exactly like
 * :c:func:`convert_uint64_matrix`.
 *
 * If @p is_zero is NULL, the default zero rule is:
 *
 * @code{.c}
 * value == 0.0f
 * @endcode
 *
 * @param src      Matrix to convert. Must not be NULL.
 * @param target   Desired destination storage format.
 * @param alloc_v  Allocator for the destination matrix.
 * @param is_zero  Optional semantic zero predicate used only for
 *                 dense-to-sparse conversion.
 *
 * @return uint64_matrix_expect_t with has_value true on success, or
 *         u.error describing the failure.
 *
 * Possible failure codes include:
 *   - NULL_POINTER
 *   - ILLEGAL_STATE
 *   - LENGTH_OVERFLOW
 *   - BAD_ALLOC
 *   - OUT_OF_MEMORY
 *
 * @code{.c}
 * static bool zero_or_neg_one(uint64 v) {
 *     return (v == 0u) || (v == 3u);
 * }
 *
 * uint64_matrix_expect_t r =
 *     convert_uint64_matrix_zero(dense_m, CSR_MATRIX, alloc, zero_or_neg_one);
 * @endcode
 *
 * @note This function is primarily intended for domain-specific sparse
 *       conversion rules where exact zero is not sufficient.
 */
uint64_matrix_expect_t convert_uint64_matrix_zero(const uint64_matrix_t* src,
                                                matrix_format_t       target,
                                                allocator_vtable_t    alloc_v,
                                                uint64_zero_fn         is_zero);
// -------------------------------------------------------------------------------- 

/**
 * @brief Print a uint64 matrix in a human-readable format.
 *
 * Dense matrices are printed row-by-row as nested bracketed arrays:
 *
 *     [ [ 1, 2, 3 ],
 *       [ 4, 5, 6 ] ]
 *
 * Sparse matrices (COO, CSR, CSC) are printed as a flat list of explicitly
 * stored logical nonzero entries:
 *
 *     [ (0, 0): 1, (0, 1): 2, (1, 2): 6 ]
 *
 * Dense printing does not wrap by column count; each row is printed on its
 * own output line after the first row.
 *
 * Sparse printing wraps at 70 columns. If appending the next `(row, col): value`
 * entry would exceed column 70, output continues on the next line with a
 * two-space indentation.
 *
 * Sparse output is printed in row-major logical order, independent of the
 * underlying sparse storage format.
 *
 * @param mat    Pointer to the matrix. Must not be NULL.
 * @param stream Output stream. Must not be NULL.
 *
 * @return NO_ERROR or:
 *         - NULL_POINTER
 *         - ILLEGAL_STATE
 *         - error from get_uint64_matrix
 */
error_code_t print_uint64_matrix(const uint64_matrix_t* mat, FILE* stream);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_uint64_H */
// ================================================================================
// ================================================================================
// eof
