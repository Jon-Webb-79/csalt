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

void test_pop_char_array(void **state) {
    char a[6];
    char_arr arr = init_array(a, 6, 0);
    push_array(arr, 0, array_length(arr));
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    char b[4] = {1, 2, 3, 4};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
} 
// --------------------------------------------------------------------------------

void test_pop_uchar_array(void **state) {
    unsigned char a[6];
    uchar_arr arr = init_array(a, 6, 0);
    push_array(arr, 0, array_length(arr));
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    unsigned char b[4] = {1, 2, 3, 4};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
}
// --------------------------------------------------------------------------------

void test_pop_short_array(void **state) {
    short int a[6];
    short_arr arr = init_array(a, 6, 0);
    push_array(arr, 0, array_length(arr));
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    short int b[4] = {1, 2, 3, 4};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
}
// --------------------------------------------------------------------------------

void test_pop_ushort_array(void **state) {
    unsigned short int a[6];
    ushort_arr arr = init_array(a, 6, 0);
    push_array(arr, 0, array_length(arr));
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    unsigned short int b[4] = {1, 2, 3, 4};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
}
// --------------------------------------------------------------------------------

void test_pop_int_array(void **state) {
    int a[6];
    int_arr arr = init_array(a, 6, 0);
    push_array(arr, 0, array_length(arr));
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    int b[4] = {1, 2, 3, 4};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
}
// --------------------------------------------------------------------------------

void test_pop_uint_array(void **state) {
    unsigned int a[6];
    uint_arr arr = init_array(a, 6, 0);
    push_array(arr, 0, array_length(arr));
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    unsigned int b[4] = {1, 2, 3, 4};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
}
// --------------------------------------------------------------------------------

void test_pop_long_array(void **state) {
    long int a[6];
    long_arr arr = init_array(a, 6, 0);
    push_array(arr, 0, array_length(arr));
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    long int b[4] = {1, 2, 3, 4};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
}
// --------------------------------------------------------------------------------

void test_pop_ulong_array(void **state) {
    unsigned long int a[6];
    ulong_arr arr = init_array(a, 6, 0);
    push_array(arr, 0, array_length(arr));
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    unsigned long int b[4] = {1, 2, 3, 4};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
}
// --------------------------------------------------------------------------------

void test_pop_llong_array(void **state) {
    long long int a[6];
    llong_arr arr = init_array(a, 6, 0);
    push_array(arr, 0, array_length(arr));
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    long long int b[4] = {1, 2, 3, 4};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
}
// --------------------------------------------------------------------------------

void test_pop_ullong_array(void **state) {
    unsigned long long int a[6];
    ullong_arr arr = init_array(a, 6, 0);
    push_array(arr, 0, array_length(arr));
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    unsigned long long int b[4] = {1, 2, 3, 4};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
}
// --------------------------------------------------------------------------------

void test_pop_float_array(void **state) {
    float a[6];
    float_arr arr = init_array(a, 6, 0);
    push_array(arr, 0., array_length(arr));
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 3., array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    float b[4] = {1., 2., 3., 4.};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_float_equal(b[i], get_array(arr, i), 1.0e-3);
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
}
// --------------------------------------------------------------------------------

void test_pop_double_array(void **state) {
    double a[6];
    double_arr arr = init_array(a, 6, 0);
    push_array(arr, 0., array_length(arr));
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 3., array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    double b[4] = {1., 2., 3., 4.};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_double_equal(b[i], get_array(arr, i), 1.0e-3);
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
}
// --------------------------------------------------------------------------------

void test_pop_ldouble_array(void **state) {
    long double a[6];
    ldouble_arr arr = init_array(a, 6, 0);
    push_array(arr, 0., array_length(arr));
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 3., array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    long double b[4] = {1., 2., 3., 4.};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_double_equal(b[i], get_array(arr, i), 1.0e-3);
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
}
// --------------------------------------------------------------------------------

void test_pop_bool_array(void **state) {
    bool a[6];
    bool_arr arr = init_array(a, 6, 0);
    push_array(arr, true, array_length(arr));
    push_array(arr, false, array_length(arr));
    push_array(arr, true, array_length(arr));
    push_array(arr, false, array_length(arr));
    push_array(arr, true, array_length(arr));
    push_array(arr, false, array_length(arr));
    pop_array(arr, 0);
    pop_array(arr, array_length(arr) - 1);
    bool b[4] = {false, true, false, true};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_float_equal(b[i], get_array(arr, i), 1.0e-3);
    }
    assert_int_equal(6, array_memory(arr));
    assert_int_equal(4, array_length(arr));
}
// ================================================================================
// ================================================================================

void test_reverse_char_array(void **state) {
    char b[6];
    char_arr arr = init_array(b, 6, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    reverse_array(arr);
    char a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(get_array(arr, i), a[i]); 
    }
}
// --------------------------------------------------------------------------------

void test_reverse_uchar_array(void **state) {
    unsigned char b[6];
    uchar_arr arr = init_array(b, 6, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    reverse_array(arr);
    unsigned char a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(get_array(arr, i), a[i]); 
    }
}
// --------------------------------------------------------------------------------

void test_reverse_short_array(void **state) {
    short int b[6];
    short_arr arr = init_array(b, 6, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    reverse_array(arr);
    short int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(get_array(arr, i), a[i]); 
    }
}
// --------------------------------------------------------------------------------

void test_reverse_ushort_array(void **state) {
    unsigned short int b[5];
    ushort_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    reverse_array(arr);
    unsigned short int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(get_array(arr, i), a[i]); 
    }
}
// --------------------------------------------------------------------------------

void test_reverse_int_array(void **state) {
    int b[5];
    int_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    reverse_array(arr);
    int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(get_array(arr, i), a[i]); 
    }
}
// --------------------------------------------------------------------------------

void test_reverse_uint_array(void **state) {
    unsigned int b[5];
    uint_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    reverse_array(arr);
    unsigned int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(get_array(arr, i), a[i]); 
    }
}
// --------------------------------------------------------------------------------

void test_reverse_long_array(void **state) {
    long int b[5];
    long_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    reverse_array(arr);
    long int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(get_array(arr, i), a[i]); 
    }
}
// --------------------------------------------------------------------------------

void test_reverse_ulong_array(void **state) {
    unsigned long int b[5];
    ulong_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    reverse_array(arr);
    unsigned long int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(get_array(arr, i), a[i]); 
    }
}
// --------------------------------------------------------------------------------

void test_reverse_llong_array(void **state) {
    long long int b[5];
    llong_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    reverse_array(arr);
    long long int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(get_array(arr, i), a[i]); 
    }
}
// --------------------------------------------------------------------------------

void test_reverse_ullong_array(void **state) {
    unsigned long long int b[5];
    ullong_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    reverse_array(arr);
    unsigned long long int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(get_array(arr, i), a[i]); 
    }
}
// --------------------------------------------------------------------------------

void test_reverse_float_array(void **state) {
    float b[5];
    float_arr arr = init_array(b, 5, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 3., array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    reverse_array(arr);
    float a[5] = {5., 4., 3., 2., 1.};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_float_equal(get_array(arr, i), a[i], 1.0e-3); 
    }
}
// --------------------------------------------------------------------------------

void test_reverse_double_array(void **state) {
    double b[5];
    double_arr arr = init_array(b, 5, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 3., array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    reverse_array(arr);
    double a[5] = {5., 4., 3., 2., 1.};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_double_equal(get_array(arr, i), a[i], 1.0e-3); 
    }
}
// -------------------------------------------------------------------------------- 

void test_reverse_ldouble_array(void **state) {
    long double b[5];
    ldouble_arr arr = init_array(b, 5, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 3., array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    reverse_array(arr);
    long double a[5] = {5., 4., 3., 2., 1.};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_double_equal(get_array(arr, i), a[i], 1.0e-3); 
    }
}
// --------------------------------------------------------------------------------

void test_reverse_bool_array(void **state) {
    bool b[5];
    bool_arr arr = init_array(b, 5, 0);
    push_array(arr, true, array_length(arr));
    push_array(arr, true, array_length(arr));
    push_array(arr, false, array_length(arr));
    push_array(arr, false, array_length(arr));
    push_array(arr, false, array_length(arr));
    reverse_array(arr);
    bool a[5] = {false, false, false, true, true};
    for (size_t i = 0; i < array_length(arr); i++) {
        assert_int_equal(get_array(arr, i), a[i]); 
    }
}
// ================================================================================
// ================================================================================

void test_bubble_sort_char_array_forward(void **state) {
    char b[7];
    char_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_char_array_reverse(void **state) {
    char b[7];
    char_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, BUBBLE, REVERSE);
    char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uchar_array_forward(void **state) {
    unsigned char b[7];
    uchar_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uchar_array_reverse(void **state) {
    unsigned char b[7];
    uchar_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, BUBBLE, REVERSE);
    unsigned char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_short_array_forward(void **state) {
    short int b[7];
    short_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_short_array_reverse(void **state) {
    short int b[7];
    short_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, BUBBLE, REVERSE);
    short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ushort_array_forward(void **state) {
    unsigned short int b[7];
    ushort_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ushort_array_reverse(void **state) {
    unsigned short int b[7];
    ushort_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, BUBBLE, REVERSE);
    unsigned short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_int_array_forward(void **state) {
    int b[7];
    int_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_int_array_reverse(void **state) {
    int b[7];
    int_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, BUBBLE, REVERSE);
    int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uint_array_forward(void **state) {
    unsigned int b[7];
    uint_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uint_array_reverse(void **state) {
    unsigned int b[7];
    uint_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, BUBBLE, REVERSE);
    unsigned int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_long_array_forward(void **state) {
    long int b[7];
    long_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_long_array_reverse(void **state) {
    long int b[7];
    long_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, BUBBLE, REVERSE);
    long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ulong_array_forward(void **state) {
    unsigned long int b[7];
    ulong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ulong_array_reverse(void **state) {
    unsigned long int b[7];
    ulong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, BUBBLE, REVERSE);
    unsigned long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_llong_array_forward(void **state) {
    long long int b[7];
    llong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_llong_array_reverse(void **state) {
    long long int b[7];
    llong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, BUBBLE, REVERSE);
    long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ullong_array_forward(void **state) {
    unsigned long long int b[7];
    ullong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ullong_array_reverse(void **state) {
    unsigned long long int b[7];
    ullong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, BUBBLE, REVERSE);
    unsigned long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_float_array_forward(void **state) {
    float b[7];
    float_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    float a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_float_array_reverse(void **state) {
    float b[7];
    float_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, BUBBLE, REVERSE);
    float a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_double_array_forward(void **state) {
    double b[7];
    double_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_double_array_reverse(void **state) {
    double b[7];
    double_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, BUBBLE, REVERSE);
    double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ldouble_array_forward(void **state) {
    long double b[7];
    ldouble_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    long double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ldouble_array_reverse(void **state) {
    long double b[7];
    ldouble_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, BUBBLE, REVERSE);
    long double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_bool_array_forward(void **state) {
    bool b[7];
    bool_arr arr = init_array(b, 7, 0);
    push_array(arr, true, 0);
    push_array(arr, false, 1);
    push_array(arr, true, 2);
    push_array(arr, false, 3);
    push_array(arr, true, 4);
    push_array(arr, false, 5);
    push_array(arr, true, 6);
    bool a[7] = {false, false, false, true, true, true, true};
    sort_array(arr, BUBBLE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_bubble_sort_bool_array_reverse(void **state) {
    bool b[7];
    bool_arr arr = init_array(b, 7, 0);
    push_array(arr, true, 0);
    push_array(arr, false, 1);
    push_array(arr, true, 2);
    push_array(arr, false, 3);
    push_array(arr, true, 4);
    push_array(arr, false, 5);
    push_array(arr, true, 6);
    sort_array(arr, BUBBLE, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// ================================================================================
// ================================================================================
// TEST INSERT SORT FUNCTIONS 

void test_insert_sort_char_array_forward(void **state) {
    char b[7];
    char_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_char_array_reverse(void **state) {
    char b[7];
    char_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, INSERT, REVERSE);
    char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_uchar_array_forward(void **state) {
    unsigned char b[7];
    uchar_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_uchar_array_reverse(void **state) {
    unsigned char b[7];
    uchar_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, INSERT, REVERSE);
    unsigned char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_short_array_forward(void **state) {
    short int b[7];
    short_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_short_array_reverse(void **state) {
    short int b[7];
    short_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, INSERT, REVERSE);
    short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_ushort_array_forward(void **state) {
    unsigned short int b[7];
    ushort_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_ushort_array_reverse(void **state) {
    unsigned short int b[7];
    ushort_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, INSERT, REVERSE);
    unsigned short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_int_array_forward(void **state) {
    int b[7];
    int_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_int_array_reverse(void **state) {
    int b[7];
    int_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, INSERT, REVERSE);
    int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_uint_array_forward(void **state) {
    unsigned int b[7];
    uint_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_uint_array_reverse(void **state) {
    unsigned int b[7];
    uint_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, INSERT, REVERSE);
    unsigned int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_long_array_forward(void **state) {
    long int b[7];
    long_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_long_array_reverse(void **state) {
    long int b[7];
    long_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, INSERT, REVERSE);
    long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_ulong_array_forward(void **state) {
    unsigned long int b[7];
    ulong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_ulong_array_reverse(void **state) {
    unsigned long int b[7];
    ulong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, INSERT, REVERSE);
    unsigned long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_llong_array_forward(void **state) {
    long long int b[7];
    llong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_llong_array_reverse(void **state) {
    long long int b[7];
    llong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, INSERT, REVERSE);
    long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_ullong_array_forward(void **state) {
    unsigned long long int b[7];
    ullong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_ullong_array_reverse(void **state) {
    unsigned long long int b[7];
    ullong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, INSERT, REVERSE);
    unsigned long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_float_array_forward(void **state) {
    float b[7];
    float_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    float a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_float_array_reverse(void **state) {
    float b[7];
    float_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, INSERT, REVERSE);
    float a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_double_array_forward(void **state) {
    double b[7];
    double_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_double_array_reverse(void **state) {
    double b[7];
    double_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, INSERT, REVERSE);
    double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_ldouble_array_forward(void **state) {
    long double b[7];
    ldouble_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    long double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_ldouble_array_reverse(void **state) {
    long double b[7];
    ldouble_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, INSERT, REVERSE);
    long double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_bool_array_forward(void **state) {
    bool b[7];
    bool_arr arr = init_array(b, 7, 0);
    push_array(arr, true, 0);
    push_array(arr, false, 1);
    push_array(arr, true, 2);
    push_array(arr, false, 3);
    push_array(arr, true, 4);
    push_array(arr, false, 5);
    push_array(arr, true, 6);
    bool a[7] = {false, false, false, true, true, true, true};
    sort_array(arr, INSERT, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_insert_sort_bool_array_reverse(void **state) {
    bool b[7];
    bool_arr arr = init_array(b, 7, 0);
    push_array(arr, true, 0);
    push_array(arr, false, 1);
    push_array(arr, true, 2);
    push_array(arr, false, 3);
    push_array(arr, true, 4);
    push_array(arr, false, 5);
    push_array(arr, true, 6);
    sort_array(arr, INSERT, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// ================================================================================
// ================================================================================
// TEST MERGE SORT FUNCTIONS 

void test_merge_sort_char_array_forward(void **state) {
    char b[7];
    char_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_char_array_reverse(void **state) {
    char b[7];
    char_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, MERGE, REVERSE);
    char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_uchar_array_forward(void **state) {
    unsigned char b[7];
    uchar_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_uchar_array_reverse(void **state) {
    unsigned char b[7];
    uchar_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, MERGE, REVERSE);
    unsigned char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_short_array_forward(void **state) {
    short int b[7];
    short_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_short_array_reverse(void **state) {
    short int b[7];
    short_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, MERGE, REVERSE);
    short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_ushort_array_forward(void **state) {
    unsigned short int b[7];
    ushort_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_ushort_array_reverse(void **state) {
    unsigned short int b[7];
    ushort_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, MERGE, REVERSE);
    unsigned short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_int_array_forward(void **state) {
    int b[7];
    int_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_int_array_reverse(void **state) {
    int b[7];
    int_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, MERGE, REVERSE);
    int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_uint_array_forward(void **state) {
    unsigned int b[7];
    uint_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_uint_array_reverse(void **state) {
    unsigned int b[7];
    uint_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, MERGE, REVERSE);
    unsigned int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_long_array_forward(void **state) {
    long int b[7];
    long_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_long_array_reverse(void **state) {
    long int b[7];
    long_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, MERGE, REVERSE);
    long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_ulong_array_forward(void **state) {
    unsigned long int b[7];
    ulong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_ulong_array_reverse(void **state) {
    unsigned long int b[7];
    ulong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, MERGE, REVERSE);
    unsigned long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_llong_array_forward(void **state) {
    long long int b[7];
    llong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_llong_array_reverse(void **state) {
    long long int b[7];
    llong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, MERGE, REVERSE);
    long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_ullong_array_forward(void **state) {
    unsigned long long int b[7];
    ullong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_ullong_array_reverse(void **state) {
    unsigned long long int b[7];
    ullong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, MERGE, REVERSE);
    unsigned long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_float_array_forward(void **state) {
    float b[7];
    float_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    float a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_float_array_reverse(void **state) {
    float b[7];
    float_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, MERGE, REVERSE);
    float a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_double_array_forward(void **state) {
    double b[7];
    double_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_double_array_reverse(void **state) {
    double b[7];
    double_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, MERGE, REVERSE);
    double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_ldouble_array_forward(void **state) {
    long double b[7];
    ldouble_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    long double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_ldouble_array_reverse(void **state) {
    long double b[7];
    ldouble_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, MERGE, REVERSE);
    long double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_bool_array_forward(void **state) {
    bool b[7];
    bool_arr arr = init_array(b, 7, 0);
    push_array(arr, true, 0);
    push_array(arr, false, 1);
    push_array(arr, true, 2);
    push_array(arr, false, 3);
    push_array(arr, true, 4);
    push_array(arr, false, 5);
    push_array(arr, true, 6);
    bool a[7] = {false, false, false, true, true, true, true};
    sort_array(arr, MERGE, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_merge_sort_bool_array_reverse(void **state) {
    bool b[7];
    bool_arr arr = init_array(b, 7, 0);
    push_array(arr, true, 0);
    push_array(arr, false, 1);
    push_array(arr, true, 2);
    push_array(arr, false, 3);
    push_array(arr, true, 4);
    push_array(arr, false, 5);
    push_array(arr, true, 6);
    sort_array(arr, MERGE, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// ================================================================================
// ================================================================================

void test_quick_sort_char_array_forward(void **state) {
    char b[7];
    char_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_char_array_reverse(void **state) {
    char b[7];
    char_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, QUICK, REVERSE);
    char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_uchar_array_forward(void **state) {
    unsigned char b[7];
    uchar_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_uchar_array_reverse(void **state) {
    unsigned char b[7];
    uchar_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, QUICK, REVERSE);
    unsigned char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_short_array_forward(void **state) {
    short int b[7];
    short_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_short_array_reverse(void **state) {
    short int b[7];
    short_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, QUICK, REVERSE);
    short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_ushort_array_forward(void **state) {
    unsigned short int b[7];
    ushort_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_ushort_array_reverse(void **state) {
    unsigned short int b[7];
    ushort_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, QUICK, REVERSE);
    unsigned short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_int_array_forward(void **state) {
    int b[7];
    int_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_int_array_reverse(void **state) {
    int b[7];
    int_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, QUICK, REVERSE);
    int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_uint_array_forward(void **state) {
    unsigned int b[7];
    uint_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_uint_array_reverse(void **state) {
    unsigned int b[7];
    uint_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, QUICK, REVERSE);
    unsigned int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_long_array_forward(void **state) {
    long int b[7];
    long_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_long_array_reverse(void **state) {
    long int b[7];
    long_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, QUICK, REVERSE);
    long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_ulong_array_forward(void **state) {
    unsigned long int b[7];
    ulong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_ulong_array_reverse(void **state) {
    unsigned long int b[7];
    ulong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, QUICK, REVERSE);
    unsigned long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_llong_array_forward(void **state) {
    long long int b[7];
    llong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_llong_array_reverse(void **state) {
    long long int b[7];
    llong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, QUICK, REVERSE);
    long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_ullong_array_forward(void **state) {
    unsigned long long int b[7];
    ullong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_ullong_array_reverse(void **state) {
    unsigned long long int b[7];
    ullong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, QUICK, REVERSE);
    unsigned long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_float_array_forward(void **state) {
    float b[7];
    float_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    float a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_float_array_reverse(void **state) {
    float b[7];
    float_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, QUICK, REVERSE);
    float a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_double_array_forward(void **state) {
    double b[7];
    double_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_double_array_reverse(void **state) {
    double b[7];
    double_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, QUICK, REVERSE);
    double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_ldouble_array_forward(void **state) {
    long double b[7];
    ldouble_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    long double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_ldouble_array_reverse(void **state) {
    long double b[7];
    ldouble_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, QUICK, REVERSE);
    long double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_bool_array_forward(void **state) {
    bool b[7];
    bool_arr arr = init_array(b, 7, 0);
    push_array(arr, true, 0);
    push_array(arr, false, 1);
    push_array(arr, true, 2);
    push_array(arr, false, 3);
    push_array(arr, true, 4);
    push_array(arr, false, 5);
    push_array(arr, true, 6);
    bool a[7] = {false, false, false, true, true, true, true};
    sort_array(arr, QUICK, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_quick_sort_bool_array_reverse(void **state) {
    bool b[7];
    bool_arr arr = init_array(b, 7, 0);
    push_array(arr, true, 0);
    push_array(arr, false, 1);
    push_array(arr, true, 2);
    push_array(arr, false, 3);
    push_array(arr, true, 4);
    push_array(arr, false, 5);
    push_array(arr, true, 6);
    sort_array(arr, QUICK, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// ================================================================================
// ================================================================================

void test_tim_sort_char_array_forward(void **state) {
    char b[7];
    char_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_char_array_reverse(void **state) {
    char b[7];
    char_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, TIM, REVERSE);
    char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_uchar_array_forward(void **state) {
    unsigned char b[7];
    uchar_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_uchar_array_reverse(void **state) {
    unsigned char b[7];
    uchar_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, TIM, REVERSE);
    unsigned char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_short_array_forward(void **state) {
    short int b[7];
    short_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_short_array_reverse(void **state) {
    short int b[7];
    short_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, TIM, REVERSE);
    short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_ushort_array_forward(void **state) {
    unsigned short int b[7];
    ushort_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_ushort_array_reverse(void **state) {
    unsigned short int b[7];
    ushort_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, TIM, REVERSE);
    unsigned short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_int_array_forward(void **state) {
    int b[7];
    int_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_int_array_reverse(void **state) {
    int b[7];
    int_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, TIM, REVERSE);
    int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_uint_array_forward(void **state) {
    unsigned int b[7];
    uint_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_uint_array_reverse(void **state) {
    unsigned int b[7];
    uint_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, TIM, REVERSE);
    unsigned int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_long_array_forward(void **state) {
    long int b[7];
    long_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_long_array_reverse(void **state) {
    long int b[7];
    long_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, TIM, REVERSE);
    long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_ulong_array_forward(void **state) {
    unsigned long int b[7];
    ulong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_ulong_array_reverse(void **state) {
    unsigned long int b[7];
    ulong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, TIM, REVERSE);
    unsigned long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_llong_array_forward(void **state) {
    long long int b[7];
    llong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_llong_array_reverse(void **state) {
    long long int b[7];
    llong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, TIM, REVERSE);
    long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_ullong_array_forward(void **state) {
    unsigned long long int b[7];
    ullong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    unsigned long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_ullong_array_reverse(void **state) {
    unsigned long long int b[7];
    ullong_arr arr = init_array(b, 7, 0);
    push_array(arr, 3, 0);
    push_array(arr, 2, 1);
    push_array(arr, 6, 2);
    push_array(arr, 5, 3);
    push_array(arr, 1, 4);
    push_array(arr, 12, 5);
    push_array(arr, 10, 6);
    sort_array(arr, TIM, REVERSE);
    unsigned long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_float_array_forward(void **state) {
    float b[7];
    float_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    float a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_float_array_reverse(void **state) {
    float b[7];
    float_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, TIM, REVERSE);
    float a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_double_array_forward(void **state) {
    double b[7];
    double_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_double_array_reverse(void **state) {
    double b[7];
    double_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, TIM, REVERSE);
    double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_ldouble_array_forward(void **state) {
    long double b[7];
    ldouble_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    long double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_ldouble_array_reverse(void **state) {
    long double b[7];
    ldouble_arr arr = init_array(b, 7, 0);
    push_array(arr, 3., 0);
    push_array(arr, 2., 1);
    push_array(arr, 6., 2);
    push_array(arr, 5., 3);
    push_array(arr, 1., 4);
    push_array(arr, 12., 5);
    push_array(arr, 10., 6);
    sort_array(arr, TIM, REVERSE);
    long double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(a[i], arr.data[i], 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_bool_array_forward(void **state) {
    bool b[7];
    bool_arr arr = init_array(b, 7, 0);
    push_array(arr, true, 0);
    push_array(arr, false, 1);
    push_array(arr, true, 2);
    push_array(arr, false, 3);
    push_array(arr, true, 4);
    push_array(arr, false, 5);
    push_array(arr, true, 6);
    bool a[7] = {false, false, false, true, true, true, true};
    sort_array(arr, TIM, FORWARD);
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// --------------------------------------------------------------------------------

void test_tim_sort_bool_array_reverse(void **state) {
    bool b[7];
    bool_arr arr = init_array(b, 7, 0);
    push_array(arr, true, 0);
    push_array(arr, false, 1);
    push_array(arr, true, 2);
    push_array(arr, false, 3);
    push_array(arr, true, 4);
    push_array(arr, false, 5);
    push_array(arr, true, 6);
    sort_array(arr, TIM, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], arr.data[i]);
    }
}
// ================================================================================
// ================================================================================

void test_max_char_array(void **state) {
    char b[5];
    char_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    char max = max_array(arr);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_uchar_array(void **state) {
    unsigned char b[5];
    uchar_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned char max = max_array(arr);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_short_array(void **state) {
    short int b[5];
    short_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    short int max = max_array(arr);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_ushort_array(void **state) {
    unsigned short int b[5];
    ushort_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned short int max = max_array(arr);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_int_array(void **state) {
    int b[5];
    int_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    int max = max_array(arr);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_uint_array(void **state) {
    unsigned int b[5];
    uint_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned short int max = max_array(arr);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_long_array(void **state) {
    long int b[5];
    long_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    long int max = max_array(arr);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_ulong_array(void **state) {
    unsigned long int b[5];
    ulong_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned long int max = max_array(arr);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_llong_array(void **state) {
    long long int b[5];
    llong_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    long long int max = max_array(arr);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_ullong_array(void **state) {
    unsigned long long int b[5];
    ullong_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned long long int max = max_array(arr);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_float_array(void **state) {
    float b[5];
    float_arr arr = init_array(b, 5, 0);
    push_array(arr, 1.f, array_length(arr));
    push_array(arr, 2.f, array_length(arr));
    push_array(arr, 10.3f, array_length(arr));
    push_array(arr, 4.f, array_length(arr));
    push_array(arr, 5.f, array_length(arr));
    float max = max_array(arr);
    assert_float_equal(max, 10.3, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_max_double_array(void **state) {
    double b[5];
    double_arr arr = init_array(b, 5, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 10.3, array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    double max = max_array(arr);
    assert_double_equal(max, 10.3, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_max_ldouble_array(void **state) {
    long double b[5];
    ldouble_arr arr = init_array(b, 5, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 10.3, array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    long double max = max_array(arr);
    assert_double_equal(max, 10.3, 1.0e-3);
}
// ================================================================================
// ================================================================================

void test_min_char_array(void **state) {
    char b[5];
    char_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    char min = min_array(arr);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_uchar_array(void **state) {
    unsigned char b[5];
    uchar_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned char min = min_array(arr);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_short_array(void **state) {
    short int b[5];
    short_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    short int min = min_array(arr);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_ushort_array(void **state) {
    unsigned short int b[5];
    ushort_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned short int min = min_array(arr);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_int_array(void **state) {
    int b[5];
    int_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    int min = min_array(arr);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_uint_array(void **state) {
    unsigned int b[5];
    uint_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned short int min = min_array(arr);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_long_array(void **state) {
    long int b[5];
    long_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    long int min = min_array(arr);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_ulong_array(void **state) {
    unsigned long int b[5];
    ulong_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned long int min = min_array(arr);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_llong_array(void **state) {
    long long int b[5];
    llong_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    long long int min = min_array(arr);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_ullong_array(void **state) {
    unsigned long long int b[5];
    ullong_arr arr = init_array(b, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned long long int min = min_array(arr);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_float_array(void **state) {
    float b[5];
    float_arr arr = init_array(b, 5, 0);
    push_array(arr, 1.f, array_length(arr));
    push_array(arr, 2.f, array_length(arr));
    push_array(arr, 10.3f, array_length(arr));
    push_array(arr, 4.f, array_length(arr));
    push_array(arr, 5.f, array_length(arr));
    float min = min_array(arr);
    assert_float_equal(min, 1.0, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_min_double_array(void **state) {
    double b[5];
    double_arr arr = init_array(b, 5, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 10.3, array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    double min = min_array(arr);
    assert_double_equal(min, 1.0, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_min_ldouble_array(void **state) {
    long double b[5];
    ldouble_arr arr = init_array(b, 5, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 10.3, array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    long double min = min_array(arr);
    assert_double_equal(min, 1.0, 1.0e-3);
}  
// ================================================================================
// ================================================================================
// TEST BINARY SEARCH FUNCTIONS 

void test_binary_search_char_array(void **state) {
    char c[20];
    char_arr arr = init_array(c, 20, 0);
    char a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, 13, false);
    assert_int_equal(arr.data[(size_t)b], 13);
}
// --------------------------------------------------------------------------------

void test_binary_search_uchar_array(void **state) {
    unsigned char c[20];
    uchar_arr arr = init_array(c, 20, 0);
    unsigned char a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, 13, false);
    assert_int_equal(arr.data[(size_t)b], 13);
}
// --------------------------------------------------------------------------------

void test_binary_search_short_array(void **state) {
    short int c[20];
    short_arr arr = init_array(c, 20, 0);
    short int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, 13, false);
    assert_int_equal(arr.data[(size_t)b], 13);
}
// --------------------------------------------------------------------------------

void test_binary_search_ushort_array(void **state) {
    unsigned short int c[20];
    ushort_arr arr = init_array(c, 20, 0);
    unsigned short int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, 13, false);
    assert_int_equal(arr.data[(size_t)b], 13);
}
// --------------------------------------------------------------------------------

void test_binary_search_int_array(void **state) {
    int c[20];
    int_arr arr = init_array(c, 20, 0);
    int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, 13, false);
    assert_int_equal(arr.data[(size_t)b], 13);
}
// --------------------------------------------------------------------------------

void test_binary_search_uint_array(void **state) {
    unsigned int c[20];
    uint_arr arr = init_array(c, 20, 0);
    unsigned int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, 13, false);
    assert_int_equal(arr.data[(size_t)b], 13);
}
// --------------------------------------------------------------------------------

void test_binary_search_long_array(void **state) {
    long int c[20];
    long_arr arr = init_array(c, 20, 0);
    long int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, 13, false);
    assert_int_equal(arr.data[(size_t)b], 13);
}
// --------------------------------------------------------------------------------

void test_binary_search_ulong_array(void **state) {
    unsigned long int c[20];
    ulong_arr arr = init_array(c, 20, 0);
    unsigned long int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, 13, false);
    assert_int_equal(arr.data[(size_t)b], 13);
}
// --------------------------------------------------------------------------------

void test_binary_search_llong_array(void **state) {
    long long int c[20];
    llong_arr arr = init_array(c, 20, 0);
    short int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, 13, false);
    assert_int_equal(arr.data[(size_t)b], 13);
}
// --------------------------------------------------------------------------------

void test_binary_search_ullong_array(void **state) {
    unsigned long long int c[20];
    ullong_arr arr = init_array(c, 20, 0);
    unsigned long long int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, 13, false);
    assert_int_equal(arr.data[(size_t)b], 13);
}
// --------------------------------------------------------------------------------

void test_binary_search_float_array(void **state) {
    float c[20];
    float_arr arr = init_array(c, 20, 0);
    float a[20] = { 1.2, 5.6, 2.1, 7.7, 8., 9., 4.2, 3.8, 6.1, 14.4, 13.6, 
                    12.9, 11.11, 10.01, 16.3, 17.4, 18.5, 19.6, 20.7, 15.8 };
    for (size_t i = 0; i < 20; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, 11.11, false);
    assert_float_equal(arr.data[(size_t)b], 11.11, 1.0e-6);
}
// --------------------------------------------------------------------------------

void test_binary_search_double_array(void **state) {
    double c[20];
    double_arr arr = init_array(c, 20, 0);
    double a[20] = { 1.2, 5.6, 2.1, 7.7, 8., 9., 4.2, 3.8, 6.1, 14.4, 13.6, 
                    12.9, 11.11, 10.01, 16.3, 17.4, 18.5, 19.6, 20.7, 15.8 };
    for (size_t i = 0; i < 20; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, 11.11, false);
    assert_double_equal(arr.data[(size_t)b], 11.11, 1.0e-6);
}
// --------------------------------------------------------------------------------

void test_binary_search_ldouble_array(void **state) {
    long double c[20];
    ldouble_arr arr = init_array(c, 20, 0);
    long double a[20] = { 1.2, 5.6, 2.1, 7.7, 8., 9., 4.2, 3.8, 6.1, 14.4, 13.6, 
                    12.9, 11.11, 10.01, 16.3, 17.4, 18.5, 19.6, 20.7, 15.8 };
    for (size_t i = 0; i < 20; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, 11.11, false);
    assert_double_equal(arr.data[(size_t)b], 11.11, 1.0e-6);
}
// --------------------------------------------------------------------------------

void test_binary_search_bool_array(void **state) {
    bool c[20];
    bool_arr arr = init_array(c, 5, 0);
    bool a[5] = {false, true, false, true, false};
    for (size_t i = 0; i < 5; i++) {
        push_array(arr, a[i], array_length(arr));
    }
    int b = bsearch_array(arr, true, false);
    assert_int_equal(arr.data[(size_t)b], true);
}
// ================================================================================
// ================================================================================

void test_sum_char_array(void **state) {
    char a[5];
    char_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    char sum = sum_array(arr);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_uchar_array(void **state) {
    unsigned char a[5];
    uchar_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned char sum = sum_array(arr);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_short_array(void **state) {
    short int a[5];
    short_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    short int sum = sum_array(arr);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_ushort_array(void **state) {
    unsigned short int a[5];
    ushort_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned short int sum = sum_array(arr);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_int_array(void **state) {
    int a[5];
    int_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    int sum = sum_array(arr);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_uint_array(void **state) {
    unsigned int a[5];
    uint_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned short int sum = sum_array(arr);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_long_array(void **state) {
    long int a[5];
    long_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    long int sum = sum_array(arr);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_ulong_array(void **state) {
    unsigned long int a[5];
    ulong_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned long int sum = sum_array(arr);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_llong_array(void **state) {
    long long int a[5];
    llong_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    long long int sum = sum_array(arr);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_ullong_array(void **state) {
    unsigned long long int a[5];
    ullong_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    unsigned long long int sum = sum_array(arr);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_float_array(void **state) {
    float a[5];
    float_arr arr = init_array(a, 5, 0);
    push_array(arr, 1.f, array_length(arr));
    push_array(arr, 2.f, array_length(arr));
    push_array(arr, 10.3f, array_length(arr));
    push_array(arr, 4.f, array_length(arr));
    push_array(arr, 5.f, array_length(arr));
    float sum = sum_array(arr);
    assert_float_equal(sum, 22.3f, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_sum_double_array(void **state) {
    double a[5];
    double_arr arr = init_array(a, 5, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 10.3, array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    double sum = sum_array(arr);
    assert_double_equal(sum, 22.3, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_sum_ldouble_array(void **state) {
    long double a[5];
    ldouble_arr arr = init_array(a, 5, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 10.3, array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    long double sum = sum_array(arr);
    assert_double_equal(sum, 22.3, 1.0e-3);
}
// ================================================================================
// ================================================================================
// eof
