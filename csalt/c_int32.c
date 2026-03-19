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
#include <string.h>
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
// -------------------------------------------------------------------------------- 

int32_expect_t int32_array_min(const int32_array_t* array) {
    if (array == NULL)
        return (int32_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_min(&array->base, _cmp_int32, INT32_TYPE);
    if (!idx.has_value)
        return (int32_expect_t){ .has_value = false, .u.error = idx.u.error };
    int32_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, INT32_TYPE);
    if (err != NO_ERROR)
        return (int32_expect_t){ .has_value = false, .u.error = err };
    return (int32_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

int32_expect_t int32_array_max(const int32_array_t* array) {
    if (array == NULL)
        return (int32_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_max(&array->base, _cmp_int32, INT32_TYPE);
    if (!idx.has_value)
        return (int32_expect_t){ .has_value = false, .u.error = idx.u.error };
    int32_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, INT32_TYPE);
    if (err != NO_ERROR)
        return (int32_expect_t){ .has_value = false, .u.error = err };
    return (int32_expect_t){ .has_value = true, .u.value = val };
}
// -------------------------------------------------------------------------------- 

static void _add_int32(void* accum, const void* element) {
    *(int32_t*)accum += *(const int32_t*)element;
}

int32_expect_t int32_array_sum(const int32_array_t* array) {
    if (array == NULL)
        return (int32_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    int32_t total = 0;
    error_code_t err = array_sum(&array->base, &total, _add_int32, INT32_TYPE);
    if (err != NO_ERROR)
        return (int32_expect_t){ .has_value = false, .u.error = err };
    return (int32_expect_t){ .has_value = true, .u.value = total };
}
// -------------------------------------------------------------------------------- 

int32_array_expect_t cumulative_int32_array(const int32_array_t* src,
                                            allocator_vtable_t   alloc) {
    if (src == NULL)
        return (int32_array_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    array_expect_t r = cumulative_array(&src->base, _add_int32, alloc, INT32_TYPE);
    if (!r.has_value)
        return (int32_array_expect_t){ .has_value = false, .u.error = r.u.error };
    return (int32_array_expect_t){ .has_value = true,
                                   .u.value   = (int32_array_t*)r.u.value };
}
// ================================================================================ 
// ================================================================================ 

static inline dict_key_t _key(const char* s, size_t len) {
    return (dict_key_t){ .data = s, .len = len };
}
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
int32_dict_expect_t init_int32_dict(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v) {
    dict_expect_t r = init_dict(capacity, sizeof(int32_t),
                                INT32_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (int32_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (int32_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
void return_int32_dict(int32_dict_t* dict) {
    return_dict(dict);
}
 
// ================================================================================
// Insert
// ================================================================================
 
error_code_t insert_int32_dict(int32_dict_t*     dict,
                                 const char*        key,
                                 int32_t           value,
                                 allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    return insert_dict(dict, _key(key, strlen(key)), &value, alloc_v);
}
 
// --------------------------------------------------------------------------------
 
error_code_t insert_int32_dict_n(int32_dict_t*     dict,
                                   const char*        key,
                                   size_t             key_len,
                                   int32_t           value,
                                   allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return insert_dict(dict, _key(key, key_len), &value, alloc_v);
}
 
// ================================================================================
// Pop
// ================================================================================
 
error_code_t pop_int32_dict(int32_dict_t* dict,
                              const char*    key,
                              int32_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    return pop_dict(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t pop_int32_dict_n(int32_dict_t* dict,
                                const char*    key,
                                size_t         key_len,
                                int32_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return pop_dict(dict, _key(key, key_len), out_value);
}
 
// ================================================================================
// Update
// ================================================================================
 
error_code_t update_int32_dict(int32_dict_t* dict,
                                 const char*    key,
                                 int32_t       value) {
    if (key == NULL) return NULL_POINTER;
    return update_dict(dict, _key(key, strlen(key)), &value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t update_int32_dict_n(int32_dict_t* dict,
                                   const char*    key,
                                   size_t         key_len,
                                   int32_t       value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return update_dict(dict, _key(key, key_len), &value);
}
 
// ================================================================================
// Lookup
// ================================================================================
 
error_code_t get_int32_dict_value(const int32_dict_t* dict,
                                    const char*          key,
                                    int32_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    return get_dict_value(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t get_int32_dict_value_n(const int32_dict_t* dict,
                                      const char*          key,
                                      size_t               key_len,
                                      int32_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return get_dict_value(dict, _key(key, key_len), out_value);
}
 
// --------------------------------------------------------------------------------
 
const int32_t* get_int32_dict_ptr(const int32_dict_t* dict, const char* key) {
    if (key == NULL) return NULL;
    return (const int32_t*)get_dict_value_ptr(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
const int32_t* get_int32_dict_ptr_n(const int32_dict_t* dict,
                                       const char*          key,
                                       size_t               key_len) {
    if (key == NULL || key_len == 0u) return NULL;
    return (const int32_t*)get_dict_value_ptr(dict, _key(key, key_len));
}
 
// --------------------------------------------------------------------------------
 
bool has_int32_dict_key(const int32_dict_t* dict, const char* key) {
    if (key == NULL) return false;
    return has_dict_key(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
bool has_int32_dict_key_n(const int32_dict_t* dict,
                            const char*          key,
                            size_t               key_len) {
    if (key == NULL || key_len == 0u) return false;
    return has_dict_key(dict, _key(key, key_len));
}
 
// ================================================================================
// Utility
// ================================================================================
 
error_code_t clear_int32_dict(int32_dict_t* dict) {
    return clear_dict(dict);
}
 
// --------------------------------------------------------------------------------
 
int32_dict_expect_t copy_int32_dict(const int32_dict_t* src,
                                      allocator_vtable_t   alloc_v) {
    dict_expect_t r = copy_dict(src, alloc_v);
    if (!r.has_value)
        return (int32_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (int32_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
int32_dict_expect_t merge_int32_dict(const int32_dict_t* a,
                                       const int32_dict_t* b,
                                       bool                 overwrite,
                                       allocator_vtable_t   alloc_v) {
    dict_expect_t r = merge_dict(a, b, overwrite, alloc_v);
    if (!r.has_value)
        return (int32_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (int32_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// ================================================================================
// Iteration
// ================================================================================
 
typedef struct {
    int32_dict_iter_fn typed_fn;
    void*               user_data;
} _int32_iter_ctx_t;
 
static void _int32_iter_shim(dict_entry_t e, void* ctx) {
    const _int32_iter_ctx_t* c = (const _int32_iter_ctx_t*)ctx;
    int32_t value;
    memcpy(&value, e.value, sizeof(int32_t));
    c->typed_fn((const char*)e.key, e.key_len, value, c->user_data);
}
 
error_code_t foreach_int32_dict(const int32_dict_t* dict,
                                  int32_dict_iter_fn  fn,
                                  void*                user_data) {
    if (fn == NULL) return NULL_POINTER;
    _int32_iter_ctx_t ctx = { .typed_fn = fn, .user_data = user_data };
    return foreach_dict(dict, _int32_iter_shim, &ctx);
}
 
// ================================================================================
// Introspection
// ================================================================================
 
size_t int32_dict_size(const int32_dict_t* dict) {
    return dict_size(dict);
}
 
size_t int32_dict_hash_size(const int32_dict_t* dict) {
    return dict_hash_size(dict);
}
 
size_t int32_dict_alloc(const int32_dict_t* dict) {
    return dict_alloc(dict);
}
 
bool is_int32_dict_empty(const int32_dict_t* dict) {
    return is_dict_empty(dict);
}
// ================================================================================
// ================================================================================
// eof
