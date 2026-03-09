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
 * @brief A type-safe dynamic array of long double elements.
 *
 * Wraps array_t with the dtype fixed to LDOUBLE_TYPE at initialization.
 * All operations enforce LDOUBLE_TYPE at the call site so the caller never
 * needs to pass a dtype_id_t argument.
 *
 * Floating-point semantics:
 *   - Equality comparisons (ldouble_array_contains) use exact bit patterns in
 *     exact mode (both tolerances 0.0L). No SIMD acceleration is available for
 *     long double on any architecture; both ldouble_array_contains and
 *     ldouble_array_binary_search execute as portable scalar loops.
 *   - NaN values do not compare equal in exact mode; -0.0L and +0.0L are
 *     treated as distinct in exact mode and equal in tolerance mode.
 *   - Sort, binary_search, binary_bracket, min, and max use the three-way
 *     comparator (va > vb) - (va < vb). NaN values produce unspecified
 *     ordering — callers must ensure the array contains no NaN before using
 *     these operations.
 *   - The size of long double is platform-dependent (10 bytes on x87,
 *     12 or 16 bytes including padding on Linux/macOS x86-64, 16 bytes on
 *     ARM64). ldouble_array_data_size reports the actual sizeof(long double).
 */
typedef struct {
    array_t base; /**< Underlying generic array. Do not modify directly. */
} ldouble_array_t;

// ================================================================================
// Expected types
// ================================================================================

/** @brief Expected return type for ldouble_array_t init, copy and slice. */
typedef struct {
    bool has_value;
    union {
        ldouble_array_t* value;
        error_code_t     error;
    } u;
} ldouble_array_expect_t;
// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialize a new heap-allocated ldouble_array_t.
 *
 * The element type is fixed to LDOUBLE_TYPE and the element size is cached as
 * sizeof(long double). See init_double_array for full parameter and return
 * documentation; behaviour is identical except for the element type.
 *
 * @param capacity  Initial element capacity. Must be > 0.
 * @param growth    true to allow automatic buffer growth on overflow.
 * @param alloc_v   Allocator vtable. alloc_v.allocate must not be NULL.
 *
 * @return ldouble_array_expect_t: has_value true + u.value on success, or
 *         has_value false + u.error (NULL_POINTER, INVALID_ARG, BAD_ALLOC,
 *         OUT_OF_MEMORY, LENGTH_OVERFLOW, ILLEGAL_STATE) on failure.
 *
 * @code
 *     allocator_vtable_t alloc = heap_allocator();
 *     ldouble_array_expect_t r = init_ldouble_array(16, true, alloc);
 *     ldouble_array_t* arr = r.u.value;
 *     push_back_ldouble_array(arr, 3.14159265358979323846L);
 *     return_ldouble_array(arr);
 * @endcode
 */
ldouble_array_expect_t init_ldouble_array(size_t             capacity,
                                          bool               growth,
                                          allocator_vtable_t alloc_v);

// --------------------------------------------------------------------------------

/**
 * @brief Release all memory owned by a ldouble_array_t.
 *
 * Frees the data buffer then the struct. NULL is safe and has no effect.
 *
 * @param array  Pointer to release. May be NULL.
 */
void return_ldouble_array(ldouble_array_t* array);

// ================================================================================
// Push operations
// ================================================================================

/**
 * @brief Append one long double to the end of the array. O(1) amortised.
 *
 * @param array  Must not be NULL.
 * @param value  The value to append.
 *
 * @return NO_ERROR, NULL_POINTER, CAPACITY_OVERFLOW, OUT_OF_MEMORY, or
 *         LENGTH_OVERFLOW.
 */
error_code_t push_back_ldouble_array(ldouble_array_t* array, long double value);

// --------------------------------------------------------------------------------

/**
 * @brief Prepend one long double to the front of the array. O(n).
 *
 * @param array  Must not be NULL.
 * @param value  The value to prepend.
 *
 * @return NO_ERROR, NULL_POINTER, CAPACITY_OVERFLOW, OUT_OF_MEMORY, or
 *         LENGTH_OVERFLOW.
 */
error_code_t push_front_ldouble_array(ldouble_array_t* array, long double value);

// --------------------------------------------------------------------------------

/**
 * @brief Insert one long double at an arbitrary index. O(n).
 *
 * @param array  Must not be NULL.
 * @param index  Insertion position. Must be <= array->base.len.
 * @param value  The value to insert.
 *
 * @return NO_ERROR, NULL_POINTER, OUT_OF_BOUNDS, CAPACITY_OVERFLOW,
 *         OUT_OF_MEMORY, or LENGTH_OVERFLOW.
 */
error_code_t push_at_ldouble_array(ldouble_array_t* array,
                                   size_t           index,
                                   long double      value);

// ================================================================================
// Get operation
// ================================================================================

/**
 * @brief Copy the element at index into *out. O(1).
 *
 * @param array  Must not be NULL.
 * @param index  Must be < array->base.len.
 * @param out    Must not be NULL.
 *
 * @return NO_ERROR, NULL_POINTER, or OUT_OF_BOUNDS.
 */
error_code_t get_ldouble_array_index(const ldouble_array_t* array,
                                     size_t                 index,
                                     long double*           out);

// ================================================================================
// Pop operations
// ================================================================================

/**
 * @brief Remove and optionally return the last element. O(1).
 *
 * @param array  Must not be NULL.
 * @param out    Receives the removed value, or NULL to discard.
 *
 * @return NO_ERROR, NULL_POINTER, or EMPTY.
 */
error_code_t pop_back_ldouble_array(ldouble_array_t* array, long double* out);

// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally return the first element. O(n).
 *
 * @param array  Must not be NULL.
 * @param out    Receives the removed value, or NULL to discard.
 *
 * @return NO_ERROR, NULL_POINTER, or EMPTY.
 */
error_code_t pop_front_ldouble_array(ldouble_array_t* array, long double* out);

// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally return the element at an arbitrary index. O(n).
 *
 * @param array  Must not be NULL.
 * @param out    Receives the removed value, or NULL to discard.
 * @param index  Must be < array->base.len.
 *
 * @return NO_ERROR, NULL_POINTER, EMPTY, or OUT_OF_BOUNDS.
 */
error_code_t pop_any_ldouble_array(ldouble_array_t* array,
                                   long double*     out,
                                   size_t           index);

// ================================================================================
// Utility operations
// ================================================================================

/**
 * @brief Reset the array to empty without releasing the buffer. O(1).
 *
 * @return NO_ERROR or NULL_POINTER.
 */
error_code_t clear_ldouble_array(ldouble_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Overwrite the element at index with a new value. O(1).
 *
 * @return NO_ERROR, NULL_POINTER, or OUT_OF_BOUNDS.
 */
error_code_t set_ldouble_array_index(ldouble_array_t* array,
                                     size_t           index,
                                     long double      value);

// --------------------------------------------------------------------------------

/**
 * @brief Allocate an independent deep copy of src.
 *
 * @return ldouble_array_expect_t: has_value true on success, or has_value false
 *         with NULL_POINTER, BAD_ALLOC, or OUT_OF_MEMORY on failure.
 */
ldouble_array_expect_t copy_ldouble_array(const ldouble_array_t* src,
                                          allocator_vtable_t     alloc_v);

// --------------------------------------------------------------------------------

/**
 * @brief Append all elements of src to the end of dst.
 *
 * @return NO_ERROR, NULL_POINTER, CAPACITY_OVERFLOW, or OUT_OF_MEMORY.
 */
error_code_t concat_ldouble_array(ldouble_array_t*       dst,
                                  const ldouble_array_t* src);

// --------------------------------------------------------------------------------

/**
 * @brief Allocate a new array containing elements at indices [start, end).
 *
 * @return ldouble_array_expect_t: has_value true on success, or has_value false
 *         with NULL_POINTER, INVALID_ARG, OUT_OF_BOUNDS, BAD_ALLOC, or
 *         OUT_OF_MEMORY on failure.
 */
ldouble_array_expect_t slice_ldouble_array(const ldouble_array_t* src,
                                           size_t                 start,
                                           size_t                 end,
                                           allocator_vtable_t     alloc_v);

// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of all elements in place. O(n).
 *
 * @return NO_ERROR or NULL_POINTER.
 */
error_code_t reverse_ldouble_array(ldouble_array_t* array);

// ================================================================================
// Sort
// ================================================================================

/**
 * @brief Sort all elements in place in ascending or descending order.
 *
 * Uses the three-way comparator (va > vb) - (va < vb). NaN values produce
 * unspecified ordering. The sort is not stable. O(n log n) average.
 *
 * @param array  Must not be NULL.
 * @param dir    FORWARD for ascending, REVERSE for descending.
 *
 * @return NO_ERROR, NULL_POINTER, or EMPTY.
 */
error_code_t sort_ldouble_array(ldouble_array_t* array, direction_t dir);

// ================================================================================
// Reduction
// ================================================================================

/**
 * @brief Find the minimum value in the array.
 *
 * NaN values produce unspecified results. -0.0L and +0.0L compare equal; the
 * one at the lower index is returned if both are present.
 *
 * @param array  Must not be NULL and must contain at least one element.
 *
 * @return ldouble_expect_t: has_value true + u.value on success, or has_value
 *         false + u.error (NULL_POINTER or EMPTY) on failure.
 */
ldouble_expect_t ldouble_array_min(const ldouble_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Find the maximum value in the array.
 *
 * @param array  Must not be NULL and must contain at least one element.
 *
 * @return ldouble_expect_t: has_value true + u.value on success, or has_value
 *         false + u.error (NULL_POINTER or EMPTY) on failure.
 */
ldouble_expect_t ldouble_array_max(const ldouble_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Sum all elements and return the result as a long double.
 *
 * NaN propagates from any NaN element. IEEE 754 rounding applies for large
 * arrays; consider Kahan summation externally if precision is critical.
 *
 * @param array  Must not be NULL.
 *
 * @return ldouble_expect_t: has_value true + u.value == sum on success, or
 *         has_value false + u.error (NULL_POINTER or EMPTY) on failure.
 */
ldouble_expect_t ldouble_array_sum(const ldouble_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Compute the inclusive prefix sum into a new long double array.
 *
 * output[0] == src[0]; output[i] == output[i-1] + src[i]. src is unchanged.
 * The caller owns the output and must call return_ldouble_array on it.
 *
 * @param src    Must not be NULL and must contain at least one element.
 * @param alloc  Allocator for the output array.
 *
 * @return ldouble_array_expect_t: has_value true on success, or has_value
 *         false with NULL_POINTER, EMPTY, BAD_ALLOC, or OUT_OF_MEMORY on
 *         failure.
 *
 * @code
 *     // src: [1.0L, -2.0L, 3.0L]  →  output: [1.0L, -1.0L, 2.0L]
 *     ldouble_array_expect_t cr = cumulative_ldouble_array(src, alloc);
 *     return_ldouble_array(cr.u.value);
 * @endcode
 */
ldouble_array_expect_t cumulative_ldouble_array(const ldouble_array_t* src,
                                                allocator_vtable_t     alloc);

// ================================================================================
// Search
// ================================================================================

/**
 * @brief Search for the first element matching value in [start, end).
 *
 * No SIMD acceleration is available for long double. This function executes
 * as a portable scalar loop.
 *
 * Comparison modes:
 *
 *   abs_tol == 0.0L && rel_tol == 0.0L  — exact bit-pattern match.
 *       NaN != NaN, -0.0L != +0.0L.
 *
 *   abs_tol >  0.0L && rel_tol == 0.0L  — absolute tolerance only.
 *       Match when |element - value| <= abs_tol.
 *
 *   abs_tol == 0.0L && rel_tol >  0.0L  — relative tolerance only.
 *       Match when |element - value| <= rel_tol * max(|element|, |value|).
 *
 *   abs_tol >  0.0L && rel_tol >  0.0L  — combined.
 *       Match when |element - value| <= max(abs_tol,
 *                                           rel_tol * max(|element|, |value|)).
 *
 * Negative tolerances are rejected with INVALID_ARG.
 *
 * @param array    Must not be NULL.
 * @param value    The long double value to search for.
 * @param start    First index to examine (inclusive).
 * @param end      One past the last index to examine (exclusive).
 *                 Must be > start and <= array->base.len.
 * @param abs_tol  Absolute tolerance >= 0.0L. Pass 0.0L to disable.
 * @param rel_tol  Relative tolerance >= 0.0L. Pass 0.0L to disable.
 *
 * @return size_expect_t: has_value true + u.value == first matching index on
 *         success, or has_value false + u.error (NULL_POINTER, INVALID_ARG,
 *         OUT_OF_BOUNDS, or NOT_FOUND) on failure.
 *
 * @code
 *     // Exact match:
 *     size_expect_t r = ldouble_array_contains(arr, 3.14L, 0, n, 0.0L, 0.0L);
 *
 *     // Absolute tolerance:
 *     r = ldouble_array_contains(arr, 1.0L, 0, n, 1e-15L, 0.0L);
 *
 *     // Relative tolerance (1%):
 *     r = ldouble_array_contains(arr, 1000.0L, 0, n, 0.0L, 0.01L);
 * @endcode
 */
size_expect_t ldouble_array_contains(const ldouble_array_t* array,
                                     long double            value,
                                     size_t                 start,
                                     size_t                 end,
                                     long double            abs_tol,
                                     long double            rel_tol);

// --------------------------------------------------------------------------------

/**
 * @brief Binary-search for the first element within tolerance of value.
 *
 * Two-phase algorithm: exact bisection to find the landing position, then a
 * scalar tolerance fan-out in both directions. Returns the lowest-index
 * element within tolerance. No SIMD acceleration is used.
 *
 * The array must be sorted (or sort must be true) using the exact three-way
 * long double comparator.
 *
 * @param array    Must not be NULL.
 * @param value    The long double to find. Must not be NaN.
 * @param sort     true to sort before searching (sorted order persists).
 * @param abs_tol  Absolute tolerance >= 0.0L. Pass 0.0L for exact match.
 * @param rel_tol  Relative tolerance >= 0.0L. Pass 0.0L for exact match.
 *
 * @return size_expect_t: has_value true + u.value == first matching index on
 *         success, or has_value false + u.error (NULL_POINTER, INVALID_ARG,
 *         EMPTY, or NOT_FOUND) on failure.
 *
 * @code
 *     // Exact match:
 *     size_expect_t r = ldouble_array_binary_search(arr, 2.5L, false, 0.0L, 0.0L);
 *
 *     // Within 1e-15L absolute:
 *     r = ldouble_array_binary_search(arr, 2.5L, false, 1e-15L, 0.0L);
 * @endcode
 */
size_expect_t ldouble_array_binary_search(ldouble_array_t* array,
                                          long double      value,
                                          bool             sort,
                                          long double      abs_tol,
                                          long double      rel_tol);

// --------------------------------------------------------------------------------

/**
 * @brief Find the bracketing indices for a value in a sorted long double array.
 *
 * Identical semantics to double_array_binary_bracket. The sort contract is
 * exact; no tolerance is applied.
 *
 * @param array  Must not be NULL.
 * @param value  The long double to bracket. Must not be NaN.
 * @param sort   true to sort before searching.
 *
 * @return bracket_expect_t: has_value true on success with:
 *           u.value.lower — first index whose element >= value
 *           u.value.upper — last  index whose element <= value
 *         On failure has_value false + u.error (NULL_POINTER, EMPTY, or
 *         NOT_FOUND).
 *
 * @code
 *     // arr: [-2.0L, -1.0L, 1.0L, 2.0L]
 *     bracket_expect_t r = ldouble_array_binary_bracket(arr, 0.0L, false);
 *     // r.u.value.lower == 2 (1.0L), r.u.value.upper == 1 (-1.0L)
 * @endcode
 */
bracket_expect_t ldouble_array_binary_bracket(ldouble_array_t* array,
                                              long double      value,
                                              bool             sort);

// ================================================================================
// Introspection
// ================================================================================

/** @brief Number of elements currently stored. Returns 0 if array is NULL. */
size_t ldouble_array_size(const ldouble_array_t* array);

/** @brief Number of elements the buffer can currently hold. Returns 0 if NULL. */
size_t ldouble_array_alloc(const ldouble_array_t* array);

/**
 * @brief Size in bytes of one element (sizeof(long double): 12 or 16 on
 *        x86-64, 16 on ARM64). Returns 0 if NULL.
 */
size_t ldouble_array_data_size(const ldouble_array_t* array);

/** @brief true if array is NULL or empty. */
bool is_ldouble_array_empty(const ldouble_array_t* array);

/** @brief true if array is NULL or len == alloc. */
bool is_ldouble_array_full(const ldouble_array_t* array);

/**
 * @brief true if ptr points into the live element region of array's buffer.
 *
 * @param array  May be NULL.
 * @param ptr    May be NULL.
 */
bool is_ldouble_array_ptr(const ldouble_array_t* array, const long double* ptr);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_ldouble_H */
// ================================================================================
// ================================================================================
// eof
