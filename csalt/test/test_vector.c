// ================================================================================
// ================================================================================
// - File:    test_vector.c
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

#include "test_vector.h"
// ================================================================================
// ================================================================================
// TEST INIT< PUSH, and FREE FUNCTIONS

/**
 * Test the basic vector attributes of init, push and free for char data type.
 */
void test_init_char_vector(void **state)
{
	(void) state;
    char_v* vec = init_vector(dChar)(20);
    push_vector(vec, 1, vec->len);
    push_vector(vec, 2, vec->len);
    push_vector(vec, 3, vec->len);
    push_vector(vec, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for unsigned char data type.
 */
void test_init_uchar_vector(void **state)
{
	(void) state;
    uchar_v* vec = init_vector(dUChar)(20);
    push_vector(vec, 1, vec->len);
    push_vector(vec, 2, vec->len);
    push_vector(vec, 3, vec->len);
    push_vector(vec, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for short int data type.
 */
void test_init_short_vector(void **state)
{
	(void) state;
    short_v* vec = init_vector(dShort)(20);
    push_vector(vec, 1, vec->len);
    push_vector(vec, 2, vec->len);
    push_vector(vec, 3, vec->len);
    push_vector(vec, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for unsigned short int data type.
 */
void test_init_ushort_vector(void **state)
{
	(void) state;
    ushort_v* vec = init_vector(dUShort)(20);
    push_vector(vec, 1, vec->len);
    push_vector(vec, 2, vec->len);
    push_vector(vec, 3, vec->len);
    push_vector(vec, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for int data type.
 */
void test_init_int_vector(void **state)
{
	(void) state;
    int_v* vec = init_vector(dInt)(20);
    push_vector(vec, 1, vec->len);
    push_vector(vec, 2, vec->len);
    push_vector(vec, 3, vec->len);
    push_vector(vec, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for unsigned int data type.
 */
void test_init_uint_vector(void **state)
{
	(void) state;
    uint_v* vec = init_vector(dUInt)(20);
    push_vector(vec, 1, vec->len);
    push_vector(vec, 2, vec->len);
    push_vector(vec, 3, vec->len);
    push_vector(vec, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for long int data type.
 */
void test_init_long_vector(void **state)
{
	(void) state;
    long_v* vec = init_vector(dLong)(20);
    push_vector(vec, 1, vec->len);
    push_vector(vec, 2, vec->len);
    push_vector(vec, 3, vec->len);
    push_vector(vec, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < vec->len; i++) {
         assert_int_equal(a[i], get_vector(vec, i));
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for unsigned long int data type.
 */
void test_init_ulong_vector(void **state)
{
	(void) state;
    ulong_v* vec = init_vector(dULong)(20);
    push_vector(vec, 1, vec->len);
    push_vector(vec, 2, vec->len);
    push_vector(vec, 3, vec->len);
    push_vector(vec, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for long long int data type.
 */
void test_init_llong_vector(void **state)
{
	(void) state;
    llong_v* vec = init_vector(dLongLong)(20);
    push_vector(vec, 1, vec->len);
    push_vector(vec, 2, vec->len);
    push_vector(vec, 3, vec->len);
    push_vector(vec, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for unsigned long long int data type.
 */
void test_init_ullong_vector(void **state)
{
	(void) state;
    ullong_v* vec = init_vector(dULongLong)(20);
    push_vector(vec, 1, vec->len);
    push_vector(vec, 2, vec->len);
    push_vector(vec, 3, vec->len);
    push_vector(vec, 4, 0);
    int a[4] = { 4, 1, 2, 3 };
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for float data type.
 */
void test_init_float_vector(void **state)
{
	(void) state;
    float_v* vec = init_vector(dFloat)(20);
    push_vector(vec, 1.1f, vec->len);
    push_vector(vec, 2.2f, vec->len);
    push_vector(vec, 3.3f, vec->len);
    push_vector(vec, 4.4f, 0);
    float a[4] = { 4.4, 1.1, 2.2, 3.3 };
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], get_vector(vec, i), 1.0e-3);
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for double data type.
 */
void test_init_double_vector(void **state)
{
	(void) state;
    double_v* vec = init_vector(dDouble)(20);
    push_vector(vec, 1.1, vec->len);
    push_vector(vec, 2.2, vec->len);
    push_vector(vec, 3.3, vec->len);
    push_vector(vec, 4.4, 0);
    double a[4] = { 4.4, 1.1, 2.2, 3.3 };
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], get_vector(vec, i), 1.0e-3);
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for long double data type.
 */
void test_init_ldouble_vector(void **state)
{
	(void) state;
    ldouble_v* vec = init_vector(dLDouble)(20);
    push_vector(vec, 1.1, vec->len);
    push_vector(vec, 2.2, vec->len);
    push_vector(vec, 3.3, vec->len);
    push_vector(vec, 4.4, 0);
    double a[4] = { 4.4, 1.1, 2.2, 3.3 };
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], get_vector(vec, i), 1.0e-3);
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for bool data type.
 */
void test_init_bool_vector(void **state)
{
	(void) state;
    bool_v* vec = init_vector(dBool)(20);
    push_vector(vec, true, vec->len);
    push_vector(vec, true, vec->len);
    push_vector(vec, false, vec->len);
    push_vector(vec, false, 0);
    bool a[4] = { false, true, true, false };
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for string data type.
 */
void test_init_string_vector(void **state)
{
	(void) state;
    string_v* vec = init_vector(dString)(20);
    push_vector(vec, "One", vec->len);
    push_vector(vec, "Two", vec->len);
    push_vector(vec, "Three", vec->len);
    push_vector(vec, "Four", 0);
    size_t b[4] = {4, 3, 3, 5 };
    size_t c[4] = {5, 4, 4, 6 };
    char *a[4] = {"Four", "One", "Two", "Three"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], get_vector(vec, i));
        assert_int_equal(b[i], vec->data[i].len);
        assert_int_equal(c[i], vec->data[i].alloc);
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

/**
 * Test the basic vector attributes of init, push and free for string data type.
 */
void test_init_str_vector(void **state)
{
	(void) state;
    string_v* vec = init_vector(dString)(20);
    str* one = init_string("One");
    str* two = init_string("Two", 20);
    str* three = init_string("Three");
    str* four = init_string("Four");
    push_str_vector(vec, one, vec->len);
    push_str_vector(vec, two, vec->len);
    push_str_vector(vec, three, vec->len);
    push_str_vector(vec, four, 0);
    char *a[4] = {"Four", "One", "Two", "Three"};
    size_t b[4] = {4, 3, 3, 5};
    size_t c[4] = {5, 4, 20, 6};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], get_vector(vec, i));
        assert_int_equal(b[i], vec->data[i].len);
        assert_int_equal(c[i], vec->data[i].alloc);
    }
    assert_int_equal(vec->alloc, 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
    free_string(one);
    free_string(two);
    free_string(three);
    free_string(four);
}
// ================================================================================
// ================================================================================
// eof
