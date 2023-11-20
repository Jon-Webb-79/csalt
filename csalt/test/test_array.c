// ================================================================================
// ================================================================================
// - File:    test_array.c
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

#include "test_array.h"

/**
 * Test the basic vector attributes of init, push and free for char data type.
 */
void test_init_char_array(void **state)
{
	(void) state;
    char val[20];
    char_arr arr = init_array(val, 20, 0);
    push_array(arr, 1, arr.len);
    push_array(arr, 2, arr.len);
    push_array(arr, 3, arr.len);
    push_array(arr, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// --------------------------------------------------------------------------------
/**
 * Test the basic vector attributes of init, push and free for char data type.
 */
void test_init_uchar_array(void **state)
{
	(void) state;
    unsigned char val[20];
    uchar_arr arr = init_array(val, 20, 0);
    push_array(arr, 1, arr.len);
    push_array(arr, 2, arr.len);
    push_array(arr, 3, arr.len);
    push_array(arr, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// --------------------------------------------------------------------------------

void test_init_short_array(void **state)
{
	(void) state;
    short int val[20];
    short_arr arr = init_array(val, 20, 0);
    push_array(arr, 1, arr.len);
    push_array(arr, 2, arr.len);
    push_array(arr, 3, arr.len);
    push_array(arr, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// --------------------------------------------------------------------------------

void test_init_ushort_array(void **state)
{
	(void) state;
    unsigned short int val[20];
    ushort_arr arr = init_array(val, 20, 0);
    push_array(arr, 1, arr.len);
    push_array(arr, 2, arr.len);
    push_array(arr, 3, arr.len);
    push_array(arr, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// --------------------------------------------------------------------------------

void test_init_int_array(void **state)
{
	(void) state;
    int val[20];
    int_arr arr = init_array(val, 20, 0);
    push_array(arr, 1, arr.len);
    push_array(arr, 2, arr.len);
    push_array(arr, 3, arr.len);
    push_array(arr, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// --------------------------------------------------------------------------------

void test_init_uint_array(void **state)
{
	(void) state;
    unsigned int val[20];
    uint_arr arr = init_array(val, 20, 0);
    push_array(arr, 1, arr.len);
    push_array(arr, 2, arr.len);
    push_array(arr, 3, arr.len);
    push_array(arr, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// --------------------------------------------------------------------------------

void test_init_long_array(void **state)
{
	(void) state;
    long int val[20];
    long_arr arr = init_array(val, 20, 0);
    push_array(arr, 1, arr.len);
    push_array(arr, 2, arr.len);
    push_array(arr, 3, arr.len);
    push_array(arr, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// --------------------------------------------------------------------------------

void test_init_ulong_array(void **state)
{
	(void) state;
    unsigned long int val[20];
    ulong_arr arr = init_array(val, 20, 0);
    push_array(arr, 1, arr.len);
    push_array(arr, 2, arr.len);
    push_array(arr, 3, arr.len);
    push_array(arr, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// --------------------------------------------------------------------------------

void test_init_llong_array(void **state)
{
	(void) state;
    long long int val[20];
    llong_arr arr = init_array(val, 20, 0);
    push_array(arr, 1, arr.len);
    push_array(arr, 2, arr.len);
    push_array(arr, 3, arr.len);
    push_array(arr, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// --------------------------------------------------------------------------------

void test_init_ullong_array(void **state)
{
	(void) state;
    unsigned long long int val[20];
    ullong_arr arr = init_array(val, 20, 0);
    push_array(arr, 1, arr.len);
    push_array(arr, 2, arr.len);
    push_array(arr, 3, arr.len);
    push_array(arr, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// --------------------------------------------------------------------------------

void test_init_float_array(void **state)
{
	(void) state;
    float val[20];
    float_arr arr = init_array(val, 20, 0);
    push_array(arr, 1., arr.len);
    push_array(arr, 2., arr.len);
    push_array(arr, 3., arr.len);
    push_array(arr, 4., 0);
    float a[4] = { 4., 1., 2., 3. };
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], get_array(arr, i), 1.0e-3);
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// --------------------------------------------------------------------------------

void test_init_double_array(void **state)
{
	(void) state;
    double val[20];
    double_arr arr = init_array(val, 20, 0);
    push_array(arr, 1., arr.len);
    push_array(arr, 2., arr.len);
    push_array(arr, 3., arr.len);
    push_array(arr, 4., 0);
    double a[4] = { 4., 1., 2., 3. };
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], get_array(arr, i), 1.0e-3);
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// --------------------------------------------------------------------------------

void test_init_ldouble_array(void **state)
{
	(void) state;
    long double val[20];
    ldouble_arr arr = init_array(val, 20, 0);
    push_array(arr, 1., arr.len);
    push_array(arr, 2., arr.len);
    push_array(arr, 3., arr.len);
    push_array(arr, 4., 0);
    long double a[4] = { 4., 1., 2., 3. };
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], get_array(arr, i), 1.0e-3);
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// --------------------------------------------------------------------------------

void test_init_bool_array(void **state)
{
	(void) state;
    bool val[20];
    bool_arr arr = init_array(val, 20, 0);
    push_array(arr, true, arr.len);
    push_array(arr, false, arr.len);
    push_array(arr, true, arr.len);
    push_array(arr, false, 0);
    bool a[4] = { false, true, false, true };
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
    assert_int_equal(array_memory(arr), 20);
    assert_int_equal(array_length(arr), 4);
}
// ================================================================================
// ================================================================================
// eof
