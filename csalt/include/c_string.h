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

#ifndef ARENA_USE_CONVENIENCE_MACROS
#  ifdef NO_FUNCTION_MACROS
#    define ARENA_USE_CONVENIENCE_MACROS 0
#  else
#    define ARENA_USE_CONVENIENCE_MACROS 1
#  endif
#endif

/* Sanity: if someone set both, ensure consistency */
#if defined(NO_ARENA_MACROS) && (ARENA_USE_CONVENIENCE_MACROS+0)!=0
#  error "NO_ARENA_MACROS set but ARENA_USE_CONVENIENCE_MACROS != 0"
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "c_error.h"
#include "c_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

typedef struct {
    char *str;
    size_t len;
    size_t alloc;
    allocator_vtable_t allocator;
} string_t;
// -------------------------------------------------------------------------------- 

typedef struct {
    bool has_value;
    union {
        string_t* value;
        error_code_t error;
    } u;
} string_expect_t;
// ================================================================================ 
// ================================================================================ 

/**
 * @brief Initialize an allocator-backed string container.
 *
 * Constructs a new ::string_t instance using the provided C-string input,
 * requested payload capacity, and allocator vtable.
 *
 * Capacity semantics:
 * - If @p capacity_bytes is 0, the allocation defaults to exactly the
 *   length of @p cstr plus space for the null terminator.
 * - If @p capacity_bytes is non-zero, the container allocates
 *   (@p capacity_bytes + 1) bytes to guarantee space for the terminator.
 * - If the requested capacity is smaller than the source string length,
 *   the stored string is truncated to fit and always null-terminated.
 *
 * Memory is obtained exclusively through the supplied allocator and must
 * later be released with ::return_string().
 *
 * @param[in] cstr            Null-terminated source C string.
 * @param[in] capacity_bytes  Requested payload capacity in characters
 *                            (excluding the null terminator).
 * @param[in] allocator       Allocator vtable used for memory management.
 *
 * @return ::string_expect_t
 * - `.has_value = true`  → valid ::string_t pointer in `.u.value`
 * - `.has_value = false` → error code in `.u.error`
 *
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("hello", 0, a);
 * if (r.has_value) {
 *     printf("%s\n", const_string(r.u.value)); // prints "hello"
 *     return_string(r.u.value);
 * }
 * @endcode
 */
string_expect_t init_string(const char* cstr, size_t capacity_bytes, allocator_vtable_t allocator);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieve the internal null-terminated C string.
 *
 * Returns a pointer to the underlying character buffer owned by the
 * ::string_t container. The returned pointer remains valid until the
 * string is released with ::return_string().
 *
 * Passing NULL is safe and returns NULL.
 *
 * @param[in] s Pointer to ::string_t instance or NULL.
 *
 * @return Pointer to null-terminated character buffer, or NULL.
 *
 * @code{.c}
 * const char* text = const_string(str);
 * if (text) {
 *     puts(text);
 * }
 * @endcode
 */
static inline const char* const_string(const string_t* s) {
    return s ? s->str : NULL;
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Get the logical length of the string.
 *
 * Returns the number of characters stored in the container, excluding
 * the null terminator.
 *
 * Passing NULL is safe and returns 0.
 *
 * @param[in] s Pointer to ::string_t instance or NULL.
 *
 * @return Character count excluding the null terminator.
 *
 * @code{.c}
 * size_t n = string_size(str);
 * printf("length = %zu\n", n);
 * @endcode
 */
static inline size_t string_size(const string_t* s) {
    return s ? s->len : 0u;
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Get the total allocated buffer size in bytes.
 *
 * Returns the number of bytes allocated for the internal buffer,
 * including space reserved for the null terminator.
 *
 * Passing NULL is safe and returns 0.
 *
 * @param[in] s Pointer to ::string_t instance or NULL.
 *
 * @return Total allocated bytes for the string buffer.
 *
 * @code{.c}
 * size_t cap = string_alloc(str);
 * printf("capacity = %zu bytes\n", cap);
 * @endcode
 */
static inline size_t string_alloc(const string_t* s) {
    return s ? s->alloc : 0u;
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Release a string and its associated memory.
 *
 * Frees the internal character buffer and the ::string_t structure using
 * the allocator stored within the container.  For some allocators like 
 * an ``arena_t`` this function may be a no-op.
 * Passing NULL is safe and performs no action.
 *
 * After this call, the pointer must not be used.
 *
 * @param[in] s Pointer to ::string_t instance or NULL.
 *
 * @code{.c}
 * string_expect_t r = init_string("example", 0, heap_allocator());
 * if (r.has_value) {
 *     return_string(r.u.value); // safe cleanup
 * }
 * @endcode
 */
void return_string(string_t* s);
// -------------------------------------------------------------------------------- 

/**
 * @brief Concatenate a C string onto a CSalt string.
 *
 * Appends the null-terminated string @p str to the end of the destination
 * string @p s. If additional capacity is required, the function attempts to
 * grow the underlying buffer using the allocator associated with @p s.
 *
 * This function is safe for overlapping memory regions. If the source pointer
 * lies within the destination buffer and reallocation is required, a temporary
 * copy is created before growth to preserve correctness.
 *
 * @param s   Destination string to be extended.
 * @param str Null-terminated C string to append.
 *
 * @retval true  Concatenation succeeded.
 * @retval false Invalid arguments, allocation failure, or size overflow.
 *
 * @note
 * - The allocator stored in @p s determines growth behavior.
 * - Arena allocators may not reclaim intermediate buffers until the arena
 *   itself is reset or destroyed.
 * - The resulting string is always null-terminated on success.
 *
 * @code{.c}
 * string_expect_t r = init_string("Hello", 0, heap_allocator());
 * if (r.has_value) {
 *     string_t* s = r.u.value;
 *
 *     if (str_concat(s, ", world!")) {
 *         printf("%s\n", const_string(s)); // "Hello, world!"
 *     }
 *
 *     return_string(s);
 * }
 * @endcode
 */
bool str_concat(string_t* s, const char* str);
// -------------------------------------------------------------------------------- 

/**
 * @brief Concatenate one CSalt string onto another.
 *
 * Appends the contents of @p str to the destination string @p s. This function
 * behaves identically to @ref str_concat but obtains the source characters from
 * another managed @ref string_t instance.
 *
 * The operation respects allocator semantics and may trigger buffer growth
 * using the destination string's allocator.
 *
 * @param s    Destination string to be extended.
 * @param str  Source string whose contents will be appended.
 *
 * @retval true  Concatenation succeeded.
 * @retval false Invalid arguments, allocation failure, or size overflow.
 *
 * @note
 * - Source and destination may reference the same underlying buffer.
 *   Overlap is handled safely.
 * - The destination string remains null-terminated on success.
 *
 * @code{.c}
 * string_expect_t a = init_string("CSalt", 0, heap_allocator());
 * string_expect_t b = init_string(" Library", 0, heap_allocator());
 *
 * if (a.has_value && b.has_value) {
 *     string_t* s1 = a.u.value;
 *     string_t* s2 = b.u.value;
 *
 *     if (string_concat(s1, s2)) {
 *         printf("%s\n", const_string(s1)); // "CSalt Library"
 *     }
 *
 *     return_string(s1);
 *     return_string(s2);
 * }
 * @endcode
 */
bool string_concat(string_t* s, const string_t * str);
// -------------------------------------------------------------------------------- 

#if ARENA_USE_CONVENIENCE_MACROS && !defined(NO_FUNCTION_MACROS)

/* Expression-safe static assertion:
 * If cond is false => negative array => compile error.
 * This is valid in expression contexts (unlike _Static_assert).
 */
#ifndef CSALT_STATIC_ASSERT_EXPR_
#define CSALT_STATIC_ASSERT_EXPR_(cond, name) \
    ((void)sizeof(char[(cond) ? 1 : -1]))
#endif

/* Returns 1 if src type is supported, else 0. */
#ifndef CONCAT_STRING_SUPPORTED_SRC_
#define CONCAT_STRING_SUPPORTED_SRC_(x) \
    _Generic((x), \
        const char*:      1, \
        char*:            1, \
        const string_t*:  1, \
        string_t*:        1, \
        default:          0)
#endif

/* Compile-time check: triggers error for unsupported src types. */
#ifndef CONCAT_STRING_TYPECHECK_
#define CONCAT_STRING_TYPECHECK_(x) \
    CSALT_STATIC_ASSERT_EXPR_(CONCAT_STRING_SUPPORTED_SRC_(x), \
                              concat_string_unsupported_src_type)
#endif

/* Convenience macro: dispatches to the correct implementation. */
#ifndef concat_string
/**
 * @brief Type-safe generic string concatenation convenience macro.
 *
 * Dispatches to the correct concatenation routine based on the type of
 * the source argument @p src using the C11 `_Generic` selection mechanism.
 *
 * Supported source types:
 * - `const char*` → @ref str_concat  
 * - `char*` → @ref str_concat  
 * - `const string_t*` → @ref string_concat  
 * - `string_t*` → @ref string_concat  
 *
 * Any unsupported source type triggers a **compile-time error** in C11
 * builds via @ref _concat_string_type_error, ensuring strong type safety
 * without runtime overhead.
 *
 * This macro is available only when:
 * - `ARENA_USE_CONVENIENCE_MACROS` is defined, and  
 * - `NO_FUNCTION_MACROS` is **not** defined (to preserve MISRA-style builds).
 *
 * @param dst Destination @ref string_t instance to be extended.
 * @param src Source data to append (`const char*` or `string_t*`).
 *
 * @retval true  Concatenation succeeded.
 * @retval false Concatenation failed (allocation error, overflow, or
 *               invalid arguments).  
 *               This return value originates from the selected function.
 *
 * @note
 * - The destination string’s allocator controls any required buffer growth.
 * - For arena allocators, intermediate buffers may persist until the arena
 *   is reset or destroyed.
 * - No runtime type checks are performed; dispatch occurs entirely at
 *   compile time.
 *
 * @code{.c}
 * string_expect_t r = init_string("Answer: ", 0, heap_allocator());
 * if (r.has_value) {
 *     string_t* s = r.u.value;
 *
 *     concat_string(s, "42");
 *     printf("%s\n", const_string(s));  // "Answer: 42"
 *
 *     return_string(s);
 * }
 * @endcode
 */
#define concat_string(dst, src) \
    (CONCAT_STRING_TYPECHECK_(src), \
     _Generic((src), \
        const char*:      str_concat, \
        char*:            str_concat, \
        const string_t*:  string_concat, \
        string_t*:        string_concat \
     )((dst), (src)))
#endif

/* NOTE on string literals:
 * If your compiler treats "B" as char[N] in _Generic, you may need:
 *   concat_string(s, (const char*)"B");
 * or:
 *   const char* lit = "B"; concat_string(s, lit);
 */

#endif /* ARENA_USE_CONVENIENCE_MACROS && !defined(NO_FUNCTION_MACROS) */
// -------------------------------------------------------------------------------- 

/**
 * @brief Compare a bounded string_t against a C string.
 *
 * Performs a lexicographical comparison between the contents of
 * @p s and the null-terminated C string @p str.  The comparison is
 * **bounded by s->len**, meaning the function never reads beyond the
 * initialized region of the string_t buffer.
 *
 * This function uses a **scalar implementation** to guarantee:
 * - Deterministic execution
 * - Strict bounds safety
 * - MISRA-compatible control flow
 *
 * @param s   Pointer to the source string_t to compare.
 * @param str Pointer to a null-terminated C string.
 *
 * @retval INT8_MIN  Invalid argument (NULL pointer or corrupt state).
 * @retval 0         Strings are equal within the bounded region.
 * @retval -1        @p s is lexicographically less than @p str.
 * @retval 1         @p s is lexicographically greater than @p str.
 *
 * @note
 * - Comparison stops at **s->len** or the first differing character.
 * - The function does **not** read beyond the bounds of @p s.
 *
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("alpha", 0, a);
 * if (r.has_value) {
 *     string_t* s = r.u.value;
 *
 *     int8_t cmp = str_compare(s, "alphabet");
 *     // cmp == -1  ("alpha" < "alphabet")
 *
 *     return_string(s);
 * }
 * @endcode
 */
int8_t str_compare(const string_t* s, const char *str);
// -------------------------------------------------------------------------------- 

/**
 * @brief Compare two bounded string_t objects.
 *
 * Performs a lexicographical comparison between @p s and @p str,
 * both of which are bounded string_t instances.
 *
 * This function may use **SIMD acceleration** when supported by the
 * target architecture and enabled at compile time:
 *
 * - AVX / AVX2 / AVX-512 on x86
 * - SSE2 / SSE3 / SSE4.1 on x86
 * - NEON on ARM
 * - SVE / SVE2 on ARM
 *
 * When SIMD is unavailable, the implementation **falls back to a
 * fully scalar, MISRA-safe comparison** with identical semantics.
 *
 * @param s   Pointer to the first string_t.
 * @param str Pointer to the second string_t.
 *
 * @retval INT8_MIN  Invalid argument (NULL pointer or corrupt state).
 * @retval 0         Strings are equal.
 * @retval -1        @p s is lexicographically less than @p str.
 * @retval 1         @p s is lexicographically greater than @p str.
 *
 * @note
 * - SIMD is used only for **bounded byte comparison** and never reads
 *   past either string’s initialized region.
 * - Return values are **architecture-independent**.
 *
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r1 = init_string("delta", 0, a);
 * string_expect_t r2 = init_string("gamma", 0, a);
 *
 * if (r1.has_value && r2.has_value) {
 *     string_t* s1 = r1.u.value;
 *     string_t* s2 = r2.u.value;
 *
 *     int8_t cmp = string_compare(s1, s2);
 *     // cmp == -1  ("delta" < "gamma")
 *
 *     return_string(s1);
 *     return_string(s2);
 * }
 * @endcode
 */
int8_t string_compare(const string_t* s, const string_t* str);
// -------------------------------------------------------------------------------- 

#if defined(ARENA_USE_CONVENIENCE_MACROS) && !defined(NO_FUNCTION_MACROS)

/* Helper: compile-time check (expression-safe) */
#define CSALT_STATIC_ASSERT_EXPR_(cond, name) \
    ((void)sizeof(char[(cond) ? 1 : -1]))

/* RHS type support check for compare_string(dst, src) */
#define COMPARE_STRING_SUPPORTED_SRC_(x) \
    _Generic((x), \
        const char*:      1, \
        char*:            1, \
        const string_t*:  1, \
        string_t*:        1, \
        default:          0)

/* Enforce supported RHS types at compile time (C11). */
#define COMPARE_STRING_TYPECHECK_(x) \
    CSALT_STATIC_ASSERT_EXPR_(COMPARE_STRING_SUPPORTED_SRC_(x), \
                              compare_string_unsupported_src_type)

/**
 * @brief Type-safe generic string comparison convenience macro.
 *
 * @details
 * `compare_string(lhs, rhs)` provides a single comparison interface that
 * selects the correct implementation at **compile time** using the C11
 * `_Generic` operator.
 *
 * Compile-time dispatch rules:
 *
 * - If @p rhs is a C string (`const char*` or `char*`), this macro expands to:
 *   @ref str_compare((const string_t*)lhs, (const char*)rhs)
 *
 * - If @p rhs is a string object (`const string_t*` or `string_t*`), this
 *   macro expands to:
 *   @ref string_compare((const string_t*)lhs, (const string_t*)rhs)
 *
 * In other words, the macro performs **zero runtime type checks** and adds
 * no dispatch overhead—selection happens entirely at compile time.
 *
 * Availability:
 * - Enabled only when `ARENA_USE_CONVENIENCE_MACROS` is defined, and
 * - Disabled when `NO_FUNCTION_MACROS` is defined (to support MISRA-style builds).
 *
 * @param lhs Pointer to the left-hand @ref string_t (treated as `const string_t*`).
 * @param rhs Right-hand operand. Must be one of:
 *            `const char*`, `char*`, `const string_t*`, `string_t*`.
 *
 * @return int8_t using the semantics of the selected function:
 * @retval INT8_MIN Invalid argument / error sentinel (e.g., NULL input).
 * @retval -1       @p lhs is lexicographically less than @p rhs.
 * @retval 0        @p lhs is equal to @p rhs.
 * @retval 1        @p lhs is lexicographically greater than @p rhs.
 *
 * @note
 * If @p rhs is not one of the supported types, this macro triggers a
 * **compile-time error** in C11 builds via @ref COMPARE_STRING_TYPECHECK_.
 *
 * @note
 * - When dispatching to @ref str_compare, comparison is bounded by `lhs->len`.
 * - When dispatching to @ref string_compare, the implementation may use
 *   SIMD acceleration internally (depending on build/architecture), but
 *   the return semantics remain identical across platforms.
 *
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r1 = init_string("alpha", 0u, a);
 * string_expect_t r2 = init_string("alphabet", 0u, a);
 *
 * if (r1.has_value && r2.has_value) {
 *     string_t* s1 = r1.u.value;
 *     string_t* s2 = r2.u.value;
 *
 *     // Dispatches to str_compare(s1, "alphabet")
 *     int8_t c1 = compare_string(s1, "alphabet");  // -> -1 
 *
 *     // Dispatches to string_compare(s1, s2) 
 *     int8_t c2 = compare_string(s1, s2);          // -> -1 
 *
 *     (void)c1;
 *     (void)c2;
 *
 *     return_string(s1);
 *     return_string(s2);
 * }
 * @endcode
 */
#define compare_string(lhs, rhs) \
    (COMPARE_STRING_TYPECHECK_(rhs), \
     _Generic((rhs), \
        const char*:      str_compare, \
        char*:            str_compare, \
        const string_t*:  string_compare, \
        string_t*:        string_compare \
     )((const string_t*)(lhs), (rhs)))

#endif /* ARENA_USE_CONVENIENCE_MACROS && !NO_FUNCTION_MACROS */
// -------------------------------------------------------------------------------- 

/**
 * @brief Reset a string to the empty state without releasing memory.
 *
 * Sets the logical length of the string to zero and, when a backing buffer
 * exists, writes a null terminator at the first character position.  
 * This allows subsequent concatenation operations (e.g., @ref str_concat or
 * @ref string_concat) to begin writing from the start of the buffer while
 * preserving the previously allocated capacity.
 *
 * This operation is **O(1)** and does not invoke the allocator.
 *
 * @param str Pointer to the @ref string_t instance to reset.
 *
 * @note
 * - If @p str is `NULL`, the function performs no action.
 * - If the string has no backing buffer (`str->str == NULL`), the function
 *   performs no action.
 * - Capacity remains unchanged; only the logical contents are cleared.
 *
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("hello", 0u, a);
 * if (r.has_value) {
 *     string_t* s = r.u.value;
 *
 *     reset_string(s);
 *
 *     // String is now empty but reusable
 *     str_concat(s, "world");
 *     printf("%s\n", const_string(s));   // prints "world"
 *
 *     return_string(s);
 * }
 * @endcode
 */
static inline void reset_string(string_t* str) {
    if (!str || !str->str) {
        return;
    }
    str->len = 0u;
    str->str[0] = '\0';
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a deep value copy of an existing string.
 *
 * Allocates a new @ref string_t instance using the supplied
 * allocator and copies the character data from the source
 * string @p s.  
 *
 * The copied string:
 * - Contains identical character contents to @p s
 * - Has **independent storage** (no shared buffer)
 * - Uses the provided @p allocator for memory management
 * - Allocates the **minimal required capacity** of
 *   `string_size(s) + 1` bytes to store the characters and
 *   null terminator
 *
 * This function performs a *value copy*, not a structural
 * clone of the original allocation. Any unused capacity in
 * the source string is **not preserved** in the copy.
 *
 * @param s         Source string to copy.
 * @param allocator Allocator used to create the new string.
 *
 * @return string_expect_t
 * @retval has_value = true
 *         `.u.value` points to a newly allocated deep copy.
 * @retval has_value = false
 *         `.u.error` contains:
 *         - @ref NULL_POINTER if @p s or `s->str` is NULL
 *         - Any error propagated from @ref init_string
 *
 * @note
 * - The returned string must be released with
 *   @ref return_string.
 * - Modifying the copy does **not** affect the source.
 * - Suitable for transferring string ownership between
 *   allocators or subsystems.
 *
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r1 = init_string("hello", 0, a);
 * if (!r1.has_value) {
 *     return;
 * }
 *
 * string_t* original = r1.u.value;
 *
 * string_expect_t r2 = copy_string(original, a);
 * if (r2.has_value) {
 *     string_t* copy = r2.u.value;
 *
 *     // Independent modification
 *     str_concat(copy, " world");
 *
 *     printf("%s\n", const_string(original)); // "hello"
 *     printf("%s\n", const_string(copy));     // "hello world"
 *
 *     return_string(copy);
 * }
 *
 * return_string(original);
 * @endcode
 */
string_expect_t copy_string(const string_t* s, allocator_vtable_t allocator);
// -------------------------------------------------------------------------------- 

/**
 * @brief Checks whether a pointer lies within a string’s allocated buffer.
 *
 * Determines if @p ptr points to a memory location inside the
 * character storage owned by the @ref string_t instance @p s.
 *
 * The valid range is:
 *
 * `[s->str, s->str + string_alloc(s))`
 *
 * This function **does not** verify:
 * - Whether the pointer references initialized characters
 * - Whether the pointer aligns to a character boundary
 * - Whether the allocator globally owns the pointer
 *
 * It only checks containment within the string’s allocation.
 *
 * @param s   Pointer to the string instance.
 * @param ptr Pointer to test.
 *
 * @retval true  Pointer lies within the string’s allocated buffer.
 * @retval false Pointer is NULL, string is invalid, or outside range.
 *
 * @note
 * - Safe for defensive validation in low-level APIs.
 * - Useful before performing pointer arithmetic or in-place mutation.
 *
 * @code{.c}
 * string_expect_t r = init_string("hello", 0, heap_allocator());
 * if (r.has_value) {
 *     string_t* s = r.u.value;
 *
 *     char* p = s->str + 1;
 *
 *     if (is_string_ptr(s, p)) {
 *         *p = 'a';  // safe mutation
 *     }
 *
 *     return_string(s);
 * }
 * @endcode
 */
bool is_string_ptr(const string_t* s, const void* ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Checks whether a sized memory range lies fully within a string’s allocated buffer.
 *
 * Determines whether the range `[ptr, ptr + bytes)` is entirely contained within
 * the character storage owned by @p s.
 *
 * The string's valid allocation range is:
 * `[s->str, s->str + s->alloc)`.
 *
 * This function is useful for validating that an object, sub-buffer, or
 * typed view fits completely within a string before performing operations
 * such as parsing, casting, or in-place mutation.
 *
 * @param s     Pointer to the string instance.
 * @param ptr   Pointer to the start of the candidate region.
 * @param bytes Size (in bytes) of the candidate region.
 *
 * @retval true  The entire range lies within the string’s allocated buffer.
 * @retval false Invalid inputs, overflow, or the range extends خارج the buffer.
 *
 * @note
 * - This checks **allocation containment**, not “used characters” containment.
 *   If you want containment within initialized characters, bound against `s->len + 1`
 *   (or `s->len`) instead of `s->alloc`.
 * - `bytes == 0` returns false to avoid “vacuously true” ranges.
 */
bool is_string_ptr_sized(const string_t* s, const void* ptr, size_t bytes);
// -------------------------------------------------------------------------------- 

#ifndef ITER_DIR_H
#define ITER_DIR_H
    typedef enum {
        FORWARD = 0,
        REVERSE = 1
    }direction_t;
#endif /* ITER_DIR_H*/

/**
 * @brief Finds the first occurrence of a substring within a bounded region.
 *
 * Searches for the string @p needle inside the character data of
 * @p haystack, restricted to the memory range [`begin`, `end`).
 *
 * The search direction is controlled by @p dir:
 * - @ref FORWARD  — scans from `begin` toward `end` and returns the
 *   earliest match.
 * - @ref REVERSE  — scans from `end` toward `begin` and returns the
 *   latest match within the region.
 *
 * If @p begin or @p end is `NULL`, the search defaults to the used
 * character region of the string:
 *
 * ```
 * [haystack->str, haystack->str + haystack->len)
 * ```
 *
 * This function is implemented to take advantage of **SIMD (Single Instruction,
 * Multiple Data)** instructions when supported by the target architecture.
 *
 * At compile time, architecture-specific vectorized implementations may be
 * selected, including:
 *
 * - AVX-512, AVX2, AVX
 * - SSE4.1, SSE3, SSE2
 * - ARM NEON
 * - ARM SVE / SVE2
 *
 * When SIMD is available, the search compares **multiple characters in
 * parallel**, significantly improving performance for:
 *
 * - long haystacks
 * - repeated substring searches
 * - forward and reverse scans over large buffers
 *
 * If no SIMD capability is detected, the function **safely falls back to a
 * fully portable scalar implementation** with identical semantics.
 *
 * SIMD usage is completely **transparent to the caller**:
 * - No API differences
 * - No alignment requirements
 * - No behavioral changes
 *
 * The return value is a **1-based offset relative to `begin`**:
 *
 * | Return value | Meaning |
 * |-------------|---------|
 * | `0`         | Not found or invalid arguments |
 * | `1`         | Match begins exactly at `begin` |
 * | `k + 1`     | Match begins `k` bytes after `begin` |
 *
 * This convention avoids ambiguity between:
 * - “not found”, and
 * - “match at index 0”.
 *
 * @param haystack String being searched.
 * @param needle   Substring to locate.
 * @param begin    Pointer to start of searchable region inside
 *                 `haystack->str` (may be `NULL`).
 * @param end      Pointer to one-past-end of searchable region inside
 *                 `haystack->str` (may be `NULL`).
 * @param dir      Search direction: @ref FORWARD or @ref REVERSE.
 *
 * @return size_t
 * @retval 0   Not found or invalid input.
 * @retval >0  1-based offset of first match relative to `begin`.
 *
 * - Region pointers must lie **within the allocated buffer** of
 *   `haystack`; otherwise the function returns `0`.
 * - The search is limited to the **used string length**, not slack
 *   allocation beyond `haystack->len`.
 * - An empty @p needle is treated as found at the beginning of the
 *   region and returns `1`.
 * - SIMD acceleration is **optional and architecture-dependent** but
 *   never changes correctness.
 *
 * @par Example: Forward search
 * @code{.c}
 * string_expect_t h = init_string("bananana", 0, heap_allocator());
 * string_expect_t n = init_string("ana", 0, heap_allocator());
 *
 * if (h.has_value && n.has_value) {
 *     size_t pos = find_substr(h.u.value, n.u.value, NULL, NULL, FORWARD);
 *     // "ana" first appears at index 1 → return value = 2 (1-based)
 * }
 * @endcode
 *
 * @par Example: Reverse search
 * @code{.c}
 * size_t pos = find_substr(h.u.value, n.u.value, NULL, NULL, REVERSE);
 * // Last occurrence at index 5 → return value = 6
 * @endcode
 *
 * @par Example: Bounded window search
 * @code{.c}
 * const uint8_t* base = (const uint8_t*)h.u.value->str;
 *
 * size_t pos = find_substr(
 *     h.u.value,
 *     n.u.value,
 *     base + 3,                 // begin search inside string
 *     base + h.u.value->len,    // end of used region
 *     FORWARD);
 *
 * // Position is relative to begin, not start of string
 * @endcode
 */
size_t find_substr(const string_t* haystack,
                   const string_t* needle,
                   const uint8_t*  begin,
                   const uint8_t*  end,
                   direction_t     dir);
// -------------------------------------------------------------------------------- 

/**
 * @brief Find the first occurrence of a literal substring within a string range.
 *
 * This function searches for the **first case-sensitive occurrence** of the
 * NUL-terminated C string `needle_lit` inside the string `haystack`,
 * optionally constrained to the byte range `[begin, end)`.
 *
 * The search semantics match @ref find_substr, but the needle is provided as a
 * **string literal** instead of a @ref string_t object. Internally, the literal
 * length is determined via `strlen`, and the search is delegated to the same
 * SIMD/scalar substring engine used by @ref find_substr.
 *
 * @param haystack
 * Pointer to the source string object to be searched.
 *
 * @param needle_lit
 * Pointer to a **NUL-terminated C string literal** representing the substring
 * to locate.
 *
 * @param begin
 * Optional pointer to the beginning of the search region within
 * `haystack->str`.  
 * If `NULL`, the search begins at the start of the used string.
 *
 * @param end
 * Optional pointer to one-past-the-last byte of the search region.  
 * If `NULL`, the search continues to the end of the used string length.
 *
 * @param dir
 * Search direction (implementation-defined; typically forward or reverse).
 *
 * @return
 * Offset in bytes from the beginning of `haystack->str` to the first matching
 * occurrence of `needle_lit`.
 *
 * @retval SIZE_MAX
 * Returned if:
 * - `haystack == NULL`
 * - `haystack->str == NULL`
 * - `needle_lit == NULL`
 * - the search range is invalid or outside the allocation
 * - the literal is not found within the specified region
 *
 * @note
 * - Matching is **case-sensitive** and **substring-based** (not word-delimited).
 * - The search range `[begin, end)` is validated against the underlying
 *   allocation and clamped to the **used length** of the string.
 * - An **empty literal** (`""`) is defined as found at the start of the search
 *   region and returns the offset corresponding to `begin`.
 * - The literal is **not copied**; only its length is computed before searching.
 *
 * @par Example
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("Hello world, hello again", 0u, a);
 * if (!r.has_value) {
 *     // handle error
 * }
 * string_t* text = r.u.value;
 *
 * // Find first lowercase "hello"
 * size_t pos = find_substr_lit(text, "hello", NULL, NULL, DIR_FWD);
 *
 * // pos == 13
 *
 * return_string(text);
 * @endcode
 *
 * @see find_substr
 * @see word_count_lit
 */
size_t find_substr_lit(const string_t* haystack,
                       const char*     needle_lit,
                       const uint8_t*  begin,
                       const uint8_t*  end,
                       direction_t     dir);
// -------------------------------------------------------------------------------- 

#if defined(ARENA_USE_CONVENIENCE_MACROS) && !defined(NO_FUNCTION_MACROS)

/* Helper: compile-time check (expression-safe)
   (If already defined elsewhere, you can remove this duplicate.) */
#ifndef CSALT_STATIC_ASSERT_EXPR_
#define CSALT_STATIC_ASSERT_EXPR_(cond, name) \
    ((void)sizeof(char[(cond) ? 1 : -1]))
#endif

/* RHS type support check for find_substring(haystack, needle, begin, end, dir) */
#define FIND_SUBSTR_SUPPORTED_NEEDLE_(x) \
    _Generic((x), \
        const char*:      1, \
        char*:            1, \
        const string_t*:  1, \
        string_t*:        1, \
        default:          0)

/* Enforce supported needle types at compile time (C11). */
#define FIND_SUBSTR_TYPECHECK_(x) \
    CSALT_STATIC_ASSERT_EXPR_(FIND_SUBSTR_SUPPORTED_NEEDLE_(x), \
                              find_substring_unsupported_needle_type)

/**
 * @brief Type-safe generic substring search convenience macro.
 *
 * @details
 * `find_substring(haystack, needle, begin, end, dir)` selects the correct
 * substring search implementation at **compile time** using the C11 `_Generic`
 * operator.
 *
 * Compile-time dispatch rules:
 *
 * - If @p needle is a C string (`const char*` or `char*`), this macro expands to:
 *   @ref find_substr_lit((const string_t*)haystack, (const char*)needle, begin, end, dir)
 *
 * - If @p needle is a string object (`const string_t*` or `string_t*`), this
 *   macro expands to:
 *   @ref find_substr((const string_t*)haystack, (const string_t*)needle, begin, end, dir)
 *
 * This macro performs **zero runtime type checks** and adds no dispatch
 * overhead—selection happens entirely at compile time.
 *
 * Availability:
 * - Enabled only when `ARENA_USE_CONVENIENCE_MACROS` is defined, and
 * - Disabled when `NO_FUNCTION_MACROS` is defined (to support MISRA-style builds).
 *
 * @param haystack Pointer to the source @ref string_t (treated as `const string_t*`).
 * @param needle   Substring to search for. Supported types:
 *                 `const char*`, `char*`, `const string_t*`, `string_t*`.
 * @param begin    Optional start pointer within `haystack->str` (or `NULL`).
 * @param end      Optional end pointer within `haystack->str` (or `NULL`).
 * @param dir      Search direction (implementation-defined by underlying functions).
 *
 * @return size_t offset from the beginning of `haystack`, or `SIZE_MAX` if not found
 *         or if arguments are invalid (per the selected implementation).
 *
 * @note
 * If @p needle is not one of the supported types, this macro triggers a
 * **compile-time error** in C11 builds via @ref FIND_SUBSTR_TYPECHECK_.
 *
 * @see find_substr
 * @see find_substr_lit
 */
#define find_substring(haystack, needle, begin, end, dir) \
    (FIND_SUBSTR_TYPECHECK_(needle), \
     _Generic((needle), \
        const char*:      find_substr_lit, \
        char*:            find_substr_lit, \
        const string_t*:  find_substr, \
        string_t*:        find_substr \
     )((const string_t*)(haystack), (needle), (begin), (end), (dir)))

#endif /* ARENA_USE_CONVENIENCE_MACROS && !NO_FUNCTION_MACROS */

// -------------------------------------------------------------------------------- 

/**
 * @brief Count case-sensitive occurrences of a substring within a string range.
 *
 * This function counts the number of **non-overlapping, case-sensitive**
 * occurrences of `word` inside the string `s`, optionally constrained to the
 * byte range `[start, end)`.
 *
 * Internally, this function repeatedly calls `find_substr()` and advances the
 * search cursor past each successful match, ensuring forward progress and
 * preventing infinite loops.
 *
 * @param s
 * Pointer to the source string object to be searched.
 *
 * @param word
 * Pointer to the substring to search for.
 *
 * @param start
 * Optional pointer to the beginning of the search region within `s->str`.
 * If `NULL`, the search begins at the start of the string.
 *
 * @param end
 * Optional pointer to one-past-the-last byte of the search region.
 * If `NULL`, the search continues to the end of the used string length.
 *
 * @return
 * The number of **non-overlapping** occurrences of `word` found within the
 * specified region.
 *
 * @retval 0
 * Returned if:
 *  - `s == NULL`
 *  - `s->str == NULL`
 *  - `word == NULL`
 *  - `word->str == NULL`
 *  - `word->len == 0`
 *  - no matches are found
 *
 * @note
 * - Matching is **case-sensitive**.
 * - Matches are **substring-based**, not whole-word delimited.
 *   For example, searching `"hello"` will match `"jonhello"`.
 * - Occurrences are counted **non-overlapping**.
 *   To count overlapping matches, advance the cursor by `+1` instead of
 *   `+word->len` after each match.
 *
 * @par Example
 * @code
 * allocator_vtable_t a = heap_allocator();
 * string_expect_t r = init_string("Hello world thisHello is hello again Hello", 45, a);
 * if (!r.has_value) {
 *     // handle error
 * }
 * string_t* text = r.u.value;
 *
 * r = init_string("Hello", 5, a);
 * if (!r.has_value) {
 *    // Handle error
 * }
 * string_t* word = r.u.value;
 * size_t count = word_count(text, word, NULL, NULL);
 *
 * // count == 2 because matching is case-sensitive:
 * //   "Hello"
 * //   "thisHello"
 * //   "Hello"
 * //
 * @endcode
 *
 * @see find_substr
 */
size_t word_count(const string_t* s,
                  const string_t* word,
                  const uint8_t*  start,
                  const uint8_t*  end);
// -------------------------------------------------------------------------------- 

/**
 * @brief Count case-sensitive occurrences of a literal substring within a string range.
 *
 * This function counts the number of **non-overlapping, case-sensitive**
 * occurrences of the C string literal `word` inside the string `s`,
 * optionally constrained to the byte range `[start, end)`.
 *
 * Internally, this function constructs a temporary non-owning substring view
 * and repeatedly calls `find_substr()`, advancing the search cursor past each
 * successful match to ensure forward progress and prevent infinite loops.
 *
 * @param s
 * Pointer to the source string object to be searched.
 *
 * @param word
 * Pointer to a **NUL-terminated C string literal** representing the substring
 * to search for.
 *
 * @param start
 * Optional pointer to the beginning of the search region within `s->str`.
 * If `NULL`, the search begins at the start of the string.
 *
 * @param end
 * Optional pointer to one-past-the-last byte of the search region.
 * If `NULL`, the search continues to the end of the used string length.
 *
 * @return
 * The number of **non-overlapping** occurrences of `word` found within the
 * specified region.
 *
 * @retval 0
 * Returned if:
 *  - `s == NULL`
 *  - `s->str == NULL`
 *  - `word == NULL`
 *  - `word` is an empty string (`""`)
 *  - no matches are found
 *
 * @note
 * - Matching is **case-sensitive**.
 * - Matches are **substring-based**, not whole-word delimited.
 *   For example, searching `"Hello"` will match `"thisHello"`.
 * - Occurrences are counted **non-overlapping**.
 *   To count overlapping matches, advance the cursor by `+1` instead of
 *   `+strlen(word)` after each match.
 * - The literal `word` is **not copied**; only a temporary non-owning view
 *   is created for the duration of the search.
 *
 * @par Example
 * @code
 * allocator_vtable_t a = heap_allocator();
 * string_expect_t r = init_string("Hello world thisHello is hello again Hello", 45, a);
 * if (!r.has_value) {
 *     // handle error
 * }
 * string_t* text = r.u.value;
 *
 * size_t count = word_count_lit(text, "Hello", NULL, NULL);
 *
 * // count == 3 because matching is case-sensitive:
 * //   "Hello"
 * //   "thisHello"
 * //   "Hello"
 * //
 * @endcode
 *
 * @see word_count
 * @see find_substr
 */
size_t word_count_lit(const string_t* s,
                      const char*     word,
                      const uint8_t*  start,
                      const uint8_t*  end);
// -------------------------------------------------------------------------------- 

#if defined(ARENA_USE_CONVENIENCE_MACROS) && !defined(NO_FUNCTION_MACROS)

/* Helper: compile-time check (expression-safe)
   (If already defined elsewhere, you can remove this duplicate.) */
#ifndef CSALT_STATIC_ASSERT_EXPR_
#define CSALT_STATIC_ASSERT_EXPR_(cond, name) \
    ((void)sizeof(char[(cond) ? 1 : -1]))
#endif

/* RHS type support check for count_words(s, word, start, end) */
#define COUNT_WORDS_SUPPORTED_WORD_(x) \
    _Generic((x), \
        const char*:      1, \
        char*:            1, \
        const string_t*:  1, \
        string_t*:        1, \
        default:          0)

/* Enforce supported word types at compile time (C11). */
#define COUNT_WORDS_TYPECHECK_(x) \
    CSALT_STATIC_ASSERT_EXPR_(COUNT_WORDS_SUPPORTED_WORD_(x), \
                              count_words_unsupported_word_type)

/**
 * @brief Type-safe generic substring occurrence counting convenience macro.
 *
 * @details
 * `count_words(s, word, start, end)` provides a single counting interface that
 * selects the correct implementation at **compile time** using the C11
 * `_Generic` operator.
 *
 * Compile-time dispatch rules:
 *
 * - If @p word is a C string (`const char*` or `char*`), this macro expands to:
 *   @ref word_count_lit((const string_t*)s, (const char*)word, start, end)
 *
 * - If @p word is a string object (`const string_t*` or `string_t*`), this macro
 *   expands to:
 *   @ref word_count((const string_t*)s, (const string_t*)word, start, end)
 *
 * In other words, the macro performs **zero runtime type checks** and adds no
 * dispatch overhead—selection happens entirely at compile time.
 *
 * Availability:
 * - Enabled only when `ARENA_USE_CONVENIENCE_MACROS` is defined, and
 * - Disabled when `NO_FUNCTION_MACROS` is defined (to support MISRA-style builds).
 *
 * @param s
 * Pointer to the source @ref string_t (treated as `const string_t*`).
 *
 * @param word
 * Substring to search for. Must be one of:
 * `const char*`, `char*`, `const string_t*`, `string_t*`.
 *
 * @param start
 * Optional pointer to the beginning of the search region within `s->str`.
 * If `NULL`, the search begins at the start of the string.
 *
 * @param end
 * Optional pointer to one-past-the-last byte of the search region.
 * If `NULL`, the search continues to the end of the used string length.
 *
 * @return size_t count using the semantics of the selected function.
 *
 * @retval 0
 * Returned if the selected implementation considers the arguments invalid
 * (e.g., `s == NULL`, `s->str == NULL`, `word == NULL`, empty word, etc.) or if
 * no matches are found.
 *
 * @note
 * If @p word is not one of the supported types, this macro triggers a
 * **compile-time error** in C11 builds via @ref COUNT_WORDS_TYPECHECK_.
 *
 * @note
 * Matching is **case-sensitive**. Occurrences are counted **non-overlapping**
 * by default (implementation-defined by the selected function).
 *
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("Hello world thisHello is hello again Hello", 45u, a);
 * if (!r.has_value) {
 *     // handle error
 * }
 * string_t* text = r.u.value;
 *
 * // Dispatches to word_count_lit(text, "Hello", NULL, NULL)
 * size_t c1 = count_words(text, "Hello", NULL, NULL);   // -> 3
 *
 * // Dispatches to word_count(text, word_obj, NULL, NULL)
 * string_expect_t r2 = init_string("hello", 0u, a);
 * if (r2.has_value) {
 *     string_t* w = r2.u.value;
 *     size_t c2 = count_words(text, w, NULL, NULL);     // -> 1
 *     return_string(w);
 *     (void)c2;
 * }
 *
 * (void)c1;
 * return_string(text);
 * @endcode
 *
 * @see word_count
 * @see word_count_lit
 * @see find_substr
 */
#define count_words(s, word, start, end) \
    (COUNT_WORDS_TYPECHECK_(word), \
     _Generic((word), \
        const char*:      word_count_lit, \
        char*:            word_count_lit, \
        const string_t*:  word_count, \
        string_t*:        word_count \
     )((const string_t*)(s), (word), (start), (end)))

#endif /* ARENA_USE_CONVENIENCE_MACROS && !NO_FUNCTION_MACROS */
 // -------------------------------------------------------------------------------- 

/**
 * @brief Count tokens in a string using a C-string delimiter set.
 *
 * Counts the number of **non-empty tokens** within the specified byte range
 * of `s`, where tokens are sequences of bytes **not contained** in the
 * delimiter set `delim`.
 *
 * A token start is defined as a transition from:
 *
 *     delimiter → non-delimiter
 *
 * The beginning of the search window is treated as if it were preceded by a
 * delimiter, ensuring that a leading non-delimiter byte forms a token.
 *
 * The delimiter set is interpreted as the first `strlen(delim)` bytes of the
 * NUL-terminated C string `delim`.
 *
 * @param s
 * Pointer to the source @ref string_t to analyze.
 *
 * @param delim
 * Pointer to a **NUL-terminated C string** containing delimiter bytes.
 * Each byte in this string is treated as an independent delimiter.
 *
 * @param begin
 * Optional pointer to the first byte of the search window within `s->str`.
 * If `NULL`, the search begins at the start of the used string.
 *
 * @param end
 * Optional pointer to one-past-the-last byte of the search window.
 * If `NULL`, the search ends at the used length of the string.
 *
 * @return
 * Number of tokens found in the specified window.
 *
 * @retval SIZE_MAX
 * Returned if:
 * - `s == NULL`
 * - `s->str == NULL`
 * - `delim == NULL`
 * - `[begin,end)` lies outside the string allocation
 *
 * @retval 0
 * Returned if:
 * - the window is empty
 * - the window contains only delimiter bytes
 *
 * @note
 * - Matching is **byte-wise** and **case-sensitive**.
 * - The window `[begin,end)` is validated against the allocation and
 *   clamped to the **used string length**.
 * - If `delim` is an empty string (`""`), the entire non-empty window
 *   is treated as a **single token**.
 * - SIMD acceleration may be used internally depending on the build
 *   configuration and target architecture.
 *
 * @par Example
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("  alpha, beta;gamma  ", 0u, a);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * string_t* text = r.u.value;
 *
 * // Delimiters: space, comma, semicolon
 * size_t count = token_count_lit(text, " ,;", NULL, NULL);
 *
 * // Tokens: "alpha", "beta", "gamma"
 * // count == 3
 *
 * return_string(text);
 * @endcode
 *
 * @see token_count
 */
size_t token_count_lit(const string_t* s,
                       const char*     delim,
                       const uint8_t*  begin,
                       const uint8_t*  end);
// -------------------------------------------------------------------------------- 

/**
 * @brief Count tokens in a string using a @ref string_t delimiter set.
 *
 * Counts the number of **non-empty tokens** within the specified byte range
 * of `s`, where tokens are sequences of bytes **not contained** in the
 * delimiter set stored in `delim`.
 *
 * A token start is defined as a transition from:
 *
 *     delimiter → non-delimiter
 *
 * The beginning of the search window is treated as if it were preceded by a
 * delimiter, ensuring that a leading non-delimiter byte forms a token.
 *
 * @param s
 * Pointer to the source @ref string_t to analyze.
 *
 * @param delim
 * Pointer to a @ref string_t containing delimiter bytes.
 * The delimiter set consists of the first `delim->len` bytes.
 *
 * @param begin
 * Optional pointer to the first byte of the search window within `s->str`.
 * If `NULL`, the search begins at the start of the used string.
 *
 * @param end
 * Optional pointer to one-past-the-last byte of the search window.
 * If `NULL`, the search ends at the used length of the string.
 *
 * @return
 * Number of tokens found in the specified window.
 *
 * @retval SIZE_MAX
 * Returned if:
 * - `s == NULL`
 * - `s->str == NULL`
 * - `delim == NULL`
 * - `delim->str == NULL`
 * - `[begin,end)` lies outside the string allocation
 *
 * @retval 0
 * Returned if:
 * - the window is empty
 * - the window contains only delimiter bytes
 *
 * @note
 * - Matching is **byte-wise** and **case-sensitive**.
 * - The window `[begin,end)` is validated against the allocation and
 *   clamped to the **used string length**.
 * - If `delim->len == 0`, the entire non-empty window is treated as a
 *   **single token**.
 * - SIMD acceleration may be used internally depending on the build
 *   configuration and target architecture.
 *
 * @par Example
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r1 = init_string("one|two||three", 0u, a);
 * string_expect_t r2 = init_string("|", 0u, a);
 *
 * if (!r1.has_value || !r2.has_value) {
 *     // handle error
 * }
 *
 * string_t* text  = r1.u.value;
 * string_t* delim = r2.u.value;
 *
 * size_t count = token_count(text, delim, NULL, NULL);
 *
 * // Tokens: "one", "two", "three"
 * // count == 3
 *
 * return_string(delim);
 * return_string(text);
 * @endcode
 *
 * @see token_count_lit
 */
size_t token_count(const string_t* s,
                   const string_t* delim,
                   const uint8_t*  begin,
                   const uint8_t*  end);
// -------------------------------------------------------------------------------- 

#if defined(ARENA_USE_CONVENIENCE_MACROS) && !defined(NO_FUNCTION_MACROS)

/* ------------------------------------------------------------------------- */
/* Expression-safe static assert                                             */
/* ------------------------------------------------------------------------- */
#define CSALT_STATIC_ASSERT_EXPR_(cond, name) \
    ((void)sizeof(char[(cond) ? 1 : -1]))

/* ------------------------------------------------------------------------- */
/* Supported delimiter RHS types                                             */
/* ------------------------------------------------------------------------- */
#define TOKEN_COUNT_SUPPORTED_DELIM_(x) \
    _Generic((x), \
        const char*:      1, \
        char*:            1, \
        const string_t*:  1, \
        string_t*:        1, \
        default:          0)

/* Enforce supported delimiter types at compile time */
#define TOKEN_COUNT_TYPECHECK_(x) \
    CSALT_STATIC_ASSERT_EXPR_(TOKEN_COUNT_SUPPORTED_DELIM_(x), \
                              token_count_unsupported_delim_type)

/**
 * @brief Type-safe generic token counting convenience macro.
 *
 * @details
 * `count_tokens(s, delim, begin, end)` selects the appropriate token
 * counting implementation at **compile time** using the C11 `_Generic`
 * operator.
 *
 * Dispatch rules:
 *
 * - If @p delim is a C string (`const char*` or `char*`), expands to:
 *   @ref token_count_lit((const string_t*)s, (const char*)delim, begin, end)
 *
 * - If @p delim is a string object (`const string_t*` or `string_t*`),
 *   expands to:
 *   @ref token_count((const string_t*)s, (const string_t*)delim, begin, end)
 *
 * No runtime type checks are performed; selection occurs entirely at
 * compile time with **zero dispatch overhead**.
 *
 * Availability:
 * - Enabled only when `ARENA_USE_CONVENIENCE_MACROS` is defined
 * - Disabled when `NO_FUNCTION_MACROS` is defined
 *
 * @param s
 * Pointer to the source @ref string_t.
 *
 * @param delim
 * Delimiter specification. Must be one of:
 * - `const char*`
 * - `char*`
 * - `const string_t*`
 * - `string_t*`
 *
 * @param begin
 * Optional pointer to the beginning of the search window.
 *
 * @param end
 * Optional pointer to one-past-the-last byte of the search window.
 *
 * @return
 * Number of tokens detected in the specified range.
 *
 * @retval SIZE_MAX
 * Invalid argument or out-of-range window.
 *
 * @note
 * Passing an unsupported delimiter type triggers a **compile-time error**
 * via @ref TOKEN_COUNT_TYPECHECK_.
 *
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("alpha,beta gamma", 0u, a);
 * assert_true(r.has_value);
 *
 * string_t* text = r.u.value;
 *
 * // Dispatch → token_count_lit
 * size_t c1 = count_tokens(text, ", ", NULL, NULL);   // == 2
 *
 * string_expect_t d = init_string(", ", 0u, a);
 * assert_true(d.has_value);
 *
 * // Dispatch → token_count
 * size_t c2 = count_tokens(text, d.u.value, NULL, NULL); // == 2
 *
 * return_string(d.u.value);
 * return_string(text);
 * @endcode
 */
#define count_tokens(s, delim, begin, end) \
    (TOKEN_COUNT_TYPECHECK_(delim), \
     _Generic((delim), \
        const char*:      token_count_lit, \
        char*:            token_count_lit, \
        const string_t*:  token_count, \
        string_t*:        token_count \
     )((const string_t*)(s), (delim), (begin), (end)))

#endif /* ARENA_USE_CONVENIENCE_MACROS && !NO_FUNCTION_MACROS */
// -------------------------------------------------------------------------------- 

/**
 * @brief Convert ASCII lowercase characters to uppercase in-place.
 *
 * Converts all bytes in the specified window of @p s from
 * `'a'..'z'` to `'A'..'Z'` using **ASCII-only** rules.
 * Bytes outside this range are left unchanged.
 *
 * The conversion is performed **in-place** and may be internally
 * accelerated using SIMD instructions depending on the build
 * configuration and target architecture.
 *
 * @param s
 * Pointer to the @ref string_t to modify.
 *
 * @param start
 * Optional pointer to the first byte of the conversion window
 * within `s->str`.  
 * If `NULL`, conversion begins at the start of the used string.
 *
 * @param end
 * Optional pointer to one-past-the-last byte of the conversion
 * window.  
 * If `NULL`, conversion continues to the end of the used string.
 *
 * @note
 * - The window `[start, end)` must lie within the string allocation.
 * - If @p end extends beyond the **used length**, it is clamped
 *   to `s->len`.
 * - If arguments are invalid or the window is empty, the function
 *   performs a **silent no-op**.
 * - Only **ASCII** case conversion is performed.  
 *   UTF-8 multibyte sequences and locale-dependent characters
 *   are not modified.
 *
 * @par Example
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("Hello world", 0u, a);
 * if (!r.has_value) {
 *     // handle allocation failure
 * }
 *
 * string_t* s = r.u.value;
 *
 * to_uppercase(s, NULL, NULL);
 * // s->str == "HELLO WORLD"
 *
 * return_string(s);
 * @endcode
 *
 * @see to_lowercase
 */
void to_uppercase(string_t* s, uint8_t* start, uint8_t* end);
// -------------------------------------------------------------------------------- 

/**
 * @brief Convert ASCII uppercase characters to lowercase in-place.
 *
 * Converts all bytes in the specified window of @p s from
 * `'A'..'Z'` to `'a'..'z'` using **ASCII-only** rules.
 * Bytes outside this range are left unchanged.
 *
 * The conversion is performed **in-place** and may be internally
 * accelerated using SIMD instructions depending on the build
 * configuration and target architecture.
 *
 * @param s
 * Pointer to the @ref string_t to modify.
 *
 * @param start
 * Optional pointer to the first byte of the conversion window
 * within `s->str`.  
 * If `NULL`, conversion begins at the start of the used string.
 *
 * @param end
 * Optional pointer to one-past-the-last byte of the conversion
 * window.  
 * If `NULL`, conversion continues to the end of the used string.
 *
 * @note
 * - The window `[start, end)` must lie within the string allocation.
 * - If @p end extends beyond the **used length**, it is clamped
 *   to `s->len`.
 * - If arguments are invalid or the window is empty, the function
 *   performs a **silent no-op**.
 * - Only **ASCII** case conversion is performed.  
 *   UTF-8 multibyte sequences and locale-dependent characters
 *   are not modified.
 *
 * @par Example
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("HELLO WORLD", 0u, a);
 * if (!r.has_value) {
 *     // handle allocation failure
 * }
 *
 * string_t* s = r.u.value;
 *
 * to_lowercase(s, NULL, NULL);
 * // s->str == "hello world"
 *
 * return_string(s);
 * @endcode
 *
 * @see to_uppercase
 */
void to_lowercase(string_t* s, uint8_t* start, uint8_t* end);
// -------------------------------------------------------------------------------- 

/**
 * @brief Remove all non-overlapping occurrences of a C-string literal substring.
 *
 * This function behaves identically to @ref drop_substr, except the substring
 * is provided as a **NUL-terminated C string literal** rather than a
 * @ref string_t object.
 *
 * Each non-overlapping occurrence of @p substring found within the window
 * `[begin, end)` of @p s is removed **in-place** by shifting the remaining
 * suffix left, preserving the terminating NUL byte.
 *
 * Matches are processed using a **reverse search** strategy to minimize
 * the total amount of memory movement required.
 *
 * If a single ASCII space `' '` immediately follows a removed occurrence,
 * that space is also removed.
 *
 * @param s
 * Pointer to the destination @ref string_t to modify.
 *
 * @param substring
 * NUL-terminated C string containing the substring to remove.
 *
 * @param begin
 * Optional pointer to the first byte of the search window within `s->str`.  
 * If `NULL`, the window begins at the start of the used string.
 *
 * @param end
 * Optional pointer to one-past-the-last byte of the search window.  
 * If `NULL`, the window extends to the end of the used string.
 *
 * @note
 * - The window `[begin, end)` must lie within the string allocation.
 * - If @p end exceeds the **used length**, it is clamped to `s->len`.
 * - If arguments are invalid, the function performs a **silent no-op**.
 * - Matches are **case-sensitive** and **substring-based**.
 * - Only **one trailing ASCII space** is removed per match.
 *
 * @par Example
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("one two two three", 0u, a);
 * if (r.has_value) {
 *     string_t* text = r.u.value;
 *
 *     drop_substr_lit(text, "two", NULL, NULL);
 *     // Result: "one three"
 *
 *     return_string(text);
 * }
 * @endcode
 *
 * @see drop_substr
 * @see find_substr_lit
 */
void drop_substr_lit(string_t* s, const char* substring, uint8_t* min_ptr, uint8_t* max_ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Remove all non-overlapping occurrences of a substring within a window.
 *
 * This function removes every **non-overlapping** occurrence of @p substring
 * from the string @p s that lies within the byte range `[begin, end)`.
 *
 * Removal is performed **in-place** by shifting the remaining suffix of the
 * string left using `memmove`, preserving the terminating NUL byte and
 * maintaining valid C-string semantics.
 *
 * To minimize data movement, matches are located using a **reverse search**
 * strategy so that shrinking operations occur from right-to-left.
 *
 * After each removal, if a single ASCII space `' '` immediately follows the
 * removed substring, that space is also removed.  
 * (This helps avoid leaving double-spaces when removing words.)
 *
 * @param s
 * Pointer to the destination @ref string_t to modify.
 *
 * @param substring
 * Pointer to the substring to remove.
 *
 * @param begin
 * Optional pointer to the first byte of the search window within `s->str`.  
 * If `NULL`, the window begins at the start of the used string.
 *
 * @param end
 * Optional pointer to one-past-the-last byte of the search window.  
 * If `NULL`, the window extends to the end of the used string.
 *
 * @note
 * - The window `[begin, end)` must lie within the string allocation.
 * - If @p end exceeds the **used length**, it is clamped to `s->len`.
 * - If arguments are invalid, the function performs a **silent no-op**.
 * - Matches are **substring-based**, not word-delimited.
 * - Only **one trailing ASCII space** is removed per match.
 *
 * @par Example
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r1 = init_string("alpha beta beta gamma", 0u, a);
 * string_expect_t r2 = init_string("beta", 0u, a);
 *
 * if (r1.has_value && r2.has_value) {
 *     string_t* text = r1.u.value;
 *     string_t* word = r2.u.value;
 *
 *     drop_substr(text, word, NULL, NULL);
 *     // Result: "alpha gamma"
 *
 *     return_string(word);
 *     return_string(text);
 * }
 * @endcode
 *
 * @see drop_substr_lit
 * @see find_substr
 */
void drop_substr(string_t* s, const string_t* substring, uint8_t* min_ptr, uint8_t* max_ptr);
// -------------------------------------------------------------------------------- 

#if defined(ARENA_USE_CONVENIENCE_MACROS) && !defined(NO_FUNCTION_MACROS)

/* Helper: compile-time check (expression-safe) */
#define CSALT_STATIC_ASSERT_EXPR_(cond, name) \
    ((void)sizeof(char[(cond) ? 1 : -1]))

/* RHS type support check for drop_substring(s, needle, ...) */
#define DROP_SUBSTRING_SUPPORTED_NEEDLE_(x) \
    _Generic((x), \
        const char*:      1, \
        char*:            1, \
        const string_t*:  1, \
        string_t*:        1, \
        default:          0)

/* Enforce supported RHS types at compile time (C11). */
#define DROP_SUBSTRING_TYPECHECK_(x) \
    CSALT_STATIC_ASSERT_EXPR_(DROP_SUBSTRING_SUPPORTED_NEEDLE_(x), \
                              drop_substring_unsupported_needle_type)

/**
 * @brief Type-safe generic substring removal convenience macro.
 *
 * @details
 * `drop_substring(s, needle, begin, end)` provides a single interface for
 * removing all occurrences of a substring from a @ref string_t within the
 * byte window `[begin, end)`. The correct implementation is selected at
 * **compile time** using the C11 `_Generic` operator.
 *
 * Dispatch rules:
 *
 * - If @p needle is a C string (`const char*` or `char*`), this macro expands to:
 *   @ref drop_substr_lit((string_t*)s, (const char*)needle, begin, end)
 *
 * - If @p needle is a string object (`const string_t*` or `string_t*`), this
 *   macro expands to:
 *   @ref drop_substr((string_t*)s, (const string_t*)needle, begin, end)
 *
 * In other words, the macro performs **zero runtime type checks** and adds
 * no dispatch overhead—selection happens entirely at compile time.
 *
 * Availability:
 * - Enabled only when `ARENA_USE_CONVENIENCE_MACROS` is defined, and
 * - Disabled when `NO_FUNCTION_MACROS` is defined (to support MISRA-style builds).
 *
 * @param s
 * Pointer to the destination @ref string_t to modify.
 *
 * @param needle
 * Substring to remove. Must be one of:
 * `const char*`, `char*`, `const string_t*`, `string_t*`.
 *
 * @param begin
 * Optional pointer to the first byte of the removal window within `s->str`.
 * Pass `NULL` to start at the beginning of the used string.
 *
 * @param end
 * Optional pointer to one-past-the-last byte of the removal window.
 * Pass `NULL` to end at the used length of the string.
 *
 * @note
 * If @p needle is not one of the supported types, this macro triggers a
 * **compile-time error** in C11 builds via @ref DROP_SUBSTRING_TYPECHECK_.
 *
 * @note
 * The behavior (non-overlapping removal, reverse search optimization, optional
 * removal of a single trailing ASCII space after each match, window clamping)
 * is defined by the selected underlying function:
 * - @ref drop_substr
 * - @ref drop_substr_lit
 *
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("alpha beta beta gamma", 0u, a);
 * assert_true(r.has_value);
 * string_t* text = r.u.value;
 *
 * // Dispatches to drop_substr_lit(text, "beta", NULL, NULL)
 * drop_substring(text, "beta", NULL, NULL);
 * // text->str == "alpha gamma"
 *
 * // Rebuild input for the string_t needle example:
 * return_string(text);
 * r = init_string("alpha beta beta gamma", 0u, a);
 * assert_true(r.has_value);
 * text = r.u.value;
 *
 * string_expect_t rn = init_string("beta", 0u, a);
 * assert_true(rn.has_value);
 * string_t* needle = rn.u.value;
 *
 * // Dispatches to drop_substr(text, needle, NULL, NULL)
 * drop_substring(text, needle, NULL, NULL);
 * // text->str == "alpha gamma"
 *
 * return_string(needle);
 * return_string(text);
 * @endcode
 *
 * @see drop_substr
 * @see drop_substr_lit
 */
#define drop_substring(s, needle, begin, end) \
    (DROP_SUBSTRING_TYPECHECK_(needle), \
     _Generic((needle), \
        const char*:      drop_substr_lit, \
        char*:            drop_substr_lit, \
        const string_t*:  drop_substr, \
        string_t*:        drop_substr \
     )((string_t*)(s), (needle), (begin), (end)))

#endif /* ARENA_USE_CONVENIENCE_MACROS && !NO_FUNCTION_MACROS */
// -------------------------------------------------------------------------------- 

/**
 * @brief Replace all non-overlapping occurrences of a literal substring in-place.
 *
 * Replaces every **case-sensitive, non-overlapping** occurrence of the
 * NUL-terminated C string @p pattern with @p replace_string inside the
 * byte window `[min_ptr, max_ptr)` of @p string.
 *
 * The operation is performed **in-place** using allocator-aware resizing:
 *
 * 1. The number of matches is determined using @ref word_count_lit.
 * 2. The final required string length is computed before modification.
 * 3. If necessary, the buffer is **reallocated once** via the string’s
 *    associated allocator.
 * 4. Matches are processed using **reverse search**
 *    (@ref find_substr_lit with `REVERSE`) to minimize the total
 *    `memmove` cost.
 *
 * @param string
 * Pointer to the destination @ref string_t to modify.
 *
 * @param pattern
 * NUL-terminated substring to search for.
 *
 * @param replace_string
 * NUL-terminated replacement substring.
 *
 * @param min_ptr
 * Optional pointer to the first byte of the replacement window within
 * `string->str`.  
 * If `NULL`, the window begins at the start of the used string.
 *
 * @param max_ptr
 * Optional pointer to one-past-the-last byte of the replacement window.  
 * If `NULL`, the window extends to the end of the used string.
 *
 * @return
 * `true` if the operation completed successfully or no replacements were
 * required.  
 * `false` if:
 * - any argument is invalid
 * - the window lies outside the string allocation
 * - memory reallocation fails
 *
 * @note
 * - Matching is **case-sensitive** and **substring-based**.
 * - Replacements are **non-overlapping**.
 * - The window is interpreted as `[min_ptr, max_ptr)` (end exclusive).
 * - The terminating NUL byte is always preserved.
 * - On failure, the original string contents remain unchanged.
 *
 * @par Example
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("red green red blue", 0u, a);
 * if (!r.has_value) {
 *     // handle allocation failure
 * }
 *
 * string_t* s = r.u.value;
 *
 * replace_substr_lit(s, "red", "yellow", NULL, NULL);
 * // Result: "yellow green yellow blue"
 *
 * return_string(s);
 * @endcode
 *
 * @see replace_substr
 * @see find_substr_lit
 * @see word_count_lit
 */
bool replace_substr_lit(string_t* string, const char* pattern, const char* replace_string,
                        uint8_t* min_ptr, uint8_t* max_ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Replace all non-overlapping occurrences of a substring in-place.
 *
 * Replaces every **case-sensitive, non-overlapping** occurrence of
 * @p pattern with @p replace_string inside the byte window
 * `[min_ptr, max_ptr)` of @p string.
 *
 * This function is the @ref string_t-based counterpart to
 * @ref replace_substr_lit and follows the same allocator-aware algorithm:
 *
 * 1. Match count determined using @ref word_count.
 * 2. Final length computed before modification.
 * 3. Buffer resized **once** via the string’s allocator if required.
 * 4. Replacement performed using **reverse search**
 *    (@ref find_substr with `REVERSE`) to minimize memory movement.
 *
 * @param string
 * Pointer to the destination @ref string_t to modify.
 *
 * @param pattern
 * Substring to search for.
 *
 * @param replace_string
 * Replacement substring.
 *
 * @param min_ptr
 * Optional pointer to the first byte of the replacement window within
 * `string->str`.  
 * If `NULL`, the window begins at the start of the used string.
 *
 * @param max_ptr
 * Optional pointer to one-past-the-last byte of the replacement window.  
 * If `NULL`, the window extends to the end of the used string.
 *
 * @return
 * `true` if the operation completed successfully or no replacements were
 * required.  
 * `false` if:
 * - any argument is invalid
 * - the window lies outside the string allocation
 * - memory reallocation fails
 *
 * @note
 * - Matching is **case-sensitive** and **substring-based**.
 * - Replacements are **non-overlapping**.
 * - The window is interpreted as `[min_ptr, max_ptr)` (end exclusive).
 * - The terminating NUL byte is preserved.
 * - On failure, the original string contents remain unchanged.
 *
 * @par Example
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r1 = init_string("one two two three", 0u, a);
 * string_expect_t r2 = init_string("two", 0u, a);
 * string_expect_t r3 = init_string("four", 0u, a);
 *
 * if (r1.has_value && r2.has_value && r3.has_value) {
 *     string_t* s   = r1.u.value;
 *     string_t* pat = r2.u.value;
 *     string_t* rep = r3.u.value;
 *
 *     replace_substr(s, pat, rep, NULL, NULL);
 *     // Result: "one four four three"
 *
 *     return_string(rep);
 *     return_string(pat);
 *     return_string(s);
 * }
 * @endcode
 *
 * @see replace_substr_lit
 * @see find_substr
 * @see word_count
 */
bool replace_substr(string_t* string, const string_t* pattern, const string_t* replace_string,
                    char* min_ptr, char* max_ptr);
// -------------------------------------------------------------------------------- 

#if defined(ARENA_USE_CONVENIENCE_MACROS) && !defined(NO_FUNCTION_MACROS)

static inline bool _replace_substring_lit_wrap_(string_t* s,
                                               const void* pat,
                                               const void* rep,
                                               void* minp,
                                               void* maxp)
{
    return replace_substr_lit(s,
                              (const char*)pat,
                              (const char*)rep,
                              (uint8_t*)minp,
                              (uint8_t*)maxp);
}

static inline bool _replace_substring_str_wrap_(string_t* s,
                                               const void* pat,
                                               const void* rep,
                                               void* minp,
                                               void* maxp)
{
    return replace_substr(s,
                          (const string_t*)pat,
                          (const string_t*)rep,
                          (char*)minp,
                          (char*)maxp);
}


/* Helper: compile-time check (expression-safe) */
#define CSALT_STATIC_ASSERT_EXPR_(cond, name) \
    ((void)sizeof(char[(cond) ? 1 : -1]))

#define REPLACE_SUBSTRING_IS_LIT_(x) \
    _Generic((x), const char*: 1, char*: 1, default: 0)

#define REPLACE_SUBSTRING_IS_STR_(x) \
    _Generic((x), const string_t*: 1, string_t*: 1, default: 0)

#define REPLACE_SUBSTRING_SUPPORTED_(x) \
    _Generic((x), \
        const char*:      1, \
        char*:            1, \
        const string_t*:  1, \
        string_t*:        1, \
        default:          0)

#define REPLACE_SUBSTRING_PATTERN_TYPECHECK_(x) \
    CSALT_STATIC_ASSERT_EXPR_(REPLACE_SUBSTRING_SUPPORTED_(x), \
                              replace_substring_unsupported_pattern_type)

#define REPLACE_SUBSTRING_REPL_TYPECHECK_(x) \
    CSALT_STATIC_ASSERT_EXPR_(REPLACE_SUBSTRING_SUPPORTED_(x), \
                              replace_substring_unsupported_replacement_type)

#define REPLACE_SUBSTRING_CATEGORY_MATCH_(pat, rep) \
    CSALT_STATIC_ASSERT_EXPR_( \
        (REPLACE_SUBSTRING_IS_LIT_(pat) && REPLACE_SUBSTRING_IS_LIT_(rep)) || \
        (REPLACE_SUBSTRING_IS_STR_(pat) && REPLACE_SUBSTRING_IS_STR_(rep)), \
        replace_substring_pattern_and_replacement_type_mismatch)

/**
 * @brief Type-safe generic substring replacement convenience macro.
 *
 * @details
 * `replace_substring(s, pattern, replacement, min_ptr, max_ptr)` provides a
 * single replacement interface that selects the correct implementation at
 * **compile time** using the C11 `_Generic` operator.
 *
 * Dispatch rules:
 *
 * - If @p pattern (and @p replacement) are C strings (`const char*` or `char*`),
 *   this macro expands to:
 *   @ref replace_substr_lit((string_t*)s, (const char*)pattern, (const char*)replacement,
 *                           (uint8_t*)min_ptr, (uint8_t*)max_ptr)
 *
 * - If @p pattern (and @p replacement) are string objects (`const string_t*` or `string_t*`),
 *   this macro expands to:
 *   @ref replace_substr((string_t*)s, (const string_t*)pattern, (const string_t*)replacement,
 *                       (char*)min_ptr, (char*)max_ptr)
 *
 * The macro enforces at compile time that:
 * - @p pattern is one of: `const char*`, `char*`, `const string_t*`, `string_t*`
 * - @p replacement is one of the same supported types
 * - @p pattern and @p replacement belong to the same category (both literal, or both string objects)
 *
 * Availability:
 * - Enabled only when `ARENA_USE_CONVENIENCE_MACROS` is defined, and
 * - Disabled when `NO_FUNCTION_MACROS` is defined (to support MISRA-style builds).
 *
 * @param s
 * Pointer to the destination @ref string_t to modify.
 *
 * @param pattern
 * Substring pattern to search for (literal or @ref string_t).
 *
 * @param replacement
 * Replacement substring (must match the category of @p pattern).
 *
 * @param min_ptr
 * Optional pointer to the first byte of the replacement window within `s->str`.
 * Pass `NULL` to start at the beginning of the used string.
 *
 * @param max_ptr
 * Optional pointer to one-past-the-last byte of the replacement window.
 * Pass `NULL` to end at the used length of the string.
 *
 * @return
 * `true`/`false` as returned by the selected underlying function.
 *
 * @note
 * The window `[min_ptr, max_ptr)` is interpreted as **end-exclusive**.
 * Both underlying implementations validate that the window lies within the
 * string allocation and clamp to the used length.
 *
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * // Literal version
 * string_expect_t r = init_string("red green red", 0u, a);
 * assert_true(r.has_value);
 * string_t* s = r.u.value;
 *
 * // Dispatches to replace_substr_lit(...)
 * (void)replace_substring(s, "red", "blue", NULL, NULL);
 * // s->str == "blue green blue"
 *
 * return_string(s);
 *
 * // string_t version
 * string_expect_t r1 = init_string("one two two", 0u, a);
 * string_expect_t r2 = init_string("two", 0u, a);
 * string_expect_t r3 = init_string("four", 0u, a);
 *
 * if (r1.has_value && r2.has_value && r3.has_value) {
 *     string_t* t   = r1.u.value;
 *     string_t* pat = r2.u.value;
 *     string_t* rep = r3.u.value;
 *
 *     // Dispatches to replace_substr(...)
 *     (void)replace_substring(t, pat, rep, NULL, NULL);
 *     // t->str == "one four four"
 *
 *     return_string(rep);
 *     return_string(pat);
 *     return_string(t);
 * }
 * @endcode
 *
 * @see replace_substr
 * @see replace_substr_lit
 */
#define replace_substring(s, pattern, replacement, min_ptr, max_ptr) \
    (REPLACE_SUBSTRING_PATTERN_TYPECHECK_(pattern), \
     REPLACE_SUBSTRING_REPL_TYPECHECK_(replacement), \
     REPLACE_SUBSTRING_CATEGORY_MATCH_(pattern, replacement), \
     _Generic((pattern), \
        const char*:      _replace_substring_lit_wrap_, \
        char*:            _replace_substring_lit_wrap_, \
        const string_t*:  _replace_substring_str_wrap_, \
        string_t*:        _replace_substring_str_wrap_ \
     )((string_t*)(s), (const void*)(pattern), (const void*)(replacement), \
       (void*)(min_ptr), (void*)(max_ptr)))

#endif /* ARENA_USE_CONVENIENCE_MACROS && !NO_FUNCTION_MACROS */
// -------------------------------------------------------------------------------- 

/**
 * @brief Pop the substring to the right of the last string_t token occurrence.
 *
 * @details
 * Searches for the **last (reverse) occurrence** of the substring specified
 * by @p token within the used portion of @p s.
 *
 * If found:
 * 1. The substring strictly to the right of the token is copied into a new
 *    @ref string_t using the supplied @p allocator.
 * 2. The original string @p s is truncated at the beginning of the token.
 *
 * The token itself is removed from @p s.
 *
 * Example:
 *
 *   Input string:   "one::two::three"
 *   Token:          "::"
 *
 *   Result:
 *     Returned string -> "three"
 *     Modified input  -> "one::two"
 *
 * Matching is case-sensitive.
 *
 * @param s
 * Pointer to the source @ref string_t to modify.
 *
 * @param token
 * Pointer to a @ref string_t representing the token substring.
 * Must not be `NULL`, and must have non-zero length.
 *
 * @param allocator
 * Allocator used to construct the returned string.
 *
 * @return
 * A @ref string_expect_t containing:
 *
 * @retval has_value == true
 * A newly allocated string containing the substring to the right of the
 * last token occurrence.
 *
 * @retval has_value == false
 * If:
 * - `s == NULL`
 * - `s->str == NULL`
 * - `token == NULL`
 * - `token->str == NULL`
 * - `token->len == 0`
 * - token is not found in `s`
 * - allocation fails
 *
 * @note
 * - The original string is modified only if the token is found.
 * - The returned string must be released by the caller.
 * - The search is performed using @ref find_substr in REVERSE mode.
 *
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t rs = init_string("path/to/file.txt", 0u, a);
 * string_expect_t rt = init_string("/", 0u, a);
 *
 * if (rs.has_value && rt.has_value) {
 *     string_t* s = rs.u.value;
 *     string_t* t = rt.u.value;
 *
 *     string_expect_t out = pop_str_token(s, t, a);
 *     assert_true(out.has_value);
 *
 *     // out.u.value->str == "file.txt"
 *     // s->str == "path/to"
 *
 *     return_string(out.u.value);
 *     return_string(t);
 *     return_string(s);
 * }
 * @endcode
 *
 * @see pop_str_token_lit
 * @see find_substr
 */
string_expect_t pop_str_token(string_t* s, 
                              const string_t* token, 
                              allocator_vtable_t allocator);
// -------------------------------------------------------------------------------- 

/**
 * @brief Pop the substring to the right of the last literal token occurrence.
 *
 * @details
 * Searches for the **last (reverse) occurrence** of the C string literal
 * `token` within the used portion of @p s. If found, all characters strictly
 * to the right of the token are:
 *
 * 1. Copied into a newly allocated @ref string_t (using the supplied
 *    @p allocator), and
 * 2. Removed from @p s by shrinking its logical length and resetting the
 *    null terminator.
 *
 * The token itself is also removed from @p s.
 *
 * Example:
 *
 *   Input string:   "alpha/beta/gamma"
 *   Token:          "/"
 *
 *   Result:
 *     Returned string -> "gamma"
 *     Modified input  -> "alpha/beta"
 *
 * Matching is case-sensitive.
 *
 * @param s
 * Pointer to the source @ref string_t to modify.
 *
 * @param token
 * Null-terminated C string literal representing the token.
 * Must not be `NULL` or empty.
 *
 * @param allocator
 * Allocator used to construct the returned string.
 *
 * @return
 * A @ref string_expect_t containing:
 *
 * @retval has_value == true
 * A newly allocated string containing the substring to the right of the
 * last token occurrence.
 *
 * @retval has_value == false
 * If:
 * - `s == NULL`
 * - `s->str == NULL`
 * - `token == NULL`
 * - `token` is empty
 * - token is not found in `s`
 * - allocation fails
 *
 * @note
 * - The original string is modified only if the token is found.
 * - The returned string is independent and must be released by the caller.
 * - The search is performed using @ref find_substr_lit in REVERSE mode.
 *
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 *
 * string_expect_t r = init_string("red/green/blue", 0u, a);
 * assert_true(r.has_value);
 *
 * string_t* s = r.u.value;
 *
 * string_expect_t popped = pop_str_token_lit(s, "/", a);
 * assert_true(popped.has_value);
 *
 * // popped.u.value->str == "blue"
 * // s->str == "red/green"
 *
 * return_string(popped.u.value);
 * return_string(s);
 * @endcode
 *
 * @see pop_str_token
 * @see find_substr_lit
 */
string_expect_t pop_str_token_lit(string_t* s, 
                                  const char* token, 
                                  allocator_vtable_t allocator);
// -------------------------------------------------------------------------------- 

#if defined(ARENA_USE_CONVENIENCE_MACROS) && !defined(NO_FUNCTION_MACROS)

/* Helper: compile-time check (expression-safe) */
#define CSALT_STATIC_ASSERT_EXPR_(cond, name) \
    ((void)sizeof(char[(cond) ? 1 : -1]))

/* Supported token types for pop_string_token(s, token, allocator) */
#define POP_STRING_TOKEN_SUPPORTED_(x) \
    _Generic((x), \
        const char*:      1, \
        char*:            1, \
        const string_t*:  1, \
        string_t*:        1, \
        default:          0)

#define POP_STRING_TOKEN_TYPECHECK_(x) \
    CSALT_STATIC_ASSERT_EXPR_(POP_STRING_TOKEN_SUPPORTED_(x), \
                              pop_string_token_unsupported_token_type)

/* Internal wrappers unify the callable signature for _Generic selection */
static inline string_expect_t _pop_string_token_lit_wrap_(string_t* s,
                                                          const void* token,
                                                          allocator_vtable_t a)
{
    return pop_str_token_lit(s, (const char*)token, a);
}

static inline string_expect_t _pop_string_token_str_wrap_(string_t* s,
                                                          const void* token,
                                                          allocator_vtable_t a)
{
    return pop_str_token(s, (const string_t*)token, a);
}

/**
 * @brief Type-safe generic token pop convenience macro.
 *
 * @details
 * `pop_string_token(s, token, allocator)` selects the correct implementation
 * at **compile time** using the C11 `_Generic` operator.
 *
 * Dispatch rules:
 * - If @p token is a C string (`const char*` or `char*`), dispatch to
 *   @ref pop_str_token_lit.
 * - If @p token is a string object (`const string_t*` or `string_t*`), dispatch
 *   to @ref pop_str_token.
 *
 * This macro performs **no runtime type checks** and adds no dispatch overhead.
 *
 * Availability:
 * - Enabled only when `ARENA_USE_CONVENIENCE_MACROS` is defined, and
 * - Disabled when `NO_FUNCTION_MACROS` is defined.
 *
 * @param s
 * Pointer to the source @ref string_t to modify.
 *
 * @param token
 * Token to search for (literal or @ref string_t).
 * Must be one of: `const char*`, `char*`, `const string_t*`, `string_t*`.
 *
 * @param allocator
 * Allocator used to construct the returned string.
 *
 * @return
 * A @ref string_expect_t as returned by the selected underlying function.
 *
 * @note
 * If @p token is not a supported type, this macro triggers a **compile-time error**
 * in C11 builds.
 *
 * @code{.c}
 * allocator_vtable_t a = heap_allocator();
 * string_expect_t r = init_string("a/b/c", 0u, a);
 * assert_true(r.has_value);
 * string_t* s = r.u.value;
 *
 * // Dispatches to pop_str_token_lit(s, "/", a)
 * string_expect_t out1 = pop_string_token(s, "/", a);
 * assert_true(out1.has_value);
 * // out1.u.value->str == "c"
 * // s->str == "a/b"
 * return_string(out1.u.value);
 *
 * string_expect_t rt = init_string("/", 0u, a);
 * assert_true(rt.has_value);
 * string_t* tok = rt.u.value;
 *
 * // Dispatches to pop_str_token(s, tok, a)
 * string_expect_t out2 = pop_string_token(s, tok, a);
 * (void)out2;
 *
 * return_string(tok);
 * return_string(s);
 * @endcode
 *
 * @see pop_str_token_lit
 * @see pop_str_token
 */
#define pop_string_token(s, token, allocator) \
    (POP_STRING_TOKEN_TYPECHECK_(token), \
     _Generic((token), \
        const char*:      _pop_string_token_lit_wrap_, \
        char*:            _pop_string_token_lit_wrap_, \
        const string_t*:  _pop_string_token_str_wrap_, \
        string_t*:        _pop_string_token_str_wrap_ \
     )((string_t*)(s), (const void*)(token), (allocator)))

#endif /* ARENA_USE_CONVENIENCE_MACROS && !NO_FUNCTION_MACROS */
// -------------------------------------------------------------------------------- 

static inline void fprint_string(FILE* stream, const string_t* s) {
    if (!stream || !s || !s->str) return;
    fwrite(s->str, 1u, s->len, stream);
}
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
