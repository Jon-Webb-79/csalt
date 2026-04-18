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
#include <inttypes.h>

#include "c_uint16.h"

#if defined(__AVX512F__)
#  include "simd_avx512_uint16.inl"
#elif defined(__AVX2__)
#  include "simd_avx2_uint16.inl"
#elif defined(__AVX__)
#  include "simd_avx_uint16.inl"
#elif defined(__SSE4_1__)
#  include "simd_sse41_uint16.inl"
#elif defined(__SSE3__)
#  include "simd_sse3_uint16.inl"
#elif defined(__SSE2__)
#  include "simd_sse2_uint16.inl"
#elif defined(__ARM_FEATURE_SVE2)
#  include "simd_sve2_uint16.inl"
#elif defined(__ARM_FEATURE_SVE)
#  include "simd_sve_uint16.inl"
#elif defined(__ARM_NEON)
#  include "simd_neon_uint16.inl"
#else
#  include "simd_scalar_uint16.inl"
#endif
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
// -------------------------------------------------------------------------------- 

uint16_expect_t uint16_array_min(const uint16_array_t* array) {
    if (array == NULL)
        return (uint16_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_min(&array->base, _cmp_uint16, UINT16_TYPE);
    if (!idx.has_value)
        return (uint16_expect_t){ .has_value = false, .u.error = idx.u.error };
    uint16_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, UINT16_TYPE);
    if (err != NO_ERROR)
        return (uint16_expect_t){ .has_value = false, .u.error = err };
    return (uint16_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

uint16_expect_t uint16_array_max(const uint16_array_t* array) {
    if (array == NULL)
        return (uint16_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_max(&array->base, _cmp_uint16, UINT16_TYPE);
    if (!idx.has_value)
        return (uint16_expect_t){ .has_value = false, .u.error = idx.u.error };
    uint16_t val = 0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, UINT16_TYPE);
    if (err != NO_ERROR)
        return (uint16_expect_t){ .has_value = false, .u.error = err };
    return (uint16_expect_t){ .has_value = true, .u.value = val };
}
// -------------------------------------------------------------------------------- 

error_code_t print_uint16_array(const uint16_array_t* array, FILE* stream) {
    if (array == NULL || stream == NULL) return NULL_POINTER;

    const uint16_t* data = (const uint16_t*)array->base.data;
    size_t len = array->base.len;
    size_t col = 0u;

    fputs("[ ", stream);
    col = 2u;

    if (len == 0u) {
        fputs("]", stream);
        return NO_ERROR;
    }

    for (size_t i = 0u; i < len; ++i) {
        char buf[16];
        int n = snprintf(buf, sizeof(buf), "%" PRIu16, data[i]);
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
// -------------------------------------------------------------------------------- 

bool uint16_array_equal(const uint16_array_t* a,
                        const uint16_array_t* b) {
    if (a == NULL || b == NULL) {
        return false;
    }

    if (a == b) {
        return true;
    }

    if (a->base.len != b->base.len) {
        return false;
    }

    if (a->base.len == 0u) {
        return true;
    }

    return simd_uint16_arrays_equal(
        (const uint16_t*)a->base.data,
        (const uint16_t*)b->base.data,
        a->base.len
    );
}
// ================================================================================ 
// ================================================================================ 

static inline dict_key_t _key(const char* s, size_t len) {
    return (dict_key_t){ .data = s, .len = len };
}
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
uint16_dict_expect_t init_uint16_dict(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v) {
    dict_expect_t r = init_dict(capacity, sizeof(uint16_t),
                                UINT16_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (uint16_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (uint16_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
void return_uint16_dict(uint16_dict_t* dict) {
    return_dict(dict);
}
 
// ================================================================================
// Insert
// ================================================================================
 
error_code_t insert_uint16_dict(uint16_dict_t*     dict,
                                 const char*        key,
                                 uint16_t           value,
                                 allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    return insert_dict(dict, _key(key, strlen(key)), &value, alloc_v);
}
 
// --------------------------------------------------------------------------------
 
error_code_t insert_uint16_dict_n(uint16_dict_t*     dict,
                                   const char*        key,
                                   size_t             key_len,
                                   uint16_t           value,
                                   allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return insert_dict(dict, _key(key, key_len), &value, alloc_v);
}
 
// ================================================================================
// Pop
// ================================================================================
 
error_code_t pop_uint16_dict(uint16_dict_t* dict,
                              const char*    key,
                              uint16_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    return pop_dict(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t pop_uint16_dict_n(uint16_dict_t* dict,
                                const char*    key,
                                size_t         key_len,
                                uint16_t*      out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return pop_dict(dict, _key(key, key_len), out_value);
}
 
// ================================================================================
// Update
// ================================================================================
 
error_code_t update_uint16_dict(uint16_dict_t* dict,
                                 const char*    key,
                                 uint16_t       value) {
    if (key == NULL) return NULL_POINTER;
    return update_dict(dict, _key(key, strlen(key)), &value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t update_uint16_dict_n(uint16_dict_t* dict,
                                   const char*    key,
                                   size_t         key_len,
                                   uint16_t       value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return update_dict(dict, _key(key, key_len), &value);
}
 
// ================================================================================
// Lookup
// ================================================================================
 
error_code_t get_uint16_dict_value(const uint16_dict_t* dict,
                                    const char*          key,
                                    uint16_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    return get_dict_value(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t get_uint16_dict_value_n(const uint16_dict_t* dict,
                                      const char*          key,
                                      size_t               key_len,
                                      uint16_t*            out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return get_dict_value(dict, _key(key, key_len), out_value);
}
 
// --------------------------------------------------------------------------------
 
const uint16_t* get_uint16_dict_ptr(const uint16_dict_t* dict, const char* key) {
    if (key == NULL) return NULL;
    return (const uint16_t*)get_dict_value_ptr(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
const uint16_t* get_uint16_dict_ptr_n(const uint16_dict_t* dict,
                                       const char*          key,
                                       size_t               key_len) {
    if (key == NULL || key_len == 0u) return NULL;
    return (const uint16_t*)get_dict_value_ptr(dict, _key(key, key_len));
}
 
// --------------------------------------------------------------------------------
 
bool has_uint16_dict_key(const uint16_dict_t* dict, const char* key) {
    if (key == NULL) return false;
    return has_dict_key(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
bool has_uint16_dict_key_n(const uint16_dict_t* dict,
                            const char*          key,
                            size_t               key_len) {
    if (key == NULL || key_len == 0u) return false;
    return has_dict_key(dict, _key(key, key_len));
}
 
// ================================================================================
// Utility
// ================================================================================
 
error_code_t clear_uint16_dict(uint16_dict_t* dict) {
    return clear_dict(dict);
}
 
// --------------------------------------------------------------------------------
 
uint16_dict_expect_t copy_uint16_dict(const uint16_dict_t* src,
                                      allocator_vtable_t   alloc_v) {
    dict_expect_t r = copy_dict(src, alloc_v);
    if (!r.has_value)
        return (uint16_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (uint16_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
uint16_dict_expect_t merge_uint16_dict(const uint16_dict_t* a,
                                       const uint16_dict_t* b,
                                       bool                 overwrite,
                                       allocator_vtable_t   alloc_v) {
    dict_expect_t r = merge_dict(a, b, overwrite, alloc_v);
    if (!r.has_value)
        return (uint16_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (uint16_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// ================================================================================
// Iteration
// ================================================================================
 
typedef struct {
    uint16_dict_iter_fn typed_fn;
    void*               user_data;
} _uint16_iter_ctx_t;
 
static void _uint16_iter_shim(dict_entry_t e, void* ctx) {
    const _uint16_iter_ctx_t* c = (const _uint16_iter_ctx_t*)ctx;
    uint16_t value;
    memcpy(&value, e.value, sizeof(uint16_t));
    c->typed_fn((const char*)e.key, e.key_len, value, c->user_data);
}
 
error_code_t foreach_uint16_dict(const uint16_dict_t* dict,
                                  uint16_dict_iter_fn  fn,
                                  void*                user_data) {
    if (fn == NULL) return NULL_POINTER;
    _uint16_iter_ctx_t ctx = { .typed_fn = fn, .user_data = user_data };
    return foreach_dict(dict, _uint16_iter_shim, &ctx);
}
 
// ================================================================================
// Introspection
// ================================================================================
 
size_t uint16_dict_size(const uint16_dict_t* dict) {
    return dict_size(dict);
}
 
size_t uint16_dict_hash_size(const uint16_dict_t* dict) {
    return dict_hash_size(dict);
}
 
size_t uint16_dict_alloc(const uint16_dict_t* dict) {
    return dict_alloc(dict);
}
 
bool is_uint16_dict_empty(const uint16_dict_t* dict) {
    return is_dict_empty(dict);
}
// -------------------------------------------------------------------------------- 

typedef struct {
    FILE*  stream;
    size_t col;
    bool   first;
} _uint16_dict_print_ctx_t;

static void _print_uint16_dict_entry(const char* key,
                                     size_t      key_len,
                                     uint16_t    value,
                                     void*       user_data) {
    _uint16_dict_print_ctx_t* ctx = (_uint16_dict_print_ctx_t*)user_data;
    char val_buf[16];
    int val_n = snprintf(val_buf, sizeof(val_buf), "%" PRIu16, value);
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

error_code_t print_uint16_dict(const uint16_dict_t* dict, FILE* stream) {
    if (dict == NULL || stream == NULL) return NULL_POINTER;

    _uint16_dict_print_ctx_t ctx;
    ctx.stream = stream;
    ctx.col    = 2u;
    ctx.first  = true;

    fputs("{ ", stream);

    error_code_t err = foreach_uint16_dict(dict, _print_uint16_dict_entry, &ctx);
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
// uint16_matrix_t — type-safe uint16 wrapper implementations
//
// Every function below is a thin delegation to the corresponding generic
// matrix_t function, fixing the dtype to UINT16_TYPE and converting between
// uint16 / uint16* and the void* interface expected by c_matrix.h.
// ================================================================================
// ================================================================================

// ================================================================================
// Internal helper: rewrap matrix_expect_t as uint16_matrix_expect_t
// ================================================================================

static inline uint16_matrix_expect_t _wrap_matrix_expect(matrix_expect_t e) {
    if (e.has_value) {
        return (uint16_matrix_expect_t){
            .has_value = true,
            .u.value   = (uint16_matrix_t*)e.u.value
        };
    }
    return (uint16_matrix_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

uint16_matrix_expect_t init_uint16_dense_matrix(size_t             rows,
                                              size_t             cols,
                                              allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_dense_matrix(rows, cols, UINT16_TYPE, alloc_v)
    );
}

// --------------------------------------------------------------------------------

uint16_matrix_expect_t init_uint16_coo_matrix(size_t             rows,
                                            size_t             cols,
                                            size_t             capacity,
                                            bool               growth,
                                            allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_coo_matrix(rows, cols, capacity, UINT16_TYPE, growth, alloc_v)
    );
}

// --------------------------------------------------------------------------------

void return_uint16_matrix(uint16_matrix_t* mat) {
    return_matrix(mat);
}

// ================================================================================
// Element access
// ================================================================================

error_code_t get_uint16_matrix(const uint16_matrix_t* mat,
                              size_t                row,
                              size_t                col,
                              uint16_t*               out) {
    if (mat == NULL || out == NULL) return NULL_POINTER;
    return get_matrix(mat, row, col, out);
}

// --------------------------------------------------------------------------------

error_code_t set_uint16_matrix(uint16_matrix_t* mat,
                              size_t          row,
                              size_t          col,
                              uint16_t         value) {
    if (mat == NULL) return NULL_POINTER;
    return set_matrix(mat, row, col, &value);
}

// ================================================================================
// COO assembly helpers
// ================================================================================

error_code_t reserve_uint16_coo_matrix(uint16_matrix_t* mat,
                                      size_t          capacity) {
    return reserve_coo_matrix(mat, capacity);
}

// --------------------------------------------------------------------------------

error_code_t push_back_uint16_coo_matrix(uint16_matrix_t* mat,
                                        size_t          row,
                                        size_t          col,
                                        uint16_t         value) {
    if (mat == NULL) return NULL_POINTER;
    return push_back_coo_matrix(mat, row, col, &value);
}

// --------------------------------------------------------------------------------

error_code_t sort_uint16_coo_matrix(uint16_matrix_t* mat) {
    return sort_coo_matrix(mat);
}

// ================================================================================
// Lifecycle / structural operations
// ================================================================================

error_code_t clear_uint16_matrix(uint16_matrix_t* mat) {
    return clear_matrix(mat);
}

// --------------------------------------------------------------------------------

uint16_matrix_expect_t copy_uint16_matrix(const uint16_matrix_t* src,
                                        allocator_vtable_t    alloc_v) {
    return _wrap_matrix_expect(copy_matrix(src, alloc_v));
}

// --------------------------------------------------------------------------------

uint16_matrix_expect_t convert_uint16_matrix(const uint16_matrix_t* src,
                                           matrix_format_t       target,
                                           allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (uint16_matrix_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }

    /* SIMD fast path: dense uint16 -> CSR */
    if (src->format == DENSE_MATRIX && target == CSR_MATRIX) {
        const uint16_t* data = NULL;
        size_t total = 0u;
        size_t nnz = 0u;
        size_t row_ptr_bytes = 0u;
        size_t col_idx_bytes = 0u;
        size_t values_bytes = 0u;
        size_t* row_ptr = NULL;
        size_t* col_idx = NULL;
        uint16_t* values = NULL;
        uint16_matrix_t* dst = NULL;
        void_ptr_expect_t mr;
        void_ptr_expect_t rp_r;
        void_ptr_expect_t ci_r;
        void_ptr_expect_t vl_r;

        /* Validate allocator interface required by this fast path */
        if (alloc_v.allocate == NULL || alloc_v.return_element == NULL) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = NULL_POINTER
            };
        }

        /* Defensive validation of dense backing storage */
        if ((src->rows > 0u) && (src->cols > 0u) && (src->rep.dense.data == NULL)) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = ILLEGAL_STATE
            };
        }

        /* Overflow check for rows * cols */
        if ((src->rows != 0u) && (src->cols > (SIZE_MAX / src->rows))) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        total = src->rows * src->cols;
        data = (const uint16_t*)src->rep.dense.data;

        /* Pass 1: count nonzeros (SIMD-accelerated) */
        nnz = simd_count_nonzero_uint16(data, total);

        /* Overflow checks for byte counts */
        if ((src->rows + 1u) < src->rows) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if ((src->rows + 1u) > (SIZE_MAX / sizeof(size_t))) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if (nnz > (SIZE_MAX / sizeof(size_t))) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if (nnz > (SIZE_MAX / sizeof(uint16_t))) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }

        /* Allocate the matrix header */
        mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
        if (!mr.has_value) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = BAD_ALLOC
            };
        }

        dst = (uint16_matrix_t*)mr.u.value;
        dst->rows      = src->rows;
        dst->cols      = src->cols;
        dst->dtype     = UINT16_TYPE;
        dst->data_size = sizeof(uint16_t);
        dst->format    = CSR_MATRIX;
        dst->alloc_v   = alloc_v;

        dst->rep.csr.nnz     = 0u;
        dst->rep.csr.row_ptr = NULL;
        dst->rep.csr.col_idx = NULL;
        dst->rep.csr.values  = NULL;

        row_ptr_bytes = (src->rows + 1u) * sizeof(size_t);
        col_idx_bytes = nnz * sizeof(size_t);
        values_bytes  = nnz * sizeof(uint16_t);

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

            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = OUT_OF_MEMORY
            };
        }

        row_ptr = (size_t*)rp_r.u.value;
        col_idx = (size_t*)ci_r.u.value;
        values  = (uint16_t*)vl_r.u.value;

        /* Pass 2: scatter nonzeros into CSR arrays */
        row_ptr[0] = 0u;
        if (nnz > 0u) {
            size_t k = 0u;

            for (size_t i = 0u; i < src->rows; ++i) {
                const uint16_t* row_data = data + (i * src->cols);
                k = simd_scatter_csr_row_uint16(
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

        return (uint16_matrix_expect_t){
            .has_value = true,
            .u.value   = dst
        };
    }

    /* All other conversions: delegate to generic path */
    return _wrap_matrix_expect(convert_matrix(src, target, alloc_v));
}

// --------------------------------------------------------------------------------

uint16_matrix_expect_t transpose_uint16_matrix(const uint16_matrix_t* src,
                                             allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (uint16_matrix_expect_t){
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
    uint16_matrix_expect_t r = init_uint16_dense_matrix(
        src->cols, src->rows, alloc_v
    );
    if (!r.has_value) return r;
 
    /* SIMD fast path: dense uint16 transpose */
    const uint16_t* sdata = (const uint16_t*)src->rep.dense.data;
    uint16_t*       ddata = (uint16_t*)r.u.value->rep.dense.data;
    simd_transpose_uint16(sdata, ddata, src->rows, src->cols);
 
    return r;
}
// ================================================================================
// Fill and zero
// ================================================================================

error_code_t fill_uint16_matrix(uint16_matrix_t* mat,
                               uint16_t         value) {
    if (mat == NULL) return NULL_POINTER;
 
    /* Zero fill: delegate to clear_matrix for all formats */
    if (value == 0u) return clear_matrix(mat);
 
    /* Non-zero fill on non-dense: fall back to generic path */
    if (mat->format != DENSE_MATRIX) return fill_matrix(mat, &value);
 
    /* SIMD fast path: dense uint16 fill */
    uint16_t* data  = (uint16_t*)mat->rep.dense.data;
    size_t count = mat->rows * mat->cols;
    simd_fill_uint16(data, count, value);
    return NO_ERROR;
}
// ================================================================================
// Shape and compatibility queries
// ================================================================================

bool uint16_matrix_has_same_shape(const uint16_matrix_t* a,
                                 const uint16_matrix_t* b) {
    return matrix_has_same_shape(a, b);
}

// --------------------------------------------------------------------------------

bool uint16_matrix_is_square(const uint16_matrix_t* mat) {
    return matrix_is_square(mat);
}

// --------------------------------------------------------------------------------

bool uint16_matrix_is_sparse(const uint16_matrix_t* mat) {
    return matrix_is_sparse(mat);
}

// --------------------------------------------------------------------------------

bool uint16_matrix_is_zero(const uint16_matrix_t* mat) {
    if (mat == NULL) return true;

    /* SIMD fast path: dense uint16 */
    if (mat->format == DENSE_MATRIX) {
        const uint16_t* data = (const uint16_t*)mat->rep.dense.data;
        size_t count = mat->rows * mat->cols;
        return simd_is_all_zero_uint16(data, count);
    }

    /* Sparse formats: delegate to generic path */
    return is_zero_matrix(mat);
}

// --------------------------------------------------------------------------------

bool uint16_matrix_equal(const uint16_matrix_t* a,
                        const uint16_matrix_t* b) {
    if (a == NULL || b == NULL) return false;
    if (!matrix_has_same_shape((const matrix_t*)a, (const matrix_t*)b)) return false;
    if (a->dtype != UINT16_TYPE || b->dtype != UINT16_TYPE) return false;

    /* Fast path: both dense */
    if (a->format == DENSE_MATRIX && b->format == DENSE_MATRIX) {
        const uint16_t* da = (const uint16_t*)a->rep.dense.data;
        const uint16_t* db = (const uint16_t*)b->rep.dense.data;
        return simd_uint16_arrays_equal(da, db, a->rows * a->cols);
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

            if (!simd_uint16_arrays_equal(
                    (const uint16_t*)a->rep.coo.values,
                    (const uint16_t*)b->rep.coo.values,
                    nnz)) {
                return false;
            }
        }

        return true;
    }

    /* General logical comparison */
    for (size_t i = 0u; i < a->rows; ++i) {
        for (size_t j = 0u; j < a->cols; ++j) {
            uint16_t va = 0u;
            uint16_t vb = 0u;

            if (get_matrix((const matrix_t*)a, i, j, &va) != NO_ERROR) return false;
            if (get_matrix((const matrix_t*)b, i, j, &vb) != NO_ERROR) return false;

            if (!(va == vb)) return false;
        }
    }

    return true;
}
// --------------------------------------------------------------------------------

bool uint16_matrix_is_add_compatible(const uint16_matrix_t* a,
                                    const uint16_matrix_t* b) {
    return matrix_is_add_compatible(a, b);
}

// --------------------------------------------------------------------------------

bool uint16_matrix_is_multiply_compatible(const uint16_matrix_t* a,
                                         const uint16_matrix_t* b) {
    return matrix_is_multiply_compatible(a, b);
}

// ================================================================================
// Row / column swaps
// ================================================================================

error_code_t swap_uint16_matrix_rows(uint16_matrix_t* mat,
                                    size_t          r1,
                                    size_t          r2) {
    return swap_matrix_rows(mat, r1, r2);
}

// --------------------------------------------------------------------------------

error_code_t swap_uint16_matrix_cols(uint16_matrix_t* mat,
                                    size_t          c1,
                                    size_t          c2) {
    return swap_matrix_cols(mat, c1, c2);
}

// ================================================================================
// Special matrix constructors
// ================================================================================

uint16_matrix_expect_t init_uint16_identity_matrix(size_t             n,
                                                 allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_identity_matrix(n, UINT16_TYPE, alloc_v)
    );
}

// --------------------------------------------------------------------------------

uint16_matrix_expect_t init_uint16_row_vector(size_t             length,
                                            allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_row_vector(length, UINT16_TYPE, alloc_v)
    );
}

// --------------------------------------------------------------------------------

uint16_matrix_expect_t init_uint16_col_vector(size_t             length,
                                            allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_col_vector(length, UINT16_TYPE, alloc_v)
    );
}

// ================================================================================
// Vector shape queries
// ================================================================================

bool uint16_matrix_is_row_vector(const uint16_matrix_t* mat) {
    return matrix_is_row_vector(mat);
}

// --------------------------------------------------------------------------------

bool uint16_matrix_is_col_vector(const uint16_matrix_t* mat) {
    return matrix_is_col_vector(mat);
}

// --------------------------------------------------------------------------------

bool uint16_matrix_is_vector(const uint16_matrix_t* mat) {
    return matrix_is_vector(mat);
}

// --------------------------------------------------------------------------------

size_t uint16_matrix_vector_length(const uint16_matrix_t* mat) {
    return matrix_vector_length(mat);
}
// ================================================================================
// convert_uint16_matrix_zero
// ================================================================================

// ================================================================================
// Internal helper
// ================================================================================

static inline bool _uint16_value_is_zero(uint16_t value, uint16_zero_fn is_zero) {
    if (is_zero != NULL) {
        return is_zero(value);
    }
    return (value == 0u);
}

// ================================================================================
// Semantic-zero conversion for uint16 matrices
// ================================================================================

uint16_matrix_expect_t convert_uint16_matrix_zero(const uint16_matrix_t* src,
                                                matrix_format_t       target,
                                                allocator_vtable_t    alloc_v,
                                                uint16_zero_fn         is_zero) {
    if (src == NULL) {
        return (uint16_matrix_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }

    /*
     * The semantic-zero callback only matters for dense -> sparse conversion.
     * For all other cases, delegate to the normal uint16 conversion path.
     */
    if (src->format != DENSE_MATRIX ||
        (target != COO_MATRIX && target != CSR_MATRIX && target != CSC_MATRIX)) {
        return convert_uint16_matrix(src, target, alloc_v);
    }

    /* Validate allocator interface needed by the direct allocation path */
    if (alloc_v.allocate == NULL || alloc_v.return_element == NULL) {
        return (uint16_matrix_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }

    /* Defensive validation of dense backing storage */
    if ((src->rows > 0u) && (src->cols > 0u) && (src->rep.dense.data == NULL)) {
        return (uint16_matrix_expect_t){
            .has_value = false,
            .u.error   = ILLEGAL_STATE
        };
    }

    /* Overflow check for rows * cols */
    if ((src->rows != 0u) && (src->cols > (SIZE_MAX / src->rows))) {
        return (uint16_matrix_expect_t){
            .has_value = false,
            .u.error   = LENGTH_OVERFLOW
        };
    }

    const uint16_t* data  = (const uint16_t*)src->rep.dense.data;
    size_t total       = src->rows * src->cols;
    size_t nnz         = 0u;

    /* Pass 1: count semantic nonzeros */
    for (size_t i = 0u; i < total; ++i) {
        if (!_uint16_value_is_zero(data[i], is_zero)) {
            ++nnz;
        }
    }

    // ----------------------------------------------------------------------------
    // dense -> COO
    // ----------------------------------------------------------------------------
    if (target == COO_MATRIX) {
        uint16_matrix_expect_t r = init_uint16_coo_matrix(
            src->rows, src->cols, (nnz > 0u) ? nnz : 1u, false, alloc_v
        );
        if (!r.has_value) {
            return r;
        }

        uint16_matrix_t* dst = r.u.value;

        for (size_t row = 0u; row < src->rows; ++row) {
            for (size_t col = 0u; col < src->cols; ++col) {
                uint16_t value = data[row * src->cols + col];
                if (!_uint16_value_is_zero(value, is_zero)) {
                    error_code_t ec = push_back_uint16_coo_matrix(dst, row, col, value);
                    if (ec != NO_ERROR) {
                        return_uint16_matrix(dst);
                        return (uint16_matrix_expect_t){
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
        (void)sort_uint16_coo_matrix(dst);
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
        uint16_t*  values  = NULL;
        uint16_matrix_t* dst = NULL;

        if ((src->rows + 1u) < src->rows) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if ((src->rows + 1u) > (SIZE_MAX / sizeof(size_t))) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if (nnz > (SIZE_MAX / sizeof(size_t)) ||
            nnz > (SIZE_MAX / sizeof(uint16_t))) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }

        row_ptr_bytes = (src->rows + 1u) * sizeof(size_t);
        col_idx_bytes = nnz * sizeof(size_t);
        values_bytes  = nnz * sizeof(uint16_t);

        mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
        if (!mr.has_value) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = BAD_ALLOC
            };
        }

        dst = (uint16_matrix_t*)mr.u.value;
        dst->rows      = src->rows;
        dst->cols      = src->cols;
        dst->dtype     = UINT16_TYPE;
        dst->data_size = sizeof(uint16_t);
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

            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = OUT_OF_MEMORY
            };
        }

        row_ptr = (size_t*)rp_r.u.value;
        col_idx = (size_t*)ci_r.u.value;
        values  = (uint16_t*)vl_r.u.value;

        row_ptr[0] = 0u;

        size_t k = 0u;
        for (size_t row = 0u; row < src->rows; ++row) {
            for (size_t col = 0u; col < src->cols; ++col) {
                uint16_t value = data[row * src->cols + col];
                if (!_uint16_value_is_zero(value, is_zero)) {
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

        return (uint16_matrix_expect_t){
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
        uint16_t*  values  = NULL;
        uint16_matrix_t* dst = NULL;

        if ((src->cols + 1u) < src->cols) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if ((src->cols + 1u) > (SIZE_MAX / sizeof(size_t))) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }
        if (nnz > (SIZE_MAX / sizeof(size_t)) ||
            nnz > (SIZE_MAX / sizeof(uint16_t))) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = LENGTH_OVERFLOW
            };
        }

        col_ptr_bytes = (src->cols + 1u) * sizeof(size_t);
        row_idx_bytes = nnz * sizeof(size_t);
        values_bytes  = nnz * sizeof(uint16_t);

        mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
        if (!mr.has_value) {
            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = BAD_ALLOC
            };
        }

        dst = (uint16_matrix_t*)mr.u.value;
        dst->rows      = src->rows;
        dst->cols      = src->cols;
        dst->dtype     = UINT16_TYPE;
        dst->data_size = sizeof(uint16_t);
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

            return (uint16_matrix_expect_t){
                .has_value = false,
                .u.error   = OUT_OF_MEMORY
            };
        }

        col_ptr = (size_t*)cp_r.u.value;
        row_idx = (size_t*)ri_r.u.value;
        values  = (uint16_t*)vl_r.u.value;

        col_ptr[0] = 0u;

        size_t k = 0u;
        for (size_t col = 0u; col < src->cols; ++col) {
            for (size_t row = 0u; row < src->rows; ++row) {
                uint16_t value = data[row * src->cols + col];
                if (!_uint16_value_is_zero(value, is_zero)) {
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

        return (uint16_matrix_expect_t){
            .has_value = true,
            .u.value   = dst
        };
    }

    return (uint16_matrix_expect_t){
        .has_value = false,
        .u.error   = ILLEGAL_STATE
    };
}
// -------------------------------------------------------------------------------- 

static error_code_t _print_uint16_dense_matrix(const uint16_matrix_t* mat,
                                               FILE*                  stream) {
    if (mat == NULL || stream == NULL) return NULL_POINTER;
    if (mat->format != DENSE_MATRIX)   return ILLEGAL_STATE;
    if ((mat->rows > 0u) && (mat->cols > 0u) && (mat->rep.dense.data == NULL)) {
        return ILLEGAL_STATE;
    }

    const uint16_t* data = (const uint16_t*)mat->rep.dense.data;

    if (fputs("[ ", stream) == EOF) return ILLEGAL_STATE;

    for (size_t i = 0u; i < mat->rows; ++i) {
        if (i == 0u) {
            if (fputs("[ ", stream) == EOF) return ILLEGAL_STATE;
        } else {
            if (fputs("\n  [ ", stream) == EOF) return ILLEGAL_STATE;
        }

        for (size_t j = 0u; j < mat->cols; ++j) {
            size_t idx = (i * mat->cols) + j;

            if (fprintf(stream, "%" PRIu16, data[idx]) < 0) {
                return ILLEGAL_STATE;
            }

            if (j + 1u < mat->cols) {
                if (fputs(", ", stream) == EOF) return ILLEGAL_STATE;
            }
        }

        if (fputs(" ]", stream) == EOF) return ILLEGAL_STATE;

        if (i + 1u < mat->rows) {
            if (fputc(',', stream) == EOF) return ILLEGAL_STATE;
        }
    }

    if (fputs(" ]", stream) == EOF) return ILLEGAL_STATE;

    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

static error_code_t _print_uint16_sparse_row_major(const uint16_matrix_t* mat,
                                                   FILE*                  stream) {
    if (mat == NULL || stream == NULL) return NULL_POINTER;

    size_t col = 0u;
    bool first = true;

    if (fputs("[ ", stream) == EOF) return ILLEGAL_STATE;
    col = 2u;

    for (size_t i = 0u; i < mat->rows; ++i) {
        for (size_t j = 0u; j < mat->cols; ++j) {
            uint16_t value = 0u;
            error_code_t err = get_uint16_matrix(mat, i, j, &value);
            if (err != NO_ERROR) return err;

            if (value == 0u) continue;

            char token[64];
            int n = snprintf(token, sizeof(token),
                             "(%zu, %zu): %" PRIu16, i, j, value);
            size_t token_len = (n > 0) ? (size_t)n : 0u;

            if (!first) {
                if ((col + 2u + token_len) > 70u) {
                    if (fputs(",\n  ", stream) == EOF) return ILLEGAL_STATE;
                    col = 2u;
                } else {
                    if (fputs(", ", stream) == EOF) return ILLEGAL_STATE;
                    col += 2u;
                }
            } else if ((col + token_len) > 70u) {
                if (fputc('\n', stream) == EOF) return ILLEGAL_STATE;
                if (fputs("  ", stream) == EOF) return ILLEGAL_STATE;
                col = 2u;
            }

            if (fputs(token, stream) == EOF) return ILLEGAL_STATE;
            col += token_len;
            first = false;
        }
    }

    if (first) {
        if (fputs("]", stream) == EOF) return ILLEGAL_STATE;
        return NO_ERROR;
    }

    if ((col + 2u) > 70u) {
        if (fputc('\n', stream) == EOF) return ILLEGAL_STATE;
        if (fputs("  ]", stream) == EOF) return ILLEGAL_STATE;
    } else {
        if (fputs(" ]", stream) == EOF) return ILLEGAL_STATE;
    }

    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

static error_code_t _print_uint16_coo_matrix(const uint16_matrix_t* mat,
                                             FILE*                  stream) {
    if (mat == NULL || stream == NULL) return NULL_POINTER;
    if (mat->format != COO_MATRIX)     return ILLEGAL_STATE;
    return _print_uint16_sparse_row_major(mat, stream);
}

// -----------------------------------------------------------------------------

static error_code_t _print_uint16_csr_matrix(const uint16_matrix_t* mat,
                                             FILE*                  stream) {
    if (mat == NULL || stream == NULL) return NULL_POINTER;
    if (mat->format != CSR_MATRIX)     return ILLEGAL_STATE;
    return _print_uint16_sparse_row_major(mat, stream);
}

// -----------------------------------------------------------------------------

static error_code_t _print_uint16_csc_matrix(const uint16_matrix_t* mat,
                                             FILE*                  stream) {
    if (mat == NULL || stream == NULL) return NULL_POINTER;
    if (mat->format != CSC_MATRIX)     return ILLEGAL_STATE;
    return _print_uint16_sparse_row_major(mat, stream);
}
// -------------------------------------------------------------------------------- 

error_code_t print_uint16_matrix(const uint16_matrix_t* mat, FILE* stream) {
    if (mat == NULL || stream == NULL) return NULL_POINTER;

    switch (mat->format) {
        case DENSE_MATRIX:
            return _print_uint16_dense_matrix(mat, stream);

        case COO_MATRIX:
            return _print_uint16_coo_matrix(mat, stream);

        case CSR_MATRIX:
            return _print_uint16_csr_matrix(mat, stream);

        case CSC_MATRIX:
            return _print_uint16_csc_matrix(mat, stream);

        default:
            return ILLEGAL_STATE;
    }
}
// -------------------------------------------------------------------------------- 

uint16_expect_t uint16_matrix_min(const uint16_matrix_t* mat) {
    if (mat == NULL) {
        return (uint16_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }

    size_expect_t idx = matrix_min(mat, _cmp_uint16, UINT16_TYPE);
    if (!idx.has_value) {
        return (uint16_expect_t){ .has_value = false, .u.error = idx.u.error };
    }

    switch (mat->format) {
        case DENSE_MATRIX: {
            const uint16_t* vals = (const uint16_t*)mat->rep.dense.data;
            uint16_t val = vals[idx.u.value];
            return (uint16_expect_t){ .has_value = true, .u.value = val };
        }

        case COO_MATRIX: {
            const uint16_t* vals = (const uint16_t*)mat->rep.coo.values;
            uint16_t val = vals[idx.u.value];
            return (uint16_expect_t){ .has_value = true, .u.value = val };
        }

        case CSR_MATRIX: {
            const uint16_t* vals = (const uint16_t*)mat->rep.csr.values;
            uint16_t val = vals[idx.u.value];
            return (uint16_expect_t){ .has_value = true, .u.value = val };
        }

        case CSC_MATRIX: {
            const uint16_t* vals = (const uint16_t*)mat->rep.csc.values;
            uint16_t val = vals[idx.u.value];
            return (uint16_expect_t){ .has_value = true, .u.value = val };
        }

        default:
            return (uint16_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    }
}
// -------------------------------------------------------------------------------- 

uint16_expect_t uint16_matrix_max(const uint16_matrix_t* mat) {
    if (mat == NULL) {
        return (uint16_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }

    size_expect_t idx = matrix_max(mat, _cmp_uint16, UINT16_TYPE);
    if (!idx.has_value) {
        return (uint16_expect_t){ .has_value = false, .u.error = idx.u.error };
    }

    switch (mat->format) {
        case DENSE_MATRIX: {
            const uint16_t* vals = (const uint16_t*)mat->rep.dense.data;
            uint16_t val = vals[idx.u.value];
            return (uint16_expect_t){ .has_value = true, .u.value = val };
        }

        case COO_MATRIX: {
            const uint16_t* vals = (const uint16_t*)mat->rep.coo.values;
            uint16_t val = vals[idx.u.value];
            return (uint16_expect_t){ .has_value = true, .u.value = val };
        }

        case CSR_MATRIX: {
            const uint16_t* vals = (const uint16_t*)mat->rep.csr.values;
            uint16_t val = vals[idx.u.value];
            return (uint16_expect_t){ .has_value = true, .u.value = val };
        }

        case CSC_MATRIX: {
            const uint16_t* vals = (const uint16_t*)mat->rep.csc.values;
            uint16_t val = vals[idx.u.value];
            return (uint16_expect_t){ .has_value = true, .u.value = val };
        }

        default:
            return (uint16_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    }
}
// ================================================================================
// ================================================================================
// eof
