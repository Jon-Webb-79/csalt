// ================================================================================
// ================================================================================
// - File:    c_int32.c
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

#include "c_int32.h"
// ================================================================================ 
// ================================================================================ 
// ================================================================================
// Internal helper: unwrap a raw array_expect_t into a int32_array_expect_t.
// The array_t* stored inside the array_expect_t is reinterpreted as the base
// of a int32_array_t since int32_array_t contains array_t as its first member.
// ================================================================================

static inline int32_array_expect_t _wrap_expect(array_expect_t e) {
    if (e.has_value) {
        return (int32_array_expect_t){
            .has_value = true,
            .u.value   = (int32_array_t*)e.u.value
        };
    }
    return (int32_array_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

int32_array_expect_t init_int32_array(size_t             capacity,
                                         bool               growth,
                                         allocator_vtable_t alloc_v) {
    return _wrap_expect(init_array(capacity, INT32_TYPE, growth, alloc_v));
}

// --------------------------------------------------------------------------------

void return_int32_array(int32_array_t* array) {
    if (array == NULL) return;
    return_array(&array->base);
}

// ================================================================================
// Push operations
// ================================================================================

error_code_t push_back_int32_array(int32_array_t* array, int32_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_back_array(&array->base, &value, INT32_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_front_int32_array(int32_array_t* array, int32_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_front_array(&array->base, &value, INT32_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_at_int32_array(int32_array_t* array,
                                   size_t          index,
                                   int32_t        value) {
    if (array == NULL) return NULL_POINTER;
    return push_at_array(&array->base, &value, index, INT32_TYPE);
}

// ================================================================================
// Get operation
// ================================================================================

error_code_t get_int32_array_index(const int32_array_t* array,
                                     size_t                index,
                                     int32_t*             out) {
    if (array == NULL || out == NULL) return NULL_POINTER;
    return get_array_index(&array->base, index, out, INT32_TYPE);
}

// ================================================================================
// Pop operations
// ================================================================================

error_code_t pop_back_int32_array(int32_array_t* array, int32_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_back_array(&array->base, out, INT32_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_front_int32_array(int32_array_t* array, int32_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_front_array(&array->base, out, INT32_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_any_int32_array(int32_array_t* array,
                                   int32_t*       out,
                                   size_t          index) {
    if (array == NULL) return NULL_POINTER;
    return pop_any_array(&array->base, out, index, INT32_TYPE);
}

// ================================================================================
// Utility operations
// ================================================================================

error_code_t clear_int32_array(int32_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return clear_array(&array->base);
}

// --------------------------------------------------------------------------------

error_code_t set_int32_array_index(int32_array_t* array,
                                     size_t          index,
                                     int32_t        value) {
    if (array == NULL) return NULL_POINTER;
    return set_array_index(&array->base, index, &value, INT32_TYPE);
}

// --------------------------------------------------------------------------------

int32_array_expect_t copy_int32_array(const int32_array_t* src,
                                         allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (int32_array_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };
    }
    return _wrap_expect(copy_array(&src->base, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t concat_int32_array(int32_array_t*       dst,
                                  const int32_array_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_array(&dst->base, &src->base);
}

// --------------------------------------------------------------------------------

int32_array_expect_t slice_int32_array(const int32_array_t* src,
                                          size_t                start,
                                          size_t                end,
                                          allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (int32_array_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };
    }
    return _wrap_expect(slice_array(&src->base, start, end, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t reverse_int32_array(int32_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return reverse_array(&array->base);
}

// --------------------------------------------------------------------------------

/*
 * Comparator for int32_t. Subtraction is unsafe: the difference of two
 * int32_t values can overflow a 32-bit int. For example,
 * INT32_MIN - 1 = -2147483649, which is undefined behaviour for signed
 * arithmetic, and (-2147483648) - (2147483647) wraps to 1 on most
 * implementations — producing the wrong sign. The three-way comparison
 * idiom is always correct regardless of platform.
 */
static int _cmp_int32(const void* a, const void* b) {
    int32_t va = *(const int32_t*)a;
    int32_t vb = *(const int32_t*)b;
    return (va > vb) - (va < vb);
}

error_code_t sort_int32_array(int32_array_t* array, direction_t dir) {
    if (array == NULL) return NULL_POINTER;
    return sort_array(&array->base, _cmp_int32, dir);
}

// ================================================================================
// Search
// ================================================================================

size_expect_t int32_array_contains(const int32_array_t* array,
                                     int32_t              value,
                                     size_t                start,
                                     size_t                end) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return array_contains(&array->base, &value, start, end, INT32_TYPE);
}

// --------------------------------------------------------------------------------

size_expect_t int32_array_binary_search(int32_array_t* array,
                                          int32_t        value,
                                          bool            sort) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_search_array(&array->base, &value, _cmp_int32, sort,
                               INT32_TYPE);
}

// --------------------------------------------------------------------------------

bracket_expect_t int32_array_binary_bracket(int32_array_t* array,
                                              int32_t        value,
                                              bool            sort) {
    if (array == NULL)
        return (bracket_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_bracket_array(&array->base, &value, _cmp_int32, sort,
                                INT32_TYPE);
}

// ================================================================================
// Introspection
// ================================================================================

size_t int32_array_size(const int32_array_t* array) {
    if (array == NULL) return 0u;
    return array_size(&array->base);
}

// --------------------------------------------------------------------------------

size_t int32_array_alloc(const int32_array_t* array) {
    if (array == NULL) return 0u;
    return array_alloc(&array->base);
}

// --------------------------------------------------------------------------------

size_t int32_array_data_size(const int32_array_t* array) {
    if (array == NULL) return 0u;
    return array_data_size(&array->base);
}

// --------------------------------------------------------------------------------

bool is_int32_array_empty(const int32_array_t* array) {
    if (array == NULL) return true;
    return is_array_empty(&array->base);
}

// --------------------------------------------------------------------------------

bool is_int32_array_full(const int32_array_t* array) {
    if (array == NULL) return true;
    return is_array_full(&array->base);
}

// --------------------------------------------------------------------------------

bool is_int32_array_ptr(const int32_array_t* array, const int32_t* ptr) {
    if (array == NULL) return false;
    return is_array_ptr(&array->base, ptr);
}
// ================================================================================
// ================================================================================
// eof
