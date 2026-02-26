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
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* array_H */
// ================================================================================
// ================================================================================
// eof
