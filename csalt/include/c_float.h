// ================================================================================
// ================================================================================
// - File:    c_float.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 06, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
#ifndef c_float_H
#define c_float_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>

#include "c_array.h"
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif 
// ================================================================================ 
// ================================================================================ 

/**
 * @brief A type-safe dynamic array of float elements.
 *
 * Wraps array_t with the dtype fixed to FLOAT_TYPE at initialization.
 * All operations enforce FLOAT_TYPE at the call site so the caller never
 * needs to pass a dtype_id_t argument. The underlying array_t is embedded
 * by value so the struct can be stack- or heap-allocated by the caller.
 *
 * Floating-point semantics:
 *   - Equality comparisons (float_array_contains, set_float_array_index
 *     return equality) use exact bit patterns — no epsilon tolerance is
 *     applied. Two NaN values do not compare equal; -0.0f and +0.0f have
 *     different bit patterns and are treated as distinct.
 *   - Sort, binary_search, binary_bracket, min, and max use the three-way
 *     comparator (va > vb) - (va < vb). NaN values produce unspecified
 *     ordering relative to normal values — callers must ensure the array
 *     contains no NaN before using these operations.
 */
typedef struct {
    array_t base; /**< Underlying generic array. Do not modify directly. */
} float_array_t;

// ================================================================================
// Expected type
// ================================================================================

/**
 * @brief Expected return type for float_array_t initialization and copy/slice.
 */
typedef struct {
    bool has_value;
    union {
        float_array_t* value;
        error_code_t   error;
    } u;
} float_array_expect_t;

// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialize a new heap-allocated float_array_t.
 *
 * Allocates both the array_t struct and its internal data buffer through
 * alloc_v. The element type is fixed to FLOAT_TYPE and the element size is
 * cached as 4 bytes (sizeof(float)). The array starts empty (len == 0) with
 * the full capacity reserved. When growth is true the data buffer will be
 * reallocated automatically whenever a push operation would exceed the current
 * capacity; when false any push beyond capacity returns CAPACITY_OVERFLOW
 * without modifying the array. The caller is responsible for calling
 * return_float_array when the array is no longer needed.
 *
 * @param capacity  Initial number of elements the buffer can hold.
 *                  Must be > 0.
 * @param growth    Pass true to allow the buffer to grow automatically on
 *                  overflow, false to use a fixed-capacity buffer.
 * @param alloc_v   Allocator vtable used for all internal memory operations.
 *                  alloc_v.allocate must not be NULL.
 *
 * @return float_array_expect_t with has_value true and u.value pointing to
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
 *     float_array_expect_t result = init_float_array(32, false, alloc);
 *     if (!result.has_value) {
 *         fprintf(stderr, "init failed: %d\n", result.u.error);
 *         return;
 *     }
 *     float_array_t* arr = result.u.value;
 *
 *     push_back_float_array(arr, -1.5f);
 *     push_back_float_array(arr,  0.0f);
 *     push_back_float_array(arr,  3.14f);
 *     // arr now contains [-1.5, 0.0, 3.14], len == 3, alloc == 32.
 *
 *     return_float_array(arr);
 * @endcode
 */
float_array_expect_t init_float_array(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v);

// --------------------------------------------------------------------------------

/**
 * @brief Release all memory owned by a float_array_t back to its allocator.
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
 *     return_float_array(arr);
 *     arr = NULL;  // Recommended to prevent accidental reuse.
 *
 *     return_float_array(NULL);  // Always safe.
 * @endcode
 */
void return_float_array(float_array_t* array);

// ================================================================================
// Push operations
// ================================================================================

/**
 * @brief Append one float to the end of the array.
 *
 * Copies value into the next available slot at index array->base.len and
 * increments len by one. If the array is full and growth is true the data
 * buffer is reallocated before the value is written; if growth is false the
 * call fails immediately with CAPACITY_OVERFLOW and the array is left
 * unmodified. This is an O(1) amortised operation when growth is true and
 * strictly O(1) when the buffer has spare capacity.
 *
 * NaN values are stored without error. Callers should avoid pushing NaN if
 * they intend to use sort, min, max, binary_search, or binary_bracket.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param value  The float value to append.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code
 *     push_back_float_array(arr, -1.0f);
 *     push_back_float_array(arr,  0.0f);
 *     push_back_float_array(arr,  1.0f);
 *     // arr contains [-1.0, 0.0, 1.0], len == 3.
 * @endcode
 */
error_code_t push_back_float_array(float_array_t* array, float value);

// --------------------------------------------------------------------------------

/**
 * @brief Prepend one float to the front of the array.
 *
 * Shifts all existing elements one position toward the back using memmove,
 * writes value into index 0, and increments len by one. If the array is full
 * and growth is true the data buffer is reallocated before the shift; if
 * growth is false the call fails immediately with CAPACITY_OVERFLOW and the
 * array is left unmodified. Because every existing element must be moved this
 * is an O(n) operation — prefer push_back_float_array for performance-sensitive
 * append-heavy workloads.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param value  The float value to prepend.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code
 *     push_back_float_array(arr,  2.0f);
 *     push_back_float_array(arr,  3.0f);
 *     push_front_float_array(arr, 1.0f);
 *     // arr contains [1.0, 2.0, 3.0].
 * @endcode
 */
error_code_t push_front_float_array(float_array_t* array, float value);

// --------------------------------------------------------------------------------

/**
 * @brief Insert one float at an arbitrary index.
 *
 * Shifts all elements at positions >= index one slot toward the back using
 * memmove, writes value at index, and increments len by one. If the array is
 * full and growth is true the data buffer is reallocated before the shift;
 * if growth is false the call fails immediately with CAPACITY_OVERFLOW. Passing
 * index == 0 is equivalent to push_front_float_array; passing index == len
 * is equivalent to push_back_float_array. This is an O(n) operation in the
 * general case.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param index  Zero-based insertion position. Must be <= array->base.len.
 * @param value  The float value to insert.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - OUT_OF_BOUNDS     if index > array->base.len
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code
 *     push_back_float_array(arr, 1.0f);
 *     push_back_float_array(arr, 3.0f);
 *     push_at_float_array(arr, 1, 2.0f);
 *     // arr contains [1.0, 2.0, 3.0].
 * @endcode
 */
error_code_t push_at_float_array(float_array_t* array,
                                 size_t         index,
                                 float          value);

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
 * @param out    Pointer to a float that receives the value. Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array or out is NULL
 *         - OUT_OF_BOUNDS if index >= array->base.len
 *
 * @code
 *     float val = 0.0f;
 *     get_float_array_index(arr, 0, &val);  // val == first element
 * @endcode
 */
error_code_t get_float_array_index(const float_array_t* array,
                                   size_t               index,
                                   float*               out);

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
 * @param out    Pointer to a float that receives the removed value, or NULL
 *               to discard it.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *
 * @code
 *     float val = 0.0f;
 *     pop_back_float_array(arr, &val);   // removes last; val holds its value
 *     pop_back_float_array(arr, NULL);   // removes last; value discarded
 * @endcode
 */
error_code_t pop_back_float_array(float_array_t* array, float* out);

// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally return the first element of the array.
 *
 * Copies the element at index 0 into *out (if out is non-NULL), then shifts
 * all remaining elements one position toward the front using memmove and
 * decrements len by one. The call is O(n).
 *
 * @param array  Pointer to the array. Must not be NULL.
 * @param out    Pointer to a float that receives the removed value, or NULL
 *               to discard it.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *
 * @code
 *     float val = 0.0f;
 *     pop_front_float_array(arr, &val);  // removes first; val holds its value
 * @endcode
 */
error_code_t pop_front_float_array(float_array_t* array, float* out);

// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally return the element at an arbitrary index.
 *
 * Copies the element at index into *out (if out is non-NULL), then shifts all
 * elements after index one position toward the front using memmove and
 * decrements len by one. Passing index == 0 is equivalent to
 * pop_front_float_array; passing index == len - 1 is equivalent to
 * pop_back_float_array. The call is O(n) in the general case.
 *
 * @param array  Pointer to the array. Must not be NULL.
 * @param out    Pointer to a float that receives the removed value, or NULL
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
 *     float val = 0.0f;
 *     pop_any_float_array(arr, &val, 2);  // removes element at index 2
 * @endcode
 */
error_code_t pop_any_float_array(float_array_t* array,
                                 float*         out,
                                 size_t         index);

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
 *     clear_float_array(arr);
 *     // float_array_size(arr) == 0, float_array_alloc(arr) unchanged.
 * @endcode
 */
error_code_t clear_float_array(float_array_t* array);

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
 * @param value  The new float value to store at index.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - OUT_OF_BOUNDS if index >= array->base.len
 *
 * @code
 *     set_float_array_index(arr, 1, -99.9f);
 * @endcode
 */
error_code_t set_float_array_index(float_array_t* array,
                                   size_t         index,
                                   float          value);

// --------------------------------------------------------------------------------

/**
 * @brief Allocate an independent deep copy of an array.
 *
 * Creates a new float_array_t with the same elements, length, capacity, dtype,
 * and growth flag as src. Both the new struct and its data buffer are allocated
 * through alloc_v. Subsequent modifications to either array do not affect the
 * other. The caller is responsible for calling return_float_array on the
 * returned array when it is no longer needed.
 *
 * @param src     Pointer to the source array. Must not be NULL.
 * @param alloc_v Allocator vtable for the new array. alloc_v.allocate must
 *                not be NULL.
 *
 * @return float_array_expect_t with has_value true and u.value pointing to
 *         the new array on success. On failure has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if src is NULL
 *         - BAD_ALLOC     if the struct allocation fails
 *         - OUT_OF_MEMORY if the data buffer allocation fails
 *
 * @code
 *     float_array_expect_t cr = copy_float_array(src, alloc);
 *     if (!cr.has_value) { return; }
 *     float_array_t* dst = cr.u.value;
 *     return_float_array(dst);
 * @endcode
 */
float_array_expect_t copy_float_array(const float_array_t* src,
                                      allocator_vtable_t   alloc_v);

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
 *     concat_float_array(dst, src);
 *     // dst now ends with all elements that were in src.
 * @endcode
 */
error_code_t concat_float_array(float_array_t*       dst,
                                const float_array_t* src);

// --------------------------------------------------------------------------------

/**
 * @brief Allocate a new array containing a sub-range of src.
 *
 * Copies the elements at indices [start, end) from src into a freshly
 * allocated float_array_t of capacity end - start. The source array is not
 * modified. The caller is responsible for calling return_float_array on the
 * returned array.
 *
 * @param src     Pointer to the source array. Must not be NULL.
 * @param start   Zero-based index of the first element to include (inclusive).
 * @param end     Zero-based index one past the last element to include
 *                (exclusive). Must be > start and <= src->base.len.
 * @param alloc_v Allocator for the new array.
 *
 * @return float_array_expect_t with has_value true and u.value pointing to
 *         the new array on success. On failure has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if src is NULL
 *         - INVALID_ARG   if start >= end
 *         - OUT_OF_BOUNDS if end > src->base.len
 *         - BAD_ALLOC     if the struct allocation fails
 *         - OUT_OF_MEMORY if the data buffer allocation fails
 *
 * @code
 *     // src: [-2.0, -1.0, 0.0, 1.0, 2.0]
 *     float_array_expect_t sr = slice_float_array(src, 1, 4, alloc);
 *     // sr.u.value contains [-1.0, 0.0, 1.0].
 *     return_float_array(sr.u.value);
 * @endcode
 */
float_array_expect_t slice_float_array(const float_array_t* src,
                                       size_t               start,
                                       size_t               end,
                                       allocator_vtable_t   alloc_v);

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
 *     // arr: [-3.0, -2.0, -1.0, 0.0, 1.0]
 *     reverse_float_array(arr);
 *     // arr: [1.0, 0.0, -1.0, -2.0, -3.0]
 * @endcode
 */
error_code_t reverse_float_array(float_array_t* array);

// ================================================================================
// Sort
// ================================================================================

/**
 * @brief Sort all elements in place in ascending or descending order.
 *
 * Uses the generic sort_array path with a floating-point comparator. The
 * comparator uses the three-way idiom (va > vb) - (va < vb) which is
 * well-defined for finite values and signed zeros. NaN values produce
 * unspecified ordering relative to any other value — callers must ensure
 * the array contains no NaN before sorting. The sort is not guaranteed to
 * be stable. The call is O(n log n) on average.
 *
 * Note: -0.0f and +0.0f compare equal under IEEE 754 and may appear in
 * either relative order after a stable-equivalent sort.
 *
 * @param array  Pointer to the array to sort. Must not be NULL.
 * @param dir    FORWARD for ascending (most-negative first), REVERSE for
 *               descending.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *
 * @code
 *     // arr: [3.0, -1.5, 2.5]
 *     sort_float_array(arr, FORWARD);
 *     // arr: [-1.5, 2.5, 3.0]
 * @endcode
 */
error_code_t sort_float_array(float_array_t* array, direction_t dir);

// ================================================================================
// Reduction
// ================================================================================

/**
 * @brief Find the minimum value in the array.
 *
 * Scans all elements using a floating-point comparison and returns the
 * smallest value found. When two or more elements compare equal the value
 * of the first occurrence is returned.
 *
 * NaN: the result is unspecified if the array contains any NaN. Callers
 * must ensure the array contains only finite values or infinities.
 *
 * Signed zero: -0.0f and +0.0f are equal under IEEE 754. If only one
 * signed-zero variant appears in the array it will be returned; if both
 * appear the one at the lower index is returned.
 *
 * @param array  Pointer to the array to scan. Must not be NULL and must
 *               contain at least one element.
 *
 * @return float_expect_t with has_value true and u.value == the minimum
 *         element on success. On failure, has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if array->base.len == 0
 *
 * @code
 *     push_back_float_array(arr,  3.0f);
 *     push_back_float_array(arr, -1.5f);
 *     push_back_float_array(arr,  2.5f);
 *
 *     float_expect_t r = float_array_min(arr);
 *     // r.has_value == true, r.u.value == -1.5f.
 * @endcode
 */
float_expect_t float_array_min(const float_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Find the maximum value in the array.
 *
 * Scans all elements using a floating-point comparison and returns the
 * largest value found. When two or more elements compare equal the value
 * of the first occurrence is returned.
 *
 * NaN: the result is unspecified if the array contains any NaN. Callers
 * must ensure the array contains only finite values or infinities.
 *
 * @param array  Pointer to the array to scan. Must not be NULL and must
 *               contain at least one element.
 *
 * @return float_expect_t with has_value true and u.value == the maximum
 *         element on success. On failure, has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if array->base.len == 0
 *
 * @code
 *     float_expect_t r = float_array_max(arr);
 *     // r.has_value == true, r.u.value == 3.0f.
 * @endcode
 */
float_expect_t float_array_max(const float_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Sum all elements and return the result as a float.
 *
 * Iterates over every element and accumulates the total in a float. The
 * result is subject to the usual IEEE 754 rounding: for large arrays with
 * widely varying magnitudes, consider using Kahan summation externally if
 * accuracy is critical.
 *
 * NaN propagation: if any element is NaN the sum will be NaN.
 *
 * @param array  Pointer to the array to sum. Must not be NULL.
 *
 * @return float_expect_t with has_value true and u.value == the sum on
 *         success. On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if array->base.len == 0
 *
 * @code
 *     push_back_float_array(arr,  1.0f);
 *     push_back_float_array(arr, -2.5f);
 *     push_back_float_array(arr,  3.5f);
 *
 *     float_expect_t r = float_array_sum(arr);
 *     // r.has_value == true, r.u.value == 2.0f.
 * @endcode
 */
float_expect_t float_array_sum(const float_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Compute the prefix sum of a float array into a new float array.
 *
 * Allocates a new float_array_t of the same length as @p src and fills it
 * with the inclusive prefix sum: output[0] == src[0], and output[i] ==
 * output[i-1] + src[i] for every subsequent index. @p src is not modified.
 *
 * IEEE 754 rounding accumulates with each addition, so the result may
 * differ from a mathematically exact prefix sum for large arrays. NaN
 * propagates from the first NaN element to all subsequent output elements.
 *
 * The output array is owned by the caller and must be freed with
 * return_float_array when no longer needed.
 *
 * @param src    Source array to scan. Must not be NULL and must contain at
 *               least one element.
 * @param alloc  Allocator used for the output array.
 *
 * @return float_array_expect_t with has_value true and u.value pointing to
 *         the newly allocated output array on success. On failure, has_value
 *         is false and u.error is one of:
 *         - NULL_POINTER  if src is NULL
 *         - EMPTY         if src contains no elements
 *         - BAD_ALLOC     if the output struct allocation failed
 *         - OUT_OF_MEMORY if the output data buffer allocation failed
 *
 * @code
 *     push_back_float_array(src,  1.0f);
 *     push_back_float_array(src, -2.0f);
 *     push_back_float_array(src,  3.0f);
 *     push_back_float_array(src, -1.0f);
 *     // src contains [1.0, -2.0, 3.0, -1.0].
 *
 *     float_array_expect_t cr = cumulative_float_array(src, alloc);
 *     // cr.u.value contains [1.0, -1.0, 2.0, 1.0].
 *
 *     return_float_array(src);
 *     return_float_array(cr.u.value);
 * @endcode
 */
float_array_expect_t cumulative_float_array(const float_array_t* src,
                                            allocator_vtable_t   alloc);

// ================================================================================
// Search
// ================================================================================

/**
 * @brief Search for the first element matching value in the range [start, end).
 *
 * Dispatches to the best available SIMD implementation selected at compile
 * time.  The comparison mode is determined by abs_tol and rel_tol:
 *
 *   abs_tol == 0.0f && rel_tol == 0.0f  — exact bit-pattern match.
 *       Identical semantics to memcmp: NaN != NaN, -0.0f != +0.0f.
 *
 *   abs_tol >  0.0f && rel_tol == 0.0f  — absolute tolerance only.
 *       Match when |element - value| <= abs_tol.
 *
 *   abs_tol == 0.0f && rel_tol >  0.0f  — relative tolerance only.
 *       Match when |element - value| <= rel_tol * max(|element|, |value|).
 *       Near zero the threshold collapses to zero; use absolute tolerance
 *       instead if values may be close to zero.
 *
 *   abs_tol >  0.0f && rel_tol >  0.0f  — combined (Python math.isclose style).
 *       Match when |element - value| <= max(abs_tol,
 *                                           rel_tol * max(|element|, |value|)).
 *       abs_tol acts as a floor so the threshold never collapses near zero.
 *
 * Negative values for either tolerance are rejected with INVALID_ARG.
 *
 * @param array    Pointer to the array to search. Must not be NULL.
 * @param value    The float value to search for.
 * @param start    Zero-based index of the first element to examine (inclusive).
 * @param end      Zero-based index one past the last element to examine
 *                 (exclusive). Must be > start and <= array->base.len.
 * @param abs_tol  Absolute tolerance >= 0.0f.  Pass 0.0f to disable.
 * @param rel_tol  Relative tolerance >= 0.0f.  Pass 0.0f to disable.
 *
 * @return size_expect_t with has_value true and u.value holding the zero-based
 *         index of the first match on success. On failure has_value is false
 *         and u.error is one of:
 *         - NULL_POINTER  if array is NULL
 *         - INVALID_ARG   if start >= end, abs_tol < 0.0f, or rel_tol < 0.0f
 *         - OUT_OF_BOUNDS if end > array->base.len
 *         - NOT_FOUND     if no element in [start, end) satisfies the match
 *
 * @code
 *     // Exact match:
 *     size_expect_t r = float_array_contains(arr, 2.5f, 0, 4, 0.0f, 0.0f);
 *
 *     // Absolute tolerance — find first element within ±0.01 of 1.0:
 *     r = float_array_contains(arr, 1.0f, 0, n, 0.01f, 0.0f);
 *
 *     // Relative tolerance — find first element within 1% of 1000.0:
 *     r = float_array_contains(arr, 1000.0f, 0, n, 0.0f, 0.01f);
 *
 *     // Combined — absolute floor of 1e-6, relative tolerance of 1e-4:
 *     r = float_array_contains(arr, val, 0, n, 1e-6f, 1e-4f);
 * @endcode
 */
size_expect_t float_array_contains(const float_array_t* array,
                                   float                value,
                                   size_t               start,
                                   size_t               end,
                                   float                abs_tol,
                                   float                rel_tol);
// --------------------------------------------------------------------------------

/**
 * @brief Binary-search for the first element within tolerance of value.
 *
 * Dispatches to the best available SIMD implementation selected at compile
 * time.  The search proceeds in two phases:
 *
 *   1. Bisection — a standard O(log n) binary search using the exact
 *      three-way float comparator finds the landing position closest to
 *      needle.  The sort contract is unchanged: the array must be sorted
 *      (or sort must be true) using the same exact comparator.
 *
 *   2. Tolerance fan-out — from the landing position the scan walks left
 *      and right, stopping as soon as an element falls outside tolerance.
 *      Because the array is sorted, all elements within tolerance form a
 *      contiguous run, so early termination is always correct.  The
 *      lowest-index match found is returned.
 *
 * Tolerance modes (same sentinel convention as float_array_contains):
 *
 *   abs_tol == 0.0f && rel_tol == 0.0f  — exact match only.
 *       The tolerance test reduces to |diff| <= 0.0f, i.e. diff == 0.0f.
 *       Under the exact comparator -0.0f == +0.0f, so either is returned
 *       if both are present.
 *
 *   abs_tol >  0.0f && rel_tol == 0.0f  — absolute tolerance only.
 *       Match when |element - value| <= abs_tol.
 *
 *   abs_tol == 0.0f && rel_tol >  0.0f  — relative tolerance only.
 *       Match when |element - value| <= rel_tol * max(|element|, |value|).
 *
 *   abs_tol >  0.0f && rel_tol >  0.0f  — combined.
 *       Match when |element - value| <= max(abs_tol,
 *                                           rel_tol * max(|element|, |value|)).
 *
 * NaN: callers must ensure the array contains no NaN. NaN values produce
 * unspecified bisection behaviour and unspecified tolerance results.
 *
 * @param array    Pointer to the array to search. Must not be NULL.
 * @param value    The float value to find. Must not be NaN.
 * @param sort     Pass true to sort the array in ascending order before
 *                 searching; the sorted order persists after the call.
 *                 Pass false if the array is already sorted.
 * @param abs_tol  Absolute tolerance >= 0.0f. Pass 0.0f to disable.
 * @param rel_tol  Relative tolerance >= 0.0f. Pass 0.0f to disable.
 *
 * @return size_expect_t with has_value true and u.value equal to the
 *         zero-based index of the first (lowest-index) element within
 *         tolerance of value on success. On failure has_value is false
 *         and u.error is one of:
 *         - NULL_POINTER  if array is NULL
 *         - INVALID_ARG   if abs_tol < 0.0f or rel_tol < 0.0f
 *         - EMPTY         if the array contains no elements
 *         - NOT_FOUND     if no element is within tolerance of value
 *
 * @code
 *     allocator_vtable_t alloc = heap_allocator();
 *     float_array_expect_t r = init_float_array(8, false, alloc);
 *     float_array_t* arr = r.u.value;
 *
 *     push_back_float_array(arr, -1.5f);
 *     push_back_float_array(arr,  2.5f);
 *     push_back_float_array(arr,  2.5f);   // duplicate
 *     push_back_float_array(arr,  3.0f);
 *
 *     // Exact match (arr already sorted):
 *     size_expect_t sr = float_array_binary_search(arr, 2.5f, false, 0.0f, 0.0f);
 *     // sr.u.value == 1  (first of the two 2.5 entries)
 *
 *     // Absolute tolerance — finds 2.5 as closest to 2.6 within ±0.2:
 *     sr = float_array_binary_search(arr, 2.6f, false, 0.2f, 0.0f);
 *     // sr.u.value == 1
 *
 *     // No match:
 *     sr = float_array_binary_search(arr, 99.0f, false, 0.0f, 0.0f);
 *     // sr.has_value == false, sr.u.error == NOT_FOUND
 *
 *     return_float_array(arr);
 * @endcode
 */
size_expect_t float_array_binary_search(float_array_t* array,
                                        float          value,
                                        bool           sort,
                                        float          abs_tol,
                                        float          rel_tol);
// --------------------------------------------------------------------------------

/**
 * @brief Find the bracketing indices for a value in a sorted array.
 *
 * If sort is true the array is sorted in ascending order before the search.
 * Returns the lower bound (first index whose element >= value) and the upper
 * bound (last index whose element <= value). When value exists exactly in the
 * array lower == upper and both point to the first occurrence. NaN values
 * produce undefined results.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param value  The float value to bracket. Must not be NaN.
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
 *     // arr: [-2.0, -1.0, 1.0, 2.0]
 *     bracket_expect_t r = float_array_binary_bracket(arr, -1.0f, false);
 *     // r.u.value.lower == 1, r.u.value.upper == 1  (exact match).
 *
 *     r = float_array_binary_bracket(arr, 0.0f, false);
 *     // r.u.value.lower == 2  (1.0), r.u.value.upper == 1  (-1.0).
 * @endcode
 */
bracket_expect_t float_array_binary_bracket(float_array_t* array,
                                            float          value,
                                            bool           sort);

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
 *     push_back_float_array(arr, 1.0f);
 *     push_back_float_array(arr, 2.0f);
 *     // float_array_size(arr) == 2.
 * @endcode
 */
size_t float_array_size(const float_array_t* array);

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
 *     // float_array_alloc(arr) == 8 after init with capacity 8.
 * @endcode
 */
size_t float_array_alloc(const float_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return the size in bytes of one element in the array.
 *
 * Reads array->base.data_size, which is fixed to 4 for all float_array_t
 * instances — a float is always four bytes on IEEE 754 platforms. The call
 * is O(1) and does not modify the array.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return 4 when array is non-NULL, or 0 if array is NULL.
 *
 * @code
 *     size_t elem_bytes = float_array_data_size(arr);  // always 4
 * @endcode
 */
size_t float_array_data_size(const float_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if the array currently contains no elements.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return true if array is NULL or array->base.len == 0, false otherwise.
 *
 * @code
 *     // is_float_array_empty(arr) == true immediately after init.
 *     push_back_float_array(arr, 1.0f);
 *     // is_float_array_empty(arr) == false.
 * @endcode
 */
bool is_float_array_empty(const float_array_t* array);

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
 *     push_back_float_array(arr, 1.0f);
 *     push_back_float_array(arr, 2.0f);
 *     // is_float_array_full(arr) == false (one slot remains).
 *
 *     push_back_float_array(arr, 3.0f);
 *     // is_float_array_full(arr) == true, size == alloc == 3.
 * @endcode
 */
bool is_float_array_full(const float_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if ptr points to a valid element within the live region
 *        of the array's data buffer.
 *
 * A pointer is considered valid if array and ptr are both non-NULL,
 * ptr >= array->base.data, and ptr < array->base.data + array->base.len * 4.
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
 *     const float* first = (const float*)arr->base.data;
 *     is_float_array_ptr(arr, first);       // true
 *     is_float_array_ptr(arr, first + 2);   // true if len >= 3
 *     is_float_array_ptr(arr, first + len); // false — spare capacity
 *     is_float_array_ptr(arr, NULL);        // false
 * @endcode
 */
bool is_float_array_ptr(const float_array_t* array, const float* ptr);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_float_H */
// ================================================================================
// ================================================================================
// eof
