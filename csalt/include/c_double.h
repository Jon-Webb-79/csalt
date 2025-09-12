// ================================================================================
// ================================================================================
// - File:    c_double.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 04, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_double_H
#define c_double_H
// ================================================================================ 
// ================================================================================ 

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "c_string.h"
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

#ifndef ITER_DIR_H
#define ITER_DIR_H
    /**
     * @brief An enum containing keywords for an iterator 
     *
     * This enum contains keywords that are used to describe the order of iteration
     * within an iterator 
     *
     * @attribute FORWARD Keyword to command a forward iteration 
     * @attribute REVERSE Keyword to command a reverese iteration
     */
    typedef enum {
        FORWARD,
        REVERSE
    }iter_dir;
#endif /* ITER_DIR_H*/
// --------------------------------------------------------------------------------    

#ifndef ALLOC_H 
#define ALLOC_H 

    /**
     * @enum alloc_t 
     * @brief An enum to discern if an array is statically or allocated 
     */
    typedef enum {
        STATIC,
        DYNAMIC
    } alloc_t;

#endif /*ALLOC_H*/

/**
* @struct float_v
* @brief Dynamic array (vector) container for float objects
*
* This structure manages a resizable array of double objects with automatic
* memory management and capacity handling.
*/
typedef struct {
    double* data;
    size_t len;
    size_t alloc;
    alloc_t alloc_type;
} double_v;
// --------------------------------------------------------------------------------

/**
* @function init_double_vector
* @brief Initializes a new dynamically allocated double vector with specified initial capacity
*
* @param buffer Initial capacity to allocate
* @return Pointer to new double_v object, or NULL on allocation failure
*         Sets errno to ENOMEM if memory allocation fails
*/
double_v* init_double_vector(size_t buffer);
// -------------------------------------------------------------------------------- 

/**
 * @function init_double_static_array
 * @brief Creates a stack-based double vector with static array
 *
 * @param size Size of the array
 */
#define init_double_array(size) \
    ((double_v){.data = (double[size]){0}, .len = 0, .alloc = size, .alloc_type = STATIC})
// --------------------------------------------------------------------------------

/**
 * @brief returns a c style pointer to the beginning of an array 
 *
 * @param vec A pointer to a double_v data type 
 * @return A pointer to the beginning of a double array
 */
double* c_double_ptr(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function push_back_double_vector
* @brief Adds a double value to the end of the vector
*
* Automatically resizes the vector if necessary.
*
* @param vec Target double vector
* @param value Double value to add
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure
*/
bool push_back_double_vector(double_v* vec, const double value);
// --------------------------------------------------------------------------------

/**
* @function push_front_double_vector
* @brief Adds a double value to the beginning of the vector
*
* Shifts existing elements right and automatically resizes if necessary.
*
* @param vec Target double vector
* @param value Double value to add
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or ENOMEM on allocation failure
*/
bool push_front_double_vector(double_v* vec, const double value);
// --------------------------------------------------------------------------------

/**
* @function insert_double_vector
* @brief Inserts a double value at specified index in the vector
*
* Shifts elements right starting at index and resizes if necessary.
*
* @param vec Target double vector
* @param value Double value to insert
* @param index Position to insert at
* @return true if successful, false on error
*         Sets errno to EINVAL for NULL inputs or index out of bounds, ENOMEM on allocation failure
*/
bool insert_double_vector(double_v* vec, const double value, size_t index);
// --------------------------------------------------------------------------------

/**
* @function double_vector_index
* @brief Retrieves pointer to string_t at specified index
*
* @param vec Source double vector
* @param index Position to access
* @return Pointer to string_t object, or NULL on error
*         Sets errno to EINVAL for NULL input or ERANGE if index out of bounds
*/
double double_vector_index(const double_v* vec, size_t index);
// -------------------------------------------------------------------------------- 

/**
* @function double_vector_size
* @brief Returns current number of strings in vector
*
* @param vec Double vector to query
* @return Number of strings in vector, or LONG_MAX on error
*         Sets errno to EINVAL for NULL input
*/
size_t double_vector_size(const double_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function double_vector_alloc
* @brief Returns current allocation size of vector
*
* @param vec Double vector to query
* @return Current allocation capacity, or LONG_MAX on error
*         Sets errno to EINVAL for NULL input
*/
size_t double_vector_alloc(const double_v* vec);
// --------------------------------------------------------------------------------

/**
* @function pop_back_double_vector
* @brief Removes and returns last double value in vector
*
* @param vec Source double vector
* @return Pointer to removed double object, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
double pop_back_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function pop_front_double_vector
* @brief Removes and returns first double value in vector
*
* Shifts remaining elements left.
*
* @param vec Source string vector
* @return Pointer to removed double object, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
double pop_front_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
* @function pup_any_double_vector
* @brief Removes and returns double value at specified index
*
* Shifts remaining elements left to fill gap.
*
* @param vec Source double vector
* @param index Position to remove from
* @return Pointer to removed double_t object, or NULL on error
*         Sets errno to EINVAL for NULL input or ERANGE if index out of bounds
*/
double pop_any_double_vector(double_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
* @function free_double_vector
* @brief Frees all memory associated with string vector
*
* Frees all contained strings and the vector itself.
*
* @param vec Double vector to free
* @return void
*         Sets errno to EINVAL for NULL input
*/
void free_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
* @function _free_double_vector
* @brief Helper function for garbage collection of double vectors
*
* Used with DBLEVEC_GBC macro for automatic cleanup.
*
* @param vec Double pointer to double vector to free
* @return void
*/
void _free_double_vector(double_v** vec);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro FLTVEC_GBC
     * @brief A macro for enabling automatic cleanup of double vector objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_double_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define DBLEVEC_GBC __attribute__((cleanup(_free_double_vector)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @function reverse_double_vector
 * @brief Reverses the order of elements in a double vector in place.
 *
 * The function reverses the order of elements by swapping elements from the
 * beginning and end of the vector until the middle is reached.
 *
 * @param vec double vector to reverse
 * @return void
 *         Sets errno to EINVAL if vec is NULL or invalid
 */
void reverse_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @function swap_double
 * @brief Swaps the contents of two double objects.
 *
 * Performs an in-place swap of double values
 *
 * @param a Pointer to first double object
 * @param b Pointer to second double object
 * @return void
 *         Sets errno to EINVAL if either input is NULL
 */
void swap_double(double* a, double* b);
// --------------------------------------------------------------------------------

/**
* @function sort_double_vector
* @brief Sorts a double vector in ascending or descending order.
*
* Uses an optimized QuickSort algorithm with median-of-three pivot selection
* and insertion sort for small subarrays. Sort direction is determined by
* the iter_dir parameter.
*
* @param vec double vector to sort
* @param direction FORWARD for ascending order, REVERSE for descending
* @return void
*         Sets errno to EINVAL if vec is NULL or invalid
*/
void sort_double_vector(double_v* vec, iter_dir direction);
// -------------------------------------------------------------------------------- 

/**
* @function trim_double_vector
* @brief Trims all un-necessary memory from a vector
*
* @param vec double vector to trim
* @return void
*         Sets errno to EINVAL if vec is NULL or invalid
*/
void trim_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function binary_search_double_vector
* @brief Searches a double vector to find the index where a value exists
*
* @param vec double vector object
* @param value The value to search for
* @param tolerance The double tolerance for finding a value 
* @param sort_first true if the vector or array needs to be sorted, false otherwise
* @return The index where a value exists, LONG_MAX if the value is not in the array.
*         Sets errno to EINVAL if vec is NULL or invalid, ENODATA if the array is 
*         not populated
*/
size_t binary_search_double_vector(double_v* vec, double value, double tolerance, bool sort_first);
// -------------------------------------------------------------------------------- 

#ifndef BIN_DAT_TYPEDEF
#define BIN_DAT_TYPEDEF
/**
 * @brief Result structure for binary search bound queries (double version).
 *
 * The `bin_dat` struct encodes the indices that bound a given search value
 * within a sorted or unsorted double vector.
 *
 * Conventions:
 * - `lower`: Index of the last element strictly less than `value`,
 *            or SIZE_MAX if none exist.
 * - `upper`: Index of the first element strictly greater than `value`,
 *            or SIZE_MAX if none exist.
 *
 * When the search value matches an element within `tolerance`, both
 * `lower` and `upper` are set to that element's index.
 */
typedef struct {
    size_t lower;
    size_t upper;
} bin_dat;
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief Locate the bounding indices of a value in a double vector.
 *
 * Performs a binary search on the given double vector to determine the
 * indices of elements that bound the provided search value. If an exact
 * match (within the specified `tolerance`) is found, both bounds are set
 * to the matching index. Otherwise:
 * - `lower` is the index of the last element less than `value`,
 * - `upper` is the index of the first element greater than `value`.
 *
 * Special cases:
 * - If `value` is smaller than all elements, `lower = SIZE_MAX, upper = 0`.
 * - If `value` is larger than all elements, `lower = len-1, upper = SIZE_MAX`.
 * - On error, both fields are set to SIZE_MAX and `errno` is set.
 *
 * @param vec        Pointer to a double vector (`double_v*`) to search.
 * @param value      The target value to locate.
 * @param tolerance  Allowed absolute error when testing for equality.
 * @param sort_first If true, the vector is sorted in ascending order
 *                   before performing the search.
 *
 * @return A `bin_dat` structure with indices bounding the value.
 */
bin_dat binary_search_bounds_double_vector(double_v* vec,
                                           double value,
                                           double tolerance,
                                           bool sort_first);

// -------------------------------------------------------------------------------- 
/**
* @function update_double_vector
* @brief Replaces the value of a vector at a specific index
*
* @param vec double vector object
* @param index The index where data will be replaced
* @param replacement_value The replacement value
* @return void, Sets errno to EINVAL if vec does not exsist, or ERANGE 
*         if the index is out of bounds
*/
void update_double_vector(double_v* vec, size_t index, double replacement_value);
// -------------------------------------------------------------------------------- 

/**
 * @function min_double_vector 
 * @brief Returns the minimum value in a vector or array 
 *
 * @param vec A double vector or array object 
 * @return The minimum value in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
double min_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function max_double_vector 
 * @brief Returns the maximum value in a vector or array 
 *
 * @param vec A double vector or array object 
 * @return The maximum value in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
double max_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function sum_double_vector 
 * @brief Returns the summation of all values in a vector or array
 *
 * @param vec A double vector or array object 
 * @return The summation of all values in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
double sum_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function average_double_vector 
 * @brief Returns the average of all values in a vector or array
 *
 * @param vec A double vector or array object 
 * @return The average of all values in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
double average_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function stdev_double_vector 
 * @brief Returns the standard deviation of all values in a vector or array
 *
 * @param vec A double vector or array object 
 * @return The standard deviation of all values in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
double stdev_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function cum_sum_double_vector 
 * @brief Returns a dynamically allocated array containing the cumulative sum of all 
 *        values in vec
 *
 * @param vec A double vector or array object 
 * @return A double_v object with the cumulative sum of all values in vec.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
double_v* cum_sum_double_vector(double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief creates a deep copy of a vector
 *
 * @param original A vector to be copied 
 * @return A copy of a double vector
 */
double_v* copy_double_vector(const double_v* original);
// ================================================================================ 
// ================================================================================ 

#ifndef MATRIX_ENUM
/**
 * @enum MatrixStorageType
 * @brief Enum representing the storage format of a matrix.
 *
 * Used to determine how the matrix data is stored and which union member to access.
 */
typedef enum {
    DENSE_MATRIX,
    SPARSE_COO_MATRIX,  // Coordinate list format
    SPARSE_CSR_MATRIX   // Compressed sparse row format
} MatrixStorageType;
#endif /* MATRIX_ENUM */
// -------------------------------------------------------------------------------- 

/**
 * @struct matrix_t
 * @brief Adaptive 2D matrix supporting dense and sparse storage formats.
 *
 * This structure allows storage of a matrix using different internal representations
 * depending on size and sparsity. It supports:
 * - Dense storage using a double array and optional initialization flags.
 * - Coordinate list (COO) sparse storage with dynamic allocation.
 * - Compressed sparse row (CSR) storage for fast row-based access.
 *
 * The actual format is selected using the `type` field and stored in a union.
 */
typedef struct {
    union {
        struct { // Dense storage
            double* data;
            uint8_t* init;
        } dense;
        
        struct { // Coordinate list format
            size_t* rows;
            size_t* cols;
            double* values;
            size_t capacity;
        } coo;
        
        struct { // Compressed sparse row format
            size_t* row_ptrs;
            size_t* col_indices;
            double* values;
        } csr;
    } storage;
    
    MatrixStorageType type;
    size_t rows;
    size_t cols;
    size_t count;  // Non-zero count
} matrix_d;
// ================================================================================ 
// ================================================================================ 

/**
 * @brief Creates an dense matrix of a double data type
 *
 * @param rows Number of matrix rows.
 * @param cols Number of matrix columns.
 *
 * @returns Pointer to a dynamically allocated `matrix_d` structure,
 *          or NULL on allocation failure (errno is set to ENOMEM).
 */
matrix_d* create_double_dense_matrix(size_t rows, size_t cols);
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a sparse coordinate list matrix of a double data type
 *
 * @param rows Number of matrix rows.
 * @param cols Number of matrix columns.
 *
 * @returns Pointer to a dynamically allocated `matrix_d` structure,
 *          or NULL on allocation failure (errno is set to ENOMEM).
 */
matrix_d* create_double_coo_matrix(size_t rows, size_t cols);
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a compressed sparse row matrix of a double data type
 *
 * @param rows Number of matrix rows.
 * @param cols Number of matrix columns.
 * @param estimated_zeros Estimated number of zero values (used to infer sparsity).
 *                        For example, use `rows * cols` to force dense allocation,
 *                        or `rows` for a diagonal matrix. 
 *
 * @returns Pointer to a dynamically allocated `matrix_d` structure,
 *          or NULL on allocation failure (errno is set to ENOMEM).
 */
matrix_d* create_double_csr_matrix(size_t rows, size_t cols, size_t nnz); 
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates an adaptive matrix based on estimated sparsity of a double data type
 *
 * This function selects the most efficient internal format (dense or sparse COO)
 * depending on the matrix dimensions and the estimated number of non-zero values.
 * For very sparse or large matrices, coordinate list (COO) format is used.
 * For small or dense matrices, standard dense format is selected.
 *
 * @param rows Number of matrix rows.
 * @param cols Number of matrix columns.
 * @param estimated_zeros Estimated number of zero values (used to infer sparsity).
 *                        For example, use `rows * cols` to force dense allocation,
 *                        or `rows` for a diagonal matrix.
 *
 * @returns Pointer to a dynamically allocated `matrix_d` structure,
 *          or NULL on allocation failure (errno is set to ENOMEM).
 */
matrix_d* create_double_matrix(size_t rows, size_t cols, size_t estimated_zeros);
// -------------------------------------------------------------------------------- 

/**
 * @brief Frees all memory associated with a matrix.
 *
 * Deallocates all internal arrays associated with the matrix's current storage format
 * and then frees the matrix structure itself. Safely handles NULL pointers and avoids
 * double-free issues.
 *
 * @param mat Pointer to the matrix to be deallocated. If NULL, the function does nothing.
 */
void free_double_matrix(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * \internal
 * @brief Frees all memory associated with a matrix.
 *
 * Deallocates all internal arrays associated with the matrix's current storage format
 * and then frees the matrix structure itself. Safely handles NULL pointers and avoids
 * double-free issues.  This function is specific to dense matrices.
 * Not recommended for average users, implement free_matrix instead
 *
 * @param mat Pointer to the matrix to be deallocated. If NULL, the function does nothing.
 */
void free_double_dense_matrix(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Frees all memory associated with a matrix.
 *
 * Deallocates all internal arrays associated with the matrix's current storage format
 * and then frees the matrix structure itself. Safely handles NULL pointers and avoids
 * double-free issues.  This function is specific to sparse csr matrices.
 * Not recommended for average users, implement free_matrix instead
 *
 * @param mat Pointer to the matrix to be deallocated. If NULL, the function does nothing.
 */
void free_double_csr_matrix(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Frees all memory associated with a matrix.
 *
 * Deallocates all internal arrays associated with the matrix's current storage format
 * and then frees the matrix structure itself. Safely handles NULL pointers and avoids
 * double-free issues. This function is specific to sparse coo matrices
 * Not recommended for average users, implement free_matrix instead
 *
 * @param mat Pointer to the matrix to be deallocated. If NULL, the function does nothing.
 */
void free_double_coo_matrix(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
* @function _free_matrix
* @brief Helper function for garbage collection of double vectors
*
* Used with FLTVMAT_GBC macro for automatic cleanup.
*
* @param mat Double pointer to double matrix to free
* @return void
*/
void _free_double_matrix(matrix_d** mat);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro FLTMAT_GBC
     * @brief A macro for enabling automatic cleanup of double matrix objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_matrix`
     * when the scope ends, ensuring proper memory management.
     */
    #define DBLMAT_GBC __attribute__((cleanup(_free_double_matrix)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief Inserts a new value into a matrix at the specified row and column.
 *
 * This function acts as a format-agnostic wrapper for inserting values into a matrix.
 * It supports dense, COO, and CSR formats. If the matrix becomes large or sparse, it
 * may be automatically converted to a more optimal format (e.g., dense → COO, COO → CSR).
 *
 * @param[in,out] pmat Pointer to the matrix pointer. May be reallocated and updated 
 *                     internally if format conversion occurs.
 * @param row Zero-based row index.
 * @param col Zero-based column index.
 * @param value Value to insert.
 * @param convert_to_csr true if an insert should trigger a conversion to csr, false otherwise
 * @return true if the insertion was successful, false otherwise.
 *
 * @retval false if:
 *         - The entry already exists (errno = EEXIST).
 *         - The matrix pointer is NULL (errno = EINVAL).
 *         - Indices are out of bounds (errno = ERANGE).
 *         - Memory allocation failed (errno = ENOMEM).
 *
 * @note If the matrix format is converted during insertion, the pointer referenced by
 *       `*pmat` is updated to the new matrix object.
 */
bool insert_double_matrix(matrix_d** mat, size_t row, size_t col, double value, bool convert_to_csr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Inserts a value into a dense matrix.
 *
 * Fails if the element has already been initialized.
 *
 * @see insert_matrix for the generic wrapper.
 */
bool insert_double_dense_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Attempts to insert a value into a CSR matrix. Not supported for new entries.
 *
 * @note This function will fail with errno = EEXIST even if the element does not exist,
 *       because dynamic insertion into CSR format is not implemented.
 *       Use COO format for inserts, then convert to CSR with convert_double_coo_to_csr().
 */
bool insert_double_csr_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Inserts a value into a COO-format sparse matrix.
 *
 * Automatically grows internal storage if capacity is exceeded.
 * Fails if the (row, col) entry already exists.
 */
bool insert_double_coo_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Updates an existing value in the matrix at the specified row and column.
 *
 * This function modifies the value of an already-initialized matrix element.
 * No format conversion is performed; the matrix's structure remains unchanged.
 *
 * @param mat Pointer to the matrix.
 * @param row Zero-based row index.
 * @param col Zero-based column index.
 * @param value New value to assign to the existing entry.
 * @return true if the update was successful, false otherwise.
 *
 * @retval false if:
 *         - The matrix pointer is NULL (errno = EINVAL)
 *         - The row or column is out of bounds (errno = ERANGE)
 *         - The entry is uninitialized (errno = ENOENT or EINVAL)
 */
bool update_double_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Updates an element in a dense matrix.
 *
 * Overwrites the existing value at the given row and column.
 */
bool update_double_dense_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Updates an element in a CSR matrix.
 *
 * Replaces the value associated with the given (row, col) pair.
 * Fails if the entry is not found.
 */
bool update_double_csr_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Updates an element in a COO matrix.
 *
 * Fails if the target (row, col) entry does not exist.
 */
bool update_double_coo_matrix(matrix_d* mat, size_t row, size_t col, double value);
// -------------------------------------------------------------------------------- 

/**
 * @brief Checks whether a matrix element is initialized or present.
 *
 * This function returns true if the specified entry has been explicitly set,
 * and false if it is uninitialized (i.e., absent from sparse formats or
 * not marked as initialized in dense storage).
 *
 * @param mat Pointer to the matrix.
 * @param row Row index.
 * @param col Column index.
 * @returns true if element is initialized, false otherwise.
 * @retval false if out of bounds or invalid input (errno set to ERANGE or EINVAL).
 */
bool is_double_element_initialized(const matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Convert a matrix_d object to dense format in-place.
 *
 * This function replaces the matrix pointed to by `*pmat` with a new matrix
 * in dense format. If the original matrix is in CSR format, it will first
 * be converted to COO, then to dense. The original matrix is freed.
 *
 * @param pmat Pointer to the matrix pointer to convert.
 *
 * @note No conversion is performed if the matrix is already in dense format.
 * @note On error, errno is set to EINVAL (invalid input) or ENOMEM (allocation failure).
 */
void convert_doubleMat_to_dense(matrix_d** pmat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Convert a matrix_d object to COO (coordinate list) format in-place.
 *
 * This function replaces the matrix pointed to by `*pmat` with a new matrix
 * in COO format. If the original matrix is in CSR format, it will be converted
 * directly to COO. The original matrix is freed.
 *
 * @param pmat Pointer to the matrix pointer to convert.
 *
 * @note No conversion is performed if the matrix is already in COO format.
 * @note On error, errno is set to EINVAL (invalid input) or ENOMEM (allocation failure).
 */
void convert_doublemat_to_coo(matrix_d** pmat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Convert a matrix_d object to CSR (compressed sparse row) format in-place.
 *
 * This function replaces the matrix pointed to by `*pmat` with a new matrix
 * in CSR format. If the original matrix is dense, it is first converted to COO,
 * and then to CSR. The original matrix is freed.
 *
 * @param pmat Pointer to the matrix pointer to convert.
 *
 * @note No conversion is performed if the matrix is already in CSR format.
 * @note On error, errno is set to EINVAL (invalid input) or ENOMEM (allocation failure).
 */
void convert_doubleMat_to_csr(matrix_d** pmat);
/**
 * @breif Converts a dense double matrix to a sparse coordinate list format 
 *
 * @param dense_mat A dense double matrix 
 * @return a matrix_d data type in a COO format
 */
// matrix_d* convert_double_dense_to_coo(const matrix_d* dense_mat);
// -------------------------------------------------------------------------------- 

/**
 * @breif Converts a sparse coordinate list double matrix to a sparse compressed storage format 
 *
 * @param dense_mat A dense double matrix 
 * @return a matrix_d data type in a CSR format
 */
// matrix_d* convert_double_coo_to_csr(const matrix_d* coo_mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieves a double value from a matrix at the given row and column.
 *
 * This function accesses the value stored at (row, col) in a matrix.
 * If the element has not been initialized, or the indices are out of bounds,
 * it returns FLT_MAX and sets errno.
 *
 * @param mat Pointer to the matrix.
 * @param row Zero-based row index.
 * @param col Zero-based column index.
 * @returns The value at (row, col), or FLT_MAX on error.
 * @retval FLT_MAX if the input is invalid, out of bounds, or uninitialized.
 */
double get_double_matrix(const matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieves a double value from a dense matrix at the given row and column.
 *
 * This function accesses the value stored at (row, col) in a dense matrix.
 * If the element has not been initialized, or the indices are out of bounds,
 * it returns FLT_MAX and sets errno.
 *
 * @param mat Pointer to the dense matrix.
 * @param row Zero-based row index.
 * @param col Zero-based column index.
 * @returns The value at (row, col), or FLT_MAX on error.
 * @retval FLT_MAX if the input is invalid, out of bounds, or uninitialized.
 */
double get_double_dense_matrix(const matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieves a value from a COO-format sparse matrix.
 *
 * @param mat Pointer to the COO matrix.
 * @param row Zero-based row index.
 * @param col Zero-based column index.
 * @return The value at the given position, or FLT_MAX if not found or on error.
 */
double get_double_coo_matrix(const matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Retrieves a value from a CSR-format sparse matrix.
 *
 * @param mat Pointer to the CSR matrix.
 * @param row Zero-based row index.
 * @param col Zero-based column index.
 * @return The value at the given position, or FLT_MAX if not found or on error.
 */
double get_double_csr_matrix(const matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes and returns a value from a dense matrix.
 *
 * @param mat Pointer to the dense matrix.
 * @param row Row index.
 * @param col Column index.
 * @return The removed value, or FLT_MAX on error.
 */
double pop_double_dense_matrix(matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes and returns a value from a COO-format sparse matrix.
 *
 * @param mat Pointer to the COO matrix.
 * @param row Row index.
 * @param col Column index.
 * @return The removed value, or FLT_MAX on error.
 */
double pop_double_coo_matrix(matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes and returns a value from a CSR-format sparse matrix.
 *
 * Consider optimizing for large arrays in the future with a lazy delete process
 *
 * @param mat Pointer to the CSR matrix.
 * @param row Row index.
 * @param col Column index.
 * @return The removed value, or FLT_MAX on error.
 */
double pop_double_csr_matrix(matrix_d* mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes and returns a value from the matrix at the specified row and column.
 *
 * This function dispatches to the appropriate pop routine based on the matrix's
 * internal storage format (dense, COO, or CSR). The value is removed from the matrix,
 * and the internal count of non-zero or initialized values is updated accordingly.
 *
 * In CSR format, this function performs a lazy deletion using a tombstone marker,
 * and may automatically trigger compaction if the number of deleted entries exceeds
 * a configurable threshold.
 *
 * @param mat Pointer to the matrix from which the value should be removed.
 * @param row Zero-based row index of the element.
 * @param col Zero-based column index of the element.
 * @return The value that was removed, or FLT_MAX on error.
 *
 * @retval FLT_MAX if:
 *         - The matrix is NULL or uninitialized (errno = EINVAL)
 *         - Indices are out of bounds (errno = ERANGE)
 *         - The element does not exist (errno = ENOENT)
 *
 * @note For CSR matrices, this function uses a tombstone deletion strategy. Actual
 *       memory cleanup may be deferred until compaction is triggered.
 */
double pop_double_matrix(matrix_d** mat, size_t row, size_t col);
// -------------------------------------------------------------------------------- 

/**
 * @brief Compacts a CSR-format matrix by removing tombstone entries.
 *
 * This function rebuilds the CSR matrix by scanning for and removing any entries
 * marked as deleted (i.e., entries where `col_indices[i] == SIZE_MAX`). It rewrites
 * the internal `row_ptrs`, `col_indices`, and `values` arrays to remove gaps and
 * reduce memory waste, while preserving the row ordering of valid elements.
 *
 * This function is typically called automatically after deletions if the number
 * of tombstones exceeds a threshold, but it can also be invoked manually to force
 * compaction at any time.
 *
 * @param mat Pointer to the CSR matrix to be compacted. Must not be NULL.
 *
 * @note This function has no effect on matrices that are not in CSR format.
 * @note The `mat->count` field is updated to reflect the new number of valid entries.
 */
void compact_double_csr_matrix(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief A function to be called one a matrix has been populated to its peak value for conversion to optimum type.
 *
 * @param mat A double pointer to a matrix_d data type.
 * @param true if conversion was successful, false otherwise
 */
bool finalize_double_matrix(matrix_d** mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief returns the type of matrix 
 *
 * @param mat A matrix_d data type 
 * @return DENSE_MATRIX, SPARSE_COO_MATRIX or SPARSE_CSR_MATRIX 
 */
MatrixStorageType get_double_matrix_type(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief returns the number of allocated rws in a matrix_d structure 
 *
 * @param mat a matrix_d data type 
 * @return The number of allocated rows 
 */
size_t get_double_matrix_rows(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief returns the number of allocated columns in a matrix_d structure 
 *
 * @param mat a matrix_d data type
 * @return The number of allocated columns
 */
size_t get_double_matrix_cols(matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Returns the number of populated elements in a matrix_d structure 
 *
 * @param mat a matrix_d data type 
 * @return The number of populated elements
 */
size_t get_double_matrix_element_count(matrix_d* mat);
// -------------------------------------------------------------------------------- 

void maybe_convert_double_matrix(matrix_d** pmat, bool convert_to_csr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Inverts a square dense double matrix in-place using Gauss-Jordan elimination.
 *
 * Only applies to matrices of type DENSE_MATRIX. On success, the contents of the matrix
 * are replaced with its inverse. On failure, the matrix is left unchanged.
 *
 * @param mat Pointer to the matrix_d* structure to invert
 * @return A pointer to a matrix_d object
 *
 * @retval EINVAL  if the input is NULL, not square, or not dense
 * @retval ERANGE  if the matrix is singular and cannot be inverted
 * @retval ENOMEM  if memory allocation fails
 */
matrix_d* invert_double_dense_matrix(const matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Transposes a dense matrix in place.
 *
 * If the matrix is square, transposition is done in-place with no memory allocation.
 * If the matrix is rectangular, a new matrix is allocated, filled with transposed values,
 * and the original matrix is freed.
 *
 * @param[in,out] pmat Pointer to a matrix pointer to be transposed
 * @returns true on success, false on failure
 * @retval EINVAL if input is NULL or not a dense matrix
 * @retval ENOMEM if memory allocation fails for rectangular case
 */
bool transpose_double_dense_matrix(matrix_d** pmat);
// --------------------------------------------------------------------------------

/**
 * @brief Transposes a sparse COO matrix in-place.
 *
 * This function swaps row and column indices of all nonzero entries in a matrix
 * stored in COO (coordinate list) format. The matrix dimensions are also updated.
 *
 * Unlike CSR transpose, this operation does not require reallocation.
 *
 * @param pmat Address of the matrix pointer (must be SPARSE_COO_MATRIX)
 * @return true if successful, false on error
 * @retval EINVAL if input is NULL or not a COO matrix
 *
 * Example:
 * @code
 * matrix_d* mat = create_double_matrix(4, 5, 10);
 * // Populate mat...
 * transpose_double_coo_matrix(&mat);
 * @endcode
 */

bool transpose_double_coo_matrix(matrix_d** mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Transposes a CSR matrix in place.
 *
 * Given a matrix in CSR (compressed sparse row) format, this function replaces it
 * with its transpose. This operation reallocates internal data structures, but
 * preserves the original pointer using an in-place update.
 *
 * @param pmat Address of matrix pointer (must point to a CSR matrix)
 * @retval EINVAL if input is NULL or not in CSR format
 * @retval ENOMEM if memory allocation fails during transpose
 *
 * Example:
 * @code
 * matrix_d* mat = create_sparse_matrix(...);
 * transpose_doubleMat(&mat);
 * @endcode
 */

bool transpose_double_csr_matrix(matrix_d** pmat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Transposes a matrix in-place based on its format.
 *
 * This function checks the internal format of the matrix and calls the corresponding
 * transpose function (e.g., COO or CSR). Dense matrix transpose is currently unsupported.
 *
 * @param pmat Address of a pointer to the matrix_d to transpose
 * @return true if transposition succeeds, false otherwise
 * @retval EINVAL if input is NULL or unsupported format
 * @retval ENOMEM if memory allocation fails during CSR transpose
 * @retval ENOTSUP if transpose for dense matrices is not implemented
 *
 * Example:
 * @code
 * matrix_d* mat = create_double_matrix(10, 20, 15);
 * insert_double_matrix(&mat, 1, 2, 3.14f, false);
 * transpose_double_matrix(&mat);
 * @endcode
 */

bool transpose_double_matrix(matrix_d** pmat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a deep copy of a dense double matrix.
 *
 * Allocates a new dense matrix and copies all initialized values and
 * metadata from the source matrix. The new matrix is independent from
 * the original, and must be freed separately.
 *
 * @param mat Pointer to the dense matrix to copy.
 * @return A pointer to a newly allocated dense matrix with copied contents,
 *         or NULL on error.
 * @retval NULL if input is NULL or not a DENSE_MATRIX (sets errno to EINVAL).
 * @retval NULL if memory allocation fails (sets errno to ENOMEM).
 */
matrix_d* copy_double_dense_matrix(const matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a deep copy of a sparse COO (Coordinate List) matrix.
 *
 * Copies row indices, column indices, and values into a newly allocated COO matrix.
 * The copy preserves the same sparsity pattern and dimensions as the original.
 *
 * @param mat Pointer to the COO matrix to copy.
 * @return A pointer to a new matrix_d object with duplicated COO data,
 *         or NULL on error.
 * @retval NULL if input is NULL or not a SPARSE_COO_MATRIX (sets errno to EINVAL).
 * @retval NULL if memory allocation fails (sets errno to ENOMEM).
 */
matrix_d* copy_double_coo_matrix(const matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a deep copy of a sparse CSR (Compressed Sparse Row) matrix.
 *
 * Copies row pointers, column indices, and values into a new CSR matrix.
 * The copy maintains the same structure and numerical values.
 *
 * @param mat Pointer to the CSR matrix to copy.
 * @return A pointer to a new matrix_d object with copied CSR content,
 *         or NULL on error.
 * @retval NULL if input is NULL or not a SPARSE_CSR_MATRIX (sets errno to EINVAL).
 * @retval NULL if memory allocation fails (sets errno to ENOMEM).
 */
matrix_d* copy_double_csr_matrix(const matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a deep copy of a double matrix, regardless of storage type.
 *
 * Dispatches to the appropriate internal copy function depending on whether
 * the matrix is dense, COO, or CSR. Returns a fully independent matrix copy.
 *
 * @param mat Pointer to the matrix to copy.
 * @return A new matrix_d object with the same contents and type, or NULL on error.
 * @retval NULL if input is NULL or has an unrecognized type (sets errno to EINVAL).
 * @retval NULL if memory allocation fails during copying (sets errno to ENOMEM).
 */
matrix_d* copy_double_matrix(const matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Computes the determinant of a dense double matrix.
 *
 * This function calculates the determinant of a matrix stored in dense format
 * using Gaussian elimination with partial pivoting. The input matrix must be
 * square and of type `DENSE_MATRIX`. The original matrix is not modified.
 *
 * @param mat Pointer to the dense matrix (`matrix_f`) structure.
 * @return The determinant as a double. If an error occurs (e.g., invalid input
 *         or singular matrix), the function returns 0.0f and sets `errno`:
 *         - EINVAL if the input is NULL or not a square dense matrix
 *         - ERANGE if the matrix is singular
 */
double double_dense_matrix_det(const matrix_d* mat);
// -------------------------------------------------------------------------------- 

/**
 * @brief Computes the dot product of two contiguous double arrays.
 *
 * This function calculates the dot product of two continuous memory blocks
 * of double values. If available, it uses SIMD acceleration (AVX or SSE)
 * for performance optimization.
 *
 * :param a: Pointer to the first double array.
 * :param b: Pointer to the second double array.
 * :param len: Number of elements in each array.
 * :returns: Dot product result as a double, or FLT_MAX on error.
 * :raises: Sets errno to EINVAL for NULL input, or if len is zero.
 *
 * Example:
 * .. code-block:: c
 *
 *    double a[] = {1.0f, 2.0f, 3.0f};
 *    double b[] = {4.0f, 5.0f, 6.0f};
 *    double result = dot_double(a, b, 3);  // result = 32.0
 */
double dot_double(const double* a, const double* b, size_t len);
// -------------------------------------------------------------------------------- 

/**
 * @brief Computes the dot product of two double vectors.
 *
 * This function takes two dynamically allocated double vectors (`double_v`)
 * and returns their dot product by delegating to the `dot_double` function.
 *
 * :param vec1: Pointer to the first double vector.
 * :param vec2: Pointer to the second double vector.
 * :returns: Dot product result as a double, or FLT_MAX on error.
 * :raises: Sets errno to EINVAL if either input is NULL or uninitialized.
 *          Sets errno to ERANGE if vector lengths do not match.
 *
 * Example:
 * .. code-block:: c
 *
 *    double_v* v1 = init_double_vector(3);
 *    double_v* v2 = init_double_vector(3);
 *    push_back_double_vector(v1, 1.0f);
 *    push_back_double_vector(v1, 2.0f);
 *    push_back_double_vector(v1, 3.0f);
 *    push_back_double_vector(v2, 4.0f);
 *    push_back_double_vector(v2, 5.0f);
 *    push_back_double_vector(v2, 6.0f);
 *    double result = dot_double_vector(v1, v2);  // result = 32.0
 *    free_double_vector(v1);
 *    free_double_vector(v2);
 */
double dot_double_vector(const double_v* vec1, const double_v* vec2);
// -------------------------------------------------------------------------------- 

/**
 * @brief Computes the cross product of two 3D double vectors.
 *
 * This function takes two input arrays of length 3 and computes the cross product,
 * storing the result in the output array.
 *
 * @param a Pointer to first 3-element double array.
 * @param b Pointer to second 3-element double array.
 * @param result Pointer to 3-element double array to store the result.
 * @return true on success, false on error (sets errno).
 */
bool cross_double(const double* a, const double* b, double* result);
// -------------------------------------------------------------------------------- 

double_v* cross_double_vector(const double_v* vec1, const double_v* vec2);
// ================================================================================ 
// ================================================================================ 
// DICTIONARY PROTOTYPES 

/**
 * @typedef dict_d
 * @brief Opaque struct representing a dictionary.
 *
 * This structure encapsulates a hash table that maps string keys to double values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_d dict_d;
// --------------------------------------------------------------------------------

/**
 * @brief Initializes a new dictionary.
 *
 * Allocates and initializes a dictionary object with a default size for the hash table.
 *
 * @return A pointer to the newly created dictionary, or NULL if allocation fails.
 */
dict_d* init_double_dict();
// --------------------------------------------------------------------------------

/**
 * @brief Inserts a key-value pair into the dictionary.
 *
 * Adds a new key-value pair to the dictionary. If the key already exists, the function
 * does nothing and returns false. If the dictionary's load factor exceeds a threshold,
 * it automatically resizes.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to insert.
 * @param value The value associated with the key.
 * @return true if the key-value pair was inserted successfully, false otherwise.
 */
bool insert_double_dict(dict_d* dict, const char* key, double value);
// --------------------------------------------------------------------------------

/**
 * @brief Removes a key-value pair from the dictionary.
 *
 * Finds the specified key in the dictionary, removes the associated key-value pair,
 * and returns the value.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to remove.
 * @return The value associated with the key if it was found and removed; FLT_MAX otherwise.
 */
double pop_double_dict(dict_d* dict,  const char* key);
// --------------------------------------------------------------------------------

/**
 * @brief Retrieves the value associated with a key.
 *
 * Searches the dictionary for the specified key and returns the corresponding value.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to search for.
 * @return The value associated with the key, or FLT_MAX if the key is not found.
 */
double get_double_dict_value(const dict_d* dict, const char* key);
// --------------------------------------------------------------------------------

/**
 * @brief Frees the memory associated with the dictionary.
 *
 * Releases all memory allocated for the dictionary, including all key-value pairs.
 *
 * @param dict Pointer to the dictionary to free.
 */
void free_double_dict(dict_d* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Safely frees a dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_double_dict(dict_d** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICT_GBC
     * @brief A macro for enabling automatic cleanup of dict_d objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define DDICT_GBC __attribute__((cleanup(_free_double_dict)))
#endif
// --------------------------------------------------------------------------------

/**
 * @brief Updates the value associated with a key in the dictionary.
 *
 * Searches for the specified key in the dictionary and updates its value.
 * If the key does not exist, the function takes no action.
 *
 * @param dict Pointer to the dictionary.
 * @param key The key to update.
 * @param value The new value to associate with the key.
 */
bool update_double_dict(dict_d* dict, const char* key, double value);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the number of non-empty buckets in the dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t double_dict_size(const dict_d* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t double_dict_alloc(const dict_d* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t double_dict_hash_size(const dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Checks if a key exists in the dictionary without retrieving its value
 * 
 * @param dict Pointer to the dictionary
 * @param key Key to check for
 * @return bool true if key exists, false otherwise
 */
bool has_key_double_dict(const dict_d* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a deep copy of a dictionary
 * 
 * @param dict Pointer to the dictionary to copy
 * @return dict_d* New dictionary containing copies of all entries, NULL on error
 */
dict_d* copy_double_dict(const dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes all entries from a dictionary without freeing the dictionary itself
 * 
 * @param dict Pointer to the dictionary to clear
 * @return bool true if successful, false otherwise
 */
bool clear_double_dict(dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all keys in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return char** Array of strings containing copies of all keys, NULL on error
 */
string_v* get_keys_double_dict(const dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all values in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return double* Array containing all values, NULL on error
 */
double_v* get_values_double_dict(const dict_d* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Merges two dictionaries into a new dictionary
 * 
 * @param dict1 First dictionary
 * @param dict2 Second dictionary
 * @param overwrite If true, values from dict2 override dict1 on key conflicts
 * @return dict_d* New dictionary containing merged entries, NULL on error
 */
dict_d* merge_double_dict(const dict_d* dict1, const dict_d* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for dictionary traversal
 */
typedef void (*ddict_iterator)(const char* key, double value, void* user_data);

/**
 * @brief Iterates over all dictionary entries in insertion order
 * 
 * @param dict Pointer to the dictionary
 * @param iter Iterator function to call for each entry
 * @param user_data Optional user data passed to iterator function
 */
bool foreach_double_dict(const dict_d* dict, ddict_iterator iter, void* user_data);
// ================================================================================ 
// ================================================================================ 
// VECTOR DICTIONARY PROTOTYPES 

/**
 * @typedef dict_fv
 * @brief Opaque struct representing a dictionary for vector data types.
 *
 * This structure encapsulates a hash table that maps string keys to a vector of double values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_dv dict_dv;
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a dictionary of double vectors
 *
 * @returns a dictionary of double vectors
 */
dict_dv* init_doublev_dict(void);
// -------------------------------------------------------------------------------- 

/**
* @function create_doublev_dict
* @brief Creates a key vector/array pair 
*
* @param dict A fict_fv data type
* @param key a string literal key
* @param size The size of the array or vector 
* @return true if the function executes succesfully, false otherwise
*/
bool create_doublev_dict(dict_dv* dict, char* key, size_t size);
// -------------------------------------------------------------------------------- 

/**
* @function pop_doublev_dict 
* @brief Removes a statically or dynamically allocated array from the dictionary
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return true if the function executes succesfully, false otherwise
*/
bool pop_doublev_dict(dict_dv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function pop_doublev_dict 
* @brief Returns a double_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return a double_v pointer so it can be used in vector and array functions
*/
double_v* return_doublev_pointer(dict_dv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function free_doublev_dict 
* @brief Returns a double_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
*/
void free_doublev_dict(dict_dv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Safely frees a vector dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_doublev_dict(dict_dv** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICTV_GBC
     * @brief A macro for enabling automatic cleanup of dict_dv objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define DDICTV_GBC __attribute__((cleanup(_free_doublev_dict)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief determines if a key value pair exists in a vector dictionary
 *
 * @param dict The double vector dictionary 
 * @param key The key value being searched for 
 * @return true if the key value pair exists, false otherwise.
 */
bool has_key_doublev_dict(const dict_dv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Inserts an already existing dynamically allocated array to dictionary
 *
 * @param dict The double vector dictionary 
 * @param key The key value being searched for 
 * @param vec A dynamically allocated array of type double_v
 * @return true if the key value pair exists, false otherwise.
 */
bool insert_doublev_dict(dict_dv* dict, const char* key, double_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets the number of non-empty buckets in the vector dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t double_dictv_size(const dict_dv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the vector dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t double_dictv_alloc(const dict_dv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the vector dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t double_dictv_hash_size(const dict_dv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Creates a deep copy of a vector double dictionary 
 *
 * @param original A double vector dictionary 
 * @return A copy of a dictionary
 */
dict_dv* copy_doublev_dict(const dict_dv* original);
// -------------------------------------------------------------------------------- 

/**
 * @brief merges to double vector dictionaries
 *
 * Returns a merged double vector dictionary
 *
 * @param dict1 Pointer to a dictionary.
 * @param dict2 Pointer to a dictionary
 * @param overwrite true if the values should be overwritten, false otherwise
 * @return A merged dictionary
 */
dict_dv* merge_doublev_dict(const dict_dv* dict1, const dict_dv* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Clears all keys and values in the vector dictionary
 *
 * @param dict Pointer to a dictionary.
 */
void clear_doublev_dict(dict_dv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for double vector dictionary traversal
 *
 * @param key        Key string of the current entry
 * @param value      Pointer to the associated double_v vector
 * @param user_data  Optional context pointer passed through
 */
typedef void (*dict_dv_iterator)(const char* key, const double_v* value, void* user_data);
// -------------------------------------------------------------------------------- 

/**
 * @brief Applies a callback function to every key-value pair in a double_v dictionary
 *
 * Iterates over all entries in the hash table and invokes the callback for each.
 *
 * @param dict       Pointer to the dictionary
 * @param iter       Function pointer to apply to each key/value pair
 * @param user_data  Optional context pointer passed through to the callback
 * @return true on success, false if dict or iter is NULL (errno set to EINVAL)
 */
bool foreach_doublev_dict(const dict_dv* dict, dict_dv_iterator iter, void* user_data);
// -------------------------------------------------------------------------------- 

string_v* get_keys_doublev_dict(const dict_dv* dict);
// ================================================================================ 
// ================================================================================ 


// ================================================================================ 
// ================================================================================ 
// GENERIC MACROS

/**
 * @macro f_size
 * @brief Generic macro to get the number of elements in a double container
 *
 * This generic macro provides a unified interface for getting the current number
 * of elements in any double container type. It automatically selects the appropriate
 * size function based on the container type.
 * Currently supported container types:
 * - double_v (calls double_vector_size)
 * Additional container types will be added as the library expands.
 *
 * @param d_struct Pointer to double container structure
 * @return Size (number of elements) in the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define d_size(d_struct) _Generic((d_struct), \
    double_v*: double_vector_size, \
    dict_d*: double_dict_size, \
    dict_dv*: double_dictv_size) (d_struct)
// --------------------------------------------------------------------------------

/**
 * @macro f_alloc
 * @brief Generic macro to get the allocation size of a double container
 *
 * This generic macro provides a unified interface for getting the current allocation
 * size in any double container type. It automatically selects the appropriate
 * allocation function based on the container type.
 * Currently supported container types:
 * - double_v (calls double_vector_alloc)
 * Additional container types will be added as the library expands.
 *
 * @param f_struct Pointer to double container structure
 * @return Allocation size (capacity) of the container
 *         Returns LONG_MAX and sets errno to EINVAL for invalid input
 */
#define d_alloc(d_struct) _Generic((d_struct), \
    double_v*: double_vector_alloc, \
    dict_d*: double_dict_alloc, \
    dict_dv*: double_dictv_alloc) (d_struct)
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_double_H */
// ================================================================================
// ================================================================================
// eof
