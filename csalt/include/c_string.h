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
 * - Returns `false` and sets `errno` to `EINVAL` if any argument is NULL or if
 *   an object's internal buffer is NULL. In this case, @p str1->error is set
 *   to `NULL_POINTER` if the destination buffer was missing.
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
 * @warning Do not pass the same object as both parameters. Overlapping source
 *          and destination leads to undefined behavior.
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
 * @function string_lit_concat
 * @brief Concatenates a string literal to a string_t object.
 *
 * @param str1 A pointer to the destination string_t object.
 * @param literal A null-terminated C string to append to the string_t object.
 * @return true if successful, false on failure. Sets errno to ENOMEM if memory
 *         allocation fails or EINVAL if either input is NULL.
 */
bool string_lit_concat(string_t* str, const char* string);
// --------------------------------------------------------------------------------

/**
 * @macro string_concat
 * @brief A generic macro that selects the appropriate concatenation function
 *        based on the type of the second argument.
 *
 * If the second argument is a `char*`, it calls `string_lit_concat`.
 * Otherwise, it calls `string_string_concat`.
 */
#define string_concat(str_one, str_two) _Generic((str_two), \
    char*: string_lit_concat, \
    default: string_string_concat) (str_one, str_two)
// --------------------------------------------------------------------------------

/**
 * @function compare_strings_lit
 * @brief Compares a string_t object with a string literal.
 *
 * The comparison is lexicographical and case-sensitive.
 *
 * @param str_struct A pointer to the string_t object.
 * @param string A null-terminated C string to compare with.
 * @return An integer less than, equal to, or greater than zero if the string_t
 *         is found, respectively, to be less than, to match, or to be greater than `string`.
 *         Returns INT_MIN on error (sets errno to EINVAL).
 */
int compare_strings_lit(const string_t* str_struct, const char* string);
// --------------------------------------------------------------------------------

/**
 * @function compare_strings_string
 * @brief Compares two string_t objects.
 *
 * The comparison is lexicographical and case-sensitive.
 *
 * @param str_struct_one A pointer to the first string_t object.
 * @param str_struct_two A pointer to the second string_t object.
 * @return An integer less than, equal to, or greater than zero if the first string_t
 *         is found, respectively, to be less than, to match, or to be greater than the second string_t.
 *         Returns INT_MIN on error (sets errno to EINVAL).
 */
int compare_strings_string(const string_t* str_struct_one, string_t* str_struct_two);
// --------------------------------------------------------------------------------

/**
 * @macro compare_strings
 * @brief A generic macro that selects the appropriate string comparison function
 *        based on the type of the second argument.
 *
 * If the second argument is a `char*`, it calls `compare_strings_lit`.
 * Otherwise, it calls `compare_strings_string`.
 */
#define compare_strings(str_one, str_two) _Generic((str_two), \
    char*: compare_strings_lit, \
    default: compare_strings_string) (str_one, str_two)
// --------------------------------------------------------------------------------

/**
 * @function copy_string
 * @brief Creates a deep copy of a string data type
 *
 * @param str A string_t data type
 * @return A string_t data type with the exact contents of str
 */
string_t* copy_string(const string_t *str);
// --------------------------------------------------------------------------------

/**
 * @function reserve_string
 * @brief Reserves memory for the string_t data type to avoid constant memory allocations
 *
 * Will not allow the user to reserve less memory than exists at function call 
 *
 * @param str A string_t data type
 * @param len The buffer length to allocate in string
 * @return true of allocation is successful, false otherwise
 */
bool reserve_string(string_t* str, size_t len);
// -------------------------------------------------------------------------------- 

/**
 * @brief Tims the string memory to the minimum necessary size 
 *
 * THis function will determine the minimum memory allocation needed to fit 
 * the string and will resize to the minimum memory if oversized.
 *
 * @param str A string container of type string_t
 * @return true if operation is succesful, false otherwise with stderr printout
 */
bool trim_string(string_t* str);
// -------------------------------------------------------------------------------- 

/**
 * @brief Finds the first occurance of a char between two pointers
 *
 * This function deterimines the first appearance of a char value in 
 * a string literal.
 *
 * @param str A pointer to the string_t data type
 * @param value The char value being search for in the string_t data type
 * @return A char pointer to character value or a NULL pointer.
 */
char* first_char_occurance(string_t* str, char value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Finds the last occurance of a char between two pointers
 *
 * This function deterimines the first appearance of a char value in 
 * a string literal.
 *
 * @param str A pointer to the string_t data type
 * @param value The char value being search for in the string_t data type
 * @return A char pointer to character value or a NULL pointer.
 */
char* last_char_occurance(string_t* str, char value);
// -------------------------------------------------------------------------------- 

/**
* @function first_lit_substr_occurance
* @brief Finds the first occurrence of a C string literal substring within a string_t object.
*
* @param str The string_t object to search within
* @param sub_str The C string literal to search for
* @return Pointer to the beginning of the first occurrence of sub_str, or NULL if not found
*         Sets errno to EINVAL if either input is NULL
*/
char* first_lit_substr_occurrence(string_t* str, char* sub_str);
// --------------------------------------------------------------------------------

/**
* @function first_string_substr_occurance
* @brief Finds the first occurrence of a string_t substring within another string_t object.
*
* @param str_1 The string_t object to search within
* @param str_2 The string_t substring to search for
* @return Pointer to the beginning of the first occurrence of str_2, or NULL if not found
*         Sets errno to EINVAL if either input is NULL
*/
char* first_string_substr_occurrence(string_t* str_1, string_t* str_2);
// -------------------------------------------------------------------------------- 

/**
* @macro first_substr_occurance
* @brief A generic macro that selects the appropriate substring search function
*        based on the type of the second argument.
*
* If the second argument is a char*, calls first_lit_substr_occurance.
* If the second argument is a string_t*, calls first_string_substr_occurance.
*
* Example usage:
*     first_substr_occurance(str, "substring")     // Uses literal version
*     first_substr_occurance(str1, str2)          // Uses string_t version
*/
#define first_substr_occurrence(str1, str2) _Generic((str2), \
    char*: first_lit_substr_occurrence, \
    string_t*: first_string_substr_occurrence) (str1, str2)
// -------------------------------------------------------------------------------- 

/**
* @function last_lit_substr_occurance
* @brief Finds the last occurrence of a C string literal substring within a string_t object.
*
* @param str The string_t object to search within
* @param sub_str The C string literal to search for
* @return Pointer to the beginning of the last occurrence of sub_str, or NULL if not found
*         Sets errno to EINVAL if either input is NULL
*/
char* last_lit_substr_occurrence(string_t* str, char* sub_str);
// --------------------------------------------------------------------------------

/**
* @function last_string_substr_occurance
* @brief Finds the last occurrence of a string_t substring within another string_t object.
*
* @param str_1 The string_t object to search within
* @param str_2 The string_t substring to search for
* @return Pointer to the beginning of the last occurrence of str_2, or NULL if not found
*         Sets errno to EINVAL if either input is NULL
*/
char* last_string_substr_occurrence(string_t* str_1, string_t* str_2);
// -------------------------------------------------------------------------------- 

/**
* @macro last_substr_occurance
* @brief A generic macro that selects the appropriate substring search function
*        based on the type of the second argument.
*
* If the second argument is a char*, calls last_lit_substr_occurance.
* If the second argument is a string_t*, calls last_string_substr_occurance.
*
* Example usage:
*     last_substr_occurance(str, "substring")      // Uses literal version
*     last_substr_occurance(str1, str2)           // Uses string_t version
*/
#define last_substr_occurrence(str1, str2) _Generic((str2), \
    char*: last_lit_substr_occurrence, \
    string_t*: last_string_substr_occurrence) (str1, str2)
// --------------------------------------------------------------------------------

/**
* @function first_char
* @brief Returns a pointer to the beginning of a string
*
* @param str A string_t object
* @return Pointer to the beginning of the string NULL if not found
*         Sets errno to EINVAL if either input is NULL
*/
char* first_char(string_t* str);
// --------------------------------------------------------------------------------

/**
* @function last_char
* @brief Returns a pointer to the end of a string
*
* @param str A string_t object
* @return Pointer to the end of the string NULL if not found
*         Sets errno to EINVAL if either input is NULL
*/
char* last_char(string_t* str);
// --------------------------------------------------------------------------------

/**
* @function is_string_ptr
* @brief Deterimes if a pointer is within the bounds of a function
*
* @param str A string_t object
* @param ptr A char pointer 
* @return true if the pointer is within the string bounds, 
*         false otherwise.  If str, str->str, or ptr are NULL,
*         the function will return false and set errno to EINVAL
*/
bool is_string_ptr(string_t* str, char* ptr);
// --------------------------------------------------------------------------------

/**
* @function drop_lit_substr
* @brief Removes all occurrences of a C string literal substring between two pointers.
*
* Searches from end to beginning of the specified range and removes each occurrence
* of the substring, preserving existing spaces between words.
*
* @param string string_t object to modify
* @param substring C string literal to remove
* @param min_ptr Pointer to start of search range within string
* @param max_ptr Pointer to end of search range within string
* @return bool true if successful (including when no matches found), false on error
*         Sets errno to EINVAL if inputs are NULL or range invalid
*         Sets errno to ERANGE if pointers are out of bounds
*/
bool drop_lit_substr(string_t* string, const char* substring, char* min_ptr,
                     char* max_ptr);
// -------------------------------------------------------------------------------- 

/**
* @function drop_string_substr
* @brief Removes all occurrences of a string_t substring between two pointers.
*
* Searches from end to beginning of the specified range and removes each occurrence
* of the substring, preserving existing spaces between words.
*
* @param string string_t object to modify
* @param substring string_t object containing substring to remove
* @param min_ptr Pointer to start of search range within string
* @param max_ptr Pointer to end of search range within string
* @return bool true if successful (including when no matches found), false on error
*         Sets errno to EINVAL if inputs are NULL or range invalid
*         Sets errno to ERANGE if pointers are out of bounds
*/
bool drop_string_substr(string_t* string, const string_t* substring, char* min_ptr,
                        char* max_ptr);
// -------------------------------------------------------------------------------- 

/**
* @macro drop_substr
* @brief Generic macro that selects appropriate substring removal function based on type.
*
* If the substring is char*, calls drop_lit_substr.
* If the substring is string_t*, calls drop_string_substr.
*
* Example usage:
*     drop_substr(str, "hello", start, end)      // Uses literal version
*     drop_substr(str1, str2, start, end)        // Uses string_t version
*/
#define drop_substr(string, substr, min_ptr, max_ptr) _Generic((substr), \
    char*: drop_lit_substr, \
    string_t*: drop_string_substr) (string, substr, min_ptr, max_ptr)
// -------------------------------------------------------------------------------- 

/**
* @function replace_lit_substring
* @brief Replaces all occurrences of a C string literal pattern with a replacement string
*        between two specified pointers in a string_t object.
*
* If the replacement string is longer than the pattern, memory will be reallocated as needed.
* If shorter, the string will be compacted. The function maintains proper null termination.
*
* @param string string_t object to modify
* @param pattern const C string literal to search for and replace
* @param replace_string const C string literal to replace pattern with
* @param min_ptr Pointer to start of search range within string
* @param max_ptr Pointer to end of search range within string
* @return bool true if successful (including when no matches found), false on error
*         Sets errno to EINVAL if inputs are NULL
*         Sets errno to ERANGE if pointers are out of bounds
*         Sets errno to ENOMEM if memory reallocation fails
*/
bool replace_lit_substr(string_t* string, const char* pattern, const char* replace_string,
                        char* min_ptr, char* max_ptr);
// --------------------------------------------------------------------------------

/**
* @function replace_string_substring
* @brief Replaces all occurrences of a string_t pattern with another string_t
*        between two specified pointers in a string_t object.
*
* If the replacement string is longer than the pattern, memory will be reallocated as needed.
* If shorter, the string will be compacted. The function maintains proper null termination.
*
* @param string string_t object to modify
* @param pattern const string_t object containing pattern to search for and replace
* @param replace_string const string_t object containing string to replace pattern with
* @param min_ptr Pointer to start of search range within string
* @param max_ptr Pointer to end of search range within string
* @return bool true if successful (including when no matches found), false on error
*         Sets errno to EINVAL if inputs are NULL
*         Sets errno to ERANGE if pointers are out of bounds
*         Sets errno to ENOMEM if memory reallocation fails
*/
bool replace_string_substr(string_t* string, const string_t* pattern, const string_t* replace_string,
                           char* min_ptr, char* max_ptr);
// -------------------------------------------------------------------------------- 

/**
* @macro replace_substr
* @brief Generic macro that selects appropriate substring replacement function based on type.
*
* If the pattern is char*, calls replace_lit_substring.
* If the pattern is string_t*, calls replace_string_substring.
*
* Example usage:
*     replace_substr(str, "old", "new", start, end)          // Uses literal version
*     replace_substr(str1, pattern, replacement, start, end)  // Uses string_t version
*/
#define replace_substr(string, pattern, replace_string, min_ptr, max_ptr) _Generic((pattern), \
    char*: replace_lit_substr, \
    string_t*: replace_string_substr) (string, pattern, replace_string, min_ptr, max_ptr)
// ================================================================================
// ================================================================================ 

/**
 * @func to_upper_char
 * @brief Transforms a char value to its uppercase form 
 *
 * Sets the value of errno to EINVAL if val points to a NULL value
 *
 * @param val A pointer to he char value to be transformed to its uppercase form
 */
void to_upper_char(char* val);
// --------------------------------------------------------------------------------

/**
 * @func to_lower_char
 * @brief Transforms a char value to its lowercase form 
 *
 * Sets the value of errno to EINVAL if val points to a NULL value
 *
 * @param val A pointer to he char value to be transformed to its lowercase form
 */
void to_lower_char(char* val);
// --------------------------------------------------------------------------------

/**
 * @func to_uppercase
 * @brief Transforms all values in a string to uppercase
 *
 * Sets the value of errno to EINVAL if val points to a NULL value or a null value 
 * of val->str
 *
 * @param val A pointer to a string_t data type
 */
void to_uppercase(string_t* val);
// --------------------------------------------------------------------------------

/**
 * @func to_lowercase
 * @brief Transforms all values in a string to lowercase
 *
 * Sets the value of errno to EINVAL if val points to a NULL value or a null value 
 * of val->str
 *
 * @param val A pointer to a string_t data type
 */
void to_lowercase(string_t* val);
// --------------------------------------------------------------------------------

/**
* @function pop_string_token
* @brief Splits a string at the rightmost occurrence of a token character.
*
* Returns the portion of the string after the token as a new string_t object and
* modifies the original string to contain only the portion before the token.
* The token character is removed from the original string.
*
* @param str_struct string_t object to split
* @param token Character to use as splitting token
* @return New string_t object containing portion after token, or NULL if token not found
*         Sets errno to EINVAL if str_struct is NULL
*/
string_t* pop_string_token(string_t* str_struct, char token);
// --------------------------------------------------------------------------------

/**
* @function token_count
* @brief Counts the number of tokens in a string separated by specified delimiter(s).
*
* Consecutive delimiters are treated as a single delimiter. Leading and trailing
* delimiters are ignored.
*
* @param str string_t object to analyze
* @param delim String containing delimiter character(s)
* @return Number of tokens found, or 0 if str is empty or on error
*         Sets errno to EINVAL if str or delim is NULL
*
* Example:
*     string_t* str = init_string("hello world there");
*     size_t count = token_count(str, " ");  // Returns 3
*
*     string_t* str2 = init_string("one,two;three");
*     size_t count2 = token_count(str2, ",;");  // Returns 3
*/
size_t token_count(const string_t* str, const char* delim);
// --------------------------------------------------------------------------------

/**
 * @function get_char 
 * @brief Returns a chare to the user from a string_t data structure.
 *
 * @param str A string_t data type 
 * @param index The index from within the string_t data type from which a char will 
 *              be retrieved 
 * @return A char value.  Sets errno to EINVAL if str or str->str is NULL, or
 *         ERANGE if index is out of range
 */
char get_char(string_t* str, size_t index);
// --------------------------------------------------------------------------------

/**
 * @function replace_char 
 * @brief Replaces an existing char value in a string_t data type with another 
 *
 * Sets errno to EINVAL if str or str->str is NULL or ERANGE if index is out
 * of range
 *
 * @param str A string_t data type 
 * @param index The index within str where a char value will be replaced 
 * @param value The char value to replace with 
 *
 */
void replace_char(string_t* str, size_t index, char value);
// --------------------------------------------------------------------------------

/**
 * @function trim_leading_whitespace 
 * @brief Removes any white space at the leading edge of a string 
 *
 * Sets errno to EINVAL if str or str->str is NULL
 *
 * @param str A string_t data type 
 */
void trim_leading_whitespace(string_t* str);
// --------------------------------------------------------------------------------

/**
 * @function trim_trailing_whitespace 
 * @brief Removes any white space at the trailing edge of a string 
 *
 * Sets errno to EINVAL if str or str->str is NULL
 *
 * @param str A string_t data type 
 */
void trim_trailing_whitespace(string_t* str);
// --------------------------------------------------------------------------------

/**
 * @function trim_all_whitespace 
 * @brief Removes all white space in a string 
 *
 * Sets errno to EINVAL if str or str->str is NULL
 *
 * @param str A string_t data type 
 */
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
