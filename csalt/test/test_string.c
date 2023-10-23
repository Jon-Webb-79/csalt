// ================================================================================
// ================================================================================
// - File:    test_string.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    October 20, 2023
// - Version: 1.0
// - Copyright: Copyright 2023, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "test_string.h"

/**
 * Test to ensure the init_string macro works properly with one argument
 */
void test_string_init_one_var(void **state)
{
	(void) state;
    str *one init_string("Hello!");
    assert_int_equal(6, string_length(one));
    assert_int_equal(7, string_memory(one));
    assert_string_equal(get_string(one), "Hello!");
    free_string(one);
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure the init_string macro works properly with two arguments
 */
void test_string_init_two_var(void **state)
{
	(void) state;
    str *one init_string("Hello!", 20);
    assert_int_equal(6, string_length(one));
    assert_int_equal(20, string_memory(one));
    assert_string_equal(get_string(one), "Hello!");
    free_string(one);
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure the init_string macro works properly when the buffer is 
 * smaller than the string
 */
void test_string_init_two_var_less(void **state)
{
	(void) state;
    str *one init_string("Hello!", 1);
    assert_int_equal(6, string_length(one));
    assert_int_equal(7, string_memory(one));
    assert_string_equal(get_string(one), "Hello!");
    free_string(one);
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure garbage collection works with one variable passed 
 * to init_string macro
 */
void test_string_init_one_var_gbc(void **state)
{
	(void) state;
    str* one init_string_gbc("Hello!");
    assert_int_equal(6, string_length(one));
    assert_int_equal(7, string_memory(one));
    assert_string_equal(get_string(one), "Hello!");
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure garbage collection works with two variables passed 
 * to init_string macro
 */
void test_string_init_two_var_gbc(void **state)
{
	(void) state;
    str *one init_string_gbc("Hello!", 20);
    assert_int_equal(6, string_length(one));
    assert_int_equal(20, string_memory(one));
    assert_string_equal(get_string(one), "Hello!");
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure garbage collection works with buffer undersized from string
 */
void test_string_init_two_var_less_gbc(void **state)
{
	(void) state;
    str *one init_string_gbc("Hello!", 1);
    assert_int_equal(6, string_length(one));
    assert_int_equal(7, string_memory(one));
    assert_string_equal(get_string(one), "Hello!");
}
// --------------------------------------------------------------------------------

/**
 * Test to make sure an attempt to free memory after garbage collection is 
 * not harmful to code execution.
 */
void test_string_init_post_free(void **state)
{
	(void) state;
    str *one init_string_gbc("Hello!", 1);
    assert_int_equal(6, string_length(one));
    assert_int_equal(7, string_memory(one));
    assert_string_equal(get_string(one), "Hello!");
    free_string(one);
}
// --------------------------------------------------------------------------------

/**
 * Verify that get_string function fails properly whena a null str struct 
 * is passed to it. Supressed stderr for this test
 */
void test_get_string_fail_one(void **state) {
    (void) state;
    // Redirect stderr to /dev/null
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    char *result = get_string(NULL);
    assert_null(result);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
}
// --------------------------------------------------------------------------------

/**
 * Verify that get_string function fails properly when a str struct containing
 * a null pointer to data is passed to it. Supressed stderr for this test
 */ 
void test_get_string_fail_two(void **state) {
    (void) state;
    // Redirect stderr to /dev/null
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    str string_struct = { .data = NULL, .len = 0, .alloc = 0 };
    char *result = get_string(&string_struct);
    assert_null(result);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
} 
// --------------------------------------------------------------------------------

/**
 * Verify string_length will fail nicely when a NULL struct is passed to it.
 * Suppressed stderr for this test.
 */
void test_string_length_fail_one(void **state) {
    (void) state;
    // Redirect stderr to /dev/null
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    size_t result = string_length(NULL);
    assert_int_equal(result, -1);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
}
// --------------------------------------------------------------------------------

/**
 * Verify string_length will fail nicely when a NULL data pointer is passed to 
 * it.  Suppressed stderr for this test.
 */
void test_string_length_fail_two(void **state) {
    (void) state;
    // Redirect stderr to /dev/null
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    str string_struct = { .data = NULL, .len = 0, .alloc = 0 };
    size_t result = string_length(&string_struct);
    assert_int_equal(result, -1);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
}
// --------------------------------------------------------------------------------

/**
 * Test insert_string with a literal inserted at the end.
 */
void test_insert_string_lit_insert_end(void **state) {
    (void) state;
    str *one init_string("Hello");
    bool return_value = insert_string(one, " World!", string_length(one));
    assert_string_equal(get_string(one), "Hello World!");
    assert_int_equal(string_length(one), 12);
    assert_int_equal(string_memory(one), 25);
    free_string(one);
    assert_true(return_value);
}
// --------------------------------------------------------------------------------

/**
 * Test insert_string with a string inserted in the middle of the original
 * string
 */
void test_insert_string_lit_insert_middle(void **state) {
    (void) state;
    str *one init_string("Hello", 20);
    bool return_value = insert_string(one, " World!", 2);
    assert_string_equal(get_string(one), "He World!llo");
    assert_int_equal(string_length(one), 12);
    assert_int_equal(string_memory(one), 20);
    free_string(one);
    assert_true(return_value);
}
// --------------------------------------------------------------------------------

/**
 * Test insert_string with a NULL original container insrted.
 * Suppressed stderr for this test
 */
void test_insert_string_lit_error_one(void **state) {
    (void) state; 
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    str string_struct = { .data = NULL, .len = 0, .alloc = 0 };
    bool result = insert_string(&string_struct, "World!", 0);
    assert_false(result);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
}
// --------------------------------------------------------------------------------

/**
 * Test insert_string with a NULL string_literal insrted.
 * Suppressed stderr for this test
 */
void test_insert_string_lit_error_two(void **state) {
    (void) state; 
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    str *one init_string("Hello");
    bool result = insert_string(one, NULL, 0);
    assert_false(result);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
    free_string(one);
}
// --------------------------------------------------------------------------------

/**
 * Test insert_string with a length that is out of bounds
 * Suppressed stderr for this test
 */
void test_insert_string_lit_error_three(void **state) {
    (void) state; 
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    str *one init_string("Hello");
    bool result = insert_string(one, " World!", 50);
    assert_false(result);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
    free_string(one);
}
// --------------------------------------------------------------------------------

/**
 * Test insert_string with a str container inserted at the end.
 */
void test_insert_string_str_insert_end(void **state) {
    (void) state;
    str *one init_string("Hello");
    str *two init_string(" World!");
    bool return_value = insert_string(one, two, string_length(one));
    assert_string_equal(get_string(one), "Hello World!");
    assert_int_equal(string_length(one), 12);
    assert_int_equal(string_memory(one), 25);
    free_string(one);
    free_string(two);
    assert_true(return_value);
}
// --------------------------------------------------------------------------------

/**
 * Test insert_string with a str container inserted in the middle of the original
 * string. Applicable error handling was tested in previous tests.
 */
void test_insert_string_str_insert_middle(void **state) {
    (void) state;
    str *one init_string("Hello", 20);
    str *two init_string(" World!");
    bool return_value = insert_string(one, two, 2);
    assert_string_equal(get_string(one), "He World!llo");
    assert_int_equal(string_length(one), 12);
    assert_int_equal(string_memory(one), 20);
    free_string(one);
    free_string(two);
    assert_true(return_value);
}
// ================================================================================
// ================================================================================

/**
 * Test trim_string to ensure nothing happens if string is properly sized
 */
void test_trim_string_equal(void **state) {
    str *one init_string("Hello");
    bool val = trim_string(one);
    assert_true(val);
    assert_int_equal(string_length(one), 5);
    assert_int_equal(string_length(one) + 1, string_memory(one));
    free_string(one);
}
// --------------------------------------------------------------------------------

/**
 * Test trim_string to ensure that the memory allocation is downsized when 
 * approprioate
 */
void test_trim_string_greater(void **state) {
    str *one init_string("Hello", 50);
    bool val = trim_string(one);
    assert_true(val);
    assert_int_equal(string_length(one), 5);
    assert_int_equal(string_length(one) + 1, string_memory(one));
    free_string(one);
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure trim_string fails properly when the struct passed to the 
 * function is a NULL value.  stderr suppressed for this test.
 */
void test_trim_string_error_one(void **state) {
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    bool val = trim_string(NULL);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
    assert_false(val);
} 
// --------------------------------------------------------------------------------

/**
 * Test to ensure trim_string fails properly when the string passed to the 
 * function is a NULL value.  stderr suppressed for this test.
 */
void test_trim_string_error_two(void **state) {
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    str string_struct = { .data = NULL, .len = 0, .alloc = 0 };
    bool val = trim_string(&string_struct);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
    assert_false(val);
}
// ================================================================================
// ================================================================================

/**
 * Test to ensure copy_string creates a deep copy of the string passed to it.
 */
void test_copy_string(void **state) {
    str *one init_string("Hello", 20);
    str *two = copy_string(one);
    assert_string_equal(get_string(one), get_string(two));
    assert_int_equal(string_length(one), string_length(two));
    assert_int_equal(string_memory(one), string_memory(two));
    free_string(one);
    free_string(two);
}
// ================================================================================
// ================================================================================
// eof
