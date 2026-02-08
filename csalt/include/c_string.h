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
