// ================================================================================
// ================================================================================
// - File:    test_string.c
// - Purpose: This file contains implementations for functions that test the 
//            dynamically allocated string with a cmocka test framework/
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 31, 2022
// - Version: 0.1
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <errno.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include "test_string.h"
#include "c_string.h"
// ================================================================================
// ================================================================================ 

/* Test cases for init_string */
void test_init_string_nominal(void **state) {
    string_t* str = init_string("hello world");
    assert_non_null(str);
    assert_string_equal(get_string(str), "hello world");
    assert_int_equal(string_size(str), 11);
    assert_int_equal(string_alloc(str), 12);  // length + null terminator
    assert_int_equal(get_string_error(str), NO_ERROR);
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_init_string_empty(void **state) {
    string_t* str = init_string("");
    assert_non_null(str);
    assert_string_equal(get_string(str), "");
    assert_int_equal(string_size(str), 0);
    assert_int_equal(string_alloc(str), 1);  // just null terminator
    assert_int_equal(get_string_error(str), NO_ERROR);
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_init_string_null(void **state) {
    string_t* str = init_string(NULL);
    assert_null(str);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

/* Test cases for free_string */
void test_free_string_nominal(void **state) {
    string_t* str = init_string("test string");
    assert_non_null(str);
    free_string(str);
    // Note: Can't test str after free as it's invalid memory
}
// --------------------------------------------------------------------------------

void test_free_string_null(void **state) {
    free_string(NULL);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

// Test string with special characters
void test_init_string_special_chars(void **state) {
    const char* test_str = "Hello\n\t!@#$%^&*()";
    string_t* str = init_string(test_str);
    assert_non_null(str);

    assert_string_equal(get_string(str), "Hello\n\t!@#$%^&*()");
    assert_int_equal(string_size(str), 17);
    assert_int_equal(string_alloc(str), 18);
    assert_int_equal(get_string_error(str), NO_ERROR);
    free_string(str);
}
// --------------------------------------------------------------------------------

// Test very long string
void test_init_string_long(void **state) {
    char long_str[1000];
    memset(long_str, 'a', 999);
    long_str[999] = '\0';
    
    string_t* str = init_string(long_str);
    assert_non_null(str);
    assert_string_equal(get_string(str), long_str);
    assert_int_equal(string_size(str), 999);
    assert_int_equal(string_alloc(str), 1000);
    assert_int_equal(get_string_error(str), NO_ERROR);
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_getters_null_string(void **state) {
    assert_null(get_string(NULL));
    assert_int_equal(string_size(NULL), SIZE_MAX);
    assert_int_equal(string_alloc(NULL), SIZE_MAX);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

#if defined(__GNUC__) || defined(__clang__)
    void test_string_cleanup_gbc(void **state) {
        string_t* str STRING_GBC = init_string("hello world");
        assert_non_null(str);
        assert_string_equal(get_string(str), "hello world");
        assert_int_equal(string_size(str), 11);
        assert_int_equal(string_alloc(str), 12);  // length + null terminator
        assert_int_equal(get_string_error(str), NO_ERROR);
    }
#endif
// --------------------------------------------------------------------------------

/* Test cases for string concatenation */
void test_concat_string_nominal(void **state) {
    string_t* str1 = init_string("Hello ");
    string_t* str2 = init_string("World!");
    
    assert_true(string_concat(str1, str2));
    assert_string_equal(get_string(str1), "Hello World!");
    assert_int_equal(string_size(str1), 12);
    assert_int_equal(string_alloc(str1), 13);
    assert_int_equal(get_string_error(str1), NO_ERROR);
    assert_int_equal(get_string_error(str2), NO_ERROR);
    
    free_string(str1);
    free_string(str2);
}
// --------------------------------------------------------------------------------

void test_concat_literal_nominal(void **state) {
    string_t* str = init_string("Hello ");
    
    assert_true(string_concat(str, "World!"));
    assert_string_equal(get_string(str), "Hello World!");
    assert_int_equal(string_size(str), 12);
    assert_int_equal(string_alloc(str), 13);
    
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_concat_empty_string(void **state) {
    string_t* str1 = init_string("Hello");
    string_t* str2 = init_string("");
    
    assert_true(string_concat(str1, str2));
    assert_string_equal(get_string(str1), "Hello");
    assert_int_equal(string_size(str1), 5);
    assert_int_equal(get_string_error(str1), NO_ERROR);
    assert_int_equal(get_string_error(str2), NO_ERROR);
    
    free_string(str1);
    free_string(str2);
}
// --------------------------------------------------------------------------------

void test_concat_empty_literal(void **state) {
    string_t* str = init_string("Hello");
    
    assert_true(string_concat(str, ""));
    assert_string_equal(get_string(str), "Hello");
    assert_int_equal(string_size(str), 5);
    
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_concat_null_inputs(void **state) {
    string_t* str = init_string("Hello");
    
    assert_false(string_concat(NULL, str));
    // assert_int_equal(errno, EINVAL);
    //  
    // assert_false(string_concat(str, NULL));
    // assert_int_equal(errno, EINVAL);
    
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_concat_reallocation(void **state) {
    string_t* str = init_string("Hello");  // 6 bytes allocated
    assert_true(string_concat(str, " World! This is a longer string to force reallocation"));
    assert_string_equal(get_string(str), "Hello World! This is a longer string to force reallocation");
    
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_concat_multiple_times(void **state) {
    string_t* str = init_string("Hello");
    
    assert_true(string_concat(str, " "));
    assert_true(string_concat(str, "World"));
    assert_true(string_concat(str, "!"));
    
    assert_string_equal(get_string(str), "Hello World!");
    assert_int_equal(string_size(str), 12);
    
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_concat_special_characters(void **state) {
    string_t* str = init_string("Line1");
    assert_true(string_concat(str, "\n\tLine2"));
    assert_string_equal(get_string(str), "Line1\n\tLine2");
    
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_concat_large_strings(void **state) {
    // Create two 1000-char strings
    char large_str[1001] = {0};
    memset(large_str, 'A', 1000);
    
    string_t* str1 = init_string(large_str);
    memset(large_str, 'B', 1000);
    
    assert_true(string_concat(str1, large_str));
    assert_int_equal(string_size(str1), 2000);
    
    // Verify first 1000 are 'A' and next 1000 are 'B'
    const char* result = get_string(str1);
    for(int i = 0; i < 1000; i++)
        assert_int_equal(result[i], 'A');
    for(int i = 1000; i < 2000; i++)
        assert_int_equal(result[i], 'B');
    
    free_string(str1);
}
// --------------------------------------------------------------------------------

/* Test cases for string comparison */
void test_compare_strings_equal(void **state) {
    string_t* str1 = init_string("hello");
    string_t* str2 = init_string("hello");
    
    assert_int_equal(compare_strings(str1, str2), 0);
    assert_int_equal(compare_strings(str1, "hello"), 0);
    
    free_string(str1);
    free_string(str2);
}
// -------------------------------------------------------------------------------- 

void test_compare_strings_less(void **state) {
    string_t* str1 = init_string("hello");
    string_t* str2 = init_string("world");
    
    assert_true(compare_strings(str1, str2) < 0);
    assert_true(compare_strings(str1, "world") < 0);
    
    free_string(str1);
    free_string(str2);
}
// --------------------------------------------------------------------------------

void test_compare_strings_greater(void **state) {
    string_t* str1 = init_string("world");
    string_t* str2 = init_string("hello");
    
    assert_true(compare_strings(str1, str2) > 0);
    assert_true(compare_strings(str1, "hello") > 0);
    
    free_string(str1);
    free_string(str2);
}
// --------------------------------------------------------------------------------

void test_compare_strings_different_lengths(void **state) {
    string_t* str1 = init_string("hello");
    string_t* str2 = init_string("hello world");
    
    assert_true(compare_strings(str1, str2) < 0);
    assert_true(compare_strings(str1, "hello world") < 0);
    
    free_string(str1);
    free_string(str2);
}
// --------------------------------------------------------------------------------

void test_compare_strings_empty(void **state) {
    string_t* str1 = init_string("");
    string_t* str2 = init_string("");
    
    assert_int_equal(compare_strings(str1, str2), 0);
    assert_int_equal(compare_strings(str1, ""), 0);
    
    free_string(str1);
    free_string(str2);
}
// -------------------------------------------------------------------------------- 

void test_compare_strings_null(void **state) {
    string_t* str = init_string("hello");
    
    assert_int_equal(compare_strings(NULL, str), INT_MIN);
    assert_int_equal(compare_strings(str, NULL), INT_MIN);
    assert_int_equal(errno, EINVAL);
    
    free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_compare_strings_case_sensitivity(void **state) {
    string_t* str1 = init_string("Hello");
    string_t* str2 = init_string("hello");
    
    assert_true(compare_strings(str1, str2) < 0);  // 'H' < 'h' in ASCII
    assert_true(compare_strings(str1, "hello") < 0);
    
    free_string(str1);
    free_string(str2);
}
// --------------------------------------------------------------------------------
/* Test cases for copy_string and reserve_string */
void test_copy_string_nominal(void **state) {
    string_t* str1 = init_string("hello world");
    string_t* str2 = copy_string(str1);
   
    assert_non_null(str2);
    assert_string_equal(get_string(str2), get_string(str1));
    assert_int_equal(string_size(str2), string_size(str1));
    assert_int_equal(string_alloc(str2), string_alloc(str1));
   
    free_string(str1);
    free_string(str2);
}
// --------------------------------------------------------------------------------

void test_copy_string_empty(void **state) {
    string_t* str1 = init_string("");
    string_t* str2 = copy_string(str1);
   
    assert_non_null(str2);
    assert_string_equal(get_string(str2), "");
    assert_int_equal(string_size(str2), 0);
    assert_int_equal(string_alloc(str2), 1);
   
    free_string(str1);
    free_string(str2);
}
// --------------------------------------------------------------------------------

void test_copy_string_null(void **state) {
    string_t* str = copy_string(NULL);
    assert_null(str);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_reserve_string_nominal(void **state) {
    string_t* str = init_string("test");
    assert_true(reserve_string(str, 20));
    
    assert_int_equal(string_alloc(str), 20);
    assert_string_equal(get_string(str), "test");  // Verify content unchanged
    assert_int_equal(string_size(str), 4);         // Verify length unchanged
    assert_int_equal(get_string_error(str), NO_ERROR);

   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_reserve_string_smaller_size(void **state) {
   string_t* str = init_string("test");
   size_t original_alloc = string_alloc(str);
   
   assert_false(reserve_string(str, 2));  // Should fail when trying to reserve less space
   assert_int_equal(errno, EINVAL);
   assert_int_equal(string_alloc(str), original_alloc);  // Allocation should be unchanged
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_reserve_string_null(void **state) {
    assert_false(reserve_string(NULL, 10));
    assert_int_equal(errno, EINVAL);

}
// -------------------------------------------------------------------------------- 

void test_trim_string_nominal(void **state) {
    string_t* str = init_string("test");
    // First reserve extra space
    assert_true(reserve_string(str, 20));
    assert_int_equal(string_alloc(str), 20);
    
    // Now trim it
    assert_true(trim_string(str));
    assert_int_equal(string_size(str), 4);
    assert_int_equal(string_alloc(str), 5);  // length + null terminator
    assert_string_equal(get_string(str), "test");
    assert_int_equal(get_string_error(str), NO_ERROR);
    
    free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_trim_already_minimal(void **state) {
    string_t* str = init_string("test");
    // Should already be minimal (len + 1)
    assert_true(trim_string(str));  // Should succeed but do nothing
    assert_int_equal(string_size(str), 4);
    assert_int_equal(string_alloc(str), 5);
    assert_string_equal(get_string(str), "test");
    assert_int_equal(get_string_error(str), NO_ERROR);
    
    free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_trim_empty_string(void **state) {
    string_t* str = init_string("");
    // Reserve some space first
    assert_true(reserve_string(str, 10));
    assert_int_equal(string_alloc(str), 10);
    
    // Now trim it
    assert_true(trim_string(str));
    assert_int_equal(string_size(str), 0);
    assert_int_equal(string_alloc(str), 1);  // Just null terminator
    assert_string_equal(get_string(str), "");
    assert_int_equal(get_string_error(str), NO_ERROR);
    
    free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_trim_null_string(void **state) {
    assert_false(trim_string(NULL));
    assert_int_equal(errno, EINVAL);

}
// -------------------------------------------------------------------------------- 

/* Test cases for first_char_occurance */
void test_first_char_occurrance_nominal(void **state) {
    string_t* str = init_string("hello world");
    
    // Find first 'l'
    char* result = first_char_occurrance(str, 'l');
    assert_non_null(result);
    assert_int_equal(*result, 'l');
    assert_int_equal(result - get_string(str), 2);  // 'l' is at index 2
    
    free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_first_char_occurrance_not_found(void **state) {
    string_t* str = init_string("hello world");
    
    char* result = first_char_occurrance(str, 'z');
    assert_null(result);
    
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_first_char_occurrance_empty_string(void **state) {
    string_t* str = init_string("");
    
    char* result = first_char_occurrance(str, 'a');
    assert_null(result);
    
    free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_first_char_occurrance_null(void **state) {
    char* result = first_char_occurrance(NULL, 'a');
    assert_null(result);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

/* Test cases for last_char_occurrance */
void test_last_char_occurrance_nominal(void **state) {
    string_t* str = init_string("hello world");
    
    // Find last 'l'
    char* result = last_char_occurrance(str, 'l');
    assert_non_null(result);
    assert_int_equal(*result, 'l');
    assert_int_equal(result - get_string(str), 9);  // Last 'l' is at index 9
    
    free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_last_char_occurrance_first_char(void **state) {
    string_t* str = init_string("hello world");
    
    // Find 'h' which is at the start
    char* result = last_char_occurrance(str, 'h');
    assert_non_null(result);
    assert_int_equal(*result, 'h');
    assert_int_equal(result - get_string(str), 0);
    
    free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_last_char_occurrance_not_found(void **state) {
    string_t* str = init_string("hello world");
    
    char* result = last_char_occurrance(str, 'z');
    assert_null(result);
    
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_last_char_occurrance_empty_string(void **state) {
    string_t* str = init_string("");
    
    char* result = last_char_occurrance(str, 'a');
    assert_null(result);
    
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_last_char_occurrance_null(void **state) {
    char* result = last_char_occurrance(NULL, 'a');
    assert_null(result);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_string_start_pointer_nominal(void **state) {
   string_t* str = init_string("hello world");
   char* start = first_char(str);
   
   assert_non_null(start);
   assert_ptr_equal(start, get_string(str));
   assert_int_equal(*start, 'h');  // First character should be 'h'
   
   free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_string_start_pointer_empty(void **state) {
   string_t* str = init_string("");
   char* start = first_char(str);
   
   assert_non_null(start);
   assert_ptr_equal(start, get_string(str));
   assert_int_equal(*start, '\0');  // Empty string starts with null terminator
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_string_start_pointer_null(void **state) {
   char* start = first_char(NULL);
   assert_null(start);
   assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_string_end_pointer_nominal(void **state) {
   string_t* str = init_string("hello world");
   char* end = last_char(str);
   
   assert_non_null(end);
   assert_int_equal(*end, 'd');  // Last character should be 'd'
   assert_ptr_equal(end, get_string(str) + string_size(str) - 1);
   
   free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_string_end_pointer_single_char(void **state) {
   string_t* str = init_string("x");
   char* end = last_char(str);
   
   assert_non_null(end);
   assert_int_equal(*end, 'x');
   assert_ptr_equal(end, get_string(str));
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_string_end_pointer_empty(void **state) {
   string_t* str = init_string("");
   char* end = last_char(str);
   
   // For empty string, end pointer should still be valid but point to position -1
   // from the null terminator
   assert_non_null(end);
   
   free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_string_end_pointer_null(void **state) {
   char* end = last_char(NULL);
   assert_null(end);
   assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_pointer_arithmetic(void **state) {
   string_t* str = init_string("hello");
   char* start = first_char(str);
   char* end = last_char(str);
   
   assert_int_equal(end - start, 4);  // "hello" is 5 chars, end points to 'o'
   
   free_string(str);
}
// -------------------------------------------------------------------------------- 

/* Tests using string literal (char*) substring */
void test_first_substr_literal_nominal(void **state) {
   string_t* str = init_string("hello world hello there");
   char* result = first_substr_occurrence(str, "hello");
   
   assert_non_null(result);
   assert_int_equal(result - get_string(str), 0);  // First occurrence at start
   
   free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_first_substr_literal_middle(void **state) {
   string_t* str = init_string("world hello there");
   char* result = first_substr_occurrence(str, "hello");
   
   assert_non_null(result);
   assert_int_equal(result - get_string(str), 6);  // "hello" starts at position 6
   
   free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_first_substr_literal_not_found(void **state) {
   string_t* str = init_string("hello world");
   char* result = first_substr_occurrence(str, "xyz");
   
   assert_null(result);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_first_substr_literal_empty_substring(void **state) {
   string_t* str = init_string("hello world");
   char* result = first_substr_occurrence(str, "");
   
   assert_null(result);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_first_substr_literal_too_long(void **state) {
   string_t* str = init_string("hi");
   char* result = first_substr_occurrence(str, "hello");
   
   assert_null(result);
   
   free_string(str);
}
// -------------------------------------------------------------------------------- 

/* Tests using string_t substring */
void test_first_substr_string_nominal(void **state) {
   string_t* str = init_string("hello world hello there");
   string_t* sub = init_string("hello");
   
   char* result = first_substr_occurrence(str, sub);
   assert_non_null(result);
   assert_int_equal(result - get_string(str), 0);
   
   free_string(str);
   free_string(sub);
}
// -------------------------------------------------------------------------------- 

void test_first_substr_string_middle(void **state) {
   string_t* str = init_string("world hello there");
   string_t* sub = init_string("hello");
   
   char* result = first_substr_occurrence(str, sub);
   assert_non_null(result);
   assert_int_equal(result - get_string(str), 6);
   
   free_string(str);
   free_string(sub);
}
// --------------------------------------------------------------------------------

void test_first_substr_string_not_found(void **state) {
   string_t* str = init_string("hello world");
   string_t* sub = init_string("xyz");
   
   char* result = first_substr_occurrence(str, sub);
   assert_null(result);
   
   free_string(str);
   free_string(sub);
}
// --------------------------------------------------------------------------------

void test_first_substr_string_empty(void **state) {
   string_t* str = init_string("hello world");
   string_t* sub = init_string("");
   
   char* result = first_substr_occurrence(str, sub);
   assert_null(result);
   
   free_string(str);
   free_string(sub);
}
// -------------------------------------------------------------------------------- 

void test_first_substr_string_too_long(void **state) {
   string_t* str = init_string("hi");
   string_t* sub = init_string("hello");
   
   char* result = first_substr_occurrence(str, sub);
   assert_null(result);
   
   free_string(str);
   free_string(sub);
}
// --------------------------------------------------------------------------------

/* Tests using string literal (char*) substring */
void test_last_substr_literal_nominal(void **state) {
   string_t* str = init_string("hello world hello there");
   char* result = last_substr_occurrence(str, "hello");
   
   assert_non_null(result);
   assert_int_equal(result - get_string(str), 12);  // Second "hello" starts at position 12
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_last_substr_literal_single_occurrence(void **state) {
   string_t* str = init_string("hello world");
   char* result = last_substr_occurrence(str, "world");
   
   assert_non_null(result);
   assert_int_equal(result - get_string(str), 6);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_last_substr_literal_not_found(void **state) {
   string_t* str = init_string("hello world");
   char* result = last_substr_occurrence(str, "xyz");
   
   assert_null(result);
   
   free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_last_substr_literal_empty_substring(void **state) {
   string_t* str = init_string("hello world");
   char* result = last_substr_occurrence(str, "");
   
   assert_non_null(result);
   assert_ptr_equal(result, get_string(str) + string_size(str));  // Empty string matches at end
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_last_substr_literal_too_long(void **state) {
   string_t* str = init_string("hi");
   char* result = last_substr_occurrence(str, "hello");
   
   assert_null(result);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_last_substr_string_nominal(void **state) {
   string_t* str = init_string("hello world hello there hello");
   string_t* sub = init_string("hello");
   
   char* result = last_substr_occurrence(str, sub);
   assert_non_null(result);
   assert_int_equal(result - get_string(str), 18);  // Last "hello" position
   
   free_string(str);
   free_string(sub);
}
// --------------------------------------------------------------------------------

void test_last_substr_string_single_occurrence(void **state) {
    string_t* str = init_string("world hello there");
    string_t* sub = init_string("hello");
   
    char* result = last_substr_occurrence(str, sub);
    assert_non_null(result);
    assert_int_equal(result - get_string(str), 6);
   
    free_string(str);
    free_string(sub);
}
// --------------------------------------------------------------------------------

void test_last_substr_string_not_found(void **state) {
    string_t* str = init_string("hello world");
    string_t* sub = init_string("xyz");
   
    char* result = last_substr_occurrence(str, sub);
    assert_null(result);
   
    free_string(str);
    free_string(sub);
}
// --------------------------------------------------------------------------------

void test_last_substr_string_empty(void **state) {
    string_t* str = init_string("hello world");
    string_t* sub = init_string("");
   
    char* result = last_substr_occurrence(str, sub);
    assert_non_null(result);
    assert_ptr_equal(result, get_string(str) + string_size(str));
   
    free_string(str);
    free_string(sub);
}
// --------------------------------------------------------------------------------

void test_last_substr_string_too_long(void **state) {
    string_t* str = init_string("hi");
    string_t* sub = init_string("hello");
   
    char* result = last_substr_occurrence(str, sub);
    assert_null(result);
   
    free_string(str);
    free_string(sub);
}
// --------------------------------------------------------------------------------

void test_is_string_ptr_within_bounds(void **state) {
    string_t* str = init_string("hello world");
   
    // Check middle of string
    char* mid = first_char(str) + 5;  // Points to space
    assert_true(is_string_ptr(str, mid, false));
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_is_string_ptr_at_boundaries(void **state) {
    string_t* str = init_string("hello");
   
    // Check first character
    char* start = first_char(str);
    assert_true(is_string_ptr(str, start, false));
   
    // Check last character
    char* last = first_char(str) + string_size(str) - 1;
    assert_true(is_string_ptr(str, last, false));
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_is_string_ptr_outside_bounds(void **state) {
    string_t* str = init_string("hello");
   
    // Check one past the end (at null terminator)
    char* past_end = first_char(str) + string_size(str);
    assert_false(is_string_ptr(str, past_end, false));
   
    // Check before start (undefined behavior in practice, but good for testing)
    char* before_start = first_char(str) - 1;
    assert_false(is_string_ptr(str, before_start, false));
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_is_string_ptr_empty_string(void **state) {
    string_t* str = init_string("");
    char* ptr = first_char(str);
   
    // Even with empty string, the pointer to the start should be valid
    assert_false(is_string_ptr(str, ptr, false));  // False because len is 0
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_is_string_ptr_null_inputs(void **state) {
    string_t* str = init_string("test");
   
    // Test NULL string_t
    assert_false(is_string_ptr(NULL, first_char(str), false));
    assert_int_equal(errno, EINVAL);

    // Test NULL pointer
    assert_false(is_string_ptr(str, NULL, false));
    assert_int_equal(errno, EINVAL);
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_drop_substring_literal_nominal(void **state) {
    string_t* str = init_string("hello world hello there hello");
    char* start = first_char(str);
    char* end = last_char(str);
   
    assert_true(drop_substr(str, "hello", start, end));
    assert_string_equal(get_string(str), "world there ");
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_drop_substring_literal_with_spaces(void **state) {
    string_t* str = init_string("test hello test hello test");
    char* start = first_char(str);
    char* end = last_char(str);
   
    assert_true(drop_substr(str, "hello", start, end));
    assert_string_equal(get_string(str), "test test test");
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_drop_substring_literal_partial_range(void **state) {
    string_t* str = init_string("hello world hello there hello");
    char* start = first_char(str) + 6;  // Start at "world"
    char* end = last_char(str);
   
    assert_true(drop_substr(str, "hello", start, end));
    assert_string_equal(get_string(str), "hello world there ");
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_drop_substring_string_nominal(void **state) {
    string_t* str = init_string("hello world hello there hello");
    string_t* sub = init_string("hello");
    char* start = first_char(str);
    char* end = last_char(str);
   
    assert_true(drop_substr(str, sub, start, end));
    assert_string_equal(get_string(str), "world there ");
   
    free_string(str);
    free_string(sub);
}
// --------------------------------------------------------------------------------

void test_drop_substring_string_with_spaces(void **state) {
    string_t* str = init_string("test hello test hello test");
    string_t* sub = init_string("hello");
    char* start = first_char(str);
    char* end = last_char(str);
   
    assert_true(drop_substr(str, sub, start, end));
    assert_string_equal(get_string(str), "test test test");
   
    free_string(str);
    free_string(sub);
}
// --------------------------------------------------------------------------------

void test_drop_substring_not_found(void **state) {
    string_t* str = init_string("hello world");
    string_t* sub = init_string("xyz");
    char* start = first_char(str);
    char* end = last_char(str);
   
    assert_true(drop_substr(str, sub, start, end));
    assert_string_equal(get_string(str), "hello world");  // Should remain unchanged
   
    free_string(str);
    free_string(sub);
}
// --------------------------------------------------------------------------------

void test_drop_substring_invalid_range(void **state) {
    string_t* str = init_string("hello world");
    char* start = first_char(str);
    char* end = last_char(str);

   // Test with end before start
    assert_false(drop_substr(str, "hello", end, start));
    assert_int_equal(errno, EINVAL);
    
   // Test with out of bounds pointers
   assert_false(drop_substr(str, "hello", start - 1, end));
   assert_int_equal(errno, ERANGE);
   assert_int_equal(get_string_error(str), OUT_OF_BOUNDS);
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_replace_substr_literal_nominal(void **state) {
    string_t* str = init_string("hello world hello there");
    char* start = first_char(str);
    char* end = last_char(str);
   
    assert_true(replace_substr(str, "hello", "Test1", start, end));
    assert_string_equal(get_string(str), "Test1 world Test1 there");
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_replace_substr_literal_shorter_replacement(void **state) {
    string_t* str = init_string("hello world hello there");
    char* start = first_char(str);
    char* end = last_char(str);
   
    assert_true(replace_substr(str, "hello", "hi", start, end));
    assert_string_equal(get_string(str), "hi world hi there");
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_replace_substr_literal_longer_replacement(void **state) {
    string_t* str = init_string("hi world hi there");
    char* start = first_char(str);
    char* end = last_char(str);
    assert_true(replace_substr(str, "hi", "hello", start, end));
    assert_string_equal(get_string(str), "hello world hello there");
   
    free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_replace_substr_literal_partial_range(void **state) {
    string_t* str = init_string("hello world hello there hello");
    char* start = first_char(str) + 6;  // Start after first "hello"
    char* end = last_char(str);
   
    assert_true(replace_substr(str, "hello", "hi", start, end));
    assert_string_equal(get_string(str), "hello world hi there hi");
   
    free_string(str);
}
// -------------------------------------------------------------------------------- 

void test_replace_substr_string_nominal(void **state) {
    string_t* str = init_string("hello world hello there");
    string_t* old_str = init_string("hello");
    string_t* new_str = init_string("Test1");
    char* start = first_char(str);
    char* end = last_char(str);
   
    assert_true(replace_substr(str, old_str, new_str, start, end));
    assert_string_equal(get_string(str), "Test1 world Test1 there");
   
    free_string(str);
    free_string(old_str);
    free_string(new_str);
}
// -------------------------------------------------------------------------------- 

void test_replace_substr_string_shorter_replacement(void **state) {
    string_t* str = init_string("hello world hello there");
    string_t* pattern = init_string("hello");
    string_t* replacement = init_string("hi");
    char* start = first_char(str);
    char* end = last_char(str);
   
    assert_true(replace_substr(str, pattern, replacement, start, end));
    assert_string_equal(get_string(str), "hi world hi there");
   
    free_string(str);
    free_string(pattern);
    free_string(replacement);
}
// -------------------------------------------------------------------------------- 

void test_replace_substr_string_longer_replacement(void **state) {
    string_t* str = init_string("hi world hi there");
    string_t* pattern = init_string("hi");
    string_t* replacement = init_string("hello");
    char* start = first_char(str);
    char* end = last_char(str);
   
    assert_true(replace_substr(str, pattern, replacement, start, end));
    assert_string_equal(get_string(str), "hello world hello there");
   
    free_string(str);
    free_string(pattern);
    free_string(replacement);
}
// -------------------------------------------------------------------------------- 

void test_replace_substr_string_partial_range(void **state) {
    string_t* str = init_string("hello world hello there hello");
    string_t* pattern = init_string("hello");
    string_t* replacement = init_string("hi");
    char* start = first_char(str) + 6;  // Start after first "hello"
    char* end = last_char(str);
   
    assert_true(replace_substr(str, pattern, replacement, start, end));
    assert_string_equal(get_string(str), "hello world hi there hi");
   
    free_string(str);
    free_string(pattern);
    free_string(replacement);
}
// -------------------------------------------------------------------------------- 

void test_replace_substr_invalid_range(void **state) {
    string_t* str = init_string("test string");
    char* start = first_char(str);
    char* end = last_char(str);
    bool val = replace_substr(str, "test", "new", end, start); 
    // Test with end before start
    assert_false(val);
 
   // Test with out of bounds pointers
    assert_false(replace_substr(str, "test", "new", start - 1, end));
    assert_int_equal(errno, ERANGE);
    assert_int_equal(get_string_error(str), OUT_OF_BOUNDS);
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_uppercase_char_nominal(void **state) {
    char val = 'a';
    to_upper_char(&val);
    assert_int_equal('A', val);
}
// --------------------------------------------------------------------------------

void test_lowercase_char_nominal(void **state) {
    char val = 'A';
    to_lower_char(&val);
    assert_int_equal('a', val);
}
// -------------------------------------------------------------------------------- 

void test_uppercase_char_null_value(void **state) {
    to_upper_char(NULL);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_lowercase_char_null_value(void **state) {
    to_lower_char(NULL);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_uppercase_string_nominal(void **state) {
    string_t* str = init_string("hello");
    to_uppercase(str);
    assert_string_equal("HELLO", get_string(str));
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_lowercase_string_nominal(void **state) {
    string_t* str = init_string("HELLO");
    to_lowercase(str);
    assert_string_equal("hello", get_string(str));
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_uppercase_string_null(void **state) {
    to_uppercase(NULL);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_lowercase_string_null(void **state) {
    to_lowercase(NULL);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_pop_token_nominal(void **state) {
   string_t* str = init_string("hello,world");
   string_t* result = pop_string_token(str, ',');
   
   assert_non_null(result);
   assert_string_equal(get_string(str), "hello");
   assert_string_equal(get_string(result), "world");
   assert_int_equal(string_size(str), 5);
   
   free_string(str);
   free_string(result);
}
// --------------------------------------------------------------------------------

void test_pop_token_multiple_tokens(void **state) {
   string_t* str = init_string("hello,world,there");
   string_t* result = pop_string_token(str, ',');
   
   assert_non_null(result);
   assert_string_equal(get_string(str), "hello,world");
   assert_string_equal(get_string(result), "there");
   
   // Pop another token
   string_t* result2 = pop_string_token(str, ',');
   assert_non_null(result2);
   assert_string_equal(get_string(str), "hello");
   assert_string_equal(get_string(result2), "world");
   
   free_string(str);
   free_string(result);
   free_string(result2);
}
// --------------------------------------------------------------------------------

void test_pop_token_at_end(void **state) {
   string_t* str = init_string("hello,");
   string_t* result = pop_string_token(str, ',');
   
   assert_non_null(result);
   assert_string_equal(get_string(str), "hello");
   assert_string_equal(get_string(result), "");
   
   free_string(str);
   free_string(result);
}
// --------------------------------------------------------------------------------

void test_pop_token_no_token(void **state) {
   string_t* str = init_string("hello world");
   string_t* result = pop_string_token(str, ',');
   
   assert_null(result);
   assert_string_equal(get_string(str), "hello world");
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_pop_token_empty_string(void **state) {
   string_t* str = init_string("");
   string_t* result = pop_string_token(str, ',');
   
   assert_null(result);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_pop_token_null_input(void **state) {
   string_t* result = pop_string_token(NULL, ',');
   
   assert_null(result);
   assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_pop_token_single_char(void **state) {
   string_t* str = init_string("a,b");
   string_t* result = pop_string_token(str, ',');
   
   assert_non_null(result);
   assert_string_equal(get_string(str), "a");
   assert_string_equal(get_string(result), "b");
   
   free_string(str);
   free_string(result);
}
// --------------------------------------------------------------------------------

void test_pop_token_consecutive_tokens(void **state) {
   string_t* str = init_string("hello,,world");
   string_t* result = pop_string_token(str, ',');
   
   assert_non_null(result);
   assert_string_equal(get_string(str), "hello,");
   assert_string_equal(get_string(result), "world");
   
   // Pop the empty segment
   string_t* result2 = pop_string_token(str, ',');
   assert_non_null(result2);
   assert_string_equal(get_string(str), "hello");
   assert_string_equal(get_string(result2), "");
   
   free_string(str);
   free_string(result);
   free_string(result2);
}
// -------------------------------------------------------------------------------- 

void test_token_count_single_delimiter(void **state) {
   string_t* str = init_string("hello world there");
   size_t count = token_count(str, " ");
   
   assert_int_equal(count, 3);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_token_count_multiple_delimiters(void **state) {
   string_t* str = init_string("hello,world;there.test");
   size_t count = token_count(str, ",.;");
   
   assert_int_equal(count, 4);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_token_count_consecutive_delimiters(void **state) {
   string_t* str = init_string("hello   world  there");
   size_t count = token_count(str, " ");
   assert_int_equal(count, 3);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_token_count_leading_trailing_delimiters(void **state) {
   string_t* str = init_string("  hello world there  ");
   size_t count = token_count(str, " ");
   
   assert_int_equal(count, 3);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_token_count_single_token(void **state) {
   string_t* str = init_string("hello");
   size_t count = token_count(str, " ");
   
   assert_int_equal(count, 1);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_token_count_empty_string(void **state) {
   string_t* str = init_string("");
   size_t count = token_count(str, " ");
   
   assert_int_equal(count, 0);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_token_count_null_inputs(void **state) {
   string_t* str = init_string("hello world");
   
   size_t count = token_count(NULL, " ");
   assert_int_equal(count, 0);
   assert_int_equal(errno, EINVAL);
   
   count = token_count(str, NULL);
   assert_int_equal(count, 0);
   assert_int_equal(errno, EINVAL);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_token_count_only_delimiters(void **state) {
   string_t* str = init_string("   ");
   size_t count = token_count(str, " ");
   
   assert_int_equal(count, 0);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_token_count_mixed_delimiters(void **state) {
   string_t* str = init_string("hello,world;;there,,,test");
   size_t count = token_count(str, ",;");
   
   assert_int_equal(count, 4);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_get_char_nominal(void **state) {
   string_t* str = init_string("hello");
   
   assert_int_equal(get_char(str, 0), 'h');
   assert_int_equal(get_char(str, 4), 'o');
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_get_char_out_of_bounds(void **state) {
   string_t* str = init_string("hello");
   
   char result = get_char(str, 5);  // Index equal to length
   assert_int_equal(result, 0);
   assert_int_equal(errno, ERANGE);
   
   result = get_char(str, 10);  // Index way beyond length
   assert_int_equal(result, 0);
   assert_int_equal(errno, ERANGE);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_get_char_null(void **state) {
   char result = get_char(NULL, 0);
   assert_int_equal(result, 0);
   assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_replace_char_nominal(void **state) {
   string_t* str = init_string("hello");
   
   replace_char(str, 0, 'H');
   assert_string_equal(get_string(str), "Hello");
   
   replace_char(str, 4, 'O');
   assert_string_equal(get_string(str), "HellO");
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_replace_char_out_of_bounds(void **state) {
   string_t* str = init_string("hello");
   
   // Should not modify string when index is out of bounds
   replace_char(str, 5, 'x');
   assert_int_equal(errno, ERANGE);
   assert_string_equal(get_string(str), "hello");
   
   replace_char(str, 10, 'x');
   assert_int_equal(errno, ERANGE);
   assert_string_equal(get_string(str), "hello");
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_replace_char_null(void **state) {
   replace_char(NULL, 0, 'x');
   assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_replace_char_special_chars(void **state) {
   string_t* str = init_string("test");
   
   replace_char(str, 1, '\n');
   assert_int_equal(get_char(str, 1), '\n');
   
   replace_char(str, 2, '\t');
   assert_int_equal(get_char(str, 2), '\t');
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_trim_leading_nominal(void **state) {
   string_t* str = init_string("   hello world");
   trim_leading_whitespace(str);
   
   assert_string_equal(get_string(str), "hello world");
   assert_int_equal(string_size(str), 11);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_trim_leading_multiple_types(void **state) {
   string_t* str = init_string("\t\n hello world");
   trim_leading_whitespace(str);
   
   assert_string_equal(get_string(str), "hello world");
   assert_int_equal(string_size(str), 11);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_trim_leading_no_whitespace(void **state) {
   string_t* str = init_string("hello world");
   trim_leading_whitespace(str);
   
   assert_string_equal(get_string(str), "hello world");
   assert_int_equal(string_size(str), 11);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_trim_trailing_nominal(void **state) {
   string_t* str = init_string("hello world   ");
   trim_trailing_whitespace(str);
   
   assert_string_equal(get_string(str), "hello world");
   assert_int_equal(string_size(str), 11);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_trim_trailing_multiple_types(void **state) {
   string_t* str = init_string("hello world\t\n ");
   trim_trailing_whitespace(str);
   
   assert_string_equal(get_string(str), "hello world");
   assert_int_equal(string_size(str), 11);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_trim_trailing_no_whitespace(void **state) {
   string_t* str = init_string("hello world");
   trim_trailing_whitespace(str);
   
   assert_string_equal(get_string(str), "hello world");
   assert_int_equal(string_size(str), 11);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_trim_all_nominal(void **state) {
    string_t* str = init_string("hello   world  there");
    trim_all_whitespace(str);
    assert_string_equal(get_string(str), "helloworldthere");
     assert_int_equal(string_size(str), 15);
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_trim_all_multiple_types(void **state) {
    string_t* str = init_string("hello\t\nworld \tthere");
    trim_all_whitespace(str);
    assert_string_equal(get_string(str), "helloworldthere");
    assert_int_equal(string_size(str), 15);
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_trim_all_no_whitespace(void **state) {
    string_t* str = init_string("helloworld");
    trim_all_whitespace(str);
     
    assert_string_equal(get_string(str), "helloworld");
    assert_int_equal(string_size(str), 10);
   
    free_string(str);
}
// --------------------------------------------------------------------------------

void test_trim_null_input(void **state) {
    // Test all three functions with NULL input
    trim_leading_whitespace(NULL);
    assert_int_equal(errno, EINVAL);
     
    trim_trailing_whitespace(NULL);
    assert_int_equal(errno, EINVAL);
   
    trim_all_whitespace(NULL);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_trim_all_whitespace_string(void **state) {
    string_t* str = init_string("   \t\n   ");
   
    trim_leading_whitespace(str);
    assert_string_equal(get_string(str), "");
    assert_int_equal(string_size(str), 0);
   
    // Reset string
    free_string(str);
    str = init_string("   \t\n   ");
   
    trim_trailing_whitespace(str);
    assert_string_equal(get_string(str), "");
    assert_int_equal(string_size(str), 0);
   
    // Reset string
    free_string(str);
    str = init_string("   \t\n   ");
   
    trim_all_whitespace(str);
    assert_string_equal(get_string(str), "");
    assert_int_equal(string_size(str), 0);
   
    free_string(str);
}
// ================================================================================ 
// ================================================================================ 

int setup_hello(void **state) {
    string_t* s = init_string("Hello, world!");
    assert_non_null(s);
    *state = s;
    return 0;
}

int teardown_string(void **state) {
    if (*state) {
        free_string((string_t*)(*state));
        *state = NULL;
    }
    return 0;
}

/* ---------- Helpers (internal) ---------- */

static char* collect_with_iter(const string_t* s) {
    size_t n = string_size((string_t*)s);
    char* out = (char*)malloc(n + 1);
    assert_non_null(out);

    cstr_iter it = cstr_iter_make(s);
    size_t i = 0;
    for (; cstr_iter_valid(&it); cstr_iter_next(&it)) {
        out[i++] = cstr_iter_get(&it);
    }
    out[i] = '\0';
    return out;
}

/* ---------- Tests ---------- */

void test_make_basic(void **state) {
    string_t* s = (string_t*)(*state);

    cstr_iter it = cstr_iter_make(s);
    const size_t n = string_size(s);

    if (n == 0) {
        assert_false(cstr_iter_valid(&it));
    } else {
        assert_true(cstr_iter_valid(&it));
        assert_int_equal(cstr_iter_pos(&it), 0u);

        const char* base = get_string(s);
        assert_non_null(base);

        /* current pointer should be the first byte */
        assert_ptr_equal(cstr_iter_ptr(&it), &base[0]);
        assert_int_equal((unsigned char)cstr_iter_get(&it),
                         (unsigned char)base[0]);
    }
}

void test_make_const_and_collect(void **state) {
    string_t* s = (string_t*)(*state);
    char* collected = collect_with_iter(s);
    assert_string_equal(collected, get_string(s));
    free(collected);
}

void test_forward_positions(void **state) {
    string_t* s = (string_t*)(*state);
    const char* base = get_string(s);
    assert_non_null(base);

    cstr_iter it = cstr_iter_make(s);
    size_t pos = 0, n = string_size(s);

    for (; cstr_iter_valid(&it); cstr_iter_next(&it), ++pos) {
        assert_int_equal(cstr_iter_pos(&it), pos);
        assert_ptr_equal(cstr_iter_ptr(&it), &base[pos]);
        assert_int_equal((unsigned char)cstr_iter_get(&it),
                         (unsigned char)base[pos]);
    }
    assert_int_equal(pos, n);
}

void test_reverse_iteration(void **state) {
    string_t* s = (string_t*)(*state);
    const char* base = get_string(s);
    assert_non_null(base);

    const size_t n = string_size(s);
    str_iter it = str_iter_make(s);

    /* End is not valid; step once back to last if non-empty */
    assert_false(str_iter_seek_end(&it));
    if (n == 0) {
        assert_false(str_iter_prev(&it));
        return;
    }
    assert_true(str_iter_prev(&it));

    size_t pos = n - 1;
    for (;;) {
        assert_int_equal(str_iter_pos(&it), pos);
        assert_int_equal((unsigned char)str_iter_get(&it),
                         (unsigned char)base[pos]);
        if (pos == 0) break;
        assert_true(str_iter_prev(&it));
        --pos;
    }
    assert_int_equal(pos, 0u);
}

void test_advance_and_clamp(void **state) {
    string_t* s = (string_t*)(*state);
    const size_t n = string_size(s);

    str_iter it = str_iter_make(s);

    /* Overshoot forward → clamp to end (becomes invalid) */
    assert_false(str_iter_advance(&it, +1000));

    if (n > 0) {
        /* One step back from end → last valid element */
        assert_true(str_iter_prev(&it));
        assert_int_equal(str_iter_pos(&it), n - 1);

        /* Overshoot backward → clamp to begin (valid) */
        assert_true(str_iter_advance(&it, -(ptrdiff_t)1000));
        assert_true(str_iter_valid(&it));
        assert_int_equal(str_iter_pos(&it), 0u);
    }
}

void test_empty_string(void **state) {
    (void)state;
    string_t* s = init_string("");
    assert_non_null(s);

    cstr_iter it = cstr_iter_make(s);
    assert_false(cstr_iter_valid(&it));
    assert_int_equal(cstr_iter_get(&it), 0);
    assert_false(cstr_iter_seek_begin(&it));
    assert_false(cstr_iter_seek_end(&it));

    free_string(s);
}

void test_invalid_inputs_make(void **state) {
    (void)state;
    errno = 0;
    cstr_iter it = cstr_iter_make(NULL);
    assert_false(cstr_iter_valid(&it));
    assert_int_equal(errno, EINVAL);
}

void test_invalidation_after_reserve(void **state) {
    string_t* s = (string_t*)(*state);

    /* build iterator, record a harmless property */
    cstr_iter it = cstr_iter_make(s);
    const size_t n_before = string_size(s);
    (void)it;

    /* Force growth → may realloc and invalidate pointers/iters */
    size_t old_alloc = string_alloc(s);
    assert_true(reserve_string(s, old_alloc + 256));

    /* Rebuild iterator after mutation */
    cstr_iter it2 = cstr_iter_make(s);
    if (n_before > 0) {
        assert_true(cstr_iter_valid(&it2));
        assert_int_equal(cstr_iter_pos(&it2), 0u);
    } else {
        assert_false(cstr_iter_valid(&it2));
    }
}
// ================================================================================ 
// ================================================================================ 
// TEST STRING VECTOR

void test_tokenize_basic(void **state) {
   string_t* str = init_string("hello world test");
   string_v* tokens = tokenize_string(str, " ");
   
   assert_non_null(tokens);
   assert_int_equal(str_vector_size(tokens), 3);
   assert_string_equal(get_string(str_vector_index(tokens, 0)), "hello");
   assert_string_equal(get_string(str_vector_index(tokens, 1)), "world");
   assert_string_equal(get_string(str_vector_index(tokens, 2)), "test");
   
   free_str_vector(tokens);
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_tokenize_multiple_delimiters(void **state) {
   string_t* str = init_string("hello,world;test.done");
   string_v* tokens = tokenize_string(str, ",.;");
   
   assert_non_null(tokens);
   assert_int_equal(str_vector_size(tokens), 4);
   assert_string_equal(get_string(str_vector_index(tokens, 0)), "hello");
   assert_string_equal(get_string(str_vector_index(tokens, 1)), "world");
   assert_string_equal(get_string(str_vector_index(tokens, 2)), "test");
   assert_string_equal(get_string(str_vector_index(tokens, 3)), "done");
   
   free_str_vector(tokens);
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_tokenize_consecutive_delimiters(void **state) {
   string_t* str = init_string("hello   world,,;test");
   string_v* tokens = tokenize_string(str, " ,;");
   
   assert_non_null(tokens);
   assert_int_equal(str_vector_size(tokens), 3);
   assert_string_equal(get_string(str_vector_index(tokens, 0)), "hello");
   assert_string_equal(get_string(str_vector_index(tokens, 1)), "world");
   assert_string_equal(get_string(str_vector_index(tokens, 2)), "test");
   
   free_str_vector(tokens);
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_tokenize_empty_string(void **state) {
   string_t* str = init_string("");
   string_v* tokens = tokenize_string(str, " ");
   
   assert_non_null(tokens);
   assert_int_equal(str_vector_size(tokens), 0);
   
   free_str_vector(tokens);
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_tokenize_no_delimiters_found(void **state) {
   string_t* str = init_string("helloworld");
   string_v* tokens = tokenize_string(str, " ");
   
   assert_non_null(tokens);
   assert_int_equal(str_vector_size(tokens), 1);
   assert_string_equal(get_string(str_vector_index(tokens, 0)), "helloworld");
   
   free_str_vector(tokens);
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_tokenize_only_delimiters(void **state) {
   string_t* str = init_string("   ,,,   ");
   string_v* tokens = tokenize_string(str, " ,");
   
   assert_non_null(tokens);
   assert_int_equal(str_vector_size(tokens), 0);
   
   free_str_vector(tokens);
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_tokenize_null_inputs(void **state) {
   string_v* tokens = tokenize_string(NULL, " ");
   assert_null(tokens);
   assert_int_equal(errno, EINVAL);
   
   string_t* str = init_string("test");
   tokens = tokenize_string(str, NULL);
   assert_null(tokens);
   assert_int_equal(errno, EINVAL);
   
   free_string(str);
}
// --------------------------------------------------------------------------------

void test_tokenize_empty_delimiter(void **state) {
   string_t* str = init_string("test");
   string_v* tokens = tokenize_string(str, "");
   
   assert_non_null(tokens);
   assert_int_equal(str_vector_size(tokens), 1);
   assert_string_equal(get_string(str_vector_index(tokens, 0)), "test");
   
   free_str_vector(tokens);
   free_string(str);
}
// ================================================================================ 
// ================================================================================ 

void test_str_vector_init(void **state) {
    string_v* vec = init_str_vector(5);
   
    assert_non_null(vec);
    assert_int_equal(str_vector_size(vec), 0);
    assert_int_equal(str_vector_alloc(vec), 5);
    assert_int_equal(get_str_vector_error(vec), NO_ERROR); 
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_str_vector_push_back(void **state) {
   string_v* vec = init_str_vector(2);
   
   // Add first string
   assert_true(push_back_str_vector(vec, "hello"));
   assert_int_equal(str_vector_size(vec), 1);
   assert_string_equal(get_string(str_vector_index(vec, 0)), "hello");
   assert_int_equal(string_size(str_vector_index(vec, 0)), 5);
   
   // Add second string
   assert_true(push_back_str_vector(vec, "world"));
   assert_int_equal(str_vector_size(vec), 2);
   assert_string_equal(get_string(str_vector_index(vec, 1)), "world");
   assert_int_equal(string_size(str_vector_index(vec, 1)), 5);
   
   free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_str_vector_auto_resize(void **state) {
   string_v* vec = init_str_vector(1);
//   size_t initial_alloc = str_vector_size(vec);
   
   // Fill initial allocation
   assert_true(push_back_str_vector(vec, "first"));
   assert_int_equal(str_vector_size(vec), 1);
   assert_int_equal(str_vector_alloc(vec), 1);
   
   // This should trigger resize
   assert_true(push_back_str_vector(vec, "second"));
   assert_int_equal(str_vector_size(vec), 2);
   assert_int_equal(str_vector_alloc(vec), 2);
    
   // Verify both strings are intact
   assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
   assert_string_equal(get_string(str_vector_index(vec, 1)), "second");
   
   free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_str_vector_null_inputs(void **state) {
   // Test init with zero size
   string_v* vec = init_str_vector(0);
   assert_non_null(vec);
   free_str_vector(vec);
   
   // Test push_back with NULL vector
   assert_false(push_back_str_vector(NULL, "test"));
   assert_int_equal(errno, EINVAL);
   
   // Test push_back with NULL string
   vec = init_str_vector(1);
   assert_false(push_back_str_vector(vec, NULL));
   assert_int_equal(errno, EINVAL);
   free_str_vector(vec);
   
   // Test free with NULL
   free_str_vector(NULL);
   assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_str_vector_empty_string(void **state) {
   string_v* vec = init_str_vector(1);
   
   assert_true(push_back_str_vector(vec, ""));
   assert_int_equal(str_vector_size(vec), 1);
   assert_string_equal(get_string(str_vector_index(vec, 0)), "");
   assert_int_equal(string_size(str_vector_index(vec, 0)), 0);
   
   free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_str_vector_multiple_pushes(void **state) {
   string_v* vec = init_str_vector(2);
   const char* strings[] = {"one", "two", "three", "four", "five"};
   
   for (size_t i = 0; i < 5; i++) {
       assert_true(push_back_str_vector(vec, strings[i]));
       assert_int_equal(str_vector_size(vec), i + 1);
       assert_string_equal(get_string(str_vector_index(vec, i)), strings[i]);
   }
   
   free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_str_vector_gbc(void **state) {
   string_v* vec STRVEC_GBC = init_str_vector(2);
   const char* strings[] = {"one", "two", "three", "four", "five"};
   
   for (size_t i = 0; i < 5; i++) {
       assert_true(push_back_str_vector(vec, strings[i]));
       assert_int_equal(str_vector_size(vec), i + 1);
       assert_string_equal(get_string(str_vector_index(vec, i)), strings[i]);
   }
}
// --------------------------------------------------------------------------------

void test_push_front_empty_vector(void **state) {
    string_v* vec = init_str_vector(2);
   
    assert_true(push_front_str_vector(vec, "first"));
    assert_int_equal(str_vector_size(vec), 1);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
   
    free_str_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_existing_elements(void **state) {
    string_v* vec = init_str_vector(2);
   
    // Add from back first
    assert_true(push_back_str_vector(vec, "back"));
    assert_true(push_front_str_vector(vec, "front"));
   
    assert_int_equal(str_vector_size(vec), 2);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "front");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "back");
   
    free_str_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_reallocation(void **state) {
    string_v* vec = init_str_vector(1);
   
    // Fill initial allocation
    assert_true(push_front_str_vector(vec, "first"));
    assert_int_equal(str_vector_size(vec), 1);
    assert_int_equal(str_vector_alloc(vec), 1);
   
    // This should trigger resize
    assert_true(push_front_str_vector(vec, "second"));
    assert_int_equal(str_vector_size(vec), 2);
    assert_true(str_vector_alloc(vec) > 1);
   
    // Verify order is maintained
    assert_string_equal(get_string(str_vector_index(vec, 0)), "second");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "first");
   
    free_str_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_multiple(void **state) {
    string_v* vec = init_str_vector(2);
    const char* strings[] = {"three", "two", "one"};
   
    for (size_t i = 0; i < 3; i++) {
        assert_true(push_front_str_vector(vec, strings[i]));
        assert_int_equal(str_vector_size(vec), i + 1);
        assert_string_equal(get_string(str_vector_index(vec, 0)), strings[i]);
    }
   
    // Verify final order
    assert_string_equal(get_string(str_vector_index(vec, 0)), "one");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "two");
    assert_string_equal(get_string(str_vector_index(vec, 2)), "three");
   
    free_str_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_null_inputs(void **state) {
    string_v* vec = init_str_vector(1);
   
    assert_false(push_front_str_vector(NULL, "test"));
    assert_int_equal(errno, EINVAL);
   
    assert_false(push_front_str_vector(vec, NULL));
    assert_int_equal(errno, EINVAL);
   
    free_str_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_push_front_empty_string(void **state) {
    string_v* vec = init_str_vector(1);
   
    assert_true(push_front_str_vector(vec, ""));
    assert_int_equal(str_vector_size(vec), 1);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "");
    assert_int_equal(string_size(str_vector_index(vec, 0)), 0);
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_str_vector_middle(void **state) {
    string_v* vec = init_str_vector(3);
   
    // Create initial vector: "first", "third"
    push_back_str_vector(vec, "first");
    push_back_str_vector(vec, "third");
   
    // Insert in middle
    assert_true(insert_str_vector(vec, "second", 1));
    assert_int_equal(str_vector_size(vec), 3);
   
    // Verify order
    assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "second");
    assert_string_equal(get_string(str_vector_index(vec, 2)), "third");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_str_vector_start(void **state) {
    string_v* vec = init_str_vector(2);
   
    push_back_str_vector(vec, "old_first");
   
    // Insert at start
    assert_true(insert_str_vector(vec, "new_first", 0));
    assert_int_equal(str_vector_size(vec), 2);
   
    assert_string_equal(get_string(str_vector_index(vec, 0)), "new_first");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "old_first");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_str_vector_end(void **state) {
    string_v* vec = init_str_vector(2);
   
    push_back_str_vector(vec, "first");
   
    // Insert at end (equivalent to push_back)
    assert_true(insert_str_vector(vec, "last", 1));
    assert_int_equal(str_vector_size(vec), 2);
    
    assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "last");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_str_vector_reallocation(void **state) {
    string_v* vec = init_str_vector(1);
   
    // Fill initial allocation
    push_back_str_vector(vec, "first");
    size_t initial_alloc = str_vector_alloc(vec);
   
    // This should trigger resize
    assert_true(insert_str_vector(vec, "middle", 1));
    assert_true(str_vector_alloc(vec) > initial_alloc);
   
    assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "middle");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_str_vector_invalid_index(void **state) {
    string_v* vec = init_str_vector(2);
    push_back_str_vector(vec, "first");
   
    // Try to insert beyond end
    assert_false(insert_str_vector(vec, "invalid", 2));
    assert_int_equal(errno, ERANGE);
   
    // Verify original string unchanged
    assert_int_equal(str_vector_size(vec), 1);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_str_vector_null_inputs(void **state) {
    string_v* vec = init_str_vector(2);
   
    assert_false(insert_str_vector(NULL, "test", 0));
    assert_int_equal(errno, EINVAL);
   
    assert_false(insert_str_vector(vec, NULL, 0));
    assert_int_equal(errno, EINVAL);
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_insert_str_vector_empty_string(void **state) {
    string_v* vec = init_str_vector(2);
    push_back_str_vector(vec, "test");
   
    assert_true(insert_str_vector(vec, "", 1));
    assert_string_equal(get_string(str_vector_index(vec, 1)), "");
    assert_int_equal(string_size(str_vector_index(vec, 1)), 0);
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_back_nominal(void **state) {
    string_v* vec = init_str_vector(2);
    push_back_str_vector(vec, "first");
    push_back_str_vector(vec, "second");
   
    string_t* popped = pop_back_str_vector(vec);
    assert_non_null(popped);
    assert_string_equal(get_string(popped), "second");
    assert_int_equal(str_vector_size(vec), 1);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
   
    free_string(popped);
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_front_nominal(void **state) {
    string_v* vec = init_str_vector(2);
    push_back_str_vector(vec, "first");
    push_back_str_vector(vec, "second");
   
    string_t* popped = pop_front_str_vector(vec);
    assert_non_null(popped);
    assert_string_equal(get_string(popped), "first");
    assert_int_equal(str_vector_size(vec), 1);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "second");
   
    free_string(popped);
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_empty_vector(void **state) {
    string_v* vec = init_str_vector(1);
   
    string_t* back = pop_back_str_vector(vec);
    assert_null(back);
    assert_int_equal(get_str_vector_error(vec), UNINITIALIZED);
    assert_int_equal(errno, ENODATA);

    string_t* front = pop_front_str_vector(vec);
    assert_null(front);
    assert_int_equal(get_str_vector_error(vec), UNINITIALIZED);
    assert_int_equal(errno, ENODATA);
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_multiple_back(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "one");
    push_back_str_vector(vec, "two");
    push_back_str_vector(vec, "three");
   
    // Pop all elements from back
    string_t* third = pop_back_str_vector(vec);
    assert_string_equal(get_string(third), "three");
    free_string(third);
   
    string_t* second = pop_back_str_vector(vec);
    assert_string_equal(get_string(second), "two");
    free_string(second);
   
    string_t* first = pop_back_str_vector(vec);
    assert_string_equal(get_string(first), "one");
    free_string(first);
   
    assert_int_equal(str_vector_size(vec), 0);
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_multiple_front(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "one");
    push_back_str_vector(vec, "two");
    push_back_str_vector(vec, "three");
   
    // Pop all elements from front
    string_t* first = pop_front_str_vector(vec);
    assert_string_equal(get_string(first), "one");
    assert_string_equal(get_string(str_vector_index(vec, 0)), "two");
    free_string(first);
   
    string_t* second = pop_front_str_vector(vec);
    assert_string_equal(get_string(second), "two");
    assert_string_equal(get_string(str_vector_index(vec, 0)), "three");
    free_string(second);
   
    string_t* third = pop_front_str_vector(vec);
    assert_string_equal(get_string(third), "three");
    free_string(third);
   
    assert_int_equal(str_vector_size(vec), 0);
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_null_vector(void **state) {
    assert_null(pop_back_str_vector(NULL));
    assert_int_equal(errno, EINVAL);
   
    assert_null(pop_front_str_vector(NULL));
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_pop_alternating(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "one");
    push_back_str_vector(vec, "two");
    push_back_str_vector(vec, "three");
   
    string_t* back = pop_back_str_vector(vec);
    assert_string_equal(get_string(back), "three");
    free_string(back);
   
    string_t* front = pop_front_str_vector(vec);
    assert_string_equal(get_string(front), "one");
    free_string(front);
   
    assert_int_equal(str_vector_size(vec), 1);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "two");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

/* Tests for pop_any_str_vector */
void test_pop_any_nominal(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "first");
    push_back_str_vector(vec, "second");
    push_back_str_vector(vec, "third");
   
    // Pop middle element
    string_t* popped = pop_any_str_vector(vec, 1);
    assert_non_null(popped);
    assert_string_equal(get_string(popped), "second");
   
    // Verify remaining elements and order
    assert_int_equal(str_vector_size(vec), 2);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "third");
   
    free_string(popped);
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_any_first(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "first");
    push_back_str_vector(vec, "second");
    push_back_str_vector(vec, "third");
   
    // Pop first element
    string_t* popped = pop_any_str_vector(vec, 0);
    assert_non_null(popped);
    assert_string_equal(get_string(popped), "first");
   
    // Verify remaining elements shifted correctly
    assert_int_equal(str_vector_size(vec), 2);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "second");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "third");
   
    free_string(popped);
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_any_last(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "first");
    push_back_str_vector(vec, "second");
    push_back_str_vector(vec, "third");
   
    // Pop last element
    string_t* popped = pop_any_str_vector(vec, 2);
    assert_non_null(popped);
    assert_string_equal(get_string(popped), "third");
   
    // Verify remaining elements unchanged
    assert_int_equal(str_vector_size(vec), 2);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "second");
   
    free_string(popped);
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_pop_any_invalid_index(void **state) {
    string_v* vec = init_str_vector(2);
    push_back_str_vector(vec, "first");
    push_back_str_vector(vec, "second");
   
    // Try to pop at invalid index
    string_t* popped = pop_any_str_vector(vec, 2);  // Index == len
    assert_null(popped);
    assert_int_equal(errno, ERANGE);
   
    // Verify vector unchanged
    assert_int_equal(str_vector_size(vec), 2);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "second");
   
    free_str_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_empty_vector(void **state) {
    string_v* vec = init_str_vector(1);
   
    string_t* popped = pop_any_str_vector(vec, 0);
    assert_null(popped);
    assert_int_equal(errno, ENODATA);
   
    free_str_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_null_vector(void **state) {
    assert_null(pop_any_str_vector(NULL, 0));
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_pop_any_multiple(void **state) {
    string_v* vec = init_str_vector(4);
    assert_int_equal(4, s_alloc(vec));
    push_back_str_vector(vec, "one");
    push_back_str_vector(vec, "two");
    push_back_str_vector(vec, "three");
    push_back_str_vector(vec, "four");
   
    assert_int_equal(4, s_size(vec));
    // Pop elements in various positions
    string_t* popped1 = pop_any_str_vector(vec, 1);  // Pop "two"
    assert_int_equal(3, s_size(popped1));
    assert_int_equal(4, s_alloc(popped1));
    assert_string_equal(get_string(popped1), "two");
    free_string(popped1);
   
    string_t* popped2 = pop_any_str_vector(vec, 2);  // Pop "four"
    assert_string_equal(get_string(popped2), "four");
    free_string(popped2);
   
    // Verify remaining elements
    assert_int_equal(str_vector_size(vec), 2);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "one");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "three");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_sort_empty_vector(void **state) {
    string_v* vec = init_str_vector(1);
   
    sort_str_vector(vec, FORWARD);  // Should handle empty vector gracefully
    assert_int_equal(str_vector_size(vec), 0);
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_sort_single_element(void **state) {
    string_v* vec = init_str_vector(1);
    push_back_str_vector(vec, "test");
   
    sort_str_vector(vec, FORWARD);  // Should handle single element gracefully
    assert_int_equal(str_vector_size(vec), 1);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "test");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_sort_forward_already_sorted(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "apple");
    push_back_str_vector(vec, "banana");
    push_back_str_vector(vec, "cherry");
   
    sort_str_vector(vec, FORWARD);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "apple");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "banana");
    assert_string_equal(get_string(str_vector_index(vec, 2)), "cherry");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_sort_forward_reverse_sorted(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "cherry");
    push_back_str_vector(vec, "banana");
    push_back_str_vector(vec, "apple");
   
    sort_str_vector(vec, FORWARD);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "apple");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "banana");
    assert_string_equal(get_string(str_vector_index(vec, 2)), "cherry");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_sort_reverse_already_sorted(void **state) {
   string_v* vec = init_str_vector(3);
   push_back_str_vector(vec, "cherry");
   push_back_str_vector(vec, "banana");
   push_back_str_vector(vec, "apple");
   
   sort_str_vector(vec, REVERSE);
   assert_string_equal(get_string(str_vector_index(vec, 0)), "cherry");
   assert_string_equal(get_string(str_vector_index(vec, 1)), "banana");
   assert_string_equal(get_string(str_vector_index(vec, 2)), "apple");
   
   free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_sort_large_vector(void **state) {
    string_v* vec = init_str_vector(100);
    // Add strings in random order
    push_back_str_vector(vec, "zebra");
    push_back_str_vector(vec, "apple");
    push_back_str_vector(vec, "monkey");
    push_back_str_vector(vec, "bear");
    push_back_str_vector(vec, "Mountain Lion");
    push_back_str_vector(vec, "elk");
    // ... add more strings ...
   
    sort_str_vector(vec, FORWARD);
    // Verify order
    assert_true(compare_strings_string(str_vector_index(vec, 0), 
                                       (string_t*)str_vector_index(vec, 1)) <= 0);
    // ... verify more elements ...
   
    free_str_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_duplicate_elements(void **state) {
    string_v* vec = init_str_vector(4);
    push_back_str_vector(vec, "apple");
    push_back_str_vector(vec, "banana");
    push_back_str_vector(vec, "apple");
    push_back_str_vector(vec, "cherry");
   
    sort_str_vector(vec, FORWARD);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "apple");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "apple");
    assert_string_equal(get_string(str_vector_index(vec, 2)), "banana");
    assert_string_equal(get_string(str_vector_index(vec, 3)), "cherry");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_sort_with_empty_strings(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "");
    push_back_str_vector(vec, "apple");
    push_back_str_vector(vec, "");
   
    sort_str_vector(vec, FORWARD);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "");
    assert_string_equal(get_string(str_vector_index(vec, 2)), "apple");
   
    free_str_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_sort_null_vector(void **state) {
    sort_str_vector(NULL, FORWARD);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_delete_back_nominal(void **state) {
    string_v* vec = init_str_vector(2);
    push_back_str_vector(vec, "first");
    push_back_str_vector(vec, "second");
   
    delete_back_str_vector(vec);
    assert_int_equal(str_vector_size(vec), 1);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_delete_front_nominal(void **state) {
    string_v* vec = init_str_vector(2);
    push_back_str_vector(vec, "first");
    push_back_str_vector(vec, "second");
   
    delete_front_str_vector(vec);
    assert_int_equal(str_vector_size(vec), 1);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "second");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_delete_empty_vector(void **state) {
    string_v* vec = init_str_vector(1);
   
    delete_back_str_vector(vec);
    assert_int_equal(errno, ENODATA);
    assert_int_equal(get_str_vector_error(vec), UNINITIALIZED);
   
    delete_front_str_vector(vec);
    assert_int_equal(errno, ENODATA);
    assert_int_equal(get_str_vector_error(vec), UNINITIALIZED);
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_delete_multiple_back(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "one");
    push_back_str_vector(vec, "two");
    push_back_str_vector(vec, "three");
   
    // Pop all elements from back
    delete_back_str_vector(vec);
    delete_back_str_vector(vec);
    delete_back_str_vector(vec);
   
    assert_int_equal(str_vector_size(vec), 0);
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_delete_multiple_front(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "one");
    push_back_str_vector(vec, "two");
    push_back_str_vector(vec, "three");
   
    // Pop all elements from front
    delete_front_str_vector(vec);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "two");
   
    delete_front_str_vector(vec);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "three");
   
    delete_front_str_vector(vec);
   
    assert_int_equal(str_vector_size(vec), 0);
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_delete_null_vector(void **state) {
    assert_null(delete_back_str_vector(NULL));
    assert_int_equal(errno, EINVAL);
   
    assert_null(delete_front_str_vector(NULL));
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

void test_delete_alternating(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "one");
    push_back_str_vector(vec, "two");
    push_back_str_vector(vec, "three");
   
    delete_back_str_vector(vec);
    delete_front_str_vector(vec);
   
    assert_int_equal(str_vector_size(vec), 1);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "two");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

/* Tests for delete_any_str_vector */
void test_delete_any_nominal(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "first");
    push_back_str_vector(vec, "second");
    push_back_str_vector(vec, "third");
   
    // Pop middle element
    delete_any_str_vector(vec, 1);
   
    // Verify remaining elements and order
    assert_int_equal(str_vector_size(vec), 2);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "third");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_delete_any_first(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "first");
    push_back_str_vector(vec, "second");
    push_back_str_vector(vec, "third");
   
    // Pop first element
    delete_any_str_vector(vec, 0);
   
    // Verify remaining elements shifted correctly
    assert_int_equal(str_vector_size(vec), 2);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "second");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "third");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_delete_any_last(void **state) {
    string_v* vec = init_str_vector(3);
    push_back_str_vector(vec, "first");
    push_back_str_vector(vec, "second");
    push_back_str_vector(vec, "third");
   
    // Pop last element
    delete_any_str_vector(vec, 2);
   
    // Verify remaining elements unchanged
    assert_int_equal(str_vector_size(vec), 2);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "second");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_delete_any_invalid_index(void **state) {
    string_v* vec = init_str_vector(2);
    push_back_str_vector(vec, "first");
    push_back_str_vector(vec, "second");
   
    // Try to delete at invalid index
    delete_any_str_vector(vec, 2);  // Index == len
    assert_int_equal(errno, ERANGE);
   
    // Verify vector unchanged
    assert_int_equal(str_vector_size(vec), 2);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "first");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "second");
   
    free_str_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_delete_any_empty_vector(void **state) {
    string_v* vec = init_str_vector(1);
   
    delete_any_str_vector(vec, 0);
    assert_int_equal(errno, ENODATA);
    assert_int_equal(get_str_vector_error(vec), UNINITIALIZED);
   
    free_str_vector(vec);
}
// -------------------------------------------------------------------------------- 

void test_delete_any_null_vector(void **state) {
    assert_null(delete_any_str_vector(NULL, 0));
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

void test_delete_any_multiple(void **state) {
    string_v* vec = init_str_vector(4);
    assert_int_equal(4, s_alloc(vec));
    push_back_str_vector(vec, "one");
    push_back_str_vector(vec, "two");
    push_back_str_vector(vec, "three");
    push_back_str_vector(vec, "four");
   
    assert_int_equal(4, s_size(vec));
    // Pop elements in various positions
    delete_any_str_vector(vec, 1);  // Pop "two"
   
    delete_any_str_vector(vec, 2);  // Pop "four"
   
    // Verify remaining elements
    assert_int_equal(str_vector_size(vec), 2);
    assert_string_equal(get_string(str_vector_index(vec, 0)), "one");
    assert_string_equal(get_string(str_vector_index(vec, 1)), "three");
   
    free_str_vector(vec);
}
// --------------------------------------------------------------------------------

void test_reverse_str_vector(void **state) {
    string_v* vec = init_str_vector(4);
    assert_int_equal(4, s_alloc(vec));
    push_back_str_vector(vec, "one");
    push_back_str_vector(vec, "two");
    push_back_str_vector(vec, "three");
    push_back_str_vector(vec, "four");    

    reverse_str_vector(vec);

    char* a[4] = {"four", "three", "two", "one"};
    for (size_t i = 0; i < s_size(vec); i++) {
        assert_string_equal(a[i], get_string(str_vector_index(vec, i)));
    }

    free_str_vector(vec);
}
// -------------------------------------------------------------------------------- 

static string_v* make_vec(const char* items[], size_t n) {
    string_v* v = init_str_vector(n > 0 ? n : 1);
    if (!v) return NULL;

    for (size_t i = 0; i < n; ++i) {
        if (!push_back_str_vector(v, items[i])) {
            free_str_vector(v);
            return NULL;
        }
    }
    return v;
}

/* Check ascending lexical order using public accessors. */
static bool is_sorted_asc(const string_v* v) {
    errno = 0;
    size_t n = str_vector_size(v);
    assert_int_equal(errno, 0);

    for (size_t i = 1; i < n; ++i) {
        const string_t* a = str_vector_index(v, i - 1);
        const string_t* b = str_vector_index(v, i);
        assert_non_null(a);
        assert_non_null(b);
        const char* as = get_string(a);
        const char* bs = get_string(b);
        assert_non_null(as);
        assert_non_null(bs);
        if (strcmp(as, bs) > 0) return false;
    }
    return true;
}

/* ---------- Tests ---------- */

void test_bs_null_vec(void **state) {
    (void)state;
    errno = 0;
    size_t idx = binary_search_str_vector(NULL, (char*)"x", false); /* see note about const */
    assert_int_equal(idx, SIZE_MAX);
    assert_int_equal(errno, EINVAL);
}

void test_bs_null_value(void **state) {
    (void)state;
    string_v* v = init_str_vector(2);
    assert_non_null(v);

    errno = 0;
    size_t idx = binary_search_str_vector(v, NULL, false);
    assert_int_equal(idx, SIZE_MAX);
    assert_int_equal(errno, EINVAL);
    assert_int_equal(get_str_vector_error(v), NULL_POINTER);

    free_str_vector(v);
}

void test_bs_empty_vector(void **state) {
    (void)state;
    string_v* v = init_str_vector(4);
    assert_non_null(v);

    errno = 0;
    size_t idx = binary_search_str_vector(v, (char*)"a", false);
    assert_int_equal(idx, SIZE_MAX);
    assert_int_equal(errno, ENOENT);
    assert_int_equal(get_str_vector_error(v), NOT_FOUND);

    free_str_vector(v);
}

void test_bs_found_sorted_no_sort(void **state) {
    (void)state;
    const char* items[] = {"alpha", "beta", "delta", "omega"}; /* already ascending */
    string_v* v = make_vec(items, 4);
    assert_non_null(v);

    errno = EINTR; /* should be preserved on success */
    size_t idx = binary_search_str_vector(v, (char*)"delta", false);
    assert_true(idx != SIZE_MAX);
    assert_int_equal(errno, EINTR);
    assert_int_equal(get_str_vector_error(v), NO_ERROR);

    /* Optional: verify we actually matched the right string */
    const string_t* hit = str_vector_index(v, idx);
    assert_non_null(hit);
    assert_string_equal(get_string(hit), "delta");

    free_str_vector(v);
}

void test_bs_not_found_sorted_no_sort(void **state) {
    (void)state;
    const char* items[] = {"alpha", "beta", "delta", "omega"};
    string_v* v = make_vec(items, 4);
    assert_non_null(v);

    errno = 0;
    size_t idx = binary_search_str_vector(v, (char*)"kappa", false);
    assert_int_equal(idx, SIZE_MAX);
    assert_int_equal(errno, ENOENT);
    assert_int_equal(get_str_vector_error(v), NOT_FOUND);

    free_str_vector(v);
}

void test_bs_found_unsorted_sort_first_true(void **state) {
    (void)state;
    const char* items[] = {"delta", "alpha", "omega", "beta"}; /* intentionally unsorted */
    string_v* v = make_vec(items, 4);
    assert_non_null(v);

    errno = EBUSY; /* should be preserved on success */
    size_t idx = binary_search_str_vector(v, (char*)"beta", true); /* will sort first */
    assert_true(idx != SIZE_MAX);
    assert_int_equal(errno, EBUSY);
    assert_int_equal(get_str_vector_error(v), NO_ERROR);
    assert_true(is_sorted_asc(v));

    const string_t* hit = str_vector_index(v, idx);
    assert_non_null(hit);
    assert_string_equal(get_string(hit), "beta");

    free_str_vector(v);
}

void test_bs_duplicates_sort_first_true(void **state) {
    (void)state;
    const char* items[] = {"beta", "alpha", "beta", "beta", "gamma"};
    string_v* v = make_vec(items, 5);
    assert_non_null(v);

    errno = 0;
    size_t idx = binary_search_str_vector(v, (char*)"beta", true);
    assert_true(idx != SIZE_MAX);
    assert_int_equal(errno, 0);
    assert_true(is_sorted_asc(v));

    const string_t* hit = str_vector_index(v, idx);
    assert_non_null(hit);
    assert_string_equal(get_string(hit), "beta");
    /* We do not assert first/last duplicate—algorithm is allowed to return any match */

    free_str_vector(v);
}
// ================================================================================
// ================================================================================
// eof
