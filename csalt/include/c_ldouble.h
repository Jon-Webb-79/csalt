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

/**
 * @brief A dictionary that maps null-terminated C-string keys to long double values.
 *
 * This is a typedef alias for @ref dict_t with the convention that
 * @c data_size is always @c sizeof(double) and @c dtype is always
 * @c LDOUBLE_TYPE.  Do not use the raw @ref dict_t API directly on a
 * @c ldouble_dict_t — always go through the functions declared in this header
 * so that the type constraints are enforced.
 *
 * @code
 *     allocator_vtable_t a = heap_allocator();
 *     ldouble_dict_expect_t r = init_ldouble_dict(16, true, a);
 *     if (!r.has_value) { // handle r.u.error }
 *     ldouble_dict_t* d = r.u.value;
 *
 *     insert_ldouble_dict(d, "width",  1920u, a);
 *     insert_ldouble_dict(d, "height", 1080u, a);
 *     insert_ldouble_dict(d, "depth",    32u, a);
 *
 *     long double v;
 *     get_ldouble_dict_value(d, "width", &v);   // v == 1920
 *
 *     return_ldouble_dict(d);
 * @endcode
 */
typedef dict_t ldouble_dict_t;
 
/**
 * @brief Expected return type for @ref init_ldouble_dict and
 *        @ref copy_ldouble_dict.
 *
 * Check @c has_value before using @c u.value.  On failure @c u.error
 * contains the relevant @ref error_code_t.
 */
typedef struct {
    bool has_value;
    union {
        ldouble_dict_t* value;
        error_code_t   error;
    } u;
} ldouble_dict_expect_t;
 
// ================================================================================
// Iterator type
// ================================================================================
 
/**
 * @brief Typed iterator callback for @ref foreach_ldouble_dict.
 *
 * @param key       Null-terminated C-string key (points into the dict's
 *                  internal storage — do not free or store beyond the
 *                  callback).
 * @param key_len   Length of @p key in bytes, excluding the null terminator.
 * @param value     The long double value associated with @p key.
 * @param user_data Caller-supplied context pointer passed unchanged from
 *                  @ref foreach_ldouble_dict; may be NULL.
 *
 * @code
 *     static void print_entry(const char* key, size_t key_len,
 *                             long double value, void* ud) {
 *         (void)ud;
 *         printf("  %.*s => %u\n", (int)key_len, key, (unsigned)value);
 *     }
 *     foreach_ldouble_dict(d, print_entry, NULL);
 * @endcode
 */
typedef void (*ldouble_dict_iter_fn)(const char* key,
                                    size_t      key_len,
                                    long double    value,
                                    void*       user_data);
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
/**
 * @brief Allocate and initialise a new ldouble_dict_t.
 *
 * The underlying @ref dict_t is created with @c data_size = sizeof(double)
 * and @c dtype = LDOUBLE_TYPE.  The bucket count is rounded up to the next
 * power of two internally.
 *
 * @param capacity  Initial bucket count.  Must be > 0.
 * @param growth    If true the table resizes automatically when the load
 *                  factor exceeds 0.75.
 * @param alloc_v   Allocator for all internal memory.  @c alloc_v.allocate
 *                  must not be NULL.
 *
 * @return @ref ldouble_dict_expect_t with @c has_value true on success.
 *         On failure @c u.error is one of NULL_POINTER, INVALID_ARG, or
 *         OUT_OF_MEMORY.
 *
 * @code
 *     allocator_vtable_t a = heap_allocator();
 *
 *     ldouble_dict_expect_t r = init_ldouble_dict(8, true, a);
 *     if (!r.has_value) {
 *         fprintf(stderr, "init failed: %d\n", r.u.error);
 *         return;
 *     }
 *     ldouble_dict_t* d = r.u.value;
 *
 *     insert_ldouble_dict(d, "port",  8080u, a);
 *     insert_ldouble_dict(d, "range", 1024u, a);
 *
 *     return_ldouble_dict(d);
 * @endcode
 */
ldouble_dict_expect_t init_ldouble_dict(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Release all memory owned by a ldouble_dict_t.
 *
 * Frees every node (key copy + value buffer), the bucket array, and the
 * dict struct itself.  Passing NULL is safe and performs no action.
 *
 * @param dict  Dictionary to release.
 *
 * @code
 *     ldouble_dict_t* d = init_ldouble_dict(8, true, a).u.value;
 *     insert_ldouble_dict(d, "x", 1000u, a);
 *     // ... use the dict ...
 *     return_ldouble_dict(d);   // d must not be used after this point
 * @endcode
 */
void return_ldouble_dict(ldouble_dict_t* dict);
 
// ================================================================================
// Insert
// ================================================================================
 
/**
 * @brief Insert a null-terminated string key with a long double value.
 *
 * The key length is measured with @c strlen(key).  The key bytes are copied
 * into the dict's own storage — the caller may free or reuse @p key
 * immediately after this call returns.
 *
 * If the key already exists the insertion is rejected and INVALID_ARG is
 * returned.  To overwrite an existing value use @ref update_ldouble_dict.
 *
 * @param dict    Must not be NULL.
 * @param key     Null-terminated C-string key.  Must not be NULL.
 * @param value   The long double value to store.
 * @param alloc_v Allocator used to allocate the new node.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG (duplicate key),
 *         CAPACITY_OVERFLOW (growth == false and table is full), or
 *         OUT_OF_MEMORY.
 *
 * @code
 *     ldouble_dict_t* d = init_ldouble_dict(8, true, a).u.value;
 *
 *     insert_ldouble_dict(d, "width",  1920u, a);   // OK
 *     insert_ldouble_dict(d, "height", 1080u, a);   // OK
 *
 *     // Duplicate key — returns INVALID_ARG, dict is unchanged
 *     error_code_t err = insert_ldouble_dict(d, "width", 800u, a);
 *     assert(err == INVALID_ARG);
 *
 *     return_ldouble_dict(d);
 * @endcode
 */
error_code_t insert_ldouble_dict(ldouble_dict_t*     dict,
                                 const char*        key,
                                 long double           value,
                                 allocator_vtable_t alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Insert a bounded key (explicit length) with a long double value.
 *
 * Identical to @ref insert_ldouble_dict but the key length is supplied by the
 * caller rather than measured with @c strlen.  Useful when the key is a
 * sub-string of a larger buffer or when the length is already known.
 *
 * @param dict    Must not be NULL.
 * @param key     Pointer to the first byte of the key.  Must not be NULL.
 * @param key_len Number of bytes in the key.  Must be > 0.
 * @param value   The long double value to store.
 * @param alloc_v Allocator used to allocate the new node.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, CAPACITY_OVERFLOW, or
 *         OUT_OF_MEMORY.
 *
 * @code
 *     // Key is a sub-string: "width_max", take only "width" (5 bytes)
 *     const char* buf = "width_max";
 *     insert_ldouble_dict_n(d, buf, 5, 1920u, a);
 *
 *     long double v;
 *     get_ldouble_dict_value_n(d, buf, 5, &v);   // v == 1920
 * @endcode
 */
error_code_t insert_ldouble_dict_n(ldouble_dict_t*     dict,
                                   const char*        key,
                                   size_t             key_len,
                                   long double           value,
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
 *     ldouble_dict_t* d = init_ldouble_dict(8, true, a).u.value;
 *     insert_ldouble_dict(d, "port", 8080u, a);
 *
 *     long double removed;
 *     error_code_t err = pop_ldouble_dict(d, "port", &removed);
 *     assert(err == NO_ERROR && removed == 8080u);
 *
 *     // Key is gone — next pop returns NOT_FOUND
 *     err = pop_ldouble_dict(d, "port", NULL);
 *     assert(err == NOT_FOUND);
 *
 *     return_ldouble_dict(d);
 * @endcode
 */
error_code_t pop_ldouble_dict(ldouble_dict_t* dict,
                              const char*    key,
                              long double*      out_value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Remove the entry for a bounded key and return its value.
 *
 * Identical to @ref pop_ldouble_dict but the key length is supplied explicitly.
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
 *     insert_ldouble_dict_n(d, buf, 4, 8080u, a);   // key = "port"
 *
 *     long double v;
 *     pop_ldouble_dict_n(d, buf, 4, &v);             // removes "port", v == 8080
 * @endcode
 */
error_code_t pop_ldouble_dict_n(ldouble_dict_t* dict,
                                const char*    key,
                                size_t         key_len,
                                long double*      out_value);
 
// ================================================================================
// Update
// ================================================================================
 
/**
 * @brief Overwrite the value of an existing null-terminated key.
 *
 * No allocation is performed.  If the key does not exist NOT_FOUND is
 * returned and the dict is unchanged.  To insert a new entry use
 * @ref insert_ldouble_dict.
 *
 * @param dict   Must not be NULL.
 * @param key    Null-terminated C-string key.  Must not be NULL.
 * @param value  New long double value to store.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND.
 *
 * @code
 *     ldouble_dict_t* d = init_ldouble_dict(8, true, a).u.value;
 *     insert_ldouble_dict(d, "port", 8080u, a);
 *
 *     update_ldouble_dict(d, "port", 443u);
 *
 *     long double v;
 *     get_ldouble_dict_value(d, "port", &v);
 *     assert(v == 443u);
 *
 *     return_ldouble_dict(d);
 * @endcode
 */
error_code_t update_ldouble_dict(ldouble_dict_t* dict,
                                 const char*    key,
                                 long double       value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Overwrite the value of an existing bounded key.
 *
 * Identical to @ref update_ldouble_dict but the key length is supplied
 * explicitly.
 *
 * @param dict     Must not be NULL.
 * @param key      Pointer to the key bytes.  Must not be NULL.
 * @param key_len  Number of bytes in the key.  Must be > 0.
 * @param value    New long double value to store.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, or NOT_FOUND.
 *
 * @code
 *     const char* buf = "port_http";
 *     insert_ldouble_dict_n(d, buf, 4, 8080u, a);   // key = "port"
 *     update_ldouble_dict_n(d, buf, 4, 443u);        // update "port" to 443
 * @endcode
 */
error_code_t update_ldouble_dict_n(ldouble_dict_t* dict,
                                   const char*    key,
                                   size_t         key_len,
                                   long double       value);
 
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
 *     ldouble_dict_t* d = init_ldouble_dict(8, true, a).u.value;
 *     insert_ldouble_dict(d, "width", 1920u, a);
 *
 *     long double v;
 *     if (get_ldouble_dict_value(d, "width", &v) == NO_ERROR)
 *         printf("width = %u\n", (unsigned)v);
 *
 *     return_ldouble_dict(d);
 * @endcode
 */
error_code_t get_ldouble_dict_value(const ldouble_dict_t* dict,
                                    const char*          key,
                                    long double*            out_value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Copy the value for a bounded key into a caller-supplied variable.
 *
 * Identical to @ref get_ldouble_dict_value but the key length is supplied
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
 *     insert_ldouble_dict_n(d, buf, 5, 1920u, a);   // key = "width"
 *
 *     long double v;
 *     get_ldouble_dict_value_n(d, buf, 5, &v);       // v == 1920
 * @endcode
 */
error_code_t get_ldouble_dict_value_n(const ldouble_dict_t* dict,
                                      const char*          key,
                                      size_t               key_len,
                                      long double*            out_value);
 
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
 * @return Pointer to the stored long double on success, NULL if not found or on
 *         error.
 *
 * @code
 *     insert_ldouble_dict(d, "port", 8080u, a);
 *
 *     const long double* p = get_ldouble_dict_ptr(d, "port");
 *     if (p) printf("port = %u\n", (unsigned)*p);
 *     // Do not store p — it may be invalidated by the next insert or pop.
 * @endcode
 */
const long double* get_ldouble_dict_ptr(const ldouble_dict_t* dict, const char* key);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Return a read-only pointer directly into the node's value buffer for
 *        a bounded key.
 *
 * Identical to @ref get_ldouble_dict_ptr but the key length is supplied
 * explicitly.
 *
 * @param dict     Must not be NULL.
 * @param key      Pointer to the key bytes.  Must not be NULL.
 * @param key_len  Number of bytes in the key.  Must be > 0.
 *
 * @return Pointer to the stored long double on success, NULL if not found or on
 *         error.
 *
 * @code
 *     const char* buf = "port_http";
 *     insert_ldouble_dict_n(d, buf, 4, 8080u, a);   // key = "port"
 *
 *     const long double* p = get_ldouble_dict_ptr_n(d, buf, 4);
 *     if (p) printf("port = %u\n", (unsigned)*p);
 * @endcode
 */
const long double* get_ldouble_dict_ptr_n(const ldouble_dict_t* dict,
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
 *     insert_ldouble_dict(d, "active", 1u, a);
 *
 *     if (has_ldouble_dict_key(d, "active"))
 *         printf("active is set\n");
 * @endcode
 */
bool has_ldouble_dict_key(const ldouble_dict_t* dict, const char* key);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Test whether a bounded key exists in the dict.
 *
 * Identical to @ref has_ldouble_dict_key but the key length is supplied
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
 *     insert_ldouble_dict_n(d, buf, 6, 1u, a);   // key = "active"
 *     assert(has_ldouble_dict_key_n(d, buf, 6));
 * @endcode
 */
bool has_ldouble_dict_key_n(const ldouble_dict_t* dict,
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
 *     insert_ldouble_dict(d, "a", 100u, a);
 *     insert_ldouble_dict(d, "b", 200u, a);
 *     assert(ldouble_dict_hash_size(d) == 2);
 *
 *     clear_ldouble_dict(d);
 *     assert(ldouble_dict_hash_size(d) == 0);
 *
 *     // The dict is still usable after clear
 *     insert_ldouble_dict(d, "c", 300u, a);
 * @endcode
 */
error_code_t clear_ldouble_dict(ldouble_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Allocate a deep copy of a ldouble_dict_t.
 *
 * All nodes are copied into a fresh bucket array of the same capacity.  The
 * new dict uses @p alloc_v for all of its allocations.
 *
 * @param src     Must not be NULL.
 * @param alloc_v Allocator for the new dict and its nodes.
 *
 * @return @ref ldouble_dict_expect_t with @c has_value true on success.
 *
 * @code
 *     ldouble_dict_t* orig = init_ldouble_dict(8, true, a).u.value;
 *     insert_ldouble_dict(orig, "x", 1000u, a);
 *     insert_ldouble_dict(orig, "y", 2000u, a);
 *
 *     ldouble_dict_expect_t cr = copy_ldouble_dict(orig, a);
 *     assert(cr.has_value);
 *     ldouble_dict_t* copy = cr.u.value;
 *
 *     // The copy is independent — mutating orig does not affect copy
 *     update_ldouble_dict(orig, "x", 9999u);
 *     long double v;
 *     get_ldouble_dict_value(copy, "x", &v);
 *     assert(v == 1000u);
 *
 *     return_ldouble_dict(copy);
 *     return_ldouble_dict(orig);
 * @endcode
 */
ldouble_dict_expect_t copy_ldouble_dict(const ldouble_dict_t* src,
                                      allocator_vtable_t   alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Merge two ldouble_dict_t instances into a new dict.
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
 * @return @ref ldouble_dict_expect_t with @c has_value true on success.
 *
 * @code
 *     ldouble_dict_t* da = init_ldouble_dict(8, true, a).u.value;
 *     ldouble_dict_t* db = init_ldouble_dict(8, true, a).u.value;
 *
 *     insert_ldouble_dict(da, "shared", 100u, a);
 *     insert_ldouble_dict(da, "only_a",   1u, a);
 *     insert_ldouble_dict(db, "shared", 999u, a);
 *     insert_ldouble_dict(db, "only_b",   2u, a);
 *
 *     // Merge without overwrite — "shared" keeps da's value of 100
 *     ldouble_dict_expect_t mr = merge_ldouble_dict(da, db, false, a);
 *     long double v;
 *     get_ldouble_dict_value(mr.u.value, "shared", &v);
 *     assert(v == 100u);
 *
 *     return_ldouble_dict(mr.u.value);
 *     return_ldouble_dict(da);
 *     return_ldouble_dict(db);
 * @endcode
 */
ldouble_dict_expect_t merge_ldouble_dict(const ldouble_dict_t* a,
                                       const ldouble_dict_t* b,
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
 * C-string pointer (into internal storage), its length, and the double
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
 *                             long double value, void* ud) {
 *         (void)key_len; (void)ud;
 *         printf("  %s = %f\n", key, (double)value);
 *     }
 *
 *     ldouble_dict_t* d = init_ldouble_dict(8, true, a).u.value;
 *     insert_ldouble_dict(d, "width",  1920u, a);
 *     insert_ldouble_dict(d, "height", 1080u, a);
 *
 *     foreach_ldouble_dict(d, print_entry, NULL);
 *     // Output (order may vary):
 *     //   width = 1920
 *     //   height = 1080
 *
 *     return_ldouble_dict(d);
 * @endcode
 */
error_code_t foreach_ldouble_dict(const ldouble_dict_t* dict,
                                  ldouble_dict_iter_fn  fn,
                                  void*                user_data);
 
// ================================================================================
// Introspection
// ================================================================================
 
/**
 * @brief Number of occupied buckets (chains with at least one entry).
 *
 * Returns 0 if @p dict is NULL.
 */
size_t ldouble_dict_size(const ldouble_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Total number of key-value pairs stored.
 *
 * Returns 0 if @p dict is NULL.
 */
size_t ldouble_dict_hash_size(const ldouble_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Number of buckets currently allocated.
 *
 * Returns 0 if @p dict is NULL.
 */
size_t ldouble_dict_alloc(const ldouble_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief true if @p dict is NULL or contains no entries.
 */
bool is_ldouble_dict_empty(const ldouble_dict_t* dict);
// ================================================================================ 
// ================================================================================ 

/**
 * @brief A type-safe ldouble-precision matrix.
 *
 * Wraps matrix_t with the dtype fixed to ldouble_TYPE.  Supports Dense,
 * COO, CSR, and CSC storage formats.
 */
// typedef matrix_t ldouble_matrix_t;
//  
// /**
//  * @brief Expected return type for ldouble_matrix_t operations that may fail.
//  *
//  * Check @c has_value before accessing @c u.value.  On failure @c u.error
//  * contains the error code.
//  */
// typedef struct {
//     bool has_value;
//     union {
//         ldouble_matrix_t* value;
//         error_code_t     error;
//     } u;
// } ldouble_matrix_expect_t;
//  
// // ================================================================================
// // Initialization and teardown
// // ================================================================================
//  
// /**
//  * @brief Initialize a dense ldouble matrix with zero-initialized storage.
//  *
//  * Creates a dense matrix of shape @p rows by @p cols with the dtype fixed to
//  * ldouble_TYPE.  All elements are initialised to zero.  The caller must free
//  * the matrix with return_ldouble_matrix when no longer needed.
//  *
//  * @param rows     Number of matrix rows.  Must be > 0.
//  * @param cols     Number of matrix columns.  Must be > 0.
//  * @param alloc_v  Allocator vtable.  alloc_v.allocate must not be NULL.
//  *
//  * @return ldouble_matrix_expect_t with has_value true on success, or u.error:
//  *         - NULL_POINTER    — alloc_v.allocate is NULL
//  *         - INVALID_ARG     — rows or cols is 0
//  *         - ILLEGAL_STATE   — dtype registry could not be initialized
//  *         - TYPE_MISMATCH   — ldouble_TYPE not found in the registry
//  *         - LENGTH_OVERFLOW — rows * cols * sizeof(ldouble) overflows size_t
//  *         - BAD_ALLOC       — matrix_t struct allocation failed
//  *         - OUT_OF_MEMORY   — data buffer allocation failed
//  *
//  * @code
//  *     allocator_vtable_t a = heap_allocator();
//  *     ldouble_matrix_expect_t r = init_ldouble_dense_matrix(3, 4, a);
//  *     if (!r.has_value) { fprintf(stderr, "%d\n", r.u.error); return; }
//  *     ldouble_matrix_t* m = r.u.value;
//  *
//  *     set_ldouble_matrix(m, 0, 0, 1.0);
//  *     set_ldouble_matrix(m, 1, 2, 5.5);
//  *
//  *     return_ldouble_matrix(m);
//  * @endcode
//  */
// ldouble_matrix_expect_t init_ldouble_dense_matrix(size_t             rows,
//                                                   size_t             cols,
//                                                   allocator_vtable_t alloc_v);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Initialize an empty COO sparse ldouble matrix.
//  *
//  * Creates a coordinate-list sparse matrix with storage for up to @p capacity
//  * entries.  The dtype is fixed to ldouble_TYPE.  The matrix starts with zero
//  * stored entries (nnz = 0).
//  *
//  * @param rows      Number of matrix rows.  Must be > 0.
//  * @param cols      Number of matrix columns.  Must be > 0.
//  * @param capacity  Initial number of entry slots.  Must be > 0.
//  * @param growth    If true, COO buffers grow automatically when full.
//  * @param alloc_v   Allocator vtable.  alloc_v.allocate must not be NULL.
//  *
//  * @return ldouble_matrix_expect_t with has_value true on success, or u.error:
//  *         - NULL_POINTER    — alloc_v.allocate is NULL
//  *         - INVALID_ARG     — rows, cols, or capacity is 0
//  *         - ILLEGAL_STATE   — dtype registry could not be initialized
//  *         - TYPE_MISMATCH   — ldouble_TYPE not found in the registry
//  *         - LENGTH_OVERFLOW — capacity overflows size_t arithmetic
//  *         - BAD_ALLOC       — matrix_t struct allocation failed
//  *         - OUT_OF_MEMORY   — COO buffer allocation failed
//  *
//  * @code
//  *     allocator_vtable_t a = heap_allocator();
//  *     ldouble_matrix_expect_t r = init_ldouble_coo_matrix(100, 100, 16, true, a);
//  *     ldouble_matrix_t* m = r.u.value;
//  *
//  *     push_back_ldouble_coo_matrix(m, 0, 5, 3.14159265);
//  *     push_back_ldouble_coo_matrix(m, 42, 99, -1.0);
//  *
//  *     return_ldouble_matrix(m);
//  * @endcode
//  */
// ldouble_matrix_expect_t init_ldouble_coo_matrix(size_t             rows,
//                                                 size_t             cols,
//                                                 size_t             capacity,
//                                                 bool               growth,
//                                                 allocator_vtable_t alloc_v);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Release all storage owned by a ldouble matrix.
//  *
//  * Frees internal buffers for the active format, then frees the matrix_t
//  * struct itself through its stored allocator.  NULL-safe.
//  *
//  * @param mat  Matrix to destroy, or NULL (no-op).
//  */
// void return_ldouble_matrix(ldouble_matrix_t* mat);
//  
// // ================================================================================
// // Element access
// // ================================================================================
//  
// /**
//  * @brief Read a ldouble element at the given row and column.
//  *
//  * For sparse formats, entries not explicitly stored are returned as 0.0.
//  *
//  * @param mat  Matrix to read from.  Must not be NULL.
//  * @param row  Zero-based row index.  Must be < mat->rows.
//  * @param col  Zero-based column index.  Must be < mat->cols.
//  * @param out  Receives the ldouble value.  Must not be NULL.
//  *
//  * @return NO_ERROR on success, or:
//  *         - NULL_POINTER — mat or out is NULL
//  *         - INVALID_ARG  — (row, col) out of bounds
//  *
//  * @code
//  *     ldouble v = 0.0;
//  *     get_ldouble_matrix(m, 1, 2, &v);
//  * @endcode
//  */
// error_code_t get_ldouble_matrix(const ldouble_matrix_t* mat,
//                                 size_t                 row,
//                                 size_t                 col,
//                                 long double*                out);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Write a ldouble element at the given row and column.
//  *
//  * Dense: direct overwrite.  COO: inserts or overwrites the entry at
//  * (row, col).  CSR/CSC: not supported (returns ILLEGAL_STATE).
//  *
//  * @param mat    Matrix to modify.  Must not be NULL.
//  * @param row    Zero-based row index.  Must be < mat->rows.
//  * @param col    Zero-based column index.  Must be < mat->cols.
//  * @param value  The ldouble value to store.
//  *
//  * @return NO_ERROR on success, or:
//  *         - NULL_POINTER      — mat is NULL
//  *         - INVALID_ARG       — (row, col) out of bounds
//  *         - ILLEGAL_STATE     — format is CSR or CSC
//  *         - CAPACITY_OVERFLOW — COO is full and growth is false
//  *         - LENGTH_OVERFLOW   — COO growth would overflow size_t
//  *         - OUT_OF_MEMORY     — COO growth allocation failed
//  *
//  * @code
//  *     set_ldouble_matrix(m, 0, 0, 42.0);
//  * @endcode
//  */
// error_code_t set_ldouble_matrix(ldouble_matrix_t* mat,
//                                 size_t           row,
//                                 size_t           col,
//                                 long double           value);
//  
// // ================================================================================
// // COO assembly helpers
// // ================================================================================
//  
// /**
//  * @brief Reserve additional entry capacity for a ldouble COO matrix.
//  *
//  * Grows COO buffers to at least @p capacity entries.  No-op if the
//  * current capacity is already sufficient.  Existing entries are preserved.
//  *
//  * @param mat       COO matrix to grow.  Must not be NULL.
//  * @param capacity  Requested minimum entry capacity.
//  *
//  * @return NO_ERROR on success, or:
//  *         - NULL_POINTER    — mat is NULL
//  *         - ILLEGAL_STATE   — mat is not COO format
//  *         - INVALID_ARG     — capacity < current nnz
//  *         - LENGTH_OVERFLOW — capacity overflows size_t arithmetic
//  *         - OUT_OF_MEMORY   — buffer reallocation failed
//  */
// error_code_t reserve_ldouble_coo_matrix(ldouble_matrix_t* mat,
//                                         size_t           capacity);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Append or overwrite a ldouble entry in a COO matrix.
//  *
//  * If an entry at (row, col) already exists, its value is overwritten
//  * without increasing nnz.  Otherwise a new entry is appended and the
//  * matrix is marked unsorted.
//  *
//  * @param mat    COO matrix to append to.  Must not be NULL.
//  * @param row    Zero-based row index.  Must be < mat->rows.
//  * @param col    Zero-based column index.  Must be < mat->cols.
//  * @param value  The ldouble value to insert.
//  *
//  * @return NO_ERROR on success, or:
//  *         - NULL_POINTER      — mat is NULL
//  *         - ILLEGAL_STATE     — mat is not COO format
//  *         - INVALID_ARG       — (row, col) out of bounds
//  *         - CAPACITY_OVERFLOW — COO is full and growth is false
//  *         - LENGTH_OVERFLOW   — growth would overflow size_t
//  *         - OUT_OF_MEMORY     — growth allocation failed
//  *
//  * @code
//  *     push_back_ldouble_coo_matrix(m, 0, 5, 3.14159265);
//  * @endcode
//  */
// error_code_t push_back_ldouble_coo_matrix(ldouble_matrix_t* mat,
//                                           size_t           row,
//                                           size_t           col,
//                                           long double           value);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Sort COO entries into row-major (row, col) order.
//  *
//  * Uses quicksort with median-of-three pivot and insertion-sort fallback.
//  * After sorting, COO lookups use binary search.  No-op if nnz < 2.
//  *
//  * @param mat  COO matrix to sort.  Must not be NULL.
//  *
//  * @return NO_ERROR on success, or:
//  *         - NULL_POINTER  — mat is NULL
//  *         - ILLEGAL_STATE — mat is not COO format
//  */
// error_code_t sort_ldouble_coo_matrix(ldouble_matrix_t* mat);
//  
// // ================================================================================
// // Lifecycle / structural operations
// // ================================================================================
//  
// /**
//  * @brief Clear a ldouble matrix while preserving its allocated storage.
//  *
//  * Dense: all bytes memset to zero.  COO: nnz reset to 0, sorted to true.
//  * CSR/CSC: not supported.
//  *
//  * @param mat  Matrix to clear.  Must not be NULL.
//  *
//  * @return NO_ERROR on success, or:
//  *         - NULL_POINTER  — mat is NULL
//  *         - ILLEGAL_STATE — format is CSR or CSC
//  */
// error_code_t clear_ldouble_matrix(ldouble_matrix_t* mat);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Deep-copy a ldouble matrix preserving its format.
//  *
//  * All four formats are supported.  The copy is fully independent of
//  * the source.
//  *
//  * @param src      Matrix to copy.  Must not be NULL.
//  * @param alloc_v  Allocator for the destination matrix.
//  *
//  * @return ldouble_matrix_expect_t with has_value true on success, or u.error:
//  *         - NULL_POINTER  — src is NULL
//  *         - BAD_ALLOC     — struct allocation failed
//  *         - OUT_OF_MEMORY — buffer allocation failed
//  *         - ILLEGAL_STATE — unrecognized format
//  */
// ldouble_matrix_expect_t copy_ldouble_matrix(const ldouble_matrix_t* src,
//                                             allocator_vtable_t     alloc_v);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Convert a ldouble matrix to a different storage format.
//  *
//  * All 12 format pairs (Dense↔COO↔CSR↔CSC) are supported.  If @p target
//  * equals the source format, this behaves like copy_ldouble_matrix.
//  *
//  * @param src      Matrix to convert.  Must not be NULL.
//  * @param target   Desired destination storage format.
//  * @param alloc_v  Allocator for the destination matrix.
//  *
//  * @return ldouble_matrix_expect_t with has_value true on success, or u.error:
//  *         - NULL_POINTER  — src is NULL
//  *         - BAD_ALLOC     — struct allocation failed
//  *         - OUT_OF_MEMORY — buffer allocation failed
//  *         - ILLEGAL_STATE — unrecognized source or target format
//  *
//  * @code
//  *     ldouble_matrix_expect_t csr =
//  *         convert_ldouble_matrix(dense_m, CSR_MATRIX, a);
//  * @endcode
//  */
// ldouble_matrix_expect_t convert_ldouble_matrix(const ldouble_matrix_t* src,
//                                                matrix_format_t        target,
//                                                allocator_vtable_t     alloc_v);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Compute the transpose of a ldouble matrix.
//  *
//  * @param src      Matrix to transpose.  Must not be NULL.
//  * @param alloc_v  Allocator for the destination matrix.
//  *
//  * @return ldouble_matrix_expect_t with has_value true on success, or u.error:
//  *         - NULL_POINTER  — src is NULL
//  *         - BAD_ALLOC     — struct allocation failed
//  *         - OUT_OF_MEMORY — buffer allocation failed
//  *         - ILLEGAL_STATE — unrecognized format
//  */
// ldouble_matrix_expect_t transpose_ldouble_matrix(const ldouble_matrix_t* src,
//                                                  allocator_vtable_t     alloc_v);
//  
// // ================================================================================
// // Fill and zero
// // ================================================================================
//  
// /**
//  * @brief Fill a ldouble matrix with a single repeated value.
//  *
//  * @param mat    Matrix to fill.  Must not be NULL.
//  * @param value  The ldouble fill value.
//  *
//  * @return NO_ERROR on success, or:
//  *         - NULL_POINTER          — mat is NULL
//  *         - OPERATION_UNAVAILABLE — nonzero fill on non-Dense format
//  *         (zero fill delegates to clear_matrix, which may return
//  *          ILLEGAL_STATE for CSR/CSC)
//  *
//  * @code
//  *     fill_ldouble_matrix(m, 1.0);   // every element becomes 1.0
//  *     fill_ldouble_matrix(m, 0.0);   // equivalent to clear
//  * @endcode
//  */
// error_code_t fill_ldouble_matrix(ldouble_matrix_t* mat,
//                                  long double           value);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Set all elements of a ldouble matrix to zero.
//  *
//  * Equivalent to clear_ldouble_matrix.  Dense: memset to zero.  COO: nnz
//  * reset to 0.  CSR/CSC: ILLEGAL_STATE.
//  *
//  * @param mat  Matrix to zero.  Must not be NULL.
//  *
//  * @return NO_ERROR on success, or:
//  *         - NULL_POINTER  — mat is NULL
//  *         - ILLEGAL_STATE — format is CSR or CSC
//  */
// error_code_t zero_ldouble_matrix(ldouble_matrix_t* mat);
//  
// // ================================================================================
// // Introspection
// // ================================================================================
//  
// /**
//  * @brief Return the number of rows in a ldouble matrix.
//  *
//  * @param mat  Matrix to inspect.
//  * @return Row count, or 0 if @p mat is NULL.
//  */
// size_t ldouble_matrix_rows(const ldouble_matrix_t* mat);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Return the number of columns in a ldouble matrix.
//  *
//  * @param mat  Matrix to inspect.
//  * @return Column count, or 0 if @p mat is NULL.
//  */
// size_t ldouble_matrix_cols(const ldouble_matrix_t* mat);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Return the entry count for a ldouble matrix.
//  *
//  * Dense: rows × cols.  COO/CSR/CSC: stored nonzero count.
//  *
//  * @param mat  Matrix to inspect.
//  * @return Entry count, or 0 if @p mat is NULL.
//  */
// size_t ldouble_matrix_nnz(const ldouble_matrix_t* mat);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Return the active storage format of a ldouble matrix.
//  *
//  * @param mat  Matrix to inspect.
//  * @return Format enum value, or DENSE_MATRIX if @p mat is NULL.
//  */
// matrix_format_t ldouble_matrix_format(const ldouble_matrix_t* mat);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Return the storage footprint of the ldouble matrix payload in bytes.
//  *
//  * Dense: rows × cols × sizeof(ldouble).
//  * COO:   cap × (2 × sizeof(size_t) + sizeof(ldouble)).
//  * CSR:   (rows+1) × sizeof(size_t) + nnz × (sizeof(size_t) + sizeof(ldouble)).
//  * CSC:   (cols+1) × sizeof(size_t) + nnz × (sizeof(size_t) + sizeof(ldouble)).
//  *
//  * @param mat  Matrix to inspect.
//  * @return Storage bytes, or 0 if @p mat is NULL or format is unrecognized.
//  */
// size_t ldouble_matrix_storage_bytes(const ldouble_matrix_t* mat);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Return a human-readable name for the ldouble matrix's storage format.
//  *
//  * @param mat  Matrix to inspect.
//  * @return Constant string (e.g. "DENSE_MATRIX"), or
//  *         "UNKNOWN_MATRIX_FORMAT" if @p mat is NULL.
//  */
// const char* ldouble_matrix_format_name(const ldouble_matrix_t* mat);
//  
// // ================================================================================
// // Shape and compatibility queries
// // ================================================================================
//  
// /**
//  * @brief Test whether two ldouble matrices have identical shape.
//  *
//  * @param a  First matrix.
//  * @param b  Second matrix.
//  * @return true if both non-NULL with matching rows and cols; false otherwise.
//  */
// bool ldouble_matrix_has_same_shape(const ldouble_matrix_t* a,
//                                    const ldouble_matrix_t* b);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Test whether a ldouble matrix is square.
//  *
//  * @param mat  Matrix to inspect.
//  * @return true if non-NULL and rows == cols; false otherwise.
//  */
// bool ldouble_matrix_is_square(const ldouble_matrix_t* mat);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Test whether a ldouble matrix uses a sparse representation.
//  *
//  * @param mat  Matrix to inspect.
//  * @return true for COO, CSR, CSC; false for Dense or NULL.
//  */
// bool ldouble_matrix_is_sparse(const ldouble_matrix_t* mat);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Test whether a ldouble matrix is logically all zeros.
//  *
//  * @param mat  Matrix to inspect.  NULL returns true.
//  * @return true if all elements are logically zero; false otherwise.
//  */
// bool is_ldouble_matrix_zero(const ldouble_matrix_t* mat);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Bitwise element-by-element equality of two ldouble matrices.
//  *
//  * @note Uses bitwise comparison: +0.0 and -0.0 compare unequal.
//  *       Two NaN values with different payloads compare unequal.
//  *
//  * @param a  First matrix.  May be NULL (returns false).
//  * @param b  Second matrix.  May be NULL (returns false).
//  * @return true if logically equal; false otherwise.
//  */
// bool ldouble_matrix_equal(const ldouble_matrix_t* a,
//                           const ldouble_matrix_t* b);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Test whether two ldouble matrices are compatible for addition.
//  *
//  * Requires equal shape (rows and cols) and equal dtype.
//  *
//  * @param a  First matrix.
//  * @param b  Second matrix.
//  * @return true if add-compatible; false otherwise (including if either is NULL).
//  */
// bool ldouble_matrix_is_add_compatible(const ldouble_matrix_t* a,
//                                       const ldouble_matrix_t* b);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Test whether two ldouble matrices are compatible for multiplication.
//  *
//  * Requires a->cols == b->rows and equal dtype.
//  *
//  * @param a  Left-hand matrix.
//  * @param b  Right-hand matrix.
//  * @return true if multiply-compatible; false otherwise (including if either
//  *         is NULL).
//  */
// bool ldouble_matrix_is_multiply_compatible(const ldouble_matrix_t* a,
//                                            const ldouble_matrix_t* b);
//  
// // ================================================================================
// // Row / column swaps
// // ================================================================================
//  
// /**
//  * @brief Swap two rows of a ldouble matrix in place.
//  *
//  * Dense: allocates a temporary buffer for one row, performs three memcpy
//  * operations.  COO: relabels row indices and re-sorts.  CSR/CSC: not
//  * supported.
//  *
//  * @param mat  Matrix to modify.  Must not be NULL.
//  * @param r1   First zero-based row index.
//  * @param r2   Second zero-based row index.
//  *
//  * @return NO_ERROR on success (including r1 == r2, which is a no-op), or:
//  *         - NULL_POINTER          — mat is NULL
//  *         - OUT_OF_BOUNDS         — r1 or r2 >= mat->rows
//  *         - OUT_OF_MEMORY         — temp buffer alloc failed (Dense only)
//  *         - OPERATION_UNAVAILABLE — format is CSR or CSC
//  *         - ILLEGAL_STATE         — unrecognized format
//  */
// error_code_t swap_ldouble_matrix_rows(ldouble_matrix_t* mat,
//                                       size_t           r1,
//                                       size_t           r2);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Swap two columns of a ldouble matrix in place.
//  *
//  * Dense: element-by-element byte swap across all rows.  COO: relabels
//  * column indices and re-sorts.  CSR/CSC: not supported.
//  *
//  * @param mat  Matrix to modify.  Must not be NULL.
//  * @param c1   First zero-based column index.
//  * @param c2   Second zero-based column index.
//  *
//  * @return NO_ERROR on success (including c1 == c2, which is a no-op), or:
//  *         - NULL_POINTER          — mat is NULL
//  *         - OUT_OF_BOUNDS         — c1 or c2 >= mat->cols
//  *         - OPERATION_UNAVAILABLE — format is CSR or CSC
//  *         - ILLEGAL_STATE         — unrecognized format
//  */
// error_code_t swap_ldouble_matrix_cols(ldouble_matrix_t* mat,
//                                       size_t           c1,
//                                       size_t           c2);
//  
// // ================================================================================
// // Special matrix constructors
// // ================================================================================
//  
// /**
//  * @brief Initialize a dense ldouble identity matrix.
//  *
//  * Creates an @p n by @p n dense matrix with 1.0 on the diagonal and 0.0
//  * elsewhere.
//  *
//  * @param n        Matrix order (rows == cols == n).  Must be > 0.
//  * @param alloc_v  Allocator vtable.
//  *
//  * @return ldouble_matrix_expect_t with has_value true on success.
//  *         Propagates all errors from init_ldouble_dense_matrix, plus
//  *         errors from clear_matrix or set_matrix during diagonal fill.
//  *
//  * @code
//  *     ldouble_matrix_expect_t r = init_ldouble_identity_matrix(3, a);
//  *     // 3×3 identity matrix with 1.0 on diagonal.
//  * @endcode
//  */
// ldouble_matrix_expect_t init_ldouble_identity_matrix(size_t             n,
//                                                      allocator_vtable_t alloc_v);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Initialize a dense double row vector (1 × length), zero-initialized.
//  *
//  * Equivalent to init_double_dense_matrix(1, length, alloc_v).
//  *
//  * @param length   Number of columns (vector length).  Must be > 0.
//  * @param alloc_v  Allocator vtable.
//  *
//  * @return double_matrix_expect_t — same errors as init_double_dense_matrix.
//  */
// ldouble_matrix_expect_t init_ldouble_row_vector(size_t             length,
//                                                 allocator_vtable_t alloc_v);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Initialize a dense double column vector (length × 1), zero-initialized.
//  *
//  * Equivalent to init_double_dense_matrix(length, 1, alloc_v).
//  *
//  * @param length   Number of rows (vector length).  Must be > 0.
//  * @param alloc_v  Allocator vtable.
//  *
//  * @return double_matrix_expect_t — same errors as init_double_dense_matrix.
//  */
// ldouble_matrix_expect_t init_ldouble_col_vector(size_t             length,
//                                                allocator_vtable_t alloc_v);
//  
// // ================================================================================
// // Vector shape queries
// // ================================================================================
//  
// /**
//  * @brief Test whether a double matrix has row-vector shape (1 × N).
//  *
//  * @param mat  Matrix to inspect.
//  * @return true if non-NULL and rows == 1; false otherwise.
//  */
// bool ldouble_matrix_is_row_vector(const ldouble_matrix_t* mat);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Test whether a double matrix has column-vector shape (N × 1).
//  *
//  * @param mat  Matrix to inspect.
//  * @return true if non-NULL and cols == 1; false otherwise.
//  */
// bool ldouble_matrix_is_col_vector(const ldouble_matrix_t* mat);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Test whether a double matrix has vector shape (row or column).
//  *
//  * @param mat  Matrix to inspect.
//  * @return true if row vector or column vector; false otherwise (including NULL).
//  */
// bool ldouble_matrix_is_vector(const ldouble_matrix_t* mat);
//  
// // -------------------------------------------------------------------------------- 
//  
// /**
//  * @brief Return the logical length of a vector-shaped double matrix.
//  *
//  * Row vector: returns cols.  Column vector: returns rows.  Non-vector
//  * matrices return 0.
//  *
//  * @param mat  Matrix to inspect.
//  * @return Vector length, or 0 if @p mat is NULL or not vector-shaped.
//  */
// size_t ldouble_matrix_vector_length(const ldouble_matrix_t* mat);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_ldouble_H */
// ================================================================================
// ================================================================================
// eof
