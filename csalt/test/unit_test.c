// ================================================================================
// ================================================================================
// - File:    unit_test.c
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

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "test_string.h"
#include "test_vector.h"

const struct CMUnitTest test_string[] = {
	cmocka_unit_test(test_string_init_one_var),
    cmocka_unit_test(test_string_init_two_var),
    cmocka_unit_test(test_string_init_two_var_less),
    #ifdef __GNUC__
    cmocka_unit_test(test_string_init_gbc),
    #endif
    cmocka_unit_test(test_string_init_post_free),
    cmocka_unit_test(test_get_string_fail_one),
    cmocka_unit_test(test_get_string_fail_two),
    cmocka_unit_test(test_string_length_fail_one),
    cmocka_unit_test(test_string_length_fail_two),
    cmocka_unit_test(test_insert_string_lit_insert_end),
    cmocka_unit_test(test_insert_string_lit_insert_middle),
    cmocka_unit_test(test_insert_string_lit_error_two),
    cmocka_unit_test(test_insert_string_lit_error_three),
    cmocka_unit_test(test_insert_string_str_insert_end),
    cmocka_unit_test(test_insert_string_str_insert_middle),
    cmocka_unit_test(test_trim_string_equal),
    cmocka_unit_test(test_trim_string_greater),
    cmocka_unit_test(test_trim_string_error_one),
    cmocka_unit_test(test_trim_string_error_two),
    cmocka_unit_test(test_copy_string),
    #ifdef __GNUC__
    cmocka_unit_test(test_copy_string_w_gbc),
    #endif
    cmocka_unit_test(test_compare_strings_lit_equal),
    cmocka_unit_test(test_compare_strings_lit_greater),
    cmocka_unit_test(test_compare_strings_lit_less),
    cmocka_unit_test(test_compare_strings_lit_oversize),
    cmocka_unit_test(test_compare_strings_str_equal),
    cmocka_unit_test(test_compare_strings_str_greater),
    cmocka_unit_test(test_compare_strings_str_less),
    cmocka_unit_test(test_compare_strings_str_oversize),
    cmocka_unit_test(test_find_first_char),
    cmocka_unit_test(test_find_first_char_does_not_exist),
    cmocka_unit_test(test_find_first_char_null_terminator),
    cmocka_unit_test(test_find_last_char),
    cmocka_unit_test(test_find_last_char_does_not_exist),
    cmocka_unit_test(test_find_last_char_null_terminator),
    cmocka_unit_test(test_find_first_string_lit),
    cmocka_unit_test(test_find_first_string_lit_null),
    cmocka_unit_test(test_find_first_string_lit_oversized),
    cmocka_unit_test(test_find_first_string_str),
    cmocka_unit_test(test_find_first_string_str_null),
    cmocka_unit_test(test_find_first_string_str_oversized),
    cmocka_unit_test(test_find_last_string_lit),
    cmocka_unit_test(test_find_last_string_lit_null),
    cmocka_unit_test(test_find_last_string_lit_oversized),
    cmocka_unit_test(test_find_last_string_str),
    cmocka_unit_test(test_find_last_string_str_null),
    cmocka_unit_test(test_find_last_string_str_oversized),
    cmocka_unit_test(test_pop_string_end_char),
    cmocka_unit_test(test_pop_string_end_null_struct),
    cmocka_unit_test(test_pop_string_end_null_data),
    cmocka_unit_test(test_pop_string_char),
    cmocka_unit_test(test_pop_string_null_struct),
    cmocka_unit_test(test_pop_string_null_data),
    cmocka_unit_test(test_pop_string_out_of_bounds),
    cmocka_unit_test(test_pop_string_token),
    cmocka_unit_test(test_pop_string_token_no_token),
    cmocka_unit_test(test_pop_string_token_null_struct),
    cmocka_unit_test(test_pop_string_token_null_data),
    #ifdef __GNUC__
    cmocka_unit_test(test_pop_string_token_gbc_true),
    #endif
    cmocka_unit_test(test_char_iterator_for_loop),
    cmocka_unit_test(test_char_iterator_while_loop),
    cmocka_unit_test(test_char_iterator_forward),
    cmocka_unit_test(test_char_iterator_forward),
    cmocka_unit_test(test_convert_to_uppercase),
    cmocka_unit_test(test_convert_to_lowercase),
    cmocka_unit_test(test_ptr_in_container),
    cmocka_unit_test(test_ptr_not_in_container),
    cmocka_unit_test(test_ptr_in_literal),
    cmocka_unit_test(test_ptr_not_in_literal),
    cmocka_unit_test(test_find_first_char_btw_pointers),
    cmocka_unit_test(test_find_last_char_btw_pointers),
    cmocka_unit_test(test_find_first_literal_btw_pointers),
    cmocka_unit_test(test_find_first_str_btw_pointers),
    cmocka_unit_test(test_find_last_str_between_pointers),
    cmocka_unit_test(test_drop_str_substring_char),
    cmocka_unit_test(test_drop_str_substring_str),
    cmocka_unit_test(test_replace_string_equal_size),
    cmocka_unit_test(test_replace_string_smaller_size),
    cmocka_unit_test(test_replace_string_larger_size),
    cmocka_unit_test(test_replace_string_equal_size_ptr),
    cmocka_unit_test(test_replace_string_smaller_size_ptr),
    cmocka_unit_test(test_replace_string_larger_size_ptr),
    cmocka_unit_test(test_replace_literal_equal_size),
    cmocka_unit_test(test_replace_literal_smaller_size),
    cmocka_unit_test(test_replace_literal_larger_size)
};

// Test vector functions 
const struct CMUnitTest test_vector[] = {
    cmocka_unit_test(test_init_char_vector),
    cmocka_unit_test(test_init_uchar_vector),
    cmocka_unit_test(test_init_short_vector),
    cmocka_unit_test(test_init_ushort_vector),
    cmocka_unit_test(test_init_int_vector),
    cmocka_unit_test(test_init_uint_vector),
    cmocka_unit_test(test_init_long_vector),
    cmocka_unit_test(test_init_ulong_vector),
    cmocka_unit_test(test_init_llong_vector),
    cmocka_unit_test(test_init_ullong_vector),
    cmocka_unit_test(test_init_float_vector),
    cmocka_unit_test(test_init_double_vector),
    cmocka_unit_test(test_init_ldouble_vector),
    cmocka_unit_test(test_init_bool_vector),
    cmocka_unit_test(test_init_string_vector),
    cmocka_unit_test(test_init_str_vector),
    cmocka_unit_test(test_get_char_vector_null_struct), 
    cmocka_unit_test(test_get_char_vector_null_data),
    cmocka_unit_test(test_get_char_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_uchar_vector_null_struct), 
    cmocka_unit_test(test_get_uchar_vector_null_data),
    cmocka_unit_test(test_get_uchar_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_short_vector_null_struct), 
    cmocka_unit_test(test_get_short_vector_null_data),
    cmocka_unit_test(test_get_short_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_ushort_vector_null_struct), 
    cmocka_unit_test(test_get_ushort_vector_null_data),
    cmocka_unit_test(test_get_ushort_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_int_vector_null_struct), 
    cmocka_unit_test(test_get_int_vector_null_data),
    cmocka_unit_test(test_get_int_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_uint_vector_null_struct), 
    cmocka_unit_test(test_get_uint_vector_null_data),
    cmocka_unit_test(test_get_uint_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_long_vector_null_struct), 
    cmocka_unit_test(test_get_long_vector_null_data),
    cmocka_unit_test(test_get_long_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_ulong_vector_null_struct), 
    cmocka_unit_test(test_get_ulong_vector_null_data),
    cmocka_unit_test(test_get_ulong_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_llong_vector_null_struct), 
    cmocka_unit_test(test_get_llong_vector_null_data),
    cmocka_unit_test(test_get_llong_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_ullong_vector_null_struct), 
    cmocka_unit_test(test_get_ullong_vector_null_data),
    cmocka_unit_test(test_get_ullong_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_float_vector_null_struct), 
    cmocka_unit_test(test_get_float_vector_null_data),
    cmocka_unit_test(test_get_float_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_double_vector_null_struct), 
    cmocka_unit_test(test_get_double_vector_null_data),
    cmocka_unit_test(test_get_double_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_ldouble_vector_null_struct), 
    cmocka_unit_test(test_get_ldouble_vector_null_data),
    cmocka_unit_test(test_get_ldouble_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_bool_vector_null_struct), 
    cmocka_unit_test(test_get_bool_vector_null_data),
    cmocka_unit_test(test_get_bool_vector_index_out_of_bounds),
    cmocka_unit_test(test_get_string_vector_null_struct), 
    cmocka_unit_test(test_get_string_vector_null_data),
};
// Begin code
int main(int argc, const char * argv[]) {
    int status;

    status = cmocka_run_group_tests(test_string, NULL, NULL);
    if (status != 0) {
        return status;
    }
    status = cmocka_run_group_tests(test_vector, NULL, NULL);
	return status;
}
// ================================================================================
// ================================================================================
// eof

