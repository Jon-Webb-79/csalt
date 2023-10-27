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
#if !defined(__GNUC__) && !defined(__clang__)
#error "This code is only compatible with GCC and Clang"
#endif

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This struct acts as a container for string data types
 *
 * @attr data A pointer to the string data in memory 
 * @attr len The length of the string 
 * @attr is_dynamic true if memory is dynamically allocated, false otherwise
 */
typedef struct {
    char *data;
    size_t len;
    size_t alloc;
} str;
//  --------------------------------------------------------------------------------

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
str* init_string_nol(const char *strlit);
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
str* init_string_len(char *strlit, size_t num);
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
// ================================================================================
// ================================================================================

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
void _free_string(str **str_struct);
// --------------------------------------------------------------------------------

/**
 * Macro to add in front of ynamically allocated strings as an indicator to 
 * collect the dynamically allocated variable and free it when it goes out
 * of scope.
 */
#define gbc_str __attribute__((cleanup(_free_string)))
// --------------------------------------------------------------------------------

/**
 * Macro to free a str struct.  The macro allows a user to pass data without
 * having to dereference it.
 */
#define free_string(str_struct) _free_string(&(str_struct))
// ================================================================================
// ================================================================================

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
char* get_string(str* str_struct);
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
size_t string_length(str* str_struct);
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
// ================================================================================
// ================================================================================

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
bool insert_string_lit(str *str_struct, char *string, size_t index);
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
bool insert_string_str(str *str_struct_one, str *str_struct_two, size_t index);
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
bool trim_string(str *str_sturct);
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
str* copy_string(str *str_struct);
// --------------------------------------------------------------------------------

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
int compare_strings_lit(str *str_struct, char *string);
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
int compare_strings_str(str *str_struct_one, str *str_struct_two);
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
 * @brief Returns a pointer to the first occurrance of a char in a string 
 * string container.
 *
 * @param str_struct A string container of type str 
 * @param string A string literal 
 * @returns A pointer to the first occurance of a user specifiec char
 */
char* find_first_char(str *str_struct, char c);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a pointer to the last occurrance of a char in a string 
 * string container.
 *
 * @param str_struct A string container of type str 
 * @param string A string literal 
 * @returns A pointer to the last occurance of a user specifiec char
 */
char* find_last_char(str *str_struct, char c);
// ================================================================================
// ================================================================================

/**
 * @brief Returns the first instance of a substring in a string container 
 *
 * This function will return the first instance of a string in a string container.
 * The string pattern is represented by a string literal in this function.
 *
 * @param str_struct A string container of type str 
 * @param string A string literal
 * @returns A char pointer to the first instance of a string
 */
char* find_first_lit_strstr(str* str_struct, char* string);

// --------------------------------------------------------------------------------

/**
 * @brief Returns the first instance of a substring in a string container 
 *
 * This function will return the first instance of a string in a string 
 * container.  The string pattern is represented by another string container
 * in this function.
 *
 * @param str_struct_one A string container of type str 
 * @param string A string literal
 * @returns A char pointer to the first instance of a string
 */
char* find_first_str_strstr(str* str_struct_one, str *str_struct_two);
// --------------------------------------------------------------------------------

#define find_first_string(str_one, str_two) _Generic((str_two), \
        char*: find_first_lit_strstr, \
        default: find_first_str_strstr) (str_one, str_two)
// --------------------------------------------------------------------------------

/**
 * @brief Returns the last instance of a substring in a string container 
 *
 * This function will return the last instance of a string in a string container.
 * The string pattern is represented by a string literal in this function.
 *
 * @param str_struct A string container of type str 
 * @param string A string literal
 * @returns A char pointer to the last instance of a string
 */
char* find_last_lit_strstr(str* str_struct, char* string);

// --------------------------------------------------------------------------------

/**
 * @brief Returns the last instance of a substring in a string container 
 *
 * This function will return the last instance of a string in a string 
 * container.  The string pattern is represented by another string container
 * in this function.
 *
 * @param str_struct_one A string container of type str 
 * @param string A string literal
 * @returns A char pointer to the last instance of a string
 */
char* find_last_str_strstr(str* str_struct_one, str *str_struct_two);
// --------------------------------------------------------------------------------

#define find_last_string(str_one, str_two) _Generic((str_two), \
        char*: find_last_lit_strstr, \
        default: find_last_str_strstr) (str_one, str_two)
// --------------------------------------------------------------------------------

/**
 * @brief Returns the last instance of a substring in a string container 
 *
 * This function will return the last instance of a string in a string container.
 * The string pattern is represented by a string literal in this function.
 *
 * @param str_struct A string container of type str 
 * @param string A string literal
 * @returns A char pointer to the last instance of a string
 */
char* find_last_lit_strstr(str* str_struct, char* string);

// --------------------------------------------------------------------------------

/**
 * @brief Returns the last instance of a substring in a string container 
 *
 * This function will return the last instance of a string in a string 
 * container.  The string pattern is represented by another string container
 * in this function.
 *
 * @param str_struct_one A string container of type str 
 * @param string A string literal
 * @returns A char pointer to the last instance of a string
 */
char* find_last_str_strstr(str* str_struct_one, str *str_struct_two);
// --------------------------------------------------------------------------------

#define find_last_string(str_one, str_two) _Generic((str_two), \
        char*: find_last_lit_strstr, \
        default: find_last_str_strstr) (str_one, str_two)
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
char pop_str_char(str *str_token);
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
char pop_str_char_index(str *str_token, size_t index);
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
str* pop_string_token(str *str_struct, char token);
// ================================================================================
// ================================================================================
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* str_H */
// ================================================================================
// ================================================================================
// eof
