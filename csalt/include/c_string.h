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
