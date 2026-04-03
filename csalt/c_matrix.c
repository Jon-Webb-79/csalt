// ================================================================================
// ================================================================================
// - File:    c_matrix.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    April 03, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_matrix.h"
#include <string.h>
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Internal helpers
// ================================================================================

static bool _value_is_zero(const uint8_t* ptr, size_t data_size) {
    size_t i;

    if ((ptr == NULL) || (data_size == 0u)) {
        return true;
    }

    /*
     * Fast path: check raw bytes
     * This works for:
     * - all integer types
     * - +0.0 for floats/doubles
     *
     * Caveat:
     * - -0.0 will NOT be detected here (handled below)
     */
    for (i = 0u; i < data_size; ++i) {
        if (ptr[i] != 0u) {
            break;
        }
    }

    if (i == data_size) {
        return true;
    }

    /*
     * Handle floating-point negative zero
     * Only needed for float/double sizes
     */
    if (data_size == sizeof(float)) {
        float val = 0.0f;
        memcpy(&val, ptr, sizeof(float));
        return (val == 0.0f);
    }

    if (data_size == sizeof(double)) {
        double val = 0.0;
        memcpy(&val, ptr, sizeof(double));
        return (val == 0.0);
    }

    /*
     * For all other types:
     * If any byte != 0, it's non-zero
     */
    return false;
}

static inline bool _matrix_in_bounds(const matrix_t* mat,
                                     size_t          row,
                                     size_t          col) {
    return mat != NULL && row < mat->rows && col < mat->cols;
}

// --------------------------------------------------------------------------------

static inline size_t _dense_offset(const matrix_t* mat,
                                   size_t          row,
                                   size_t          col) {
    return ((row * mat->cols) + col) * mat->data_size;
}

// --------------------------------------------------------------------------------

static inline int _coo_compare_pair(size_t r1, size_t c1, size_t r2, size_t c2) {
    if (r1 < r2) return -1;
    if (r1 > r2) return  1;
    if (c1 < c2) return -1;
    if (c1 > c2) return  1;
    return 0;
}

// --------------------------------------------------------------------------------

static void _coo_swap_entries(matrix_t* mat, size_t i, size_t j) {
    if (i == j) return;

    coo_matrix_t* coo = &mat->rep.coo;

    size_t tmp_row = coo->row_idx[i];
    coo->row_idx[i] = coo->row_idx[j];
    coo->row_idx[j] = tmp_row;

    size_t tmp_col = coo->col_idx[i];
    coo->col_idx[i] = coo->col_idx[j];
    coo->col_idx[j] = tmp_col;

    uint8_t* vi = coo->values + (i * mat->data_size);
    uint8_t* vj = coo->values + (j * mat->data_size);

    for (size_t k = 0; k < mat->data_size; ++k) {
        uint8_t tmp = vi[k];
        vi[k] = vj[k];
        vj[k] = tmp;
    }
}

// --------------------------------------------------------------------------------

static bool _coo_linear_search(const matrix_t* mat,
                               size_t          row,
                               size_t          col,
                               size_t*         index_out) {
    const coo_matrix_t* coo = &mat->rep.coo;

    for (size_t i = 0; i < coo->nnz; ++i) {
        if (coo->row_idx[i] == row && coo->col_idx[i] == col) {
            if (index_out) *index_out = i;
            return true;
        }
    }
    return false;
}

// --------------------------------------------------------------------------------

static bool _coo_binary_search(const matrix_t* mat,
                               size_t          row,
                               size_t          col,
                               size_t*         index_out) {
    const coo_matrix_t* coo = &mat->rep.coo;

    if (coo->nnz == 0u) return false;

    size_t lo = 0u;
    size_t hi = coo->nnz;

    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2u;

        int cmp = _coo_compare_pair(row, col,
                                    coo->row_idx[mid], coo->col_idx[mid]);

        if (cmp < 0) {
            hi = mid;
        } else if (cmp > 0) {
            lo = mid + 1u;
        } else {
            if (index_out) *index_out = mid;
            return true;
        }
    }

    return false;
}

// --------------------------------------------------------------------------------

static error_code_t _get_dense_matrix(const matrix_t* mat,
                                      size_t          row,
                                      size_t          col,
                                      void*           out) {
    if (!_matrix_in_bounds(mat, row, col) || out == NULL) return INVALID_ARG;

    size_t off = _dense_offset(mat, row, col);
    memcpy(out, mat->rep.dense.data + off, mat->data_size);
    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

static error_code_t _get_coo_matrix(const matrix_t* mat,
                                    size_t          row,
                                    size_t          col,
                                    void*           out) {
    if (!_matrix_in_bounds(mat, row, col) || out == NULL) return INVALID_ARG;

    const coo_matrix_t* coo = &mat->rep.coo;
    size_t idx = 0u;
    bool found = coo->sorted
        ? _coo_binary_search(mat, row, col, &idx)
        : _coo_linear_search(mat, row, col, &idx);

    if (!found) {
        memset(out, 0, mat->data_size);
        return NO_ERROR;
    }

    memcpy(out, coo->values + (idx * mat->data_size), mat->data_size);
    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

static error_code_t _get_csr_matrix(const matrix_t* mat,
                                    size_t          row,
                                    size_t          col,
                                    void*           out) {
    const csr_matrix_t* csr = NULL;
    size_t start = 0u;
    size_t end = 0u;
    size_t k = 0u;

    if ((out == NULL) || !_matrix_in_bounds(mat, row, col)) {
        return INVALID_ARG;
    }

    csr = &mat->rep.csr;

    start = csr->row_ptr[row];
    end   = csr->row_ptr[row + 1u];

    for (k = start; k < end; ++k) {
        if (csr->col_idx[k] == col) {
            memcpy(out,
                   csr->values + (k * mat->data_size),
                   mat->data_size);
            return NO_ERROR;
        }
    }

    memset(out, 0, mat->data_size);
    return NO_ERROR;
}
// -------------------------------------------------------------------------------- 

static error_code_t _get_csc_matrix(const matrix_t* mat,
                                    size_t          row,
                                    size_t          col,
                                    void*           out) {
    const csc_matrix_t* csc = NULL;
    size_t start = 0u;
    size_t end = 0u;
    size_t k = 0u;

    if ((out == NULL) || !_matrix_in_bounds(mat, row, col)) {
        return INVALID_ARG;
    }

    csc = &mat->rep.csc;

    start = csc->col_ptr[col];
    end   = csc->col_ptr[col + 1u];

    for (k = start; k < end; ++k) {
        if (csc->row_idx[k] == row) {
            memcpy(out,
                   csc->values + (k * mat->data_size),
                   mat->data_size);
            return NO_ERROR;
        }
    }

    memset(out, 0, mat->data_size);
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 

static error_code_t _set_dense_matrix(matrix_t*    mat,
                                      size_t       row,
                                      size_t       col,
                                      const void*  value) {
    if (!_matrix_in_bounds(mat, row, col) || value == NULL) return INVALID_ARG;

    size_t off = _dense_offset(mat, row, col);
    memcpy(mat->rep.dense.data + off, value, mat->data_size);
    return NO_ERROR;
}
// ================================================================================ 
// ================================================================================ 

static error_code_t _reserve_coo_matrix(matrix_t* mat, size_t new_cap) {
    if (mat == NULL) return NULL_POINTER;
    if (mat->format != COO_MATRIX) return ILLEGAL_STATE;

    coo_matrix_t* coo = &mat->rep.coo;

    if (new_cap <= coo->cap) return NO_ERROR;
    if (new_cap < coo->nnz)  return INVALID_ARG;

    if (new_cap > SIZE_MAX / sizeof(size_t)) return LENGTH_OVERFLOW;
    if (mat->data_size != 0u && new_cap > SIZE_MAX / mat->data_size)
        return LENGTH_OVERFLOW;

    void_ptr_expect_t row_r = mat->alloc_v.allocate(
        mat->alloc_v.ctx, new_cap * sizeof(size_t), true
    );
    if (!row_r.has_value) return OUT_OF_MEMORY;

    void_ptr_expect_t col_r = mat->alloc_v.allocate(
        mat->alloc_v.ctx, new_cap * sizeof(size_t), true
    );
    if (!col_r.has_value) {
        mat->alloc_v.return_element(mat->alloc_v.ctx, row_r.u.value);
        return OUT_OF_MEMORY;
    }

    void_ptr_expect_t val_r = mat->alloc_v.allocate(
        mat->alloc_v.ctx, new_cap * mat->data_size, true
    );
    if (!val_r.has_value) {
        mat->alloc_v.return_element(mat->alloc_v.ctx, row_r.u.value);
        mat->alloc_v.return_element(mat->alloc_v.ctx, col_r.u.value);
        return OUT_OF_MEMORY;
    }

    size_t*  new_rows = (size_t*)row_r.u.value;
    size_t*  new_cols = (size_t*)col_r.u.value;
    uint8_t* new_vals = (uint8_t*)val_r.u.value;

    if (coo->nnz > 0u) {
        memcpy(new_rows, coo->row_idx, coo->nnz * sizeof(size_t));
        memcpy(new_cols, coo->col_idx, coo->nnz * sizeof(size_t));
        memcpy(new_vals, coo->values, coo->nnz * mat->data_size);
    }

    if (coo->row_idx) mat->alloc_v.return_element(mat->alloc_v.ctx, coo->row_idx);
    if (coo->col_idx) mat->alloc_v.return_element(mat->alloc_v.ctx, coo->col_idx);
    if (coo->values)  mat->alloc_v.return_element(mat->alloc_v.ctx, coo->values);

    coo->row_idx = new_rows;
    coo->col_idx = new_cols;
    coo->values  = new_vals;
    coo->cap     = new_cap;

    return NO_ERROR;
}

// --------------------------------------------------------------------------------

static error_code_t _push_back_coo_matrix(matrix_t*    mat,
                                          size_t       row,
                                          size_t       col,
                                          const void*  value) {
    if (mat == NULL || value == NULL) return NULL_POINTER;
    if (mat->format != COO_MATRIX)    return ILLEGAL_STATE;
    if (!_matrix_in_bounds(mat, row, col)) return INVALID_ARG;

    coo_matrix_t* coo = &mat->rep.coo;

    if (coo->nnz == coo->cap) {
        if (!coo->growth) return CAPACITY_OVERFLOW;

        size_t new_cap = (coo->cap == 0u) ? 1u : (coo->cap * 2u);
        if (new_cap < coo->cap) return LENGTH_OVERFLOW;

        error_code_t r = _reserve_coo_matrix(mat, new_cap);
        if (r != NO_ERROR) return r;
    }

    size_t idx = coo->nnz;
    coo->row_idx[idx] = row;
    coo->col_idx[idx] = col;
    memcpy(coo->values + (idx * mat->data_size), value, mat->data_size);

    coo->nnz++;
    coo->sorted = false;
    return NO_ERROR;
}

// --------------------------------------------------------------------------------

static error_code_t _sort_coo_matrix(matrix_t* mat) {
    if (mat == NULL) return NULL_POINTER;
    if (mat->format != COO_MATRIX) return ILLEGAL_STATE;

    coo_matrix_t* coo = &mat->rep.coo;
    if (coo->nnz < 2u) {
        coo->sorted = true;
        return NO_ERROR;
    }

    for (size_t i = 1u; i < coo->nnz; ++i) {
        size_t j = i;
        while (j > 0u) {
            int cmp = _coo_compare_pair(coo->row_idx[j - 1u],
                                        coo->col_idx[j - 1u],
                                        coo->row_idx[j],
                                        coo->col_idx[j]);
            if (cmp <= 0) break;
            _coo_swap_entries(mat, j - 1u, j);
            --j;
        }
    }

    coo->sorted = true;
    return NO_ERROR;
}

// --------------------------------------------------------------------------------

static void _return_dense_matrix(matrix_t* mat) {
    if (mat->rep.dense.data != NULL) {
        mat->alloc_v.return_element(mat->alloc_v.ctx, mat->rep.dense.data);
        mat->rep.dense.data = NULL;
    }
}

// --------------------------------------------------------------------------------

static void _return_coo_matrix(matrix_t* mat) {
    coo_matrix_t* coo = &mat->rep.coo;

    if (coo->row_idx != NULL) {
        mat->alloc_v.return_element(mat->alloc_v.ctx, coo->row_idx);
        coo->row_idx = NULL;
    }

    if (coo->col_idx != NULL) {
        mat->alloc_v.return_element(mat->alloc_v.ctx, coo->col_idx);
        coo->col_idx = NULL;
    }

    if (coo->values != NULL) {
        mat->alloc_v.return_element(mat->alloc_v.ctx, coo->values);
        coo->values = NULL;
    }

    coo->nnz = 0u;
    coo->cap = 0u;
    coo->growth = false;
    coo->sorted = false;
}

// --------------------------------------------------------------------------------

static void _return_csr_matrix(matrix_t* mat) {
    if (mat->rep.csr.row_ptr != NULL) {
        mat->alloc_v.return_element(mat->alloc_v.ctx, mat->rep.csr.row_ptr);
        mat->rep.csr.row_ptr = NULL;
    }

    if (mat->rep.csr.col_idx != NULL) {
        mat->alloc_v.return_element(mat->alloc_v.ctx, mat->rep.csr.col_idx);
        mat->rep.csr.col_idx = NULL;
    }

    if (mat->rep.csr.values != NULL) {
        mat->alloc_v.return_element(mat->alloc_v.ctx, mat->rep.csr.values);
        mat->rep.csr.values = NULL;
    }

    mat->rep.csr.nnz = 0u;
}

// --------------------------------------------------------------------------------

static void _return_csc_matrix(matrix_t* mat) {
    if (mat->rep.csc.col_ptr != NULL) {
        mat->alloc_v.return_element(mat->alloc_v.ctx, mat->rep.csc.col_ptr);
        mat->rep.csc.col_ptr = NULL;
    }

    if (mat->rep.csc.row_idx != NULL) {
        mat->alloc_v.return_element(mat->alloc_v.ctx, mat->rep.csc.row_idx);
        mat->rep.csc.row_idx = NULL;
    }

    if (mat->rep.csc.values != NULL) {
        mat->alloc_v.return_element(mat->alloc_v.ctx, mat->rep.csc.values);
        mat->rep.csc.values = NULL;
    }

    mat->rep.csc.nnz = 0u;
}

// ================================================================================
// Initialization
// ================================================================================

matrix_expect_t init_dense_matrix(size_t             rows,
                                  size_t             cols,
                                  dtype_id_t         dtype,
                                  allocator_vtable_t alloc_v) {
    if (alloc_v.allocate == NULL)
        return (matrix_expect_t){ .has_value = false, .u.error = NULL_POINTER };

    if (rows == 0u || cols == 0u || dtype == UNKNOWN_TYPE)
        return (matrix_expect_t){ .has_value = false, .u.error = INVALID_ARG };

    if (!init_dtype_registry())
        return (matrix_expect_t){ .has_value = false, .u.error = ILLEGAL_STATE };

    const dtype_t* desc = lookup_dtype(dtype);
    if (desc == NULL)
        return (matrix_expect_t){ .has_value = false, .u.error = TYPE_MISMATCH };

    if (rows > SIZE_MAX / cols)
        return (matrix_expect_t){ .has_value = false, .u.error = LENGTH_OVERFLOW };

    size_t elems = rows * cols;
    if (desc->data_size != 0u && elems > SIZE_MAX / desc->data_size)
        return (matrix_expect_t){ .has_value = false, .u.error = LENGTH_OVERFLOW };

    void_ptr_expect_t mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
    if (!mr.has_value)
        return (matrix_expect_t){ .has_value = false, .u.error = BAD_ALLOC };

    matrix_t* mat = (matrix_t*)mr.u.value;

    void_ptr_expect_t dr = alloc_v.allocate(
        alloc_v.ctx, elems * desc->data_size, true
    );
    if (!dr.has_value) {
        alloc_v.return_element(alloc_v.ctx, mat);
        return (matrix_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    mat->rows      = rows;
    mat->cols      = cols;
    mat->data_size = desc->data_size;
    mat->dtype     = dtype;
    mat->format    = DENSE_MATRIX;
    mat->alloc_v   = alloc_v;
    mat->rep.dense.data = (uint8_t*)dr.u.value;

    memset(mat->rep.dense.data, 0, elems * desc->data_size);

    return (matrix_expect_t){ .has_value = true, .u.value = mat };
}

// --------------------------------------------------------------------------------

matrix_expect_t init_coo_matrix(size_t             rows,
                                size_t             cols,
                                size_t             capacity,
                                dtype_id_t         dtype,
                                bool               growth,
                                allocator_vtable_t alloc_v) {
    if (alloc_v.allocate == NULL)
        return (matrix_expect_t){ .has_value = false, .u.error = NULL_POINTER };

    if (rows == 0u || cols == 0u || capacity == 0u || dtype == UNKNOWN_TYPE)
        return (matrix_expect_t){ .has_value = false, .u.error = INVALID_ARG };

    if (!init_dtype_registry())
        return (matrix_expect_t){ .has_value = false, .u.error = ILLEGAL_STATE };

    const dtype_t* desc = lookup_dtype(dtype);
    if (desc == NULL)
        return (matrix_expect_t){ .has_value = false, .u.error = TYPE_MISMATCH };

    if (capacity > SIZE_MAX / sizeof(size_t))
        return (matrix_expect_t){ .has_value = false, .u.error = LENGTH_OVERFLOW };
    if (desc->data_size != 0u && capacity > SIZE_MAX / desc->data_size)
        return (matrix_expect_t){ .has_value = false, .u.error = LENGTH_OVERFLOW };

    void_ptr_expect_t mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
    if (!mr.has_value)
        return (matrix_expect_t){ .has_value = false, .u.error = BAD_ALLOC };

    matrix_t* mat = (matrix_t*)mr.u.value;

    void_ptr_expect_t rr = alloc_v.allocate(
        alloc_v.ctx, capacity * sizeof(size_t), true
    );
    if (!rr.has_value) {
        alloc_v.return_element(alloc_v.ctx, mat);
        return (matrix_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    void_ptr_expect_t cr = alloc_v.allocate(
        alloc_v.ctx, capacity * sizeof(size_t), true
    );
    if (!cr.has_value) {
        alloc_v.return_element(alloc_v.ctx, rr.u.value);
        alloc_v.return_element(alloc_v.ctx, mat);
        return (matrix_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    void_ptr_expect_t vr = alloc_v.allocate(
        alloc_v.ctx, capacity * desc->data_size, true
    );
    if (!vr.has_value) {
        alloc_v.return_element(alloc_v.ctx, rr.u.value);
        alloc_v.return_element(alloc_v.ctx, cr.u.value);
        alloc_v.return_element(alloc_v.ctx, mat);
        return (matrix_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    mat->rows      = rows;
    mat->cols      = cols;
    mat->data_size = desc->data_size;
    mat->dtype     = dtype;
    mat->format    = COO_MATRIX;
    mat->alloc_v   = alloc_v;

    mat->rep.coo.nnz     = 0u;
    mat->rep.coo.cap     = capacity;
    mat->rep.coo.growth  = growth;
    mat->rep.coo.sorted  = true;
    mat->rep.coo.row_idx = (size_t*)rr.u.value;
    mat->rep.coo.col_idx = (size_t*)cr.u.value;
    mat->rep.coo.values  = (uint8_t*)vr.u.value;

    return (matrix_expect_t){ .has_value = true, .u.value = mat };
}

// ================================================================================
// Teardown
// ================================================================================

void return_matrix(matrix_t* mat) {
    if (mat == NULL) return;

    switch (mat->format) {
        case DENSE_MATRIX:
            _return_dense_matrix(mat);
            break;
        case COO_MATRIX:
            _return_coo_matrix(mat);
            break;
        case CSR_MATRIX:
            _return_csr_matrix(mat);
            break;
        case CSC_MATRIX:
            _return_csc_matrix(mat);
            break;
        default:
            break;
    }

    mat->alloc_v.return_element(mat->alloc_v.ctx, mat);
}

// ================================================================================
// Introspection
// ================================================================================

size_t matrix_rows(const matrix_t* mat) {
    return mat ? mat->rows : 0u;
}

// --------------------------------------------------------------------------------

size_t matrix_cols(const matrix_t* mat) {
    return mat ? mat->cols : 0u;
}

// --------------------------------------------------------------------------------

size_t matrix_data_size(const matrix_t* mat) {
    return mat ? mat->data_size : 0u;
}

// --------------------------------------------------------------------------------

dtype_id_t matrix_dtype(const matrix_t* mat) {
    return mat ? mat->dtype : UNKNOWN_TYPE;
}

// --------------------------------------------------------------------------------

matrix_format_t matrix_format(const matrix_t* mat) {
    return mat ? mat->format : DENSE_MATRIX;
}

// --------------------------------------------------------------------------------

size_t matrix_nnz(const matrix_t* mat) {
    if (mat == NULL) return 0u;

    switch (mat->format) {
        case DENSE_MATRIX:
            return mat->rows * mat->cols;
        case COO_MATRIX:
            return mat->rep.coo.nnz;
        case CSR_MATRIX:
            return mat->rep.csr.nnz;
        case CSC_MATRIX:
            return mat->rep.csc.nnz;
        default:
            return 0u;
    }
}

// ================================================================================
// Generic access / mutation
// ================================================================================

error_code_t get_matrix(const matrix_t* mat,
                        size_t          row,
                        size_t          col,
                        void*           out) {
    if ((mat == NULL) || (out == NULL)) {
        return NULL_POINTER;
    }

    switch (mat->format) {
        case DENSE_MATRIX:
            return _get_dense_matrix(mat, row, col, out);

        case COO_MATRIX:
            return _get_coo_matrix(mat, row, col, out);

        case CSR_MATRIX:
            return _get_csr_matrix(mat, row, col, out);

        case CSC_MATRIX:
            return _get_csc_matrix(mat, row, col, out);

        default:
            return ILLEGAL_STATE;
    }
}
// --------------------------------------------------------------------------------

error_code_t set_matrix(matrix_t*    mat,
                        size_t       row,
                        size_t       col,
                        const void*  value) {
    if (mat == NULL || value == NULL) return NULL_POINTER;

    switch (mat->format) {
        case DENSE_MATRIX:
            return _set_dense_matrix(mat, row, col, value);
        case COO_MATRIX:
            return _push_back_coo_matrix(mat, row, col, value);
        case CSR_MATRIX:
        case CSC_MATRIX:
            return ILLEGAL_STATE;
        default:
            return ILLEGAL_STATE;
    }
}

// ================================================================================
// COO assembly helpers
// ================================================================================

error_code_t reserve_coo_matrix(matrix_t* mat, size_t capacity) {
    return _reserve_coo_matrix(mat, capacity);
}

// --------------------------------------------------------------------------------

error_code_t push_back_coo_matrix(matrix_t*    mat,
                                  size_t       row,
                                  size_t       col,
                                  const void*  value) {
    return _push_back_coo_matrix(mat, row, col, value);
}

// --------------------------------------------------------------------------------

error_code_t sort_coo_matrix(matrix_t* mat) {
    return _sort_coo_matrix(mat);
}

// ================================================================================
// Internal helpers (local to this file)
// ================================================================================

static matrix_expect_t _copy_dense_matrix(const matrix_t* src,
                                          allocator_vtable_t alloc_v) {
    matrix_expect_t r = init_dense_matrix(src->rows, src->cols, src->dtype, alloc_v);
    if (!r.has_value) return r;

    memcpy(r.u.value->rep.dense.data,
           src->rep.dense.data,
           src->rows * src->cols * src->data_size);

    return r;
}

// --------------------------------------------------------------------------------

static matrix_expect_t _copy_coo_matrix(const matrix_t* src,
                                        allocator_vtable_t alloc_v) {
    const coo_matrix_t* s = &src->rep.coo;

    matrix_expect_t r = init_coo_matrix(src->rows, src->cols,
                                        s->cap, src->dtype, s->growth, alloc_v);
    if (!r.has_value) return r;

    matrix_t* d = r.u.value;
    coo_matrix_t* dc = &d->rep.coo;

    dc->nnz = s->nnz;
    dc->sorted = s->sorted;

    memcpy(dc->row_idx, s->row_idx, s->nnz * sizeof(size_t));
    memcpy(dc->col_idx, s->col_idx, s->nnz * sizeof(size_t));
    memcpy(dc->values,  s->values,  s->nnz * src->data_size);

    return r;
}
// -------------------------------------------------------------------------------- 

static matrix_expect_t _copy_csr_matrix(const matrix_t* src,
                                        allocator_vtable_t alloc_v) {
    matrix_t* dst = NULL;
    size_t row_ptr_bytes = 0u;
    size_t col_idx_bytes = 0u;
    size_t values_bytes = 0u;

    void_ptr_expect_t mr;
    void_ptr_expect_t row_ptr_r;
    void_ptr_expect_t col_idx_r;
    void_ptr_expect_t values_r;

    if (src == NULL) {
        return (matrix_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }

    mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
    if (!mr.has_value) {
        return (matrix_expect_t){ .has_value = false, .u.error = BAD_ALLOC };
    }

    dst = (matrix_t*)mr.u.value;

    dst->rows = src->rows;
    dst->cols = src->cols;
    dst->dtype = src->dtype;
    dst->data_size = src->data_size;
    dst->format = CSR_MATRIX;
    dst->alloc_v = alloc_v;

    dst->rep.csr.nnz = 0u;
    dst->rep.csr.row_ptr = NULL;
    dst->rep.csr.col_idx = NULL;
    dst->rep.csr.values = NULL;

    row_ptr_bytes = (src->rows + 1u) * sizeof(size_t);
    col_idx_bytes = src->rep.csr.nnz * sizeof(size_t);
    values_bytes  = src->rep.csr.nnz * src->data_size;

    row_ptr_r = alloc_v.allocate(alloc_v.ctx, row_ptr_bytes, true);
    if (!row_ptr_r.has_value) {
        alloc_v.return_element(alloc_v.ctx, dst);
        return (matrix_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    col_idx_r = alloc_v.allocate(alloc_v.ctx, col_idx_bytes, true);
    if (!col_idx_r.has_value) {
        alloc_v.return_element(alloc_v.ctx, row_ptr_r.u.value);
        alloc_v.return_element(alloc_v.ctx, dst);
        return (matrix_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    values_r = alloc_v.allocate(alloc_v.ctx, values_bytes, true);
    if (!values_r.has_value) {
        alloc_v.return_element(alloc_v.ctx, row_ptr_r.u.value);
        alloc_v.return_element(alloc_v.ctx, col_idx_r.u.value);
        alloc_v.return_element(alloc_v.ctx, dst);
        return (matrix_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    dst->rep.csr.nnz = src->rep.csr.nnz;
    dst->rep.csr.row_ptr = (size_t*)row_ptr_r.u.value;
    dst->rep.csr.col_idx = (size_t*)col_idx_r.u.value;
    dst->rep.csr.values  = (uint8_t*)values_r.u.value;

    memcpy(dst->rep.csr.row_ptr, src->rep.csr.row_ptr, row_ptr_bytes);

    if (src->rep.csr.nnz > 0u) {
        memcpy(dst->rep.csr.col_idx, src->rep.csr.col_idx, col_idx_bytes);
        memcpy(dst->rep.csr.values,  src->rep.csr.values,  values_bytes);
    }

    return (matrix_expect_t){ .has_value = true, .u.value = dst };
}
// -------------------------------------------------------------------------------- 

static matrix_expect_t _copy_csc_matrix(const matrix_t* src,
                                        allocator_vtable_t alloc_v) {
    matrix_t* dst = NULL;
    size_t col_ptr_bytes = 0u;
    size_t row_idx_bytes = 0u;
    size_t values_bytes = 0u;

    void_ptr_expect_t mr;
    void_ptr_expect_t col_ptr_r;
    void_ptr_expect_t row_idx_r;
    void_ptr_expect_t values_r;

    if (src == NULL) {
        return (matrix_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }

    mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
    if (!mr.has_value) {
        return (matrix_expect_t){ .has_value = false, .u.error = BAD_ALLOC };
    }

    dst = (matrix_t*)mr.u.value;

    dst->rows = src->rows;
    dst->cols = src->cols;
    dst->dtype = src->dtype;
    dst->data_size = src->data_size;
    dst->format = CSC_MATRIX;
    dst->alloc_v = alloc_v;

    dst->rep.csc.nnz = 0u;
    dst->rep.csc.col_ptr = NULL;
    dst->rep.csc.row_idx = NULL;
    dst->rep.csc.values = NULL;

    col_ptr_bytes = (src->cols + 1u) * sizeof(size_t);
    row_idx_bytes = src->rep.csc.nnz * sizeof(size_t);
    values_bytes  = src->rep.csc.nnz * src->data_size;

    col_ptr_r = alloc_v.allocate(alloc_v.ctx, col_ptr_bytes, true);
    if (!col_ptr_r.has_value) {
        alloc_v.return_element(alloc_v.ctx, dst);
        return (matrix_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    row_idx_r = alloc_v.allocate(alloc_v.ctx, row_idx_bytes, true);
    if (!row_idx_r.has_value) {
        alloc_v.return_element(alloc_v.ctx, col_ptr_r.u.value);
        alloc_v.return_element(alloc_v.ctx, dst);
        return (matrix_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    values_r = alloc_v.allocate(alloc_v.ctx, values_bytes, true);
    if (!values_r.has_value) {
        alloc_v.return_element(alloc_v.ctx, col_ptr_r.u.value);
        alloc_v.return_element(alloc_v.ctx, row_idx_r.u.value);
        alloc_v.return_element(alloc_v.ctx, dst);
        return (matrix_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    dst->rep.csc.nnz = src->rep.csc.nnz;
    dst->rep.csc.col_ptr = (size_t*)col_ptr_r.u.value;
    dst->rep.csc.row_idx = (size_t*)row_idx_r.u.value;
    dst->rep.csc.values  = (uint8_t*)values_r.u.value;

    memcpy(dst->rep.csc.col_ptr, src->rep.csc.col_ptr, col_ptr_bytes);

    if (src->rep.csc.nnz > 0u) {
        memcpy(dst->rep.csc.row_idx, src->rep.csc.row_idx, row_idx_bytes);
        memcpy(dst->rep.csc.values,  src->rep.csc.values,  values_bytes);
    }

    return (matrix_expect_t){ .has_value = true, .u.value = dst };
}
// ================================================================================ 
// ================================================================================ 

static matrix_expect_t _dense_to_coo_matrix(const matrix_t* src,
                                            allocator_vtable_t alloc_v) {
    matrix_expect_t r;
    matrix_t* dst = NULL;
    size_t i = 0u;
    size_t j = 0u;
    const uint8_t* ptr = NULL;
    error_code_t err = NO_ERROR;

    r = init_coo_matrix(src->rows,
                        src->cols,
                        ((src->rows * src->cols) > 0u) ? (src->rows * src->cols) : 1u,
                        src->dtype,
                        false,
                        alloc_v);
    if (!r.has_value) return r;

    dst = r.u.value;

    for (i = 0u; i < src->rows; ++i) {
        for (j = 0u; j < src->cols; ++j) {
            ptr = src->rep.dense.data + _dense_offset(src, i, j);

            if (!_value_is_zero(ptr, src->data_size)) {
                err = _push_back_coo_matrix(dst, i, j, ptr);
                if (err != NO_ERROR) {
                    return_matrix(dst);
                    return (matrix_expect_t){ .has_value = false, .u.error = err };
                }
            }
        }
    }

    dst->rep.coo.sorted = true;
    return r;
}

// --------------------------------------------------------------------------------

static matrix_expect_t _coo_to_dense_matrix(const matrix_t* src,
                                            allocator_vtable_t alloc_v) {
    matrix_expect_t r;
    matrix_t* dst = NULL;
    const coo_matrix_t* coo = NULL;
    size_t i = 0u;
    error_code_t err = NO_ERROR;

    r = init_dense_matrix(src->rows, src->cols, src->dtype, alloc_v);
    if (!r.has_value) return r;

    dst = r.u.value;
    coo = &src->rep.coo;

    for (i = 0u; i < coo->nnz; ++i) {
        err = _set_dense_matrix(dst,
                                coo->row_idx[i],
                                coo->col_idx[i],
                                coo->values + (i * src->data_size));
        if (err != NO_ERROR) {
            return_matrix(dst);
            return (matrix_expect_t){ .has_value = false, .u.error = err };
        }
    }

    return r;
}

// --------------------------------------------------------------------------------

static matrix_expect_t _dense_to_csr_matrix(const matrix_t* src,
                                            allocator_vtable_t alloc_v) {
    matrix_t* dst = NULL;
    size_t i = 0u;
    size_t j = 0u;
    size_t k = 0u;
    size_t nnz = 0u;
    const uint8_t* ptr = NULL;

    for (i = 0u; i < src->rows; ++i) {
        for (j = 0u; j < src->cols; ++j) {
            ptr = src->rep.dense.data + _dense_offset(src, i, j);
            if (!_value_is_zero(ptr, src->data_size)) {
                nnz++;
            }
        }
    }

    void_ptr_expect_t mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
    if (!mr.has_value) {
        return (matrix_expect_t){ .has_value = false, .u.error = BAD_ALLOC };
    }

    dst = (matrix_t*)mr.u.value;

    dst->rows = src->rows;
    dst->cols = src->cols;
    dst->dtype = src->dtype;
    dst->data_size = src->data_size;
    dst->format = CSR_MATRIX;
    dst->alloc_v = alloc_v;

    dst->rep.csr.nnz = 0u;
    dst->rep.csr.row_ptr = NULL;
    dst->rep.csr.col_idx = NULL;
    dst->rep.csr.values = NULL;

    void_ptr_expect_t row_ptr_r = alloc_v.allocate(alloc_v.ctx,
                                                   (src->rows + 1u) * sizeof(size_t),
                                                   true);
    void_ptr_expect_t col_idx_r = alloc_v.allocate(alloc_v.ctx,
                                                   nnz * sizeof(size_t),
                                                   true);
    void_ptr_expect_t values_r  = alloc_v.allocate(alloc_v.ctx,
                                                   nnz * src->data_size,
                                                   true);

    if (!row_ptr_r.has_value || !col_idx_r.has_value || !values_r.has_value) {
        if (row_ptr_r.has_value) alloc_v.return_element(alloc_v.ctx, row_ptr_r.u.value);
        if (col_idx_r.has_value) alloc_v.return_element(alloc_v.ctx, col_idx_r.u.value);
        if (values_r.has_value)  alloc_v.return_element(alloc_v.ctx, values_r.u.value);
        alloc_v.return_element(alloc_v.ctx, dst);
        return (matrix_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    size_t* row_ptr = (size_t*)row_ptr_r.u.value;
    size_t* col_idx = (size_t*)col_idx_r.u.value;
    uint8_t* values = (uint8_t*)values_r.u.value;

    row_ptr[0] = 0u;
    k = 0u;

    for (i = 0u; i < src->rows; ++i) {
        for (j = 0u; j < src->cols; ++j) {
            ptr = src->rep.dense.data + _dense_offset(src, i, j);
            if (!_value_is_zero(ptr, src->data_size)) {
                col_idx[k] = j;
                memcpy(values + (k * src->data_size), ptr, src->data_size);
                k++;
            }
        }
        row_ptr[i + 1u] = k;
    }

    dst->rep.csr.nnz = nnz;
    dst->rep.csr.row_ptr = row_ptr;
    dst->rep.csr.col_idx = col_idx;
    dst->rep.csr.values  = values;

    return (matrix_expect_t){ .has_value = true, .u.value = dst };
}

// --------------------------------------------------------------------------------

static matrix_expect_t _dense_to_csc_matrix(const matrix_t* src,
                                            allocator_vtable_t alloc_v) {
    matrix_t* dst = NULL;
    size_t i = 0u;
    size_t j = 0u;
    size_t k = 0u;
    size_t nnz = 0u;
    const uint8_t* ptr = NULL;

    for (i = 0u; i < src->rows; ++i) {
        for (j = 0u; j < src->cols; ++j) {
            ptr = src->rep.dense.data + _dense_offset(src, i, j);
            if (!_value_is_zero(ptr, src->data_size)) {
                nnz++;
            }
        }
    }

    void_ptr_expect_t mr = alloc_v.allocate(alloc_v.ctx, sizeof(matrix_t), true);
    if (!mr.has_value) {
        return (matrix_expect_t){ .has_value = false, .u.error = BAD_ALLOC };
    }

    dst = (matrix_t*)mr.u.value;

    dst->rows = src->rows;
    dst->cols = src->cols;
    dst->dtype = src->dtype;
    dst->data_size = src->data_size;
    dst->format = CSC_MATRIX;
    dst->alloc_v = alloc_v;

    dst->rep.csc.nnz = 0u;
    dst->rep.csc.col_ptr = NULL;
    dst->rep.csc.row_idx = NULL;
    dst->rep.csc.values = NULL;

    void_ptr_expect_t col_ptr_r = alloc_v.allocate(alloc_v.ctx,
                                                   (src->cols + 1u) * sizeof(size_t),
                                                   true);
    void_ptr_expect_t row_idx_r = alloc_v.allocate(alloc_v.ctx,
                                                   nnz * sizeof(size_t),
                                                   true);
    void_ptr_expect_t values_r  = alloc_v.allocate(alloc_v.ctx,
                                                   nnz * src->data_size,
                                                   true);

    if (!col_ptr_r.has_value || !row_idx_r.has_value || !values_r.has_value) {
        if (col_ptr_r.has_value) alloc_v.return_element(alloc_v.ctx, col_ptr_r.u.value);
        if (row_idx_r.has_value) alloc_v.return_element(alloc_v.ctx, row_idx_r.u.value);
        if (values_r.has_value)  alloc_v.return_element(alloc_v.ctx, values_r.u.value);
        alloc_v.return_element(alloc_v.ctx, dst);
        return (matrix_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }

    size_t* col_ptr = (size_t*)col_ptr_r.u.value;
    size_t* row_idx = (size_t*)row_idx_r.u.value;
    uint8_t* values = (uint8_t*)values_r.u.value;

    col_ptr[0] = 0u;
    k = 0u;

    for (j = 0u; j < src->cols; ++j) {
        for (i = 0u; i < src->rows; ++i) {
            ptr = src->rep.dense.data + _dense_offset(src, i, j);
            if (!_value_is_zero(ptr, src->data_size)) {
                row_idx[k] = i;
                memcpy(values + (k * src->data_size), ptr, src->data_size);
                k++;
            }
        }
        col_ptr[j + 1u] = k;
    }

    dst->rep.csc.nnz = nnz;
    dst->rep.csc.col_ptr = col_ptr;
    dst->rep.csc.row_idx = row_idx;
    dst->rep.csc.values  = values;

    return (matrix_expect_t){ .has_value = true, .u.value = dst };
}

// --------------------------------------------------------------------------------

static matrix_expect_t _coo_to_csr_matrix(const matrix_t* src,
                                          allocator_vtable_t alloc_v) {
    matrix_expect_t tmp;
    matrix_expect_t out;

    tmp = _coo_to_dense_matrix(src, alloc_v);
    if (!tmp.has_value) return tmp;

    out = _dense_to_csr_matrix(tmp.u.value, alloc_v);
    return_matrix(tmp.u.value);
    return out;
}

// --------------------------------------------------------------------------------

static matrix_expect_t _coo_to_csc_matrix(const matrix_t* src,
                                          allocator_vtable_t alloc_v) {
    matrix_expect_t tmp;
    matrix_expect_t out;

    tmp = _coo_to_dense_matrix(src, alloc_v);
    if (!tmp.has_value) return tmp;

    out = _dense_to_csc_matrix(tmp.u.value, alloc_v);
    return_matrix(tmp.u.value);
    return out;
}

// --------------------------------------------------------------------------------

static matrix_expect_t _csr_to_dense_matrix(const matrix_t* src,
                                            allocator_vtable_t alloc_v) {
    matrix_expect_t r;
    matrix_t* dst = NULL;
    size_t i = 0u;
    size_t k = 0u;
    size_t j = 0u;

    r = init_dense_matrix(src->rows, src->cols, src->dtype, alloc_v);
    if (!r.has_value) return r;

    dst = r.u.value;

    for (i = 0u; i < src->rows; ++i) {
        for (k = src->rep.csr.row_ptr[i];
             k < src->rep.csr.row_ptr[i + 1u];
             ++k) {

            j = src->rep.csr.col_idx[k];
            memcpy(dst->rep.dense.data + _dense_offset(dst, i, j),
                   src->rep.csr.values + (k * src->data_size),
                   src->data_size);
        }
    }

    return r;
}

// --------------------------------------------------------------------------------

static matrix_expect_t _csc_to_dense_matrix(const matrix_t* src,
                                            allocator_vtable_t alloc_v) {
    matrix_expect_t r;
    matrix_t* dst = NULL;
    size_t j = 0u;
    size_t k = 0u;
    size_t i = 0u;

    r = init_dense_matrix(src->rows, src->cols, src->dtype, alloc_v);
    if (!r.has_value) return r;

    dst = r.u.value;

    for (j = 0u; j < src->cols; ++j) {
        for (k = src->rep.csc.col_ptr[j];
             k < src->rep.csc.col_ptr[j + 1u];
             ++k) {

            i = src->rep.csc.row_idx[k];
            memcpy(dst->rep.dense.data + _dense_offset(dst, i, j),
                   src->rep.csc.values + (k * src->data_size),
                   src->data_size);
        }
    }

    return r;
}

// --------------------------------------------------------------------------------

static matrix_expect_t _csr_to_coo_matrix(const matrix_t* src,
                                          allocator_vtable_t alloc_v) {
    matrix_expect_t tmp;
    matrix_expect_t out;

    tmp = _csr_to_dense_matrix(src, alloc_v);
    if (!tmp.has_value) return tmp;

    out = _dense_to_coo_matrix(tmp.u.value, alloc_v);
    return_matrix(tmp.u.value);
    return out;
}

// --------------------------------------------------------------------------------

static matrix_expect_t _csc_to_coo_matrix(const matrix_t* src,
                                          allocator_vtable_t alloc_v) {
    matrix_expect_t tmp;
    matrix_expect_t out;

    tmp = _csc_to_dense_matrix(src, alloc_v);
    if (!tmp.has_value) return tmp;

    out = _dense_to_coo_matrix(tmp.u.value, alloc_v);
    return_matrix(tmp.u.value);
    return out;
}

// --------------------------------------------------------------------------------

static matrix_expect_t _csr_to_csc_matrix(const matrix_t* src,
                                          allocator_vtable_t alloc_v) {
    matrix_expect_t tmp;
    matrix_expect_t out;

    tmp = _csr_to_dense_matrix(src, alloc_v);
    if (!tmp.has_value) return tmp;

    out = _dense_to_csc_matrix(tmp.u.value, alloc_v);
    return_matrix(tmp.u.value);
    return out;
}

// --------------------------------------------------------------------------------

static matrix_expect_t _csc_to_csr_matrix(const matrix_t* src,
                                          allocator_vtable_t alloc_v) {
    matrix_expect_t tmp;
    matrix_expect_t out;

    tmp = _csc_to_dense_matrix(src, alloc_v);
    if (!tmp.has_value) return tmp;

    out = _dense_to_csr_matrix(tmp.u.value, alloc_v);
    return_matrix(tmp.u.value);
    return out;
}

// ================================================================================
// Public dispatcher
// ================================================================================

matrix_expect_t convert_matrix(const matrix_t*   src,
                               matrix_format_t   target,
                               allocator_vtable_t alloc_v) {
    if (src == NULL) {
        return (matrix_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }

    if (src->format == target) {
        return copy_matrix(src, alloc_v);
    }

    switch (src->format) {
        case DENSE_MATRIX:
            if (target == COO_MATRIX) return _dense_to_coo_matrix(src, alloc_v);
            if (target == CSR_MATRIX) return _dense_to_csr_matrix(src, alloc_v);
            if (target == CSC_MATRIX) return _dense_to_csc_matrix(src, alloc_v);
            break;

        case COO_MATRIX:
            if (target == DENSE_MATRIX) return _coo_to_dense_matrix(src, alloc_v);
            if (target == CSR_MATRIX)   return _coo_to_csr_matrix(src, alloc_v);
            if (target == CSC_MATRIX)   return _coo_to_csc_matrix(src, alloc_v);
            break;

        case CSR_MATRIX:
            if (target == DENSE_MATRIX) return _csr_to_dense_matrix(src, alloc_v);
            if (target == COO_MATRIX)   return _csr_to_coo_matrix(src, alloc_v);
            if (target == CSC_MATRIX)   return _csr_to_csc_matrix(src, alloc_v);
            break;

        case CSC_MATRIX:
            if (target == DENSE_MATRIX) return _csc_to_dense_matrix(src, alloc_v);
            if (target == COO_MATRIX)   return _csc_to_coo_matrix(src, alloc_v);
            if (target == CSR_MATRIX)   return _csc_to_csr_matrix(src, alloc_v);
            break;

        default:
            break;
    }

    return (matrix_expect_t){ .has_value = false, .u.error = ILLEGAL_STATE };
}
// -------------------------------------------------------------------------------- 

static matrix_expect_t _transpose_dense_matrix(const matrix_t* src,
                                               allocator_vtable_t alloc_v) {
    matrix_expect_t r = init_dense_matrix(src->cols, src->rows, src->dtype, alloc_v);
    if (!r.has_value) return r;

    matrix_t* dst = r.u.value;

    for (size_t i = 0; i < src->rows; ++i) {
        for (size_t j = 0; j < src->cols; ++j) {
            set_matrix(dst, j, i,
                       src->rep.dense.data +
                       ((i * src->cols + j) * src->data_size));
        }
    }

    return r;
}

// --------------------------------------------------------------------------------

static matrix_expect_t _transpose_coo_matrix(const matrix_t* src,
                                             allocator_vtable_t alloc_v) {
    const coo_matrix_t* s = &src->rep.coo;

    matrix_expect_t r = init_coo_matrix(src->cols, src->rows,
                                        s->cap, src->dtype, s->growth, alloc_v);
    if (!r.has_value) return r;

    matrix_t* dst = r.u.value;

    for (size_t i = 0; i < s->nnz; ++i) {
        push_back_coo_matrix(dst,
                             s->col_idx[i],
                             s->row_idx[i],
                             s->values + (i * src->data_size));
    }

    sort_coo_matrix(dst);
    return r;
}
// ================================================================================
// Lifecycle / structural operations
// ================================================================================

error_code_t clear_matrix(matrix_t* mat) {
    if (mat == NULL) return NULL_POINTER;

    switch (mat->format) {
        case DENSE_MATRIX: {
            size_t bytes = mat->rows * mat->cols * mat->data_size;
            memset(mat->rep.dense.data, 0, bytes);
            return NO_ERROR;
        }

        case COO_MATRIX:
            mat->rep.coo.nnz = 0u;
            mat->rep.coo.sorted = true;
            return NO_ERROR;

        default:
            return ILLEGAL_STATE;
    }
}

// --------------------------------------------------------------------------------

matrix_expect_t copy_matrix(const matrix_t*   src,
                            allocator_vtable_t alloc_v) {
    if (src == NULL) {
        return (matrix_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }

    switch (src->format) {
        case DENSE_MATRIX:
            return _copy_dense_matrix(src, alloc_v);

        case COO_MATRIX:
            return _copy_coo_matrix(src, alloc_v);

        case CSR_MATRIX:
            return _copy_csr_matrix(src, alloc_v);

        case CSC_MATRIX:
            return _copy_csc_matrix(src, alloc_v);

        default:
            return (matrix_expect_t){ .has_value = false, .u.error = ILLEGAL_STATE };
    }
}
// --------------------------------------------------------------------------------

matrix_expect_t transpose_matrix(const matrix_t* src,
                                 allocator_vtable_t alloc_v) {
    if (src == NULL)
        return (matrix_expect_t){ .has_value = false, .u.error = NULL_POINTER };

    switch (src->format) {
        case DENSE_MATRIX:
            return _transpose_dense_matrix(src, alloc_v);

        case COO_MATRIX:
            return _transpose_coo_matrix(src, alloc_v);

        default:
            return (matrix_expect_t){ .has_value = false, .u.error = ILLEGAL_STATE };
    }
}

// ================================================================================
// Shape helpers
// ================================================================================

bool matrix_has_same_shape(const matrix_t* a,
                           const matrix_t* b) {
    if (!a || !b) return false;
    return a->rows == b->rows && a->cols == b->cols;
}

// --------------------------------------------------------------------------------

bool matrix_is_square(const matrix_t* mat) {
    return mat && (mat->rows == mat->cols);
}

// --------------------------------------------------------------------------------

bool matrix_is_sparse(const matrix_t* mat) {
    if (!mat) return false;
    return mat->format != DENSE_MATRIX;
}

// ================================================================================
// Introspection
// ================================================================================

size_t matrix_storage_bytes(const matrix_t* mat) {
    if (!mat) return 0;

    switch (mat->format) {
        case DENSE_MATRIX:
            return mat->rows * mat->cols * mat->data_size;

        case COO_MATRIX:
            return (mat->rep.coo.cap * sizeof(size_t) * 2) +
                   (mat->rep.coo.cap * mat->data_size);

        default:
            return 0;
    }
}

// --------------------------------------------------------------------------------

const char* matrix_format_name(matrix_format_t format) {
    switch (format) {
        case DENSE_MATRIX: return "DENSE_MATRIX";
        case COO_MATRIX:   return "COO_MATRIX";
        case CSR_MATRIX:   return "CSR_MATRIX";
        case CSC_MATRIX:   return "CSC_MATRIX";
        default:           return "UNKNOWN_MATRIX_FORMAT";
    }
}

// ================================================================================
// Comparison
// ================================================================================

bool matrix_equal(const matrix_t* a,
                  const matrix_t* b) {
    if (!a || !b) return false;
    if (!matrix_has_same_shape(a, b)) return false;
    if (a->dtype != b->dtype) return false;

    uint8_t* va = (uint8_t*)malloc(a->data_size);
    uint8_t* vb = (uint8_t*)malloc(a->data_size);

    if (!va || !vb) {
        free(va); free(vb);
        return false;
    }

    bool equal = true;

    for (size_t i = 0; i < a->rows && equal; ++i) {
        for (size_t j = 0; j < a->cols; ++j) {
            if (get_matrix(a, i, j, va) != NO_ERROR ||
                get_matrix(b, i, j, vb) != NO_ERROR ||
                memcmp(va, vb, a->data_size) != 0) {
                equal = false;
                break;
            }
        }
    }

    free(va);
    free(vb);
    return equal;
}
// ================================================================================
// ================================================================================
// eof
