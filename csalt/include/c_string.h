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
#include "error_codes.h"

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
 * - Returns `NULL` when the byte is not found; in this case `errno` is left
 *   unchanged. On valid inputs, `str->error` may be set to ::NO_ERROR.
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
 * - Returns `NULL` when the byte is not found; in this case `errno` is left
 *   unchanged. On valid inputs, `str->error` may be set to ::NO_ERROR.
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
char* first_substr__type_mismatch(string_t*, void*);
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

bool is_string_ptr(string_t* str, const char* ptr, bool include_terminator);
// --------------------------------------------------------------------------------

bool drop_lit_substr(string_t* string, const char* substring, char* min_ptr,
                     char* max_ptr);
// -------------------------------------------------------------------------------- 

bool drop_string_substr(string_t* string, const string_t* substring, char* min_ptr,
                        char* max_ptr);
// -------------------------------------------------------------------------------- 

#define drop_substr(string, substr, min_ptr, max_ptr) _Generic((substr), \
    char*: drop_lit_substr, \
    string_t*: drop_string_substr) (string, substr, min_ptr, max_ptr)
// -------------------------------------------------------------------------------- 

bool replace_lit_substr(string_t* string, const char* pattern, const char* replace_string,
                        char* min_ptr, char* max_ptr);
// --------------------------------------------------------------------------------

bool replace_string_substr(string_t* string, const string_t* pattern, const string_t* replace_string,
                           char* min_ptr, char* max_ptr);
// -------------------------------------------------------------------------------- 

#define replace_substr(string, pattern, replace_string, min_ptr, max_ptr) _Generic((pattern), \
    char*: replace_lit_substr, \
    string_t*: replace_string_substr) (string, pattern, replace_string, min_ptr, max_ptr)
// ================================================================================
// ================================================================================ 

void to_upper_char(char* val);
// --------------------------------------------------------------------------------

void to_lower_char(char* val);
// --------------------------------------------------------------------------------

void to_uppercase(string_t* val);
// --------------------------------------------------------------------------------

void to_lowercase(string_t* val);
// --------------------------------------------------------------------------------

string_t* pop_string_token(string_t* str_struct, char token);
// --------------------------------------------------------------------------------

size_t token_count(const string_t* str, const char* delim);
// --------------------------------------------------------------------------------

char get_char(string_t* str, size_t index);
// --------------------------------------------------------------------------------

void replace_char(string_t* str, size_t index, char value);
// --------------------------------------------------------------------------------

void trim_leading_whitespace(string_t* str);
// --------------------------------------------------------------------------------

void trim_trailing_whitespace(string_t* str);
// --------------------------------------------------------------------------------

void trim_all_whitespace(string_t* str);
// ================================================================================
// ================================================================================
// STRING ITERATOR

typedef struct str_iter {
    char* (*begin) (string_t *s);
    char* (*end) (string_t *s);
    void (*next) (char** current);
    void (*prev) (char** current);
    char (*get) (char** current);
} str_iter;
// -------------------------------------------------------------------------------- 

str_iter init_str_iter();
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
* @function init_str_vector
* @brief Initializes a new string vector with specified initial capacity
*
* @param buffer Initial capacity (number of strings) to allocate
* @return Pointer to new string_v object, or NULL on allocation failure
*         Sets errno to ENOMEM if memory allocation fails
*/
string_v* init_str_vector(size_t buffer);
// --------------------------------------------------------------------------------

/**
* @function push_back_str_vector
* @brief Adds a string to the end of the vector
*
* Automatically resizes the vector if necessary.
*
* @param vec Target string vector
* @param value String to add
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure
*/
bool push_back_str_vector(string_v* vec, const char* value);
// --------------------------------------------------------------------------------

/**
* @function push_front_str_vector
* @brief Adds a string to the beginning of the vector
*
* Shifts existing elements right and automatically resizes if necessary.
*
* @param vec Target string vector
* @param value String to add
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure
*/
bool push_front_str_vector(string_v* vec, const char* value);
// --------------------------------------------------------------------------------

/**
* @function insert_str_vector
* @brief Inserts a string at specified index in the vector
*
* Shifts elements right starting at index and resizes if necessary.
*
* @param vec Target string vector
* @param value String to insert
* @param index Position to insert at
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or index out of bounds, ENOMEM on allocation failure
*/
bool insert_str_vector(string_v* vec, const char* value, size_t index);
// --------------------------------------------------------------------------------

/**
* @function str_vector_index
* @brief Retrieves pointer to string_t at specified index
*
* @param vec Source string vector
* @param index Position to access
* @return Pointer to string_t object, or NULL on error
*         Sets errno to EINVAL for NULL input or ERANGE if index out of bounds
*/
const string_t* str_vector_index(const string_v* vec, size_t index);
// -------------------------------------------------------------------------------- 

/**
* @function str_vector_size
* @brief Returns current number of strings in vector
*
* @param vec String vector to query
* @return Number of strings in vector, or LONG_MAX on error
*         Sets errno to EINVAL for NULL input
*/
size_t str_vector_size(const string_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function str_vector_alloc
* @brief Returns current allocation size of vector
*
* @param vec String vector to query
* @return Current allocation capacity, or LONG_MAX on error
*         Sets errno to EINVAL for NULL input
*/
size_t str_vector_alloc(const string_v* vec);
// --------------------------------------------------------------------------------

/**
* @function pop_back_str_vector
* @brief Removes and returns last string in vector
*
* @param vec Source string vector
* @return Pointer to removed string_t object, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
string_t* pop_back_str_vector(string_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function pop_front_str_vector
* @brief Removes and returns first string in vector
*
* Shifts remaining elements left.
*
* @param vec Source string vector
* @return Pointer to removed string_t object, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
string_t* pop_front_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
* @function pup_any_str_vector
* @brief Removes and returns string at specified index
*
* Shifts remaining elements left to fill gap.
*
* @param vec Source string vector
* @param index Position to remove from
* @return Pointer to removed string_t object, or NULL on error
*         Sets errno to EINVAL for NULL input or ERANGE if index out of bounds
*/
string_t* pop_any_str_vector(string_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
* @function delete_back_str_vector
* @brief Removes the last string in a vector
*
* @param vec Source string vector
* @return true if successful, false otherwise, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
bool delete_back_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
* @function delete_front_str_vector
* @brief Removes the first string in a vector
*
* @param vec Source string vector
* @return true if successful, false otherwise, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
bool delete_front_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
* @function delete_any_str_vector
* @brief Removes any string in a vector
*
* @param vec Source string vector
* @param index The index to be deleted
* @return true if successful, false otherwise, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
bool delete_any_str_vector(string_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
* @function free_str_vector
* @brief Frees all memory associated with string vector
*
* Frees all contained strings and the vector itself.
*
* @param vec String vector to free
* @return void
*         Sets errno to EINVAL for NULL input
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

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro STRVEC_GBC
     * @brief A macro for enabling automatic cleanup of str_vector objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_str_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define STRVEC_GBC __attribute__((cleanup(_free_str_vector)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @function reverse_str_vector
 * @brief Reverses the order of elements in a string vector in place.
 *
 * The function reverses the order of elements by swapping elements from the
 * beginning and end of the vector until the middle is reached.
 *
 * @param vec string vector to reverse
 * @return void
 *         Sets errno to EINVAL if vec is NULL or invalid
 */
void reverse_str_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
 * @function swap_string
 * @brief Swaps the contents of two string_t objects.
 *
 * Performs an in-place swap of all attributes (str pointer, length, and allocation)
 * between two string_t objects. The operation maintains memory ownership and
 * allocation status.
 *
 * @param a Pointer to first string_t object
 * @param b Pointer to second string_t object
 * @return void
 *         Sets errno to EINVAL if either input is NULL
 */
void swap_string(string_t* a, string_t* b);
// -------------------------------------------------------------------------------- 

/**
* @function binary_search_str_vector
* @brief Searches a float vector to find the index where a value exists
*
* @param vec string vector object
* @param value The value to search for
* @param sort_first true if the vector or array needs to be sorted, false otherwise
* @return The index where a value exists, SIZE_MAX if the value is not in the array.
*         Sets errno to EINVAL if vec is NULL or invalid, ENODATA if the array is 
*         not populated
*/
size_t binary_search_str_vector(string_v* vec, char* value, bool sort_first);
// ================================================================================
// ================================================================================ 
// DICTIONARY PROTOTYPES

/**
 * @typedef dict_t
 * @brief Opaque struct representing a dictionary.
 *
 * This structure encapsulates a hash table that maps string keys to int values.
 * The details of the struct are hidden from the user and managed internally.
 */
// typedef struct dict_t dict_t;
// // --------------------------------------------------------------------------------
//
// /**
//  * @brief Initializes a new dictionary.
//  *
//  * Allocates and initializes a dictionary object with a default size for the hash table.
//  *
//  * @return A pointer to the newly created dictionary, or NULL if allocation fails.
//  */
// dict_t* init_dict();
// // -------------------------------------------------------------------------------- 
//
// /**
//  * @brief Inserts a key-value pair into the dictionary.
//  *
//  * Adds a new key-value pair to the dictionary. If the key already exists, the function
//  * does nothing and returns false. If the dictionary's load factor exceeds a threshold,
//  * it automatically resizes.
//  *
//  * @param dict Pointer to the dictionary.
//  * @param key The key to insert.
//  * @param value The value associated with the key.
//  * @return true if the key-value pair was inserted successfully, false otherwise.
//  */
// bool insert_dict(dict_t* dict, const char* key, size_t value);
// // --------------------------------------------------------------------------------
//
// /**
//  * @brief Removes a key-value pair from the dictionary.
//  *
//  * Finds the specified key in the dictionary, removes the associated key-value pair,
//  * and returns the value.
//  *
//  * @param dict Pointer to the dictionary.
//  * @param key The key to remove.
//  * @return The value associated with the key if it was found and removed; FLT_MAX otherwise.
//  */
// size_t pop_dict(dict_t* dict,  char* key);
// // --------------------------------------------------------------------------------
//
// /**
//  * @brief Retrieves the value associated with a key.
//  *
//  * Searches the dictionary for the specified key and returns the corresponding value.
//  *
//  * @param dict Pointer to the dictionary.
//  * @param key The key to search for.
//  * @return The value associated with the key, or FLT_MAX if the key is not found.
//  */
// const size_t get_dict_value(const dict_t* dict, char* key);
// // --------------------------------------------------------------------------------
//
// /**
//  * @brief Frees the memory associated with the dictionary.
//  *
//  * Releases all memory allocated for the dictionary, including all key-value pairs.
//  *
//  * @param dict Pointer to the dictionary to free.
//  */
// void free_dict(dict_t* dict);
// // --------------------------------------------------------------------------------
//
// /**
//  * @brief Safely frees a dictionary and sets the pointer to NULL.
//  *
//  * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
//  * after being freed. Useful for preventing dangling pointers.
//  *
//  * @param dict Pointer to the dictionary pointer to free.
//  */
// void _free_dict(dict_t** dict);
// // --------------------------------------------------------------------------------
//
// #if defined(__GNUC__) || defined (__clang__)
//     /**
//      * @macro DICT_GBC
//      * @brief A macro for enabling automatic cleanup of dict_t objects.
//      *
//      * This macro uses the cleanup attribute to automatically call `_free_vector`
//      * when the scope ends, ensuring proper memory management.
//      */
//     #define DICT_GBC __attribute__((cleanup(_free_dict)))
// #endif
// // --------------------------------------------------------------------------------
//
// /**
//  * @brief Updates the value associated with a key in the dictionary.
//  *
//  * Searches for the specified key in the dictionary and updates its value.
//  * If the key does not exist, the function takes no action.
//  *
//  * @param dict Pointer to the dictionary.
//  * @param key The key to update.
//  * @param value The new value to associate with the key.
//  */
// bool update_dict(dict_t* dict, char* key, size_t value);
// // --------------------------------------------------------------------------------
//
// /**
//  * @brief Gets the number of non-empty buckets in the dictionary.
//  *
//  * Returns the total number of buckets in the hash table that contain at least one key-value pair.
//  *
//  * @param dict Pointer to the dictionary.
//  * @return The number of non-empty buckets.
//  */
// const size_t dict_size(const dict_t* dict);
// // --------------------------------------------------------------------------------
//
// /**
//  * @brief Gets the total capacity of the dictionary.
//  *
//  * Returns the total number of buckets currently allocated in the hash table.
//  *
//  * @param dict Pointer to the dictionary.
//  * @return The total number of buckets in the dictionary.
//  */
// const size_t dict_alloc(const dict_t* dict);
// // --------------------------------------------------------------------------------
//
// /**
//  * @brief Gets the total number of key-value pairs in the dictionary.
//  *
//  * Returns the total number of key-value pairs currently stored in the dictionary.
//  *
//  * @param dict Pointer to the dictionary.
//  * @return The number of key-value pairs.
//  */
// const size_t dict_hash_size(const dict_t* dict);
// // --------------------------------------------------------------------------------
//
// /**
// * @function is_key_value
// * @brief returns true if a key value pair exists in dictionary
// *
// * @param str dict_t object to tokenize
// * @param key A string literal representing a key word
// * @return returns true of key value pair exists, false otherwise.  if char 
// *         pointer or dict_t data type is a null pointer will sett errno 
// *         to ENOMEM and return false
// */
// bool is_key_value(const dict_t* dict, const char* key);
// ================================================================================ 
// ================================================================================ 
// GENERIC MACROS 

/**
* @macro s_size
* @brief Generic macro to return the populated length of a string data type such as
*        string_t and string_v
*
* @param dat A pointer to a string_t or strinv_v data type
* @return The populated length of a string data type
*/
#define s_size(dat) _Generic((dat), \
    string_t*: string_size, \
    string_v*: str_vector_size) (dat)
// --------------------------------------------------------------------------------

/**
* @macro s_alloc
* @brief Generic macro to return the allocated size of a string data type such as
*        string_t and string_v
*
* @param dat A pointer to a string_t or strinv_v data type
* @return The allocated length of a string data type
*/
#define s_alloc(dat) _Generic((dat), \
    string_t*: string_alloc, \
    string_v*: str_vector_alloc) (dat)
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
* @function sort_str_vector
* @brief Sorts a string vector in ascending or descending order.
*
* Uses an optimized QuickSort algorithm with median-of-three pivot selection
* and insertion sort for small subarrays. Sort direction is determined by
* the iter_dir parameter.
*
* @param vec string vector to sort
* @param direction FORWARD for ascending order, REVERSE for descending
* @return void
*         Sets errno to EINVAL if vec is NULL or invalid
*/
void sort_str_vector(string_v* vec, iter_dir direction);
// --------------------------------------------------------------------------------

/**
* @function tokenize_string
* @brief Splits a string into tokens based on delimiter characters.
*
* Creates a vector of strings containing each token found in the input string.
* Multiple consecutive delimiters are treated as a single delimiter, and
* the original string is preserved.
*
* @param str string_t object to tokenize
* @param delim string containing delimiter characters (e.g., " ,;")
* @return string vector containing tokens, or NULL on error
*         Sets errno to EINVAL for NULL inputs, ENOMEM for allocation failure
*
* Example:
*     string_t* str = init_string("hello,world;test");
*     string_v* tokens = tokenize_string(str, ",;");
*     // tokens now contains ["hello", "world", "test"]
*/
string_v* tokenize_string(const string_t* str, const char* delim);
// // -------------------------------------------------------------------------------- 
//
// /**
// * @function get_dict_keys
// * @brief Returns a string vector of dictionary keys
// *
// * @param dict A dict_t type
// * @return string vector containing dictionary keys, or NULL on error
// *         Sets errno to EINVAL for NULL inputs, ENOMEM for allocation failure
// */
// string_v* get_dict_keys(const dict_t* dict);
// // --------------------------------------------------------------------------------
//
// /**
// * @function count_words
// * @brief Returns a dictionary of words that occur in a string and the number 
// *        of their occurrances
// *
// * @param dict A dict_t type
// * @param delim A stirng literal of delimters used to parse a string
// * @return A dictionary containing dictionary keys, or NULL on error
// *         Sets errno to EINVAL for NULL inputs, ENOMEM for allocation failure
// *
// */
// dict_t* count_words(const string_t* str, const char* delim);
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
