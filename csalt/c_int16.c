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

#include <string.h>

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
// -------------------------------------------------------------------------------- 

int16_expect_t int16_array_min(const int16_array_t* array) {
    if (array == NULL)
        return (int16_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_min(&array->base, _cmp_int16, INT16_TYPE);
    if (!idx.has_value)
        return (int16_expect_t){ .has_value = false, .u.error = idx.u.error };
    int16_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, INT16_TYPE);
    if (err != NO_ERROR)
        return (int16_expect_t){ .has_value = false, .u.error = err };
    return (int16_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

int16_expect_t int16_array_max(const int16_array_t* array) {
    if (array == NULL)
        return (int16_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_max(&array->base, _cmp_int16, INT16_TYPE);
    if (!idx.has_value)
        return (int16_expect_t){ .has_value = false, .u.error = idx.u.error };
    int16_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, INT16_TYPE);
    if (err != NO_ERROR)
        return (int16_expect_t){ .has_value = false, .u.error = err };
    return (int16_expect_t){ .has_value = true, .u.value = val };
}
// ================================================================================ 
// ================================================================================ 

static inline dict_key_t _key(const char* s, size_t len) {
    return (dict_key_t){ .data = s, .len = len };
}
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
int16_dict_expect_t init_int16_dict(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v) {
    dict_expect_t r = init_dict(capacity, sizeof(int16_t),
                                INT16_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (int16_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (int16_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
void return_int16_dict(int16_dict_t* dict) {
    return_dict(dict);
}
 
// ================================================================================
// Insert
// ================================================================================
 
error_code_t insert_int16_dict(int16_dict_t*     dict,
                                 const char*        key,
                                 int16_t           value,
                                 allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    return insert_dict(dict, _key(key, strlen(key)), &value, alloc_v);
}
 
// --------------------------------------------------------------------------------
 
error_code_t insert_int16_dict_n(int16_dict_t*     dict,
                                   const char*        key,
                                   size_t             key_len,
                                   int16_t           value,
                                   allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return insert_dict(dict, _key(key, key_len), &value, alloc_v);
}
 
// ================================================================================
// Pop
// ================================================================================
 
error_code_t pop_int16_dict(int16_dict_t* dict,
                              const char*    key,
                              int16_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    return pop_dict(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t pop_int16_dict_n(int16_dict_t* dict,
                                const char*    key,
                                size_t         key_len,
                                int16_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return pop_dict(dict, _key(key, key_len), out_value);
}
 
// ================================================================================
// Update
// ================================================================================
 
error_code_t update_int16_dict(int16_dict_t* dict,
                                 const char*    key,
                                 int16_t       value) {
    if (key == NULL) return NULL_POINTER;
    return update_dict(dict, _key(key, strlen(key)), &value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t update_int16_dict_n(int16_dict_t* dict,
                                   const char*    key,
                                   size_t         key_len,
                                   int16_t       value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return update_dict(dict, _key(key, key_len), &value);
}
 
// ================================================================================
// Lookup
// ================================================================================
 
error_code_t get_int16_dict_value(const int16_dict_t* dict,
                                    const char*          key,
                                    int16_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    return get_dict_value(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t get_int16_dict_value_n(const int16_dict_t* dict,
                                      const char*          key,
                                      size_t               key_len,
                                      int16_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return get_dict_value(dict, _key(key, key_len), out_value);
}
 
// --------------------------------------------------------------------------------
 
const int16_t* get_int16_dict_ptr(const int16_dict_t* dict, const char* key) {
    if (key == NULL) return NULL;
    return (const int16_t*)get_dict_value_ptr(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
const int16_t* get_int16_dict_ptr_n(const int16_dict_t* dict,
                                       const char*          key,
                                       size_t               key_len) {
    if (key == NULL || key_len == 0u) return NULL;
    return (const int16_t*)get_dict_value_ptr(dict, _key(key, key_len));
}
 
// --------------------------------------------------------------------------------
 
bool has_int16_dict_key(const int16_dict_t* dict, const char* key) {
    if (key == NULL) return false;
    return has_dict_key(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
bool has_int16_dict_key_n(const int16_dict_t* dict,
                            const char*          key,
                            size_t               key_len) {
    if (key == NULL || key_len == 0u) return false;
    return has_dict_key(dict, _key(key, key_len));
}
 
// ================================================================================
// Utility
// ================================================================================
 
error_code_t clear_int16_dict(int16_dict_t* dict) {
    return clear_dict(dict);
}
 
// --------------------------------------------------------------------------------
 
int16_dict_expect_t copy_int16_dict(const int16_dict_t* src,
                                      allocator_vtable_t   alloc_v) {
    dict_expect_t r = copy_dict(src, alloc_v);
    if (!r.has_value)
        return (int16_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (int16_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
int16_dict_expect_t merge_int16_dict(const int16_dict_t* a,
                                       const int16_dict_t* b,
                                       bool                 overwrite,
                                       allocator_vtable_t   alloc_v) {
    dict_expect_t r = merge_dict(a, b, overwrite, alloc_v);
    if (!r.has_value)
        return (int16_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (int16_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// ================================================================================
// Iteration
// ================================================================================
 
typedef struct {
    int16_dict_iter_fn typed_fn;
    void*               user_data;
} _int16_iter_ctx_t;
 
static void _int16_iter_shim(dict_entry_t e, void* ctx) {
    const _int16_iter_ctx_t* c = (const _int16_iter_ctx_t*)ctx;
    int16_t value;
    memcpy(&value, e.value, sizeof(int16_t));
    c->typed_fn((const char*)e.key, e.key_len, value, c->user_data);
}
 
error_code_t foreach_int16_dict(const int16_dict_t* dict,
                                  int16_dict_iter_fn  fn,
                                  void*                user_data) {
    if (fn == NULL) return NULL_POINTER;
    _int16_iter_ctx_t ctx = { .typed_fn = fn, .user_data = user_data };
    return foreach_dict(dict, _int16_iter_shim, &ctx);
}
 
// ================================================================================
// Introspection
// ================================================================================
 
size_t int16_dict_size(const int16_dict_t* dict) {
    return dict_size(dict);
}
 
size_t int16_dict_hash_size(const int16_dict_t* dict) {
    return dict_hash_size(dict);
}
 
size_t int16_dict_alloc(const int16_dict_t* dict) {
    return dict_alloc(dict);
}
 
bool is_int16_dict_empty(const int16_dict_t* dict) {
    return is_dict_empty(dict);
}
// ================================================================================
// ================================================================================
// eof
