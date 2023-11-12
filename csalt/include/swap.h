// ================================================================================
// ================================================================================
// - File:    swap.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    November 11, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#error "This code requires C11 or later."
#endif

#ifndef swap_H
#define swap_H

#include <stdio.h>
#include "str.h"

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================
// ================================================================================

/**
 * @brief Swaps two char values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_char(char *a, char *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two unsigned char values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_uchar(unsigned char *a, unsigned char *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two short int values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_short(short int *a, short int *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two unsigned short int values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_ushort(unsigned short int *a, unsigned short int *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two int values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_int(int *a, int *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two unsigned int values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_uint(unsigned int *a, unsigned int *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two long int values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_long(long int *a, long int *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two unsigned long int values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_ulong(unsigned long int *a, unsigned long int *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two long long int values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_llong(long long int *a, long long int *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two unsigned long long int values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_ullong(unsigned long long int *a, unsigned long long int *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two float values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_float(float *a, float *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two double values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_double(double *a, double *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two long double values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_ldouble(long double *a, long double *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two bool values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_bool(bool *a, bool *b);
// --------------------------------------------------------------------------------

/**
 * @brief Swaps two str values in memory
 * @param a variable one
 * @param b variable two
 */
void swap_string(str *a, str *b);
// --------------------------------------------------------------------------------
// SWAP GENERIC OPERATOR

/*
 * @brief This macro will swap two like data types in memory.
 */
#define SWAP(a, b) _Generic( (a), char: swap_char, \
		                          unsigned char: swap_uchar, \
								  short int: swap_short, \
                                  unsigned short int: swap_ushort, \
								  int: swap_int, \
		                          unsigned int: swap_uint, \
								  long int: swap_long, \
								  unsigned long int: swap_ulong, \
								  long long int: swap_llong, \
								  unsigned long long int: swap_ullong, \
								  float: swap_float, \
								  double: swap_double, \
								  long double: swap_ldouble, \
								  bool: swap_bool)(&a, &b)

#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* swap_H */
// ================================================================================
// ================================================================================
// eof
