// ================================================================================
// ================================================================================
// - File:    str.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    October 20, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#error "This code requires C11 or later."
#endif

#ifndef str_H
#define str_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "admin.h"

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================
// ================================================================================
// CONSTANTS FOR MEMORY MANAGEMENT STRATEGY

extern const size_t STR_THRESHOLD;
extern const size_t STR_FIXED_AMOUNT;

// ================================================================================
// ================================================================================
// STRUCT DEFINITION FOR STRING CONTAINER

typedef struct str str;  // Forward declaratioon

/**
 * @brief This struct acts as a container for string data types
 *
 * @attribute data A pointer to the string data in memory 
 * @attribute len The length of the string 
 * @attribute is_dynamic true if memory is dynamically allocated, false otherwise
 * @function dec_iter A pointer to a function that provices a decorator to
 *                    the str iterator.
 */
struct str {
    char *data;
    size_t len;
    size_t alloc;
};
// ================================================================================
// ================================================================================
// PROTOTYPES FOR BASIC STRING OPERATIONS

/**
 * @brief Initializes a string structure with the given string literal.
 * 
 * This function allocates memory for a `str` structure and its associated data,
 * then copies the provided string literal into that data. The length and allocated
 * size are set accordingly.
 *
 * @param strlit The string literal to initialize the `str` structure with.
 * @return A pointer to the initialized `str` structure, or NULL if memory allocation fails.
 */
str* init_string_nol(const char* strlit);
// --------------------------------------------------------------------------------

/**
 * @brief Initializes a string structure with the given string literal
 * 
 * This function allocates memory for a `str` structure and its associated data,
 * then copies the provided string literal into that data. The length and allocated
 * size are set accordingly.  This function also sets a bugger size in number
 * of characters.
 *
 * @param strlit The string literal to initialize the `str` structure with.
 * @param num The number of characters to allocate in the buffer size
 * @return A pointer to the initialized `str` structure, or NULL if memory allocation fails.
 */
str* init_string_len(char* strlit, size_t num);
// --------------------------------------------------------------------------------

/**
 * @brief A Macro that supports string initialization overloading
 *
 * This Macro interacts with other Macros to support overloading in the 
 * process of initializing a string container.
 */
#define TWO_FUNC_SELECT(arg1, arg2, func, ...) func
// --------------------------------------------------------------------------------

/**
 * @brief This macro is used to initialize a string
 *
 * This Macro allows a user to pass a variable number of arguments in the 
 * process of initializing a string container.  The user can pass just the 
 * string to encapsulate in the container, which will allocate memory just 
 * for that string, or the user can pass the string and a buffer size, which 
 * will allow for the overallocation of memory to better support future 
 * resizing.
 *
 * @param str_struct A string container of type str 
 * @param string A string literal 
 * @param num A buffer size for the string, defaulted to strlen(string) + 1
 */
#define init_string(...) \
    TWO_FUNC_SELECT(__VA_ARGS__, init_string_len, init_string_nol)(__VA_ARGS__)
// --------------------------------------------------------------------------------

/**
 * @brief Frees all memory from a str container 
 *
 * This function will check to see if the memory for the struct and the 
 * string has not yet been freed.  If the memory was freed, the function 
 * will exit without error.  If the memory has not been freed, it will 
 * free the memory.
 *
 * @param str_struct A string container of type str 
 */
void _free_string(str** str_struct);
// --------------------------------------------------------------------------------

/**
 * Macro to add in front of ynamically allocated strings as an indicator to 
 * collect the dynamically allocated variable and free it when it goes out
 * of scope.
 */
#if defined(__GNUC__) || defined(__clang__)
#define gbc_str __attribute__((cleanup(_free_string)))
// --------------------------------------------------------------------------------

/**
 * Macro to free a str struct.  The macro allows a user to pass data without
 * having to dereference it.  This macro only works with GCC and clang 
 * compilers
 */
    #define free_string(str_struct) _free_string(&(str_struct))
#endif
// --------------------------------------------------------------------------------

/**
 * @brief Retrieves the string value
 *
 * This function returns the string value in a struct container.  This allows
 * a user to avoid directly interacting with the struct which could cause
 * undefined behavior if the user accidentally modified a value.
 *
 * @param str_struct A string container of type str 
 * @return The string encapsulated in the struct as a literal
 */
char* get_string(const str* str_struct);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the length of the string 
 *
 * This function returns the length of the string, so the user does not have 
 * to directly interact with the Struct which could cause undefined behavior
 * if the user accidentally modified a value.
 *
 * @param str_struct A string container of type str 
 * @return The length of a string 
 */
int string_length(str* str_struct);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the size of allocated memory in chars
 *
 * This function returns the memory allocation of the string, so the user does not have 
 * to directly interact with the Struct which could cause undefined behavior
 * if the user accidentally modified a value.
 *
 * @param str_struct A string container of type str 
 * @return The memory allocation of a string in chars
 */
size_t string_memory(str* str_struct);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts a string literal into a struct of type str 
 *
 * This function will insert a string literal into the string within a 
 * str struct.  This function allows a user to insert a string at any 
 * index within the existing string.
 *
 * @param str_struct A struct of type str.
 * @param string A null terminated string literal.
 * @return true if the function executes succesfully, false otherwise
 */
bool insert_string_lit(str* str_struct, const char* string, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts a string container into a struct of type str 
 *
 * This function will insert a string container into the string within a 
 * str struct.  This function allows a user to insert a string at any 
 * index within the existing string.
 *
 * @param str_struct_one A struct of type str whic contains the string to be modified.
 * @param string A string container of type str which will be inserted into another container.
 * @return true if the function executes succesfully, false otherwise
 */
bool insert_string_str(str* str_struct_one, str* str_struct_two, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A macro that inserts a string into another string 
 *
 * This macro will allow a string literal or a string container to be inserted 
 * into a string container at a user defined location.
 *
 * @param str_one A string container of type str. This is the string having data inserted into it
 * @param str_two A string literal or string container of type str 
 * @param index An index that can range from 0 to the length of str_one
 * @param true if the function executes succesfully, false otherwise.
 */
#define insert_string(str_one, str_two, index) _Generic((str_two), \
    char*: insert_string_lit, \
    default: insert_string_str) (str_one, str_two, index)
// ================================================================================
// ================================================================================

/**
 * @brief Tims the string memory to the minimum necessary size 
 *
 * THis function will determine the minimum memory allocation needed to fit 
 * the string and will resize to the minimum memory if oversized.
 *
 * @param str_struct A string container of type str 
 * @return true if operation is succesful, false otherwise with stderr printout
 */
bool trim_string(str* str_sturct);
// --------------------------------------------------------------------------------

/**
 * @brief Creates a deep copy of one string container
 *
 * This function will create a deep copy of an input string container to include 
 * the memory allocation 
 *
 * @param str_struct A string container of type str 
 * @return A string container of type str
 */
str* copy_string(str* str_struct);
// ================================================================================
// ================================================================================

/**
 * @brief Compares the string within a str container with a string literal 
 *
 * This function will compare the string within a str container with a 
 * string literal.
 *
 * @param str_struct A string container of type str 
 * @param string A string literal 
 * @return 0 if the strings are equal, >0 if the first non-matching character in 
 *         str_struct s greater than string, < 0 otherwise.
 */
int compare_strings_lit(str* str_struct, char* string);
// --------------------------------------------------------------------------------

/**
 * @brief Compare the string within a str struct with another str struct 
 *
 * This function will compare the strings between two str structs.
 *
 * @param str_struct_one A string container of type str 
 * @param str_struct_two A string container of type str 
 * @return 0 if the strings are equal, >0 if the first non-matching character in 
 *         str_struct s greater than the other string, < 0 otherwise. 
 */
int compare_strings_str(str* str_struct_one, str* str_struct_two);
// --------------------------------------------------------------------------------

/**
 * @brief Compares two strings 
 *
 * This macro will compare two string containers or a string container with a
 * string literal.
 *
 * @param str_one A string container of type str 
 * @param str_two A string container or a string literal 
 * @return 0 if the strings are equal, >0 if the first non-matching character in 
 *         str_struct s greater than the other string, < 0 otherwise. 
 */
#define compare_strings(str_one, str_two) _Generic((str_two), \
    char*: compare_strings_lit, \
    default: compare_strings_str) (str_one, str_two)
// ================================================================================
// ================================================================================

/**
 * @brief Finds the first occurance of a char between two pointers
 *
 * This function searches all chars between two char pointers and returns 
 * the first occurance of a char character.
 *
 * @param c The char being searched for 
 * @param min_ptr The min pointer position 
 * @param max_ptr The max pointer position
 * @return A char pointer to character c or a NULL pointer.
 */
char* first_char(char c, char* min_ptr, char* max_ptr);
// --------------------------------------------------------------------------------

/**
 * @brief Finds the last occurance of a char between two pointers
 *
 * This function searches all chars between two char pointers and returns 
 * the last occurance of a char character.
 *
 * @param c The char being searched for 
 * @param min_ptr The min pointer position 
 * @param max_ptr The max pointer position
 * @return A char pointer to character c or a NULL pointer.
 */
char* last_char(char c, char* min_ptr, char* max_ptr);
// ================================================================================
// ================================================================================

/**
 * @brief Finds the first occurance of a string literal between two pointers 
 *
 * @param string A string literal to be searched for 
 * @param min_ptr A pointer to the minimum bounds of the string 
 * @param max_ptr A pointer to the maximum bounds of the string.
 * @return A pointer to the first occurance of a substring
 */
char* first_literal_between_ptrs(char* string, char* min_ptr, char* max_ptr);
// --------------------------------------------------------------------------------

/**
 * @brief Finds the first occurance of a string container between two pointers 
 *
 * @param string A string literal to be searched for 
 * @param min_ptr A pointer to the minimum bounds of the string 
 * @param max_ptr A pointer to the maximum bounds of the string.
 * @return A pointer to the first occurance of a substring
 */
char* first_str_between_ptrs(str* str_struct, char* min_ptr, char* max_ptr);
// --------------------------------------------------------------------------------

/**
 * @brief Finds the first substring between two pointers
 */
#define first_substring(string, min_ptr, max_ptr) _Generic((string), \
        char*: first_literal_between_ptrs, \
        default: first_str_between_ptrs) (string, min_ptr, max_ptr)
// --------------------------------------------------------------------------------

/**
 * @brief Finds the occurance occurance of a string literal between two pointers 
 *
 * @param string A string literal to be searched for 
 * @param min_ptr A pointer to the minimum bounds of the string 
 * @param max_ptr A pointer to the maximum bounds of the string.
 * @return A pointer to the last occurance of a substring
 */
char* last_literal_between_ptrs(char* string, char* min_ptr, char* max_ptr);
// --------------------------------------------------------------------------------

/**
 * @brief Finds the last occurance of a string container between two pointers 
 *
 * @param string A string literal to be searched for 
 * @param min_ptr A pointer to the minimum bounds of the string 
 * @param max_ptr A pointer to the maximum bounds of the string.
 * @return A pointer to the last occurance of a substring
 */
char* last_str_between_ptrs(str* string, char* min_ptr, char* max_ptr);
// --------------------------------------------------------------------------------
/**
 * @brief Finds the first substring between two pointers
 */
#define last_substring(string, min_ptr, max_ptr) _Generic((string), \
        char*: last_literal_between_ptrs, \
        default: last_str_between_ptrs) (string, min_ptr, max_ptr)
// ================================================================================
// ================================================================================

/**
 * @brief pop and return the last char in a string container 
 *
 * This function will pop the last index prior to the null terminator from a
 * string and return it it the user 
 *
 * @breif str_token A string container of type string
 * @returns a char value
 */
char pop_str_char(str* str_token);
// --------------------------------------------------------------------------------

/**
 * @brief Pops and returns a char from a user defined index
 *
 * This function will remove a char from a user defined index and return it to
 * the user.
 *
 * @brief str_token A string container of type str 
 * @brief index An index to pop from
 * @brief A char value
 */
char pop_str_char_index(str* str_token, size_t index);
// --------------------------------------------------------------------------------

#define pop_string_char(...) \
    TWO_FUNC_SELECT(__VA_ARGS__, pop_str_char_index, pop_str_char)(__VA_ARGS__)
// ================================================================================
// ================================================================================

/**
 * @brief Pops a substring to the right of a token 
 *
 * This function will allow a user to pop the sub-string to the right of the right
 * most token in a string.
 *
 * @param str_struct A string container of type str 
 * @param token a char token that divides string data 
 * @returns A string container of type str
 */
str* pop_string_token(str* str_struct, char token);
// ================================================================================
// ================================================================================
/**
 * @breif Determines if a pointer is within the string in a str container 
 *
 * @param str_struct A string container of type str 
 * @param ptr A char pointer
 * @return true if pointer is inside container, false otherwise
 */
bool ptr_in_str_container(str* str_struct, char* ptr);
// --------------------------------------------------------------------------------

/**
 * @brief Determine if a pointer is within the min and maximum of a string literal 
 *
 * @brief A pointer to a char array or string literal.
 * @brief ptr A char pointer 
 * @return true if pointer is inside container, false otherwise
 */
bool ptr_in_literal(char* string, char* ptr);
// --------------------------------------------------------------------------------
typedef void (*str_decorator)(char*);
// Define iterators

/**
 * @brief A struct that provides funciton pointers to various string iterator function.
 *
 * @function begin A pointer to a function that provides a pointer to the first 
 *           pseudo index in a st data structure
 * @function end A pointer to a function that provides a pointer to the last 
 *               pseudo index in a data str structure
 * @function next A pointer to a function that provides a pointer to the next 
 *                psuedo index in a str data structure
 * @function prev A pointer to a function that provides a pointer to the previous 
 *                pseudo index in a str data structure.
 */
typedef struct {
    char* (*begin) (str *s);
    char* (*end) (str *s);
    void (*next) (char** current);
    void (*prev) (char** current);
    char (*get) (char** current);
} str_iterator;
// --------------------------------------------------------------------------------

/**
 * @brief Initializes a char iterator
 */
str_iterator init_str_iterator();
// --------------------------------------------------------------------------------

/**
 * brief A forward iterator with a decarator
 *
 * This function encapsulates a forward iterator on a string container of type 
 * str, such that a develepor can insert a function of their own making to 
 * act on each char character in memory.
 *
 * @param begin A pointer to the start point of the iteration.
 * @param end A pointer to the end point of the iteration.
 * @param direction An enum that can either be FORWARD or REVERSE 
 * @param decorator A function of type void func(char*)
 */
void dec_str_iter(char* begin, char* end,
                  iter_dir direction, str_decorator decorator);
// ================================================================================
// ================================================================================

/**
 * @brief Transforms chars in an array to uppercase in memory
 *
 * @param s A string container of type str
 */
void to_uppercase(str* s);
// --------------------------------------------------------------------------------

/**
 * @brief Transforms chars in an array to lowercase in memory 
 *
 * @param s A string container of type str
 */
void to_lowercase(str *s);
// ================================================================================
// ================================================================================

/**
 * @brief Removes all occurances of a sub-string from a string between the 
 * bound of an upper an lower pointer value 
 *
 * This function looks for any occurance of a sub-string container in a string
 * container and removes all instances of that string from another string 
 * container.
 *
 * @param string The string containing the sub-string to be removed.
 * @param substring The substring pattern to be removed from the string.
 * @return true if the function executes succesfully, false otherwise.
 */
bool drop_str_substring(str* string, str* substring, char* min_ptr, char* max_ptr);
// --------------------------------------------------------------------------------

/**
 * @brief Removes all occurances of a sub-string from a string between the 
 * bound of an upper an lower pointer value 
 *
 * This function looks for any occurance of a sub-string container as a literal
 * and removes all instances of that string from another string container.
 *
 * @param string The string containing the sub-string to be removed.
 * @param substring The substring pattern to be removed from the string.
 * @return true if function executes succesfully, false otherwise
 */
bool drop_literal_substring(str* string, char* substring, char* min_ptr, char* max_ptr);
// --------------------------------------------------------------------------------

/**
 * @brief Removes all occurances of a sub-string from a string
 */
#define drop_substring(string, substring, min_ptr, max_ptr) _Generic((substring), \
        str*: drop_str_substring, \
        default: drop_literal_substring) (string, substring, min_ptr, max_ptr)
// ================================================================================
// ================================================================================

/**
 * @brief Replaces a sub-string in a string with another sub-string 
 *
 * This function replaces a sub-string within a string container with another 
 * sub-string in a container.
 *
 * @param string A string containing a sub-string
 * @param pattern The sub-string pattern to replace 
 * @param replace_string The replacement pattern for a sub-string 
 * @param min_ptr A pointer to the minimum pointer position 
 * @param max_ptr A pointer to the max pointer position 
 * @return true of function executes succesfully, false otherwise
 */
bool replace_str_substring(str* string, str* pattern, str* replace_string,
                           char* min_ptr, char* max_ptr);
// --------------------------------------------------------------------------------

/**
 * @brief Replaces a sub-string in another sub-string 
 *
 * This function replaces a sub-string within a string literal with another 
 * sub-string 
 *
 * @param string A string containing a sub-string
 * @param pattern The sub-string pattern to replace 
 * @param replace_string The replacement pattern for a sub-string 
 * @param min_ptr A pointer to the minimum pointer position 
 * @param max_ptr A pointer to the max pointer position 
 * @return true of function executes succesfully, false otherwise 
 */
bool replace_literal_substring(str* string, char* pattern, char* replace_string,
                               char* min_ptr, char* max_ptr);
// --------------------------------------------------------------------------------

/**
 * @brief Generic interface for functions that replace one sub-string with another.
 *
 * This function will select between the use of the replace_str_substring or
 * the replace_literal_substring based on the data type of pattern.  This 
 * allows a developer to use a string container or a string literal to represent
 * the sub-string to be searched for and to replace the original sub-string.
 *
 * @param string A string containing a sub-string
 * @param pattern The sub-string pattern to replace 
 * @param replace_string The replacement pattern for a sub-string 
 * @param min_ptr A pointer to the minimum pointer position 
 * @param max_ptr A pointer to the max pointer position 
 * @return true of function executes succesfully, false otherwise
 */
#define replace_substring(string, pattern, replace_string, min_ptr, max_ptr) \
    _Generic((pattern), \
    str*: replace_str_substring, \
    default: replace_literal_substring) (string, pattern, replace_string, min_ptr, max_ptr)
// ================================================================================
// ================================================================================
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* str_H */
// ================================================================================
// ================================================================================
// eof
