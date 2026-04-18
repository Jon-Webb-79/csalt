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

/**
 * @brief Storage format selector for matrix_t.
 */
typedef enum {
    DENSE_MATRIX = 0,   /**< Row-major contiguous buffer.                   */
    COO_MATRIX   = 1,   /**< Coordinate list (row, col, value) triplets.    */
    CSR_MATRIX   = 2,   /**< Compressed Sparse Row.                         */
    CSC_MATRIX   = 3    /**< Compressed Sparse Column.                      */
} matrix_format_t;
 
// ================================================================================
// Storage representations
// ================================================================================
 
/**
 * @brief Dense storage: flat row-major buffer.
 *
 * Element at (row, col) is at offset (row * cols + col) * data_size.
 *
 * @note Type wrappers can cast @c data to @c T* for direct SIMD access
 *       to the contiguous buffer.
 */
typedef struct {
    uint8_t* data;   /**< Flat contiguous data buffer. */
} dense_matrix_t;
 
// --------------------------------------------------------------------------------
 
/**
 * @brief COO storage: three parallel arrays (row_idx, col_idx, values).
 *
 * Entries are stored in insertion order unless @c sorted is true, in which
 * case they are in row-major (row, col) order and binary search is used
 * for lookups.
 */
typedef struct {
    size_t   nnz;       /**< Number of active entries.                  */
    size_t   cap;       /**< Allocated entry capacity.                  */
    bool     growth;    /**< Allow dynamic growth when capacity full.   */
    bool     sorted;    /**< True if entries sorted by (row, col).      */
    size_t*  row_idx;   /**< Row indices, length = cap.                 */
    size_t*  col_idx;   /**< Column indices, length = cap.              */
    uint8_t* values;    /**< Value buffer, size = cap * data_size.      */
} coo_matrix_t;
 
// --------------------------------------------------------------------------------
 
/**
 * @brief CSR storage: compressed row pointers with column indices.
 *
 * Row @c i contains entries at indices row_ptr[i] .. row_ptr[i+1]-1 in
 * the col_idx and values arrays.
 */
typedef struct {
    size_t   nnz;       /**< Number of nonzero entries.                 */
    size_t*  row_ptr;   /**< Row pointer array, length = rows + 1.      */
    size_t*  col_idx;   /**< Column index array, length = nnz.          */
    uint8_t* values;    /**< Value buffer, size = nnz * data_size.      */
} csr_matrix_t;
 
// --------------------------------------------------------------------------------
 
/**
 * @brief CSC storage: compressed column pointers with row indices.
 *
 * Column @c j contains entries at indices col_ptr[j] .. col_ptr[j+1]-1
 * in the row_idx and values arrays.
 */
typedef struct {
    size_t   nnz;       /**< Number of nonzero entries.                 */
    size_t*  col_ptr;   /**< Column pointer array, length = cols + 1.   */
    size_t*  row_idx;   /**< Row index array, length = nnz.             */
    uint8_t* values;    /**< Value buffer, size = nnz * data_size.      */
} csc_matrix_t;
 
// ================================================================================
// Generic matrix type
// ================================================================================
 
/**
 * @brief Generic matrix supporting multiple storage formats.
 *
 * The active representation is selected by @c format.  Only the
 * corresponding union member is valid at any time.
 *
 * @note Type wrappers should typedef this directly:
 *       @code typedef matrix_t float_matrix_t; @endcode
 *       The wrapper functions then fix @c dtype and cast @c void* to @c T*.
 */
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
 
/**
 * @brief Result type for matrix operations that may fail.
 *
 * Check @c has_value before accessing @c u.value.
 *
 * @note Type wrappers should define an equivalent struct with
 *       @c T_matrix_t* in the union and a helper to convert:
 *       @code{.c}
 *       static inline T_matrix_expect_t wrap(matrix_expect_t e) {
 *           if (e.has_value)
 *               return (T_matrix_expect_t){ true, .u.value = (T_matrix_t*)e.u.value };
 *           return (T_matrix_expect_t){ false, .u.error = e.u.error };
 *       }
 *       @endcode
 */
typedef struct {
    bool has_value;
    union {
        matrix_t*     value;
        error_code_t  error;
    } u;
} matrix_expect_t;

// ================================================================================
// Callback types for semantic value operations
// ================================================================================

/**
 * @brief Semantic equality predicate for one matrix element.
 *
 * The callback receives pointers to two values of the same dtype and
 * returns true if they are logically equal.
 */
typedef bool (*matrix_equal_fn)(const void* a,
                                const void* b);

/**
 * @brief Semantic zero-test predicate for one matrix element.
 *
 * The callback receives a pointer to one value and returns true if it
 * should be treated as zero.
 */
typedef bool (*matrix_zero_fn)(const void* value);
 
// ================================================================================
// Initialization and teardown
// ================================================================================
 
/**
 * @brief Allocate a dense matrix, zero-initialized.
 *
 * All elements are set to zero bytes via memset.  The caller must free
 * the matrix with return_matrix when no longer needed.
 *
 * Wrapper: fix @p dtype to the type constant (e.g. FLOAT_TYPE).
 *
 * @param rows     Number of rows.  Must be > 0.
 * @param cols     Number of columns.  Must be > 0.
 * @param dtype    Registered runtime data type identifier.  Must not be
 *                 UNKNOWN_TYPE.
 * @param alloc_v  Allocator vtable.  alloc_v.allocate must not be NULL.
 *
 * @return matrix_expect_t with has_value true on success, or u.error:
 *         - NULL_POINTER    — alloc_v.allocate is NULL
 *         - INVALID_ARG     — rows or cols is 0, or dtype is UNKNOWN_TYPE
 *         - ILLEGAL_STATE   — dtype registry failed to initialize
 *         - TYPE_MISMATCH   — dtype not found in the registry
 *         - LENGTH_OVERFLOW — rows * cols * data_size overflows size_t
 *         - BAD_ALLOC       — matrix_t struct allocation failed
 *         - OUT_OF_MEMORY   — data buffer allocation failed
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(3, 3, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t value = 5;
 * set_matrix(mat, 1, 1, &value);
 *
 * return_matrix(mat);
 * @endcode
 */
matrix_expect_t init_dense_matrix(size_t             rows,
                                  size_t             cols,
                                  dtype_id_t         dtype,
                                  allocator_vtable_t alloc_v);
// -------------------------------------------------------------------------------- 
 
/**
 * @brief Allocate an empty COO matrix with the given entry capacity.
 *
 * The matrix starts with zero stored entries (nnz = 0).  Use
 * push_back_coo_matrix to add entries.
 *
 * Wrapper: fix @p dtype.
 *
 * @param rows      Number of rows.  Must be > 0.
 * @param cols      Number of columns.  Must be > 0.
 * @param capacity  Initial number of entry slots.  Must be > 0.
 * @param dtype     Registered runtime data type identifier.  Must not be
 *                  UNKNOWN_TYPE.
 * @param growth    If true, buffers grow automatically when full.
 * @param alloc_v   Allocator vtable.  alloc_v.allocate must not be NULL.
 *
 * @return matrix_expect_t with has_value true on success, or u.error:
 *         - NULL_POINTER    — alloc_v.allocate is NULL
 *         - INVALID_ARG     — rows, cols, or capacity is 0, or dtype is
 *                             UNKNOWN_TYPE
 *         - ILLEGAL_STATE   — dtype registry failed to initialize
 *         - TYPE_MISMATCH   — dtype not found in the registry
 *         - LENGTH_OVERFLOW — capacity * sizeof(size_t) or capacity *
 *                             data_size overflows size_t
 *         - BAD_ALLOC       — matrix_t struct allocation failed
 *         - OUT_OF_MEMORY   — row_idx, col_idx, or values buffer
 *                             allocation failed
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_coo_matrix(4, 4, 8, INT32_TYPE, true, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t v1 = 10;
 * int32_t v2 = 20;
 *
 * push_back_coo_matrix(mat, 0, 1, &v1);
 * push_back_coo_matrix(mat, 3, 2, &v2);
 *
 * sort_coo_matrix(mat);
 *
 * return_matrix(mat);
 * @endcode
 */
matrix_expect_t init_coo_matrix(size_t             rows,
                                size_t             cols,
                                size_t             capacity,
                                dtype_id_t         dtype,
                                bool               growth,
                                allocator_vtable_t alloc_v);
// -------------------------------------------------------------------------------- 
 
/**
 * @brief Free all storage owned by a matrix.
 *
 * Releases internal buffers for the active format, then frees the
 * matrix_t struct itself through its stored allocator.  NULL-safe.
 *
 * Wrapper: delegate directly.
 *
 * @param mat  Matrix to destroy, or NULL (no-op).
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(2, 2, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t value = 42;
 * set_matrix(mat, 0, 0, &value);
 *
 * return_matrix(mat);
 *
 * // Safe: no-op
 * return_matrix(NULL);
 * @endcode
 */
void return_matrix(matrix_t* mat);
// ================================================================================
// Introspection — delegate directly, no performance concern
// ================================================================================
 
/**
 * @brief Return the number of rows.
 * @param mat  Matrix to inspect.
 * @return Row count, or 0 if @p mat is NULL.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(3, 4, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * size_t rows = matrix_rows(mat);   // rows == 3
 *
 * return_matrix(mat);
 * @endcode
 */
size_t matrix_rows(const matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the number of columns.
 * @param mat  Matrix to inspect.
 * @return Column count, or 0 if @p mat is NULL.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(3, 4, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * size_t cols = matrix_cols(mat);   // cols == 4
 *
 * return_matrix(mat);
 * @endcode
 */
size_t matrix_cols(const matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the element size in bytes.
 * @param mat  Matrix to inspect.
 * @return Element size, or 0 if @p mat is NULL.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(2, 2, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * size_t elem_size = matrix_data_size(mat);  // elem_size == sizeof(int32_t)
 *
 * return_matrix(mat);
 * @endcode
 */
size_t matrix_data_size(const matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the runtime dtype identifier.
 * @param mat  Matrix to inspect.
 * @return dtype, or UNKNOWN_TYPE if @p mat is NULL.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(2, 2, FLOAT_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * dtype_id_t dtype = matrix_dtype(mat);  // dtype == FLOAT_TYPE
 *
 * return_matrix(mat);
 * @endcode
 */
dtype_id_t matrix_dtype(const matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the active storage format.
 * @param mat  Matrix to inspect.
 * @return Format enum, or DENSE_MATRIX if @p mat is NULL.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_coo_matrix(4, 4, 8, INT32_TYPE, true, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * matrix_format_t fmt = matrix_format(mat);  // fmt == COO_MATRIX
 *
 * return_matrix(mat);
 * @endcode
 */
matrix_format_t matrix_format(const matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the entry count.
 *
 * Dense: rows × cols.  COO/CSR/CSC: stored nonzero count.
 *
 * @param mat  Matrix to inspect.
 * @return Entry count, or 0 if @p mat is NULL.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_coo_matrix(4, 4, 8, INT32_TYPE, true, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t v1 = 10;
 * int32_t v2 = 20;
 *
 * push_back_coo_matrix(mat, 0, 1, &v1);
 * push_back_coo_matrix(mat, 3, 2, &v2);
 *
 * size_t nnz = matrix_nnz(mat);   // nnz == 2
 *
 * return_matrix(mat);
 * @endcode
 */
size_t matrix_nnz(const matrix_t* mat);
// ================================================================================
// Element access
// ================================================================================
 
/**
 * @brief Read element at (row, col) into @p out.
 *
 * Dispatches to the format-specific getter.  For sparse formats, entries
 * not explicitly stored are returned as zero bytes.
 *
 * Wrapper: replace @c void* out with @c T* out.
 *
 * @param mat  Matrix to read.  Must not be NULL.
 * @param row  Zero-based row index.  Must be < mat->rows.
 * @param col  Zero-based column index.  Must be < mat->cols.
 * @param out  Output buffer of at least data_size bytes.  Must not be NULL.
 *
 * @return NO_ERROR on success, or:
 *         - NULL_POINTER  — mat or out is NULL
 *         - INVALID_ARG   — (row, col) out of bounds, or out is NULL
 *         - ILLEGAL_STATE — unrecognized format
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(3, 3, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t value = 42;
 * set_matrix(mat, 1, 2, &value);
 *
 * int32_t out = 0;
 * if (get_matrix(mat, 1, 2, &out) == NO_ERROR) {
 *     // out == 42
 * }
 *
 * return_matrix(mat);
 * @endcode
 */
error_code_t get_matrix(const matrix_t* mat,
                        size_t          row,
                        size_t          col,
                        void*           out);
// -------------------------------------------------------------------------------- 
 
/**
 * @brief Write element at (row, col) from @p value.
 *
 * Dense: direct overwrite.  COO: inserts or overwrites the entry at
 * (row, col).  CSR/CSC: not supported (return ILLEGAL_STATE).
 *
 * Wrapper: take @c T value by value, pass @c &value.
 *
 * @param mat    Matrix to modify.  Must not be NULL.
 * @param row    Zero-based row index.  Must be < mat->rows.
 * @param col    Zero-based column index.  Must be < mat->cols.
 * @param value  Pointer to the value to write.  Must not be NULL.
 *
 * @return NO_ERROR on success, or:
 *         - NULL_POINTER      — mat or value is NULL
 *         - INVALID_ARG       — (row, col) out of bounds
 *         - ILLEGAL_STATE     — format is CSR or CSC
 *         - CAPACITY_OVERFLOW — COO is full and growth is false
 *         - LENGTH_OVERFLOW   — COO growth would overflow size_t
 *         - OUT_OF_MEMORY     — COO growth allocation failed
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(2, 2, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t value = 7;
 * if (set_matrix(mat, 0, 1, &value) != NO_ERROR) {
 *     // handle error
 * }
 *
 * int32_t out = 0;
 * get_matrix(mat, 0, 1, &out);   // out == 7
 *
 * return_matrix(mat);
 * @endcode
 */
error_code_t set_matrix(matrix_t*    mat,
                        size_t       row,
                        size_t       col,
                        const void*  value);
// ================================================================================
// COO assembly helpers — delegate directly
// ================================================================================
 
/**
 * @brief Grow COO buffers to at least @p capacity entries.
 *
 * No-op if current capacity is already sufficient.  Existing entries
 * are preserved.
 *
 * Wrapper: delegate directly.
 *
 * @param mat       COO matrix to grow.  Must not be NULL.
 * @param capacity  Requested minimum entry capacity.
 *
 * @return NO_ERROR on success, or:
 *         - NULL_POINTER    — mat is NULL
 *         - ILLEGAL_STATE   — mat is not COO format
 *         - INVALID_ARG     — capacity < current nnz
 *         - LENGTH_OVERFLOW — capacity overflows size_t arithmetic
 *         - OUT_OF_MEMORY   — buffer reallocation failed
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_coo_matrix(4, 4, 2, INT32_TYPE, true, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t v1 = 10;
 * int32_t v2 = 20;
 *
 * push_back_coo_matrix(mat, 0, 0, &v1);
 * push_back_coo_matrix(mat, 3, 1, &v2);
 *
 * // Increase entry capacity while preserving existing values
 * reserve_coo_matrix(mat, 8);
 *
 * return_matrix(mat);
 * @endcode
 */
error_code_t reserve_coo_matrix(matrix_t* mat, size_t capacity);
 
// -------------------------------------------------------------------------------- 
 
/**
 * @brief Insert or overwrite a COO entry at (row, col).
 *
 * If an entry at (row, col) already exists, its value is overwritten
 * without increasing nnz.  Otherwise a new entry is appended and the
 * matrix is marked unsorted.
 *
 * Wrapper: take @c T value by value, pass @c &value.
 *
 * @param mat    COO matrix to modify.  Must not be NULL.
 * @param row    Zero-based row index.  Must be < mat->rows.
 * @param col    Zero-based column index.  Must be < mat->cols.
 * @param value  Pointer to the value to insert.  Must not be NULL.
 *
 * @return NO_ERROR on success, or:
 *         - NULL_POINTER      — mat or value is NULL
 *         - ILLEGAL_STATE     — mat is not COO format
 *         - INVALID_ARG       — (row, col) out of bounds
 *         - CAPACITY_OVERFLOW — COO is full and growth is false
 *         - LENGTH_OVERFLOW   — growth would overflow size_t
 *         - OUT_OF_MEMORY     — growth allocation failed
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_coo_matrix(4, 4, 4, INT32_TYPE, true, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t value = 15;
 * push_back_coo_matrix(mat, 2, 3, &value);
 *
 * int32_t out = 0;
 * get_matrix(mat, 2, 3, &out);   // out == 15
 *
 * return_matrix(mat);
 * @endcode
 */
error_code_t push_back_coo_matrix(matrix_t*    mat,
                                  size_t       row,
                                  size_t       col,
                                  const void*  value);
 
// -------------------------------------------------------------------------------- 
 
/**
 * @brief Sort COO entries into row-major (row, col) order.
 *
 * Uses quicksort with median-of-three pivot and insertion-sort fallback
 * for small partitions.  After sorting, COO lookups use binary search.
 * No-op if nnz < 2.
 *
 * Wrapper: delegate directly.
 *
 * @param mat  COO matrix to sort.  Must not be NULL.
 *
 * @return NO_ERROR on success, or:
 *         - NULL_POINTER  — mat is NULL
 *         - ILLEGAL_STATE — mat is not COO format
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_coo_matrix(5, 5, 8, INT32_TYPE, true, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t v1 = 10;
 * int32_t v2 = 20;
 * int32_t v3 = 30;
 *
 * push_back_coo_matrix(mat, 4, 2, &v1);
 * push_back_coo_matrix(mat, 1, 4, &v2);
 * push_back_coo_matrix(mat, 1, 1, &v3);
 *
 * // Reorder entries into row-major (row, col) order
 * sort_coo_matrix(mat);
 *
 * return_matrix(mat);
 * @endcode
 */
error_code_t sort_coo_matrix(matrix_t* mat);
 
// ================================================================================
// Lifecycle / structural operations
// ================================================================================
 
/**
 * @brief Clear a matrix while preserving its allocated storage.
 *
 * Dense: all bytes memset to zero.  COO: nnz reset to 0, sorted to true.
 * CSR/CSC: not supported.
 *
 * Wrapper: delegate directly.
 *
 * @param mat  Matrix to clear.  Must not be NULL.
 *
 * @return NO_ERROR on success, or:
 *         - NULL_POINTER  — mat is NULL
 *         - ILLEGAL_STATE — format is CSR or CSC
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(2, 2, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t value = 9;
 * set_matrix(mat, 0, 0, &value);
 * set_matrix(mat, 1, 1, &value);
 *
 * clear_matrix(mat);
 *
 * int32_t out = -1;
 * get_matrix(mat, 0, 0, &out);   // out == 0
 *
 * return_matrix(mat);
 * @endcode
 */
error_code_t clear_matrix(matrix_t* mat);
 
// -------------------------------------------------------------------------------- 
 
/**
 * @brief Deep-copy a matrix preserving its format.
 *
 * All four formats are supported.  The copy is fully independent of the
 * source.
 *
 * Wrapper: wrap the returned matrix_expect_t.
 *
 * @param src      Matrix to copy.  Must not be NULL.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return matrix_expect_t with has_value true on success, or u.error:
 *         - NULL_POINTER  — src is NULL
 *         - BAD_ALLOC     — matrix_t struct allocation failed
 *         - OUT_OF_MEMORY — buffer allocation failed
 *         - ILLEGAL_STATE — unrecognized format
 *         (Dense/COO copies may also propagate init errors)
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(2, 2, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* src = r.u.value;
 *
 * int32_t value = 42;
 * set_matrix(src, 1, 0, &value);
 *
 * matrix_expect_t cr = copy_matrix(src, alloc);
 * if (!cr.has_value) {
 *     return_matrix(src);
 *     // handle error
 * }
 *
 * matrix_t* copy = cr.u.value;
 *
 * int32_t out = 0;
 * get_matrix(copy, 1, 0, &out);   // out == 42
 *
 * return_matrix(src);
 * return_matrix(copy);
 * @endcode
 */
matrix_expect_t copy_matrix(const matrix_t*   src,
                            allocator_vtable_t alloc_v);
 
// -------------------------------------------------------------------------------- 
 
/**
 * @brief Convert to a different storage format.
 *
 * All format pairs are supported.  If @p target equals the source format,
 * this behaves like copy_matrix.
 *
 * Wrapper: intercept specialized fast paths if desired, otherwise delegate.
 *
 * @param src      Matrix to convert.  Must not be NULL.
 * @param target   Desired destination format.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return matrix_expect_t with has_value true on success, or u.error:
 *         - NULL_POINTER  — src is NULL
 *         - BAD_ALLOC     — struct allocation failed
 *         - OUT_OF_MEMORY — buffer allocation failed
 *         - ILLEGAL_STATE — unrecognized source or target format
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(3, 3, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* dense = r.u.value;
 *
 * int32_t v1 = 10;
 * int32_t v2 = 20;
 *
 * set_matrix(dense, 0, 0, &v1);
 * set_matrix(dense, 2, 1, &v2);
 *
 * matrix_expect_t csr_r = convert_matrix(dense, CSR_MATRIX, alloc);
 * if (!csr_r.has_value) {
 *     return_matrix(dense);
 *     // handle error
 * }
 *
 * matrix_t* csr = csr_r.u.value;
 *
 * // csr now stores the same logical matrix in CSR format
 *
 * return_matrix(dense);
 * return_matrix(csr);
 * @endcode
 */
matrix_expect_t convert_matrix(const matrix_t*   src,
                               matrix_format_t   target,
                               allocator_vtable_t alloc_v);


// -------------------------------------------------------------------------------- 

/**
 * @brief Convert to a different storage format using an optional zero-test.
 *
 * This function performs the same structural conversion as
 * :c:func:`convert_matrix`, but allows the caller to define what values
 * should be treated as *zero* when converting from a dense matrix to a
 * sparse representation.
 *
 * When @p src is a dense matrix and the target format is sparse
 * (COO, CSR, or CSC), the @p is_zero callback is used to determine
 * whether each element should be omitted from the sparse structure.
 *
 * If @p is_zero is NULL, the function falls back to the default
 * bytewise zero detection used by :c:func:`convert_matrix`.
 *
 * For all other conversions (sparse → dense or sparse → sparse),
 * the @p is_zero callback is ignored and the behavior is identical
 * to :c:func:`convert_matrix`.
 *
 * @param src      Matrix to convert. Must not be NULL.
 * @param target   Desired destination format.
 * @param alloc_v  Allocator for the destination matrix.
 * @param is_zero  Optional semantic zero-test callback used only for
 *                 dense → sparse conversions.
 *
 * @return matrix_expect_t with has_value true on success, or u.error on failure.
 *
 * @note
 * This function is primarily intended for dense → sparse conversions where
 * the default bytewise definition of zero is insufficient.
 *
 * @warning
 * Passing a non-dense matrix as @p src is valid, but the @p is_zero callback
 * will have no effect in that case.
 *
 * @code{.c}
 * static bool zero_or_neg_one(const void* v) {
 *     const int32_t* x = (const int32_t*)v;
 *     return (*x == 0 || *x == -1);
 * }
 *
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(2, 3, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* dense = r.u.value;
 *
 * int32_t a = 5;
 * int32_t b = -1;  // treated as zero
 * int32_t c = 8;
 *
 * set_matrix(dense, 0, 0, &a);
 * set_matrix(dense, 0, 1, &b);
 * set_matrix(dense, 1, 2, &c);
 *
 * matrix_expect_t csr_r =
 *     convert_matrix_zero(dense, CSR_MATRIX, alloc, zero_or_neg_one);
 *
 * if (csr_r.has_value) {
 *     matrix_t* csr = csr_r.u.value;
 *
 *     // Only 5 and 8 are stored in the sparse structure
 *
 *     return_matrix(csr);
 * }
 *
 * return_matrix(dense);
 * @endcode
 */
matrix_expect_t convert_matrix_zero(const matrix_t*    src,
                                    matrix_format_t    target,
                                    allocator_vtable_t alloc_v,
                                    matrix_zero_fn     is_zero);
 
// -------------------------------------------------------------------------------- 
 
/**
 * @brief Transpose a matrix.
 *
 * Dense→Dense, COO→COO, CSR→CSR, and CSC→CSC are supported.
 *
 * Wrapper: intercept specialized fast paths if desired, otherwise delegate.
 *
 * @param src      Matrix to transpose.  Must not be NULL.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return matrix_expect_t with has_value true on success, or u.error:
 *         - NULL_POINTER  — src is NULL
 *         - ILLEGAL_STATE — unrecognized format
 *         - BAD_ALLOC     — struct allocation failed
 *         - OUT_OF_MEMORY — buffer allocation failed
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(2, 3, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* src = r.u.value;
 *
 * int32_t v1 = 10;
 * int32_t v2 = 20;
 *
 * set_matrix(src, 0, 1, &v1);
 * set_matrix(src, 1, 2, &v2);
 *
 * matrix_expect_t tr_r = transpose_matrix(src, alloc);
 * if (!tr_r.has_value) {
 *     return_matrix(src);
 *     // handle error
 * }
 *
 * matrix_t* tr = tr_r.u.value;
 *
 * int32_t out = 0;
 * get_matrix(tr, 1, 0, &out);   // out == 10
 *
 * return_matrix(src);
 * return_matrix(tr);
 * @endcode
 */
matrix_expect_t transpose_matrix(const matrix_t*   src,
                                 allocator_vtable_t alloc_v);
 
// ================================================================================
// Shape and compatibility queries — delegate directly, trivial cost
// ================================================================================
 
/**
 * @brief Test whether two matrices have identical shape.
 * @param a  First matrix.
 * @param b  Second matrix.
 * @return true if both non-NULL with matching rows and cols; false otherwise.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r1 = init_dense_matrix(3, 4, INT32_TYPE, alloc);
 * matrix_expect_t r2 = init_coo_matrix(3, 4, 8, INT32_TYPE, true, alloc);
 *
 * if (!r1.has_value || !r2.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* a = r1.u.value;
 * matrix_t* b = r2.u.value;
 *
 * bool same = matrix_has_same_shape(a, b);   // same == true
 *
 * return_matrix(a);
 * return_matrix(b);
 * @endcode
 */
bool matrix_has_same_shape(const matrix_t* a,
                           const matrix_t* b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a matrix is square.
 * @param mat  Matrix to inspect.
 * @return true if non-NULL and rows == cols; false otherwise.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(4, 4, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * bool square = matrix_is_square(mat);   // square == true
 *
 * return_matrix(mat);
 * @endcode
 */
bool matrix_is_square(const matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a matrix uses a sparse representation.
 * @param mat  Matrix to inspect.
 * @return true for COO, CSR, CSC; false for Dense or NULL.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_coo_matrix(4, 4, 8, INT32_TYPE, true, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * bool sparse = matrix_is_sparse(mat);   // sparse == true
 *
 * return_matrix(mat);
 * @endcode
 */
bool matrix_is_sparse(const matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the storage footprint of the active matrix payload.
 *
 * Dense: rows × cols × data_size.
 * COO:   cap × (2 × sizeof(size_t) + data_size).
 * CSR:   (rows+1) × sizeof(size_t) + nnz × (sizeof(size_t) + data_size).
 * CSC:   (cols+1) × sizeof(size_t) + nnz × (sizeof(size_t) + data_size).
 *
 * @param mat  Matrix to inspect.
 * @return Storage bytes, or 0 if @p mat is NULL or format is unrecognized.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(3, 4, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * size_t bytes = matrix_storage_bytes(mat);
 * // bytes == 3 * 4 * sizeof(int32_t)
 *
 * return_matrix(mat);
 * @endcode
 */
size_t matrix_storage_bytes(const matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a human-readable name for a format enum.
 * @param format  Format enumeration value.
 * @return Constant string (e.g. "DENSE_MATRIX"), or
 *         "UNKNOWN_MATRIX_FORMAT" for unrecognized values.
 *
 * @code{.c}
 * const char* name1 = matrix_format_name(DENSE_MATRIX);  // "DENSE_MATRIX"
 * const char* name2 = matrix_format_name(CSR_MATRIX);    // "CSR_MATRIX"
 * @endcode
 */
const char* matrix_format_name(matrix_format_t format);
 
// ================================================================================
// Comparison and content operations
// ================================================================================
 
/**
 * @brief Bitwise element-by-element equality.
 *
 * Requires same shape and dtype.  Works across different formats by
 * reading each element through get_matrix.  Returns false if either
 * matrix is NULL, shapes differ, or dtypes differ.
 *
 * Wrapper: use type-specific comparators when semantic equality is needed.
 *
 * @param a  First matrix.  May be NULL (returns false).
 * @param b  Second matrix.  May be NULL (returns false).
 *
 * @return true if logically equal under the default bytewise comparison;
 *         false otherwise.
 *
 * @note
 * This function uses the default bytewise interpretation of values.
 * It is intended for primitive numeric types. For user-defined types
 * or custom semantics, use the corresponding comparator-aware API.*
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r1 = init_dense_matrix(2, 2, INT32_TYPE, alloc);
 * matrix_expect_t r2 = init_dense_matrix(2, 2, INT32_TYPE, alloc);
 *
 * if (!r1.has_value || !r2.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* a = r1.u.value;
 * matrix_t* b = r2.u.value;
 *
 * int32_t v1 = 10;
 * int32_t v2 = 20;
 *
 * set_matrix(a, 0, 0, &v1);
 * set_matrix(a, 1, 1, &v2);
 *
 * set_matrix(b, 0, 0, &v1);
 * set_matrix(b, 1, 1, &v2);
 *
 * bool equal = matrix_equal(a, b);   // equal == true
 *
 * return_matrix(a);
 * return_matrix(b);
 * @endcode
 */
bool matrix_equal(const matrix_t* a,
                  const matrix_t* b);


// -------------------------------------------------------------------------------- 

/**
 * @brief Element-by-element equality using an optional semantic comparator.
 *
 * If @p cmp is NULL, this behaves like :c:func:`matrix_equal` and falls
 * back to bytewise comparison of retrieved element buffers.  If @p cmp
 * is provided, it is used to determine whether corresponding elements
 * are logically equal.
 *
 * @param a    First matrix.  May be NULL (returns false).
 * @param b    Second matrix.  May be NULL (returns false).
 * @param cmp  Optional element comparator.
 *
 * @return true if the matrices have the same shape, dtype, and all
 *         corresponding elements compare equal; false otherwise.
 *
 * @code{.c}
 * static bool abs_equal(const void* a, const void* b) {
 *     const int32_t* x = (const int32_t*)a;
 *     const int32_t* y = (const int32_t*)b;
 *
 *     int32_t ax = (*x < 0) ? -(*x) : *x;
 *     int32_t ay = (*y < 0) ? -(*y) : *y;
 *
 *     return (ax == ay);
 * }
 *
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r1 = init_dense_matrix(2, 2, INT32_TYPE, alloc);
 * matrix_expect_t r2 = init_dense_matrix(2, 2, INT32_TYPE, alloc);
 *
 * if (!r1.has_value || !r2.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* a = r1.u.value;
 * matrix_t* b = r2.u.value;
 *
 * int32_t va = -5;
 * int32_t vb =  5;
 *
 * set_matrix(a, 0, 0, &va);
 * set_matrix(b, 0, 0, &vb);
 *
 * bool equal = matrix_equal_cmp(a, b, abs_equal);   // equal == true
 *
 * return_matrix(a);
 * return_matrix(b);
 * @endcode
 */
bool matrix_equal_cmp(const matrix_t* a,
                      const matrix_t* b,
                      matrix_equal_fn cmp);
 
// -------------------------------------------------------------------------------- 
 
/**
 * @brief Fill every element with a single value.
 *
 * If @p value represents zero under the default generic rule, the matrix
 * is cleared via :c:func:`clear_matrix`.  Nonzero fill is supported only
 * for Dense format.
 *
 * Wrapper: type-specific modules may provide faster or more semantic
 * variants when needed.
 *
 * @param mat    Matrix to fill.  Must not be NULL.
 * @param value  Pointer to the fill value.  Must not be NULL.
 *
 * @return NO_ERROR on success, or:
 *         - NULL_POINTER          — mat or value is NULL
 *         - OPERATION_UNAVAILABLE — nonzero fill on non-Dense format
 *         - ILLEGAL_STATE         — zero fill delegates to clear_matrix for
 *                                   unsupported formats
 *
 * @note
 * This function uses the default bytewise interpretation of values.
 * It is intended for primitive numeric types. For user-defined types
 * or custom semantics, use the corresponding comparator-aware API. 
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(2, 3, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t value = 7;
 * fill_matrix(mat, &value);
 *
 * int32_t out = 0;
 * get_matrix(mat, 1, 2, &out);   // out == 7
 *
 * return_matrix(mat);
 * @endcode
 */
error_code_t fill_matrix(matrix_t* mat,
                         const void* value);


// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether all matrix elements are zero. 
 *
 * Dense matrices are checked element-by-element across the full buffer.
 * Sparse matrices are considered zero when they contain no stored entries.
 *
 * Wrapper: type-specific modules may provide specialized fast paths or
 * semantic zero handling when needed.
 *
 * @param mat  Matrix to inspect.
 *
 * @return true if the matrix is logically zero, or if @p mat is NULL;
 *         false otherwise.
 *
 * @note
 * This function uses the default bytewise interpretation of values.
 * It is intended for primitive numeric types. For user-defined types
 * or custom semantics, use the corresponding comparator-aware API. 
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(2, 2, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * bool zero_before = is_zero_matrix(mat);   // zero_before == true
 *
 * int32_t value = 5;
 * set_matrix(mat, 1, 1, &value);
 *
 * bool zero_after = is_zero_matrix(mat);    // zero_after == false
 *
 * return_matrix(mat);
 * @endcode
 */
bool is_zero_matrix(const matrix_t* mat);
 
// ================================================================================
// Type and compatibility predicates — delegate directly
// ================================================================================
 
/**
 * @brief Test whether two matrices store the same runtime dtype.
 * @param a  First matrix.
 * @param b  Second matrix.
 * @return true if both non-NULL with matching dtype; false otherwise.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r1 = init_dense_matrix(2, 2, INT32_TYPE, alloc);
 * matrix_expect_t r2 = init_dense_matrix(3, 3, INT32_TYPE, alloc);
 * matrix_expect_t r3 = init_dense_matrix(2, 2, FLOAT_TYPE, alloc);
 *
 * if (!r1.has_value || !r2.has_value || !r3.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* a = r1.u.value;
 * matrix_t* b = r2.u.value;
 * matrix_t* c = r3.u.value;
 *
 * bool same_ab = matrix_has_same_dtype(a, b);  // true
 * bool same_ac = matrix_has_same_dtype(a, c);  // false
 *
 * return_matrix(a);
 * return_matrix(b);
 * return_matrix(c);
 * @endcode
 */
bool matrix_has_same_dtype(const matrix_t* a,
                           const matrix_t* b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether two matrices are compatible for elementwise addition.
 *
 * Requires equal shape (rows and cols) and equal dtype.
 *
 * @param a  First matrix.
 * @param b  Second matrix.
 * @return true if add-compatible; false otherwise (including if either is NULL).
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r1 = init_dense_matrix(2, 3, INT32_TYPE, alloc);
 * matrix_expect_t r2 = init_coo_matrix(2, 3, 6, INT32_TYPE, true, alloc);
 * matrix_expect_t r3 = init_dense_matrix(3, 2, INT32_TYPE, alloc);
 *
 * if (!r1.has_value || !r2.has_value || !r3.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* a = r1.u.value;
 * matrix_t* b = r2.u.value;
 * matrix_t* c = r3.u.value;
 *
 * bool ok_ab = matrix_is_add_compatible(a, b);  // true (same shape + dtype)
 * bool ok_ac = matrix_is_add_compatible(a, c);  // false (shape mismatch)
 *
 * return_matrix(a);
 * return_matrix(b);
 * return_matrix(c);
 * @endcode
 */
bool matrix_is_add_compatible(const matrix_t* a,
                              const matrix_t* b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether two matrices are compatible for matrix multiplication.
 *
 * Requires a->cols == b->rows and equal dtype.
 *
 * @param a  Left-hand matrix.
 * @param b  Right-hand matrix.
 * @return true if multiply-compatible; false otherwise (including if either
 *         is NULL).
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r1 = init_dense_matrix(2, 3, INT32_TYPE, alloc);
 * matrix_expect_t r2 = init_dense_matrix(3, 4, INT32_TYPE, alloc);
 * matrix_expect_t r3 = init_dense_matrix(3, 4, FLOAT_TYPE, alloc);
 *
 * if (!r1.has_value || !r2.has_value || !r3.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* a = r1.u.value;
 * matrix_t* b = r2.u.value;
 * matrix_t* c = r3.u.value;
 *
 * bool ok_ab = matrix_is_multiply_compatible(a, b);  // true
 * bool ok_ac = matrix_is_multiply_compatible(a, c);  // false (dtype mismatch)
 *
 * return_matrix(a);
 * return_matrix(b);
 * return_matrix(c);
 * @endcode
 */
bool matrix_is_multiply_compatible(const matrix_t* a,
                                   const matrix_t* b);
 
// ================================================================================
// Row / column swaps — delegate directly
// ================================================================================
 
/**
 * @brief Swap two rows in a matrix.
 *
 * Dense matrices perform an in-place row swap.  Sparse formats may
 * reorder internal structures accordingly or return ILLEGAL_STATE
 * if unsupported.
 *
 * @param mat   Matrix to modify.  Must not be NULL.
 * @param r1    First row index.
 * @param r2    Second row index.
 *
 * @return NO_ERROR on success, or:
 *         - NULL_POINTER  — mat is NULL
 *         - INVALID_ARG   — indices out of bounds
 *         - ILLEGAL_STATE — format does not support row swapping
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(3, 3, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t a = 1;
 * int32_t b = 2;
 *
 * set_matrix(mat, 0, 0, &a);   // row 0
 * set_matrix(mat, 1, 0, &b);   // row 1
 *
 * swap_matrix_rows(mat, 0, 1);
 *
 * int32_t out = 0;
 * get_matrix(mat, 0, 0, &out);   // out == 2
 * get_matrix(mat, 1, 0, &out);   // out == 1
 *
 * return_matrix(mat);
 * @endcode
 */
error_code_t swap_matrix_rows(matrix_t* mat,
                              size_t r1,
                              size_t r2);
// -------------------------------------------------------------------------------- 

/**
 * @brief Swap two columns in a matrix.
 *
 * Dense matrices perform an in-place column swap.  Sparse formats may
 * reorder internal structures accordingly or return ILLEGAL_STATE
 * if unsupported.
 *
 * @param mat   Matrix to modify.  Must not be NULL.
 * @param c1    First column index.
 * @param c2    Second column index.
 *
 * @return NO_ERROR on success, or:
 *         - NULL_POINTER  — mat is NULL
 *         - INVALID_ARG   — indices out of bounds
 *         - ILLEGAL_STATE — format does not support column swapping
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_dense_matrix(2, 3, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t a = 5;
 * int32_t b = 9;
 *
 * set_matrix(mat, 0, 0, &a);   // column 0
 * set_matrix(mat, 0, 2, &b);   // column 2
 *
 * swap_matrix_cols(mat, 0, 2);
 *
 * int32_t out = 0;
 * get_matrix(mat, 0, 0, &out);   // out == 9
 * get_matrix(mat, 0, 2, &out);   // out == 5
 *
 * return_matrix(mat);
 * @endcode
 */
error_code_t swap_matrix_cols(matrix_t* mat,
                              size_t c1,
                              size_t c2);
 
// ================================================================================
// Special constructors
// ================================================================================
 
/**
 * @brief Create an n×n dense identity matrix.
 *
 * The diagonal is set to 1 (1.0f for float, 1.0 for double, 1.0L for
 * long double, byte value 1 for integer types).  Off-diagonal elements
 * are zero.
 *
 * Wrapper: fix @p dtype.
 *
 * @param n        Matrix order (rows == cols == n).  Must be > 0.
 * @param dtype    Registered runtime data type identifier.
 * @param alloc_v  Allocator vtable.
 *
 * @return matrix_expect_t with has_value true on success, or u.error:
 *         (propagates all errors from init_dense_matrix, plus)
 *         - errors from clear_matrix or set_matrix during diagonal fill
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_identity_matrix(3, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* mat = r.u.value;
 *
 * int32_t out = 0;
 *
 * get_matrix(mat, 0, 0, &out);   // out == 1
 * get_matrix(mat, 1, 1, &out);   // out == 1
 * get_matrix(mat, 2, 2, &out);   // out == 1
 *
 * get_matrix(mat, 0, 1, &out);   // out == 0
 *
 * return_matrix(mat);
 * @endcode
 */
matrix_expect_t init_identity_matrix(size_t n,
                                     dtype_id_t dtype,
                                     allocator_vtable_t alloc_v);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a dense 1×length row vector, zero-initialized.
 *
 * Equivalent to init_dense_matrix(1, length, dtype, alloc_v).
 *
 * Wrapper: fix @p dtype.
 *
 * @param length   Number of columns (vector length).  Must be > 0.
 * @param dtype    Registered runtime data type identifier.
 * @param alloc_v  Allocator vtable.
 *
 * @return matrix_expect_t — same errors as init_dense_matrix.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_row_vector(4, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* vec = r.u.value;
 *
 * int32_t value = 7;
 * set_matrix(vec, 0, 2, &value);   // row vector, only row index is 0
 *
 * int32_t out = 0;
 * get_matrix(vec, 0, 2, &out);     // out == 7
 *
 * return_matrix(vec);
 * @endcode
 */
matrix_expect_t init_row_vector(size_t length,
                                dtype_id_t dtype,
                                allocator_vtable_t alloc_v);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a dense length×1 column vector, zero-initialized.
 *
 * Equivalent to init_dense_matrix(length, 1, dtype, alloc_v).
 *
 * Wrapper: fix @p dtype.
 *
 * @param length   Number of rows (vector length).  Must be > 0.
 * @param dtype    Registered runtime data type identifier.
 * @param alloc_v  Allocator vtable.
 *
 * @return matrix_expect_t — same errors as init_dense_matrix.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_col_vector(4, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* vec = r.u.value;
 *
 * int32_t value = 9;
 * set_matrix(vec, 2, 0, &value);   // column vector, only column index is 0
 *
 * int32_t out = 0;
 * get_matrix(vec, 2, 0, &out);     // out == 9
 *
 * return_matrix(vec);
 * @endcode
 */
matrix_expect_t init_col_vector(size_t length,
                                dtype_id_t dtype,
                                allocator_vtable_t alloc_v);
 
// ================================================================================
// Vector shape queries — delegate directly
// ================================================================================
 
/**
 * @brief Test whether a matrix has row-vector shape (1 × N).
 * @param mat  Matrix to inspect.
 * @return true if non-NULL and rows == 1; false otherwise.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_row_vector(5, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* vec = r.u.value;
 *
 * bool is_row = matrix_is_row_vector(vec);   // true
 *
 * return_matrix(vec);
 * @endcode
 */
bool matrix_is_row_vector(const matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a matrix has column-vector shape (N × 1).
 * @param mat  Matrix to inspect.
 * @return true if non-NULL and cols == 1; false otherwise.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r = init_col_vector(5, INT32_TYPE, alloc);
 * if (!r.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* vec = r.u.value;
 *
 * bool is_col = matrix_is_col_vector(vec);   // true
 *
 * return_matrix(vec);
 * @endcode
 */
bool matrix_is_col_vector(const matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Test whether a matrix is either a row or column vector.
 * @param mat  Matrix to inspect.
 * @return true if row vector or column vector; false otherwise.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r1 = init_row_vector(4, INT32_TYPE, alloc);
 * matrix_expect_t r2 = init_col_vector(4, INT32_TYPE, alloc);
 * matrix_expect_t r3 = init_dense_matrix(2, 2, INT32_TYPE, alloc);
 *
 * if (!r1.has_value || !r2.has_value || !r3.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* row = r1.u.value;
 * matrix_t* col = r2.u.value;
 * matrix_t* mat = r3.u.value;
 *
 * bool v1 = matrix_is_vector(row);   // true
 * bool v2 = matrix_is_vector(col);   // true
 * bool v3 = matrix_is_vector(mat);   // false
 *
 * return_matrix(row);
 * return_matrix(col);
 * return_matrix(mat);
 * @endcode
 */
bool matrix_is_vector(const matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the logical length of a vector-shaped matrix.
 *
 * Row vector: returns cols.  Column vector: returns rows.
 *
 * @param mat  Matrix to inspect.
 * @return Vector length, or 0 if @p mat is NULL or not vector-shaped.
 *
 * @code{.c}
 * allocator_vtable_t alloc = heap_allocator();
 *
 * matrix_expect_t r1 = init_row_vector(6, INT32_TYPE, alloc);
 * matrix_expect_t r2 = init_col_vector(6, INT32_TYPE, alloc);
 * matrix_expect_t r3 = init_dense_matrix(2, 3, INT32_TYPE, alloc);
 *
 * if (!r1.has_value || !r2.has_value || !r3.has_value) {
 *     // handle error
 * }
 *
 * matrix_t* row = r1.u.value;
 * matrix_t* col = r2.u.value;
 * matrix_t* mat = r3.u.value;
 *
 * size_t len_row = matrix_vector_length(row);   // 6
 * size_t len_col = matrix_vector_length(col);   // 6
 * size_t len_mat = matrix_vector_length(mat);   // 0 (not a vector)
 *
 * return_matrix(row);
 * return_matrix(col);
 * return_matrix(mat);
 * @endcode
 */
size_t matrix_vector_length(const matrix_t* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Find the index of the minimum element in a matrix.
 *
 * This function scans the underlying storage of a matrix and returns the index
 * of the minimum element according to a user-provided comparison function.
 * The returned index refers to the internal storage layout of the matrix and
 * must be interpreted by the caller based on the matrix format.
 *
 * The comparison function must return:
 * - a negative value if a < b
 * - zero if a == b
 * - a positive value if a > b
 *
 * @param mat   Pointer to the matrix.
 * @param cmp   Comparison function for elements of the matrix.
 * @param dtype Expected data type of the matrix (must match mat->dtype).
 *
 * @return size_expect_t
 * @retval has_value = true   The minimum index was found and is stored in u.value.
 * @retval has_value = false  An error occurred, and the error code is stored in u.error.
 *
 * @errors
 * - NULL_POINTER   if @p mat or @p cmp is NULL.
 * - TYPE_MISMATCH  if @p dtype does not match mat->dtype.
 * - EMPTY          if the matrix contains no elements:
 *                  - dense: rows * cols == 0
 *                  - sparse: nnz == 0
 * - INVALID_ARG    if the matrix format is not recognized.
 * - LENGTH_OVERFLOW if rows * cols overflows size_t (dense matrices only).
 *
 * @note
 * The returned index depends on the matrix format:
 * - DENSE_MATRIX: index is the row-major offset
 *                 (row = idx / cols, col = idx % cols).
 * - COO_MATRIX:   index refers to the position in row_idx[], col_idx[], and values[].
 * - CSR_MATRIX:   index refers to the position in col_idx[] and values[].
 * - CSC_MATRIX:   index refers to the position in row_idx[] and values[].
 *
 * @warning
 * For sparse matrices, this function operates only on stored elements (nnz).
 * Implicit zero elements are not considered in the comparison.
 *
 * @note
 * This function does not return the value itself. Type-specific wrapper
 * functions should use the returned index to retrieve the corresponding value.
 *
 * @see matrix_max
 * @see matrix_equal
 */
size_expect_t matrix_min(const matrix_t* mat,
                         int (*cm)(const void*, const void*),
                         dtype_id_t);
// -------------------------------------------------------------------------------- 

/**
 * @brief Find the index of the maximum element in a matrix.
 *
 * This function scans the underlying storage of a matrix and returns the index
 * of the maximum element according to a user-provided comparison function.
 * The returned index refers to the internal storage layout of the matrix and
 * must be interpreted by the caller based on the matrix format.
 *
 * The comparison function must return:
 * - a negative value if a < b
 * - zero if a == b
 * - a positive value if a > b
 *
 * @param mat   Pointer to the matrix.
 * @param cmp   Comparison function for elements of the matrix.
 * @param dtype Expected data type of the matrix (must match mat->dtype).
 *
 * @return size_expect_t
 * @retval has_value = true   The maximum index was found and is stored in u.value.
 * @retval has_value = false  An error occurred, and the error code is stored in u.error.
 *
 * @errors
 * - NULL_POINTER   if @p mat or @p cmp is NULL.
 * - TYPE_MISMATCH  if @p dtype does not match mat->dtype.
 * - EMPTY          if the matrix contains no elements:
 *                  - dense: rows * cols == 0
 *                  - sparse: nnz == 0
 * - INVALID_ARG    if the matrix format is not recognized.
 * - LENGTH_OVERFLOW if rows * cols overflows size_t (dense matrices only).
 *
 * @note
 * The returned index depends on the matrix format:
 * - DENSE_MATRIX: index is the row-major offset
 *                 (row = idx / cols, col = idx % cols).
 * - COO_MATRIX:   index refers to the position in row_idx[], col_idx[], and values[].
 * - CSR_MATRIX:   index refers to the position in col_idx[] and values[].
 * - CSC_MATRIX:   index refers to the position in row_idx[] and values[].
 *
 * @warning
 * For sparse matrices, this function operates only on stored elements (nnz).
 * Implicit zero elements are not considered in the comparison.
 *
 * @note
 * This function does not return the value itself. Type-specific wrapper
 * functions should use the returned index to retrieve the corresponding value.
 *
 * @see matrix_max
 * @see matrix_equal
 */
size_expect_t matrix_max(const matrix_t* mat,
                         int (*cmp)(const void*, const void*),
                         dtype_id_t dtype);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_matrix_H */
// ================================================================================
// ================================================================================
// eof
