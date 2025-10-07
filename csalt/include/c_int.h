// ================================================================================
// - File:    c_int.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 12, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here
#ifndef c_int_H
#define c_int_H
// ================================================================================ 
// ================================================================================ 
 
#include <stdlib.h>
#include <stdbool.h>
#include "c_string.h"
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
    int* data;
    size_t len;
    size_t alloc;
    ErrorCode error;
    alloc_t alloc_type;
} int_v;
// --------------------------------------------------------------------------------

/**
 * @brief Create a dynamically allocated vector of @c int with given capacity.
 *
 * Allocates a @ref int_v control block and a zero-initialized data buffer
 * of @p buff elements on the heap. On success, the vector is configured for
 * dynamic ownership ( @ref alloc_t :: DYNAMIC ), its size is set to 0, and
 * its last-error field is cleared to @c NO_ERROR.
 *
 * @param buff
 *     Requested capacity in elements (must be > 0).
 *
 * @return
 *     Pointer to a newly created @ref int_v on success; @c NULL on failure.
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
 * library’s designated destructor (e.g., @c free_int_vector(result) ), which
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
 * int_v* v = init_int_vector(128);
 * if (!v) {
 *     perror("init_int_vector");
 *     return 1;
 * }
 * // ...
 * free_int_vector(v); // library-provided destructor
 * @endcode
 */
int_v* init_int_vector(size_t buffer);
// -------------------------------------------------------------------------------- 

#if defined(DOXYGEN)

/**
 * \def init_int_array(size)
 * \brief Create a ::int_v that wraps a zero-initialized, fixed-capacity array.
 *
 * Expands to a value of type ::int_v with:
 * - data       → points to `(int[size]){0}` (automatic storage, zero-initialized)
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
 *     int_v v = init_int_array(16);
 *
 *     // Fill first three elements and record logical length
 *     push_back_int_vector(&v, 1);
 *     push_back_int_vector(&v, 2);
 *     push_back_int_vector(&v, 3);
 *     size_t len = double_vector_size(&v);
 *
 *     // Use only APIs that respect STATIC semantics (no reallocation/free).
 * } // v.data becomes invalid here
 * \endcode
 */
#define init_int_array(size) /* docs-only definition */

/**
 * \brief Wrap caller-provided storage as a ::int_v with ::STATIC semantics.
 *
 * Use this function as the **supported replacement** for static allocation when
 * the `init_int_array` macro is disabled (e.g., strict/MISRA builds) or when
 * you prefer not to rely on compound literals. The storage lifetime/ownership
 * remains with the caller.
 *
 * \param buf Pointer to at least \p n int.
 * \param n   Capacity of \p buf (must be > 0).
 * \return    A ::int_v referring to \p buf with len=0, alloc=n, error=NO_ERROR,
 *            alloc_type=STATIC.
 *
 * \warning The function does **not** allocate or free memory. The caller must
 * manage \p buf and ensure it outlives the ::int_v usage.
 *
 * \par Example A (file-scope static buffer)
 * \code
 * static int file_buf[32];
 *
 * void demo_wrap_filescope(void) {
 *     int_v v = wrap_int_array(file_buf, 32);
 *     push_back_int_vector(&v, 42);
 *     size_t size = int_vector_size(&v); // 1
 *     size_t alloc = int_vector_alloc(&v); // 32
 *     // file_buf persists for program lifetime (or translation unit scope).
 * }
 * \endcode
 *
 * \par Example B (local fixed array without compound literals)
 * \code
 * void demo_wrap_local(void) {
 *     int local_buf[8] = {0};          // automatic storage
 *     int_v v = wrap_int_array(local_buf, 8);
 *     // v.data valid until end of this block (same as local_buf)
 * }
 * \endcode
 */
double_v wrap_int_array(float *buf, size_t n);

#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && !defined(__STDC_NO_GENERIC__)

/* Real macro (C99 compound literal). Attach docs via copydoc so they render even if
 * Doxygen parses this branch rather than the DOXYGEN stub above. */
/** \copydoc init_double_array */
#define init_int_array(size)                                                    \
    ((int_v){ .data = (int[(size)]){0}, .len = 0, .alloc = (size),            \
                .error = NO_ERROR, .alloc_type = STATIC })

/* Provide the wrapper in all builds (convenient and portable). */
/** \copydoc wrap_int_array */
static inline int_v wrap_int_array(int *buf, size_t n)
{
    int_v v;
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
static inline int_v wrap_int_array(int *buf, size_t n)
{
    int_v v;
    v.data = buf;
    v.len = 0;
    v.alloc = n;
    v.error = NO_ERROR;
    v.alloc_type = STATIC;
    return v;
}

/* Breadcrumb for users trying to use the macro in a disabled build. */
#if !defined(init_int_array)
#define init_int_array(size) /* unavailable: use wrap_float_array(buf, n) */
#endif

#endif /* feature gate */

// -------------------------------------------------------------------------------- 

/**
 * @brief Get the last nonzero error code recorded on a ::int_v.
 *
 * Returns the value of @p vec->error when it is a nonzero ::ErrorCode set by a
 * prior csalt operation. This accessor is intended for use *after* an API call
 * has failed and stored a diagnostic on the vector.
 *
 * If @p vec is NULL **or** if @p vec->error equals ::NO_ERROR (i.e., no error
 * has been recorded), the function sets @c errno to @c EINVAL and returns
 * ::INVALID_ERROR.
 *
 * @param vec  Pointer to a ::int_v to query (may be NULL).
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
 * if (!push_back_int_vector(v, 3.14f)) {
 *     ErrorCode ec = get_int_vector_error(v);
 *     if (ec == INVALID_ERROR) {
 *         // Either v is NULL or no error was recorded
 *         perror("get_int_vector_error");
 *     } else {
 *         // Handle the specific error code 'ec'
 *     }
 * }
 *
 * // Example: calling when no error was recorded -> INVALID_ERROR + errno=EINVAL
 * ErrorCode ec2 = get_int_vector_error(v);
 * // ec2 == INVALID_ERROR if v->error was NO_ERROR
 * @endcode
 */
static inline ErrorCode get_int_vector_error(const int_v* vec) {
    if (!vec || !vec->error) {
        errno = EINVAL;
        return INVALID_ERROR;
    }
    return vec->error;
}
// --------------------------------------------------------------------------------

/**
 * @brief Append a value to a ::int_v, growing capacity if needed.
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
 * @param vec    Pointer to a ::int_v (must be non-NULL and have @c data != NULL).
 * @param value  The @c int value to append.
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
 * int_v* v = init_int_vector(4);
 * if (!v) { perror("init_int_vector"); return; }
 * for (int i = 0; i < 10; ++i) {
 *     if (!push_back_int_vector(v, (int)i)) {
 *         perror("push_back_int_vector");
 *         // v->error contains a csalt-specific code (e.g., REALLOC_FAIL)
 *         free_int_vector(v);
 *         return;
 *     }
 * }
 * // v->len == 10; capacity likely grew according to the policy
 * free_int_vector(v);
 *
 * // Example 2: static vector fails when full
 * {
 *     int_v s = init_int_array(2);  // STATIC storage
 *     (void)push_back_int_vector(&s, 1.0f);  // ok
 *     (void)push_back_int_vector(&s, 2.0f);  // ok
 *     if (!push_back_int_vector(&s, 3.0f)) { // exceeds capacity
 *         // s.error == INVALID_ARG; errno set via set_errno_from_error()
 *     }
 * } // s.data lifetime ends with the block
 * @endcode
 */
bool push_back_int_vector(int_v* vec, int value);
// --------------------------------------------------------------------------------

bool push_front_int_vector(int_v* vec, const int value);
// --------------------------------------------------------------------------------

bool insert_int_vector(int_v* vec, const int value, size_t index);
// --------------------------------------------------------------------------------

int int_vector_index(const int_v* vec, size_t index);
// -------------------------------------------------------------------------------- 

size_t int_vector_size(const int_v* vec);
// -------------------------------------------------------------------------------- 

size_t int_vector_alloc(const int_v* vec);
// --------------------------------------------------------------------------------

int pop_back_int_vector(int_v* vec);
// -------------------------------------------------------------------------------- 

int pop_front_int_vector(int_v* vec);
// --------------------------------------------------------------------------------

int pop_any_int_vector(int_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Destroy a dynamically allocated ::int_v and its heap buffer.
 *
 * Frees the data buffer (if present) and then frees the vector control block
 * itself. This function is only valid for vectors created with dynamic
 * ownership (e.g., @ref init_float_vector) where @c alloc_type == ::DYNAMIC.
 *
 * @param vec  Pointer to a ::int_v previously created with dynamic allocation.
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
 *   such as those from @ref init_float_array or @ref wrap_float_array; the
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
 * int_v* v = init_int_vector(64);
 * if (!v) { perror("init_int_vector"); return; }
 * // ... use v ...
 * free_int_vector(v);
 * v = NULL; // prevent accidental reuse
 *
 * // Incorrect: static vector (will set errno=EINVAL and return)
 * {
 *     int_v s = init_int_array(16);   // STATIC storage, automatic duration
 *     // free_int_vector(&s);           // <-- don't do this
 * } // s.data becomes invalid here automatically
 *
 * // Correct: static wrapper when macros are disabled or avoided
 * {
 *     int buf[8] = {0};
 *     int_v w = wrap_int_array(buf, 8); // STATIC, caller-owned buffer
 *     // ... use w ...
 *     // Do NOT call free_int_vector(&w); just let 'w' go out of scope.
 * }
 * @endcode
 */
void free_int_vector(int_v* vec);
// --------------------------------------------------------------------------------

void _free_int_vector(int_v** vec);
// --------------------------------------------------------------------------------

#if defined(DOXYGEN)

/**
 * \def INTVEC_GBC
 * \brief GCC/Clang cleanup attribute that auto-frees a ::int_v* on scope exit.
 *
 * When applied to a variable of type `int_v*`, the pointer will be passed to an internal
 * cleanup handler at scope exit, which calls ::free_int_vector on it and then nulls it.
 *
 * **Availability:** Defined only for GCC/Clang builds when the project does not define
 * `__STDC_NO_GENERIC__` (used here as a global “strict/MISRA” switch). In other builds the
 * macro expands to nothing.
 *
 * \par Correct usage
 * \code{.c}
 * void demo(void) {
 *     int_v* v INTVEC_GBC = init_int_vector(256);
 *     if (!v) { perror("init_int_vector"); return; }
 *     // ... use v ...
 * } // v is automatically freed here
 * \endcode
 *
 * \par Returning ownership (cancel auto-free)
 * \code{.c}
 * int_v* make_vec(void) {
 *     int_v* v INTVEC_GBC = init_int_vector(64);
 *     if (!v) return NULL;
 *     // ... populate v ...
 *     int_v* out = v;   // transfer ownership
 *     v = NULL;           // cancel cleanup (handler sees NULL and does nothing)
 *     return out;
 * }
 * \endcode
 *
 * \warning Apply to **`int_v*` variables only** (not to `int_v` by value).
 * Do not use for ::STATIC wrappers (e.g., from init_int_array / wrap_int_array).
 * The handler calls ::free_int_vector, which only frees ::DYNAMIC vectors.
 *
 * \note This is a compiler extension (not ISO C). MSVC and some toolchains do not support it.
 * On such builds `INTVEC_GBC` is a no-op.
 */
#define INTVEC_GBC

/**
 * \brief Cleanup handler used by ::INTVEC_GBC (documentation stub).
 * \param pp  Address of a `int_v*` variable. If non-NULL, frees `*pp` and sets it to NULL.
 * \internal Users should not call this directly.
 */
void _free_int_vector(int_v **pp);

#elif (defined(__GNUC__) || defined(__clang__)) && !defined(__STDC_NO_GENERIC__)

/* Apply as a suffix: int_v* v INTVEC_GBC = init_int_vector(...); */
#define INTVEC_GBC __attribute__((cleanup(_free_int_vector)))

#else  /* Portable/strict builds: make it a no-op so code still compiles cleanly. */

/* No cleanup attribute available (or globally disabled by __STDC_NO_GENERIC__). */
#define INTVEC_GBC /* no-op: call free_int_vector() manually */

#endif /* feature gate */
// -------------------------------------------------------------------------------- 

void reverse_int_vector(int_v* vec);
// --------------------------------------------------------------------------------

void swap_int(int* a, int* b);
// --------------------------------------------------------------------------------

void sort_int_vector(int_v* vec, iter_dir direction);
// -------------------------------------------------------------------------------- 

void trim_int_vector(int_v* vec);
// -------------------------------------------------------------------------------- 


size_t binary_search_int_vector(int_v* vec, int value, bool sort_first);
// -------------------------------------------------------------------------------- 

#ifndef BIN_DAT_TYPEDEF
#define BIN_DAT_TYPEDEF
typedef struct {
    size_t lower;
    size_t upper;
} bin_dat;
#endif
// -------------------------------------------------------------------------------- 

bin_dat binary_search_bounds_int_vector(int_v *vec,
                                        int value,
                                        bool sort_first);
// -------------------------------------------------------------------------------- 

void update_int_vector(int_v* vec, size_t index, int replacement_value);
// -------------------------------------------------------------------------------- 

int min_int_vector(int_v* vec);
// -------------------------------------------------------------------------------- 

int max_int_vector(int_v* vec);
// -------------------------------------------------------------------------------- 

int sum_int_vector(int_v* vec);
// -------------------------------------------------------------------------------- 

float average_int_vector(int_v* vec);
// -------------------------------------------------------------------------------- 

float stdev_int_vector(int_v* vec);
// -------------------------------------------------------------------------------- 

int_v* cum_sum_int_vector(int_v* vec);
// -------------------------------------------------------------------------------- 

int_v* copy_int_vector(const int_v* original);
// -------------------------------------------------------------------------------- 

int int_lin_interp(int x1, int y1,
                   int x2, int y2,
                   int x3);
// ================================================================================ 
// ================================================================================ 
// DICTIONARY PROTOTYPES 

/**
 * @typedef dict_f
 * @brief Opaque struct representing a dictionary.
 *
 * This structure encapsulates a hash table that maps string keys to int values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_i dict_i;
// --------------------------------------------------------------------------------

/**
 * @brief Initializes a new dictionary.
 *
 * Allocates and initializes a dictionary object with a default size for the hash table.
 *
 * @return A pointer to the newly created dictionary, or NULL if allocation fails.
 */
dict_i* init_int_dict();
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
bool insert_int_dict(dict_i* dict, const char* key, int value);
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
int pop_int_dict(dict_i* dict,  const char* key);
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
int get_int_dict_value(const dict_i* dict, const char* key);
// --------------------------------------------------------------------------------

/**
 * @brief Frees the memory associated with the dictionary.
 *
 * Releases all memory allocated for the dictionary, including all key-value pairs.
 *
 * @param dict Pointer to the dictionary to free.
 */
void free_int_dict(dict_i* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Safely frees a dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_int_dict(dict_i** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICT_GBC
     * @brief A macro for enabling automatic cleanup of dict_t objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define IDICT_GBC __attribute__((cleanup(_free_int_dict)))
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
bool update_int_dict(dict_i* dict, const char* key, int value);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the number of non-empty buckets in the dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t int_dict_size(const dict_i* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t int_dict_alloc(const dict_i* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t int_dict_hash_size(const dict_i* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Checks if a key exists in the dictionary without retrieving its value
 * 
 * @param dict Pointer to the dictionary
 * @param key Key to check for
 * @return bool true if key exists, false otherwise
 */
bool has_key_int_dict(const dict_i* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a deep copy of a dictionary
 * 
 * @param dict Pointer to the dictionary to copy
 * @return dict_i* New dictionary containing copies of all entries, NULL on error
 */
dict_i* copy_int_dict(const dict_i* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes all entries from a dictionary without freeing the dictionary itself
 * 
 * @param dict Pointer to the dictionary to clear
 * @return bool true if successful, false otherwise
 */
bool clear_int_dict(dict_i* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all keys in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return char** Array of strings containing copies of all keys, NULL on error
 */
string_v* get_keys_int_dict(const dict_i* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all values in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return int* Array containing all values, NULL on error
 */
int_v* get_values_int_dict(const dict_i* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Merges two dictionaries into a new dictionary
 * 
 * @param dict1 First dictionary
 * @param dict2 Second dictionary
 * @param overwrite If true, values from dict2 override dict1 on key conflicts
 * @return dict_i* New dictionary containing merged entries, NULL on error
 */
dict_i* merge_int_dict(const dict_i* dict1, const dict_i* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for dictionary traversal
 */
typedef void (*idict_iterator)(const char* key, int value, void* user_data);

/**
 * @brief Iterates over all dictionary entries in insertion order
 * 
 * @param dict Pointer to the dictionary
 * @param iter Iterator function to call for each entry
 * @param user_data Optional user data passed to iterator function
 */
bool foreach_int_dict(const dict_i* dict, idict_iterator iter, void* user_data);
// ================================================================================ 
// ================================================================================ 
// VECTOR DICTIONARY PROTOTYPES 

/**
 * @typedef dict_fv
 * @brief Opaque struct representing a dictionary for vector data types.
 *
 * This structure encapsulates a hash table that maps string keys to a vector of float values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_iv dict_iv;
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a dictionary of int vectors
 *
 * @returns a dictionary of int vectors
 */
dict_iv* init_intv_dict(void);
// -------------------------------------------------------------------------------- 

/**
* @function create_intv_dict
* @brief Creates a key vector/array pair 
*
* @param dict A fict_fv data type
* @param key a string literal key
* @param size The size of the array or vector 
* @return true if the function executes succesfully, false otherwise
*/
bool create_intv_dict(dict_iv* dict, char* key, size_t size);
// -------------------------------------------------------------------------------- 

/**
* @function pop_intv_dict 
* @brief Removes a statically or dynamically allocated array from the dictionary
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return true if the function executes succesfully, false otherwise
*/
bool pop_intv_dict(dict_iv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function pop_intv_dict 
* @brief Returns a int_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return a int_v pointer so it can be used in vector and array functions
*/
int_v* return_intv_pointer(dict_iv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function free_intv_dict 
* @brief Returns a int_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
*/
void free_intv_dict(dict_iv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Safely frees a vector dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_intv_dict(dict_iv** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICTV_GBC
     * @brief A macro for enabling automatic cleanup of dict_iv objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define IDICTV_GBC __attribute__((cleanup(_free_intv_dict)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief determines if a key value pair exists in a vector dictionary
 *
 * @param dict The int vector dictionary 
 * @param key The key value being searched for 
 * @return true if the key value pair exists, false otherwise.
 */
bool has_key_intv_dict(const dict_iv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Inserts an already existing dynamically allocated array to dictionary
 *
 * @param dict The int vector dictionary 
 * @param key The key value being searched for 
 * @param vec A dynamically allocated array of type int_v
 * @return true if the key value pair exists, false otherwise.
 */
bool insert_intv_dict(dict_iv* dict, const char* key, int_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets the number of non-empty buckets in the vector dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t int_dictv_size(const dict_iv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the vector dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t int_dictv_alloc(const dict_iv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the vector dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t int_dictv_hash_size(const dict_iv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Creates a deep copy of a vector int dictionary 
 *
 * @param original A int vector dictionary 
 * @return A copy of a dictionary
 */
dict_iv* copy_intv_dict(const dict_iv* original);
// -------------------------------------------------------------------------------- 

/**
 * @brief merges to int vector dictionaries
 *
 * Returns a merged int vector dictionary
 *
 * @param dict1 Pointer to a dictionary.
 * @param dict2 Pointer to a dictionary
 * @param overwrite true if the values should be overwritten, false otherwise
 * @return A merged dictionary
 */
dict_iv* merge_intv_dict(const dict_iv* dict1, const dict_iv* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Clears all keys and values in the vector dictionary
 *
 * @param dict Pointer to a dictionary.
 */
void clear_intv_dict(dict_iv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for int vector dictionary traversal
 *
 * @param key        Key string of the current entry
 * @param value      Pointer to the associated int_v vector
 * @param user_data  Optional context pointer passed through
 */
typedef void (*dict_iv_iterator)(const char* key, const int_v* value, void* user_data);
// -------------------------------------------------------------------------------- 

/**
 * @brief Applies a callback function to every key-value pair in a int_v dictionary
 *
 * Iterates over all entries in the hash table and invokes the callback for each.
 *
 * @param dict       Pointer to the dictionary
 * @param iter       Function pointer to apply to each key/value pair
 * @param user_data  Optional context pointer passed through to the callback
 * @return true on success, false if dict or iter is NULL (errno set to EINVAL)
 */
bool foreach_intv_dict(const dict_iv* dict, dict_iv_iterator iter, void* user_data);
// -------------------------------------------------------------------------------- 

string_v* get_keys_intv_dict(const dict_iv* dict);
// ================================================================================ 
// ================================================================================ 
// GENERIC MACROS

/**
 * @macro f_size
 * @brief Generic macro to get the number of elements in a int container
 *
 * This generic macro provides a unified interface for getting the current number
 * of elements in any int container type. It automatically selects the appropriate
 * size function based on the container type.
 * Currently supported container types:
 * - int_v (calls int_vector_size)
 * Additional container types will be added as the library expands.
 *
 * @param f_struct Pointer to int container structure
 * @return Size (number of elements) in the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define i_size(f_struct) _Generic((f_struct), \
    int_v*: int_vector_size, \
    dict_i*: int_dict_size, \
    dict_iv*: int_dictv_size) (f_struct)
// --------------------------------------------------------------------------------

/**
 * @macro f_alloc
 * @brief Generic macro to get the allocation size of a int container
 *
 * This generic macro provides a unified interface for getting the current allocation
 * size in any int container type. It automatically selects the appropriate
 * allocation function based on the container type.
 * Currently supported container types:
 * - int_v (calls int_vector_alloc)
 * Additional container types will be added as the library expands.
 *
 * @param f_struct Pointer to int container structure
 * @return Allocation size (capacity) of the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define i_alloc(f_struct) _Generic((f_struct), \
    int_v*: int_vector_alloc, \
    dict_i*: int_dict_alloc, \
    dict_iv*: int_dictv_alloc) (f_struct)
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_int_H */
// ================================================================================
// ================================================================================
// eof
