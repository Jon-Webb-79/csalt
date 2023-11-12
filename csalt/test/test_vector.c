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
// TEST POP FUNCTIONS 

void test_pop_char_vector(void **state) {
    char_v* vec = init_vector(dChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
   
    char var_one = pop_vector(vec, 4);
    assert_int_equal(var_one, 5);
    char var_two = pop_vector(vec, 0);
    assert_int_equal(var_two, 1);
    char a[3] = {2, 3, 4};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_uchar_vector(void **state) {
    uchar_v* vec = init_vector(dUChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
   
    unsigned char var_one = pop_vector(vec, 4);
    assert_int_equal(var_one, 5);
    unsigned char var_two = pop_vector(vec, 0);
    assert_int_equal(var_two, 1);
    unsigned char a[3] = {2, 3, 4};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_short_vector(void **state) {
    short_v* vec = init_vector(dShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
   
    short var_one = pop_vector(vec, 4);
    assert_int_equal(var_one, 5);
    short var_two = pop_vector(vec, 0);
    assert_int_equal(var_two, 1);
    short a[3] = {2, 3, 4};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_ushort_vector(void **state) {
    ushort_v* vec = init_vector(dUShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
   
    unsigned short var_one = pop_vector(vec, 4);
    assert_int_equal(var_one, 5);
    unsigned short var_two = pop_vector(vec, 0);
    assert_int_equal(var_two, 1);
    unsigned short a[3] = {2, 3, 4};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_int_vector(void **state) {
    int_v* vec = init_vector(dInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
   
    int var_one = pop_vector(vec, 4);
    assert_int_equal(var_one, 5);
    int var_two = pop_vector(vec, 0);
    assert_int_equal(var_two, 1);
    int a[3] = {2, 3, 4};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_uint_vector(void **state) {
    uint_v* vec = init_vector(dUInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
   
    unsigned int var_one = pop_vector(vec, 4);
    assert_int_equal(var_one, 5);
    unsigned int var_two = pop_vector(vec, 0);
    assert_int_equal(var_two, 1);
    unsigned int a[3] = {2, 3, 4};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_long_vector(void **state) {
    long_v* vec = init_vector(dLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
   
    long int var_one = pop_vector(vec, 4);
    assert_int_equal(var_one, 5);
    long int var_two = pop_vector(vec, 0);
    assert_int_equal(var_two, 1);
    long int a[3] = {2, 3, 4};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_ulong_vector(void **state) {
    ulong_v* vec = init_vector(dULong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
   
    unsigned long int var_one = pop_vector(vec, 4);
    assert_int_equal(var_one, 5);
    unsigned long int var_two = pop_vector(vec, 0);
    assert_int_equal(var_two, 1);
    unsigned long int a[3] = {2, 3, 4};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_llong_vector(void **state) {
    llong_v* vec = init_vector(dLongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
   
    long long int var_one = pop_vector(vec, 4);
    assert_int_equal(var_one, 5);
    long long int var_two = pop_vector(vec, 0);
    assert_int_equal(var_two, 1);
    long long int a[3] = {2, 3, 4};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_ullong_vector(void **state) {
    ullong_v* vec = init_vector(dULongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
   
    unsigned long long int var_one = pop_vector(vec, 4);
    assert_int_equal(var_one, 5);
    unsigned long long int var_two = pop_vector(vec, 0);
    assert_int_equal(var_two, 1);
    unsigned long long int a[3] = {2, 3, 4};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_float_vector(void **state) {
    float_v* vec = init_vector(dFloat)(5);
    push_vector(vec, 1.f, vector_length(vec));
    push_vector(vec, 2.f, vector_length(vec));
    push_vector(vec, 3.f, vector_length(vec));
    push_vector(vec, 4.f, vector_length(vec));
    push_vector(vec, 5.f, vector_length(vec));
   
    float var_one = pop_vector(vec, 4);
    assert_float_equal(var_one, 5.f, 1.0e-3);
    float var_two = pop_vector(vec, 0);
    assert_float_equal(var_two, 1.f, 1.0e-3);
    float a[3] = {2, 3, 4};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_float_equal(get_vector(vec, i), a[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_double_vector(void **state) {
    double_v* vec = init_vector(dDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 3., vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
   
    double var_one = pop_vector(vec, 4);
    assert_double_equal(var_one, 5.f, 1.0e-3);
    double var_two = pop_vector(vec, 0);
    assert_double_equal(var_two, 1.f, 1.0e-3);
    double a[3] = {2, 3, 4};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_double_equal(get_vector(vec, i), a[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_ldouble_vector(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 3., vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
   
    long double var_one = pop_vector(vec, 4);
    assert_double_equal(var_one, 5.f, 1.0e-3);
    long double var_two = pop_vector(vec, 0);
    assert_double_equal(var_two, 1.f, 1.0e-3);
    long double a[3] = {2, 3, 4};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_double_equal(get_vector(vec, i), a[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_bool_vector(void **state) {
    bool_v* vec = init_vector(dBool)(5);
    push_vector(vec, true, vector_length(vec));
    push_vector(vec, true, vector_length(vec));
    push_vector(vec, false, vector_length(vec));
    push_vector(vec, false, vector_length(vec));
    push_vector(vec, false, vector_length(vec));
   
    bool var_one = pop_vector(vec, 4);
    assert_false(var_one);
    bool var_two = pop_vector(vec, 0);
    assert_true(var_two);
    bool a[3] = {true, false, false};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_string_vector(void **state) {
    string_v* vec = init_vector(dString)(5);
    push_vector(vec, "One", vector_length(vec));
    push_vector(vec, "Two", vector_length(vec));
    push_vector(vec, "Three", vector_length(vec));
    push_vector(vec, "Four", vector_length(vec));
    push_vector(vec, "Five", vector_length(vec));
   
    str* var_one = pop_vector(vec, 4);
    assert_string_equal(get_string(var_one), "Five");
    str* var_two = pop_vector(vec, 0);
    assert_string_equal(get_string(var_two), "One");
    char *a[3] = {"Two", "Three", "Four"};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_string_equal(get_vector(vec, i), a[i]);
    }
    free_vector(vec);
    free_string(var_one);
    free_string(var_two);
}
// ================================================================================
// ================================================================================
// TEST GARBAGE COLLECTION MACROS 

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_char_garbage_vector(void **state) {
    char_v* vec gbc_char_v = init_vector(dChar)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_uchar_garbage_vector(void **state) {
    uchar_v* vec gbc_uchar_v = init_vector(dUChar)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_short_garbage_vector(void **state) {
    short_v* vec gbc_short_v = init_vector(dShort)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_ushort_garbage_vector(void **state) {
    ushort_v* vec gbc_ushort_v = init_vector(dUShort)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_int_garbage_vector(void **state) {
    int_v* vec gbc_int_v = init_vector(dInt)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_uint_garbage_vector(void **state) {
    uint_v* vec gbc_uint_v = init_vector(dUInt)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_long_garbage_vector(void **state) {
    long_v* vec gbc_long_v = init_vector(dLong)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_ulong_garbage_vector(void **state) {
    ulong_v* vec gbc_ulong_v = init_vector(dULong)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_llong_garbage_vector(void **state) {
    llong_v* vec gbc_llong_v = init_vector(dLongLong)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_ullong_garbage_vector(void **state) {
    ullong_v* vec gbc_ullong_v = init_vector(dULongLong)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_float_garbage_vector(void **state) {
    float_v* vec gbc_float_v = init_vector(dFloat)(4);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 3., vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_double_garbage_vector(void **state) {
    double_v* vec gbc_double_v = init_vector(dDouble)(4);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 3., vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_ldouble_garbage_vector(void **state) {
    ldouble_v* vec gbc_ldouble_v = init_vector(dLDouble)(4);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 3., vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_bool_garbage_vector(void **state) {
    bool_v* vec gbc_bool_v = init_vector(dBool)(4);
    push_vector(vec, true, vector_length(vec));
    push_vector(vec, false, vector_length(vec));
    push_vector(vec, true, vector_length(vec));
    push_vector(vec, false, vector_length(vec));
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure no valgrind failures without explicit call to free
 */
void test_string_garbage_vector(void **state) {
    string_v* vec gbc_string_v = init_vector(dString)(4);
    push_vector(vec, "One", vector_length(vec));
    push_vector(vec, "Two", vector_length(vec));
    push_vector(vec, "Three", vector_length(vec));
    push_vector(vec, "Four", vector_length(vec));
}
// ================================================================================
// ================================================================================
// TEST REVERSE VECTOR FUNCTINS 

void test_reverse_char_vector(void **state) {
    char_v* vec = init_vector(dChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    reverse_vector(vec);
    char a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]); 
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_uchar_vector(void **state) {
    uchar_v* vec = init_vector(dUChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    reverse_vector(vec);
    unsigned char a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]); 
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_short_vector(void **state) {
    short_v* vec = init_vector(dShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    reverse_vector(vec);
    short int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]); 
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_ushort_vector(void **state) {
    ushort_v* vec = init_vector(dUShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    reverse_vector(vec);
    unsigned short int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]); 
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_int_vector(void **state) {
    int_v* vec = init_vector(dInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    reverse_vector(vec);
    int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]); 
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_uint_vector(void **state) {
    uint_v* vec = init_vector(dUInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    reverse_vector(vec);
    unsigned int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]); 
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_long_vector(void **state) {
    long_v* vec = init_vector(dLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    reverse_vector(vec);
    long int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]); 
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_ulong_vector(void **state) {
    ulong_v* vec = init_vector(dULong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    reverse_vector(vec);
    unsigned long int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]); 
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_llong_vector(void **state) {
    llong_v* vec = init_vector(dLongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    reverse_vector(vec);
    long long int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]); 
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_ullong_vector(void **state) {
    ullong_v* vec = init_vector(dULongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    reverse_vector(vec);
    unsigned long long int a[5] = {5, 4, 3, 2, 1};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]); 
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_float_vector(void **state) {
    float_v* vec = init_vector(dFloat)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 3., vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    reverse_vector(vec);
    float a[5] = {5., 4., 3., 2., 1.};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_float_equal(get_vector(vec, i), a[i], 1.0e-3); 
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_double_vector(void **state) {
    double_v* vec = init_vector(dDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 3., vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    reverse_vector(vec);
    double a[5] = {5., 4., 3., 2., 1.};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_double_equal(get_vector(vec, i), a[i], 1.0e-3); 
    }
    free_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_reverse_ldouble_vector(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 3., vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    reverse_vector(vec);
    long double a[5] = {5., 4., 3., 2., 1.};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_double_equal(get_vector(vec, i), a[i], 1.0e-3); 
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_bool_vector(void **state) {
    bool_v* vec = init_vector(dBool)(5);
    push_vector(vec, true, vector_length(vec));
    push_vector(vec, true, vector_length(vec));
    push_vector(vec, false, vector_length(vec));
    push_vector(vec, false, vector_length(vec));
    push_vector(vec, false, vector_length(vec));
    reverse_vector(vec);
    bool a[5] = {false, false, false, true, true};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(get_vector(vec, i), a[i]); 
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_string_vector(void **state) {
    string_v* vec = init_vector(dString)(5);
    push_vector(vec, "One", vector_length(vec));
    push_vector(vec, "Two", vector_length(vec));
    push_vector(vec, "Three", vector_length(vec));
    push_vector(vec, "Four", vector_length(vec));
    push_vector(vec, "Five", vector_length(vec));
    reverse_vector(vec);
    char *a[5] = {"Five", "Four", "Three", "Two", "One"};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_string_equal(get_vector(vec, i), a[i]); 
    }
    free_vector(vec);
}
// ================================================================================
// ================================================================================
// TEST MAX VECTOR FUNCTIONS 

void test_max_char_vector(void **state) {
    char_v* vec = init_vector(dChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    char max = max_vector(vec);
    free_vector(vec);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_uchar_vector(void **state) {
    uchar_v* vec = init_vector(dUChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned char max = max_vector(vec);
    free_vector(vec);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_short_vector(void **state) {
    short_v* vec = init_vector(dShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    short int max = max_vector(vec);
    free_vector(vec);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_ushort_vector(void **state) {
    ushort_v* vec = init_vector(dUShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned short int max = max_vector(vec);
    free_vector(vec);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_int_vector(void **state) {
    int_v* vec = init_vector(dInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    int max = max_vector(vec);
    free_vector(vec);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_uint_vector(void **state) {
    uint_v* vec = init_vector(dUInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned short int max = max_vector(vec);
    free_vector(vec);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_long_vector(void **state) {
    long_v* vec = init_vector(dLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    long int max = max_vector(vec);
    free_vector(vec);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_ulong_vector(void **state) {
    ulong_v* vec = init_vector(dULong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned long int max = max_vector(vec);
    free_vector(vec);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_llong_vector(void **state) {
    llong_v* vec = init_vector(dLongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    long long int max = max_vector(vec);
    free_vector(vec);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_ullong_vector(void **state) {
    ullong_v* vec = init_vector(dULongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned long long int max = max_vector(vec);
    free_vector(vec);
    assert_int_equal(max, 10);
}
// --------------------------------------------------------------------------------

void test_max_float_vector(void **state) {
    float_v* vec = init_vector(dFloat)(5);
    push_vector(vec, 1.f, vector_length(vec));
    push_vector(vec, 2.f, vector_length(vec));
    push_vector(vec, 10.3f, vector_length(vec));
    push_vector(vec, 4.f, vector_length(vec));
    push_vector(vec, 5.f, vector_length(vec));
    float max = max_vector(vec);
    free_vector(vec);
    assert_float_equal(max, 10.3, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_max_double_vector(void **state) {
    double_v* vec = init_vector(dDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 10.3, vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    double max = max_vector(vec);
    free_vector(vec);
    assert_double_equal(max, 10.3, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_max_ldouble_vector(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 10.3, vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    long double max = max_vector(vec);
    free_vector(vec);
    assert_double_equal(max, 10.3, 1.0e-3);
}
// ================================================================================
// ================================================================================
// TEST MAX VECTOR FUNCTIONS 

void test_min_char_vector(void **state) {
    char_v* vec = init_vector(dChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    char min = max_vector(vec);
    free_vector(vec);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_uchar_vector(void **state) {
    uchar_v* vec = init_vector(dUChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned char min = max_vector(vec);
    free_vector(vec);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_short_vector(void **state) {
    short_v* vec = init_vector(dShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    short int min = max_vector(vec);
    free_vector(vec);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_ushort_vector(void **state) {
    ushort_v* vec = init_vector(dUShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned short int min = max_vector(vec);
    free_vector(vec);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_int_vector(void **state) {
    int_v* vec = init_vector(dInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    int min = max_vector(vec);
    free_vector(vec);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_uint_vector(void **state) {
    uint_v* vec = init_vector(dUInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned short int min = max_vector(vec);
    free_vector(vec);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_long_vector(void **state) {
    long_v* vec = init_vector(dLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    long int min = max_vector(vec);
    free_vector(vec);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_ulong_vector(void **state) {
    ulong_v* vec = init_vector(dULong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned long int min = max_vector(vec);
    free_vector(vec);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_llong_vector(void **state) {
    llong_v* vec = init_vector(dLongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    long long int min = max_vector(vec);
    free_vector(vec);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_ullong_vector(void **state) {
    ullong_v* vec = init_vector(dULongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned long long int min = max_vector(vec);
    free_vector(vec);
    assert_int_equal(min, 1);
}
// --------------------------------------------------------------------------------

void test_min_float_vector(void **state) {
    float_v* vec = init_vector(dFloat)(5);
    push_vector(vec, 1.f, vector_length(vec));
    push_vector(vec, 2.f, vector_length(vec));
    push_vector(vec, 10.3f, vector_length(vec));
    push_vector(vec, 4.f, vector_length(vec));
    push_vector(vec, 5.f, vector_length(vec));
    float min = max_vector(vec);
    free_vector(vec);
    assert_float_equal(min, 1.f, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_min_double_vector(void **state) {
    double_v* vec = init_vector(dDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 10.3, vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    double min = max_vector(vec);
    free_vector(vec);
    assert_double_equal(min, 1.0, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_min_ldouble_vector(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 10.3, vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    long double min = max_vector(vec);
    free_vector(vec);
    assert_double_equal(min, 1.0, 1.0e-3);
}
// ================================================================================
// ================================================================================
// eof
