// ================================================================================
// ================================================================================
// - File:    c_uint32.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 27, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_uint32_H
#define c_uint32_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "c_array.h"
#include "c_dict.h"
#include "c_matrix.h"
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================
// ================================================================================

/**
 * @brief A type-safe dynamic array of uint32_t elements.
 *
 * Wraps array_t with the dtype fixed to UINT32_TYPE at initialization.
 * All operations enforce UINT32_TYPE at the call site so the caller never
 * needs to pass a dtype_id_t argument. The underlying array_t is embedded
 * by value so the struct can be stack- or heap-allocated by the caller.
 */
typedef struct {
    array_t base; /**< Underlying generic array. Do not modify directly. */
} uint32_array_t;

// ================================================================================
// Expected type
// ================================================================================

/**
 * @brief Expected return type for uint32_array_t initialization and copy/slice.
 */
typedef struct {
    bool has_value;
    union {
        uint32_array_t* value;
        error_code_t    error;
    } u;
} uint32_array_expect_t;

// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialize a new heap-allocated uint32_array_t.
 *
 * Allocates both the array_t struct and its internal data buffer through
 * alloc_v. The element type is fixed to UINT32_TYPE and the element size is
 * cached as 2 bytes. The array starts empty (len == 0) with the full capacity
 * reserved. When growth is true the data buffer will be reallocated
 * automatically whenever a push operation would exceed the current capacity;
 * when false any push beyond capacity returns CAPACITY_OVERFLOW without
 * modifying the array. The caller is responsible for calling
 * return_uint32_array when the array is no longer needed.
 *
 * @param capacity  Initial number of elements the buffer can hold.
 *                  Must be > 0.
 * @param growth    Pass true to allow the buffer to grow automatically on
 *                  overflow, false to use a fixed-capacity buffer.
 * @param alloc_v   Allocator vtable used for all internal memory operations.
 *                  alloc_v.allocate must not be NULL.
 *
 * @return uint32_array_expect_t with has_value true and u.value pointing to
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
 *     uint32_array_expect_t result = init_uint32_array(32, false, alloc);
 *     if (!result.has_value) {
 *         fprintf(stderr, "init failed: %d\n", result.u.error);
 *         return;
 *     }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     // arr now contains [1000, 2000, 3000], len == 3, alloc == 32.
 *
 *     return_uint32_array(arr);
 * @endcode
 */
uint32_array_expect_t init_uint32_array(size_t             capacity,
                                         bool               growth,
                                         allocator_vtable_t alloc_v);

// --------------------------------------------------------------------------------

/**
 * @brief Release all memory owned by a uint32_array_t back to its allocator.
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
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 10000);
 *
 *     // Release all memory. arr must not be used after this point.
 *     return_uint32_array(arr);
 *     arr = NULL;  // Optional but recommended to prevent accidental reuse.
 *
 *     // Passing NULL is always safe.
 *     return_uint32_array(NULL);
 * @endcode
 */
void return_uint32_array(uint32_array_t* array);

// ================================================================================
// Push operations
// ================================================================================

/**
 * @brief Append one uint32_t to the end of the array.
 *
 * Copies value into the next available slot at index array->base.len and
 * increments len by one. If the array is full and growth is true the data
 * buffer is reallocated before the value is written; if growth is false the
 * call fails immediately with CAPACITY_OVERFLOW and the array is left
 * unmodified. This is an O(1) amortised operation when growth is true and
 * strictly O(1) when the buffer has spare capacity.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param value  The uint32_t value to append.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint32_array_expect_t result = init_uint32_array(4, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     // arr contains [1000, 2000, 3000], len == 3.
 *
 *     uint32_t out = 0;
 *     get_uint32_array_index(arr, 2, &out);  // out == 3000
 *
 *     return_uint32_array(arr);
 * @endcode
 */
error_code_t push_back_uint32_array(uint32_array_t* array, uint32_t value);

// --------------------------------------------------------------------------------

/**
 * @brief Prepend one uint32_t to the front of the array.
 *
 * Shifts all existing elements one position toward the back using memmove,
 * writes value into index 0, and increments len by one. If the array is full
 * and growth is true the data buffer is reallocated before the shift; if
 * growth is false the call fails immediately with CAPACITY_OVERFLOW and the
 * array is left unmodified. Because every existing element must be moved this
 * is an O(n) operation — prefer push_back_uint32_array for performance-sensitive
 * append-heavy workloads.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param value  The uint32_t value to prepend.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - CAPACITY_OVERFLOW if the array is full and growth is false
 *         - OUT_OF_MEMORY     if growth is true but reallocation fails
 *         - LENGTH_OVERFLOW   if the new capacity would overflow size_t
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     // arr contains [2000, 3000].
 *
 *     push_front_uint32_array(arr, 1000);
 *     // arr now contains [1000, 2000, 3000].
 *
 *     // Prepending in sequence reverses insertion order:
 *     // push_front(1), push_front(2), push_front(3) -> [3, 2, 1].
 *
 *     return_uint32_array(arr);
 * @endcode
 */
error_code_t push_front_uint32_array(uint32_array_t* array, uint32_t value);

// --------------------------------------------------------------------------------

/**
 * @brief Insert one uint32_t at an arbitrary position in the array.
 *
 * Shifts all elements from index onward one position toward the back using
 * memmove, writes value at the given index, and increments len by one. If
 * the array is full and growth is true the data buffer is reallocated before
 * the shift; if growth is false the call fails with CAPACITY_OVERFLOW and
 * the array is left unmodified. Inserting at index 0 is equivalent to
 * push_front_uint32_array; inserting at index == len is equivalent to
 * push_back_uint32_array. Both boundary cases are accepted without error.
 * Inserting at any other valid position is an O(n) operation due to the
 * element shift.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param index  Position at which to insert. Must be <= array->base.len.
 *               Elements at [index, len) are shifted right by one.
 * @param value  The uint32_t value to insert.
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
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 3000);
 *     // arr contains [1000, 3000].
 *
 *     push_at_uint32_array(arr, 1, 2000);
 *     // arr now contains [1000, 2000, 3000].
 *
 *     // Boundary cases:
 *     push_at_uint32_array(arr, 0,  500);  // prepend -> [500, 1000, 2000, 3000]
 *     push_at_uint32_array(arr, 4, 4000);  // append  -> [500, 1000, 2000, 3000, 4000]
 *
 *     return_uint32_array(arr);
 * @endcode
 */
error_code_t push_at_uint32_array(uint32_array_t* array,
                                   size_t          index,
                                   uint32_t        value);

// ================================================================================
// Get operation
// ================================================================================

/**
 * @brief Retrieve the uint32_t at a given index, copying it into out.
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
 * @param out    Pointer to a uint32_t that receives the copied value.
 *               Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array or out is NULL
 *         - OUT_OF_BOUNDS if index >= array->base.len
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     // arr contains [1000, 2000, 3000], len == 3.
 *
 *     uint32_t val = 0;
 *     get_uint32_array_index(arr, 0, &val);  // val == 1000
 *     get_uint32_array_index(arr, 1, &val);  // val == 2000
 *     get_uint32_array_index(arr, 2, &val);  // val == 3000
 *
 *     // Reading past the end is an error regardless of buffer capacity.
 *     error_code_t err = get_uint32_array_index(arr, 3, &val);
 *     // err == OUT_OF_BOUNDS, val is unchanged.
 *
 *     return_uint32_array(arr);
 * @endcode
 */
error_code_t get_uint32_array_index(const uint32_array_t* array,
                                     size_t                index,
                                     uint32_t*             out);

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
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     // arr contains [1000, 2000, 3000], len == 3.
 *
 *     uint32_t val = 0;
 *     pop_back_uint32_array(arr, &val);  // val == 3000, len == 2
 *     pop_back_uint32_array(arr, &val);  // val == 2000, len == 1
 *     pop_back_uint32_array(arr, NULL);  // discard,    len == 0
 *
 *     // Popping from an empty array is an error.
 *     error_code_t err = pop_back_uint32_array(arr, &val);
 *     // err == EMPTY, arr and val are unchanged.
 *
 *     return_uint32_array(arr);
 * @endcode
 */
error_code_t pop_back_uint32_array(uint32_array_t* array, uint32_t* out);

// --------------------------------------------------------------------------------

/**
 * @brief Remove the first element from the array and optionally retrieve it.
 *
 * Copies the value at index 0 into out (if non-NULL), then shifts all
 * remaining elements one position toward the front using memmove, and
 * decrements len by one. The data buffer is not reallocated. Because every
 * remaining element must be moved this is an O(n) operation — prefer
 * pop_back_uint32_array for performance-sensitive drain loops. The array must
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
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     // arr contains [1000, 2000, 3000], len == 3.
 *
 *     uint32_t val = 0;
 *     pop_front_uint32_array(arr, &val);  // val == 1000, arr is [2000, 3000]
 *     pop_front_uint32_array(arr, &val);  // val == 2000, arr is [3000]
 *     pop_front_uint32_array(arr, NULL);  // discard,    arr is []
 *
 *     return_uint32_array(arr);
 * @endcode
 */
error_code_t pop_front_uint32_array(uint32_array_t* array, uint32_t* out);

// --------------------------------------------------------------------------------

/**
 * @brief Remove the element at an arbitrary index and optionally retrieve it.
 *
 * Copies the value at index into out (if non-NULL), then shifts all elements
 * from index + 1 onward one position toward the front using memmove, and
 * decrements len by one. The data buffer is not reallocated. Removing from
 * index 0 is equivalent to pop_front_uint32_array; removing from index
 * len - 1 is equivalent to pop_back_uint32_array. Both boundary cases are
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
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     push_back_uint32_array(arr, 4000);
 *     // arr contains [1000, 2000, 3000, 4000], len == 4.
 *
 *     uint32_t val = 0;
 *     pop_any_uint32_array(arr, &val, 1);  // val == 2000, arr is [1000, 3000, 4000]
 *     pop_any_uint32_array(arr, &val, 0);  // val == 1000, arr is [3000, 4000]
 *     pop_any_uint32_array(arr, &val, 1);  // val == 4000, arr is [3000]
 *
 *     return_uint32_array(arr);
 * @endcode
 */
error_code_t pop_any_uint32_array(uint32_array_t* array,
                                   uint32_t*       out,
                                   size_t          index);

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
 * directly. Use clear_uint32_array when you need a guaranteed clean slate.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 *
 * @return NO_ERROR on success, or NULL_POINTER if array is NULL.
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     // arr contains [1000, 2000, 3000], len == 3, alloc == 8.
 *
 *     clear_uint32_array(arr);
 *     // arr is now empty: len == 0, alloc == 8 (buffer retained).
 *
 *     // The array is immediately ready for reuse.
 *     push_back_uint32_array(arr, 9999);
 *     // arr contains [9999], len == 1.
 *
 *     return_uint32_array(arr);
 * @endcode
 */
error_code_t clear_uint32_array(uint32_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Overwrite the element at a given index in place.
 *
 * Copies value into the slot at index, replacing the current occupant.
 * len, alloc, and all other elements are unchanged. This is a bounds-checked
 * in-place write — it cannot extend the array and will not trigger
 * reallocation. Use push_at_uint32_array to insert a new element and shift
 * existing ones.
 *
 * @param array  Pointer to the target array. Must not be NULL.
 * @param index  Zero-based position to overwrite. Must be < array->base.len.
 * @param value  The replacement uint32_t value.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - OUT_OF_BOUNDS if index >= array->base.len
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint32_array_expect_t result = init_uint32_array(4, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     // arr contains [1000, 2000, 3000].
 *
 *     set_uint32_array_index(arr, 1, 9999);
 *     // arr now contains [1000, 9999, 3000]. len is still 3.
 *
 *     // Writing past the live region is an error even if capacity exists.
 *     error_code_t err = set_uint32_array_index(arr, 3, 4000);
 *     // err == OUT_OF_BOUNDS, arr is unchanged.
 *
 *     return_uint32_array(arr);
 * @endcode
 */
error_code_t set_uint32_array_index(uint32_array_t* array,
                                     size_t          index,
                                     uint32_t        value);

// --------------------------------------------------------------------------------

/**
 * @brief Create an independent deep copy of src using the given allocator.
 *
 * Allocates a new uint32_array_t struct and a new data buffer through alloc_v,
 * then copies all len elements from src into the new array. The copy has the
 * same len, alloc, growth setting, and dtype as src but owns its own buffer —
 * subsequent modifications to src do not affect the copy and vice versa. The
 * copy may be given a different allocator than src. The caller is responsible
 * for calling return_uint32_array on the returned array when it is no longer
 * needed.
 *
 * @param src      Pointer to the source array. Must not be NULL.
 * @param alloc_v  Allocator vtable for the new array's memory.
 *                 alloc_v.allocate must not be NULL.
 *
 * @return uint32_array_expect_t with has_value true and u.value pointing to
 *         the new array on success. On failure has_value is false and
 *         u.error is one of:
 *         - NULL_POINTER  if src is NULL or alloc_v.allocate is NULL
 *         - BAD_ALLOC     if the struct allocation fails
 *         - OUT_OF_MEMORY if the data buffer allocation fails
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* src = result.u.value;
 *
 *     push_back_uint32_array(src, 1000);
 *     push_back_uint32_array(src, 2000);
 *     push_back_uint32_array(src, 3000);
 *
 *     uint32_array_expect_t copy_result = copy_uint32_array(src, alloc);
 *     if (!copy_result.has_value) { return_uint32_array(src); return; }
 *     uint32_array_t* dst = copy_result.u.value;
 *     // src and dst both contain [1000, 2000, 3000] but own separate buffers.
 *
 *     set_uint32_array_index(src, 0, 9999);
 *     // src is [9999, 2000, 3000], dst is still [1000, 2000, 3000].
 *
 *     return_uint32_array(src);
 *     return_uint32_array(dst);
 * @endcode
 */
uint32_array_expect_t copy_uint32_array(const uint32_array_t* src,
                                         allocator_vtable_t    alloc_v);

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
 *     uint32_array_expect_t r1 = init_uint32_array(8, false, alloc);
 *     uint32_array_expect_t r2 = init_uint32_array(4, false, alloc);
 *     if (!r1.has_value || !r2.has_value) { return; }
 *     uint32_array_t* dst = r1.u.value;
 *     uint32_array_t* src = r2.u.value;
 *
 *     push_back_uint32_array(dst, 1000);
 *     push_back_uint32_array(dst, 2000);
 *     push_back_uint32_array(src, 3000);
 *     push_back_uint32_array(src, 4000);
 *
 *     concat_uint32_array(dst, src);
 *     // dst contains [1000, 2000, 3000, 4000], src is unchanged as [3000, 4000].
 *
 *     return_uint32_array(dst);
 *     return_uint32_array(src);
 * @endcode
 */
error_code_t concat_uint32_array(uint32_array_t*       dst,
                                  const uint32_array_t* src);

// --------------------------------------------------------------------------------

/**
 * @brief Allocate a new array containing the elements of src in [start, end).
 *
 * Allocates a new uint32_array_t and data buffer through alloc_v, then copies
 * the elements at indices [start, end) from src into it. The resulting array
 * has len == end - start and alloc == end - start. src is treated as
 * read-only and is not modified. The returned array owns its own buffer and
 * is completely independent of src — subsequent changes to src do not affect
 * the slice and vice versa. The range is half-open: start is inclusive, end
 * is exclusive. The caller is responsible for calling return_uint32_array on
 * the returned array when it is no longer needed.
 *
 * @param src      Pointer to the source array. Must not be NULL.
 * @param start    Zero-based index of the first element to include (inclusive).
 * @param end      Zero-based index one past the last element to include
 *                 (exclusive). Must be > start and <= src->base.len.
 * @param alloc_v  Allocator vtable for the new array's memory.
 *                 alloc_v.allocate must not be NULL.
 *
 * @return uint32_array_expect_t with has_value true and u.value pointing to
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
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* src = result.u.value;
 *
 *     push_back_uint32_array(src, 1000);
 *     push_back_uint32_array(src, 2000);
 *     push_back_uint32_array(src, 3000);
 *     push_back_uint32_array(src, 4000);
 *     push_back_uint32_array(src, 5000);
 *     // src contains [1000, 2000, 3000, 4000, 5000].
 *
 *     uint32_array_expect_t sr = slice_uint32_array(src, 1, 4, alloc);
 *     if (!sr.has_value) { return_uint32_array(src); return; }
 *     uint32_array_t* slc = sr.u.value;
 *     // slc contains [2000, 3000, 4000], len == 3.
 *
 *     // Modifying src does not affect slc.
 *     set_uint32_array_index(src, 1, 9999);
 *     // src is [1000, 9999, 3000, 4000, 5000], slc is still [2000, 3000, 4000].
 *
 *     return_uint32_array(src);
 *     return_uint32_array(slc);
 * @endcode
 */
uint32_array_expect_t slice_uint32_array(const uint32_array_t* src,
                                          size_t                start,
                                          size_t                end,
                                          allocator_vtable_t    alloc_v);

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
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     push_back_uint32_array(arr, 4000);
 *     // arr contains [1000, 2000, 3000, 4000].
 *
 *     reverse_uint32_array(arr);
 *     // arr now contains [4000, 3000, 2000, 1000].
 *
 *     // Reversing twice restores the original order.
 *     reverse_uint32_array(arr);
 *     // arr is [1000, 2000, 3000, 4000] again.
 *
 *     return_uint32_array(arr);
 * @endcode
 */
error_code_t reverse_uint32_array(uint32_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Sort the elements of the array in place.
 *
 * Sorts using median-of-three quicksort with an insertion sort fallback for
 * partitions of fewer than 10 elements and tail-call optimisation to keep
 * stack depth O(log n). The comparator is a built-in unsigned 32-bit
 * comparison so no comparator function needs to be supplied. The sort is not
 * stable — equal elements may change relative order. FORWARD produces
 * ascending order (smallest value first); REVERSE produces descending order
 * (largest value first).
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
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 5000);
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 4000);
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     // arr contains [5000, 1000, 4000, 2000, 3000].
 *
 *     sort_uint32_array(arr, FORWARD);
 *     // arr is now [1000, 2000, 3000, 4000, 5000].
 *
 *     sort_uint32_array(arr, REVERSE);
 *     // arr is now [5000, 4000, 3000, 2000, 1000].
 *
 *     return_uint32_array(arr);
 * @endcode
 */
error_code_t sort_uint32_array(uint32_array_t* array, direction_t dir);

// ================================================================================
// Search
// ================================================================================

/**
 * @brief Search for the first occurrence of value in the range [start, end).
 *
 * Scans elements at indices [start, end) using the best SIMD instruction set
 * available at compile time. The search performs an exact bitwise comparison —
 * the first element whose stored value equals value is returned. If the value
 * appears more than once only the lowest index within the range is returned.
 * The array is not modified.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param value  The uint32_t value to search for.
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
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 3000);
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     push_back_uint32_array(arr, 4000);
 *     // arr contains [1000, 3000, 2000, 3000, 4000].
 *
 *     size_expect_t r = uint32_array_contains(arr, 3000, 0, 5);
 *     // r.has_value == true, r.u.value == 1  (first occurrence).
 *
 *     r = uint32_array_contains(arr, 3000, 2, 5);
 *     // r.has_value == true, r.u.value == 3  (search starts after index 1).
 *
 *     r = uint32_array_contains(arr, 9999, 0, 5);
 *     // r.has_value == false, r.u.error == NOT_FOUND.
 *
 *     return_uint32_array(arr);
 * @endcode
 */
size_expect_t uint32_array_contains(const uint32_array_t* array,
                                     uint32_t              value,
                                     size_t                start,
                                     size_t                end);

// --------------------------------------------------------------------------------

/**
 * @brief Search for the first occurrence of value using binary search.
 *
 * Uses a built-in unsigned 32-bit comparator — the caller does not need to
 * supply one. When sort is true the array is sorted in place in ascending
 * order before searching; the sorted order persists after the call. When
 * sort is false the array must already be sorted in ascending order.
 * After any matching element is found the implementation scans leftward to
 * return the index of the first occurrence, consistent with
 * uint32_array_contains.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param value  The uint32_t value to search for.
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
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 4000);
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 3000);
 *     push_back_uint32_array(arr, 2000);
 *     // arr contains [4000, 1000, 1000, 3000, 2000].
 *
 *     // sort == true: arr is sorted in place to [1000, 1000, 2000, 3000, 4000].
 *     size_expect_t r = uint32_array_binary_search(arr, 1000, true);
 *     // r.has_value == true, r.u.value == 0 (first of two 100s).
 *
 *     // arr is now sorted; sort == false is safe for subsequent calls.
 *     r = uint32_array_binary_search(arr, 3000, false);
 *     // r.has_value == true, r.u.value == 3.
 *
 *     r = uint32_array_binary_search(arr, 9999, false);
 *     // r.has_value == false, r.u.error == NOT_FOUND.
 *
 *     return_uint32_array(arr);
 * @endcode
 */
size_expect_t uint32_array_binary_search(uint32_array_t* array,
                                          uint32_t        value,
                                          bool            sort);

// --------------------------------------------------------------------------------

/**
 * @brief Find the bracketing indices around value using binary search.
 *
 * Locates the first element >= value (returned as u.value.lower) and the
 * last element <= value (returned as u.value.upper) within the array. If
 * value exists in the array u.value.lower == u.value.upper and both point
 * to the first occurrence. If value is outside the range [min, max] of the
 * array's values the call returns has_value false with NOT_FOUND.
 *
 * Uses a built-in unsigned 32-bit comparator. When sort is true the array
 * is sorted in place in ascending order before searching; the sorted order
 * persists after the call. When sort is false the array must already be
 * sorted in ascending order.
 *
 * @param array  Pointer to the array to search. Must not be NULL.
 * @param value  The uint32_t value to bracket.
 * @param sort   If true, sort the array in place before searching.
 *               If false, the array must already be sorted ascending.
 *
 * @return bracket_expect_t with has_value true on success:
 *           u.value.lower — index of the first element >= value
 *                           (first occurrence when value exists exactly).
 *           u.value.upper — index of the last element  <= value
 *                           (same as lower when value exists exactly).
 *           lower == upper indicates value was found exactly; both
 *           point to the first occurrence.
 *         On failure has_value is false and u.error is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *         - NOT_FOUND     if value is outside [min, max] of the array
 *
 * @code{.c}
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 3000);
 *     push_back_uint32_array(arr, 5000);
 *     push_back_uint32_array(arr, 7000);
 *     // arr contains [1000, 3000, 5000, 7000] (already sorted).
 *
 *     // Value exists exactly — lower == upper, both at first occurrence.
 *     bracket_expect_t r = uint32_array_binary_bracket(arr, 3000, false);
 *     // r.has_value == true, r.u.value.lower == 1, r.u.value.upper == 1.
 *
 *     // Value falls between elements.
 *     r = uint32_array_binary_bracket(arr, 4000, false);
 *     // r.has_value == true
 *     // r.u.value.lower == 2  (first element >= 4000, which is 5000 at index 2)
 *     // r.u.value.upper == 1  (last  element <= 4000, which is 3000 at index 1)
 *
 *     // Value outside range returns NOT_FOUND.
 *     r = uint32_array_binary_bracket(arr, 500, false);
 *     // r.has_value == false, r.u.error == NOT_FOUND.
 *
 *     return_uint32_array(arr);
 * @endcode
 */
bracket_expect_t uint32_array_binary_bracket(uint32_array_t* array,
                                              uint32_t        value,
                                              bool            sort);

// ================================================================================
// Introspection
// ================================================================================

/**
 * @brief Return the number of elements currently stored in the array.
 *
 * Reads array->base.len. The call is O(1) and does not modify the array.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return The current element count, or 0 if array is NULL.
 *
 * @code{.c}
 *     // uint32_array_size(arr) == 0 after init.
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 2000);
 *     // uint32_array_size(arr) == 2.
 *
 *     pop_back_uint32_array(arr, NULL);
 *     // uint32_array_size(arr) == 1.
 * @endcode
 */
size_t uint32_array_size(const uint32_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return the number of elements the buffer can currently hold.
 *
 * Reads array->base.alloc. The spare capacity available without reallocation
 * is uint32_array_alloc(arr) - uint32_array_size(arr). The call is O(1) and
 * does not modify the array.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return The allocated capacity in elements, or 0 if array is NULL.
 *
 * @code{.c}
 *     // uint32_array_alloc(arr) == 4 after init with capacity 4.
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     push_back_uint32_array(arr, 4000);
 *     // Buffer is full: size == 4, alloc == 4.
 *
 *     push_back_uint32_array(arr, 5000);  // growth == true
 *     // Growth triggered: size == 5, alloc > 4.
 * @endcode
 */
size_t uint32_array_alloc(const uint32_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return the size in bytes of one element in the array.
 *
 * Reads array->base.data_size, which is fixed to 4 for all uint32_array_t
 * instances — a uint32_t is always four bytes. The call is O(1) and does not
 * modify the array.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return 2 when array is non-NULL, or 0 if array is NULL.
 *
 * @code{.c}
 *     size_t elem_bytes = uint32_array_data_size(arr);  // always 4
 *
 *     // Total buffer size in bytes:
 *     size_t total_bytes = uint32_array_alloc(arr) * uint32_array_data_size(arr);
 * @endcode
 */
size_t uint32_array_data_size(const uint32_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if the array currently contains no elements.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return true if array is NULL or array->base.len == 0, false otherwise.
 *
 * @code{.c}
 *     // is_uint32_array_empty(arr) == true immediately after init.
 *     push_back_uint32_array(arr, 1000);
 *     // is_uint32_array_empty(arr) == false.
 *
 *     uint32_t val = 0;
 *     while (!is_uint32_array_empty(arr))
 *         pop_back_uint32_array(arr, &val);
 *     // is_uint32_array_empty(arr) == true again.
 * @endcode
 */
bool is_uint32_array_empty(const uint32_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if the array has no remaining spare capacity.
 *
 * @param array  Pointer to the array. May be NULL.
 *
 * @return true if array is NULL or array->base.len == array->base.alloc,
 *         false otherwise.
 *
 * @code{.c}
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 2000);
 *     // is_uint32_array_full(arr) == false (capacity 3, one slot remains).
 *
 *     push_back_uint32_array(arr, 3000);
 *     // is_uint32_array_full(arr) == true, size == alloc == 3.
 * @endcode
 */
bool is_uint32_array_full(const uint32_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Return true if ptr points to a valid element within the live region
 *        of the array's data buffer.
 *
 * A pointer is considered valid if array and ptr are both non-NULL,
 * ptr >= array->base.data, and ptr < array->base.data + array->base.len * 2.
 * Spare capacity beyond len is not considered valid. The call is O(1) and
 * does not modify the array.
 *
 * @param array  Pointer to the array to check against. May be NULL.
 * @param ptr    Pointer to test. May be NULL.
 *
 * @return true only if both arguments are non-NULL and ptr falls within the
 *         live element region. Returns false in all other cases.
 *
 * @code{.c}
 *     push_back_uint32_array(arr, 1000);
 *     push_back_uint32_array(arr, 2000);
 *     push_back_uint32_array(arr, 3000);
 *     // arr contains [1000, 2000, 3000], len == 3, alloc == 8.
 *
 *     const uint32_t* p = (const uint32_t*)arr->base.data + 1;
 *
 *     is_uint32_array_ptr(arr, p);                               // true
 *     is_uint32_array_ptr(arr, (const uint32_t*)arr->base.data); // true  — first
 *     is_uint32_array_ptr(arr, (const uint32_t*)arr->base.data + 2); // true — last
 *     is_uint32_array_ptr(arr, (const uint32_t*)arr->base.data + 3); // false — spare
 *     is_uint32_array_ptr(arr, NULL);                            // false
 *     is_uint32_array_ptr(NULL, p);                              // false
 * @endcode
 */
bool is_uint32_array_ptr(const uint32_array_t* array, const uint32_t* ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Find the minimum value in the array.
 *
 * Scans all elements using an unsigned 32-bit comparison and returns the
 * smallest value found. When two or more elements share the minimum value the
 * value of the first occurrence is returned, consistent with the behaviour of
 * uint32_array_contains. The array is not modified. The scan dispatches to
 * the best SIMD horizontal-reduction path available at compile time and falls
 * back to a scalar loop on unsupported targets.
 *
 * @param array  Pointer to the array to scan. Must not be NULL and must
 *               contain at least one element.
 *
 * @return uint32_expect_t with has_value true and u.value == the minimum
 *         element on success. On failure, has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if array->base.len == 0
 *
 * @code
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
 *     if (!result.has_value) { return; }
 *     uint32_array_t* arr = result.u.value;
 *
 *     push_back_uint32_array(arr, 70000u);
 *     push_back_uint32_array(arr, 10000u);
 *     push_back_uint32_array(arr, 40000u);
 *     // arr contains [70000, 10000, 40000].
 *
 *     uint32_expect_t r = uint32_array_min(arr);
 *     // r.has_value == true, r.u.value == 10000.
 *
 *     return_uint32_array(arr);
 * @endcode
 */
uint32_expect_t uint32_array_min(const uint32_array_t* array);

// --------------------------------------------------------------------------------

/**
 * @brief Find the maximum value in the array.
 *
 * Scans all elements using an unsigned 32-bit comparison and returns the
 * largest value found. When two or more elements share the maximum value the
 * value of the first occurrence is returned, consistent with the behaviour of
 * uint32_array_contains. The array is not modified. The scan dispatches to
 * the best SIMD horizontal-reduction path available at compile time and falls
 * back to a scalar loop on unsupported targets.
 *
 * @param array  Pointer to the array to scan. Must not be NULL and must
 *               contain at least one element.
 *
 * @return uint32_expect_t with has_value true and u.value == the maximum
 *         element on success. On failure, has_value is false and u.error
 *         is one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if array->base.len == 0
 *
 * @code
 *     uint32_expect_t r = uint32_array_max(arr);
 *     // r.has_value == true, r.u.value == 70000.
 * @endcode
 */
uint32_expect_t uint32_array_max(const uint32_array_t* array);
// -------------------------------------------------------------------------------- 

// /**
//  * @brief Sum all elements and return the result as a uint32_t.
//  *
//  * Iterates over every element and accumulates the total in a uint32_t. The
//  * inner loop dispatches to the best SIMD load path available at compile time.
//  *
//  * Overflow: the accumulator is a uint32_t. If the true sum exceeds UINT32_MAX
//  * the result wraps modulo 2^32. The caller is responsible for ensuring the
//  * array is small enough that overflow will not occur.
//  *
//  * @param array  Pointer to the array to sum. Must not be NULL.
//  *
//  * @return uint32_expect_t with has_value true and u.value == the sum on
//  *         success. On failure, has_value is false and u.error is one of:
//  *         - NULL_POINTER  if array is NULL
//  *         - EMPTY         if array->base.len == 0
//  *
//  * @code
//  *     allocator_vtable_t alloc = heap_allocator();
//  *     uint32_array_expect_t result = init_uint32_array(8, false, alloc);
//  *     if (!result.has_value) { return; }
//  *     uint32_array_t* arr = result.u.value;
//  *
//  *     push_back_uint32_array(arr, 100000u);
//  *     push_back_uint32_array(arr, 200000u);
//  *     push_back_uint32_array(arr, 300000u);
//  *     // arr contains [100000, 200000, 300000].
//  *
//  *     uint32_expect_t r = uint32_array_sum(arr);
//  *     // r.has_value == true, r.u.value == 600000.
//  *
//  *     return_uint32_array(arr);
//  * @endcode
//  */
// uint32_expect_t uint32_array_sum(const uint32_array_t* array);
// -------------------------------------------------------------------------------- 

/**
 * @brief Compute the prefix sum of a uint32 array into a new uint32 array.
 *
 * Allocates a new uint32_array_t of the same length as @p src and fills it
 * with the inclusive prefix sum: output[0] == src[0], and output[i] ==
 * output[i-1] + src[i] for every subsequent index. @p src is not modified.
 *
 * Overflow: each element is accumulated in a uint32_t. If the running total
 * exceeds UINT32_MAX it wraps modulo 2^32. The caller is responsible for
 * ensuring the values remain within range.
 *
 * The output array is owned by the caller and must be freed with
 * return_uint32_array when no longer needed.
 *
 * @param src    Source array to scan. Must not be NULL and must contain at
 *               least one element.
 * @param alloc  Allocator used for the output array.
 *
 * @return uint32_array_expect_t with has_value true and u.value pointing to
 *         the newly allocated output array on success. On failure, has_value
 *         is false and u.error is one of:
 *         - NULL_POINTER  if src is NULL
 *         - EMPTY         if src contains no elements
 *         - BAD_ALLOC     if the output struct allocation failed
 *         - OUT_OF_MEMORY if the output data buffer allocation failed
 *
 * @code
 *     allocator_vtable_t alloc = heap_allocator();
 *     uint32_array_expect_t r = init_uint32_array(8, false, alloc);
 *     if (!r.has_value) { return; }
 *     uint32_array_t* src = r.u.value;
 *
 *     push_back_uint32_array(src, 1000u);
 *     push_back_uint32_array(src, 2000u);
 *     push_back_uint32_array(src, 3000u);
 *     push_back_uint32_array(src, 4000u);
 *     // src contains [1000, 2000, 3000, 4000].
 *
 *     uint32_array_expect_t cr = cumulative_uint32_array(src, alloc);
 *     // cr.u.value contains [1000, 3000, 6000, 10000].
 *
 *     return_uint32_array(src);
 *     return_uint32_array(cr.u.value);
 * @endcode
 */
uint32_array_expect_t cumulative_uint32_array(const uint32_array_t* src,
                                              allocator_vtable_t    alloc);
// -------------------------------------------------------------------------------- 

/**
 * @brief Compute the prefix sum of an int32 array into a new int32 array.
 *
 * Allocates a new int32_array_t of the same length as @p src and fills it
 * with the inclusive prefix sum: output[0] == src[0], and output[i] ==
 * output[i-1] + src[i] for every subsequent index. @p src is not modified.
 *
 * Overflow: each element is accumulated in an int32_t. If the running total
 * exceeds INT32_MAX or falls below INT32_MIN it wraps with signed overflow.
 * The caller is responsible for ensuring the values remain within range.
 *
 * The output array is owned by the caller and must be freed with
 * return_int32_array when no longer needed.
 *
 * @param src    Source array to scan. Must not be NULL and must contain at
 *               least one element.
 * @param alloc  Allocator used for the output array.
 *
 * @return int32_array_expect_t with has_value true and u.value pointing to
 *         the newly allocated output array on success. On failure, has_value
 *         is false and u.error is one of:
 *         - NULL_POINTER  if src is NULL
 *         - EMPTY         if src contains no elements
 *         - BAD_ALLOC     if the output struct allocation failed
 *         - OUT_OF_MEMORY if the output data buffer allocation failed
 *
 * @code
 *     allocator_vtable_t alloc = heap_allocator();
 *     int32_array_expect_t r = init_int32_array(8, false, alloc);
 *     if (!r.has_value) { return; }
 *     int32_array_t* src = r.u.value;
 *
 *     push_back_int32_array(src, 1000);
 *     push_back_int32_array(src, 2000);
 *     push_back_int32_array(src, 3000);
 *     push_back_int32_array(src, 4000);
 *     // src contains [1000, 2000, 3000, 4000].
 *
 *     int32_array_expect_t cr = cumulative_int32_array(src, alloc);
 *     // cr.u.value contains [1000, 3000, 6000, 10000].
 *
 *     return_int32_array(src);
 *     return_int32_array(cr.u.value);
 * @endcode
 */
uint32_array_expect_t cumulative_uint32_array(const uint32_array_t* src,
                                              allocator_vtable_t   alloc);
// -------------------------------------------------------------------------------- 

/**
 * @brief Print a uint32 array in bracketed form with line wrapping.
 *
 * Prints the array in the form:
 *
 *     [ 1, 5, 2, 3, 6 ]
 *
 * If appending the next value would cause the current line to exceed
 * 70 columns, printing continues on the next line. Continuation lines
 * are indented by two spaces.
 *
 * @param array   Array to print. Must not be NULL.
 * @param stream  Output stream to write to. Must not be NULL.
 *
 * @return NO_ERROR on success, or NULL_POINTER if array or stream is NULL.
 *
 * @code{.c}
 *     print_uint32_array(arr, stdout);
 * @endcode
 */
error_code_t print_uint32_array(const uint32_array_t* array, FILE* stream);
// -------------------------------------------------------------------------------- 

/**
 * @brief Compare two uint32 arrays for exact equality using a SIMD-accelerated path.
 *
 * Two arrays are considered equal if:
 *   1. Both pointers are non-NULL.
 *   2. They contain the same number of elements (len).
 *   3. Every element compares equal using exact integer comparison.
 *
 * This function uses a SIMD-accelerated implementation when available
 * (e.g., AVX2, SSE, NEON) to compare elements in blocks, with a scalar
 * fallback for any remaining elements. The comparison is exact and
 * bitwise for each element.
 *
 * This function does NOT compare allocator state, capacity, or any
 * metadata beyond element count and data values.
 *
 * @param a  Pointer to the first uint32 array.
 * @param b  Pointer to the second uint32 array.
 *
 * @return true if both arrays are equal, false otherwise.
 */
bool uint32_array_equal(const uint32_array_t* a,
                        const uint32_array_t* b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Add a scalar value to every element of a uint32 array.
 *
 * This function adds the specified scalar value to each element in the
 * array in place. The operation is applied to all elements currently
 * stored in the array and preserves the array length and capacity.
 *
 * Internally, this function delegates to the generic add_scalar_array()
 * routine, which may utilize SIMD-accelerated paths when available,
 * falling back to scalar iteration otherwise.
 *
 * @param array Pointer to the uint32 array to modify.
 * @param value Scalar value to add to each element.
 *
 * @return error_code_t
 * @retval NO_ERROR       The operation completed successfully.
 * @retval NULL_POINTER   If @p array is NULL.
 * @retval EMPTY          If the array contains no elements.
 * @retval TYPE_MISMATCH  If the underlying dtype does not match uint32_TYPE.
 *
 * @note
 * The addition uses standard unsigned 8-bit arithmetic. If the result of
 * adding @p value to an element exceeds uint32_MAX, it wraps around modulo 256.
 *
 * @warning
 * This function modifies the array in place. No copy of the data is made.
 *
 * @see add_scalar_array
 * @see uint32_array_sum
 */
error_code_t uint32_add_scalar_array(uint32_array_t* array, uint32_t value);
// ================================================================================ 
// ================================================================================ 

/**
 * @brief A dictionary that maps null-terminated C-string keys to uint32_t values.
 *
 * This is a typedef alias for @ref dict_t with the convention that
 * @c data_size is always @c sizeof(uint32_t) (4 bytes) and @c dtype is
 * always @c UINT32_TYPE.  Do not use the raw @ref dict_t API directly on a
 * @c uint32_dict_t — always go through the functions declared in this header
 * so that the type constraints are enforced.
 *
 * @code
 *     allocator_vtable_t a = heap_allocator();
 *     uint32_dict_expect_t r = init_uint32_dict(16, true, a);
 *     if (!r.has_value) { // handle r.u.error }
 *     uint32_dict_t* d = r.u.value;
 *
 *     insert_uint32_dict(d, "width",  1920u, a);
 *     insert_uint32_dict(d, "height", 1080u, a);
 *     insert_uint32_dict(d, "depth",    32u, a);
 *
 *     uint32_t v;
 *     get_uint32_dict_value(d, "width", &v);   // v == 1920
 *
 *     return_uint32_dict(d);
 * @endcode
 */
typedef dict_t uint32_dict_t;
 
/**
 * @brief Expected return type for @ref init_uint32_dict and
 *        @ref copy_uint32_dict.
 *
 * Check @c has_value before using @c u.value.  On failure @c u.error
 * contains the relevant @ref error_code_t.
 */
typedef struct {
    bool has_value;
    union {
        uint32_dict_t* value;
        error_code_t   error;
    } u;
} uint32_dict_expect_t;
 
// ================================================================================
// Iterator type
// ================================================================================
 
/**
 * @brief Typed iterator callback for @ref foreach_uint32_dict.
 *
 * @param key       Null-terminated C-string key (points into the dict's
 *                  internal storage — do not free or store beyond the
 *                  callback).
 * @param key_len   Length of @p key in bytes, excluding the null terminator.
 * @param value     The uint32_t value associated with @p key.
 * @param user_data Caller-supplied context pointer passed unchanged from
 *                  @ref foreach_uint32_dict; may be NULL.
 *
 * @code
 *     static void print_entry(const char* key, size_t key_len,
 *                             uint32_t value, void* ud) {
 *         (void)ud;
 *         printf("  %.*s => %u\n", (int)key_len, key, (unsigned)value);
 *     }
 *     foreach_uint32_dict(d, print_entry, NULL);
 * @endcode
 */
typedef void (*uint32_dict_iter_fn)(const char* key,
                                    size_t      key_len,
                                    uint32_t    value,
                                    void*       user_data);
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
/**
 * @brief Allocate and initialise a new uint32_dict_t.
 *
 * The underlying @ref dict_t is created with @c data_size = sizeof(uint32_t)
 * and @c dtype = UINT32_TYPE.  The bucket count is rounded up to the next
 * power of two internally.
 *
 * @param capacity  Initial bucket count.  Must be > 0.
 * @param growth    If true the table resizes automatically when the load
 *                  factor exceeds 0.75.
 * @param alloc_v   Allocator for all internal memory.  @c alloc_v.allocate
 *                  must not be NULL.
 *
 * @return @ref uint32_dict_expect_t with @c has_value true on success.
 *         On failure @c u.error is one of NULL_POINTER, INVALID_ARG, or
 *         OUT_OF_MEMORY.
 *
 * @code
 *     allocator_vtable_t a = heap_allocator();
 *
 *     uint32_dict_expect_t r = init_uint32_dict(8, true, a);
 *     if (!r.has_value) {
 *         fprintf(stderr, "init failed: %d\n", r.u.error);
 *         return;
 *     }
 *     uint32_dict_t* d = r.u.value;
 *
 *     insert_uint32_dict(d, "port",  8080u, a);
 *     insert_uint32_dict(d, "range", 1024u, a);
 *
 *     return_uint32_dict(d);
 * @endcode
 */
uint32_dict_expect_t init_uint32_dict(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Release all memory owned by a uint32_dict_t.
 *
 * Frees every node (key copy + value buffer), the bucket array, and the
 * dict struct itself.  Passing NULL is safe and performs no action.
 *
 * @param dict  Dictionary to release.
 *
 * @code
 *     uint32_dict_t* d = init_uint32_dict(8, true, a).u.value;
 *     insert_uint32_dict(d, "x", 1000u, a);
 *     // ... use the dict ...
 *     return_uint32_dict(d);   // d must not be used after this point
 * @endcode
 */
void return_uint32_dict(uint32_dict_t* dict);
 
// ================================================================================
// Insert
// ================================================================================
 
/**
 * @brief Insert a null-terminated string key with a uint32_t value.
 *
 * The key length is measured with @c strlen(key).  The key bytes are copied
 * into the dict's own storage — the caller may free or reuse @p key
 * immediately after this call returns.
 *
 * If the key already exists the insertion is rejected and INVALID_ARG is
 * returned.  To overwrite an existing value use @ref update_uint32_dict.
 *
 * @param dict    Must not be NULL.
 * @param key     Null-terminated C-string key.  Must not be NULL.
 * @param value   The uint32_t value to store.
 * @param alloc_v Allocator used to allocate the new node.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG (duplicate key),
 *         CAPACITY_OVERFLOW (growth == false and table is full), or
 *         OUT_OF_MEMORY.
 *
 * @code
 *     uint32_dict_t* d = init_uint32_dict(8, true, a).u.value;
 *
 *     insert_uint32_dict(d, "width",  1920u, a);   // OK
 *     insert_uint32_dict(d, "height", 1080u, a);   // OK
 *
 *     // Duplicate key — returns INVALID_ARG, dict is unchanged
 *     error_code_t err = insert_uint32_dict(d, "width", 800u, a);
 *     assert(err == INVALID_ARG);
 *
 *     return_uint32_dict(d);
 * @endcode
 */
error_code_t insert_uint32_dict(uint32_dict_t*     dict,
                                 const char*        key,
                                 uint32_t           value,
                                 allocator_vtable_t alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Insert a bounded key (explicit length) with a uint32_t value.
 *
 * Identical to @ref insert_uint32_dict but the key length is supplied by the
 * caller rather than measured with @c strlen.  Useful when the key is a
 * sub-string of a larger buffer or when the length is already known.
 *
 * @param dict    Must not be NULL.
 * @param key     Pointer to the first byte of the key.  Must not be NULL.
 * @param key_len Number of bytes in the key.  Must be > 0.
 * @param value   The uint32_t value to store.
 * @param alloc_v Allocator used to allocate the new node.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, CAPACITY_OVERFLOW, or
 *         OUT_OF_MEMORY.
 *
 * @code
 *     // Key is a sub-string: "width_max", take only "width" (5 bytes)
 *     const char* buf = "width_max";
 *     insert_uint32_dict_n(d, buf, 5, 1920u, a);
 *
 *     uint32_t v;
 *     get_uint32_dict_value_n(d, buf, 5, &v);   // v == 1920
 * @endcode
 */
error_code_t insert_uint32_dict_n(uint32_dict_t*     dict,
                                   const char*        key,
                                   size_t             key_len,
                                   uint32_t           value,
                                   allocator_vtable_t alloc_v);
 
// ================================================================================
// Pop (remove and retrieve)
// ================================================================================
 
/**
 * @brief Remove the entry for a null-terminated key and return its value.
 *
 * If the key is found the node is freed and the stored value is written into
 * @p out_value (if non-NULL).  If the key is not found @p out_value is not
 * written.
 *
 * @param dict       Must not be NULL.
 * @param key        Null-terminated C-string key.  Must not be NULL.
 * @param out_value  Destination for the removed value, or NULL to discard.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND.
 *
 * @code
 *     uint32_dict_t* d = init_uint32_dict(8, true, a).u.value;
 *     insert_uint32_dict(d, "port", 8080u, a);
 *
 *     uint32_t removed;
 *     error_code_t err = pop_uint32_dict(d, "port", &removed);
 *     assert(err == NO_ERROR && removed == 8080u);
 *
 *     // Key is gone — next pop returns NOT_FOUND
 *     err = pop_uint32_dict(d, "port", NULL);
 *     assert(err == NOT_FOUND);
 *
 *     return_uint32_dict(d);
 * @endcode
 */
error_code_t pop_uint32_dict(uint32_dict_t* dict,
                              const char*    key,
                              uint32_t*      out_value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Remove the entry for a bounded key and return its value.
 *
 * Identical to @ref pop_uint32_dict but the key length is supplied explicitly.
 *
 * @param dict       Must not be NULL.
 * @param key        Pointer to the key bytes.  Must not be NULL.
 * @param key_len    Number of bytes in the key.  Must be > 0.
 * @param out_value  Destination for the removed value, or NULL to discard.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, or NOT_FOUND.
 *
 * @code
 *     const char* buf = "port_http";
 *     insert_uint32_dict_n(d, buf, 4, 8080u, a);   // key = "port"
 *
 *     uint32_t v;
 *     pop_uint32_dict_n(d, buf, 4, &v);             // removes "port", v == 8080
 * @endcode
 */
error_code_t pop_uint32_dict_n(uint32_dict_t* dict,
                                const char*    key,
                                size_t         key_len,
                                uint32_t*      out_value);
 
// ================================================================================
// Update
// ================================================================================
 
/**
 * @brief Overwrite the value of an existing null-terminated key.
 *
 * No allocation is performed.  If the key does not exist NOT_FOUND is
 * returned and the dict is unchanged.  To insert a new entry use
 * @ref insert_uint32_dict.
 *
 * @param dict   Must not be NULL.
 * @param key    Null-terminated C-string key.  Must not be NULL.
 * @param value  New uint32_t value to store.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND.
 *
 * @code
 *     uint32_dict_t* d = init_uint32_dict(8, true, a).u.value;
 *     insert_uint32_dict(d, "port", 8080u, a);
 *
 *     update_uint32_dict(d, "port", 443u);
 *
 *     uint32_t v;
 *     get_uint32_dict_value(d, "port", &v);
 *     assert(v == 443u);
 *
 *     return_uint32_dict(d);
 * @endcode
 */
error_code_t update_uint32_dict(uint32_dict_t* dict,
                                 const char*    key,
                                 uint32_t       value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Overwrite the value of an existing bounded key.
 *
 * Identical to @ref update_uint32_dict but the key length is supplied
 * explicitly.
 *
 * @param dict     Must not be NULL.
 * @param key      Pointer to the key bytes.  Must not be NULL.
 * @param key_len  Number of bytes in the key.  Must be > 0.
 * @param value    New uint32_t value to store.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, or NOT_FOUND.
 *
 * @code
 *     const char* buf = "port_http";
 *     insert_uint32_dict_n(d, buf, 4, 8080u, a);   // key = "port"
 *     update_uint32_dict_n(d, buf, 4, 443u);        // update "port" to 443
 * @endcode
 */
error_code_t update_uint32_dict_n(uint32_dict_t* dict,
                                   const char*    key,
                                   size_t         key_len,
                                   uint32_t       value);
 
// ================================================================================
// Lookup
// ================================================================================
 
/**
 * @brief Copy the value for a null-terminated key into a caller-supplied
 *        variable.
 *
 * @param dict       Must not be NULL.
 * @param key        Null-terminated C-string key.  Must not be NULL.
 * @param out_value  Destination for the retrieved value.  Must not be NULL.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND.
 *
 * @code
 *     uint32_dict_t* d = init_uint32_dict(8, true, a).u.value;
 *     insert_uint32_dict(d, "width", 1920u, a);
 *
 *     uint32_t v;
 *     if (get_uint32_dict_value(d, "width", &v) == NO_ERROR)
 *         printf("width = %u\n", (unsigned)v);
 *
 *     return_uint32_dict(d);
 * @endcode
 */
error_code_t get_uint32_dict_value(const uint32_dict_t* dict,
                                    const char*          key,
                                    uint32_t*            out_value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Copy the value for a bounded key into a caller-supplied variable.
 *
 * Identical to @ref get_uint32_dict_value but the key length is supplied
 * explicitly.
 *
 * @param dict       Must not be NULL.
 * @param key        Pointer to the key bytes.  Must not be NULL.
 * @param key_len    Number of bytes in the key.  Must be > 0.
 * @param out_value  Destination for the retrieved value.  Must not be NULL.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, or NOT_FOUND.
 *
 * @code
 *     const char* buf = "width_max";
 *     insert_uint32_dict_n(d, buf, 5, 1920u, a);   // key = "width"
 *
 *     uint32_t v;
 *     get_uint32_dict_value_n(d, buf, 5, &v);       // v == 1920
 * @endcode
 */
error_code_t get_uint32_dict_value_n(const uint32_dict_t* dict,
                                      const char*          key,
                                      size_t               key_len,
                                      uint32_t*            out_value);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Return a read-only pointer directly into the node's value buffer for
 *        a null-terminated key.
 *
 * The pointer is valid until the next mutation of the dict.  The caller must
 * not free it or write through it.
 *
 * @param dict  Must not be NULL.
 * @param key   Null-terminated C-string key.  Must not be NULL.
 *
 * @return Pointer to the stored uint32_t on success, NULL if not found or on
 *         error.
 *
 * @code
 *     insert_uint32_dict(d, "port", 8080u, a);
 *
 *     const uint32_t* p = get_uint32_dict_ptr(d, "port");
 *     if (p) printf("port = %u\n", (unsigned)*p);
 *     // Do not store p — it may be invalidated by the next insert or pop.
 * @endcode
 */
const uint32_t* get_uint32_dict_ptr(const uint32_dict_t* dict, const char* key);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Return a read-only pointer directly into the node's value buffer for
 *        a bounded key.
 *
 * Identical to @ref get_uint32_dict_ptr but the key length is supplied
 * explicitly.
 *
 * @param dict     Must not be NULL.
 * @param key      Pointer to the key bytes.  Must not be NULL.
 * @param key_len  Number of bytes in the key.  Must be > 0.
 *
 * @return Pointer to the stored uint32_t on success, NULL if not found or on
 *         error.
 *
 * @code
 *     const char* buf = "port_http";
 *     insert_uint32_dict_n(d, buf, 4, 8080u, a);   // key = "port"
 *
 *     const uint32_t* p = get_uint32_dict_ptr_n(d, buf, 4);
 *     if (p) printf("port = %u\n", (unsigned)*p);
 * @endcode
 */
const uint32_t* get_uint32_dict_ptr_n(const uint32_dict_t* dict,
                                       const char*          key,
                                       size_t               key_len);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Test whether a null-terminated key exists in the dict.
 *
 * @param dict  Must not be NULL.
 * @param key   Null-terminated C-string key.  Must not be NULL.
 *
 * @return true if the key exists, false otherwise (including on error).
 *
 * @code
 *     insert_uint32_dict(d, "active", 1u, a);
 *
 *     if (has_uint32_dict_key(d, "active"))
 *         printf("active is set\n");
 * @endcode
 */
bool has_uint32_dict_key(const uint32_dict_t* dict, const char* key);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Test whether a bounded key exists in the dict.
 *
 * Identical to @ref has_uint32_dict_key but the key length is supplied
 * explicitly.
 *
 * @param dict     Must not be NULL.
 * @param key      Pointer to the key bytes.  Must not be NULL.
 * @param key_len  Number of bytes in the key.  Must be > 0.
 *
 * @return true if the key exists, false otherwise.
 *
 * @code
 *     const char* buf = "active_flag";
 *     insert_uint32_dict_n(d, buf, 6, 1u, a);   // key = "active"
 *     assert(has_uint32_dict_key_n(d, buf, 6));
 * @endcode
 */
bool has_uint32_dict_key_n(const uint32_dict_t* dict,
                            const char*          key,
                            size_t               key_len);
 
// ================================================================================
// Utility
// ================================================================================
 
/**
 * @brief Remove all entries without freeing the dict or its bucket array.
 *
 * All nodes are freed via the dict's stored allocator.  The bucket array is
 * retained and zeroed, ready for reuse.  @c len and @c hash_size are reset
 * to 0.
 *
 * @param dict  Must not be NULL.
 *
 * @return NO_ERROR or NULL_POINTER.
 *
 * @code
 *     insert_uint32_dict(d, "a", 100u, a);
 *     insert_uint32_dict(d, "b", 200u, a);
 *     assert(uint32_dict_hash_size(d) == 2);
 *
 *     clear_uint32_dict(d);
 *     assert(uint32_dict_hash_size(d) == 0);
 *
 *     // The dict is still usable after clear
 *     insert_uint32_dict(d, "c", 300u, a);
 * @endcode
 */
error_code_t clear_uint32_dict(uint32_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Allocate a deep copy of a uint32_dict_t.
 *
 * All nodes are copied into a fresh bucket array of the same capacity.  The
 * new dict uses @p alloc_v for all of its allocations.
 *
 * @param src     Must not be NULL.
 * @param alloc_v Allocator for the new dict and its nodes.
 *
 * @return @ref uint32_dict_expect_t with @c has_value true on success.
 *
 * @code
 *     uint32_dict_t* orig = init_uint32_dict(8, true, a).u.value;
 *     insert_uint32_dict(orig, "x", 1000u, a);
 *     insert_uint32_dict(orig, "y", 2000u, a);
 *
 *     uint32_dict_expect_t cr = copy_uint32_dict(orig, a);
 *     assert(cr.has_value);
 *     uint32_dict_t* copy = cr.u.value;
 *
 *     // The copy is independent — mutating orig does not affect copy
 *     update_uint32_dict(orig, "x", 9999u);
 *     uint32_t v;
 *     get_uint32_dict_value(copy, "x", &v);
 *     assert(v == 1000u);
 *
 *     return_uint32_dict(copy);
 *     return_uint32_dict(orig);
 * @endcode
 */
uint32_dict_expect_t copy_uint32_dict(const uint32_dict_t* src,
                                      allocator_vtable_t   alloc_v);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Merge two uint32_dict_t instances into a new dict.
 *
 * All entries from @p a are inserted first.  Entries from @p b are then
 * processed:
 * - If the key does not exist in the result it is inserted.
 * - If the key already exists and @p overwrite is true the value from @p b
 *   replaces the value from @p a.
 * - If the key already exists and @p overwrite is false the value from @p a
 *   is kept.
 *
 * Neither source dict is modified.
 *
 * @param a          First source dict.  Must not be NULL.
 * @param b          Second source dict.  Must not be NULL.
 * @param overwrite  If true, @p b's values win on key conflicts.
 * @param alloc_v    Allocator for the new dict.
 *
 * @return @ref uint32_dict_expect_t with @c has_value true on success.
 *
 * @code
 *     uint32_dict_t* da = init_uint32_dict(8, true, a).u.value;
 *     uint32_dict_t* db = init_uint32_dict(8, true, a).u.value;
 *
 *     insert_uint32_dict(da, "shared", 100u, a);
 *     insert_uint32_dict(da, "only_a",   1u, a);
 *     insert_uint32_dict(db, "shared", 999u, a);
 *     insert_uint32_dict(db, "only_b",   2u, a);
 *
 *     // Merge without overwrite — "shared" keeps da's value of 100
 *     uint32_dict_expect_t mr = merge_uint32_dict(da, db, false, a);
 *     uint32_t v;
 *     get_uint32_dict_value(mr.u.value, "shared", &v);
 *     assert(v == 100u);
 *
 *     return_uint32_dict(mr.u.value);
 *     return_uint32_dict(da);
 *     return_uint32_dict(db);
 * @endcode
 */
uint32_dict_expect_t merge_uint32_dict(const uint32_dict_t* a,
                                       const uint32_dict_t* b,
                                       bool                 overwrite,
                                       allocator_vtable_t   alloc_v);
 
// ================================================================================
// Iteration
// ================================================================================
 
/**
 * @brief Call @p fn once for every entry in the dict.
 *
 * Traversal order follows bucket order, which is not guaranteed to match
 * insertion order.  The callback receives the key as a null-terminated
 * C-string pointer (into internal storage), its length, and the uint32_t
 * value.  The callback must not insert or remove entries during traversal.
 *
 * @param dict       Must not be NULL.
 * @param fn         Typed callback.  Must not be NULL.
 * @param user_data  Passed unchanged to @p fn; may be NULL.
 *
 * @return NO_ERROR or NULL_POINTER.
 *
 * @code
 *     static void print_entry(const char* key, size_t key_len,
 *                             uint32_t value, void* ud) {
 *         (void)key_len; (void)ud;
 *         printf("  %s = %u\n", key, (unsigned)value);
 *     }
 *
 *     uint32_dict_t* d = init_uint32_dict(8, true, a).u.value;
 *     insert_uint32_dict(d, "width",  1920u, a);
 *     insert_uint32_dict(d, "height", 1080u, a);
 *
 *     foreach_uint32_dict(d, print_entry, NULL);
 *     // Output (order may vary):
 *     //   width = 1920
 *     //   height = 1080
 *
 *     return_uint32_dict(d);
 * @endcode
 */
error_code_t foreach_uint32_dict(const uint32_dict_t* dict,
                                  uint32_dict_iter_fn  fn,
                                  void*                user_data);
 
// ================================================================================
// Introspection
// ================================================================================
 
/**
 * @brief Number of occupied buckets (chains with at least one entry).
 *
 * Returns 0 if @p dict is NULL.
 */
size_t uint32_dict_size(const uint32_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Total number of key-value pairs stored.
 *
 * Returns 0 if @p dict is NULL.
 */
size_t uint32_dict_hash_size(const uint32_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Number of buckets currently allocated.
 *
 * Returns 0 if @p dict is NULL.
 */
size_t uint32_dict_alloc(const uint32_dict_t* dict);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief true if @p dict is NULL or contains no entries.
 */
bool is_uint32_dict_empty(const uint32_dict_t* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Print a uint32_dict_t in a JSON-like key-value format with line wrapping.
 *
 * Values are printed using uint32_t formatting.
 *
 * Example:
 *
 *     { "a": 100000, "b": 4294967295 }
 *
 * Line wrapping occurs at 70 columns.
 *
 * @param dict    Pointer to dictionary. Must not be NULL.
 * @param stream  Output stream. Must not be NULL.
 *
 * @return NO_ERROR or:
 *         - NULL_POINTER
 *         - error from foreach_uint32_dict
 *
 * @note Output order is implementation-dependent.
 */
error_code_t print_uint32_dict(const uint32_dict_t* dict, FILE* stream);
// ================================================================================ 
// ================================================================================ 

// ================================================================================ 
// ================================================================================ 
// uint32_matrix_t — type-safe uint32 wrapper around matrix_t
//
// This section provides a uint32-specialised interface to the generic matrix_t
// type.  uint32_matrix_t is a typedef for matrix_t; all functions below fix
// the dtype to uint32_TYPE and accept / return uint32 values directly so that
// callers never need to pass a dtype_id_t or void* value.
//
// The wrapper covers every public function in c_matrix.h that either:
//   (a) takes a dtype_id_t argument (replaced with uint32_TYPE), or
//   (b) takes or returns void* element pointers (replaced with uint32 / uint32*).
//
// Functions that are already type-agnostic (matrix_rows, matrix_cols,
// matrix_is_square, matrix_has_same_shape, etc.) are wrapped with a
// uint32_matrix_t* signature for consistency, but simply delegate.
// ================================================================================ 
// ================================================================================ 

typedef matrix_t uint32_matrix_t;

typedef struct {
    bool has_value;
    union {
        uint32_matrix_t* value;
        error_code_t    error;
    } u;
} uint32_matrix_expect_t;

/* =============================================================================
 * Comparator / Predicate Function Types
 * ========================================================================== */

/**
 * @brief Predicate function to determine semantic zero for uint32 values.
 *
 * This function defines what values should be treated as "zero" during
 * dense-to-sparse conversion operations. It is primarily used by:
 *
 * - convert_uint32_matrix_zero()
 *
 * @param value The uint32 value to evaluate.
 *
 * @return true if the value should be treated as zero, false otherwise.
 *
 * @note If NULL is passed, the default behavior is:
 *       value == 0.0f
 *
 * @example
 * Treat both 0.0 and -1.0 as zero:
 * @code{.c}
 * bool zero_or_neg_one(uint32 v) {
 *     return (v == 0u) || (v == 3u);
 * }
 * @endcode
 */
typedef bool (*uint32_zero_fn)(uint32_t value);

// ================================================================================
// Initialization and teardown
// ================================================================================

/**
 * @brief Initialize a dense uint32 matrix with zero-initialized storage.
 *
 * Creates a dense matrix of shape @p rows by @p cols with the dtype fixed to
 * uint32_TYPE. All elements are initialised to zero.
 *
 * @param rows     Number of matrix rows. Must be > 0.
 * @param cols     Number of matrix columns. Must be > 0.
 * @param alloc_v  Allocator for the matrix header and data buffer.
 *
 * @return uint32_matrix_expect_t with has_value true on success.
 *
 * @code{.c}
 *     allocator_vtable_t a = heap_allocator();
 *     uint32_matrix_expect_t r = init_uint32_dense_matrix(3, 4, a);
 *     if (!r.has_value) { fprintf(stderr, "%d\n", r.u.error); return; }
 *     uint32_matrix_t* m = r.u.value;
 *
 *     set_uint32_matrix(m, 0, 0, 1u);
 *     set_uint32_matrix(m, 1, 2, 5u);
 *
 *     return_uint32_matrix(m);
 * @endcode
 */
uint32_matrix_expect_t init_uint32_dense_matrix(size_t             rows,
                                              size_t             cols,
                                              allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize an empty COO sparse uint32 matrix.
 *
 * Creates a coordinate-list sparse matrix with storage for up to @p capacity
 * entries. The dtype is fixed to uint32_TYPE.
 *
 * @param rows      Number of matrix rows. Must be > 0.
 * @param cols      Number of matrix columns. Must be > 0.
 * @param capacity  Initial entry capacity. Must be > 0.
 * @param growth    If true, the COO storage may grow when full.
 * @param alloc_v   Allocator for the matrix header and COO buffers.
 *
 * @return uint32_matrix_expect_t with has_value true on success.
 *
 * @code{.c}
 *     allocator_vtable_t a = heap_allocator();
 *     uint32_matrix_expect_t r = init_uint32_coo_matrix(100, 100, 16, true, a);
 *     uint32_matrix_t* m = r.u.value;
 *
 *     push_back_uint32_coo_matrix(m, 0, 5, 3u);
 *     push_back_uint32_coo_matrix(m, 42, 99, 7u);
 *
 *     return_uint32_matrix(m);
 * @endcode
 */
uint32_matrix_expect_t init_uint32_coo_matrix(size_t             rows,
                                            size_t             cols,
                                            size_t             capacity,
                                            bool               growth,
                                            allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Release all storage owned by a uint32 matrix.
 *
 * Frees internal buffers and the matrix object itself. Passing NULL is safe.
 *
 * @param mat  Matrix to destroy, or NULL.
 */
void return_uint32_matrix(uint32_matrix_t* mat);

// ================================================================================
// Element access
// ================================================================================

/**
 * @brief Read a uint32 element at the given row and column.
 *
 * For sparse formats, missing entries are returned as 0.0f.
 *
 * @param mat  Matrix to read from.
 * @param row  Zero-based row index.
 * @param col  Zero-based column index.
 * @param out  Receives the uint32 value. Must not be NULL.
 *
 * @return NO_ERROR on success, or NULL_POINTER / INVALID_ARG / OUT_OF_BOUNDS.
 *
 * @code
 *     uint32_t v = 0u;
 *     get_uint32_matrix(m, 1, 2, &v);
 * @endcode
 */
error_code_t get_uint32_matrix(const uint32_matrix_t* mat,
                              size_t                row,
                              size_t                col,
                              uint32_t*              out);

// -------------------------------------------------------------------------------- 

/**
 * @brief Write a uint32 element at the given row and column.
 *
 * Dense and COO matrices are supported. CSR/CSC return ILLEGAL_STATE.
 *
 * @param mat    Matrix to modify.
 * @param row    Zero-based row index.
 * @param col    Zero-based column index.
 * @param value  The uint32 value to store.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 *
 * @code
 *     set_uint32_matrix(m, 0, 0, 42u);
 * @endcode
 */
error_code_t set_uint32_matrix(uint32_matrix_t* mat,
                              size_t          row,
                              size_t          col,
                              uint32_t         value);

// ================================================================================
// COO assembly helpers
// ================================================================================

/**
 * @brief Reserve additional entry capacity for a uint32 COO matrix.
 *
 * @param mat       COO matrix to grow.
 * @param capacity  Requested minimum entry capacity.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t reserve_uint32_coo_matrix(uint32_matrix_t* mat,
                                      size_t          capacity);

// -------------------------------------------------------------------------------- 

/**
 * @brief Append or overwrite a uint32 entry in a COO matrix.
 *
 * If (row, col) already exists its value is overwritten; otherwise a new
 * entry is appended. The matrix is marked unsorted after insertion.
 *
 * @param mat    COO matrix to append to.
 * @param row    Zero-based row index.
 * @param col    Zero-based column index.
 * @param value  The uint32 value to insert.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 *
 * @code
 *     push_back_uint32_coo_matrix(m, 0, 5, 3u);
 * @endcode
 */
error_code_t push_back_uint32_coo_matrix(uint32_matrix_t* mat,
                                        size_t          row,
                                        size_t          col,
                                        uint32_t         value);

// -------------------------------------------------------------------------------- 

/**
 * @brief Sort COO entries by (row, col).
 *
 * @param mat  COO matrix to sort.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t sort_uint32_coo_matrix(uint32_matrix_t* mat);

// ================================================================================
// Lifecycle / structural operations
// ================================================================================

/**
 * @brief Clear a uint32 matrix, preserving its allocated storage.
 *
 * Dense matrices are zeroed. COO matrices reset to zero logical entries.
 *
 * @param mat  Matrix to clear.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t clear_uint32_matrix(uint32_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Deep-copy a uint32 matrix.
 *
 * @param src      Matrix to copy.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return uint32_matrix_expect_t with has_value true on success.
 */
uint32_matrix_expect_t copy_uint32_matrix(const uint32_matrix_t* src,
                                        allocator_vtable_t    alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Convert a uint32 matrix to a different storage format.
 *
 * Produces a new matrix with the same logical values in the target format.
 *
 * @param src      Matrix to convert.
 * @param target   Desired destination storage format. Can be any value in 
 * ``matrix_format_t`` enum, (i.e. ``DENSE_MATRIX``, ``COO_MATRIX``, 
 * ``CSR_MATRIX``, ``CSC__MATRIX``)
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return uint32_matrix_expect_t with has_value true on success.
 *
 * @code
 *     uint32_matrix_expect_t csr =
 *         convert_uint32_matrix(dense_m, CSR_MATRIX, a);
 * @endcode
 */
uint32_matrix_expect_t convert_uint32_matrix(const uint32_matrix_t* src,
                                           matrix_format_t       target,
                                           allocator_vtable_t    alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Compute the transpose of a uint32 matrix.
 *
 * Dense stays dense, COO stays COO, CSR transposes to CSR, CSC to CSC.
 *
 * @param src      Matrix to transpose.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return uint32_matrix_expect_t with has_value true on success.
 */
uint32_matrix_expect_t transpose_uint32_matrix(const uint32_matrix_t* src,
                                             allocator_vtable_t    alloc_v);

// ================================================================================
// Fill and zero
// ================================================================================

/**
 * @brief Fill a uint32 matrix with a single repeated value.
 *
 * Nonzero fill is supported only for dense matrices. Supplying 0.0f resets
 * the matrix through clear_uint32_matrix.
 *
 * @param mat    Matrix to fill.
 * @param value  The uint32 fill value.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 *
 * @code
 *     fill_uint32_matrix(m, 1u);   // every element becomes 1.0
 *     fill_uint32_matrix(m, 0u);   // equivalent to clear
 * @endcode
 */
error_code_t fill_uint32_matrix(uint32_matrix_t* mat,
                               uint32_t         value);
// ================================================================================
// Shape and compatibility queries
// ================================================================================

/**
 * @brief Test whether two uint32 matrices have identical shape.
 */
bool uint32_matrix_has_same_shape(const uint32_matrix_t* a,
                                 const uint32_matrix_t* b);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a uint32 matrix is square.
 */
bool uint32_matrix_is_square(const uint32_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a uint32 matrix uses a sparse representation.
 */
bool uint32_matrix_is_sparse(const uint32_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a uint32 matrix is logically all zeros.
 */
bool uint32_matrix_is_zero(const uint32_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Compare two uint32 matrices for numerical equality.
 *
 * This function determines whether two matrices are logically equal based on
 * their numerical values, independent of their internal storage format.
 *
 * Equality is defined using standard uint32ing-point comparison semantics:
 *
 * Matrices must:
 * - be non-NULL
 * - have the same dimensions (rows and columns)
 * - have dtype == uint32_TYPE
 *
 * The comparison is **format-agnostic**, meaning matrices with different
 * storage formats (e.g., dense vs COO vs CSR vs CSC) are considered equal
 * if they represent the same logical values.
 *
 * Performance optimizations:
 * - Dense × Dense comparisons use SIMD-accelerated numerical comparison.
 * - COO × COO comparisons use a fast path when both matrices are sorted,
 *   comparing index arrays and values directly.
 * - All other cases fall back to element-wise comparison using logical
 *   matrix access.
 *
 * @param a First matrix (may be NULL).
 * @param b Second matrix (may be NULL).
 *
 * @return true if the matrices are numerically equal, false otherwise.
 *
 * @note This function performs numerical comparison only. For custom
 *       comparison rules (e.g., tolerance-based equality or sign-sensitive
 *       comparisons), use :c:func:`uint32_matrix_equal_cmp`.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * uint32_matrix_expect_t r1 = init_uint32_dense_matrix(2, 2, alloc);
 * uint32_matrix_expect_t r2 = init_uint32_dense_matrix(2, 2, alloc);
 *
 * uint32_matrix_t* a = r1.u.value;
 * uint32_matrix_t* b = r2.u.value;
 *
 * set_uint32_matrix(a, 0, 0, 4u);
 * set_uint32_matrix(b, 0, 0,  0u);
 *
 * assert_true(uint32_matrix_equal(a, b)); // false
 *
 * return_uint32_matrix(a);
 * return_uint32_matrix(b);
 * @endcode
 *
 * @code{.c}
 * // Equality across different storage formats
 *
 * uint32_matrix_expect_t dense_r = init_uint32_dense_matrix(2, 2, alloc);
 * uint32_matrix_t* dense = dense_r.u.value;
 *
 * set_uint32_matrix(dense, 0, 1, 3u);
 *
 * uint32_matrix_expect_t csr_r =
 *     convert_uint32_matrix(dense, CSR_MATRIX, alloc);
 *
 * uint32_matrix_t* csr = csr_r.u.value;
 *
 * // Same logical values → equal
 * assert_true(uint32_matrix_equal(dense, csr)); // true
 *
 * return_uint32_matrix(csr);
 * return_uint32_matrix(dense);
 * @endcode
 */
bool uint32_matrix_equal(const uint32_matrix_t* a,
                        const uint32_matrix_t* b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether two uint32 matrices are compatible for addition.
 *
 * Requires equal shape and equal dtype.
 */
bool uint32_matrix_is_add_compatible(const uint32_matrix_t* a,
                                    const uint32_matrix_t* b);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether two uint32 matrices are compatible for multiplication.
 *
 * Requires a->cols == b->rows and equal dtype.
 */
bool uint32_matrix_is_multiply_compatible(const uint32_matrix_t* a,
                                         const uint32_matrix_t* b);

// ================================================================================
// Row / column swaps
// ================================================================================

/**
 * @brief Swap two rows of a uint32 matrix in place.
 *
 * Dense and COO matrices are supported. CSR/CSC return OPERATION_UNAVAILABLE.
 *
 * @param mat  Matrix to modify.
 * @param r1   First zero-based row index.
 * @param r2   Second zero-based row index.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t swap_uint32_matrix_rows(uint32_matrix_t* mat,
                                    size_t          r1,
                                    size_t          r2);

// -------------------------------------------------------------------------------- 

/**
 * @brief Swap two columns of a uint32 matrix in place.
 *
 * Dense and COO matrices are supported. CSR/CSC return OPERATION_UNAVAILABLE.
 *
 * @param mat  Matrix to modify.
 * @param c1   First zero-based column index.
 * @param c2   Second zero-based column index.
 *
 * @return NO_ERROR on success, or an appropriate error_code_t.
 */
error_code_t swap_uint32_matrix_cols(uint32_matrix_t* mat,
                                    size_t          c1,
                                    size_t          c2);

// ================================================================================
// Special matrix constructors
// ================================================================================

/**
 * @brief Initialize a dense uint32 identity matrix.
 *
 * Creates an @p n by @p n dense matrix with 1.0f on the diagonal and 0.0f
 * elsewhere.
 *
 * @param n        Matrix order (rows == cols).
 * @param alloc_v  Allocator for the matrix.
 *
 * @return uint32_matrix_expect_t with has_value true on success.
 *
 * @code
 *     uint32_matrix_expect_t r = init_uint32_identity_matrix(3, a);
 *     // 3x3 identity matrix.
 * @endcode
 */
uint32_matrix_expect_t init_uint32_identity_matrix(size_t             n,
                                                 allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a dense uint32 row vector (1 x length).
 *
 * @param length   Vector length (number of columns).
 * @param alloc_v  Allocator for the matrix.
 *
 * @return uint32_matrix_expect_t with has_value true on success.
 */
uint32_matrix_expect_t init_uint32_row_vector(size_t             length,
                                            allocator_vtable_t alloc_v);

// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a dense uint32 column vector (length x 1).
 *
 * @param length   Vector length (number of rows).
 * @param alloc_v  Allocator for the matrix.
 *
 * @return uint32_matrix_expect_t with has_value true on success.
 */
uint32_matrix_expect_t init_uint32_col_vector(size_t             length,
                                            allocator_vtable_t alloc_v);

// ================================================================================
// Vector shape queries
// ================================================================================

/**
 * @brief Test whether a uint32 matrix has row-vector shape (1 x N).
 */
bool uint32_matrix_is_row_vector(const uint32_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a uint32 matrix has column-vector shape (N x 1).
 */
bool uint32_matrix_is_col_vector(const uint32_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a uint32 matrix has vector shape (row or column).
 */
bool uint32_matrix_is_vector(const uint32_matrix_t* mat);

// -------------------------------------------------------------------------------- 

/**
 * @brief Return the logical length of a vector-shaped uint32 matrix.
 *
 * For row vectors returns cols; for column vectors returns rows.
 * Non-vector matrices return 0.
 */
size_t uint32_matrix_vector_length(const uint32_matrix_t* mat);

// ================================================================================
// Conversion with semantic zero callback
// ================================================================================

/**
 * @brief Convert a uint32 matrix to a different storage format using an
 *        optional semantic zero-test.
 *
 * This function performs the same structural conversion as
 * :c:func:`convert_uint32_matrix`, but allows the caller to define what
 * uint32 values should be treated as zero during dense-to-sparse conversion.
 *
 * The @p is_zero callback is used only when:
 *   - @p src is a dense matrix, and
 *   - @p target is COO_MATRIX, CSR_MATRIX, or CSC_MATRIX.
 *
 * In all other cases, this function behaves exactly like
 * :c:func:`convert_uint32_matrix`.
 *
 * If @p is_zero is NULL, the default zero rule is:
 *
 * @code{.c}
 * value == 0.0f
 * @endcode
 *
 * @param src      Matrix to convert. Must not be NULL.
 * @param target   Desired destination storage format.
 * @param alloc_v  Allocator for the destination matrix.
 * @param is_zero  Optional semantic zero predicate used only for
 *                 dense-to-sparse conversion.
 *
 * @return uint32_matrix_expect_t with has_value true on success, or
 *         u.error describing the failure.
 *
 * Possible failure codes include:
 *   - NULL_POINTER
 *   - ILLEGAL_STATE
 *   - LENGTH_OVERFLOW
 *   - BAD_ALLOC
 *   - OUT_OF_MEMORY
 *
 * @code{.c}
 * static bool zero_or_neg_one(uint32 v) {
 *     return (v == 0u) || (v == 3u);
 * }
 *
 * uint32_matrix_expect_t r =
 *     convert_uint32_matrix_zero(dense_m, CSR_MATRIX, alloc, zero_or_neg_one);
 * @endcode
 *
 * @note This function is primarily intended for domain-specific sparse
 *       conversion rules where exact zero is not sufficient.
 */
uint32_matrix_expect_t convert_uint32_matrix_zero(const uint32_matrix_t* src,
                                                matrix_format_t       target,
                                                allocator_vtable_t    alloc_v,
                                                uint32_zero_fn         is_zero);
// -------------------------------------------------------------------------------- 

/**
 * @brief Print a uint32 matrix in a human-readable format.
 *
 * Dense matrices are printed row-by-row as nested bracketed arrays:
 *
 *     [ [ 1, 2, 3 ],
 *       [ 4, 5, 6 ] ]
 *
 * Sparse matrices (COO, CSR, CSC) are printed as a flat list of explicitly
 * stored logical nonzero entries:
 *
 *     [ (0, 0): 1, (0, 1): 2, (1, 2): 6 ]
 *
 * Dense printing does not wrap by column count; each row is printed on its
 * own output line after the first row.
 *
 * Sparse printing wraps at 70 columns. If appending the next `(row, col): value`
 * entry would exceed column 70, output continues on the next line with a
 * two-space indentation.
 *
 * Sparse output is printed in row-major logical order, independent of the
 * underlying sparse storage format.
 *
 * @param mat    Pointer to the matrix. Must not be NULL.
 * @param stream Output stream. Must not be NULL.
 *
 * @return NO_ERROR or:
 *         - NULL_POINTER
 *         - ILLEGAL_STATE
 *         - error from get_uint32_matrix
 */
error_code_t print_uint32_matrix(const uint32_matrix_t* mat, FILE* stream);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieve the minimum value from a uint32 matrix.
 *
 * This function determines the minimum element in a matrix of type
 * `uint32_matrix_t`. It delegates the index computation to the generic
 * `matrix_min()` function and then retrieves the corresponding value
 * based on the matrix storage format.
 *
 * The comparison is performed using unsigned 8-bit integer ordering.
 *
 * @param mat Pointer to the uint32 matrix.
 *
 * @return uint32_expect_t
 * @retval has_value = true   The minimum value was found and is stored in u.value.
 * @retval has_value = false  An error occurred, and the error code is stored in u.error.
 *
 * @errors
 * - NULL_POINTER   if @p mat is NULL.
 * - TYPE_MISMATCH  if the matrix dtype is not uint32_TYPE.
 * - EMPTY          if the matrix contains no elements:
 *                  - dense: rows * cols == 0
 *                  - sparse: nnz == 0
 * - INVALID_ARG    if the matrix format is not recognized.
 * - LENGTH_OVERFLOW if an internal size computation overflows (dense matrices only).
 *
 * @note
 * This function relies on the generic `matrix_min()` function to determine the
 * index of the minimum element. The returned index is interpreted according to
 * the matrix format:
 *
 * - DENSE_MATRIX: index is a row-major offset.
 * - COO_MATRIX:   index refers to entries in row_idx[], col_idx[], and values[].
 * - CSR_MATRIX:   index refers to entries in col_idx[] and values[].
 * - CSC_MATRIX:   index refers to entries in row_idx[] and values[].
 *
 * @warning
 * For sparse matrices, only stored values (nnz) are considered. Implicit zero
 * elements are not included in the comparison.
 *
 * @see matrix_min
 */
uint32_expect_t uint32_matrix_min(const uint32_matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieve the maximum value from a uint32 matrix.
 *
 * This function determines the maximum element in a matrix of type
 * `uint32_matrix_t`. It delegates the index computation to the generic
 * `matrix_max()` function and then retrieves the corresponding value
 * based on the matrix storage format.
 *
 * The comparison is performed using unsigned 8-bit integer ordering.
 *
 * @param mat Pointer to the uint32 matrix.
 *
 * @return uint32_expect_t
 * @retval has_value = true   The maximum value was found and is stored in u.value.
 * @retval has_value = false  An error occurred, and the error code is stored in u.error.
 *
 * @errors
 * - NULL_POINTER   if @p mat is NULL.
 * - TYPE_MISMATCH  if the matrix dtype is not uint32_TYPE.
 * - EMPTY          if the matrix contains no elements:
 *                  - dense: rows * cols == 0
 *                  - sparse: nnz == 0
 * - INVALID_ARG    if the matrix format is not recognized.
 * - LENGTH_OVERFLOW if an internal size computation overflows (dense matrices only).
 *
 * @note
 * This function relies on the generic `matrix_max()` function to determine the
 * index of the maximum element. The returned index is interpreted according to
 * the matrix format:
 *
 * - DENSE_MATRIX: index is a row-major offset.
 * - COO_MATRIX:   index refers to entries in row_idx[], col_idx[], and values[].
 * - CSR_MATRIX:   index refers to entries in col_idx[] and values[].
 * - CSC_MATRIX:   index refers to entries in row_idx[] and values[].
 *
 * @warning
 * For sparse matrices, only stored values (nnz) are considered. Implicit zero
 * elements are not included in the comparison.
 *
 * @see matrix_max
 */
uint32_expect_t uint32_matrix_max(const uint32_matrix_t* mat);
// -------------------------------------------------------------------------------- 

// /**
//  * @brief Compute the sum of all elements in a uint32 matrix.
//  *
//  * This function computes the sum of all elements in a matrix of type
//  * `uint32_matrix_t`. It delegates traversal to the generic `matrix_sum()`
//  * function and accumulates the result using unsigned 32-bit arithmetic.
//  *
//  * The accumulator is initialized to zero and updated for each element in
//  * the matrix using standard C unsigned integer addition.
//  *
//  * @param mat Pointer to the uint32 matrix.
//  *
//  * @return uint32_expect_t
//  * @retval has_value = true   The sum was successfully computed and is stored in u.value.
//  * @retval has_value = false  An error occurred, and the error code is stored in u.error.
//  *
//  * @errors
//  * - NULL_POINTER   if @p mat is NULL.
//  * - TYPE_MISMATCH  if the matrix dtype is not UINT32_TYPE.
//  * - EMPTY          if the matrix contains no elements:
//  *                  - dense: rows * cols == 0
//  *                  - sparse: nnz == 0
//  * - INVALID_ARG    if the matrix format is not recognized.
//  * - LENGTH_OVERFLOW if an internal size computation overflows (dense matrices only).
//  *
//  * @note
//  * For dense matrices, all elements (rows * cols) are included in the sum.
//  * For sparse matrices (COO, CSR, CSC), only stored elements (nnz) are included.
//  *
//  * @warning
//  * This function uses standard unsigned 32-bit arithmetic. If the mathematical
//  * sum exceeds `UINT32_MAX`, the result will wrap around modulo 2^32. Overflow
//  * is not detected or reported.
//  *
//  * @see matrix_sum
//  * @see uint32_matrix_min
//  * @see uint32_matrix_max
//  */
// uint32_expect_t uint32_matrix_sum(const uint32_matrix_t* mat);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_uint32_H */
// ================================================================================
// ================================================================================
// eof
