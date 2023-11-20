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
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* array_H */
// ================================================================================
// ================================================================================
// eof
