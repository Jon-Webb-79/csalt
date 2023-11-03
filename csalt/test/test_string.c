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

void uppercase_char(char* a) {
    if (*a >= 'a' && *a <= 'z') {
        *a -= 32;
    }
}

/**
 * Test to ensure the init_string macro works properly with one argument
 */
void test_string_init_one_var(void **state)
{
	(void) state;
    str *one = init_string("Hello!");
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
    str *one = init_string("Hello!", 20);
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
    str *one = init_string("Hello!", 1);
    assert_int_equal(6, string_length(one));
    assert_int_equal(7, string_memory(one));
    assert_string_equal(get_string(one), "Hello!");
    free_string(one);
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure the gbc_str macro assigns a garbage collector to a variable
 */ 
#ifdef __GNUC__
void test_string_init_gbc(void **state)
{
	(void) state;
    str *one gbc_str = init_string("Hello!", 1);
    assert_int_equal(6, string_length(one));
    assert_int_equal(7, string_memory(one));
    assert_string_equal(get_string(one), "Hello!");
}
#endif
// --------------------------------------------------------------------------------
//
/**
 * Test to make sure an attempt to free memory after garbage collection is 
 * not harmful to code execution.
 */
void test_string_init_post_free(void **state)
{
 	(void) state;
    str *one gbc_str = init_string("Hello!", 1);
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
    str *one = init_string("Hello");
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
    str *one = init_string("Hello", 20);
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
    str *one = init_string("Hello");
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
    str *one = init_string("Hello");
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
    str *one = init_string("Hello");
    str *two = init_string(" World!");
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
    str *one = init_string("Hello", 20);
    str *two = init_string(" World!");
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
    (void) state;
    str *one = init_string("Hello");
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
    (void) state;
    str *one = init_string("Hello", 50);
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
    (void) state;
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
    (void) state;
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
    (void) state;
    str *one = init_string("Hello", 20);
    str *two = copy_string(one);
    assert_string_equal(get_string(one), get_string(two));
    assert_int_equal(string_length(one), string_length(two));
    assert_int_equal(string_memory(one), string_memory(two));
    free_string(one);
    free_string(two);
}
// --------------------------------------------------------------------------------

/**
 * Test to ensure copy_string creates a deep copy of the string passed to it
 * with garbage collection.
 */
#ifdef __GNUC__
void test_copy_string_w_gbc(void **state) {
    (void) state;
    str *one gbc_str = init_string("Hello", 20);
    str *two gbc_str = copy_string(one);
    assert_string_equal(get_string(one), get_string(two));
    assert_int_equal(string_length(one), string_length(two));
    assert_int_equal(string_memory(one), string_memory(two));
}
#endif
// --------------------------------------------------------------------------------

void test_compare_strings_lit_equal(void **state) {
    (void) state;
    str *one = init_string("Hello");
    int val = compare_strings(one, "Hello");
    assert_int_equal(val, 0);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_compare_strings_lit_greater(void **state) {
    (void) state;
    str *one = init_string("Hello");
    int val = compare_strings(one, "Henlo");
    assert_int_equal(val, -2);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_compare_strings_lit_less(void **state) {
    (void) state;
    str *one = init_string("Hello");
    int val = compare_strings(one, "Healo");
    assert_int_equal(val, 11);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_compare_strings_lit_oversize(void **state) {
    (void) state;
    str *one = init_string("Hello");
    int val = compare_strings(one, "Helloo");
    assert_int_equal(val, -1);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_compare_strings_str_equal(void **state) {
    (void) state;
    str *one = init_string("Hello");
    str *two = init_string("Hello");
    int val = compare_strings(one, two);
    assert_int_equal(val, 0);
    free_string(one);
    free_string(two);
}
// --------------------------------------------------------------------------------

void test_compare_strings_str_greater(void **state) {
    (void) state;
    str *one = init_string("Hello");
    str *two = init_string("Henlo");
    int val = compare_strings(one, two);
    assert_int_equal(val, -2);
    free_string(one);
    free_string(two);
}
// --------------------------------------------------------------------------------

void test_compare_strings_str_less(void **state) {
    (void) state;
    str *one = init_string("Hello");
    str *two = init_string("Healo");
    int val = compare_strings(one, two);
    assert_int_equal(val, 11);
    free_string(one);
    free_string(two);
}
// --------------------------------------------------------------------------------

void test_compare_strings_str_oversize(void **state) {
    (void) state;
    str *one = init_string("Hello");
    str *two = init_string("Helloo");
    int val = compare_strings(one, two);
    assert_int_equal(val, -1);
    free_string(one);
    free_string(two);
}
// --------------------------------------------------------------------------------

void test_find_first_char(void **state) {
    (void) state;
    str *one = init_string("Hello");
    char *ptr = first_char('l', get_string(one), get_string(one) + string_length(one));
    //char *ptr = first_char(one, 'l');
    assert_non_null(ptr);
    assert_ptr_equal(ptr, one->data + 2);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_find_first_char_btw_pointers(void **state) {
    (void) state;
    char* a = "abecdefghi";
    char c = 'e';
    char* b = first_char(c, a + 3, a + strlen(a));
    assert_string_equal(b, "efghi");
}
// --------------------------------------------------------------------------------

void test_find_first_char_does_not_exist(void **state) {
    (void) state;
    str *one = init_string("Hello");
    char* ptr = first_char('q', get_string(one), get_string(one) + string_length(one));
    assert_null(ptr);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_find_first_char_null_terminator(void **state) {
    (void) state;
    str *one = init_string("Hello");
    char* ptr = first_char('\0', get_string(one), get_string(one) + string_length(one) + 1);
    assert_non_null(ptr);
    assert_ptr_equal(ptr, one->data + 5);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_find_last_char(void **state) {
    (void) state;
    str *one = init_string("Hello");
    char* ptr = last_char('l', get_string(one), get_string(one) + string_length(one));
    assert_non_null(ptr);
    assert_ptr_equal(ptr, one->data + 3);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_find_last_char_btw_pointers(void **state) {
    (void) state;
    char* a = "abecdefghi";
    char c = 'e';
    char* b = last_char(c, a, a + strlen(a));
    assert_string_equal(b, "efghi");
}
// --------------------------------------------------------------------------------

void test_find_last_char_does_not_exist(void **state) {
    (void) state;
    str *one = init_string("Hello");
    char* ptr = last_char('q', get_string(one), get_string(one) + string_length(one));
    assert_null(ptr);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_find_last_char_null_terminator(void **state) {
    (void) state;
    str *one = init_string("Hello");
    char* ptr = last_char('\0', get_string(one), get_string(one) + string_length(one));
    assert_non_null(ptr);
    assert_ptr_equal(ptr, one->data + 5);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_find_first_string_lit(void **state) {
    (void) state;
    str *one = init_string("Where in the World is Carmen SanDiego!");
    char *two = "World";
    char *ptr = first_substring(two, get_string(one), get_string(one) + string_length(one));
    assert_non_null(ptr);
    assert_ptr_equal(ptr, one->data + 13);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_find_first_string_lit_null(void **state) {
    (void) state;
    str *one = init_string("Where in the World is Carmen SanDiego!");
    char *two = "What";
    char *ptr = first_substring(two, get_string(one), get_string(one) + string_length(one));
    assert_null(ptr);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_find_first_string_lit_oversized(void **state) {
    (void) state;
    str *one = init_string("Where");
    char *two = "What is going on";
    char *ptr = first_substring(two, get_string(one), get_string(one) + string_length(one));
    assert_null(ptr);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_find_first_string_str(void **state) {
    (void) state;
    str *one = init_string("Where in the World is Carmen SanDiego!");
    str *two = init_string("World");
    char *ptr = first_substring(two, get_string(one), get_string(one) + string_length(one));
    assert_non_null(ptr);
    assert_ptr_equal(ptr, one->data + 13);
    free_string(one);
    free_string(two);
}
// --------------------------------------------------------------------------------

void test_find_first_string_str_null(void **state) {
    (void) state;
    str *one = init_string("Where in the World is Carmen SanDiego!");
    str *two = init_string("What");
    char *ptr = first_substring(two, get_string(one), get_string(one) + string_length(one));
    assert_null(ptr);
    free_string(one);
    free_string(two);
}
// --------------------------------------------------------------------------------

void test_find_first_string_str_oversized(void **state) {
    (void) state;
    str *one = init_string("Where");
    str *two = init_string("What is going on");
    char *ptr = first_substring(two, get_string(one), get_string(one) + string_length(one));
    assert_null(ptr);
    free_string(one);
    free_string(two);
}
// --------------------------------------------------------------------------------

void test_find_last_string_lit(void **state) {
    (void) state;
    str *one = init_string("Hello this is Hello again!");
    char *two = "Hello";
    char *ptr = last_substring(two, get_string(one), get_string(one) + string_length(one));
    assert_non_null(ptr);
    assert_ptr_equal(ptr, one->data + 14);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_find_last_string_lit_null(void **state) {
    (void) state;
    str *one = init_string("Where in the World is Carmen SanDiego!");
    char *two = "What";
    char *ptr = last_substring(two, get_string(one), get_string(one) + string_length(one));
    assert_null(ptr);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_find_last_string_lit_oversized(void **state) {
    (void) state;
    str *one = init_string("Where");
    char *two = "What is going on";
    char *ptr = last_substring(two, get_string(one), get_string(one) + string_length(one));
    assert_null(ptr);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_find_last_string_str(void **state) {
    (void) state;
    str *one = init_string("Hello this is Hello again!");
    str *two = init_string("Hello");
    char *ptr = last_substring(two, get_string(one), get_string(one) + string_length(one));
    assert_non_null(ptr);
    assert_ptr_equal(ptr, one->data + 14);
    free_string(one);
    free_string(two);
}
// --------------------------------------------------------------------------------

void test_find_last_string_str_null(void **state) {
    (void) state;
    str *one = init_string("Where in the World is Carmen SanDiego!");
    str *two = init_string("What");
    char *ptr = last_substring(two, get_string(one), get_string(one) + string_length(one));
    assert_null(ptr);
    free_string(one);
    free_string(two);
}
// --------------------------------------------------------------------------------

void test_find_last_string_str_oversized(void **state) {
    (void) state;
    str *one = init_string("Where");
    str *two = init_string("What is going on");
    char *ptr = last_substring(two, get_string(one), get_string(one) + string_length(one));
    assert_null(ptr);
    free_string(one);
    free_string(two);
}
// --------------------------------------------------------------------------------

void test_pop_string_end_char(void **state) {
    str *one = init_string("Hello");
    char var = pop_string_char(one);
    assert_int_equal(var, 'o');
    assert_string_equal(get_string(one), "Hell");
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_pop_string_end_null_struct(void **state) {
    (void) state;
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    str string_struct = { .data = NULL, .len = 0, .alloc = 0 };
    char val = pop_string_char(&string_struct);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
    assert_int_equal(val, '\0');
}
// --------------------------------------------------------------------------------

void test_pop_string_end_null_data(void **state) {
    (void) state;
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    char val = pop_string_char(NULL);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
    assert_int_equal(val, '\0');
}
// --------------------------------------------------------------------------------

void test_pop_string_char(void **state) {
    str *one = init_string("Hello");
    char var = pop_string_char(one, 2);
    assert_int_equal(var, 'l');
    assert_string_equal(get_string(one), "Helo");
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_pop_string_null_struct(void **state) {
    (void) state;
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    str string_struct = { .data = NULL, .len = 0, .alloc = 0 };
    char val = pop_string_char(&string_struct, 2);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
    assert_int_equal(val, '\0');
}
// --------------------------------------------------------------------------------

void test_pop_string_null_data(void **state) {
    (void) state;
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    char val = pop_string_char(NULL, 2);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
    assert_int_equal(val, '\0');
}
// --------------------------------------------------------------------------------

void test_pop_string_out_of_bounds(void **state) {
    (void) state;
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    str *one = init_string("Hello");
    char val = pop_string_char(one, 12);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
    assert_int_equal(val, '\0');
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_pop_string_token(void **state) {
    (void) state;
    str *one = init_string("2023/10/24");
    str *two = pop_string_token(one, '/');
    assert_string_equal("24", get_string(two));
    assert_string_equal("2023/10", get_string(one));
    assert_int_equal(7, string_length(one));
    free_string(one);
    free_string(two);
}
// --------------------------------------------------------------------------------

void test_pop_string_token_no_token(void **state) {
    (void) state;
    str *one = init_string("2023/10/24");
    str *two = pop_string_token(one, '+');
    assert_null(two);
    assert_string_equal("2023/10/24", get_string(one));
    assert_int_equal(10, string_length(one));
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_pop_string_token_null_struct(void **state) {
    (void) state;
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    str string_struct = { .data = NULL, .len = 0, .alloc = 0 };
    str *val = pop_string_token(&string_struct, '/');
    assert_null(val);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);
}
// --------------------------------------------------------------------------------

void test_pop_string_token_null_data(void **state) {
    (void) state;
    int stderr_copy = dup(STDERR_FILENO);
    int devnull = open("/dev/null", O_WRONLY);
    dup2(devnull, STDERR_FILENO);
    close(devnull);
    str *val = pop_string_token(NULL, '/');
    assert_null(val);
    // Restore stderr
    dup2(stderr_copy, STDERR_FILENO);
    close(stderr_copy);    
}
// --------------------------------------------------------------------------------

#ifdef __GNUC__
void test_pop_string_token_gbc_true(void **state) {
    (void) state;
    str *one = init_string("2023/10/24");
    str *two gbc_str = pop_string_token(one, '/');
    assert_string_equal("24", get_string(two));
    assert_string_equal("2023/10", get_string(one));
    assert_int_equal(7, string_length(one));
    free_string(one);
}
#endif
// --------------------------------------------------------------------------------

void test_char_iterator_for_loop(void **state) {
    (void) state;
    str *one = init_string("This is a Long String");
    str_iterator it = init_str_iterator();
    char* begin = it.begin(one);
    char* end = it.end(one);
    char a;
    for (char* i =  begin; i != end; it.next(&i)) {
        a = it.get(&i);
        if (a >= 'a' && a <= 'z') *i -= 32;
    }
    assert_string_equal("THIS IS A LONG STRING", one->data);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_char_iterator_while_loop(void **state) {
    (void) state;
    str *one = init_string("This is a Long String");
    str_iterator it = init_str_iterator();
    char* begin = it.begin(one);
    char* end = it.end(one);
    char a; 
    while(begin != end) {
        a = it.get(&begin);
        if (a >= 'a' && a <= 'z') *begin -= 32;
        it.next(&begin);
    }
    assert_string_equal("THIS IS A LONG STRING", one->data); 
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_char_iterator_forward(void **state) {
    (void) state;
    str *one = init_string("This is a Long String");
    str_iterator it = init_str_iterator();
    char* begin = it.begin(one);
    char* end = it.end(one); 
    dec_str_iter(begin + 3, end, FORWARD, uppercase_char);
    assert_string_equal("ThiS IS A LONG STRING", get_string(one));
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_char_iterator_reverse(void **state) {
    (void) state;
    str *one = init_string("This is a Long String");
    str_iterator it = init_str_iterator();
    char* begin = it.begin(one);
    char* end = it.end(one); 
    dec_str_iter(end, begin - 1, REVERSE, uppercase_char);
    assert_string_equal("THIS IS A LONG STRING", get_string(one));
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_convert_to_uppercase(void **state) {
    (void) state;
    str *one = init_string("This is a Long String"); 
    to_uppercase(one);
    assert_string_equal("THIS IS A LONG STRING", get_string(one));
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_convert_to_lowercase(void **state) {
    (void) state;
    str *one = init_string("This is a Long String"); 
    to_lowercase(one);
    assert_string_equal("this is a long string", get_string(one));
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_ptr_in_container(void **state) {
    (void) state;
    str* one = init_string("Hello Again!");
    bool  result = ptr_in_str_container(one, one->data + 3);
    assert_true(result);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_ptr_not_in_container(void **state) {
    (void) state;
    str* one = init_string("Hello Again!");
    bool  result = ptr_in_str_container(one, one->data - 3);
    assert_false(result);
    free_string(one);
}
// --------------------------------------------------------------------------------

void test_ptr_in_literal(void **state) {
    (void) state;
    char* one = "Hello Again!";
    bool result = ptr_in_literal(one + 2, one, one + strlen(one));
    assert_true(result);
}
// --------------------------------------------------------------------------------

void test_ptr_not_in_literal(void **state) {
    (void) state;
    char* one = "Hello Again!";
    bool result = ptr_in_literal(one - 2, one, one + strlen(one));
    assert_false(result);
}
// --------------------------------------------------------------------------------

void test_find_first_literal_btw_pointers(void **state) {
    (void) state;
    char* one = "this is this Test";
    char* input = "this";
    char* two = first_substring(input, one + 5, one + strlen(one));
    assert_string_equal(two, "this Test");
}
// --------------------------------------------------------------------------------

void test_find_first_str_btw_pointers(void **state) {
    (void) state;
    str* one = init_string("this is this Test");
    str* three = init_string("this");
    char* two = first_substring(three, one->data + 5, one->data + one->len);
    assert_string_equal(two, "this Test");
    free_string(one);
    free_string(three);
}
// --------------------------------------------------------------------------------

void test_find_last_str_between_pointers(void **state) {
    (void) state;
    char* one = "This is a test, I repeat, this is a test";
    char* two = "test";
    char* three = last_substring(two, one, one + 17);
    assert_string_equal(three, "test, I repeat, this is a test");
}
// --------------------------------------------------------------------------------

void test_drop_str_substring_char(void **state) {
    (void) state;
    str* one = init_string("Remove all Remove instances from Remove");
    char* pattern = "Remove";
    drop_substring(one, pattern, get_string(one), get_string(one) + string_length(one));
    assert_string_equal(one->data, "all instances from ");
    free_string(one);
}
// -------------------------------------------------------------------------------

void test_drop_str_substring_str(void **state) {
    (void) state;
    str* one = init_string("Remove all Remove instances from Remove");
    str* pattern = init_string("Remove");
    drop_substring(one, pattern, get_string(one), get_string(one) + string_length(one));
    assert_string_equal(one->data, "all instances from ");
    free_string(one);
    free_string(pattern);
}

// ================================================================================
// ================================================================================
// eof
