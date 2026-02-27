// ================================================================================
// ================================================================================
// - File:    c_int8.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    February 27, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_int8.h"
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Internal helper: unwrap a raw array_expect_t into a int8_array_expect_t.
// The array_t* stored inside the array_expect_t is reinterpreted as the base
// of a int8_array_t since int8_array_t contains array_t as its first member.
// ================================================================================

static inline int8_array_expect_t _wrap_expect(array_expect_t e) {
    if (e.has_value) {
        return (int8_array_expect_t){
            .has_value = true,
            .u.value   = (int8_array_t*)e.u.value
        };
    }
    return (int8_array_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

int8_array_expect_t init_int8_array(size_t             capacity,
                                     bool               growth,
                                     allocator_vtable_t alloc_v) {
    return _wrap_expect(init_array(capacity, INT8_TYPE, growth, alloc_v));
}

// --------------------------------------------------------------------------------

void return_int8_array(int8_array_t* array) {
    if (array == NULL) return;
    return_array(&array->base);
}

// ================================================================================
// Push operations
// ================================================================================

error_code_t push_back_int8_array(int8_array_t* array, int8_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_back_array(&array->base, &value, INT8_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_front_int8_array(int8_array_t* array, int8_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_front_array(&array->base, &value, INT8_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_at_int8_array(int8_array_t* array,
                                 size_t        index,
                                 int8_t        value) {
    if (array == NULL) return NULL_POINTER;
    return push_at_array(&array->base, &value, index, INT8_TYPE);
}

// ================================================================================
// Get operation
// ================================================================================

error_code_t get_int8_array_index(const int8_array_t* array,
                                   size_t              index,
                                   int8_t*             out) {
    if (array == NULL || out == NULL) return NULL_POINTER;
    return get_array_index(&array->base, index, out, INT8_TYPE);
}

// ================================================================================
// Pop operations
// ================================================================================

error_code_t pop_back_int8_array(int8_array_t* array, int8_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_back_array(&array->base, out, INT8_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_front_int8_array(int8_array_t* array, int8_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_front_array(&array->base, out, INT8_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_any_int8_array(int8_array_t* array,
                                 int8_t*       out,
                                 size_t        index) {
    if (array == NULL) return NULL_POINTER;
    return pop_any_array(&array->base, out, index, INT8_TYPE);
}

// ================================================================================
// Utility operations
// ================================================================================

error_code_t clear_int8_array(int8_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return clear_array(&array->base);
}

// --------------------------------------------------------------------------------

error_code_t set_int8_array_index(int8_array_t* array,
                                   size_t        index,
                                   int8_t        value) {
    if (array == NULL) return NULL_POINTER;
    return set_array_index(&array->base, index, &value, INT8_TYPE);
}

// --------------------------------------------------------------------------------

int8_array_expect_t copy_int8_array(const int8_array_t* src,
                                     allocator_vtable_t  alloc_v) {
    if (src == NULL) {
        return (int8_array_expect_t){ .has_value = false,
                                      .u.error   = NULL_POINTER };
    }
    return _wrap_expect(copy_array(&src->base, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t concat_int8_array(int8_array_t*       dst,
                                const int8_array_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_array(&dst->base, &src->base);
}

// --------------------------------------------------------------------------------

int8_array_expect_t slice_int8_array(const int8_array_t* src,
                                      size_t              start,
                                      size_t              end,
                                      allocator_vtable_t  alloc_v) {
    if (src == NULL) {
        return (int8_array_expect_t){ .has_value = false,
                                      .u.error   = NULL_POINTER };
    }
    return _wrap_expect(slice_array(&src->base, start, end, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t reverse_int8_array(int8_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return reverse_array(&array->base);
}

// --------------------------------------------------------------------------------

static int _cmp_int8(const void* a, const void* b) {
    return (int)(*(const int8_t*)a) - (int)(*(const int8_t*)b);
}

error_code_t sort_int8_array(int8_array_t* array, direction_t dir) {
    if (array == NULL) return NULL_POINTER;
    return sort_array(&array->base, _cmp_int8, dir);
}

// ================================================================================
// Search
// ================================================================================

size_expect_t int8_array_contains(const int8_array_t* array,
                                   int8_t              value,
                                   size_t              start,
                                   size_t              end) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return array_contains(&array->base, &value, start, end, INT8_TYPE);
}

// --------------------------------------------------------------------------------

size_expect_t int8_array_binary_search(int8_array_t* array,
                                        int8_t        value,
                                        bool          sort) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_search_array(&array->base, &value, _cmp_int8, sort, INT8_TYPE);
}

// --------------------------------------------------------------------------------

bracket_expect_t int8_array_binary_bracket(int8_array_t* array,
                                            int8_t        value,
                                            bool          sort) {
    if (array == NULL)
        return (bracket_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_bracket_array(&array->base, &value, _cmp_int8, sort, INT8_TYPE);
}

// ================================================================================
// Introspection
// ================================================================================

size_t int8_array_size(const int8_array_t* array) {
    if (array == NULL) return 0u;
    return array_size(&array->base);
}

// --------------------------------------------------------------------------------

size_t int8_array_alloc(const int8_array_t* array) {
    if (array == NULL) return 0u;
    return array_alloc(&array->base);
}

// --------------------------------------------------------------------------------

size_t int8_array_data_size(const int8_array_t* array) {
    if (array == NULL) return 0u;
    return array_data_size(&array->base);
}

// --------------------------------------------------------------------------------

bool is_int8_array_empty(const int8_array_t* array) {
    if (array == NULL) return true;
    return is_array_empty(&array->base);
}

// --------------------------------------------------------------------------------

bool is_int8_array_full(const int8_array_t* array) {
    if (array == NULL) return true;
    return is_array_full(&array->base);
}

// --------------------------------------------------------------------------------

bool is_int8_array_ptr(const int8_array_t* array, const int8_t* ptr) {
    if (array == NULL) return false;
    return is_array_ptr(&array->base, ptr);
}
// ================================================================================
// ================================================================================
// eof
