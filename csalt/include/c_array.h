// ================================================================================
// ================================================================================
// - File:    array.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 21, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
#ifndef array_H
#define array_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "c_dtypes.h"
#include "c_allocator.h"
#include "c_error.h"
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

/**
 * @brief A generic, type-safe, heap-agnostic dynamic array.
 *
 * All memory is managed through the embedded allocator_vtable_t. The data
 * buffer holds type-erased bytes; dtype and data_size are fixed at init time
 * and cached on the struct so every operation is allocation-free and requires
 * no registry lookups after initialization. Do not modify any field directly
 * — use the provided API functions.
 */
typedef struct {
    uint8_t*           data;       /**< Type-erased byte buffer holding elements.       */
    size_t             len;        /**< Current number of elements stored.              */
    size_t             alloc;      /**< Allocated capacity in number of elements.       */
    size_t             data_size;  /**< Size of one element in bytes, cached from dtype.*/
    dtype_id_t         dtype;      /**< Runtime type identity. Fixed at init time.      */
    bool               growth;     /**< If true, buffer grows automatically when full.  */
    allocator_vtable_t alloc_v;    /**< Allocator vtable used for all memory ops.       */
} array_t;

// ================================================================================
// Expected type
// ================================================================================

/**
 * @brief Expected return type for array initialization.
 *
 * On success, has_value is true and u.value points to the initialized array_t.
 * On failure, has_value is false and u.error contains the error code.
 */
typedef struct {
    bool has_value;
    union {
        array_t*     value;
        error_code_t error;
    } u;
} array_expect_t;
// -------------------------------------------------------------------------------- 

#ifndef ITER_DIR_H
#define ITER_DIR_H
    typedef enum {
        FORWARD = 0,
        REVERSE = 1
    }direction_t;
#endif /* ITER_DIR_H*/

// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialize a new array.
 *
 * Allocates the array_t struct and its data buffer through the provided
 * allocator vtable. The dtype must be registered in the dtype registry before
 * calling this function. data_size is resolved from the registry once at init
 * and cached on the struct — no further registry lookups are performed during
 * the lifetime of the array. If growth is true, the data buffer will be
 * reallocated automatically when full, provided the allocator supports
 * reallocate. If growth is false, push operations return CAPACITY_OVERFLOW
 * when the array is full.
 *
 * @param capacity  Initial number of elements to allocate space for. Must be > 0.
 * @param dtype     Type identifier. Must be registered in the dtype registry.
 * @param growth    If true, the array grows automatically when full.
 * @param alloc_v   Allocator vtable to use for all memory operations.
 *
 * @return array_expect_t with has_value true and a valid array_t* on success.
 *         On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER      if alloc_v.allocate is NULL
 *         - INVALID_ARG       if capacity is 0 or dtype is UNKNOWN_TYPE
 *         - ILLEGAL_STATE     if the dtype registry could not be initialized
 *         - TYPE_MISMATCH     if dtype is not registered in the dtype registry
 *         - LENGTH_OVERFLOW   if capacity * data_size would overflow size_t
 *         - BAD_ALLOC         if the allocator fails to allocate the struct
 *         - OUT_OF_MEMORY     if the allocator fails to allocate the data buffer
 */
array_expect_t init_array(size_t             capacity,
                          dtype_id_t         dtype,
                          bool               growth,
                          allocator_vtable_t alloc_v);

// --------------------------------------------------------------------------------

/**
 * @brief Return an array's memory back to its allocator.
 *
 * Returns the data buffer and the array_t struct to the allocator via
 * return_element. This does NOT free or destroy the allocator itself.
 * After this call the pointer must not be used.
 *
 * @param array  Pointer to the array to return. Silently ignored if NULL.
 */
void return_array(array_t* array);

// ================================================================================
// Push operations
// ================================================================================

/**
 * @brief Append one element to the end of the array.
 *
 * If the array is full and growth is true, the data buffer is reallocated
 * using the tiered growth strategy. If the array is full and growth is false,
 * returns CAPACITY_OVERFLOW without modifying the array.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param data   Pointer to the element to copy in. Must not be NULL.
 *               The pointed-to data must be exactly array->data_size bytes.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array or data is NULL
 *         - TYPE_MISMATCH     if dtype != array->dtype
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 */
error_code_t push_back_array(array_t*    array,
                             const void* data,
                             dtype_id_t  dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Prepend one element to the front of the array.
 *
 * Shifts all existing elements one position toward the end before inserting.
 * O(n) operation. Growth behaviour is identical to push_back_array.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param data   Pointer to the element to copy in. Must not be NULL.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array or data is NULL
 *         - TYPE_MISMATCH     if dtype != array->dtype
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 */
error_code_t push_front_array(array_t*    array,
                              const void* data,
                              dtype_id_t  dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Insert one element at an arbitrary index.
 *
 * Elements at index and beyond are shifted one position toward the end.
 * Delegates to push_back_array if index == array->len, and to
 * push_front_array if index == 0. O(n) operation. Growth behaviour is
 * identical to push_back_array.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param data   Pointer to the element to copy in. Must not be NULL.
 * @param index  Position at which to insert. Must be <= array->len.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array or data is NULL
 *         - TYPE_MISMATCH     if dtype != array->dtype
 *         - OUT_OF_BOUNDS     if index > array->len
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 */
error_code_t push_at_array(array_t*    array,
                           const void* data,
                           size_t      index,
                           dtype_id_t  dtype);

// ================================================================================
// Get operation
// ================================================================================

/**
 * @brief Copy one element out of the array by index.
 *
 * Copies exactly array->data_size bytes from the array's internal buffer
 * into the caller-provided output buffer. The caller is responsible for
 * ensuring the output buffer is at least array->data_size bytes.
 *
 * @param array  Pointer to the source array. Must not be NULL.
 * @param index  Zero-based index of the element to retrieve.
 * @param out    Caller-provided buffer to copy the element into. Must not be NULL.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array or out is NULL
 *         - TYPE_MISMATCH if dtype != array->dtype
 *         - OUT_OF_BOUNDS if index >= array->len
 */
error_code_t get_array_index(const array_t* array,
                             size_t         index,
                             void*          out,
                             dtype_id_t     dtype);

// ================================================================================
// Pop operations
// ================================================================================

/**
 * @brief Remove and optionally retrieve the last element of the array.
 *
 * Decrements len by one. If out is non-NULL, copies the removed element into
 * the caller-provided buffer before removing it. If out is NULL the element
 * is discarded silently.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param out    Caller-provided buffer to copy the removed element into, or
 *               NULL to discard. If non-NULL must be at least data_size bytes.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - TYPE_MISMATCH if dtype != array->dtype
 *         - EMPTY         if the array contains no elements
 */
error_code_t pop_back_array(array_t*   array,
                            void*      out,
                            dtype_id_t dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally retrieve the first element of the array.
 *
 * Shifts all remaining elements one position toward the front after removal.
 * O(n) operation. If out is non-NULL, copies the removed element into the
 * caller-provided buffer before shifting. If out is NULL the element is
 * discarded silently.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param out    Caller-provided buffer to copy the removed element into, or
 *               NULL to discard. If non-NULL must be at least data_size bytes.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - TYPE_MISMATCH if dtype != array->dtype
 *         - EMPTY         if the array contains no elements
 */
error_code_t pop_front_array(array_t*   array,
                             void*      out,
                             dtype_id_t dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally retrieve the element at an arbitrary index.
 *
 * Elements after index are shifted one position toward the front after removal.
 * Delegates to pop_back_array if index == array->len - 1, and to
 * pop_front_array if index == 0. O(n) operation. If out is non-NULL, copies
 * the removed element into the caller-provided buffer before shifting. If out
 * is NULL the element is discarded silently.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param out    Caller-provided buffer to copy the removed element into, or
 *               NULL to discard. If non-NULL must be at least data_size bytes.
 * @param index  Zero-based index of the element to remove. Must be < array->len.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - TYPE_MISMATCH if dtype != array->dtype
 *         - EMPTY         if the array contains no elements
 *         - OUT_OF_BOUNDS if index >= array->len
 */
error_code_t pop_any_array(array_t*   array,
                           void*      out,
                           size_t     index,
                           dtype_id_t dtype);

// ================================================================================
// Utility operations
// ================================================================================

/**
 * @brief Reset the array to empty without releasing its allocated buffer.
 *
 * Sets len to zero so the array can be reused from scratch. The allocated
 * capacity and the data buffer are preserved. All previously stored element
 * bytes are zeroed so no stale data remains readable in the buffer.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 *
 * @return NO_ERROR on success, or:
 *         - NULL_POINTER if array is NULL
 */
error_code_t clear_array(array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Overwrite the element at an existing index without changing len.
 *
 * Copies exactly data_size bytes from data into the slot at index, replacing
 * the current value. The array length is unchanged. Use push_at_array to
 * insert a new element and shift existing ones.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param index  Zero-based index of the slot to overwrite. Must be < array->len.
 * @param data   Pointer to the replacement value. Must not be NULL.
 *               Must point to at least array->data_size bytes.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array or data is NULL
 *         - TYPE_MISMATCH if dtype != array->dtype
 *         - OUT_OF_BOUNDS if index >= array->len
 */
error_code_t set_array_index(array_t*    array,
                             size_t      index,
                             const void* data,
                             dtype_id_t  dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Create a deep copy of src into a newly initialized array.
 *
 * Allocates a new array_t and a new data buffer through the provided allocator
 * vtable, then copies all len elements from src into the new array. The copy
 * has the same dtype, data_size, and growth setting as src. The caller is
 * responsible for calling return_array on the returned array when done.
 *
 * @param src      Pointer to the source array. Must not be NULL.
 * @param alloc_v  Allocator vtable to use for the new array's memory. The
 *                 copy may use a different allocator than src.
 *
 * @return array_expect_t with has_value true and a valid array_t* on success.
 *         On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER  if src is NULL or alloc_v.allocate is NULL
 *         - BAD_ALLOC     if the allocator fails to allocate the new struct
 *         - OUT_OF_MEMORY if the allocator fails to allocate the new data buffer
 */
array_expect_t copy_array(const array_t*     src,
                          allocator_vtable_t alloc_v);

// ================================================================================
// Type query and concatenation
// ================================================================================

/**
 * @brief Return the dtype_id_t of the array.
 *
 * Convenience getter for the type identifier fixed at init time. Equivalent
 * to reading array->dtype directly but consistent with the rest of the API.
 *
 * @param array  Pointer to the array.
 * @return The dtype_id_t of the array, or UNKNOWN_TYPE if array is NULL.
 */
dtype_id_t array_type(const array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Append all elements of src onto the end of dst in a single operation.
 *
 * The two arrays must share the same dtype. If dst does not have sufficient
 * spare capacity and growth is true, dst's data buffer is reallocated once to
 * accommodate all of src's elements before any data is copied. If dst does not
 * have sufficient spare capacity and growth is false, the operation fails with
 * CAPACITY_OVERFLOW and dst is left unmodified. If src is empty the function
 * returns NO_ERROR immediately without modifying dst.
 *
 * @param dst  Pointer to the destination array. Must not be NULL.
 * @param src  Pointer to the source array. Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if dst or src is NULL
 *         - TYPE_MISMATCH     if dst->dtype != src->dtype
 *         - CAPACITY_OVERFLOW if dst has insufficient capacity and growth is false
 *         - OUT_OF_MEMORY     if the reallocation required for growth fails
 *         - LENGTH_OVERFLOW   if the combined length would overflow size_t
 */
error_code_t concat_array(array_t*       dst,
                          const array_t* src);

// --------------------------------------------------------------------------------

/**
 * @brief Allocate a new array containing a copy of a sub-range of src.
 *
 * Copies elements from src in the half-open range [start, end) into a newly
 * allocated array. The result has the same dtype, data_size, and growth
 * setting as src, and its capacity equals exactly (end - start) elements.
 * The caller is responsible for calling return_array on the result when done.
 *
 * Example: slice_array(arr, 1, 4, alloc_v) copies elements at indices 1, 2, 3.
 *
 * @param src      Pointer to the source array. Must not be NULL.
 * @param start    Index of the first element to include (inclusive).
 * @param end      Index one past the last element to include (exclusive).
 *                 Must be > start and <= src->len.
 * @param alloc_v  Allocator vtable to use for the new array's memory.
 *
 * @return array_expect_t with has_value true and a valid array_t* on success.
 *         On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER  if src is NULL or alloc_v.allocate is NULL
 *         - INVALID_ARG   if start >= end
 *         - OUT_OF_BOUNDS if end > src->len
 *         - BAD_ALLOC     if the allocator fails to allocate the new struct
 *         - OUT_OF_MEMORY if the allocator fails to allocate the new data buffer
 */
array_expect_t slice_array(const array_t*     src,
                           size_t             start,
                           size_t             end,
                           allocator_vtable_t alloc_v);

// ================================================================================
// Search
// ================================================================================

/**
 * @brief Search for the first element matching needle in the range [start, end).
 *
 * Performs an exact bitwise comparison of each element against needle using
 * the best available SIMD instruction set detected at compile time. The SIMD
 * fast path covers element sizes of 1, 2, 4, and 8 bytes; all other sizes
 * fall through to a scalar memcmp loop. Floating point types are compared by
 * exact bit pattern — no epsilon tolerance is applied.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param needle Pointer to the value to search for. Must not be NULL.
 *               Must point to exactly array->data_size bytes.
 * @param start  Index of the first element to include in the search (inclusive).
 * @param end    Index one past the last element to include (exclusive).
 *               Must be > start and <= array->len.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return size_expect_t with has_value true and the index of the first match
 *         on success. On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER  if array or needle is NULL
 *         - TYPE_MISMATCH if dtype != array->dtype
 *         - INVALID_ARG   if start >= end
 *         - OUT_OF_BOUNDS if end > array->len
 *         - NOT_FOUND     if no matching element exists in [start, end)
 */
size_expect_t array_contains(const array_t* array,
                             const void*   needle,
                             size_t        start,
                             size_t        end,
                             dtype_id_t    dtype);
// -------------------------------------------------------------------------------- 

/**
 * @brief Search for the first occurrence of needle using binary search.
 *
 * When sort is true the array is sorted in place before searching using the
 * provided comparator — the sorted order persists after the call. When sort
 * is false the array must already be sorted in ascending order according to
 * cmp; behaviour is undefined if it is not. After locating any matching
 * element the implementation scans leftward to guarantee the index of the
 * first occurrence is returned, consistent with array_contains.
 *
 * The comparator follows the qsort(3) convention:
 *   - Returns < 0 if a should sort before b.
 *   - Returns   0 if a and b are equal.
 *   - Returns > 0 if a should sort after b.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param needle Pointer to the value to search for. Must not be NULL.
 *               Must point to exactly array->data_size bytes.
 * @param cmp    Comparator function. Must not be NULL.
 * @param sort   If true, sort the array in place before searching.
 *               If false, the array must already be sorted ascending by cmp.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return size_expect_t with has_value true and the index of the first match
 *         on success. On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER  if array, needle, or cmp is NULL
 *         - TYPE_MISMATCH if dtype != array->dtype
 *         - EMPTY         if array->len == 0
 *         - NOT_FOUND     if no matching element exists in the array
 */
size_expect_t binary_search_array(array_t*    array,
                                   const void* needle,
                                   int       (*cmp)(const void*, const void*),
                                   bool        sort,
                                   dtype_id_t  dtype);
// -------------------------------------------------------------------------------- 

/**
 * @brief Find the bracketing indices around needle using binary search.
 *
 * Locates the largest element <= needle (lower bound) and the smallest
 * element >= needle (upper bound) in the array. If needle exists in the array
 * the first occurrence is returned as both lower and upper (lower == upper).
 * If needle falls below the minimum element or above the maximum element the
 * call returns has_value false with NOT_FOUND — the bracket is undefined when
 * needle is outside the range of the array's values.
 *
 * When sort is true the array is sorted in place before searching using the
 * provided comparator — the sorted order persists after the call. When sort
 * is false the array must already be sorted in ascending order according to
 * cmp; behaviour is undefined if it is not.
 *
 * The comparator follows the qsort(3) convention:
 *   - Returns < 0 if a should sort before b.
 *   - Returns   0 if a and b are equal.
 *   - Returns > 0 if a should sort after b.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param needle Pointer to the value to bracket. Must not be NULL.
 *               Must point to exactly array->data_size bytes.
 * @param cmp    Comparator function. Must not be NULL.
 * @param sort   If true, sort the array in place before searching.
 *               If false, the array must already be sorted ascending by cmp.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return bracket_expect_t with has_value true on success:
 *           u.value.lower — index of the largest element  <= needle.
 *           u.value.upper — index of the smallest element >= needle.
 *           lower == upper indicates needle was found exactly; both point
 *           to the first occurrence.
 *         On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER  if array, needle, or cmp is NULL
 *         - TYPE_MISMATCH if dtype != array->dtype
 *         - EMPTY         if array->len == 0
 *         - NOT_FOUND     if needle is outside [min, max] of the array
 */
bracket_expect_t binary_bracket_array(array_t*    array,
                                       const void* needle,
                                       int       (*cmp)(const void*, const void*),
                                       bool        sort,
                                       dtype_id_t  dtype);

// ================================================================================
// Introspection
// ================================================================================

/**
 * @brief Return the number of elements currently stored in the array.
 *
 * @param array  Pointer to the array.
 * @return Number of elements, or 0 if array is NULL.
 */
size_t array_size(const array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return the current allocated capacity of the array in elements.
 *
 * @param array  Pointer to the array.
 * @return Allocated capacity in elements, or 0 if array is NULL.
 */
size_t array_alloc(const array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return the size of one element in bytes.
 *
 * @param array  Pointer to the array.
 * @return Element size in bytes, or 0 if array is NULL.
 */
size_t array_data_size(const array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if the array contains no elements.
 *
 * @param array  Pointer to the array.
 * @return true if len == 0 or array is NULL, false otherwise.
 */
bool is_array_empty(const array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if the array is at full capacity.
 *
 * @param array  Pointer to the array.
 * @return true if len == alloc or array is NULL, false otherwise.
 */
bool is_array_full(const array_t* array);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reverse the elements of the array in place.
 *
 * Reverses the order of all len elements using the best available SIMD
 * instruction set detected at compile time, falling back to a scalar swap
 * loop when no SIMD path applies (large or non-power-of-two element sizes,
 * or non-SIMD targets). The array's len, alloc, dtype, and allocator are
 * unchanged.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if array is NULL
 *         - EMPTY        if array->len < 2 (nothing to reverse)
 */
error_code_t reverse_array(array_t* array);
// -------------------------------------------------------------------------------- 

/**
 * @brief Sort the elements of the array in place using an introsort strategy.
 *
 * Implements median-of-three quicksort with an insertion sort fallback for
 * partitions of fewer than 10 elements and tail-call optimisation to bound
 * stack depth. The sort is not stable — equal elements may change relative
 * order. The algorithm is generic: it operates on raw bytes using data_size
 * as the element stride and delegates all comparisons to the caller-supplied
 * comparator.
 *
 * The comparator follows the same convention as qsort(3):
 *   - Returns < 0 if the element at a should come before the element at b.
 *   - Returns   0 if the elements are equal.
 *   - Returns > 0 if the element at a should come after the element at b.
 * When direction is REVERSE the comparator result is negated internally so
 * the caller always writes a natural ascending comparator regardless of
 * direction.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param cmp    Comparator function. Must not be NULL. Receives pointers to
 *               two elements within the array's data buffer.
 * @param dir    FORWARD for ascending order, REVERSE for descending.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if array or cmp is NULL
 *         - EMPTY        if array->len < 2 (nothing to sort)
 */
error_code_t sort_array(array_t* array,
                         int    (*cmp)(const void*, const void*),
                         direction_t dir);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return true if ptr points to a valid element within the live region
 *        of the array's data buffer.
 *
 * A pointer is considered valid if all of the following hold:
 *   1. array and ptr are non-NULL.
 *   2. ptr >= array->data  (at or after the start of the buffer).
 *   3. ptr <  array->data + array->len * array->data_size  (before end of
 *      live region — does not include spare capacity).
 *   4. (ptr - array->data) is an exact multiple of array->data_size
 *      (ptr is aligned to an element boundary, not mid-element).
 *
 * This does NOT validate pointers into spare capacity beyond array->len.
 *
 * @param array  Pointer to the array.
 * @param ptr    Pointer to test.
 * @return true if ptr is a valid element-aligned pointer within the live
 *         region, false otherwise (including when either argument is NULL).
 */
bool is_array_ptr(const array_t* array, const void* ptr);
// -------------------------------------------------------------------------------- 

// ================================================================================
// Reduction operations
// ================================================================================

/**
 * @brief Find the index of the minimum element using a caller-supplied comparator.
 *
 * Scans all len elements and returns the index of the element for which cmp
 * returns the most negative result against every other element. When two or
 * more elements compare equal as the minimum, the index of the first one
 * (lowest index) is returned, consistent with the behaviour of array_contains.
 * The array is not modified.
 *
 * The comparator follows the qsort(3) convention:
 *   - Returns < 0 if a should sort before b  (a is "smaller").
 *   - Returns   0 if a and b are equal.
 *   - Returns > 0 if a should sort after b   (a is "larger").
 *
 * The inner scan dispatches to a SIMD fast path for element sizes of 1, 2, 4,
 * and 8 bytes when a compatible instruction set is detected at compile time.
 * All other element sizes fall back to the scalar comparator loop.
 *
 * @param array  Pointer to the array to scan. Must not be NULL.
 * @param cmp    Comparator function. Must not be NULL. Receives pointers to two
 *               elements within the array's data buffer.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return size_expect_t with has_value true and u.value == index of the minimum
 *         element on success. On failure, has_value is false and u.error is:
 *         - NULL_POINTER  if array or cmp is NULL
 *         - TYPE_MISMATCH if dtype != array->dtype
 *         - EMPTY         if array->len == 0
 *
 * @code
 *     allocator_vtable_t alloc = heap_allocator();
 *     // Assume arr is a uint16_array_t already populated with [300, 100, 200].
 *     // Use the typed wrapper min instead — this documents the generic layer.
 *     static int cmp_u16(const void* a, const void* b) {
 *         uint16_t va = *(const uint16_t*)a, vb = *(const uint16_t*)b;
 *         return (va > vb) - (va < vb);
 *     }
 *     size_expect_t r = array_min(&arr->base, cmp_u16, UINT16_TYPE);
 *     // r.has_value == true, r.u.value == 1  (value 100 is at index 1).
 * @endcode
 */
size_expect_t array_min(const array_t* array,
                        int          (*cmp)(const void*, const void*),
                        dtype_id_t     dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Find the index of the maximum element using a caller-supplied comparator.
 *
 * Scans all len elements and returns the index of the element for which cmp
 * returns the most positive result against every other element. When two or
 * more elements compare equal as the maximum, the index of the first one
 * (lowest index) is returned, consistent with the behaviour of array_contains.
 * The array is not modified.
 *
 * The comparator follows the qsort(3) convention:
 *   - Returns < 0 if a should sort before b  (a is "smaller").
 *   - Returns   0 if a and b are equal.
 *   - Returns > 0 if a should sort after b   (a is "larger").
 *
 * The inner scan dispatches to a SIMD fast path for element sizes of 1, 2, 4,
 * and 8 bytes when a compatible instruction set is detected at compile time.
 * All other element sizes fall back to the scalar comparator loop.
 *
 * @param array  Pointer to the array to scan. Must not be NULL.
 * @param cmp    Comparator function. Must not be NULL. Receives pointers to two
 *               elements within the array's data buffer.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return size_expect_t with has_value true and u.value == index of the maximum
 *         element on success. On failure, has_value is false and u.error is:
 *         - NULL_POINTER  if array or cmp is NULL
 *         - TYPE_MISMATCH if dtype != array->dtype
 *         - EMPTY         if array->len == 0
 *
 * @code
 *     // Continuing the example above:
 *     size_expect_t r = array_max(&arr->base, cmp_u16, UINT16_TYPE);
 *     // r.has_value == true, r.u.value == 0  (value 300 is at index 0).
 * @endcode
 */
size_expect_t array_max(const array_t* array,
                        int          (*cmp)(const void*, const void*),
                        dtype_id_t     dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Accumulate all elements into a caller-provided buffer using add.
 *
 * Iterates over all len elements and calls add(accum, &element) once per
 * element. The caller is responsible for:
 *   1. Providing accum pointing to a buffer large enough to hold one element
 *      of the type being accumulated (or a wider accumulator type if the add
 *      function widens internally).
 *   2. Initialising *accum to the identity value for the addition operation
 *      (e.g. 0 for integer/float summation) before calling this function.
 *   3. Reading the final result from *accum after the call returns.
 *
 * The add callback signature is:
 *   void add(void* accum, const void* element)
 * where accum points to the running total and element points to the current
 * array element. Both pointers are guaranteed non-NULL during the callback.
 *
 * The inner loop dispatches to a SIMD fast path for element sizes of 1, 2, 4,
 * and 8 bytes when a compatible instruction set is detected at compile time,
 * using the SIMD path to load batches and then calling add on each loaded
 * scalar. For all other element sizes the loop falls back to scalar iteration.
 *
 * @param array  Pointer to the array to sum. Must not be NULL.
 * @param accum  Pointer to the accumulator buffer. Must not be NULL.
 *               Must remain valid for the duration of the call.
 * @param add    Callback that adds one element into *accum. Must not be NULL.
 * @param dtype  Type identifier. Must match array->dtype.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array, accum, or add is NULL
 *         - TYPE_MISMATCH if dtype != array->dtype
 *         - EMPTY         if array->len == 0
 *
 * @code
 *     // Sum a uint16_array_t containing [100, 200, 300].
 *     static void add_u16(void* accum, const void* elem) {
 *         *(uint32_t*)accum += *(const uint16_t*)elem;
 *     }
 *     uint32_t total = 0;
 *     array_sum(&arr->base, &total, add_u16, UINT16_TYPE);
 *     // total == 600.
 * @endcode
 */
error_code_t array_sum(const array_t* array,
                       void*          accum,
                       void         (*add)(void* accum, const void* element),
                       dtype_id_t     dtype);

// --------------------------------------------------------------------------------

/**
 * @brief Allocate a new array holding the cumulative (prefix) sum of src.
 *
 * Allocates a new array_t of the same dtype and len as src and fills it so
 * that result[i] = src[0] + src[1] + ... + src[i]. The result is computed
 * using the caller-supplied add callback, which has the same signature as for
 * array_sum. The caller is responsible for calling return_array on the
 * returned array when it is no longer needed.
 *
 * The first element of the output is always a copy of src[0] regardless of
 * any accumulator identity — the function does not require the caller to
 * supply an identity element. Each subsequent output element is produced by
 * calling add(out_ptr, src_ptr) where out_ptr points to the previous output
 * element and src_ptr points to the current input element, then copying the
 * updated accumulator into the output slot.
 *
 * @param src      Pointer to the input array. Must not be NULL.
 * @param add      Callback that adds one element into an accumulator.
 *                 Must not be NULL. The accumulator is always an element of
 *                 the same type as src (not a wider type): add(accum, elem)
 *                 where *accum is a src-typed value being updated in place.
 * @param alloc_v  Allocator vtable for the new output array.
 *                 alloc_v.allocate must not be NULL.
 * @param dtype    Type identifier. Must match src->dtype.
 *
 * @return array_expect_t with has_value true and a valid array_t* on success.
 *         The returned array has the same dtype, data_size, len, and alloc as
 *         src, and growth is false (the result is a fixed-length snapshot).
 *         On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER  if src, add, or alloc_v.allocate is NULL
 *         - TYPE_MISMATCH if dtype != src->dtype
 *         - EMPTY         if src->len == 0
 *         - BAD_ALLOC     if the allocator fails to allocate the output struct
 *         - OUT_OF_MEMORY if the allocator fails to allocate the output buffer
 *
 * @code
 *     // Cumulative sum of a uint16_array_t containing [1, 2, 3, 4].
 *     static void add_u16(void* accum, const void* elem) {
 *         *(uint16_t*)accum += *(const uint16_t*)elem;
 *     }
 *     allocator_vtable_t alloc = heap_allocator();
 *     array_expect_t r = cumulative_array(&arr->base, add_u16, alloc, UINT16_TYPE);
 *     // result contains [1, 3, 6, 10].
 *     return_array(r.u.value);
 * @endcode
 */
array_expect_t cumulative_array(const array_t*     src,
                                void             (*add)(void* accum,
                                                        const void* element),
                                allocator_vtable_t alloc_v,
                                dtype_id_t         dtype);
// -------------------------------------------------------------------------------- 

/**
 * @brief Compare two arrays for exact equality.
 *
 * Two arrays are considered equal if:
 *   1. Both are non-NULL
 *   2. They have the same length
 *   3. They have the same dtype
 *   4. Their element data matches exactly (byte-wise comparison)
 *
 * This function performs an exact bitwise comparison of the underlying
 * data buffer. No tolerance is applied for floating point types.
 *
 * @param a  Pointer to the first array.
 * @param b  Pointer to the second array.
 * @return true if arrays are equal, false otherwise.
 */
bool array_equal(const array_t* a, const array_t* b);
// ================================================================================ 
// ================================================================================ 

/**
 * @brief A generic, type-safe binary heap backed by a dynamic array.
 *
 * heap_t wraps an array_t with a caller-supplied comparator that determines
 * the heap's ordering. Passing a comparator that returns > 0 when *a > *b
 * produces a max-heap (largest element at the root); passing one that returns
 * > 0 when *a < *b produces a min-heap. Any total ordering consistent with
 * the qsort(3) convention is accepted.
 *
 * The dtype and data_size are fixed at init time and stored on the struct so
 * that push, pop, peek, and foreach operations require no additional type
 * arguments from the caller. Do not modify any field directly — use the
 * provided API functions.
 *
 * Memory is managed entirely through the embedded allocator_vtable_t inherited
 * from the inner array_t. The heap_t struct itself is also allocated through
 * the same vtable, matching the pattern used by array_t.
 */
typedef struct {
    array_t*   data;                         /**< Backing storage for heap elements.      */
    dtype_id_t dtype;                        /**< Runtime type identity, fixed at init.   */
    int      (*cmp)(const void*, const void*); /**< Comparator defining heap order.       */
} heap_t;
 
// ================================================================================
// Expected return type
// ================================================================================
 
/**
 * @brief Expected return type for heap initialization and copy operations.
 *
 * On success, has_value is true and u.value points to a valid heap_t.
 * On failure, has_value is false and u.error contains the error code.
 */
typedef struct {
    bool has_value;
    union {
        heap_t*      value;
        error_code_t error;
    } u;
} heap_expect_t;
 
// ================================================================================
// Initialization and teardown
// ================================================================================
 
/**
 * @brief Initialize a new binary heap.
 *
 * Allocates a heap_t struct and its backing array_t (including data buffer)
 * through the provided allocator vtable. The dtype must be registered in the
 * dtype registry before calling this function. The comparator cmp determines
 * heap ordering and must remain valid for the lifetime of the heap.
 *
 * The comparator follows the qsort(3) convention:
 *   - Returns < 0 if *a should be closer to the root than *b.
 *   - Returns   0 if *a and *b are considered equal in priority.
 *   - Returns > 0 if *b should be closer to the root than *a.
 *
 * Pass a comparator returning (*(T*)a > *(T*)b) - (*(T*)a < *(T*)b) for a
 * max-heap, or its negation for a min-heap.
 *
 * If growth is true, the backing array expands automatically when full using
 * the tiered growth strategy of array_t. If growth is false, heap_push returns
 * CAPACITY_OVERFLOW once the initial capacity is exhausted.
 *
 * @param capacity  Initial number of elements to allocate space for. Must be > 0.
 * @param dtype     Type identifier. Must be registered in the dtype registry.
 * @param growth    If true, the backing array grows automatically when full.
 * @param alloc_v   Allocator vtable used for all memory operations.
 * @param cmp       Comparator function defining heap order. Must not be NULL.
 *
 * @return heap_expect_t with has_value true and a valid heap_t* on success.
 *         On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER    if alloc_v.allocate or cmp is NULL
 *         - INVALID_ARG     if capacity is 0 or dtype is UNKNOWN_TYPE
 *         - ILLEGAL_STATE   if the dtype registry could not be initialized
 *         - TYPE_MISMATCH   if dtype is not registered in the dtype registry
 *         - LENGTH_OVERFLOW if capacity * data_size would overflow size_t
 *         - BAD_ALLOC       if the allocator fails to allocate the heap_t struct
 *         - OUT_OF_MEMORY   if the allocator fails to allocate the data buffer
 *
 * @code
 *     static int cmp_int(const void* a, const void* b) {
 *         int va = *(const int*)a, vb = *(const int*)b;
 *         return (va > vb) - (va < vb);   // max-heap
 *     }
 *     allocator_vtable_t alloc = heap_allocator();
 *     heap_expect_t r = init_heap(16, INT32_TYPE, true, alloc, cmp_int);
 *     if (!r.has_value) { handle_error(r.u.error); }
 *     heap_t* h = r.u.value;
 * @endcode
 */
heap_expect_t init_heap(size_t             capacity,
                        dtype_id_t         dtype,
                        bool               growth,
                        allocator_vtable_t alloc_v,
                        int              (*cmp)(const void*, const void*));
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Return a heap's memory back to its allocator.
 *
 * Delegates to return_array to release the backing array_t and its data buffer,
 * then returns the heap_t struct itself to the allocator via return_element.
 * This does NOT free or destroy the allocator itself. After this call the
 * pointer must not be used.
 *
 * @param heap  Pointer to the heap to return. Silently ignored if NULL.
 */
void return_heap(heap_t* heap);
 
// ================================================================================
// Mutation operations
// ================================================================================
 
/**
 * @brief Insert one element into the heap and restore the heap property.
 *
 * Appends the new element to the end of the backing array and then sifts it
 * upward until the comparator is satisfied at every parent-child pair. The
 * element is copied by value; the caller retains ownership of the buffer
 * pointed to by data.
 *
 * If the backing array is full and growth is true, the data buffer is
 * reallocated before insertion. If growth is false, returns CAPACITY_OVERFLOW
 * without modifying the heap.
 *
 * @param heap  Pointer to the target heap. Must not be NULL.
 * @param data  Pointer to the element to insert. Must not be NULL.
 *              The pointed-to data must be exactly heap->data->data_size bytes.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if heap or data is NULL
 *         - CAPACITY_OVERFLOW if the heap is full and growth is false
 *         - OUT_OF_MEMORY     if growth reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code
 *     int val = 42;
 *     error_code_t err = heap_push(h, &val);
 * @endcode
 */
error_code_t heap_push(heap_t* heap, const void* data);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Remove the root element from the heap and restore the heap property.
 *
 * Copies the root element into the caller-supplied buffer out, replaces the
 * root with the last element, decrements the length, and then sifts the new
 * root downward until the heap property is restored. The removed value is
 * written to out before any modification of the internal buffer, so the
 * caller's buffer may safely alias stack memory.
 *
 * @param heap  Pointer to the target heap. Must not be NULL.
 * @param out   Caller-provided buffer to receive the removed root element.
 *              Must not be NULL. Must be at least heap->data->data_size bytes.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if heap or out is NULL
 *         - EMPTY        if the heap contains no elements
 *
 * @code
 *     int top;
 *     error_code_t err = heap_pop(h, &top);
 *     // top now holds the former root (highest-priority element).
 * @endcode
 */
error_code_t heap_pop(heap_t* heap, void* out);
 
// ================================================================================
// Inspection operations
// ================================================================================
 
/**
 * @brief Return a read-only pointer to the root (highest-priority) element.
 *
 * Returns a direct pointer into the backing array's internal buffer. The
 * pointer is valid only until the next mutation (heap_push or heap_pop).
 * Do not free or write through the returned pointer.
 *
 * @param heap  Pointer to the heap to inspect. Must not be NULL.
 *
 * @return Pointer to the root element on success, or NULL if heap is NULL
 *         or the heap is empty.
 *
 * @code
 *     const int* top = (const int*)heap_peek(h);
 *     if (top) printf("root = %d\n", *top);
 * @endcode
 */
const void* heap_peek(const heap_t* heap);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Return the number of elements currently stored in the heap.
 *
 * Delegates to the backing array's len field. Returns 0 if heap is NULL.
 *
 * @param heap  Pointer to the heap to query. Must not be NULL.
 * @return Number of elements in the heap, or 0 if heap is NULL.
 */
size_t heap_size(const heap_t* heap);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Return the total number of elements the heap can hold without
 *        reallocation.
 *
 * Delegates to the backing array's alloc field. This is the currently
 * allocated capacity, not the number of elements stored. If growth is true,
 * this value increases automatically as elements are pushed. Returns 0 if
 * heap is NULL.
 *
 * @param heap  Pointer to the heap to query.
 * @return Allocated capacity in number of elements, or 0 if heap is NULL.
 *
 * @code
 *     size_t used  = heap_size(h);
 *     size_t cap   = heap_alloc(h);
 *     size_t spare = cap - used;
 * @endcode
 */
size_t heap_alloc(const heap_t* heap);
 
// ================================================================================
// Iteration
// ================================================================================
 
/**
 * @brief Visit every element in the heap, calling fn once per element.
 *
 * Iterates over the backing array's internal buffer in heap-internal order,
 * which satisfies the heap property but is not sorted order. The traversal
 * order is unspecified beyond this guarantee — callers must not rely on
 * elements being visited highest-priority first.
 *
 * If ordered traversal is required, copy the heap with copy_heap and pop
 * elements one at a time from the copy instead.
 *
 * The callback signature is:
 *   void fn(const void* element, void* ctx)
 * where element points to one element within the backing buffer (valid only
 * for the duration of the callback) and ctx is the opaque pointer supplied
 * by the caller, which may be used to accumulate results, filter, or pass
 * any other state into the callback without globals.
 *
 * The heap must not be mutated during iteration. Behaviour is undefined if
 * fn calls heap_push or heap_pop on the same heap.
 *
 * @param heap  Pointer to the heap to iterate. Must not be NULL.
 * @param fn    Callback invoked once per element. Must not be NULL.
 * @param ctx   Opaque pointer forwarded to every fn call. May be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if heap or fn is NULL
 *         - EMPTY        if the heap contains no elements
 *
 * @code
 *     // Print every integer in the heap (in unspecified order).
 *     static void print_int(const void* elem, void* ctx) {
 *         (void)ctx;
 *         printf("%d\n", *(const int*)elem);
 *     }
 *     heap_foreach(h, print_int, NULL);
 *
 *     // Accumulate a sum using ctx.
 *     static void sum_int(const void* elem, void* ctx) {
 *         *(long*)ctx += *(const int*)elem;
 *     }
 *     long total = 0;
 *     heap_foreach(h, sum_int, &total);
 * @endcode
 */
error_code_t heap_foreach(const heap_t* heap,
                          void        (*fn)(const void* element, void* ctx),
                          void*         ctx);
 
// ================================================================================
// Copy
// ================================================================================
 
/**
 * @brief Create a deep copy of a heap using a (possibly different) allocator.
 *
 * Allocates a new heap_t struct and a new backing array_t (including its data
 * buffer) through alloc_v, then copies all len elements from src into the new
 * heap. The copy shares the same dtype, growth setting, and comparator pointer
 * as src. The comparator pointer is copied by value — function pointers carry
 * no ownership and require no deep copy.
 *
 * The returned heap is independent of src: mutations to either heap do not
 * affect the other, and each must be individually returned via return_heap.
 *
 * A common use case is ordered traversal: copy the heap, then pop from the
 * copy in a loop to visit elements in priority order without destroying the
 * original.
 *
 * @param src      Pointer to the source heap. Must not be NULL.
 * @param alloc_v  Allocator vtable for all memory operations on the new heap.
 *                 May differ from src's allocator. alloc_v.allocate must not
 *                 be NULL.
 *
 * @return heap_expect_t with has_value true and a valid heap_t* on success.
 *         On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER  if src is NULL or alloc_v.allocate is NULL
 *         - BAD_ALLOC     if the allocator fails to allocate the new heap_t
 *         - OUT_OF_MEMORY if the allocator fails to allocate the new data buffer
 *
 * @code
 *     // Ordered traversal without destroying the original heap.
 *     allocator_vtable_t alloc = heap_allocator();
 *     heap_expect_t r = copy_heap(h, alloc);
 *     if (!r.has_value) { handle_error(r.u.error); }
 *     heap_t* tmp = r.u.value;
 *     int val;
 *     while (heap_size(tmp) > 0) {
 *         heap_pop(tmp, &val);
 *         printf("%d\n", val);   // printed in priority order
 *     }
 *     return_heap(tmp);
 * @endcode
 */
heap_expect_t copy_heap(const heap_t* src, allocator_vtable_t alloc_v);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* array_H */
// ================================================================================
// ================================================================================
// eof
