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

#if defined(__AVX512F__)
#  include "simd_avx512_uint8.inl"
#elif defined(__AVX2__)
#  include "simd_avx2_uint8.inl"
#elif defined(__AVX__)
#  include "simd_avx_uint8.inl"
#elif defined(__SSE4_1__)
#  include "simd_sse41_uint8.inl"
#elif defined(__SSSE3__)
#  include "simd_sse3_uint8.inl"
#elif defined(__SSE2__)
#  include "simd_sse2_uint8.inl"
#elif defined(__ARM_FEATURE_SVE2)
#  include "simd_sve2_uint8.inl"
#elif defined(__ARM_FEATURE_SVE)
#  include "simd_sve_uint8.inl"
#elif defined(__ARM_NEON)
#  include "simd_neon_uint8.inl"
#else
#  include "simd_scalar_uint8.inl"
#endif


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

// uint8_matrix_t — type-safe uint8 wrapper implementations
//
// Every function below is a thin delegation to the corresponding generic
// matrix_t function, fixing the dtype to uint8_TYPE and converting between
// uint8 / uint8* and the void* interface expected by c_matrix.h.
// ================================================================================
// ================================================================================

// ================================================================================
// Internal helper: rewrap matrix_expect_t as uint8_matrix_expect_t
// ================================================================================

static inline uint8_matrix_expect_t _wrap_matrix_expect(matrix_expect_t e) {
    if (e.has_value) {
        return (uint8_matrix_expect_t){
            .has_value = true,
            .u.value   = (uint8_matrix_t*)e.u.value
        };
    }
    return (uint8_matrix_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

uint8_matrix_expect_t init_uint8_dense_matrix(size_t             rows,
                                              size_t             cols,
                                              allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_dense_matrix(rows, cols, UINT8_TYPE, alloc_v)
    );
}

// --------------------------------------------------------------------------------

uint8_matrix_expect_t init_uint8_coo_matrix(size_t             rows,
                                            size_t             cols,
                                            size_t             capacity,
                                            bool               growth,
                                            allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_coo_matrix(rows, cols, capacity, UINT8_TYPE, growth, alloc_v)
    );
}

// --------------------------------------------------------------------------------

void return_uint8_matrix(uint8_matrix_t* mat) {
    return_matrix(mat);
}

// ================================================================================
// Element access
// ================================================================================

error_code_t get_uint8_matrix(const uint8_matrix_t* mat,
                              size_t                row,
                              size_t                col,
                              uint8_t*              out) {
    if (mat == NULL || out == NULL) return NULL_POINTER;
    return get_matrix(mat, row, col, out);
}

// --------------------------------------------------------------------------------

error_code_t set_uint8_matrix(uint8_matrix_t* mat,
                              size_t          row,
                              size_t          col,
                              uint8_t         value) {
    if (mat == NULL) return NULL_POINTER;
    return set_matrix(mat, row, col, &value);
}

// ================================================================================
// COO assembly helpers
// ================================================================================

error_code_t reserve_uint8_coo_matrix(uint8_matrix_t* mat,
                                      size_t          capacity) {
    return reserve_coo_matrix(mat, capacity);
}

// --------------------------------------------------------------------------------

error_code_t push_back_uint8_coo_matrix(uint8_matrix_t* mat,
                                        size_t          row,
                                        size_t          col,
                                        uint8_t         value) {
    if (mat == NULL) return NULL_POINTER;
    return push_back_coo_matrix(mat, row, col, &value);
}

// --------------------------------------------------------------------------------

error_code_t sort_uint8_coo_matrix(uint8_matrix_t* mat) {
    return sort_coo_matrix(mat);
}

// ================================================================================
// Lifecycle / structural operations
// ================================================================================

error_code_t clear_uint8_matrix(uint8_matrix_t* mat) {
    return clear_matrix(mat);
}

// --------------------------------------------------------------------------------

uint8_matrix_expect_t copy_uint8_matrix(const uint8_matrix_t* src,
                                        allocator_vtable_t    alloc_v) {
    return _wrap_matrix_expect(copy_matrix(src, alloc_v));
}

// --------------------------------------------------------------------------------

uint8_matrix_expect_t convert_uint8_matrix(const uint8_matrix_t* src,
                                           matrix_format_t       target,
                                           allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (uint8_matrix_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }
 
    /* SIMD fast path: dense uint8 → CSR */
    if (src->format == DENSE_MATRIX && target == CSR_MATRIX) {
        const uint8_t* data = (const uint8_t*)src->rep.dense.data;
        size_t total = src->rows * src->cols;
 
        /* Pass 1: count nonzeros (SIMD-accelerated) */
        size_t nnz = simd_count_nonzero_uint8(data, total);
 
        /* Allocate the matrix_t header */
        void_ptr_expect_t mr = alloc_v.allocate(
            alloc_v.ctx, sizeof(matrix_t), true);
        if (!mr.has_value) {
            return (uint8_matrix_expect_t){
                .has_value = false, .u.error = BAD_ALLOC };
        }
 
        uint8_matrix_t* dst = (uint8_matrix_t*)mr.u.value;
 
        dst->rows      = src->rows;
        dst->cols      = src->cols;
        dst->dtype     = UINT8_TYPE;
        dst->data_size = sizeof(uint8_t);
        dst->format    = CSR_MATRIX;
        dst->alloc_v   = alloc_v;
 
        dst->rep.csr.nnz     = 0u;
        dst->rep.csr.row_ptr = NULL;
        dst->rep.csr.col_idx = NULL;
        dst->rep.csr.values  = NULL;
 
        /* Allocate CSR arrays */
        size_t row_ptr_bytes = (src->rows + 1u) * sizeof(size_t);
        size_t col_idx_bytes = nnz * sizeof(size_t);
        size_t values_bytes  = nnz * sizeof(uint8_t);
 
        void_ptr_expect_t rp_r = alloc_v.allocate(
            alloc_v.ctx, row_ptr_bytes, true);
        void_ptr_expect_t ci_r = alloc_v.allocate(
            alloc_v.ctx, (nnz > 0u) ? col_idx_bytes : 1u, true);
        void_ptr_expect_t vl_r = alloc_v.allocate(
            alloc_v.ctx, (nnz > 0u) ? values_bytes : 1u, true);
 
        if (!rp_r.has_value || !ci_r.has_value || !vl_r.has_value) {
            if (rp_r.has_value) alloc_v.return_element(alloc_v.ctx, rp_r.u.value);
            if (ci_r.has_value) alloc_v.return_element(alloc_v.ctx, ci_r.u.value);
            if (vl_r.has_value) alloc_v.return_element(alloc_v.ctx, vl_r.u.value);
            alloc_v.return_element(alloc_v.ctx, dst);
            return (uint8_matrix_expect_t){
                .has_value = false, .u.error = OUT_OF_MEMORY };
        }
 
        size_t* row_ptr = (size_t*)rp_r.u.value;
        size_t* col_idx = (size_t*)ci_r.u.value;
        uint8_t*  values  = (uint8_t*)vl_r.u.value;
 
        /* Pass 2: scatter nonzeros into CSR arrays (SIMD where available) */
        row_ptr[0] = 0u;
        size_t k = 0u;
 
        for (size_t i = 0u; i < src->rows; ++i) {
            const uint8_t* row_data = data + (i * src->cols);
            k = simd_scatter_csr_row_uint8(
                row_data, src->cols, 0u, col_idx, values, k);
            row_ptr[i + 1u] = k;
        }
 
        dst->rep.csr.nnz     = nnz;
        dst->rep.csr.row_ptr = row_ptr;
        dst->rep.csr.col_idx = col_idx;
        dst->rep.csr.values  = (uint8_t*)values;
 
        return (uint8_matrix_expect_t){
            .has_value = true, .u.value = dst };
    }
 
    /* All other conversions: delegate to generic path */
    return _wrap_matrix_expect(convert_matrix(src, target, alloc_v));
}

// --------------------------------------------------------------------------------

uint8_matrix_expect_t transpose_uint8_matrix(const uint8_matrix_t* src,
                                             allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (uint8_matrix_expect_t){
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
    uint8_matrix_expect_t r = init_uint8_dense_matrix(
        src->cols, src->rows, alloc_v
    );
    if (!r.has_value) return r;
 
    /* SIMD fast path: dense uint8 transpose */
    const uint8_t* sdata = (const uint8_t*)src->rep.dense.data;
    uint8_t*       ddata = (uint8_t*)r.u.value->rep.dense.data;
    simd_transpose_uint8(sdata, ddata, src->rows, src->cols);
 
    return r;
}
// ================================================================================
// Fill and zero
// ================================================================================

error_code_t fill_uint8_matrix(uint8_matrix_t* mat,
                               uint8_t         value) {
    if (mat == NULL) return NULL_POINTER;
 
    /* Zero fill: delegate to clear_matrix for all formats */
    if (value == 0u) return clear_matrix(mat);
 
    /* Non-zero fill on non-dense: fall back to generic path */
    if (mat->format != DENSE_MATRIX) return fill_matrix(mat, &value);
 
    /* SIMD fast path: dense uint8 fill */
    uint8_t* data  = (uint8_t*)mat->rep.dense.data;
    size_t count = mat->rows * mat->cols;
    simd_fill_uint8(data, count, value);
    return NO_ERROR;
}
// --------------------------------------------------------------------------------

error_code_t zero_uint8_matrix(uint8_matrix_t* mat) {
    return clear_matrix(mat);
}

// ================================================================================
// Introspection
// ================================================================================

size_t uint8_matrix_rows(const uint8_matrix_t* mat) {
    return matrix_rows(mat);
}

// --------------------------------------------------------------------------------

size_t uint8_matrix_cols(const uint8_matrix_t* mat) {
    return matrix_cols(mat);
}

// --------------------------------------------------------------------------------

size_t uint8_matrix_nnz(const uint8_matrix_t* mat) {
    return matrix_nnz(mat);
}

// --------------------------------------------------------------------------------

matrix_format_t uint8_matrix_format(const uint8_matrix_t* mat) {
    return matrix_format(mat);
}

// --------------------------------------------------------------------------------

size_t uint8_matrix_storage_bytes(const uint8_matrix_t* mat) {
    return matrix_storage_bytes(mat);
}

// --------------------------------------------------------------------------------

const char* uint8_matrix_format_name(const uint8_matrix_t* mat) {
    if (mat == NULL) return "UNKNOWN_MATRIX_FORMAT";
    return matrix_format_name(mat->format);
}

// ================================================================================
// Shape and compatibility queries
// ================================================================================

bool uint8_matrix_has_same_shape(const uint8_matrix_t* a,
                                 const uint8_matrix_t* b) {
    return matrix_has_same_shape(a, b);
}

// --------------------------------------------------------------------------------

bool uint8_matrix_is_square(const uint8_matrix_t* mat) {
    return matrix_is_square(mat);
}

// --------------------------------------------------------------------------------

bool uint8_matrix_is_sparse(const uint8_matrix_t* mat) {
    return matrix_is_sparse(mat);
}

// --------------------------------------------------------------------------------

bool is_uint8_matrix_zero(const uint8_matrix_t* mat) {
    if (mat == NULL) return true;

    /* SIMD fast path: dense uint8 */
    if (mat->format == DENSE_MATRIX) {
        const uint8_t* data = (const uint8_t*)mat->rep.dense.data;
        size_t count = mat->rows * mat->cols;
        return simd_is_all_zero_uint8(data, count);
    }

    /* Sparse formats: delegate to generic path */
    return is_zero_matrix(mat);
}

// --------------------------------------------------------------------------------

bool uint8_matrix_equal(const uint8_matrix_t* a,
                        const uint8_matrix_t* b) {
    if (a == NULL || b == NULL) return false;
    if (!matrix_has_same_shape(a, b)) return false;
    if (a->dtype != b->dtype) return false;
 
    /* SIMD fast path: both dense uint8 matrices — flat linear comparison */
    if (a->format == DENSE_MATRIX && b->format == DENSE_MATRIX) {
        const uint8_t* da = (const uint8_t*)a->rep.dense.data;
        const uint8_t* db = (const uint8_t*)b->rep.dense.data;
        return simd_uint8_arrays_equal(da, db, a->rows * a->cols);
    }
 
    /* Mixed/sparse formats: fall back to generic element-wise comparison */
    return matrix_equal(a, b);
}

// --------------------------------------------------------------------------------

bool uint8_matrix_is_add_compatible(const uint8_matrix_t* a,
                                    const uint8_matrix_t* b) {
    return matrix_is_add_compatible(a, b);
}

// --------------------------------------------------------------------------------

bool uint8_matrix_is_multiply_compatible(const uint8_matrix_t* a,
                                         const uint8_matrix_t* b) {
    return matrix_is_multiply_compatible(a, b);
}

// ================================================================================
// Row / column swaps
// ================================================================================

error_code_t swap_uint8_matrix_rows(uint8_matrix_t* mat,
                                    size_t          r1,
                                    size_t          r2) {
    return swap_matrix_rows(mat, r1, r2);
}

// --------------------------------------------------------------------------------

error_code_t swap_uint8_matrix_cols(uint8_matrix_t* mat,
                                    size_t          c1,
                                    size_t          c2) {
    return swap_matrix_cols(mat, c1, c2);
}

// ================================================================================
// Special matrix constructors
// ================================================================================

uint8_matrix_expect_t init_uint8_identity_matrix(size_t             n,
                                                 allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_identity_matrix(n, UINT8_TYPE, alloc_v)
    );
}

// --------------------------------------------------------------------------------

uint8_matrix_expect_t init_uint8_row_vector(size_t             length,
                                            allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_row_vector(length, UINT8_TYPE, alloc_v)
    );
}

// --------------------------------------------------------------------------------

uint8_matrix_expect_t init_uint8_col_vector(size_t             length,
                                            allocator_vtable_t alloc_v) {
    return _wrap_matrix_expect(
        init_col_vector(length, UINT8_TYPE, alloc_v)
    );
}

// ================================================================================
// Vector shape queries
// ================================================================================

bool uint8_matrix_is_row_vector(const uint8_matrix_t* mat) {
    return matrix_is_row_vector(mat);
}

// --------------------------------------------------------------------------------

bool uint8_matrix_is_col_vector(const uint8_matrix_t* mat) {
    return matrix_is_col_vector(mat);
}

// --------------------------------------------------------------------------------

bool uint8_matrix_is_vector(const uint8_matrix_t* mat) {
    return matrix_is_vector(mat);
}

// --------------------------------------------------------------------------------

size_t uint8_matrix_vector_length(const uint8_matrix_t* mat) {
    return matrix_vector_length(mat);
}
// ================================================================================
// ================================================================================
// eof
