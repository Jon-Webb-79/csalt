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

int original_stderr_fd = -1;

void suppress_stderr(void) {
    fflush(stderr);
    original_stderr_fd = dup(fileno(stderr));
    int dev_null_fd = open("/dev/null", O_WRONLY);
    dup2(dev_null_fd, fileno(stderr));
    close(dev_null_fd);
}

void restore_stderr(void) {
    if (original_stderr_fd != -1) {
        fflush(stderr);
        dup2(original_stderr_fd, fileno(stderr));
        close(original_stderr_fd);
        original_stderr_fd = -1;
    }
}
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
    assert_int_equal(vector_memory(vec), 20);
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
    assert_int_equal(vector_memory(vec), 20);
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
    assert_int_equal(vector_memory(vec), 20);
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
    assert_int_equal(vector_memory(vec), 20);
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
    assert_int_equal(vector_memory(vec), 20);
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
    assert_int_equal(vector_memory(vec), 20);
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
    assert_int_equal(vector_memory(vec), 20);
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
    assert_int_equal(vector_memory(vec), 20);
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
    assert_int_equal(vector_memory(vec), 20);
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
    assert_int_equal(vector_memory(vec), 20);
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
    assert_int_equal(vector_memory(vec), 20);
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
    assert_int_equal(vector_memory(vec), 20);
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
    assert_int_equal(vector_memory(vec), 20);
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
    assert_int_equal(vector_memory(vec), 20);
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
        assert_int_equal(b[i], string_length(&vec->data[i]));
        assert_int_equal(c[i], string_memory(&vec->data[i]));
    }
    assert_int_equal(vector_memory(vec), 20);
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
        assert_int_equal(b[i], string_length(&vec->data[i]));
        assert_int_equal(c[i], string_memory(&vec->data[i]));
    }
    assert_int_equal(vector_memory(vec), 20);
    assert_int_equal(vector_length(vec), 4);
    free_vector(vec);
    free_string(one);
    free_string(two);
    free_string(three);
    free_string(four);
}
// ================================================================================
// ================================================================================

void test_get_char_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    char result = get_char_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_char_vector_null_data(void **state) {
    (void) state; // Unused parameter

    char_v vec = {NULL, 0, 0};

    errno = 0;
    suppress_stderr();
    char result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_char_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    unsigned char data[] = {1, 2, 3};
    uchar_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    char result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_uchar_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    unsigned char result = get_uchar_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_uchar_vector_null_data(void **state) {
    (void) state; // Unused parameter

    uchar_v vec = {NULL, 0, 0};

    errno = 0;
    suppress_stderr();
    char result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_uchar_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    unsigned char data[] = {1, 2, 3};
    uchar_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    unsigned char result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_short_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    short int result = get_short_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_short_vector_null_data(void **state) {
    (void) state; // Unused parameter

    short_v vec = {NULL, 0, 0};

    errno = 0;
    suppress_stderr();
    short int result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_short_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    short int data[] = {1, 2, 3};
    short_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    short int result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_ushort_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    unsigned short int result = get_ushort_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_ushort_vector_null_data(void **state) {
    (void) state; // Unused parameter

    ushort_v vec = {NULL, 0, 0};

    errno = 0;
    suppress_stderr();
    unsigned short int result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_ushort_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    unsigned short int data[] = {1, 2, 3};
    ushort_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    unsigned short int result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_int_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    int result = get_int_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_int_vector_null_data(void **state) {
    (void) state; // Unused parameter

    int_v vec = {NULL, 0, 0};

    errno = 0;
    suppress_stderr();
    int result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_int_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    int data[] = {1, 2, 3};
    int_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    int result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_uint_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    unsigned int result = get_uint_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_uint_vector_null_data(void **state) {
    (void) state; // Unused parameter

    uint_v vec = {NULL, 0, 0};

    errno = 0;
    suppress_stderr();
    unsigned int result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_uint_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    unsigned int data[] = {1, 2, 3};
    uint_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    unsigned int result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_long_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    long int result = get_long_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_long_vector_null_data(void **state) {
    (void) state; // Unused parameter

    long_v vec = {NULL, 0, 0};

    errno = 0;
    suppress_stderr();
    long int result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_long_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    long int data[] = {1, 2, 3};
    long_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    long int result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_ulong_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    unsigned long int result = get_ulong_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_ulong_vector_null_data(void **state) {
    (void) state; // Unused parameter

    ulong_v vec = {NULL, 0, 0};

    errno = 0;
    suppress_stderr();
    unsigned long int result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_ulong_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    unsigned long int data[] = {1, 2, 3};
    ulong_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    unsigned long int result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_llong_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    long long int result = get_llong_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_llong_vector_null_data(void **state) {
    (void) state; // Unused parameter

    llong_v vec = {NULL, 0, 0};

    errno = 0;
    suppress_stderr();
    long long int result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_llong_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    long long int data[] = {1, 2, 3};
    llong_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    long long int result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_ullong_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    unsigned long long int result = get_ullong_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_ullong_vector_null_data(void **state) {
    (void) state; // Unused parameter

    ullong_v vec = {NULL, 0, 0};

    errno = 0;
    suppress_stderr();
    unsigned long long int result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_ullong_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    unsigned long long int data[] = {1, 2, 3};
    ullong_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    unsigned long long int result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_float_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    float result = get_float_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_float_vector_null_data(void **state) {
    (void) state; // Unused parameter

    float_v vec = {NULL, 0.f, 0.f};

    errno = 0;
    suppress_stderr();
    float result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_float_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    float data[] = {1.f, 2.f, 3.f};
    float_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    float result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_double_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    double result = get_double_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_double_vector_null_data(void **state) {
    (void) state; // Unused parameter

    double_v vec = {NULL, 0., 0.};

    errno = 0;
    suppress_stderr();
    double result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_double_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    double data[] = {1., 2., 3.};
    double_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    double result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_ldouble_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    long double result = get_ldouble_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_ldouble_vector_null_data(void **state) {
    (void) state; // Unused parameter

    ldouble_v vec = {NULL, 0., 0.};

    errno = 0;
    suppress_stderr();
    long double result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_ldouble_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    long double data[] = {1., 2., 3.};
    ldouble_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    long double result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_int_equal(result, 0);
}
// --------------------------------------------------------------------------------

void test_get_bool_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    bool result = get_bool_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_false(result);
}
// --------------------------------------------------------------------------------

void test_get_bool_vector_null_data(void **state) {
    (void) state; // Unused parameter

    bool_v vec = {NULL, 0., 0.};

    errno = 0;
    suppress_stderr();
    bool result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_false(result);
}
// --------------------------------------------------------------------------------

void test_get_bool_vector_index_out_of_bounds(void **state) {
    (void) state; // Unused parameter

    bool data[] = {true, false, true};
    bool_v vec = {data, 3, 3}; // len = 3

    errno = 0;
    suppress_stderr();
    bool result = get_vector(&vec, 4); // Index out of bounds
    restore_stderr();
    assert_int_equal(errno, ERANGE);
    assert_false(result);
}
// --------------------------------------------------------------------------------

void test_get_string_vector_null_struct(void **state) {
    (void) state; // Unused parameter

    errno = 0;
    suppress_stderr();
    char* result = get_string_vector(NULL, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_string_equal(result, "\0");
}
// --------------------------------------------------------------------------------

void test_get_string_vector_null_data(void **state) {
    (void) state; // Unused parameter

    string_v vec = {NULL, 0., 0.};

    errno = 0;
    suppress_stderr();
    char* result = get_vector(&vec, 0);
    restore_stderr();
    assert_int_equal(errno, EINVAL);
    assert_string_equal(result, "\0");
}
// --------------------------------------------------------------------------------

void test_print(void **state) {
    float_v* vec = init_vector(dFloat)(5);
    push_vector(vec, 1.0f, vec->len);
    push_vector(vec, 2.0f, vec->len);
    push_vector(vec, 3.0f, vec->len);
    push_vector(vec, 4.0f, 0);
    print(vec);
    free_vector(vec);
}
// ================================================================================
// ================================================================================
// eof
