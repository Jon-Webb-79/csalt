// ================================================================================
// ================================================================================
// - File:    vector.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    November 06, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#error "This code requires C11 or later."
#endif

#ifndef vector_H
#define vector_H

#include <stdio.h>
#include <math.h>

#include "admin.h"
#include "str.h"
#include "swap.h"

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================
// ================================================================================
// Vector data type structs 

/**
 * @brief A macro to create structs for vectors of various data types.
 *
 * @param type The data type that will be assigned to the data pointer.
 * @param dat_type The name given to the typedef for the struct.
 */
#define VECTOR_STRUCT(type, dat_type) \
    typedef struct { type *data; size_t len; size_t alloc; } dat_type
// --------------------------------------------------------------------------------
// Create typedef for each possible vector type 

VECTOR_STRUCT(char, char_v);
VECTOR_STRUCT(unsigned char, uchar_v);
VECTOR_STRUCT(short int, short_v);
VECTOR_STRUCT(unsigned short int, ushort_v);
VECTOR_STRUCT(int, int_v);
VECTOR_STRUCT(unsigned int, uint_v);
VECTOR_STRUCT(long int, long_v);
VECTOR_STRUCT(unsigned long int, ulong_v);
VECTOR_STRUCT(long long int, llong_v);
VECTOR_STRUCT(unsigned long long int, ullong_v);
VECTOR_STRUCT(float, float_v);
VECTOR_STRUCT(double, double_v);
VECTOR_STRUCT(long double, ldouble_v);
VECTOR_STRUCT(bool, bool_v);
VECTOR_STRUCT(str, string_v);
// ================================================================================
// ================================================================================
// Initialization functions 

/**
 * @brief typedef function pointer to support init_vector functions
 */
typedef void* (*InitVecFunc)(size_t);

/**
 * @brief Function that selects the correct vector initialization function based on type 
 *
 * @param dat_type An enum of type dtype containing a keyword defining the data type 
 * @return A vector initialization function for a specific data type.
 */
InitVecFunc init_vector(dtype dat_type);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of char indices
 * @return A char_v struct 
 */
char_v* init_char_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned char indices
 * @return A uchar_v struct 
 */
uchar_v* init_uchar_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of short int indices
 * @return A short_v struct 
 */
short_v* init_short_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned short int indices
 * @return A ushort_v struct 
 */
ushort_v* init_ushort_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of int indices
 * @return A int_v struct 
 */
int_v* init_int_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned int indices
 * @return A uint_v struct 
 */
uint_v* init_uint_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of long int indices
 * @return A long_v struct 
 */
long_v* init_long_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned long int indices
 * @return A ulong_v struct 
 */
ulong_v* init_ulong_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of long long int indices
 * @return A llong_v struct 
 */
llong_v* init_llong_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned long long int indices
 * @return A llong_v struct 
 */
ullong_v* init_ullong_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of float indices
 * @return A float_v struct 
 */
float_v* init_float_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of double indices
 * @return A double_v struct 
 */
double_v* init_double_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of long double indices
 * @return A ldouble_v struct 
 */
ldouble_v* init_ldouble_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of bool indices
 * @return A bool_v struct 
 */
bool_v* init_bool_vector(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of str indices
 * @return A string_v struct 
 */
string_v* init_string_vector(size_t buff);
// ================================================================================
// ================================================================================
// FREE FUNCTIONS 

/**
 * @brief Free all memory associated with a char_v data type 
 *
 * @param A char_v data struct
 */
void free_char_vector(char_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a uchar_v data type 
 *
 * @param A uchar_v data struct
 */
void free_uchar_vector(uchar_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a short_v data type 
 *
 * @param A short_v data struct
 */
void free_short_vector(short_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a ushort_v data type 
 *
 * @param A ushort_v data struct
 */
void free_ushort_vector(ushort_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a int_v data type 
 *
 * @param A int_v data struct
 */
void free_int_vector(int_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a uint_v data type 
 *
 * @param A uint_v data struct
 */
void free_uint_vector(uint_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a long_v data type 
 *
 * @param A long_v data struct
 */
void free_long_vector(long_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a ulong_v data type 
 *
 * @param A ulong_v data struct
 */
void free_ulong_vector(ulong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a llong_v data type 
 *
 * @param A ullong_v data struct
 */
void free_llong_vector(llong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a ullong_v data type 
 *
 * @param A ullong_v data struct
 */
void free_ullong_vector(ullong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a float_v data type 
 *
 * @param A float_v data struct
 */
void free_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a double_v data type 
 *
 * @param A double_v data struct
 */
void free_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a ldouble_v data type 
 *
 * @param A ldouble_v data struct
 */
void free_ldouble_vector(ldouble_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a bool_v data type 
 *
 * @param A bool_v data struct
 */
void free_bool_vector(bool_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a string_v data type 
 *
 * @param A string_v data struct
 */
void free_string_vector(string_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Macro to free all types of dynamically allocated vectors.
 *
 * @param vec A dynamically allocated vector struct.
 */
#define free_vector(vec) _Generic((vec), \
    char_v*: free_char_vector, \
    uchar_v*: free_uchar_vector, \
    short_v*: free_short_vector, \
    ushort_v*: free_ushort_vector, \
    int_v*: free_int_vector, \
    uint_v*: free_uint_vector, \
    long_v*: free_long_vector, \
    ulong_v*: free_ulong_vector, \
    llong_v*: free_llong_vector, \
    ullong_v*: free_ullong_vector, \
    float_v*: free_float_vector, \
    double_v*: free_double_vector, \
    ldouble_v*: free_ldouble_vector, \
    bool_v*: free_bool_vector, \
    default: free_string_vector) (vec)
// ================================================================================
// ================================================================================
// Cleanup functions for garbage collection
// - while being public functions, theese are not mean to be accessed directly
//   by developers, and instead are prototypes for the garbage collection macros.

void _free_char_vector(char_v** vec);
void _free_uchar_vector(uchar_v** vec);
void _free_short_vector(short_v** vec);
void _free_ushort_vector(ushort_v** vec);
void _free_int_vector(int_v** vec);
void _free_uint_vector(uint_v** vec);
void _free_long_vector(long_v** vec);
void _free_ulong_vector(ulong_v** vec);
void _free_llong_vector(llong_v** vec);
void _free_ullong_vector(ullong_v** vec);
void _free_float_vector(float_v** vec);
void _free_double_vector(double_v** vec);
void _free_ldouble_vector(ldouble_v** vec);
void _free_bool_vector(bool_v** vec);
void _free_sstring_vector(string_v** vec);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined(__clang__)
#define gbc_char_v __attribute__((cleanup(_free_char_vector)))
#define gbc_uchar_v __attribute__((cleanup(_free_uchar_vector)))
#define gbc_short_v __attribute__((cleanup(_free_short_vector)))
#define gbc_ushort_v __attribute__((cleanup(_free_ushort_vector)))
#define gbc_int_v __attribute__((cleanup(_free_int_vector)))
#define gbc_uint_v __attribute__((cleanup(_free_uint_vector)))
#define gbc_long_v __attribute__((cleanup(_free_long_vector)))
#define gbc_ulong_v __attribute__((cleanup(_free_ulong_vector)))
#define gbc_llong_v __attribute__((cleanup(_free_llong_vector)))
#define gbc_ullong_v __attribute__((cleanup(_free_ullong_vector)))
#define gbc_float_v __attribute__((cleanup(_free_float_vector)))
#define gbc_double_v __attribute__((cleanup(_free_double_vector)))
#define gbc_ldouble_v __attribute__((cleanup(_free_ldouble_vector)))
#define gbc_bool_v __attribute__((cleanup(_free_bool_vector)))
#define gbc_string_v __attribute__((cleanup(_free_sstring_vector)))
#endif
// ================================================================================
// ================================================================================
// PUSH_VECTOR FUNCTIONS 

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert char data as an 8-bit integer representation
 * into dynamically allocated vector, to a user specified location.
 *
 * @param vec A char_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise.
 */
bool push_char_vector(char_v* vec, char var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert unsigned char data as an 8-bit integer 
 * representation into dynamically allocated vector, to a user specified location.
 *
 * @param vec A uchar_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_uchar_vector(uchar_v* vec, unsigned char var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert short int data into dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A short_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_short_vector(short_v* vec, short int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert unsigned short int data into dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A ushort_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_ushort_vector(ushort_v* vec, unsigned short int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert int data into dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A int_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_int_vector(int_v* vec, int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert unsigned int data into dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A uint_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_uint_vector(uint_v* vec, unsigned int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert long int data into dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A long_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_long_vector(long_v* vec, long int var, size_t index);

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert unsigned long int data into dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A ulong_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_ulong_vector(ulong_v* vec, unsigned long int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert long long int data into dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A llong_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_llong_vector(llong_v* vec, long long int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert unsigned long long int data into dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A ullong_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_ullong_vector(ullong_v* vec, unsigned long long int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert float data into dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A float_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_float_vector(float_v* vec, float var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert double data into dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A double_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_double_vector(double_v* vec, double var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert ldouble data into dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A ldouble_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_ldouble_vector(ldouble_v* vec, long double var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert bool data into dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A bool_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_bool_vector(bool_v* vec, bool var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector.
 *
 * This function allows a user to insert string literal data into dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A string_v struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_string_vector(string_v* vec, const char* var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vectr. 
 *
 * This function allows a user to insert a str data type into a dynamically 
 * allocated vector, to a user specified location.
 *
 * @param vec A string_v vstruct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise.
 */
bool push_str_vector(string_v* vec, str* var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated vector 
 *
 * This function allows a user to insert data of any type into 
 * dynamically allocate vector, to a user specified location.
 *
 * @param vec A vector struct.
 * @param var The variable being inserted.  Must conform to the data dtype of vec.
 * @param index The index where data is being inserted.
 * @return true of the function executes succesfully, false otherwise.
 */
#define push_vector(vec, var,index) _Generic((vec), \
    char_v*: push_char_vector, \
    uchar_v*: push_uchar_vector, \
    short_v*: push_short_vector, \
    ushort_v*: push_ushort_vector, \
    int_v*: push_int_vector, \
    uint_v*: push_uint_vector, \
    long_v*: push_long_vector, \
    ulong_v*: push_ulong_vector, \
    llong_v*: push_llong_vector, \
    ullong_v*: push_ullong_vector, \
    float_v*: push_float_vector, \
    double_v*: push_double_vector, \
    ldouble_v*: push_ldouble_vector, \
    bool_v*: push_bool_vector, \
    default: push_string_vector) (vec, var, index)
// ================================================================================
// ================================================================================
// GET_VECTOR FUNCTIONS

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type char_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A char value as in 8 bit integer on 64 bit machine or 4 bit integer on 32 bit machine.
 */
char get_char_vector(char_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type uchar_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A unsgined char value as in 8 bit integer on 64 bit machine or 4 bit integer on 32 bit machine.
 */
unsigned char get_uchar_vector(uchar_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type short_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A short int value
 */
short int get_short_vector(short_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type ushort_v 
 * @param index The index location for the retrieved data in the vector 
 * @return An unsigned short int value
 */
unsigned short int get_ushort_vector(ushort_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type int_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A int value
 */
int get_int_vector(int_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type uint_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A unsigned int value
 */
unsigned int get_uint_vector(uint_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type long_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A long int value
 */
long int get_long_vector(long_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type ulong_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A unsigned long int value
 */
unsigned long int get_ulong_vector(ulong_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type llong_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A long long int value
 */
long long int get_llong_vector(llong_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type ullong_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A unsigned long long int value
 */
unsigned long long int get_ullong_vector(ullong_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type float_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A float value
 */
float get_float_vector(float_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type double_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A double value
 */
double get_double_vector(double_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type ldouble_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A long double value
 */
long double get_ldouble_vector(ldouble_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type bool_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A bool value
 */
bool get_bool_vector(bool_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type string_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A char* value
 */
char* get_string_vector(string_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct
 * @param index The index location for the retrieved data in the vector 
 * @return return value
 */
#define get_vector(vec, index) _Generic((vec), \
    char_v*: get_char_vector, \
    uchar_v*: get_uchar_vector, \
    short_v*: get_short_vector, \
    ushort_v*: get_ushort_vector, \
    int_v*: get_int_vector, \
    uint_v*: get_uint_vector, \
    long_v*: get_long_vector, \
    ulong_v*: get_ulong_vector, \
    llong_v*: get_llong_vector, \
    ullong_v*: get_ullong_vector, \
    float_v*: get_float_vector, \
    double_v*: get_double_vector, \
    ldouble_v*: get_ldouble_vector, \
    bool_v*: get_bool_vector, \
    string_v*: get_string_vector) (vec, index)
// ================================================================================
// ================================================================================
// VECTOR_LENGTH FUNCTIONS 

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type char_v 
 * @returns Length of the vector
 */
size_t char_vector_length(char_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type uchar_v 
 * @returns Length of the vector
 */
size_t uchar_vector_length(uchar_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type short_v 
 * @returns Length of the vector
 */
size_t short_vector_length(short_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type ushort_v 
 * @returns Length of the vector
 */
size_t ushort_vector_length(ushort_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type int_v 
 * @returns Length of the vector
 */
size_t int_vector_length(int_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type uint_v 
 * @returns Length of the vector
 */
size_t uint_vector_length(uint_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type long_v 
 * @returns Length of the vector
 */
size_t long_vector_length(long_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type ulong_v 
 * @returns Length of the vector
 */
size_t ulong_vector_length(ulong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type llong_v 
 * @returns Length of the vector
 */
size_t llong_vector_length(llong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type ullong_v 
 * @returns Length of the vector
 */
size_t ullong_vector_length(ullong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type float_v 
 * @returns Length of the vector
 */
size_t float_vector_length(float_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type double_v 
 * @returns Length of the vector
 */
size_t double_vector_length(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type ldouble_v 
 * @returns Length of the vector
 */
size_t ldouble_vector_length(ldouble_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type bool_v 
 * @returns Length of the vector
 */
size_t bool_vector_length(bool_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the vector length 
 *
 * @param vac A vector struct of type string_v 
 * @returns Length of the vector
 */
size_t string_vector_length(string_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Macro to return the length of a vector to the user 
 *
 * @param vec A vector struct
 * @return The length of a vector
 */
#define vector_length(vec) _Generic((vec), \
    char_v*: char_vector_length, \
    uchar_v*: uchar_vector_length, \
    short_v*: short_vector_length, \
    ushort_v*: ushort_vector_length, \
    int_v*: int_vector_length, \
    uint_v*: uint_vector_length, \
    long_v*: long_vector_length, \
    ulong_v*: ulong_vector_length, \
    llong_v*: llong_vector_length, \
    ullong_v*: ullong_vector_length, \
    float_v*: float_vector_length, \
    double_v*: double_vector_length, \
    ldouble_v*: ldouble_vector_length, \
    bool_v*: bool_vector_length, \
    string_v*: string_vector_length) (vec)
// ================================================================================
// ================================================================================
// VECTOR_MEMORY FUNCTIONS 

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type char_v 
 * @return The memory allocated in units of indices
 */
size_t char_vector_memory(char_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type uchar_v 
 * @return The memory allocated in units of indices
 */
size_t uchar_vector_memory(uchar_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type short_v 
 * @return The memory allocated in units of indices
 */
size_t short_vector_memory(short_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type ushort_v 
 * @return The memory allocated in units of indices
 */
size_t ushort_vector_memory(ushort_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type int_v 
 * @return The memory allocated in units of indices
 */
size_t int_vector_memory(int_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type uint_v 
 * @return The memory allocated in units of indices
 */
size_t uint_vector_memory(uint_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type long_v 
 * @return The memory allocated in units of indices
 */
size_t long_vector_memory(long_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type ulong_v 
 * @return The memory allocated in units of indices
 */
size_t ulong_vector_memory(ulong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type llong_v 
 * @return The memory allocated in units of indices
 */
size_t llong_vector_memory(llong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type ullong_v 
 * @return The memory allocated in units of indices
 */
size_t ullong_vector_memory(ullong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type float_v 
 * @return The memory allocated in units of indices
 */
size_t float_vector_memory(float_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type double_v 
 * @return The memory allocated in units of indices
 */
size_t double_vector_memory(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type ldouble_v 
 * @return The memory allocated in units of indices
 */
size_t ldouble_vector_memory(ldouble_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type bool_v 
 * @return The memory allocated in units of indices
 */
size_t bool_vector_memory(bool_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct of type string_v 
 * @return The memory allocated in units of indices
 */
size_t string_vector_memory(string_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a vector in number of indices 
 *
 * @param vec A vector struct 
 * @return The allocated memory in units of indices 
 */
#define vector_memory(vec) _Generic((vec), \
    char_v*: char_vector_memory, \
    uchar_v*: uchar_vector_memory, \
    short_v*: short_vector_memory, \
    ushort_v*: ushort_vector_memory, \
    int_v*: int_vector_memory, \
    uint_v*: uint_vector_memory, \
    long_v*: long_vector_memory, \
    ulong_v*: ulong_vector_memory, \
    llong_v*: llong_vector_memory, \
    ullong_v*: ullong_vector_memory, \
    float_v*: float_vector_memory, \
    double_v*: double_vector_memory, \
    ldouble_v*: ldouble_vector_memory, \
    bool_v*: bool_vector_memory, \
    string_v*: string_vector_memory) (vec)
// ================================================================================
// ================================================================================
// POP VECTOR FUNCTIONS

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a char_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type char_v 
 * @param index An index in the vector where data will be removed.
 * @return A char value
 */
char pop_char_vector(char_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a uchar_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type uchar_v 
 * @param index An index in the vector where data will be removed.
 * @return A unsigned char value
 */
unsigned char pop_uchar_vector(uchar_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a short_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type short_v 
 * @param index An index in the vector where data will be removed.
 * @return A short int value
 */
short int pop_short_vector(short_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a ushort_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type ushort_v 
 * @param index An index in the vector where data will be removed.
 * @return A short int value
 */
unsigned short int pop_ushort_vector(ushort_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a int_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type int_v 
 * @param index An index in the vector where data will be removed.
 * @return A unsigned int value
 */
int pop_int_vector(int_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a uint_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type uint_v 
 * @param index An index in the vector where data will be removed.
 * @return A unsigned int value
 */
unsigned int pop_uint_vector(uint_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a long_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type long_v 
 * @param index An index in the vector where data will be removed.
 * @return A long int value
 */
long int pop_long_vector(long_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a ulong_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type ulong_v 
 * @param index An index in the vector where data will be removed.
 * @return A unsigned long int value
 */
unsigned long int pop_ulong_vector(ulong_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a llong_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type llong_v 
 * @param index An index in the vector where data will be removed.
 * @return A long long int value
 */
long long int pop_llong_vector(llong_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a ullong_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type ullong_v 
 * @param index An index in the vector where data will be removed.
 * @return A unsigned long long value
 */
unsigned long long int pop_ullong_vector(ullong_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a float_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type float_v 
 * @param index An index in the vector where data will be removed.
 * @return A float value
 */
float pop_float_vector(float_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a double_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type double_v 
 * @param index An index in the vector where data will be removed.
 * @return A double value
 */
double pop_double_vector(double_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a ldouble_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type ldouble_v 
 * @param index An index in the vector where data will be removed.
 * @return A long double value
 */
long double pop_ldouble_vector(ldouble_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a bool_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type bool_v 
 * @param index An index in the vector where data will be removed.
 * @return A bool value
 */
bool pop_bool_vector(bool_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a string_v vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector of type string_v 
 * @param index An index in the vector where data will be removed.
 * @return A str* double value
 */
str* pop_string_vector(string_v* vec, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a vector and returns it 
 *
 * This program pops a value from a vector and returns it to the user 
 *
 * @param vec A dynamically allocated vector
 * @param index An index in the vector where data will be removed.
 * @return a value of type consistent with vec
 */
#define pop_vector(vec, index) _Generic((vec), \
    char_v*: pop_char_vector, \
    uchar_v*: pop_uchar_vector, \
    short_v*: pop_short_vector, \
    ushort_v*: pop_ushort_vector, \
    int_v*: pop_int_vector, \
    uint_v*: pop_uint_vector, \
    long_v*: pop_long_vector, \
    ulong_v*: pop_ulong_vector, \
    llong_v*: pop_llong_vector, \
    ullong_v*: pop_ullong_vector, \
    float_v*: pop_float_vector, \
    double_v*: pop_double_vector, \
    ldouble_v*: pop_ldouble_vector, \
    bool_v*: pop_bool_vector, \
    string_v*: pop_string_vector) (vec, index)
// ================================================================================
// ================================================================================
// REVERSE FUNCTIONS 

/**
 * @brief Reverse the order of a char_v vector 
 * @param vec A vector of type char_v
 */
void reverse_char_vector(char_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a uchar_v vector 
 * @param vec A vector of type uchar_v
 */
void reverse_uchar_vector(uchar_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a short_v vector 
 * @param vec A vector of type short_v
 */
void reverse_short_vector(short_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a ushort_v vector 
 * @param vec A vector of type ushort_v
 */
void reverse_ushort_vector(ushort_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a int_v vector 
 * @param vec A vector of type int_v
 */
void reverse_int_vector(int_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a uint_v vector 
 * @param vec A vector of type uint_v
 */
void reverse_uint_vector(uint_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a long_v vector 
 * @param vec A vector of type long_v
 */
void reverse_long_vector(long_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a ulong_v vector 
 * @param vec A vector of type ulong_v
 */
void reverse_ulong_vector(ulong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a llong_v vector 
 * @param vec A vector of type llong_v
 */
void reverse_llong_vector(llong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a ullong_v vector 
 * @param vec A vector of type ullong_v
 */
void reverse_ullong_vector(ullong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of float_v vector 
 * @param vec A vector of type float_v
 */
void reverse_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a double_v vector 
 * @param vec A vector of type double_v
 */
void reverse_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a ldouble_v vector 
 * @param vec A vector of type ldouble_v
 */
void reverse_ldouble_vector(ldouble_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a bool_v vector 
 * @param vec A vector of type bool_v
 */
void reverse_bool_vector(bool_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a string_v vector 
 * @param vec A vector of type string_v
 */
void reverse_string_vector(string_v* vec);
// --------------------------------------------------------------------------------

#define reverse_vector(vec) _Generic((vec), \
    char_v*: reverse_char_vector, \
    uchar_v*: reverse_uchar_vector, \
    short_v*: reverse_short_vector, \
    ushort_v*: reverse_ushort_vector, \
    int_v*: reverse_int_vector, \
    uint_v*: reverse_uint_vector, \
    long_v*: reverse_long_vector, \
    ulong_v*: reverse_ulong_vector, \
    llong_v*: reverse_llong_vector, \
    ullong_v*: reverse_ullong_vector, \
    float_v*: reverse_float_vector, \
    double_v*: reverse_double_vector, \
    ldouble_v*: reverse_ldouble_vector, \
    bool_v*: reverse_bool_vector, \
    string_v*: reverse_string_vector) (vec)
// ================================================================================
// ================================================================================
// MAX VECTOR FUNCTIONS 

/**
 * @brief Detrmines the maximum value in a vector 
 *
 * @param vec A vector data structure of type char_v.
 * @return The maximum value in the vector.
 */
char max_char_vector(char_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a vector 
 *
 * @param vec A vector data structure of type uchar_v.
 * @return The maximum value in the vector.
 */
unsigned char max_uchar_vector(uchar_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a vector 
 *
 * @param vec A vector data structure of type short_v.
 * @return The maximum value in the vector.
 */
short int max_short_vector(short_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a vector 
 *
 * @param vec A vector data structure of type ushort_v.
 * @return The maximum value in the vector.
 */
unsigned short int max_ushort_vector(ushort_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a vector 
 *
 * @param vec A vector data structure of type int_v.
 * @return The maximum value in the vector.
 */
int max_int_vector(int_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a vector 
 *
 * @param vec A vector data structure of type uint_v.
 * @return The maximum value in the vector.
 */
unsigned int max_uint_vector(uint_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a vector 
 *
 * @param vec A vector data structure of type long_v.
 * @return The maximum value in the vector.
 */
long int max_long_vector(long_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a vector 
 *
 * @param vec A vector data structure of type ulong_v.
 * @return The maximum value in the vector.
 */
unsigned long int max_ulong_vector(ulong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a vector 
 *
 * @param vec A vector data structure of type llong_v.
 * @return The maximum value in the vector.
 */
long long int max_llong_vector(llong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a vector 
 *
 * @param vec A vector data structure of type ullong_v.
 * @return The maximum value in the vector.
 */
unsigned long long int max_ullong_vector(ullong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a vector 
 *
 * @param vec A vector data structure of type float_v.
 * @return The maximum value in the vector.
 */
float max_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a vector 
 *
 * @param vec A vector data structure of type double_v.
 * @return The maximum value in the vector.
 */
double max_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a vector 
 *
 * @param vec A vector data structure of type ldouble_v.
 * @return The maximum value in the vector.
 */
long double max_ldouble_vector(ldouble_v* vec);
// --------------------------------------------------------------------------------

#define max_vector(vec) _Generic((vec), \
    char_v*: max_char_vector, \
    uchar_v*: max_uchar_vector, \
    short_v*: max_short_vector, \
    ushort_v*: max_ushort_vector, \
    int_v*: max_int_vector, \
    uint_v*: max_uint_vector, \
    long_v*: max_long_vector, \
    ulong_v*: max_ulong_vector, \
    llong_v*: max_llong_vector, \
    ullong_v*: max_ullong_vector, \
    float_v*: max_float_vector, \
    double_v*: max_double_vector, \
    ldouble_v*: max_ldouble_vector) (vec)
// ================================================================================
// ================================================================================
// MIN VECTOR FUNCTIONS 

/**
 * @brief Detrmines the minimum value in a vector 
 *
 * @param vec A vector data structure of type char_v.
 * @return The minimum value in the vector.
 */
char min_char_vector(char_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a vector 
 *
 * @param vec A vector data structure of type uchar_v.
 * @return The minimum value in the vector.
 */
unsigned char min_uchar_vector(uchar_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a vector 
 *
 * @param vec A vector data structure of type short_v.
 * @return The maximum value in the vector.
 */
short int min_short_vector(short_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a vector 
 *
 * @param vec A vector data structure of type ushort_v.
 * @return The minimum value in the vector.
 */
unsigned short int min_ushort_vector(ushort_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a vector 
 *
 * @param vec A vector data structure of type int_v.
 * @return The minimum value in the vector.
 */
int min_int_vector(int_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a vector 
 *
 * @param vec A vector data structure of type uint_v.
 * @return The minimum value in the vector.
 */
unsigned int min_uint_vector(uint_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a vector 
 *
 * @param vec A vector data structure of type long_v.
 * @return The minimum value in the vector.
 */
long int min_long_vector(long_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a vector 
 *
 * @param vec A vector data structure of type ulong_v.
 * @return The minimum value in the vector.
 */
unsigned long int min_ulong_vector(ulong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a vector 
 *
 * @param vec A vector data structure of type llong_v.
 * @return The minimum value in the vector.
 */
long long int min_llong_vector(llong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a vector 
 *
 * @param vec A vector data structure of type ullong_v.
 * @return The minimum value in the vector.
 */
unsigned long long int min_ullong_vector(ullong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a vector 
 *
 * @param vec A vector data structure of type float_v.
 * @return The minimum value in the vector.
 */
float min_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a vector 
 *
 * @param vec A vector data structure of type double_v.
 * @return The minimum value in the vector.
 */
double min_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a vector 
 *
 * @param vec A vector data structure of type ldouble_v.
 * @return The minimum value in the vector.
 */
long double min_ldouble_vector(ldouble_v* vec);
// --------------------------------------------------------------------------------

#define min_vector(vec) _Generic((vec), \
    char_v*: min_char_vector, \
    uchar_v*: min_uchar_vector, \
    short_v*: min_short_vector, \
    ushort_v*: min_ushort_vector, \
    int_v*: min_int_vector, \
    uint_v*: min_uint_vector, \
    long_v*: min_long_vector, \
    ulong_v*: min_ulong_vector, \
    llong_v*: min_llong_vector, \
    ullong_v*: min_ullong_vector, \
    float_v*: min_float_vector, \
    double_v*: min_double_vector, \
    ldouble_v*: min_ldouble_vector) (vec)
// ================================================================================
// ================================================================================
// SUM VECTOR FUNCTIONS 

/**
 * @brief Returns the summation of all values in a vector.
 *
 * @param vec A vector data structure of type char_v 
 * @return The sum of all values in a vector.
 */
char sum_char_vector(char_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the summation of all values in a vector.
 *
 * @param vec A vector data structure of type uchar_v 
 * @return The sum of all values in a vector.
 */
unsigned char sum_uchar_vector(uchar_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the summation of all values in a vector.
 *
 * @param vec A vector data structure of type short_v 
 * @return The sum of all values in a vector.
 */
short int sum_short_vector(short_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the summation of all values in a vector.
 *
 * @param vec A vector data structure of type ushort_v 
 * @return The sum of all values in a vector.
 */
unsigned short int sum_ushort_vector(ushort_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the summation of all values in a vector.
 *
 * @param vec A vector data structure of type int_v 
 * @return The sum of all values in a vector.
 */
int sum_int_vector(int_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the summation of all values in a vector.
 *
 * @param vec A vector data structure of type uint_v 
 * @return The sum of all values in a vector.
 */
unsigned int sum_uint_vector(uint_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the summation of all values in a vector.
 *
 * @param vec A vector data structure of type long_v 
 * @return The sum of all values in a vector.
 */
long int sum_long_vector(long_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the summation of all values in a vector.
 *
 * @param vec A vector data structure of type ulong_v 
 * @return The sum of all values in a vector.
 */
unsigned long int sum_ulong_vector(ulong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the summation of all values in a vector.
 *
 * @param vec A vector data structure of type llong_v 
 * @return The sum of all values in a vector.
 */
long long int sum_llong_vector(llong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the summation of all values in a vector.
 *
 * @param vec A vector data structure of type ullong_v 
 * @return The sum of all values in a vector.
 */
unsigned long long int sum_ullong_vector(ullong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the summation of all values in a vector.
 *
 * @param vec A vector data structure of type float_v 
 * @return The sum of all values in a vector.
 */
float sum_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the summation of all values in a vector.
 *
 * @param vec A vector data structure of type double_v 
 * @return The sum of all values in a vector.
 */
double sum_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the summation of all values in a vector.
 *
 * @param vec A vector data structure of type ldouble_v 
 * @return The sum of all values in a vector.
 */
long double sum_ldouble_vector(ldouble_v* vec);
// --------------------------------------------------------------------------------

#define sum_vector(vec) _Generic((vec), \
    char_v*: sum_char_vector, \
    uchar_v*: sum_uchar_vector, \
    short_v*: sum_short_vector, \
    ushort_v*: sum_ushort_vector, \
    int_v*: sum_int_vector, \
    uint_v*: sum_uint_vector, \
    long_v*: sum_long_vector, \
    ulong_v*: sum_ulong_vector, \
    llong_v*: sum_llong_vector, \
    ullong_v*: sum_ullong_vector, \
    float_v*: sum_float_vector, \
    double_v*: sum_double_vector, \
    ldouble_v*: sum_ldouble_vector) (vec)
// ================================================================================
// ================================================================================
// AVERAGE VECTOR FUNCTIONS 

/**
 * @brief Returns the average value for the data in a vector.
 *
 * @param vec A vector of type char_v 
 * @return The average value of type float
 */
float average_char_vector(char_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the average value for the data in a vector.
 *
 * @param vec A vector of type uchar_v 
 * @return The average value of type float
 */
float average_uchar_vector(uchar_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the average value for the data in a vector.
 *
 * @param vec A vector of type short_v 
 * @return The average value of type float
 */
float average_short_vector(short_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the average value for the data in a vector.
 *
 * @param vec A vector of type ushort_v 
 * @return The average value of type float
 */
float average_ushort_vector(ushort_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the average value for the data in a vector.
 *
 * @param vec A vector of type int_v 
 * @return The average value of type double
 */
double average_int_vector(int_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the average value for the data in a vector.
 *
 * @param vec A vector of type uint_v 
 * @return The average value of type double
 */
double average_uint_vector(uint_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the average value for the data in a vector.
 *
 * @param vec A vector of type long_v 
 * @return The average value of type double
 */
double average_long_vector(long_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the average value for the data in a vector.
 *
 * @param vec A vector of type ulong_v 
 * @return The average value of type double
 */
double average_ulong_vector(ulong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the average value for the data in a vector.
 *
 * @param vec A vector of type llong_v 
 * @return The average value of type long double
 */
long double average_llong_vector(llong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the average value for the data in a vector.
 *
 * @param vec A vector of type ullong_v 
 * @return The average value of type long double
 */
long double average_ullong_vector(ullong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the average value for the data in a vector.
 *
 * @param vec A vector of type float_v 
 * @return The average value of type float
 */
float average_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the average value for the data in a vector.
 *
 * @param vec A vector of type double_v 
 * @return The average value of type double
 */
double average_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the average value for the data in a vector.
 *
 * @param vec A vector of type ldouble_v 
 * @return The average value of type long double
 */
long double average_ldouble_vector(ldouble_v* vec);
// --------------------------------------------------------------------------------

#define vector_average(vec) _Generic((vec), \
    char_v*: average_char_vector, \
    uchar_v*: average_uchar_vector, \
    short_v*: average_short_vector, \
    ushort_v*: average_ushort_vector, \
    int_v*: average_int_vector, \
    uint_v*: average_uint_vector, \
    long_v*: average_long_vector, \
    ulong_v*: average_ulong_vector, \
    llong_v*: average_llong_vector, \
    ullong_v*: average_ullong_vector, \
    float_v*: average_float_vector, \
    double_v*: average_double_vector, \
    ldouble_v*: average_ldouble_vector) (vec)
// ================================================================================
// ================================================================================
// VECTOR STDEV FUNCTIONS 

/**
 * @brief Returns the standard deviation of the values in a vector.
 *
 * @param A vector of type char_v.
 * @return The standard deviation as a float type.
 */
float stdev_char_vector(char_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the standard deviation of the values in a vector.
 *
 * @param A vector of type uchar_v.
 * @return The standard deviation as a float type.
 */
float stdev_uchar_vector(uchar_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the standard deviation of the values in a vector.
 *
 * @param A vector of type short_v.
 * @return The standard deviation as a float type.
 */
float stdev_short_vector(short_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the standard deviation of the values in a vector.
 *
 * @param A vector of type ushort_v.
 * @return The standard deviation as a float type.
 */
float stdev_ushort_vector(ushort_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the standard deviation of the values in a vector.
 *
 * @param A vector of type int_v.
 * @return The standard deviation as a double type.
 */
double stdev_int_vector(int_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the standard deviation of the values in a vector.
 *
 * @param A vector of type uint_v.
 * @return The standard deviation as a double type.
 */
double stdev_uint_vector(uint_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the standard deviation of the values in a vector.
 *
 * @param A vector of type long_v.
 * @return The standard deviation as a double type.
 */
double stdev_long_vector(long_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the standard deviation of the values in a vector.
 *
 * @param A vector of type ulong_v.
 * @return The standard deviation as a double type.
 */
double stdev_ulong_vector(ulong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the standard deviation of the values in a vector.
 *
 * @param A vector of type llong_v.
 * @return The standard deviation as a long double type.
 */
long double stdev_llong_vector(llong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the standard deviation of the values in a vector.
 *
 * @param A vector of type ullong_v.
 * @return The standard deviation as a long double type.
 */
long double stdev_ullong_vector(ullong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the standard deviation of the values in a vector.
 *
 * @param A vector of type float_v.
 * @return The standard deviation as a float type.
 */
float stdev_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the standard deviation of the values in a vector.
 *
 * @param A vector of type double_v.
 * @return The standard deviation as a double type.
 */
double stdev_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the standard deviation of the values in a vector.
 *
 * @param A vector of type ldouble_v.
 * @return The standard deviation as a long double type.
 */
long double stdev_ldouble_vector(ldouble_v* vec);
// --------------------------------------------------------------------------------

#define vector_stdev(vec) _Generic((vec), \
    char_v*: stdev_char_vector, \
    uchar_v*: stdev_uchar_vector, \
    short_v*: stdev_short_vector, \
    ushort_v*: stdev_ushort_vector, \
    int_v*: stdev_int_vector, \
    uint_v*: stdev_uint_vector, \
    long_v*: stdev_long_vector, \
    ulong_v*: stdev_ulong_vector, \
    llong_v*: stdev_llong_vector, \
    ullong_v*: stdev_ullong_vector, \
    float_v*: stdev_float_vector, \
    double_v*: stdev_double_vector, \
    ldouble_v*: stdev_ldouble_vector) (vec)
// ================================================================================
// ================================================================================
// CUMULATIVE SUM FUNCTIONS 

/**
 * @brief Returns a vector containing the cumulative sum of the input vectr.
 * @param A vector of type char_v 
 * @return A vector of type char_v
 */
char_v* cumsum_char_vector(char_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector containing the cumulative sum of the input vectr.
 * @param A vector of type uchar_v 
 * @return A vector of type uchar_v
 */
uchar_v* cumsum_uchar_vector(uchar_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector containing the cumulative sum of the input vectr.
 * @param A vector of type short_v 
 * @return A vector of type short_v
 */
short_v* cumsum_short_vector(short_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector containing the cumulative sum of the input vectr.
 * @param A vector of type ushort_v 
 * @return A vector of type ushort_v
 */
ushort_v* cumsum_ushort_vector(ushort_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector containing the cumulative sum of the input vectr.
 * @param A vector of type int_v 
 * @return A vector of type int_v
 */
int_v* cumsum_int_vector(int_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector containing the cumulative sum of the input vectr.
 * @param A vector of type uint_v 
 * @return A vector of type unit_v
 */
uint_v* cumsum_uint_vector(uint_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector containing the cumulative sum of the input vectr.
 * @param A vector of type long_v 
 * @return A vector of type long_v
 */
long_v* cumsum_long_vector(long_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector containing the cumulative sum of the input vectr.
 * @param A vector of type ulong_v 
 * @return A vector of type ulong_v
 */
ulong_v* cumsum_ulong_vector(ulong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector containing the cumulative sum of the input vectr.
 * @param A vector of type llong_v 
 * @return A vector of type llong_v
 */
llong_v* cumsum_llong_vector(llong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector containing the cumulative sum of the input vectr.
 * @param A vector of type ullong_v 
 * @return A vector of type ullong_v
 */
ullong_v* cumsum_ullong_vector(ullong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector containing the cumulative sum of the input vectr.
 * @param A vector of type float_v 
 * @return A vector of type float_v
 */
float_v* cumsum_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector containing the cumulative sum of the input vectr.
 * @param A vector of type double_v 
 * @return A vector of type double_v
 */
double_v* cumsum_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector containing the cumulative sum of the input vectr.
 * @param A vector of type ldouble_v 
 * @return A vector of type ldouble_v
 */
ldouble_v* cumsum_ldouble_vector(ldouble_v* vec);
// --------------------------------------------------------------------------------

#define vector_cumsum(vec) _Generic((vec), \
    char_v*: cumsum_char_vector, \
    uchar_v*: cumsum_uchar_vector, \
    short_v*: cumsum_short_vector, \
    ushort_v*: cumsum_ushort_vector, \
    int_v*: cumsum_int_vector, \
    uint_v*: cumsum_uint_vector, \
    long_v*: cumsum_long_vector, \
    ulong_v*: cumsum_ulong_vector, \
    llong_v*: cumsum_llong_vector, \
    ullong_v*: cumsum_ullong_vector, \
    float_v*: cumsum_float_vector, \
    double_v*: cumsum_double_vector, \
    ldouble_v*: cumsum_ldouble_vector) (vec)
// ================================================================================
// ================================================================================
// COPY VECTOR FUNCTIONS 

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type char_v 
 * @return A vector of type char_v
 */
char_v* copy_char_vector(char_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type uchar_v 
 * @return A vector of type uchar_v
 */
uchar_v* copy_uchar_vector(uchar_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type short_v 
 * @return A vector of type short_v
 */
short_v* copy_short_vector(short_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type ushort_v 
 * @return A vector of type ushort_v
 */
ushort_v* copy_ushort_vector(ushort_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type int_v 
 * @return A vector of type int_v
 */
int_v* copy_int_vector(int_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type uint_v 
 * @return A vector of type uint_v
 */
uint_v* copy_uint_vector(uint_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type long_v 
 * @return A vector of type long_v
 */
long_v* copy_long_vector(long_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type ulong_v 
 * @return A vector of type ulong_v
 */
ulong_v* copy_ulong_vector(ulong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type llong_v 
 * @return A vector of type llong_v
 */
llong_v* copy_llong_vector(llong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type ullong_v 
 * @return A vector of type ullong_v
 */
ullong_v* copy_ullong_vector(ullong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type float_v 
 * @return A vector of type float_v
 */
float_v* copy_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type double_v 
 * @return A vector of type double_v
 */
double_v* copy_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type ldouble_v 
 * @return A vector of type louble_v
 */
ldouble_v* copy_ldouble_vector(ldouble_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type bool_v 
 * @return A vector of type bool_v
 */
bool_v* copy_bool_vector(bool_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Returns a deep copy of a vector data structure.
 *
 * @param vec A vectory of type string_v 
 * @return A vector of type string_v
 */
string_v* copy_string_vector(string_v* vec);
// --------------------------------------------------------------------------------

#define vector_copy(vec) _Generic((vec), \
    char_v*: copy_char_vector, \
    uchar_v*: copy_uchar_vector, \
    short_v*: copy_short_vector, \
    ushort_v*: copy_ushort_vector, \
    int_v*: copy_int_vector, \
    uint_v*: copy_uint_vector, \
    long_v*: copy_long_vector, \
    ulong_v*: copy_ulong_vector, \
    llong_v*: copy_llong_vector, \
    ullong_v*: copy_ullong_vector, \
    float_v*: copy_float_vector, \
    double_v*: copy_double_vector, \
    ldouble_v*: copy_ldouble_vector, \
    bool_v*: copy_bool_vector, \
    string_v*: copy_string_vector) (vec)
// ================================================================================
// ================================================================================
//  TRIM VECTOR FUNCTIONS 

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type char_v 
 */
bool trim_char_vector(char_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type uchar_v 
 */
bool trim_uchar_vector(uchar_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type short_v 
 */
bool trim_short_vector(short_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type ushort_v 
 */
bool trim_ushort_vector(ushort_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type int_v 
 */
bool trim_int_vector(int_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type uint_v 
 */
bool trim_uint_vector(uint_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type long_v 
 */
bool trim_long_vector(long_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type ulong_v 
 */
bool trim_ulong_vector(ulong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type llong_v 
 */
bool trim_llong_vector(llong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type ullong_v 
 */
bool trim_ullong_vector(ullong_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type float_v 
 */
bool trim_float_vector(float_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type double_v 
 */
bool trim_double_vector(double_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type ldouble_v 
 */
bool trim_ldouble_vector(ldouble_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type bool_v 
 */
bool trim_bool_vector(bool_v* vec);
// --------------------------------------------------------------------------------

/**
 * @brief reduces vector memory to the minimum value necessary 
 *
 * @param vec A vector data structure of type string_v 
 */
bool trim_string_vector(string_v* vec);
// --------------------------------------------------------------------------------

#define trim_vector(vec) _Generic((vec), \
    char_v*: trim_char_vector, \
    uchar_v*: trim_uchar_vector, \
    short_v*: trim_short_vector, \
    ushort_v*: trim_ushort_vector, \
    int_v*: trim_int_vector, \
    uint_v*: trim_uint_vector, \
    long_v*: trim_long_vector, \
    ulong_v*: trim_ulong_vector, \
    llong_v*: trim_llong_vector, \
    ullong_v*: trim_ullong_vector, \
    float_v*: trim_float_vector, \
    double_v*: trim_double_vector, \
    ldouble_v*: trim_ldouble_vector, \
    bool_v*: trim_bool_vector, \
    string_v*: trim_string_vector) (vec)
// ================================================================================
// ================================================================================
// REPLACE INDEX FUNCTIONS 

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type char_v 
 * @param dat The data replacement of type char
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_char_vector_index(char_v* vec, char dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type uchar_v 
 * @param dat The data replacement of type unsgned char 
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_uchar_vector_index(uchar_v* vec, unsigned char dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type short_v 
 * @param dat The data replacement of type short int 
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_short_vector_index(short_v* vec, short int dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type ushort_v 
 * @param dat The data replacement of type unsigned short int 
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_ushort_vector_index(ushort_v* vec, unsigned short int dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type int_v 
 * @param dat The data replacement of type int 
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_int_vector_index(int_v* vec, int dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type uint_v 
 * @param dat The data replacement of type unsigned int 
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_uint_vector_index(uint_v* vec, unsigned int dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type long_v 
 * @param dat The data replacement of type long int 
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_long_vector_index(long_v* vec, long int dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type ulong_v 
 * @param dat The data replacement of type unsigned long int 
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_ulong_vector_index(ulong_v* vec, unsigned long int dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type llong_v 
 * @param dat The data replacement of type long long int 
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_llong_vector_index(llong_v* vec, long long int dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type ullong_v 
 * @param dat The data replacement of type unsigned long long int 
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_ullong_vector_index(ullong_v* vec, unsigned long long int dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type float_v 
 * @param dat The data replacement of type float 
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_float_vector_index(float_v* vec, float dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type double_v 
 * @param dat The data replacement of type double 
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_double_vector_index(double_v* vec, double dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type short_v 
 * @param dat The data replacement of type long double 
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_ldouble_vector_index(ldouble_v* vec, long double dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type ushort_v 
 * @param The data replacement of type bool 
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_bool_vector_index(bool_v* vec, bool dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type short_v 
 * @param dat The data replacement of type char*
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_string_vector_index(string_v* vec, char* dat, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Allows a usert to replace data at an index.
 *
 * @param vec A vector of type string_v
 * @param dat The data replacement of type string_v
 * @param index The index where data will be replaced 
 * @return true if function executes succesfully, false otherwise.
 */
bool replace_str_vector_index(string_v* vec, str* dat, size_t index);
// --------------------------------------------------------------------------------

#define replace_vector_index(vec, dat, index) _Generic((vec), \
    char_v*: replace_char_vector_index, \
    uchar_v*: replace_uchar_vector_index, \
    short_v*: replace_short_vector_index, \
    ushort_v*: replace_ushort_vector_index, \
    int_v*: replace_int_vector_index, \
    uint_v*: replace_uint_vector_index, \
    long_v*: replace_long_vector_index, \
    ulong_v*: replace_ulong_vector_index, \
    llong_v*: replace_llong_vector_index, \
    ullong_v*: replace_ullong_vector_index, \
    float_v*: replace_float_vector_index, \
    double_v*: replace_double_vector_index, \
    ldouble_v*: replace_ldouble_vector_index, \
    bool_v*: replace_bool_vector_index, \
    string_v*: replace_string_vector_index) (vec, dat, index)
// ================================================================================
// ================================================================================

/**
 * @brief Macro to create vector iterator structs for each data type.
 *
 * This macro creates a struct with pointers to a function that returns 
 * a pointer to the first variable in a data structure (begin), a pointer 
 * to the last variable in a data structure (en), a pointer to the next 
 * variable in a data structure (next), a pointer to the previous variable 
 * in a data structure (prev), and one that returns tha data at a position 
 * in the data structure (get)
 *
 * @param type_one The data type contained in a vector data structure.
 * @param type_two The derived data type for the vector data structure.
 */
#define VECTOR_ITERATOR(type_one, type_two) \
    typedef struct { \
        type_one* (*begin) (type_two *s); \
        type_one* (*end) (type_two *s); \
        void (*next) (type_one** current); \
        void (*prev) (type_one** current); \
        type_one (*get) (type_one** current); \
    } type_two##_iterator;
// --------------------------------------------------------------------------------

VECTOR_ITERATOR(char, char_v)
VECTOR_ITERATOR(unsigned char, uchar_v)
VECTOR_ITERATOR(short int, short_v)
VECTOR_ITERATOR(unsigned short int, ushort_v)
VECTOR_ITERATOR(int, int_v)
VECTOR_ITERATOR(unsigned int, uint_v)
VECTOR_ITERATOR(long int, long_v)
VECTOR_ITERATOR(unsigned long int, ulong_v)
VECTOR_ITERATOR(long long int, llong_v)
VECTOR_ITERATOR(unsigned long long int, ullong_v)
VECTOR_ITERATOR(float, float_v)
VECTOR_ITERATOR(double, double_v)
VECTOR_ITERATOR(long double, ldouble_v)
VECTOR_ITERATOR(bool, bool_v)

typedef struct {
    str* (*begin) (string_v *s);
    str* (*end) (string_v* s);
    void (*next) (str** current);
    void (*prev) (str** current);
    char* (*get) (str** current);
} string_v_iterator;
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type char_v vectors.
 */
char_v_iterator init_char_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type uchar_v vectors.
 */
uchar_v_iterator init_uchar_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type short_v vectors.
 */
short_v_iterator init_short_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type ushort_v vectors.
 */
ushort_v_iterator init_ushort_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type int_v vectors.
 */
int_v_iterator init_int_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type uint_v vectors.
 */
uint_v_iterator init_uint_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type long_v vectors.
 */
long_v_iterator init_long_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type ulong_v vectors.
 */
ulong_v_iterator init_ulong_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type llong_v vectors.
 */
llong_v_iterator init_llong_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type llong_v vectors.
 */
ullong_v_iterator init_ullong_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type float_v vectors.
 */
float_v_iterator init_float_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type double_v vectors.
 */
double_v_iterator init_double_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type ldouble_v vectors.
 */
ldouble_v_iterator init_ldouble_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type bool_v vectors.
 */
bool_v_iterator init_bool_vector_iterator();
// --------------------------------------------------------------------------------

/**
 * @brief Returns a vector iterator for type string_v vectors.
 */
string_v_iterator init_string_vector_iterator();
// --------------------------------------------------------------------------------

#define vector_iterator(vec) _Generic((vec), \
    char_v*: init_char_vector_iterator, \
    uchar_v*: init_uchar_vector_iterator, \
    short_v*: init_short_vector_iterator, \
    ushort_v*: init_ushort_vector_iterator, \
    int_v*: init_int_vector_iterator, \
    uint_v*: init_uint_vector_iterator, \
    long_v*: init_long_vector_iterator, \
    ulong_v*: init_ulong_vector_iterator, \
    llong_v*: init_llong_vector_iterator, \
    ullong_v*: init_ullong_vector_iterator, \
    float_v*: init_float_vector_iterator, \
    double_v*: init_double_vector_iterator, \
    ldouble_v*: init_ldouble_vector_iterator, \
    bool_v*: init_bool_vector_iterator, \
    string_v*: init_string_vector_iterator) ()
// ================================================================================
// ================================================================================
// SORT VECTOR FUNCTIONS

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type char_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_char_vector(char_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type uchar_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_uchar_vector(uchar_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type short_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_short_vector(short_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type ushort_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_ushort_vector(ushort_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type int_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_int_vector(int_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type uint_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_uint_vector(uint_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type long_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_long_vector(long_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type ulong_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_ulong_vector(ulong_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type llong_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_llong_vector(llong_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type ullong_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_ullong_vector(ullong_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type float_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_float_vector(float_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type double_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_double_vector(double_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type ldouble_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_ldouble_vector(ldouble_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type bool_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_bool_vector(bool_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param vec A vector of type string_v.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_string_vector(string_v* vec, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

#define sort_vector(vec, stype, direction) _Generic((vec), \
    char_v*: sort_char_vector, \
    uchar_v*: sort_uchar_vector, \
    short_v*: sort_short_vector, \
    ushort_v*: sort_ushort_vector, \
    int_v*: sort_int_vector, \
    uint_v*: sort_uint_vector, \
    long_v*: sort_long_vector, \
    ulong_v*: sort_ulong_vector, \
    llong_v*: sort_llong_vector, \
    ullong_v*: sort_ullong_vector, \
    float_v*: sort_float_vector, \
    double_v*: sort_double_vector, \
    ldouble_v*: sort_ldouble_vector, \
    bool_v*: sort_bool_vector, \
    string_v*: sort_string_vector) (vec, stype, direction)
// ================================================================================
// ================================================================================
// BINARY SEARCH FUNCTIONS 

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type char_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists.
 */
int bsearch_char_vector(char_v* vec, char val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type uchar_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_uchar_vector(uchar_v* vec, unsigned char val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type short_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_short_vector(short_v* vec, short int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type ushort_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists.
 */
int bsearch_ushort_vector(ushort_v* vec, unsigned short int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type int_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists.
 */
int bsearch_int_vector(int_v* vec, int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type uint_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_uint_vector(uint_v* vec, unsigned int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type long_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_long_vector(long_v* vec, long int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type ulong_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_ulong_vector(ulong_v* vec, unsigned long int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type llong_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_llong_vector(llong_v* vec, long long int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type ullong_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_ullong_vector(ullong_v* vec, unsigned long long int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type float_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_float_vector(float_v* vec, float val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type double_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_double_vector(double_v* vec, double val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type ldouble_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_ldouble_vector(ldouble_v* vec, long double val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type bool_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_bool_vector(bool_v* vec, bool val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type string_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for.
 * @return The index where a variable exists. 
 */
int bsearch_string_vector(string_v* vec, char* val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type string_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for.
 * @return The index where a variable exists. 
 */
int bsearch_string_vector(string_v* vec, char* val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param vec A vector data structure of type string_v 
 * @param sorted: true if the vector is already sorted, false otherwise.
 * @param val The value being searched for.
 * @return The index where a variable exists. 
 */
int bsearch_str_vector(string_v* vec, str* val, bool sorted);
// --------------------------------------------------------------------------------

#define bsearch_vector(vec, val, sorted) _Generic((vec), \
    char_v*: bsearch_char_vector, \
    uchar_v*: bsearch_uchar_vector, \
    short_v*: bsearch_short_vector, \
    ushort_v*: bsearch_ushort_vector, \
    int_v*: bsearch_int_vector, \
    uint_v*: bsearch_uint_vector, \
    long_v*: bsearch_long_vector, \
    ulong_v*: bsearch_ulong_vector, \
    llong_v*: bsearch_llong_vector, \
    ullong_v*: bsearch_ullong_vector, \
    float_v*: bsearch_float_vector, \
    double_v*: bsearch_double_vector, \
    ldouble_v*: bsearch_ldouble_vector, \
    bool_v*: bsearch_bool_vector, \
    string_v*: bsearch_string_vector) (vec, val, sorted)
// ================================================================================
// ================================================================================
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* vector_H */
// ================================================================================
// ================================================================================
// eof
