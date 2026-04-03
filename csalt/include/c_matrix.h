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

/**
 * @brief Initialize a dense matrix with zero-initialized storage.
 *
 * Allocates and initializes a dense matrix of shape @p rows by @p cols using
 * the allocator in @p alloc_v. All elements are initialized to zero.
 *
 * @param rows     Number of matrix rows. Must be greater than zero.
 * @param cols     Number of matrix columns. Must be greater than zero.
 * @param dtype    Registered runtime data type identifier.
 * @param alloc_v  Allocator used for the matrix header and data buffer.
 *
 * @return A ::matrix_expect_t containing the new matrix on success or an
 *         appropriate ::error_code_t on failure.
 */
matrix_expect_t init_dense_matrix(size_t             rows,
                                  size_t             cols,
                                  dtype_id_t         dtype,
                                  allocator_vtable_t alloc_v);

/**
 * @brief Initialize an empty COO sparse matrix.
 *
 * Allocates and initializes a coordinate-list sparse matrix with storage for
 * up to @p capacity entries. The logical matrix shape is @p rows by @p cols.
 *
 * @param rows      Number of matrix rows. Must be greater than zero.
 * @param cols      Number of matrix columns. Must be greater than zero.
 * @param capacity  Initial entry capacity. Must be greater than zero.
 * @param dtype     Registered runtime data type identifier.
 * @param growth    If true, the COO storage may grow when full.
 * @param alloc_v   Allocator used for the matrix header and COO buffers.
 *
 * @return A ::matrix_expect_t containing the new matrix on success or an
 *         appropriate ::error_code_t on failure.
 */
matrix_expect_t init_coo_matrix(size_t             rows,
                                size_t             cols,
                                size_t             capacity,
                                dtype_id_t         dtype,
                                bool               growth,
                                allocator_vtable_t alloc_v);

/**
 * @brief Release all storage owned by a matrix.
 *
 * Frees the internal buffers associated with the active matrix format and then
 * releases the matrix object itself through its stored allocator.
 *
 * @param mat  Matrix to destroy. If NULL, the call is ignored.
 */
void return_matrix(matrix_t* mat);

/**
 * @brief Return the number of rows in a matrix.
 *
 * @param mat  Matrix to inspect.
 *
 * @return The row count, or 0 if @p mat is NULL.
 */
size_t matrix_rows(const matrix_t* mat);

/**
 * @brief Return the number of columns in a matrix.
 *
 * @param mat  Matrix to inspect.
 *
 * @return The column count, or 0 if @p mat is NULL.
 */
size_t matrix_cols(const matrix_t* mat);

/**
 * @brief Return the size in bytes of one matrix element.
 *
 * @param mat  Matrix to inspect.
 *
 * @return The element size in bytes, or 0 if @p mat is NULL.
 */
size_t matrix_data_size(const matrix_t* mat);

/**
 * @brief Return the runtime dtype identifier for a matrix.
 *
 * @param mat  Matrix to inspect.
 *
 * @return The stored ::dtype_id_t value, or ::UNKNOWN_TYPE if @p mat is NULL.
 */
dtype_id_t matrix_dtype(const matrix_t* mat);

/**
 * @brief Return the active storage format of a matrix.
 *
 * @param mat  Matrix to inspect.
 *
 * @return The active ::matrix_format_t value. If @p mat is NULL, the current
 *         implementation returns ::DENSE_MATRIX.
 */
matrix_format_t matrix_format(const matrix_t* mat);

/**
 * @brief Return the format-dependent entry count for a matrix.
 *
 * For sparse formats this is the stored number of active entries. For dense
 * matrices the current implementation returns @c rows * cols.
 *
 * @param mat  Matrix to inspect.
 *
 * @return The entry count, or 0 if @p mat is NULL.
 */
size_t matrix_nnz(const matrix_t* mat);

/**
 * @brief Read a matrix element at the given row and column.
 *
 * Retrieves the element at (@p row, @p col) and copies it into @p out.
 * For sparse formats, missing entries are returned as zero values.
 *
 * @param mat  Matrix to read from.
 * @param row  Zero-based row index.
 * @param col  Zero-based column index.
 * @param out  Output buffer of at least @c matrix_data_size(mat) bytes.
 *
 * @return ::NO_ERROR on success or an appropriate ::error_code_t on failure.
 */
error_code_t get_matrix(const matrix_t* mat,
                        size_t          row,
                        size_t          col,
                        void*           out);

/**
 * @brief Write a matrix element at the given row and column.
 *
 * Sets the element at (@p row, @p col) from the bytes pointed to by @p value.
 * The current implementation supports mutation for dense and COO matrices.
 * CSR and CSC matrices currently return an error for direct mutation.
 *
 * @param mat    Matrix to modify.
 * @param row    Zero-based row index.
 * @param col    Zero-based column index.
 * @param value  Pointer to the value to write.
 *
 * @return ::NO_ERROR on success or an appropriate ::error_code_t on failure.
 */
error_code_t set_matrix(matrix_t*    mat,
                        size_t       row,
                        size_t       col,
                        const void*  value);

/**
 * @brief Reserve additional entry capacity for a COO matrix.
 *
 * Reallocates the internal COO buffers when @p capacity exceeds the current
 * capacity. Existing entries are preserved.
 *
 * @param mat       COO matrix to grow.
 * @param capacity  Requested minimum entry capacity.
 *
 * @return ::NO_ERROR on success or an appropriate ::error_code_t on failure.
 */
error_code_t reserve_coo_matrix(matrix_t* mat, size_t capacity);

/**
 * @brief Append a raw entry to a COO matrix.
 *
 * Adds a new COO entry at (@p row, @p col) using the bytes pointed to by
 * @p value. The matrix is marked unsorted after insertion.
 *
 * @param mat    COO matrix to append to.
 * @param row    Zero-based row index.
 * @param col    Zero-based column index.
 * @param value  Pointer to the value to insert.
 *
 * @return ::NO_ERROR on success or an appropriate ::error_code_t on failure.
 */
error_code_t push_back_coo_matrix(matrix_t*    mat,
                                  size_t       row,
                                  size_t       col,
                                  const void*  value);

/**
 * @brief Sort COO entries by row and then column.
 *
 * Reorders the COO entry arrays into canonical row-major order and marks the
 * matrix as sorted.
 *
 * @param mat  COO matrix to sort.
 *
 * @return ::NO_ERROR on success or an appropriate ::error_code_t on failure.
 */
error_code_t sort_coo_matrix(matrix_t* mat);

/**
 * @brief Clear a matrix while preserving its allocated storage.
 *
 * Dense matrices are zeroed in place. COO matrices are reset to zero logical
 * entries. The current implementation reports an error for CSR and CSC
 * matrices.
 *
 * @param mat  Matrix to clear.
 *
 * @return ::NO_ERROR on success or an appropriate ::error_code_t on failure.
 */
error_code_t clear_matrix(matrix_t* mat);

/**
 * @brief Deep-copy a matrix into newly allocated storage.
 *
 * Creates a logically equivalent matrix using @p alloc_v for all newly owned
 * storage while preserving the source format.
 *
 * @param src      Matrix to copy.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return A ::matrix_expect_t containing the copied matrix on success or an
 *         appropriate ::error_code_t on failure.
 */
matrix_expect_t copy_matrix(const matrix_t*   src,
                            allocator_vtable_t alloc_v);

/**
 * @brief Convert a matrix to a different storage format.
 *
 * Produces a new matrix with the same logical values as @p src in the target
 * format given by @p target.
 *
 * @param src      Matrix to convert.
 * @param target   Desired destination storage format.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return A ::matrix_expect_t containing the converted matrix on success or an
 *         appropriate ::error_code_t on failure.
 */
matrix_expect_t convert_matrix(const matrix_t*   src,
                               matrix_format_t   target,
                               allocator_vtable_t alloc_v);

/**
 * @brief Compute the matrix transpose.
 *
 * Produces a new matrix with rows and columns exchanged. Dense matrices remain
 * dense, COO remains COO, CSR transposes to CSC, and CSC transposes to CSR.
 *
 * @param src      Matrix to transpose.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return A ::matrix_expect_t containing the transposed matrix on success or an
 *         appropriate ::error_code_t on failure.
 */
matrix_expect_t transpose_matrix(const matrix_t*   src,
                                 allocator_vtable_t alloc_v);

/**
 * @brief Test whether two matrices have identical shape.
 *
 * @param a  First matrix.
 * @param b  Second matrix.
 *
 * @return true if both matrices are non-NULL and have the same row and column
 *         counts; otherwise false.
 */
bool matrix_has_same_shape(const matrix_t* a,
                           const matrix_t* b);

/**
 * @brief Test whether a matrix is square.
 *
 * @param mat  Matrix to inspect.
 *
 * @return true if @p mat is non-NULL and has @c rows == cols; otherwise false.
 */
bool matrix_is_square(const matrix_t* mat);

/**
 * @brief Test whether a matrix uses a sparse representation.
 *
 * @param mat  Matrix to inspect.
 *
 * @return true for COO, CSR, and CSC matrices; false for dense or NULL inputs.
 */
bool matrix_is_sparse(const matrix_t* mat);

/**
 * @brief Return the storage footprint of the active matrix payload.
 *
 * The current implementation reports payload bytes for dense and COO matrices.
 * Other formats currently return zero.
 *
 * @param mat  Matrix to inspect.
 *
 * @return Storage size in bytes for the active representation, or 0 if unknown
 *         or unsupported by the current implementation.
 */
size_t matrix_storage_bytes(const matrix_t* mat);

/**
 * @brief Return a human-readable name for a matrix format.
 *
 * @param format  Format enumeration value.
 *
 * @return Constant string naming the format, or
 *         @c "UNKNOWN_MATRIX_FORMAT" for unrecognized values.
 */
const char* matrix_format_name(matrix_format_t format);

/**
 * @brief Compare two matrices for logical equality.
 *
 * Matrices are considered equal when they have the same shape, the same dtype,
 * and identical element values at every logical position.
 *
 * @param a  First matrix.
 * @param b  Second matrix.
 *
 * @return true if the matrices are logically equal; otherwise false.
 */
bool matrix_equal(const matrix_t* a,
                  const matrix_t* b);

/**
 * @brief Reset a matrix to the logical zero matrix.
 *
 * Dense matrices are zero-filled. Sparse matrices are reset to zero logical
 * entries while preserving their allocated storage.
 *
 * @param mat  Matrix to reset.
 *
 * @return ::NO_ERROR on success or an appropriate ::error_code_t on failure.
 */
error_code_t zero_matrix(matrix_t* mat);

/**
 * @brief Fill a matrix with a single repeated value.
 *
 * Nonzero fill is currently supported only for dense matrices. Supplying a zero
 * value resets the matrix through ::zero_matrix().
 *
 * @param mat    Matrix to fill.
 * @param value  Pointer to the fill value.
 *
 * @return ::NO_ERROR on success or an appropriate ::error_code_t on failure.
 */
error_code_t fill_matrix(matrix_t* mat,
                         const void* value);

/**
 * @brief Test whether a matrix is logically all zeros.
 *
 * For dense matrices, all stored elements are checked. For sparse matrices, the
 * result is true when the stored sparse entry count is zero.
 *
 * @param mat  Matrix to inspect.
 *
 * @return true if the matrix is logically zero; otherwise false.
 */
bool is_zero_matrix(const matrix_t* mat);

/**
 * @brief Test whether two matrices store the same runtime dtype.
 *
 * @param a  First matrix.
 * @param b  Second matrix.
 *
 * @return true if both matrices are non-NULL and have the same dtype;
 *         otherwise false.
 */
bool matrix_has_same_dtype(const matrix_t* a,
                           const matrix_t* b);

/**
 * @brief Test whether two matrices are compatible for elementwise addition.
 *
 * Addition compatibility requires equal shape and equal dtype.
 *
 * @param a  First matrix.
 * @param b  Second matrix.
 *
 * @return true if the matrices are add-compatible; otherwise false.
 */
bool matrix_is_add_compatible(const matrix_t* a,
                              const matrix_t* b);

/**
 * @brief Test whether two matrices are compatible for matrix multiplication.
 *
 * Multiplication compatibility requires @c a->cols == b->rows and equal dtype.
 *
 * @param a  Left-hand matrix.
 * @param b  Right-hand matrix.
 *
 * @return true if the matrices are multiply-compatible; otherwise false.
 */
bool matrix_is_multiply_compatible(const matrix_t* a,
                                   const matrix_t* b);

/**
 * @brief Swap two rows of a matrix in place.
 *
 * Dense and COO matrices are currently supported. Compressed sparse formats
 * currently report the operation as unavailable.
 *
 * @param mat  Matrix to modify.
 * @param r1   First zero-based row index.
 * @param r2   Second zero-based row index.
 *
 * @return ::NO_ERROR on success or an appropriate ::error_code_t on failure.
 */
error_code_t swap_matrix_rows(matrix_t* mat,
                              size_t r1,
                              size_t r2);

/**
 * @brief Swap two columns of a matrix in place.
 *
 * Dense and COO matrices are currently supported. Compressed sparse formats
 * currently report the operation as unavailable.
 *
 * @param mat  Matrix to modify.
 * @param c1   First zero-based column index.
 * @param c2   Second zero-based column index.
 *
 * @return ::NO_ERROR on success or an appropriate ::error_code_t on failure.
 */
error_code_t swap_matrix_cols(matrix_t* mat,
                              size_t c1,
                              size_t c2);

/**
 * @brief Initialize a dense identity matrix.
 *
 * Creates an @p n by @p n dense matrix with ones on the diagonal and zeros
 * elsewhere.
 *
 * @param n        Matrix order.
 * @param dtype    Registered runtime data type identifier.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return A ::matrix_expect_t containing the identity matrix on success or an
 *         appropriate ::error_code_t on failure.
 */
matrix_expect_t init_identity_matrix(size_t n,
                                     dtype_id_t dtype,
                                     allocator_vtable_t alloc_v);

/**
 * @brief Initialize a dense row vector.
 *
 * Creates a dense matrix of shape @c 1 x length.
 *
 * @param length   Vector length.
 * @param dtype    Registered runtime data type identifier.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return A ::matrix_expect_t containing the row vector on success or an
 *         appropriate ::error_code_t on failure.
 */
matrix_expect_t init_row_vector(size_t length,
                                dtype_id_t dtype,
                                allocator_vtable_t alloc_v);

/**
 * @brief Initialize a dense column vector.
 *
 * Creates a dense matrix of shape @c length x 1.
 *
 * @param length   Vector length.
 * @param dtype    Registered runtime data type identifier.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return A ::matrix_expect_t containing the column vector on success or an
 *         appropriate ::error_code_t on failure.
 */
matrix_expect_t init_col_vector(size_t length,
                                dtype_id_t dtype,
                                allocator_vtable_t alloc_v);

/**
 * @brief Test whether a matrix has row-vector shape.
 *
 * A row vector is any matrix with exactly one row.
 *
 * @param mat  Matrix to inspect.
 *
 * @return true if @p mat is non-NULL and has @c rows == 1; otherwise false.
 */
bool matrix_is_row_vector(const matrix_t* mat);

/**
 * @brief Test whether a matrix has column-vector shape.
 *
 * A column vector is any matrix with exactly one column.
 *
 * @param mat  Matrix to inspect.
 *
 * @return true if @p mat is non-NULL and has @c cols == 1; otherwise false.
 */
bool matrix_is_col_vector(const matrix_t* mat);

/**
 * @brief Test whether a matrix has vector shape.
 *
 * A matrix is treated as a vector when it is either a row vector or a column
 * vector.
 *
 * @param mat  Matrix to inspect.
 *
 * @return true if @p mat has vector shape; otherwise false.
 */
bool matrix_is_vector(const matrix_t* mat);

/**
 * @brief Return the logical length of a vector-shaped matrix.
 *
 * For row vectors the length is the column count. For column vectors the length
 * is the row count. Non-vector matrices return zero.
 *
 * @param mat  Matrix to inspect.
 *
 * @return Vector length, or 0 if @p mat is NULL or not vector-shaped.
 */
size_t matrix_vector_length(const matrix_t* mat);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_matrix_H */
// ================================================================================
// ================================================================================
// eof
