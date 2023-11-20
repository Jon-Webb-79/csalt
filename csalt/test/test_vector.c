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
    char* a[4] = {"Four", "One", "Two", "Three"};
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
    char min = min_vector(vec);
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
    unsigned char min = min_vector(vec);
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
    short int min = min_vector(vec);
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
    unsigned short int min = min_vector(vec);
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
    int min = min_vector(vec);
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
    unsigned short int min = min_vector(vec);
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
    long int min = min_vector(vec);
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
    unsigned long int min = min_vector(vec);
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
    long long int min = min_vector(vec);
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
    unsigned long long int min = min_vector(vec);
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
    float min = min_vector(vec);
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
    double min = min_vector(vec);
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
    long double min = min_vector(vec);
    free_vector(vec);
    assert_double_equal(min, 1.0, 1.0e-3);
}
// ================================================================================
// ================================================================================
// TEST SUM VECTOR FUNCTIONS

void test_sum_char_vector(void **state) {
    char_v* vec = init_vector(dChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    char sum = sum_vector(vec);
    free_vector(vec);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_uchar_vector(void **state) {
    uchar_v* vec = init_vector(dUChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned char sum = sum_vector(vec);
    free_vector(vec);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_short_vector(void **state) {
    short_v* vec = init_vector(dShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    short int sum = sum_vector(vec);
    free_vector(vec);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_ushort_vector(void **state) {
    ushort_v* vec = init_vector(dUShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned short int sum = sum_vector(vec);
    free_vector(vec);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_int_vector(void **state) {
    int_v* vec = init_vector(dInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    int sum = sum_vector(vec);
    free_vector(vec);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_uint_vector(void **state) {
    uint_v* vec = init_vector(dUInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned short int sum = sum_vector(vec);
    free_vector(vec);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_long_vector(void **state) {
    long_v* vec = init_vector(dLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    long int sum = sum_vector(vec);
    free_vector(vec);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_ulong_vector(void **state) {
    ulong_v* vec = init_vector(dULong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned long int sum = sum_vector(vec);
    free_vector(vec);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_llong_vector(void **state) {
    llong_v* vec = init_vector(dLongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    long long int sum = sum_vector(vec);
    free_vector(vec);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_ullong_vector(void **state) {
    ullong_v* vec = init_vector(dULongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    unsigned long long int sum = sum_vector(vec);
    free_vector(vec);
    assert_int_equal(sum, 22);
}
// --------------------------------------------------------------------------------

void test_sum_float_vector(void **state) {
    float_v* vec = init_vector(dFloat)(5);
    push_vector(vec, 1.f, vector_length(vec));
    push_vector(vec, 2.f, vector_length(vec));
    push_vector(vec, 10.3f, vector_length(vec));
    push_vector(vec, 4.f, vector_length(vec));
    push_vector(vec, 5.f, vector_length(vec));
    float sum = sum_vector(vec);
    free_vector(vec);
    assert_float_equal(sum, 22.3f, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_sum_double_vector(void **state) {
    double_v* vec = init_vector(dDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 10.3, vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    double sum = sum_vector(vec);
    free_vector(vec);
    assert_double_equal(sum, 22.3, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_sum_ldouble_vector(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 10.3, vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    long double sum = sum_vector(vec);
    free_vector(vec);
    assert_double_equal(sum, 22.3, 1.0e-3);
}
// ================================================================================
// ================================================================================
// TEST AVERAGE VECTOR FUNCTIONS 

void test_average_char_vector(void **state) {
    char_v* vec = init_vector(dChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    float average = vector_average(vec);
    free_vector(vec);
    assert_float_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_uchar_vector(void **state) {
    uchar_v* vec = init_vector(dUChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    float average = vector_average(vec);
    free_vector(vec);
    assert_float_equal(average, 4.40, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_short_vector(void **state) {
    short_v* vec = init_vector(dShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    float average = vector_average(vec);
    free_vector(vec);
    assert_float_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_ushort_vector(void **state) {
    ushort_v* vec = init_vector(dUShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    float average = vector_average(vec);
    free_vector(vec);
    assert_float_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_int_vector(void **state) {
    int_v* vec = init_vector(dInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    double average = vector_average(vec);
    free_vector(vec);
    assert_double_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_uint_vector(void **state) {
    uint_v* vec = init_vector(dUInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    double average = vector_average(vec);
    free_vector(vec);
    assert_double_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_long_vector(void **state) {
    long_v* vec = init_vector(dLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    double average = vector_average(vec);
    free_vector(vec);
    assert_double_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_ulong_vector(void **state) {
    ulong_v* vec = init_vector(dULong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    double average = vector_average(vec);
    free_vector(vec);
    assert_float_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_llong_vector(void **state) {
    llong_v* vec = init_vector(dLongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    long double average = vector_average(vec);
    free_vector(vec);
    assert_double_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_ullong_vector(void **state) {
    ullong_v* vec = init_vector(dULongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    long double average = vector_average(vec);
    free_vector(vec);
    assert_double_equal(average, 4.4, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_float_vector(void **state) {
    float_v* vec = init_vector(dFloat)(5);
    push_vector(vec, 1.f, vector_length(vec));
    push_vector(vec, 2.f, vector_length(vec));
    push_vector(vec, 10.3f, vector_length(vec));
    push_vector(vec, 4.f, vector_length(vec));
    push_vector(vec, 5.f, vector_length(vec));
    float average = vector_average(vec);
    free_vector(vec);
    assert_float_equal(average, 4.46f, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_double_vector(void **state) {
    double_v* vec = init_vector(dDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 10.3, vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    double average = vector_average(vec);
    free_vector(vec);
    assert_double_equal(average, 4.46, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_average_ldouble_vector(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 10.3, vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    long double average = vector_average(vec);
    free_vector(vec);
    assert_double_equal(average, 4.46, 1.0e-3);
}
// ================================================================================
// ================================================================================
// TEST STDEV FUNCTIONS 

void test_stdev_char_vector(void **state) {
    char_v* vec = init_vector(dChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    float stdev = vector_stdev(vec);
    free_vector(vec);
    assert_float_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_uchar_vector(void **state) {
    uchar_v* vec = init_vector(dUChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    float stdev = vector_stdev(vec);
    free_vector(vec);
    assert_float_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_short_vector(void **state) {
    short_v* vec = init_vector(dShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    float stdev = vector_stdev(vec);
    free_vector(vec);
    assert_float_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_ushort_vector(void **state) {
    ushort_v* vec = init_vector(dUShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    float stdev = vector_stdev(vec);
    free_vector(vec);
    assert_float_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_int_vector(void **state) {
    int_v* vec = init_vector(dInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    double stdev = vector_stdev(vec);
    free_vector(vec);
    assert_double_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_uint_vector(void **state) {
    uint_v* vec = init_vector(dUInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    double stdev = vector_stdev(vec);
    free_vector(vec);
    assert_double_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_long_vector(void **state) {
    long_v* vec = init_vector(dLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    double stdev = vector_stdev(vec);
    free_vector(vec);
    assert_double_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_ulong_vector(void **state) {
    ulong_v* vec = init_vector(dULong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    double stdev = vector_stdev(vec);
    free_vector(vec);
    assert_float_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_llong_vector(void **state) {
    llong_v* vec = init_vector(dLongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    long double stdev = vector_stdev(vec);
    free_vector(vec);
    assert_double_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_ullong_vector(void **state) {
    ullong_v* vec = init_vector(dULongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 10, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    long double stdev = vector_stdev(vec);
    free_vector(vec);
    assert_double_equal(stdev, 3.13687, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_float_vector(void **state) {
    float_v* vec = init_vector(dFloat)(5);
    push_vector(vec, 1.f, vector_length(vec));
    push_vector(vec, 2.f, vector_length(vec));
    push_vector(vec, 10.3f, vector_length(vec));
    push_vector(vec, 4.f, vector_length(vec));
    push_vector(vec, 5.f, vector_length(vec));
    float stdev = vector_stdev(vec);
    free_vector(vec);
    assert_float_equal(stdev, 3.24444, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_double_vector(void **state) {
    double_v* vec = init_vector(dDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 10.3, vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    double stdev = vector_stdev(vec);
    free_vector(vec);
    assert_double_equal(stdev, 3.24444, 1.0e-3);
}
// --------------------------------------------------------------------------------

void test_stdev_ldouble_vector(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 10.3, vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    long double stdev = vector_stdev(vec);
    free_vector(vec);
    assert_double_equal(stdev, 3.24444, 1.0e-3);
}
// ================================================================================
// ================================================================================
// TEST CUMSUM FUNCTIONS 

void test_cumsum_char_vector(void **state) {
    char_v* vec = init_vector(dChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    char_v* new_vec = vector_cumsum(vec);
    char a[5] = {1, 3, 6, 10, 15};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(a[i], get_vector(new_vec, i));
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_cumsum_uchar_vector(void **state) {
    uchar_v* vec = init_vector(dUChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    uchar_v* new_vec = vector_cumsum(vec);
    unsigned char a[5] = {1, 3, 6, 10, 15};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(a[i], get_vector(new_vec, i));
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_cumsum_short_vector(void **state) {
    short_v* vec = init_vector(dShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    short_v* new_vec = vector_cumsum(vec);
    short int a[5] = {1, 3, 6, 10, 15};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(a[i], get_vector(new_vec, i));
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_cumsum_ushort_vector(void **state) {
    ushort_v* vec = init_vector(dUShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    ushort_v* new_vec = vector_cumsum(vec);
    unsigned short int a[5] = {1, 3, 6, 10, 15};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(a[i], get_vector(new_vec, i));
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_cumsum_int_vector(void **state) {
    int_v* vec = init_vector(dInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    int_v* new_vec = vector_cumsum(vec);
    int a[5] = {1, 3, 6, 10, 15};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(a[i], get_vector(new_vec, i));
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_cumsum_uint_vector(void **state) {
    uint_v* vec = init_vector(dUInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    uint_v* new_vec = vector_cumsum(vec);
    unsigned int a[5] = {1, 3, 6, 10, 15};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(a[i], get_vector(new_vec, i));
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_cumsum_long_vector(void **state) {
    long_v* vec = init_vector(dLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    long_v* new_vec = vector_cumsum(vec);
    long int a[5] = {1, 3, 6, 10, 15};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(a[i], get_vector(new_vec, i));
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_cumsum_ulong_vector(void **state) {
    ulong_v* vec = init_vector(dULong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    ulong_v* new_vec = vector_cumsum(vec);
    unsigned long int a[5] = {1, 3, 6, 10, 15};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(a[i], get_vector(new_vec, i));
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_cumsum_llong_vector(void **state) {
    llong_v* vec = init_vector(dLongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    llong_v* new_vec = vector_cumsum(vec);
    long long int a[5] = {1, 3, 6, 10, 15};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(a[i], get_vector(new_vec, i));
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_cumsum_ullong_vector(void **state) {
    ullong_v* vec = init_vector(dULongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    ullong_v* new_vec = vector_cumsum(vec);
    unsigned long long int a[5] = {1, 3, 6, 10, 15};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_int_equal(a[i], get_vector(new_vec, i));
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_cumsum_float_vector(void **state) {
    float_v* vec = init_vector(dFloat)(5);
    push_vector(vec, 1.f, vector_length(vec));
    push_vector(vec, 2.f, vector_length(vec));
    push_vector(vec, 3.f, vector_length(vec));
    push_vector(vec, 4.f, vector_length(vec));
    push_vector(vec, 5.f, vector_length(vec));
    float_v* new_vec = vector_cumsum(vec);
    float a[5] = {1., 3., 6., 10., 15.};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_float_equal(a[i], get_vector(new_vec, i), 1.0e-3);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_cumsum_double_vector(void **state) {
    double_v* vec = init_vector(dDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 3., vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    double_v* new_vec = vector_cumsum(vec);
    double a[5] = {1., 3., 6., 10., 15.};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_double_equal(a[i], get_vector(new_vec, i), 1.0e-3);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_cumsum_ldouble_vector(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(5);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 3., vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    ldouble_v* new_vec = vector_cumsum(vec);
    long double a[5] = {1., 3., 6., 10., 15.};
    for (size_t i = 0; i < vector_length(vec); i++) {
        assert_double_equal(a[i], get_vector(new_vec, i), 1.0e-3);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// ================================================================================
// ================================================================================
// TEST COPY VECTOR 

void test_copy_char_vector(void **state) {
    char_v* vec = init_vector(dChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    char_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(vec->data[i], new_vec->data[i]);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_copy_uchar_vector(void **state) {
    uchar_v* vec = init_vector(dUChar)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    uchar_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(vec->data[i], new_vec->data[i]);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_copy_short_vector(void **state) {
    short_v* vec = init_vector(dShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    short_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(vec->data[i], new_vec->data[i]);
    }
    free_vector(vec);
    free_vector(new_vec);
} 
// --------------------------------------------------------------------------------

void test_copy_ushort_vector(void **state) {
    ushort_v* vec = init_vector(dUShort)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    ushort_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(vec->data[i], new_vec->data[i]);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_copy_int_vector(void **state) {
    int_v* vec = init_vector(dInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    int_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(vec->data[i], new_vec->data[i]);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_copy_uint_vector(void **state) {
    uint_v* vec = init_vector(dUInt)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    uint_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(vec->data[i], new_vec->data[i]);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_copy_long_vector(void **state) {
    long_v* vec = init_vector(dLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    long_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(vec->data[i], new_vec->data[i]);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_copy_ulong_vector(void **state) {
    ulong_v* vec = init_vector(dULong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    ulong_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(vec->data[i], new_vec->data[i]);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_copy_llong_vector(void **state) {
    llong_v* vec = init_vector(dLongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    llong_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(vec->data[i], new_vec->data[i]);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_copy_ullong_vector(void **state) {
    ullong_v* vec = init_vector(dULongLong)(5);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    ullong_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(vec->data[i], new_vec->data[i]);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_copy_float_vector(void **state) {
    float_v* vec = init_vector(dFloat)(5);
    push_vector(vec, 1.f, vector_length(vec));
    push_vector(vec, 2.f, vector_length(vec));
    push_vector(vec, 3.f, vector_length(vec));
    push_vector(vec, 4.f, vector_length(vec));
    push_vector(vec, 5.f, vector_length(vec));
    float_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(vec->data[i], new_vec->data[i], 1.0e-3);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_copy_double_vector(void **state) {
    double_v* vec = init_vector(dDouble)(5);
    push_vector(vec, 1.f, vector_length(vec));
    push_vector(vec, 2.f, vector_length(vec));
    push_vector(vec, 3.f, vector_length(vec));
    push_vector(vec, 4.f, vector_length(vec));
    push_vector(vec, 5.f, vector_length(vec));
    double_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(vec->data[i], new_vec->data[i], 1.0e-3);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_copy_ldouble_vector(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(5);
    push_vector(vec, 1.f, vector_length(vec));
    push_vector(vec, 2.f, vector_length(vec));
    push_vector(vec, 3.f, vector_length(vec));
    push_vector(vec, 4.f, vector_length(vec));
    push_vector(vec, 5.f, vector_length(vec));
    ldouble_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(vec->data[i], new_vec->data[i], 1.0e-3);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_copy_bool_vector(void **state) {
    bool_v* vec = init_vector(dBool)(5);
    push_vector(vec, true, vector_length(vec));
    push_vector(vec, true, vector_length(vec));
    push_vector(vec, false, vector_length(vec));
    push_vector(vec, false, vector_length(vec));
    push_vector(vec, false, vector_length(vec));
    bool_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(vec->data[i], new_vec->data[i]);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// --------------------------------------------------------------------------------

void test_copy_string_vector(void **state) {
    string_v* vec = init_vector(dString)(5);
    push_vector(vec, "One", vector_length(vec));
    push_vector(vec, "Two", vector_length(vec));
    push_vector(vec, "Three", vector_length(vec));
    push_vector(vec, "Four", vector_length(vec));
    push_vector(vec, "Five", vector_length(vec));
    string_v* new_vec = vector_copy(vec); 
    assert_int_equal(vec->alloc, new_vec->alloc);
    assert_int_equal(vec->len, new_vec->len);
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(vec->data[i].data, new_vec->data[i].data);
    }
    free_vector(vec);
    free_vector(new_vec);
}
// ================================================================================
// ================================================================================

void test_trim_char_vector(void **state) {
    char_v* vec = init_vector(dChar)(15);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_uchar_vector(void **state) {
    uchar_v* vec = init_vector(dUChar)(15);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_short_vector(void **state) {
    short_v* vec = init_vector(dShort)(15);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_ushort_vector(void **state) {
    ushort_v* vec = init_vector(dUShort)(15);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_int_vector(void **state) {
    int_v* vec = init_vector(dInt)(15);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_uint_vector(void **state) {
    uint_v* vec = init_vector(dUInt)(15);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_long_vector(void **state) {
    long_v* vec = init_vector(dLong)(15);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_ulong_vector(void **state) {
    ulong_v* vec = init_vector(dULong)(15);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_llong_vector(void **state) {
    llong_v* vec = init_vector(dLongLong)(15);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_ullong_vector(void **state) {
    ullong_v* vec = init_vector(dULongLong)(15);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_float_vector(void **state) {
    char_v* vec = init_vector(dFloat)(15);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 3., vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    push_vector(vec, 5., vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_double_vector(void **state) {
    double_v* vec = init_vector(dDouble)(15);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_ldouble_vector(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(15);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    push_vector(vec, 5, vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_bool_vector(void **state) {
    bool_v* vec = init_vector(dBool)(15);
    push_vector(vec, true, vector_length(vec));
    push_vector(vec, false, vector_length(vec));
    push_vector(vec, true, vector_length(vec));
    push_vector(vec, false, vector_length(vec));
    push_vector(vec, true, vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_trim_string_vector(void **state) {
    string_v* vec = init_vector(dString)(15);
    push_vector(vec, "One", vector_length(vec));
    push_vector(vec, "Two", vector_length(vec));
    push_vector(vec, "Three", vector_length(vec));
    push_vector(vec, "Four", vector_length(vec));
    push_vector(vec, "Five", vector_length(vec));
    trim_vector(vec); 
    assert_int_equal(vec->alloc, 5);
    free_vector(vec);
}
// ================================================================================
// ================================================================================
// TEST REPLACE VECTOR FUNCTIONS 

void test_replace_char_vector(void **state) {
    char_v* vec = init_vector(dChar)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    replace_vector_index(vec, 5, 1);
    char a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_uchar_vector(void **state) {
    uchar_v* vec = init_vector(dUChar)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    replace_vector_index(vec, 5, 1);
    unsigned char a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_short_vector(void **state) {
    short_v* vec = init_vector(dShort)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    replace_vector_index(vec, 5, 1);
    short int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_ushort_vector(void **state) {
    ushort_v* vec = init_vector(dUShort)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    replace_vector_index(vec, 5, 1);
    unsigned short int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_int_vector(void **state) {
    int_v* vec = init_vector(dInt)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    replace_vector_index(vec, 5, 1);
    int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_uint_vector(void **state) {
    uint_v* vec = init_vector(dUInt)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    replace_vector_index(vec, 5, 1);
    unsigned int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_long_vector(void **state) {
    long_v* vec = init_vector(dLong)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    replace_vector_index(vec, 5, 1);
    long int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_ulong_vector(void **state) {
    ulong_v* vec = init_vector(dULong)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    replace_vector_index(vec, 5, 1);
    unsigned long int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_llong_vector(void **state) {
    llong_v* vec = init_vector(dLongLong)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    replace_vector_index(vec, 5, 1);
    long long int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_ullong_vector(void **state) {
    ullong_v* vec = init_vector(dULongLong)(4);
    push_vector(vec, 1, vector_length(vec));
    push_vector(vec, 2, vector_length(vec));
    push_vector(vec, 3, vector_length(vec));
    push_vector(vec, 4, vector_length(vec));
    replace_vector_index(vec, 5, 1);
    unsigned long long int a[4] = {1, 5, 3, 4};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_float_vector(void **state) {
    float_v* vec = init_vector(dFloat)(4);
    push_vector(vec, 1.f, vector_length(vec));
    push_vector(vec, 2.f, vector_length(vec));
    push_vector(vec, 3.f, vector_length(vec));
    push_vector(vec, 4.f, vector_length(vec));
    replace_vector_index(vec, 5.f, 1);
    float a[4] = {1., 5., 3., 4.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], get_vector(vec, i), 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_double_vector(void **state) {
    double_v* vec = init_vector(dDouble)(4);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 3., vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    replace_vector_index(vec, 5., 1);
    double a[4] = {1., 5., 3., 4.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], get_vector(vec, i), 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_ldouble_vector(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(4);
    push_vector(vec, 1., vector_length(vec));
    push_vector(vec, 2., vector_length(vec));
    push_vector(vec, 3., vector_length(vec));
    push_vector(vec, 4., vector_length(vec));
    replace_vector_index(vec, 5., 1);
    long double a[4] = {1., 5., 3., 4.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], get_vector(vec, i), 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_bool_vector(void **state) {
    bool_v* vec = init_vector(dBool)(4);
    push_vector(vec, true, vector_length(vec));
    push_vector(vec, true, vector_length(vec));
    push_vector(vec, true, vector_length(vec));
    push_vector(vec, true, vector_length(vec));
    replace_vector_index(vec, false, 1);
    bool a[4] = {true, false, true, true};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_string_vector(void **state) {
    string_v* vec = init_vector(dString)(4);
    push_vector(vec, "One", vector_length(vec));
    push_vector(vec, "Two", vector_length(vec));
    push_vector(vec, "Three", vector_length(vec));
    push_vector(vec, "Four", vector_length(vec));
    replace_vector_index(vec, "One", 1);
    char *a[4] = {"One", "One", "Three", "Four"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_replace_str_vector(void **state) {
    string_v* vec = init_vector(dString)(4);
    push_vector(vec, "One", vector_length(vec));
    push_vector(vec, "Two", vector_length(vec));
    push_vector(vec, "Three", vector_length(vec));
    push_vector(vec, "Four", vector_length(vec));
    str* b = init_string("One");
    replace_str_vector_index(vec, b, 1);
    char *a[4] = {"One", "One", "Three", "Four"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], get_vector(vec, i));
    }
    free_vector(vec);
    free_string(b);
}
// ================================================================================
// ================================================================================
// TEST ITERATOR FUNCTIONS 

void test_iter_char_vector(void **state) {
    char_v* vec = init_vector(dChar)(5);
    push_vector(vec, 1, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 3, 2);
    push_vector(vec, 4, 3);
    push_vector(vec, 5, 4);
    char_v_iterator it = vector_iterator(vec);
    char* begin = it.begin(vec);
    char* end = it.end(vec);
    char a;
    for (char* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    char b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_iter_uchar_vector(void **state) {
    uchar_v* vec = init_vector(dUChar)(5);
    push_vector(vec, 1, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 3, 2);
    push_vector(vec, 4, 3);
    push_vector(vec, 5, 4);
    uchar_v_iterator it = vector_iterator(vec);
    unsigned char* begin = it.begin(vec);
    unsigned char* end = it.end(vec);
    unsigned char a;
    for (unsigned char* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    unsigned char b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_iter_short_vector(void **state) {
    short_v* vec = init_vector(dShort)(5);
    push_vector(vec, 1, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 3, 2);
    push_vector(vec, 4, 3);
    push_vector(vec, 5, 4);
    short_v_iterator it = vector_iterator(vec);
    short int* begin = it.begin(vec);
    short int* end = it.end(vec);
    short int a;
    for (short int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    short int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_iter_ushort_vector(void **state) {
    ushort_v* vec = init_vector(dUShort)(5);
    push_vector(vec, 1, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 3, 2);
    push_vector(vec, 4, 3);
    push_vector(vec, 5, 4);
    ushort_v_iterator it = vector_iterator(vec);
    unsigned short int* begin = it.begin(vec);
    unsigned short int* end = it.end(vec);
    unsigned short int a;
    for (unsigned short int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    unsigned short int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_iter_int_vector(void **state) {
    int_v* vec = init_vector(dInt)(5);
    push_vector(vec, 1, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 3, 2);
    push_vector(vec, 4, 3);
    push_vector(vec, 5, 4);
    int_v_iterator it = vector_iterator(vec);
    int* begin = it.begin(vec);
    int* end = it.end(vec);
    int a;
    for (int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_iter_uint_vector(void **state) {
    uint_v* vec = init_vector(dUInt)(5);
    push_vector(vec, 1, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 3, 2);
    push_vector(vec, 4, 3);
    push_vector(vec, 5, 4);
    uint_v_iterator it = vector_iterator(vec);
    unsigned int* begin = it.begin(vec);
    unsigned int* end = it.end(vec);
    unsigned int a;
    for (unsigned int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    unsigned int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_iter_long_vector(void **state) {
    long_v* vec = init_vector(dLong)(5);
    push_vector(vec, 1, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 3, 2);
    push_vector(vec, 4, 3);
    push_vector(vec, 5, 4);
    long_v_iterator it = vector_iterator(vec);
    long int* begin = it.begin(vec);
    long int* end = it.end(vec);
    long int a;
    for (long int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    long int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_iter_ulong_vector(void **state) {
    ulong_v* vec = init_vector(dULong)(5);
    push_vector(vec, 1, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 3, 2);
    push_vector(vec, 4, 3);
    push_vector(vec, 5, 4);
    ulong_v_iterator it = vector_iterator(vec);
    unsigned long int* begin = it.begin(vec);
    unsigned long int* end = it.end(vec);
    unsigned long int a;
    for (unsigned long int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    unsigned long int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_iter_llong_vector(void **state) {
    llong_v* vec = init_vector(dLongLong)(5);
    push_vector(vec, 1, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 3, 2);
    push_vector(vec, 4, 3);
    push_vector(vec, 5, 4);
    llong_v_iterator it = vector_iterator(vec);
    long long int* begin = it.begin(vec);
    long long int* end = it.end(vec);
    long long int a;
    for (long long int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    long long int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_iter_ullong_vector(void **state) {
    ullong_v* vec = init_vector(dULongLong)(5);
    push_vector(vec, 1, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 3, 2);
    push_vector(vec, 4, 3);
    push_vector(vec, 5, 4);
    ullong_v_iterator it = vector_iterator(vec);
    unsigned long long int* begin = it.begin(vec);
    unsigned long long int* end = it.end(vec);
    unsigned long long int a;
    for (unsigned long long int* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10;
    }
    unsigned long long int b[5] = {11, 12, 13, 14, 15};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], get_vector(vec, i));
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_iter_float_vector(void **state) {
    float_v* vec = init_vector(dFloat)(5);
    push_vector(vec, 1., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 3., 2);
    push_vector(vec, 4., 3);
    push_vector(vec, 5., 4);
    float_v_iterator it = vector_iterator(vec);
    float* begin = it.begin(vec);
    float* end = it.end(vec);
    float a;
    for (float* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10.f;
    }
    float b[5] = {11., 12., 13., 14., 15.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(b[i], get_vector(vec, i), 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_iter_double_vector(void **state) {
    double_v* vec = init_vector(dDouble)(5);
    push_vector(vec, 1., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 3., 2);
    push_vector(vec, 4., 3);
    push_vector(vec, 5., 4);
    double_v_iterator it = vector_iterator(vec);
    double* begin = it.begin(vec);
    double* end = it.end(vec);
    double a;
    for (double* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10.f;
    }
    double b[5] = {11., 12., 13., 14., 15.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(b[i], get_vector(vec, i), 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_iter_ldouble_vector(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(5);
    push_vector(vec, 1., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 3., 2);
    push_vector(vec, 4., 3);
    push_vector(vec, 5., 4);
    ldouble_v_iterator it = vector_iterator(vec);
    long double* begin = it.begin(vec);
    long double* end = it.end(vec);
    long double a;
    for (long double* i = begin; i != end; it.next(&i)) {
        a = it.get(&i);
        *i = a + 10.f;
    }
    long double b[5] = {11., 12., 13., 14., 15.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(b[i], get_vector(vec, i), 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_iter_bool_vector(void **state) {
    bool_v* vec = init_vector(dBool)(5);
    push_vector(vec, false, 0);
    push_vector(vec, false, 1);
    push_vector(vec, false, 2);
    push_vector(vec, false, 3);
    push_vector(vec, false, 4);
    bool_v_iterator it = vector_iterator(vec);
    bool* begin = it.begin(vec);
    bool* end = it.end(vec);
    for (bool* i = begin; i != end; it.next(&i)) {
        *i = true;
    }
    bool b[5] = {true, true, true, true, true};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(b[i], get_vector(vec, i));
    }
    free_vector(vec);
} 
// --------------------------------------------------------------------------------

void test_iter_string_vector(void **state) {
    string_v* vec = init_vector(dString)(5);
    push_vector(vec, "One", 0);
    push_vector(vec, "Two", 1);
    push_vector(vec, "Three", 2);
    push_vector(vec, "Four", 3);
    push_vector(vec, "Five", 4);
    string_v_iterator it = vector_iterator(vec);
    str* begin = it.begin(vec);
    str* end = it.end(vec);
    size_t j = 0;
    char *a[5] = {"One", "Two", "Three", "Four", "Five"};
    for (str* i = begin; i != end; it.next(&i)) {
        assert_string_equal(a[j], it.get(&i)); 
        j++;
    }
    free_vector(vec);
}
// ================================================================================
// ================================================================================
// TEST BUBBLE SORT FUNCTIONS 

void test_bubble_sort_char_vector_forward(void **state) {
    char_v* vec = init_vector(dChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_char_vector_reverse(void **state) {
    char_v* vec = init_vector(dChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, BUBBLE, REVERSE);
    char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uchar_vector_forward(void **state) {
    uchar_v* vec = init_vector(dUChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uchar_vector_reverse(void **state) {
    uchar_v* vec = init_vector(dUChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, BUBBLE, REVERSE);
    unsigned char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_short_vector_forward(void **state) {
    short_v* vec = init_vector(dShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_short_vector_reverse(void **state) {
    short_v* vec = init_vector(dShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, BUBBLE, REVERSE);
    short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ushort_vector_forward(void **state) {
    ushort_v* vec = init_vector(dUShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ushort_vector_reverse(void **state) {
    ushort_v* vec = init_vector(dUShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, BUBBLE, REVERSE);
    unsigned short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_int_vector_forward(void **state) {
    int_v* vec = init_vector(dInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_int_vector_reverse(void **state) {
    int_v* vec = init_vector(dInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, BUBBLE, REVERSE);
    int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uint_vector_forward(void **state) {
    uint_v* vec = init_vector(dUInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_uint_vector_reverse(void **state) {
    uint_v* vec = init_vector(dUInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, BUBBLE, REVERSE);
    unsigned int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_long_vector_forward(void **state) {
    long_v* vec = init_vector(dLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_long_vector_reverse(void **state) {
    long_v* vec = init_vector(dLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, BUBBLE, REVERSE);
    long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ulong_vector_forward(void **state) {
    ulong_v* vec = init_vector(dULong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ulong_vector_reverse(void **state) {
    ulong_v* vec = init_vector(dULong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, BUBBLE, REVERSE);
    unsigned long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_llong_vector_forward(void **state) {
    llong_v* vec = init_vector(dLongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_llong_vector_reverse(void **state) {
    llong_v* vec = init_vector(dLongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, BUBBLE, REVERSE);
    long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ullong_vector_forward(void **state) {
    ullong_v* vec = init_vector(dULongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ullong_vector_reverse(void **state) {
    ullong_v* vec = init_vector(dULongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, BUBBLE, REVERSE);
    unsigned long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_float_vector_forward(void **state) {
    float_v* vec = init_vector(dFloat)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    float a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_float_vector_reverse(void **state) {
    float_v* vec = init_vector(dFloat)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, BUBBLE, REVERSE);
    float a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_double_vector_forward(void **state) {
    double_v* vec = init_vector(dDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_double_vector_reverse(void **state) {
    double_v* vec = init_vector(dDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, BUBBLE, REVERSE);
    double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ldouble_vector_forward(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    long double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_ldouble_vector_reverse(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, BUBBLE, REVERSE);
    long double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_bool_vector_forward(void **state) {
    bool_v* vec = init_vector(dBool)(7);
    push_vector(vec, true, 0);
    push_vector(vec, false, 1);
    push_vector(vec, true, 2);
    push_vector(vec, false, 3);
    push_vector(vec, true, 4);
    push_vector(vec, false, 5);
    push_vector(vec, true, 6);
    bool a[7] = {false, false, false, true, true, true, true};
    sort_vector(vec, BUBBLE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_bool_vector_reverse(void **state) {
    bool_v* vec = init_vector(dBool)(7);
    push_vector(vec, true, 0);
    push_vector(vec, false, 1);
    push_vector(vec, true, 2);
    push_vector(vec, false, 3);
    push_vector(vec, true, 4);
    push_vector(vec, false, 5);
    push_vector(vec, true, 6);
    sort_vector(vec, BUBBLE, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_string_vector_forward(void **state) {
    string_v* vec = init_vector(dString)(7);
    push_vector(vec, "One", 0);
    push_vector(vec, "Two", 1);
    push_vector(vec, "Three", 2);
    push_vector(vec, "Four", 3);
    push_vector(vec, "Five", 4);
    push_vector(vec, "Six", 5);
    push_vector(vec, "Seven", 6);
    sort_vector(vec, BUBBLE, FORWARD);
    char *a[7] = {"Five", "Four", "One", "Seven", "Six", "Three", "Two"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], vec->data[i].data);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bubble_sort_string_vector_reverse(void **state) {
    string_v* vec = init_vector(dString)(7);
    push_vector(vec, "One", 0);
    push_vector(vec, "Two", 1);
    push_vector(vec, "Three", 2);
    push_vector(vec, "Four", 3);
    push_vector(vec, "Five", 4);
    push_vector(vec, "Six", 5);
    push_vector(vec, "Seven", 6);
    sort_vector(vec, BUBBLE, REVERSE);
    char *a[7] = {"Two", "Three", "Six", "Seven", "One", "Four", "Five"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], vec->data[i].data);
    }
    free_vector(vec);
}
// ================================================================================
// ================================================================================
// TEST INSERT SORT FUNCTIONS 

void test_insert_sort_char_vector_forward(void **state) {
    char_v* vec = init_vector(dChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_char_vector_reverse(void **state) {
    char_v* vec = init_vector(dChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, INSERT, REVERSE);
    char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_uchar_vector_forward(void **state) {
    uchar_v* vec = init_vector(dUChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_uchar_vector_reverse(void **state) {
    uchar_v* vec = init_vector(dUChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, INSERT, REVERSE);
    unsigned char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_short_vector_forward(void **state) {
    short_v* vec = init_vector(dShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_short_vector_reverse(void **state) {
    short_v* vec = init_vector(dShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, INSERT, REVERSE);
    short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ushort_vector_forward(void **state) {
    ushort_v* vec = init_vector(dUShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ushort_vector_reverse(void **state) {
    ushort_v* vec = init_vector(dUShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, INSERT, REVERSE);
    unsigned short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_int_vector_forward(void **state) {
    int_v* vec = init_vector(dInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_int_vector_reverse(void **state) {
    int_v* vec = init_vector(dInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, INSERT, REVERSE);
    int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_uint_vector_forward(void **state) {
    uint_v* vec = init_vector(dUInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_uint_vector_reverse(void **state) {
    uint_v* vec = init_vector(dUInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, INSERT, REVERSE);
    unsigned int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_long_vector_forward(void **state) {
    long_v* vec = init_vector(dLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_long_vector_reverse(void **state) {
    long_v* vec = init_vector(dLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, INSERT, REVERSE);
    long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ulong_vector_forward(void **state) {
    ulong_v* vec = init_vector(dULong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ulong_vector_reverse(void **state) {
    ulong_v* vec = init_vector(dULong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, INSERT, REVERSE);
    unsigned long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_llong_vector_forward(void **state) {
    llong_v* vec = init_vector(dLongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_llong_vector_reverse(void **state) {
    llong_v* vec = init_vector(dLongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, INSERT, REVERSE);
    long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ullong_vector_forward(void **state) {
    ullong_v* vec = init_vector(dULongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ullong_vector_reverse(void **state) {
    ullong_v* vec = init_vector(dULongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, INSERT, REVERSE);
    unsigned long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_float_vector_forward(void **state) {
    float_v* vec = init_vector(dFloat)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    float a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_float_vector_reverse(void **state) {
    float_v* vec = init_vector(dFloat)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, INSERT, REVERSE);
    float a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_double_vector_forward(void **state) {
    double_v* vec = init_vector(dDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_double_vector_reverse(void **state) {
    double_v* vec = init_vector(dDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, INSERT, REVERSE);
    double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ldouble_vector_forward(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    long double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_ldouble_vector_reverse(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, INSERT, REVERSE);
    long double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_bool_vector_forward(void **state) {
    bool_v* vec = init_vector(dBool)(7);
    push_vector(vec, true, 0);
    push_vector(vec, false, 1);
    push_vector(vec, true, 2);
    push_vector(vec, false, 3);
    push_vector(vec, true, 4);
    push_vector(vec, false, 5);
    push_vector(vec, true, 6);
    bool a[7] = {false, false, false, true, true, true, true};
    sort_vector(vec, INSERT, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_bool_vector_reverse(void **state) {
    bool_v* vec = init_vector(dBool)(7);
    push_vector(vec, true, 0);
    push_vector(vec, false, 1);
    push_vector(vec, true, 2);
    push_vector(vec, false, 3);
    push_vector(vec, true, 4);
    push_vector(vec, false, 5);
    push_vector(vec, true, 6);
    sort_vector(vec, INSERT, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_string_vector_forward(void **state) {
    string_v* vec = init_vector(dString)(7);
    push_vector(vec, "One", 0);
    push_vector(vec, "Two", 1);
    push_vector(vec, "Three", 2);
    push_vector(vec, "Four", 3);
    push_vector(vec, "Five", 4);
    push_vector(vec, "Six", 5);
    push_vector(vec, "Seven", 6);
    sort_vector(vec, INSERT, FORWARD);
    char *a[7] = {"Five", "Four", "One", "Seven", "Six", "Three", "Two"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], vec->data[i].data);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_sort_string_vector_reverse(void **state) {
    string_v* vec = init_vector(dString)(7);
    push_vector(vec, "One", 0);
    push_vector(vec, "Two", 1);
    push_vector(vec, "Three", 2);
    push_vector(vec, "Four", 3);
    push_vector(vec, "Five", 4);
    push_vector(vec, "Six", 5);
    push_vector(vec, "Seven", 6);
    sort_vector(vec, INSERT, REVERSE);
    char *a[7] = {"Two", "Three", "Six", "Seven", "One", "Four", "Five"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], vec->data[i].data);
    }
    free_vector(vec);
}
// ================================================================================
// ================================================================================
// TEST MERGE SORT FUNCTIONS

void test_merge_sort_char_vector_forward(void **state) {
    char_v* vec = init_vector(dChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_char_vector_reverse(void **state) {
    char_v* vec = init_vector(dChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_char_vector(vec, MERGE, REVERSE);
    char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_uchar_vector_forward(void **state) {
    uchar_v* vec = init_vector(dUChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_uchar_vector_reverse(void **state) {
    uchar_v* vec = init_vector(dUChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, MERGE, REVERSE);
    unsigned char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_short_vector_forward(void **state) {
    short_v* vec = init_vector(dShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_short_vector_reverse(void **state) {
    short_v* vec = init_vector(dShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, MERGE, REVERSE);
    short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ushort_vector_forward(void **state) {
    ushort_v* vec = init_vector(dUShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ushort_vector_reverse(void **state) {
    ushort_v* vec = init_vector(dUShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, MERGE, REVERSE);
    unsigned short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_int_vector_forward(void **state) {
    int_v* vec = init_vector(dInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_int_vector_reverse(void **state) {
    int_v* vec = init_vector(dInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, MERGE, REVERSE);
    int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_uint_vector_forward(void **state) {
    uint_v* vec = init_vector(dUInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_uint_vector_reverse(void **state) {
    uint_v* vec = init_vector(dUInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, MERGE, REVERSE);
    unsigned int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_long_vector_forward(void **state) {
    long_v* vec = init_vector(dLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_long_vector_reverse(void **state) {
    long_v* vec = init_vector(dLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, MERGE, REVERSE);
    long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ulong_vector_forward(void **state) {
    ulong_v* vec = init_vector(dULong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ulong_vector_reverse(void **state) {
    ulong_v* vec = init_vector(dULong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, MERGE, REVERSE);
    unsigned long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_llong_vector_forward(void **state) {
    llong_v* vec = init_vector(dLongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_llong_vector_reverse(void **state) {
    llong_v* vec = init_vector(dLongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, MERGE, REVERSE);
    long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ullong_vector_forward(void **state) {
    ullong_v* vec = init_vector(dULongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ullong_vector_reverse(void **state) {
    ullong_v* vec = init_vector(dULongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, MERGE, REVERSE);
    unsigned long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_float_vector_forward(void **state) {
    float_v* vec = init_vector(dFloat)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    float a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_float_vector_reverse(void **state) {
    float_v* vec = init_vector(dFloat)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, MERGE, REVERSE);
    float a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_double_vector_forward(void **state) {
    double_v* vec = init_vector(dDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_double_vector_reverse(void **state) {
    double_v* vec = init_vector(dDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, MERGE, REVERSE);
    double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ldouble_vector_forward(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    long double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_ldouble_vector_reverse(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, MERGE, REVERSE);
    long double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_bool_vector_forward(void **state) {
    bool_v* vec = init_vector(dBool)(7);
    push_vector(vec, true, 0);
    push_vector(vec, false, 1);
    push_vector(vec, true, 2);
    push_vector(vec, false, 3);
    push_vector(vec, true, 4);
    push_vector(vec, false, 5);
    push_vector(vec, true, 6);
    bool a[7] = {false, false, false, true, true, true, true};
    sort_vector(vec, MERGE, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_bool_vector_reverse(void **state) {
    bool_v* vec = init_vector(dBool)(7);
    push_vector(vec, true, 0);
    push_vector(vec, false, 1);
    push_vector(vec, true, 2);
    push_vector(vec, false, 3);
    push_vector(vec, true, 4);
    push_vector(vec, false, 5);
    push_vector(vec, true, 6);
    sort_vector(vec, MERGE, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_string_vector_forward(void **state) {
    string_v* vec = init_vector(dString)(7);
    push_vector(vec, "One", 0);
    push_vector(vec, "Two", 1);
    push_vector(vec, "Three", 2);
    push_vector(vec, "Four", 3);
    push_vector(vec, "Five", 4);
    push_vector(vec, "Six", 5);
    push_vector(vec, "Seven", 6);
    sort_vector(vec, MERGE, FORWARD);
    char *a[7] = {"Five", "Four", "One", "Seven", "Six", "Three", "Two"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], vec->data[i].data);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_merge_sort_string_vector_reverse(void **state) {
    string_v* vec = init_vector(dString)(7);
    push_vector(vec, "One", 0);
    push_vector(vec, "Two", 1);
    push_vector(vec, "Three", 2);
    push_vector(vec, "Four", 3);
    push_vector(vec, "Five", 4);
    push_vector(vec, "Six", 5);
    push_vector(vec, "Seven", 6);
    sort_vector(vec, MERGE, REVERSE);
    char *a[7] = {"Two", "Three", "Six", "Seven", "One", "Four", "Five"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], vec->data[i].data);
    }
    free_vector(vec);
}
// ================================================================================
// ================================================================================
// TEST QUICK SORT FUNCTIONS 

void test_quick_sort_char_vector_forward(void **state) {
    char_v* vec = init_vector(dChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_char_vector_reverse(void **state) {
    char_v* vec = init_vector(dChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, QUICK, REVERSE);
    char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_uchar_vector_forward(void **state) {
    uchar_v* vec = init_vector(dUChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_uchar_vector_reverse(void **state) {
    uchar_v* vec = init_vector(dUChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, QUICK, REVERSE);
    unsigned char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_short_vector_forward(void **state) {
    short_v* vec = init_vector(dShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_short_vector_reverse(void **state) {
    short_v* vec = init_vector(dShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, QUICK, REVERSE);
    short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_ushort_vector_forward(void **state) {
    ushort_v* vec = init_vector(dUShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_ushort_vector_reverse(void **state) {
    ushort_v* vec = init_vector(dUShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, QUICK, REVERSE);
    unsigned short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_int_vector_forward(void **state) {
    int_v* vec = init_vector(dInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_int_vector_reverse(void **state) {
    int_v* vec = init_vector(dInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, QUICK, REVERSE);
    int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_uint_vector_forward(void **state) {
    uint_v* vec = init_vector(dUInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_uint_vector_reverse(void **state) {
    uint_v* vec = init_vector(dUInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, QUICK, REVERSE);
    unsigned int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_long_vector_forward(void **state) {
    long_v* vec = init_vector(dLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_long_vector_reverse(void **state) {
    long_v* vec = init_vector(dLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, QUICK, REVERSE);
    long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_ulong_vector_forward(void **state) {
    ulong_v* vec = init_vector(dULong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_ulong_vector_reverse(void **state) {
    ulong_v* vec = init_vector(dULong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, QUICK, REVERSE);
    unsigned long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_llong_vector_forward(void **state) {
    llong_v* vec = init_vector(dLongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_llong_vector_reverse(void **state) {
    llong_v* vec = init_vector(dLongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, QUICK, REVERSE);
    long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_ullong_vector_forward(void **state) {
    ullong_v* vec = init_vector(dULongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_ullong_vector_reverse(void **state) {
    ullong_v* vec = init_vector(dULongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, QUICK, REVERSE);
    unsigned long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_float_vector_forward(void **state) {
    float_v* vec = init_vector(dFloat)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    float a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_float_vector_reverse(void **state) {
    float_v* vec = init_vector(dFloat)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, QUICK, REVERSE);
    float a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_double_vector_forward(void **state) {
    double_v* vec = init_vector(dDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_double_vector_reverse(void **state) {
    double_v* vec = init_vector(dDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, QUICK, REVERSE);
    double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_ldouble_vector_forward(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    long double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_ldouble_vector_reverse(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, QUICK, REVERSE);
    long double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_bool_vector_forward(void **state) {
    bool_v* vec = init_vector(dBool)(7);
    push_vector(vec, true, 0);
    push_vector(vec, false, 1);
    push_vector(vec, true, 2);
    push_vector(vec, false, 3);
    push_vector(vec, true, 4);
    push_vector(vec, false, 5);
    push_vector(vec, true, 6);
    bool a[7] = {false, false, false, true, true, true, true};
    sort_vector(vec, QUICK, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_bool_vector_reverse(void **state) {
    bool_v* vec = init_vector(dBool)(7);
    push_vector(vec, true, 0);
    push_vector(vec, false, 1);
    push_vector(vec, true, 2);
    push_vector(vec, false, 3);
    push_vector(vec, true, 4);
    push_vector(vec, false, 5);
    push_vector(vec, true, 6);
    sort_vector(vec, QUICK, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_string_vector_forward(void **state) {
    string_v* vec = init_vector(dString)(7);
    push_vector(vec, "One", 0);
    push_vector(vec, "Two", 1);
    push_vector(vec, "Three", 2);
    push_vector(vec, "Four", 3);
    push_vector(vec, "Five", 4);
    push_vector(vec, "Six", 5);
    push_vector(vec, "Seven", 6);
    sort_vector(vec, QUICK, FORWARD);
    char *a[7] = {"Five", "Four", "One", "Seven", "Six", "Three", "Two"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], vec->data[i].data);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_quick_sort_string_vector_reverse(void **state) {
    string_v* vec = init_vector(dString)(7);
    push_vector(vec, "One", 0);
    push_vector(vec, "Two", 1);
    push_vector(vec, "Three", 2);
    push_vector(vec, "Four", 3);
    push_vector(vec, "Five", 4);
    push_vector(vec, "Six", 5);
    push_vector(vec, "Seven", 6);
    sort_vector(vec, QUICK, REVERSE);
    char *a[7] = {"Two", "Three", "Six", "Seven", "One", "Four", "Five"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], vec->data[i].data);
    }
    free_vector(vec);
}
// ================================================================================
// ================================================================================
// TEST QUICK SORT FUNCTIONS

void test_tim_sort_char_vector_forward(void **state) {
    char_v* vec = init_vector(dChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_char_vector_reverse(void **state) {
    char_v* vec = init_vector(dChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, TIM, REVERSE);
    char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_uchar_vector_forward(void **state) {
    uchar_v* vec = init_vector(dUChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned char a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_uchar_vector_reverse(void **state) {
    uchar_v* vec = init_vector(dUChar)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, TIM, REVERSE);
    unsigned char a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_short_vector_forward(void **state) {
    short_v* vec = init_vector(dShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_short_vector_reverse(void **state) {
    short_v* vec = init_vector(dShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, TIM, REVERSE);
    short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_ushort_vector_forward(void **state) {
    ushort_v* vec = init_vector(dUShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned short int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_ushort_vector_reverse(void **state) {
    ushort_v* vec = init_vector(dUShort)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, TIM, REVERSE);
    unsigned short int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_int_vector_forward(void **state) {
    int_v* vec = init_vector(dInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_int_vector_reverse(void **state) {
    int_v* vec = init_vector(dInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, TIM, REVERSE);
    int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_uint_vector_forward(void **state) {
    uint_v* vec = init_vector(dUInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_uint_vector_reverse(void **state) {
    uint_v* vec = init_vector(dUInt)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, TIM, REVERSE);
    unsigned int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_long_vector_forward(void **state) {
    long_v* vec = init_vector(dLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_long_vector_reverse(void **state) {
    long_v* vec = init_vector(dLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, TIM, REVERSE);
    long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_ulong_vector_forward(void **state) {
    ulong_v* vec = init_vector(dULong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_ulong_vector_reverse(void **state) {
    ulong_v* vec = init_vector(dULong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, TIM, REVERSE);
    unsigned long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_llong_vector_forward(void **state) {
    llong_v* vec = init_vector(dLongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_llong_vector_reverse(void **state) {
    llong_v* vec = init_vector(dLongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, TIM, REVERSE);
    long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_ullong_vector_forward(void **state) {
    ullong_v* vec = init_vector(dULongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    unsigned long long int a[7] = {1, 2, 3, 5, 6, 10, 12};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_ullong_vector_reverse(void **state) {
    ullong_v* vec = init_vector(dULongLong)(7);
    push_vector(vec, 3, 0);
    push_vector(vec, 2, 1);
    push_vector(vec, 6, 2);
    push_vector(vec, 5, 3);
    push_vector(vec, 1, 4);
    push_vector(vec, 12, 5);
    push_vector(vec, 10, 6);
    sort_vector(vec, TIM, REVERSE);
    unsigned long long int a[7] = {12, 10, 6, 5, 3, 2, 1};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_float_vector_forward(void **state) {
    float_v* vec = init_vector(dFloat)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    float a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_float_vector_reverse(void **state) {
    float_v* vec = init_vector(dFloat)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, TIM, REVERSE);
    float a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_float_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_double_vector_forward(void **state) {
    double_v* vec = init_vector(dDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_double_vector_reverse(void **state) {
    double_v* vec = init_vector(dDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, TIM, REVERSE);
    double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_ldouble_vector_forward(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    long double a[7] = {1., 2., 3., 5., 6., 10., 12.};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_ldouble_vector_reverse(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(7);
    push_vector(vec, 3., 0);
    push_vector(vec, 2., 1);
    push_vector(vec, 6., 2);
    push_vector(vec, 5., 3);
    push_vector(vec, 1., 4);
    push_vector(vec, 12., 5);
    push_vector(vec, 10., 6);
    sort_vector(vec, TIM, REVERSE);
    long double a[7] = {12., 10., 6., 5., 3., 2., 1.};
    for (size_t i = 0; i < vec->len; i++) {
        assert_double_equal(a[i], vec->data[i], 1.0e-3);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_bool_vector_forward(void **state) {
    bool_v* vec = init_vector(dBool)(7);
    push_vector(vec, true, 0);
    push_vector(vec, false, 1);
    push_vector(vec, true, 2);
    push_vector(vec, false, 3);
    push_vector(vec, true, 4);
    push_vector(vec, false, 5);
    push_vector(vec, true, 6);
    bool a[7] = {false, false, false, true, true, true, true};
    sort_vector(vec, TIM, FORWARD);
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_bool_vector_reverse(void **state) {
    bool_v* vec = init_vector(dBool)(7);
    push_vector(vec, true, 0);
    push_vector(vec, false, 1);
    push_vector(vec, true, 2);
    push_vector(vec, false, 3);
    push_vector(vec, true, 4);
    push_vector(vec, false, 5);
    push_vector(vec, true, 6);
    sort_vector(vec, TIM, REVERSE);
    bool a[7] = {true, true, true, true, false, false, false};
    for (size_t i = 0; i < vec->len; i++) {
        assert_int_equal(a[i], vec->data[i]);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_string_vector_forward(void **state) {
    string_v* vec = init_vector(dString)(7);
    push_vector(vec, "One", 0);
    push_vector(vec, "Two", 1);
    push_vector(vec, "Three", 2);
    push_vector(vec, "Four", 3);
    push_vector(vec, "Five", 4);
    push_vector(vec, "Six", 5);
    push_vector(vec, "Seven", 6);
    sort_vector(vec, TIM, FORWARD);
    char *a[7] = {"Five", "Four", "One", "Seven", "Six", "Three", "Two"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], vec->data[i].data);
    }
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_tim_sort_string_vector_reverse(void **state) {
    string_v* vec = init_vector(dString)(7);
    push_vector(vec, "One", 0);
    push_vector(vec, "Two", 1);
    push_vector(vec, "Three", 2);
    push_vector(vec, "Four", 3);
    push_vector(vec, "Five", 4);
    push_vector(vec, "Six", 5);
    push_vector(vec, "Seven", 6);
    sort_vector(vec, TIM, REVERSE);
    char *a[7] = {"Two", "Three", "Six", "Seven", "One", "Four", "Five"};
    for (size_t i = 0; i < vec->len; i++) {
        assert_string_equal(a[i], vec->data[i].data);
    }
    free_vector(vec);
}
// ================================================================================
// ================================================================================
// TEST BINARY SEARCH FUNCTIONS 

void test_char_binary_vector_search(void **state) {
    char_v* vec = init_vector(dChar)(20);
    char a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, 13, false);
    assert_int_equal(vec->data[(size_t)b], 13);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_uchar_binary_vector_search(void **state) {
    uchar_v* vec = init_vector(dUChar)(20);
    unsigned char a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, 13, false);
    assert_int_equal(vec->data[(size_t)b], 13);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_short_binary_vector_search(void **state) {
    short_v* vec = init_vector(dShort)(20);
    short int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, 13, false);
    assert_int_equal(vec->data[(size_t)b], 13);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_ushort_binary_vector_search(void **state) {
    ushort_v* vec = init_vector(dUShort)(20);
    unsigned short int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, 13, false);
    assert_int_equal(vec->data[(size_t)b], 13);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_int_binary_vector_search(void **state) {
    int_v* vec = init_vector(dInt)(20);
    int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, 13, false);
    assert_int_equal(vec->data[(size_t)b], 13);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_uint_binary_vector_search(void **state) {
    uint_v* vec = init_vector(dUInt)(20);
    unsigned int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, 13, false);
    assert_int_equal(vec->data[(size_t)b], 13);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_long_binary_vector_search(void **state) {
    long_v* vec = init_vector(dLong)(20);
    long int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, 13, false);
    assert_int_equal(vec->data[(size_t)b], 13);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_ulong_binary_vector_search(void **state) {
    ulong_v* vec = init_vector(dULong)(20);
    unsigned long int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, 13, false);
    assert_int_equal(vec->data[(size_t)b], 13);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_llong_binary_vector_search(void **state) {
    llong_v* vec = init_vector(dLongLong)(20);
    short int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, 13, false);
    assert_int_equal(vec->data[(size_t)b], 13);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_ullong_binary_vector_search(void **state) {
    ullong_v* vec = init_vector(dULongLong)(20);
    unsigned long long int a[20] = { 1, 5, 2, 7, 8, 9, 4, 3, 6, 14, 13, 12, 11, 10, 16, 17, 18, 19, 20, 15 };
    for (size_t i = 0; i < 20; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, 13, false);
    assert_int_equal(vec->data[(size_t)b], 13);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_float_binary_vector_search(void **state) {
    float_v* vec = init_vector(dFloat)(20);
    float a[20] = { 1.2, 5.6, 2.1, 7.7, 8., 9., 4.2, 3.8, 6.1, 14.4, 13.6, 
                    12.9, 11.11, 10.01, 16.3, 17.4, 18.5, 19.6, 20.7, 15.8 };
    for (size_t i = 0; i < 20; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, 11.11, false);
    assert_float_equal(vec->data[(size_t)b], 11.11, 1.0e-6);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_double_binary_vector_search(void **state) {
    double_v* vec = init_vector(dDouble)(20);
    double a[20] = { 1.2, 5.6, 2.1, 7.7, 8., 9., 4.2, 3.8, 6.1, 14.4, 13.6, 
                    12.9, 11.11, 10.01, 16.3, 17.4, 18.5, 19.6, 20.7, 15.8 };
    for (size_t i = 0; i < 20; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, 11.11, false);
    assert_double_equal(vec->data[(size_t)b], 11.11, 1.0e-6);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_ldouble_binary_vector_search(void **state) {
    ldouble_v* vec = init_vector(dLDouble)(20);
    long double a[20] = { 1.2, 5.6, 2.1, 7.7, 8., 9., 4.2, 3.8, 6.1, 14.4, 13.6, 
                    12.9, 11.11, 10.01, 16.3, 17.4, 18.5, 19.6, 20.7, 15.8 };
    for (size_t i = 0; i < 20; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, 11.11, false);
    assert_double_equal(vec->data[(size_t)b], 11.11, 1.0e-6);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_bool_binary_vector_search(void **state) {
    bool_v* vec = init_vector(dBool)(5);
    bool a[5] = {false, true, false, true, false};
    for (size_t i = 0; i < 5; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, true, false);
    assert_int_equal(vec->data[(size_t)b], true);
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_string_binary_vector_search(void **state) {
    string_v* vec = init_vector(dString)(5);
    char *a[5] = {"One", "Two", "Three", "Four", "Five"};
    for (size_t i = 0; i < 5; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    int b = bsearch_vector(vec, "Three", false);
    assert_string_equal(vec->data[(size_t)b].data, "Three");
    free_vector(vec);
}
// --------------------------------------------------------------------------------

void test_str_binary_vector_search(void **state) {
    string_v* vec = init_vector(dString)(5);
    char *a[5] = {"One", "Two", "Three", "Four", "Five"};
    for (size_t i = 0; i < 5; i++) {
        push_vector(vec, a[i], vector_length(vec));
    }
    str* c = init_string("Three");
    int b = bsearch_str_vector(vec, c, false);
    assert_string_equal(vec->data[(size_t)b].data, "Three");
    free_vector(vec);
    free_string(c);
}
// ================================================================================
// ================================================================================
// eof
