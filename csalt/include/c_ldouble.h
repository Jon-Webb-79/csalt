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

// DICTIONARY PROTOTYPES 

/**
 * @typedef dict_ld
 * @brief Opaque struct representing a dictionary.
 *
 * This structure encapsulates a hash table that maps string keys to long double values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_ld dict_ld;
// --------------------------------------------------------------------------------

/**
 * @brief Initializes a new dictionary.
 *
 * Allocates and initializes a dictionary object with a default size for the hash table.
 *
 * @return A pointer to the newly created dictionary, or NULL if allocation fails.
 */
dict_ld* init_ldouble_dict();
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
bool insert_ldouble_dict(dict_ld* dict, const char* key, long double value);
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
long double pop_ldouble_dict(dict_ld* dict,  const char* key);
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
long double get_ldouble_dict_value(const dict_ld* dict, const char* key);
// --------------------------------------------------------------------------------

/**
 * @brief Frees the memory associated with the dictionary.
 *
 * Releases all memory allocated for the dictionary, including all key-value pairs.
 *
 * @param dict Pointer to the dictionary to free.
 */
void free_ldouble_dict(dict_ld* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Safely frees a dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_ldouble_dict(dict_ld** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICT_GBC
     * @brief A macro for enabling automatic cleanup of dict_ld objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define LDDICT_GBC __attribute__((cleanup(_free_ldouble_dict)))
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
bool update_ldouble_dict(dict_ld* dict, const char* key, long double value);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the number of non-empty buckets in the dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t ldouble_dict_size(const dict_ld* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t ldouble_dict_alloc(const dict_ld* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t ldouble_dict_hash_size(const dict_ld* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Checks if a key exists in the dictionary without retrieving its value
 * 
 * @param dict Pointer to the dictionary
 * @param key Key to check for
 * @return bool true if key exists, false otherwise
 */
bool has_key_ldouble_dict(const dict_ld* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a deep copy of a dictionary
 * 
 * @param dict Pointer to the dictionary to copy
 * @return dict_ld* New dictionary containing copies of all entries, NULL on error
 */
dict_ld* copy_ldouble_dict(const dict_ld* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Removes all entries from a dictionary without freeing the dictionary itself
 * 
 * @param dict Pointer to the dictionary to clear
 * @return bool true if successful, false otherwise
 */
bool clear_ldouble_dict(dict_ld* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all keys in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return char** Array of strings containing copies of all keys, NULL on error
 */
string_v* get_keys_ldouble_dict(const dict_ld* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets all values in the dictionary
 * 
 * @param dict Pointer to the dictionary
 * @return long double* Array containing all values, NULL on error
 */
ldouble_v* get_values_ldouble_dict(const dict_ld* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Merges two dictionaries into a new dictionary
 * 
 * @param dict1 First dictionary
 * @param dict2 Second dictionary
 * @param overwrite If true, values from dict2 override dict1 on key conflicts
 * @return dict_ld* New dictionary containing merged entries, NULL on error
 */
dict_ld* merge_ldouble_dict(const dict_ld* dict1, const dict_ld* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for dictionary traversal
 */
typedef void (*dict_iterator)(const char* key, long double value, void* user_data);

/**
 * @brief Iterates over all dictionary entries in insertion order
 * 
 * @param dict Pointer to the dictionary
 * @param iter Iterator function to call for each entry
 * @param user_data Optional user data passed to iterator function
 */
bool foreach_ldouble_dict(const dict_ld* dict, dict_iterator iter, void* user_data);
// ================================================================================ 
// ================================================================================ 
// VECTOR DICTIONARY PROTOTYPES 

/**
 * @typedef dict_fv
 * @brief Opaque struct representing a dictionary for vector data types.
 *
 * This structure encapsulates a hash table that maps string keys to a vector of ldouble values.
 * The details of the struct are hidden from the user and managed internally.
 */
typedef struct dict_ldv dict_ldv;
// -------------------------------------------------------------------------------- 

/**
 * @brief Creates a dictionary of ldouble vectors
 *
 * @returns a dictionary of ldouble vectors
 */
dict_ldv* init_ldoublev_dict(void);
// -------------------------------------------------------------------------------- 

/**
* @function create_ldoublev_dict
* @brief Creates a key vector/array pair 
*
* @param dict A fict_fv data type
* @param key a string literal key
* @param size The size of the array or vector 
* @return true if the function executes succesfully, false otherwise
*/
bool create_ldoublev_dict(dict_ldv* dict, char* key, size_t size);
// -------------------------------------------------------------------------------- 

/**
* @function pop_ldoublev_dict 
* @brief Removes a statically or dynamically allocated array from the dictionary
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return true if the function executes succesfully, false otherwise
*/
bool pop_ldoublev_dict(dict_ldv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function pop_ldoublev_dict 
* @brief Returns a ldouble_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
* @param key a string literal key
* @return a ldouble_v pointer so it can be used in vector and array functions
*/
ldouble_v* return_ldoublev_pointer(dict_ldv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
* @function free_ldoublev_dict 
* @brief Returns a ldouble_v pointer for use in vector and array functions
*
* @param dict A fict_fv data type
*/
void free_ldoublev_dict(dict_ldv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Safely frees a vector dictionary and sets the pointer to NULL.
 *
 * A wrapper around `free_dict` that ensures the dictionary pointer is also set to NULL
 * after being freed. Useful for preventing dangling pointers.
 *
 * @param dict Pointer to the dictionary pointer to free.
 */
void _free_ldoublev_dict(dict_ldv** dict);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined (__clang__)
    /**
     * @macro DICTV_GBC
     * @brief A macro for enabling automatic cleanup of dict_ldv objects.
     *
     * This macro uses the cleanup attribute to automatically call `_free_vector`
     * when the scope ends, ensuring proper memory management.
     */
    #define LDDICTV_GBC __attribute__((cleanup(_free_ldoublev_dict)))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief determines if a key value pair exists in a vector dictionary
 *
 * @param dict The ldouble vector dictionary 
 * @param key The key value being searched for 
 * @return true if the key value pair exists, false otherwise.
 */
bool has_key_ldoublev_dict(const dict_ldv* dict, const char* key);
// -------------------------------------------------------------------------------- 

/**
 * @brief Inserts an already existing dynamically allocated array to dictionary
 *
 * @param dict The ldouble vector dictionary 
 * @param key The key value being searched for 
 * @param vec A dynamically allocated array of type ldouble_v
 * @return true if the key value pair exists, false otherwise.
 */
bool insert_ldoublev_dict(dict_ldv* dict, const char* key, ldouble_v* vec);
// -------------------------------------------------------------------------------- 

/**
 * @brief Gets the number of non-empty buckets in the vector dictionary.
 *
 * Returns the total number of buckets in the hash table that contain at least one key-value pair.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of non-empty buckets.
 */
size_t ldouble_dictv_size(const dict_ldv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total capacity of the vector dictionary.
 *
 * Returns the total number of buckets currently allocated in the hash table.
 *
 * @param dict Pointer to the dictionary.
 * @return The total number of buckets in the dictionary.
 */
size_t ldouble_dictv_alloc(const dict_ldv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Gets the total number of key-value pairs in the vector dictionary.
 *
 * Returns the total number of key-value pairs currently stored in the dictionary.
 *
 * @param dict Pointer to the dictionary.
 * @return The number of key-value pairs.
 */
size_t ldouble_dictv_hash_size(const dict_ldv* dict);
// --------------------------------------------------------------------------------

/**
 * @brief Creates a deep copy of a vector ldouble dictionary 
 *
 * @param original A ldouble vector dictionary 
 * @return A copy of a dictionary
 */
dict_ldv* copy_ldoublev_dict(const dict_ldv* original);
// -------------------------------------------------------------------------------- 

/**
 * @brief merges to ldouble vector dictionaries
 *
 * Returns a merged ldouble vector dictionary
 *
 * @param dict1 Pointer to a dictionary.
 * @param dict2 Pointer to a dictionary
 * @param overwrite true if the values should be overwritten, false otherwise
 * @return A merged dictionary
 */
dict_ldv* merge_ldoublev_dict(const dict_ldv* dict1, const dict_ldv* dict2, bool overwrite);
// -------------------------------------------------------------------------------- 

/**
 * @brief Clears all keys and values in the vector dictionary
 *
 * @param dict Pointer to a dictionary.
 */
void clear_ldoublev_dict(dict_ldv* dict);
// -------------------------------------------------------------------------------- 

/**
 * @brief Iterator function type for ldouble vector dictionary traversal
 *
 * @param key        Key string of the current entry
 * @param value      Pointer to the associated ldouble_v vector
 * @param user_data  Optional context pointer passed through
 */
typedef void (*dict_ldv_iterator)(const char* key, const ldouble_v* value, void* user_data);
// -------------------------------------------------------------------------------- 

/**
 * @brief Applies a callback function to every key-value pair in a ldouble_v dictionary
 *
 * Iterates over all entries in the hash table and invokes the callback for each.
 *
 * @param dict       Pointer to the dictionary
 * @param iter       Function pointer to apply to each key/value pair
 * @param user_data  Optional context pointer passed through to the callback
 * @return true on success, false if dict or iter is NULL (errno set to EINVAL)
 */
bool foreach_ldoublev_dict(const dict_ldv* dict, dict_ldv_iterator iter, void* user_data);
// -------------------------------------------------------------------------------- 

string_v* get_keys_ldoublev_dict(const dict_ldv* dict);
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
#define ld_size(d_struct) _Generic((d_struct), \
    ldouble_v*: ldouble_vector_size, \
    dict_ld*: ldouble_dict_size, \
    dict_ldv*: ldouble_dictv_size) (d_struct)
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
#define ld_alloc(d_struct) _Generic((d_struct), \
    ldouble_v*: ldouble_vector_alloc, \
    dict_ld*: ldouble_dict_alloc, \
    dict_ldv*: ldouble_dictv_alloc) (d_struct)
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_ldouble_H */
// ================================================================================
// ================================================================================
// eof
