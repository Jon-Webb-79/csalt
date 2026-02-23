// ================================================================================
// ================================================================================
// - File:    c_uint8.c
// - Purpose: This file contains the implementations for funcitons related to 
//            uint8_t data structures.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 23, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_uint8.h"
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Internal helper: unwrap a raw array_expect_t into a uint8_array_expect_t.
// The array_t* stored inside the array_expect_t is reinterpreted as the base
// of a uint8_array_t since uint8_array_t contains array_t as its first member.
// ================================================================================

static inline uint8_array_expect_t _wrap_expect(array_expect_t e) {
    if (e.has_value) {
        return (uint8_array_expect_t){
            .has_value = true,
            .u.value   = (uint8_array_t*)e.u.value
        };
    }
    return (uint8_array_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}
// ================================================================================
// Initialization and teardown
// ================================================================================

uint8_array_expect_t init_uint8_array(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v) {
    return _wrap_expect(init_array(capacity, UINT8_TYPE, growth, alloc_v));
}
// --------------------------------------------------------------------------------

void return_uint8_array(uint8_array_t* array) {
    if (array == NULL) return;
    return_array(&array->base);
}
// ================================================================================
// Push operations
// ================================================================================

error_code_t push_back_uint8_array(uint8_array_t* array, uint8_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_back_array(&array->base, &value, UINT8_TYPE);
}
// --------------------------------------------------------------------------------

error_code_t push_front_uint8_array(uint8_array_t* array, uint8_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_front_array(&array->base, &value, UINT8_TYPE);
}
// --------------------------------------------------------------------------------

error_code_t push_at_uint8_array(uint8_array_t* array,
                                  size_t         index,
                                  uint8_t        value) {
    if (array == NULL) return NULL_POINTER;
    return push_at_array(&array->base, &value, index, UINT8_TYPE);
}
// ================================================================================
// Get operation
// ================================================================================

error_code_t get_uint8_array_index(const uint8_array_t* array,
                                    size_t               index,
                                    uint8_t*             out) {
    if (array == NULL || out == NULL) return NULL_POINTER;
    return get_array_index(&array->base, index, out, UINT8_TYPE);
}
// ================================================================================
// Pop operations
// ================================================================================

error_code_t pop_back_uint8_array(uint8_array_t* array, uint8_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_back_array(&array->base, out, UINT8_TYPE);
}
// --------------------------------------------------------------------------------

error_code_t pop_front_uint8_array(uint8_array_t* array, uint8_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_front_array(&array->base, out, UINT8_TYPE);
}
// --------------------------------------------------------------------------------

error_code_t pop_any_uint8_array(uint8_array_t* array,
                                  uint8_t*       out,
                                  size_t         index) {
    if (array == NULL) return NULL_POINTER;
    return pop_any_array(&array->base, out, index, UINT8_TYPE);
}
// ================================================================================
// Utility operations
// ================================================================================

error_code_t clear_uint8_array(uint8_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return clear_array(&array->base);
}
// --------------------------------------------------------------------------------

error_code_t set_uint8_array_index(uint8_array_t* array,
                                    size_t         index,
                                    uint8_t        value) {
    if (array == NULL) return NULL_POINTER;
    return set_array_index(&array->base, index, &value, UINT8_TYPE);
}
// --------------------------------------------------------------------------------

uint8_array_expect_t copy_uint8_array(const uint8_array_t* src,
                                       allocator_vtable_t   alloc_v) {
    if (src == NULL) {
        return (uint8_array_expect_t){ .has_value = false,
                                       .u.error   = NULL_POINTER };
    }
    return _wrap_expect(copy_array(&src->base, alloc_v));
}
// --------------------------------------------------------------------------------

error_code_t concat_uint8_array(uint8_array_t*       dst,
                                 const uint8_array_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_array(&dst->base, &src->base);
}
// --------------------------------------------------------------------------------

uint8_array_expect_t slice_uint8_array(const uint8_array_t* src,
                                        size_t               start,
                                        size_t               end,
                                        allocator_vtable_t   alloc_v) {
    if (src == NULL) {
        return (uint8_array_expect_t){ .has_value = false,
                                       .u.error   = NULL_POINTER };
    }
    return _wrap_expect(slice_array(&src->base, start, end, alloc_v));
}
// --------------------------------------------------------------------------------

error_code_t reverse_uint8_array(uint8_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return reverse_array(&array->base);
}
// ================================================================================
// Search
// ================================================================================

size_expect_t uint8_array_contains(const uint8_array_t* array,
                                    uint8_t              value,
                                    size_t               start,
                                    size_t               end) {
    if (array == NULL) {
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }
    return array_contains(&array->base, &value, start, end, UINT8_TYPE);
}
// ================================================================================
// Introspection
// ================================================================================

size_t uint8_array_size(const uint8_array_t* array) {
    if (array == NULL) return 0u;
    return array_size(&array->base);
}
// --------------------------------------------------------------------------------

size_t uint8_array_alloc(const uint8_array_t* array) {
    if (array == NULL) return 0u;
    return array_alloc(&array->base);
}
// --------------------------------------------------------------------------------

size_t uint8_array_data_size(const uint8_array_t* array) {
    if (array == NULL) return 0u;
    return array_data_size(&array->base);
}
// --------------------------------------------------------------------------------

bool is_uint8_array_empty(const uint8_array_t* array) {
    if (array == NULL) return true;
    return is_array_empty(&array->base);
}
// --------------------------------------------------------------------------------

bool is_uint8_array_full(const uint8_array_t* array) {
    if (array == NULL) return true;
    return is_array_full(&array->base);
}
// --------------------------------------------------------------------------------

bool is_uint8_array_ptr(const uint8_array_t* array, const uint8_t* ptr) {
    if (array == NULL) return false;
    return is_array_ptr(&array->base, ptr);
}
// ================================================================================
// ================================================================================
// eof
