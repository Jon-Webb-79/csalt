// ================================================================================
// ================================================================================
// - File:    c_uint32.c
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
#include <string.h>

#include "c_uint32.h"
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Internal helper: unwrap a raw array_expect_t into a uint32_array_expect_t.
// The array_t* stored inside the array_expect_t is reinterpreted as the base
// of a uint32_array_t since uint32_array_t contains array_t as its first member.
// ================================================================================

static inline uint32_array_expect_t _wrap_expect(array_expect_t e) {
    if (e.has_value) {
        return (uint32_array_expect_t){
            .has_value = true,
            .u.value   = (uint32_array_t*)e.u.value
        };
    }
    return (uint32_array_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

uint32_array_expect_t init_uint32_array(size_t             capacity,
                                         bool               growth,
                                         allocator_vtable_t alloc_v) {
    return _wrap_expect(init_array(capacity, UINT32_TYPE, growth, alloc_v));
}

// --------------------------------------------------------------------------------

void return_uint32_array(uint32_array_t* array) {
    if (array == NULL) return;
    return_array(&array->base);
}

// ================================================================================
// Push operations
// ================================================================================

error_code_t push_back_uint32_array(uint32_array_t* array, uint32_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_back_array(&array->base, &value, UINT32_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_front_uint32_array(uint32_array_t* array, uint32_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_front_array(&array->base, &value, UINT32_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_at_uint32_array(uint32_array_t* array,
                                   size_t          index,
                                   uint32_t        value) {
    if (array == NULL) return NULL_POINTER;
    return push_at_array(&array->base, &value, index, UINT32_TYPE);
}

// ================================================================================
// Get operation
// ================================================================================

error_code_t get_uint32_array_index(const uint32_array_t* array,
                                     size_t                index,
                                     uint32_t*             out) {
    if (array == NULL || out == NULL) return NULL_POINTER;
    return get_array_index(&array->base, index, out, UINT32_TYPE);
}

// ================================================================================
// Pop operations
// ================================================================================

error_code_t pop_back_uint32_array(uint32_array_t* array, uint32_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_back_array(&array->base, out, UINT32_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_front_uint32_array(uint32_array_t* array, uint32_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_front_array(&array->base, out, UINT32_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_any_uint32_array(uint32_array_t* array,
                                   uint32_t*       out,
                                   size_t          index) {
    if (array == NULL) return NULL_POINTER;
    return pop_any_array(&array->base, out, index, UINT32_TYPE);
}

// ================================================================================
// Utility operations
// ================================================================================

error_code_t clear_uint32_array(uint32_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return clear_array(&array->base);
}

// --------------------------------------------------------------------------------

error_code_t set_uint32_array_index(uint32_array_t* array,
                                     size_t          index,
                                     uint32_t        value) {
    if (array == NULL) return NULL_POINTER;
    return set_array_index(&array->base, index, &value, UINT32_TYPE);
}

// --------------------------------------------------------------------------------

uint32_array_expect_t copy_uint32_array(const uint32_array_t* src,
                                         allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (uint32_array_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };
    }
    return _wrap_expect(copy_array(&src->base, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t concat_uint32_array(uint32_array_t*       dst,
                                  const uint32_array_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_array(&dst->base, &src->base);
}

// --------------------------------------------------------------------------------

uint32_array_expect_t slice_uint32_array(const uint32_array_t* src,
                                          size_t                start,
                                          size_t                end,
                                          allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (uint32_array_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };
    }
    return _wrap_expect(slice_array(&src->base, start, end, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t reverse_uint32_array(uint32_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return reverse_array(&array->base);
}

// --------------------------------------------------------------------------------

/*
 * Comparator for uint32_t. Subtraction is unsafe: the maximum difference
 * between two uint32_t values is 4294967295, which exceeds INT_MAX on any
 * platform where int is 32 bits. Casting the result to int would produce the
 * wrong sign for large pairs such as (0, 2147483649). The three-way
 * comparison idiom is always correct regardless of platform.
 */
static int _cmp_uint32(const void* a, const void* b) {
    uint32_t va = *(const uint32_t*)a;
    uint32_t vb = *(const uint32_t*)b;
    return (va > vb) - (va < vb);
}

error_code_t sort_uint32_array(uint32_array_t* array, direction_t dir) {
    if (array == NULL) return NULL_POINTER;
    return sort_array(&array->base, _cmp_uint32, dir);
}

// ================================================================================
// Search
// ================================================================================

size_expect_t uint32_array_contains(const uint32_array_t* array,
                                     uint32_t              value,
                                     size_t                start,
                                     size_t                end) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return array_contains(&array->base, &value, start, end, UINT32_TYPE);
}

// --------------------------------------------------------------------------------

size_expect_t uint32_array_binary_search(uint32_array_t* array,
                                          uint32_t        value,
                                          bool            sort) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_search_array(&array->base, &value, _cmp_uint32, sort,
                               UINT32_TYPE);
}

// --------------------------------------------------------------------------------

bracket_expect_t uint32_array_binary_bracket(uint32_array_t* array,
                                              uint32_t        value,
                                              bool            sort) {
    if (array == NULL)
        return (bracket_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_bracket_array(&array->base, &value, _cmp_uint32, sort,
                                UINT32_TYPE);
}

// ================================================================================
// Introspection
// ================================================================================

size_t uint32_array_size(const uint32_array_t* array) {
    if (array == NULL) return 0u;
    return array_size(&array->base);
}

// --------------------------------------------------------------------------------

size_t uint32_array_alloc(const uint32_array_t* array) {
    if (array == NULL) return 0u;
    return array_alloc(&array->base);
}

// --------------------------------------------------------------------------------

size_t uint32_array_data_size(const uint32_array_t* array) {
    if (array == NULL) return 0u;
    return array_data_size(&array->base);
}

// --------------------------------------------------------------------------------

bool is_uint32_array_empty(const uint32_array_t* array) {
    if (array == NULL) return true;
    return is_array_empty(&array->base);
}

// --------------------------------------------------------------------------------

bool is_uint32_array_full(const uint32_array_t* array) {
    if (array == NULL) return true;
    return is_array_full(&array->base);
}

// --------------------------------------------------------------------------------

bool is_uint32_array_ptr(const uint32_array_t* array, const uint32_t* ptr) {
    if (array == NULL) return false;
    return is_array_ptr(&array->base, ptr);
}
// -------------------------------------------------------------------------------- 

uint32_expect_t uint32_array_min(const uint32_array_t* array) {
    if (array == NULL)
        return (uint32_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_min(&array->base, _cmp_uint32, UINT32_TYPE);
    if (!idx.has_value)
        return (uint32_expect_t){ .has_value = false, .u.error = idx.u.error };
    uint32_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, UINT32_TYPE);
    if (err != NO_ERROR)
        return (uint32_expect_t){ .has_value = false, .u.error = err };
    return (uint32_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

uint32_expect_t uint32_array_max(const uint32_array_t* array) {
    if (array == NULL)
        return (uint32_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_max(&array->base, _cmp_uint32, UINT32_TYPE);
    if (!idx.has_value)
        return (uint32_expect_t){ .has_value = false, .u.error = idx.u.error };
    uint32_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, UINT32_TYPE);
    if (err != NO_ERROR)
        return (uint32_expect_t){ .has_value = false, .u.error = err };
    return (uint32_expect_t){ .has_value = true, .u.value = val };
}
// -------------------------------------------------------------------------------- 

static void _add_uint32(void* accum, const void* element) {
    *(uint32_t*)accum += *(const uint32_t*)element;
}

uint32_expect_t uint32_array_sum(const uint32_array_t* array) {
    if (array == NULL)
        return (uint32_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    uint32_t total = 0;
    error_code_t err = array_sum(&array->base, &total, _add_uint32, UINT32_TYPE);
    if (err != NO_ERROR)
        return (uint32_expect_t){ .has_value = false, .u.error = err };
    return (uint32_expect_t){ .has_value = true, .u.value = total };
}
// -------------------------------------------------------------------------------- 

uint32_array_expect_t cumulative_uint32_array(const uint32_array_t* src,
                                              allocator_vtable_t    alloc) {
    if (src == NULL)
        return (uint32_array_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    array_expect_t r = cumulative_array(&src->base, _add_uint32, alloc, UINT32_TYPE);
    if (!r.has_value)
        return (uint32_array_expect_t){ .has_value = false, .u.error = r.u.error };
    return (uint32_array_expect_t){ .has_value = true,
                                    .u.value   = (uint32_array_t*)r.u.value };
}
// ================================================================================ 
// ================================================================================ 

static inline dict_key_t _key(const char* s, size_t len) {
    return (dict_key_t){ .data = s, .len = len };
}
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
uint32_dict_expect_t init_uint32_dict(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v) {
    dict_expect_t r = init_dict(capacity, sizeof(uint32_t),
                                UINT32_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (uint32_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (uint32_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
void return_uint32_dict(uint32_dict_t* dict) {
    return_dict(dict);
}
 
// ================================================================================
// Insert
// ================================================================================
 
error_code_t insert_uint32_dict(uint32_dict_t*     dict,
                                 const char*        key,
                                 uint32_t           value,
                                 allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    return insert_dict(dict, _key(key, strlen(key)), &value, alloc_v);
}
 
// --------------------------------------------------------------------------------
 
error_code_t insert_uint32_dict_n(uint32_dict_t*     dict,
                                   const char*        key,
                                   size_t             key_len,
                                   uint32_t           value,
                                   allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return insert_dict(dict, _key(key, key_len), &value, alloc_v);
}
 
// ================================================================================
// Pop
// ================================================================================
 
error_code_t pop_uint32_dict(uint32_dict_t* dict,
                              const char*    key,
                              uint32_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    return pop_dict(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t pop_uint32_dict_n(uint32_dict_t* dict,
                                const char*    key,
                                size_t         key_len,
                                uint32_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return pop_dict(dict, _key(key, key_len), out_value);
}
 
// ================================================================================
// Update
// ================================================================================
 
error_code_t update_uint32_dict(uint32_dict_t* dict,
                                 const char*    key,
                                 uint32_t       value) {
    if (key == NULL) return NULL_POINTER;
    return update_dict(dict, _key(key, strlen(key)), &value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t update_uint32_dict_n(uint32_dict_t* dict,
                                   const char*    key,
                                   size_t         key_len,
                                   uint32_t       value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return update_dict(dict, _key(key, key_len), &value);
}
 
// ================================================================================
// Lookup
// ================================================================================
 
error_code_t get_uint32_dict_value(const uint32_dict_t* dict,
                                    const char*          key,
                                    uint32_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    return get_dict_value(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t get_uint32_dict_value_n(const uint32_dict_t* dict,
                                      const char*          key,
                                      size_t               key_len,
                                      uint32_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return get_dict_value(dict, _key(key, key_len), out_value);
}
 
// --------------------------------------------------------------------------------
 
const uint32_t* get_uint32_dict_ptr(const uint32_dict_t* dict, const char* key) {
    if (key == NULL) return NULL;
    return (const uint32_t*)get_dict_value_ptr(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
const uint32_t* get_uint32_dict_ptr_n(const uint32_dict_t* dict,
                                       const char*          key,
                                       size_t               key_len) {
    if (key == NULL || key_len == 0u) return NULL;
    return (const uint32_t*)get_dict_value_ptr(dict, _key(key, key_len));
}
 
// --------------------------------------------------------------------------------
 
bool has_uint32_dict_key(const uint32_dict_t* dict, const char* key) {
    if (key == NULL) return false;
    return has_dict_key(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
bool has_uint32_dict_key_n(const uint32_dict_t* dict,
                            const char*          key,
                            size_t               key_len) {
    if (key == NULL || key_len == 0u) return false;
    return has_dict_key(dict, _key(key, key_len));
}
 
// ================================================================================
// Utility
// ================================================================================
 
error_code_t clear_uint32_dict(uint32_dict_t* dict) {
    return clear_dict(dict);
}
 
// --------------------------------------------------------------------------------
 
uint32_dict_expect_t copy_uint32_dict(const uint32_dict_t* src,
                                      allocator_vtable_t   alloc_v) {
    dict_expect_t r = copy_dict(src, alloc_v);
    if (!r.has_value)
        return (uint32_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (uint32_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
uint32_dict_expect_t merge_uint32_dict(const uint32_dict_t* a,
                                       const uint32_dict_t* b,
                                       bool                 overwrite,
                                       allocator_vtable_t   alloc_v) {
    dict_expect_t r = merge_dict(a, b, overwrite, alloc_v);
    if (!r.has_value)
        return (uint32_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (uint32_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// ================================================================================
// Iteration
// ================================================================================
 
typedef struct {
    uint32_dict_iter_fn typed_fn;
    void*               user_data;
} _uint32_iter_ctx_t;
 
static void _uint32_iter_shim(dict_entry_t e, void* ctx) {
    const _uint32_iter_ctx_t* c = (const _uint32_iter_ctx_t*)ctx;
    uint32_t value;
    memcpy(&value, e.value, sizeof(uint32_t));
    c->typed_fn((const char*)e.key, e.key_len, value, c->user_data);
}
 
error_code_t foreach_uint32_dict(const uint32_dict_t* dict,
                                  uint32_dict_iter_fn  fn,
                                  void*                user_data) {
    if (fn == NULL) return NULL_POINTER;
    _uint32_iter_ctx_t ctx = { .typed_fn = fn, .user_data = user_data };
    return foreach_dict(dict, _uint32_iter_shim, &ctx);
}
 
// ================================================================================
// Introspection
// ================================================================================
 
size_t uint32_dict_size(const uint32_dict_t* dict) {
    return dict_size(dict);
}
 
size_t uint32_dict_hash_size(const uint32_dict_t* dict) {
    return dict_hash_size(dict);
}
 
size_t uint32_dict_alloc(const uint32_dict_t* dict) {
    return dict_alloc(dict);
}
 
bool is_uint32_dict_empty(const uint32_dict_t* dict) {
    return is_dict_empty(dict);
}
// ================================================================================
// ================================================================================
// eof
