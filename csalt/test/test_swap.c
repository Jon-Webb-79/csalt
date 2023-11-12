// ================================================================================
// ================================================================================
// - File:    test_swap.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 31, 2022
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "test_swap.h"

void test_swap_short(void **state)
{
	(void) state;
	short int a = 5;
	short int b = 2;
	SWAP(a, b);
	assert_int_equal(b, (short int)5);
	assert_int_equal(a, (short int)2);
}
// ------------------------------------------------------------------------------------------

void test_swap_ushort(void **state)
{
	(void) state;
	unsigned short int a = 5;
	unsigned short int b = 2;
	SWAP(a, b);
	assert_int_equal(b, (unsigned short int)5);
	assert_int_equal(a, (unsigned short int)2);
}
// ------------------------------------------------------------------------------------------

void test_swap_int(void **state)
{
	(void) state;
	int a = 5;
	int b = 2;
	SWAP(a, b);
	assert_int_equal(b, (int)5);
	assert_int_equal(a, (int)2);
}
// ------------------------------------------------------------------------------------------

void test_swap_uint(void **state)
{
	(void) state;
	unsigned int a = 5;
	unsigned int b = 2;
	SWAP(a, b);
	assert_int_equal(b, (unsigned int)5);
	assert_int_equal(a, (unsigned int)2);
}
// ------------------------------------------------------------------------------------------

void test_swap_long(void **state)
{
	(void) state;
	long int a = 5;
	long int b = 2;
	SWAP(a, b);
	assert_int_equal(b, (long int)5);
	assert_int_equal(a, (long int)2);
}
// ------------------------------------------------------------------------------------------

void test_swap_ulong(void **state)
{
	(void) state;
	unsigned long int a = 5;
	unsigned long int b = 2;
	SWAP(a, b);
	assert_int_equal(b, (unsigned long int)5);
	assert_int_equal(a, (unsigned long int)2);
}
// ------------------------------------------------------------------------------------------

void test_swap_llong(void **state)
{
	(void) state;
	long long int a = 5;
	long long int b = 2;
	SWAP(a, b);
	assert_int_equal(b, (long long int)5);
	assert_int_equal(a, (long long int)2);
}
// ------------------------------------------------------------------------------------------

void test_swap_ullong(void **state)
{
	(void) state;
	unsigned long long int a = 5;
	unsigned long long int b = 2;
	SWAP(a, b);
	assert_int_equal(b, (unsigned long long int)5);
	assert_int_equal(a, (unsigned long long int)2);
}
// ------------------------------------------------------------------------------------------

void test_swap_float(void **state)
{
	(void) state;
	float a = 5.2;
	float b = 2.2;
	SWAP(a, b);
	assert_float_equal(b, 5.2f, 1.0e-4);
	assert_float_equal(a, 2.2f, 1.0e-4);
}
// ------------------------------------------------------------------------------------------

void test_swap_double(void **state)
{
	(void) state;
	double a = 5.2;
	double b = 2.2;
	SWAP(a, b);
	assert_float_equal(b, 5.2, 1.0e-4);
	assert_float_equal(a, 2.2, 1.0e-4);
}
// ------------------------------------------------------------------------------------------

void test_swap_ldouble(void **state)
{
	(void) state;
	long double a = 5.2;
	long double b = 2.2;
	SWAP(a, b);
	assert_float_equal(b, (long double)5.2, 1.0e-4);
	assert_float_equal(a, (long double)2.2, 1.0e-4);
}
// ------------------------------------------------------------------------------------------

void test_swap_char(void **state)
{
	(void) state;
	char a = 'a';
	char b = 'b';
	SWAP(a, b);
	assert_int_equal(b, 'a');
	assert_int_equal(a, 'b');
}
// --------------------------------------------------------------------------------

void test_swap_uchar(void **state)
{
	(void) state;
	unsigned char a = 'a';
	unsigned char b = 'b';
	SWAP(a, b);
	assert_int_equal(b, 'a');
	assert_int_equal(a, 'b');
}
// ------------------------------------------------------------------------------------------

void test_swap_bool(void **state)
{
	(void) state;
	bool a = true;
	bool b = false;
	SWAP(a, b);
	assert_int_equal(b, true);
	assert_int_equal(a, false);
}
// --------------------------------------------------------------------------------
void test_swap_string(void **state)
{
	(void) state;
    str* a = init_string("Hello");
    str* b = init_string("Goodbye");
    swap_string(a, b);
    assert_string_equal(a->data, "Goodbye");
    assert_string_equal(b->data, "Hello");
    free_string(a);
    free_string(b);
}
// ================================================================================
// ================================================================================
// eof
