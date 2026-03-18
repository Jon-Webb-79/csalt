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
// -------------------------------------------------------------------------------- 

static int _cmp_uint8(const void* a, const void* b) {
    return (int)(*(const uint8_t*)a) - (int)(*(const uint8_t*)b);
}

error_code_t sort_uint8_array(uint8_array_t* array, direction_t dir) {
    if (array == NULL) return NULL_POINTER;
    return sort_array(&array->base, _cmp_uint8, dir);
}
// -------------------------------------------------------------------------------- 

size_expect_t uint8_array_binary_search(uint8_array_t* array,
                                         uint8_t        value,
                                         bool           sort) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_search_array(&array->base, &value, _cmp_uint8, sort, UINT8_TYPE);
}

// --------------------------------------------------------------------------------

bracket_expect_t uint8_array_binary_bracket(uint8_array_t* array,
                                             uint8_t        value,
                                             bool           sort) {
    if (array == NULL)
        return (bracket_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_bracket_array(&array->base, &value, _cmp_uint8, sort, UINT8_TYPE);
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

uint8_expect_t uint8_array_min(const uint8_array_t* array) {
    if (array == NULL)
        return (uint8_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_min(&array->base, _cmp_uint8, UINT8_TYPE);
    if (!idx.has_value)
        return (uint8_expect_t){ .has_value = false, .u.error = idx.u.error };
    uint8_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, UINT8_TYPE);
    if (err != NO_ERROR)
        return (uint8_expect_t){ .has_value = false, .u.error = err };
    return (uint8_expect_t){ .has_value = true, .u.value = val };
}
// --------------------------------------------------------------------------------

uint8_expect_t uint8_array_max(const uint8_array_t* array) {
    if (array == NULL)
        return (uint8_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_max(&array->base, _cmp_uint8, UINT8_TYPE);
    if (!idx.has_value)
        return (uint8_expect_t){ .has_value = false, .u.error = idx.u.error };
    uint8_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, UINT8_TYPE);
    if (err != NO_ERROR)
        return (uint8_expect_t){ .has_value = false, .u.error = err };
    return (uint8_expect_t){ .has_value = true, .u.value = val };
}
// ================================================================================ 
// ================================================================================ 

/*
 * Inline helper used by every function in this file.  Avoids repeating the
 * compound-literal construction at every call site.
 */
static inline dict_key_t _key(const char* s, size_t len) {
    return (dict_key_t){ .data = s, .len = len };
}
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
uint8_dict_expect_t init_uint8_dict(size_t             capacity,
                                    bool               growth,
                                    allocator_vtable_t alloc_v) {
    dict_expect_t r = init_dict(capacity, sizeof(uint8_t),
                                UINT8_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (uint8_dict_expect_t){ .has_value = false,
                                      .u.error   = r.u.error };
 
    return (uint8_dict_expect_t){ .has_value = true,
                                   .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
void return_uint8_dict(uint8_dict_t* dict) {
    return_dict(dict);
}
 
// ================================================================================
// Insert
// ================================================================================
 
error_code_t insert_uint8_dict(uint8_dict_t*      dict,
                                const char*        key,
                                uint8_t            value,
                                allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    return insert_dict(dict, _key(key, strlen(key)), &value, alloc_v);
}
 
// --------------------------------------------------------------------------------
 
error_code_t insert_uint8_dict_n(uint8_dict_t*      dict,
                                  const char*        key,
                                  size_t             key_len,
                                  uint8_t            value,
                                  allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return insert_dict(dict, _key(key, key_len), &value, alloc_v);
}
 
// ================================================================================
// Pop
// ================================================================================
 
error_code_t pop_uint8_dict(uint8_dict_t* dict,
                             const char*   key,
                             uint8_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    return pop_dict(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t pop_uint8_dict_n(uint8_dict_t* dict,
                               const char*   key,
                               size_t        key_len,
                               uint8_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return pop_dict(dict, _key(key, key_len), out_value);
}
 
// ================================================================================
// Update
// ================================================================================
 
error_code_t update_uint8_dict(uint8_dict_t* dict,
                                const char*   key,
                                uint8_t       value) {
    if (key == NULL) return NULL_POINTER;
    return update_dict(dict, _key(key, strlen(key)), &value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t update_uint8_dict_n(uint8_dict_t* dict,
                                  const char*   key,
                                  size_t        key_len,
                                  uint8_t       value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return update_dict(dict, _key(key, key_len), &value);
}
 
// ================================================================================
// Lookup
// ================================================================================
 
error_code_t get_uint8_dict_value(const uint8_dict_t* dict,
                                   const char*         key,
                                   uint8_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    return get_dict_value(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t get_uint8_dict_value_n(const uint8_dict_t* dict,
                                     const char*         key,
                                     size_t              key_len,
                                     uint8_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return get_dict_value(dict, _key(key, key_len), out_value);
}
 
// --------------------------------------------------------------------------------
 
const uint8_t* get_uint8_dict_ptr(const uint8_dict_t* dict, const char* key) {
    if (key == NULL) return NULL;
    return (const uint8_t*)get_dict_value_ptr(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
const uint8_t* get_uint8_dict_ptr_n(const uint8_dict_t* dict,
                                     const char*         key,
                                     size_t              key_len) {
    if (key == NULL || key_len == 0u) return NULL;
    return (const uint8_t*)get_dict_value_ptr(dict, _key(key, key_len));
}
 
// --------------------------------------------------------------------------------
 
bool has_uint8_dict_key(const uint8_dict_t* dict, const char* key) {
    if (key == NULL) return false;
    return has_dict_key(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
bool has_uint8_dict_key_n(const uint8_dict_t* dict,
                           const char*         key,
                           size_t              key_len) {
    if (key == NULL || key_len == 0u) return false;
    return has_dict_key(dict, _key(key, key_len));
}
 
// ================================================================================
// Utility
// ================================================================================
 
error_code_t clear_uint8_dict(uint8_dict_t* dict) {
    return clear_dict(dict);
}
 
// --------------------------------------------------------------------------------
 
uint8_dict_expect_t copy_uint8_dict(const uint8_dict_t* src,
                                    allocator_vtable_t  alloc_v) {
    dict_expect_t r = copy_dict(src, alloc_v);
    if (!r.has_value)
        return (uint8_dict_expect_t){ .has_value = false,
                                      .u.error   = r.u.error };
 
    return (uint8_dict_expect_t){ .has_value = true,
                                   .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
uint8_dict_expect_t merge_uint8_dict(const uint8_dict_t* a,
                                     const uint8_dict_t* b,
                                     bool                overwrite,
                                     allocator_vtable_t  alloc_v) {
    dict_expect_t r = merge_dict(a, b, overwrite, alloc_v);
    if (!r.has_value)
        return (uint8_dict_expect_t){ .has_value = false,
                                      .u.error   = r.u.error };
 
    return (uint8_dict_expect_t){ .has_value = true,
                                   .u.value   = r.u.value };
}
 
// ================================================================================
// Iteration — adapts the generic dict_iter_fn to uint8_dict_iter_fn
// ================================================================================
 
/*
 * Thin shim stored on the stack inside foreach_uint8_dict.
 * Holds the typed callback and the original user_data so the generic
 * foreach_dict call can reach them through a single void* context.
 */
typedef struct {
    uint8_dict_iter_fn typed_fn;
    void*              user_data;
} _uint8_iter_ctx_t;
 
static void _uint8_iter_shim(dict_entry_t e, void* ctx) {
    const _uint8_iter_ctx_t* c = (const _uint8_iter_ctx_t*)ctx;
    uint8_t value;
    memcpy(&value, e.value, sizeof(uint8_t));
    c->typed_fn((const char*)e.key, e.key_len, value, c->user_data);
}
 
error_code_t foreach_uint8_dict(const uint8_dict_t* dict,
                                 uint8_dict_iter_fn  fn,
                                 void*               user_data) {
    if (fn == NULL) return NULL_POINTER;
    _uint8_iter_ctx_t ctx = { .typed_fn = fn, .user_data = user_data };
    return foreach_dict(dict, _uint8_iter_shim, &ctx);
}
 
// ================================================================================
// Introspection
// ================================================================================
 
size_t uint8_dict_size(const uint8_dict_t* dict) {
    return dict_size(dict);
}
 
size_t uint8_dict_hash_size(const uint8_dict_t* dict) {
    return dict_hash_size(dict);
}
 
size_t uint8_dict_alloc(const uint8_dict_t* dict) {
    return dict_alloc(dict);
}
 
bool is_uint8_dict_empty(const uint8_dict_t* dict) {
    return is_dict_empty(dict);
}
// ================================================================================
// ================================================================================
// eof
