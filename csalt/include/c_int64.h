// ================================================================================
// ================================================================================
// - File:    c_int64.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 06, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
#ifndef c_int64_H
#define c_int64_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "c_array.h"
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

/**
 * @brief A type-safe dynamic array of int64_t elements.
 *
 * Wraps array_t with the dtype fixed to INT64_TYPE at initialization.
 * All operations enforce INT64_TYPE at the call site so the caller never
 * needs to pass a dtype_id_t argument. The underlying array_t is embedded
 * by value so the struct can be stack- or heap-allocated by the caller.
 */
typedef struct {
    array_t base; /**< Underlying generic array. Do not modify directly. */
} int64_array_t;

// ================================================================================
// Expected type
// ================================================================================

/**
 * @brief Expected return type for int64_array_t initialization and copy/slice.
 */
typedef struct {
    bool has_value;
    union {
        int64_array_t* value;
        error_code_t   error;
    } u;
} int64_array_expect_t;

// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialize a new heap-allocated int64_array_t.
 *
 * Allocates both the array_t struct and its internal data buffer through
 * alloc_v. The element type is fixed to INT64_TYPE and the element size is
 * cached as 8 bytes. The array starts empty (len == 0) with the full capacity
 * reserved. When growth is true the data buffer will be reallocated
 * automatically whenever a push operation would exceed the current capacity;
 * when false any push beyond capacity returns CAPACITY_OVERFLOW without
 * modifying the array. The caller is responsible for calling
 * return_int64_array when the array is no longer needed.
 *
 * @param capacity  Initial number of elements the buffer can hold.
 *                  Must be > 0.
 * @param growth    Pass true to allow the buffer to grow automatically on
 *                  overflow, false to use a fixed-capacity buffer.
 * @param alloc_v   Allocator vtable used for all internal memory operations.
 *                  alloc_v.allocate must not be NULL.
 *
 * @return int64_array_expect_t with has_value true and u.value pointing to
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
 *     int64_array_expect_t result = init_int64_array(32, false, alloc);
 *     if (!result.has_value) {
 *         fprintf(stderr, "init failed: %d\n", result.u.error);
 *         return;
 *     }
 *     int64_array_t* arr = result.u.value;
 *
 *     push_back_int64_array(arr, -1000000LL);
 *     push_back_int64_array(arr,        0LL);
 *     push_back_int64_array(arr,  1000000LL);
 *     // arr now contains [-1000000, 0, 1000000], len == 3, alloc == 32.
 *
 *     return_int64_array(arr);
 * @endcode
 */
int64_array_expect_t init_int64_array(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v);

// --------------------------------------------------------------------------------

/**
 * @brief Release all memory owned by an int64_array_t back to its allocator.
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
 *     int64_array_expect_t result = init_int64_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     int64_array_t* arr = result.u.value;
 *
 *     push_back_int64_array(arr, -99999LL);
 *
 *     // Release all memory. arr must not be used after this point.
 *     return_int64_array(arr);
 *     arr = NULL;  // Optional but recommended to prevent accidental reuse.
 *
 *     // Passing NULL is always safe.
 *     return_int64_array(NULL);
 * @endcode
 */
void return_int64_array(int64_array_t* array);

// ================================================================================
// Push operations
// ================================================================================

/**
 * @brief Append one int64_t to the end of the array.
 *
 * Copies value into the next available slot at index array->base.len and
 * increments len by one. If the array is full and growth is true the data
 * buffer is reallocated before the value is written; if growth is false the
 * call fails immediately with CAPACITY_OVERFLOW and the array is left
 * unmodified. This is an O(1) amortised operation when growth is true and
 * strictly O(1) when the buffer has spare capacity.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param value  The int64_t value to append.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code
 *     push_back_int64_array(arr, -1000LL);
 *     push_back_int64_array(arr,     0LL);
 *     push_back_int64_array(arr,  1000LL);
 *     // arr contains [-1000, 0, 1000], len == 3.
 *
 *     int64_t out = 0;
 *     get_int64_array_index(arr, 2, &out);  // out == 1000
 * @endcode
 */
error_code_t push_back_int64_array(int64_array_t* array, int64_t value);

// --------------------------------------------------------------------------------

/**
 * @brief Prepend one int64_t to the front of the array.
 *
 * Shifts all existing elements one position toward the back using memmove,
 * writes value into index 0, and increments len by one. If the array is full
 * and growth is true the data buffer is reallocated before the shift; if
 * growth is false the call fails immediately with CAPACITY_OVERFLOW and the
 * array is left unmodified. Because every existing element must be moved this
 * is an O(n) operation — prefer push_back_int64_array for performance-sensitive
 * append-heavy workloads.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param value  The int64_t value to prepend.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code
 *     push_back_int64_array(arr,  2000LL);
 *     push_back_int64_array(arr,  3000LL);
 *     push_front_int64_array(arr, 1000LL);
 *     // arr contains [1000, 2000, 3000].
 * @endcode
 */
error_code_t push_front_int64_array(int64_array_t* array, int64_t value);

// --------------------------------------------------------------------------------

/**
 * @brief Insert one int64_t at an arbitrary index.
 *
 * Shifts all elements at positions >= index one slot toward the back using
 * memmove, writes value at index, and increments len by one. If the array is
 * full and growth is true the data buffer is reallocated before the shift;
 * if growth is false the call fails immediately with CAPACITY_OVERFLOW. Passing
 * index == 0 is equivalent to push_front_int64_array; passing index == len
 * is equivalent to push_back_int64_array. This is an O(n) operation in the
 * general case.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param index  Zero-based insertion position. Must be <= array->base.len.
 * @param value  The int64_t value to insert.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - OUT_OF_BOUNDS     if index > array->base.len
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code
 *     push_back_int64_array(arr, 1000LL);
 *     push_back_int64_array(arr, 3000LL);
 *     push_at_int64_array(arr, 1, 2000LL);
 *     // arr contains [1000, 2000, 3000].
 * @endcode
 */
error_code_t push_at_int64_array(int64_array_t* array,
                                 size_t         index,
                                 int64_t        value);

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
 * @param out    Pointer to an int64_t that receives the value. Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array or out is NULL
 *         - OUT_OF_BOUNDS if index >= array->base.len
 *
 * @code
 *     int64_t val = 0;
 *     get_int64_array_index(arr, 0, &val);  // val == first element
 * @endcode
 */
error_code_t get_int64_array_index(const int64_array_t* array,
                                   size_t               index,
                                   int64_t*             out);

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
 * @param out    Pointer to an int64_t that receives the removed value, or NULL
 *               to discard it.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *
 * @code
 *     int64_t val = 0;
 *     pop_back_int64_array(arr, &val);   // removes last; val holds its value
 *     pop_back_int64_array(arr, NULL);   // removes last; value discarded
 * @endcode
 */
error_code_t pop_back_int64_array(int64_array_t* array, int64_t* out);

// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally return the first element of the array.
 *
 * Copies the element at index 0 into *out (if out is non-NULL), then shifts
 * all remaining elements one position toward the front using memmove and
 * decrements len by one. The call is O(n).
 *
 * @param array  Pointer to the array. Must not be NULL.
 * @param out    Pointer to an int64_t that receives the removed value, or NULL
 *               to discard it.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *
 * @code
 *     int64_t val = 0;
 *     pop_front_int64_array(arr, &val);  // removes first; val holds its value
 * @endcode
 */
error_code_t pop_front_int64_array(int64_array_t* array, int64_t* out);

// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally return the element at an arbitrary index.
 *
 * Copies the element at index into *out (if out is non-NULL), then shifts all
 * elements after index one position toward the front using memmove and
 * decrements len by one. Passing index == 0 is equivalent to
 * pop_front_int64_array; passing index == len - 1 is equivalent to
 * pop_back_int64_array. The call is O(n) in the general case.
 *
 * @param array  Pointer to the array. Must not be NULL.
 * @param out    Pointer to an int64_t that receives the removed value, or NULL
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
 *     int64_t val = 0;
 *     pop_any_int64_array(arr, &val, 2);  // removes element at index 2
 * @endcode
 */
error_code_t pop_any_int64_array(int64_array_t* array,
                                 int64_t*       out,
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
 *     clear_int64_array(arr);
 *     // int64_array_size(arr) == 0, int64_array_alloc(arr) unchanged.
 * @endcode
 */
error_code_t clear_int64_array(int64_array_t* array);

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
 * @param value  The new int64_t value to store at index.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - OUT_OF_BOUNDS if index >= array->base.len
 *
 * @code
 *     set_int64_array_index(arr, 1, INT64_MIN);  // overwrites element at index 1
 * @endcode
 */
error_code_t set_int64_array_index(int64_array_t* array,
                                   size_t         index,
                                   int64_t        value);

// --------------------------------------------------------------------------------

/**
 * @brief Allocate an independent deep copy of an array.
 *
 * Creates a new int64_array_t with the same elements, length, capacity, dtype,
 * and growth flag as src. Both the new struct and its data buffer are allocated
 * through alloc_v. Subsequent modifications to either array do not affect the
 * other. The caller is responsible for calling return_int64_array on the
 * returned array when it is no longer needed.
 *
 * @param src     Pointer to the source array. Must not be NULL.
 * @param alloc_v Allocator vtable for the new array. alloc_v.allocate must
 *                not be NULL.
 *
 * @return int64_array_expect_t with has_value true and u.value pointing to
 *         the new array on success. On failure has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if src is NULL
 *         - BAD_ALLOC     if the struct allocation fails
 *         - OUT_OF_MEMORY if the data buffer allocation fails
 *
 * @code
 *     int64_array_expect_t cr = copy_int64_array(src, alloc);
 *     if (!cr.has_value) { return; }
 *     int64_array_t* dst = cr.u.value;
 *     // dst is independent of src.
 *     return_int64_array(dst);
 * @endcode
 */
int64_array_expect_t copy_int64_array(const int64_array_t* src,
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
 *     concat_int64_array(dst, src);
 *     // dst now ends with all elements that were in src.
 * @endcode
 */
error_code_t concat_int64_array(int64_array_t*       dst,
                                const int64_array_t* src);

// --------------------------------------------------------------------------------

/**
 * @brief Allocate a new array containing a sub-range of src.
 *
 * Copies the elements at indices [start, end) from src into a freshly
 * allocated int64_array_t of capacity end - start. The source array is not
 * modified. The caller is responsible for calling return_int64_array on the
 * returned array.
 *
 * @param src     Pointer to the source array. Must not be NULL.
 * @param start   Zero-based index of the first element to include (inclusive).
 * @param end     Zero-based index one past the last element to include
 *                (exclusive). Must be > start and <= src->base.len.
 * @param alloc_v Allocator for the new array.
 *
 * @return int64_array_expect_t with has_value true and u.value pointing to
 *         the new array on success. On failure has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if src is NULL
 *         - INVALID_ARG   if start >= end
 *         - OUT_OF_BOUNDS if end > src->base.len
 *         - BAD_ALLOC     if the struct allocation fails
 *         - OUT_OF_MEMORY if the data buffer allocation fails
 *
 * @code
 *     // src: [-20, -10, 0, 10, 20]
 *     int64_array_expect_t sr = slice_int64_array(src, 1, 4, alloc);
 *     // sr.u.value contains [-10, 0, 10].
 *     return_int64_array(sr.u.value);
 * @endcode
 */
int64_array_expect_t slice_int64_array(const int64_array_t* src,
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
 *     // arr: [-3, -2, -1, 0, 1]
 *     reverse_int64_array(arr);
 *     // arr: [1, 0, -1, -2, -3]
 * @endcode
 */
error_code_t reverse_int64_array(int64_array_t* array);

// ================================================================================
// Sort
// ================================================================================

/**
 * @brief Sort all elements in place in ascending or descending order.
 *
 * Uses the generic sort_array path with a signed 64-bit comparator. The
 * comparator uses the three-way idiom to avoid undefined behaviour for
 * extreme value pairs such as (INT64_MIN, INT64_MAX) where subtraction
 * overflows. The sort is not guaranteed to be stable. The call is O(n log n)
 * on average.
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
 *     // arr: [30, -10, 20]
 *     sort_int64_array(arr, FORWARD);
 *     // arr: [-10, 20, 30]
 *
 *     sort_int64_array(arr, REVERSE);
 *     // arr: [30, 20, -10]
 * @endcode
 */
error_code_t sort_int64_array(int64_array_t* array, direction_t dir);

// ================================================================================
// Reduction
// ================================================================================

/**
 * @brief Find the minimum value in the array.
 *
 * Scans all elements using a signed 64-bit comparison and returns the
 * most-negative value found. When two or more elements share the minimum
 * value the value of the first occurrence is returned. The array is not
 * modified.
 *
 * @param array  Pointer to the array to scan. Must not be NULL and must
 *               contain at least one element.
 *
 * @return int64_expect_t with has_value true and u.value == the minimum
 *         element on success. On failure, has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if array->base.len == 0
 *
 * @code
 *     push_back_int64_array(arr,  300LL);
 *     push_back_int64_array(arr, -100LL);
 *     push_back_int64_array(arr,  200LL);
 *
 *     int64_expect_t r = int64_array_min(arr);
 *     // r.has_value == true, r.u.value == -100.
 * @endcode
 */
int64_expect_t int64_array_min(const int64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Find the maximum value in the array.
 *
 * Scans all elements using a signed 64-bit comparison and returns the
 * largest value found. When two or more elements share the maximum value
 * the value of the first occurrence is returned. The array is not modified.
 *
 * @param array  Pointer to the array to scan. Must not be NULL and must
 *               contain at least one element.
 *
 * @return int64_expect_t with has_value true and u.value == the maximum
 *         element on success. On failure, has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if array->base.len == 0
 *
 * @code
 *     int64_expect_t r = int64_array_max(arr);
 *     // r.has_value == true, r.u.value == 300.
 * @endcode
 */
int64_expect_t int64_array_max(const int64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Sum all elements and return the result as an int64_t.
 *
 * Iterates over every element and accumulates the total in an int64_t.
 *
 * Overflow: the accumulator is an int64_t. If the true sum exceeds INT64_MAX
 * or falls below INT64_MIN the result wraps with signed overflow behaviour
 * as defined by the platform. The caller is responsible for ensuring the
 * array is small enough that overflow will not occur in practice.
 *
 * @param array  Pointer to the array to sum. Must not be NULL.
 *
 * @return int64_expect_t with has_value true and u.value == the sum on
 *         success. On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if array->base.len == 0
 *
 * @code
 *     push_back_int64_array(arr,  1000000000LL);
 *     push_back_int64_array(arr, -2000000000LL);
 *     push_back_int64_array(arr,  3000000000LL);
 *
 *     int64_expect_t r = int64_array_sum(arr);
 *     // r.has_value == true, r.u.value == 2000000000.
 * @endcode
 */
int64_expect_t int64_array_sum(const int64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Compute the prefix sum of an int64 array into a new int64 array.
 *
 * Allocates a new int64_array_t of the same length as @p src and fills it
 * with the inclusive prefix sum: output[0] == src[0], and output[i] ==
 * output[i-1] + src[i] for every subsequent index. @p src is not modified.
 *
 * Overflow: each element is accumulated in an int64_t. If the running total
 * exceeds INT64_MAX or falls below INT64_MIN the behaviour is as described
 * for int64_array_sum. The caller is responsible for ensuring values remain
 * within range.
 *
 * The output array is owned by the caller and must be freed with
 * return_int64_array when no longer needed.
 *
 * @param src    Source array to scan. Must not be NULL and must contain at
 *               least one element.
 * @param alloc  Allocator used for the output array.
 *
 * @return int64_array_expect_t with has_value true and u.value pointing to
 *         the newly allocated output array on success. On failure, has_value
 *         is false and u.error is one of:
 *         - NULL_POINTER  if src is NULL
 *         - EMPTY         if src contains no elements
 *         - BAD_ALLOC     if the output struct allocation failed
 *         - OUT_OF_MEMORY if the output data buffer allocation failed
 *
 * @code
 *     push_back_int64_array(src,  1000LL);
 *     push_back_int64_array(src, -2000LL);
 *     push_back_int64_array(src,  3000LL);
 *     push_back_int64_array(src, -4000LL);
 *     // src contains [1000, -2000, 3000, -4000].
 *
 *     int64_array_expect_t cr = cumulative_int64_array(src, alloc);
 *     // cr.u.value contains [1000, -1000, 2000, -2000].
 *
 *     return_int64_array(src);
 *     return_int64_array(cr.u.value);
 * @endcode
 */
int64_array_expect_t cumulative_int64_array(const int64_array_t* src,
                                            allocator_vtable_t   alloc);

// ================================================================================
// Search
// ================================================================================

/**
 * @brief Search for the first occurrence of value in the range [start, end).
 *
 * Scans elements at indices [start, end) using an exact bitwise comparison.
 * The first element whose stored value equals value is returned. If the value
 * appears more than once only the lowest index within the range is returned.
 * The array is not modified.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param value  The int64_t value to search for.
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
 *     // arr: [-100, 300, -200, 300, 400]
 *     size_expect_t r = int64_array_contains(arr, 300LL, 0, 5);
 *     // r.has_value == true, r.u.value == 1  (first occurrence).
 * @endcode
 */
size_expect_t int64_array_contains(const int64_array_t* array,
                                   int64_t              value,
                                   size_t               start,
                                   size_t               end);

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
 * @param value  The int64_t value to find.
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
 *     // arr (unsorted): [300, -100, 200]
 *     size_expect_t r = int64_array_binary_search(arr, 200LL, true);
 *     // arr is now [-100, 200, 300]; r.u.value == 1.
 * @endcode
 */
size_expect_t int64_array_binary_search(int64_array_t* array,
                                        int64_t        value,
                                        bool           sort);

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
 * @param value  The int64_t value to bracket.
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
 *     // arr: [INT64_MIN, -1000, 1000, INT64_MAX]
 *     bracket_expect_t r = int64_array_binary_bracket(arr, 0LL, false);
 *     // r.u.value.lower == 2  (1000), r.u.value.upper == 1  (-1000).
 * @endcode
 */
bracket_expect_t int64_array_binary_bracket(int64_array_t* array,
                                            int64_t        value,
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
 *     push_back_int64_array(arr, -1000LL);
 *     push_back_int64_array(arr,  2000LL);
 *     // int64_array_size(arr) == 2.
 * @endcode
 */
size_t int64_array_size(const int64_array_t* array);

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
 *     // int64_array_alloc(arr) == 8 after init with capacity 8.
 * @endcode
 */
size_t int64_array_alloc(const int64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return the size in bytes of one element in the array.
 *
 * Reads array->base.data_size, which is fixed to 8 for all int64_array_t
 * instances — an int64_t is always eight bytes. The call is O(1) and does not
 * modify the array.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return 8 when array is non-NULL, or 0 if array is NULL.
 *
 * @code
 *     size_t elem_bytes = int64_array_data_size(arr);  // always 8
 * @endcode
 */
size_t int64_array_data_size(const int64_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if the array currently contains no elements.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return true if array is NULL or array->base.len == 0, false otherwise.
 *
 * @code
 *     // is_int64_array_empty(arr) == true immediately after init.
 *     push_back_int64_array(arr, -1000LL);
 *     // is_int64_array_empty(arr) == false.
 * @endcode
 */
bool is_int64_array_empty(const int64_array_t* array);

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
 *     push_back_int64_array(arr, -1000LL);
 *     push_back_int64_array(arr,  2000LL);
 *     // is_int64_array_full(arr) == false (one slot remains).
 *
 *     push_back_int64_array(arr, -3000LL);
 *     // is_int64_array_full(arr) == true, size == alloc == 3.
 * @endcode
 */
bool is_int64_array_full(const int64_array_t* array);

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
 *     const int64_t* first = (const int64_t*)arr->base.data;
 *     is_int64_array_ptr(arr, first);       // true
 *     is_int64_array_ptr(arr, first + 2);   // true if len >= 3
 *     is_int64_array_ptr(arr, first + len); // false — spare capacity
 *     is_int64_array_ptr(arr, NULL);        // false
 * @endcode
 */
bool is_int64_array_ptr(const int64_array_t* array, const int64_t* ptr);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_int64_H */
// ================================================================================
// ================================================================================
// eof
