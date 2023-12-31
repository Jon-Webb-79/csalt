// ================================================================================
// ================================================================================
// - File:    heap.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    December 01, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#error "This code requires C11 or later."
#endif

#ifndef heap_H
#define heap_H

#include <stdio.h>

#include "str.h"
#include "admin.h"
#include "swap.h"

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================
// ================================================================================
// CONSTANTS FOR VECTOR MEMORY MANAGEMENT

extern const size_t HEAP_THRESHOLD;
extern const size_t HEAP_FIXED_AMOUNT;
// ================================================================================
// ================================================================================
// Heap data type structs 

/**
 * @brief A macro to create structs for heaps of various data types.
 *
 * @param type The data type that will be assigned to the data pointer.
 * @param dat_type The name given to the typedef for the struct.
 */
#define HEAP_STRUCT(type, dat_type) \
    typedef struct { type *data; size_t len; size_t alloc; } dat_type
// --------------------------------------------------------------------------------
// Create typedef for each possible vector type 

HEAP_STRUCT(char, char_min_hp);
HEAP_STRUCT(unsigned char, uchar_min_hp);
HEAP_STRUCT(short int, short_min_hp);
HEAP_STRUCT(unsigned short int, ushort_min_hp);
HEAP_STRUCT(int, int_min_hp);
HEAP_STRUCT(unsigned int, uint_min_hp);
HEAP_STRUCT(long int, long_min_hp);
HEAP_STRUCT(unsigned long int, ulong_min_hp);
HEAP_STRUCT(long long int, llong_min_hp);
HEAP_STRUCT(unsigned long long int, ullong_min_hp);
HEAP_STRUCT(float, float_min_hp);
HEAP_STRUCT(double, double_min_hp);
HEAP_STRUCT(long double, ldouble_min_hp);
HEAP_STRUCT(bool, bool_min_hp);
HEAP_STRUCT(str, string_min_hp);

HEAP_STRUCT(char, char_max_hp);
HEAP_STRUCT(unsigned char, uchar_max_hp);
HEAP_STRUCT(short int, short_max_hp);
HEAP_STRUCT(unsigned short int, ushort_max_hp);
HEAP_STRUCT(int, int_max_hp);
HEAP_STRUCT(unsigned int, uint_max_hp);
HEAP_STRUCT(long int, long_max_hp);
HEAP_STRUCT(unsigned long int, ulong_max_hp);
HEAP_STRUCT(long long int, llong_max_hp);
HEAP_STRUCT(unsigned long long int, ullong_max_hp);
HEAP_STRUCT(float, float_max_hp);
HEAP_STRUCT(double, double_max_hp);
HEAP_STRUCT(long double, ldouble_max_hp);
HEAP_STRUCT(bool, bool_max_hp);
HEAP_STRUCT(str, string_max_hp);
// ================================================================================
// ================================================================================
// Initialization functions 

/**
 * @brief typedef function pointer to support init_heap functions
 */
typedef void* (*InitHeapFunc)(size_t);
// --------------------------------------------------------------------------------

/**
 * @brief Function that selects the correct heap initialization function based on type 
 *
 * @param dat_type An enum of type dtype containing a keyword defining the data type 
 * @return A vector initialization function for a specific data type.
 */
InitHeapFunc init_min_heap(dtype dat_type);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of char indices
 * @return A char_hp struct 
 */
char_min_hp* init_char_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned char indices
 * @return A uchar_min_hp struct 
 */
uchar_min_hp* init_uchar_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of short int indices
 * @return A short_min_hp struct 
 */
short_min_hp* init_short_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned short int indices
 * @return A ushort_min_hp struct 
 */
ushort_min_hp* init_ushort_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of int indices
 * @return A int_min_hp struct 
 */
int_min_hp* init_int_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned int indices
 * @return A uint_min_hp struct 
 */
uint_min_hp* init_uint_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of long int indices
 * @return A long_min_hp struct 
 */
long_min_hp* init_long_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned long int indices
 * @return A ulong_min_hp struct 
 */
ulong_min_hp* init_ulong_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of long long int indices
 * @return A llong_min_hp struct 
 */
llong_min_hp* init_llong_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned long long int indices
 * @return A llong_min_hp struct 
 */
ullong_min_hp* init_ullong_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of float indices
 * @return A float_min_hp struct 
 */
float_min_hp* init_float_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of double indices
 * @return A double_min_hp struct 
 */
double_min_hp* init_double_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of long double indices
 * @return A ldouble_min_hp struct 
 */
ldouble_min_hp* init_ldouble_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of bool indices
 * @return A bool_min_hp struct 
 */
bool_min_hp* init_bool_min_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of str indices
 * @return A string_min_hp struct 
 */
string_min_hp* init_string_min_heap(size_t buff);
// ================================================================================
// ================================================================================

/**
 * @brief Function that selects the correct heap initialization function based on type 
 *
 * @param dat_type An enum of type dtype containing a keyword defining the data type 
 * @return A vector initialization function for a specific data type.
 */
InitHeapFunc init_max_heap(dtype dat_type);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of char indices
 * @return A char_hp struct 
 */
char_max_hp* init_char_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned char indices
 * @return A uchar_max_hp struct 
 */
uchar_max_hp* init_uchar_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of short int indices
 * @return A short_max_hp struct 
 */
short_max_hp* init_short_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned short int indices
 * @return A ushort_max_hp struct 
 */
ushort_max_hp* init_ushort_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of int indices
 * @return A int_max_hp struct 
 */
int_max_hp* init_int_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned int indices
 * @return A uint_max_hp struct 
 */
uint_max_hp* init_uint_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of long int indices
 * @return A long_max_hp struct 
 */
long_max_hp* init_long_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned long int indices
 * @return A ulong_max_hp struct 
 */
ulong_max_hp* init_ulong_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of long long int indices
 * @return A llong_max_hp struct 
 */
llong_max_hp* init_llong_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of unsigned long long int indices
 * @return A llong_max_hp struct 
 */
ullong_max_hp* init_ullong_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of float indices
 * @return A float_max_hp struct 
 */
float_max_hp* init_float_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of double indices
 * @return A double_max_hp struct 
 */
double_max_hp* init_double_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of long double indices
 * @return A ldouble_max_hp struct 
 */
ldouble_max_hp* init_ldouble_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of bool indices
 * @return A bool_max_hp struct 
 */
bool_max_hp* init_bool_max_heap(size_t buff);
// --------------------------------------------------------------------------------

/**
 * @breif Initializes and returns a charv struct for a dynamically allocated array
 *
 * @param buff A buffer size where buffer is measured in number of str indices
 * @return A string_max_hp struct 
 */
string_max_hp* init_string_max_heap(size_t buff);
// ================================================================================
// ================================================================================
// FREE FUNCTIONS 

/**
 * @brief Free all memory associated with a char_min_hp data type 
 *
 * @param A char_min_hp data struct
 */
void free_char_min_heap(char_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a uchar_min_hp data type 
 *
 * @param A uchar_min_hp data struct
 */
void free_uchar_min_heap(uchar_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a short_min_hp data type 
 *
 * @param A short_min_hp data struct
 */
void free_short_min_heap(short_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a ushort_min_hp data type 
 *
 * @param A ushort_min_hp data struct
 */
void free_ushort_min_heap(ushort_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a int_min_hp data type 
 *
 * @param A int_min_hp data struct
 */
void free_int_min_heap(int_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a uint_min_hp data type 
 *
 * @param A uint_min_hp data struct
 */
void free_uint_min_heap(uint_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a long_min_hp data type 
 *
 * @param A long_min_hp data struct
 */
void free_long_min_heap(long_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a ulong_min_hp data type 
 *
 * @param A ulong_min_hp data struct
 */
void free_ulong_min_heap(ulong_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a llong_min_hp data type 
 *
 * @param A ullong_min_hp data struct
 */
void free_llong_min_heap(llong_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a ullong_min_hp data type 
 *
 * @param A ullong_min_hp data struct
 */
void free_ullong_min_heap(ullong_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a float_min_hp data type 
 *
 * @param A float_min_hp data struct
 */
void free_float_min_heap(float_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a double_min_hp data type 
 *
 * @param A double_min_hp data struct
 */
void free_double_min_heap(double_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a ldouble_min_hp data type 
 *
 * @param A ldouble_min_hp data struct
 */
void free_ldouble_min_heap(ldouble_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a bool_min_hp data type 
 *
 * @param A bool_min_hp data struct
 */
void free_bool_min_heap(bool_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a string_min_hp data type 
 *
 * @param A string_min_hp data struct
 */
void free_string_min_heap(string_min_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Macro to free all types of dynamically allocated vectors.
 *
 * @param vec A dynamically allocated vector struct.
 */
#define free_min_heap(vec) _Generic((vec), \
    char_min_hp*: free_char_min_heap, \
    uchar_min_hp*: free_uchar_min_heap, \
    short_min_hp*: free_short_min_heap, \
    ushort_min_hp*: free_ushort_min_heap, \
    int_min_hp*: free_int_min_heap, \
    uint_min_hp*: free_uint_min_heap, \
    long_min_hp*: free_long_min_heap, \
    ulong_min_hp*: free_ulong_min_heap, \
    llong_min_hp*: free_llong_min_heap, \
    ullong_min_hp*: free_ullong_min_heap, \
    float_min_hp*: free_float_min_heap, \
    double_min_hp*: free_double_min_heap, \
    ldouble_min_hp*: free_ldouble_min_heap, \
    bool_min_hp*: free_bool_min_heap, \
    default: free_string_min_heap) (vec)
// ================================================================================
// ================================================================================
// FREE FUNCTIONS 

/**
 * @brief Free all memory associated with a char_max_hp data type 
 *
 * @param A char_max_hp data struct
 */
void free_char_max_heap(char_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a uchar_max_hp data type 
 *
 * @param A uchar_max_hp data struct
 */
void free_uchar_max_heap(uchar_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a short_max_hp data type 
 *
 * @param A short_max_hp data struct
 */
void free_short_max_heap(short_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a ushort_max_hp data type 
 *
 * @param A ushort_max_hp data struct
 */
void free_ushort_max_heap(ushort_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a int_max_hp data type 
 *
 * @param A int_max_hp data struct
 */
void free_int_max_heap(int_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a uint_max_hp data type 
 *
 * @param A uint_max_hp data struct
 */
void free_uint_max_heap(uint_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a long_max_hp data type 
 *
 * @param A long_max_hp data struct
 */
void free_long_max_heap(long_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a ulong_max_hp data type 
 *
 * @param A ulong_max_hp data struct
 */
void free_ulong_max_heap(ulong_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a llong_max_hp data type 
 *
 * @param A ullong_max_hp data struct
 */
void free_llong_max_heap(llong_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a ullong_max_hp data type 
 *
 * @param A ullong_max_hp data struct
 */
void free_ullong_max_heap(ullong_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a float_max_hp data type 
 *
 * @param A float_max_hp data struct
 */
void free_float_max_heap(float_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a double_max_hp data type 
 *
 * @param A double_max_hp data struct
 */
void free_double_max_heap(double_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a ldouble_max_hp data type 
 *
 * @param A ldouble_max_hp data struct
 */
void free_ldouble_max_heap(ldouble_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a bool_max_hp data type 
 *
 * @param A bool_max_hp data struct
 */
void free_bool_max_heap(bool_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Free all memory associated with a string_max_hp data type 
 *
 * @param A string_max_hp data struct
 */
void free_string_max_heap(string_max_hp* vec);
// --------------------------------------------------------------------------------

/**
 * @brief Macro to free all types of dynamically allocated vectors.
 *
 * @param vec A dynamically allocated vector struct.
 */
#define free_max_heap(vec) _Generic((vec), \
    char_max_hp*: free_char_max_heap, \
    uchar_max_hp*: free_uchar_max_heap, \
    short_max_hp*: free_short_max_heap, \
    ushort_max_hp*: free_ushort_max_heap, \
    int_max_hp*: free_int_max_heap, \
    uint_max_hp*: free_uint_max_heap, \
    long_max_hp*: free_long_max_heap, \
    ulong_max_hp*: free_ulong_max_heap, \
    llong_max_hp*: free_llong_max_heap, \
    ullong_max_hp*: free_ullong_max_heap, \
    float_max_hp*: free_float_max_heap, \
    double_max_hp*: free_double_max_heap, \
    ldouble_max_hp*: free_ldouble_max_heap, \
    bool_max_hp*: free_bool_max_heap, \
    default: free_string_max_heap) (vec)
// ================================================================================
// ================================================================================
// Cleanup functions for garbage collection
// - while being public functions, theese are not mean to be accessed directly
//   by developers, and instead are prototypes for the garbage collection macros.

void _free_char_min_heap(char_min_hp** vec);
void _free_uchar_min_heap(uchar_min_hp** vec);
void _free_short_min_heap(short_min_hp** vec);
void _free_ushort_min_heap(ushort_min_hp** vec);
void _free_int_min_heap(int_min_hp** vec);
void _free_uint_min_heap(uint_min_hp** vec);
void _free_long_min_heap(long_min_hp** vec);
void _free_ulong_min_heap(ulong_min_hp** vec);
void _free_llong_min_heap(llong_min_hp** vec);
void _free_ullong_min_heap(ullong_min_hp** vec);
void _free_float_min_heap(float_min_hp** vec);
void _free_double_min_heap(double_min_hp** vec);
void _free_ldouble_min_heap(ldouble_min_hp** vec);
void _free_bool_min_heap(bool_min_hp** vec);
void _free_sstring_min_heap(string_min_hp** vec);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined(__clang__)
#define gbc_char_min_hp __attribute__((cleanup(_free_char_min_heap)))
#define gbc_uchar_min_hp __attribute__((cleanup(_free_uchar_min_heap)))
#define gbc_short_min_hp __attribute__((cleanup(_free_short_min_heap)))
#define gbc_ushort_min_hp __attribute__((cleanup(_free_ushort_min_heap)))
#define gbc_int_min_hp __attribute__((cleanup(_free_int_min_heap)))
#define gbc_uint_min_hp __attribute__((cleanup(_free_uint_min_heap)))
#define gbc_long_min_hp __attribute__((cleanup(_free_long_min_heap)))
#define gbc_ulong_min_hp __attribute__((cleanup(_free_ulong_min_heap)))
#define gbc_llong_min_hp __attribute__((cleanup(_free_llong_min_heap)))
#define gbc_ullong_min_hp __attribute__((cleanup(_free_ullong_min_heap)))
#define gbc_float_min_hp __attribute__((cleanup(_free_float_min_heap)))
#define gbc_double_min_hp __attribute__((cleanup(_free_double_min_heap)))
#define gbc_ldouble_min_hp __attribute__((cleanup(_free_ldouble_min_heap)))
#define gbc_bool_min_hp __attribute__((cleanup(_free_bool_min_heap)))
#define gbc_string_min_hp __attribute__((cleanup(_free_sstring_min_heap)))
#endif
// ================================================================================
// ================================================================================
// Cleanup functions for garbage collection
// - while being public functions, theese are not mean to be accessed directly
//   by developers, and instead are prototypes for the garbage collection macros.

void _free_char_max_heap(char_max_hp** vec);
void _free_uchar_max_heap(uchar_max_hp** vec);
void _free_short_max_heap(short_max_hp** vec);
void _free_ushort_max_heap(ushort_max_hp** vec);
void _free_int_max_heap(int_max_hp** vec);
void _free_uint_max_heap(uint_max_hp** vec);
void _free_long_max_heap(long_max_hp** vec);
void _free_ulong_max_heap(ulong_max_hp** vec);
void _free_llong_max_heap(llong_max_hp** vec);
void _free_ullong_max_heap(ullong_max_hp** vec);
void _free_float_max_heap(float_max_hp** vec);
void _free_double_max_heap(double_max_hp** vec);
void _free_ldouble_max_heap(ldouble_max_hp** vec);
void _free_bool_max_heap(bool_max_hp** vec);
void _free_sstring_max_heap(string_max_hp** vec);
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined(__clang__)
#define gbc_char_max_hp __attribute__((cleanup(_free_char_max_heap)))
#define gbc_uchar_max_hp __attribute__((cleanup(_free_uchar_max_heap)))
#define gbc_short_max_hp __attribute__((cleanup(_free_short_max_heap)))
#define gbc_ushort_max_hp __attribute__((cleanup(_free_ushort_max_heap)))
#define gbc_int_max_hp __attribute__((cleanup(_free_int_max_heap)))
#define gbc_uint_max_hp __attribute__((cleanup(_free_uint_max_heap)))
#define gbc_long_max_hp __attribute__((cleanup(_free_long_max_heap)))
#define gbc_ulong_max_hp __attribute__((cleanup(_free_ulong_max_heap)))
#define gbc_llong_max_hp __attribute__((cleanup(_free_llong_max_heap)))
#define gbc_ullong_max_hp __attribute__((cleanup(_free_ullong_max_heap)))
#define gbc_float_max_hp __attribute__((cleanup(_free_float_max_heap)))
#define gbc_double_max_hp __attribute__((cleanup(_free_double_max_heap)))
#define gbc_ldouble_max_hp __attribute__((cleanup(_free_ldouble_max_heap)))
#define gbc_bool_max_hp __attribute__((cleanup(_free_bool_max_heap)))
#define gbc_string_max_hp __attribute__((cleanup(_free_sstring_max_heap)))
#endif
// ================================================================================
// ================================================================================
// BUILD HEAP FROM EXISTING ARRAY 

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type char_min_hp 
 * @param element A char element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_char(char_min_hp* heap, char element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type uchar_min_hp 
 * @param element A uchar element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_uchar(uchar_min_hp* heap, unsigned char element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type short_min_hp 
 * @param element A short int element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_short(short_min_hp* heap, short int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type ushort_min_hp 
 * @param element A unsigned short element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_ushort(ushort_min_hp* heap, unsigned short int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type int_min_hp 
 * @param element A int element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_int(int_min_hp* heap, int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type uint_min_hp 
 * @param element A unsigned int element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_uint(uint_min_hp* heap, unsigned int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type long_min_hp 
 * @param element A long int element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_long(long_min_hp* heap, long int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type ulong_min_hp 
 * @param element A unsigned long element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_ulong(ulong_min_hp* heap, unsigned long int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type llong_min_hp 
 * @param element A long long int element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_llong(llong_min_hp* heap, long long int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type ullong_min_hp 
 * @param element A unsigned long long int element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_ullong(ullong_min_hp* heap, unsigned long long int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type float_min_hp 
 * @param element A float element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_float(float_min_hp* heap, float element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type double_min_hp 
 * @param element A double element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_double(double_min_hp* heap, double element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type ldouble_min_hp 
 * @param element A long double element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_ldouble(ldouble_min_hp* heap, long double element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type bool_min_hp 
 * @param element A bool element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_bool(bool_min_hp* heap, bool element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type string_min_hp 
 * @param element A char* element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_string(string_min_hp* heap, char* element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a min heap data structure.
 *
 * @param heap a min heap data structure of type str_min_hp 
 * @param element A str element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_min_heap_str(string_min_hp* heap, str* element);
// --------------------------------------------------------------------------------

#define enqueue_min_heap(heap, element) _Generic((heap), \
    char_min_hp*: enqueue_min_heap_char, \
    uchar_min_hp*: enqueue_min_heap_uchar, \
    short_min_hp*: enqueue_min_heap_short, \
    ushort_min_hp*: enqueue_min_heap_ushort, \
    int_min_hp*: enqueue_min_heap_int, \
    uint_min_hp*: enqueue_min_heap_uint, \
    long_min_hp*: enqueue_min_heap_long, \
    ulong_min_hp*: enqueue_min_heap_ulong, \
    llong_min_hp*: enqueue_min_heap_llong, \
    ullong_min_hp*: enqueue_min_heap_ullong, \
    float_min_hp*: enqueue_min_heap_float, \
    double_min_hp*: enqueue_min_heap_double, \
    ldouble_min_hp*: enqueue_min_heap_ldouble, \
    bool_min_hp*: enqueue_min_heap_bool, \
    string_min_hp*: enqueue_min_heap_string) (heap, element)
// ================================================================================
// ================================================================================

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type char_max_hp 
 * @param element A char element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_char(char_max_hp* heap, char element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type uchar_max_hp 
 * @param element A uchar element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_uchar(uchar_max_hp* heap, unsigned char element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type short_max_hp 
 * @param element A short int element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_short(short_max_hp* heap, short int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type ushort_max_hp 
 * @param element A unsigned short element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_ushort(ushort_max_hp* heap, unsigned short int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type int_max_hp 
 * @param element A int element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_int(int_max_hp* heap, int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type uint_max_hp 
 * @param element A unsigned int element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_uint(uint_max_hp* heap, unsigned int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type long_max_hp 
 * @param element A long int element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_long(long_max_hp* heap, long int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type ulong_max_hp 
 * @param element A unsigned long element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_ulong(ulong_max_hp* heap, unsigned long int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type llong_max_hp 
 * @param element A long long int element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_llong(llong_max_hp* heap, long long int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type ullong_max_hp 
 * @param element A unsigned long long int element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_ullong(ullong_max_hp* heap, unsigned long long int element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type float_max_hp 
 * @param element A float element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_float(float_max_hp* heap, float element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type double_max_hp 
 * @param element A double element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_double(double_max_hp* heap, double element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type ldouble_max_hp 
 * @param element A long double element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_ldouble(ldouble_max_hp* heap, long double element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type bool_max_hp 
 * @param element A bool element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_bool(bool_max_hp* heap, bool element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type string_max_hp 
 * @param element A char* element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_string(string_max_hp* heap, char* element);
// --------------------------------------------------------------------------------

/**
 * @brief a Function to insert an element into a max heap data structure.
 *
 * @param heap a max heap data structure of type str_max_hp 
 * @param element A str element 
 * @return true if function executes sucesfully, false otherwise.
 */
bool enqueue_max_heap_str(string_max_hp* heap, str* element);
// --------------------------------------------------------------------------------

#define enqueue_max_heap(heap, element) _Generic((heap), \
    char_max_hp*: enqueue_max_heap_char, \
    uchar_max_hp*: enqueue_max_heap_uchar, \
    short_max_hp*: enqueue_max_heap_short, \
    ushort_max_hp*: enqueue_max_heap_ushort, \
    int_max_hp*: enqueue_max_heap_int, \
    uint_max_hp*: enqueue_max_heap_uint, \
    long_max_hp*: enqueue_max_heap_long, \
    ulong_max_hp*: enqueue_max_heap_ulong, \
    llong_max_hp*: enqueue_max_heap_llong, \
    ullong_max_hp*: enqueue_max_heap_ullong, \
    float_max_hp*: enqueue_max_heap_float, \
    double_max_hp*: enqueue_max_heap_double, \
    ldouble_max_hp*: enqueue_max_heap_ldouble, \
    bool_max_hp*: enqueue_max_heap_bool, \
    string_max_hp*: enqueue_max_heap_string) (heap, element)
// ================================================================================
// ================================================================================
// DEQUEUE MIN HEAP 

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
char dequeue_min_heap_char(char_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
unsigned char dequeue_min_heap_uchar(uchar_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
short int dequeue_min_heap_short(short_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
unsigned short int dequeue_min_heap_ushort(ushort_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
int dequeue_min_heap_int(int_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
unsigned int dequeue_min_heap_uint(uint_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
long int dequeue_min_heap_long(long_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
unsigned long int dequeue_min_heap_ulong(ulong_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
long long int dequeue_min_heap_llong(llong_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
unsigned long long int dequeue_min_heap_ullong(ullong_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
float dequeue_min_heap_float(float_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
double dequeue_min_heap_double(double_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
long double dequeue_min_heap_ldouble(ldouble_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
bool dequeue_min_heap_bool(bool_min_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the minimum value from a min heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the minimum value
 */
str* dequeue_min_heap_string(string_min_hp* heap);
// --------------------------------------------------------------------------------

#define dequeue_min_heap(heap) _Generic((heap), \
    char_min_hp*: dequeue_min_heap_char, \
    uchar_min_hp*: dequeue_min_heap_uchar, \
    short_min_hp*: dequeue_min_heap_short, \
    ushort_min_hp*: dequeue_min_heap_ushort, \
    int_min_hp*: dequeue_min_heap_int, \
    uint_min_hp*: dequeue_min_heap_uint, \
    long_min_hp*: dequeue_min_heap_long, \
    ulong_min_hp*: dequeue_min_heap_ulong, \
    llong_min_hp*: dequeue_min_heap_llong, \
    ullong_min_hp*: dequeue_min_heap_ullong, \
    float_min_hp*: dequeue_min_heap_float, \
    double_min_hp*: dequeue_min_heap_double, \
    ldouble_min_hp*: dequeue_min_heap_ldouble, \
    bool_min_hp*: dequeue_min_heap_bool, \
    string_min_hp*: dequeue_min_heap_string) (heap)
// ================================================================================
// ================================================================================
// DEQUEUE MAX HEAP 

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
char dequeue_max_heap_char(char_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
unsigned char dequeue_max_heap_uchar(uchar_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
short int dequeue_max_heap_short(short_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
unsigned short int dequeue_max_heap_ushort(ushort_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
int dequeue_max_heap_int(int_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
unsigned int dequeue_max_heap_uint(uint_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
long int dequeue_max_heap_long(long_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
unsigned long int dequeue_max_heap_ulong(ulong_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
long long int dequeue_max_heap_llong(llong_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
unsigned long long int dequeue_max_heap_ullong(ullong_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
float dequeue_max_heap_float(float_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
double dequeue_max_heap_double(double_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
long double dequeue_max_heap_ldouble(ldouble_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
bool dequeue_max_heap_bool(bool_max_hp* heap);
// --------------------------------------------------------------------------------

/**
 * @brief Pops the maximum value from a max heap and returns the value to the user.
 *
 * @param heap A heap data structure
 * @return the maximum value
 */
str* dequeue_max_heap_string(string_max_hp* heap);
// --------------------------------------------------------------------------------

#define dequeue_max_heap(heap) _Generic((heap), \
    char_max_hp*: dequeue_max_heap_char, \
    uchar_max_hp*: dequeue_max_heap_uchar, \
    short_max_hp*: dequeue_max_heap_short, \
    ushort_max_hp*: dequeue_max_heap_ushort, \
    int_max_hp*: dequeue_max_heap_int, \
    uint_max_hp*: dequeue_max_heap_uint, \
    long_max_hp*: dequeue_max_heap_long, \
    ulong_max_hp*: dequeue_max_heap_ulong, \
    llong_max_hp*: dequeue_max_heap_llong, \
    ullong_max_hp*: dequeue_max_heap_ullong, \
    float_max_hp*: dequeue_max_heap_float, \
    double_max_hp*: dequeue_max_heap_double, \
    ldouble_max_hp*: dequeue_max_heap_ldouble, \
    bool_max_hp*: dequeue_max_heap_bool, \
    string_max_hp*: dequeue_max_heap_string) (heap)
// ================================================================================
// ================================================================================
// REPLACE INDEX FUNCTION 

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_char_min_heap_index(char_min_hp* heap, size_t index, char value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_uchar_min_heap_index(uchar_min_hp* heap, size_t index, unsigned char value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_short_min_heap_index(short_min_hp* heap, size_t index, short int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_ushort_min_heap_index(ushort_min_hp* heap, size_t index, unsigned short int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_int_min_heap_index(int_min_hp* heap, size_t index, int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_uint_min_heap_index(uint_min_hp* heap, size_t index, unsigned int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_long_min_heap_index(long_min_hp* heap, size_t index, long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_ulong_min_heap_index(ulong_min_hp* heap, size_t index, unsigned long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_llong_min_heap_index(llong_min_hp* heap, size_t index, long long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_ullong_min_heap_index(ullong_min_hp* heap, size_t index, unsigned long long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_float_min_heap_index(float_min_hp* heap, size_t index, float value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_double_min_heap_index(double_min_hp* heap, size_t index, double value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_ldouble_min_heap_index(ldouble_min_hp* heap, size_t index, long double value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_bool_min_heap_index(bool_min_hp* heap, size_t index, bool value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a min heap index with another.
 *
 * This function will replace the value at a min heap index with another value 
 * and re-order the heap data structure to be consistent with the min heap invariant.
 *
 * @param heap A min heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_string_min_heap_index(string_min_hp* heap, size_t index, char* value);
// --------------------------------------------------------------------------------

#define replace_min_heap_index(heap, index, value) _Generic((heap), \
    char_min_hp*: replace_char_min_heap_index, \
    uchar_min_hp*: replace_uchar_min_heap_index, \
    short_min_hp*: replace_short_min_heap_index, \
    ushort_min_hp*: replace_ushort_min_heap_index, \
    int_min_hp*: replace_int_min_heap_index, \
    uint_min_hp*: replace_uint_min_heap_index, \
    long_min_hp*: replace_long_min_heap_index, \
    ulong_min_hp*: replace_ulong_min_heap_index, \
    llong_min_hp*: replace_llong_min_heap_index, \
    ullong_min_hp*: replace_ullong_min_heap_index, \
    float_min_hp*: replace_float_min_heap_index, \
    double_min_hp*: replace_double_min_heap_index, \
    ldouble_min_hp*: replace_ldouble_min_heap_index, \
    bool_min_hp*: replace_bool_min_heap_index, \
    string_min_hp*: replace_string_min_heap_index) (heap, index, value)
// ================================================================================
// ================================================================================

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_char_max_heap_index(char_max_hp* heap, size_t index, char value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_uchar_max_heap_index(uchar_max_hp* heap, size_t index, unsigned char value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_short_max_heap_index(short_max_hp* heap, size_t index, short int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_ushort_max_heap_index(ushort_max_hp* heap, size_t index, unsigned short int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_int_max_heap_index(int_max_hp* heap, size_t index, int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_uint_max_heap_index(uint_max_hp* heap, size_t index, unsigned int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_long_max_heap_index(long_max_hp* heap, size_t index, long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_ulong_max_heap_index(ulong_max_hp* heap, size_t index, unsigned long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_llong_max_heap_index(llong_max_hp* heap, size_t index, long long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_ullong_max_heap_index(ullong_max_hp* heap, size_t index, unsigned long long int value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_float_max_heap_index(float_max_hp* heap, size_t index, float value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_double_max_heap_index(double_max_hp* heap, size_t index, double value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_ldouble_max_heap_index(ldouble_max_hp* heap, size_t index, long double value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_bool_max_heap_index(bool_max_hp* heap, size_t index, bool value);
// --------------------------------------------------------------------------------

/**
 * @brief This function will replace the value at a max heap index with another.
 *
 * This function will replace the value at a max heap index with another value 
 * and re-order the heap data structure to be consistent with the max heap invariant.
 *
 * @param heap A max heap data structure 
 * @param index The index where data will be replaced 
 * @param value The value which will replace the original value
 * @return true if the function executes sucesfully, false otherwise
 */
bool replace_string_max_heap_index(string_max_hp* heap, size_t index, char* value);
// --------------------------------------------------------------------------------

#define replace_max_heap_index(heap, index, value) _Generic((heap), \
    char_max_hp*: replace_char_max_heap_index, \
    uchar_max_hp*: replace_uchar_max_heap_index, \
    short_max_hp*: replace_short_max_heap_index, \
    ushort_max_hp*: replace_ushort_max_heap_index, \
    int_max_hp*: replace_int_max_heap_index, \
    uint_max_hp*: replace_uint_max_heap_index, \
    long_max_hp*: replace_long_max_heap_index, \
    ulong_max_hp*: replace_ulong_max_heap_index, \
    llong_max_hp*: replace_llong_max_heap_index, \
    ullong_max_hp*: replace_ullong_max_heap_index, \
    float_max_hp*: replace_float_max_heap_index, \
    double_max_hp*: replace_double_max_heap_index, \
    ldouble_max_hp*: replace_ldouble_max_heap_index, \
    bool_max_hp*: replace_bool_max_heap_index, \
    string_max_hp*: replace_string_max_heap_index) (heap, index, value)
// ================================================================================
// ================================================================================
// GET MIN HEAP LENGTH 

/**
 * @brief Returns the length of a min heap data structure
 */
size_t char_min_heap_length(char_min_hp* vec);
size_t uchar_min_heap_length(uchar_min_hp* vec);
size_t short_min_heap_length(short_min_hp* vec);
size_t ushort_min_heap_length(ushort_min_hp* vec);
size_t int_min_heap_length(int_min_hp* vec);
size_t uint_min_heap_length(uint_min_hp* vec);
size_t long_min_heap_length(long_min_hp* vec);
size_t ulong_min_heap_length(ulong_min_hp* vec);
size_t llong_min_heap_length(llong_min_hp* vec);
size_t ullong_min_heap_length(ullong_min_hp* vec);
size_t float_min_heap_length(float_min_hp* vec);
size_t double_min_heap_length(double_min_hp* vec);
size_t ldouble_min_heap_length(ldouble_min_hp* vec);
size_t bool_min_heap_length(bool_min_hp* vec);
size_t string_min_heap_length(string_min_hp* vec);
size_t char_max_heap_length(char_max_hp* vec);
size_t uchar_max_heap_length(uchar_max_hp* vec);
size_t short_max_heap_length(short_max_hp* vec);
size_t ushort_max_heap_length(ushort_max_hp* vec);
size_t int_max_heap_length(int_max_hp* vec);
size_t uint_max_heap_length(uint_max_hp* vec);
size_t long_max_heap_length(long_max_hp* vec);
size_t ulong_max_heap_length(ulong_max_hp* vec);
size_t llong_max_heap_length(llong_max_hp* vec);
size_t ullong_max_heap_length(ullong_max_hp* vec);
size_t float_max_heap_length(float_max_hp* vec);
size_t double_max_heap_length(double_max_hp* vec);
size_t ldouble_max_heap_length(ldouble_max_hp* vec);
size_t bool_max_heap_length(bool_max_hp* vec);
size_t string_max_heap_length(string_max_hp* vec);
// --------------------------------------------------------------------------------

#define heap_length(heap) _Generic((heap), \
    char_min_hp*: char_min_heap_length, \
    uchar_min_hp*: uchar_min_heap_length, \
    short_min_hp*: short_min_heap_length, \
    ushort_min_hp*: ushort_min_heap_length, \
    int_min_hp*: int_min_heap_length, \
    uint_min_hp*: uint_min_heap_length, \
    long_min_hp*: long_min_heap_length, \
    ulong_min_hp*: ulong_min_heap_length, \
    llong_min_hp*: llong_min_heap_length, \
    ullong_min_hp*: ullong_min_heap_length, \
    float_min_hp*: float_min_heap_length, \
    double_min_hp*: double_min_heap_length, \
    ldouble_min_hp*: ldouble_min_heap_length, \
    bool_min_hp*: bool_min_heap_length, \
    string_min_hp*: string_min_heap_length, \
    char_max_hp*: char_max_heap_length, \
    uchar_max_hp*: uchar_max_heap_length, \
    short_max_hp*: short_max_heap_length, \
    ushort_max_hp*: ushort_max_heap_length, \
    int_max_hp*: int_max_heap_length, \
    uint_max_hp*: uint_max_heap_length, \
    long_max_hp*: long_max_heap_length, \
    ulong_max_hp*: ulong_max_heap_length, \
    llong_max_hp*: llong_max_heap_length, \
    ullong_max_hp*: ullong_max_heap_length, \
    float_max_hp*: float_max_heap_length, \
    double_max_hp*: double_max_heap_length, \
    ldouble_max_hp*: ldouble_max_heap_length, \
    bool_max_hp*: bool_max_heap_length, \
    string_max_hp*: string_max_heap_length) (heap)

// ================================================================================
// ================================================================================
// GET ALLOC FUNCTIONS
/**
 * @brief Returns the memory allocation of a min heap data structure
 */
size_t char_min_heap_memory(char_min_hp* vec);
size_t uchar_min_heap_memory(uchar_min_hp* vec);
size_t short_min_heap_memory(short_min_hp* vec);
size_t ushort_min_heap_memory(ushort_min_hp* vec);
size_t int_min_heap_memory(int_min_hp* vec);
size_t uint_min_heap_memory(uint_min_hp* vec);
size_t long_min_heap_memory(long_min_hp* vec);
size_t ulong_min_heap_memory(ulong_min_hp* vec);
size_t llong_min_heap_memory(llong_min_hp* vec);
size_t ullong_min_heap_memory(ullong_min_hp* vec);
size_t float_min_heap_memory(float_min_hp* vec);
size_t double_min_heap_memory(double_min_hp* vec);
size_t ldouble_min_heap_memory(ldouble_min_hp* vec);
size_t bool_min_heap_memory(bool_min_hp* vec);
size_t string_min_heap_memory(string_min_hp* vec);
size_t char_max_heap_memory(char_max_hp* vec);
size_t uchar_max_heap_memory(uchar_max_hp* vec);
size_t short_max_heap_memory(short_max_hp* vec);
size_t ushort_max_heap_memory(ushort_max_hp* vec);
size_t int_max_heap_memory(int_max_hp* vec);
size_t uint_max_heap_memory(uint_max_hp* vec);
size_t long_max_heap_memory(long_max_hp* vec);
size_t ulong_max_heap_memory(ulong_max_hp* vec);
size_t llong_max_heap_memory(llong_max_hp* vec);
size_t ullong_max_heap_memory(ullong_max_hp* vec);
size_t float_max_heap_memory(float_max_hp* vec);
size_t double_max_heap_memory(double_max_hp* vec);
size_t ldouble_max_heap_memory(ldouble_max_hp* vec);
size_t bool_max_heap_memory(bool_max_hp* vec);
size_t string_max_heap_memory(string_max_hp* vec);
// --------------------------------------------------------------------------------

#define heap_memory(heap) _Generic((heap), \
    char_min_hp*: char_min_heap_memory, \
    uchar_min_hp*: uchar_min_heap_memory, \
    short_min_hp*: short_min_heap_memory, \
    ushort_min_hp*: ushort_min_heap_memory, \
    int_min_hp*: int_min_heap_memory, \
    uint_min_hp*: uint_min_heap_memory, \
    long_min_hp*: long_min_heap_memory, \
    ulong_min_hp*: ulong_min_heap_memory, \
    llong_min_hp*: llong_min_heap_memory, \
    ullong_min_hp*: ullong_min_heap_memory, \
    float_min_hp*: float_min_heap_memory, \
    double_min_hp*: double_min_heap_memory, \
    ldouble_min_hp*: ldouble_min_heap_memory, \
    bool_min_hp*: bool_min_heap_memory, \
    string_min_hp*: string_min_heap_memory, \
    char_max_hp*: char_max_heap_memory, \
    uchar_max_hp*: uchar_max_heap_memory, \
    short_max_hp*: short_max_heap_memory, \
    ushort_max_hp*: ushort_max_heap_memory, \
    int_max_hp*: int_max_heap_memory, \
    uint_max_hp*: uint_max_heap_memory, \
    long_max_hp*: long_max_heap_memory, \
    ulong_max_hp*: ulong_max_heap_memory, \
    llong_max_hp*: llong_max_heap_memory, \
    ullong_max_hp*: ullong_max_heap_memory, \
    float_max_hp*: float_max_heap_memory, \
    double_max_hp*: double_max_heap_memory, \
    ldouble_max_hp*: ldouble_max_heap_memory, \
    bool_max_hp*: bool_max_heap_memory, \
    string_max_hp*: string_max_heap_memory) (heap)
// ================================================================================
// ================================================================================
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* heap_H */
// ================================================================================
// ================================================================================
// eof
