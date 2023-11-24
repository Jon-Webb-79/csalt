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
// ================================================================================
// ================================================================================
// Initialization functions 

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

#define init_array(arr, buff, len) _Generic((arr), \
    char*: init_char_array, \
    unsigned char*: init_uchar_array, \
    short int*: init_short_array, \
    unsigned short int*: init_ushort_array, \
    int*: init_int_array, \
    unsigned int*: init_uint_array, \
    long int*: init_long_array, \
    unsigned long int*: init_ulong_array, \
    long long int*: init_llong_array, \
    unsigned long long int*: init_ullong_array, \
    float*: init_float_array, \
    double*: init_double_array, \
    long double*: init_ldouble_array, \
    bool*: init_bool_array) (arr, buff, len)
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
    char_arr: push_char_array, \
    uchar_arr: push_uchar_array, \
    short_arr: push_short_array, \
    ushort_arr: push_ushort_array, \
    int_arr: push_int_array, \
    uint_arr: push_uint_array, \
    long_arr: push_long_array, \
    ulong_arr: push_ulong_array, \
    llong_arr: push_llong_array, \
    ullong_arr: push_ullong_array, \
    float_arr: push_float_array, \
    double_arr: push_double_array, \
    ldouble_arr: push_ldouble_array, \
    bool_arr: push_bool_array) (&arr, var, index)
// ================================================================================
// ================================================================================
// GET_ARRAY FUNCTIONS 

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct of type char_v 
 * @param index The index location for the retrieved data in the vector 
 * @return A char value as in 8 bit integer on 64 bit machine or 4 bit integer on 32 bit machine.
 */
char get_char_array(char_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a arrtor.
 *
 * @param arr A dynamically allocated vector struct of type uchar_arr 
 * @param index The index location for the retrieved data in the arrtor 
 * @return A unsgined char value as in 8 bit integer on 64 bit machine or 4 bit integer on 32 bit machine.
 */
unsigned char get_uchar_array(uchar_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a arrtor.
 *
 * @param arr A dynamically allocated vector struct of type short_arr 
 * @param index The index location for the retrieved data in the arrtor 
 * @return A short int value
 */
short int get_short_array(short_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a arrtor.
 *
 * @param arr A dynamically allocated vector struct of type ushort_arr 
 * @param index The index location for the retrieved data in the arrtor 
 * @return An unsigned short int value
 */
unsigned short int get_ushort_array(ushort_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a arrtor.
 *
 * @param arr A dynamically allocated vector struct of type int_arr 
 * @param index The index location for the retrieved data in the arrtor 
 * @return A int value
 */
int get_int_array(int_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a arrtor.
 *
 * @param arr A dynamically allocated vector struct of type uint_arr 
 * @param index The index location for the retrieved data in the arrtor 
 * @return A unsigned int value
 */
unsigned int get_uint_array(uint_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a arrtor.
 *
 * @param arr A dynamically allocated vector struct of type long_arr 
 * @param index The index location for the retrieved data in the arrtor 
 * @return A long int value
 */
long int get_long_array(long_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a arrtor.
 *
 * @param arr A dynamically allocated vector struct of type ulong_arr 
 * @param index The index location for the retrieved data in the arrtor 
 * @return A unsigned long int value
 */
unsigned long int get_ulong_array(ulong_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a arrtor.
 *
 * @param arr A dynamically allocated vector struct of type llong_arr 
 * @param index The index location for the retrieved data in the arrtor 
 * @return A long long int value
 */
long long int get_llong_array(llong_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a arrtor.
 *
 * @param arr A dynamically allocated vector struct of type ullong_arr 
 * @param index The index location for the retrieved data in the arrtor 
 * @return A unsigned long long int value
 */
unsigned long long int get_ullong_array(ullong_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a arrtor.
 *
 * @param arr A dynamically allocated vector struct of type float_arr 
 * @param index The index location for the retrieved data in the arrtor 
 * @return A float value
 */
float get_float_array(float_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a arrtor.
 *
 * @param arr A dynamically allocated vector struct of type double_arr 
 * @param index The index location for the retrieved data in the arrtor 
 * @return A double value
 */
double get_double_array(double_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a arrtor.
 *
 * @param arr A dynamically allocated vector struct of type ldouble_arr 
 * @param index The index location for the retrieved data in the arrtor 
 * @return A long double value
 */
long double get_ldouble_array(ldouble_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a arrtor.
 *
 * @param arr A dynamically allocated vector struct of type bool_arr 
 * @param index The index location for the retrieved data in the arrtor 
 * @return A bool value
 */
bool get_bool_array(bool_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief A function to retrieve data from a specific index in a vector.
 *
 * @param vec A dynamically allocated vector struct
 * @param index The index location for the retrieved data in the vector 
 * @return return value
 */
#define get_array(arr, index) _Generic((arr), \
    char_arr: get_char_array, \
    uchar_arr: get_uchar_array, \
    short_arr: get_short_array, \
    ushort_arr: get_ushort_array, \
    int_arr: get_int_array, \
    uint_arr: get_uint_array, \
    long_arr: get_long_array, \
    ulong_arr: get_ulong_array, \
    llong_arr: get_llong_array, \
    ullong_arr: get_ullong_array, \
    float_arr: get_float_array, \
    double_arr: get_double_array, \
    ldouble_arr: get_ldouble_array, \
    bool_arr: get_bool_array) (&arr, index)
// ================================================================================
// ================================================================================
// VECTOR LENGTH FUNCTIONS  

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type char_arr 
 * @returns Length of the array
 */
size_t char_array_length(char_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type uchar_arr 
 * @returns Length of the array
 */
size_t uchar_array_length(uchar_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type short_arr 
 * @returns Length of the array
 */
size_t short_array_length(short_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type ushort_arr 
 * @returns Length of the array
 */
size_t ushort_array_length(ushort_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type int_arr 
 * @returns Length of the array
 */
size_t int_array_length(int_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type uint_arr 
 * @returns Length of the array
 */
size_t uint_array_length(uint_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type long_arr 
 * @returns Length of the array
 */
size_t long_array_length(long_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type ulong_arr 
 * @returns Length of the array
 */
size_t ulong_array_length(ulong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type llong_arr 
 * @returns Length of the array
 */
size_t llong_array_length(llong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type ullong_arr 
 * @returns Length of the array
 */
size_t ullong_array_length(ullong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type float_arr 
 * @returns Length of the array
 */
size_t float_array_length(float_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type double_arr 
 * @returns Length of the array
 */
size_t double_array_length(double_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type ldouble_arr 
 * @returns Length of the array
 */
size_t ldouble_array_length(ldouble_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief retrieves the array length 
 *
 * @param vac A array struct of type bool_arr 
 * @returns Length of the array
 */
size_t bool_array_length(bool_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Macro to return the length of a array to the user 
 *
 * @param arr A array struct
 * @return The length of a array
 */
#define array_length(arr) _Generic((arr), \
    char_arr: char_array_length, \
    uchar_arr: uchar_array_length, \
    short_arr: short_array_length, \
    ushort_arr: ushort_array_length, \
    int_arr: int_array_length, \
    uint_arr: uint_array_length, \
    long_arr: long_array_length, \
    ulong_arr: ulong_array_length, \
    llong_arr: llong_array_length, \
    ullong_arr: ullong_array_length, \
    float_arr: float_array_length, \
    double_arr: double_array_length, \
    ldouble_arr: ldouble_array_length, \
    bool_arr: bool_array_length) (&arr)
// ================================================================================
// ================================================================================
// ARRAY MEMORY FUNCTIONS 

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type char_arr 
 * @return The memory allocated in units of indices
 */
size_t char_array_memory(char_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type uchar_arr 
 * @return The memory allocated in units of indices
 */
size_t uchar_array_memory(uchar_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type short_arr 
 * @return The memory allocated in units of indices
 */
size_t short_array_memory(short_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type ushort_arr 
 * @return The memory allocated in units of indices
 */
size_t ushort_array_memory(ushort_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type int_arr 
 * @return The memory allocated in units of indices
 */
size_t int_array_memory(int_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type uint_arr 
 * @return The memory allocated in units of indices
 */
size_t uint_array_memory(uint_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type long_arr 
 * @return The memory allocated in units of indices
 */
size_t long_array_memory(long_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type ulong_arr 
 * @return The memory allocated in units of indices
 */
size_t ulong_array_memory(ulong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type llong_arr 
 * @return The memory allocated in units of indices
 */
size_t llong_array_memory(llong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type ullong_arr 
 * @return The memory allocated in units of indices
 */
size_t ullong_array_memory(ullong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type float_arr 
 * @return The memory allocated in units of indices
 */
size_t float_array_memory(float_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type double_arr 
 * @return The memory allocated in units of indices
 */
size_t double_array_memory(double_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type ldouble_arr 
 * @return The memory allocated in units of indices
 */
size_t ldouble_array_memory(ldouble_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct of type bool_arr 
 * @return The memory allocated in units of indices
 */
size_t bool_array_memory(bool_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the memory allocated to a array in number of indices 
 *
 * @param arr A array struct 
 * @return The allocated memory in units of indices 
 */
#define array_memory(arr) _Generic((arr), \
    char_arr: char_array_memory, \
    uchar_arr: uchar_array_memory, \
    short_arr: short_array_memory, \
    ushort_arr: ushort_array_memory, \
    int_arr: int_array_memory, \
    uint_arr: uint_array_memory, \
    long_arr: long_array_memory, \
    ulong_arr: ulong_array_memory, \
    llong_arr: llong_array_memory, \
    ullong_arr: ullong_array_memory, \
    float_arr: float_array_memory, \
    double_arr: double_array_memory, \
    ldouble_arr: ldouble_array_memory, \
    bool_arr: bool_array_memory) (&arr)
// ================================================================================
// ================================================================================
// POP ARRAY FUNCTIONS 

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a char_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type char_arr 
 * @param index An index in the array where data will be removed.
 * @return A char value
 */
char pop_char_array(char_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a uchar_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type uchar_arr 
 * @param index An index in the array where data will be removed.
 * @return A unsigned char value
 */
unsigned char pop_uchar_array(uchar_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a short_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type short_arr 
 * @param index An index in the array where data will be removed.
 * @return A short int value
 */
short int pop_short_array(short_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a ushort_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type ushort_arr 
 * @param index An index in the array where data will be removed.
 * @return A short int value
 */
unsigned short int pop_ushort_array(ushort_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a int_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type int_arr 
 * @param index An index in the array where data will be removed.
 * @return A unsigned int value
 */
int pop_int_array(int_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a uint_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type uint_arr 
 * @param index An index in the array where data will be removed.
 * @return A unsigned int value
 */
unsigned int pop_uint_array(uint_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a long_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type long_arr 
 * @param index An index in the array where data will be removed.
 * @return A long int value
 */
long int pop_long_array(long_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a ulong_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type ulong_arr 
 * @param index An index in the array where data will be removed.
 * @return A unsigned long int value
 */
unsigned long int pop_ulong_array(ulong_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a llong_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type llong_arr 
 * @param index An index in the array where data will be removed.
 * @return A long long int value
 */
long long int pop_llong_array(llong_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a ullong_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type ullong_arr 
 * @param index An index in the array where data will be removed.
 * @return A unsigned long long value
 */
unsigned long long int pop_ullong_array(ullong_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a float_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type float_arr 
 * @param index An index in the array where data will be removed.
 * @return A float value
 */
float pop_float_array(float_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a double_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type double_arr 
 * @param index An index in the array where data will be removed.
 * @return A double value
 */
double pop_double_array(double_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a ldouble_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type ldouble_arr 
 * @param index An index in the array where data will be removed.
 * @return A long double value
 */
long double pop_ldouble_array(ldouble_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a bool_arr array and returns it to the user 
 *
 * @param arr A dynamically allocated array of type bool_arr 
 * @param index An index in the array where data will be removed.
 * @return A bool value
 */
bool pop_bool_array(bool_arr* arr, size_t index);
// --------------------------------------------------------------------------------

/**
 * @brief Pops a value from a user defined index in a array and returns it 
 *
 * This program pops a value from a array and returns it to the user 
 *
 * @param arr A dynamically allocated array
 * @param index An index in the array where data will be removed.
 * @return a value of type consistent with arr
 */
#define pop_array(arr, index) _Generic((arr), \
    char_arr: pop_char_array, \
    uchar_arr: pop_uchar_array, \
    short_arr: pop_short_array, \
    ushort_arr: pop_ushort_array, \
    int_arr: pop_int_array, \
    uint_arr: pop_uint_array, \
    long_arr: pop_long_array, \
    ulong_arr: pop_ulong_array, \
    llong_arr: pop_llong_array, \
    ullong_arr: pop_ullong_array, \
    float_arr: pop_float_array, \
    double_arr: pop_double_array, \
    ldouble_arr: pop_ldouble_array, \
    bool_arr: pop_bool_array) (&arr, index)
// ================================================================================
// ================================================================================

/**
 * @brief Reverse the order of a char_arr array 
 * @param arr A array of type char_arr
 */
void reverse_char_array(char_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a uchar_arr array 
 * @param arr A array of type uchar_arr
 */
void reverse_uchar_array(uchar_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a short_arr array 
 * @param arr A array of type short_arr
 */
void reverse_short_array(short_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a ushort_arr array 
 * @param arr A array of type ushort_arr
 */
void reverse_ushort_array(ushort_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a int_arr array 
 * @param arr A array of type int_arr
 */
void reverse_int_array(int_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a uint_arr array 
 * @param arr A array of type uint_arr
 */
void reverse_uint_array(uint_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a long_arr array 
 * @param arr A array of type long_arr
 */
void reverse_long_array(long_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a ulong_arr array 
 * @param arr A array of type ulong_arr
 */
void reverse_ulong_array(ulong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a llong_arr array 
 * @param arr A array of type llong_arr
 */
void reverse_llong_array(llong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a ullong_arr array 
 * @param arr A array of type ullong_arr
 */
void reverse_ullong_array(ullong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of float_arr array 
 * @param arr A array of type float_arr
 */
void reverse_float_array(float_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a double_arr array 
 * @param arr A array of type double_arr
 */
void reverse_double_array(double_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a ldouble_arr array 
 * @param arr A array of type ldouble_arr
 */
void reverse_ldouble_array(ldouble_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Reverse the order of a bool_arr array 
 * @param arr A array of type bool_arr
 */
void reverse_bool_array(bool_arr* arr);
// --------------------------------------------------------------------------------

#define reverse_array(arr) _Generic((arr), \
    char_arr: reverse_char_array, \
    uchar_arr: reverse_uchar_array, \
    short_arr: reverse_short_array, \
    ushort_arr: reverse_ushort_array, \
    int_arr: reverse_int_array, \
    uint_arr: reverse_uint_array, \
    long_arr: reverse_long_array, \
    ulong_arr: reverse_ulong_array, \
    llong_arr: reverse_llong_array, \
    ullong_arr: reverse_ullong_array, \
    float_arr: reverse_float_array, \
    double_arr: reverse_double_array, \
    ldouble_arr: reverse_ldouble_array, \
    bool_arr: reverse_bool_array) (&arr)
// ================================================================================
// ================================================================================
// SORT ARRAY FUNCTIONS

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type char_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_char_array(char_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type uchar_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_uchar_array(uchar_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type short_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_short_array(short_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type ushort_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_ushort_array(ushort_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type int_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_int_array(int_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type uint_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_uint_array(uint_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type long_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_long_array(long_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type ulong_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_ulong_array(ulong_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type llong_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_llong_array(llong_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type ullong_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_ullong_array(ullong_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type float_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_float_array(float_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type double_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_double_array(double_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type ldouble_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_ldouble_array(ldouble_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

/**
 * @brief Function to sort data in forward or reverse order 
 *
 * This function allows users to select the appropriate sorting algorithm and 
 * direction for their data.
 *
 * @param arr A array of type bool_arr.
 * @param sort_type An enum that can be BUBBLE, INSERT, MERGE, QUICK, or TIM.
 * @param direction An enum that can be FORWARD or REVERSE.
 */
void sort_bool_array(bool_arr* arr, sort_type stype, iter_dir direction);
// --------------------------------------------------------------------------------

#define sort_array(arr, stype, direction) _Generic((arr), \
    char_arr: sort_char_array, \
    uchar_arr: sort_uchar_array, \
    short_arr: sort_short_array, \
    ushort_arr: sort_ushort_array, \
    int_arr: sort_int_array, \
    uint_arr: sort_uint_array, \
    long_arr: sort_long_array, \
    ulong_arr: sort_ulong_array, \
    llong_arr: sort_llong_array, \
    ullong_arr: sort_ullong_array, \
    float_arr: sort_float_array, \
    double_arr: sort_double_array, \
    ldouble_arr: sort_ldouble_array, \
    bool_arr: sort_bool_array) (&arr, stype, direction)
// ================================================================================
// ================================================================================
// MAX ARRAY FUNCTIONS 

/**
 * @brief Detrmines the maximum value in a array 
 *
 * @param arr A array data structure of type char_arr.
 * @return The maximum value in the array.
 */
char max_char_array(char_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a array 
 *
 * @param arr A array data structure of type uchar_arr.
 * @return The maximum value in the array.
 */
unsigned char max_uchar_array(uchar_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a array 
 *
 * @param arr A array data structure of type short_arr.
 * @return The maximum value in the array.
 */
short int max_short_array(short_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a array 
 *
 * @param arr A array data structure of type ushort_arr.
 * @return The maximum value in the array.
 */
unsigned short int max_ushort_array(ushort_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a array 
 *
 * @param arr A array data structure of type int_arr.
 * @return The maximum value in the array.
 */
int max_int_array(int_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a array 
 *
 * @param arr A array data structure of type uint_arr.
 * @return The maximum value in the array.
 */
unsigned int max_uint_array(uint_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a array 
 *
 * @param arr A array data structure of type long_arr.
 * @return The maximum value in the array.
 */
long int max_long_array(long_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a array 
 *
 * @param arr A array data structure of type ulong_arr.
 * @return The maximum value in the array.
 */
unsigned long int max_ulong_array(ulong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a array 
 *
 * @param arr A array data structure of type llong_arr.
 * @return The maximum value in the array.
 */
long long int max_llong_array(llong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a array 
 *
 * @param arr A array data structure of type ullong_arr.
 * @return The maximum value in the array.
 */
unsigned long long int max_ullong_array(ullong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a array 
 *
 * @param arr A array data structure of type float_arr.
 * @return The maximum value in the array.
 */
float max_float_array(float_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a array 
 *
 * @param arr A array data structure of type double_arr.
 * @return The maximum value in the array.
 */
double max_double_array(double_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the maximum value in a array 
 *
 * @param arr A array data structure of type ldouble_arr.
 * @return The maximum value in the array.
 */
long double max_ldouble_array(ldouble_arr* arr);
// --------------------------------------------------------------------------------

#define max_array(arr) _Generic((arr), \
    char_arr: max_char_array, \
    uchar_arr: max_uchar_array, \
    short_arr: max_short_array, \
    ushort_arr: max_ushort_array, \
    int_arr: max_int_array, \
    uint_arr: max_uint_array, \
    long_arr: max_long_array, \
    ulong_arr: max_ulong_array, \
    llong_arr: max_llong_array, \
    ullong_arr: max_ullong_array, \
    float_arr: max_float_array, \
    double_arr: max_double_array, \
    ldouble_arr: max_ldouble_array) (&arr)
// ================================================================================
// ================================================================================
// MIN ARRAY FUNCTIONS 

/**
 * @brief Detrmines the minimum value in a array 
 *
 * @param arr A array data structure of type char_arr.
 * @return The minimum value in the array.
 */
char min_char_array(char_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a array 
 *
 * @param arr A array data structure of type uchar_arr.
 * @return The minimum value in the array.
 */
unsigned char min_uchar_array(uchar_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a array 
 *
 * @param arr A array data structure of type short_arr.
 * @return The maximum value in the array.
 */
short int min_short_array(short_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a array 
 *
 * @param arr A array data structure of type ushort_arr.
 * @return The minimum value in the array.
 */
unsigned short int min_ushort_array(ushort_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a array 
 *
 * @param arr A array data structure of type int_arr.
 * @return The minimum value in the array.
 */
int min_int_array(int_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a array 
 *
 * @param arr A array data structure of type uint_arr.
 * @return The minimum value in the array.
 */
unsigned int min_uint_array(uint_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a array 
 *
 * @param arr A array data structure of type long_arr.
 * @return The minimum value in the array.
 */
long int min_long_array(long_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a array 
 *
 * @param arr A array data structure of type ulong_arr.
 * @return The minimum value in the array.
 */
unsigned long int min_ulong_array(ulong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a array 
 *
 * @param arr A array data structure of type llong_arr.
 * @return The minimum value in the array.
 */
long long int min_llong_array(llong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a array 
 *
 * @param arr A array data structure of type ullong_arr.
 * @return The minimum value in the array.
 */
unsigned long long int min_ullong_array(ullong_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a array 
 *
 * @param arr A array data structure of type float_arr.
 * @return The minimum value in the array.
 */
float min_float_array(float_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a array 
 *
 * @param arr A array data structure of type double_arr.
 * @return The minimum value in the array.
 */
double min_double_array(double_arr* arr);
// --------------------------------------------------------------------------------

/**
 * @brief Detrmines the minimum value in a array 
 *
 * @param arr A array data structure of type ldouble_arr.
 * @return The minimum value in the array.
 */
long double min_ldouble_array(ldouble_arr* arr);
// --------------------------------------------------------------------------------

#define min_array(arr) _Generic((arr), \
    char_arr: min_char_array, \
    uchar_arr: min_uchar_array, \
    short_arr: min_short_array, \
    ushort_arr: min_ushort_array, \
    int_arr: min_int_array, \
    uint_arr: min_uint_array, \
    long_arr: min_long_array, \
    ulong_arr: min_ulong_array, \
    llong_arr: min_llong_array, \
    ullong_arr: min_ullong_array, \
    float_arr: min_float_array, \
    double_arr: min_double_array, \
    ldouble_arr: min_ldouble_array) (&arr)
// ================================================================================
// ================================================================================
// BINARY SEARCH FUNCTIONS 

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type char_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists.
 */
int bsearch_char_array(char_arr* arr, char val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type uchar_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_uchar_array(uchar_arr* arr, unsigned char val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type short_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_short_array(short_arr* arr, short int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type ushort_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists.
 */
int bsearch_ushort_array(ushort_arr* arr, unsigned short int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type int_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists.
 */
int bsearch_int_array(int_arr* arr, int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type uint_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_uint_array(uint_arr* arr, unsigned int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type long_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_long_array(long_arr* arr, long int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type ulong_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_ulong_array(ulong_arr* arr, unsigned long int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type llong_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_llong_array(llong_arr* arr, long long int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type ullong_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_ullong_array(ullong_arr* arr, unsigned long long int val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type float_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_float_array(float_arr* arr, float val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type double_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_double_array(double_arr* arr, double val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type ldouble_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_ldouble_array(ldouble_arr* arr, long double val, bool sorted);
// --------------------------------------------------------------------------------

/**
 * @brief Returns the index where the first occurance of a value occurs.
 *
 * @param arr A array data structure of type bool_arr 
 * @param sorted: true if the array is already sorted, false otherwise.
 * @param val The value being searched for. 
 * @return The index where a variable exists. 
 */
int bsearch_bool_array(bool_arr* arr, bool val, bool sorted);
// --------------------------------------------------------------------------------

#define bsearch_array(arr, val, sorted) _Generic((arr), \
    char_arr: bsearch_char_array, \
    uchar_arr: bsearch_uchar_array, \
    short_arr: bsearch_short_array, \
    ushort_arr: bsearch_ushort_array, \
    int_arr: bsearch_int_array, \
    uint_arr: bsearch_uint_array, \
    long_arr: bsearch_long_array, \
    ulong_arr: bsearch_ulong_array, \
    llong_arr: bsearch_llong_array, \
    ullong_arr: bsearch_ullong_array, \
    float_arr: bsearch_float_array, \
    double_arr: bsearch_double_array, \
    ldouble_arr: bsearch_ldouble_array, \
    bool_arr: bsearch_bool_array) (&arr, val, sorted)
// ================================================================================
// ================================================================================
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* array_H */
// ================================================================================
// ================================================================================
// eof
