// ================================================================================
// ================================================================================
// - File:    c_string.h
// - Purpose: This file contains the prototypes for functions used in the 
//            c_string library
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    December 30, 2024
// - Version: 0.1
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_string_H
#define c_string_H

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <errno.h>
#include "c_error.h"

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

#define CSALT_CSTR(p) ((const char*)(p))

/**
 * @struct string_t
 * @brief Forward declaration for a dynamic data structure for storing strings.
 *
 * Fields:
 *  - chart* str: Pointer to a string literal
 *  - size_t len: The current number of elements in the arrays.
 *  - size_t alloc: The total allocated capacity of the arrays.
 *  - ErrorCode error: The error enum
 */
typedef struct string_t string_t;
// --------------------------------------------------------------------------------

/**
 * @brief Allocate and initialize a `string_t` from a NUL-terminated C string.
 *
 * Creates a new ::string_t that owns an independent copy of @p str. The copy
 * is exactly the bytes up to (but not including) the terminating NUL, and the
 * internal buffer is NUL-terminated. The implementation determines the length
 * with a standard C string scan (e.g., `strlen`). This function has no shared 
 * mutable state; `errno` is thread local on conforming platform, safe for concurrant 
 * calls.  This function is of time complexity O(n), typically vectorized by libc.
 *
 * Error reporting:
 * - On failure, returns NULL and sets `errno` because no object exists to carry
 *   an ::ErrorCode.
 * - On success, the returned object's internal error status is initialized to ::NO_ERROR.
 *
 * @param str  Non-NULL pointer to a readable, NUL-terminated input string.
 *
 * @return Pointer to a newly allocated ::string_t on success; NULL on failure.
 *
 * @retval NULL
 *   - If @p str is NULL (`errno` = `EINVAL`).
 *   - If memory allocation fails for the object or its buffer (`errno` = `ENOMEM`).
 *   - If a pathological size overflow is detected when allocating the terminator
 *     (`errno` = `EOVERFLOW`).  (Defensive check for `len + 1`.)
 *
 * @pre  @p str points to a valid, accessible, NUL-terminated byte string.
 * @post On success, the caller owns the returned object and must release it with
 *       the corresponding destroy function (e.g., `free_string()`), unless using
 *       an auto-cleanup macro such as ::STRING_GBC.
 *
 * @warning This unbounded initializer assumes a valid NUL terminator. Passing a
 *          non–NUL-terminated buffer results in a read past the end of the buffer
 *          (undefined behavior). For untrusted data, prefer a bounded or length-
 *          based initializer if available.
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("Hello, World!");
 * if (s == NULL) {
 *     perror("init_string failed");   // errno set to EINVAL, ENOMEM, or EOVERFLOW
 *     return 1;
 * }
 * printf("String content: %s\n", get_string(s));
 *
 * // If compiling with GCC/Clang and using your auto-cleanup macro:
 * // string_t* s STRING_GBC = init_string("Hello, World!");
 * // ...use s...
 * // (no explicit free needed when using STRING_GBC)
 *
 * free_string(s);  // required if not using STRING_GBC
 * @endcode
 *
 * @see error_to_string, free_string, STRING_GBC
 */
string_t* init_string(const char* str);
// -------------------------------------------------------------------------------- 

/**
 * @brief Get the last ::ErrorCode recorded on a csalt string object.
 *
 * Returns the object's current error status without modifying it. This const
 * accessor is useful after a prior API call that may have set an error on the
 * object. It does not inspect `errno` or change the object's state.
 *
 * @param str  Pointer to a ::string_t instance (may be NULL).
 *
 * @return The last recorded ::ErrorCode on success. If @p str is NULL, returns
 *         ::ERR_INVALID_ERROR and sets `errno` to `EINVAL`.
 *
 * @errors Sets `errno` to:
 *   - `EINVAL` — if @p str is NULL.
 *
 * @thread_safety Thread-compatible. Safe for concurrent reads of distinct objects.
 *                Callers must synchronize if other threads may modify or free
 *                the same object concurrently.
 *
 * @complexity O(1).
 *
 * @par Example
 * @code{.c}
 * #include <errno.h>
 * #include <stdio.h>
 *
 * string_t* s = init_string("Hello");
 * if (!s) { perror("init_string"); return 1; }
 *
 * // Normal case: freshly constructed object has NO_ERROR
 * ErrorCode ec1 = get_string_error(s);
 * printf("ec1 = %d (%s)\n", (int)ec1, error_to_string(ec1));
 *
 * // Error path: NULL object — returns ERR_INVALID_ERROR and sets errno=EINVAL
 * ErrorCode ec2 = get_string_error(NULL);
 * if (ec2 == INVALID_ERROR && errno == EINVAL) {
 *     printf("NULL object -> %s\n", error_to_string(ec2));
 * }
 *
 * free_string(s);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * ec1 = 0 (No error)
 * NULL object -> Invalid error entry
 * @endcode
 *
 * @see error_to_string
 */
ErrorCode get_string_error(const string_t* str);
// --------------------------------------------------------------------------------

/**
 * @brief Release a csalt string object and its owned buffer.
 *
 * Deallocates the internal character buffer (if present) and the ::string_t
 * container itself. After this call, the pointer is invalid and must not be
 * dereferenced. It is recommended to set the pointer to NULL after freeing.
 * Safe for concurrant calls on *different* objects. Do not free same object 
 * from multiple threads concurrently. Time complexity of O(1).
 *
 * Error reporting:
 * - If @p str is NULL, no deallocation occurs; `errno` is set to `EINVAL`
 *   and the function returns. (NULL is treated as an error, not a no-op.)
 *
 * @param str  Pointer to a ::string_t previously returned by ::init_string
 *             (or an equivalent constructor). Must not be freed twice or used
 *             after this call.
 *
 * @pre  @p str is either NULL or a valid pointer obtained from the csalt API.
 * @post If @p str was non-NULL, all memory owned by the object is released.
 *
 * @warning Calling this function twice on the same non-NULL pointer results
 *          in undefined behavior (double free). Do not use @p str after freeing.
 *
 * @see init_string, STRING_GBC
 */
void free_string(string_t* str);
// -------------------------------------------------------------------------------- 

/**
 * @function _free_string
 * @brief A helper function for use with cleanup attributes to free string_t objects.
 *
 * This function frees the memory of a string_t object and sets the pointer to NULL.
 *
 * @param str A double pointer to the string_t object to be freed.
 * @return void.
 */
void _free_string(string_t** str);
// --------------------------------------------------------------------------------

/* compiler attribute helper */
#if defined(__GNUC__) || defined(__clang__)
  #define CSALT_STR_CLEANUP(f) __attribute__((cleanup(f)))
#else
  #define CSALT_STR_CLEANUP(f) /* no-op on non-GNU/Clang compilers */
#endif

/**
 * @def STRING_GBC
 * @brief GCC/Clang auto-cleanup qualifier for ::string_t*.
 *
 * When appended to a pointer variable declaration, registers a cleanup handler
 * so the pointer is automatically freed when it goes out of scope (via
 * `__attribute__((cleanup(...)))`).
 *
 * @par Usage
 * @code{.c}
 * string_t* s STRING_GBC = init_string("Hello");
 * if (!s) { perror("init_string"); return 1; }
 * printf("%s\n", get_string(s));
 * // no explicit free needed on scope exit
 * @endcode
 *
 * @warning Supported only on GCC and Clang. On other compilers this macro
 *          expands to a no-op (no automatic free).
 *
 * @see init_string, free_string
 */
#define STRING_GBC CSALT_STR_CLEANUP(_free_string)
// --------------------------------------------------------------------------------

/**
 * @brief Borrow the internal NUL-terminated C string from a csalt string object.
 *
 * Returns a read-only pointer to the object's internal buffer. The pointer is
 * valid until the object is modified or destroyed; the caller must not free it.
 * This const accessor does not modify the object's ::ErrorCode field.  Safe for 
 * concurrant reads of distinct objects.  Not safe if another thread may modify 
 * or free the same object concurrently. This function is of a time complexity 
 * of O(1).
 *
 * Error reporting:
 * - On failure, returns NULL and sets `errno` (no mutation of the object).
 *
 * @param str  Pointer to a valid ::string_t instance (may be NULL).
 *
 * @return Read-only pointer to the internal NUL-terminated string on success;
 *         NULL on failure.
 *
 * @retval NULL
 *   - if @p str is NULL (`errno` = `EINVAL`);
 *   - if @p str is non-NULL but its internal buffer is missing (`errno` = `EINVAL`).
 *
 * @warning The returned pointer is *borrowed*; do not free it and do not use it
 *          after the object is modified or freed.
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("Hello, World!");
 * if (!s) {
 *     perror("init_string failed");
 *     return 1;
 * }
 *
 * const char* view = get_string(s);
 * if (!view) {
 *     perror("get_string failed");  // errno = EINVAL
 *     free_string(s);
 *     return 1;
 * }
 *
 * printf("String content: %s\n", view);  // prints: Hello, World!
 * free_string(s);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * String content Hello, World!
 * @endcode*
 *
 * @see init_string, free_string
 */
const char* get_string(const string_t* str);
// --------------------------------------------------------------------------------

/**
 * @brief Return the number of bytes in a csalt string (excluding the terminating NUL).
 *
 * Retrieves the cached length of the string in constant time. This accessor does
 * not modify the object and does not return the terminating NUL in the count.
 * Safe for cuncurrant reads of distinct objects.  Callers must synchronize if 
 * another thread may modify or free the same object concurrently..  This function 
 * is of time complexity O(1).
 *
 * Error reporting:
 * - On invalid input (e.g., @p str is NULL or its internal buffer is NULL),
 *   the function returns ::LONG_MAX and sets `errno` to `EINVAL`.
 *
 * @param str  Pointer to a valid ::string_t instance (may be NULL).
 *
 * @return The length of the string in bytes (excluding the terminating NUL) on
 *         success; ::SIZE_MAX on failure with `errno` set to `EINVAL`.
 *
 * @par Example
 * @code{.c}
 * #include <errno.h>
 * #include <limits.h>
 * #include <stdio.h>
 *
 * string_t* s = init_string("Hello");
 * if (!s) {
 *     perror("init_string");
 *     return 1;
 * }
 *
 * size_t n = string_size(s);
 * if (n == SIZE_MAX) {     // error path
 *     perror("string_size");
 *     free_string(s);
 *     return 1;
 * }
 *
 * printf("Length = %zu bytes\n", n);   // prints: Length = 5 bytes
 * free_string(s);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * Length = 5 bytes
 * @endcode*
 *
 * @see init_string, get_string, free_string
 */
size_t string_size(const string_t* str);
// --------------------------------------------------------------------------------

/**
 * @brief Return the total allocated size in bytes for a csalt string's buffer
 *        (including the terminating NUL).
 *
 * Retrieves the buffer capacity recorded for @p str. For csalt strings created
 * by ::init_string, this is typically `string_size(str) + 1`. Future growth
 * strategies may reserve more than the current length, so the allocation size
 * can exceed the payload length.  Safe for cuncurrant reads of distinct objects.
 * Callers must synchronize if another thread may modify or free the same object 
 * concurrantly.  This function is of time complexity O(1).
 *
 * Error reporting:
 * - On invalid input (e.g., @p str is NULL or its internal buffer is NULL),
 *   the function returns ::SIZE_MAX and sets `errno` to `EINVAL`.
 *
 * @param str  Pointer to a valid ::string_t instance (may be NULL).
 *
 * @return The allocated size in bytes (including the NUL terminator) on success;
 *         SIZE_MAX on failure with `errno` set to `EINVAL`.
 *
 * @par Example
 * @code{.c}
 * #include <errno.h>
 * #include <limits.h>
 * #include <stdio.h>
 *
 * string_t* s = init_string("Hello");
 * if (!s) {
 *     perror("init_string");
 *     return 1;
 * }
 *
 * size_t cap = string_alloc(s);
 * if (cap == SIZE_MAX) {
 *     perror("string_alloc");   // errno = EINVAL
 *     free_string(s);
 *     return 1;
 * }
 *
 * printf("Allocated capacity = %zu bytes\n", cap);
 * free_string(s);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * Allocated capacity = 6 bytes
 * @endcode
 *
 * @see init_string, string_size, get_string, free_string
 */
size_t string_alloc(const string_t* str);
// --------------------------------------------------------------------------------

/**
 * @brief Append the contents of @p str2 to @p str1, growing @p str1 as needed.
 *
 * Concatenates two csalt strings. If @p str1's buffer is too small, it is
 * reallocated to fit the result (including the terminating NUL). On success,
 * @p str1 becomes a NUL-terminated string of length `str1->len + str2->len`,
 * and its `len`/`alloc` fields are updated. @p str2 is not modified. Not thread-safe 
 * for the same object.  This function mutates `str1`.  Callers must synchornize
 * if another thread may read/modify/free `str1`.  This function is of time complexity 
 * O(n1)+O(n2) due to the use of `strncat` within the function (which scans the destinationlength)
 * plus potential reallocation cost.
 *
 * Error reporting:
 * - Returns `false` and sets `errno` to `EINVAL` if any argument is NULL.  If  
 *   `str1` is present but `str2` is NULL, the error code is set to `INVALID_ARG` 
 *   on `str1`.  If any of the buffers are missing `errno` is set to `EINVAL` and 
 *   the error code for `str1` is set to `NULL_POINTER`.
 * - Returns `false` and sets `errno` to `ENOMEM` if reallocation fails; in this
 *   case, @p str1->error is set to `REALLOC_FAIL`.
 * - On success, returns `true`. (The function does not modify @p str1->error
 *   on success; it typically remains `NO_ERROR`.)
 *
 * @param str1  Destination string to be extended (must be non-NULL, NUL-terminated).
 * @param str2  Source string whose bytes are appended (must be non-NULL, NUL-terminated).
 *
 * @return `true` on success; `false` on failure with `errno` set as described above.
 *
 * @pre  `str1 != NULL`, `str2 != NULL`, `str1->str != NULL`, `str2->str != NULL`.
 *       The two underlying buffers must not alias in a way that causes overlap.
 * @post On success, `str1` contains the concatenation and remains NUL-terminated;
 *       `str2` is unchanged. If a reallocation occurred, any previously borrowed
 *       pointer from `get_string(str1)` is invalidated.
 *
 * @par Example
 * @code{.c}
 * #include <errno.h>
 * #include <stdio.h>
 *
 * string_t* a = init_string("Hello");
 * string_t* b = init_string(", World!");
 * if (!a || !b) { perror("init_string"); free_string(a); free_string(b); return 1; }
 *
 * if (!string_string_concat(a, b)) {
 *     perror("string_string_concat");
 *     fprintf(stderr, "dest error: %s\n", error_to_string(get_string_error(a)));
 *     free_string(a);
 *     free_string(b);
 *     return 1;
 * }
 *
 * printf("Result: %s\n", get_string(a));
 * printf("Length: %zu, Alloc: %zu\n", string_size(a), string_alloc(a));
 *
 * free_string(a);
 * free_string(b);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * Result: Hello, World!
 * Length: 13, Alloc: 14
 * @endcode
 *
 * @see init_string, get_string, string_size, string_alloc, free_string
 */
bool string_string_concat(string_t* str1, const string_t* str2);
// -------------------------------------------------------------------------------- 

/**
 * @brief Append a C string to a csalt string, growing the destination as needed.
 *
 * Concatenates the bytes of @p literal to the end of @p str1. If @p str1 does not
 * have enough capacity, its internal buffer is reallocated to fit the result,
 * including the terminating NUL. Overlapping inputs are handled safely:
 * if @p literal points into @p str1's current buffer, the implementation uses
 * a temporary copy (when growth is required) and `memmove` to avoid undefined
 * behavior.  Not thread-safe for the same object (mutates @p str`). Callers 
 * must synchronize if other threads may read/modify/free @p str1.  This function 
 * has a time complexity of )(n) driven by the reallocation cost and by the search 
 * for the nul terminator.
 *
 * Error reporting:
 * - Returns `false` and sets `errno` to `EINVAL` for invalid arguments (NULL
 *   pointers). In this case, `str1->error` is set to ::INVALID_ARG or
 *   ::NULL_POINTER as appropriate.
 * - Returns `false` and sets `errno` to `EOVERFLOW` if the required size would
 *   exceed `SIZE_MAX` (sets `str1->error` to ::NUMERIC_OVERFLOW).
 * - Returns `false` and sets `errno` to `ENOMEM` on allocation failure
 *   (sets `str1->error` to ::BAD_ALLOC or ::REALLOC_FAIL).
 * - On success, returns `true` and sets `str1->error` to ::NO_ERROR.
 *
 * @param str1     Destination string to be extended (must be non-NULL and initialized).
 * @param literal  NUL-terminated C string to append (may alias @p str1->str).
 *
 * @return `true` on success; `false` on failure with `errno` and `str1->error` set.
 *
 * @pre  `str1 != NULL`, `str1->str != NULL`, `literal != NULL`.
 * @post On success, @p str1 contains the concatenation and remains NUL-terminated.
 *       If a reallocation occurred, any previously borrowed pointer obtained via
 *       `get_string(str1)` is invalidated.
 *
 * @par Example
 * @code{.c}
 * #include <errno.h>
 * #include <stdio.h>
 *
 * string_t* s = init_string("Hello");
 * if (!s) { perror("init_string"); return 1; }
 *
 * // Simple append
 * if (!string_lit_concat(s, ", World!")) {
 *     perror("string_lit_concat");
 *     fprintf(stderr, "error: %s\n", error_to_string(get_string_error(s)));
 *     free_string(s);
 *     return 1;
 * }
 * printf("Result: %s\n", get_string(s));
 *
 * // Overlapping append (safe): append from inside the current buffer
 * // After the first concat, get_string(s) == "Hello, World!"
 * const char* tail = get_string(s) + 7; // points to "World!"
 * if (!string_lit_concat(s, tail)) {
 *     perror("string_lit_concat (overlap)");
 *     fprintf(stderr, "error: %s\n", error_to_string(get_string_error(s)));
 *     free_string(s);
 *     return 1;
 * }
 * printf("After overlap: %s\n", get_string(s));
 *
 * free_string(s);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * Result: Hello, World!
 * After overlap: Hello, World!World!
 * @endcode
 *
 * @see init_string, get_string, string_string_concat, string_size, string_alloc, free_string
 */
bool string_lit_concat(string_t* str1, const char* literal);
// --------------------------------------------------------------------------------

/**
 * @def string_concat
 * @brief Generic front-end to concatenate onto a csalt string.
 *
 * Dispatches at compile time (C11 `_Generic`) based on the type of @p str_two:
 * - If @p str_two is a C string pointer (`const char*` or `char*`), calls
 *   ::string_lit_concat(@p str_one, @p str_two).
 * - Otherwise, calls ::string_string_concat(@p str_one, @p str_two).
 *
 * @param str_one  Destination ::string_t* to be extended.
 * @param str_two  Either a C string (``const char*``/``char*``) or a
 *                 ::string_t pointer (``const string_t*``/``string_t*``).
 *
 * @return See the selected function:
 *         ::string_lit_concat or ::string_string_concat (both return `bool`).
 *
 * @note This macro relies on C11 generic selection. It requires a compiler
 *       with `_Generic` support (C11) and that `__STDC_NO_GENERIC__` is not defined.
 *
 * @warning Error handling and thread-safety characteristics are those of the
 *          selected function. See ::string_lit_concat and ::string_string_concat
 *          for details (they set `errno` and update `str_one->error`).
 *
 * @par Example (C string)
 * @code{.c}
 * string_t* s = init_string("pi = ");
 * if (!s) { perror("init_string"); return 1; }
 *
 * if (!string_concat(s, "3.14159")) {    // dispatches to string_lit_concat
 *     perror("string_concat");
 *     fprintf(stderr, "error: %s\n", error_to_string(get_string_error(s)));
 *     free_string(s);
 *     return 1;
 * }
 * printf("%s\n", get_string(s));
 * free_string(s);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * pi = 3.14159
 * @endcode
 *
 * @par Example (csalt string)
 * @code{.c}
 * string_t* a = init_string("Hello");
 * string_t* b = init_string(", world!");
 * if (!a || !b) { perror("init_string"); free_string(a); free_string(b); return 1; }
 *
 * if (!string_concat(a, b)) {            // dispatches to string_string_concat
 *     perror("string_concat");
 *     fprintf(stderr, "error: %s\n", error_to_string(get_string_error(a)));
 *     free_string(a); free_string(b);
 *     return 1;
 * }
 * printf("%s\n", get_string(a));
 * free_string(a); free_string(b);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * Hello, world!
 * @endcode
 */ 
/**
 * @def string_concat
 * @brief Generic front-end to concatenate onto a csalt string.
 * ...
 */
bool string_concat__type_mismatch(const string_t*, void*);

#if defined(DOXYGEN)
/* Simple definition so Doxygen indexes the macro unconditionally */
#  define string_concat(str_one, str_two) string_string_concat((str_one), (str_two))
#elif __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_GENERIC__)
#  define string_concat(str_one, str_two) \
     _Generic((str_two), \
        const char*:     string_lit_concat, \
        char*:           string_lit_concat, \
        string_t*:       string_string_concat, \
        const string_t*: string_string_concat, \
        void*:           string_string_concat, \
        default:         string_concat__type_mismatch \
     )((str_one), (str_two))
#else
#  define string_concat(str_one, str_two) string_string_concat((str_one), (str_two))
#endif
// --------------------------------------------------------------------------------

/**
 * @brief Lexicographically compare a csalt string with a NUL-terminated C string.
 *
 * Compares the bytes of @p str_struct to those of @p string in lexicographic
 * order (like `strcmp`). Returns a negative value if @p str_struct is less than
 * @p string, zero if they are equal, or a positive value if it is greater.
 *
 * Error reporting:
 * - On invalid input (e.g., @p str_struct is NULL, its buffer is NULL, or
 *   @p string is NULL), sets `errno` to `EINVAL` and returns ::INT_MIN as an
 *   error sentinel.
 *
 * @param str_struct  Pointer to a valid ::string_t.
 * @param string      Pointer to a NUL-terminated C string.
 *
 * @return Negative, zero, or positive according to lexicographic order;
 *         ::INT_MIN on failure with `errno` = `EINVAL`.
 *
 * @note For consistent `strcmp`-like semantics with non-ASCII bytes, compare
 *       as **unsigned** bytes (i.e., cast to `unsigned char` before subtraction).
 *
 * @thread_safety Thread-compatible. Safe for concurrent reads of distinct objects.
 *
 * @complexity O(min(n, m)).
 *
 * @par Example
 * @code{.c}
 * string_t* a = init_string("abc");
 * int r1 = compare_strings_lit(a, "abd");   // r1 < 0
 * int r2 = compare_strings_lit(a, "abc");   // r2 == 0
 * int r3 = compare_strings_lit(a, "ab");    // r3 > 0
 * printf("%d %d %d\n", r1, r2, r3);
 * free_string(a);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * -1 0 1
 * @endcode
 */
int compare_strings_lit(const string_t* str_struct, const char* string);
// --------------------------------------------------------------------------------

/**
 * @brief Lexicographically compare two csalt strings.
 *
 * Compares @p str_struct_one to @p str_struct_two byte-by-byte in lexicographic
 * order (like `strcmp`). Returns a negative value if the first is less than the
 * second, zero if they are equal, or a positive value if it is greater.
 *
 * Error reporting:
 * - On invalid input (any pointer NULL or missing internal buffer), sets `errno`
 *   to `EINVAL` and returns ::INT_MIN as an error sentinel.
 *
 * @param str_struct_one  First ::string_t to compare.
 * @param str_struct_two  Second ::string_t to compare.
 *
 * @return Negative, zero, or positive according to lexicographic order;
 *         ::INT_MIN on failure with `errno` = `EINVAL`.
 *
 * @note The second parameter is not modified and could be declared `const`
 *       in future revisions of the API.
 * @note For consistent `strcmp`-like semantics with non-ASCII bytes, compare
 *       as **unsigned** bytes (i.e., cast to `unsigned char` before subtraction).
 *
 * @thread_safety Thread-compatible. Safe for concurrent reads of distinct objects.
 *
 * @complexity O(min(n, m)).
 *
 * @par Example
 * @code{.c}
 * string_t* a = init_string("Hello");
 * string_t* b = init_string("Hello");
 * string_t* c = init_string("Help");
 *
 * int ab = compare_strings_string(a, b);  // 0
 * int ac = compare_strings_string(a, c);  // negative ('l' < 'p')
 * int ca = compare_strings_string(c, a);  // positive
 *
 * printf("%d %d %d\n", ab, ac, ca);
 * free_string(a); free_string(b); free_string(c);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * 0 -4 4
 * @endcode
 */
int compare_strings_string(const string_t* str_struct_one, string_t* str_struct_two);
// --------------------------------------------------------------------------------

/**
 * @def compare_strings
 * @brief Generic, `strcmp`-style comparison for csalt strings.
 *
 * Dispatches at compile time (C11 `_Generic`) based on the type of @p str_two:
 * - If @p str_two is a C string pointer, calls ::compare_strings_lit(@p str_one, @p str_two).
 * - Otherwise, calls ::compare_strings_string(@p str_one, @p str_two).
 *
 * Semantics mirror `strcmp`:
 * - return < 0  ⇒  @p str_one is lexicographically less than @p str_two
 * - return = 0  ⇒  equal
 * - return > 0  ⇒  greater
 *
 * Error reporting:
 * - On invalid input, the selected function sets `errno = EINVAL` and returns
 *   ::INT_MIN as an error sentinel.
 *
 * @param str_one  Pointer to the first ::string_t to compare.
 * @param str_two  Either a C string (``char*``/``const char*``) or a csalt string
 *                 (``string_t*``/``const string_t*``).
 *
 * @return Negative, zero, or positive according to lexicographic order; ::INT_MIN on failure.
 *
 * @note Requires a C11 compiler with `_Generic` support (not available if
 *       `__STDC_NO_GENERIC__` is defined).
 *
 * @warning The macro inherits thread-safety and error behavior from the selected
 *          function. See ::compare_strings_lit and ::compare_strings_string.
 *
 * @par Example (C string)
 * @code{.c}
 * string_t* a = init_string("abc");
 * int r = compare_strings(a, "abd");   // dispatches to compare_strings_lit
 * printf("%d\n", r);                   // negative (e.g., -1)
 * free_string(a);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * -1
 * @endcode
 *
 * @par Example (csalt string)
 * @code{.c}
 * string_t* x = init_string("Hello");
 * string_t* y = init_string("Help");
 * int rx = compare_strings(x, y);      // dispatches to compare_strings_string
 * printf("%d\n", rx);                  // negative ('l' < 'p')
 * free_string(x); free_string(y);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * -4
 * @endcode
 */ 
#if defined(DOXYGEN)
/* Simple definition so Doxygen always indexes the macro */
#  define compare_strings(str_one, str_two) compare_strings_string((str_one), (str_two))
#elif __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_GENERIC__)
#  define compare_strings(str_one, str_two) \
     _Generic((str_two), \
        const char*:      compare_strings_lit,    \
        char*:            compare_strings_lit,    \
        const string_t*:  compare_strings_string, \
        string_t*:        compare_strings_string, \
        void*:            compare_strings_string, \
        default:          compare_strings__type_mismatch \
     )((str_one), (str_two))
int compare_strings__type_mismatch(const string_t*, void*);
#else
/* Pre-C11 fallback: caller must pass csalt strings */
#  define compare_strings(str_one, str_two) compare_strings_string((str_one), (str_two))
#endif

// --------------------------------------------------------------------------------

/**
 * @brief Deep-copy a csalt string.
 *
 * Allocates a new ::string_t and copies the contents of @p str into it. The
 * resulting object owns its own buffer (no sharing). If the source has a larger
 * reserved capacity than its current length, this function attempts to reserve
 * at least the same capacity in the copy so future appends behave similarly.
 * This function is thread-compaitible; however, the source must not be modified 
 * or freed concurrently with this call.  this function has a time complexity 
 * of O(n).
 *
 * Error reporting:
 * - Returns NULL and sets `errno` to `EINVAL` if @p str is NULL, if @p str has
 *   no internal buffer, or if borrowing the source buffer via ::get_string fails.
 * - Returns NULL and sets `errno` to `ENOMEM` if allocation fails (either during
 *   construction or while reserving capacity to match the source).
 *
 * @param str  Source ::string_t to copy (must be non-NULL and initialized).
 *
 * @return Pointer to a newly allocated ::string_t on success; NULL on failure
 *         with `errno` set as described above.
 *
 * @par Example
 * @code{.c}
 * string_t* a = init_string("Hello");
 * if (!a) { perror("init_string"); return 1; }
 *
 * // Make a deep copy 'b' of 'a'
 * string_t* b = copy_string(a);
 * if (!b) {
 *     perror("copy_string");
 *     free_string(a);
 *     return 1;
 * }
 *
 * printf("a: %s (len=%zu, alloc=%zu)\n",
 *        get_string(a), string_size(a), string_alloc(a));
 * printf("b: %s (len=%zu, alloc=%zu)\n",
 *        get_string(b), string_size(b), string_alloc(b));
 *
 * free_string(a);
 * free_string(b);
 * @endcode
 *
 * @par Output (example)
 * @code{.text}
 * a: Hello (len=5, alloc=6)
 * b: Hello (len=5, alloc=6)
 * @endcode
 *
 * @see init_string, get_string, reserve_string, free_string
 */
string_t* copy_string(const string_t* str);
// --------------------------------------------------------------------------------

/**
 * @brief Increase a csalt string's buffer capacity to a specific size.
 *
 * Attempts to reallocate @p str so that its total buffer capacity becomes
 * exactly @p len bytes (including space for the terminating NUL). The current
 * contents up to `str->len` are preserved. This function only grows capacity:
 * if @p len is less than or equal to the current allocation, the call fails.
 *
 * Error reporting:
 * - Returns `false` and sets `errno = EINVAL` if @p str is NULL, if the
 *   internal buffer is NULL (sets `str->error = ::NULL_POINTER`), or if
 *   @p len is not strictly greater than the current allocation
 *   (sets `str->error = ::INVALID_ARG`).
 * - Returns `false` and sets `errno = ENOMEM` if reallocation fails
 *   (sets `str->error = ::REALLOC_FAIL`).
 * - On success, returns `true` and leaves `str->error` unchanged (typically ::NO_ERROR).
 *
 * @param str  Pointer to a ::string_t to be grown.
 * @param len  Requested total capacity in bytes **including** the NUL terminator.
 *             Must satisfy `len > str->alloc`.
 *
 * @return `true` on success; `false` on failure with `errno` and `str->error` set.
 *
 * @note A successful reallocation may move the buffer. Any previously borrowed
 *       pointer obtained from `get_string(str)` becomes invalid after success.
 *
 * @par Example
 * @code{.c}
 * #include <errno.h>
 * #include <stdio.h>
 *
 * string_t* s = init_string("Hello");
 * if (!s) { perror("init_string"); return 1; }
 *
 * printf("Before: len=%zu alloc=%zu\n", string_size(s), string_alloc(s));
 *
 * // Grow capacity to 128 bytes (including NUL)
 * if (!reserve_string(s, 128)) {
 *     perror("reserve_string (grow)");   // errno = ENOMEM on failure
 *     free_string(s);
 *     return 1;
 * }
 *
 * printf("After grow: len=%zu alloc=%zu\n", string_size(s), string_alloc(s));
 *
 * // This call fails because requested size is not greater than current alloc
 * if (!reserve_string(s, string_alloc(s))) {
 *     perror("reserve_string (no-op rejected)");  // errno = EINVAL
 * }
 *
 * free_string(s);
 * @endcode
 *
 * @par Output (example)
 * @code{.text}
 * Before: len=5 alloc=6
 * After grow: len=5 alloc=128
 * reserve_string (no-op rejected): Invalid argument
 * @endcode
 *
 * @see trim_string, string_alloc, string_size, get_string
 */
bool reserve_string(string_t* str, size_t len);
// -------------------------------------------------------------------------------- 

/**
 * @brief Shrink a csalt string's buffer to the minimum needed size.
 *
 * Reduces the allocation of @p str so that `alloc == len + 1` (just enough
 * for the payload plus the terminating NUL). The string contents are unchanged.
 * If the buffer is already minimal, this is a no-op and succeeds.
 *
 * Error reporting:
 * - Returns `false` and sets `errno = EINVAL` if @p str is NULL or if the
 *   internal buffer is NULL (sets `str->error = ::NULL_POINTER`).
 * - Returns `false` and sets `errno = EINVAL` if a corrupted size invariant is
 *   detected (`len + 1 > alloc`; sets `str->error = ::SIZE_MISMATCH`).
 * - Returns `false` and sets `errno = ENOMEM` if the shrink `realloc` fails
 *   (sets `str->error = ::REALLOC_FAIL`).
 * - On success, returns `true` and sets `str->error = ::NO_ERROR`.
 *
 * @param str  Pointer to a ::string_t to be trimmed.
 *
 * @return `true` on success; `false` on failure with `errno` and `str->error` set.
 *
 * @note This function may move the buffer (via `realloc`). Any previously
 *       borrowed pointer obtained from `get_string(str)` becomes invalid after
 *       a successful trim.
 *
 * @par Example
 * @code{.c}
 * #include <errno.h>
 * #include <stdio.h>
 *
 * string_t* s = init_string("Hello");
 * if (!s) { perror("init_string"); return 1; }
 *
 * // Ensure excess capacity so trim has an effect (API may differ in your project)
 * if (!reserve_string(s, 128)) {  // grow capacity to at least 128 bytes
 *     perror("reserve_string");
 *     free_string(s);
 *     return 1;
 * }
 *
 * printf("Before trim: len=%zu, alloc=%zu\n", string_size(s), string_alloc(s));
 *
 * if (!trim_string(s)) {
 *     perror("trim_string");
 *     free_string(s);
 *     return 1;
 * }
 *
 * printf("After trim:  len=%zu, alloc=%zu\n", string_size(s), string_alloc(s));
 * free_string(s);
 * @endcode
 *
 * @par Output (example)
 * @code{.text}
 * Before trim: len=5, alloc=128
 * After trim:  len=5, alloc=6
 * @endcode
 *
 * @see reserve_string, string_size, string_alloc, get_string, free_string
 */
bool trim_string(string_t* str);
// -------------------------------------------------------------------------------- 

/**
 * @brief Find the **first** occurrence of a byte in a csalt string and return a
 *        writable pointer into the string's buffer.
 *
 * Searches the current payload of @p str (exactly `str->len` bytes; the
 * terminating NUL is not examined) for the first byte equal to @p value and
 * returns a *borrowed* pointer to that position on success. The pointer aliases
 * the object’s internal storage and remains valid until the string is
 * reallocated, trimmed, otherwise modified, or destroyed. Comparison is
 * performed as `(unsigned char)value`.
 *
 * **Thread-safety:** This function does not modify the character data, but it
 * may update `str->error` for status reporting. Treat it as **not thread-safe
 * for the same object** unless callers synchronize access to @p str (i.e., no
 * concurrent reads that also update `str->error`, and no concurrent writes or
 * frees). Concurrent calls on **distinct** objects are fine.
 *
 * **Time complexity:** O(n) in the string length. Implementations commonly use
 * `memchr()` which may be vectorized by the C library on many platforms, but
 * worst-case complexity remains linear.
 *
 * Error reporting:
 * - Returns `NULL` and sets `errno = EINVAL` if @p str is `NULL` or if the
 *   internal buffer is `NULL` (in the latter case, `str->error = ::NULL_POINTER`).
 * - Returns `NULL` when the byte is not found; in this case `errno` is set to 
 *   ENOENT and str->error is set to NOT_FOUND. 
 *   On valid inputs, `str->error` may be set to ::NO_ERROR.
 *
 * @param str    Pointer to a ::string_t instance (must be initialized).
 * @param value  Byte to search for (compared as `(unsigned char)value`).
 *
 * @return Writable pointer to the first matching byte on success; `NULL` on
 *         failure or if no match exists.
 *
 * @warning The returned pointer aliases @p str’s internal storage. It becomes
 *          invalid after any operation that can move or modify the buffer
 *          (e.g., concatenation, reserve, trim, or destruction). Do **not**
 *          `free()` the returned pointer.
 *
 * @note To search for the terminator `'\0'`, include it explicitly in the
 *       search length; this function searches only `str->len` bytes.
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("abracadabra");
 * if (!s) { perror("init_string"); return 1; }
 *
 * char* p = first_char_occurrance(s, 'a');
 * if (p) {
 *     const char* base = get_string(s);
 *     printf("first 'a' at %zu: \"%s\"\n", (size_t)(p - base), p);
 *     *p = 'A';  // mutate in place
 *     printf("after mutate: %s\n", get_string(s));
 * } else {
 *     puts("byte not found");
 * }
 *
 * free_string(s);
 * @endcode
 *
 * @par Output (example)
 * @code{.text}
 * first 'a' at 0: "abracadabra"
 * after mutate: Abracadabra
 * @endcode
 *
 * @see last_char_occurrance, get_string, string_lit_concat, string_string_concat,
 *      reserve_string, trim_string
 */
char* first_char_occurrance(string_t* str, char value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Find the **last** occurrence of a byte in a csalt string and return a
 *        writable pointer into the string's buffer.
 *
 * Searches the current payload of @p str (exactly `str->len` bytes; the
 * terminating NUL is not examined) for the last byte equal to @p value and
 * returns a *borrowed* pointer to that position on success. The pointer aliases
 * the object’s internal storage and remains valid until the string is
 * reallocated, trimmed, otherwise modified, or destroyed. Comparison is
 * performed as `(unsigned char)value`.
 *
 * **Thread-safety:** This function does not modify the character data, but it
 * may update `str->error` for status reporting. Treat it as **not thread-safe
 * for the same object** unless callers synchronize access to @p str (i.e., no
 * concurrent reads that also update `str->error`, and no concurrent writes or
 * frees). Concurrent calls on **distinct** objects are fine.
 *
 * **Time complexity:** O(n) in the string length. The implementation may use
 * SIMD-accelerated forward scans internally (tracking the last match per chunk)
 * when built with vector backends (e.g., SSE2/AVX2/AVX-512/NEON/SVE), but the
 * worst-case complexity is still linear.
 *
 * Error reporting:
 * - Returns `NULL` and sets `errno = EINVAL` if @p str is `NULL` or if the
 *   internal buffer is `NULL` (in the latter case, `str->error = ::NULL_POINTER`).
 * - Returns `NULL` when the byte is not found; in this case `errno` is set to ENOENT,
 *   and str->error is set to NOT_FOUND. On valid inputs, `str->error` may be set to ::NO_ERROR.
 *
 * @param str    Pointer to a ::string_t instance (must be initialized).
 * @param value  Byte to search for (compared as `(unsigned char)value`).
 *
 * @return Writable pointer to the last matching byte on success; `NULL` on
 *         failure or if no match exists.
 *
 * @warning The returned pointer aliases @p str’s internal storage. It becomes
 *          invalid after any operation that can move or modify the buffer
 *          (e.g., concatenation, reserve, trim, or destruction). Do **not**
 *          `free()` the returned pointer.
 *
 * @note To search for the terminator `'\0'`, include it explicitly in the
 *       search length; this function searches only `str->len` bytes.
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("Hello, World!");
 * if (!s) { perror("init_string"); return 1; }
 *
 * char* p = last_char_occurrance(s, 'l');
 * if (p) {
 *     const char* base = get_string(s);
 *     printf("last 'l' at %zu: \"%s\"\n", (size_t)(p - base), p);
 *     *p = 'L';  // mutate in place
 *     printf("after mutate: %s\n", get_string(s));
 * } else {
 *     puts("byte not found");
 * }
 *
 * free_string(s);
 * @endcode
 *
 * @par Output (example)
 * @code{.text}
 * last 'l' at 10: "ld!"
 * after mutate: Hello, WorLd!
 * @endcode
 *
 * @see first_char_occurrance, get_string, string_lit_concat, string_string_concat,
 *      reserve_string, trim_string
 */
char* last_char_occurrance(string_t* str, char value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Find the first occurrence of a C-string substring in a csalt string.
 *
 * Searches the payload of @p str (exactly `str->len` bytes; the terminating
 * NUL is not examined) for the first occurrence of @p sub_str and returns a
 * *borrowed* pointer into the object’s internal buffer at the match position.
 * The returned pointer remains valid until the string is reallocated, trimmed,
 * otherwise modified, or destroyed.
 *
 * When built with vector backends (e.g., SSE2/AVX/AVX2/AVX-512BW on x86, NEON
 * or SVE/SVE2 on ARM), the implementation may use SIMD to prefilter candidate
 * positions (by scanning for the first byte of @p sub_str) and then verify with
 * `memcmp`. This accelerates the common case while preserving correctness.
 *
 * **Thread-safety:** This function does not modify the character payload, but
 * it may update `str->error` for status reporting. Treat it as **not
 * thread-safe for the same object** unless callers synchronize access to @p str
 * (no concurrent calls that could update `str->error`, and no concurrent
 * writes/frees). Concurrent calls on **distinct** objects are safe.
 *
 * **Time complexity:** Worst case O(n) for single-character substrings and
 * O(n · α) for longer substrings, where `α` is the average fraction of
 * candidate positions that need verification. SIMD prefiltering typically
 * reduces `α` substantially on supported hardware, but the asymptotic bound
 * remains linear in the haystack length @p str->len.
 *
 * Error reporting:
 * - Returns `NULL` and sets `errno = EINVAL` if @p str is `NULL`, if
 *   `str->str` is `NULL` (also sets `str->error = ::NULL_POINTER`), or if
 *   @p sub_str is `NULL` (also sets `str->error = ::INVALID_ARG`).
 * - On a valid search (including “not found”), leaves `errno` unchanged and
 *   sets `str->error = ::NO_ERROR`.
 *
 * Matching policy:
 * - If `@p sub_str` is empty (`""`), this function returns `str->str`
 *   (match at offset 0), consistent with `strstr`.
 * - If the needle is longer than the haystack, returns `NULL`.
 *
 * @param str      Pointer to a ::string_t to search (must be initialized).
 * @param sub_str  NUL-terminated C string to find.
 *
 * @return Pointer to the first match inside @p str on success; `NULL` if no
 *         match exists or on invalid input (see errors above).
 *
 * @warning The returned pointer aliases @p str’s internal storage. It becomes
 *          invalid after any operation that can move or modify the buffer
 *          (e.g., concatenation, reserve, trim, or destruction). Do **not**
 *          `free()` the returned pointer.
 *
 * @note Only `str->len` bytes are searched; the terminator is not considered.
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("abracadabra");
 * if (!s) { perror("init_string"); return 1; }
 *
 * char* p = first_lit_substr_occurrence(s, "cad");
 * if (p) {
 *     const char* base = get_string(s);
 *     printf("found at offset %zu: \"%s\"\n", (size_t)(p - base), p);
 * } else {
 *     puts("not found");
 * }
 *
 * // Empty needle matches at start
 * char* p0 = first_lit_substr_occurrence(s, "");
 * printf("empty needle offset: %zu\n", (size_t)(p0 - get_string(s)));
 *
 * free_string(s);
 * @endcode
 *
 * @par Output (example)
 * @code{.text}
 * found at offset 4: "cadabra"
 * empty needle offset: 0
 * @endcode
 *
 * @see first_char_occurrance, last_char_occurrance, get_string,
 *      string_lit_concat, string_string_concat, reserve_string, trim_string
 */
char* first_lit_substr_occurrence(string_t* str, const char* sub_str);
// --------------------------------------------------------------------------------

/**
 * @brief Find the first occurrence of a csalt string (needle) inside another
 *        csalt string (haystack) and return a writable pointer into the
 *        haystack’s buffer.
 *
 * Searches the payload of @p hay (exactly `hay->len` bytes; the terminating
 * NUL is not examined) for the first occurrence of @p needle’s bytes and
 * returns a *borrowed* pointer into @p hay’s internal buffer at the match
 * position. The pointer remains valid until the haystack is reallocated,
 * trimmed, otherwise modified, or destroyed.
 *
 * When built with vector backends (e.g., SSE2/AVX/AVX2/AVX-512BW on x86,
 * NEON or SVE/SVE2 on ARM), the implementation may use SIMD to prefilter
 * candidate positions (scanning for the first byte of @p needle) and then
 * verify with `memcmp`. This accelerates the common case while preserving
 * correctness.
 *
 * **Thread-safety:** The function may update `hay->error` for status reporting
 * and returns a pointer aliasing `hay`’s internal storage. Treat it as **not
 * thread-safe for the same object** unless callers synchronize access (no
 * concurrent ops that might update `hay->error`, mutate, or free `hay`). Calls
 * on **distinct** objects are safe.
 *
 * **Time complexity:** Worst case O(n) for single-byte needles and O(n·α) for
 * longer needles, where `α` is the fraction of candidate positions that reach
 * full verification. SIMD prefiltering typically reduces `α`, but the
 * asymptotic bound remains linear in `hay->len`.
 *
 * Matching policy:
 * - If `needle->len == 0`, the function returns `hay->str` (match at offset 0),
 *   consistent with `strstr`.
 * - If `needle->len > hay->len`, the function returns `NULL`.
 *
 * Error reporting:
 * - Returns `NULL` and sets `errno = EINVAL` if @p hay is `NULL` or @p needle
 *   is `NULL` (also sets `hay->error = ::INVALID_ARG` when @p hay is non-NULL).
 * - Returns `NULL` and sets `errno = EINVAL` if `hay->str` is `NULL`
 *   (also sets `hay->error = ::NULL_POINTER`).
 * - Returns `NULL` and sets `errno = EINVAL` if `needle->str` is `NULL`
 *   (does not modify `hay->error`).
 * - On a valid search (including “not found”), leaves `errno` unchanged and
 *   sets `hay->error = ::NO_ERROR`.
 *
 * @param hay     Haystack ::string_t to search (must be initialized).
 * @param needle  Needle ::string_t to find (must be initialized).
 *
 * @return Pointer to the first match inside @p hay on success; `NULL` if no
 *         match exists or on invalid input (see errors above).
 *
 * @warning The returned pointer aliases @p hay’s internal storage. It becomes
 *          invalid after any operation that can move or modify the buffer
 *          (e.g., concatenation, reserve, trim, or destruction). Do **not**
 *          `free()` the returned pointer.
 *
 * @note Only `hay->len` bytes are searched; the terminator is not considered.
 *
 * @par Example
 * @code{.c}
 * string_t* h = init_string("abracadabra");
 * string_t* n = init_string("cad");
 * if (!h || !n) { perror("init_string"); free_string(h); free_string(n); return 1; }
 *
 * char* p = first_string_substr_occurrence(h, n);
 * if (p) {
 *     const char* base = get_string(h);
 *     printf("found at offset %zu: \"%s\"\n", (size_t)(p - base), p);
 * } else {
 *     puts("not found");
 * }
 *
 * // Empty needle matches at start
 * n->len = 0; n->str[0] = '\0';
 * char* p0 = first_string_substr_occurrence(h, n);
 * printf("empty needle offset: %zu\n", (size_t)(p0 - get_string(h)));
 *
 * free_string(h);
 * free_string(n);
 * @endcode
 *
 * @par Output (example)
 * @code{.text}
 * found at offset 4: "cadabra"
 * empty needle offset: 0
 * @endcode
 *
 * @see first_lit_substr_occurrence, first_char_occurrance, last_char_occurrance,
 *      get_string, reserve_string, trim_string
 */
char* first_string_substr_occurrence(string_t* hay, const string_t* needle);
// -------------------------------------------------------------------------------- 

/**
 * @def first_substr_occurrence
 * @brief Generic front-end that finds the first occurrence of a substring in a
 *        csalt string, dispatching to the appropriate implementation based on
 *        the needle’s type.
 *
 * **Dispatch rules**
 * - `const char*` / `char*`          ➜ ::first_lit_substr_occurrence(hay, lit)
 * - `const string_t*` / `string_t*`  ➜ ::first_string_substr_occurrence(hay, needle)
 *
 * The haystack argument (`str1`) must be a valid ::string_t*. The needle
 * (`str2`) may be either a NUL-terminated C string or another ::string_t.
 * For string literals (type `const char[N]`), wrap with ::CSALT_CSTR to
 * ensure array-to-pointer decay, e.g.:
 *
 * @code{.c}
 * char* p = first_substr_occurrence(hay, CSALT_CSTR("cad"));
 * @endcode
 *
 * **Thread-safety:** The selected function may update `hay->error` and returns
 * a pointer aliasing `hay`’s internal storage. Treat calls as **not
 * thread-safe for the same object** unless you synchronize access to @p hay.
 * Concurrent calls on distinct objects are safe.
 *
 * **Time complexity:** Worst-case linear in the haystack length
 * (O(n) for 1-byte needles; O(n·α) for longer needles, where α is the fraction
 * of candidate positions that require full verification). Builds targeting
 * SIMD ISAs (e.g., SSE2/AVX/AVX2/AVX-512BW, NEON, SVE/SVE2) may prefilter
 * candidates vectorially but the asymptotic bound remains linear.
 *
 * **Matching policy**
 * - Empty needle matches at offset 0 (like `strstr`).
 * - If the needle is longer than the haystack, no match is reported.
 * - Only `hay->len` bytes are searched; the terminating NUL is not examined.
 *
 * **Return value**
 * - On success: a *borrowed* writable pointer into `hay->str` at the first
 *   match position.
 * - On “not found”: `NULL` (with `errno` unchanged and typically `hay->error = ::NO_ERROR`).
 * - On invalid input: `NULL` and `errno = EINVAL`. When @p hay is non-NULL,
 *   its error code may be set (e.g., ::INVALID_ARG, ::NULL_POINTER).
 *
 * @warning The returned pointer aliases `hay`’s internal buffer and becomes
 * invalid after any operation that may move or modify the buffer (concat,
 * reserve, trim, destroy). Do **not** `free()` the returned pointer.
 *
 * **Compile-time checks**
 * - If `str2`’s type is unsupported, the macro resolves to
 *   `first_substr__type_mismatch`, which is annotated to produce a compile-time
 *   diagnostic on GCC/Clang (and a strong deprecation on MSVC).
 * - For documentation builds, defining `DOXYGEN` ensures this macro is always
 *   visible to the indexer.
 *
 * @param str1  Haystack ::string_t* to search.
 * @param str2  Needle as either `const char*`/`char*` or `const string_t*`/`string_t*`.
 *
 * @see first_lit_substr_occurrence, first_string_substr_occurrence,
 *      first_char_occurrance, last_char_occurrance, CSALT_CSTR
 *
 * @par Example (C string needle)
 * @code{.c}
 * string_t* h = init_string("abracadabra");
 * if (!h) { perror("init_string"); return 1; }
 * char* p = first_substr_occurrence(h, CSALT_CSTR("cad"));
 * if (p) printf("found at %zu: \"%s\"\n", (size_t)(p - get_string(h)), p);
 * free_string(h);
 * @endcode
 *
 * @par Example (csalt string needle)
 * @code{.c}
 * string_t* h = init_string("mississippi");
 * string_t* n = init_string("issi");
 * if (!h || !n) { perror("init_string"); free_string(h); free_string(n); return 1; }
 * char* p = first_substr_occurrence(h, n);
 * if (p) printf("found at %zu\n", (size_t)(p - get_string(h)));
 * free_string(h); free_string(n);
 * @endcode
 */
#if defined(DOXYGEN)
/* Make Doxygen always see the macro symbol */
#  define first_substr_occurrence(str1, str2) first_string_substr_occurrence((str1), (str2))
#elif __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_GENERIC__)
#  define first_substr_occurrence(str1, str2) \
     _Generic((str2), \
        const char*:      first_lit_substr_occurrence,    \
        char*:            first_lit_substr_occurrence,    \
        const string_t*:  first_string_substr_occurrence, \
        string_t*:        first_string_substr_occurrence, \
        void*:            first_string_substr_occurrence, /* allow NULL → runtime check */ \
        default:          first_substr__type_mismatch     \
     )((str1), (str2))
char* first_substr__type_mismatch(string_t*, void*);
#else
/* Pre-C11 fallback: only the string_t path is supported via explicit call */
#  define first_substr_occurrence(str1, str2) first_string_substr_occurrence((str1), (str2))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief Find the **last** occurrence of a C-string substring in a csalt string.
 *
 * Searches the payload of @p str (exactly `str->len` bytes; the terminating
 * NUL is not examined) for the last occurrence of @p sub_str and returns a
 * *borrowed* pointer into the object’s internal buffer at the match position.
 * The pointer remains valid until the string is reallocated, trimmed, otherwise
 * modified, or destroyed.
 *
 * When built with vector backends (e.g., SSE2/AVX/AVX2/AVX-512BW on x86, NEON
 * or SVE/SVE2 on ARM), the implementation may use a SIMD prefilter (scan for
 * the first byte of @p sub_str across wide chunks) and then verify candidates
 * with `memcmp`, keeping the rightmost match. This typically speeds up common
 * cases while preserving correctness.
 *
 * **Thread-safety:** This function may update `str->error` for status
 * reporting and returns a pointer aliasing `str`’s internal storage. Treat it
 * as **not thread-safe for the same object** unless callers synchronize access
 * (no concurrent updates to `str->error`, and no concurrent writes/frees).
 * Calls on **distinct** objects are safe.
 *
 * **Time complexity:** Worst case O(n) for a 1-byte needle and O(n·α) for
 * longer needles, where `α` is the fraction of candidate positions requiring
 * full verification. SIMD prefiltering typically reduces `α`, but the
 * asymptotic bound remains linear in `str->len`.
 *
 * **Matching policy:**
 * - If `@p sub_str` is empty (`""`), this function returns `str->str + str->len`
 *   (match at end).
 * - If the needle is longer than the haystack, returns `NULL`.
 * - Only `str->len` bytes are searched; the terminator is not considered.
 *
 * **Error reporting:**
 * - Returns `NULL` and sets `errno = EINVAL` if @p str is `NULL`, if
 *   `str->str` is `NULL` (also sets `str->error = ::NULL_POINTER`), or if
 *   @p sub_str is `NULL` (also sets `str->error = ::INVALID_ARG`).
 * - On a valid search (including “not found”), leaves `errno` unchanged and
 *   sets `str->error = ::NO_ERROR`.
 *
 * @param str      Haystack ::string_t to search (must be initialized).
 * @param sub_str  NUL-terminated C string to find (needle).
 *
 * @return Pointer to the last match inside @p str on success; `NULL` if no
 *         match exists or on invalid input (see above).
 *
 * @warning The returned pointer aliases @p str’s internal buffer and becomes
 *          invalid after any operation that can move or modify the buffer
 *          (e.g., concatenation, reserve, trim, destruction). Do **not**
 *          `free()` the returned pointer.
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("abracadabra");
 * if (!s) { perror("init_string"); return 1; }
 *
 * const char* needle = "abra";
 * char* p = last_lit_substr_occurrence(s, needle);
 * if (p) {
 *     const char* base = get_string(s);
 *     printf("last \"%s\" at offset %zu: \"%s\"\n", needle, (size_t)(p - base), p);
 * } else {
 *     puts("not found");
 * }
 *
 * // Empty needle matches at end
 * char* pend = last_lit_substr_occurrence(s, "");
 * printf("empty needle offset: %zu\n", (size_t)(pend - get_string(s)));
 *
 * free_string(s);
 * @endcode
 *
 * @par Output (example)
 * @code{.text}
 * last "abra" at offset 7: "abra"
 * empty needle offset: 11
 * @endcode
 *
 * @see first_lit_substr_occurrence, first_string_substr_occurrence,
 *      last_string_substr_occurrence, get_string, reserve_string, trim_string
 */
char* last_lit_substr_occurrence(string_t* str, const char* sub_str);
// --------------------------------------------------------------------------------

/**
 * @brief Find the **last** occurrence of a csalt string (needle) inside another
 *        csalt string (haystack) and return a writable pointer into the
 *        haystack’s buffer.
 *
 * Searches the payload of @p hay (exactly `hay->len` bytes; the terminating
 * NUL is not examined) for the last occurrence of @p needle’s bytes and returns
 * a *borrowed* pointer into @p hay’s internal buffer at the match position. The
 * pointer remains valid until the haystack is reallocated, trimmed, otherwise
 * modified, or destroyed.
 *
 * When built with vector backends (e.g., SSE2/AVX/AVX-512BW on x86, NEON or
 * SVE/SVE2 on ARM), the implementation may employ SIMD prefiltering to locate
 * candidate positions (by scanning for the first byte of @p needle) and then
 * verify candidates with `memcmp`, tracking the rightmost match.
 *
 * **Thread-safety:** The function may update `hay->error` for status reporting
 * and returns a pointer aliasing `hay`’s internal storage. Treat it as **not
 * thread-safe for the same object** unless callers synchronize access (no
 * concurrent updates to `hay->error`, and no concurrent writes/frees). Calls on
 * **distinct** objects are safe.
 *
 * **Time complexity:** Worst case O(n) for 1-byte needles and O(n·α) for longer
 * needles, with `α` the fraction of candidates that require full verification.
 * SIMD filtering typically reduces `α`, but the bound remains linear in
 * `hay->len`.
 *
 * **Matching policy:**
 * - If `needle->len == 0`, this function returns `hay->str + hay->len`
 *   (match at end).
 * - If `needle->len > hay->len`, the function returns `NULL`.
 * - Only `hay->len` bytes are searched; the terminator is not considered.
 *
 * **Error reporting:**
 * - Returns `NULL` and sets `errno = EINVAL` if @p hay is `NULL` or @p needle
 *   is `NULL` (also sets `hay->error = ::INVALID_ARG` when @p hay is non-NULL).
 * - Returns `NULL` and sets `errno = EINVAL` if `hay->str` is `NULL`
 *   (also sets `hay->error = ::NULL_POINTER`).
 * - Returns `NULL` and sets `errno = EINVAL` if `needle->str` is `NULL`
 *   (does not modify `hay->error`).
 * - On a valid search (including “not found”), leaves `errno` unchanged and
 *   sets `hay->error = ::NO_ERROR`.
 *
 * @param hay     Haystack ::string_t to search (must be initialized).
 * @param needle  Needle ::string_t to find (must be initialized).
 *
 * @return Pointer to the last match inside @p hay on success; `NULL` if no
 *         match exists or on invalid input (see above).
 *
 * @warning The returned pointer aliases @p hay’s internal buffer and becomes
 *          invalid after any operation that can move or modify the buffer
 *          (e.g., concatenation, reserve, trim, destruction). Do **not**
 *          `free()` the returned pointer.
 *
 * @note Only the payload is searched (`hay->len` bytes); the NUL terminator is
 *       not examined.
 *
 * @par Example
 * @code{.c}
 * string_t* h = init_string("mississippi");
 * string_t* n = init_string("iss");
 * if (!h || !n) { perror("init_string"); free_string(h); free_string(n); return 1; }
 *
 * char* p = last_string_substr_occurrence(h, n);
 * if (p) {
 *     const char* base = get_string(h);
 *     printf("last \"%s\" at offset %zu: \"%s\"\n", get_string(n), (size_t)(p - base), p);
 * } else {
 *     puts("not found");
 * }
 *
 * // Empty needle matches at end
 * n->len = 0; n->str[0] = '\0';
 * char* pend = last_string_substr_occurrence(h, n);
 * printf("empty needle offset: %zu\n", (size_t)(pend - get_string(h)));
 *
 * free_string(h);
 * free_string(n);
 * @endcode
 *
 * @par Output (example)
 * @code{.text}
 * last "iss" at offset 4: "issippi"
 * empty needle offset: 11
 * @endcode
 *
 * @see last_lit_substr_occurrence, first_string_substr_occurrence,
 *      first_lit_substr_occurrence, get_string, reserve_string, trim_string
 */
char* last_string_substr_occurrence(string_t* hay, const string_t* needle);
// -------------------------------------------------------------------------------- 

/**
 * @def last_substr_occurrence
 * @brief Generic front-end that finds the **last** occurrence of a substring in
 *        a csalt string, dispatching to the correct implementation based on the
 *        needle’s type.
 *
 * **Dispatch rules**
 * - `const char*` / `char*`
 *     ➜ ::last_lit_substr_occurrence(@p str1, @p str2)
 * - `const string_t*` / `string_t*`
 *     ➜ ::last_string_substr_occurrence(@p str1, @p str2)
 *
 * The haystack argument (`str1`) must be a valid ::string_t*. The needle
 * (`str2`) may be either a NUL-terminated C string or another ::string_t.
 * For string literals (type `const char[N]`), wrap with a cast/helper to ensure
 * array-to-pointer decay, e.g.:
 *
 * @code{.c}
 * char* p = last_substr_occurrence(hay, (const char*)"abra");           // or CSALT_CSTR("abra")
 * @endcode
 *
 * **Thread-safety:** The selected function may update `str1->error` and returns
 * a pointer that aliases `str1`’s internal storage. Treat calls as **not
 * thread-safe for the same object** unless you synchronize access to @p str1.
 * Concurrent calls on **distinct** objects are safe.
 *
 * **Time complexity:** Worst-case linear in the haystack length. Implementations
 * may use SIMD prefiltering on supported ISAs (e.g., SSE2/AVX/AVX2/AVX-512BW,
 * NEON, SVE/SVE2) but the asymptotic bound remains O(n).
 *
 * **Matching policy**
 * - Empty needle matches at the end: returns `str1->str + str1->len`.
 * - If the needle is longer than the haystack, no match is reported.
 * - Only the payload is searched (`str1->len` bytes); the NUL terminator is not examined.
 *
 * **Return value**
 * - On success: a *borrowed* writable pointer into `str1->str` at the **last**
 *   match position.
 * - On “not found”: `NULL` (with `errno` unchanged and typically `str1->error = ::NO_ERROR`).
 * - On invalid input: `NULL` and `errno = EINVAL`. When @p str1 is non-NULL,
 *   its error code may be set (e.g., ::INVALID_ARG, ::NULL_POINTER).
 *
 * @warning The returned pointer aliases `str1`’s internal buffer and becomes
 * invalid after any operation that can move or modify the buffer (concatenate,
 * reserve, trim, destroy). Do **not** `free()` the returned pointer.
 *
 * **Compile-time checks**
 * - If `str2`’s type is unsupported, the macro resolves to
 *   `last_substr__type_mismatch`, which is annotated to produce a compile-time
 *   diagnostic on GCC/Clang (and a strong deprecation on MSVC).
 * - For documentation builds, defining `DOXYGEN` ensures this macro is indexed.
 *
 * @param str1  Haystack ::string_t* to search.
 * @param str2  Needle as either `const char*`/`char*` or `const string_t*`/`string_t*`.
 *
 * @see last_lit_substr_occurrence, last_string_substr_occurrence,
 *      first_substr_occurrence, first_lit_substr_occurrence, first_string_substr_occurrence
 *
 * @par Example (C string needle)
 * @code{.c}
 * string_t* s = init_string("abracadabra");
 * if (!s) { perror("init_string"); return 1; }
 * char* p = last_substr_occurrence(s, (const char*)"abra");
 * if (p) printf("last \"abra\" at %zu\n", (size_t)(p - get_string(s)));
 * free_string(s);
 * @endcode
 *
 * @par Example (csalt string needle)
 * @code{.c}
 * string_t* h = init_string("mississippi");
 * string_t* n = init_string("iss");
 * if (!h || !n) { perror("init_string"); free_string(h); free_string(n); return 1; }
 * char* p = last_substr_occurrence(h, n);
 * if (p) printf("last \"%s\" at %zu\n", get_string(n), (size_t)(p - get_string(h)));
 * free_string(h); free_string(n);
 * @endcode
 */

#if defined(DOXYGEN)
#  define last_substr_occurrence(str1, str2) last_string_substr_occurrence((str1), (str2))
#elif __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_GENERIC__)
#  define last_substr_occurrence(str1, str2) \
     _Generic((str2), \
        const char*:      last_lit_substr_occurrence,    \
        char*:            last_lit_substr_occurrence,    \
        const string_t*:  last_string_substr_occurrence, \
        string_t*:        last_string_substr_occurrence, \
        void*:            last_string_substr_occurrence, /* allow NULL → runtime check */ \
        default:          last_substr__type_mismatch     \
     )((str1), (str2))
char* last_substr__type_mismatch(string_t*, void*);
#else
/* Pre-C11 fallback */
#  define last_substr_occurrence(str1, str2) last_string_substr_occurrence((str1), (str2))
#endif
// --------------------------------------------------------------------------------

/**
 * @brief Return a writable pointer to the **first byte** of a csalt string.
 *
 * Provides a *borrowed* pointer to the beginning of @p str’s internal buffer.
 * This is useful for iterator-style traversal: the corresponding one-past-last
 * pointer is `first_char(str) + string_size(str)`. The pointer remains valid
 * until the string is reallocated, trimmed, otherwise modified, or destroyed.
 *
 * **Thread-safety:** This function updates `str->error` and returns a pointer
 * aliasing @p str’s internal storage. Treat it as **not thread-safe for the
 * same object** unless you synchronize access to @p str. Calls on distinct
 * objects are safe.
 *
 * **Notes:**
 * - For an empty string (`string_size(str) == 0`), the returned pointer points
 *   to the terminating NUL (`'\0'`). That is a valid address for read/write
 *   of the terminator, but there is no payload to iterate.
 *
 * **Error reporting:**
 * - Returns `NULL` and sets `errno = EINVAL` if @p str is `NULL` or if the
 *   internal buffer is `NULL` (also sets `str->error = ::NULL_POINTER`).
 * - On success, sets `str->error = ::NO_ERROR`.
 *
 * @param str  Target ::string_t (must be initialized).
 * @return Pointer to the first byte on success; `NULL` on invalid input.
 *
 * @warning The returned pointer becomes invalid after any operation that may
 *          move or modify the buffer (e.g., concatenation, reserve, trim, or
 *          destruction). Do **not** `free()` the returned pointer.
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("iterate");
 * if (!s) { perror("init_string"); return 1; }
 *
 * char* begin = first_char(s);
 * char* end   = begin + string_size(s);   // one-past-last
 * for (char* it = begin; it < end; ++it) putchar(*it);
 * putchar('\n');
 *
 * free_string(s);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * iterate
 * @endcode
 */
char* first_char(string_t* str);
// --------------------------------------------------------------------------------

/**
 * @brief Return a writable pointer to the **last byte** of a csalt string.
 *
 * Provides a *borrowed* pointer to the final character in @p str’s payload
 * (i.e., `first_char(str) + string_size(str) - 1`). The pointer remains valid
 * until the string is reallocated, trimmed, otherwise modified, or destroyed.
 *
 * **Thread-safety:** This function updates `str->error` and returns a pointer
 * aliasing @p str’s internal storage. Treat it as **not thread-safe for the
 * same object** unless you synchronize access to @p str. Calls on distinct
 * objects are safe.
 *
 * **Precondition (important):**
 * - The string must be **non-empty** (`string_size(str) > 0`). Calling this
 *   function on an empty string yields an invalid address (one byte *before*
 *   the buffer) and is undefined behavior. Check the size first.
 *
 * **Error reporting:**
 * - Returns `NULL` and sets `errno = EINVAL` if @p str is `NULL` or if the
 *   internal buffer is `NULL` (also sets `str->error = ::NULL_POINTER`).
 * - On success, sets `str->error = ::NO_ERROR`.
 *
 * @param str  Target ::string_t (must be initialized and non-empty).
 * @return Pointer to the last byte on success; `NULL` on invalid input.
 *
 * @warning The returned pointer becomes invalid after any operation that may
 *          move or modify the buffer (e.g., concatenation, reserve, trim, or
 *          destruction). Do **not** `free()` the returned pointer.
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("xyz");
 * if (!s) { perror("init_string"); return 1; }
 *
 * if (string_size(s) > 0) {
 *     char* b = first_char(s);
 *     char* e = last_char(s);
 *     printf("first='%c', last='%c'\n", *b, *e);
 * }
 *
 * free_string(s);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * first='x', last='z'
 * @endcode
 */
char* last_char(string_t* str);
// --------------------------------------------------------------------------------

/**
 * @brief Test whether a pointer lies inside a csalt string’s buffer.
 *
 * Determines if @p ptr points into @p str’s storage. By default this checks
 * the **payload region** only (the first @c str->len bytes). When
 * @p include_terminator is @c true, the check also accepts a pointer to the
 * trailing NUL, i.e., the range becomes @c [str->str, str->str + str->len + 1).
 *
 * This function does not modify the character data. It may update
 * @c str->error for status reporting.
 *
 * **Thread-safety:** Not thread-safe for the same object because it writes
 * @c str->error. Calls on distinct objects are safe.
 *
 * **Time complexity:** @c O(1).
 *
 * **Edge cases:**
 * - For an **empty** string (`str->len == 0`), the payload range is empty.
 *   With @p include_terminator = @c false, only @c false can be returned.
 *   With @p include_terminator = @c true, a pointer equal to @c str->str
 *   (the NUL terminator) is accepted.
 *
 * **Error reporting:**
 * - Returns @c false and sets @c errno = @c EINVAL if @p str is @c NULL,
 *   if @p ptr is @c NULL (also sets @c str->error = ::INVALID_ARG when
 *   @p str is non-NULL), or if @c str->str is @c NULL (also sets
 *   @c str->error = ::NULL_POINTER).
 * - On a valid check, leaves @c errno unchanged and sets
 *   @c str->error = ::NO_ERROR.
 *
 * @param str                 Target ::string_t (must be initialized).
 * @param ptr                 Pointer to test (may alias @c str->str or be
 *                            unrelated).
 * @param include_terminator  If @c true, accept the pointer to the trailing
 *                            NUL as “inside” the buffer; if @c false, only
 *                            the payload bytes are considered.
 *
 * @return @c true if @p ptr lies in the selected range of @p str’s buffer;
 *         @c false otherwise (or on invalid input; see errors above).
 *
 * @warning The returned boolean reflects the buffer layout at the time of
 *          the call. Any operation that can reallocate or modify the buffer
 *          (concatenate, reserve, trim, destroy) may invalidate prior
 *          pointers and change subsequent results.
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("abc");
 * if (!s) { perror("init_string"); return 1; }
 *
 * const char* base = get_string(s);
 * const char* p0   = base;          // 'a'
 * const char* p1   = base + 3;      // points to terminating '\0'
 * const char* px   = base + 10;     // clearly out of range
 *
 * printf("p0 in payload? %s\n", is_string_ptr(s, p0, false) ? "yes" : "no");
 * printf("p1 in payload? %s\n", is_string_ptr(s, p1, false) ? "yes" : "no");
 * printf("p1 in buffer ? %s\n", is_string_ptr(s, p1, true ) ? "yes" : "no");
 * printf("px in buffer ? %s\n", is_string_ptr(s, px, true ) ? "yes" : "no");
 *
 * free_string(s);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * p0 in payload? yes
 * p1 in payload? no
 * p1 in buffer ? yes
 * px in buffer ? no
 * @endcode
 */
bool is_string_ptr(string_t* str, const char* ptr, bool include_terminator);
// --------------------------------------------------------------------------------

/**
 * @brief Remove all occurrences of a literal C substring within a bounded window,
 *        compacting the string in-place.
 *
 * Repeatedly locates the **rightmost** occurrence of @p substring inside the
 * inclusive window `[min_ptr, max_ptr]` of @p string, removes it, and shifts
 * the remainder of the buffer (including the terminating NUL) left with
 * `memmove`. After each removal:
 * - `string->len` is decremented by the number of bytes removed, and
 * - the `'\0'` terminator is moved left accordingly (because the entire suffix,
 *   including the terminator, is moved).
 *
 * As a convenience, if a removed match is immediately followed by a single
 * space that is still inside the payload, that space is also removed. The
 * window’s right edge is internally slid left to remain aligned with the
 * now-shorter buffer. The function stops when no further matches exist within
 * the window or the remaining window can no longer accommodate the needle.
 *
 * When compiled with supported ISAs, the search phase may be SIMD-accelerated
 * (SSE2/AVX/AVX2/AVX-512BW, NEON, SVE/SVE2). The erase phase uses `memmove`,
 * which is typically optimized by libc.
 *
 * **Thread-safety:** Not thread-safe for the **same** object. The function
 * mutates `string->str`, `string->len`, and `string->error`. Calls on distinct
 * objects are safe.
 *
 * **Time complexity:** Let `W = max_ptr - min_ptr + 1`, and let `R` be the
 * number of removals. Search is ~O(W) in total (often faster with SIMD).
 * Each removal performs one `memmove` proportional to the remaining suffix,
 * so worst-case is O(W·R). In practice, `R` is small for typical inputs.
 *
 * **Error reporting:**
 * - Returns `false`, sets `errno = EINVAL` if any argument is invalid
 *   (`string == NULL`, `substring == NULL`, `string->str == NULL`,
 *   or `max_ptr < min_ptr`). Also sets `string->error` to ::INVALID_ARG
 *   or ::NULL_POINTER as appropriate.
 * - Returns `false`, sets `errno = ERANGE` if `min_ptr` or `max_ptr` do not
 *   lie within the **payload** of `string` (checked via `is_string_ptr(..., false)`).
 * - Returns `true` on success, even if no occurrences were found/removed.
 *   On any successful exit, `string->error = ::NO_ERROR`.
 *
 * **Notes:**
 * - An empty needle (`substring[0] == '\0'`) is a no-op and returns `true`.
 * - Capacity is not reduced; call ::trim_string if you wish to shrink `alloc`.
 * - Any previously saved pointers into `string->str` may no longer point to the
 *   same characters after removals, since content is shifted.
 *
 * @param string     Haystack ::string_t to modify (must be initialized).
 * @param substring  NUL-terminated C string to remove (needle).
 * @param min_ptr    Inclusive lower bound pointer into `string->str` payload.
 * @param max_ptr    Inclusive upper bound pointer into `string->str` payload.
 *
 * @return `true` if the operation completed (even with zero removals);
 *         `false` on invalid input or range errors (see above).
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("foo bar foo baz");
 * if (!s) { perror("init_string"); return 1; }
 * char* lo = first_char(s);
 * char* hi = last_char(s);
 *
 * if (!drop_lit_substr(s, "foo", lo, hi)) {
 *     perror("drop_lit_substr");
 *     fprintf(stderr, "err: %s\n", error_to_string(get_string_error(s)));
 *     free_string(s);
 *     return 1;
 * }
 * printf("Result: \"%s\" (len=%zu)\n", get_string(s), string_size(s));
 * free_string(s);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * Result: "bar baz" (len=7)
 * @endcode
 *
 * @see drop_string_substr, is_string_ptr, first_char, last_char, trim_string
 */
bool drop_lit_substr(string_t* string, const char* substring,
                     char* min_ptr, char* max_ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Remove all occurrences of a csalt string needle within a bounded window
 *        of another csalt string, compacting the haystack in-place.
 *
 * Repeatedly locates the **rightmost** occurrence of @p substring’s payload
 * inside the inclusive window `[min_ptr, max_ptr]` of @p string, removes it,
 * and shifts the remainder of the buffer (including the terminating NUL) left
 * with `memmove`. After each removal:
 * - `string->len` is decremented by the number of bytes removed, and
 * - the `'\0'` terminator is moved left accordingly.
 *
 * If a removed match is immediately followed by a single space still inside
 * the payload, that space is also removed. The window’s right edge is updated
 * internally to track the shorter buffer. The function stops when no further
 * matches exist within the window or the remaining window cannot accommodate
 * the needle.
 *
 * The search phase may be SIMD-accelerated on supported ISAs (SSE2/AVX/AVX2/AVX-512BW,
 * NEON, SVE/SVE2). The erase phase uses `memmove`.
 *
 * **Thread-safety:** Not thread-safe for the **same** object. The function
 * mutates `string->str`, `string->len`, and `string->error`. Calls on distinct
 * objects are safe. The needle object @p substring is not modified.
 *
 * **Time complexity:** Let `W = max_ptr - min_ptr + 1`, and let `R` be the
 * number of removals. Search is ~O(W) in total (often faster with SIMD).
 * Each removal uses one `memmove` of the remaining suffix, yielding a worst case
 * around O(W·R).
 *
 * **Error reporting:**
 * - Returns `false`, sets `errno = EINVAL` if any argument is invalid
 *   (`string == NULL`, `substring == NULL`, `string->str == NULL`,
 *   `substring->str == NULL`, or `max_ptr < min_ptr`). Also sets
 *   `string->error` to ::INVALID_ARG or ::NULL_POINTER as appropriate.
 * - Returns `false`, sets `errno = ERANGE` if `min_ptr` or `max_ptr` do not
 *   lie within the **payload** of `string` (checked via `is_string_ptr(..., false)`).
 * - Returns `true` on success, even if no occurrences were found/removed.
 *   On any successful exit, `string->error = ::NO_ERROR`.
 *
 * **Notes:**
 * - An empty needle (`substring->len == 0`) is a no-op and returns `true`.
 * - Capacity is not reduced; call ::trim_string to shrink `alloc` if desired.
 * - Previously saved pointers into `string->str` may no longer refer to the
 *   same characters after removals, since content is shifted.
 *
 * @param string     Haystack ::string_t to modify (must be initialized).
 * @param substring  Needle ::string_t whose payload will be removed where found
 *                   (must be initialized; not modified).
 * @param min_ptr    Inclusive lower bound pointer into `string->str` payload.
 * @param max_ptr    Inclusive upper bound pointer into `string->str` payload.
 *
 * @return `true` if the operation completed (even with zero removals);
 *         `false` on invalid input or range errors (see above).
 *
 * @par Example
 * @code{.c}
 * string_t* hay = init_string("alpha beta alpha gamma alpha");
 * string_t* ndl = init_string("alpha");
 * if (!hay || !ndl) { perror("init_string"); free_string(hay); free_string(ndl); return 1; }
 *
 * char* lo = first_char(hay);
 * char* hi = last_char(hay);
 *
 * if (!drop_string_substr(hay, ndl, lo, hi)) {
 *     perror("drop_string_substr");
 *     fprintf(stderr, "err: %s\n", error_to_string(get_string_error(hay)));
 *     free_string(hay); free_string(ndl); return 1;
 * }
 * printf("Result: \"%s\" (len=%zu)\n", get_string(hay), string_size(hay));
 *
 * free_string(hay);
 * free_string(ndl);
 * @endcode
 *
 * @par Output (example)
 * @code{.text}
 * Result: "beta gamma" (len=10)
 * @endcode
 *
 * @see drop_lit_substr, is_string_ptr, first_char, last_char, trim_string
 */
bool drop_string_substr(string_t* string, const string_t* substring, char* min_ptr,
                        char* max_ptr);
// -------------------------------------------------------------------------------- 

/**
 * @def drop_substr
 * @brief Generic front-end that removes all occurrences of a substring within a
 *        bounded window of a csalt string, dispatching by needle type.
 *
 * **Dispatch rules**
 * - `const char*` / `char*` / (`const unsigned char*` / `unsigned char*`)
 *     ➜ ::drop_lit_substr(@p string, @p substr, @p min_ptr, @p max_ptr)
 * - `const string_t*` / `string_t*`
 *     ➜ ::drop_string_substr(@p string, @p substr, @p min_ptr, @p max_ptr)
 *
 * The window is **inclusive**: the function removes matches that start in
 * `[min_ptr, max_ptr]`. For each removal, the remainder of the buffer
 * (including the terminating NUL) is shifted left with `memmove`, and
 * `string->len` is decreased accordingly. If a removed match is immediately
 * followed by a single space still inside the payload, that space is also
 * removed. The macro repeats removal from the **rightmost** match inside the
 * window until no more matches fit.
 *
 * **Thread-safety:** This macro mutates the target string (content, `len`,
 * and `error`) through the selected function. Treat calls as **not thread-safe
 * for the same object** unless you synchronize access. Calls on distinct
 * objects are safe.
 *
 * **Time complexity:** Let `W = max_ptr - min_ptr + 1`, and let `R` be the
 * number of removals. The search phase is ~O(W) in total (often faster on
 * machines with SIMD), and each removal performs one `memmove` proportional to
 * the remaining suffix, yielding a worst case around O(W · R).
 *
 * **Error reporting:** The selected function returns `bool`.
 * - On invalid arguments (nulls, `max_ptr < min_ptr`), returns `false`,
 *   sets `errno = EINVAL`, and sets `string->error` to ::INVALID_ARG or
 *   ::NULL_POINTER as appropriate.
 * - If `min_ptr` or `max_ptr` do not lie inside the payload of @p string,
 *   returns `false`, sets `errno = ERANGE`, and sets `string->error` =
 *   ::OUT_OF_BOUNDS.
 * - Returns `true` on success, even when zero occurrences were removed; on any
 *   successful exit, `string->error = ::NO_ERROR`.
 *
 * **Notes**
 * - An empty needle is a no-op and yields `true`.
 * - Capacity is not reduced; call ::trim_string if you want to shrink `alloc`.
 * - Any saved pointers into the buffer may become invalid as content moves.
 * - The macro forces array-to-pointer decay for string literals; you may still
 *   cast explicitly, e.g., `(const char*)"foo"`, if you prefer.
 *
 * **Compile-time checks**
 * - If @p substr has an unsupported type, the macro resolves to an internal
 *   `drop_substr__type_mismatch` symbol that is annotated to produce a
 *   compile-time diagnostic on GCC/Clang (strong deprecation on MSVC).
 *
 * @param string   Haystack ::string_t* to modify.
 * @param substr   Needle as either a C string pointer or a ::string_t*.
 * @param min_ptr  Inclusive lower-bound pointer into `string->str` (payload).
 * @param max_ptr  Inclusive upper-bound pointer into `string->str` (payload).
 *
 * @return `bool` — `true` if the operation completed; `false` on invalid input
 *         or range errors (see above).
 *
 * @par Example (C-string needle)
 * @code{.c}
 * string_t* s = init_string("foo bar foo baz");
 * char* lo = first_char(s);
 * char* hi = last_char(s);
 * if (!drop_substr(s, (const char*)"foo", lo, hi)) {
 *     perror("drop_substr");
 *     fprintf(stderr, "%s\n", error_to_string(get_string_error(s)));
 * }
 * printf("Result: \"%s\" (len=%zu)\n", get_string(s), string_size(s));
 * free_string(s);
 * @endcode
 * @par Output
 * @code{.text}
 * Result: "bar baz" (len=7)
 * @endcode
 *
 * @par Example (csalt string needle)
 * @code{.c}
 * string_t* h = init_string("alpha beta alpha gamma alpha");
 * string_t* n = init_string("alpha");
 * char* lo = first_char(h);
 * char* hi = last_char(h);
 * (void)drop_substr(h, n, lo, hi);
 * printf("Result: \"%s\"\n", get_string(h));
 * free_string(h); free_string(n);
 * @endcode
 * @par Output
 * @code{.text}
 * Result: "beta gamma"
 * @endcode
 *
 * @see drop_lit_substr, drop_string_substr, is_string_ptr, first_char, last_char, trim_string
 */

#if defined(DOXYGEN)
#  define drop_substr(string, substr, min_ptr, max_ptr) \
     drop_string_substr((string), (substr), (min_ptr), (max_ptr))
#elif __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_GENERIC__)
#  define drop_substr(string, substr, min_ptr, max_ptr)                                      \
     ( _Generic(((substr)),        /* +0 forces array-to-pointer decay for literals */   \
         const char*:          drop_lit_substr,                                              \
         char*:                drop_lit_substr,                                              \
         const unsigned char*: drop_lit_substr,                                              \
         unsigned char*:       drop_lit_substr,                                              \
         const string_t*:      drop_string_substr,                                           \
         string_t*:            drop_string_substr,                                           \
         void*:                drop_lit_substr,   /* tolerate NULL as (void*)0 */            \
         default:              drop_substr__type_mismatch                                    \
       )((string), (substr), (min_ptr), (max_ptr)) )
bool drop_substr__type_mismatch(string_t*, void*, char*, char*);
#else
#  define drop_substr(string, substr, min_ptr, max_ptr) \
     drop_string_substr((string), (substr), (min_ptr), (max_ptr))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief Replace all occurrences of a **literal C string** needle within a bounded
 *        window of a csalt string, compacting/growing the buffer in-place.
 *
 * Searches the inclusive window `[min_ptr, max_ptr]` of @p string for the **rightmost**
 * occurrence of the C-string @p pattern, replaces it with the C-string @p repl, and
 * repeats right-to-left until no further matches remain in the window. The suffix
 * (including the terminating NUL) is moved with `memmove` and `string->len` is updated
 * after each replacement.
 *
 * The search may be SIMD-accelerated on supported ISAs via the project dispatcher.
 * When the replacement text aliases @p string’s buffer (e.g., `repl` points inside
 * `string->str`), the implementation stashes a temporary copy to avoid overlap hazards.
 * If the replacement is longer than the pattern, the function first **counts matches**
 * (bounded by the window) and performs at most one `realloc` to grow capacity.
 *
 * Thread-safety: not thread-safe for the **same** object; the function mutates
 * `string->str`, `string->len`, and `string->error`. Calls on distinct objects are safe.
 *
 * Time complexity: With window width `W` and `R` replacements, total search ~O(W),
 * and each replacement uses one `memmove` of the remaining suffix → worst case O(W·R).
 *
 * Error reporting:
 * - Returns `false`, sets `errno = EINVAL` if any argument is invalid (`string == NULL`,
 *   `pattern == NULL`, `repl == NULL`, `string->str == NULL`, or `max_ptr < min_ptr`).
 *   Also sets `string->error` to ::INVALID_ARG or ::NULL_POINTER as appropriate.
 * - Returns `false`, sets `errno = ERANGE` if `min_ptr` or `max_ptr` are outside the
 *   **payload** (checked via `is_string_ptr(..., false)`).
 * - Returns `false`, sets `errno = ENOMEM` on allocation failure (and
 *   `string->error = ::REALLOC_FAIL` when `realloc` fails).
 * - Returns `false`, sets `errno = EOVERFLOW` if a size computation would overflow.
 * - Returns `true` on success (even with zero replacements); on any successful exit,
 *   `string->error = ::NO_ERROR`.
 *
 * Notes:
 * - Empty pattern (`pattern[0] == '\0'`) is a no-op and returns `true`.
 * - Capacity may grow but is not trimmed automatically (see ::trim_string).
 * - Saved pointers into `string->str` may be invalidated by growth.
 *
 * @param string   Haystack ::string_t to be modified (must be initialized).
 * @param pattern  NUL-terminated C string to search for (needle).
 * @param repl     NUL-terminated C string to write in place of each match.
 * @param min_ptr  Inclusive lower-bound pointer into `string->str` (payload).
 * @param max_ptr  Inclusive upper-bound pointer into `string->str` (payload).
 *
 * @return `true` if the operation completed; `false` on error (see above).
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("foo bar foo baz");
 * if (!s) { perror("init_string"); return 1; }
 * char* lo = first_char(s);
 * char* hi = last_char(s);
 *
 * if (!replace_lit_substr(s, "foo", "qux", lo, hi)) {
 *     perror("replace_lit_substr");
 *     fprintf(stderr, "%s\n", error_to_string(get_string_error(s)));
 * }
 *
 * printf("Result: \"%s\" (len=%zu)\n", get_string(s), string_size(s));
 * free_string(s);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * Result: "qux bar qux baz" (len=15)
 * @endcode
 */
bool replace_lit_substr(string_t* string, const char* pattern, const char* replace_string,
                        char* min_ptr, char* max_ptr);
// --------------------------------------------------------------------------------

/**
 * @brief Replace all occurrences of a csalt-string needle within a bounded window
 *        of another csalt string, compacting/growing the buffer in-place.
 *
 * Searches the inclusive window `[min_ptr, max_ptr]` of @p string for the **rightmost**
 * occurrence of @p pattern’s payload, replaces it with @p repl’s payload, and repeats
 * right-to-left until no further matches remain in the window. Edits are performed with
 * `memmove`, moving the **entire suffix including the terminating NUL**, and
 * `string->len` is updated after each replacement.
 *
 * The search phase may be SIMD-accelerated on supported ISAs (SSE2/AVX/AVX2/AVX-512BW,
 * NEON, SVE/SVE2) via the project’s dispatcher. Replacing from right to left avoids
 * rescanning shifted content and prevents overlapping artifacts within the same pass.
 * If the replacement is longer than the pattern, the function first **counts matches**
 * (using the same bounded search) and performs at most one `realloc` to grow capacity.
 *
 * Thread-safety: not thread-safe for the **same** object; the function mutates
 * `string->str`, `string->len`, and `string->error`. Calls on distinct objects are safe.
 *
 * Time complexity: Let `W = max_ptr - min_ptr + 1` and `R` the number of replacements.
 * The total search is ~O(W) (often faster with SIMD); each replacement performs one
 * `memmove` proportional to the remaining suffix, yielding a worst case around O(W·R).
 *
 * Error reporting:
 * - Returns `false`, sets `errno = EINVAL` if any argument is invalid
 *   (`string == NULL`, `pattern == NULL`, `repl == NULL`,
 *   `string->str == NULL`, `pattern->str == NULL`, `repl->str == NULL`,
 *   or `max_ptr < min_ptr`). Also sets `string->error` to ::INVALID_ARG or
 *   ::NULL_POINTER as appropriate.
 * - Returns `false`, sets `errno = ERANGE` if `min_ptr` or `max_ptr` lie outside
 *   the **payload** of @p string (validated via `is_string_ptr(..., false)`).
 * - Returns `false`, sets `errno = ENOMEM` on allocation failure (and
 *   `string->error = ::REALLOC_FAIL` when `realloc` fails).
 * - Returns `false`, sets `errno = EOVERFLOW` if a size computation would overflow.
 * - Returns `true` on success, even when zero replacements occurred; on any successful
 *   exit, `string->error = ::NO_ERROR`.
 *
 * Notes:
 * - Empty pattern (`pattern->len == 0`) is a no-op and returns `true`.
 * - Capacity may grow but is not automatically trimmed; call ::trim_string to shrink.
 * - Any previously saved pointers into `string->str` may be invalidated by growth.
 *
 * @param string       Haystack ::string_t to be modified (must be initialized).
 * @param pattern      Needle ::string_t to search for (read-only).
 * @param replace_string  Replacement ::string_t whose payload replaces matches (read-only).
 * @param min_ptr      Inclusive lower-bound pointer into `string->str` (payload).
 * @param max_ptr      Inclusive upper-bound pointer into `string->str` (payload).
 *
 * @return `true` if the operation completed; `false` on error (see above).
 *
 * @par Example
 * @code{.c}
 * string_t* s   = init_string("alpha beta alpha gamma alpha");
 * string_t* pat = init_string("alpha");
 * string_t* rep = init_string("ALPHA");
 * if (!s || !pat || !rep) { perror("init_string"); goto done; }
 *
 * char* lo = first_char(s);
 * char* hi = last_char(s);
 * if (!replace_string_substr(s, pat, rep, lo, hi)) {
 *     perror("replace_string_substr");
 *     fprintf(stderr, "%s\n", error_to_string(get_string_error(s)));
 * }
 * printf("Result: \"%s\" (len=%zu)\n", get_string(s), string_size(s));
 * done:
 * free_string(s); free_string(pat); free_string(rep);
 * @endcode
 *
 * @par Output (example)
 * @code{.text}
 * Result: "ALPHA beta ALPHA gamma ALPHA" (len=29)
 * @endcode
 */
bool replace_string_substr(string_t* string, const string_t* pattern, const string_t* replace_string,
                           char* min_ptr, char* max_ptr);
// -------------------------------------------------------------------------------- 

/**
 * @def replace_substr
 * @brief Generic front-end to replace a substring inside a bounded window of a
 *        csalt string, dispatching by **pattern** type and validating the
 *        replacement type at compile time.
 *
 * **Dispatch rules (via C11 `_Generic`):**
 * - If @p pattern is a C string pointer (`const char*`, `char*`,
 *   `const unsigned char*`, or `unsigned char*`, and also `void*` to allow
 *   `(void*)0`/`NULL`), this macro calls
 *   ::replace_lit_substr(@p string, @p pattern, @p replace_string, @p min_ptr, @p max_ptr).
 *   In this branch, @p replace_string must also be a C string pointer (same set).
 * - If @p pattern is a csalt string (`const string_t*` or `string_t*`), this macro calls
 *   ::replace_string_substr(@p string, @p pattern, @p replace_string, @p min_ptr, @p max_ptr).
 *   In this branch, @p replace_string must also be `const string_t*` / `string_t*`.
 *
 * The window is **inclusive**: matches starting in `[@p min_ptr, @p max_ptr]` are eligible.
 * The selected function replaces occurrences **right-to-left**, moving the entire suffix
 * (including the NUL terminator) with `memmove`, and updates `string->len` after each edit.
 * If the replacement is longer than the pattern, capacity may be grown (with at most one
 * `realloc` in the typical implementation).
 *
 * **Thread-safety:** Not thread-safe for the **same** object; the selected function mutates
 * `string->str`, `string->len`, and `string->error`. Calls on distinct objects are safe.
 *
 * **Time complexity:** Let `W = max_ptr - min_ptr + 1`, and `R` the number of replacements.
 * Total search is ~O(W) (often faster on SIMD builds); each replacement performs one
 * `memmove` of the remaining suffix → worst case about O(W · R).
 *
 * **Error reporting (from the selected function):**
 * - Returns `false`, sets `errno = EINVAL` for invalid inputs (`NULL` args, or `max_ptr < min_ptr`);
 *   also sets `string->error` to ::INVALID_ARG or ::NULL_POINTER as appropriate.
 * - Returns `false`, sets `errno = ERANGE` if @p min_ptr or @p max_ptr lie outside the payload
 *   of @p string (validated via `is_string_ptr(..., false)`).
 * - Returns `false`, sets `errno = ENOMEM` on allocation failure (and `string->error = ::REALLOC_FAIL`
 *   when `realloc` fails).
 * - Returns `false`, sets `errno = EOVERFLOW` if a size computation would overflow.
 * - Returns `true` on success (even if zero replacements occur); on success,
 *   `string->error = ::NO_ERROR`.
 *
 * **Notes:**
 * - An **empty pattern** is treated as a no-op and yields `true`.
 * - Capacity may grow during replacement but is not automatically trimmed; call ::trim_string
 *   to shrink `alloc` if needed.
 * - The macro forces **array-to-pointer decay** for string literals so `"foo"` matches the C-string
 *   branch. If your platform’s `NULL` is plain `0`, cast to `(const char*)NULL` to select the
 *   literal branch explicitly.
 * - Saved pointers into `string->str` may be invalidated when the buffer grows.
 *
 * **Compile-time checks:** Passing a mismatched type for @p replace_string relative to @p pattern
 * (e.g., `pattern` is `string_t*` but `replace_string` is `char*`) triggers a compile-time
 * diagnostic on GCC/Clang (strong deprecation on MSVC), via an internal
 * `replace_substr__type_mismatch` symbol.
 *
 * @param string          Haystack ::string_t* to modify (must be initialized).
 * @param pattern         Needle: C string pointer or ::string_t* (determines dispatch).
 * @param replace_string  Replacement: must match @p pattern’s category (C string vs ::string_t*).
 * @param min_ptr         Inclusive lower-bound pointer into `string->str` (payload).
 * @param max_ptr         Inclusive upper-bound pointer into `string->str` (payload).
 *
 * @return `bool` — `true` if the operation completed; `false` on error (see above).
 *
 * @par Example (C-string pattern)
 * @code{.c}
 * string_t* s = init_string("foo bar foo baz");
 * char* lo = first_char(s);
 * char* hi = last_char(s);
 * if (!replace_substr(s, "foo", "qux", lo, hi)) {
 *     perror("replace_substr");
 *     fprintf(stderr, "%s\n", error_to_string(get_string_error(s)));
 * }
 * printf("Result: \"%s\" (len=%zu)\n", get_string(s), string_size(s));
 * free_string(s);
 * @endcode
 * @par Output
 * @code{.text}
 * Result: "qux bar qux baz" (len=15)
 * @endcode
 *
 * @par Example (csalt string pattern)
 * @code{.c}
 * string_t* s   = init_string("alpha beta alpha gamma");
 * string_t* pat = init_string("alpha");
 * string_t* rep = init_string("ALPHA");
 * char* lo = first_char(s);
 * char* hi = last_char(s);
 * (void)replace_substr(s, pat, rep, lo, hi);   // dispatches to replace_string_substr
 * printf("Result: \"%s\"\n", get_string(s));
 * free_string(s); free_string(pat); free_string(rep);
 * @endcode
 * @par Output
 * @code{.text}
 * Result: "ALPHA beta ALPHA gamma"
 * @endcode
 *
 * @see replace_lit_substr, replace_string_substr, drop_substr, is_string_ptr,
 *      first_char, last_char, trim_string
 */
bool replace_substr__type_mismatch(string_t*, void*, void*, char*, char*);

#if defined(DOXYGEN)
/* Simple mapping so Doxygen always indexes it */
#  define replace_substr(string, pattern, replace_string, min_ptr, max_ptr) \
       replace_string_substr((string), (pattern), (replace_string), (min_ptr), (max_ptr))
#elif __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_GENERIC__)
/* Single-macro, nested _Generic: dispatch by pattern type, then validate replacement type */
#  define replace_substr(string, pattern, replace_string, min_ptr, max_ptr)                              \
( _Generic((&*(pattern)),                                                                                 \
    /* ---- C-string pattern ---- */                                                                      \
    const char*: _Generic((&*(replace_string)),                                                           \
        const char*:          replace_lit_substr,                                                         \
        char*:                replace_lit_substr,                                                         \
        const unsigned char*: replace_lit_substr,                                                         \
        unsigned char*:       replace_lit_substr,                                                         \
        default:              replace_substr__type_mismatch                                               \
    ),                                                                                                    \
    char*: _Generic((&*(replace_string)),                                                                 \
        const char*:          replace_lit_substr,                                                         \
        char*:                replace_lit_substr,                                                         \
        const unsigned char*: replace_lit_substr,                                                         \
        unsigned char*:       replace_lit_substr,                                                         \
        default:              replace_substr__type_mismatch                                               \
    ),                                                                                                    \
    const unsigned char*: _Generic((&*(replace_string)),                                                  \
        const char*:          replace_lit_substr,                                                         \
        char*:                replace_lit_substr,                                                         \
        const unsigned char*: replace_lit_substr,                                                         \
        unsigned char*:       replace_lit_substr,                                                         \
        default:              replace_substr__type_mismatch                                               \
    ),                                                                                                    \
    unsigned char*: _Generic((&*(replace_string)),                                                        \
        const char*:          replace_lit_substr,                                                         \
        char*:                replace_lit_substr,                                                         \
        const unsigned char*: replace_lit_substr,                                                         \
        unsigned char*:       replace_lit_substr,                                                         \
        default:              replace_substr__type_mismatch                                               \
    ),                                                                                                    \
    /* ---- csalt string pattern ---- */                                                                  \
    const string_t*: _Generic((replace_string),                                                           \
        const string_t*:      replace_string_substr,                                                      \
        string_t*:            replace_string_substr,                                                      \
        default:              replace_substr__type_mismatch                                               \
    ),                                                                                                    \
    string_t*: _Generic((replace_string),                                                                 \
        const string_t*:      replace_string_substr,                                                      \
        string_t*:            replace_string_substr,                                                      \
        default:              replace_substr__type_mismatch                                               \
    ),                                                                                                    \
    /* ---- anything else ---- */                                                                          \
    default: replace_substr__type_mismatch                                                                \
  )((string), (pattern), (replace_string), (min_ptr), (max_ptr)) )
#else
/* Pre-C11 fallback */
#  define replace_substr(string, pattern, replace_string, min_ptr, max_ptr) \
       replace_string_substr((string), (pattern), (replace_string), (min_ptr), (max_ptr))
#endif
// ================================================================================
// ================================================================================ 

 /**
 * @brief Uppercase a single ASCII character in place.
 *
 * Converts the byte referenced by @p val to uppercase if it lies in the ASCII
 * range `'a'..'z'`. Non-letters are left unchanged. The pointer must be valid.
 *
 * Error handling:
 * - If @p val is NULL, the function returns immediately and sets `errno = EINVAL`.
 *
 * Thread-safety: Safe for concurrent calls; operates only on the provided byte.
 * Complexity: O(1).
 *
 * @param val Pointer to the character to modify in place.
 *
 * @return void
 *
 * @par Example
 * @code{.c}
 * char c = 'b';
 * to_upper_char(&c);
 * printf("%c\n", c);
 * @endcode
 * @par Output
 * @code{.text}
 * B
 * @endcode
 *
 * @note ASCII-only. For locale-aware behavior use `<ctype.h>` and
 *       `toupper((unsigned char)*p)`.
 */
void to_upper_char(char* val);
// --------------------------------------------------------------------------------

/**
 * @brief Lowercase a single ASCII character in place.
 *
 * Converts the byte referenced by @p val to lowercase if it lies in the ASCII
 * range `'A'..'Z'`. Non-letters are left unchanged. The pointer must be valid.
 *
 * Error handling:
 * - If @p val is NULL, the function returns immediately and sets `errno = EINVAL`.
 *
 * Thread-safety: Safe for concurrent calls; operates only on the provided byte.
 * Complexity: O(1).
 *
 * @param val Pointer to the character to modify in place.
 *
 * @return void
 *
 * @par Example
 * @code{.c}
 * char c = 'G';
 * to_lower_char(&c);
 * printf("%c\n", c);
 * @endcode
 * @par Output
 * @code{.text}
 * g
 * @endcode
 *
 * @note ASCII-only. For locale-aware behavior use `<ctype.h>` and
 *       `tolower((unsigned char)*p)`.
 */
void to_lower_char(char* val);
// --------------------------------------------------------------------------------

/**
 * @brief Convert all ASCII letters in a csalt string to uppercase, in place.
 *
 * Walks the payload of @p s and uppercases bytes in the range `'a'..'z'`. Bytes
 * outside ASCII letters are left unchanged. The function mutates the buffer
 * referenced by `s->str`.
 *
 * Error handling:
 * - If @p s is NULL, or `s->str` is NULL, the function returns immediately and
 *   sets `errno = EINVAL`. When `s` is non-NULL but `s->str` is NULL, it also sets
 *   `s->error = ::NULL_POINTER`.
 *
 * Thread-safety: Not thread-safe for the same object (mutates `s->str` and may
 * update `s->error`). Calls on distinct objects are safe.
 * Complexity: O(n), where n = `s->len`.
 *
 * @param s Pointer to an initialized ::string_t to modify in place.
 *
 * @return void
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("Hello, World! 123");
 * to_uppercase(s);
 * printf("%s\n", get_string(s));
 * free_string(s);
 * @endcode
 * @par Output
 * @code{.text}
 * HELLO, WORLD! 123
 * @endcode
 *
 * @note ASCII-only transformation. For locale/Unicode-aware casing, consider a
 *       higher-level library. If you prefer to clear error state on success,
 *       set `s->error = ::NO_ERROR` before returning.
 */
void to_uppercase(string_t *s);
// --------------------------------------------------------------------------------

/**
 * @brief Convert all ASCII letters in a csalt string to lowercase, in place.
 *
 * Walks the payload of @p s and lowercases bytes in the range `'A'..'Z'`. Bytes
 * outside ASCII letters are left unchanged. The function mutates the buffer
 * referenced by `s->str`.
 *
 * Error handling:
 * - If @p s is NULL, or `s->str` is NULL, the function returns immediately and
 *   sets `errno = EINVAL`. When `s` is non-NULL but `s->str` is NULL, it also sets
 *   `s->error = ::NULL_POINTER`.
 *
 * Thread-safety: Not thread-safe for the same object (mutates `s->str` and may
 * update `s->error`). Calls on distinct objects are safe.
 * Complexity: O(n), where n = `s->len`.
 *
 * @param s Pointer to an initialized ::string_t to modify in place.
 *
 * @return void
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("Hello, World! 123");
 * to_lowercase(s);
 * printf("%s\n", get_string(s));
 * free_string(s);
 * @endcode
 * @par Output
 * @code{.text}
 * hello, world! 123
 * @endcode
 *
 * @note ASCII-only transformation. For locale/Unicode-aware casing, consider a
 *       higher-level library. If you prefer to clear error state on success,
 *       set `s->error = ::NO_ERROR` before returning.
 */
void to_lowercase(string_t *s);
// --------------------------------------------------------------------------------

/**
 * @brief Pop and return the suffix after the **last** occurrence of @p token, truncating @p s in place.
 *
 * Scans the payload of @p s from right to left for the last occurrence of the delimiter
 * byte @p token. If found at index `i`, the function:
 * - Allocates and returns a new ::string_t containing the bytes **after** the delimiter
 *   (`s->str + i + 1 .. s->str + s->len - 1`). If the delimiter is the last byte, the
 *   returned string is `""` (empty).
 * - Truncates @p s in place at the delimiter by writing `'\0'` at `s->str[i]` and setting
 *   `s->len = i`. The capacity `s->alloc` is unchanged.
 *
 * Thread-safety: **Not thread-safe for the same object.** This function mutates @p s
 * (`str`, `len`, and `error`). Calls on distinct objects are safe.
 *
 * Time complexity: **O(n)** for a string of length `n`, plus O(k) to construct the
 * returned suffix (where `k` is the suffix length). No extra scans are performed.
 *
 * Error reporting:
 * - If `s == NULL` or `s->str == NULL`: returns `NULL`, sets `errno = EINVAL`;
 *   when `s != NULL` but `s->str == NULL`, also sets `s->error = ::NULL_POINTER`.
 * - If `s->len == 0` or `token == '\0'`: returns `NULL`, sets `errno = EINVAL`,
 *   and sets `s->error = ::INVALID_ARG`.
 * - If @p token does **not** occur in the payload: returns `NULL`, sets `errno = ENOENT`,
 *   and sets `s->error = ::NOT_FOUND`.
 * - If allocation of the returned suffix fails: returns `NULL`, leaves @p s unchanged,
 *   sets `errno = ENOMEM`, and sets `s->error = ::BAD_ALLOC` (or ::OUT_OF_MEMORY).
 * - On success: sets `s->error = ::NO_ERROR`.
 *
 * Notes:
 * - The operation reduces the logical length only; capacity is preserved. Use
 *   ::trim_string if you want to shrink `alloc` to `len + 1`.
 * - Any pointers previously obtained with ::first_char / ::last_char that point
 *   **beyond** the new `s->len` become invalid; pointers into the kept prefix
 *   remain valid.
 *
 * @param s      Source ::string_t to modify in place (must be initialized).
 * @param token  Delimiter byte to search for (raw ASCII/byte comparison).
 *
 * @return Newly allocated ::string_t containing the popped suffix (caller owns it),
 *         or `NULL` on error / not found (see Error reporting).
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("path/to/file.txt");
 * if (!s) { perror("init_string"); return 1; }
 *
 * string_t* tail = pop_string_token(s, '/');
 * if (!tail) {
 *     perror("pop_string_token");                       // ENOENT if '/' not found
 *     fprintf(stderr, "%s\n", error_to_string(get_string_error(s)));
 *     free_string(s);
 *     return 1;
 * }
 *
 * printf("head = \"%s\" (len=%zu)\n", get_string(s), string_size(s));
 * printf("tail = \"%s\" (len=%zu)\n", get_string(tail), string_size(tail));
 *
 * free_string(tail);
 * free_string(s);
 * @endcode
 *
 * @par Output
 * @code{.text}
 * head = "path/to" (len=7)
 * tail = "file.txt" (len=8)
 * @endcode
 */
string_t* pop_string_token(string_t* s, char token);
// --------------------------------------------------------------------------------

/**
 * @brief Count tokens in a csalt string separated by any of the delimiter bytes.
 *
 * Interprets @p delim as a **set of separator bytes** and scans the payload of
 * @p str to count the number of maximal runs of non-delimiter bytes (“tokens”).
 * Consecutive delimiters do not produce empty tokens; leading and trailing
 * delimiters are ignored.
 *
 * Semantics:
 * - A byte belongs to the delimiter set if it matches **any** byte in @p delim.
 * - Tokens are maximal substrings of bytes **not** in the delimiter set.
 * - If @p delim is the empty string (`""`) or `str->len == 0`, the result is 0.
 * - Comparison is byte-wise (ASCII). Multibyte encodings (e.g., UTF-8) are not
 *   recognized as single logical delimiters.
 *
 * Error handling:
 * - On invalid input (`str == NULL`, `str->str == NULL`, or `delim == NULL`),
 *   the function returns 0 and sets `errno = EINVAL`. (No ::ErrorCode is set
 *   because @p str is `const`.)
 * - On success, `errno` is not modified.
 *
 * Thread-safety: Read-only; safe for concurrent calls on the same or different
 * objects (does not mutate @p str).
 *
 * Complexity: O(n) for a string of length `n`. When compiled with suitable ISA
 * flags, the implementation may use SIMD to process 16/32/64 bytes per step
 * (SSE2/SSE4.1/AVX/AVX2/AVX-512BW, NEON, SVE/SVE2), with a scalar tail.
 *
 * @param str    Pointer to an initialized ::string_t (must have `str->str != NULL`).
 * @param delim  NUL-terminated C string listing delimiter bytes (e.g., `" ,;\t\n"`).
 *
 * @return Number of tokens as `size_t`. Returns 0 on error or when no tokens exist
 *         (distinguish via `errno`).
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("  hello, world;  this,is  ");
 * if (!s) { perror("init_string"); return 1; }
 *
 * size_t n = token_count(s, " ,;");
 * printf("tokens = %zu\n", n);
 *
 * free_string(s);
 * @endcode
 * @par Output
 * @code{.text}
 * tokens = 4
 * @endcode
 *
 * @note Duplicate characters in @p delim are harmless. A NUL byte (`'\0'`) in
 *       @p delim is ignored because @p delim is parsed as a C string.
 */
size_t token_count(const string_t* str, const char* delim);
// --------------------------------------------------------------------------------

/**
 * @brief Get the byte at @p index from a csalt string.
 *
 * Returns the character stored at zero-based @p index. The function does not
 * modify the string. If the index is out of range or the inputs are invalid,
 * it returns 0 (NUL) and sets errno / the error code as described below.
 *
 * Error handling:
 * - If @p str is NULL or `str->str` is NULL: returns 0 and sets `errno = EINVAL`;
 *   when @p str is non-NULL but `str->str` is NULL, also sets `str->error = ::INVALID_ARG`.
 * - If @p index is out of bounds (greater than the last valid position): returns 0,
 *   sets `errno = ERANGE`, and sets `str->error = ::OUT_OF_BOUNDS`.
 * - If `str->len == 0`: returns 0, sets `errno = ERANGE`, and sets
 *   `str->error = ::INVALID_ARG`.
 * - On success: sets `str->error = ::NO_ERROR`.
 *
 * Thread-safety: Read-only; safe for concurrent calls on the same or different objects.
 * Complexity: O(1).
 *
 * @param str    Pointer to an initialized ::string_t.
 * @param index  Zero-based position into the payload (`0 <= index < str->len`).
 *
 * @return The byte value at @p index on success; 0 on error (check `errno`).
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("Hello");
 * char c = get_char(s, 4);
 * printf("%c\n", c);
 * free_string(s);
 * @endcode
 * @par Output
 * @code{.text}
 * o
 * @endcode
 */
char get_char(string_t* str, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Replace the byte at @p index with @p value, in place.
 *
 * Overwrites `str->str[index]` with @p value, leaving `len`/`alloc` unchanged.
 *
 * Error handling:
 * - If @p str is NULL or `str->str` is NULL: sets `errno = EINVAL`;
 *   when @p str is non-NULL but `str->str` is NULL, sets `str->error = ::NULL_POINTER`.
 * - If @p index is out of bounds: sets `errno = ERANGE` and `str->error = ::OUT_OF_BOUNDS`.
 * - If `str->len == 0`: sets `errno = ERANGE` and `str->error = ::INVALID_ARG`.
 * - On success: sets `str->error = ::NO_ERROR`.
 *
 * Thread-safety: Not thread-safe for the same object (mutates the buffer and error field).
 * Complexity: O(1).
 *
 * @param str    Pointer to an initialized ::string_t to modify.
 * @param index  Zero-based position into the payload (`0 <= index < str->len`).
 * @param value  Replacement byte.
 *
 * @return void
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("Hello");
 * replace_char(s, 0, 'h');
 * printf("%s\n", get_string(s));
 * free_string(s);
 * @endcode
 * @par Output
 * @code{.text}
 * hello
 * @endcode
 */
void replace_char(string_t* str, size_t index, char value);
// --------------------------------------------------------------------------------

/**
 * @brief Remove leading ASCII whitespace from a csalt string, in place.
 *
 * Advances the payload past any run of leading whitespace, moves the remaining
 * bytes to the front (using `memmove`), writes the NUL terminator, and updates
 * `str->len`. Whitespace is detected with `isspace((unsigned char)c)` and is
 * therefore locale-dependent.
 *
 * Error handling:
 * - If @p str is NULL or `str->str` is NULL: sets `errno = EINVAL`;
 *   when @p str is non-NULL but `str->str` is NULL, sets `str->error = ::NULL_POINTER`.
 * - If `str->len == 0`: sets `errno = ERANGE` and `str->error = ::INVALID_ARG`.
 * - On success: sets `str->error = ::NO_ERROR`.
 *
 * Thread-safety: Not thread-safe for the same object (mutates the buffer/length).
 * Complexity: O(n) in the worst case; O(k) where k is the number of leading
 * whitespace bytes plus the size of the shifted tail.
 *
 * @param str Pointer to an initialized ::string_t to modify.
 *
 * @return void
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("   abc  ");
 * trim_leading_whitespace(s);
 * printf("\"%s\" (len=%zu)\n", get_string(s), string_size(s));
 * free_string(s);
 * @endcode
 * @par Output
 * @code{.text}
 * "abc  " (len=5)
 * @endcode
 */
void trim_leading_whitespace(string_t* str);
// --------------------------------------------------------------------------------

/**
 * @brief Remove trailing ASCII whitespace from a csalt string, in place.
 *
 * Walks backward over the payload trimming a run of trailing whitespace
 * (space, tab, newline in this implementation), writes the NUL terminator,
 * and updates `str->len`.
 *
 * Error handling:
 * - If @p str is NULL or `str->str` is NULL: sets `errno = EINVAL`;
 *   when @p str is non-NULL but `str->str` is NULL, sets `str->error = ::NULL_POINTER`.
 * - If `str->len == 0`: sets `errno = ERANGE` and `str->error = ::INVALID_ARG`.
 * - On success: sets `str->error = ::NO_ERROR`.
 *
 * Thread-safety: Not thread-safe for the same object (mutates the buffer/length).
 * Complexity: O(t) where t is the number of trailing whitespace bytes (≤ n).
 *
 * @param str Pointer to an initialized ::string_t to modify.
 *
 * @return void
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string("abc  \n\t");
 * trim_trailing_whitespace(s);
 * printf("\"%s\" (len=%zu)\n", get_string(s), string_size(s));
 * free_string(s);
 * @endcode
 * @par Output
 * @code{.text}
 * "abc" (len=3)
 * @endcode
 *
 * @note You may widen the definition of whitespace (e.g., use `isspace`) if desired.
 */
void trim_trailing_whitespace(string_t* str);
// --------------------------------------------------------------------------------

/**
 * @brief Remove **all** ASCII whitespace from a csalt string, in place.
 *
 * Compacts the payload by copying non-whitespace bytes (space, tab, newline in
 * this implementation) over the original buffer, then NUL-terminates and
 * updates `str->len`. The relative order of non-whitespace bytes is preserved.
 *
 * Error handling:
 * - If @p str is NULL or `str->str` is NULL: sets `errno = EINVAL`;
 *   when @p str is non-NULL but `str->str` is NULL, sets `str->error = ::NULL_POINTER`.
 * - If `str->len == 0`: sets `errno = ERANGE` and `str->error = ::INVALID_ARG`.
 * - On success: sets `str->error = ::NO_ERROR`.
 *
 * Thread-safety: Not thread-safe for the same object (mutates the buffer/length).
 * Complexity: O(n), where n is the original length.
 *
 * @param str Pointer to an initialized ::string_t to modify.
 *
 * @return void
 *
 * @par Example
 * @code{.c}
 * string_t* s = init_string(" a\tb \n c ");
 * trim_all_whitespace(s);
 * printf("\"%s\" (len=%zu)\n", get_string(s), string_size(s));
 * free_string(s);
 * @endcode
 * @par Output
 * @code{.text}
 * "abc" (len=3)
 * @endcode
 *
 * @note For locale-aware whitespace detection, prefer `isspace((unsigned char)c)`.
 */
void trim_all_whitespace(string_t* str);
// ================================================================================
// ================================================================================
// STRING ITERATOR

/**
 * @brief Mutable byte iterator over a ::string_t payload.
 *
 * Iterates over the half-open range `[begin, end)`, where:
 * - `begin == owner->str`
 * - `end   == owner->str + owner->len`  (one past last byte; not the NUL)
 * - `cur` points at the **current** element; `cur < end` means valid.
 *
 * Invalidation: any operation that may reallocate or resize `owner->str`
 * (e.g., concat, reserve, replace, drop) invalidates all iterators for that
 * object. Do not continue using an iterator across such mutations.
 */
typedef struct {
    string_t *owner;
    char     *begin;
    char     *end;    /* one past last */
    char     *cur;    /* current; valid iff cur < end */
} str_iter;

/**
 * @brief Read-only byte iterator over a ::string_t payload.
 *
 * Same semantics as ::str_iter, but cannot mutate the underlying string.
 */
typedef struct {
    const string_t *owner;
    const char     *begin;
    const char     *end;    /* one past last */
    const char     *cur;    /* current; valid iff cur < end */
} cstr_iter;

/* -------- Construction -------- */

/**
 * @brief Create a mutable iterator for @p s.
 *
 * On invalid input (`s == NULL` or `s->str == NULL`), returns an empty iterator
 * (`owner = begin = end = cur = NULL`) and sets `errno = EINVAL`.
 * If @p s is non-NULL but has a NULL buffer, sets `s->error = ::NULL_POINTER`.
 */
str_iter  str_iter_make (string_t *s);

/**
 * @brief Create a const iterator for @p s.
 *
 * On invalid input (`s == NULL` or `s->str == NULL`), returns an empty iterator
 * and sets `errno = EINVAL`.
 */
cstr_iter cstr_iter_make(const string_t *s);

/* -------- Predicates -------- */

/** @return true iff @p it is non-NULL and points at a valid element. */
bool str_iter_valid (const str_iter  *it);
/** @return true iff @p it is non-NULL and points at a valid element. */
bool cstr_iter_valid(const cstr_iter *it);

/** @return true iff @p it is at or beyond end (i.e., not valid). */
bool str_iter_at_end (const str_iter  *it);
/** @return true iff @p it is at or beyond end (i.e., not valid). */
bool cstr_iter_at_end(const cstr_iter *it);

/* -------- Accessors -------- */

/**
 * @brief Get current byte; returns 0 if not valid.
 * Does not advance the iterator.
 */
char        str_iter_get (const str_iter  *it);
/** @copydoc str_iter_get */
char        cstr_iter_get(const cstr_iter *it);

/**
 * @brief Return the current pointer (or NULL if not valid).
 * Useful for APIs that accept `char*` directly.
 */
char*       str_iter_ptr (const str_iter  *it);

/**
 * @brief Return the current pointer (or NULL if not valid).
 * Useful for APIs that accept `const char*` directly.
 */
const char* cstr_iter_ptr(const cstr_iter *it);

/**
 * @brief Zero-based offset of `cur` from `begin`.
 * Returns SIZE_MAX on invalid iterator and sets `errno = EINVAL`.
 */
size_t      str_iter_pos (const str_iter  *it);

/**
 * @brief Zero-based offset of `cur` from `begin`.
 * Returns SIZE_MAX on invalid iterator and sets `errno = EINVAL`.
 */
size_t      cstr_iter_pos(const cstr_iter *it);

/* -------- Movement -------- */

/**
 * @brief Advance to the next byte (if any).
 * @return true iff iterator is valid after the move.
 */
bool str_iter_next (str_iter  *it);

/**
 * @brief Advance to the next byte (if any).
 * @return true iff iterator is valid after the move.
 */
bool cstr_iter_next(cstr_iter *it);

/**
 * @brief Move to the previous byte (if any).
 * @return true iff iterator is valid after the move.
 */
bool str_iter_prev (str_iter  *it);

/**
 * @brief Move to the previous byte (if any).
 * @return true iff iterator is valid after the move.
 */
bool cstr_iter_prev(cstr_iter *it);

/**
 * @brief Move by @p delta bytes; positive moves forward, negative backward.
 * Clamps to the valid range `[begin, end]` (end means “not valid”).
 * @return true iff iterator is valid after the move.
 */
bool str_iter_advance (str_iter  *it, ptrdiff_t delta);

/**
 * @brief Move by @p delta bytes; positive moves forward, negative backward.
 * Clamps to the valid range `[begin, end]` (end means “not valid”).
 * @return true iff iterator is valid after the move.
 */
bool cstr_iter_advance(cstr_iter *it, ptrdiff_t delta);

/**
 * @brief Reset to the first byte (if any).
 * @return true iff iterator becomes valid.
 */
bool str_iter_seek_begin (str_iter  *it);

/**
 * @brief Reset to the first byte (if any).
 * @return true iff iterator becomes valid.
 */
bool cstr_iter_seek_begin(cstr_iter *it);

/**
 * @brief Reset to one-past-last (i.e., end / not valid).
 * @return false (by definition, end is not valid).
 */
bool str_iter_seek_end (str_iter  *it);

/**
 * @brief Reset to one-past-last (i.e., end / not valid).
 * @return false (by definition, end is not valid).
 */
bool cstr_iter_seek_end(cstr_iter *it);
// ================================================================================ 
// ================================================================================ 

/**
* @struct string_v
* @brief Dynamic array (vector) container for string_t objects
*
* This structure manages a resizable array of string_t objects with automatic
* memory management and capacity handling.
*/
typedef struct string_v string_v;
// --------------------------------------------------------------------------------

/**
 * @brief Allocate and initialize a dynamic vector of ::string_t.
 *
 * Creates a new opaque ::string_v container with capacity for @p buffer
 * elements. The vector’s metadata is initialized (length = 0, capacity =
 * @p buffer, error = ::NO_ERROR) and the element storage is zero-initialized so
 * each slot begins as an empty/uninitialized ::string_t. This function has no
 * shared mutable state; `errno` is thread-local on conforming platforms and
 * safe for concurrent calls.
 *
 * **Error reporting:**
 * - On failure, returns NULL and sets `errno` because no object exists to carry
 *   an ::ErrorCode.
 * - On success, the returned object’s internal error status is initialized to ::NO_ERROR.
 *
 * @param buffer  Initial capacity (number of ::string_t slots to allocate).
 *                A value of 0 is permitted; the vector will be valid with
 *                capacity 0 and length 0.
 *
 * @return Pointer to a newly allocated ::string_v on success; NULL on failure.
 *
 * @retval NULL
 *   - If memory allocation fails for the container or its element storage
 *     (`errno` = `ENOMEM`).
 *
 * @pre  None beyond standard memory availability.
 * @post On success:
 *       - `vec->len == 0`
 *       - `vec->alloc == buffer`
 *       - `vec->error == NO_ERROR`
 *       - All element slots are zeroed (safe to initialize later).
 *       The caller owns the returned object and must release it with
 *       ::free_str_vector(), unless using an auto-cleanup macro such as ::STRVEC_GBC.
 *
 * @warning This function performs dynamic allocation. Do not share a single
 *          instance across threads without external synchronization for
 *          subsequent mutations (push/pop/resize, etc.).
 *
 * @par Example
 * @code{.c}
 * string_v* vec = init_str_vector(16);
 * if (vec == NULL) {
 *     perror("init_str_vector failed");  // errno = ENOMEM
 *     return 1;
 * }
 *
 * // ... push/assign strings into vec ...
 *
 * free_str_vector(vec);  // or declare with: string_v* vec STRVEC_GBC = init_str_vector(16);
 * @endcode
 *
 * @see free_str_vector, STRVEC_GBC
 */
string_v* init_str_vector(size_t buffer);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieve the last recorded ::ErrorCode for a ::string_v.
 *
 * Returns the current error/status field stored inside @p vec. This is an
 * O(1) accessor and does not modify the vector. It can be used by callers to
 * inspect the result of the most recent operation that updates the vector’s
 * internal error state.
 *
 * **Error reporting:**
 * - On invalid input (@p vec is NULL), sets `errno` to `EINVAL` and returns
 *   ::INVALID_ERROR.
 * - On valid input, no `errno` is set.
 *
 * @param vec  Pointer to a ::string_v (may be NULL).
 *
 * @return The ::ErrorCode stored in @p vec on success; ::INVALID_ERROR if
 *         @p vec is NULL.
 *
 * @retval INVALID_ERROR
 *   - If @p vec is NULL (`errno` = `EINVAL`).
 *
 * @pre  None beyond @p vec being either NULL or a valid ::string_v pointer.
 * @post No state changes; this function is const and thread-safe for concurrent
 *       reads of distinct ::string_v instances.
 *
 * @warning A return value of ::NO_ERROR does not guarantee that a previous API
 *          call succeeded unless that API documents updating this field. Consult
 *          the specific mutating function’s contract to understand when
 *          `vec->error` is set.
 *
 * @par Example
 * @code{.c}
 * string_v* vec = init_str_vector(8);
 * if (!vec) { perror("init_str_vector"); return 1; }
 *
 * ErrorCode ec = get_str_vector_error(vec);
 * if (ec != NO_ERROR) {
 *     fprintf(stderr, "vector error: %d\n", (int)ec);
 * }
 *
 * free_str_vector(vec);
 * @endcode
 *
 * @see init_str_vector, free_str_vector
 */
ErrorCode get_str_vector_error(const string_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Append a NUL-terminated C string to a string vector (by copying).
 *
 * Appends @p value as a new element to the opaque ::string_v container created
 * by ::init_str_vector(). The function ensures capacity (growing the internal
 * storage if needed), allocates an independent buffer for the new string,
 * copies the bytes (excluding the terminator), and updates the vector state.
 *
 * On failure, returns `false`, sets `errno` via ::set_errno_from_error(), and
 * records a specific ::ErrorCode in the vector’s internal error slot, which
 * you can retrieve with ::get_str_vector_error(). On success, `errno` is not
 * modified.
 *
 * Growth policy (implementation detail):
 * - If the vector is full, capacity increases—typically by doubling up to a
 *   threshold and then by a fixed increment thereafter. Newly added slots are
 *   zero-initialized. Exact thresholds/increments may change between releases.
 *
 * Error handling (returns `false`):
 * - ::INVALID_ARG / ::NULL_POINTER — if @p vec is NULL, or the vector is not
 *   properly initialized, or @p value is NULL (mapped to `EINVAL`).
 * - ::REALLOC_FAIL — if growing the element array fails (mapped to `ENOMEM`).
 * - ::BAD_ALLOC — if allocating the new element’s character buffer fails
 *   (mapped to `ENOMEM`).
 *
 * @param vec    Opaque pointer to a valid string vector (from ::init_str_vector()).
 * @param value  Pointer to a readable, NUL-terminated byte string to append.
 *
 * @return `true` on success; `false` on error (see Error handling above).
 *
 * @post On success:
 * - The vector contains one additional element that owns an independent copy of
 *   @p value (NUL-terminated).
 * - The element order is preserved; the new string appears at the end.
 * - The vector’s capacity may have changed (internal reallocation).
 *
 * **thread_safety**
 * - **Not thread-safe** for concurrent mutation of the same vector; external
 *   synchronization is required if multiple threads may append/modify.
 * - Concurrent reads on a *stable* vector are safe provided no other thread
 *   might trigger growth/reallocation at the same time.
 *
 * **complexity**
 * - Amortized O(1) per append for capacity management, plus O(n) to copy the
 *   bytes of @p value (where n = `strlen(value)`). Worst-case O(n) when a
 *   growth reallocation occurs.
 *
 * @note Any raw element pointers obtained through accessors may be invalidated
 *       if this call grows the vector’s internal storage.
 *
 * @par Example
 * @code{.c}
 * string_v *v = init_str_vector(4);
 * if (!v) { perror("init_str_vector"); return 1; }
 *
 * if (!push_back_str_vector(v, "alpha")) {
 *     ErrorCode ec = get_str_vector_error(v);
 *     fprintf(stderr, "[%d] %s\n", (int)ec, error_to_string(ec));
 *     perror("push_back_str_vector");  // errno set by callee on error
 *     free_str_vector(v);
 *     return 1;
 * }
 *
 * // ... use other vector APIs to inspect elements ...
 *
 * free_str_vector(v);
 * @endcode
 *
 * @see init_str_vector, free_str_vector, get_str_vector_error,
 *      set_errno_from_error, error_to_string
 */
bool push_back_str_vector(string_v* vec, const char* value);
// --------------------------------------------------------------------------------

/**
 * @brief Prepend a NUL-terminated C string to a string vector (insert at index 0).
 *
 * Appends @p value to the **front** of the opaque ::string_v created by
 * ::init_str_vector(). The function ensures capacity (growing internal storage
 * if needed), shifts existing elements one slot to the right, allocates an
 * independent buffer for the new first element, copies the bytes of @p value
 * (excluding the terminator), and updates the vector state.
 *
 * On failure, returns `false`, sets `errno` via ::set_errno_from_error(), and
 * records a specific ::ErrorCode internally (retrievable with
 * ::get_str_vector_error()). On success, `errno` is not modified.
 *
 * Behavior & guarantees:
 * - If the vector must grow, capacity increases using the implementation’s
 *   growth policy (e.g., geometric growth up to a threshold then fixed
 *   increments). Newly added slots are zero-initialized.
 * - Existing elements are shifted using a shallow move of element metadata
 *   (e.g., pointers). Underlying strings are not copied—only their owning
 *   element slots move.
 * - If allocation of the new first element’s buffer fails **after** growth and
 *   shifting, the function restores the original order (shifts elements back)
 *   and returns `false`. Thus, on failure the vector’s externally observable
 *   contents and length are unchanged.
 *
 * Error handling (returns `false`):
 * - ::NULL_POINTER — if @p vec is non-NULL but uninitialized, or @p value is NULL
 *   (mapped to `EINVAL`).
 * - **`EINVAL` directly** — if @p vec is NULL (no error slot to write).
 * - ::REALLOC_FAIL — if growing the element array fails (mapped to `ENOMEM`).
 * - ::BAD_ALLOC — if allocating the new element’s character buffer fails
 *   (mapped to `ENOMEM`).
 *
 * @param vec    Opaque pointer to a valid string vector (from ::init_str_vector()).
 * @param value  Pointer to a readable, NUL-terminated byte string to insert at the front.
 *
 * @return `true` on success; `false` on error (see Error handling above).
 *
 * @post On success:
 * - The vector contains one additional element at index 0 that owns an
 *   independent, NUL-terminated copy of @p value.
 * - The relative order of previous elements is preserved but each index is
 *   increased by 1 due to the front insertion.
 * - Internal storage may have been reallocated; any previously held raw
 *   pointers into the vector’s element array are invalidated.
 *
 * **thread_safety**
 * - **Not thread-safe** for concurrent mutation of the same vector; use external
 *   synchronization if multiple threads may insert/modify concurrently.
 * - Concurrent readers must ensure no other thread can trigger growth or
 *   reordering at the same time.
 *
 * **complexity**
 * - Let @c m be the current number of elements and @c n = `strlen(value)`.
 *   - Shifting elements: O(m)
 *   - Copying @p value: O(n)
 *   - Capacity growth (when it occurs): O(m) to move element slots (amortized
 *     constant per push across many operations).
 *   Overall worst-case: **O(m + n)**; amortized cost dominated by O(n) for the copy.
 *
 * @note Any raw element pointers obtained through accessors may change even
 *       without a reallocation, because front insertion reindexes elements.
 *
 * @par Example
 * @code{.c}
 * string_v *v = init_str_vector(2);
 * if (!v) { perror("init_str_vector"); return 1; }
 *
 * if (!push_front_str_vector(v, "first")) {
 *     ErrorCode ec = get_str_vector_error(v);
 *     fprintf(stderr, "[%d] %s\n", (int)ec, error_to_string(ec));
 *     perror("push_front_str_vector");  // errno set by callee on error
 *     free_str_vector(v);
 *     return 1;
 * }
 *
 * // ... use other vector APIs to access elements in order ...
 *
 * free_str_vector(v);
 * @endcode
 *
 * @see init_str_vector, push_back_str_vector, free_str_vector,
 *      get_str_vector_error, set_errno_from_error, error_to_string
 */
bool push_front_str_vector(string_v* vec, const char* value);
// --------------------------------------------------------------------------------

/**
 * @brief Insert a NUL-terminated C string at a given index in a string vector.
 *
 * Inserts @p str as a new element of the opaque ::string_v (created by
 * ::init_str_vector()) at position @p index. If the vector is full, the
 * function grows internal storage, shifts existing elements at and after
 * @p index one slot to the right, allocates an independent buffer for the new
 * element, copies the bytes of @p str (excluding the terminator), and updates
 * the vector state.
 *
 * On failure, returns `false`, sets `errno` via ::set_errno_from_error(), and
 * records a specific ::ErrorCode in the vector’s internal error slot, which you
 * can retrieve with ::get_str_vector_error(). On success, `errno` is not
 * modified.
 *
 * Semantics & bounds:
 * - Valid positions are `0 <= index <= length`. Inserting at `index == length`
 *   is equivalent to a push-back; inserting at `index == 0` is a push-front.
 * - If growth occurs, newly added slots are zero-initialized.
 * - If allocation of the new element’s character buffer fails **after** any
 *   growth and shifting, the function restores the original order (shifts
 *   elements back) and returns `false`. Thus, on failure the externally
 *   observable contents and length remain unchanged.
 *
 * Error handling (returns `false`):
 * - ::NULL_POINTER — if @p vec is non-NULL but uninitialized, or @p str is NULL
 *   (mapped to `EINVAL`). If @p vec itself is NULL, the function sets `errno`
 *   to `EINVAL` (no internal error slot to update).
 * - ::OUT_OF_BOUNDS — if @p index is greater than the current length.
 * - ::REALLOC_FAIL — if growing the element array fails (mapped to `ENOMEM`).
 * - ::BAD_ALLOC — if allocating the new element’s character buffer fails
 *   (mapped to `ENOMEM`).
 *
 * @param vec    Opaque pointer to a valid string vector (from ::init_str_vector()).
 * @param str    Pointer to a readable, NUL-terminated byte string to insert.
 * @param index  Zero-based insertion position; must satisfy `0 <= index <= length`.
 *
 * @return `true` on success; `false` on error (see Error handling above).
 *
 * @post On success:
 * - The vector contains one additional element at @p index that owns an
 *   independent, NUL-terminated copy of @p str.
 * - The relative order of previous elements is preserved, but items at and
 *   after @p index are shifted one position to the right.
 * - Internal storage may have been reallocated; any previously held raw
 *   pointers/iterators into the element array are invalidated.
 *
 * **thread_safety**
 * - **Not thread-safe** for concurrent mutation of the same vector; use external
 *   synchronization if multiple threads may insert/modify concurrently.
 * - Concurrent readers must ensure no other thread can trigger growth or
 *   reindexing at the same time.
 *
 * **complexity**
 * - Let @c m be the current number of elements and @c k = (m - index) the
 *   number of shifted elements; let @c n = `strlen(str)`.
 *   - Shifting element slots: O(k)
 *   - Copying @p str: O(n)
 *   - Capacity growth (when it occurs): O(m) to move element slots (amortized
 *     constant per operation across many inserts).
 *   Overall worst-case: **O(k + n)**; amortized cost dominated by O(n) for the copy.
 *
 * @note This function follows the conventional `errno` policy: it sets `errno`
 *       only when an error occurs; on success, it leaves `errno` unchanged.
 *
 * @par Example
 * @code{.c}
 * string_v *v = init_str_vector(2);
 * if (!v) { perror("init_str_vector"); return 1; }
 *
 * if (!insert_str_vector(v, "middle", 0)) {
 *     ErrorCode ec = get_str_vector_error(v);
 *     fprintf(stderr, "[%d] %s\n", (int)ec, error_to_string(ec));
 *     perror("insert_str_vector");  // errno set by callee on error
 *     free_str_vector(v);
 *     return 1;
 * }
 *
 * // ... use other vector APIs to access elements ...
 *
 * free_str_vector(v);
 * @endcode
 *
 * @see init_str_vector, push_front_str_vector, push_back_str_vector,
 *      get_str_vector_error, set_errno_from_error, error_to_string
 *
 */
bool insert_str_vector(string_v* vec, const char* value, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Return a read-only handle to the element at @p index in a string vector.
 *
 * Retrieves a pointer to the stored ::string_t at zero-based position @p index
 * in the opaque ::string_v created by ::init_str_vector(). The element is not
 * copied; the returned pointer refers to storage owned by the vector and must
 * **not** be freed by the caller.
 *
 * On invalid input or out-of-range index, returns `NULL` and sets `errno`
 * (see Error reporting). On success, `errno` is not modified.
 *
 * Lifetime & aliasing:
 * - The returned pointer remains valid until a subsequent operation that may
 *   reallocate or reorder internal storage (e.g., insert, erase, push, growth).
 *   After such mutations, previously obtained pointers may be invalid.
 *
 * Error reporting:
 * - `EINVAL` — if @p vec is `NULL` or not properly initialized.
 * - `ERANGE` — if @p index is outside the valid range (`index >= size`).
 *
 * @param vec    Opaque pointer to a valid string vector (from ::init_str_vector()).
 * @param index  Zero-based index; must satisfy `0 <= index < size`.
 *
 * @return Pointer to the const element on success; `NULL` on error.
 *
 * **complexity O(1)**
 *
 * @thread_safety
 * - Safe for concurrent **reads** provided no other thread mutates the same
 *   vector concurrently.
 * - Not safe if another thread may insert/erase/grow at the same time.
 *
 * @par Example
 * @code{.c}
 * const string_t *elt = cstr_vector_index(v, i);
 * if (!elt) {
 *     perror("str_vector_index");       // errno set to EINVAL or ERANGE
 *     return;
 * }
 * printf("%zu: %s\n", i, get_string(elt));  // via your string_t accessors
 * @endcode
 *
 * @see str_vector_size, str_vector_alloc
 */
const string_t* cstr_vector_index(const string_v* vec, size_t index);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a **mutable** handle to the element at @p index.
 *
 * Looks up the @p index-th element of the opaque ::string_v (created by
 * ::init_str_vector()) and returns a **non-const** ::string_t* that aliases the
 * vector’s internal storage. This allows in-place modification of the element
 * via your public ::string_t API (e.g., append/assign helpers).
 *
 * **Safety & ownership caveats (important):**
 * - The returned pointer refers to **internal storage owned by the vector**.
 *   Do **not** call `free()` on the returned ::string_t or its internal buffer;
 *   doing so will corrupt the vector and likely cause double-free or UAF.
 * - Do **not** manually null out fields or otherwise mutate internals directly.
 *   Use the provided ::string_t API (e.g., `string_lit_concat`, `string_string_concat`,
 *   `swap_string`, etc.) which preserves invariants.
 * - Any operation that grows/shrinks/reorders the vector (push/insert/erase,
 *   sort, reverse, reallocation) can **invalidate** previously returned pointers.
 *   Re-acquire the pointer after such operations.
 * - For read-only access, prefer ::cstr_vector_index() which returns a `const` handle.
 *
 * @param vec    Opaque pointer to a string vector.
 * @param index  Zero-based element index; must satisfy `index < size`.
 *
 * @return On success, a non-NULL pointer to the internal ::string_t at @p index.
 *         On error, returns NULL and sets `errno`.
 *
 * @retval NULL with `errno = EINVAL`
 *         if @p vec is NULL or not properly initialized.
 * @retval NULL with `errno = ERANGE`
 *         if @p index is out of bounds (`index >= size`).
 *
 * **thread_safety**
 * - **Not thread-safe** for concurrent mutation of the same vector/element.
 *   External synchronization is required if other threads can modify the vector
 *   or the returned element concurrently.
 *
 * **complexity**
 * - O(1).
 *
 * @warning Do **not** free or detach the returned element or its buffer; the
 *          vector remains the owner. If you need an independently owned object,
 *          use a copying API (e.g., `copy_string(...)`) or one of the `pop_*`
 *          helpers that return a heap-owned ::string_t.
 *
 * @note On success, this function does **not** modify `errno`.
 *
 * @par Example (safe in-place edit)
 * @code{.c}
 * string_t *s = str_vector_index(v, 3);
 * if (!s) {
 *     perror("str_vector_index");
 *     return;
 * }
 * // Modify via string_t API (do NOT free or poke internals directly)
 * if (!string_lit_concat(s, " suffix")) {
 *     perror("string_lit_concat");
 * }
 * @endcode
 *
 * @see cstr_vector_index, str_vector_size, get_str_vector_error,
 *      string_lit_concat, string_string_concat, swap_string
 */
string_t* str_vector_index(string_v* vec, size_t index);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the number of elements currently stored in the vector.
 *
 * @param vec  Opaque pointer to a string vector.
 *
 * @return Current logical size (element count) on success.
 *         On error, returns `LONG_MAX` and sets `errno = EINVAL`.
 *
 * @thread_safety
 * - Safe to call when no other thread mutates the same vector concurrently.
 *
 * **complexity**
 * - O(1).
 *
 * @note On success, this function does **not** modify `errno`.
 * @note The error sentinel is `LONG_MAX`; callers should prefer checking
 *       `errno != 0` to detect failure rather than relying solely on the
 *       sentinel value.
 *
 * @par Example
 * @code{.c}
 * errno = 0;
 * size_t n = str_vector_size(v);
 * if (errno != 0) { perror("str_vector_size");}
 * else            { printf("size = %zu\n", n); }
 * @endcode
 */
size_t str_vector_size(const string_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the current capacity (allocated slots) of the vector.
 *
 * Reports how many element slots are currently allocated internally. This is
 * the maximum number of elements that can be stored without triggering a grow.
 *
 * @param vec  Opaque pointer to a string vector.
 *
 * @return Capacity in elements on success.
 *         On error, returns `LONG_MAX` and sets `errno = EINVAL`.
 *
 * @thread_safety
 * - Safe to call when no other thread mutates the same vector concurrently.
 *
 * @complexity
 * - O(1).
 *
 * @note On success, this function does **not** modify `errno`.
 * @note The error sentinel is `LONG_MAX`; callers should prefer checking
 *       `errno != 0` to detect failure.
 *
 * @par Example
 * @code{.c}
 * errno = 0;
 * size_t cap = str_vector_alloc(v);
 * if (errno != 0) { perror("str_vector_alloc");}
 * else            { printf("capacity = %zu\n", cap); }
 * @endcode
 */
size_t str_vector_alloc(const string_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Remove the last element from a string vector and return a heap-owned copy.
 *
 * Removes the final element of an opaque ::string_v (created by ::init_str_vector())
 * and returns a newly allocated ::string_t that owns an independent copy of that
 * string. The vector’s internal slot is cleared for reuse; capacity is unchanged.
 *
 * Strong safety:
 * - The function first creates a copy of the last element. If copying fails,
 *   the vector is left **unchanged** and `NULL` is returned.
 *
 * Error handling:
 * - On failure, returns `NULL` and sets `errno` (via ::set_errno_from_error()).
 *   A specific ::ErrorCode is recorded internally and can be retrieved with
 *   ::get_str_vector_error().
 *   - Invalid/NULL inputs or uninitialized vector storage → `EINVAL`
 *     (internal error: ::NULL_POINTER).
 *   - Pop from an empty vector → mapped to a category error (implementation uses
 *     ::UNINITIALIZED for “empty”; callers should treat it as “no element to pop”).
 *   - Copy failure for the outgoing element → `ENOMEM` (internal error: ::BAD_ALLOC).
 *
 * @param vec  Opaque pointer to a valid string vector.
 *
 * @return Pointer to a newly allocated ::string_t on success; `NULL` on error.
 *         The caller owns the returned object and must destroy it with the
 *         appropriate destructor (e.g., `free_string()`).
 *
 * @post On success:
 * - The vector’s logical size decreases by 1 (last element removed).
 * - The removed element’s internal storage is freed and its slot zeroed.
 * - Vector capacity is unchanged; existing (non-removed) elements remain valid.
 *
 * **thread_safety**
 * - **Not thread-safe** for concurrent mutation of the same vector; external
 *   synchronization is required if multiple threads may pop/modify.
 * - Concurrent readers must ensure no other thread mutates the vector at the same time.
 *
 * **complexity**
 * - Let @c n be the length of the removed string. Copying the element is O(n);
 *   all other operations are O(1). Overall: **O(n)**.
 *
 * @note On success, this function leaves `errno` unchanged (conventional POSIX policy).
 *
 * @par Example
 * @code{.c}
 * string_t *last = pop_back_str_vector(v);
 * if (!last) {
 *     ErrorCode ec = get_str_vector_error(v);
 *     fprintf(stderr, "pop failed: %s\n", error_to_string(ec));
 *     perror("pop_back_str_vector");
 * } else {
 *     // use 'last' via your string_t API
 *     // ...
 *     free_string(last);  // caller responsibility
 * }
 * @endcode
 *
 * @see init_str_vector, get_str_vector_error, error_to_string, set_errno_from_error,
 *      free_str_vector
 */

string_t* pop_back_str_vector(string_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief Remove the first element from a string vector and return a heap-owned copy.
 *
 * Removes the element at index 0 of an opaque ::string_v (created by
 * ::init_str_vector()) and returns a newly allocated ::string_t that owns an
 * independent copy of that string. Remaining elements are shifted one slot to
 * the left; vector capacity is unchanged.
 *
 * Strong safety:
 * - The function first creates a copy of the first element. If copying fails,
 *   the vector is left **unchanged** and `NULL` is returned.
 *
 * Error handling:
 * - On failure, returns `NULL` and sets `errno` via ::set_errno_from_error().
 *   A specific ::ErrorCode is recorded internally and can be read with
 *   ::get_str_vector_error().
 *   - Invalid/NULL inputs or uninitialized storage → `EINVAL`
 *     (internal error: ::NULL_POINTER).
 *   - Pop from an empty vector → mapped category error (implementation uses
 *     ::UNINITIALIZED to signal “no element to pop”).
 *   - Copy failure for the outgoing element → `ENOMEM` (internal error: ::BAD_ALLOC).
 *
 * @param vec  Opaque pointer to a valid string vector.
 *
 * @return Pointer to a newly allocated ::string_t on success; `NULL` on error.
 *         The caller owns the returned object and must destroy it with the
 *         appropriate destructor (e.g., `free_string()`).
 *
 * @post On success:
 * - The vector’s logical size decreases by 1, elements shift left, and order
 *   among the remaining elements is preserved.
 * - The removed element’s internal storage is freed; its former slot is zeroed.
 * - Capacity is unchanged; any previously held raw pointers to elements may be
 *   invalidated due to shifting.
 *
 * **thread_safety**
 * - **Not thread-safe** for concurrent mutation of the same vector; synchronize
 *   externally if multiple threads may pop/modify concurrently.
 * - Concurrent readers must ensure no other thread mutates or reindexes the vector.
 *
 * **complexity**
 * - Let @c n be the length (in bytes) of the removed string and @c m be the
 *   number of remaining elements. Copying the element is O(n); shifting is O(m).
 *   Overall: **O(m + n)**.
 *
 * @note On success, this function leaves `errno` unchanged (conventional POSIX policy).
 *
 * @par Example
 * @code{.c}
 * string_t *first = pop_front_str_vector(v);
 * if (!first) {
 *     ErrorCode ec = get_str_vector_error(v);
 *     fprintf(stderr, "pop_front failed: %s\n", error_to_string(ec));
 *     perror("pop_front_str_vector");
 * } else {
 *     // use 'first' via your string_t API
 *     // ...
 *     free_string(first);  // caller responsibility
 * }
 * @endcode
 *
 * @see init_str_vector, pop_back_str_vector, get_str_vector_error,
 *      error_to_string, set_errno_from_error, free_str_vector
 */
string_t* pop_front_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Remove the element at @p index from a string vector and return a heap-owned copy.
 *
 * Removes the element at position @p index from an opaque ::string_v (created by
 * ::init_str_vector()) and returns a newly allocated ::string_t that owns an
 * independent copy of that string. Remaining elements are shifted one slot left
 * to fill the gap; vector capacity is unchanged.
 *
 * Strong safety:
 * - The function first creates a copy of the target element. If copying fails,
 *   the vector is left **unchanged** and `NULL` is returned.
 *
 * Bounds & behavior:
 * - Valid indices satisfy `0 <= index < size`. Use ::str_vector_size() to query
 *   the current size if needed.
 * - On success the relative order of the other elements is preserved (aside from
 *   the left-shift caused by the removal).
 *
 * Error handling:
 * - On failure, returns `NULL` and sets `errno` via ::set_errno_from_error().
 *   A specific ::ErrorCode is recorded internally and may be retrieved with
 *   ::get_str_vector_error().
 *   - Invalid/NULL inputs or uninitialized storage → `EINVAL`
 *     (internal error: ::NULL_POINTER).
 *   - Empty vector (no element to remove) → mapped category error
 *     (implementation uses ::UNINITIALIZED).
 *   - @p index out of range → `ERANGE` (internal error: ::OUT_OF_BOUNDS).
 *   - Copy failure for the outgoing element → `ENOMEM` (internal error: ::BAD_ALLOC).
 *
 * @param vec    Opaque pointer to a valid string vector.
 * @param index  Zero-based position of the element to remove; must be `< size`.
 *
 * @return Pointer to a newly allocated ::string_t on success; `NULL` on error.
 *         The caller owns the returned object and must destroy it with the
 *         appropriate destructor (e.g., `free_string()`).
 *
 * @post On success:
 * - The vector’s logical size decreases by 1; elements after @p index shift left.
 * - The removed element’s internal storage is freed; its former slot is zeroed.
 * - Capacity is unchanged; any previously held raw pointers/iterators into the
 *   vector’s element array are invalidated due to shifting.
 *
 * **thread_safety**
 * - **Not thread-safe** for concurrent mutation of the same vector; synchronize
 *   externally if multiple threads may remove/modify concurrently.
 * - Concurrent readers must ensure no other thread mutates or reindexes the vector.
 *
 * **complexity**
 * - Let @c m be the number of elements after @p index (i.e., `size - index - 1`)
 *   that must shift, and @c n be the length of the removed string in bytes.
 *   Copying the element is O(n); shifting is O(m). Overall: **O(m + n)**.
 *
 * @note On success, this function leaves `errno` unchanged (conventional POSIX policy).
 *
 * @par Example
 * @code{.c}
 * size_t idx = 2;
 * string_t *removed = pop_any_str_vector(v, idx);
 * if (!removed) {
 *     ErrorCode ec = get_str_vector_error(v);
 *     fprintf(stderr, "pop_any failed: %s\n", error_to_string(ec));
 *     perror("pop_any_str_vector");
 * } else {
 *     // use 'removed' via your string_t API
 *     // ...
 *     free_string(removed);  // caller responsibility
 * }
 * @endcode
 *
 * @see init_str_vector, pop_front_str_vector, pop_back_str_vector,
 *      str_vector_size, get_str_vector_error, error_to_string, set_errno_from_error
 */
string_t* pop_any_str_vector(string_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Remove the last element from a string vector (no return value).
 *
 * Deletes (pops) the final element of an opaque ::string_v created by
 * ::init_str_vector(). The element’s owned character buffer is freed, the slot
 * is cleared for reuse, and the vector’s length is decremented. Capacity is
 * unchanged.
 *
 * Error handling:
 * - On failure, returns `false` and sets `errno` via ::set_errno_from_error().
 *   An internal ::ErrorCode is recorded and can be retrieved with
 *   ::get_str_vector_error().
 *   - ::NULL_POINTER → `EINVAL` if @p vec is non-NULL but not properly
 *     initialized (or @p vec->data is NULL). If @p vec itself is NULL, the
 *     function sets `errno = EINVAL` (no internal error slot to update).
 *   - ::UNINITIALIZED → mapped to `ENODATA` when available (else `EINVAL`) if
 *     the vector is empty (no element to delete).
 * - On success, `errno` is **not** modified.
 *
 * @param vec  Opaque pointer to a valid string vector.
 *
 * @return `true` on success; `false` on error (see Error handling above).
 *
 * @post On success:
 * - The last element is removed; the vector’s size decreases by 1.
 * - The removed element’s storage is freed and its slot zeroed.
 * - Capacity is unchanged; remaining elements and their order are unaffected.
 *
 * **thread_safety**
 * - **Not thread-safe** for concurrent mutation of the same vector; use external
 *   synchronization if multiple threads may delete/modify concurrently.
 * - Concurrent readers must ensure no other thread mutates the vector at the
 *   same time.
 *
 * **complexity**
 * - O(1) with respect to the number of elements (freeing one string and updating
 *   metadata).
 *
 * @par Example
 * @code{.c}
 * if (!delete_back_str_vector(v)) {
 *     ErrorCode ec = get_str_vector_error(v);
 *     fprintf(stderr, "delete_back failed: %s\n", error_to_string(ec));
 *     perror("delete_back_str_vector");
 * } else {
 *     // last element removed; capacity unchanged
 * }
 * @endcode
 *
 * @see init_str_vector, pop_back_str_vector, get_str_vector_error,
 *      set_errno_from_error, error_to_string, free_str_vector
 */

bool delete_back_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Remove the first element from a string vector (no return value).
 *
 * Deletes the element at index 0 of an opaque ::string_v created by
 * ::init_str_vector(). The element’s owned character buffer is freed, remaining
 * elements are shifted one slot to the left to fill the gap, the vacated tail
 * slot is zeroed, and the vector’s length is decremented. Capacity is unchanged.
 *
 * Error handling:
 * - On failure, returns `false` and sets `errno` via ::set_errno_from_error();
 *   an internal ::ErrorCode is recorded and can be retrieved with
 *   ::get_str_vector_error().
 *   - ::NULL_POINTER → `EINVAL` if @p vec is non-NULL but not properly
 *     initialized (e.g., internal storage missing). If @p vec itself is NULL,
 *     the function sets `errno = EINVAL` (no internal error slot to update).
 *   - ::UNINITIALIZED → mapped to `ENODATA` when available (else `EINVAL`)
 *     if the vector is empty (no element to delete).
 * - On success, `errno` is **not** modified.
 *
 * @param vec  Opaque pointer to a valid string vector.
 *
 * @return `true` on success; `false` on error (see Error handling above).
 *
 * @post On success:
 * - The first element is removed; the vector’s size decreases by 1.
 * - Elements formerly at indices `[1 .. size-1]` shift left by one.
 * - The removed element’s storage is freed and the last slot is zeroed.
 * - Capacity is unchanged.
 *
 * **thread_safety**
 * - **Not thread-safe** for concurrent mutation of the same vector; use external
 *   synchronization if multiple threads may delete/modify concurrently.
 * - Concurrent readers must ensure no other thread mutates or reindexes the vector.
 *
 * **complexity**
 * - Let @c m be the number of remaining elements after removal. Shifting element
 *   metadata is O(m); freeing one string is O(1). Overall: **O(m)**.
 *
 * @warning Any raw element pointers/iterators previously obtained may be
 *          invalidated due to reindexing and (in other operations) possible
 *          reallocation.
 *
 * @par Example
 * @code{.c}
 * if (!delete_front_str_vector(v)) {
 *     ErrorCode ec = get_str_vector_error(v);
 *     fprintf(stderr, "delete_front failed: %s\n", error_to_string(ec));
 *     perror("delete_front_str_vector");
 * } else {
 *     // first element removed; order of remaining elements preserved
 * }
 * @endcode
 *
 * @see init_str_vector, delete_back_str_vector, pop_front_str_vector,
 *      get_str_vector_error, set_errno_from_error, error_to_string, free_str_vector
 */

bool delete_front_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Remove the element at @p index from a string vector (no return value).
 *
 * Deletes the element at position @p index from an opaque ::string_v created by
 * ::init_str_vector(). The element’s owned character buffer is freed, elements
 * after @p index are shifted one slot to the left to fill the gap, the vacated
 * tail slot is zeroed, and the vector’s length is decremented. Capacity is
 * unchanged.
 *
 * Bounds & behavior:
 * - Valid indices satisfy `0 <= index < size`. Use ::str_vector_size() to query
 *   the current size if needed.
 * - The relative order of the *remaining* elements is preserved aside from the
 *   left-shift induced by the removal.
 *
 * Error handling:
 * - On failure, returns `false` and sets `errno` via ::set_errno_from_error();
 *   a specific ::ErrorCode is recorded internally and can be retrieved with
 *   ::get_str_vector_error().
 *   - ::NULL_POINTER → `EINVAL` if @p vec is non-NULL but uninitialized (or
 *     internal storage missing). If @p vec itself is NULL, the function sets
 *     `errno = EINVAL` (no internal error slot to update).
 *   - ::UNINITIALIZED → `ENODATA` when available (else `EINVAL`) if the vector
 *     is empty (no element to delete).
 *   - ::OUT_OF_BOUNDS → `EINVAL` if @p index is not less than the current size.
 * - On success, this function leaves `errno` **unchanged** (conventional POSIX policy).
 *
 * @param vec    Opaque pointer to a valid string vector.
 * @param index  Zero-based position of the element to remove; must be `< size`.
 *
 * @return `true` on success; `false` on error (see Error handling above).
 *
 * @post On success:
 * - The vector’s logical size decreases by 1; elements after @p index shift left.
 * - The removed element’s storage is freed; the last slot is zeroed.
 * - Capacity is unchanged.
 *
 * **thread_safety**
 * - **Not thread-safe** for concurrent mutation of the same vector; synchronize
 *   externally if multiple threads may remove/modify concurrently.
 * - Concurrent readers must ensure no other thread mutates or reindexes the vector.
 *
 * **complexity**
 * - Let @c m be the number of elements after @p index (i.e., `size - index - 1`).
 *   Freeing one string is O(1); shifting element slots is O(m). Overall: **O(m)**.
 *
 * @warning Any raw element pointers/iterators previously obtained may be
 *          invalidated due to reindexing (and, in other operations, possible
 *          reallocation).
 *
 * @par Example
 * @code{.c}
 * size_t idx = 2;
 * if (!delete_any_str_vector(v, idx)) {
 *     ErrorCode ec = get_str_vector_error(v);
 *     fprintf(stderr, "delete_any failed: %s\n", error_to_string(ec));
 *     perror("delete_any_str_vector");
 * } else {
 *     // element at idx removed; order preserved among remaining elements
 * }
 * @endcode
 *
 * @see init_str_vector, delete_front_str_vector, delete_back_str_vector,
 *      pop_any_str_vector, str_vector_size, get_str_vector_error,
 *      set_errno_from_error, error_to_string
 */

bool delete_any_str_vector(string_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Destroy a ::string_v and release all owned resources.
 *
 * Frees each initialized ::string_t element in the vector and then releases
 * the vector’s element storage and the vector object itself. Time complexity
 * is O(n) in the current length (`vec->len`).
 *
 * **Error reporting:**
 * - On invalid input (@p vec is NULL), the function does nothing and sets
 *   `errno` to `EINVAL`.
 * - On valid input, no `errno` is set on success.
 *
 * @param vec  Pointer to a ::string_v previously returned by ::init_str_vector().
 *
 * @retval void
 *
 * @pre  @p vec is either NULL or points to a valid ::string_v.
 * @post If @p vec was non-NULL, all contained strings and internal buffers
 *       are freed, and the ::string_v object itself is freed. The caller’s
 *       pointer is left indeterminate (dangling); assign it to NULL if it will
 *       be reused.
 *
 * @warning Do not use @p vec after this call. This function does not set the
 *          caller’s pointer to NULL. If you need scope-bound cleanup with
 *          automatic nulling, consider declaring the pointer with ::STRVEC_GBC.
 *
 * @note If your ::string_t type has a dedicated destructor (e.g., `free_string()`),
 *       prefer calling that in place of directly freeing internal fields.
 *
 * @par Example
 * @code{.c}
 * // Manual cleanup
 * string_v* vec = init_str_vector(16);
 * if (!vec) { perror("init_str_vector"); return 1; }
 * // ... use vec ...
 * free_str_vector(vec);
 * vec = NULL;  // avoid dangling pointer
 *
 * // Or with GCC/Clang auto-cleanup:
 * // string_v* vec STRVEC_GBC = init_str_vector(16);
 * // ... use vec ...
 * // (no explicit free needed on scope exit)
 * @endcode
 *
 * @see init_str_vector, STRVEC_GBC
 */
void free_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
* @function _free_str_vector
* @brief Helper function for garbage collection of string vectors
*
* Used with STRVEC_GBC macro for automatic cleanup.
*
* @param vec Double pointer to string vector to free
* @return void
*/
void _free_str_vector(string_v** vec);
// --------------------------------------------------------------------------------

/**
 * @brief Cleanup hook for ::STRVEC_GBC; frees and NULLs a ::string_v* variable.
 *
 * Designed to be used with the GNU/Clang `cleanup` attribute. The compiler
 * passes the address of the annotated variable (i.e., a `string_v**`) to this
 * function at scope exit. If non-NULL and pointing to a non-NULL object, it
 * calls ::free_str_vector() and then sets the caller’s pointer to NULL to
 * avoid dangling references.
 *
 * **Error reporting:**
 * - On invalid input (@p vec is NULL), the function does nothing and sets
 *   `errno` to `EINVAL`.
 * - On valid input, no `errno` is set on success.*
 *
 * @param vec  Address of a `string_v*` local variable (may be NULL or point to NULL).
 *
 * @retval void
 *
 * @pre  `vec` either equals NULL or points to a valid `string_v*` variable.
 * @post If `vec && *vec`, the vector is freed and `*vec` is set to NULL.
 *
 * @par Example
 * @code{.c}
 * // The compiler emits a call equivalent to: _free_str_vector(&vec);
 * string_v* vec STRVEC_GBC = init_str_vector(8);
 * // ... use vec ...
 * @endcode
 *
 * @see STRVEC_GBC, free_str_vector
 */
#if defined(__GNUC__) || defined(__clang__)
    #define STRVEC_GBC __attribute__((cleanup(_free_str_vector)))
#else
    // Portable fallback: macro becomes a no-op on non-GNU compilers.
    #define STRVEC_GBC
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief Reverse the order of elements in a string vector in place.
 *
 * Reverses the opaque ::string_v created by ::init_str_vector() by swapping the
 * first and last elements, then the second and second-to-last, and so on. The
 * operation is **in place**: no element buffers are copied or reallocated;
 * ownership of each element’s internal string simply moves to the mirror index
 * (via ::swap_string). Capacity is unchanged.
 *
 * Error handling:
 * - On invalid input, the function returns immediately, sets `errno` (via
 *   ::set_errno_from_error), and records an internal ::ErrorCode retrievable with
 *   ::get_str_vector_error().
 *   - ::NULL_POINTER → `EINVAL` if @p vec is non-NULL but uninitialized (e.g.,
 *     internal storage missing). If @p vec itself is NULL, `errno` is set to `EINVAL`.
 * - On success (including the trivial cases of size 0 or 1), `errno` is **not**
 *   modified and the vector’s internal error state is set to ::NO_ERROR.
 *
 * @param vec  Opaque pointer to a valid string vector.
 *
 * @return void
 *
 * @post On success:
 * - The element order is reversed: item formerly at index @c i moves to
 *   index @c (size-1-i).
 * - Element ownership is swapped; underlying string buffers are not duplicated.
 * - Capacity and total size are unchanged.
 *
 * **thread_safety**
 * - **Not thread-safe** for concurrent mutation of the same vector; use external
 *   synchronization if other threads may modify the vector.
 * - Concurrent readers must not assume stable ordering while this runs.
 *
 * **complexity**
 * - Let @c m be the number of elements. Performs ~@c m/2 swaps.
 *   Time: **O(m)**; Space: **O(1)**.
 *
 * @warning Any previously held indices/iterators to elements will now refer to
 *          different logical positions due to the reversal.
 *
 * @par Example
 * @code{.c}
 * // v contains: ["alpha", "beta", "gamma"]
 * reverse_str_vector(v);
 * // v now contains: ["gamma", "beta", "alpha"]
 * @endcode
 *
 * @see swap_string, init_str_vector, get_str_vector_error,
 *      error_to_string, set_errno_from_error
 */

void reverse_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Swap two ::string_t objects by value (shallow, O(1)).
 *
 * Exchanges the complete contents of @p a and @p b, including ownership of any
 * internal buffers. This is a **shallow** swap: the character buffers are not
 * copied or reallocated; only the owners of those buffers are exchanged. If
 * @p a and @p b are the same pointer, the function is a no-op.
 *
 * Error handling:
 * - On invalid input, the function returns immediately and sets `errno`
 *   (via ::set_errno_from_error) without modifying either object’s buffers.
 *   - If exactly one pointer is non-NULL, that object’s internal error slot is
 *     set to ::NULL_POINTER and `errno` is set accordingly.
 *   - If both pointers are NULL, `errno` is set to `EINVAL`.
 * - On success (both non-NULL), `errno` is **not** modified and both objects’
 *   internal error slots are set to ::NO_ERROR.
 *
 * @param a  Pointer to the first ::string_t.
 * @param b  Pointer to the second ::string_t.
 *
 * @return void
 *
 * @post On success:
 * - @p a receives the prior contents of @p b, and @p b receives the prior contents
 *   of @p a, including buffer pointers, lengths, and capacities.
 * - Both objects report ::NO_ERROR internally.
 *
 * **thread_safety**
 * - **Not thread-safe** if the same ::string_t is accessed concurrently from
 *   multiple threads. External synchronization is required for concurrent mutation.
 *
 * **complexity**
 * - **O(1)** time and **O(1)** additional space.
 *
 * @warning This is a shallow swap. Any external pointers you held to the
 *          character buffers prior to the call now refer to the *other* object’s
 *          buffer after the swap.
 *
 * @par Example
 * @code{.c}
 * string_t *x = init_string("left");
 * string_t *y = init_string("right");
 * if (!x || !y) { // hanlde allocation failure }
 *
 * swap_string(x, y);
 * // Now *x owns "right", *y owns "left"
 *
 * free_string(x);
 * free_string(y);
 * @endcode
 *
 * @see set_errno_from_error, error_to_string, init_string, free_string
 */
void swap_string(string_t* a, string_t* b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Binary search a string vector for a NUL-terminated C string.
 *
 * Searches the opaque ::string_v (created by ::init_str_vector()) for an
 * element whose contents are equal to @p value (lexicographic equality). If
 * @p sort_first is `true`, the vector is first sorted in **ascending** order
 * (via ::sort_str_vector(FORWARD)); otherwise, the caller must ensure the
 * vector is already sorted ascending according to the library comparator.
 *
 * On success, returns the zero-based index of a matching element and leaves
 * `errno` unchanged. On failure (invalid input, empty vector, or no match),
 * returns `SIZE_MAX` and sets an ::ErrorCode internally along with `errno`
 * via ::set_errno_from_error().
 *
 * Semantics:
 * - Comparison is performed against each element’s character buffer using
 *   lexicographic order (like `strcmp`).
 * - If multiple equal elements exist, the index of **one** matching element
 *   is returned (no stability/first-occurrence guarantee).
 * - If @p sort_first is `true`, this call **reorders** the vector (in place).
 *
 * Error handling (returns `SIZE_MAX`):
 * - ::NULL_POINTER → `EINVAL` if @p vec is non-NULL but uninitialized
 *   (e.g., internal storage missing) or if @p value is NULL.
 *   If @p vec itself is NULL, `errno` is set to `EINVAL`.
 * - ::NOT_FOUND → `ENOENT` if no matching element exists, or if the vector
 *   is empty.
 * - ::STATE_CORRUPT → `EFAULT` if a searched element’s internal string buffer
 *   is unexpectedly NULL (defensive check).
 * - Any error emitted by ::sort_str_vector(FORWARD) when @p sort_first is `true`
 *   is propagated (e.g., ::NULL_POINTER).
 *
 * @param vec         Opaque pointer to a string vector.
 * @param value       Pointer to a readable, NUL-terminated key to search for.
 * @param sort_first  If `true`, sort ascending before searching; if `false`,
 *                    the vector must already be sorted ascending.
 *
 * @return Index of a matching element on success; `SIZE_MAX` on failure/not found.
 *
 * @post
 * - On success: `errno` is not modified; internal error is ::NO_ERROR.
 * - On failure: internal error is set (see above) and `errno` is set accordingly.
 * - If @p sort_first is `true`, the vector is sorted in ascending order.
 *
 * @thread_safety
 * - **Not thread-safe** for concurrent mutation of the same vector. If
 *   @p sort_first is `true`, this function also reorders elements and thus
 *   invalidates any external indices/iterators.
 *
 * @complexity
 * - If @p sort_first is `false` and the vector is already sorted: **O(log m)**,
 *   where @c m is the number of elements.
 * - If @p sort_first is `true`: **O(m log m)** for the sort + **O(log m)**
 *   for the search (overall dominated by the sort).
 *
 * @example
 * @code{.c}
 * size_t idx = binary_search_str_vector(v, "needle", false); // assumes v is sorted asc
 * if (idx == SIZE_MAX) {
 *     ErrorCode ec = get_str_vector_error(v);
 *     if (ec == NOT_FOUND) {
 *         // handle no-match case
 *     } else {
 *         perror("binary_search_str_vector"); // errno set by callee
 *     }
 * } else {
 *     // found at idx
 * }
 * @endcode
 *
 * @see sort_str_vector, get_str_vector_error, error_to_string, set_errno_from_error
 */
size_t binary_search_str_vector(string_v* vec, char* value, bool sort_first);
// ================================================================================
// ================================================================================ 
// GENERIC MACROS 

#define str_size(dat) _Generic((dat), \
    string_t*: string_size, \
    string_v*: str_vector_size, \
    void*: string_size, \
    default: string_size) (dat)
// --------------------------------------------------------------------------------

#define str_alloc(dat) _Generic((dat), \
    string_t*: string_alloc, \
    string_v*: str_vector_alloc, \
    void*: string_alloc, \
    default: string_alloc) (dat)
// -------------------------------------------------------------------------------- 

#define str_error(dat) _Generic((dat), \
    string_t*: get_string_error, \
    string_v*: get_str_vector_error, \
    void*: get_string_error, \
    default: get_string_error) (dat)
// ================================================================================
// ================================================================================

#ifndef ITER_DIR_H
#define ITER_DIR_H
    /**
     * @brief An enum containing keywords for an iterator 
     *
     * This enum contains keywords that are used to describe the order of iteration
     * within an iterator 
     *
     * @attribute FORWARD Keyword to command a forward iteration 
     * @attribute REVERSE Keyword to command a reverese iteration
     */
    typedef enum {
        FORWARD,
        REVERSE
    }iter_dir;
#endif /* ITER_DIR_H*/
// --------------------------------------------------------------------------------

/**
 * @brief Sort the elements of a string vector in place (ascending or descending).
 *
 * Reorders the opaque ::string_v created by ::init_str_vector() according to
 * lexicographic order using the library comparator (`compare_strings_string`).
 * The sort is performed **in place** and is **unstable**: element ownership
 * (their internal string buffers) is swapped/moved, but buffers are not copied.
 *
 * Algorithm (implementation detail):
 * - Hybrid quicksort with median-of-three pivot selection and tail-recursion
 *   elimination to bound stack depth.
 * - Small partitions finalized with insertion sort (cache-friendly).
 * - Direction selects order:
 *   - ::FORWARD  → ascending
 *   - ::REVERSE  → descending
 *
 * Error handling:
 * - If @p vec is NULL or not initialized (e.g., internal storage missing),
 *   sets the vector’s internal error to ::NULL_POINTER, sets `errno`
 *   accordingly via ::set_errno_from_error(), and returns without sorting.
 * - If the vector has fewer than two elements, the function returns without
 *   modifying `errno` and sets the internal error to ::NO_ERROR.
 * - On successful sort, sets the internal error to ::NO_ERROR and does **not**
 *   modify `errno`.
 * - You may retrieve the last error via ::get_str_vector_error().
 *
 * @param vec        Opaque pointer to a valid string vector.
 * @param direction  ::FORWARD for ascending, ::REVERSE for descending.
 *
 * @return void
 *
 * @post On success:
 * - The elements are permuted into the requested order.
 * - Capacity is unchanged (sorting does not reallocate the vector storage).
 * - Internal error state is ::NO_ERROR.
 *
 * **thread_safety**
 * - **Not thread-safe** for concurrent mutation of the same vector; use external
 *   synchronization if multiple threads may sort/modify concurrently.
 * - Any concurrently held raw element pointers/iterators become invalid due to
 *   reordering.
 *
 * **complexity**
 * - Let @c m be the number of elements:
 *   - Average: **O(m log m)**
 *   - Worst-case: **O(m²)** (mitigated by median-of-three)
 *   - Extra space: **O(log m)** for recursion (further reduced by tail-call
 *     elimination on the larger partition).
 *
 * @warning The sort is **unstable**: equal keys may not preserve their original
 *          relative order. Use a stable algorithm if stability is required.
 *
 * @note Underlying string buffers are not duplicated or reallocated; only the
 *       owning element slots are swapped.
 *
 * @par Example
 * @code{.c}
 * sort_str_vector(v, FORWARD);
 * if (get_str_vector_error(v) != NO_ERROR) {
 *     perror("sort_str_vector"); // errno set on error
 * }
 * @endcode
 *
 * @see init_str_vector, reverse_str_vector, compare_strings_string,
 *      get_str_vector_error, set_errno_from_error
 */

void sort_str_vector(string_v* vec, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Split a \c string_t into tokens using a set of delimiter characters.
 *
 * Scans the bytes of @p str and produces a new string vector (\c string_v)
 * containing copies of each maximal run of non-delimiter characters. The set
 * of delimiters is specified by @p delim as a NUL-terminated C string; **each
 * byte in @p delim is treated as an independent delimiter character** (like
 * \c strpbrk), not as multi-character separators. Consecutive delimiters are
 * collapsed—no empty tokens are produced. Leading and trailing delimiters are
 * skipped.
 *
 * Implementation notes:
 * - The function first calls \c token_count(str, delim) to pre-compute capacity.
 *   If it returns zero, this function returns \c NULL and forwards \c errno from
 *   \c token_count.
 * - A new vector is created with \c init_str_vector(count). Each token is copied
 *   and appended via ::push_back_str_vector(). On any append failure, the vector
 *   is freed and \c NULL is returned (with \c errno set by the failing call).
 * - Token detection is byte-wise and locale-agnostic (no multibyte/Unicode
 *   awareness); comparisons are raw \c char equality to any delimiter byte.
 *
 * Error handling:
 * - Returns \c NULL and sets \c errno in these cases:
 *   - \c EINVAL if @p str is NULL, its internal buffer is NULL, or @p delim is NULL.
 *   - Errors signaled by \c token_count(str, delim) when no tokens are found or
 *     an input is invalid (this function forwards \c errno as set by \c token_count).
 *   - \c ENOMEM if vector allocation via ::init_str_vector fails, or if any
 *     element allocation within ::push_back_str_vector fails (the latter sets
 *     \c errno itself; this function forwards it).
 * - On success, \c errno is **not** modified.
 *
 * @param str    Pointer to a readable \c string_t whose contents are to be tokenized.
 *               Assumes the content has no embedded NUL bytes that must be preserved
 *               within tokens (see warning below).
 * @param delim  NUL-terminated C string; each character is treated as a delimiter.
 *
 * @return Pointer to a newly allocated \c string_v on success; \c NULL on failure.
 *         The caller owns the returned vector and must release it with
 *         ::free_str_vector().
 *
 * @post On success:
 * - The returned vector contains one \c string_t per token, each owning an
 *   independent, NUL-terminated copy of that token.
 * - The order of tokens reflects their left-to-right order in @p str.
 *
 * @thread_safety
 * - Not thread-safe with respect to the returned vector (it is a new, mutable
 *   object). Reading @p str concurrently is safe if no other thread mutates it.
 *
 * @complexity
 * - Let \c n = \c str->len. Scanning is O(n). Total copying is proportional to the
 *   number of non-delimiter bytes, so overall O(n) time; O(k) auxiliary memory
 *   in the returned vector, where \c k is the number of tokens.
 *
 * @warning
 * - This implementation builds each token as a temporary NUL-terminated C string
 *   before pushing; if @p str contains embedded NUL bytes within a token, they
 *   will truncate the token at the first NUL when measured by \c strlen() inside
 *   the push routine. If you must support embedded NULs, switch to a length-based
 *   element initializer instead of C-string-based copies.
 * - For very large tokens, the temporary token buffer is stack-allocated as a VLA;
 *   extremely large tokens can exhaust stack space. Consider refactoring to a
 *   heap buffer if unbounded token sizes are expected.
 *
 * @par Example
 * @code{.c}
 * string_t *s = init_string("a,,b;c  d");
 * const char *delims = ",; ";
 * string_v *v = tokenize_string(s, delims);
 * if (!v) {
 *     perror("tokenize_string");
 *     free_string(s);
 *     return 1;
 * }
 * // v now contains: ["a", "b", "c", "d"]
 * free_str_vector(v);
 * free_string(s);
 * @endcode
 *
 * @see token_count, init_str_vector, push_back_str_vector, free_str_vector
 */
string_v* tokenize_string(const string_t* str, const char* delim);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
// ================================================================================
// ================================================================================ 
#endif /* cplusplus */
#endif /* c_string_H */
// ================================================================================
// ================================================================================
// eof
