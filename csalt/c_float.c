// ================================================================================
// ================================================================================
// - File:    c_float.c
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
#include "c_float.h"


#if defined(__AVX512F__)
#  include "simd_avx512_float.inl"
#elif defined(__AVX2__)
#  include "simd_avx2_float.inl"
#elif defined(__AVX__)
#  include "simd_avx_float.inl"
#elif defined(__SSE4_1__)
#  include "simd_sse41_float.inl"
#elif defined(__SSSE3__)
#  include "simd_sse3_float.inl"
#elif defined(__SSE2__)
#  include "simd_sse2_float.inl"
#elif defined(__ARM_FEATURE_SVE2)
#  include "simd_sve2_float.inl"
#elif defined(__ARM_FEATURE_SVE)
#  include "simd_sve_float.inl"
#elif defined(__ARM_NEON)
#  include "simd_neon_float.inl"
#else
#  include "simd_scalar_float.inl"
#endif
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Internal helper: unwrap a raw array_expect_t into a float_array_expect_t.
// The array_t* stored inside the array_expect_t is reinterpreted as the base
// of a float_array_t since float_array_t contains array_t as its first member.
// ================================================================================

static inline float_array_expect_t _wrap_expect(array_expect_t e) {
    if (e.has_value) {
        return (float_array_expect_t){
            .has_value = true,
            .u.value   = (float_array_t*)e.u.value
        };
    }
    return (float_array_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

float_array_expect_t init_float_array(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v) {
    return _wrap_expect(init_array(capacity, FLOAT_TYPE, growth, alloc_v));
}

// --------------------------------------------------------------------------------

void return_float_array(float_array_t* array) {
    if (array == NULL) return;
    return_array(&array->base);
}

// ================================================================================
// Push operations
// ================================================================================

error_code_t push_back_float_array(float_array_t* array, float value) {
    if (array == NULL) return NULL_POINTER;
    return push_back_array(&array->base, &value, FLOAT_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_front_float_array(float_array_t* array, float value) {
    if (array == NULL) return NULL_POINTER;
    return push_front_array(&array->base, &value, FLOAT_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_at_float_array(float_array_t* array,
                                 size_t         index,
                                 float          value) {
    if (array == NULL) return NULL_POINTER;
    return push_at_array(&array->base, &value, index, FLOAT_TYPE);
}

// ================================================================================
// Get operation
// ================================================================================

error_code_t get_float_array_index(const float_array_t* array,
                                   size_t               index,
                                   float*               out) {
    if (array == NULL || out == NULL) return NULL_POINTER;
    return get_array_index(&array->base, index, out, FLOAT_TYPE);
}

// ================================================================================
// Pop operations
// ================================================================================

error_code_t pop_back_float_array(float_array_t* array, float* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_back_array(&array->base, out, FLOAT_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_front_float_array(float_array_t* array, float* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_front_array(&array->base, out, FLOAT_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_any_float_array(float_array_t* array,
                                 float*         out,
                                 size_t         index) {
    if (array == NULL) return NULL_POINTER;
    return pop_any_array(&array->base, out, index, FLOAT_TYPE);
}

// ================================================================================
// Utility operations
// ================================================================================

error_code_t clear_float_array(float_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return clear_array(&array->base);
}

// --------------------------------------------------------------------------------

error_code_t set_float_array_index(float_array_t* array,
                                   size_t         index,
                                   float          value) {
    if (array == NULL) return NULL_POINTER;
    return set_array_index(&array->base, index, &value, FLOAT_TYPE);
}

// --------------------------------------------------------------------------------

float_array_expect_t copy_float_array(const float_array_t* src,
                                      allocator_vtable_t   alloc_v) {
    if (src == NULL) {
        return (float_array_expect_t){ .has_value = false,
                                       .u.error   = NULL_POINTER };
    }
    return _wrap_expect(copy_array(&src->base, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t concat_float_array(float_array_t*       dst,
                                const float_array_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_array(&dst->base, &src->base);
}

// --------------------------------------------------------------------------------

float_array_expect_t slice_float_array(const float_array_t* src,
                                       size_t               start,
                                       size_t               end,
                                       allocator_vtable_t   alloc_v) {
    if (src == NULL) {
        return (float_array_expect_t){ .has_value = false,
                                       .u.error   = NULL_POINTER };
    }
    return _wrap_expect(slice_array(&src->base, start, end, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t reverse_float_array(float_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return reverse_array(&array->base);
}

// --------------------------------------------------------------------------------

/*
 * Comparator for float. The three-way idiom (va > vb) - (va < vb) is correct
 * for all finite values and signed zeros under IEEE 754. NaN comparisons
 * produce 0 for both sub-expressions, yielding 0 (equal), which gives
 * unspecified sort order — callers must ensure no NaN are present.
 *
 * Subtraction is not used because for floats it can produce -0.0f for equal
 * values and does not return an int, requiring a cast that loses the sign for
 * subnormal differences smaller than INT_MIN.
 */
static int _cmp_float(const void* a, const void* b) {
    float va = *(const float*)a;
    float vb = *(const float*)b;
    return (va > vb) - (va < vb);
}

error_code_t sort_float_array(float_array_t* array, direction_t dir) {
    if (array == NULL) return NULL_POINTER;
    return sort_array(&array->base, _cmp_float, dir);
}

// ================================================================================
// Reduction
// ================================================================================

float_expect_t float_array_min(const float_array_t* array) {
    if (array == NULL)
        return (float_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_min(&array->base, _cmp_float, FLOAT_TYPE);
    if (!idx.has_value)
        return (float_expect_t){ .has_value = false, .u.error = idx.u.error };
    float val = 0.0f;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, FLOAT_TYPE);
    if (err != NO_ERROR)
        return (float_expect_t){ .has_value = false, .u.error = err };
    return (float_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

float_expect_t float_array_max(const float_array_t* array) {
    if (array == NULL)
        return (float_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_max(&array->base, _cmp_float, FLOAT_TYPE);
    if (!idx.has_value)
        return (float_expect_t){ .has_value = false, .u.error = idx.u.error };
    float val = 0.0f;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, FLOAT_TYPE);
    if (err != NO_ERROR)
        return (float_expect_t){ .has_value = false, .u.error = err };
    return (float_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

static void _add_float(void* accum, const void* element) {
    *(float*)accum += *(const float*)element;
}

float_expect_t float_array_sum(const float_array_t* array) {
    if (array == NULL)
        return (float_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    float total = 0.0f;
    error_code_t err = array_sum(&array->base, &total, _add_float, FLOAT_TYPE);
    if (err != NO_ERROR)
        return (float_expect_t){ .has_value = false, .u.error = err };
    return (float_expect_t){ .has_value = true, .u.value = total };
}

// --------------------------------------------------------------------------------

float_array_expect_t cumulative_float_array(const float_array_t* src,
                                            allocator_vtable_t   alloc) {
    if (src == NULL)
        return (float_array_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    array_expect_t r = cumulative_array(&src->base, _add_float, alloc, FLOAT_TYPE);
    if (!r.has_value)
        return (float_array_expect_t){ .has_value = false, .u.error = r.u.error };
    return (float_array_expect_t){ .has_value = true,
                                   .u.value   = (float_array_t*)r.u.value };
}

// ================================================================================
// Search
// ================================================================================

size_expect_t float_array_contains(const float_array_t* array,
                                   float                value,
                                   size_t               start,
                                   size_t               end,
                                   float                abs_tol,
                                   float                rel_tol) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (abs_tol < 0.0f || rel_tol < 0.0f)
        return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (start >= end)
        return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (end > array->base.len)
        return (size_expect_t){ .has_value = false, .u.error = OUT_OF_BOUNDS };

    const float* data = (const float*)array->base.data;
    size_t idx = simd_contains_float(data, start, end, value, abs_tol, rel_tol);

    if (idx == SIZE_MAX)
        return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
    return (size_expect_t){ .has_value = true, .u.value = idx };
}

// --------------------------------------------------------------------------------

size_expect_t float_array_binary_search(float_array_t* array,
                                        float          value,
                                        bool           sort,
                                        float          abs_tol,
                                        float          rel_tol) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (abs_tol < 0.0f || rel_tol < 0.0f)
        return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (array->base.len == 0u)
        return (size_expect_t){ .has_value = false, .u.error = EMPTY };

    if (sort)
        sort_array(&array->base, _cmp_float, FORWARD);

    const float* data = (const float*)array->base.data;
    size_t idx = simd_binary_search_float(data, array->base.len, value,
                                          abs_tol, rel_tol, _cmp_float);
    if (idx == SIZE_MAX)
        return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
    return (size_expect_t){ .has_value = true, .u.value = idx };
}

// --------------------------------------------------------------------------------

bracket_expect_t float_array_binary_bracket(float_array_t* array,
                                            float          value,
                                            bool           sort) {
    if (array == NULL)
        return (bracket_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_bracket_array(&array->base, &value, _cmp_float, sort,
                                FLOAT_TYPE);
}

// ================================================================================
// Introspection
// ================================================================================

size_t float_array_size(const float_array_t* array) {
    if (array == NULL) return 0u;
    return array_size(&array->base);
}

// --------------------------------------------------------------------------------

size_t float_array_alloc(const float_array_t* array) {
    if (array == NULL) return 0u;
    return array_alloc(&array->base);
}

// --------------------------------------------------------------------------------

size_t float_array_data_size(const float_array_t* array) {
    if (array == NULL) return 0u;
    return array_data_size(&array->base);
}

// --------------------------------------------------------------------------------

bool is_float_array_empty(const float_array_t* array) {
    if (array == NULL) return true;
    return is_array_empty(&array->base);
}

// --------------------------------------------------------------------------------

bool is_float_array_full(const float_array_t* array) {
    if (array == NULL) return true;
    return is_array_full(&array->base);
}

// --------------------------------------------------------------------------------

bool is_float_array_ptr(const float_array_t* array, const float* ptr) {
    if (array == NULL) return false;
    return is_array_ptr(&array->base, ptr);
}
// --------------------------------------------------------------------------------

error_code_t print_float_array(const float_array_t* array, FILE* stream) {
    if (array == NULL || stream == NULL) return NULL_POINTER;

    const float* data = (const float*)array->base.data;
    size_t len = array->base.len;
    size_t col = 0u;

    fputs("[ ", stream);
    col = 2u;

    if (len == 0u) {
        fputs("]", stream);
        return NO_ERROR;
    }

    for (size_t i = 0u; i < len; ++i) {
        char buf[32];
        int n = snprintf(buf, sizeof(buf), "%g", (double)data[i]);
        size_t value_len = (size_t)n;

        if (i == 0u) {
            if (col + value_len > 70u) {
                fputc('\n', stream);
                fputs("  ", stream);
                col = 2u;
            }
            fputs(buf, stream);
            col += value_len;
        } else {
            size_t token_len = 2u + value_len;
            if (col + token_len > 70u) {
                fputs(",\n  ", stream);
                col = 2u;
                fputs(buf, stream);
                col += value_len;
            } else {
                fputs(", ", stream);
                fputs(buf, stream);
                col += token_len;
            }
        }
    }

    if (col + 2u > 70u) {
        fputc('\n', stream);
        fputs("  ]", stream);
    } else {
        fputs(" ]", stream);
    }

    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 

static inline dict_key_t _key(const char* s, size_t len) {
    return (dict_key_t){ .data = s, .len = len };
}
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
float_dict_expect_t init_float_dict(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v) {
    dict_expect_t r = init_dict(capacity, sizeof(float_t),
                                FLOAT_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (float_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (float_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
void return_float_dict(float_dict_t* dict) {
    return_dict(dict);
}
 
// ================================================================================
// Insert
// ================================================================================
 
error_code_t insert_float_dict(float_dict_t*     dict,
                                 const char*        key,
                                 float_t           value,
                                 allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    return insert_dict(dict, _key(key, strlen(key)), &value, alloc_v);
}
 
// --------------------------------------------------------------------------------
 
error_code_t insert_float_dict_n(float_dict_t*     dict,
                                   const char*        key,
                                   size_t             key_len,
                                   float_t           value,
                                   allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return insert_dict(dict, _key(key, key_len), &value, alloc_v);
}
 
// ================================================================================
// Pop
// ================================================================================
 
error_code_t pop_float_dict(float_dict_t* dict,
                              const char*    key,
                              float_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    return pop_dict(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t pop_float_dict_n(float_dict_t* dict,
                                const char*    key,
                                size_t         key_len,
                                float_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return pop_dict(dict, _key(key, key_len), out_value);
}
 
// ================================================================================
// Update
// ================================================================================
 
error_code_t update_float_dict(float_dict_t* dict,
                                 const char*    key,
                                 float_t       value) {
    if (key == NULL) return NULL_POINTER;
    return update_dict(dict, _key(key, strlen(key)), &value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t update_float_dict_n(float_dict_t* dict,
                                   const char*    key,
                                   size_t         key_len,
                                   float_t       value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return update_dict(dict, _key(key, key_len), &value);
}
 
// ================================================================================
// Lookup
// ================================================================================
 
error_code_t get_float_dict_value(const float_dict_t* dict,
                                    const char*          key,
                                    float_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    return get_dict_value(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t get_float_dict_value_n(const float_dict_t* dict,
                                      const char*          key,
                                      size_t               key_len,
                                      float_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return get_dict_value(dict, _key(key, key_len), out_value);
}
 
// --------------------------------------------------------------------------------
 
const float_t* get_float_dict_ptr(const float_dict_t* dict, const char* key) {
    if (key == NULL) return NULL;
    return (const float_t*)get_dict_value_ptr(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
const float_t* get_float_dict_ptr_n(const float_dict_t* dict,
                                       const char*          key,
                                       size_t               key_len) {
    if (key == NULL || key_len == 0u) return NULL;
    return (const float_t*)get_dict_value_ptr(dict, _key(key, key_len));
}
 
// --------------------------------------------------------------------------------
 
bool has_float_dict_key(const float_dict_t* dict, const char* key) {
    if (key == NULL) return false;
    return has_dict_key(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
bool has_float_dict_key_n(const float_dict_t* dict,
                            const char*          key,
                            size_t               key_len) {
    if (key == NULL || key_len == 0u) return false;
    return has_dict_key(dict, _key(key, key_len));
}
 
// ================================================================================
// Utility
// ================================================================================
 
error_code_t clear_float_dict(float_dict_t* dict) {
    return clear_dict(dict);
}
 
// --------------------------------------------------------------------------------
 
float_dict_expect_t copy_float_dict(const float_dict_t* src,
                                      allocator_vtable_t   alloc_v) {
    dict_expect_t r = copy_dict(src, alloc_v);
    if (!r.has_value)
        return (float_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (float_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
float_dict_expect_t merge_float_dict(const float_dict_t* a,
                                       const float_dict_t* b,
                                       bool                 overwrite,
                                       allocator_vtable_t   alloc_v) {
    dict_expect_t r = merge_dict(a, b, overwrite, alloc_v);
    if (!r.has_value)
        return (float_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (float_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// ================================================================================
// Iteration
// ================================================================================
 
typedef struct {
    float_dict_iter_fn typed_fn;
    void*               user_data;
} _float_iter_ctx_t;
 
static void _float_iter_shim(dict_entry_t e, void* ctx) {
    const _float_iter_ctx_t* c = (const _float_iter_ctx_t*)ctx;
    float_t value;
    memcpy(&value, e.value, sizeof(float_t));
    c->typed_fn((const char*)e.key, e.key_len, value, c->user_data);
}
 
error_code_t foreach_float_dict(const float_dict_t* dict,
                                  float_dict_iter_fn  fn,
                                  void*                user_data) {
    if (fn == NULL) return NULL_POINTER;
    _float_iter_ctx_t ctx = { .typed_fn = fn, .user_data = user_data };
    return foreach_dict(dict, _float_iter_shim, &ctx);
}
 
// ================================================================================
// Introspection
// ================================================================================
 
size_t float_dict_size(const float_dict_t* dict) {
    return dict_size(dict);
}
 
size_t float_dict_hash_size(const float_dict_t* dict) {
    return dict_hash_size(dict);
}
 
size_t float_dict_alloc(const float_dict_t* dict) {
    return dict_alloc(dict);
}
 
bool is_float_dict_empty(const float_dict_t* dict) {
    return is_dict_empty(dict);
}
// -------------------------------------------------------------------------------- 

typedef struct {
    FILE*  stream;
    size_t col;
    bool   first;
} _float_dict_print_ctx_t;

static void _print_float_dict_entry(const char* key,
                                    size_t      key_len,
                                    float       value,
                                    void*       user_data) {
    _float_dict_print_ctx_t* ctx = (_float_dict_print_ctx_t*)user_data;
    char val_buf[32];
    int val_n = snprintf(val_buf, sizeof(val_buf), "%g", (double)value);
    size_t value_len = (val_n > 0) ? (size_t)val_n : 0u;

    size_t token_len = key_len + value_len + 4u;
    if (!ctx->first) {
        token_len += 2u;
    }

    if (ctx->col + token_len > 70u) {
        if (!ctx->first) {
            fputs(",\n  ", ctx->stream);
            ctx->col = 2u;
        } else {
            fputc('\n', ctx->stream);
            fputs("  ", ctx->stream);
            ctx->col = 2u;
        }
    } else if (!ctx->first) {
        fputs(", ", ctx->stream);
        ctx->col += 2u;
    }

    fprintf(ctx->stream, "\"%.*s\": %s", (int)key_len, key, val_buf);
    ctx->col += key_len + value_len + 4u;
    ctx->first = false;
}

error_code_t print_float_dict(const float_dict_t* dict, FILE* stream) {
    if (dict == NULL || stream == NULL) return NULL_POINTER;

    _float_dict_print_ctx_t ctx;
    ctx.stream = stream;
    ctx.col    = 2u;
    ctx.first  = true;

    fputs("{ ", stream);

    error_code_t err = foreach_float_dict(dict, _print_float_dict_entry, &ctx);
    if (err != NO_ERROR) return err;

    if (ctx.first) {
        fputs("}", stream);
        return NO_ERROR;
    }

    if (ctx.col + 2u > 70u) {
        fputc('\n', stream);
        fputs("  }", stream);
    } else {
        fputs(" }", stream);
    }

    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// ================================================================================
// float_matrix_t — type-safe float wrapper implementations
//
// Every function below is a thin delegation to the corresponding generic
// matrix_t function, fixing the dtype to FLOAT_TYPE and converting between
// float / float* and the void* interface expected by c_matrix.h.
// ================================================================================
// ================================================================================

// ================================================================================
// Internal helper: rewrap matrix_expect_t as float_matrix_expect_t
// ================================================================================

static inline float_matrix_expect_t _wrap_matrix_expect(matrix_expect_t e) {
    if (e.has_value) {
        return (float_matrix_expect_t){
            .has_value = true,
            .u.value   = (float_matrix_t*)e.u.value
        };
    }
    return (float_matrix_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

float_matrix_expect_t init_float_dense_matrix(size_t             rows,
                                              size_t             cols,
                                              allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_dense_matrix(rows, cols, FLOAT_TYPE, alloc_v)
    );
}

// --------------------------------------------------------------------------------

float_matrix_expect_t init_float_coo_matrix(size_t             rows,
                                            size_t             cols,
                                            size_t             capacity,
                                            bool               growth,
                                            allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_coo_matrix(rows, cols, capacity, FLOAT_TYPE, growth, alloc_v)
    );
}

// --------------------------------------------------------------------------------

void return_float_matrix(float_matrix_t* mat) {
    return_matrix(mat);
}

// ================================================================================
// Element access
// ================================================================================

error_code_t get_float_matrix(const float_matrix_t* mat,
                              size_t                row,
                              size_t                col,
                              float*                out) {
    if (mat == NULL || out == NULL) return NULL_POINTER;
    return get_matrix(mat, row, col, out);
}

// --------------------------------------------------------------------------------

error_code_t set_float_matrix(float_matrix_t* mat,
                              size_t          row,
                              size_t          col,
                              float           value) {
    if (mat == NULL) return NULL_POINTER;
    return set_matrix(mat, row, col, &value);
}

// ================================================================================
// COO assembly helpers
// ================================================================================

error_code_t reserve_float_coo_matrix(float_matrix_t* mat,
                                      size_t          capacity) {
    return reserve_coo_matrix(mat, capacity);
}

// --------------------------------------------------------------------------------

error_code_t push_back_float_coo_matrix(float_matrix_t* mat,
                                        size_t          row,
                                        size_t          col,
                                        float           value) {
    if (mat == NULL) return NULL_POINTER;
    return push_back_coo_matrix(mat, row, col, &value);
}

// --------------------------------------------------------------------------------

error_code_t sort_float_coo_matrix(float_matrix_t* mat) {
    return sort_coo_matrix(mat);
}

// ================================================================================
// Lifecycle / structural operations
// ================================================================================

error_code_t clear_float_matrix(float_matrix_t* mat) {
    return clear_matrix(mat);
}

// --------------------------------------------------------------------------------

float_matrix_expect_t copy_float_matrix(const float_matrix_t* src,
                                        allocator_vtable_t    alloc_v) {
    return _wrap_matrix_expect(copy_matrix(src, alloc_v));
}

// --------------------------------------------------------------------------------

float_matrix_expect_t convert_float_matrix(const float_matrix_t* src,
                                           matrix_format_t       target,
                                           allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (float_matrix_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }

    /* SIMD fast path: dense float -> CSR */
    if (src->format == DENSE_MATRIX && target == CSR_MATRIX) {
        const float* data = NULL;
        size_t total = 0u;
        size_t nnz = 0u;
        size_t row_ptr_bytes = 0u;
        size_t col_idx_bytes = 0u;
        size_t values_bytes = 0u;
        size_t* row_ptr = NULL;
        size_t* col_idx = NULL;
        float* values = NULL;
        float_matrix_t* dst = NULL;
        void_ptr_expect_t mr;
        void_ptr_expect_t rp_r;
        void_ptr_expect_t ci_r;
        void_ptr_expect_t vl_r;

        /* Validate allocator interface required by this fast path */
        if (alloc_v.allocate == NULL || alloc_v.return_element == NULL) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = NULL_POINTER
            };
        }

        /* Defensive validation of dense backing storage */
        if ((src->rows > 0u) && (src->cols > 0u) && (src->rep.dense.data == NULL)) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = ILLEGAL_STATE
            };
        }

        /* Overflow check for rows * cols */
        if ((src->rows != 0u) && (src->cols > (SIZE_MAX / src->rows))) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        total = src->rows * src->cols;
        data = (const float*)src->rep.dense.data;

        /* Pass 1: count nonzeros (SIMD-accelerated) */
        nnz = simd_count_nonzero_float(data, total);

        /* Overflow checks for byte counts */
        if ((src->rows + 1u) < src->rows) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if ((src->rows + 1u) > (SIZE_MAX / sizeof(size_t))) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if (nnz > (SIZE_MAX / sizeof(size_t))) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if (nnz > (SIZE_MAX / sizeof(float))) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }

        /* Allocate the matrix header */
        mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
        if (!mr.has_value) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = BAD_ALLOC
            };
        }

        dst = (float_matrix_t*)mr.u.value;
        dst->rows      = src->rows;
        dst->cols      = src->cols;
        dst->dtype     = FLOAT_TYPE;
        dst->data_size = sizeof(float);
        dst->format    = CSR_MATRIX;
        dst->alloc_v   = alloc_v;

        dst->rep.csr.nnz     = 0u;
        dst->rep.csr.row_ptr = NULL;
        dst->rep.csr.col_idx = NULL;
        dst->rep.csr.values  = NULL;

        row_ptr_bytes = (src->rows + 1u) * sizeof(size_t);
        col_idx_bytes = nnz * sizeof(size_t);
        values_bytes  = nnz * sizeof(float);

        /*
         * For nnz == 0, allocate a minimum of 1 byte for col_idx/values so the
         * allocator is never asked for a zero-size block. The logical nnz stays 0.
         */
        rp_r = alloc_v.allocate(alloc_v.ctx, row_ptr_bytes, true);
        ci_r = alloc_v.allocate(alloc_v.ctx, (nnz > 0u) ? col_idx_bytes : 1u, true);
        vl_r = alloc_v.allocate(alloc_v.ctx, (nnz > 0u) ? values_bytes  : 1u, true);

        if (!rp_r.has_value || !ci_r.has_value || !vl_r.has_value) {
            if (rp_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, rp_r.u.value);
            }
            if (ci_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, ci_r.u.value);
            }
            if (vl_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, vl_r.u.value);
            }
            alloc_v.return_element(alloc_v.ctx, dst);

            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = OUT_OF_MEMORY
            };
        }

        row_ptr = (size_t*)rp_r.u.value;
        col_idx = (size_t*)ci_r.u.value;
        values  = (float*)vl_r.u.value;

        /* Pass 2: scatter nonzeros into CSR arrays */
        row_ptr[0] = 0u;
        if (nnz > 0u) {
            size_t k = 0u;

            for (size_t i = 0u; i < src->rows; ++i) {
                const float* row_data = data + (i * src->cols);
                k = simd_scatter_csr_row_float(
                    row_data, src->cols, 0u, col_idx, values, k
                );
                row_ptr[i + 1u] = k;
            }

            dst->rep.csr.nnz = k;
        } else {
            for (size_t i = 0u; i < src->rows; ++i) {
                row_ptr[i + 1u] = 0u;
            }
            dst->rep.csr.nnz = 0u;
        }

        dst->rep.csr.row_ptr = row_ptr;
        dst->rep.csr.col_idx = col_idx;
        dst->rep.csr.values  = (uint8_t*)values;

        return (float_matrix_expect_t){
            .has_value = true,
            .u.value   = dst
        };
    }

    /* All other conversions: delegate to generic path */
    return _wrap_matrix_expect(convert_matrix(src, target, alloc_v));
}

// --------------------------------------------------------------------------------

float_matrix_expect_t transpose_float_matrix(const float_matrix_t* src,
                                             allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (float_matrix_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }
 
    /* Non-dense formats: delegate to the generic path which handles
       COO, CSR, and CSC transposes natively. */
    if (src->format != DENSE_MATRIX) {
        return _wrap_matrix_expect(transpose_matrix(src, alloc_v));
    }
 
    /* Allocate destination dense matrix with transposed dimensions */
    float_matrix_expect_t r = init_float_dense_matrix(
        src->cols, src->rows, alloc_v
    );
    if (!r.has_value) return r;
 
    /* SIMD fast path: dense float transpose */
    const float* sdata = (const float*)src->rep.dense.data;
    float*       ddata = (float*)r.u.value->rep.dense.data;
    simd_transpose_float(sdata, ddata, src->rows, src->cols);
 
    return r;
}
// ================================================================================
// Fill and zero
// ================================================================================

error_code_t fill_float_matrix(float_matrix_t* mat,
                               float           value) {
    if (mat == NULL) return NULL_POINTER;
 
    /* Zero fill: delegate to clear_matrix for all formats */
    if (value == 0.0f) return clear_matrix(mat);
 
    /* Non-zero fill on non-dense: fall back to generic path */
    if (mat->format != DENSE_MATRIX) return fill_matrix(mat, &value);
 
    /* SIMD fast path: dense float fill */
    float* data  = (float*)mat->rep.dense.data;
    size_t count = mat->rows * mat->cols;
    simd_fill_float(data, count, value);
    return NO_ERROR;
}
// ================================================================================
// Shape and compatibility queries
// ================================================================================

bool float_matrix_has_same_shape(const float_matrix_t* a,
                                 const float_matrix_t* b) {
    return matrix_has_same_shape(a, b);
}

// --------------------------------------------------------------------------------

bool float_matrix_is_square(const float_matrix_t* mat) {
    return matrix_is_square(mat);
}

// --------------------------------------------------------------------------------

bool float_matrix_is_sparse(const float_matrix_t* mat) {
    return matrix_is_sparse(mat);
}

// --------------------------------------------------------------------------------

bool float_matrix_is_zero(const float_matrix_t* mat) {
    if (mat == NULL) return true;

    /* SIMD fast path: dense float */
    if (mat->format == DENSE_MATRIX) {
        const float* data = (const float*)mat->rep.dense.data;
        size_t count = mat->rows * mat->cols;
        return simd_is_all_zero_float(data, count);
    }

    /* Sparse formats: delegate to generic path */
    return is_zero_matrix(mat);
}

// --------------------------------------------------------------------------------

bool float_matrix_equal(const float_matrix_t* a,
                        const float_matrix_t* b) {
    if (a == NULL || b == NULL) return false;
    if (!matrix_has_same_shape((const matrix_t*)a, (const matrix_t*)b)) return false;
    if (a->dtype != FLOAT_TYPE || b->dtype != FLOAT_TYPE) return false;

    /* Fast path: both dense */
    if (a->format == DENSE_MATRIX && b->format == DENSE_MATRIX) {
        const float* da = (const float*)a->rep.dense.data;
        const float* db = (const float*)b->rep.dense.data;
        return simd_float_arrays_equal(da, db, a->rows * a->cols);
    }

    /* Fast path: both COO and sorted */
    if (a->format == COO_MATRIX && b->format == COO_MATRIX &&
        a->rep.coo.sorted && b->rep.coo.sorted) {

        size_t nnz = a->rep.coo.nnz;
        if (nnz != b->rep.coo.nnz) return false;

        if (nnz > 0u) {
            if (memcmp(a->rep.coo.row_idx, b->rep.coo.row_idx,
                       nnz * sizeof(size_t)) != 0) {
                return false;
            }

            if (memcmp(a->rep.coo.col_idx, b->rep.coo.col_idx,
                       nnz * sizeof(size_t)) != 0) {
                return false;
            }

            if (!simd_float_arrays_equal(
                    (const float*)a->rep.coo.values,
                    (const float*)b->rep.coo.values,
                    nnz)) {
                return false;
            }
        }

        return true;
    }

    /* General logical comparison */
    for (size_t i = 0u; i < a->rows; ++i) {
        for (size_t j = 0u; j < a->cols; ++j) {
            float va = 0.0f;
            float vb = 0.0f;

            if (get_matrix((const matrix_t*)a, i, j, &va) != NO_ERROR) return false;
            if (get_matrix((const matrix_t*)b, i, j, &vb) != NO_ERROR) return false;

            if (!(va == vb)) return false;
        }
    }

    return true;
}
// --------------------------------------------------------------------------------

bool float_matrix_is_add_compatible(const float_matrix_t* a,
                                    const float_matrix_t* b) {
    return matrix_is_add_compatible(a, b);
}

// --------------------------------------------------------------------------------

bool float_matrix_is_multiply_compatible(const float_matrix_t* a,
                                         const float_matrix_t* b) {
    return matrix_is_multiply_compatible(a, b);
}

// ================================================================================
// Row / column swaps
// ================================================================================

error_code_t swap_float_matrix_rows(float_matrix_t* mat,
                                    size_t          r1,
                                    size_t          r2) {
    return swap_matrix_rows(mat, r1, r2);
}

// --------------------------------------------------------------------------------

error_code_t swap_float_matrix_cols(float_matrix_t* mat,
                                    size_t          c1,
                                    size_t          c2) {
    return swap_matrix_cols(mat, c1, c2);
}

// ================================================================================
// Special matrix constructors
// ================================================================================

float_matrix_expect_t init_float_identity_matrix(size_t             n,
                                                 allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_identity_matrix(n, FLOAT_TYPE, alloc_v)
    );
}

// --------------------------------------------------------------------------------

float_matrix_expect_t init_float_row_vector(size_t             length,
                                            allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_row_vector(length, FLOAT_TYPE, alloc_v)
    );
}

// --------------------------------------------------------------------------------

float_matrix_expect_t init_float_col_vector(size_t             length,
                                            allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_col_vector(length, FLOAT_TYPE, alloc_v)
    );
}

// ================================================================================
// Vector shape queries
// ================================================================================

bool float_matrix_is_row_vector(const float_matrix_t* mat) {
    return matrix_is_row_vector(mat);
}

// --------------------------------------------------------------------------------

bool float_matrix_is_col_vector(const float_matrix_t* mat) {
    return matrix_is_col_vector(mat);
}

// --------------------------------------------------------------------------------

bool float_matrix_is_vector(const float_matrix_t* mat) {
    return matrix_is_vector(mat);
}

// --------------------------------------------------------------------------------

size_t float_matrix_vector_length(const float_matrix_t* mat) {
    return matrix_vector_length(mat);
}
// ================================================================================
// convert_float_matrix_zero
// ================================================================================

// ================================================================================
// Internal helper
// ================================================================================

static inline bool _float_value_is_zero(float value, float_zero_fn is_zero) {
    if (is_zero != NULL) {
        return is_zero(value);
    }
    return (value == 0.0f);
}

// ================================================================================
// Semantic-zero conversion for float matrices
// ================================================================================

float_matrix_expect_t convert_float_matrix_zero(const float_matrix_t* src,
                                                matrix_format_t       target,
                                                allocator_vtable_t    alloc_v,
                                                float_zero_fn         is_zero) {
    if (src == NULL) {
        return (float_matrix_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }

    /*
     * The semantic-zero callback only matters for dense -> sparse conversion.
     * For all other cases, delegate to the normal float conversion path.
     */
    if (src->format != DENSE_MATRIX ||
        (target != COO_MATRIX && target != CSR_MATRIX && target != CSC_MATRIX)) {
        return convert_float_matrix(src, target, alloc_v);
    }

    /* Validate allocator interface needed by the direct allocation path */
    if (alloc_v.allocate == NULL || alloc_v.return_element == NULL) {
        return (float_matrix_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }

    /* Defensive validation of dense backing storage */
    if ((src->rows > 0u) && (src->cols > 0u) && (src->rep.dense.data == NULL)) {
        return (float_matrix_expect_t){
            .has_value = false,
            .u.error   = ILLEGAL_STATE
        };
    }

    /* Overflow check for rows * cols */
    if ((src->rows != 0u) && (src->cols > (SIZE_MAX / src->rows))) {
        return (float_matrix_expect_t){
            .has_value = false,
            .u.error   = LENGTH_OVERFLOW
        };
    }

    const float* data  = (const float*)src->rep.dense.data;
    size_t total       = src->rows * src->cols;
    size_t nnz         = 0u;

    /* Pass 1: count semantic nonzeros */
    for (size_t i = 0u; i < total; ++i) {
        if (!_float_value_is_zero(data[i], is_zero)) {
            ++nnz;
        }
    }

    // ----------------------------------------------------------------------------
    // dense -> COO
    // ----------------------------------------------------------------------------
    if (target == COO_MATRIX) {
        float_matrix_expect_t r = init_float_coo_matrix(
            src->rows, src->cols, (nnz > 0u) ? nnz : 1u, false, alloc_v
        );
        if (!r.has_value) {
            return r;
        }

        float_matrix_t* dst = r.u.value;

        for (size_t row = 0u; row < src->rows; ++row) {
            for (size_t col = 0u; col < src->cols; ++col) {
                float value = data[row * src->cols + col];
                if (!_float_value_is_zero(value, is_zero)) {
                    error_code_t ec = push_back_float_coo_matrix(dst, row, col, value);
                    if (ec != NO_ERROR) {
                        return_float_matrix(dst);
                        return (float_matrix_expect_t){
                            .has_value = false,
                            .u.error   = ec
                        };
                    }
                }
            }
        }

        /*
         * Row-major traversal naturally inserts in sorted order, but make the
         * intent explicit and robust.
         */
        (void)sort_float_coo_matrix(dst);
        return r;
    }

    // ----------------------------------------------------------------------------
    // dense -> CSR
    // ----------------------------------------------------------------------------
    if (target == CSR_MATRIX) {
        size_t row_ptr_bytes = 0u;
        size_t col_idx_bytes = 0u;
        size_t values_bytes  = 0u;

        void_ptr_expect_t mr;
        void_ptr_expect_t rp_r;
        void_ptr_expect_t ci_r;
        void_ptr_expect_t vl_r;

        size_t* row_ptr = NULL;
        size_t* col_idx = NULL;
        float*  values  = NULL;
        float_matrix_t* dst = NULL;

        if ((src->rows + 1u) < src->rows) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if ((src->rows + 1u) > (SIZE_MAX / sizeof(size_t))) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if (nnz > (SIZE_MAX / sizeof(size_t)) ||
            nnz > (SIZE_MAX / sizeof(float))) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }

        row_ptr_bytes = (src->rows + 1u) * sizeof(size_t);
        col_idx_bytes = nnz * sizeof(size_t);
        values_bytes  = nnz * sizeof(float);

        mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
        if (!mr.has_value) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = BAD_ALLOC
            };
        }

        dst = (float_matrix_t*)mr.u.value;
        dst->rows      = src->rows;
        dst->cols      = src->cols;
        dst->dtype     = FLOAT_TYPE;
        dst->data_size = sizeof(float);
        dst->format    = CSR_MATRIX;
        dst->alloc_v   = alloc_v;

        dst->rep.csr.nnz     = 0u;
        dst->rep.csr.row_ptr = NULL;
        dst->rep.csr.col_idx = NULL;
        dst->rep.csr.values  = NULL;

        rp_r = alloc_v.allocate(alloc_v.ctx, row_ptr_bytes, true);
        ci_r = alloc_v.allocate(alloc_v.ctx, (nnz > 0u) ? col_idx_bytes : 1u, true);
        vl_r = alloc_v.allocate(alloc_v.ctx, (nnz > 0u) ? values_bytes  : 1u, true);

        if (!rp_r.has_value || !ci_r.has_value || !vl_r.has_value) {
            if (rp_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, rp_r.u.value);
            }
            if (ci_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, ci_r.u.value);
            }
            if (vl_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, vl_r.u.value);
            }
            alloc_v.return_element(alloc_v.ctx, dst);

            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = OUT_OF_MEMORY
            };
        }

        row_ptr = (size_t*)rp_r.u.value;
        col_idx = (size_t*)ci_r.u.value;
        values  = (float*)vl_r.u.value;

        row_ptr[0] = 0u;

        size_t k = 0u;
        for (size_t row = 0u; row < src->rows; ++row) {
            for (size_t col = 0u; col < src->cols; ++col) {
                float value = data[row * src->cols + col];
                if (!_float_value_is_zero(value, is_zero)) {
                    col_idx[k] = col;
                    values[k]  = value;
                    ++k;
                }
            }
            row_ptr[row + 1u] = k;
        }

        dst->rep.csr.nnz     = k;
        dst->rep.csr.row_ptr = row_ptr;
        dst->rep.csr.col_idx = col_idx;
        dst->rep.csr.values  = (uint8_t*)values;

        return (float_matrix_expect_t){
            .has_value = true,
            .u.value   = dst
        };
    }

    // ----------------------------------------------------------------------------
    // dense -> CSC
    // ----------------------------------------------------------------------------
    if (target == CSC_MATRIX) {
        size_t col_ptr_bytes = 0u;
        size_t row_idx_bytes = 0u;
        size_t values_bytes  = 0u;

        void_ptr_expect_t mr;
        void_ptr_expect_t cp_r;
        void_ptr_expect_t ri_r;
        void_ptr_expect_t vl_r;

        size_t* col_ptr = NULL;
        size_t* row_idx = NULL;
        float*  values  = NULL;
        float_matrix_t* dst = NULL;

        if ((src->cols + 1u) < src->cols) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if ((src->cols + 1u) > (SIZE_MAX / sizeof(size_t))) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if (nnz > (SIZE_MAX / sizeof(size_t)) ||
            nnz > (SIZE_MAX / sizeof(float))) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }

        col_ptr_bytes = (src->cols + 1u) * sizeof(size_t);
        row_idx_bytes = nnz * sizeof(size_t);
        values_bytes  = nnz * sizeof(float);

        mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
        if (!mr.has_value) {
            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = BAD_ALLOC
            };
        }

        dst = (float_matrix_t*)mr.u.value;
        dst->rows      = src->rows;
        dst->cols      = src->cols;
        dst->dtype     = FLOAT_TYPE;
        dst->data_size = sizeof(float);
        dst->format    = CSC_MATRIX;
        dst->alloc_v   = alloc_v;

        dst->rep.csc.nnz     = 0u;
        dst->rep.csc.col_ptr = NULL;
        dst->rep.csc.row_idx = NULL;
        dst->rep.csc.values  = NULL;

        cp_r = alloc_v.allocate(alloc_v.ctx, col_ptr_bytes, true);
        ri_r = alloc_v.allocate(alloc_v.ctx, (nnz > 0u) ? row_idx_bytes : 1u, true);
        vl_r = alloc_v.allocate(alloc_v.ctx, (nnz > 0u) ? values_bytes  : 1u, true);

        if (!cp_r.has_value || !ri_r.has_value || !vl_r.has_value) {
            if (cp_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, cp_r.u.value);
            }
            if (ri_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, ri_r.u.value);
            }
            if (vl_r.has_value) {
                alloc_v.return_element(alloc_v.ctx, vl_r.u.value);
            }
            alloc_v.return_element(alloc_v.ctx, dst);

            return (float_matrix_expect_t){
                .has_value = false,
                .u.error   = OUT_OF_MEMORY
            };
        }

        col_ptr = (size_t*)cp_r.u.value;
        row_idx = (size_t*)ri_r.u.value;
        values  = (float*)vl_r.u.value;

        col_ptr[0] = 0u;

        size_t k = 0u;
        for (size_t col = 0u; col < src->cols; ++col) {
            for (size_t row = 0u; row < src->rows; ++row) {
                float value = data[row * src->cols + col];
                if (!_float_value_is_zero(value, is_zero)) {
                    row_idx[k] = row;
                    values[k]  = value;
                    ++k;
                }
            }
            col_ptr[col + 1u] = k;
        }

        dst->rep.csc.nnz     = k;
        dst->rep.csc.col_ptr = col_ptr;
        dst->rep.csc.row_idx = row_idx;
        dst->rep.csc.values  = (uint8_t*)values;

        return (float_matrix_expect_t){
            .has_value = true,
            .u.value   = dst
        };
    }

    return (float_matrix_expect_t){
        .has_value = false,
        .u.error   = ILLEGAL_STATE
    };
}
// ================================================================================
// ================================================================================
// eof
// ================================================================================
// ================================================================================
// eof
