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
string_min_hp* init_string_min_min_heap(size_t buff);
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
    string_min_hp*: string_min_heap_length) (heap)
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
    string_min_hp*: string_min_heap_memory) (heap)
// ================================================================================
// ================================================================================
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* heap_H */
// ================================================================================
// ================================================================================
// eof
