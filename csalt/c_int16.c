// ================================================================================
// ================================================================================
// - File:    c_int16.c
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

#include "c_int16.h"
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Internal helper: unwrap a raw array_expect_t into a int16_array_expect_t.
// The array_t* stored inside the array_expect_t is reinterpreted as the base
// of a int16_array_t since int16_array_t contains array_t as its first member.
// ================================================================================

static inline int16_array_expect_t _wrap_expect(array_expect_t e) {
    if (e.has_value) {
        return (int16_array_expect_t){
            .has_value = true,
            .u.value   = (int16_array_t*)e.u.value
        };
    }
    return (int16_array_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

int16_array_expect_t init_int16_array(size_t             capacity,
                                         bool               growth,
                                         allocator_vtable_t alloc_v) {
    return _wrap_expect(init_array(capacity, INT16_TYPE, growth, alloc_v));
}

// --------------------------------------------------------------------------------

void return_int16_array(int16_array_t* array) {
    if (array == NULL) return;
    return_array(&array->base);
}

// ================================================================================
// Push operations
// ================================================================================

error_code_t push_back_int16_array(int16_array_t* array, int16_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_back_array(&array->base, &value, INT16_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_front_int16_array(int16_array_t* array, int16_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_front_array(&array->base, &value, INT16_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_at_int16_array(int16_array_t* array,
                                   size_t          index,
                                   int16_t        value) {
    if (array == NULL) return NULL_POINTER;
    return push_at_array(&array->base, &value, index, INT16_TYPE);
}

// ================================================================================
// Get operation
// ================================================================================

error_code_t get_int16_array_index(const int16_array_t* array,
                                     size_t                index,
                                     int16_t*             out) {
    if (array == NULL || out == NULL) return NULL_POINTER;
    return get_array_index(&array->base, index, out, INT16_TYPE);
}

// ================================================================================
// Pop operations
// ================================================================================

error_code_t pop_back_int16_array(int16_array_t* array, int16_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_back_array(&array->base, out, INT16_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_front_int16_array(int16_array_t* array, int16_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_front_array(&array->base, out, INT16_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_any_int16_array(int16_array_t* array,
                                   int16_t*       out,
                                   size_t          index) {
    if (array == NULL) return NULL_POINTER;
    return pop_any_array(&array->base, out, index, INT16_TYPE);
}

// ================================================================================
// Utility operations
// ================================================================================

error_code_t clear_int16_array(int16_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return clear_array(&array->base);
}

// --------------------------------------------------------------------------------

error_code_t set_int16_array_index(int16_array_t* array,
                                     size_t          index,
                                     int16_t        value) {
    if (array == NULL) return NULL_POINTER;
    return set_array_index(&array->base, index, &value, INT16_TYPE);
}

// --------------------------------------------------------------------------------

int16_array_expect_t copy_int16_array(const int16_array_t* src,
                                         allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (int16_array_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };
    }
    return _wrap_expect(copy_array(&src->base, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t concat_int16_array(int16_array_t*       dst,
                                  const int16_array_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_array(&dst->base, &src->base);
}

// --------------------------------------------------------------------------------

int16_array_expect_t slice_int16_array(const int16_array_t* src,
                                          size_t                start,
                                          size_t                end,
                                          allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (int16_array_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };
    }
    return _wrap_expect(slice_array(&src->base, start, end, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t reverse_int16_array(int16_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return reverse_array(&array->base);
}

// --------------------------------------------------------------------------------

/*
 * Comparator for int16_t. Both operands are widened to int before subtraction,
 * so the result fits without overflow on any platform where int is at least
 * 17 bits (i.e. every conforming C implementation in practice). The full
 * signed range [-32768, 32767] produces a difference in [-65535, 65535],
 * which is representable as int on all 32- and 64-bit platforms.
 */
static int _cmp_int16(const void* a, const void* b) {
    return (int)(*(const int16_t*)a) - (int)(*(const int16_t*)b);
}

error_code_t sort_int16_array(int16_array_t* array, direction_t dir) {
    if (array == NULL) return NULL_POINTER;
    return sort_array(&array->base, _cmp_int16, dir);
}

// ================================================================================
// Search
// ================================================================================

size_expect_t int16_array_contains(const int16_array_t* array,
                                     int16_t              value,
                                     size_t                start,
                                     size_t                end) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return array_contains(&array->base, &value, start, end, INT16_TYPE);
}

// --------------------------------------------------------------------------------

size_expect_t int16_array_binary_search(int16_array_t* array,
                                          int16_t        value,
                                          bool            sort) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_search_array(&array->base, &value, _cmp_int16, sort,
                               INT16_TYPE);
}

// --------------------------------------------------------------------------------

bracket_expect_t int16_array_binary_bracket(int16_array_t* array,
                                              int16_t        value,
                                              bool            sort) {
    if (array == NULL)
        return (bracket_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_bracket_array(&array->base, &value, _cmp_int16, sort,
                                INT16_TYPE);
}

// ================================================================================
// Introspection
// ================================================================================

size_t int16_array_size(const int16_array_t* array) {
    if (array == NULL) return 0u;
    return array_size(&array->base);
}

// --------------------------------------------------------------------------------

size_t int16_array_alloc(const int16_array_t* array) {
    if (array == NULL) return 0u;
    return array_alloc(&array->base);
}

// --------------------------------------------------------------------------------

size_t int16_array_data_size(const int16_array_t* array) {
    if (array == NULL) return 0u;
    return array_data_size(&array->base);
}

// --------------------------------------------------------------------------------

bool is_int16_array_empty(const int16_array_t* array) {
    if (array == NULL) return true;
    return is_array_empty(&array->base);
}

// --------------------------------------------------------------------------------

bool is_int16_array_full(const int16_array_t* array) {
    if (array == NULL) return true;
    return is_array_full(&array->base);
}

// --------------------------------------------------------------------------------

bool is_int16_array_ptr(const int16_array_t* array, const int16_t* ptr) {
    if (array == NULL) return false;
    return is_array_ptr(&array->base, ptr);
}
// ================================================================================
// ================================================================================
// eof
