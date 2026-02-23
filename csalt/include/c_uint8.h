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
 * @brief Initialize a new uint8_array_t.
 *
 * @param capacity  Initial element capacity. Must be > 0.
 * @param growth    If true, the buffer grows automatically when full.
 * @param alloc_v   Allocator vtable for all memory operations.
 *
 * @return uint8_array_expect_t. On failure u.error is one of:
 *         - NULL_POINTER    if alloc_v.allocate is NULL
 *         - INVALID_ARG     if capacity is 0
 *         - ILLEGAL_STATE   if the dtype registry could not be initialized
 *         - LENGTH_OVERFLOW if capacity would overflow size_t
 *         - BAD_ALLOC       if the struct allocation fails
 *         - OUT_OF_MEMORY   if the data buffer allocation fails
 */
uint8_array_expect_t init_uint8_array(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v);
// --------------------------------------------------------------------------------

/**
 * @brief Return a uint8_array_t's memory back to its allocator.
 *
 * @param array  Pointer to the array. Silently ignored if NULL.
 */
void return_uint8_array(uint8_array_t* array);
// ================================================================================
// Push operations
// ================================================================================

/**
 * @brief Append one uint8_t to the end of the array.
 *
 * @param array  Must not be NULL.
 * @param value  Value to append.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - CAPACITY_OVERFLOW if full and growth is false
 *         - OUT_OF_MEMORY     if reallocation fails
 *         - LENGTH_OVERFLOW   if new capacity would overflow size_t
 */
error_code_t push_back_uint8_array(uint8_array_t* array, uint8_t value);
// --------------------------------------------------------------------------------

/**
 * @brief Prepend one uint8_t to the front of the array.
 *
 * @param array  Must not be NULL.
 * @param value  Value to prepend.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - CAPACITY_OVERFLOW if full and growth is false
 *         - OUT_OF_MEMORY     if reallocation fails
 *         - LENGTH_OVERFLOW   if new capacity would overflow size_t
 */
error_code_t push_front_uint8_array(uint8_array_t* array, uint8_t value);
// --------------------------------------------------------------------------------

/**
 * @brief Insert one uint8_t at an arbitrary index.
 *
 * @param array  Must not be NULL.
 * @param index  Must be <= array->base.len.
 * @param value  Value to insert.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if array is NULL
 *         - OUT_OF_BOUNDS     if index > array->base.len
 *         - CAPACITY_OVERFLOW if full and growth is false
 *         - OUT_OF_MEMORY     if reallocation fails
 *         - LENGTH_OVERFLOW   if new capacity would overflow size_t
 */
error_code_t push_at_uint8_array(uint8_array_t* array,
                                  size_t         index,
                                  uint8_t        value);
// ================================================================================
// Get operation
// ================================================================================

/**
 * @brief Retrieve the uint8_t at index, copying it into out.
 *
 * @param array  Must not be NULL.
 * @param index  Must be < array->base.len.
 * @param out    Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array or out is NULL
 *         - OUT_OF_BOUNDS if index >= array->base.len
 */
error_code_t get_uint8_array_index(const uint8_array_t* array,
                                    size_t               index,
                                    uint8_t*             out);
// ================================================================================
// Pop operations
// ================================================================================

/**
 * @brief Remove and optionally retrieve the last element.
 *
 * @param array  Must not be NULL.
 * @param out    Receives the removed value, or NULL to discard.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 */
error_code_t pop_back_uint8_array(uint8_array_t* array, uint8_t* out);
// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally retrieve the first element.
 *
 * @param array  Must not be NULL.
 * @param out    Receives the removed value, or NULL to discard.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 */
error_code_t pop_front_uint8_array(uint8_array_t* array, uint8_t* out);
// --------------------------------------------------------------------------------

/**
 * @brief Remove and optionally retrieve the element at index.
 *
 * @param array  Must not be NULL.
 * @param out    Receives the removed value, or NULL to discard.
 * @param index  Must be < array->base.len.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - EMPTY         if the array contains no elements
 *         - OUT_OF_BOUNDS if index >= array->base.len
 */
error_code_t pop_any_uint8_array(uint8_array_t* array,
                                  uint8_t*       out,
                                  size_t         index);
// ================================================================================
// Utility operations
// ================================================================================

/**
 * @brief Reset the array to empty, zeroing the buffer.
 *
 * @param array  Must not be NULL.
 * @return NO_ERROR on success, or NULL_POINTER if array is NULL.
 */
error_code_t clear_uint8_array(uint8_array_t* array);
// --------------------------------------------------------------------------------

/**
 * @brief Overwrite the element at index without changing len.
 *
 * @param array  Must not be NULL.
 * @param index  Must be < array->base.len.
 * @param value  Replacement value.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER  if array is NULL
 *         - OUT_OF_BOUNDS if index >= array->base.len
 */
error_code_t set_uint8_array_index(uint8_array_t* array,
                                    size_t         index,
                                    uint8_t        value);
// --------------------------------------------------------------------------------

/**
 * @brief Deep copy src into a new uint8_array_t allocated through alloc_v.
 *
 * @param src      Must not be NULL.
 * @param alloc_v  Allocator for the new array.
 *
 * @return uint8_array_expect_t. On failure u.error is one of:
 *         - NULL_POINTER  if src is NULL or alloc_v.allocate is NULL
 *         - BAD_ALLOC     if struct allocation fails
 *         - OUT_OF_MEMORY if data buffer allocation fails
 */
uint8_array_expect_t copy_uint8_array(const uint8_array_t* src,
                                       allocator_vtable_t   alloc_v);
// --------------------------------------------------------------------------------

/**
 * @brief Append all elements of src onto the end of dst.
 *
 * @param dst  Must not be NULL.
 * @param src  Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if dst or src is NULL
 *         - CAPACITY_OVERFLOW if dst is full and growth is false
 *         - OUT_OF_MEMORY     if reallocation fails
 *         - LENGTH_OVERFLOW   if combined length would overflow size_t
 */
error_code_t concat_uint8_array(uint8_array_t*       dst,
                                 const uint8_array_t* src);
// --------------------------------------------------------------------------------

/**
 * @brief Allocate a new array from a sub-range [start, end) of src.
 *
 * @param src      Must not be NULL.
 * @param start    Inclusive start index.
 * @param end      Exclusive end index. Must be > start and <= src->base.len.
 * @param alloc_v  Allocator for the new array.
 *
 * @return uint8_array_expect_t. On failure u.error is one of:
 *         - NULL_POINTER  if src is NULL or alloc_v.allocate is NULL
 *         - INVALID_ARG   if start >= end
 *         - OUT_OF_BOUNDS if end > src->base.len
 *         - BAD_ALLOC     if struct allocation fails
 *         - OUT_OF_MEMORY if data buffer allocation fails
 */
uint8_array_expect_t slice_uint8_array(const uint8_array_t* src,
                                        size_t               start,
                                        size_t               end,
                                        allocator_vtable_t   alloc_v);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the elements of the array in place.
 *
 * @param array  Must not be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if array is NULL
 *         - EMPTY        if array->base.len < 2
 */
error_code_t reverse_uint8_array(uint8_array_t* array);
// ================================================================================
// Search
// ================================================================================

/**
 * @brief Search for the first occurrence of value in [start, end).
 *
 * @param array  Must not be NULL.
 * @param value  Value to search for.
 * @param start  Inclusive start index.
 * @param end    Exclusive end index. Must be > start and <= array->base.len.
 *
 * @return size_expect_t with the index of the first match on success, or
 *         u.error set to one of:
 *         - NULL_POINTER  if array is NULL
 *         - INVALID_ARG   if start >= end
 *         - OUT_OF_BOUNDS if end > array->base.len
 *         - NOT_FOUND     if no match exists in [start, end)
 */
size_expect_t uint8_array_contains(const uint8_array_t* array,
                                    uint8_t              value,
                                    size_t               start,
                                    size_t               end);
// ================================================================================
// Introspection
// ================================================================================

/**
 * @brief Return the number of elements stored in the array.
 * @return 0 if array is NULL.
 */
size_t uint8_array_size(const uint8_array_t* array);
// --------------------------------------------------------------------------------

/**
 * @brief Return the allocated capacity in elements.
 * @return 0 if array is NULL.
 */
size_t uint8_array_alloc(const uint8_array_t* array);
// --------------------------------------------------------------------------------

/**
 * @brief Return the size of one element in bytes (always 1 for uint8_t).
 * @return 0 if array is NULL.
 */
size_t uint8_array_data_size(const uint8_array_t* array);
// --------------------------------------------------------------------------------

/**
 * @brief Return true if the array contains no elements.
 * @return true if array is NULL or len == 0.
 */
bool is_uint8_array_empty(const uint8_array_t* array);
// --------------------------------------------------------------------------------

/**
 * @brief Return true if the array is at full capacity.
 * @return true if array is NULL or len == alloc.
 */
bool is_uint8_array_full(const uint8_array_t* array);
// --------------------------------------------------------------------------------

/**
 * @brief Return true if ptr is a valid element-aligned pointer into the
 *        live region of the array's data buffer.
 *
 * @param array  The array to check against.
 * @param ptr    Pointer to test.
 * @return false if either argument is NULL or ptr is out of bounds.
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
