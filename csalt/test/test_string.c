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
    assert_int_equal(7, one->alloc);
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
    assert_int_equal(20, one->alloc);
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
    assert_int_equal(7, one->alloc);
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
    assert_int_equal(7, one->alloc);
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
    assert_int_equal(20, one->alloc);
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
    assert_int_equal(7, one->alloc);
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
    assert_int_equal(7, one->alloc);
    assert_string_equal(get_string(one), "Hello!");
    free_string(one);
}
// ================================================================================
// ================================================================================
// eof
