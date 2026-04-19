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
// -------------------------------------------------------------------------------- 

/**
 * @brief Print a float array in bracketed form with line wrapping.
 *
 * Prints the array in the form:
 *
 *     [ 1.000, 5.000, 2.000, 3.000, 6.000 ]
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
 *     print_float_array(arr, stdout);
 * @endcode
 */
error_code_t print_float_array(const float_array_t* array, FILE* stream);
// -------------------------------------------------------------------------------- 

/**
 * @brief Compare two float arrays for exact numeric equality using a
 *        SIMD-accelerated path.
 *
 * Two arrays are considered equal if:
 *   1. Both pointers are non-NULL.
 *   2. They contain the same number of elements (len).
 *   3. Every element compares equal using IEEE floating-point equality.
 *
 * This function uses a SIMD-accelerated implementation when available
 * (e.g., AVX, SSE, NEON) to compare elements in blocks, with a scalar
 * fallback for any remaining elements.
 *
 * Equality follows normal floating-point comparison rules:
 *   - +0.0f and -0.0f compare equal
 *   - NaN does not compare equal to any value, including itself
 *
 * This function does NOT compare allocator state, capacity, or any
 * metadata beyond element count and data values.
 *
 * @param a  Pointer to the first float array.
 * @param b  Pointer to the second float array.
 *
 * @return true if both arrays are equal, false otherwise.
 */
bool float_array_equal(const float_array_t* a,
                       const float_array_t* b);
// ================================================================================ 
// ================================================================================ 

/**
 * @brief A dictionary that maps null-terminated C-string keys to float_t values.
 *
 * This is a typedef alias for @ref dict_t with the convention that
 * @c data_size is always @c sizeof(float_t) and @c dtype is always
 * @c FLOAT_TYPE.  Do not use the raw @ref dict_t API directly on a
 * @c float_dict_t — always go through the functions declared in this header
 * so that the type constraints are enforced.
 *
 * @code
 *     allocator_vtable_t a = heap_allocator();
 *     float_dict_expect_t r = init_float_dict(16, true, a);
 *     if (!r.has_value) { // handle r.u.error }
 *     float_dict_t* d = r.u.value;
 *
 *     insert_float_dict(d, "width",  1920u, a);
 *     insert_float_dict(d, "height", 1080u, a);
 *     insert_float_dict(d, "depth",    32u, a);
 *
 *     float_t v;
 *     get_float_dict_value(d, "width", &v);   // v == 1920
 *
 *     return_float_dict(d);
 * @endcode
 */
typedef dict_t float_dict_t;
 
/**
 * @brief Expected return type for @ref init_float_dict and
 *        @ref copy_float_dict.
 *
 * Check @c has_value before using @c u.value.  On failure @c u.error
 * contains the relevant @ref error_code_t.
 */
typedef struct {
    bool has_value;
    union {
        float_dict_t* value;
        error_code_t   error;
    } u;
} float_dict_expect_t;
 
// ================================================================================
// Iterator type
// ================================================================================
 
/**
 * @brief Typed iterator callback for @ref foreach_float_dict.
 *
 * @param key       Null-terminated C-string key (points into the dict's
 *                  internal storage — do not free or store beyond the
 *                  callback).
 * @param key_len   Length of @p key in bytes, excluding the null terminator.
 * @param value     The float_t value associated with @p key.
 * @param user_data Caller-supplied context pointer passed unchanged from
 *                  @ref foreach_float_dict; may be NULL.
 *
 * @code
 *     static void print_entry(const char* key, size_t key_len,
 *                             float_t value, void* ud) {
 *         (void)ud;
 *         printf("  %.*s => %u\n", (int)key_len, key, (unsigned)value);
 *     }
 *     foreach_float_dict(d, print_entry, NULL);
 * @endcode
 */
typedef void (*float_dict_iter_fn)(const char* key,
                                    size_t      key_len,
                                    float_t    value,
                                    void*       user_data);
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
/**
 * @brief Allocate and initialise a new float_dict_t.
 *
 * The underlying @ref dict_t is created with @c data_size = sizeof(float_t)
 * and @c dtype = FLOAT_TYPE.  The bucket count is rounded up to the next
 * power of two internally.
 *
 * @param capacity  Initial bucket count.  Must be > 0.
 * @param growth    If true the table resizes automatically when the load
 *                  factor exceeds 0.75.
 * @param alloc_v   Allocator for all internal memory.  @c alloc_v.allocate
 *                  must not be NULL.
 *
 * @return @ref float_dict_expect_t with @c has_value true on success.
 *         On failure @c u.error is one of NULL_POINTER, INVALID_ARG, or
 *         OUT_OF_MEMORY.
 *
 * @code
 *     allocator_vtable_t a = heap_allocator();
 *
 *     float_dict_expect_t r = init_float_dict(8, true, a);
 *     if (!r.has_value) {
 *         fprintf(stderr, "init failed: %d\n", r.u.error);
 *         return;
 *     }
 *     float_dict_t* d = r.u.value;
 *
 *     insert_float_dict(d, "port",  8080u, a);
 *     insert_float_dict(d, "range", 1024u, a);
 *
 *     return_float_dict(d);
 * @endcode
 */
float_dict_expect_t init_float_dict(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Release all memory owned by a float_dict_t.
 *
 * Frees every node (key copy + value buffer), the bucket array, and the
 * dict struct itself.  Passing NULL is safe and performs no action.
 *
 * @param dict  Dictionary to release.
 *
 * @code
 *     float_dict_t* d = init_float_dict(8, true, a).u.value;
 *     insert_float_dict(d, "x", 1000u, a);
 *     // ... use the dict ...
 *     return_float_dict(d);   // d must not be used after this point
 * @endcode
 */
void return_float_dict(float_dict_t* dict);
 
// ================================================================================
// Insert
// ================================================================================
 
/**
 * @brief Insert a null-terminated string key with a float_t value.
 *
 * The key length is measured with @c strlen(key).  The key bytes are copied
 * into the dict's own storage — the caller may free or reuse @p key
 * immediately after this call returns.
 *
 * If the key already exists the insertion is rejected and INVALID_ARG is
 * returned.  To overwrite an existing value use @ref update_float_dict.
 *
 * @param dict    Must not be NULL.
 * @param key     Null-terminated C-string key.  Must not be NULL.
 * @param value   The float_t value to store.
 * @param alloc_v Allocator used to allocate the new node.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG (duplicate key),
 *         CAPACITY_OVERFLOW (growth == false and table is full), or
 *         OUT_OF_MEMORY.
 *
 * @code
 *     float_dict_t* d = init_float_dict(8, true, a).u.value;
 *
 *     insert_float_dict(d, "width",  1920u, a);   // OK
 *     insert_float_dict(d, "height", 1080u, a);   // OK
 *
 *     // Duplicate key — returns INVALID_ARG, dict is unchanged
 *     error_code_t err = insert_float_dict(d, "width", 800u, a);
 *     assert(err == INVALID_ARG);
 *
 *     return_float_dict(d);
 * @endcode
 */
error_code_t insert_float_dict(float_dict_t*     dict,
                                 const char*        key,
                                 float_t           value,
                                 allocator_vtable_t alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Insert a bounded key (explicit length) with a float_t value.
 *
 * Identical to @ref insert_float_dict but the key length is supplied by the
 * caller rather than measured with @c strlen.  Useful when the key is a
 * sub-string of a larger buffer or when the length is already known.
 *
 * @param dict    Must not be NULL.
 * @param key     Pointer to the first byte of the key.  Must not be NULL.
 * @param key_len Number of bytes in the key.  Must be > 0.
 * @param value   The float_t value to store.
 * @param alloc_v Allocator used to allocate the new node.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, CAPACITY_OVERFLOW, or
 *         OUT_OF_MEMORY.
 *
 * @code
 *     // Key is a sub-string: "width_max", take only "width" (5 bytes)
 *     const char* buf = "width_max";
 *     insert_float_dict_n(d, buf, 5, 1920u, a);
 *
 *     float_t v;
 *     get_float_dict_value_n(d, buf, 5, &v);   // v == 1920
 * @endcode
 */
error_code_t insert_float_dict_n(float_dict_t*     dict,
                                   const char*        key,
                                   size_t             key_len,
                                   float_t           value,
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
 *     float_dict_t* d = init_float_dict(8, true, a).u.value;
 *     insert_float_dict(d, "port", 8080u, a);
 *
 *     float_t removed;
 *     error_code_t err = pop_float_dict(d, "port", &removed);
 *     assert(err == NO_ERROR && removed == 8080u);
 *
 *     // Key is gone — next pop returns NOT_FOUND
 *     err = pop_float_dict(d, "port", NULL);
 *     assert(err == NOT_FOUND);
 *
 *     return_float_dict(d);
 * @endcode
 */
error_code_t pop_float_dict(float_dict_t* dict,
                              const char*    key,
                              float_t*      out_value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Remove the entry for a bounded key and return its value.
 *
 * Identical to @ref pop_float_dict but the key length is supplied explicitly.
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
 *     insert_float_dict_n(d, buf, 4, 8080u, a);   // key = "port"
 *
 *     float_t v;
 *     pop_float_dict_n(d, buf, 4, &v);             // removes "port", v == 8080
 * @endcode
 */
error_code_t pop_float_dict_n(float_dict_t* dict,
                                const char*    key,
                                size_t         key_len,
                                float_t*      out_value);
 
// ================================================================================
// Update
// ================================================================================
 
/**
 * @brief Overwrite the value of an existing null-terminated key.
 *
 * No allocation is performed.  If the key does not exist NOT_FOUND is
 * returned and the dict is unchanged.  To insert a new entry use
 * @ref insert_float_dict.
 *
 * @param dict   Must not be NULL.
 * @param key    Null-terminated C-string key.  Must not be NULL.
 * @param value  New float_t value to store.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND.
 *
 * @code
 *     float_dict_t* d = init_float_dict(8, true, a).u.value;
 *     insert_float_dict(d, "port", 8080u, a);
 *
 *     update_float_dict(d, "port", 443u);
 *
 *     float_t v;
 *     get_float_dict_value(d, "port", &v);
 *     assert(v == 443u);
 *
 *     return_float_dict(d);
 * @endcode
 */
error_code_t update_float_dict(float_dict_t* dict,
                                 const char*    key,
                                 float_t       value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Overwrite the value of an existing bounded key.
 *
 * Identical to @ref update_float_dict but the key length is supplied
 * explicitly.
 *
 * @param dict     Must not be NULL.
 * @param key      Pointer to the key bytes.  Must not be NULL.
 * @param key_len  Number of bytes in the key.  Must be > 0.
 * @param value    New float_t value to store.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, or NOT_FOUND.
 *
 * @code
 *     const char* buf = "port_http";
 *     insert_float_dict_n(d, buf, 4, 8080u, a);   // key = "port"
 *     update_float_dict_n(d, buf, 4, 443u);        // update "port" to 443
 * @endcode
 */
error_code_t update_float_dict_n(float_dict_t* dict,
                                   const char*    key,
                                   size_t         key_len,
                                   float_t       value);
 
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
 *     float_dict_t* d = init_float_dict(8, true, a).u.value;
 *     insert_float_dict(d, "width", 1920u, a);
 *
 *     float_t v;
 *     if (get_float_dict_value(d, "width", &v) == NO_ERROR)
 *         printf("width = %u\n", (unsigned)v);
 *
 *     return_float_dict(d);
 * @endcode
 */
error_code_t get_float_dict_value(const float_dict_t* dict,
                                    const char*          key,
                                    float_t*            out_value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Copy the value for a bounded key into a caller-supplied variable.
 *
 * Identical to @ref get_float_dict_value but the key length is supplied
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
 *     insert_float_dict_n(d, buf, 5, 1920u, a);   // key = "width"
 *
 *     float_t v;
 *     get_float_dict_value_n(d, buf, 5, &v);       // v == 1920
 * @endcode
 */
error_code_t get_float_dict_value_n(const float_dict_t* dict,
                                      const char*          key,
                                      size_t               key_len,
                                      float_t*            out_value);
 
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
 * @return Pointer to the stored float_t on success, NULL if not found or on
 *         error.
 *
 * @code
 *     insert_float_dict(d, "port", 8080u, a);
 *
 *     const float_t* p = get_float_dict_ptr(d, "port");
 *     if (p) printf("port = %u\n", (unsigned)*p);
 *     // Do not store p — it may be invalidated by the next insert or pop.
 * @endcode
 */
const float_t* get_float_dict_ptr(const float_dict_t* dict, const char* key);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Return a read-only pointer directly into the node's value buffer for
 *        a bounded key.
 *
 * Identical to @ref get_float_dict_ptr but the key length is supplied
 * explicitly.
 *
 * @param dict     Must not be NULL.
 * @param key      Pointer to the key bytes.  Must not be NULL.
 * @param key_len  Number of bytes in the key.  Must be > 0.
 *
 * @return Pointer to the stored float_t on success, NULL if not found or on
 *         error.
 *
 * @code
 *     const char* buf = "port_http";
 *     insert_float_dict_n(d, buf, 4, 8080u, a);   // key = "port"
 *
 *     const float_t* p = get_float_dict_ptr_n(d, buf, 4);
 *     if (p) printf("port = %u\n", (unsigned)*p);
 * @endcode
 */
const float_t* get_float_dict_ptr_n(const float_dict_t* dict,
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
 *     insert_float_dict(d, "active", 1u, a);
 *
 *     if (has_float_dict_key(d, "active"))
 *         printf("active is set\n");
 * @endcode
 */
bool has_float_dict_key(const float_dict_t* dict, const char* key);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Test whether a bounded key exists in the dict.
 *
 * Identical to @ref has_float_dict_key but the key length is supplied
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
 *     insert_float_dict_n(d, buf, 6, 1u, a);   // key = "active"
 *     assert(has_float_dict_key_n(d, buf, 6));
 * @endcode
 */
bool has_float_dict_key_n(const float_dict_t* dict,
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
 *     insert_float_dict(d, "a", 100u, a);
 *     insert_float_dict(d, "b", 200u, a);
 *     assert(float_dict_hash_size(d) == 2);
 *
 *     clear_float_dict(d);
 *     assert(float_dict_hash_size(d) == 0);
 *
 *     // The dict is still usable after clear
 *     insert_float_dict(d, "c", 300u, a);
 * @endcode
 */
error_code_t clear_float_dict(float_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Allocate a deep copy of a float_dict_t.
 *
 * All nodes are copied into a fresh bucket array of the same capacity.  The
 * new dict uses @p alloc_v for all of its allocations.
 *
 * @param src     Must not be NULL.
 * @param alloc_v Allocator for the new dict and its nodes.
 *
 * @return @ref float_dict_expect_t with @c has_value true on success.
 *
 * @code
 *     float_dict_t* orig = init_float_dict(8, true, a).u.value;
 *     insert_float_dict(orig, "x", 1000u, a);
 *     insert_float_dict(orig, "y", 2000u, a);
 *
 *     float_dict_expect_t cr = copy_float_dict(orig, a);
 *     assert(cr.has_value);
 *     float_dict_t* copy = cr.u.value;
 *
 *     // The copy is independent — mutating orig does not affect copy
 *     update_float_dict(orig, "x", 9999u);
 *     float_t v;
 *     get_float_dict_value(copy, "x", &v);
 *     assert(v == 1000u);
 *
 *     return_float_dict(copy);
 *     return_float_dict(orig);
 * @endcode
 */
float_dict_expect_t copy_float_dict(const float_dict_t* src,
                                      allocator_vtable_t   alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Merge two float_dict_t instances into a new dict.
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
 * @return @ref float_dict_expect_t with @c has_value true on success.
 *
 * @code
 *     float_dict_t* da = init_float_dict(8, true, a).u.value;
 *     float_dict_t* db = init_float_dict(8, true, a).u.value;
 *
 *     insert_float_dict(da, "shared", 100u, a);
 *     insert_float_dict(da, "only_a",   1u, a);
 *     insert_float_dict(db, "shared", 999u, a);
 *     insert_float_dict(db, "only_b",   2u, a);
 *
 *     // Merge without overwrite — "shared" keeps da's value of 100
 *     float_dict_expect_t mr = merge_float_dict(da, db, false, a);
 *     float_t v;
 *     get_float_dict_value(mr.u.value, "shared", &v);
 *     assert(v == 100u);
 *
 *     return_float_dict(mr.u.value);
 *     return_float_dict(da);
 *     return_float_dict(db);
 * @endcode
 */
float_dict_expect_t merge_float_dict(const float_dict_t* a,
                                       const float_dict_t* b,
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
 * C-string pointer (into internal storage), its length, and the float_t
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
 *                             float_t value, void* ud) {
 *         (void)key_len; (void)ud;
 *         printf("  %s = %f\n", key, (double)value);
 *     }
 *
 *     float_dict_t* d = init_float_dict(8, true, a).u.value;
 *     insert_float_dict(d, "width",  1920u, a);
 *     insert_float_dict(d, "height", 1080u, a);
 *
 *     foreach_float_dict(d, print_entry, NULL);
 *     // Output (order may vary):
 *     //   width = 1920
 *     //   height = 1080
 *
 *     return_float_dict(d);
 * @endcode
 */
error_code_t foreach_float_dict(const float_dict_t* dict,
                                  float_dict_iter_fn  fn,
                                  void*                user_data);
 
// ================================================================================
// Introspection
// ================================================================================
 
/**
 * @brief Number of occupied buckets (chains with at least one entry).
 *
 * Returns 0 if @p dict is NULL.
 */
size_t float_dict_size(const float_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Total number of key-value pairs stored.
 *
 * Returns 0 if @p dict is NULL.
 */
size_t float_dict_hash_size(const float_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Number of buckets currently allocated.
 *
 * Returns 0 if @p dict is NULL.
 */
size_t float_dict_alloc(const float_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief true if @p dict is NULL or contains no entries.
 */
bool is_float_dict_empty(const float_dict_t* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Print a float_dict_t in a JSON-like key-value format with line wrapping.
 *
 * Values are printed using %g formatting for compact representation.
 *
 * Example:
 *
 *     { "a": 1.5, "b": 2.75, "c": 1e+06 }
 *
 * Wrapping occurs at 70 columns.
 *
 * @param dict    Pointer to dictionary. Must not be NULL.
 * @param stream  Output stream. Must not be NULL.
 *
 * @return NO_ERROR or:
 *         - NULL_POINTER
 *         - error from foreach_float_dict
 *
 * @note Floating-point precision is controlled by %g formatting.
 */
error_code_t print_float_dict(const float_dict_t* dict, FILE* stream);
// ================================================================================ 
// ================================================================================ 

// ================================================================================ 
// ================================================================================ 
// float_matrix_t — type-safe float wrapper around matrix_t
//
// This section provides a float-specialised interface to the generic matrix_t
// type.  float_matrix_t is a typedef for matrix_t; all functions below fix
// the dtype to FLOAT_TYPE and accept / return float values directly so that
// callers never need to pass a dtype_id_t or void* value.
//
// The wrapper covers every public function in c_matrix.h that either:
//   (a) takes a dtype_id_t argument (replaced with FLOAT_TYPE), or
//   (b) takes or returns void* element pointers (replaced with float / float*).
//
// Functions that are already type-agnostic (matrix_rows, matrix_cols,
// matrix_is_square, matrix_has_same_shape, etc.) are wrapped with a
// float_matrix_t* signature for consistency, but simply delegate.
// ================================================================================ 
// ================================================================================ 

typedef matrix_t float_matrix_t;

typedef struct {
    bool has_value;
    union {
        float_matrix_t* value;
        error_code_t    error;
    } u;
} float_matrix_expect_t;

/* =============================================================================
 * Comparator / Predicate Function Types
 * ========================================================================== */

/**
 * @brief Predicate function to determine semantic zero for float values.
 *
 * This function defines what values should be treated as "zero" during
 * dense-to-sparse conversion operations. It is primarily used by:
 *
 * - convert_float_matrix_zero()
 *
 * @param value The float value to evaluate.
 *
 * @return true if the value should be treated as zero, false otherwise.
 *
 * @note If NULL is passed, the default behavior is:
 *       value == 0.0f
 *
 * @example
 * Treat both 0.0 and -1.0 as zero:
 * @code{.c}
 * bool zero_or_neg_one(float v) {
 *     return (v == 0.0f) || (v == -1.0f);
 * }
 * @endcode
 */
typedef bool (*float_zero_fn)(float value);

// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialize a dense float matrix with zero-initialized storage.
 *
 * Creates a dense matrix of shape @p rows by @p cols with the dtype fixed to
 * FLOAT_TYPE. All elements are initialised to zero.
 *
 * @param rows     Number of matrix rows. Must be > 0.
 * @param cols     Number of matrix columns. Must be > 0.
 * @param alloc_v  Allocator for the matrix header and data buffer.
 *
 * @return float_matrix_expect_t with has_value true on success.
 *
 * @code{.c}
 *     allocator_vtable_t a = heap_allocator();
 *     float_matrix_expect_t r = init_float_dense_matrix(3, 4, a);
 *     if (!r.has_value) { fprintf(stderr, "%d\n", r.u.error); return; }
 *     float_matrix_t* m = r.u.value;
 *
 *     set_float_matrix(m, 0, 0, 1.0f);
 *     set_float_matrix(m, 1, 2, 5.5f);
 *
 *     return_float_matrix(m);
 * @endcode
 */
float_matrix_expect_t init_float_dense_matrix(size_t             rows,
                                              size_t             cols,
                                              allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize an empty COO sparse float matrix.
 *
 * Creates a coordinate-list sparse matrix with storage for up to @p capacity
 * entries. The dtype is fixed to FLOAT_TYPE.
 *
 * @param rows      Number of matrix rows. Must be > 0.
 * @param cols      Number of matrix columns. Must be > 0.
 * @param capacity  Initial entry capacity. Must be > 0.
 * @param growth    If true, the COO storage may grow when full.
 * @param alloc_v   Allocator for the matrix header and COO buffers.
 *
 * @return float_matrix_expect_t with has_value true on success.
 *
 * @code{.c}
 *     allocator_vtable_t a = heap_allocator();
 *     float_matrix_expect_t r = init_float_coo_matrix(100, 100, 16, true, a);
 *     float_matrix_t* m = r.u.value;
 *
 *     push_back_float_coo_matrix(m, 0, 5, 3.14f);
 *     push_back_float_coo_matrix(m, 42, 99, -1.0f);
 *
 *     return_float_matrix(m);
 * @endcode
 */
float_matrix_expect_t init_float_coo_matrix(size_t             rows,
                                            size_t             cols,
                                            size_t             capacity,
                                            bool               growth,
                                            allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Release all storage owned by a float matrix.
 *
 * Frees internal buffers and the matrix object itself. Passing NULL is safe.
 *
 * @param mat  Matrix to destroy, or NULL.
 */
void return_float_matrix(float_matrix_t* mat);

// ================================================================================
// Element access
// ================================================================================

/**
 * @brief Read a float element at the given row and column.
 *
 * For sparse formats, missing entries are returned as 0.0f.
 *
 * @param mat  Matrix to read from.
 * @param row  Zero-based row index.
 * @param col  Zero-based column index.
 * @param out  Receives the float value. Must not be NULL.
 *
 * @return NO_ERROR on success, or NULL_POINTER / INVALID_ARG / OUT_OF_BOUNDS.
 *
 * @code
 *     float v = 0.0f;
 *     get_float_matrix(m, 1, 2, &v);
 * @endcode
 */
error_code_t get_float_matrix(const float_matrix_t* mat,
                              size_t                row,
                              size_t                col,
                              float*                out);

// -------------------------------------------------------------------------------- 

/**
 * @brief Write a float element at the given row and column.
 *
 * Dense and COO matrices are supported. CSR/CSC return ILLEGAL_STATE.
 *
 * @param mat    Matrix to modify.
 * @param row    Zero-based row index.
 * @param col    Zero-based column index.
 * @param value  The float value to store.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 *
 * @code
 *     set_float_matrix(m, 0, 0, 42.0f);
 * @endcode
 */
error_code_t set_float_matrix(float_matrix_t* mat,
                              size_t          row,
                              size_t          col,
                              float           value);

// ================================================================================
// COO assembly helpers
// ================================================================================

/**
 * @brief Reserve additional entry capacity for a float COO matrix.
 *
 * @param mat       COO matrix to grow.
 * @param capacity  Requested minimum entry capacity.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t reserve_float_coo_matrix(float_matrix_t* mat,
                                      size_t          capacity);

// -------------------------------------------------------------------------------- 

/**
 * @brief Append or overwrite a float entry in a COO matrix.
 *
 * If (row, col) already exists its value is overwritten; otherwise a new
 * entry is appended. The matrix is marked unsorted after insertion.
 *
 * @param mat    COO matrix to append to.
 * @param row    Zero-based row index.
 * @param col    Zero-based column index.
 * @param value  The float value to insert.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 *
 * @code
 *     push_back_float_coo_matrix(m, 0, 5, 3.14f);
 * @endcode
 */
error_code_t push_back_float_coo_matrix(float_matrix_t* mat,
                                        size_t          row,
                                        size_t          col,
                                        float           value);

// -------------------------------------------------------------------------------- 

/**
 * @brief Sort COO entries by (row, col).
 *
 * @param mat  COO matrix to sort.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t sort_float_coo_matrix(float_matrix_t* mat);

// ================================================================================
// Lifecycle / structural operations
// ================================================================================

/**
 * @brief Clear a float matrix, preserving its allocated storage.
 *
 * Dense matrices are zeroed. COO matrices reset to zero logical entries.
 *
 * @param mat  Matrix to clear.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t clear_float_matrix(float_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Deep-copy a float matrix.
 *
 * @param src      Matrix to copy.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return float_matrix_expect_t with has_value true on success.
 */
float_matrix_expect_t copy_float_matrix(const float_matrix_t* src,
                                        allocator_vtable_t    alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Convert a float matrix to a different storage format.
 *
 * Produces a new matrix with the same logical values in the target format.
 *
 * @param src      Matrix to convert.
 * @param target   Desired destination storage format. Can be any value in 
 * ``matrix_format_t`` enum, (i.e. ``DENSE_MATRIX``, ``COO_MATRIX``, 
 * ``CSR_MATRIX``, ``CSC__MATRIX``)
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return float_matrix_expect_t with has_value true on success.
 *
 * @code
 *     float_matrix_expect_t csr =
 *         convert_float_matrix(dense_m, CSR_MATRIX, a);
 * @endcode
 */
float_matrix_expect_t convert_float_matrix(const float_matrix_t* src,
                                           matrix_format_t       target,
                                           allocator_vtable_t    alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Compute the transpose of a float matrix.
 *
 * Dense stays dense, COO stays COO, CSR transposes to CSR, CSC to CSC.
 *
 * @param src      Matrix to transpose.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return float_matrix_expect_t with has_value true on success.
 */
float_matrix_expect_t transpose_float_matrix(const float_matrix_t* src,
                                             allocator_vtable_t    alloc_v);

// ================================================================================
// Fill and zero
// ================================================================================

/**
 * @brief Fill a float matrix with a single repeated value.
 *
 * Nonzero fill is supported only for dense matrices. Supplying 0.0f resets
 * the matrix through clear_float_matrix.
 *
 * @param mat    Matrix to fill.
 * @param value  The float fill value.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 *
 * @code
 *     fill_float_matrix(m, 1.0f);   // every element becomes 1.0
 *     fill_float_matrix(m, 0.0f);   // equivalent to clear
 * @endcode
 */
error_code_t fill_float_matrix(float_matrix_t* mat,
                               float           value);
// ================================================================================
// Shape and compatibility queries
// ================================================================================

/**
 * @brief Test whether two float matrices have identical shape.
 */
bool float_matrix_has_same_shape(const float_matrix_t* a,
                                 const float_matrix_t* b);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a float matrix is square.
 */
bool float_matrix_is_square(const float_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a float matrix uses a sparse representation.
 */
bool float_matrix_is_sparse(const float_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a float matrix is logically all zeros.
 */
bool float_matrix_is_zero(const float_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Compare two float matrices for numerical equality.
 *
 * This function determines whether two matrices are logically equal based on
 * their numerical values, independent of their internal storage format.
 *
 * Equality is defined using standard floating-point comparison semantics:
 *
 * - Values are compared using the C `==` operator.
 * - `-0.0f` and `+0.0f` are considered equal.
 * - `NaN` values are not equal to any value, including themselves.
 *
 * Matrices must:
 * - be non-NULL
 * - have the same dimensions (rows and columns)
 * - have dtype == FLOAT_TYPE
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
 *       comparisons), use :c:func:`float_matrix_equal_cmp`.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * float_matrix_expect_t r1 = init_float_dense_matrix(2, 2, alloc);
 * float_matrix_expect_t r2 = init_float_dense_matrix(2, 2, alloc);
 *
 * float_matrix_t* a = r1.u.value;
 * float_matrix_t* b = r2.u.value;
 *
 * set_float_matrix(a, 0, 0, -0.0f);
 * set_float_matrix(b, 0, 0,  0.0f);
 *
 * // Numerical equality: -0.0f == +0.0f
 * assert_true(float_matrix_equal(a, b));
 *
 * return_float_matrix(a);
 * return_float_matrix(b);
 * @endcode
 *
 * @code{.c}
 * // Equality across different storage formats
 *
 * float_matrix_expect_t dense_r = init_float_dense_matrix(2, 2, alloc);
 * float_matrix_t* dense = dense_r.u.value;
 *
 * set_float_matrix(dense, 0, 1, 3.14f);
 *
 * float_matrix_expect_t csr_r =
 *     convert_float_matrix(dense, CSR_MATRIX, alloc);
 *
 * float_matrix_t* csr = csr_r.u.value;
 *
 * // Same logical values → equal
 * assert_true(float_matrix_equal(dense, csr));
 *
 * return_float_matrix(csr);
 * return_float_matrix(dense);
 * @endcode
 */
bool float_matrix_equal(const float_matrix_t* a,
                        const float_matrix_t* b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether two float matrices are compatible for addition.
 *
 * Requires equal shape and equal dtype.
 */
bool float_matrix_is_add_compatible(const float_matrix_t* a,
                                    const float_matrix_t* b);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether two float matrices are compatible for multiplication.
 *
 * Requires a->cols == b->rows and equal dtype.
 */
bool float_matrix_is_multiply_compatible(const float_matrix_t* a,
                                         const float_matrix_t* b);

// ================================================================================
// Row / column swaps
// ================================================================================

/**
 * @brief Swap two rows of a float matrix in place.
 *
 * Dense and COO matrices are supported. CSR/CSC return OPERATION_UNAVAILABLE.
 *
 * @param mat  Matrix to modify.
 * @param r1   First zero-based row index.
 * @param r2   Second zero-based row index.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t swap_float_matrix_rows(float_matrix_t* mat,
                                    size_t          r1,
                                    size_t          r2);

// -------------------------------------------------------------------------------- 

/**
 * @brief Swap two columns of a float matrix in place.
 *
 * Dense and COO matrices are supported. CSR/CSC return OPERATION_UNAVAILABLE.
 *
 * @param mat  Matrix to modify.
 * @param c1   First zero-based column index.
 * @param c2   Second zero-based column index.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t swap_float_matrix_cols(float_matrix_t* mat,
                                    size_t          c1,
                                    size_t          c2);

// ================================================================================
// Special matrix constructors
// ================================================================================

/**
 * @brief Initialize a dense float identity matrix.
 *
 * Creates an @p n by @p n dense matrix with 1.0f on the diagonal and 0.0f
 * elsewhere.
 *
 * @param n        Matrix order (rows == cols).
 * @param alloc_v  Allocator for the matrix.
 *
 * @return float_matrix_expect_t with has_value true on success.
 *
 * @code
 *     float_matrix_expect_t r = init_float_identity_matrix(3, a);
 *     // 3x3 identity matrix.
 * @endcode
 */
float_matrix_expect_t init_float_identity_matrix(size_t             n,
                                                 allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a dense float row vector (1 x length).
 *
 * @param length   Vector length (number of columns).
 * @param alloc_v  Allocator for the matrix.
 *
 * @return float_matrix_expect_t with has_value true on success.
 */
float_matrix_expect_t init_float_row_vector(size_t             length,
                                            allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a dense float column vector (length x 1).
 *
 * @param length   Vector length (number of rows).
 * @param alloc_v  Allocator for the matrix.
 *
 * @return float_matrix_expect_t with has_value true on success.
 */
float_matrix_expect_t init_float_col_vector(size_t             length,
                                            allocator_vtable_t alloc_v);

// ================================================================================
// Vector shape queries
// ================================================================================

/**
 * @brief Test whether a float matrix has row-vector shape (1 x N).
 */
bool float_matrix_is_row_vector(const float_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a float matrix has column-vector shape (N x 1).
 */
bool float_matrix_is_col_vector(const float_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a float matrix has vector shape (row or column).
 */
bool float_matrix_is_vector(const float_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Return the logical length of a vector-shaped float matrix.
 *
 * For row vectors returns cols; for column vectors returns rows.
 * Non-vector matrices return 0.
 */
size_t float_matrix_vector_length(const float_matrix_t* mat);

// ================================================================================
// Conversion with semantic zero callback
// ================================================================================

/**
 * @brief Convert a float matrix to a different storage format using an
 *        optional semantic zero-test.
 *
 * This function performs the same structural conversion as
 * :c:func:`convert_float_matrix`, but allows the caller to define what
 * float values should be treated as zero during dense-to-sparse conversion.
 *
 * The @p is_zero callback is used only when:
 *   - @p src is a dense matrix, and
 *   - @p target is COO_MATRIX, CSR_MATRIX, or CSC_MATRIX.
 *
 * In all other cases, this function behaves exactly like
 * :c:func:`convert_float_matrix`.
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
 * @return float_matrix_expect_t with has_value true on success, or
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
 * static bool zero_or_neg_one(float v) {
 *     return (v == 0.0f) || (v == -1.0f);
 * }
 *
 * float_matrix_expect_t r =
 *     convert_float_matrix_zero(dense_m, CSR_MATRIX, alloc, zero_or_neg_one);
 * @endcode
 *
 * @note This function is primarily intended for domain-specific sparse
 *       conversion rules where exact zero is not sufficient.
 */
float_matrix_expect_t convert_float_matrix_zero(const float_matrix_t* src,
                                                matrix_format_t       target,
                                                allocator_vtable_t    alloc_v,
                                                float_zero_fn         is_zero);
// -------------------------------------------------------------------------------- 

/**
 * @brief Print a float matrix in a human-readable format.
 *
 * Dense matrices are printed row-by-row as nested bracketed arrays:
 *
 *     [ [ 1.5, 2, 3 ],
 *       [ 4, 5.25, 6 ] ]
 *
 * Sparse matrices (COO, CSR, CSC) are printed as a flat list of explicitly
 * stored logical nonzero entries:
 *
 *     [ (0, 0): 1.5, (0, 1): -2.25, (1, 2): 6 ]
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
 *         - error from get_float_matrix
 */
error_code_t print_float_matrix(const float_matrix_t* mat, FILE* stream);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieve the minimum value from a float matrix.
 *
 * This function determines the minimum element in a matrix of type
 * `float_matrix_t`. It delegates the index computation to the generic
 * `matrix_min()` function and then retrieves the corresponding value
 * based on the matrix storage format.
 *
 * The comparison is performed using unsigned 8-bit integer ordering.
 *
 * @param mat Pointer to the int16 matrix.
 *
 * @return int16_expect_t
 * @retval has_value = true   The minimum value was found and is stored in u.value.
 * @retval has_value = false  An error occurred, and the error code is stored in u.error.
 *
 * @errors
 * - NULL_POINTER   if @p mat is NULL.
 * - TYPE_MISMATCH  if the matrix dtype is not int16_TYPE.
 * - EMPTY          if the matrix contains no elements:
 *                  - dense: rows * cols == 0
 *                  - sparse: nnz == 0
 * - INVALID_ARG    if the matrix format is not recognized.
 * - LENGTH_OVERFLOW if an internal size computation overflows (dense matrices only).
 *
 * @note
 * This function relies on the generic `matrix_min()` function to determine the
 * index of the minimum element. The returned index is interpreted according to
 * the matrix format:
 *
 * - DENSE_MATRIX: index is a row-major offset.
 * - COO_MATRIX:   index refers to entries in row_idx[], col_idx[], and values[].
 * - CSR_MATRIX:   index refers to entries in col_idx[] and values[].
 * - CSC_MATRIX:   index refers to entries in row_idx[] and values[].
 *
 * @warning
 * For sparse matrices, only stored values (nnz) are considered. Implicit zero
 * elements are not included in the comparison.
 *
 * @see matrix_min
 */
float_expect_t float_matrix_min(const float_matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieve the maximum value from a float matrix.
 *
 * This function determines the maximum element in a matrix of type
 * `float_matrix_t`. It delegates the index computation to the generic
 * `matrix_max()` function and then retrieves the corresponding value
 * based on the matrix storage format.
 *
 * The comparison is performed using unsigned 8-bit integer ordering.
 *
 * @param mat Pointer to the float matrix.
 *
 * @return float_expect_t
 * @retval has_value = true   The maximum value was found and is stored in u.value.
 * @retval has_value = false  An error occurred, and the error code is stored in u.error.
 *
 * @errors
 * - NULL_POINTER   if @p mat is NULL.
 * - TYPE_MISMATCH  if the matrix dtype is not float_TYPE.
 * - EMPTY          if the matrix contains no elements:
 *                  - dense: rows * cols == 0
 *                  - sparse: nnz == 0
 * - INVALID_ARG    if the matrix format is not recognized.
 * - LENGTH_OVERFLOW if an internal size computation overflows (dense matrices only).
 *
 * @note
 * This function relies on the generic `matrix_max()` function to determine the
 * index of the maximum element. The returned index is interpreted according to
 * the matrix format:
 *
 * - DENSE_MATRIX: index is a row-major offset.
 * - COO_MATRIX:   index refers to entries in row_idx[], col_idx[], and values[].
 * - CSR_MATRIX:   index refers to entries in col_idx[] and values[].
 * - CSC_MATRIX:   index refers to entries in row_idx[] and values[].
 *
 * @warning
 * For sparse matrices, only stored values (nnz) are considered. Implicit zero
 * elements are not included in the comparison.
 *
 * @see matrix_max
 */
float_expect_t float_matrix_max(const float_matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Compute the sum of all elements in a float matrix.
 *
 * This function computes the sum of all elements in a matrix of type
 * `float_matrix_t`. It delegates traversal to the generic `matrix_sum()`
 * function and accumulates the result using unsigned 32-bit arithmetic.
 *
 * The accumulator is initialized to zero and updated for each element in
 * the matrix using standard C unsigned integer addition.
 *
 * @param mat Pointer to the float matrix.
 *
 * @return float_expect_t
 * @retval has_value = true   The sum was successfully computed and is stored in u.value.
 * @retval has_value = false  An error occurred, and the error code is stored in u.error.
 *
 * @errors
 * - NULL_POINTER   if @p mat is NULL.
 * - TYPE_MISMATCH  if the matrix dtype is not float_TYPE.
 * - EMPTY          if the matrix contains no elements:
 *                  - dense: rows * cols == 0
 *                  - sparse: nnz == 0
 * - INVALID_ARG    if the matrix format is not recognized.
 * - LENGTH_OVERFLOW if an internal size computation overflows (dense matrices only).
 *
 * @note
 * For dense matrices, all elements (rows * cols) are included in the sum.
 * For sparse matrices (COO, CSR, CSC), only stored elements (nnz) are included.
 *
 * @warning
 * This function uses standard unsigned 32-bit arithmetic. If the mathematical
 * sum exceeds `float_MAX`, the result will wrap around modulo 2^32. Overflow
 * is not detected or reported.
 *
 * @see matrix_sum
 * @see float_matrix_min
 * @see float_matrix_max
 */
float_expect_t float_matrix_sum(const float_matrix_t* mat);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_float_H */
// ================================================================================
// ================================================================================
// eof
