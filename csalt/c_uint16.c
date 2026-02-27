// ================================================================================
// ================================================================================
// - File:    c_uint16.c
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

#include "c_uint16.h"
// ================================================================================ 
// ================================================================================ 

static inline uint16_array_expect_t _wrap_expect(array_expect_t e) {
    if (e.has_value) {
        return (uint16_array_expect_t){
            .has_value = true,
            .u.value   = (uint16_array_t*)e.u.value
        };
    }
    return (uint16_array_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

uint16_array_expect_t init_uint16_array(size_t             capacity,
                                         bool               growth,
                                         allocator_vtable_t alloc_v) {
    return _wrap_expect(init_array(capacity, UINT16_TYPE, growth, alloc_v));
}

// --------------------------------------------------------------------------------

void return_uint16_array(uint16_array_t* array) {
    if (array == NULL) return;
    return_array(&array->base);
}

// ================================================================================
// Push operations
// ================================================================================

error_code_t push_back_uint16_array(uint16_array_t* array, uint16_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_back_array(&array->base, &value, UINT16_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_front_uint16_array(uint16_array_t* array, uint16_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_front_array(&array->base, &value, UINT16_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_at_uint16_array(uint16_array_t* array,
                                   size_t          index,
                                   uint16_t        value) {
    if (array == NULL) return NULL_POINTER;
    return push_at_array(&array->base, &value, index, UINT16_TYPE);
}

// ================================================================================
// Get operation
// ================================================================================

error_code_t get_uint16_array_index(const uint16_array_t* array,
                                     size_t                index,
                                     uint16_t*             out) {
    if (array == NULL || out == NULL) return NULL_POINTER;
    return get_array_index(&array->base, index, out, UINT16_TYPE);
}

// ================================================================================
// Pop operations
// ================================================================================

error_code_t pop_back_uint16_array(uint16_array_t* array, uint16_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_back_array(&array->base, out, UINT16_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_front_uint16_array(uint16_array_t* array, uint16_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_front_array(&array->base, out, UINT16_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_any_uint16_array(uint16_array_t* array,
                                   uint16_t*       out,
                                   size_t          index) {
    if (array == NULL) return NULL_POINTER;
    return pop_any_array(&array->base, out, index, UINT16_TYPE);
}

// ================================================================================
// Utility operations
// ================================================================================

error_code_t clear_uint16_array(uint16_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return clear_array(&array->base);
}

// --------------------------------------------------------------------------------

error_code_t set_uint16_array_index(uint16_array_t* array,
                                     size_t          index,
                                     uint16_t        value) {
    if (array == NULL) return NULL_POINTER;
    return set_array_index(&array->base, index, &value, UINT16_TYPE);
}

// --------------------------------------------------------------------------------

uint16_array_expect_t copy_uint16_array(const uint16_array_t* src,
                                         allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (uint16_array_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };
    }
    return _wrap_expect(copy_array(&src->base, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t concat_uint16_array(uint16_array_t*       dst,
                                  const uint16_array_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_array(&dst->base, &src->base);
}

// --------------------------------------------------------------------------------

uint16_array_expect_t slice_uint16_array(const uint16_array_t* src,
                                          size_t                start,
                                          size_t                end,
                                          allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (uint16_array_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };
    }
    return _wrap_expect(slice_array(&src->base, start, end, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t reverse_uint16_array(uint16_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return reverse_array(&array->base);
}

// --------------------------------------------------------------------------------

/*
 * Comparator for uint16_t. Subtraction cannot be used safely here because
 * the difference of two uint16_t values can exceed INT_MAX when widened to
 * int on platforms where int is 16 bits, and can produce the wrong sign when
 * the magnitude of the difference exceeds INT_MAX on 32-bit platforms if the
 * values were near UINT16_MAX. An explicit three-way comparison is always
 * correct regardless of platform.
 */
static int _cmp_uint16(const void* a, const void* b) {
    uint16_t va = *(const uint16_t*)a;
    uint16_t vb = *(const uint16_t*)b;
    return (va > vb) - (va < vb);
}

error_code_t sort_uint16_array(uint16_array_t* array, direction_t dir) {
    if (array == NULL) return NULL_POINTER;
    return sort_array(&array->base, _cmp_uint16, dir);
}

// ================================================================================
// Search
// ================================================================================

size_expect_t uint16_array_contains(const uint16_array_t* array,
                                     uint16_t              value,
                                     size_t                start,
                                     size_t                end) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return array_contains(&array->base, &value, start, end, UINT16_TYPE);
}

// --------------------------------------------------------------------------------

size_expect_t uint16_array_binary_search(uint16_array_t* array,
                                          uint16_t        value,
                                          bool            sort) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_search_array(&array->base, &value, _cmp_uint16, sort,
                               UINT16_TYPE);
}

// --------------------------------------------------------------------------------

bracket_expect_t uint16_array_binary_bracket(uint16_array_t* array,
                                              uint16_t        value,
                                              bool            sort) {
    if (array == NULL)
        return (bracket_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_bracket_array(&array->base, &value, _cmp_uint16, sort,
                                UINT16_TYPE);
}

// ================================================================================
// Introspection
// ================================================================================

size_t uint16_array_size(const uint16_array_t* array) {
    if (array == NULL) return 0u;
    return array_size(&array->base);
}

// --------------------------------------------------------------------------------

size_t uint16_array_alloc(const uint16_array_t* array) {
    if (array == NULL) return 0u;
    return array_alloc(&array->base);
}

// --------------------------------------------------------------------------------

size_t uint16_array_data_size(const uint16_array_t* array) {
    if (array == NULL) return 0u;
    return array_data_size(&array->base);
}

// --------------------------------------------------------------------------------

bool is_uint16_array_empty(const uint16_array_t* array) {
    if (array == NULL) return true;
    return is_array_empty(&array->base);
}

// --------------------------------------------------------------------------------

bool is_uint16_array_full(const uint16_array_t* array) {
    if (array == NULL) return true;
    return is_array_full(&array->base);
}

// --------------------------------------------------------------------------------

bool is_uint16_array_ptr(const uint16_array_t* array, const uint16_t* ptr) {
    if (array == NULL) return false;
    return is_array_ptr(&array->base, ptr);
}
// ================================================================================
// ================================================================================
// eof
