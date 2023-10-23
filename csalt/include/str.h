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
#define INIT_STRING_SELECT(arg1, arg2, func, ...) func
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
    = INIT_STRING_SELECT(__VA_ARGS__, init_string_len, init_string_nol)(__VA_ARGS__)
// --------------------------------------------------------------------------------

/**
 * @brief This macro is used to initialize a string with automated garbage collection.
 *
 * This Macro allows a user to pass a variable number of arguments in the 
 * process of initializing a string container.  The user can pass just the 
 * string to encapsulate in the container, which will allocate memory just 
 * for that string, or the user can pass the string and a buffer size, which 
 * will allow for the overallocation of memory to better support future 
 * resizing.  In addition, this macro allows for automated garbage collection,
 * so memory does not have to be manually freed.  However, the memory can 
 * also be manually freed.
 *
 * @param str_struct A string container of type str 
 * @param string A string literal 
 * @param num A buffer size for the string, defaulted to strlen(string) + 1
 */
#define init_string_gbc(...) \
    __attribute__((cleanup(cleanup_string))) = INIT_STRING_SELECT(__VA_ARGS__, init_string_len, init_string_nol)(__VA_ARGS__);

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
void free_string(str *str_struct);
// --------------------------------------------------------------------------------

/**
 * @brief an intermediate function used to faciliate garbage cleanup 
 *
 * @param s A string container of type str
 */
void cleanup_string(str **s);
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
// --------------------------------------------------------------------------------

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
// ================================================================================
// ================================================================================
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* str_H */
// ================================================================================
// ================================================================================
// eof
