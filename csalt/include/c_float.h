// ================================================================================
// ================================================================================
// - File:    c_float.h
// - Purpose: This file contains prototypes for the c_float functions in the 
//            c_float.c file.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    January 12, 2025
// - Version: 0.1
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_float_H
#define c_float_H
// ================================================================================
// ================================================================================ 

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "c_string.h"
#include "c_error.h"
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================
// ================================================================================

/**
 * @enum iter_dir
 * @brief Direction selector for container iteration.
 *
 * Use this enum to configure iterators or traversal helpers to walk a
 * sequence either from the first element to the last or in reverse.
 *
 * Typical usage:
 * - `FORWARD`  — visit elements in ascending index order: 0, 1, ..., len-1
 * - `REVERSE`  — visit elements in descending index order: len-1, ..., 0
 */
#ifndef ITER_DIR_H
#define ITER_DIR_H
    typedef enum {
        FORWARD = 0,
        REVERSE = 1
    }iter_dir;
#endif /* ITER_DIR_H*/
// --------------------------------------------------------------------------------    

/**
 * @enum alloc_t
 * @brief Allocation/ownership mode for a vector's storage.
 *
 * This enum communicates whether a vector's underlying memory is owned
 * and managed by the csalt container, or whether it wraps caller-provided
 * storage with fixed capacity.
 *
 * Semantics:
 * - `DYNAMIC` — memory is owned by the vector and may be resized/reallocated
 *               by csalt functions. The vector is responsible for freeing it.
 * - `STATIC`  — memory is supplied by the caller (non-owned). Capacity is
 *               fixed to the provided buffer size; resize operations must fail
 *               gracefully (e.g., set an error code) instead of reallocating.
 */
#ifndef ALLOC_H 
#define ALLOC_H 

    typedef enum {
        STATIC = 0,
        DYNAMIC = 1
    } alloc_t;

#endif /*ALLOC_H*/

/**
 * @struct float_v
 * @brief Dynamically sized vector of `float` values with error/status metadata.
 *
 * A `float_v` holds a contiguous array of `float` elements together with
 * size/capacity information, a last-error code, and an allocation mode
 * (`alloc_type`) indicating ownership semantics.
 *
 * ### Invariants
 * - `alloc >= len`
 * - `data == NULL` implies `len == 0` and typically `alloc == 0`
 * - When `alloc_type == STATIC`, the buffer is non-owned and must **not** be
 *   reallocated or freed by the vector; capacity is fixed at `alloc`.
 * - When `alloc_type == DYNAMIC`, the buffer is owned by the vector and may be
 *   grown/shrunk by csalt functions; the vector is responsible for freeing it.
 *
 * ### Error handling
 * - `error` contains the last error reported by a csalt operation on this
 *   instance (see `ErrorCode` enum). Successful operations should set it to
 *   the library’s success code (commonly `ERR_OK`).
 *
 * ### Thread-safety
 * - `float_v` is **not** thread-safe. Protect instances with external
 *   synchronization if accessed from multiple threads.
 *
 * @note The `ErrorCode` enum is defined elsewhere in csalt and is shared across
 *       components for consistent error reporting.
 */
typedef struct {
    float* data;
    size_t len;
    size_t alloc;
    ErrorCode error;
    alloc_t alloc_type;
} float_v;
// --------------------------------------------------------------------------------

/**
 * @brief Create a dynamically allocated vector of @c float with given capacity.
 *
 * Allocates a @ref float_v control block and a zero-initialized data buffer
 * of @p buff elements on the heap. On success, the vector is configured for
 * dynamic ownership ( @ref alloc_t :: DYNAMIC ), its size is set to 0, and
 * its last-error field is cleared to @c NO_ERROR.
 *
 * @param buff
 *     Requested capacity in elements (must be > 0).
 *
 * @return
 *     Pointer to a newly created @ref float_v on success; @c NULL on failure.
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
 * library’s designated destructor (e.g., @c free_float_vector(result) ), which
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
 * float_v* v = init_float_vector(128);
 * if (!v) {
 *     perror("init_float_vector");
 *     return 1;
 * }
 * // ...
 * free_float_vector(v); // library-provided destructor
 * @endcode
 */
float_v* init_float_vector(size_t buff);
// -------------------------------------------------------------------------------- 

#if defined(DOXYGEN)

/**
 * \def init_float_array(size)
 * \brief Create a ::float_v that wraps a zero-initialized, fixed-capacity array.
 *
 * Expands to a value of type ::float_v with:
 * - data       → points to `(float[size]){0}` (automatic storage, zero-initialized)
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
 *     float_v v = init_float_array(16);
 *
 *     // Fill first three elements and record logical length
 *     push_back_float_vector(&v, 1.0f);
 *     push_back_float_vector(&v, 2.0f);
 *     push_back_float_vector(&v, 3.0f);
 *     size_t len = float_vector_size(&v);
 *
 *     // Use only APIs that respect STATIC semantics (no reallocation/free).
 * } // v.data becomes invalid here
 * \endcode
 */
#define init_float_array(size) /* docs-only definition */

/**
 * \brief Wrap caller-provided storage as a ::float_v with ::STATIC semantics.
 *
 * Use this function as the **supported replacement** for static allocation when
 * the `init_float_array` macro is disabled (e.g., strict/MISRA builds) or when
 * you prefer not to rely on compound literals. The storage lifetime/ownership
 * remains with the caller.
 *
 * \param buf Pointer to at least \p n floats.
 * \param n   Capacity of \p buf (must be > 0).
 * \return    A ::float_v referring to \p buf with len=0, alloc=n, error=NO_ERROR,
 *            alloc_type=STATIC.
 *
 * \warning The function does **not** allocate or free memory. The caller must
 * manage \p buf and ensure it outlives the ::float_v usage.
 *
 * \par Example A (file-scope static buffer)
 * \code
 * static float file_buf[32];
 *
 * void demo_wrap_filescope(void) {
 *     float_v v = wrap_float_array(file_buf, 32);
 *     push_back_float_vector(&v, 42.0f);
 *     size_t size = float_vector_size(&v); // 1
 *     size_t alloc = float_vector_alloc(&v); // 32
 *     // file_buf persists for program lifetime (or translation unit scope).
 * }
 * \endcode
 *
 * \par Example B (local fixed array without compound literals)
 * \code
 * void demo_wrap_local(void) {
 *     float local_buf[8] = {0};          // automatic storage
 *     float_v v = wrap_float_array(local_buf, 8);
 *     // v.data valid until end of this block (same as local_buf)
 * }
 * \endcode
 */
float_v wrap_float_array(float *buf, size_t n);

#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && !defined(__STDC_NO_GENERIC__)

/* Real macro (C99 compound literal). Attach docs via copydoc so they render even if
 * Doxygen parses this branch rather than the DOXYGEN stub above. */
/** \copydoc init_float_array */
#define init_float_array(size)                                                    \
    ((float_v){ .data = (float[(size)]){0}, .len = 0, .alloc = (size),            \
                .error = NO_ERROR, .alloc_type = STATIC })

/* Provide the wrapper in all builds (convenient and portable). */
/** \copydoc wrap_float_array */
static inline float_v wrap_float_array(float *buf, size_t n)
{
    float_v v;
    v.data = buf;
    v.len = 0;
    v.alloc = n;
    v.error = NO_ERROR;
    v.alloc_type = STATIC;
    return v;
}

#else /* Strict/MISRA or non-C99 compilers (macro disabled) */

/* Macro disabled; only the wrapper is available. */
/** \copydoc wrap_float_array */
static inline float_v wrap_float_array(float *buf, size_t n)
{
    float_v v;
    v.data = buf;
    v.len = 0;
    v.alloc = n;
    v.error = NO_ERROR;
    v.alloc_type = STATIC;
    return v;
}

/* Breadcrumb for users trying to use the macro in a disabled build. */
#if !defined(init_float_array)
#define init_float_array(size) /* unavailable: use wrap_float_array(buf, n) */
#endif

#endif /* feature gate */
// -------------------------------------------------------------------------------- 

float* c_float_ptr(float_v* vec);
// --------------------------------------------------------------------------------

bool push_back_float_vector(float_v* vec, const float value);
// --------------------------------------------------------------------------------

bool push_front_float_vector(float_v* vec, const float value);
// --------------------------------------------------------------------------------

bool insert_float_vector(float_v* vec, const float value, size_t index);
// --------------------------------------------------------------------------------

float float_vector_index(const float_v* vec, size_t index);
// -------------------------------------------------------------------------------- 

size_t float_vector_size(const float_v* vec);
// -------------------------------------------------------------------------------- 

size_t float_vector_alloc(const float_v* vec);
// --------------------------------------------------------------------------------

float pop_back_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

float pop_front_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

float pop_any_float_vector(float_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Destroy a dynamically allocated ::float_v and its heap buffer.
 *
 * Frees the data buffer (if present) and then frees the vector control block
 * itself. This function is only valid for vectors created with dynamic
 * ownership (e.g., @ref init_float_vector) where @c alloc_type == ::DYNAMIC.
 *
 * @param vec  Pointer to a ::float_v previously created with dynamic allocation.
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
 * float_v* v = init_float_vector(64);
 * if (!v) { perror("init_float_vector"); return; }
 * // ... use v ...
 * free_float_vector(v);
 * v = NULL; // prevent accidental reuse
 *
 * // Incorrect: static vector (will set errno=EINVAL and return)
 * {
 *     float_v s = init_float_array(16);   // STATIC storage, automatic duration
 *     // free_float_vector(&s);           // <-- don't do this
 * } // s.data becomes invalid here automatically
 *
 * // Correct: static wrapper when macros are disabled or avoided
 * {
 *     float buf[8] = {0};
 *     float_v w = wrap_float_array(buf, 8); // STATIC, caller-owned buffer
 *     // ... use w ...
 *     // Do NOT call free_float_vector(&w); just let 'w' go out of scope.
 * }
 * @endcode
 */
void free_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

void _free_float_vector(float_v** vec);
// --------------------------------------------------------------------------------
#if defined(DOXYGEN)

/**
 * \def FLTVEC_GBC
 * \brief GCC/Clang cleanup attribute that auto-frees a ::float_v* on scope exit.
 *
 * When applied to a variable of type `float_v*`, the pointer will be passed to an internal
 * cleanup handler at scope exit, which calls ::free_float_vector on it and then nulls it.
 *
 * **Availability:** Defined only for GCC/Clang builds when the project does not define
 * `__STDC_NO_GENERIC__` (used here as a global “strict/MISRA” switch). In other builds the
 * macro expands to nothing.
 *
 * \par Correct usage
 * \code{.c}
 * void demo(void) {
 *     float_v* v FLTVEC_GBC = init_float_vector(256);
 *     if (!v) { perror("init_float_vector"); return; }
 *     // ... use v ...
 * } // v is automatically freed here
 * \endcode
 *
 * \par Returning ownership (cancel auto-free)
 * \code{.c}
 * float_v* make_vec(void) {
 *     float_v* v FLTVEC_GBC = init_float_vector(64);
 *     if (!v) return NULL;
 *     // ... populate v ...
 *     float_v* out = v;   // transfer ownership
 *     v = NULL;           // cancel cleanup (handler sees NULL and does nothing)
 *     return out;
 * }
 * \endcode
 *
 * \warning Apply to **`float_v*` variables only** (not to `float_v` by value).
 * Do not use for ::STATIC wrappers (e.g., from init_float_array / wrap_float_array).
 * The handler calls ::free_float_vector, which only frees ::DYNAMIC vectors.
 *
 * \note This is a compiler extension (not ISO C). MSVC and some toolchains do not support it.
 * On such builds `FLTVEC_GBC` is a no-op.
 */
#define FLTVEC_GBC

/**
 * \brief Cleanup handler used by ::FLTVEC_GBC (documentation stub).
 * \param pp  Address of a `float_v*` variable. If non-NULL, frees `*pp` and sets it to NULL.
 * \internal Users should not call this directly.
 */
void _free_float_vector(float_v **pp);

#elif (defined(__GNUC__) || defined(__clang__)) && !defined(__STDC_NO_GENERIC__)

/* Apply as a suffix: float_v* v FLTVEC_GBC = init_float_vector(...); */
#define FLTVEC_GBC __attribute__((cleanup(_free_float_vector)))

#else  /* Portable/strict builds: make it a no-op so code still compiles cleanly. */

/* No cleanup attribute available (or globally disabled by __STDC_NO_GENERIC__). */
#define FLTVEC_GBC /* no-op: call free_float_vector() manually */

#endif /* feature gate */
// -------------------------------------------------------------------------------- 

void reverse_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

void swap_float(float* a, float* b);
// --------------------------------------------------------------------------------

void sort_float_vector(float_v* vec, iter_dir direction);
// -------------------------------------------------------------------------------- 

void trim_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

size_t binary_search_float_vector(float_v* vec, float value, float tolerance, bool sort_first);
// -------------------------------------------------------------------------------- 

#ifndef BIN_DAT_TYPEDEF
#define BIN_DAT_TYPEDEF
/**
 * @brief Result structure for binary search bound queries.
 *
 * The `bin_dat` struct encodes the indices that bound a given search value
 * within a sorted or unsorted float vector.
 *
 * Conventions:
 * - `lower`: Index of the last element strictly less than `value`,
 *            or SIZE_MAX if none exist.
 * - `upper`: Index of the first element strictly greater than `value`,
 *            or SIZE_MAX if none exist.
 *
 * When the search value matches an element within `tolerance`, both
 * `lower` and `upper` are set to that element's index.
 */
typedef struct {
    size_t lower;
    size_t upper;
} bin_dat;
#endif
// -------------------------------------------------------------------------------- 

bin_dat binary_search_bounds_float_vector(float_v* vec, 
                                          float value, 
                                          float tolerance, 
                                          bool sort_first);
// -------------------------------------------------------------------------------- 

void update_float_vector(float_v* vec, size_t index, float replacement_value);
// -------------------------------------------------------------------------------- 

float min_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

float max_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

float sum_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

float average_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

float stdev_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

float_v* cum_sum_float_vector(float_v* vec);
// -------------------------------------------------------------------------------- 

float_v* copy_float_vector(const float_v* original);
// -------------------------------------------------------------------------------- 

float dot_float(const float* a, const float* b, size_t len);
// -------------------------------------------------------------------------------- 

float dot_float_vector(const float_v* vec1, const float_v* vec2);
// -------------------------------------------------------------------------------- 

bool cross_float(const float* a, const float* b, float* result);
// -------------------------------------------------------------------------------- 

float_v* cross_float_vector(const float_v* vec1, const float_v* vec2);
// -------------------------------------------------------------------------------- 

float float_lin_interp(float x1, float y1, float x2, float y2, float x);
// ================================================================================ 
// ================================================================================ 
// DICTIONARY PROTOTYPES 

/**
 * @typedef dict_f
 * @brief Opaque struct representing a dictionary.
 *
 * This structure encapsulates a hash table that maps string keys to float values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_f dict_f;
// --------------------------------------------------------------------------------

/**
 * @brief Initializes a new dictionary.
 *
 * Allocates and initializes a dictionary object with a default size for the hash table.
 *
 * @return A pointer to the newly created dictionary, or NULL if allocation fails.
 */
dict_f* init_float_dict();
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
bool insert_float_dict(dict_f* dict, const char* key, float value);
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
float pop_float_dict(dict_f* dict,  const char* key);
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
float get_float_dict_value(const dict_f* dict, const char* key);
// --------------------------------------------------------------------------------

/**
 * @brief Frees the memory associated with the dictionary.
 *
 * Releases all memory allocated for the dictionary, including all key-value pairs.
 *
 * @param dict Pointer to the dictionary to free.
 */
void free_float_dict(dict_f* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Safely frees a dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_float_dict(dict_f** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICT_GBC
     * @brief A macro for enabling automatic cleanup of dict_t objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define FDICT_GBC __attribute__((cleanup(_free_float_dict)))
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
bool update_float_dict(dict_f* dict, const char* key, float value);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the number of non-empty buckets in the dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t float_dict_size(const dict_f* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t float_dict_alloc(const dict_f* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t float_dict_hash_size(const dict_f* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Checks if a key exists in the dictionary without retrieving its value
 * 
 * @param dict Pointer to the dictionary
 * @param key Key to check for
 * @return bool true if key exists, false otherwise
 */
bool has_key_float_dict(const dict_f* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a deep copy of a dictionary
 * 
 * @param dict Pointer to the dictionary to copy
 * @return dict_f* New dictionary containing copies of all entries, NULL on error
 */
dict_f* copy_float_dict(const dict_f* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes all entries from a dictionary without freeing the dictionary itself
 * 
 * @param dict Pointer to the dictionary to clear
 * @return bool true if successful, false otherwise
 */
bool clear_float_dict(dict_f* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all keys in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return char** Array of strings containing copies of all keys, NULL on error
 */
string_v* get_keys_float_dict(const dict_f* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all values in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return float* Array containing all values, NULL on error
 */
float_v* get_values_float_dict(const dict_f* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Merges two dictionaries into a new dictionary
 * 
 * @param dict1 First dictionary
 * @param dict2 Second dictionary
 * @param overwrite If true, values from dict2 override dict1 on key conflicts
 * @return dict_f* New dictionary containing merged entries, NULL on error
 */
dict_f* merge_float_dict(const dict_f* dict1, const dict_f* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for dictionary traversal
 */
typedef void (*dict_iterator)(const char* key, float value, void* user_data);

/**
 * @brief Iterates over all dictionary entries in insertion order
 * 
 * @param dict Pointer to the dictionary
 * @param iter Iterator function to call for each entry
 * @param user_data Optional user data passed to iterator function
 */
bool foreach_float_dict(const dict_f* dict, dict_iterator iter, void* user_data);
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
typedef struct dict_fv dict_fv;
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a dictionary of float vectors
 *
 * @returns a dictionary of float vectors
 */
dict_fv* init_floatv_dict(void);
// -------------------------------------------------------------------------------- 

/**
* @function create_floatv_dict
* @brief Creates a key vector/array pair 
*
* @param dict A fict_fv data type
* @param key a string literal key
* @param size The size of the array or vector 
* @return true if the function executes succesfully, false otherwise
*/
bool create_floatv_dict(dict_fv* dict, char* key, size_t size);
// -------------------------------------------------------------------------------- 

/**
* @function pop_floatv_dict 
* @brief Removes a statically or dynamically allocated array from the dictionary
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return true if the function executes succesfully, false otherwise
*/
bool pop_floatv_dict(dict_fv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function pop_floatv_dict 
* @brief Returns a float_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return a float_v pointer so it can be used in vector and array functions
*/
float_v* return_floatv_pointer(dict_fv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function free_floatv_dict 
* @brief Returns a float_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
*/
void free_floatv_dict(dict_fv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Safely frees a vector dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_floatv_dict(dict_fv** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICTV_GBC
     * @brief A macro for enabling automatic cleanup of dict_fv objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define FDICTV_GBC __attribute__((cleanup(_free_floatv_dict)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief determines if a key value pair exists in a vector dictionary
 *
 * @param dict The float vector dictionary 
 * @param key The key value being searched for 
 * @return true if the key value pair exists, false otherwise.
 */
bool has_key_floatv_dict(const dict_fv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Inserts an already existing dynamically allocated array to dictionary
 *
 * @param dict The float vector dictionary 
 * @param key The key value being searched for 
 * @param vec A dynamically allocated array of type float_v
 * @return true if the key value pair exists, false otherwise.
 */
bool insert_floatv_dict(dict_fv* dict, const char* key, float_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets the number of non-empty buckets in the vector dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t float_dictv_size(const dict_fv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the vector dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t float_dictv_alloc(const dict_fv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the vector dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t float_dictv_hash_size(const dict_fv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Creates a deep copy of a vector float dictionary 
 *
 * @param original A float vector dictionary 
 * @return A copy of a dictionary
 */
dict_fv* copy_floatv_dict(const dict_fv* original);
// -------------------------------------------------------------------------------- 

/**
 * @brief merges to float vector dictionaries
 *
 * Returns a merged float vector dictionary
 *
 * @param dict1 Pointer to a dictionary.
 * @param dict2 Pointer to a dictionary
 * @param overwrite true if the values should be overwritten, false otherwise
 * @return A merged dictionary
 */
dict_fv* merge_floatv_dict(const dict_fv* dict1, const dict_fv* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Clears all keys and values in the vector dictionary
 *
 * @param dict Pointer to a dictionary.
 */
void clear_floatv_dict(dict_fv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for float vector dictionary traversal
 *
 * @param key        Key string of the current entry
 * @param value      Pointer to the associated float_v vector
 * @param user_data  Optional context pointer passed through
 */
typedef void (*dict_fv_iterator)(const char* key, const float_v* value, void* user_data);
// -------------------------------------------------------------------------------- 

/**
 * @brief Applies a callback function to every key-value pair in a float_v dictionary
 *
 * Iterates over all entries in the hash table and invokes the callback for each.
 *
 * @param dict       Pointer to the dictionary
 * @param iter       Function pointer to apply to each key/value pair
 * @param user_data  Optional context pointer passed through to the callback
 * @return true on success, false if dict or iter is NULL (errno set to EINVAL)
 */
bool foreach_floatv_dict(const dict_fv* dict, dict_fv_iterator iter, void* user_data);
// -------------------------------------------------------------------------------- 

string_v* get_keys_floatv_dict(const dict_fv* dict);
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
    MATRIX_INVALID = -1,    // No matrix option for NULL types
    DENSE = 0,              // Dense Matrix
    COO,                    // Coordinate list format
    CSR,                    // Compressed sparse row format
    CSC                     // Compressed sparse column format
} MatrixType;
#endif /* MATRIX_ENUM */
// -------------------------------------------------------------------------------- 

typedef struct { 
    uint32_t row;
    uint32_t col;
    float data;
} triplet_f;
// -------------------------------------------------------------------------------- 

typedef struct {
    float* data;        // Values in 1-D array translated to matrix
    size_t alloc;         // Allocated size of 1-D array
    size_t size;          // populated size of 1-D array
    uint8_t* init_bits;   // Array of 1 and 0's denote initialized element
} dense_f;
// -------------------------------------------------------------------------------- 

typedef struct {
    triplet_f* triplets;  // 1-D array of triplets containing row, col and values 
    size_t alloc;         // Allocated size of triplets array 
    size_t size;           // Populated size of triplets array 
    bool  fast_set;        // true if optimized for data addition, false for data retrieval
} coo_f;
// -------------------------------------------------------------------------------- 

typedef struct { 
    float* data;       // 1-D array of values 
    uint32_t* col_idx;   // Column index 
    size_t* row_ptr;     // pointer to where row values begin 
    size_t alloc;        // Allocated size of arrays 
    size_t size;         // Populated size of arrays 
} csr_f;
// -------------------------------------------------------------------------------- 

typedef struct {
    float* data;      // 1-D array of values 
    uint32_t* row_idx;  // Row index 
    size_t* col_ptr;    // Pointer to where column begins 
    size_t alloc;       // Allocated size of arrays 
    size_t size;        // Populated size of arrays 
} csc_f;
// -------------------------------------------------------------------------------- 

typedef struct {
    MatrixType type;
    size_t rows;
    size_t cols;
    union {
        dense_f dense;
        coo_f coo;
        csr_f csr;
        csc_f csc;
    } storage;
} matrix_f;
// -------------------------------------------------------------------------------- 

matrix_f* init_float_matrix(size_t rows, size_t cols, size_t initial_alloc, MatrixType mat_type);
// -------------------------------------------------------------------------------- 

void free_float_matrix(matrix_f* mat);
// -------------------------------------------------------------------------------- 

void _free_float_matrix(matrix_f** pm);
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
    #define FLTMAT_GBC __attribute__((cleanup(_free_float_matrix)))
#endif
// -------------------------------------------------------------------------------- 

bool insert_float_matrix(matrix_f* m, size_t r, size_t c, float v);
// --------------------------------------------------------------------------------

bool set_float_matrix(matrix_f* m, size_t r, size_t c, float v);
// -------------------------------------------------------------------------------- 

bool finalize_float_coo_matrix(matrix_f* mat);
// -------------------------------------------------------------------------------- 

size_t float_matrix_size(const matrix_f* mat);
// -------------------------------------------------------------------------------- 

size_t float_matrix_alloc(const matrix_f* mat);
// -------------------------------------------------------------------------------- 

MatrixType float_matrix_type(const matrix_f* mat);
// -------------------------------------------------------------------------------- 

bool set_float_fast_set(matrix_f* mat);
// -------------------------------------------------------------------------------- 

bool delete_float_matrix_element(matrix_f* m, size_t r, size_t c);
// -------------------------------------------------------------------------------- 

float get_float_matrix_element(const matrix_f* m, size_t r, size_t c);
// -------------------------------------------------------------------------------- 

matrix_f* float_matrix_from_array(size_t rows, size_t cols, MatrixType type,
                                  const float* array, size_t arr_len,
                                  size_t initial_alloc);
// ================================================================================ 
// ================================================================================ 
// GENERIC MACROS

/**
 * @macro f_size
 * @brief Generic macro to get the number of elements in a float container
 *
 * This generic macro provides a unified interface for getting the current number
 * of elements in any float container type. It automatically selects the appropriate
 * size function based on the container type.
 * Currently supported container types:
 * - float_v (calls float_vector_size)
 * Additional container types will be added as the library expands.
 *
 * @param f_struct Pointer to float container structure
 * @return Size (number of elements) in the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define f_size(f_struct) _Generic((f_struct), \
    float_v*: float_vector_size, \
    dict_f*: float_dict_size, \
    dict_fv*: float_dictv_size) (f_struct)
// --------------------------------------------------------------------------------

/**
 * @macro f_alloc
 * @brief Generic macro to get the allocation size of a float container
 *
 * This generic macro provides a unified interface for getting the current allocation
 * size in any float container type. It automatically selects the appropriate
 * allocation function based on the container type.
 * Currently supported container types:
 * - float_v (calls float_vector_alloc)
 * Additional container types will be added as the library expands.
 *
 * @param f_struct Pointer to float container structure
 * @return Allocation size (capacity) of the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define f_alloc(f_struct) _Generic((f_struct), \
    float_v*: float_vector_alloc, \
    dict_f*: float_dict_alloc, \
    dict_fv*: float_dictv_alloc) (f_struct)
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_float_H */
// ================================================================================
// ================================================================================
// eof
