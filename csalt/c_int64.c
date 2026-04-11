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
#include <string.h>
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

static inline dict_key_t _key(const char* s, size_t len) {
    return (dict_key_t){ .data = s, .len = len };
}
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
int64_dict_expect_t init_int64_dict(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v) {
    dict_expect_t r = init_dict(capacity, sizeof(int64_t),
                                INT64_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (int64_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (int64_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
void return_int64_dict(int64_dict_t* dict) {
    return_dict(dict);
}
 
// ================================================================================
// Insert
// ================================================================================
 
error_code_t insert_int64_dict(int64_dict_t*     dict,
                                 const char*        key,
                                 int64_t           value,
                                 allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    return insert_dict(dict, _key(key, strlen(key)), &value, alloc_v);
}
 
// --------------------------------------------------------------------------------
 
error_code_t insert_int64_dict_n(int64_dict_t*     dict,
                                   const char*        key,
                                   size_t             key_len,
                                   int64_t           value,
                                   allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return insert_dict(dict, _key(key, key_len), &value, alloc_v);
}
 
// ================================================================================
// Pop
// ================================================================================
 
error_code_t pop_int64_dict(int64_dict_t* dict,
                              const char*    key,
                              int64_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    return pop_dict(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t pop_int64_dict_n(int64_dict_t* dict,
                                const char*    key,
                                size_t         key_len,
                                int64_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return pop_dict(dict, _key(key, key_len), out_value);
}
 
// ================================================================================
// Update
// ================================================================================
 
error_code_t update_int64_dict(int64_dict_t* dict,
                                 const char*    key,
                                 int64_t       value) {
    if (key == NULL) return NULL_POINTER;
    return update_dict(dict, _key(key, strlen(key)), &value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t update_int64_dict_n(int64_dict_t* dict,
                                   const char*    key,
                                   size_t         key_len,
                                   int64_t       value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return update_dict(dict, _key(key, key_len), &value);
}
 
// ================================================================================
// Lookup
// ================================================================================
 
error_code_t get_int64_dict_value(const int64_dict_t* dict,
                                    const char*          key,
                                    int64_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    return get_dict_value(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t get_int64_dict_value_n(const int64_dict_t* dict,
                                      const char*          key,
                                      size_t               key_len,
                                      int64_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return get_dict_value(dict, _key(key, key_len), out_value);
}
 
// --------------------------------------------------------------------------------
 
const int64_t* get_int64_dict_ptr(const int64_dict_t* dict, const char* key) {
    if (key == NULL) return NULL;
    return (const int64_t*)get_dict_value_ptr(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
const int64_t* get_int64_dict_ptr_n(const int64_dict_t* dict,
                                       const char*          key,
                                       size_t               key_len) {
    if (key == NULL || key_len == 0u) return NULL;
    return (const int64_t*)get_dict_value_ptr(dict, _key(key, key_len));
}
 
// --------------------------------------------------------------------------------
 
bool has_int64_dict_key(const int64_dict_t* dict, const char* key) {
    if (key == NULL) return false;
    return has_dict_key(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
bool has_int64_dict_key_n(const int64_dict_t* dict,
                            const char*          key,
                            size_t               key_len) {
    if (key == NULL || key_len == 0u) return false;
    return has_dict_key(dict, _key(key, key_len));
}
 
// ================================================================================
// Utility
// ================================================================================
 
error_code_t clear_int64_dict(int64_dict_t* dict) {
    return clear_dict(dict);
}
 
// --------------------------------------------------------------------------------
 
int64_dict_expect_t copy_int64_dict(const int64_dict_t* src,
                                      allocator_vtable_t   alloc_v) {
    dict_expect_t r = copy_dict(src, alloc_v);
    if (!r.has_value)
        return (int64_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (int64_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
int64_dict_expect_t merge_int64_dict(const int64_dict_t* a,
                                       const int64_dict_t* b,
                                       bool                 overwrite,
                                       allocator_vtable_t   alloc_v) {
    dict_expect_t r = merge_dict(a, b, overwrite, alloc_v);
    if (!r.has_value)
        return (int64_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (int64_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// ================================================================================
// Iteration
// ================================================================================
 
typedef struct {
    int64_dict_iter_fn typed_fn;
    void*               user_data;
} _int64_iter_ctx_t;
 
static void _int64_iter_shim(dict_entry_t e, void* ctx) {
    const _int64_iter_ctx_t* c = (const _int64_iter_ctx_t*)ctx;
    int64_t value;
    memcpy(&value, e.value, sizeof(int64_t));
    c->typed_fn((const char*)e.key, e.key_len, value, c->user_data);
}
 
error_code_t foreach_int64_dict(const int64_dict_t* dict,
                                  int64_dict_iter_fn  fn,
                                  void*                user_data) {
    if (fn == NULL) return NULL_POINTER;
    _int64_iter_ctx_t ctx = { .typed_fn = fn, .user_data = user_data };
    return foreach_dict(dict, _int64_iter_shim, &ctx);
}
 
// ================================================================================
// Introspection
// ================================================================================
 
size_t int64_dict_size(const int64_dict_t* dict) {
    return dict_size(dict);
}
 
size_t int64_dict_hash_size(const int64_dict_t* dict) {
    return dict_hash_size(dict);
}
 
size_t int64_dict_alloc(const int64_dict_t* dict) {
    return dict_alloc(dict);
}
 
bool is_int64_dict_empty(const int64_dict_t* dict) {
    return is_dict_empty(dict);
}
// ================================================================================ 
// ================================================================================ 


// ================================================================================
// ================================================================================
// eof
