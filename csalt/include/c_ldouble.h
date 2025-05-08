// ================================================================================
// ================================================================================
// - File:    c_ldouble.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    May 05, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef c_ldouble_H
#define c_ldouble_H
// ================================================================================ 
// ================================================================================ 

#include <stdlib.h>
#include <stdbool.h>
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
    long double* data;
    size_t len;
    size_t alloc;
    alloc_t alloc_type;
} ldouble_v;
// --------------------------------------------------------------------------------

/**
* @function init_double_vector
* @brief Initializes a new dynamically allocated double vector with specified initial capacity
*
* @param buffer Initial capacity to allocate
* @return Pointer to new double_v object, or NULL on allocation failure
*         Sets errno to ENOMEM if memory allocation fails
*/
ldouble_v* init_ldouble_vector(size_t buffer);
// -------------------------------------------------------------------------------- 

/**
 * @function init_double_static_array
 * @brief Creates a stack-based double vector with static array
 *
 * @param size Size of the array
 */
#define init_ldouble_array(size) \
    ((ldouble_v){.data = (long double[size]){0}, .len = 0, .alloc = size, .alloc_type = STATIC})
// --------------------------------------------------------------------------------

/**
 * @brief returns a c style pointer to the beginning of an array 
 *
 * @param vec A pointer to a double_v data type 
 * @return A pointer to the beginning of a double array
 */
long double* c_ldouble_ptr(ldouble_v* vec);
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
bool push_back_ldouble_vector(ldouble_v* vec, const long double value);
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
bool push_front_ldouble_vector(ldouble_v* vec, const long double value);
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
bool insert_ldouble_vector(ldouble_v* vec, const long double value, size_t index);
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
const long double ldouble_vector_index(const ldouble_v* vec, size_t index);
// -------------------------------------------------------------------------------- 

/**
* @function double_vector_size
* @brief Returns current number of strings in vector
*
* @param vec Double vector to query
* @return Number of strings in vector, or LONG_MAX on error
*         Sets errno to EINVAL for NULL input
*/
const size_t ldouble_vector_size(const ldouble_v* vec);
// -------------------------------------------------------------------------------- 

/**
* @function double_vector_alloc
* @brief Returns current allocation size of vector
*
* @param vec Double vector to query
* @return Current allocation capacity, or LONG_MAX on error
*         Sets errno to EINVAL for NULL input
*/
const size_t ldouble_vector_alloc(const ldouble_v* vec);
// --------------------------------------------------------------------------------

/**
* @function pop_back_double_vector
* @brief Removes and returns last double value in vector
*
* @param vec Source double vector
* @return Pointer to removed double object, or NULL if vector empty
*         Sets errno to EINVAL for NULL input
*/
long double pop_back_ldouble_vector(ldouble_v* vec);
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
long double pop_front_ldouble_vector(ldouble_v* vec);
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
long double pop_any_ldouble_vector(ldouble_v* vec, size_t index);
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
void free_ldouble_vector(ldouble_v* vec);
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
void _free_ldouble_vector(ldouble_v** vec);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro FLTVEC_GBC
     * @brief A macro for enabling automatic cleanup of double vector objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_double_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define LDBLEVEC_GBC __attribute__((cleanup(_free_ldouble_vector)))
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
void reverse_ldouble_vector(ldouble_v* vec);
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
void swap_ldouble(long double* a, long double* b);
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
void sort_ldouble_vector(ldouble_v* vec, iter_dir direction);
// -------------------------------------------------------------------------------- 

/**
* @function trim_double_vector
* @brief Trims all un-necessary memory from a vector
*
* @param vec double vector to trim
* @return void
*         Sets errno to EINVAL if vec is NULL or invalid
*/
void trim_ldouble_vector(ldouble_v* vec);
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
size_t binary_search_ldouble_vector(ldouble_v* vec, long double value, long double tolerance, bool sort_first);
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
void update_ldouble_vector(ldouble_v* vec, size_t index, long double replacement_value);
// -------------------------------------------------------------------------------- 

/**
 * @function min_double_vector 
 * @brief Returns the minimum value in a vector or array 
 *
 * @param vec A double vector or array object 
 * @return The minimum value in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
long double min_ldouble_vector(ldouble_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function max_double_vector 
 * @brief Returns the maximum value in a vector or array 
 *
 * @param vec A double vector or array object 
 * @return The maximum value in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
long double max_ldouble_vector(ldouble_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function sum_double_vector 
 * @brief Returns the summation of all values in a vector or array
 *
 * @param vec A double vector or array object 
 * @return The summation of all values in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
long double sum_ldouble_vector(ldouble_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function average_double_vector 
 * @brief Returns the average of all values in a vector or array
 *
 * @param vec A double vector or array object 
 * @return The average of all values in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
long double average_ldouble_vector(ldouble_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @function stdev_double_vector 
 * @brief Returns the standard deviation of all values in a vector or array
 *
 * @param vec A double vector or array object 
 * @return The standard deviation of all values in a vector.  Sets errno to EINVAL if vec or 
 *         vec-data is NULL, or if length is 0 and returns FLT_MAX
 */
long double stdev_ldouble_vector(ldouble_v* vec);
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
ldouble_v* cum_sum_ldouble_vector(ldouble_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief creates a deep copy of a vector
 *
 * @param original A vector to be copied 
 * @return A copy of a double vector
 */
ldouble_v* copy_ldouble_vector(const ldouble_v* original);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_ldouble_H */
// ================================================================================
// ================================================================================
// eof
