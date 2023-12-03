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
#include "vector.h"
#include "array.h"
#include "heap.h"

extern const size_t PRINT_LENGTH;

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
                                         char_v*: true, \
                                         uchar_v*: true, \
                                         short_v*: true, \
                                         ushort_v*: true, \
                                         int_v*: true, \
                                         uint_v*: true, \
                                         long_v*: true, \
                                         ulong_v*: true, \
                                         llong_v*: true, \
                                         ullong_v*: true, \
                                         float_v*: true, \
                                         double_v*: true, \
                                         ldouble_v*: true, \
                                         bool_v*: true, \
                                         string_v*: true, \
                                         char_arr*: true, \
                                         uchar_arr*: true, \
                                         short_arr*: true, \
                                         ushort_arr*: true, \
                                         int_arr*: true, \
                                         uint_arr*: true, \
                                         long_arr*: true, \
                                         ulong_arr*: true, \
                                         llong_arr*: true, \
                                         ullong_arr*: true, \
                                         float_arr*: true, \
                                         double_arr*: true, \
                                         ldouble_arr*: true, \
                                         bool_arr*: true, \
                                         char_min_hp*: true, \
                                         uchar_min_hp*: true, \
                                         short_min_hp*: true, \
                                         ushort_min_hp*: true, \
                                         int_min_hp*: true, \
                                         uint_min_hp*: true, \
                                         long_min_hp*: true, \
                                         ulong_min_hp*: true, \
                                         llong_min_hp*: true, \
                                         ullong_min_hp*: true, \
                                         float_min_hp*: true, \
                                         double_min_hp*: true, \
                                         ldouble_min_hp*: true, \
                                         bool_min_hp*: true, \
                                         string_min_hp*: true, \
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
 * Prototypes for prting of all vector data types.
 */
void print_char_vector(char_v* vec);
void print_uchar_vector(uchar_v* vec);
void print_short_vector(short_v* vec);
void print_ushort_vector(ushort_v* vec);
void print_int_vector(int_v* vec);
void print_uint_vector(uint_v* vec);
void print_long_vector(long_v* vec);
void print_ulong_vector(ulong_v* vec);
void print_llong_vector(llong_v* vec);
void print_ullong_vector(ullong_v* vec);
void print_float_vector(float_v* vec);
void print_double_vector(double_v* vec);
void print_ldouble_vector(ldouble_v* vec);
void print_bool_vector(bool_v* vec);
void print_string_vector(string_v* vec);
// ================================================================================
// ================================================================================

void print_char_array(char_arr* arr);
void print_uchar_array(uchar_arr* arr);
void print_short_array(short_arr* arr);
void print_ushort_array(ushort_arr* arr);
void print_int_array(int_arr* arr);
void print_uint_array(uint_arr* arr);
void print_long_array(long_arr* arr);
void print_ulong_array(ulong_arr* arr);
void print_llong_array(llong_arr* arr);
void print_ullong_array(ullong_arr* arr);
void print_float_array(float_arr* arr);
void print_double_array(double_arr* arr);
void print_ldouble_array(ldouble_arr* arr);
void print_bool_array(bool_arr* arr);
// ================================================================================
// ================================================================================

/**
 * Prototypes for prting of all heap data types.
 */
void print_char_min_heap(char_min_hp* vec);
void print_uchar_min_heap(uchar_min_hp* vec);
void print_short_min_heap(short_min_hp* vec);
void print_ushort_min_heap(ushort_min_hp* vec);
void print_int_min_heap(int_min_hp* vec);
void print_uint_min_heap(uint_min_hp* vec);
void print_long_min_heap(long_min_hp* vec);
void print_ulong_min_heap(ulong_min_hp* vec);
void print_llong_min_heap(llong_min_hp* vec);
void print_ullong_min_heap(ullong_min_hp* vec);
void print_float_min_heap(float_min_hp* vec);
void print_double_min_heap(double_min_hp* vec);
void print_ldouble_min_heap(ldouble_min_hp* vec);
void print_bool_min_heap(bool_min_hp* vec);
void print_string_min_heap(string_min_hp* vec);
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
                                       str*: print_sstring, \
									   char*: print_string, \
                                       char_v*: print_char_vector, \
                                       uchar_v*: print_uchar_vector, \
                                       short_v*: print_short_vector, \
                                       ushort_v*: print_ushort_vector, \
                                       int_v*: print_int_vector, \
                                       uint_v*: print_uint_vector, \
                                       long_v*: print_long_vector, \
                                       ulong_v*: print_ulong_vector, \
                                       llong_v*: print_llong_vector, \
                                       ullong_v*: print_ullong_vector, \
                                       float_v*: print_float_vector, \
                                       double_v*: print_double_vector, \
                                       ldouble_v*: print_ldouble_vector, \
                                       bool_v*: print_bool_vector, \
                                       string_v*: print_string_vector, \
                                       char_arr*: print_char_array, \
                                       uchar_arr*: print_uchar_array, \
                                       short_arr*: print_short_array, \
                                       ushort_arr*: print_ushort_array, \
                                       int_arr*: print_int_array, \
                                       uint_arr*: print_uint_array, \
                                       long_arr*: print_long_array, \
                                       ulong_arr*: print_ulong_array, \
                                       llong_arr*: print_llong_array, \
                                       ullong_arr*: print_ullong_array, \
                                       float_arr*: print_float_array, \
                                       double_arr*: print_double_array, \
                                       ldouble_arr*: print_ldouble_array, \
                                       bool_arr*: print_bool_array, \
                                       char_min_hp*: print_char_min_heap, \
                                       uchar_min_hp*: print_uchar_min_heap, \
                                       short_min_hp*: print_short_min_heap, \
                                       ushort_min_hp*: print_ushort_min_heap, \
                                       int_min_hp*: print_int_min_heap, \
                                       uint_min_hp*: print_uint_min_heap, \
                                       long_min_hp*: print_long_min_heap, \
                                       ulong_min_hp*: print_ulong_min_heap, \
                                       llong_min_hp*: print_llong_min_heap, \
                                       ullong_min_hp*: print_ullong_min_heap, \
                                       float_min_hp*: print_float_min_heap, \
                                       double_min_hp*: print_double_min_heap, \
                                       ldouble_min_hp*: print_ldouble_min_heap, \
                                       bool_min_hp*: print_bool_min_heap, \
                                       string_min_hp*: print_string_min_heap)(T)
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
#define print(...) GET_MACRO(__VA_ARGS__, PRINT8, PRINT7, PRINT6, PRINT5, PRINT4, PRINT3, PRINT2, PRINT1)(__VA_ARGS__)
// ================================================================================
// ================================================================================

#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* print_H */
// ================================================================================
// ================================================================================
// eof
