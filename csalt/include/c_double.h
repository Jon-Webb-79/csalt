// ================================================================================
// ================================================================================
// - File:    c_double.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 08, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_double_H
#define c_double_H

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
 * @brief A type-safe dynamic array of double elements.
 *
 * Wraps array_t with the dtype fixed to DOUBLE_TYPE at initialization.
 * All operations enforce DOUBLE_TYPE at the call site so the caller never
 * needs to pass a dtype_id_t argument.
 *
 * Floating-point semantics:
 *   - Equality comparisons (double_array_contains) use exact bit patterns —
 *     no epsilon tolerance is applied. Two NaN values do not compare equal;
 *     -0.0 and +0.0 have different bit patterns and are treated as distinct.
 *   - Sort, binary_search, binary_bracket, min, and max use the three-way
 *     comparator (va > vb) - (va < vb). NaN values produce unspecified
 *     ordering — callers must ensure the array contains no NaN before using
 *     these operations.
 */
typedef struct {
    array_t base; /**< Underlying generic array. Do not modify directly. */
} double_array_t;

// ================================================================================
// Expected types
// ================================================================================

/** @brief Expected return type for double_array_t init, copy and slice. */
typedef struct {
    bool has_value;
    union {
        double_array_t* value;
        error_code_t    error;
    } u;
} double_array_expect_t;
// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialize a new heap-allocated double_array_t.
 *
 * The element type is fixed to DOUBLE_TYPE and the element size is cached as
 * sizeof(double) (8 bytes on all IEEE 754 platforms). See init_float_array for
 * full parameter and return documentation; behaviour is identical except for the
 * element type.
 *
 * @param capacity  Initial element capacity. Must be > 0.
 * @param growth    true to allow automatic buffer growth on overflow.
 * @param alloc_v   Allocator vtable. alloc_v.allocate must not be NULL.
 *
 * @return double_array_expect_t: has_value true + u.value on success, or
 *         has_value false + u.error (NULL_POINTER, INVALID_ARG, BAD_ALLOC,
 *         OUT_OF_MEMORY, LENGTH_OVERFLOW, ILLEGAL_STATE) on failure.
 *
 * @code
 *     allocator_vtable_t alloc = heap_allocator();
 *     double_array_expect_t r = init_double_array(16, true, alloc);
 *     double_array_t* arr = r.u.value;
 *     push_back_double_array(arr, 3.14159265358979);
 *     return_double_array(arr);
 * @endcode
 */
double_array_expect_t init_double_array(size_t             capacity,
                                        bool               growth,
                                        allocator_vtable_t alloc_v);

// --------------------------------------------------------------------------------

/**
 * @brief Release all memory owned by a double_array_t.
 *
 * Frees the data buffer then the struct. NULL is safe and has no effect.
 *
 * @param array  Pointer to release. May be NULL.
 */
void return_double_array(double_array_t* array);

// ================================================================================
// Push operations
// ================================================================================

/**
 * @brief Append one double to the end of the array. O(1) amortised.
 *
 * @param array  Must not be NULL.
 * @param value  The value to append.
 *
 * @return NO_ERROR, NULL_POINTER, CAPACITY_OVERFLOW, OUT_OF_MEMORY, or
 *         LENGTH_OVERFLOW.
 */
error_code_t push_back_double_array(double_array_t* array, double value);

// --------------------------------------------------------------------------------

/**
 * @brief Prepend one double to the front of the array. O(n).
 *
 * @param array  Must not be NULL.
 * @param value  The value to prepend.
 *
 * @return NO_ERROR, NULL_POINTER, CAPACITY_OVERFLOW, OUT_OF_MEMORY, or
 *         LENGTH_OVERFLOW.
 */
error_code_t push_front_double_array(double_array_t* array, double value);

// --------------------------------------------------------------------------------

/**
 * @brief Insert one double at an arbitrary index. O(n).
 *
 * @param array  Must not be NULL.
 * @param index  Insertion position. Must be <= array->base.len.
 * @param value  The value to insert.
 *
 * @return NO_ERROR, NULL_POINTER, OUT_OF_BOUNDS, CAPACITY_OVERFLOW,
 *         OUT_OF_MEMORY, or LENGTH_OVERFLOW.
 */
error_code_t push_at_double_array(double_array_t* array,
                                  size_t          index,
                                  double          value);

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
error_code_t get_double_array_index(const double_array_t* array,
                                    size_t                index,
                                    double*               out);

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
error_code_t pop_back_double_array(double_array_t* array, double* out);

// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally return the first element. O(n).
 *
 * @param array  Must not be NULL.
 * @param out    Receives the removed value, or NULL to discard.
 *
 * @return NO_ERROR, NULL_POINTER, or EMPTY.
 */
error_code_t pop_front_double_array(double_array_t* array, double* out);

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
error_code_t pop_any_double_array(double_array_t* array,
                                  double*         out,
                                  size_t          index);

// ================================================================================
// Utility operations
// ================================================================================

/**
 * @brief Reset the array to empty without releasing the buffer. O(1).
 *
 * @return NO_ERROR or NULL_POINTER.
 */
error_code_t clear_double_array(double_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Overwrite the element at index with a new value. O(1).
 *
 * @return NO_ERROR, NULL_POINTER, or OUT_OF_BOUNDS.
 */
error_code_t set_double_array_index(double_array_t* array,
                                    size_t          index,
                                    double          value);

// --------------------------------------------------------------------------------

/**
 * @brief Allocate an independent deep copy of src.
 *
 * @return double_array_expect_t: has_value true on success, or has_value false
 *         with NULL_POINTER, BAD_ALLOC, or OUT_OF_MEMORY on failure.
 */
double_array_expect_t copy_double_array(const double_array_t* src,
                                        allocator_vtable_t    alloc_v);

// --------------------------------------------------------------------------------

/**
 * @brief Append all elements of src to the end of dst.
 *
 * @return NO_ERROR, NULL_POINTER, CAPACITY_OVERFLOW, or OUT_OF_MEMORY.
 */
error_code_t concat_double_array(double_array_t*       dst,
                                 const double_array_t* src);

// --------------------------------------------------------------------------------

/**
 * @brief Allocate a new array containing elements at indices [start, end).
 *
 * @return double_array_expect_t: has_value true on success, or has_value false
 *         with NULL_POINTER, INVALID_ARG, OUT_OF_BOUNDS, BAD_ALLOC, or
 *         OUT_OF_MEMORY on failure.
 */
double_array_expect_t slice_double_array(const double_array_t* src,
                                         size_t                start,
                                         size_t                end,
                                         allocator_vtable_t    alloc_v);

// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of all elements in place. O(n).
 *
 * @return NO_ERROR or NULL_POINTER.
 */
error_code_t reverse_double_array(double_array_t* array);

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
 *
 * @code
 *     sort_double_array(arr, FORWARD);
 * @endcode
 */
error_code_t sort_double_array(double_array_t* array, direction_t dir);

// ================================================================================
// Reduction
// ================================================================================

/**
 * @brief Find the minimum value in the array.
 *
 * NaN values produce unspecified results. -0.0 and +0.0 compare equal; the
 * one at the lower index is returned if both are present.
 *
 * @param array  Must not be NULL and must contain at least one element.
 *
 * @return double_expect_t: has_value true + u.value on success, or has_value
 *         false + u.error (NULL_POINTER or EMPTY) on failure.
 *
 * @code
 *     double_expect_t r = double_array_min(arr);
 *     // r.u.value == smallest element.
 * @endcode
 */
double_expect_t double_array_min(const double_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Find the maximum value in the array.
 *
 * @param array  Must not be NULL and must contain at least one element.
 *
 * @return double_expect_t: has_value true + u.value on success, or has_value
 *         false + u.error (NULL_POINTER or EMPTY) on failure.
 */
double_expect_t double_array_max(const double_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Sum all elements and return the result as a double.
 *
 * NaN propagates from any NaN element. IEEE 754 rounding applies for large
 * arrays; consider Kahan summation externally if precision is critical.
 *
 * @param array  Must not be NULL.
 *
 * @return double_expect_t: has_value true + u.value == sum on success, or
 *         has_value false + u.error (NULL_POINTER or EMPTY) on failure.
 */
double_expect_t double_array_sum(const double_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Compute the inclusive prefix sum into a new double array.
 *
 * output[0] == src[0]; output[i] == output[i-1] + src[i]. src is unchanged.
 * The caller owns the output and must call return_double_array on it.
 *
 * @param src    Must not be NULL and must contain at least one element.
 * @param alloc  Allocator for the output array.
 *
 * @return double_array_expect_t: has_value true on success, or has_value false
 *         with NULL_POINTER, EMPTY, BAD_ALLOC, or OUT_OF_MEMORY on failure.
 *
 * @code
 *     // src: [1.0, -2.0, 3.0]  →  output: [1.0, -1.0, 2.0]
 *     double_array_expect_t cr = cumulative_double_array(src, alloc);
 *     return_double_array(cr.u.value);
 * @endcode
 */
double_array_expect_t cumulative_double_array(const double_array_t* src,
                                              allocator_vtable_t    alloc);

// ================================================================================
// Search
// ================================================================================

/**
 * @brief Search for the first element matching value in [start, end).
 *
 * Dispatches to the best SIMD implementation at compile time.
 *
 * Comparison modes (same sentinel convention as float_array_contains):
 *
 *   abs_tol == 0.0 && rel_tol == 0.0  — exact bit-pattern match.
 *       NaN != NaN, -0.0 != +0.0.
 *
 *   abs_tol >  0.0 && rel_tol == 0.0  — absolute tolerance only.
 *       Match when |element - value| <= abs_tol.
 *
 *   abs_tol == 0.0 && rel_tol >  0.0  — relative tolerance only.
 *       Match when |element - value| <= rel_tol * max(|element|, |value|).
 *
 *   abs_tol >  0.0 && rel_tol >  0.0  — combined.
 *       Match when |element - value| <= max(abs_tol,
 *                                           rel_tol * max(|element|, |value|)).
 *
 * Negative tolerances are rejected with INVALID_ARG.
 *
 * @param array    Must not be NULL.
 * @param value    The double value to search for.
 * @param start    First index to examine (inclusive).
 * @param end      One past the last index to examine (exclusive).
 *                 Must be > start and <= array->base.len.
 * @param abs_tol  Absolute tolerance >= 0.0. Pass 0.0 to disable.
 * @param rel_tol  Relative tolerance >= 0.0. Pass 0.0 to disable.
 *
 * @return size_expect_t: has_value true + u.value == first matching index on
 *         success, or has_value false + u.error (NULL_POINTER, INVALID_ARG,
 *         OUT_OF_BOUNDS, or NOT_FOUND) on failure.
 *
 * @code
 *     // Exact match:
 *     size_expect_t r = double_array_contains(arr, 3.14, 0, n, 0.0, 0.0);
 *
 *     // Absolute tolerance:
 *     r = double_array_contains(arr, 1.0, 0, n, 1e-9, 0.0);
 *
 *     // Relative tolerance (1%):
 *     r = double_array_contains(arr, 1000.0, 0, n, 0.0, 0.01);
 * @endcode
 */
size_expect_t double_array_contains(const double_array_t* array,
                                    double                value,
                                    size_t                start,
                                    size_t                end,
                                    double                abs_tol,
                                    double                rel_tol);

// --------------------------------------------------------------------------------

/**
 * @brief Binary-search for the first element within tolerance of value.
 *
 * Same two-phase algorithm as float_array_binary_search: exact bisection to
 * find the landing position, then a SIMD-accelerated tolerance fan-out in
 * both directions. Returns the lowest-index element within tolerance.
 *
 * The array must be sorted (or sort must be true) using the exact three-way
 * double comparator.
 *
 * @param array    Must not be NULL.
 * @param value    The double to find. Must not be NaN.
 * @param sort     true to sort before searching (sorted order persists).
 * @param abs_tol  Absolute tolerance >= 0.0. Pass 0.0 for exact match.
 * @param rel_tol  Relative tolerance >= 0.0. Pass 0.0 for exact match.
 *
 * @return size_expect_t: has_value true + u.value == first matching index on
 *         success, or has_value false + u.error (NULL_POINTER, INVALID_ARG,
 *         EMPTY, or NOT_FOUND) on failure.
 *
 * @code
 *     // Exact match:
 *     size_expect_t r = double_array_binary_search(arr, 2.5, false, 0.0, 0.0);
 *
 *     // Within 1e-9 absolute:
 *     r = double_array_binary_search(arr, 2.5, false, 1e-9, 0.0);
 * @endcode
 */
size_expect_t double_array_binary_search(double_array_t* array,
                                         double          value,
                                         bool            sort,
                                         double          abs_tol,
                                         double          rel_tol);

// --------------------------------------------------------------------------------

/**
 * @brief Find the bracketing indices for a value in a sorted double array.
 *
 * Identical semantics to float_array_binary_bracket. The sort contract is
 * exact; no tolerance is applied (see float_array_binary_bracket docstring
 * for the rationale).
 *
 * @param array  Must not be NULL.
 * @param value  The double to bracket. Must not be NaN.
 * @param sort   true to sort before searching.
 *
 * @return bracket_expect_t: has_value true on success with:
 *           u.value.lower — first index whose element >= value
 *           u.value.upper — last  index whose element <= value
 *         On failure has_value false + u.error (NULL_POINTER, EMPTY, or
 *         NOT_FOUND).
 *
 * @code
 *     // arr: [-2.0, -1.0, 1.0, 2.0]
 *     bracket_expect_t r = double_array_binary_bracket(arr, 0.0, false);
 *     // r.u.value.lower == 2 (1.0), r.u.value.upper == 1 (-1.0)
 * @endcode
 */
bracket_expect_t double_array_binary_bracket(double_array_t* array,
                                             double          value,
                                             bool            sort);

// ================================================================================
// Introspection
// ================================================================================

/** @brief Number of elements currently stored. Returns 0 if array is NULL. */
size_t double_array_size(const double_array_t* array);

/** @brief Number of elements the buffer can currently hold. Returns 0 if NULL. */
size_t double_array_alloc(const double_array_t* array);

/** @brief Size in bytes of one element (always 8). Returns 0 if NULL. */
size_t double_array_data_size(const double_array_t* array);

/** @brief true if array is NULL or empty. */
bool is_double_array_empty(const double_array_t* array);

/** @brief true if array is NULL or len == alloc. */
bool is_double_array_full(const double_array_t* array);

/**
 * @brief true if ptr points into the live element region of array's buffer.
 *
 * @param array  May be NULL.
 * @param ptr    May be NULL.
 */
bool is_double_array_ptr(const double_array_t* array, const double* ptr);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_double_H */
// ================================================================================
// ================================================================================
// eof
