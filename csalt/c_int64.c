// ================================================================================
// ================================================================================
// - File:    c_int64.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 06, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_int64.h"
// ================================================================================ 
// ================================================================================ 

static inline int64_array_expect_t _wrap_expect(array_expect_t e) {
    if (e.has_value) {
        return (int64_array_expect_t){
            .has_value = true,
            .u.value   = (int64_array_t*)e.u.value
        };
    }
    return (int64_array_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

int64_array_expect_t init_int64_array(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v) {
    return _wrap_expect(init_array(capacity, INT64_TYPE, growth, alloc_v));
}

// --------------------------------------------------------------------------------

void return_int64_array(int64_array_t* array) {
    if (array == NULL) return;
    return_array(&array->base);
}

// ================================================================================
// Push operations
// ================================================================================

error_code_t push_back_int64_array(int64_array_t* array, int64_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_back_array(&array->base, &value, INT64_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_front_int64_array(int64_array_t* array, int64_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_front_array(&array->base, &value, INT64_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_at_int64_array(int64_array_t* array,
                                 size_t         index,
                                 int64_t        value) {
    if (array == NULL) return NULL_POINTER;
    return push_at_array(&array->base, &value, index, INT64_TYPE);
}

// ================================================================================
// Get operation
// ================================================================================

error_code_t get_int64_array_index(const int64_array_t* array,
                                   size_t               index,
                                   int64_t*             out) {
    if (array == NULL || out == NULL) return NULL_POINTER;
    return get_array_index(&array->base, index, out, INT64_TYPE);
}

// ================================================================================
// Pop operations
// ================================================================================

error_code_t pop_back_int64_array(int64_array_t* array, int64_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_back_array(&array->base, out, INT64_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_front_int64_array(int64_array_t* array, int64_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_front_array(&array->base, out, INT64_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_any_int64_array(int64_array_t* array,
                                 int64_t*       out,
                                 size_t         index) {
    if (array == NULL) return NULL_POINTER;
    return pop_any_array(&array->base, out, index, INT64_TYPE);
}

// ================================================================================
// Utility operations
// ================================================================================

error_code_t clear_int64_array(int64_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return clear_array(&array->base);
}

// --------------------------------------------------------------------------------

error_code_t set_int64_array_index(int64_array_t* array,
                                   size_t         index,
                                   int64_t        value) {
    if (array == NULL) return NULL_POINTER;
    return set_array_index(&array->base, index, &value, INT64_TYPE);
}

// --------------------------------------------------------------------------------

int64_array_expect_t copy_int64_array(const int64_array_t* src,
                                      allocator_vtable_t   alloc_v) {
    if (src == NULL) {
        return (int64_array_expect_t){ .has_value = false,
                                       .u.error   = NULL_POINTER };
    }
    return _wrap_expect(copy_array(&src->base, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t concat_int64_array(int64_array_t*       dst,
                                const int64_array_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_array(&dst->base, &src->base);
}

// --------------------------------------------------------------------------------

int64_array_expect_t slice_int64_array(const int64_array_t* src,
                                       size_t               start,
                                       size_t               end,
                                       allocator_vtable_t   alloc_v) {
    if (src == NULL) {
        return (int64_array_expect_t){ .has_value = false,
                                       .u.error   = NULL_POINTER };
    }
    return _wrap_expect(slice_array(&src->base, start, end, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t reverse_int64_array(int64_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return reverse_array(&array->base);
}

// --------------------------------------------------------------------------------

/*
 * Comparator for int64_t. Subtraction is unsafe: the difference of two
 * int64_t values can overflow. For example, INT64_MIN - 1 is undefined
 * behaviour for signed arithmetic, and (INT64_MIN - INT64_MAX) wraps to 1
 * on most implementations — producing the wrong sign. The three-way
 * comparison idiom is always correct regardless of platform.
 */
static int _cmp_int64(const void* a, const void* b) {
    int64_t va = *(const int64_t*)a;
    int64_t vb = *(const int64_t*)b;
    return (va > vb) - (va < vb);
}

error_code_t sort_int64_array(int64_array_t* array, direction_t dir) {
    if (array == NULL) return NULL_POINTER;
    return sort_array(&array->base, _cmp_int64, dir);
}

// ================================================================================
// Reduction
// ================================================================================

int64_expect_t int64_array_min(const int64_array_t* array) {
    if (array == NULL)
        return (int64_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_min(&array->base, _cmp_int64, INT64_TYPE);
    if (!idx.has_value)
        return (int64_expect_t){ .has_value = false, .u.error = idx.u.error };
    int64_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, INT64_TYPE);
    if (err != NO_ERROR)
        return (int64_expect_t){ .has_value = false, .u.error = err };
    return (int64_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

int64_expect_t int64_array_max(const int64_array_t* array) {
    if (array == NULL)
        return (int64_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_max(&array->base, _cmp_int64, INT64_TYPE);
    if (!idx.has_value)
        return (int64_expect_t){ .has_value = false, .u.error = idx.u.error };
    int64_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, INT64_TYPE);
    if (err != NO_ERROR)
        return (int64_expect_t){ .has_value = false, .u.error = err };
    return (int64_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

static void _add_int64(void* accum, const void* element) {
    *(int64_t*)accum += *(const int64_t*)element;
}

int64_expect_t int64_array_sum(const int64_array_t* array) {
    if (array == NULL)
        return (int64_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    int64_t total = 0;
    error_code_t err = array_sum(&array->base, &total, _add_int64, INT64_TYPE);
    if (err != NO_ERROR)
        return (int64_expect_t){ .has_value = false, .u.error = err };
    return (int64_expect_t){ .has_value = true, .u.value = total };
}

// --------------------------------------------------------------------------------

int64_array_expect_t cumulative_int64_array(const int64_array_t* src,
                                            allocator_vtable_t   alloc) {
    if (src == NULL)
        return (int64_array_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    array_expect_t r = cumulative_array(&src->base, _add_int64, alloc, INT64_TYPE);
    if (!r.has_value)
        return (int64_array_expect_t){ .has_value = false, .u.error = r.u.error };
    return (int64_array_expect_t){ .has_value = true,
                                   .u.value   = (int64_array_t*)r.u.value };
}

// ================================================================================
// Search
// ================================================================================

size_expect_t int64_array_contains(const int64_array_t* array,
                                   int64_t              value,
                                   size_t               start,
                                   size_t               end) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return array_contains(&array->base, &value, start, end, INT64_TYPE);
}

// --------------------------------------------------------------------------------

size_expect_t int64_array_binary_search(int64_array_t* array,
                                        int64_t        value,
                                        bool           sort) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_search_array(&array->base, &value, _cmp_int64, sort,
                               INT64_TYPE);
}

// --------------------------------------------------------------------------------

bracket_expect_t int64_array_binary_bracket(int64_array_t* array,
                                            int64_t        value,
                                            bool           sort) {
    if (array == NULL)
        return (bracket_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_bracket_array(&array->base, &value, _cmp_int64, sort,
                                INT64_TYPE);
}

// ================================================================================
// Introspection
// ================================================================================

size_t int64_array_size(const int64_array_t* array) {
    if (array == NULL) return 0u;
    return array_size(&array->base);
}

// --------------------------------------------------------------------------------

size_t int64_array_alloc(const int64_array_t* array) {
    if (array == NULL) return 0u;
    return array_alloc(&array->base);
}

// --------------------------------------------------------------------------------

size_t int64_array_data_size(const int64_array_t* array) {
    if (array == NULL) return 0u;
    return array_data_size(&array->base);
}

// --------------------------------------------------------------------------------

bool is_int64_array_empty(const int64_array_t* array) {
    if (array == NULL) return true;
    return is_array_empty(&array->base);
}

// --------------------------------------------------------------------------------

bool is_int64_array_full(const int64_array_t* array) {
    if (array == NULL) return true;
    return is_array_full(&array->base);
}

// --------------------------------------------------------------------------------

bool is_int64_array_ptr(const int64_array_t* array, const int64_t* ptr) {
    if (array == NULL) return false;
    return is_array_ptr(&array->base, ptr);
}
// ================================================================================
// ================================================================================
// eof
