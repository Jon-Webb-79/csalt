// ================================================================================
// ================================================================================
// - File:    array.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    November 20, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#error "This code requires C11 or later."
#endif

#ifndef array_H
#define array_H

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
// CONSTANTS FOR VECTOR MEMORY MANAGEMENT

extern const size_t ARR_THRESHOLD;
extern const size_t ARR_FIXED_AMOUNT;
extern const size_t ARR_RUN;
// ================================================================================
// ================================================================================
// Array data type structs 

/**
 * @brief A macro to create structs for vectors of various data types.
 *
 * @param type The data type that will be assigned to the data pointer.
 * @param dat_type The name given to the typedef for the struct.
 */
#define ARRAY_STRUCT(type, dat_type) \
    typedef struct { type *data; size_t len; size_t alloc; } dat_type
// --------------------------------------------------------------------------------
// Create typedef for each possible vector type 

ARRAY_STRUCT(char, char_arr);
ARRAY_STRUCT(unsigned char, uchar_arr);
ARRAY_STRUCT(short int, short_arr);
ARRAY_STRUCT(unsigned short int, ushort_arr);
ARRAY_STRUCT(int, int_arr);
ARRAY_STRUCT(unsigned int, uint_arr);
ARRAY_STRUCT(long int, long_arr);
ARRAY_STRUCT(unsigned long int, ulong_arr);
ARRAY_STRUCT(long long int, llong_arr);
ARRAY_STRUCT(unsigned long long int, ullong_arr);
ARRAY_STRUCT(float, float_arr);
ARRAY_STRUCT(double, double_arr);
ARRAY_STRUCT(long double, ldouble_arr);
ARRAY_STRUCT(bool, bool_arr);
ARRAY_STRUCT(str, string_arr);
// ================================================================================
// ================================================================================
// Initialization functions 

/**
 * @brief typedef function pointer to support init_vector functions
 */
typedef void* (*InitArrFunc)(size_t);

/**
 * @brief Function that selects the correct vector initialization function based on type 
 *
 * @param dat_type An enum of type dtype containing a keyword defining the data type 
 * @return A vector initialization function for a specific data type.
 */
InitArrFunc init_array(dtype dat_type);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of char indices
 * @return A char_v struct 
 */
char_arr init_char_array(char* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned char indices
 * @return A uchar_arr struct 
 */
uchar_arr init_uchar_array(unsigned char* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of short int indices
 * @return A short_arr struct 
 */
short_arr init_short_array(short int* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned short int indices
 * @return A ushort_arr struct 
 */
ushort_arr init_ushort_array(unsigned short int* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of int indices
 * @return A int_arr struct 
 */
int_arr init_int_array(int* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned int indices
 * @return A uint_arr struct 
 */
uint_arr init_uint_array(unsigned int* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of long int indices
 * @return A long_arr struct 
 */
long_arr init_long_array(long int* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned long int indices
 * @return A ulong_arr struct 
 */
ulong_arr init_ulong_array(unsigned long int* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of long long int indices
 * @return A llong_arr struct 
 */
llong_arr init_llong_array(long long int* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned long long int indices
 * @return A llong_arr struct 
 */
ullong_arr init_ullong_array(unsigned long long int* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of float indices
 * @return A float_arr struct 
 */
float_arr init_float_array(float* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of double indices
 * @return A double_arr struct 
 */
double_arr init_double_array(double* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of long double indices
 * @return A ldouble_arr struct 
 */
ldouble_arr init_ldouble_array(long double* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of bool indices
 * @return A bool_arr struct 
 */
bool_arr init_bool_array(bool* arr, size_t buff, size_t len);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of str indices
 * @return A string_arr struct 
 */
string_arr init_string_array(str* arr, size_t buff, size_t len);
// ================================================================================
// ================================================================================
// PUSH_ARRAY FUNCTIONS 

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
bool push_char_array(char_arr* arr, char var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert unsigned char data as an 8-bit integer 
 * representation into dynamically allocated arrtor, to a user specified location.
 *
 * @param arr A uchar_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_uchar_array(uchar_arr* arr, unsigned char var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert short int data into dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A short_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_short_array(short_arr* arr, short int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert unsigned short int data into dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A ushort_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_ushort_array(ushort_arr* arr, unsigned short int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert int data into dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A int_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_int_array(int_arr* arr, int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert unsigned int data into dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A uint_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_uint_array(uint_arr* arr, unsigned int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert long int data into dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A long_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_long_array(long_arr* arr, long int var, size_t index);

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert unsigned long int data into dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A ulong_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_ulong_array(ulong_arr* arr, unsigned long int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert long long int data into dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A llong_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_llong_array(llong_arr* arr, long long int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert unsigned long long int data into dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A ullong_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_ullong_array(ullong_arr* arr, unsigned long long int var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert float data into dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A float_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_float_array(float_arr* arr, float var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert double data into dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A double_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_double_array(double_arr* arr, double var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert ldouble data into dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A ldouble_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_ldouble_array(ldouble_arr* arr, long double var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert bool data into dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A bool_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_bool_array(bool_arr* arr, bool var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtor.
 *
 * This function allows a user to insert string literal data into dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A string_arr struct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise. 
 */
bool push_string_array(string_arr* arr, const char* var, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Inserts data into a dynamically allocated arrtr. 
 *
 * This function allows a user to insert a str data type into a dynamically 
 * allocated arrtor, to a user specified location.
 *
 * @param arr A string_arr vstruct.
 * @param var The variable being inserted.
 * @param index The index where data is being inserted.
 * @return true if the function executes succesfully, false otherwise.
 */
bool push_str_array(string_arr* arr, str* var, size_t index);
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
#define push_array(arr, var,index) _Generic((arr), \
    char_v*: push_char_array, \
    uchar_v*: push_uchar_array, \
    short_v*: push_short_array, \
    ushort_v*: push_ushort_array, \
    int_v*: push_int_array, \
    uint_v*: push_uint_array, \
    long_v*: push_long_array, \
    ulong_v*: push_ulong_array, \
    llong_v*: push_llong_array, \
    ullong_v*: push_ullong_array, \
    float_v*: push_float_array, \
    double_v*: push_double_array, \
    ldouble_v*: push_ldouble_array, \
    bool_v*: push_bool_array, \
    default: push_string_array) (arr, var, index)
// ================================================================================
// ================================================================================
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* array_H */
// ================================================================================
// ================================================================================
// eof
