// ================================================================================
// ================================================================================
// - File:    print.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    October 28, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#error "This code requires C11 or later."
#endif

#ifndef print_H
#define print_H

#include <stdio.h>

#include "str.h"

//extern const size_t PRINT_LENGTH;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Type check the data to ensure it is valid
 */
#define IS_ITYPE_VALID(T) _Generic( (T), long int: true, \
		                                 unsigned long int: true, \
									     long long int: true, \
									     unsigned long long int: true, \
										 int: true, \
										 unsigned int: true, \
									     float: true, \
									     double: true, \
									     long double: true, \
									     char: true, \
										 unsigned char: true, \
										 bool: true, \
									     char*: true, \
                                         str*: true, \
										 default: false)
// ================================================================================
// ================================================================================

/**
 * Prototypes for the printing of all standard data types, plus str data types.
 */
void print_short(short x);
void print_ushort(unsigned short x);
void print_int(int x);
void print_uint(unsigned int x);
void print_long(long int x);
void print_ulong(unsigned long int x);
void print_llong(long long int x);
void print_ullong(unsigned long long int x);
void print_float(float x);
void print_double(double x);
void print_longdouble(long double x);
void print_char(char x);
void print_uchar(unsigned char x);
void print_bool(bool x);
void print_string(char *x);
void print_sstring(str* x);
// ================================================================================
// ================================================================================

/**
 * Macro using _Generic operator to determine which functions get implemented
 * in a print statement.
 */
#define ITYPE_FORMAT(T) _Generic( (T), long int: print_long, \
									   unsigned long int: print_ullong, \
									   long long int: print_llong, \
									   unsigned long long int: print_ullong, \
									   float: print_float, \
									   double: print_double, \
									   long double: print_longdouble, \
									   char: print_char, \
									   unsigned char: print_uchar, \
									   int: print_int, \
									   unsigned int: print_uint, \
									   bool: print_bool, \
									   char*: print_string)(T)
// ================================================================================
// ================================================================================

#define PRINT1(a) do {															\
	if (IS_ITYPE_VALID(a)) ITYPE_FORMAT(a);										\
	printf("\n");																\
} while (0)
// --------------------------------------------------------------------------------

#define PRINT2(a, b) do {														\
	if (IS_ITYPE_VALID(a)) ITYPE_FORMAT(a);										\
	if (IS_ITYPE_VALID(b)) ITYPE_FORMAT(b);										\
	printf("\n");																\
} while (0)
// --------------------------------------------------------------------------------

#define PRINT3(a, b, c) do {													\
	if (IS_ITYPE_VALID(a)) ITYPE_FORMAT(a);										\
	if (IS_ITYPE_VALID(b)) ITYPE_FORMAT(b);										\
	if (IS_ITYPE_VALID(c)) ITYPE_FORMAT(c);										\
	printf("\n");																\
} while (0)
// --------------------------------------------------------------------------------

#define PRINT4(a, b, c, d) do {													\
	if (IS_ITYPE_VALID(a)) ITYPE_FORMAT(a);										\
	if (IS_ITYPE_VALID(b)) ITYPE_FORMAT(b);										\
	if (IS_ITYPE_VALID(c)) ITYPE_FORMAT(c);										\
	if (IS_ITYPE_VALID(d)) ITYPE_FORMAT(d);										\
	printf("\n");																\
} while (0)
// --------------------------------------------------------------------------------

#define PRINT5(a, b, c, d, e) do {												\
	if (IS_ITYPE_VALID(a)) ITYPE_FORMAT(a);										\
	if (IS_ITYPE_VALID(b)) ITYPE_FORMAT(b);										\
	if (IS_ITYPE_VALID(c)) ITYPE_FORMAT(c);										\
	if (IS_ITYPE_VALID(d)) ITYPE_FORMAT(d);										\
	if (IS_ITYPE_VALID(e)) ITYPE_FORMAT(e);										\
	printf("\n");																\
} while (0)
// --------------------------------------------------------------------------------

#define PRINT6(a, b, c, d, e, f) do {											\
	if (IS_ITYPE_VALID(a)) ITYPE_FORMAT(a);										\
	if (IS_ITYPE_VALID(b)) ITYPE_FORMAT(b);										\
	if (IS_ITYPE_VALID(c)) ITYPE_FORMAT(c);										\
	if (IS_ITYPE_VALID(d)) ITYPE_FORMAT(d);										\
	if (IS_ITYPE_VALID(e)) ITYPE_FORMAT(e);										\
	if (IS_ITYPE_VALID(f)) ITYPE_FORMAT(f);										\
	printf("\n");																\
} while (0)
// --------------------------------------------------------------------------------

#define PRINT7(a, b, c, d, e, f, g) do {										\
	if (IS_ITYPE_VALID(a)) ITYPE_FORMAT(a);										\
	if (IS_ITYPE_VALID(b)) ITYPE_FORMAT(b);										\
	if (IS_ITYPE_VALID(c)) ITYPE_FORMAT(c);										\
	if (IS_ITYPE_VALID(d)) ITYPE_FORMAT(d);										\
	if (IS_ITYPE_VALID(e)) ITYPE_FORMAT(e);										\
	if (IS_ITYPE_VALID(f)) ITYPE_FORMAT(f);										\
	if (IS_ITYPE_VALID(g)) ITYPE_FORMAT(g);										\
	printf("\n");																\
} while (0)
// --------------------------------------------------------------------------------

#define PRINT8(a, b, c, d, e, f, g, h) do {										\
	if (IS_ITYPE_VALID(a)) ITYPE_FORMAT(a);										\
	if (IS_ITYPE_VALID(b)) ITYPE_FORMAT(b);										\
	if (IS_ITYPE_VALID(c)) ITYPE_FORMAT(c);										\
	if (IS_ITYPE_VALID(d)) ITYPE_FORMAT(d);										\
	if (IS_ITYPE_VALID(e)) ITYPE_FORMAT(e);										\
	if (IS_ITYPE_VALID(f)) ITYPE_FORMAT(f);										\
	if (IS_ITYPE_VALID(g)) ITYPE_FORMAT(g);										\
	if (IS_ITYPE_VALID(h)) ITYPE_FORMAT(h);										\
	printf("\n");																\
} while (0)
// --------------------------------------------------------------------------------

#define GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,NAME,...) NAME
// --------------------------------------------------------------------------------

/**
 * This function will print up to 8 arguments of any data type without requiring the
 * user implement a specific format.  Unfortunately the present version of this
 * Macro does not allow a user to tailor the format of the print function
 *
 * @code
 * float p = 3.14;
 * PRINT("The value of pi is ", pi);
 * // >> The value of pi is 3.14000
 * @endcode
 */
#define PRINT(...) GET_MACRO(__VA_ARGS__, PRINT8, PRINT7, PRINT6, PRINT5, PRINT4, PRINT3, PRINT2, PRINT1)(__VA_ARGS__)
// ================================================================================
// ================================================================================

#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* print_H */
// ================================================================================
// ================================================================================
// eof
