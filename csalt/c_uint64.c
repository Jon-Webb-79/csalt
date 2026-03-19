// ================================================================================
// ================================================================================
// - File:    c_uint64.c
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

#include <string.h>
#include "c_uint64.h"
// ================================================================================ 
// ================================================================================ 

static inline uint64_array_expect_t _wrap_expect(array_expect_t e) {
    if (e.has_value) {
        return (uint64_array_expect_t){
            .has_value = true,
            .u.value   = (uint64_array_t*)e.u.value
        };
    }
    return (uint64_array_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

uint64_array_expect_t init_uint64_array(size_t             capacity,
                                        bool               growth,
                                        allocator_vtable_t alloc_v) {
    return _wrap_expect(init_array(capacity, UINT64_TYPE, growth, alloc_v));
}

// --------------------------------------------------------------------------------

void return_uint64_array(uint64_array_t* array) {
    if (array == NULL) return;
    return_array(&array->base);
}

// ================================================================================
// Push operations
// ================================================================================

error_code_t push_back_uint64_array(uint64_array_t* array, uint64_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_back_array(&array->base, &value, UINT64_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_front_uint64_array(uint64_array_t* array, uint64_t value) {
    if (array == NULL) return NULL_POINTER;
    return push_front_array(&array->base, &value, UINT64_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_at_uint64_array(uint64_array_t* array,
                                  size_t          index,
                                  uint64_t        value) {
    if (array == NULL) return NULL_POINTER;
    return push_at_array(&array->base, &value, index, UINT64_TYPE);
}

// ================================================================================
// Get operation
// ================================================================================

error_code_t get_uint64_array_index(const uint64_array_t* array,
                                    size_t                index,
                                    uint64_t*             out) {
    if (array == NULL || out == NULL) return NULL_POINTER;
    return get_array_index(&array->base, index, out, UINT64_TYPE);
}

// ================================================================================
// Pop operations
// ================================================================================

error_code_t pop_back_uint64_array(uint64_array_t* array, uint64_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_back_array(&array->base, out, UINT64_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_front_uint64_array(uint64_array_t* array, uint64_t* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_front_array(&array->base, out, UINT64_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_any_uint64_array(uint64_array_t* array,
                                  uint64_t*       out,
                                  size_t          index) {
    if (array == NULL) return NULL_POINTER;
    return pop_any_array(&array->base, out, index, UINT64_TYPE);
}

// ================================================================================
// Utility operations
// ================================================================================

error_code_t clear_uint64_array(uint64_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return clear_array(&array->base);
}

// --------------------------------------------------------------------------------

error_code_t set_uint64_array_index(uint64_array_t* array,
                                    size_t          index,
                                    uint64_t        value) {
    if (array == NULL) return NULL_POINTER;
    return set_array_index(&array->base, index, &value, UINT64_TYPE);
}

// --------------------------------------------------------------------------------

uint64_array_expect_t copy_uint64_array(const uint64_array_t* src,
                                        allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (uint64_array_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };
    }
    return _wrap_expect(copy_array(&src->base, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t concat_uint64_array(uint64_array_t*       dst,
                                 const uint64_array_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_array(&dst->base, &src->base);
}

// --------------------------------------------------------------------------------

uint64_array_expect_t slice_uint64_array(const uint64_array_t* src,
                                         size_t                start,
                                         size_t                end,
                                         allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (uint64_array_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };
    }
    return _wrap_expect(slice_array(&src->base, start, end, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t reverse_uint64_array(uint64_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return reverse_array(&array->base);
}

// --------------------------------------------------------------------------------

/*
 * Comparator for uint64_t. Subtraction would overflow for large pairs such as
 * (0, 9223372036854775809). The three-way comparison idiom is always correct
 * regardless of the values involved.
 */
static int _cmp_uint64(const void* a, const void* b) {
    uint64_t va = *(const uint64_t*)a;
    uint64_t vb = *(const uint64_t*)b;
    return (va > vb) - (va < vb);
}

error_code_t sort_uint64_array(uint64_array_t* array, direction_t dir) {
    if (array == NULL) return NULL_POINTER;
    return sort_array(&array->base, _cmp_uint64, dir);
}

// ================================================================================
// Reduction
// ================================================================================

uint64_expect_t uint64_array_min(const uint64_array_t* array) {
    if (array == NULL)
        return (uint64_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_min(&array->base, _cmp_uint64, UINT64_TYPE);
    if (!idx.has_value)
        return (uint64_expect_t){ .has_value = false, .u.error = idx.u.error };
    uint64_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, UINT64_TYPE);
    if (err != NO_ERROR)
        return (uint64_expect_t){ .has_value = false, .u.error = err };
    return (uint64_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

uint64_expect_t uint64_array_max(const uint64_array_t* array) {
    if (array == NULL)
        return (uint64_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_max(&array->base, _cmp_uint64, UINT64_TYPE);
    if (!idx.has_value)
        return (uint64_expect_t){ .has_value = false, .u.error = idx.u.error };
    uint64_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, UINT64_TYPE);
    if (err != NO_ERROR)
        return (uint64_expect_t){ .has_value = false, .u.error = err };
    return (uint64_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

static void _add_uint64(void* accum, const void* element) {
    *(uint64_t*)accum += *(const uint64_t*)element;
}

uint64_expect_t uint64_array_sum(const uint64_array_t* array) {
    if (array == NULL)
        return (uint64_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    uint64_t total = 0;
    error_code_t err = array_sum(&array->base, &total, _add_uint64, UINT64_TYPE);
    if (err != NO_ERROR)
        return (uint64_expect_t){ .has_value = false, .u.error = err };
    return (uint64_expect_t){ .has_value = true, .u.value = total };
}

// --------------------------------------------------------------------------------

uint64_array_expect_t cumulative_uint64_array(const uint64_array_t* src,
                                              allocator_vtable_t    alloc) {
    if (src == NULL)
        return (uint64_array_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    array_expect_t r = cumulative_array(&src->base, _add_uint64, alloc, UINT64_TYPE);
    if (!r.has_value)
        return (uint64_array_expect_t){ .has_value = false, .u.error = r.u.error };
    return (uint64_array_expect_t){ .has_value = true,
                                    .u.value   = (uint64_array_t*)r.u.value };
}

// ================================================================================
// Search
// ================================================================================

size_expect_t uint64_array_contains(const uint64_array_t* array,
                                    uint64_t              value,
                                    size_t                start,
                                    size_t                end) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return array_contains(&array->base, &value, start, end, UINT64_TYPE);
}

// --------------------------------------------------------------------------------

size_expect_t uint64_array_binary_search(uint64_array_t* array,
                                         uint64_t        value,
                                         bool            sort) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_search_array(&array->base, &value, _cmp_uint64, sort,
                               UINT64_TYPE);
}

// --------------------------------------------------------------------------------

bracket_expect_t uint64_array_binary_bracket(uint64_array_t* array,
                                             uint64_t        value,
                                             bool            sort) {
    if (array == NULL)
        return (bracket_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_bracket_array(&array->base, &value, _cmp_uint64, sort,
                                UINT64_TYPE);
}

// ================================================================================
// Introspection
// ================================================================================

size_t uint64_array_size(const uint64_array_t* array) {
    if (array == NULL) return 0u;
    return array_size(&array->base);
}

// --------------------------------------------------------------------------------

size_t uint64_array_alloc(const uint64_array_t* array) {
    if (array == NULL) return 0u;
    return array_alloc(&array->base);
}

// --------------------------------------------------------------------------------

size_t uint64_array_data_size(const uint64_array_t* array) {
    if (array == NULL) return 0u;
    return array_data_size(&array->base);
}

// --------------------------------------------------------------------------------

bool is_uint64_array_empty(const uint64_array_t* array) {
    if (array == NULL) return true;
    return is_array_empty(&array->base);
}

// --------------------------------------------------------------------------------

bool is_uint64_array_full(const uint64_array_t* array) {
    if (array == NULL) return true;
    return is_array_full(&array->base);
}

// --------------------------------------------------------------------------------

bool is_uint64_array_ptr(const uint64_array_t* array, const uint64_t* ptr) {
    if (array == NULL) return false;
    return is_array_ptr(&array->base, ptr);
}
// ================================================================================ 
// ================================================================================ 

static inline dict_key_t _key(const char* s, size_t len) {
    return (dict_key_t){ .data = s, .len = len };
}
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
uint64_dict_expect_t init_uint64_dict(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v) {
    dict_expect_t r = init_dict(capacity, sizeof(uint64_t),
                                UINT64_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (uint64_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (uint64_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
void return_uint64_dict(uint64_dict_t* dict) {
    return_dict(dict);
}
 
// ================================================================================
// Insert
// ================================================================================
 
error_code_t insert_uint64_dict(uint64_dict_t*     dict,
                                 const char*        key,
                                 uint64_t           value,
                                 allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    return insert_dict(dict, _key(key, strlen(key)), &value, alloc_v);
}
 
// --------------------------------------------------------------------------------
 
error_code_t insert_uint64_dict_n(uint64_dict_t*     dict,
                                   const char*        key,
                                   size_t             key_len,
                                   uint64_t           value,
                                   allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return insert_dict(dict, _key(key, key_len), &value, alloc_v);
}
 
// ================================================================================
// Pop
// ================================================================================
 
error_code_t pop_uint64_dict(uint64_dict_t* dict,
                              const char*    key,
                              uint64_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    return pop_dict(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t pop_uint64_dict_n(uint64_dict_t* dict,
                                const char*    key,
                                size_t         key_len,
                                uint64_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return pop_dict(dict, _key(key, key_len), out_value);
}
 
// ================================================================================
// Update
// ================================================================================
 
error_code_t update_uint64_dict(uint64_dict_t* dict,
                                 const char*    key,
                                 uint64_t       value) {
    if (key == NULL) return NULL_POINTER;
    return update_dict(dict, _key(key, strlen(key)), &value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t update_uint64_dict_n(uint64_dict_t* dict,
                                   const char*    key,
                                   size_t         key_len,
                                   uint64_t       value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return update_dict(dict, _key(key, key_len), &value);
}
 
// ================================================================================
// Lookup
// ================================================================================
 
error_code_t get_uint64_dict_value(const uint64_dict_t* dict,
                                    const char*          key,
                                    uint64_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    return get_dict_value(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t get_uint64_dict_value_n(const uint64_dict_t* dict,
                                      const char*          key,
                                      size_t               key_len,
                                      uint64_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return get_dict_value(dict, _key(key, key_len), out_value);
}
 
// --------------------------------------------------------------------------------
 
const uint64_t* get_uint64_dict_ptr(const uint64_dict_t* dict, const char* key) {
    if (key == NULL) return NULL;
    return (const uint64_t*)get_dict_value_ptr(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
const uint64_t* get_uint64_dict_ptr_n(const uint64_dict_t* dict,
                                       const char*          key,
                                       size_t               key_len) {
    if (key == NULL || key_len == 0u) return NULL;
    return (const uint64_t*)get_dict_value_ptr(dict, _key(key, key_len));
}
 
// --------------------------------------------------------------------------------
 
bool has_uint64_dict_key(const uint64_dict_t* dict, const char* key) {
    if (key == NULL) return false;
    return has_dict_key(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
bool has_uint64_dict_key_n(const uint64_dict_t* dict,
                            const char*          key,
                            size_t               key_len) {
    if (key == NULL || key_len == 0u) return false;
    return has_dict_key(dict, _key(key, key_len));
}
 
// ================================================================================
// Utility
// ================================================================================
 
error_code_t clear_uint64_dict(uint64_dict_t* dict) {
    return clear_dict(dict);
}
 
// --------------------------------------------------------------------------------
 
uint64_dict_expect_t copy_uint64_dict(const uint64_dict_t* src,
                                      allocator_vtable_t   alloc_v) {
    dict_expect_t r = copy_dict(src, alloc_v);
    if (!r.has_value)
        return (uint64_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (uint64_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
uint64_dict_expect_t merge_uint64_dict(const uint64_dict_t* a,
                                       const uint64_dict_t* b,
                                       bool                 overwrite,
                                       allocator_vtable_t   alloc_v) {
    dict_expect_t r = merge_dict(a, b, overwrite, alloc_v);
    if (!r.has_value)
        return (uint64_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (uint64_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// ================================================================================
// Iteration
// ================================================================================
 
typedef struct {
    uint64_dict_iter_fn typed_fn;
    void*               user_data;
} _uint64_iter_ctx_t;
 
static void _uint64_iter_shim(dict_entry_t e, void* ctx) {
    const _uint64_iter_ctx_t* c = (const _uint64_iter_ctx_t*)ctx;
    uint64_t value;
    memcpy(&value, e.value, sizeof(uint64_t));
    c->typed_fn((const char*)e.key, e.key_len, value, c->user_data);
}
 
error_code_t foreach_uint64_dict(const uint64_dict_t* dict,
                                  uint64_dict_iter_fn  fn,
                                  void*                user_data) {
    if (fn == NULL) return NULL_POINTER;
    _uint64_iter_ctx_t ctx = { .typed_fn = fn, .user_data = user_data };
    return foreach_dict(dict, _uint64_iter_shim, &ctx);
}
 
// ================================================================================
// Introspection
// ================================================================================
 
size_t uint64_dict_size(const uint64_dict_t* dict) {
    return dict_size(dict);
}
 
size_t uint64_dict_hash_size(const uint64_dict_t* dict) {
    return dict_hash_size(dict);
}
 
size_t uint64_dict_alloc(const uint64_dict_t* dict) {
    return dict_alloc(dict);
}
 
bool is_uint64_dict_empty(const uint64_dict_t* dict) {
    return is_dict_empty(dict);
}
// ================================================================================
// ================================================================================
// eof
