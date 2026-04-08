// ================================================================================
// ================================================================================
// - File:    c_double.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 08, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here
#include <string.h>
#include "c_double.h"

#if defined(__AVX512F__)
#  include "simd_avx512_double.inl"
#elif defined(__AVX2__)
#  include "simd_avx2_double.inl"
#elif defined(__AVX__)
#  include "simd_avx_double.inl"
#elif defined(__SSE4_1__)
#  include "simd_sse41_double.inl"
#elif defined(__SSSE3__)
#  include "simd_sse3_double.inl"
#elif defined(__SSE2__)
#  include "simd_sse2_double.inl"
#elif defined(__ARM_FEATURE_SVE2)
#  include "simd_sve2_double.inl"
#elif defined(__ARM_FEATURE_SVE)
#  include "simd_sve_double.inl"
#elif defined(__ARM_NEON)
#  include "simd_neon_double.inl"
#else
#  include "simd_scalar_double.inl"
#endif
// ================================================================================ 
// ================================================================================ 


static inline double_array_expect_t _wrap_expect(array_expect_t e) {
    if (e.has_value) {
        return (double_array_expect_t){
            .has_value = true,
            .u.value   = (double_array_t*)e.u.value
        };
    }
    return (double_array_expect_t){
        .has_value = false,
        .u.error   = e.u.error
    };
}

// ================================================================================
// Initialization and teardown
// ================================================================================

double_array_expect_t init_double_array(size_t             capacity,
                                        bool               growth,
                                        allocator_vtable_t alloc_v) {
    return _wrap_expect(init_array(capacity, DOUBLE_TYPE, growth, alloc_v));
}

// --------------------------------------------------------------------------------

void return_double_array(double_array_t* array) {
    if (array == NULL) return;
    return_array(&array->base);
}

// ================================================================================
// Push operations
// ================================================================================

error_code_t push_back_double_array(double_array_t* array, double value) {
    if (array == NULL) return NULL_POINTER;
    return push_back_array(&array->base, &value, DOUBLE_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_front_double_array(double_array_t* array, double value) {
    if (array == NULL) return NULL_POINTER;
    return push_front_array(&array->base, &value, DOUBLE_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t push_at_double_array(double_array_t* array,
                                  size_t          index,
                                  double          value) {
    if (array == NULL) return NULL_POINTER;
    return push_at_array(&array->base, &value, index, DOUBLE_TYPE);
}

// ================================================================================
// Get operation
// ================================================================================

error_code_t get_double_array_index(const double_array_t* array,
                                    size_t                index,
                                    double*               out) {
    if (array == NULL || out == NULL) return NULL_POINTER;
    return get_array_index(&array->base, index, out, DOUBLE_TYPE);
}

// ================================================================================
// Pop operations
// ================================================================================

error_code_t pop_back_double_array(double_array_t* array, double* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_back_array(&array->base, out, DOUBLE_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_front_double_array(double_array_t* array, double* out) {
    if (array == NULL) return NULL_POINTER;
    return pop_front_array(&array->base, out, DOUBLE_TYPE);
}

// --------------------------------------------------------------------------------

error_code_t pop_any_double_array(double_array_t* array,
                                  double*         out,
                                  size_t          index) {
    if (array == NULL) return NULL_POINTER;
    return pop_any_array(&array->base, out, index, DOUBLE_TYPE);
}

// ================================================================================
// Utility operations
// ================================================================================

error_code_t clear_double_array(double_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return clear_array(&array->base);
}

// --------------------------------------------------------------------------------

error_code_t set_double_array_index(double_array_t* array,
                                    size_t          index,
                                    double          value) {
    if (array == NULL) return NULL_POINTER;
    return set_array_index(&array->base, index, &value, DOUBLE_TYPE);
}

// --------------------------------------------------------------------------------

double_array_expect_t copy_double_array(const double_array_t* src,
                                        allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (double_array_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };
    }
    return _wrap_expect(copy_array(&src->base, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t concat_double_array(double_array_t*       dst,
                                 const double_array_t* src) {
    if (dst == NULL || src == NULL) return NULL_POINTER;
    return concat_array(&dst->base, &src->base);
}

// --------------------------------------------------------------------------------

double_array_expect_t slice_double_array(const double_array_t* src,
                                          size_t                start,
                                          size_t                end,
                                          allocator_vtable_t    alloc_v) {
    if (src == NULL) {
        return (double_array_expect_t){ .has_value = false,
                                        .u.error   = NULL_POINTER };
    }
    return _wrap_expect(slice_array(&src->base, start, end, alloc_v));
}

// --------------------------------------------------------------------------------

error_code_t reverse_double_array(double_array_t* array) {
    if (array == NULL) return NULL_POINTER;
    return reverse_array(&array->base);
}

// --------------------------------------------------------------------------------

/*
 * Three-way double comparator. The (va > vb) - (va < vb) idiom is safe for all
 * finite values and signed zeros under IEEE 754. NaN produces 0 for both
 * sub-expressions, giving unspecified ordering — callers must ensure no NaN.
 */
static int _cmp_double(const void* a, const void* b) {
    double va = *(const double*)a;
    double vb = *(const double*)b;
    return (va > vb) - (va < vb);
}

error_code_t sort_double_array(double_array_t* array, direction_t dir) {
    if (array == NULL) return NULL_POINTER;
    return sort_array(&array->base, _cmp_double, dir);
}

// ================================================================================
// Reduction
// ================================================================================

double_expect_t double_array_min(const double_array_t* array) {
    if (array == NULL)
        return (double_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_min(&array->base, _cmp_double, DOUBLE_TYPE);
    if (!idx.has_value)
        return (double_expect_t){ .has_value = false, .u.error = idx.u.error };
    double val = 0.0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, DOUBLE_TYPE);
    if (err != NO_ERROR)
        return (double_expect_t){ .has_value = false, .u.error = err };
    return (double_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

double_expect_t double_array_max(const double_array_t* array) {
    if (array == NULL)
        return (double_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    size_expect_t idx = array_max(&array->base, _cmp_double, DOUBLE_TYPE);
    if (!idx.has_value)
        return (double_expect_t){ .has_value = false, .u.error = idx.u.error };
    double val = 0.0;
    error_code_t err = get_array_index(&array->base, idx.u.value, &val, DOUBLE_TYPE);
    if (err != NO_ERROR)
        return (double_expect_t){ .has_value = false, .u.error = err };
    return (double_expect_t){ .has_value = true, .u.value = val };
}

// --------------------------------------------------------------------------------

static void _add_double(void* accum, const void* element) {
    *(double*)accum += *(const double*)element;
}

double_expect_t double_array_sum(const double_array_t* array) {
    if (array == NULL)
        return (double_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    double total = 0.0;
    error_code_t err = array_sum(&array->base, &total, _add_double, DOUBLE_TYPE);
    if (err != NO_ERROR)
        return (double_expect_t){ .has_value = false, .u.error = err };
    return (double_expect_t){ .has_value = true, .u.value = total };
}

// --------------------------------------------------------------------------------

double_array_expect_t cumulative_double_array(const double_array_t* src,
                                              allocator_vtable_t    alloc) {
    if (src == NULL)
        return (double_array_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    array_expect_t r = cumulative_array(&src->base, _add_double, alloc, DOUBLE_TYPE);
    if (!r.has_value)
        return (double_array_expect_t){ .has_value = false, .u.error = r.u.error };
    return (double_array_expect_t){ .has_value = true,
                                    .u.value   = (double_array_t*)r.u.value };
}
// -------------------------------------------------------------------------------- 

size_expect_t double_array_contains(const double_array_t* array,
                                    double                value,
                                    size_t                start,
                                    size_t                end,
                                    double                abs_tol,
                                    double                rel_tol) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (abs_tol < 0.0 || rel_tol < 0.0)
        return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (start >= end)
        return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (end > array->base.len)
        return (size_expect_t){ .has_value = false, .u.error = OUT_OF_BOUNDS };

    const double* data = (const double*)array->base.data;
    size_t idx = simd_contains_double(data, start, end, value, abs_tol, rel_tol);

    if (idx == SIZE_MAX)
        return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
    return (size_expect_t){ .has_value = true, .u.value = idx };
}

// --------------------------------------------------------------------------------

size_expect_t double_array_binary_search(double_array_t* array,
                                         double          value,
                                         bool            sort,
                                         double          abs_tol,
                                         double          rel_tol) {
    if (array == NULL)
        return (size_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (abs_tol < 0.0 || rel_tol < 0.0)
        return (size_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (array->base.len == 0u)
        return (size_expect_t){ .has_value = false, .u.error = EMPTY };

    if (sort)
        sort_array(&array->base, _cmp_double, FORWARD);

    const double* data = (const double*)array->base.data;
    size_t idx = simd_binary_search_double(data, array->base.len, value,
                                           abs_tol, rel_tol, _cmp_double);
    if (idx == SIZE_MAX)
        return (size_expect_t){ .has_value = false, .u.error = NOT_FOUND };
    return (size_expect_t){ .has_value = true, .u.value = idx };
}

// --------------------------------------------------------------------------------

bracket_expect_t double_array_binary_bracket(double_array_t* array,
                                             double          value,
                                             bool            sort) {
    if (array == NULL)
        return (bracket_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    return binary_bracket_array(&array->base, &value, _cmp_double, sort,
                                DOUBLE_TYPE);
}

// ================================================================================
// Introspection
// ================================================================================

size_t double_array_size(const double_array_t* array) {
    if (array == NULL) return 0u;
    return array_size(&array->base);
}

// --------------------------------------------------------------------------------

size_t double_array_alloc(const double_array_t* array) {
    if (array == NULL) return 0u;
    return array_alloc(&array->base);
}

// --------------------------------------------------------------------------------

size_t double_array_data_size(const double_array_t* array) {
    if (array == NULL) return 0u;
    return array_data_size(&array->base);
}

// --------------------------------------------------------------------------------

bool is_double_array_empty(const double_array_t* array) {
    if (array == NULL) return true;
    return is_array_empty(&array->base);
}

// --------------------------------------------------------------------------------

bool is_double_array_full(const double_array_t* array) {
    if (array == NULL) return true;
    return is_array_full(&array->base);
}

// --------------------------------------------------------------------------------

bool is_double_array_ptr(const double_array_t* array, const double* ptr) {
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
 
double_dict_expect_t init_double_dict(size_t             capacity,
                                      bool               growth,
                                      allocator_vtable_t alloc_v) {
    dict_expect_t r = init_dict(capacity, sizeof(double),
                                DOUBLE_TYPE, growth, alloc_v);
    if (!r.has_value)
        return (double_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (double_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
void return_double_dict(double_dict_t* dict) {
    return_dict(dict);
}
 
// ================================================================================
// Insert
// ================================================================================
 
error_code_t insert_double_dict(double_dict_t*     dict,
                                 const char*        key,
                                 double           value,
                                 allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    return insert_dict(dict, _key(key, strlen(key)), &value, alloc_v);
}
 
// --------------------------------------------------------------------------------
 
error_code_t insert_double_dict_n(double_dict_t*     dict,
                                   const char*        key,
                                   size_t             key_len,
                                   double           value,
                                   allocator_vtable_t alloc_v) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return insert_dict(dict, _key(key, key_len), &value, alloc_v);
}
 
// ================================================================================
// Pop
// ================================================================================
 
error_code_t pop_double_dict(double_dict_t* dict,
                              const char*    key,
                              double*      out_value) {
    if (key == NULL) return NULL_POINTER;
    return pop_dict(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t pop_double_dict_n(double_dict_t* dict,
                                const char*    key,
                                size_t         key_len,
                                double*      out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return pop_dict(dict, _key(key, key_len), out_value);
}
 
// ================================================================================
// Update
// ================================================================================
 
error_code_t update_double_dict(double_dict_t* dict,
                                 const char*    key,
                                 double       value) {
    if (key == NULL) return NULL_POINTER;
    return update_dict(dict, _key(key, strlen(key)), &value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t update_double_dict_n(double_dict_t* dict,
                                   const char*    key,
                                   size_t         key_len,
                                   double       value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return update_dict(dict, _key(key, key_len), &value);
}
 
// ================================================================================
// Lookup
// ================================================================================
 
error_code_t get_double_dict_value(const double_dict_t* dict,
                                    const char*          key,
                                    double*            out_value) {
    if (key == NULL) return NULL_POINTER;
    return get_dict_value(dict, _key(key, strlen(key)), out_value);
}
 
// --------------------------------------------------------------------------------
 
error_code_t get_double_dict_value_n(const double_dict_t* dict,
                                      const char*          key,
                                      size_t               key_len,
                                      double*            out_value) {
    if (key == NULL) return NULL_POINTER;
    if (key_len == 0u) return INVALID_ARG;
    return get_dict_value(dict, _key(key, key_len), out_value);
}
 
// --------------------------------------------------------------------------------
 
const double* get_double_dict_ptr(const double_dict_t* dict, const char* key) {
    if (key == NULL) return NULL;
    return (const double*)get_dict_value_ptr(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
const double* get_double_dict_ptr_n(const double_dict_t* dict,
                                       const char*          key,
                                       size_t               key_len) {
    if (key == NULL || key_len == 0u) return NULL;
    return (const double*)get_dict_value_ptr(dict, _key(key, key_len));
}
 
// --------------------------------------------------------------------------------
 
bool has_double_dict_key(const double_dict_t* dict, const char* key) {
    if (key == NULL) return false;
    return has_dict_key(dict, _key(key, strlen(key)));
}
 
// --------------------------------------------------------------------------------
 
bool has_double_dict_key_n(const double_dict_t* dict,
                            const char*          key,
                            size_t               key_len) {
    if (key == NULL || key_len == 0u) return false;
    return has_dict_key(dict, _key(key, key_len));
}
 
// ================================================================================
// Utility
// ================================================================================
 
error_code_t clear_double_dict(double_dict_t* dict) {
    return clear_dict(dict);
}
 
// --------------------------------------------------------------------------------
 
double_dict_expect_t copy_double_dict(const double_dict_t* src,
                                      allocator_vtable_t   alloc_v) {
    dict_expect_t r = copy_dict(src, alloc_v);
    if (!r.has_value)
        return (double_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (double_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// --------------------------------------------------------------------------------
 
double_dict_expect_t merge_double_dict(const double_dict_t* a,
                                       const double_dict_t* b,
                                       bool                 overwrite,
                                       allocator_vtable_t   alloc_v) {
    dict_expect_t r = merge_dict(a, b, overwrite, alloc_v);
    if (!r.has_value)
        return (double_dict_expect_t){ .has_value = false,
                                       .u.error   = r.u.error };
 
    return (double_dict_expect_t){ .has_value = true,
                                    .u.value   = r.u.value };
}
 
// ================================================================================
// Iteration
// ================================================================================
 
typedef struct {
    double_dict_iter_fn typed_fn;
    void*               user_data;
} _double_iter_ctx_t;
 
static void _double_iter_shim(dict_entry_t e, void* ctx) {
    const _double_iter_ctx_t* c = (const _double_iter_ctx_t*)ctx;
    double value;
    memcpy(&value, e.value, sizeof(double));
    c->typed_fn((const char*)e.key, e.key_len, value, c->user_data);
}
 
error_code_t foreach_double_dict(const double_dict_t* dict,
                                  double_dict_iter_fn  fn,
                                  void*                user_data) {
    if (fn == NULL) return NULL_POINTER;
    _double_iter_ctx_t ctx = { .typed_fn = fn, .user_data = user_data };
    return foreach_dict(dict, _double_iter_shim, &ctx);
}
 
// ================================================================================
// Introspection
// ================================================================================
 
size_t double_dict_size(const double_dict_t* dict) {
    return dict_size(dict);
}
 
size_t double_dict_hash_size(const double_dict_t* dict) {
    return dict_hash_size(dict);
}
 
size_t double_dict_alloc(const double_dict_t* dict) {
    return dict_alloc(dict);
}
 
bool is_double_dict_empty(const double_dict_t* dict) {
    return is_dict_empty(dict);
}
// ================================================================================ 
// ================================================================================ 

// static inline double_matrix_expect_t _wrap_double_matrix_expect(matrix_expect_t e) {
//     if (e.has_value) {
//         return (double_matrix_expect_t){
//             .has_value = true,
//             .u.value   = (double_matrix_t*)e.u.value
//         };
//     }
//     return (double_matrix_expect_t){
//         .has_value = false,
//         .u.error   = e.u.error
//     };
// }
//  
// // ================================================================================
// // Initialization and teardown
// // ================================================================================
//  
// double_matrix_expect_t init_double_dense_matrix(size_t             rows,
//                                                 size_t             cols,
//                                                 allocator_vtable_t alloc_v) {
//     return _wrap_double_matrix_expect(
//         init_dense_matrix(rows, cols, DOUBLE_TYPE, alloc_v)
//     );
// }
//  
// // --------------------------------------------------------------------------------
//  
// double_matrix_expect_t init_double_coo_matrix(size_t             rows,
//                                               size_t             cols,
//                                               size_t             capacity,
//                                               bool               growth,
//                                               allocator_vtable_t alloc_v) {
//     return _wrap_double_matrix_expect(
//         init_coo_matrix(rows, cols, capacity, DOUBLE_TYPE, growth, alloc_v)
//     );
// }
//  
// // --------------------------------------------------------------------------------
//  
// void return_double_matrix(double_matrix_t* mat) {
//     return_matrix(mat);
// }
//  
// // ================================================================================
// // Element access
// // ================================================================================
//  
// error_code_t get_double_matrix(const double_matrix_t* mat,
//                                size_t                 row,
//                                size_t                 col,
//                                double*                out) {
//     if (mat == NULL || out == NULL) return NULL_POINTER;
//     return get_matrix(mat, row, col, out);
// }
//  
// // --------------------------------------------------------------------------------
//  
// error_code_t set_double_matrix(double_matrix_t* mat,
//                                size_t           row,
//                                size_t           col,
//                                double           value) {
//     if (mat == NULL) return NULL_POINTER;
//     return set_matrix(mat, row, col, &value);
// }
//  
// // ================================================================================
// // COO assembly helpers
// // ================================================================================
//  
// error_code_t reserve_double_coo_matrix(double_matrix_t* mat,
//                                        size_t           capacity) {
//     return reserve_coo_matrix(mat, capacity);
// }
//  
// // --------------------------------------------------------------------------------
//  
// error_code_t push_back_double_coo_matrix(double_matrix_t* mat,
//                                          size_t           row,
//                                          size_t           col,
//                                          double           value) {
//     if (mat == NULL) return NULL_POINTER;
//     return push_back_coo_matrix(mat, row, col, &value);
// }
//  
// // --------------------------------------------------------------------------------
//  
// error_code_t sort_double_coo_matrix(double_matrix_t* mat) {
//     return sort_coo_matrix(mat);
// }
//  
// // ================================================================================
// // Lifecycle / structural operations
// // ================================================================================
//  
// error_code_t clear_double_matrix(double_matrix_t* mat) {
//     return clear_matrix(mat);
// }
//  
// // --------------------------------------------------------------------------------
//  
// double_matrix_expect_t copy_double_matrix(const double_matrix_t* src,
//                                           allocator_vtable_t     alloc_v) {
//     return _wrap_double_matrix_expect(copy_matrix(src, alloc_v));
// }
//  
// // --------------------------------------------------------------------------------
//  
// double_matrix_expect_t convert_double_matrix(const double_matrix_t* src,
//                                              matrix_format_t        target,
//                                              allocator_vtable_t     alloc_v) {
//     if (src == NULL) {
//         return (double_matrix_expect_t){
//             .has_value = false,
//             .u.error   = NULL_POINTER
//         };
//     }
//  
//     /* SIMD fast path: dense double → CSR */
//     if (src->format == DENSE_MATRIX && target == CSR_MATRIX) {
//         const double* data = (const double*)src->rep.dense.data;
//         size_t total = src->rows * src->cols;
//  
//         /* Pass 1: count nonzeros (SIMD-accelerated) */
//         size_t nnz = simd_count_nonzero_double(data, total);
//  
//         /* Allocate the matrix_t header */
//         void_ptr_expect_t mr = alloc_v.allocate(
//             alloc_v.ctx, sizeof(matrix_t), true);
//         if (!mr.has_value) {
//             return (double_matrix_expect_t){
//                 .has_value = false, .u.error = BAD_ALLOC };
//         }
//  
//         double_matrix_t* dst = (double_matrix_t*)mr.u.value;
//  
//         dst->rows      = src->rows;
//         dst->cols      = src->cols;
//         dst->dtype     = DOUBLE_TYPE;
//         dst->data_size = sizeof(double);
//         dst->format    = CSR_MATRIX;
//         dst->alloc_v   = alloc_v;
//  
//         dst->rep.csr.nnz     = 0u;
//         dst->rep.csr.row_ptr = NULL;
//         dst->rep.csr.col_idx = NULL;
//         dst->rep.csr.values  = NULL;
//  
//         /* Allocate CSR arrays */
//         size_t row_ptr_bytes = (src->rows + 1u) * sizeof(size_t);
//         size_t col_idx_bytes = nnz * sizeof(size_t);
//         size_t values_bytes  = nnz * sizeof(double);
//  
//         void_ptr_expect_t rp_r = alloc_v.allocate(
//             alloc_v.ctx, row_ptr_bytes, true);
//         void_ptr_expect_t ci_r = alloc_v.allocate(
//             alloc_v.ctx, (nnz > 0u) ? col_idx_bytes : 1u, true);
//         void_ptr_expect_t vl_r = alloc_v.allocate(
//             alloc_v.ctx, (nnz > 0u) ? values_bytes : 1u, true);
//  
//         if (!rp_r.has_value || !ci_r.has_value || !vl_r.has_value) {
//             if (rp_r.has_value) alloc_v.return_element(alloc_v.ctx, rp_r.u.value);
//             if (ci_r.has_value) alloc_v.return_element(alloc_v.ctx, ci_r.u.value);
//             if (vl_r.has_value) alloc_v.return_element(alloc_v.ctx, vl_r.u.value);
//             alloc_v.return_element(alloc_v.ctx, dst);
//             return (double_matrix_expect_t){
//                 .has_value = false, .u.error = OUT_OF_MEMORY };
//         }
//  
//         size_t* row_ptr = (size_t*)rp_r.u.value;
//         size_t* col_idx = (size_t*)ci_r.u.value;
//         double* values  = (double*)vl_r.u.value;
//  
//         /* Pass 2: scatter nonzeros into CSR arrays (SIMD where available) */
//         row_ptr[0] = 0u;
//         size_t k = 0u;
//  
//         for (size_t i = 0u; i < src->rows; ++i) {
//             const double* row_data = data + (i * src->cols);
//             k = simd_scatter_csr_row_double(
//                 row_data, src->cols, 0u, col_idx, values, k);
//             row_ptr[i + 1u] = k;
//         }
//  
//         dst->rep.csr.nnz     = nnz;
//         dst->rep.csr.row_ptr = row_ptr;
//         dst->rep.csr.col_idx = col_idx;
//         dst->rep.csr.values  = (uint8_t*)values;
//  
//         return (double_matrix_expect_t){
//             .has_value = true, .u.value = dst };
//     }
//  
//     /* All other conversions: delegate to generic path */
//     return _wrap_double_matrix_expect(convert_matrix(src, target, alloc_v));
// }
//  
// // --------------------------------------------------------------------------------
//  
// double_matrix_expect_t transpose_double_matrix(const double_matrix_t* src,
//                                                allocator_vtable_t     alloc_v) {
//     if (src == NULL) {
//         return (double_matrix_expect_t){
//             .has_value = false,
//             .u.error   = NULL_POINTER
//         };
//     }
//  
//     /* Non-dense formats: delegate to the generic path */
//     if (src->format != DENSE_MATRIX) {
//         return _wrap_double_matrix_expect(transpose_matrix(src, alloc_v));
//     }
//  
//     /* Allocate destination dense matrix with transposed dimensions */
//     double_matrix_expect_t r = init_double_dense_matrix(
//         src->cols, src->rows, alloc_v
//     );
//     if (!r.has_value) return r;
//  
//     /* SIMD fast path: dense double transpose */
//     const double* sdata = (const double*)src->rep.dense.data;
//     double*       ddata = (double*)r.u.value->rep.dense.data;
//     simd_transpose_double(sdata, ddata, src->rows, src->cols);
//  
//     return r;
// }
//  
// // ================================================================================
// // Fill and zero
// // ================================================================================
//  
// error_code_t fill_double_matrix(double_matrix_t* mat,
//                                 double           value) {
//     if (mat == NULL) return NULL_POINTER;
//  
//     /* Zero fill: delegate to clear_matrix for all formats */
//     if (value == 0.0) return clear_matrix(mat);
//  
//     /* Non-zero fill on non-dense: fall back to generic path */
//     if (mat->format != DENSE_MATRIX) return fill_matrix(mat, &value);
//  
//     /* SIMD fast path: dense double fill */
//     double* data = (double*)mat->rep.dense.data;
//     size_t count = mat->rows * mat->cols;
//     simd_fill_double(data, count, value);
//     return NO_ERROR;
// }
//  
// // --------------------------------------------------------------------------------
//  
// error_code_t zero_double_matrix(double_matrix_t* mat) {
//     return clear_matrix(mat);
// }
//  
// // ================================================================================
// // Introspection
// // ================================================================================
//  
// size_t double_matrix_rows(const double_matrix_t* mat) {
//     return matrix_rows(mat);
// }
//  
// // --------------------------------------------------------------------------------
//  
// size_t double_matrix_cols(const double_matrix_t* mat) {
//     return matrix_cols(mat);
// }
//  
// // --------------------------------------------------------------------------------
//  
// size_t double_matrix_nnz(const double_matrix_t* mat) {
//     return matrix_nnz(mat);
// }
//  
// // --------------------------------------------------------------------------------
//  
// matrix_format_t double_matrix_format(const double_matrix_t* mat) {
//     return matrix_format(mat);
// }
//  
// // --------------------------------------------------------------------------------
//  
// size_t double_matrix_storage_bytes(const double_matrix_t* mat) {
//     return matrix_storage_bytes(mat);
// }
//  
// // --------------------------------------------------------------------------------
//  
// const char* double_matrix_format_name(const double_matrix_t* mat) {
//     if (mat == NULL) return "UNKNOWN_MATRIX_FORMAT";
//     return matrix_format_name(mat->format);
// }
//  
// // ================================================================================
// // Shape and compatibility queries
// // ================================================================================
//  
// bool double_matrix_has_same_shape(const double_matrix_t* a,
//                                   const double_matrix_t* b) {
//     return matrix_has_same_shape(a, b);
// }
//  
// // --------------------------------------------------------------------------------
//  
// bool double_matrix_is_square(const double_matrix_t* mat) {
//     return matrix_is_square(mat);
// }
//  
// // --------------------------------------------------------------------------------
//  
// bool double_matrix_is_sparse(const double_matrix_t* mat) {
//     return matrix_is_sparse(mat);
// }
//  
// // --------------------------------------------------------------------------------
//  
// bool is_double_matrix_zero(const double_matrix_t* mat) {
//     if (mat == NULL) return true;
//  
//     /* SIMD fast path: dense double — linear scan with early exit */
//     if (mat->format == DENSE_MATRIX) {
//         const double* data = (const double*)mat->rep.dense.data;
//         size_t count = mat->rows * mat->cols;
//         return simd_is_all_zero_double(data, count);
//     }
//  
//     /* Sparse formats: delegate (just checks nnz == 0) */
//     return is_zero_matrix(mat);
// }
//  
// // --------------------------------------------------------------------------------
//  
// bool double_matrix_equal(const double_matrix_t* a,
//                          const double_matrix_t* b) {
//     if (a == NULL || b == NULL) return false;
//     if (!matrix_has_same_shape(a, b)) return false;
//     if (a->dtype != b->dtype) return false;
//  
//     /* SIMD fast path: both dense double matrices — flat linear comparison */
//     if (a->format == DENSE_MATRIX && b->format == DENSE_MATRIX) {
//         const double* da = (const double*)a->rep.dense.data;
//         const double* db = (const double*)b->rep.dense.data;
//         return simd_equal_double(da, db, a->rows * a->cols);
//     }
//  
//     /* Mixed/sparse formats: fall back to generic element-wise comparison */
//     return matrix_equal(a, b);
// }
//  
// // --------------------------------------------------------------------------------
//  
// bool double_matrix_is_add_compatible(const double_matrix_t* a,
//                                      const double_matrix_t* b) {
//     return matrix_is_add_compatible(a, b);
// }
//  
// // --------------------------------------------------------------------------------
//  
// bool double_matrix_is_multiply_compatible(const double_matrix_t* a,
//                                           const double_matrix_t* b) {
//     return matrix_is_multiply_compatible(a, b);
// }
//  
// // ================================================================================
// // Row / column swaps
// // ================================================================================
//  
// error_code_t swap_double_matrix_rows(double_matrix_t* mat,
//                                      size_t           r1,
//                                      size_t           r2) {
//     return swap_matrix_rows(mat, r1, r2);
// }
//  
// // --------------------------------------------------------------------------------
//  
// error_code_t swap_double_matrix_cols(double_matrix_t* mat,
//                                      size_t           c1,
//                                      size_t           c2) {
//     return swap_matrix_cols(mat, c1, c2);
// }
//  
// // ================================================================================
// // Special matrix constructors
// // ================================================================================
//  
// double_matrix_expect_t init_double_identity_matrix(size_t             n,
//                                                    allocator_vtable_t alloc_v) {
//     return _wrap_double_matrix_expect(
//         init_identity_matrix(n, DOUBLE_TYPE, alloc_v)
//     );
// }
//  
// // --------------------------------------------------------------------------------
//  
// double_matrix_expect_t init_double_row_vector(size_t             length,
//                                               allocator_vtable_t alloc_v) {
//     return _wrap_double_matrix_expect(
//         init_row_vector(length, DOUBLE_TYPE, alloc_v)
//     );
// }
//  
// // --------------------------------------------------------------------------------
//  
// double_matrix_expect_t init_double_col_vector(size_t             length,
//                                               allocator_vtable_t alloc_v) {
//     return _wrap_double_matrix_expect(
//         init_col_vector(length, DOUBLE_TYPE, alloc_v)
//     );
// }
//  
// // ================================================================================
// // Vector shape queries
// // ================================================================================
//  
// bool double_matrix_is_row_vector(const double_matrix_t* mat) {
//     return matrix_is_row_vector(mat);
// }
//  
// // --------------------------------------------------------------------------------
//  
// bool double_matrix_is_col_vector(const double_matrix_t* mat) {
//     return matrix_is_col_vector(mat);
// }
//  
// // --------------------------------------------------------------------------------
//  
// bool double_matrix_is_vector(const double_matrix_t* mat) {
//     return matrix_is_vector(mat);
// }
//  
// // --------------------------------------------------------------------------------
//  
// size_t double_matrix_vector_length(const double_matrix_t* mat) {
//     return matrix_vector_length(mat);
// }
// ================================================================================
// ================================================================================
// eof
