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
 * @param string A string container of type str 
 * @return The string encapsulated in the struct as a literal
 */
char* get_string(str* string);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the length of the string 
 *
 * This function returns the length of the string, so the user does not have 
 * to directly interact with the Struct which could cause undefined behavior
 * if the user accidentally modified a value.
 *
 * @param string A string container of type str 
 * @return The length of a string 
 */
size_t string_length(str* string);
// ================================================================================
// ================================================================================
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* str_H */
// ================================================================================
// ================================================================================
// eof
