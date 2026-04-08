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

// TODO: fill_float_matrix SIMD 
// TODO: transpose_float_matrix add SIMD
// TODO: float_matrix_equal add SIMD instead of VLA
// TODO: convert_float_matrix dense to CSR (see if SIMD is better)
// TODO: is_float_matrix_zero  add SIMD (We left off here)
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
 *       @code
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
 *         - LENGTH_OVERFLOW  — rows * cols * data_size overflows size_t
 *         - BAD_ALLOC       — matrix_t struct allocation failed
 *         - OUT_OF_MEMORY   — data buffer allocation failed
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
 *         - LENGTH_OVERFLOW  — capacity * sizeof(size_t) or capacity *
 *                             data_size overflows size_t
 *         - BAD_ALLOC       — matrix_t struct allocation failed
 *         - OUT_OF_MEMORY   — row_idx, col_idx, or values buffer
 *                             allocation failed
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
 */
void return_matrix(matrix_t* mat);
 
// ================================================================================
// Introspection — delegate directly, no performance concern
// ================================================================================
 
/**
 * @brief Return the number of rows.
 * @param mat  Matrix to inspect.
 * @return Row count, or 0 if @p mat is NULL.
 */
size_t matrix_rows(const matrix_t* mat);
 
/**
 * @brief Return the number of columns.
 * @param mat  Matrix to inspect.
 * @return Column count, or 0 if @p mat is NULL.
 */
size_t matrix_cols(const matrix_t* mat);
 
/**
 * @brief Return the element size in bytes.
 * @param mat  Matrix to inspect.
 * @return Element size, or 0 if @p mat is NULL.
 */
size_t matrix_data_size(const matrix_t* mat);
 
/**
 * @brief Return the runtime dtype identifier.
 * @param mat  Matrix to inspect.
 * @return dtype, or UNKNOWN_TYPE if @p mat is NULL.
 */
dtype_id_t matrix_dtype(const matrix_t* mat);
 
/**
 * @brief Return the active storage format.
 * @param mat  Matrix to inspect.
 * @return Format enum, or DENSE_MATRIX if @p mat is NULL.
 */
matrix_format_t matrix_format(const matrix_t* mat);
 
/**
 * @brief Return the entry count.
 *
 * Dense: rows × cols.  COO/CSR/CSC: stored nonzero count.
 *
 * @param mat  Matrix to inspect.
 * @return Entry count, or 0 if @p mat is NULL.
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
 */
matrix_expect_t copy_matrix(const matrix_t*   src,
                            allocator_vtable_t alloc_v);
 
// -------------------------------------------------------------------------------- 
 
/**
 * @brief Convert to a different storage format.
 *
 * All 12 format pairs (Dense↔COO↔CSR↔CSC) are supported.  If @p target
 * equals the source format, this behaves like copy_matrix.
 *
 * [SIMD CANDIDATE — dense → CSR]
 * The dense-to-CSR path scans every element twice: once to count nonzeros
 * (the bottleneck — calls _value_is_zero per element), once to scatter.
 * A type wrapper can replace both passes with SIMD:
 *   Pass 1: vectorized compare-against-zero + popcount
 *   Pass 2: compress-store (AVX-512 vcompressps / SVE svcompact)
 *           or scalar scatter on ISAs without native compress
 *
 * Wrapper: intercept the DENSE→CSR case and build the CSR arrays
 *          directly using SIMD helpers.  Delegate all other pairs.
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
 *         (may also propagate init and push_back errors for
 *          intermediate format conversions)
 */
matrix_expect_t convert_matrix(const matrix_t*   src,
                               matrix_format_t   target,
                               allocator_vtable_t alloc_v);
 
// -------------------------------------------------------------------------------- 
 
/**
 * @brief Transpose a matrix.
 *
 * Dense→Dense (rows/cols swapped), COO→COO (indices swapped + re-sorted),
 * CSR→CSR (via histogram + scatter), CSC→CSC (via histogram + scatter).
 *
 * [SIMD CANDIDATE — dense]
 * The generic dense transpose copies elements one-by-one through void*
 * with per-element offset computation.  A type wrapper can instead cast
 * to T* and use tiled in-register transpose kernels:
 *   SSE/NEON:   4×4 blocks (unpacklo/hi + movelh/hl, or vtrnq)
 *   AVX/AVX2:   8×8 blocks (unpack + shuffle + permute2f128)
 *   AVX-512:    16×16 blocks (unpack + shuffle + shuffle_f32x4)
 * with scalar remainder for non-multiple dimensions.
 *
 * Wrapper: intercept DENSE_MATRIX, allocate the transposed matrix,
 *          call the SIMD kernel on the flat T* buffers, and delegate
 *          all sparse formats.
 *
 * @param src      Matrix to transpose.  Must not be NULL.
 * @param alloc_v  Allocator for the destination matrix.
 *
 * @return matrix_expect_t with has_value true on success, or u.error:
 *         - NULL_POINTER  — src is NULL
 *         - ILLEGAL_STATE — unrecognized format
 *         - BAD_ALLOC     — struct allocation failed
 *         - OUT_OF_MEMORY — buffer allocation failed
 *         (COO transpose may propagate sort errors)
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
 */
bool matrix_has_same_shape(const matrix_t* a,
                           const matrix_t* b);
 
/**
 * @brief Test whether a matrix is square.
 * @param mat  Matrix to inspect.
 * @return true if non-NULL and rows == cols; false otherwise.
 */
bool matrix_is_square(const matrix_t* mat);
 
/**
 * @brief Test whether a matrix uses a sparse representation.
 * @param mat  Matrix to inspect.
 * @return true for COO, CSR, CSC; false for Dense or NULL.
 */
bool matrix_is_sparse(const matrix_t* mat);
 
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
 */
size_t matrix_storage_bytes(const matrix_t* mat);
 
/**
 * @brief Return a human-readable name for a format enum.
 * @param format  Format enumeration value.
 * @return Constant string (e.g. "DENSE_MATRIX"), or
 *         "UNKNOWN_MATRIX_FORMAT" for unrecognized values.
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
 * [SIMD CANDIDATE — both dense]
 * When both matrices are dense, this can be replaced with a linear scan
 * over the flat buffers using vectorized XOR + testz/movemask with early
 * exit on the first mismatch.  The generic path calls get_matrix twice
 * per element (virtual dispatch + VLA + memcmp) — roughly 20–50× slower.
 *
 * Wrapper: check if both are DENSE_MATRIX, cast to T*, call SIMD compare.
 *          Fall back to matrix_equal for mixed-format pairs.
 *
 * @param a  First matrix.  May be NULL (returns false).
 * @param b  Second matrix.  May be NULL (returns false).
 *
 * @return true if logically equal; false otherwise.
 */
bool matrix_equal(const matrix_t* a,
                  const matrix_t* b);
 
// -------------------------------------------------------------------------------- 
 
/**
 * @brief Fill every element with a single value.
 *
 * If @p value represents zero (all bytes zero, or ±0.0 for floats), the
 * matrix is cleared via clear_matrix which supports all formats.  Nonzero
 * fill is Dense-only.
 *
 * [SIMD CANDIDATE — dense, nonzero fill]
 * The generic version loops with per-element memcpy of data_size bytes.
 * A type wrapper can broadcast the value into a SIMD register and do
 * aligned stores (4/8/16 elements per instruction).  This is the simplest
 * and highest-payoff SIMD optimization.
 *
 * Wrapper: check for zero (delegate to clear_matrix), check for
 *          DENSE_MATRIX, cast to T*, call SIMD fill.  Delegate non-dense.
 *
 * @param mat    Matrix to fill.  Must not be NULL.
 * @param value  Pointer to the fill value.  Must not be NULL.
 *
 * @return NO_ERROR on success, or:
 *         - NULL_POINTER           — mat or value is NULL
 *         - OPERATION_UNAVAILABLE  — nonzero fill on non-Dense format
 *         (zero fill delegates to clear_matrix, which returns
 *          ILLEGAL_STATE for CSR/CSC)
 */
error_code_t fill_matrix(matrix_t* mat,
                         const void* value);
 
// -------------------------------------------------------------------------------- 
 
/**
 * @brief Test whether every element is logically zero.
 *
 * Dense: checks every stored element via byte scan with float ±0
 * special-casing.  COO/CSR/CSC: checks nnz == 0.
 *
 * [SIMD CANDIDATE — dense]
 * The generic dense path calls _value_is_zero per element (byte loop +
 * float special case).  A type wrapper can compare the flat T* buffer
 * against zero using SIMD with early exit on the first nonzero lane.
 * Use IEEE float comparison (not bitwise) so that -0.0f is treated as
 * zero, matching the generic behavior.
 *
 * Wrapper: check for DENSE_MATRIX, cast to T*, call SIMD is_all_zero.
 *          Delegate sparse formats (already just nnz == 0).
 *
 * @param mat  Matrix to inspect.  NULL returns true.
 *
 * @return true if the matrix is logically all zeros; false otherwise.
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
 */
bool matrix_has_same_dtype(const matrix_t* a,
                           const matrix_t* b);
 
/**
 * @brief Test whether two matrices are compatible for elementwise addition.
 *
 * Requires equal shape (rows and cols) and equal dtype.
 *
 * @param a  First matrix.
 * @param b  Second matrix.
 * @return true if add-compatible; false otherwise (including if either is NULL).
 */
bool matrix_is_add_compatible(const matrix_t* a,
                              const matrix_t* b);
 
/**
 * @brief Test whether two matrices are compatible for matrix multiplication.
 *
 * Requires a->cols == b->rows and equal dtype.
 *
 * @param a  Left-hand matrix.
 * @param b  Right-hand matrix.
 * @return true if multiply-compatible; false otherwise (including if either
 *         is NULL).
 */
bool matrix_is_multiply_compatible(const matrix_t* a,
                                   const matrix_t* b);
 
// ================================================================================
// Row / column swaps — delegate directly
// ================================================================================
 
/**
 * @brief Swap two rows of a matrix in place.
 *
 * Dense: allocates a temporary buffer for one row, performs three memcpy
 * operations.  COO: relabels row indices and re-sorts.  CSR/CSC: not
 * supported.
 *
 * Wrapper: delegate directly.
 *
 * @param mat  Matrix to modify.  Must not be NULL.
 * @param r1   First zero-based row index.
 * @param r2   Second zero-based row index.
 *
 * @return NO_ERROR on success (including r1 == r2, which is a no-op), or:
 *         - NULL_POINTER           — mat is NULL
 *         - OUT_OF_BOUNDS          — r1 or r2 >= mat->rows
 *         - OUT_OF_MEMORY          — temporary buffer allocation failed
 *                                    (Dense only)
 *         - OPERATION_UNAVAILABLE  — format is CSR or CSC
 *         - ILLEGAL_STATE          — unrecognized format
 */
error_code_t swap_matrix_rows(matrix_t* mat,
                              size_t r1,
                              size_t r2);
 
/**
 * @brief Swap two columns of a matrix in place.
 *
 * Dense: element-by-element byte swap across all rows.  COO: relabels
 * column indices and re-sorts.  CSR/CSC: not supported.
 *
 * Wrapper: delegate directly.
 *
 * @param mat  Matrix to modify.  Must not be NULL.
 * @param c1   First zero-based column index.
 * @param c2   Second zero-based column index.
 *
 * @return NO_ERROR on success (including c1 == c2, which is a no-op), or:
 *         - NULL_POINTER           — mat is NULL
 *         - OUT_OF_BOUNDS          — c1 or c2 >= mat->cols
 *         - OPERATION_UNAVAILABLE  — format is CSR or CSC
 *         - ILLEGAL_STATE          — unrecognized format
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
 */
matrix_expect_t init_identity_matrix(size_t n,
                                     dtype_id_t dtype,
                                     allocator_vtable_t alloc_v);
 
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
 */
matrix_expect_t init_row_vector(size_t length,
                                dtype_id_t dtype,
                                allocator_vtable_t alloc_v);
 
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
 */
bool matrix_is_row_vector(const matrix_t* mat);
 
/**
 * @brief Test whether a matrix has column-vector shape (N × 1).
 * @param mat  Matrix to inspect.
 * @return true if non-NULL and cols == 1; false otherwise.
 */
bool matrix_is_col_vector(const matrix_t* mat);
 
/**
 * @brief Test whether a matrix is either a row or column vector.
 * @param mat  Matrix to inspect.
 * @return true if row vector or column vector; false otherwise.
 */
bool matrix_is_vector(const matrix_t* mat);
 
/**
 * @brief Return the logical length of a vector-shaped matrix.
 *
 * Row vector: returns cols.  Column vector: returns rows.
 *
 * @param mat  Matrix to inspect.
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
