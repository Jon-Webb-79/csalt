// ================================================================================
// ================================================================================
// - File:    c_double.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 04, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_double_H
#define c_double_H
// ================================================================================ 
// ================================================================================ 

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <float.h>
#include "c_string.h"
#include "c_error.h"
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

#ifndef ITER_DIR_H
#define ITER_DIR_H
    typedef enum {
        FORWARD = 0,
        REVERSE = 1
    }iter_dir;
#endif /* ITER_DIR_H*/
// --------------------------------------------------------------------------------    

#ifndef ALLOC_H 
#define ALLOC_H 
    typedef enum {
        STATIC = 0,
        DYNAMIC = 1
    } alloc_t;

#endif /*ALLOC_H*/

typedef struct {
    double* data;
    size_t len;
    size_t alloc;
    ErrorCode error;
    alloc_t alloc_type;
} double_v;
// --------------------------------------------------------------------------------

/**
 * @brief Create a dynamically allocated vector of @c double with given capacity.
 *
 * Allocates a @ref double_v control block and a zero-initialized data buffer
 * of @p buff elements on the heap. On success, the vector is configured for
 * dynamic ownership ( @ref alloc_t :: DYNAMIC ), its size is set to 0, and
 * its last-error field is cleared to @c NO_ERROR.
 *
 * @param buff
 *     Requested capacity in elements (must be > 0).
 *
 * @return
 *     Pointer to a newly created @ref double_v on success; @c NULL on failure.
 *
 * @par Errors
 * - Sets @c errno to:
 *   - @c EINVAL if @p buff == 0
 *   - @c ENOMEM if allocation of the control block or data buffer fails
 *
 * @post
 * - On success:
 *   - @c result->data != NULL
 *   - @c result->len == 0
 *   - @c result->alloc == buff
 *   - @c result->alloc_type == DYNAMIC
 *   - @c result->error == NO_ERROR
 * - On failure:
 *   - Returns @c NULL and sets @c errno as above. No allocations are leaked.
 *
 * @attention Ownership
 * The returned vector and its buffer are heap-allocated. Free them with the
 * library’s designated destructor (e.g., @c free_double_vector(result) ), which
 * must release both the data buffer and the control block. Do not @c free()
 * the @c data pointer separately.
 *
 * **thread_safety**
 * Not thread-safe. External synchronization is required if accessed from
 * multiple threads.
 *
 * **complexity**
 * O(1) time; O(@p buff) zero-initialization of the data buffer.
 *
 * @code
 * double_v* v = init_double_vector(128);
 * if (!v) {
 *     perror("init_double_vector");
 *     return 1;
 * }
 * // ...
 * free_double_vector(v); // library-provided destructor
 * @endcode
 */
double_v* init_double_vector(size_t buffer);
// -------------------------------------------------------------------------------- 

#if defined(DOXYGEN)

/**
 * \def init_double_array(size)
 * \brief Create a ::double_v that wraps a zero-initialized, fixed-capacity array.
 *
 * Expands to a value of type ::float_v with:
 * - data       → points to `(double[size]){0}` (automatic storage, zero-initialized)
 * - len        → 0
 * - alloc      → size
 * - error      → NO_ERROR
 * - alloc_type → ::STATIC (non-owned, fixed capacity)
 *
 * \param size Number of elements (must be > 0). Evaluated twice — avoid side effects.
 *
 * \warning The backing storage has **automatic duration** and is valid **only
 * within the enclosing block**. Do not return the vector or store pointers to
 * its data beyond that scope.
 *
 * \note Requires C99 compound literals; if `__STDC_NO_GENERIC__` is defined, \p size
 * must be a compile-time constant.
 *
 * \par Example (macro enabled)
 * \code
 * void demo_macro(void) {
 *     // Storage is automatic: valid only inside this block.
 *     double_v v = init_double_array(16);
 *
 *     // Fill first three elements and record logical length
 *     push_back_double_vector(&v, 1.0);
 *     push_back_double_vector(&v, 2.0);
 *     push_back_double_vector(&v, 3.0);
 *     size_t len = double_vector_size(&v);
 *
 *     // Use only APIs that respect STATIC semantics (no reallocation/free).
 * } // v.data becomes invalid here
 * \endcode
 */
#define init_double_array(size) /* docs-only definition */

/**
 * \brief Wrap caller-provided storage as a ::double_v with ::STATIC semantics.
 *
 * Use this function as the **supported replacement** for static allocation when
 * the `init_double_array` macro is disabled (e.g., strict/MISRA builds) or when
 * you prefer not to rely on compound literals. The storage lifetime/ownership
 * remains with the caller.
 *
 * \param buf Pointer to at least \p n double.
 * \param n   Capacity of \p buf (must be > 0).
 * \return    A ::double_v referring to \p buf with len=0, alloc=n, error=NO_ERROR,
 *            alloc_type=STATIC.
 *
 * \warning The function does **not** allocate or free memory. The caller must
 * manage \p buf and ensure it outlives the ::double_v usage.
 *
 * \par Example A (file-scope static buffer)
 * \code
 * static double file_buf[32];
 *
 * void demo_wrap_filescope(void) {
 *     double_v v = wrap_double_array(file_buf, 32);
 *     push_back_double_vector(&v, 42.0);
 *     size_t size = double_vector_size(&v); // 1
 *     size_t alloc = double_vector_alloc(&v); // 32
 *     // file_buf persists for program lifetime (or translation unit scope).
 * }
 * \endcode
 *
 * \par Example B (local fixed array without compound literals)
 * \code
 * void demo_wrap_local(void) {
 *     double local_buf[8] = {0};          // automatic storage
 *     double_v v = wrap_double_array(local_buf, 8);
 *     // v.data valid until end of this block (same as local_buf)
 * }
 * \endcode
 */
double_v wrap_double_array(float *buf, size_t n);

#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && !defined(__STDC_NO_GENERIC__)

/* Real macro (C99 compound literal). Attach docs via copydoc so they render even if
 * Doxygen parses this branch rather than the DOXYGEN stub above. */
/** \copydoc init_double_array */
#define init_double_array(size)                                                    \
    ((double_v){ .data = (double[(size)]){0}, .len = 0, .alloc = (size),            \
                .error = NO_ERROR, .alloc_type = STATIC })

/* Provide the wrapper in all builds (convenient and portable). */
/** \copydoc wrap_double_array */
static inline double_v wrap_double_array(double *buf, size_t n)
{
    double_v v;
    v.data = buf;
    v.len = 0;
    v.alloc = n;
    v.error = NO_ERROR;
    v.alloc_type = STATIC;
    return v;
}

#else /* Strict/MISRA or non-C99 compilers (macro disabled) */

/* Macro disabled; only the wrapper is available. */
/** \copydoc wrap_double_array */
static inline double_v wrap_double_array(double *buf, size_t n)
{
    double_v v;
    v.data = buf;
    v.len = 0;
    v.alloc = n;
    v.error = NO_ERROR;
    v.alloc_type = STATIC;
    return v;
}

/* Breadcrumb for users trying to use the macro in a disabled build. */
#if !defined(init_double_array)
#define init_double_array(size) /* unavailable: use wrap_float_array(buf, n) */
#endif

#endif /* feature gate */
// --------------------------------------------------------------------------------

/**
 * @brief Get the last nonzero error code recorded on a ::double_v.
 *
 * Returns the value of @p vec->error when it is a nonzero ::ErrorCode set by a
 * prior csalt operation. This accessor is intended for use *after* an API call
 * has failed and stored a diagnostic on the vector.
 *
 * If @p vec is NULL **or** if @p vec->error equals ::NO_ERROR (i.e., no error
 * has been recorded), the function sets @c errno to @c EINVAL and returns
 * ::INVALID_ERROR.
 *
 * @param vec  Pointer to a ::double_v to query (may be NULL).
 *
 * @return
 *   - The nonzero ::ErrorCode stored in @p vec->error on success.
 *   - ::INVALID_ERROR if @p vec is NULL or @p vec->error == ::NO_ERROR.
 *
 * @par Errors
 * - Sets @c errno to @c EINVAL if:
 *   - @p vec is @c NULL, or
 *   - @p vec->error == ::NO_ERROR (no error recorded to report).
 *
 * @note This function does not modify or clear @p vec->error.
 *
 * @warning This accessor deliberately treats “no error recorded” as invalid
 * input for retrieval. Only call it when you expect a prior operation to have
 * set a nonzero error code.
 *
 * **thread_safety** Not thread-safe. Synchronize externally if shared.
 * **complexity** O(1).
 *
 * @code{.c}
 * // Example: retrieving an error after a failed operation
 * if (!push_back_double_vector(v, 3.14f)) {
 *     ErrorCode ec = get_double_vector_error(v);
 *     if (ec == INVALID_ERROR) {
 *         // Either v is NULL or no error was recorded
 *         perror("get_double_vector_error");
 *     } else {
 *         // Handle the specific error code 'ec'
 *     }
 * }
 *
 * // Example: calling when no error was recorded -> INVALID_ERROR + errno=EINVAL
 * ErrorCode ec2 = get_double_vector_error(v);
 * // ec2 == INVALID_ERROR if v->error was NO_ERROR
 * @endcode
 */
static inline ErrorCode get_double_vector_error(const double_v* vec) {
    if (!vec || !vec->error) {
        errno = EINVAL;
        return INVALID_ERROR;
    }
    return vec->error;
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Append a value to a ::double_v, growing capacity if needed.
 *
 * Appends @p value at index @c vec->len and increments the length. If the
 * vector is ::DYNAMIC and at capacity, the function grows the buffer using an
 * adaptive policy:
 *
 * - If @c vec->alloc < ::VEC_THRESHOLD, capacity doubles.
 * - Otherwise, capacity increases by ::VEC_FIXED_AMOUNT.
 *
 * Newly added capacity is zero-initialized.
 *
 * @param vec    Pointer to a ::double_v (must be non-NULL and have @c data != NULL).
 * @param value  The @c double value to append.
 *
 * @return
 * - @c true  on success (value stored, length incremented, @c vec->error = ::NO_ERROR)
 * - @c false on failure (no append; @c vec->error set and @c errno updated)
 *
 * @par Growth policy
 * Controlled by the compile-time macros ::VEC_THRESHOLD and ::VEC_FIXED_AMOUNT.
 * When reallocation occurs, @c vec->data may change; all previous raw pointers
 * into the buffer become invalid.
 *
 * @par STATIC vs DYNAMIC
 * - ::DYNAMIC vectors may grow; reallocation can occur.
 * - ::STATIC vectors cannot grow. If an append would exceed @c alloc, the call
 *   fails with ::INVALID_ARG.
 *
 * @par Errors
 * On failure the function returns @c false and sets @c vec->error (when @p vec is
 * non-NULL) and @c errno via @c set_errno_from_error():
 * - ::NULL_POINTER if @p vec is non-NULL but @c vec->data is @c NULL
 *   (also sets @c errno = set_errno_from_error(::NULL_POINTER)).
 * - ::INVALID_ARG if @p vec is ::STATIC and @c vec->len == @c vec->alloc
 *   (also sets @c errno accordingly).
 * - ::REALLOC_FAIL if a reallocation attempt fails
 *   (also sets @c errno accordingly).
 * If @p vec is @c NULL, sets @c errno = @c EINVAL and returns @c false.
 *
 * @post
 * - Success: one element appended; @c vec->len increased by 1; @c vec->error = ::NO_ERROR.
 * - Failure: no change to contents or length; for reallocation failure the original
 *   buffer remains valid (strong exception-safety for data and length).
 *
 * @warning
 * - Reallocation may occur; any saved pointers into @c vec->data become invalid.
 * - Do not call on ::STATIC vectors once full; resize is not permitted.
 * - This function is not thread-safe.
 *
 * **complexity**
 * - Amortized O(1) appends for ::DYNAMIC vectors.
 * - O(n) when a growth step occurs (due to @c realloc + zero-initialization of new tail).
 *
 * @code{.c}
 * // Example 1: dynamic vector append
 * double_v* v = init_double_vector(4);
 * if (!v) { perror("init_double_vector"); return; }
 * for (int i = 0; i < 10; ++i) {
 *     if (!push_back_double_vector(v, (double)i)) {
 *         perror("push_back_double_vector");
 *         // v->error contains a csalt-specific code (e.g., REALLOC_FAIL)
 *         free_double_vector(v);
 *         return;
 *     }
 * }
 * // v->len == 10; capacity likely grew according to the policy
 * free_double_vector(v);
 *
 * // Example 2: static vector fails when full
 * {
 *     double_v s = init_double_array(2);  // STATIC storage
 *     (void)push_back_double_vector(&s, 1.0f);  // ok
 *     (void)push_back_double_vector(&s, 2.0f);  // ok
 *     if (!push_back_double_vector(&s, 3.0f)) { // exceeds capacity
 *         // s.error == INVALID_ARG; errno set via set_errno_from_error()
 *     }
 * } // s.data lifetime ends with the block
 * @endcode
 */
bool push_back_double_vector(double_v* vec, double value);
// --------------------------------------------------------------------------------

/**
 * @brief Prepend a value to a ::double_v, shifting elements right; grows if needed.
 *
 * Inserts @p value at index 0. If the vector is ::DYNAMIC and at capacity, the
 * function grows the buffer using an adaptive policy (see **Growth policy**).
 * After ensuring capacity, existing elements `[0 .. len-1]` are shifted one
 * slot to the right via `memmove`, then `data[0]` is set and `len` incremented.
 *
 * @param vec    Pointer to a ::double_v (must be non-NULL and have `data != NULL`).
 * @param value  The @c double value to insert at the front.
 *
 * @return
 * - `true`  on success (value prepended, length incremented, `vec->error = ::NO_ERROR`)
 * - `false` on failure (no prepend; `vec->error` set and `errno` updated)
 *
 * @par Growth policy
 * When `len == alloc` and `alloc_type == ::DYNAMIC`, capacity is increased:
 * - If `alloc < ::VEC_THRESHOLD`, capacity doubles.
 * - Otherwise, capacity grows by `::VEC_FIXED_AMOUNT`.
 * Newly added capacity is zero-initialized. Reallocation may change `vec->data`.
 *
 * @par STATIC vs DYNAMIC
 * - ::DYNAMIC vectors may grow; reallocation can occur.
 * - ::STATIC vectors cannot grow. If an insert would exceed `alloc`, the call
 *   fails with ::INVALID_ARG.
 *
 * @par Errors
 * On failure the function returns `false` and sets `vec->error` (when `vec` is non-NULL)
 * and `errno` via `set_errno_from_error()`:
 * - ::NULL_POINTER if `vec` is non-NULL but `vec->data` is `NULL`
 * - ::INVALID_ARG  if `vec` is ::STATIC and `len == alloc`
 * - ::REALLOC_FAIL if a growth attempt fails
 * If `vec` is `NULL`, sets `errno = EINVAL` and returns `false`.
 *
 * @post
 * - Success: `data[0] == value`, prior elements shifted to `[1 .. old_len]`,
 *   `len` increased by 1, `error == ::NO_ERROR`.
 * - Failure: contents and `len` unchanged; on reallocation failure, the original
 *   buffer remains valid.
 *
 * @warning
 * - Reallocation (when growing) may invalidate any saved pointers to `vec->data`.
 * - Even without reallocation, the front insert shifts elements; any previously
 *   saved element addresses may now point to different values.
 * - Not thread-safe.
 *
 * **complexity**
 * - O(n) for the shift (`memmove`) on every successful prepend.
 * - If a growth step occurs: O(n) for reallocation + O(k) for zero-initializing new tail.
 *
 * @code{.c}
 * // Example 1: dynamic vector, multiple prepends
 * double_v* v = init_double_vector(2);
 * if (!v) { perror("init_double_vector"); return; }
 * (void)push_front_double_vector(v, 2.0);  // [2]
 * (void)push_front_double_vector(v, 1.0);  // [1, 2]
 * (void)push_front_double_vector(v, 0.0);  // grows if needed -> [0, 1, 2]
 * // v->len == 3
 * free_double_vector(v);
 *
 * // Example 2: static vector fails when full
 * {
 *     double_v s = init_double_array(1);     // STATIC storage, capacity 1
 *     (void)push_front_double_vector(&s, 1.0); // ok -> [1]
 *     if (!push_front_double_vector(&s, 2.0)) { // would exceed capacity
 *         // s.error == INVALID_ARG; errno set via set_errno_from_error()
 *     }
 * } // s.data lifetime ends with the block
 * @endcode
 */
bool push_front_double_vector(double_v* vec, double value);
// --------------------------------------------------------------------------------

/**
 * @brief Insert a value at @p index in a ::double_v, shifting the tail right; grows if needed.
 *
 * Inserts @p value at position @p index, where @p index may be anywhere in
 * `[0, vec->len]`. An @p index equal to `vec->len` is equivalent to append;
 * `index == 0` is equivalent to a front insert.
 *
 * For ::DYNAMIC vectors that are full (`len == alloc`), capacity is increased
 * using the library’s adaptive policy (see **Growth policy**). Newly added
 * capacity is zero-initialized. Existing elements in `[index .. len-1]` are
 * shifted one slot to the right via `memmove`, then the new element is written.
 *
 * @param vec    Pointer to a ::double_v (must be non-NULL and have `data != NULL`).
 * @param value  The @c double value to insert.
 * @param index  Insertion position in `[0, vec->len]`.
 *
 * @return
 * - `true`  on success (value inserted, length incremented, `vec->error = ::NO_ERROR`)
 * - `false` on failure (no insert; `vec->error` set and `errno` updated)
 *
 * @par Indexing
 * - Valid indices: `0 .. vec->len`
 * - `index > vec->len` → failure with ::OUT_OF_BOUNDS.
 *
 * @par Growth policy
 * When `len == alloc` and `alloc_type == ::DYNAMIC`, capacity increases:
 * - If `alloc < ::VEC_THRESHOLD`, capacity doubles.
 * - Otherwise, capacity grows by `::VEC_FIXED_AMOUNT`.
 * Reallocation may change `vec->data`; any saved pointers become invalid.
 *
 * @par STATIC vs DYNAMIC
 * - ::DYNAMIC vectors may grow and reallocate.
 * - ::STATIC vectors cannot grow. If an insert would exceed `alloc`, the call
 *   fails with ::INVALID_ARG.
 *
 * @par Errors
 * On failure the function returns `false` and sets `vec->error` (when `vec` is non-NULL)
 * and `errno` via `set_errno_from_error()`:
 * - ::NULL_POINTER if `vec` is non-NULL but `vec->data` is `NULL`
 * - ::OUT_OF_BOUNDS if `index > vec->len`
 * - ::INVALID_ARG  if `vec` is ::STATIC and `len == alloc`
 * - ::REALLOC_FAIL if a growth attempt fails
 * - ::SIZE_MISMATCH if an internal size computation for the shift would overflow
 * If `vec` is `NULL`, sets `errno = EINVAL` and returns `false`.
 *
 * @post
 * - Success: element placed at `data[index]`; prior elements `[index .. old_len-1]`
 *   moved to `[index+1 .. old_len]`; `len` increased by 1; `error == ::NO_ERROR`.
 * - Failure: contents and `len` unchanged; on reallocation failure, the original
 *   buffer remains valid (strong safety for length + existing data).
 *
 * @warning
 * - Reallocation may invalidate any saved pointers to `vec->data`.
 * - The shifting operation repositions elements; previously saved element addresses
 *   may no longer refer to the same logical item even if no reallocation occurs.
 * - Not thread-safe.
 *
 * **complexity**
 * - O(n - index) for the shift on every successful insert.
 * - If a growth step occurs: O(n) for reallocation + zero-initialization of new tail.
 *
 * @code{.c}
 * // Example 1: dynamic insert in the middle
 * double_v* v = init_double_vector(2);
 * push_back_double_vector(v, 10.0);         // [10]
 * push_back_double_vector(v, 30.0);         // [10, 30]
 * (void)insert_double_vector(v, 20.0, 1);   // [10, 20, 30]
 * free_double_vector(v);
 *
 * // Example 2: append via insert at len
 * double_v* w = init_double_vector(1);
 * (void)insert_double_vector(w, 1.0, 0);             // [1]
 * (void)insert_double_vector(w, 2.0, double_vector_size(w));        // append -> [1, 2]
 * free_double_vector(w);
 *
 * // Example 3: static vector fails when full
 * {
 *     double_v s = init_double_array(1);               // STATIC storage
 *     (void)insert_double_vector(&s, 42.0, 0);       // ok
 *     if (!insert_double_vector(&s, 7.0, 0)) {       // would exceed capacity
 *         // s.error == INVALID_ARG; errno set via set_errno_from_error()
 *     }
 * } // s.data lifetime ends with the block
 * @endcode
 */
bool insert_double_vector(double_v* vec, double value, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Return the element at @p index from an ::double_v with bounds checking.
 *
 * On success, returns `vec->data[index]`. On failure, returns the sentinel
 * `DBL_MAX` and sets `errno` to indicate the reason. This accessor does **not**
 * modify `vec->error`; callers should check `errno` (or validate inputs) to
 * distinguish a real value equal to `INT_MAX` from an error.
 *
 * @param vec    Podoubleer to an ::double_v (must be non-NULL and have `data != NULL`).
 * @param index  Zero-based index to access; must satisfy `index < vec->len`.
 *
 * @return
 * - The `double` at position @p index on success.
 * - `DBL_MAX` on failure (and `errno` is set).
 *
 * @par Errors
 * - Sets `errno = EINVAL` if `vec == NULL` or `vec->data == NULL`.
 * - Sets `errno = ERANGE` if `index >= vec->len` (out of bounds).
 *
 * @note This function does not alter `vec->error`. If your vectors can store
 * `DBL_MAX` legitimately, check `errno` (or pre-validate inputs) to disambiguate.
 *
 * @warning
 * - Not thread-safe. Do not call concurrently with writers to the same vector.
 * - Ensure the index is valid (`index < vec->len`). Calling with `vec->len == 0`
 *   is always invalid.
 *
 * @complexity O(1).
 *
 * @code
 * // Success
 * double_v* v = init_double_vector(3);
 * push_back_double_vector(v, 10.0);
 * push_back_double_vector(v, 20.0);
 * errno = 0;
 * double x = double_vector_index(v, 1);   // x == 20, errno == 0
 *
 * // Failure: out of bounds
 * errno = 0;
 * double y = double_vector_index(v, 5);   // y == DBL_MAX, errno == ERANGE
 *
 * // Failure: NULL vector
 * errno = 0;
 * double z = double_vector_index(NULL, 0); // z == DBL_MAX, errno == EINVAL
 * free_double_vector(v);
 * @endcode
 */
static inline double double_vector_index(const double_v* vec, size_t index) {
    if (!vec || !vec->data) {
        errno = EINVAL;
        return DBL_MAX;
    }
    if (index > vec->len - 1) {
        errno = ERANGE;
        return DBL_MAX;
    }
    return vec->data[index]; 
}
// -------------------------------------------------------------------------------- 

size_t double_vector_size(const double_v* vec);
// -------------------------------------------------------------------------------- 

size_t double_vector_alloc(const double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Remove and return the last element of an ::double_v.
 *
 * Decrements @p vec->len by one, returns the previous last value, and
 * zero-initializes the vacated slot. Works for both ::STATIC and ::DYNAMIC
 * vectors since no reallocation occurs.
 *
 * @param vec  Podoubleer to an ::double_v (must be non-NULL with @c data != NULL and @c len > 0).
 *
 * @return
 * - The removed @c double value on success (and sets @c vec->error = ::NO_ERROR).
 * - @c DBL_MAX on failure (no removal performed).
 *
 * @par Errors
 * On failure the function returns @c INT_MAX and:
 * - If @p vec is non-NULL but @c vec->data is @c NULL:
 *   - sets @c vec->error = ::NULL_POINTER and @c errno via @c set_errno_from_error()
 * - If @p vec is non-NULL and @c vec->len == 0 (empty vector):
 *   - sets @c vec->error = ::INVALID_ARG and @c errno accordingly
 * - If @p vec is @c NULL:
 *   - sets @c errno = @c EINVAL
 *
 * @post
 * - Success: @c vec->len is decremented by 1; @c vec->data[vec->len] is set to 0;
 *   @c vec->error == ::NO_ERROR; returned value is the removed element.
 * - Failure: @c vec and its contents are unchanged.
 *
 * @note The error sentinel is @c DBL_MAX (from <limits.h>). If your vector can
 * legitimately contain @c DBL_MAX, distinguish success from failure by checking
 * @c vec->error (or @c errno) after the call.
 *
 * @warning Not thread-safe. External synchronization is required for concurrent use.
 *
 * **complexity** O(1).
 *
 * @code{.c}
 * // Example: pop from a dynamic vector
 * double_v* v = init_double_vector(4);
 * push_back_double_vector(v, 10.0);
 * push_back_double_vector(v, 20.0);
 * double last = pop_back_double_vector(v);  // last == 20, v->len decremented, slot zeroed
 * if (last == DBL_MAX && v->error != NO_ERROR) {
 *     perror("pop_back_double_vector");
 * }
 * free_double_vector(v);
 *
 * // Example: popping from an empty vector -> failure
 * double_v* w = init_double_vector(1);
 * double val = pop_back_double_vector(w);   // empty -> returns INT_MAX, w->error == INVALID_ARG
 * free_double_vector(w);
 * @endcode
 */
double pop_back_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief Remove and return the first element of an ::double_v, shifting the rest left.
 *
 * Removes the element at index 0, shifts the remaining `len-1` elements one slot
 * to the left via `memmove`, zeroes the vacated tail slot, decrements `len`, and
 * returns the removed value. Works for both ::STATIC and ::DYNAMIC vectors; no
 * reallocation occurs.
 *
 * @param vec  Podoubleer to an ::double_v (must be non-NULL with `data != NULL` and `len > 0`).
 *
 * @return
 * - The removed `double` value on success (and sets `vec->error = ::NO_ERROR`).
 * - `DBL_MAX` on failure (no removal performed).
 *
 * @par Errors
 * On failure the function returns `INT_MAX` and:
 * - If `vec` is non-NULL but `vec->data` is `NULL`:
 *   - sets `vec->error = ::NULL_POINTER` and `errno` via `set_errno_from_error()`
 * - If `vec` is non-NULL and `vec->len == 0` (empty vector):
 *   - sets `vec->error = ::INVALID_ARG` and `errno` accordingly
 * - If `vec` is `NULL`:
 *   - sets `errno = EINVAL`
 *
 * @post
 * - Success: original `data[0]` is returned; elements `[1 .. old_len-1]` are moved to
 *   `[0 .. old_len-2]`; `data[old_len-1]` is set to `0`; `len` is decremented by 1;
 *   `error == ::NO_ERROR`.
 * - Failure: contents and `len` unchanged.
 *
 * @note The error sentinel is `DBL_MAX` (see `<limits.h>`). If your vector can
 * legitimately contain `DBL_MAX`, distinguish success from failure by checking
 * `vec->error` (or `errno`) after the call.
 *
 * @warning
 * - This operation is O(n) due to the left shift; any saved podoubleers/references to
 *   elements may no longer refer to the same logical item afterward.
 * - Not thread-safe. Synchronize externally if shared.
 *
 * **complexity**
 * - O(n) for the shift; O(1) otherwise.
 *
 * @code{.c}
 * // Example: pop from the front of a dynamic vector
 * double_v* v = init_double_vector(4);
 * push_back_double_vector(v, 10.0);
 * push_back_double_vector(v, 20.0);
 * push_back_double_vector(v, 30.0);       // v = [10, 20, 30], len = 3
 *
 * double first = pop_front_double_vector(v); // first == 10, v -> [20, 30, 0], len = 2
 * if (first == DBL_MAX && v->error != NO_ERROR) {
 *     perror("pop_front_double_vector");
 * }
 * free_double_vector(v);
 *
 * // Example: popping from an empty vector -> failure
 * double_v* w = init_double_vector(1);
 * double val = pop_front_double_vector(w);   // returns DBL_MAX, w->error == INVALID_ARG
 * free_double_vector(w);
 * @endcode
 */
double pop_front_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Remove and return the element at @p index from an ::double_v, shifting the tail left.
 *
 * Removes the element at position @p index (0-based), shifts any following elements
 * one slot to the left via `memmove`, zeroes the vacated tail slot, decrements
 * `len`, and returns the removed value. Works for both ::STATIC and ::DYNAMIC
 * vectors; no reallocation occurs.
 *
 * @param vec    Podoubleer to an ::double_v (must be non-NULL and have `data != NULL`).
 * @param index  Position of the element to remove. Must satisfy `index < vec->len`.
 *
 * @return
 * - The removed `double` value on success (and sets `vec->error = ::NO_ERROR`).
 * - `DBL_MAX` on failure (no removal performed).
 *
 * @par Valid indices
 * - `0 <= index < vec->len`
 * - `index == vec->len - 1` removes the last element (no shift needed).
 *
 * @par Errors
 * On failure the function returns `DBL_MAX` and:
 * - If `vec` is non-NULL but `vec->data` is `NULL`:
 *   - sets `vec->error = ::NULL_POINTER` and `errno` via `set_errno_from_error()`
 * - If `vec` is non-NULL and `vec->len == 0` (empty vector):
 *   - sets `vec->error = ::INVALID_ARG` and `errno` accordingly
 * - If `index >= vec->len`:
 *   - sets `vec->error = ::OUT_OF_BOUNDS` and `errno` accordingly
 * - If the doubleernal byte count for `memmove` would overflow `size_t`:
 *   - sets `vec->error = ::SIZE_MISMATCH` and `errno` accordingly
 * - If `vec` is `NULL`:
 *   - sets `errno = EINVAL`
 *
 * @post
 * - Success: element at `data[index]` is returned; elements `[index+1 .. old_len-1]`
 *   are moved to `[index .. old_len-2]`; `data[old_len-1]` is set to `0`; `len`
 *   is decremented by 1; `error == ::NO_ERROR`.
 * - Failure: contents and `len` are unchanged.
 *
 * @note The error sentinel is `DBL_MAX` (see `<limits.h>`). If your vector can
 * legitimately contain `DBL_MAX`, distinguish success from failure by checking
 * `vec->error` (or `errno`) after the call.
 *
 * @warning
 * - This operation is O(n - index) due to the left shift; any saved podoubleers/
 *   references to elements may no longer refer to the same logical item afterward.
 * - Not thread-safe. Synchronize externally if shared.
 *
 * **complexity**
 * - O(n - index) for the shift; O(1) otherwise.
 *
 * @code{.c}
 * // Example 1: remove middle element
 * double_v* v = init_double_vector(4);
 * push_back_double_vector(v, 10); // [10]
 * push_back_double_vector(v, 20); // [10,20]
 * push_back_double_vector(v, 30); // [10,20,30]
 * double removed = pop_any_double_vector(v, 1); // removes 20; v -> [10,30,0], len=2
 * if (removed == DBL_MAX && v->error != NO_ERROR) {
 *     perror("pop_any_double_vector");
 * }
 * free_double_vector(v);
 *
 * // Example 2: remove last element (no shift)
 * double_v* w = init_double_vector(2);
 * push_back_double_vector(w, 1.0);  // [1]
 * push_back_double_vector(w, 2.0);  // [1,2]
 * (void)pop_any_double_vector(w, w->len - 1); // removes 2; w -> [1,0], len=1
 * free_double_vector(w);
 *
 * // Example 3: invalid index
 * double_v* z = init_double_vector(1);
 * push_back_double_vector(z, 5.0);  // [5]
 * double bad = pop_any_double_vector(z, 3); // OUT_OF_BOUNDS -> returns DBL_MAX
 * free_double_vector(z);
 * @endcode
 */
double pop_any_double_vector(double_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Destroy a dynamically allocated ::double_v and its heap buffer.
 *
 * Frees the data buffer (if present) and then frees the vector control block
 * itself. This function is only valid for vectors created with dynamic
 * ownership (e.g., @ref init_double_vector) where @c alloc_type == ::DYNAMIC.
 *
 * @param vec  Pointer to a ::double_v previously created with dynamic allocation.
 *             Must be non-NULL and have @c alloc_type == ::DYNAMIC.
 *
 * @par Errors
 * - Sets @c errno to @c EINVAL and returns without freeing if:
 *   - @p vec is @c NULL, or
 *   - @p vec->alloc_type is ::STATIC (i.e., vector wraps caller-owned storage).
 *
 * @post
 * - On success, both @p vec->data and @p vec are freed; @p vec becomes a
 *   dangling pointer. The caller should set their pointer to @c NULL after
 *   calling this function.
 *
 * @warning
 * - Do **not** call this function on vectors created with ::STATIC semantics,
 *   such as those from @ref init_double_array or @ref wrap_double_array; the
 *   storage is caller-owned and must not be freed here. Simply let the
 *   wrapper object go out of scope (or manage its lifetime yourself).
 * - Calling this function more than once on the same pointer results in
 *   undefined behavior (double-free). Set your pointer to @c NULL after use.
 *
 * **thread_safety** Not thread-safe. Synchronize externally if shared.
 * **complexity**    O(1).
 *
 * @code{.c}
 * // Correct: dynamic vector
 * double_v* v = init_double_vector(64);
 * if (!v) { perror("init_double_vector"); return; }
 * // ... use v ...
 * free_double_vector(v);
 * v = NULL; // prevent accidental reuse
 *
 * // Incorrect: static vector (will set errno=EINVAL and return)
 * {
 *     double_v s = init_double_array(16);   // STATIC storage, automatic duration
 *     // free_double_vector(&s);           // <-- don't do this
 * } // s.data becomes invalid here automatically
 *
 * // Correct: static wrapper when macros are disabled or avoided
 * {
 *     double buf[8] = {0};
 *     double_v w = wrap_double_array(buf, 8); // STATIC, caller-owned buffer
 *     // ... use w ...
 *     // Do NOT call free_double_vector(&w); just let 'w' go out of scope.
 * }
 * @endcode
 */
void free_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

void _free_double_vector(double_v** vec);
// --------------------------------------------------------------------------------

#if defined(DOXYGEN)

/**
 * \def DBLEVEC_GBC
 * \brief GCC/Clang cleanup attribute that auto-frees a ::double_v* on scope exit.
 *
 * When applied to a variable of type `double_v*`, the pointer will be passed to an internal
 * cleanup handler at scope exit, which calls ::free_double_vector on it and then nulls it.
 *
 * **Availability:** Defined only for GCC/Clang builds when the project does not define
 * `__STDC_NO_GENERIC__` (used here as a global “strict/MISRA” switch). In other builds the
 * macro expands to nothing.
 *
 * \par Correct usage
 * \code{.c}
 * void demo(void) {
 *     double_v* v DBLEVEC_GBC = init_double_vector(256);
 *     if (!v) { perror("init_double_vector"); return; }
 *     // ... use v ...
 * } // v is automatically freed here
 * \endcode
 *
 * \par Returning ownership (cancel auto-free)
 * \code{.c}
 * double_v* make_vec(void) {
 *     double_v* v DBLEVEC_GBC = init_double_vector(64);
 *     if (!v) return NULL;
 *     // ... populate v ...
 *     double_v* out = v;   // transfer ownership
 *     v = NULL;           // cancel cleanup (handler sees NULL and does nothing)
 *     return out;
 * }
 * \endcode
 *
 * \warning Apply to **`double_v*` variables only** (not to `double_v` by value).
 * Do not use for ::STATIC wrappers (e.g., from init_double_array / wrap_double_array).
 * The handler calls ::free_double_vector, which only frees ::DYNAMIC vectors.
 *
 * \note This is a compiler extension (not ISO C). MSVC and some toolchains do not support it.
 * On such builds `DBLEVEC_GBC` is a no-op.
 */
#define DBLEVEC_GBC

/**
 * \brief Cleanup handler used by ::DBLEVEC_GBC (documentation stub).
 * \param pp  Address of a `double_v*` variable. If non-NULL, frees `*pp` and sets it to NULL.
 * \internal Users should not call this directly.
 */
void _free_double_vector(double_v **pp);

#elif (defined(__GNUC__) || defined(__clang__)) && !defined(__STDC_NO_GENERIC__)

/* Apply as a suffix: double_v* v DBLEVEC_GBC = init_double_vector(...); */
#define DBLEVEC_GBC __attribute__((cleanup(_free_double_vector)))

#else  /* Portable/strict builds: make it a no-op so code still compiles cleanly. */

/* No cleanup attribute available (or globally disabled by __STDC_NO_GENERIC__). */
#define DBLEVEC_GBC /* no-op: call free_double_vector() manually */

#endif /* feature gate */
// -------------------------------------------------------------------------------- 

void reverse_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

void swap_double(double* a, double* b);
// --------------------------------------------------------------------------------

void sort_double_vector(double_v* vec, iter_dir direction);
// -------------------------------------------------------------------------------- 

void trim_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

size_t binary_search_double_vector(double_v* vec, double value, double tolerance, bool sort_first);
// -------------------------------------------------------------------------------- 

#ifndef BIN_DAT_TYPEDEF
#define BIN_DAT_TYPEDEF
typedef struct {
    size_t lower;
    size_t upper;
} bin_dat;
#endif
// -------------------------------------------------------------------------------- 

bin_dat binary_search_bounds_double_vector(double_v* vec,
                                           double value,
                                           double tolerance,
                                           bool sort_first);

// -------------------------------------------------------------------------------- 

void update_double_vector(double_v* vec, size_t index, double replacement_value);
// -------------------------------------------------------------------------------- 

double min_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

double max_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

double sum_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

double average_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

double stdev_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

double_v* cum_sum_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

double_v* copy_double_vector(const double_v* original);
// ================================================================================ 
// ================================================================================ 

#ifndef MATRIX_ENUM
/**
 * @enum MatrixStorageType
 * @brief Enum representing the storage format of a matrix.
 *
 * Used to determine how the matrix data is stored and which union member to access.
 */
typedef enum {
    DENSE_MATRIX,
    SPARSE_COO_MATRIX,  // Coordinate list format
    SPARSE_CSR_MATRIX   // Compressed sparse row format
} MatrixStorageType;
#endif /* MATRIX_ENUM */
// -------------------------------------------------------------------------------- 

/**
 * @struct matrix_t
 * @brief Adaptive 2D matrix supporting dense and sparse storage formats.
 *
 * This structure allows storage of a matrix using different internal representations
 * depending on size and sparsity. It supports:
 * - Dense storage using a double array and optional initialization flags.
 * - Coordinate list (COO) sparse storage with dynamic allocation.
 * - Compressed sparse row (CSR) storage for fast row-based access.
 *
 * The actual format is selected using the `type` field and stored in a union.
 */
typedef struct {
    union {
        struct { // Dense storage
            double* data;
            uint8_t* init;
        } dense;
        
        struct { // Coordinate list format
            size_t* rows;
            size_t* cols;
            double* values;
            size_t capacity;
        } coo;
        
        struct { // Compressed sparse row format
            size_t* row_ptrs;
            size_t* col_indices;
            double* values;
        } csr;
    } storage;
    
    MatrixStorageType type;
    size_t rows;
    size_t cols;
    size_t count;  // Non-zero count
} matrix_d;
// ================================================================================ 
// ================================================================================ 

/**
 * @brief Creates an dense matrix of a double data type
 *
 * @param rows Number of matrix rows.
 * @param cols Number of matrix columns.
 *
 * @returns Pointer to a dynamically allocated `matrix_d` structure,
 *          or NULL on allocation failure (errno is set to ENOMEM).
 */
matrix_d* create_double_dense_matrix(size_t rows, size_t cols);
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a sparse coordinate list matrix of a double data type
 *
 * @param rows Number of matrix rows.
 * @param cols Number of matrix columns.
 *
 * @returns Pointer to a dynamically allocated `matrix_d` structure,
 *          or NULL on allocation failure (errno is set to ENOMEM).
 */
matrix_d* create_double_coo_matrix(size_t rows, size_t cols);
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a compressed sparse row matrix of a double data type
 *
 * @param rows Number of matrix rows.
 * @param cols Number of matrix columns.
 * @param estimated_zeros Estimated number of zero values (used to infer sparsity).
 *                        For example, use `rows * cols` to force dense allocation,
 *                        or `rows` for a diagonal matrix. 
 *
 * @returns Pointer to a dynamically allocated `matrix_d` structure,
 *          or NULL on allocation failure (errno is set to ENOMEM).
 */
matrix_d* create_double_csr_matrix(size_t rows, size_t cols, size_t nnz); 
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates an adaptive matrix based on estimated sparsity of a double data type
 *
 * This function selects the most efficient internal format (dense or sparse COO)
 * depending on the matrix dimensions and the estimated number of non-zero values.
 * For very sparse or large matrices, coordinate list (COO) format is used.
 * For small or dense matrices, standard dense format is selected.
 *
 * @param rows Number of matrix rows.
 * @param cols Number of matrix columns.
 * @param estimated_zeros Estimated number of zero values (used to infer sparsity).
 *                        For example, use `rows * cols` to force dense allocation,
 *                        or `rows` for a diagonal matrix.
 *
 * @returns Pointer to a dynamically allocated `matrix_d` structure,
 *          or NULL on allocation failure (errno is set to ENOMEM).
 */
matrix_d* create_double_matrix(size_t rows, size_t cols, size_t estimated_zeros);
// -------------------------------------------------------------------------------- 

/**
 * @brief Frees all memory associated with a matrix.
 *
 * Deallocates all internal arrays associated with the matrix's current storage format
 * and then frees the matrix structure itself. Safely handles NULL pointers and avoids
 * double-free issues.
 *
 * @param mat Pointer to the matrix to be deallocated. If NULL, the function does nothing.
 */
void free_double_matrix(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * \internal
 * @brief Frees all memory associated with a matrix.
 *
 * Deallocates all internal arrays associated with the matrix's current storage format
 * and then frees the matrix structure itself. Safely handles NULL pointers and avoids
 * double-free issues.  This function is specific to dense matrices.
 * Not recommended for average users, implement free_matrix instead
 *
 * @param mat Pointer to the matrix to be deallocated. If NULL, the function does nothing.
 */
void free_double_dense_matrix(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Frees all memory associated with a matrix.
 *
 * Deallocates all internal arrays associated with the matrix's current storage format
 * and then frees the matrix structure itself. Safely handles NULL pointers and avoids
 * double-free issues.  This function is specific to sparse csr matrices.
 * Not recommended for average users, implement free_matrix instead
 *
 * @param mat Pointer to the matrix to be deallocated. If NULL, the function does nothing.
 */
void free_double_csr_matrix(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Frees all memory associated with a matrix.
 *
 * Deallocates all internal arrays associated with the matrix's current storage format
 * and then frees the matrix structure itself. Safely handles NULL pointers and avoids
 * double-free issues. This function is specific to sparse coo matrices
 * Not recommended for average users, implement free_matrix instead
 *
 * @param mat Pointer to the matrix to be deallocated. If NULL, the function does nothing.
 */
void free_double_coo_matrix(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
* @function _free_matrix
* @brief Helper function for garbage collection of double vectors
*
* Used with FLTVMAT_GBC macro for automatic cleanup.
*
* @param mat Double pointer to double matrix to free
* @return void
*/
void _free_double_matrix(matrix_d** mat);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro FLTMAT_GBC
     * @brief A macro for enabling automatic cleanup of double matrix objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_matrix`
     * when the scope ends, ensuring proper memory management.
     */
    #define DBLMAT_GBC __attribute__((cleanup(_free_double_matrix)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief Inserts a new value into a matrix at the specified row and column.
 *
 * This function acts as a format-agnostic wrapper for inserting values into a matrix.
 * It supports dense, COO, and CSR formats. If the matrix becomes large or sparse, it
 * may be automatically converted to a more optimal format (e.g., dense → COO, COO → CSR).
 *
 * @param[in,out] pmat Pointer to the matrix pointer. May be reallocated and updated 
 *                     internally if format conversion occurs.
 * @param row Zero-based row index.
 * @param col Zero-based column index.
 * @param value Value to insert.
 * @param convert_to_csr true if an insert should trigger a conversion to csr, false otherwise
 * @return true if the insertion was successful, false otherwise.
 *
 * @retval false if:
 *         - The entry already exists (errno = EEXIST).
 *         - The matrix pointer is NULL (errno = EINVAL).
 *         - Indices are out of bounds (errno = ERANGE).
 *         - Memory allocation failed (errno = ENOMEM).
 *
 * @note If the matrix format is converted during insertion, the pointer referenced by
 *       `*pmat` is updated to the new matrix object.
 */
bool insert_double_matrix(matrix_d** mat, size_t row, size_t col, double value, bool convert_to_csr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Inserts a value into a dense matrix.
 *
 * Fails if the element has already been initialized.
 *
 * @see insert_matrix for the generic wrapper.
 */
bool insert_double_dense_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Attempts to insert a value into a CSR matrix. Not supported for new entries.
 *
 * @note This function will fail with errno = EEXIST even if the element does not exist,
 *       because dynamic insertion into CSR format is not implemented.
 *       Use COO format for inserts, then convert to CSR with convert_double_coo_to_csr().
 */
bool insert_double_csr_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Inserts a value into a COO-format sparse matrix.
 *
 * Automatically grows internal storage if capacity is exceeded.
 * Fails if the (row, col) entry already exists.
 */
bool insert_double_coo_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Updates an existing value in the matrix at the specified row and column.
 *
 * This function modifies the value of an already-initialized matrix element.
 * No format conversion is performed; the matrix's structure remains unchanged.
 *
 * @param mat Pointer to the matrix.
 * @param row Zero-based row index.
 * @param col Zero-based column index.
 * @param value New value to assign to the existing entry.
 * @return true if the update was successful, false otherwise.
 *
 * @retval false if:
 *         - The matrix pointer is NULL (errno = EINVAL)
 *         - The row or column is out of bounds (errno = ERANGE)
 *         - The entry is uninitialized (errno = ENOENT or EINVAL)
 */
bool update_double_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Updates an element in a dense matrix.
 *
 * Overwrites the existing value at the given row and column.
 */
bool update_double_dense_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Updates an element in a CSR matrix.
 *
 * Replaces the value associated with the given (row, col) pair.
 * Fails if the entry is not found.
 */
bool update_double_csr_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Updates an element in a COO matrix.
 *
 * Fails if the target (row, col) entry does not exist.
 */
bool update_double_coo_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Checks whether a matrix element is initialized or present.
 *
 * This function returns true if the specified entry has been explicitly set,
 * and false if it is uninitialized (i.e., absent from sparse formats or
 * not marked as initialized in dense storage).
 *
 * @param mat Pointer to the matrix.
 * @param row Row index.
 * @param col Column index.
 * @returns true if element is initialized, false otherwise.
 * @retval false if out of bounds or invalid input (errno set to ERANGE or EINVAL).
 */
bool is_double_element_initialized(const matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Convert a matrix_d object to dense format in-place.
 *
 * This function replaces the matrix pointed to by `*pmat` with a new matrix
 * in dense format. If the original matrix is in CSR format, it will first
 * be converted to COO, then to dense. The original matrix is freed.
 *
 * @param pmat Pointer to the matrix pointer to convert.
 *
 * @note No conversion is performed if the matrix is already in dense format.
 * @note On error, errno is set to EINVAL (invalid input) or ENOMEM (allocation failure).
 */
void convert_doubleMat_to_dense(matrix_d** pmat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Convert a matrix_d object to COO (coordinate list) format in-place.
 *
 * This function replaces the matrix pointed to by `*pmat` with a new matrix
 * in COO format. If the original matrix is in CSR format, it will be converted
 * directly to COO. The original matrix is freed.
 *
 * @param pmat Pointer to the matrix pointer to convert.
 *
 * @note No conversion is performed if the matrix is already in COO format.
 * @note On error, errno is set to EINVAL (invalid input) or ENOMEM (allocation failure).
 */
void convert_doublemat_to_coo(matrix_d** pmat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Convert a matrix_d object to CSR (compressed sparse row) format in-place.
 *
 * This function replaces the matrix pointed to by `*pmat` with a new matrix
 * in CSR format. If the original matrix is dense, it is first converted to COO,
 * and then to CSR. The original matrix is freed.
 *
 * @param pmat Pointer to the matrix pointer to convert.
 *
 * @note No conversion is performed if the matrix is already in CSR format.
 * @note On error, errno is set to EINVAL (invalid input) or ENOMEM (allocation failure).
 */
void convert_doubleMat_to_csr(matrix_d** pmat);
/**
 * @breif Converts a dense double matrix to a sparse coordinate list format 
 *
 * @param dense_mat A dense double matrix 
 * @return a matrix_d data type in a COO format
 */
// matrix_d* convert_double_dense_to_coo(const matrix_d* dense_mat);
// -------------------------------------------------------------------------------- 

/**
 * @breif Converts a sparse coordinate list double matrix to a sparse compressed storage format 
 *
 * @param dense_mat A dense double matrix 
 * @return a matrix_d data type in a CSR format
 */
// matrix_d* convert_double_coo_to_csr(const matrix_d* coo_mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieves a double value from a matrix at the given row and column.
 *
 * This function accesses the value stored at (row, col) in a matrix.
 * If the element has not been initialized, or the indices are out of bounds,
 * it returns FLT_MAX and sets errno.
 *
 * @param mat Pointer to the matrix.
 * @param row Zero-based row index.
 * @param col Zero-based column index.
 * @returns The value at (row, col), or FLT_MAX on error.
 * @retval FLT_MAX if the input is invalid, out of bounds, or uninitialized.
 */
double get_double_matrix(const matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieves a double value from a dense matrix at the given row and column.
 *
 * This function accesses the value stored at (row, col) in a dense matrix.
 * If the element has not been initialized, or the indices are out of bounds,
 * it returns FLT_MAX and sets errno.
 *
 * @param mat Pointer to the dense matrix.
 * @param row Zero-based row index.
 * @param col Zero-based column index.
 * @returns The value at (row, col), or FLT_MAX on error.
 * @retval FLT_MAX if the input is invalid, out of bounds, or uninitialized.
 */
double get_double_dense_matrix(const matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieves a value from a COO-format sparse matrix.
 *
 * @param mat Pointer to the COO matrix.
 * @param row Zero-based row index.
 * @param col Zero-based column index.
 * @return The value at the given position, or FLT_MAX if not found or on error.
 */
double get_double_coo_matrix(const matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieves a value from a CSR-format sparse matrix.
 *
 * @param mat Pointer to the CSR matrix.
 * @param row Zero-based row index.
 * @param col Zero-based column index.
 * @return The value at the given position, or FLT_MAX if not found or on error.
 */
double get_double_csr_matrix(const matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes and returns a value from a dense matrix.
 *
 * @param mat Pointer to the dense matrix.
 * @param row Row index.
 * @param col Column index.
 * @return The removed value, or FLT_MAX on error.
 */
double pop_double_dense_matrix(matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes and returns a value from a COO-format sparse matrix.
 *
 * @param mat Pointer to the COO matrix.
 * @param row Row index.
 * @param col Column index.
 * @return The removed value, or FLT_MAX on error.
 */
double pop_double_coo_matrix(matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes and returns a value from a CSR-format sparse matrix.
 *
 * Consider optimizing for large arrays in the future with a lazy delete process
 *
 * @param mat Pointer to the CSR matrix.
 * @param row Row index.
 * @param col Column index.
 * @return The removed value, or FLT_MAX on error.
 */
double pop_double_csr_matrix(matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes and returns a value from the matrix at the specified row and column.
 *
 * This function dispatches to the appropriate pop routine based on the matrix's
 * internal storage format (dense, COO, or CSR). The value is removed from the matrix,
 * and the internal count of non-zero or initialized values is updated accordingly.
 *
 * In CSR format, this function performs a lazy deletion using a tombstone marker,
 * and may automatically trigger compaction if the number of deleted entries exceeds
 * a configurable threshold.
 *
 * @param mat Pointer to the matrix from which the value should be removed.
 * @param row Zero-based row index of the element.
 * @param col Zero-based column index of the element.
 * @return The value that was removed, or FLT_MAX on error.
 *
 * @retval FLT_MAX if:
 *         - The matrix is NULL or uninitialized (errno = EINVAL)
 *         - Indices are out of bounds (errno = ERANGE)
 *         - The element does not exist (errno = ENOENT)
 *
 * @note For CSR matrices, this function uses a tombstone deletion strategy. Actual
 *       memory cleanup may be deferred until compaction is triggered.
 */
double pop_double_matrix(matrix_d** mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Compacts a CSR-format matrix by removing tombstone entries.
 *
 * This function rebuilds the CSR matrix by scanning for and removing any entries
 * marked as deleted (i.e., entries where `col_indices[i] == SIZE_MAX`). It rewrites
 * the internal `row_ptrs`, `col_indices`, and `values` arrays to remove gaps and
 * reduce memory waste, while preserving the row ordering of valid elements.
 *
 * This function is typically called automatically after deletions if the number
 * of tombstones exceeds a threshold, but it can also be invoked manually to force
 * compaction at any time.
 *
 * @param mat Pointer to the CSR matrix to be compacted. Must not be NULL.
 *
 * @note This function has no effect on matrices that are not in CSR format.
 * @note The `mat->count` field is updated to reflect the new number of valid entries.
 */
void compact_double_csr_matrix(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief A function to be called one a matrix has been populated to its peak value for conversion to optimum type.
 *
 * @param mat A double pointer to a matrix_d data type.
 * @param true if conversion was successful, false otherwise
 */
bool finalize_double_matrix(matrix_d** mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief returns the type of matrix 
 *
 * @param mat A matrix_d data type 
 * @return DENSE_MATRIX, SPARSE_COO_MATRIX or SPARSE_CSR_MATRIX 
 */
MatrixStorageType get_double_matrix_type(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief returns the number of allocated rws in a matrix_d structure 
 *
 * @param mat a matrix_d data type 
 * @return The number of allocated rows 
 */
size_t get_double_matrix_rows(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief returns the number of allocated columns in a matrix_d structure 
 *
 * @param mat a matrix_d data type
 * @return The number of allocated columns
 */
size_t get_double_matrix_cols(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Returns the number of populated elements in a matrix_d structure 
 *
 * @param mat a matrix_d data type 
 * @return The number of populated elements
 */
size_t get_double_matrix_element_count(matrix_d* mat);
// -------------------------------------------------------------------------------- 

void maybe_convert_double_matrix(matrix_d** pmat, bool convert_to_csr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Inverts a square dense double matrix in-place using Gauss-Jordan elimination.
 *
 * Only applies to matrices of type DENSE_MATRIX. On success, the contents of the matrix
 * are replaced with its inverse. On failure, the matrix is left unchanged.
 *
 * @param mat Pointer to the matrix_d* structure to invert
 * @return A pointer to a matrix_d object
 *
 * @retval EINVAL  if the input is NULL, not square, or not dense
 * @retval ERANGE  if the matrix is singular and cannot be inverted
 * @retval ENOMEM  if memory allocation fails
 */
matrix_d* invert_double_dense_matrix(const matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Transposes a dense matrix in place.
 *
 * If the matrix is square, transposition is done in-place with no memory allocation.
 * If the matrix is rectangular, a new matrix is allocated, filled with transposed values,
 * and the original matrix is freed.
 *
 * @param[in,out] pmat Pointer to a matrix pointer to be transposed
 * @returns true on success, false on failure
 * @retval EINVAL if input is NULL or not a dense matrix
 * @retval ENOMEM if memory allocation fails for rectangular case
 */
bool transpose_double_dense_matrix(matrix_d** pmat);
// --------------------------------------------------------------------------------

/**
 * @brief Transposes a sparse COO matrix in-place.
 *
 * This function swaps row and column indices of all nonzero entries in a matrix
 * stored in COO (coordinate list) format. The matrix dimensions are also updated.
 *
 * Unlike CSR transpose, this operation does not require reallocation.
 *
 * @param pmat Address of the matrix pointer (must be SPARSE_COO_MATRIX)
 * @return true if successful, false on error
 * @retval EINVAL if input is NULL or not a COO matrix
 *
 * Example:
 * @code
 * matrix_d* mat = create_double_matrix(4, 5, 10);
 * // Populate mat...
 * transpose_double_coo_matrix(&mat);
 * @endcode
 */

bool transpose_double_coo_matrix(matrix_d** mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Transposes a CSR matrix in place.
 *
 * Given a matrix in CSR (compressed sparse row) format, this function replaces it
 * with its transpose. This operation reallocates internal data structures, but
 * preserves the original pointer using an in-place update.
 *
 * @param pmat Address of matrix pointer (must point to a CSR matrix)
 * @retval EINVAL if input is NULL or not in CSR format
 * @retval ENOMEM if memory allocation fails during transpose
 *
 * Example:
 * @code
 * matrix_d* mat = create_sparse_matrix(...);
 * transpose_doubleMat(&mat);
 * @endcode
 */

bool transpose_double_csr_matrix(matrix_d** pmat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Transposes a matrix in-place based on its format.
 *
 * This function checks the internal format of the matrix and calls the corresponding
 * transpose function (e.g., COO or CSR). Dense matrix transpose is currently unsupported.
 *
 * @param pmat Address of a pointer to the matrix_d to transpose
 * @return true if transposition succeeds, false otherwise
 * @retval EINVAL if input is NULL or unsupported format
 * @retval ENOMEM if memory allocation fails during CSR transpose
 * @retval ENOTSUP if transpose for dense matrices is not implemented
 *
 * Example:
 * @code
 * matrix_d* mat = create_double_matrix(10, 20, 15);
 * insert_double_matrix(&mat, 1, 2, 3.14f, false);
 * transpose_double_matrix(&mat);
 * @endcode
 */

bool transpose_double_matrix(matrix_d** pmat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a deep copy of a dense double matrix.
 *
 * Allocates a new dense matrix and copies all initialized values and
 * metadata from the source matrix. The new matrix is independent from
 * the original, and must be freed separately.
 *
 * @param mat Pointer to the dense matrix to copy.
 * @return A pointer to a newly allocated dense matrix with copied contents,
 *         or NULL on error.
 * @retval NULL if input is NULL or not a DENSE_MATRIX (sets errno to EINVAL).
 * @retval NULL if memory allocation fails (sets errno to ENOMEM).
 */
matrix_d* copy_double_dense_matrix(const matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a deep copy of a sparse COO (Coordinate List) matrix.
 *
 * Copies row indices, column indices, and values into a newly allocated COO matrix.
 * The copy preserves the same sparsity pattern and dimensions as the original.
 *
 * @param mat Pointer to the COO matrix to copy.
 * @return A pointer to a new matrix_d object with duplicated COO data,
 *         or NULL on error.
 * @retval NULL if input is NULL or not a SPARSE_COO_MATRIX (sets errno to EINVAL).
 * @retval NULL if memory allocation fails (sets errno to ENOMEM).
 */
matrix_d* copy_double_coo_matrix(const matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a deep copy of a sparse CSR (Compressed Sparse Row) matrix.
 *
 * Copies row pointers, column indices, and values into a new CSR matrix.
 * The copy maintains the same structure and numerical values.
 *
 * @param mat Pointer to the CSR matrix to copy.
 * @return A pointer to a new matrix_d object with copied CSR content,
 *         or NULL on error.
 * @retval NULL if input is NULL or not a SPARSE_CSR_MATRIX (sets errno to EINVAL).
 * @retval NULL if memory allocation fails (sets errno to ENOMEM).
 */
matrix_d* copy_double_csr_matrix(const matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a deep copy of a double matrix, regardless of storage type.
 *
 * Dispatches to the appropriate internal copy function depending on whether
 * the matrix is dense, COO, or CSR. Returns a fully independent matrix copy.
 *
 * @param mat Pointer to the matrix to copy.
 * @return A new matrix_d object with the same contents and type, or NULL on error.
 * @retval NULL if input is NULL or has an unrecognized type (sets errno to EINVAL).
 * @retval NULL if memory allocation fails during copying (sets errno to ENOMEM).
 */
matrix_d* copy_double_matrix(const matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Computes the determinant of a dense double matrix.
 *
 * This function calculates the determinant of a matrix stored in dense format
 * using Gaussian elimination with partial pivoting. The input matrix must be
 * square and of type `DENSE_MATRIX`. The original matrix is not modified.
 *
 * @param mat Pointer to the dense matrix (`matrix_f`) structure.
 * @return The determinant as a double. If an error occurs (e.g., invalid input
 *         or singular matrix), the function returns 0.0f and sets `errno`:
 *         - EINVAL if the input is NULL or not a square dense matrix
 *         - ERANGE if the matrix is singular
 */
double double_dense_matrix_det(const matrix_d* mat);
// -------------------------------------------------------------------------------- 

double dot_double(const double* a, const double* b, size_t len);
// -------------------------------------------------------------------------------- 

double dot_double_vector(const double_v* vec1, const double_v* vec2);
// -------------------------------------------------------------------------------- 

bool cross_double(const double* a, const double* b, double* result);
// -------------------------------------------------------------------------------- 

double_v* cross_double_vector(const double_v* vec1, const double_v* vec2);
// -------------------------------------------------------------------------------- 

double double_lin_interp(double x1, double y1, double x2, double y2, double x);
// ================================================================================ 
// ================================================================================ 
// DICTIONARY PROTOTYPES 

/**
 * @typedef dict_d
 * @brief Opaque struct representing a dictionary.
 *
 * This structure encapsulates a hash table that maps string keys to double values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_d dict_d;
// --------------------------------------------------------------------------------

/**
 * @brief Initializes a new dictionary.
 *
 * Allocates and initializes a dictionary object with a default size for the hash table.
 *
 * @return A pointer to the newly created dictionary, or NULL if allocation fails.
 */
dict_d* init_double_dict();
// --------------------------------------------------------------------------------

/**
 * @brief Inserts a key-value pair into the dictionary.
 *
 * Adds a new key-value pair to the dictionary. If the key already exists, the function
 * does nothing and returns false. If the dictionary's load factor exceeds a threshold,
 * it automatically resizes.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to insert.
 * @param value The value associated with the key.
 * @return true if the key-value pair was inserted successfully, false otherwise.
 */
bool insert_double_dict(dict_d* dict, const char* key, double value);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a key-value pair from the dictionary.
 *
 * Finds the specified key in the dictionary, removes the associated key-value pair,
 * and returns the value.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to remove.
 * @return The value associated with the key if it was found and removed; FLT_MAX otherwise.
 */
double pop_double_dict(dict_d* dict,  const char* key);
// --------------------------------------------------------------------------------

/**
 * @brief Retrieves the value associated with a key.
 *
 * Searches the dictionary for the specified key and returns the corresponding value.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to search for.
 * @return The value associated with the key, or FLT_MAX if the key is not found.
 */
double get_double_dict_value(const dict_d* dict, const char* key);
// --------------------------------------------------------------------------------

/**
 * @brief Frees the memory associated with the dictionary.
 *
 * Releases all memory allocated for the dictionary, including all key-value pairs.
 *
 * @param dict Pointer to the dictionary to free.
 */
void free_double_dict(dict_d* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Safely frees a dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_double_dict(dict_d** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICT_GBC
     * @brief A macro for enabling automatic cleanup of dict_d objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define DDICT_GBC __attribute__((cleanup(_free_double_dict)))
#endif
// --------------------------------------------------------------------------------

/**
 * @brief Updates the value associated with a key in the dictionary.
 *
 * Searches for the specified key in the dictionary and updates its value.
 * If the key does not exist, the function takes no action.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to update.
 * @param value The new value to associate with the key.
 */
bool update_double_dict(dict_d* dict, const char* key, double value);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the number of non-empty buckets in the dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t double_dict_size(const dict_d* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t double_dict_alloc(const dict_d* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t double_dict_hash_size(const dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Checks if a key exists in the dictionary without retrieving its value
 * 
 * @param dict Pointer to the dictionary
 * @param key Key to check for
 * @return bool true if key exists, false otherwise
 */
bool has_key_double_dict(const dict_d* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a deep copy of a dictionary
 * 
 * @param dict Pointer to the dictionary to copy
 * @return dict_d* New dictionary containing copies of all entries, NULL on error
 */
dict_d* copy_double_dict(const dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes all entries from a dictionary without freeing the dictionary itself
 * 
 * @param dict Pointer to the dictionary to clear
 * @return bool true if successful, false otherwise
 */
bool clear_double_dict(dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all keys in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return char** Array of strings containing copies of all keys, NULL on error
 */
string_v* get_keys_double_dict(const dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all values in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return double* Array containing all values, NULL on error
 */
double_v* get_values_double_dict(const dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Merges two dictionaries into a new dictionary
 * 
 * @param dict1 First dictionary
 * @param dict2 Second dictionary
 * @param overwrite If true, values from dict2 override dict1 on key conflicts
 * @return dict_d* New dictionary containing merged entries, NULL on error
 */
dict_d* merge_double_dict(const dict_d* dict1, const dict_d* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for dictionary traversal
 */
typedef void (*ddict_iterator)(const char* key, double value, void* user_data);

/**
 * @brief Iterates over all dictionary entries in insertion order
 * 
 * @param dict Pointer to the dictionary
 * @param iter Iterator function to call for each entry
 * @param user_data Optional user data passed to iterator function
 */
bool foreach_double_dict(const dict_d* dict, ddict_iterator iter, void* user_data);
// ================================================================================ 
// ================================================================================ 
// VECTOR DICTIONARY PROTOTYPES 

/**
 * @typedef dict_fv
 * @brief Opaque struct representing a dictionary for vector data types.
 *
 * This structure encapsulates a hash table that maps string keys to a vector of double values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_dv dict_dv;
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a dictionary of double vectors
 *
 * @returns a dictionary of double vectors
 */
dict_dv* init_doublev_dict(void);
// -------------------------------------------------------------------------------- 

/**
* @function create_doublev_dict
* @brief Creates a key vector/array pair 
*
* @param dict A fict_fv data type
* @param key a string literal key
* @param size The size of the array or vector 
* @return true if the function executes succesfully, false otherwise
*/
bool create_doublev_dict(dict_dv* dict, char* key, size_t size);
// -------------------------------------------------------------------------------- 

/**
* @function pop_doublev_dict 
* @brief Removes a statically or dynamically allocated array from the dictionary
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return true if the function executes succesfully, false otherwise
*/
bool pop_doublev_dict(dict_dv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function pop_doublev_dict 
* @brief Returns a double_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return a double_v pointer so it can be used in vector and array functions
*/
double_v* return_doublev_pointer(dict_dv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function free_doublev_dict 
* @brief Returns a double_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
*/
void free_doublev_dict(dict_dv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Safely frees a vector dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_doublev_dict(dict_dv** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICTV_GBC
     * @brief A macro for enabling automatic cleanup of dict_dv objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define DDICTV_GBC __attribute__((cleanup(_free_doublev_dict)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief determines if a key value pair exists in a vector dictionary
 *
 * @param dict The double vector dictionary 
 * @param key The key value being searched for 
 * @return true if the key value pair exists, false otherwise.
 */
bool has_key_doublev_dict(const dict_dv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Inserts an already existing dynamically allocated array to dictionary
 *
 * @param dict The double vector dictionary 
 * @param key The key value being searched for 
 * @param vec A dynamically allocated array of type double_v
 * @return true if the key value pair exists, false otherwise.
 */
bool insert_doublev_dict(dict_dv* dict, const char* key, double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets the number of non-empty buckets in the vector dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t double_dictv_size(const dict_dv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the vector dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t double_dictv_alloc(const dict_dv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the vector dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t double_dictv_hash_size(const dict_dv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Creates a deep copy of a vector double dictionary 
 *
 * @param original A double vector dictionary 
 * @return A copy of a dictionary
 */
dict_dv* copy_doublev_dict(const dict_dv* original);
// -------------------------------------------------------------------------------- 

/**
 * @brief merges to double vector dictionaries
 *
 * Returns a merged double vector dictionary
 *
 * @param dict1 Pointer to a dictionary.
 * @param dict2 Pointer to a dictionary
 * @param overwrite true if the values should be overwritten, false otherwise
 * @return A merged dictionary
 */
dict_dv* merge_doublev_dict(const dict_dv* dict1, const dict_dv* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Clears all keys and values in the vector dictionary
 *
 * @param dict Pointer to a dictionary.
 */
void clear_doublev_dict(dict_dv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for double vector dictionary traversal
 *
 * @param key        Key string of the current entry
 * @param value      Pointer to the associated double_v vector
 * @param user_data  Optional context pointer passed through
 */
typedef void (*dict_dv_iterator)(const char* key, const double_v* value, void* user_data);
// -------------------------------------------------------------------------------- 

/**
 * @brief Applies a callback function to every key-value pair in a double_v dictionary
 *
 * Iterates over all entries in the hash table and invokes the callback for each.
 *
 * @param dict       Pointer to the dictionary
 * @param iter       Function pointer to apply to each key/value pair
 * @param user_data  Optional context pointer passed through to the callback
 * @return true on success, false if dict or iter is NULL (errno set to EINVAL)
 */
bool foreach_doublev_dict(const dict_dv* dict, dict_dv_iterator iter, void* user_data);
// -------------------------------------------------------------------------------- 

string_v* get_keys_doublev_dict(const dict_dv* dict);
// ================================================================================ 
// ================================================================================ 


// ================================================================================ 
// ================================================================================ 
// GENERIC MACROS

/**
 * @macro f_size
 * @brief Generic macro to get the number of elements in a double container
 *
 * This generic macro provides a unified interface for getting the current number
 * of elements in any double container type. It automatically selects the appropriate
 * size function based on the container type.
 * Currently supported container types:
 * - double_v (calls double_vector_size)
 * Additional container types will be added as the library expands.
 *
 * @param d_struct Pointer to double container structure
 * @return Size (number of elements) in the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define d_size(d_struct) _Generic((d_struct), \
    double_v*: double_vector_size, \
    dict_d*: double_dict_size, \
    dict_dv*: double_dictv_size) (d_struct)
// --------------------------------------------------------------------------------

/**
 * @macro f_alloc
 * @brief Generic macro to get the allocation size of a double container
 *
 * This generic macro provides a unified interface for getting the current allocation
 * size in any double container type. It automatically selects the appropriate
 * allocation function based on the container type.
 * Currently supported container types:
 * - double_v (calls double_vector_alloc)
 * Additional container types will be added as the library expands.
 *
 * @param f_struct Pointer to double container structure
 * @return Allocation size (capacity) of the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define d_alloc(d_struct) _Generic((d_struct), \
    double_v*: double_vector_alloc, \
    dict_d*: double_dict_alloc, \
    dict_dv*: double_dictv_alloc) (d_struct)
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_double_H */
// ================================================================================
// ================================================================================
// eof
