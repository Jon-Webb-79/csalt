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
// TEST AVERAGE VECTOR FUNCTIONS 

void test_average_char_array(void **state) {
    char a[5];
    char_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    float average = array_average(arr);
    assert_float_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_uchar_array(void **state) {
    unsigned char a[5];
    uchar_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    float average = array_average(arr);
    assert_float_equal(average, 4.40, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_short_array(void **state) {
    short int a[5];
    short_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    float average = array_average(arr);
    assert_float_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_ushort_array(void **state) {
    unsigned short int a[5];
    ushort_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    float average = array_average(arr);
    assert_float_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_int_array(void **state) {
    int a[5];
    int_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    double average = array_average(arr);
    assert_double_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_uint_array(void **state) {
    unsigned int a[5];
    uint_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    double average = array_average(arr);
    assert_double_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_long_array(void **state) {
    long int a[5];
    long_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    double average = array_average(arr);
    assert_double_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_ulong_array(void **state) {
    unsigned long int a[5];
    ulong_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    double average = array_average(arr);
    assert_float_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_llong_array(void **state) {
    long long int a[5];
    llong_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    long double average = array_average(arr);
    assert_double_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_ullong_array(void **state) {
    unsigned long long int a[5];
    ullong_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    long double average = array_average(arr);
    assert_double_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_float_array(void **state) {
    float a[5];
    float_arr arr = init_array(a, 5, 0);
    push_array(arr, 1.f, array_length(arr));
    push_array(arr, 2.f, array_length(arr));
    push_array(arr, 10.3f, array_length(arr));
    push_array(arr, 4.f, array_length(arr));
    push_array(arr, 5.f, array_length(arr));
    float average = array_average(arr);
    assert_float_equal(average, 4.46f, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_double_array(void **state) {
    double a[5];
    double_arr arr = init_array(a, 5, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 10.3, array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    double average = array_average(arr);
    assert_double_equal(average, 4.46, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_ldouble_array(void **state) {
    long double a[5];
    ldouble_arr arr = init_array(a, 5, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 10.3, array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    long double average = array_average(arr);
    assert_double_equal(average, 4.46, 1.0e-3);
}
// ================================================================================
// ================================================================================
// TEST STDEV FUNCTIONS 

void test_stdev_char_array(void **state) {
    char a[5];
    char_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    float stdev = array_stdev(arr);
    assert_float_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_uchar_array(void **state) {
    unsigned char a[5];
    uchar_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    float stdev = array_stdev(arr);
    assert_float_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_short_array(void **state) {
    short int a[5];
    short_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    float stdev = array_stdev(arr);
    assert_float_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_ushort_array(void **state) {
    unsigned short int a[5];
    ushort_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    float stdev = array_stdev(arr);
    assert_float_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_int_array(void **state) {
    int a[5];
    int_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    double stdev = array_stdev(arr);
    assert_double_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_uint_array(void **state) {
    unsigned int a[5];
    uint_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    double stdev = array_stdev(arr);
    assert_double_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_long_array(void **state) {
    long int a[5];
    long_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    double stdev = array_stdev(arr);
    assert_double_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_ulong_array(void **state) {
    unsigned long int a[5];
    ulong_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    double stdev = array_stdev(arr);
    assert_float_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_llong_array(void **state) {
    long long int a[5];
    llong_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    long double stdev = array_stdev(arr);
    assert_double_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_ullong_array(void **state) {
    unsigned long long int a[5];
    ullong_arr arr = init_array(a, 5, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 10, array_length(arr));
    push_array(arr, 4, array_length(arr));
    push_array(arr, 5, array_length(arr));
    long double stdev = array_stdev(arr);
    assert_double_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_float_array(void **state) {
    float a[5];
    float_arr arr = init_array(a, 5, 0);
    push_array(arr, 1.f, array_length(arr));
    push_array(arr, 2.f, array_length(arr));
    push_array(arr, 10.3f, array_length(arr));
    push_array(arr, 4.f, array_length(arr));
    push_array(arr, 5.f, array_length(arr));
    float stdev = array_stdev(arr);
    assert_float_equal(stdev, 3.24444, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_double_array(void **state) {
    double a[5];
    double_arr arr = init_array(a, 5, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 10.3, array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    double stdev = array_stdev(arr);
    assert_double_equal(stdev, 3.24444, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_ldouble_array(void **state) {
    long double a[5];
    ldouble_arr arr = init_array(a, 5, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 10.3, array_length(arr));
    push_array(arr, 4., array_length(arr));
    push_array(arr, 5., array_length(arr));
    long double stdev = array_stdev(arr);
    assert_double_equal(stdev, 3.24444, 1.0e-3);
}
// ================================================================================
// ================================================================================
// TEST REPLACE ARRAY FUNCTIONS 

void test_replace_char_array(void **state) {
    char b[4];
    char_arr arr = init_array(b, 4, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    replace_array_index(arr, 5, 1);
    char a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_replace_uchar_array(void **state) {
    unsigned char b[4];
    uchar_arr arr = init_array(b, 4, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    replace_array_index(arr, 5, 1);
    unsigned char a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_replace_short_array(void **state) {
    short int b[4];
    short_arr arr = init_array(b, 4, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    replace_array_index(arr, 5, 1);
    short int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_replace_ushort_array(void **state) {
    unsigned short int b[4];
    ushort_arr arr = init_array(b, 4, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    replace_array_index(arr, 5, 1);
    unsigned short int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_replace_int_array(void **state) {
    int b[4];
    int_arr arr = init_array(b, 4, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    replace_array_index(arr, 5, 1);
    int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_replace_uint_array(void **state) {
    unsigned int b[4];
    uint_arr arr = init_array(b, 4, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    replace_array_index(arr, 5, 1);
    unsigned int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_replace_long_array(void **state) {
    long int b[4];
    long_arr arr = init_array(b, 4, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    replace_array_index(arr, 5, 1);
    long int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_replace_ulong_array(void **state) {
    unsigned long int b[4];
    ulong_arr arr = init_array(b, 4, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    replace_array_index(arr, 5, 1);
    unsigned long int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_replace_llong_array(void **state) {
    long long int b[4];
    llong_arr arr = init_array(b, 4, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    replace_array_index(arr, 5, 1);
    long long int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_replace_ullong_array(void **state) {
    unsigned long long int b[4];
    ullong_arr arr = init_array(b, 4, 0);
    push_array(arr, 1, array_length(arr));
    push_array(arr, 2, array_length(arr));
    push_array(arr, 3, array_length(arr));
    push_array(arr, 4, array_length(arr));
    replace_array_index(arr, 5, 1);
    unsigned long long int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_replace_float_array(void **state) {
    float b[4];
    float_arr arr = init_array(b, 4, 0);
    push_array(arr, 1.f, array_length(arr));
    push_array(arr, 2.f, array_length(arr));
    push_array(arr, 3.f, array_length(arr));
    push_array(arr, 4.f, array_length(arr));
    replace_array_index(arr, 5.f, 1);
    float a[4] = {1., 5., 3., 4.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], get_array(arr, i), 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_replace_double_array(void **state) {
    double b[4];
    double_arr arr = init_array(b, 4, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 3., array_length(arr));
    push_array(arr, 4., array_length(arr));
    replace_array_index(arr, 5., 1);
    double a[4] = {1., 5., 3., 4.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], get_array(arr, i), 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_replace_ldouble_array(void **state) {
    long double b[4];
    ldouble_arr arr = init_array(b, 4, 0);
    push_array(arr, 1., array_length(arr));
    push_array(arr, 2., array_length(arr));
    push_array(arr, 3., array_length(arr));
    push_array(arr, 4., array_length(arr));
    replace_array_index(arr, 5., 1);
    long double a[4] = {1., 5., 3., 4.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(a[i], get_array(arr, i), 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_replace_bool_array(void **state) {
    bool b[4];
    bool_arr arr = init_array(b, 4, 0);
    push_array(arr, true, array_length(arr));
    push_array(arr, true, array_length(arr));
    push_array(arr, true, array_length(arr));
    push_array(arr, true, array_length(arr));
    replace_array_index(arr, false, 1);
    bool a[4] = {true, false, true, true};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(a[i], get_array(arr, i));
    }
}
// ================================================================================
// ================================================================================
// TEST ITERATOR FUNCTIONS 

void test_iter_char_array(void **state) {
    char c[5];
    char_arr arr = init_array(c, 5, 0);
    push_array(arr, 1, 0);
    push_array(arr, 2, 1);
    push_array(arr, 3, 2);
    push_array(arr, 4, 3);
    push_array(arr, 5, 4);
    char_arr_iterator it = array_iterator(arr);
    char* begin = it.begin(&arr);
    char* end = it.end(&arr);
    char a;
    for (char* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    char b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_iter_uchar_array(void **state) {
    unsigned char c[5];
    uchar_arr arr = init_array(c, 5, 0);
    push_array(arr, 1, 0);
    push_array(arr, 2, 1);
    push_array(arr, 3, 2);
    push_array(arr, 4, 3);
    push_array(arr, 5, 4);
    uchar_arr_iterator it = array_iterator(arr);
    unsigned char* begin = it.begin(&arr);
    unsigned char* end = it.end(&arr);
    unsigned char a;
    for (unsigned char* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    unsigned char b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_iter_short_array(void **state) {
    short int c[5];
    short_arr arr = init_array(c, 5, 0);
    push_array(arr, 1, 0);
    push_array(arr, 2, 1);
    push_array(arr, 3, 2);
    push_array(arr, 4, 3);
    push_array(arr, 5, 4);
    short_arr_iterator it = array_iterator(arr);
    short int* begin = it.begin(&arr);
    short int* end = it.end(&arr);
    short int a;
    for (short int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    short int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_iter_ushort_array(void **state) {
    unsigned short int c[5];
    ushort_arr arr = init_array(c, 5, 0);
    push_array(arr, 1, 0);
    push_array(arr, 2, 1);
    push_array(arr, 3, 2);
    push_array(arr, 4, 3);
    push_array(arr, 5, 4);
    ushort_arr_iterator it = array_iterator(arr);
    unsigned short int* begin = it.begin(&arr);
    unsigned short int* end = it.end(&arr);
    unsigned short int a;
    for (unsigned short int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    unsigned short int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_iter_int_array(void **state) {
    int c[5];
    int_arr arr = init_array(c, 5, 0);
    push_array(arr, 1, 0);
    push_array(arr, 2, 1);
    push_array(arr, 3, 2);
    push_array(arr, 4, 3);
    push_array(arr, 5, 4);
    int_arr_iterator it = array_iterator(arr);
    int* begin = it.begin(&arr);
    int* end = it.end(&arr);
    int a;
    for (int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_iter_uint_array(void **state) {
    unsigned int c[5];
    uint_arr arr = init_array(c, 5, 0);
    push_array(arr, 1, 0);
    push_array(arr, 2, 1);
    push_array(arr, 3, 2);
    push_array(arr, 4, 3);
    push_array(arr, 5, 4);
    uint_arr_iterator it = array_iterator(arr);
    unsigned int* begin = it.begin(&arr);
    unsigned int* end = it.end(&arr);
    unsigned int a;
    for (unsigned int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    unsigned int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_iter_long_array(void **state) {
    long int c[5];
    long_arr arr = init_array(c, 5, 0);
    push_array(arr, 1, 0);
    push_array(arr, 2, 1);
    push_array(arr, 3, 2);
    push_array(arr, 4, 3);
    push_array(arr, 5, 4);
    long_arr_iterator it = array_iterator(arr);
    long int* begin = it.begin(&arr);
    long int* end = it.end(&arr);
    long int a;
    for (long int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    long int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_iter_ulong_array(void **state) {
    unsigned long int c[5];
    ulong_arr arr = init_array(c, 5, 0);
    push_array(arr, 1, 0);
    push_array(arr, 2, 1);
    push_array(arr, 3, 2);
    push_array(arr, 4, 3);
    push_array(arr, 5, 4);
    ulong_arr_iterator it = array_iterator(arr);
    unsigned long int* begin = it.begin(&arr);
    unsigned long int* end = it.end(&arr);
    unsigned long int a;
    for (unsigned long int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    unsigned long int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_iter_llong_array(void **state) {
    long long int c[5];
    llong_arr arr = init_array(c, 5, 0);
    push_array(arr, 1, 0);
    push_array(arr, 2, 1);
    push_array(arr, 3, 2);
    push_array(arr, 4, 3);
    push_array(arr, 5, 4);
    llong_arr_iterator it = array_iterator(arr);
    long long int* begin = it.begin(&arr);
    long long int* end = it.end(&arr);
    long long int a;
    for (long long int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    long long int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_iter_ullong_array(void **state) {
    unsigned long long int c[5];
    ullong_arr arr = init_array(c, 5, 0);
    push_array(arr, 1, 0);
    push_array(arr, 2, 1);
    push_array(arr, 3, 2);
    push_array(arr, 4, 3);
    push_array(arr, 5, 4);
    ullong_arr_iterator it = array_iterator(arr);
    unsigned long long int* begin = it.begin(&arr);
    unsigned long long int* end = it.end(&arr);
    unsigned long long int a;
    for (unsigned long long int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    unsigned long long int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
}
// --------------------------------------------------------------------------------

void test_iter_float_array(void **state) {
    float c[5];
    float_arr arr = init_array(c, 5, 0);
    push_array(arr, 1., 0);
    push_array(arr, 2., 1);
    push_array(arr, 3., 2);
    push_array(arr, 4., 3);
    push_array(arr, 5., 4);
    float_arr_iterator it = array_iterator(arr);
    float* begin = it.begin(&arr);
    float* end = it.end(&arr);
    float a;
    for (float* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10.f;
    }
    float b[5] = {11., 12., 13., 14., 15.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_float_equal(b[i], get_array(arr, i), 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_iter_double_array(void **state) {
    double c[5];
    double_arr arr = init_array(c, 5, 0);
    push_array(arr, 1., 0);
    push_array(arr, 2., 1);
    push_array(arr, 3., 2);
    push_array(arr, 4., 3);
    push_array(arr, 5., 4);
    double_arr_iterator it = array_iterator(arr);
    double* begin = it.begin(&arr);
    double* end = it.end(&arr);
    double a;
    for (double* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10.f;
    }
    double b[5] = {11., 12., 13., 14., 15.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(b[i], get_array(arr, i), 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_iter_ldouble_array(void **state) {
    long double c[5];
    ldouble_arr arr = init_array(c, 5, 0);
    push_array(arr, 1., 0);
    push_array(arr, 2., 1);
    push_array(arr, 3., 2);
    push_array(arr, 4., 3);
    push_array(arr, 5., 4);
    ldouble_arr_iterator it = array_iterator(arr);
    long double* begin = it.begin(&arr);
    long double* end = it.end(&arr);
    long double a;
    for (long double* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10.f;
    }
    long double b[5] = {11., 12., 13., 14., 15.};
    for (size_t i = 0; i < arr.len; i++) {
        assert_double_equal(b[i], get_array(arr, i), 1.0e-3);
    }
}
// --------------------------------------------------------------------------------

void test_iter_bool_array(void **state) {
    bool c[5];
    bool_arr arr = init_array(c, 5, 0);
    push_array(arr, false, 0);
    push_array(arr, false, 1);
    push_array(arr, false, 2);
    push_array(arr, false, 3);
    push_array(arr, false, 4);
    bool_arr_iterator it = array_iterator(arr);
    bool* begin = it.begin(&arr);
    bool* end = it.end(&arr);
    for (bool* i = begin; i != end; it.next(&i)) {
        *i = true;
    }
    bool b[5] = {true, true, true, true, true};
    for (size_t i = 0; i < arr.len; i++) {
        assert_int_equal(b[i], get_array(arr, i));
    }
}
// ================================================================================
// ================================================================================
// eof
