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

#include "admin.h"
#include "str.h"

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================
// ================================================================================
// CONSTANTS FOR VECTOR MEMORY MANAGEMENT

extern const size_t VEC_THRESHOLD;
extern const size_t VEC_FIXED_AMOUNT;
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
 * This function allows a user to insert str data into dynamically 
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
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* vector_H */
// ================================================================================
// ================================================================================
// eof
