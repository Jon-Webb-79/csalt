// ================================================================================
// ================================================================================
// - File:    c_uint8.h
// - Purpose: This file contains prototypes for uint8_t data structures and 
//            associated functions
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 23, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here
#ifndef c_uint8_H
#define c_uint8_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "c_array.h"
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

/**
 * @brief A type-safe dynamic array of uint8_t elements.
 *
 * Wraps array_t with the dtype fixed to UINT8_TYPE at initialization.
 * All operations enforce UINT8_TYPE at the call site so the caller never
 * needs to pass a dtype_id_t argument. The underlying array_t is embedded
 * by value so the struct can be stack- or heap-allocated by the caller.
 */
typedef struct {
    array_t base; /**< Underlying generic array. Do not modify directly. */
} uint8_array_t;

// ================================================================================
// Expected type
// ================================================================================

/**
 * @brief Expected return type for uint8_array_t initialization and copy/slice.
 */
typedef struct {
    bool has_value;
    union {
        uint8_array_t* value;
        error_code_t   error;
    } u;
} uint8_array_expect_t;
// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialize a new heap-allocated uint8_array_t.
 *
 * Allocates both the array_t struct and its internal data buffer through
 * alloc_v. The element type is fixed to UINT8_TYPE and the element size is
 * cached as 1 byte. The array starts empty (len == 0) with the full capacity
 * reserved. When growth is true the data buffer will be reallocated
 * automatically whenever a push operation would exceed the current capacity;
 * when false any push beyond capacity returns CAPACITY_OVERFLOW without
 * modifying the array. The caller is responsible for calling
 * return_uint8_array when the array is no longer needed.
 *
 * @param capacity  Initial number of elements the buffer can hold.
 *                  Must be > 0.
 * @param growth    Pass true to allow the buffer to grow automatically on
 *                  overflow, false to use a fixed-capacity buffer.
 * @param alloc_v   Allocator vtable used for all internal memory operations.
 *                  alloc_v.allocate must not be NULL.
 *
 * @return uint8_array_expect_t with has_value true and u.value pointing to
 *         the initialized array on success. On failure has_value is false
 *         and u.error is one of:
 *         - NULL_POINTER    if alloc_v.allocate is NULL
 *         - INVALID_ARG     if capacity is 0
 *         - ILLEGAL_STATE   if the dtype registry could not be initialized
 *         - LENGTH_OVERFLOW if capacity would overflow size_t
 *         - BAD_ALLOC       if the struct allocation fails
 *         - OUT_OF_MEMORY   if the data buffer allocation fails
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *
 *     // Fixed-capacity buffer of 32 elements, no automatic growth.
 *     uint8_array_expect_t result = init_uint8_array(32, false, alloc);
 *     if (!result.has_value) {
 *         fprintf(stderr, "init failed: %d\n", result.u.error);
 *         return;
 *     }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 20);
 *     push_back_uint8_array(arr, 30);
 *     // arr now contains [10, 20, 30], len == 3, alloc == 32.
 *
 *     return_uint8_array(arr);
 * @endcode
 */
uint8_array_expect_t init_uint8_array(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v);
// --------------------------------------------------------------------------------

/**
 * @brief Release all memory owned by a uint8_array_t back to its allocator.
 *
 * Frees the internal data buffer and then the array_t struct itself using
 * the allocator vtable that was supplied at initialization. After this call
 * the pointer is invalid and must not be dereferenced. Passing NULL is safe
 * and has no effect, making it suitable for use in cleanup paths without a
 * prior NULL check.
 *
 * @param array  Pointer to the array to release. May be NULL.
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 255);
 *
 *     // Release all memory. arr must not be used after this point.
 *     return_uint8_array(arr);
 *     arr = NULL;  // Optional but recommended to prevent accidental reuse.
 *
 *     // Passing NULL is always safe.
 *     return_uint8_array(NULL);
 * @endcode
 */
void return_uint8_array(uint8_array_t* array);
// ================================================================================
// Push operations
// ================================================================================

/**
 * @brief Append one uint8_t to the end of the array.
 *
 * Copies value into the next available slot at index array->base.len and
 * increments len by one. If the array is full and growth is true the data
 * buffer is reallocated before the value is written; if growth is false the
 * call fails immediately with CAPACITY_OVERFLOW and the array is left
 * unmodified. This is an O(1) amortised operation when growth is true and
 * strictly O(1) when the buffer has spare capacity.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param value  The uint8_t value to append.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(4, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 20);
 *     push_back_uint8_array(arr, 30);
 *     // arr contains [10, 20, 30], len == 3.
 *
 *     uint8_t out = 0;
 *     get_uint8_array_index(arr, 2, &out);  // out == 30
 *
 *     return_uint8_array(arr);
 * @endcode
 */
error_code_t push_back_uint8_array(uint8_array_t* array, uint8_t value);
// --------------------------------------------------------------------------------

/**
 * @brief Prepend one uint8_t to the front of the array.
 *
 * Shifts all existing elements one position toward the back using memmove,
 * writes value into index 0, and increments len by one. If the array is full
 * and growth is true the data buffer is reallocated before the shift; if
 * growth is false the call fails immediately with CAPACITY_OVERFLOW and the
 * array is left unmodified. Because every existing element must be moved this
 * is an O(n) operation — prefer push_back_uint8_array for performance-sensitive
 * append-heavy workloads.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param value  The uint8_t value to prepend.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 20);
 *     push_back_uint8_array(arr, 30);
 *     // arr contains [20, 30].
 *
 *     push_front_uint8_array(arr, 10);
 *     // arr now contains [10, 20, 30].
 *
 *     // Prepending in sequence reverses insertion order:
 *     // push_front(1), push_front(2), push_front(3) -> [3, 2, 1].
 *
 *     return_uint8_array(arr);
 * @endcode
 */
error_code_t push_front_uint8_array(uint8_array_t* array, uint8_t value);
// --------------------------------------------------------------------------------

/**
 * @brief Insert one uint8_t at an arbitrary position in the array.
 *
 * Shifts all elements from index onward one position toward the back using
 * memmove, writes value at the given index, and increments len by one. If
 * the array is full and growth is true the data buffer is reallocated before
 * the shift; if growth is false the call fails with CAPACITY_OVERFLOW and
 * the array is left unmodified. Inserting at index 0 is equivalent to
 * push_front_uint8_array; inserting at index == len is equivalent to
 * push_back_uint8_array. Both boundary cases are accepted without error.
 * Inserting at any other valid position is an O(n) operation due to the
 * element shift.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param index  Position at which to insert. Must be <= array->base.len.
 *               Elements at [index, len) are shifted right by one.
 * @param value  The uint8_t value to insert.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - OUT_OF_BOUNDS     if index > array->base.len
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 30);
 *     // arr contains [10, 30].
 *
 *     push_at_uint8_array(arr, 1, 20);
 *     // arr now contains [10, 20, 30].
 *
 *     // Boundary cases:
 *     push_at_uint8_array(arr, 0, 5);   // prepend -> [5, 10, 20, 30]
 *     push_at_uint8_array(arr, 4, 40);  // append  -> [5, 10, 20, 30, 40]
 *
 *     return_uint8_array(arr);
 * @endcode
 */
error_code_t push_at_uint8_array(uint8_array_t* array,
                                  size_t         index,
                                  uint8_t        value);
// ================================================================================
// Get operation
// ================================================================================

/**
 * @brief Retrieve the uint8_t at a given index, copying it into out.
 *
 * Performs a bounds-checked read of the element at index. The value is
 * copied into the memory pointed to by out — the array itself is not
 * modified and len is unchanged. The function is safe to call on a const
 * array and may be called repeatedly on the same index without side effects.
 *
 * @param array  Pointer to the source array. Must not be NULL.
 * @param index  Zero-based position to read. Must be < array->base.len.
 *               Passing an index equal to len or beyond it is an error even
 *               if spare capacity exists at that offset in the buffer.
 * @param out    Pointer to a uint8_t that receives the copied value.
 *               Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array or out is NULL
 *         - OUT_OF_BOUNDS if index >= array->base.len
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 20);
 *     push_back_uint8_array(arr, 30);
 *     // arr contains [10, 20, 30], len == 3.
 *
 *     uint8_t val = 0;
 *     get_uint8_array_index(arr, 0, &val);  // val == 10
 *     get_uint8_array_index(arr, 1, &val);  // val == 20
 *     get_uint8_array_index(arr, 2, &val);  // val == 30
 *
 *     // Reading past the end is an error regardless of buffer capacity.
 *     error_code_t err = get_uint8_array_index(arr, 3, &val);
 *     // err == OUT_OF_BOUNDS, val is unchanged.
 *
 *     return_uint8_array(arr);
 * @endcode
 */
error_code_t get_uint8_array_index(const uint8_array_t* array,
                                    size_t               index,
                                    uint8_t*             out);
// ================================================================================
// Pop operations
// ================================================================================

/**
 * @brief Remove the last element from the array and optionally retrieve it.
 *
 * Decrements len by one. If out is non-NULL the removed value is copied into
 * it before len is decremented; if out is NULL the value is silently
 * discarded. The data buffer is not reallocated or zeroed — the element slot
 * becomes spare capacity. This is an O(1) operation. The array must contain
 * at least one element; calling on an empty array is an error.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param out    Pointer that receives the removed value. May be NULL if the
 *               value is not needed.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 20);
 *     push_back_uint8_array(arr, 30);
 *     // arr contains [10, 20, 30], len == 3.
 *
 *     uint8_t val = 0;
 *     pop_back_uint8_array(arr, &val);  // val == 30, len == 2
 *     pop_back_uint8_array(arr, &val);  // val == 20, len == 1
 *     pop_back_uint8_array(arr, NULL);  // discard,   len == 0
 *
 *     // Popping from an empty array is an error.
 *     error_code_t err = pop_back_uint8_array(arr, &val);
 *     // err == EMPTY, arr and val are unchanged.
 *
 *     return_uint8_array(arr);
 * @endcode
 */
error_code_t pop_back_uint8_array(uint8_array_t* array, uint8_t* out);
// --------------------------------------------------------------------------------

/**
 * @brief Remove the first element from the array and optionally retrieve it.
 *
 * Copies the value at index 0 into out (if non-NULL), then shifts all
 * remaining elements one position toward the front using memmove, and
 * decrements len by one. The data buffer is not reallocated. Because every
 * remaining element must be moved this is an O(n) operation — prefer
 * pop_back_uint8_array for performance-sensitive drain loops. The array must
 * contain at least one element; calling on an empty array is an error.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param out    Pointer that receives the removed value. May be NULL if the
 *               value is not needed.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 20);
 *     push_back_uint8_array(arr, 30);
 *     // arr contains [10, 20, 30], len == 3.
 *
 *     uint8_t val = 0;
 *     pop_front_uint8_array(arr, &val);  // val == 10, arr is [20, 30]
 *     pop_front_uint8_array(arr, &val);  // val == 20, arr is [30]
 *     pop_front_uint8_array(arr, NULL);  // discard,   arr is []
 *
 *     return_uint8_array(arr);
 * @endcode
 */
error_code_t pop_front_uint8_array(uint8_array_t* array, uint8_t* out);
// --------------------------------------------------------------------------------

/**
 * @brief Remove the element at an arbitrary index and optionally retrieve it.
 *
 * Copies the value at index into out (if non-NULL), then shifts all elements
 * from index + 1 onward one position toward the front using memmove, and
 * decrements len by one. The data buffer is not reallocated. Removing from
 * index 0 is equivalent to pop_front_uint8_array; removing from index
 * len - 1 is equivalent to pop_back_uint8_array. Both boundary cases are
 * accepted without error. Removing from any other position is an O(n)
 * operation due to the element shift. The array must contain at least one
 * element and index must be within the live region.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param out    Pointer that receives the removed value. May be NULL if the
 *               value is not needed.
 * @param index  Zero-based position to remove. Must be < array->base.len.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *         - OUT_OF_BOUNDS if index >= array->base.len
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 20);
 *     push_back_uint8_array(arr, 30);
 *     push_back_uint8_array(arr, 40);
 *     // arr contains [10, 20, 30, 40], len == 4.
 *
 *     uint8_t val = 0;
 *     pop_any_uint8_array(arr, &val, 1);  // val == 20, arr is [10, 30, 40]
 *     pop_any_uint8_array(arr, &val, 0);  // val == 10, arr is [30, 40]
 *     pop_any_uint8_array(arr, &val, 1);  // val == 40, arr is [30]
 *
 *     return_uint8_array(arr);
 * @endcode
 */
error_code_t pop_any_uint8_array(uint8_array_t* array,
                                  uint8_t*       out,
                                  size_t         index);
// ================================================================================
// Utility operations
// ================================================================================

/**
 * @brief Reset the array to empty without releasing the data buffer.
 *
 * Sets len to zero and zeroes the contents of the data buffer with memset.
 * The allocated capacity is preserved — the buffer can be reused immediately
 * after the call without any reallocation. This is an O(n) operation
 * proportional to the number of bytes zeroed (alloc * data_size). If you
 * only need to reset len without zeroing the buffer contents, and security
 * or data-hiding is not a concern, you can instead assign array->base.len = 0
 * directly. Use clear_uint8_array when you need a guaranteed clean slate.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 *
 * @return NO_ERROR on success, or NULL_POINTER if array is NULL.
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 20);
 *     push_back_uint8_array(arr, 30);
 *     // arr contains [10, 20, 30], len == 3, alloc == 8.
 *
 *     clear_uint8_array(arr);
 *     // arr is now empty: len == 0, alloc == 8 (buffer retained).
 *
 *     // The array is immediately ready for reuse.
 *     push_back_uint8_array(arr, 99);
 *     // arr contains [99], len == 1.
 *
 *     return_uint8_array(arr);
 * @endcode
 */
error_code_t clear_uint8_array(uint8_array_t* array);
// --------------------------------------------------------------------------------

/**
 * @brief Overwrite the element at a given index in place.
 *
 * Copies value into the slot at index, replacing the current occupant.
 * len, alloc, and all other elements are unchanged. This is a bounds-checked
 * in-place write — it cannot extend the array and will not trigger
 * reallocation. Use push_at_uint8_array to insert a new element and shift
 * existing ones.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param index  Zero-based position to overwrite. Must be < array->base.len.
 * @param value  The replacement uint8_t value.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - OUT_OF_BOUNDS if index >= array->base.len
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(4, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 20);
 *     push_back_uint8_array(arr, 30);
 *     // arr contains [10, 20, 30].
 *
 *     set_uint8_array_index(arr, 1, 99);
 *     // arr now contains [10, 99, 30]. len is still 3.
 *
 *     // Writing past the live region is an error even if capacity exists.
 *     error_code_t err = set_uint8_array_index(arr, 3, 42);
 *     // err == OUT_OF_BOUNDS, arr is unchanged.
 *
 *     return_uint8_array(arr);
 * @endcode
 */
error_code_t set_uint8_array_index(uint8_array_t* array,
                                    size_t         index,
                                    uint8_t        value);
// --------------------------------------------------------------------------------

/**
 * @brief Create an independent deep copy of src using the given allocator.
 *
 * Allocates a new uint8_array_t struct and a new data buffer through alloc_v,
 * then copies all len elements from src into the new array. The copy has the
 * same len, alloc, growth setting, and dtype as src but owns its own buffer —
 * subsequent modifications to src do not affect the copy and vice versa. The
 * copy may be given a different allocator than src. The caller is responsible
 * for calling return_uint8_array on the returned array when it is no longer
 * needed.
 *
 * @param src      Pointer to the source array. Must not be NULL.
 * @param alloc_v  Allocator vtable for the new array's memory.
 *                 alloc_v.allocate must not be NULL.
 *
 * @return uint8_array_expect_t with has_value true and u.value pointing to
 *         the new array on success. On failure has_value is false and
 *         u.error is one of:
 *         - NULL_POINTER  if src is NULL or alloc_v.allocate is NULL
 *         - BAD_ALLOC     if the struct allocation fails
 *         - OUT_OF_MEMORY if the data buffer allocation fails
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* src = result.u.value;
 *
 *     push_back_uint8_array(src, 10);
 *     push_back_uint8_array(src, 20);
 *     push_back_uint8_array(src, 30);
 *
 *     uint8_array_expect_t copy_result = copy_uint8_array(src, alloc);
 *     if (!copy_result.has_value) { return_uint8_array(src); return; }
 *     uint8_array_t* dst = copy_result.u.value;
 *     // src and dst both contain [10, 20, 30] but own separate buffers.
 *
 *     set_uint8_array_index(src, 0, 99);
 *     // src is [99, 20, 30], dst is still [10, 20, 30].
 *
 *     return_uint8_array(src);
 *     return_uint8_array(dst);
 * @endcode
 */
uint8_array_expect_t copy_uint8_array(const uint8_array_t* src,
                                       allocator_vtable_t   alloc_v);
// --------------------------------------------------------------------------------

/**
 * @brief Append all elements of src onto the end of dst in a single operation.
 *
 * Copies all len elements from src into dst starting at dst's current len,
 * then increments dst->base.len by src->base.len. src is treated as read-only
 * and is not modified. If src is empty the function returns NO_ERROR
 * immediately without touching dst. If dst does not have sufficient spare
 * capacity and growth is true, dst's buffer is reallocated once to fit all of
 * src's elements before any data is copied; if growth is false and dst lacks
 * the space, the call fails with CAPACITY_OVERFLOW and dst is left entirely
 * unmodified — no partial copy is performed.
 *
 * @param dst  Pointer to the destination array. Must not be NULL.
 * @param src  Pointer to the source array. Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if dst or src is NULL
 *         - CAPACITY_OVERFLOW if dst lacks space and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the combined length would overflow size_t
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *
 *     uint8_array_expect_t r1 = init_uint8_array(8, false, alloc);
 *     uint8_array_expect_t r2 = init_uint8_array(4, false, alloc);
 *     if (!r1.has_value || !r2.has_value) { return; }
 *     uint8_array_t* dst = r1.u.value;
 *     uint8_array_t* src = r2.u.value;
 *
 *     push_back_uint8_array(dst, 10);
 *     push_back_uint8_array(dst, 20);
 *     push_back_uint8_array(src, 30);
 *     push_back_uint8_array(src, 40);
 *
 *     concat_uint8_array(dst, src);
 *     // dst contains [10, 20, 30, 40], src is unchanged as [30, 40].
 *
 *     return_uint8_array(dst);
 *     return_uint8_array(src);
 * @endcode
 */
error_code_t concat_uint8_array(uint8_array_t*       dst,
                                 const uint8_array_t* src);
// --------------------------------------------------------------------------------

/**
 * @brief Allocate a new array containing the elements of src in [start, end).
 *
 * Allocates a new uint8_array_t and data buffer through alloc_v, then copies
 * the elements at indices [start, end) from src into it. The resulting array
 * has len == end - start and alloc == end - start. src is treated as
 * read-only and is not modified. The returned array owns its own buffer and
 * is completely independent of src — subsequent changes to src do not affect
 * the slice and vice versa. The range is half-open: start is inclusive, end
 * is exclusive. The caller is responsible for calling return_uint8_array on
 * the returned array when it is no longer needed.
 *
 * @param src      Pointer to the source array. Must not be NULL.
 * @param start    Zero-based index of the first element to include (inclusive).
 * @param end      Zero-based index one past the last element to include
 *                 (exclusive). Must be > start and <= src->base.len.
 * @param alloc_v  Allocator vtable for the new array's memory.
 *                 alloc_v.allocate must not be NULL.
 *
 * @return uint8_array_expect_t with has_value true and u.value pointing to
 *         the new array on success. On failure has_value is false and
 *         u.error is one of:
 *         - NULL_POINTER  if src is NULL or alloc_v.allocate is NULL
 *         - INVALID_ARG   if start >= end
 *         - OUT_OF_BOUNDS if end > src->base.len
 *         - BAD_ALLOC     if the struct allocation fails
 *         - OUT_OF_MEMORY if the data buffer allocation fails
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* src = result.u.value;
 *
 *     push_back_uint8_array(src, 10);
 *     push_back_uint8_array(src, 20);
 *     push_back_uint8_array(src, 30);
 *     push_back_uint8_array(src, 40);
 *     push_back_uint8_array(src, 50);
 *     // src contains [10, 20, 30, 40, 50].
 *
 *     uint8_array_expect_t sr = slice_uint8_array(src, 1, 4, alloc);
 *     if (!sr.has_value) { return_uint8_array(src); return; }
 *     uint8_array_t* slc = sr.u.value;
 *     // slc contains [20, 30, 40], len == 3.
 *
 *     // Modifying src does not affect slc.
 *     set_uint8_array_index(src, 1, 99);
 *     // src is [10, 99, 30, 40, 50], slc is still [20, 30, 40].
 *
 *     return_uint8_array(src);
 *     return_uint8_array(slc);
 * @endcode
 */
uint8_array_expect_t slice_uint8_array(const uint8_array_t* src,
                                        size_t               start,
                                        size_t               end,
                                        allocator_vtable_t   alloc_v);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the elements of the array in place.
 *
 * Swaps elements symmetrically around the midpoint so that the element
 * previously at index 0 moves to index len - 1, index 1 moves to
 * len - 2, and so on. The swap is performed using the best SIMD instruction
 * set available at compile time for element sizes of 1, 2, 4, and 8 bytes,
 * falling back to a scalar memcpy swap for all other sizes. len, alloc, and
 * the data buffer pointer are unchanged — no allocation is performed.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if array is NULL
 *         - EMPTY        if array->base.len < 2 (zero or one element,
 *                        nothing to swap)
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 20);
 *     push_back_uint8_array(arr, 30);
 *     push_back_uint8_array(arr, 40);
 *     // arr contains [10, 20, 30, 40].
 *
 *     reverse_uint8_array(arr);
 *     // arr now contains [40, 30, 20, 10].
 *
 *     // Reversing twice restores the original order.
 *     reverse_uint8_array(arr);
 *     // arr is [10, 20, 30, 40] again.
 *
 *     return_uint8_array(arr);
 * @endcode
 */
error_code_t reverse_uint8_array(uint8_array_t* array);
// -------------------------------------------------------------------------------- 

/**
 * @brief Sort the elements of the array in place.
 *
 * Sorts using median-of-three quicksort with an insertion sort fallback for
 * partitions of fewer than 10 elements and tail-call optimisation to keep
 * stack depth O(log n). The comparator is a built-in unsigned byte comparison
 * so no comparator function needs to be supplied. The sort is not stable —
 * equal elements may change relative order. FORWARD produces ascending order
 * (smallest value first); REVERSE produces descending order (largest value
 * first).
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param dir    FORWARD for ascending order, REVERSE for descending order.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if array is NULL
 *         - EMPTY        if array->base.len < 2 (nothing to sort)
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 50);
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 40);
 *     push_back_uint8_array(arr, 20);
 *     push_back_uint8_array(arr, 30);
 *     // arr contains [50, 10, 40, 20, 30].
 *
 *     sort_uint8_array(arr, FORWARD);
 *     // arr is now [10, 20, 30, 40, 50].
 *
 *     sort_uint8_array(arr, REVERSE);
 *     // arr is now [50, 40, 30, 20, 10].
 *
 *     return_uint8_array(arr);
 * @endcode
 */
error_code_t sort_uint8_array(uint8_array_t* array, direction_t dir);
// -------------------------------------------------------------------------------- 

/**
 * @brief Search for the first occurrence of value using binary search.
 *
 * Uses a built-in unsigned byte comparator — the caller does not need to
 * supply one. When sort is true the array is sorted in place in ascending
 * order before searching; the sorted order persists after the call. When
 * sort is false the array must already be sorted in ascending order.
 * After any matching element is found the implementation scans leftward to
 * return the index of the first occurrence, consistent with
 * uint8_array_contains.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param value  The uint8_t value to search for.
 * @param sort   If true, sort the array in place before searching.
 *               If false, the array must already be sorted ascending.
 *
 * @return size_expect_t with has_value true and u.value holding the
 *         zero-based index of the first match on success. On failure
 *         has_value is false and u.error is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *         - NOT_FOUND     if value does not exist in the array
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 40);
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 30);
 *     push_back_uint8_array(arr, 20);
 *     // arr contains [40, 10, 10, 30, 20].
 *
 *     // sort == true: arr is sorted in place to [10, 10, 20, 30, 40].
 *     size_expect_t r = uint8_array_binary_search(arr, 10, true);
 *     // r.has_value == true, r.u.value == 0 (first of two 10s).
 *
 *     // arr is now sorted; sort == false is safe for subsequent calls.
 *     r = uint8_array_binary_search(arr, 30, false);
 *     // r.has_value == true, r.u.value == 3.
 *
 *     r = uint8_array_binary_search(arr, 99, false);
 *     // r.has_value == false, r.u.error == NOT_FOUND.
 *
 *     return_uint8_array(arr);
 * @endcode
 */
size_expect_t uint8_array_binary_search(uint8_array_t* array,
                                         uint8_t        value,
                                         bool           sort);

// --------------------------------------------------------------------------------

/**
 * @brief Find the bracketing indices around value using binary search.
 *
 * Locates the largest element <= value (lower bound) and the smallest
 * element >= value (upper bound) within the array. If value exists in
 * the array u.value.lower == u.value.upper and both point to the first
 * occurrence. If value is outside the range [min, max] of the array's
 * values the call returns has_value false with NOT_FOUND.
 *
 * Uses a built-in unsigned byte comparator. When sort is true the array
 * is sorted in place in ascending order before searching; the sorted order
 * persists after the call. When sort is false the array must already be
 * sorted in ascending order.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param value  The uint8_t value to bracket.
 * @param sort   If true, sort the array in place before searching.
 *               If false, the array must already be sorted ascending.
 *
 * @return bracket_expect_t with has_value true on success:
 *           u.value.lower — index of the largest element  <= value.
 *           u.value.upper — index of the smallest element >= value.
 *           lower == upper indicates value was found exactly; both
 *           point to the first occurrence.
 *         On failure has_value is false and u.error is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *         - NOT_FOUND     if value is outside [min, max] of the array
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 30);
 *     push_back_uint8_array(arr, 50);
 *     push_back_uint8_array(arr, 70);
 *     // arr contains [10, 30, 50, 70] (already sorted).
 *
 *     // Value exists exactly — lower == upper, both at first occurrence.
 *     bracket_expect_t r = uint8_array_binary_bracket(arr, 30, false);
 *     // r.has_value == true, r.u.value.lower == 1, r.u.value.upper == 1.
 *
 *     // Value falls between elements.
 *     r = uint8_array_binary_bracket(arr, 40, false);
 *     // r.has_value == true, r.u.value.lower == 1 (30), upper == 2 (50).
 *
 *     // Value outside range returns NOT_FOUND.
 *     r = uint8_array_binary_bracket(arr, 5, false);
 *     // r.has_value == false, r.u.error == NOT_FOUND.
 *
 *     return_uint8_array(arr);
 * @endcode
 */
bracket_expect_t uint8_array_binary_bracket(uint8_array_t* array,
                                             uint8_t        value,
                                             bool           sort);
// ================================================================================
// Introspection
// ================================================================================

/**
 * @brief Search for the first occurrence of value in the range [start, end).
 *
 * Scans elements at indices [start, end) using the best SIMD instruction set
 * available at compile time. The search performs an exact bitwise comparison —
 * the first element whose stored byte equals value is returned. If the value
 * appears more than once only the lowest index within the range is returned.
 * The array is not modified. Floating-point types stored in a uint8_array are
 * not applicable here since the element type is fixed to uint8_t; no epsilon
 * tolerance is relevant.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param value  The uint8_t value to search for.
 * @param start  Zero-based index of the first element to examine (inclusive).
 * @param end    Zero-based index one past the last element to examine
 *               (exclusive). Must be > start and <= array->base.len.
 *
 * @return size_expect_t with has_value true and u.value holding the zero-based
 *         index of the first match on success. On failure has_value is false
 *         and u.error is one of:
 *         - NULL_POINTER  if array is NULL
 *         - INVALID_ARG   if start >= end
 *         - OUT_OF_BOUNDS if end > array->base.len
 *         - NOT_FOUND     if no element in [start, end) equals value
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 30);
 *     push_back_uint8_array(arr, 20);
 *     push_back_uint8_array(arr, 30);
 *     push_back_uint8_array(arr, 40);
 *     // arr contains [10, 30, 20, 30, 40].
 *
 *     size_expect_t r = uint8_array_contains(arr, 30, 0, 5);
 *     // r.has_value == true, r.u.value == 1  (first occurrence).
 *
 *     r = uint8_array_contains(arr, 30, 2, 5);
 *     // r.has_value == true, r.u.value == 3  (search starts after index 1).
 *
 *     r = uint8_array_contains(arr, 99, 0, 5);
 *     // r.has_value == false, r.u.error == NOT_FOUND.
 *
 *     return_uint8_array(arr);
 * @endcode
 */
size_expect_t uint8_array_contains(const uint8_array_t* array,
                                    uint8_t              value,
                                    size_t               start,
                                    size_t               end);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the number of elements currently stored in the array.
 *
 * Reads array->base.len. This value reflects only elements that have been
 * pushed and not yet popped or cleared — it does not include any spare
 * capacity reserved in the buffer. The call is O(1) and does not modify
 * the array.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return The current element count, or 0 if array is NULL.
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     // uint8_array_size(arr) == 0 after init.
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 20);
 *     // uint8_array_size(arr) == 2.
 *
 *     pop_back_uint8_array(arr, NULL);
 *     // uint8_array_size(arr) == 1.
 *
 *     return_uint8_array(arr);
 * @endcode
 */
size_t uint8_array_size(const uint8_array_t* array);
// --------------------------------------------------------------------------------

/**
 * @brief Return the number of elements the buffer can currently hold.
 *
 * Reads array->base.alloc. This is the total capacity of the allocated data
 * buffer in element units, not the number of elements currently stored. The
 * spare capacity available for new pushes without reallocation is
 * uint8_array_alloc(arr) - uint8_array_size(arr). When growth is true alloc
 * may increase after a push that triggers reallocation; it never decreases
 * automatically. The call is O(1) and does not modify the array.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return The allocated capacity in elements, or 0 if array is NULL.
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(4, true, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     // uint8_array_alloc(arr) == 4 after init.
 *     push_back_uint8_array(arr, 1);
 *     push_back_uint8_array(arr, 2);
 *     push_back_uint8_array(arr, 3);
 *     push_back_uint8_array(arr, 4);
 *     // Buffer is full: size == 4, alloc == 4.
 *
 *     push_back_uint8_array(arr, 5);
 *     // Growth triggered: size == 5, alloc > 4.
 *
 *     return_uint8_array(arr);
 * @endcode
 */
size_t uint8_array_alloc(const uint8_array_t* array);
// --------------------------------------------------------------------------------

/**
 * @brief Return the size in bytes of one element in the array.
 *
 * Reads array->base.data_size, which is fixed to 1 for all uint8_array_t
 * instances — a uint8_t is always one byte. This function exists to keep the
 * introspection API uniform with other typed array wrappers where element
 * sizes differ. The call is O(1) and does not modify the array.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return 1 when array is non-NULL, or 0 if array is NULL.
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     size_t elem_bytes = uint8_array_data_size(arr);  // always 1
 *
 *     // Useful when writing code that is generic across array types:
 *     size_t total_bytes = uint8_array_alloc(arr) * uint8_array_data_size(arr);
 *
 *     return_uint8_array(arr);
 * @endcode
 */
size_t uint8_array_data_size(const uint8_array_t* array);
// --------------------------------------------------------------------------------

/**
 * @brief Return true if the array currently contains no elements.
 *
 * Compares array->base.len to zero. A freshly initialised array is always
 * empty. The result may change after any push or pop call. This function is
 * the idiomatic guard before attempting a pop — calling any pop variant on
 * an empty array returns EMPTY, so callers should check is_uint8_array_empty
 * first in loops that drain the array. The call is O(1) and does not modify
 * the array.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return true if array is NULL or array->base.len == 0, false otherwise.
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(4, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     // is_uint8_array_empty(arr) == true immediately after init.
 *
 *     push_back_uint8_array(arr, 42);
 *     // is_uint8_array_empty(arr) == false.
 *
 *     // Idiomatic drain loop:
 *     uint8_t val = 0;
 *     while (!is_uint8_array_empty(arr)) {
 *         pop_back_uint8_array(arr, &val);
 *         // process val ...
 *     }
 *     // is_uint8_array_empty(arr) == true again.
 *
 *     return_uint8_array(arr);
 * @endcode
 */
bool is_uint8_array_empty(const uint8_array_t* array);
// --------------------------------------------------------------------------------

/**
 * @brief Return true if the array has no remaining spare capacity.
 *
 * Compares array->base.len to array->base.alloc. When this returns true any
 * subsequent push to an array with growth == false will return
 * CAPACITY_OVERFLOW. When growth == true a push will instead trigger a
 * reallocation and is_uint8_array_full will return false afterward. The call
 * is O(1) and does not modify the array.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return true if array is NULL or array->base.len == array->base.alloc,
 *         false otherwise.
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(3, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 20);
 *     // is_uint8_array_full(arr) == false, one slot remains.
 *
 *     push_back_uint8_array(arr, 30);
 *     // is_uint8_array_full(arr) == true, size == alloc == 3.
 *
 *     // With growth == false, the next push fails.
 *     error_code_t err = push_back_uint8_array(arr, 40);
 *     // err == CAPACITY_OVERFLOW, arr is unchanged.
 *
 *     return_uint8_array(arr);
 * @endcode
 */
bool is_uint8_array_full(const uint8_array_t* array);
// --------------------------------------------------------------------------------

/**
 * @brief Return true if ptr points to a valid element within the live region
 *        of the array's data buffer.
 *
 * A pointer is considered valid if all of the following hold:
 *   1. array and ptr are both non-NULL.
 *   2. ptr >= array->base.data  (at or after the start of the buffer).
 *   3. ptr <  array->base.data + array->base.len  (before the end of the
 *      live region — spare capacity beyond len is not considered valid).
 *   4. (ptr - array->base.data) is an exact multiple of 1 (always satisfied
 *      for uint8_t, included for consistency with the generic is_array_ptr).
 *
 * This is useful for validating a pointer obtained by direct buffer access
 * before using it in a subsequent operation. It does not validate pointers
 * into spare capacity. The call is O(1) and does not modify the array.
 *
 * @param array  Pointer to the array to check against. May be NULL.
 * @param ptr    Pointer to test. May be NULL.
 *
 * @return true only if both arguments are non-NULL and ptr falls within the
 *         live element region [data, data + len). Returns false in all other
 *         cases including when either argument is NULL.
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint8_array_expect_t result = init_uint8_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint8_array_t* arr = result.u.value;
 *
 *     push_back_uint8_array(arr, 10);
 *     push_back_uint8_array(arr, 20);
 *     push_back_uint8_array(arr, 30);
 *     // arr contains [10, 20, 30], len == 3, alloc == 8.
 *
 *     // Obtain a pointer to the second element via direct buffer access.
 *     const uint8_t* p = arr->base.data + 1;
 *
 *     is_uint8_array_ptr(arr, p);               // true  — inside live region
 *     is_uint8_array_ptr(arr, arr->base.data);  // true  — first element
 *     is_uint8_array_ptr(arr, arr->base.data + 2); // true  — last element
 *     is_uint8_array_ptr(arr, arr->base.data + 3); // false — spare capacity
 *     is_uint8_array_ptr(arr, NULL);            // false — NULL ptr
 *     is_uint8_array_ptr(NULL, p);              // false — NULL array
 *
 *     return_uint8_array(arr);
 * @endcode
 */
bool is_uint8_array_ptr(const uint8_array_t* array, const uint8_t* ptr);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_uint8_H */
// ================================================================================
// ================================================================================
// eof
