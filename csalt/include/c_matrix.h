// ================================================================================
// ================================================================================
// - File:    c_matrix.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    April 03, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_matrix_H
#define c_matrix_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "c_allocator.h"
#include "c_error.h"
#include "c_dtypes.h"
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

typedef enum {
    DENSE_MATRIX = 0,
    COO_MATRIX   = 1,
    CSR_MATRIX   = 2,
    CSC_MATRIX   = 3
} matrix_format_t;

// ================================================================================
// Dense matrix representation
// ================================================================================

typedef struct {
    uint8_t* data;   /**< Flat contiguous data buffer. */
} dense_matrix_t;

// ================================================================================
// COO matrix representation
// ================================================================================

typedef struct {
    size_t   nnz;       /**< Number of active entries.                  */
    size_t   cap;       /**< Allocated entry capacity.                  */
    bool     growth;    /**< Allow dynamic growth when capacity full.   */
    bool     sorted;    /**< True if entries sorted by (row, col).      */
    size_t*  row_idx;   /**< Row indices, length = cap.                 */
    size_t*  col_idx;   /**< Column indices, length = cap.              */
    uint8_t* values;    /**< Value buffer, size = cap * data_size.      */
} coo_matrix_t;

// ================================================================================
// CSR matrix representation
// ================================================================================

typedef struct {
    size_t   nnz;       /**< Number of nonzero entries.                 */
    size_t*  row_ptr;   /**< Row pointer array, length = rows + 1.      */
    size_t*  col_idx;   /**< Column index array, length = nnz.          */
    uint8_t* values;    /**< Value buffer, size = nnz * data_size.      */
} csr_matrix_t;

// ================================================================================
// CSC matrix representation
// ================================================================================

typedef struct {
    size_t   nnz;       /**< Number of nonzero entries.                 */
    size_t*  col_ptr;   /**< Column pointer array, length = cols + 1.   */
    size_t*  row_idx;   /**< Row index array, length = nnz.             */
    uint8_t* values;    /**< Value buffer, size = nnz * data_size.      */
} csc_matrix_t;

// ================================================================================
// Generic matrix type
// ================================================================================

typedef struct {
    size_t             rows;       /**< Number of rows.                         */
    size_t             cols;       /**< Number of columns.                      */
    size_t             data_size;  /**< Size of one element in bytes.           */
    dtype_id_t         dtype;      /**< Runtime data type identifier.           */
    matrix_format_t    format;     /**< Active matrix representation.           */
    allocator_vtable_t alloc_v;    /**< Allocator for all owned memory.         */

    union {
        dense_matrix_t dense;      /**< Dense representation.                   */
        coo_matrix_t   coo;        /**< COO representation.                     */
        csr_matrix_t   csr;        /**< CSR representation.                     */
        csc_matrix_t   csc;        /**< CSC representation.                     */
    } rep;
} matrix_t;

// ================================================================================
// Expected return type
// ================================================================================

typedef struct {
    bool has_value;
    union {
        matrix_t*     value;
        error_code_t  error;
    } u;
} matrix_expect_t;

// ================================================================================
// Initialization
// ================================================================================

matrix_expect_t init_dense_matrix(size_t             rows,
                                  size_t             cols,
                                  dtype_id_t         dtype,
                                  allocator_vtable_t alloc_v);

matrix_expect_t init_coo_matrix(size_t             rows,
                                size_t             cols,
                                size_t             capacity,
                                dtype_id_t         dtype,
                                bool               growth,
                                allocator_vtable_t alloc_v);

// ================================================================================
// Teardown
// ================================================================================

void return_matrix(matrix_t* mat);

// ================================================================================
// Introspection
// ================================================================================

size_t matrix_rows(const matrix_t* mat);
size_t matrix_cols(const matrix_t* mat);
size_t matrix_data_size(const matrix_t* mat);
dtype_id_t matrix_dtype(const matrix_t* mat);
matrix_format_t matrix_format(const matrix_t* mat);
size_t matrix_nnz(const matrix_t* mat);

// ================================================================================
// Generic access / mutation
// ================================================================================

error_code_t get_matrix(const matrix_t* mat,
                        size_t          row,
                        size_t          col,
                        void*           out);

error_code_t set_matrix(matrix_t*    mat,
                        size_t       row,
                        size_t       col,
                        const void*  value);

// ================================================================================
// COO assembly helpers
// ================================================================================

error_code_t reserve_coo_matrix(matrix_t* mat, size_t capacity);

error_code_t push_back_coo_matrix(matrix_t*    mat,
                                  size_t       row,
                                  size_t       col,
                                  const void*  value);

error_code_t sort_coo_matrix(matrix_t* mat);

// ------------------------------------------------------------------------------
// Lifecycle / structural operations
// ------------------------------------------------------------------------------

error_code_t clear_matrix(matrix_t* mat);

matrix_expect_t copy_matrix(const matrix_t*   src,
                            allocator_vtable_t alloc_v);

matrix_expect_t convert_matrix(const matrix_t*   src,
                               matrix_format_t   target,
                               allocator_vtable_t alloc_v);

matrix_expect_t transpose_matrix(const matrix_t*   src,
                                 allocator_vtable_t alloc_v);

// ------------------------------------------------------------------------------
// Shape / structure helpers
// ------------------------------------------------------------------------------

bool matrix_has_same_shape(const matrix_t* a,
                           const matrix_t* b);

bool matrix_is_square(const matrix_t* mat);

bool matrix_is_sparse(const matrix_t* mat);

// ------------------------------------------------------------------------------
// Introspection / diagnostics
// ------------------------------------------------------------------------------

size_t matrix_storage_bytes(const matrix_t* mat);

const char* matrix_format_name(matrix_format_t format);

// ------------------------------------------------------------------------------
// Comparison
// ------------------------------------------------------------------------------

bool matrix_equal(const matrix_t* a,
                  const matrix_t* b);
// ================================================================================ 
// ================================================================================ 

// ------------------------------------------------------------------------------
// Additional helpers
// ------------------------------------------------------------------------------

error_code_t zero_matrix(matrix_t* mat);

error_code_t fill_matrix(matrix_t* mat,
                         const void* value);

bool is_zero_matrix(const matrix_t* mat);

bool matrix_has_same_dtype(const matrix_t* a,
                           const matrix_t* b);

bool matrix_is_add_compatible(const matrix_t* a,
                              const matrix_t* b);

bool matrix_is_multiply_compatible(const matrix_t* a,
                                   const matrix_t* b);

error_code_t swap_matrix_rows(matrix_t* mat,
                              size_t r1,
                              size_t r2);

error_code_t swap_matrix_cols(matrix_t* mat,
                              size_t c1,
                              size_t c2);

matrix_expect_t init_identity_matrix(size_t n,
                                     dtype_id_t dtype,
                                     allocator_vtable_t alloc_v);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_matrix_H */
// ================================================================================
// ================================================================================
// eof
