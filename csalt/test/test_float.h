// ================================================================================
// ================================================================================
// - File:    test.h
// - Purpose: This file contains prototypes for unit tests related to the 
//            c_float.h file.  This file support cmocka testing environments.
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 31, 2022
// - Version: 0.1
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef test_flt_struct_H
#define test_flt_struct_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
// ================================================================================ 
// ================================================================================ 

void test_init_float_vector_success(void **state);
// --------------------------------------------------------------------------------

void test_init_float_vector_zero_size(void **state);
// -------------------------------------------------------------------------------- 

void test_init_float_array_success(void **state);
// --------------------------------------------------------------------------------

void test_free_float_vector_null(void **state);
// -------------------------------------------------------------------------------- 

void test_free_float_vector_static(void **state);
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
    void test_float_vector_gbc(void **state);
#endif
// ================================================================================ 
// ================================================================================ 

void test_push_back_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_push_back_growth(void **state);
// -------------------------------------------------------------------------------- 

void test_push_back_static(void **state);
// -------------------------------------------------------------------------------- 

void test_generic_macros(void **state);
// -------------------------------------------------------------------------------- 

void test_float_vector_index(void **state);
// -------------------------------------------------------------------------------- 

void test_push_back_special_values(void **state);
// --------------------------------------------------------------------------------

void test_static_array_bounds(void **state);
// --------------------------------------------------------------------------------

void test_static_array_index_bounds(void **state);
// --------------------------------------------------------------------------------

void test_static_array_initialization(void **state);
// -------------------------------------------------------------------------------- 

void test_static_array_free(void **state);
// ================================================================================ 
// ================================================================================ 

void test_push_front_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_push_front_growth(void **state);
// -------------------------------------------------------------------------------- 

void test_push_front_static(void **state);
// -------------------------------------------------------------------------------- 

void test_push_front_special_values(void **state);
// -------------------------------------------------------------------------------- 

void test_push_front_error_cases(void **state);
// ================================================================================ 
// ================================================================================ 

void test_insert_vector_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_insert_vector_growth(void **state);
// -------------------------------------------------------------------------------- 

void test_insert_array_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_insert_array_bounds(void **state);
// -------------------------------------------------------------------------------- 

void test_insert_error_cases(void **state);
// -------------------------------------------------------------------------------- 

void test_insert_special_values(void **state);
// ================================================================================ 
// ================================================================================ 

void test_pop_back_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_back_empty(void **state);
// --------------------------------------------------------------------------------

void test_pop_back_errors(void **state);
// --------------------------------------------------------------------------------

void test_pop_back_special_values(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_back_static(void **state);
// ================================================================================ 
// ================================================================================ 

void test_pop_front_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_front_empty(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_front_errors(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_front_special_values(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_front_static(void **state);
// ================================================================================ 
// ================================================================================ 

void test_pop_any_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_any_errors(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_any_static(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_any_special_values(void **state);
// ================================================================================ 
// ================================================================================ 

void test_reverse_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_reverse_errors(void **state);
// -------------------------------------------------------------------------------- 

void test_reverse_static(void **state);
// -------------------------------------------------------------------------------- 

void test_reverse_special_values(void **state);
// ================================================================================ 
// ================================================================================ 

void test_sort_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_sort_edge_cases(void **state);
// -------------------------------------------------------------------------------- 

void test_sort_duplicates(void **state);
// -------------------------------------------------------------------------------- 

void test_sort_special_values(void **state);
// -------------------------------------------------------------------------------- 

void test_sort_static_array(void **state);
// -------------------------------------------------------------------------------- 

void test_sort_errors(void **state);
// ================================================================================ 
// ================================================================================ 

void test_trim_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_trim_empty_vector(void **state);
// -------------------------------------------------------------------------------- 

void test_trim_static_array(void **state);
// -------------------------------------------------------------------------------- 

void test_trim_already_optimal(void **state);
// -------------------------------------------------------------------------------- 

void test_trim_errors(void **state);
// ================================================================================ 
// ================================================================================ 

void test_binary_search_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_binary_search_tolerance(void **state);
// -------------------------------------------------------------------------------- 

void test_binary_search_with_sort(void **state);
// -------------------------------------------------------------------------------- 

void test_binary_search_errors(void **state);
// -------------------------------------------------------------------------------- 

void test_binary_search_static(void **state);
// ================================================================================ 
// ================================================================================ 

void test_update_float_vector_nominal(void **state);
// -------------------------------------------------------------------------------- 

void test_update_float_vector_null(void **state);
// -------------------------------------------------------------------------------- 

void test_update_float_vector_bad_index(void **state);
// ================================================================================ 
// ================================================================================ 

void test_min_float_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_max_float_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_min_max_special_values(void **state);
// -------------------------------------------------------------------------------- 

void test_min_max_static_array(void **state);
// -------------------------------------------------------------------------------- 

void test_min_max_errors(void **state);
// ================================================================================ 
// ================================================================================ 

void test_sum_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_average_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_sum_average_special_values(void **state);
// -------------------------------------------------------------------------------- 

void test_sum_average_negative(void **state);
// -------------------------------------------------------------------------------- 

void test_sum_average_static(void **state);
// -------------------------------------------------------------------------------- 

void test_sum_average_errors(void **state);
// ================================================================================ 
// ================================================================================ 

void test_stdev_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_stdev_single_value(void **state);
// -------------------------------------------------------------------------------- 

void test_stdev_same_values(void **state);
// -------------------------------------------------------------------------------- 

void test_cum_sum_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_cum_sum_negative(void **state);
// -------------------------------------------------------------------------------- 

void test_stdev_cum_sum_special_values(void **state);
// -------------------------------------------------------------------------------- 

void test_stdev_cum_sum_errors(void **state);
// -------------------------------------------------------------------------------- 

void test_dot_product_basic(void **state);
// ----------------------------------------------------------------------------

void test_dot_product_mismatched_lengths(void **state);
// ----------------------------------------------------------------------------

void test_dot_product_null_inputs(void **state);
// ----------------------------------------------------------------------------

void test_dot_product_zero_length(void **state);
// -------------------------------------------------------------------------------- 

void test_cross_float_valid(void **state);
// --------------------------------------------------------------------------------

void test_cross_float_null_inputs(void **state);
// -------------------------------------------------------------------------------- 

void test_cross_float_vector_valid(void **state);
// --------------------------------------------------------------------------------

void test_cross_float_vector_invalid_inputs(void **state);
// ================================================================================ 
// ================================================================================ 

void test_init_float_dict(void** state);
// -------------------------------------------------------------------------------- 

void test_insert_float_dict_basic(void** state);
// -------------------------------------------------------------------------------- 

void test_insert_float_dict_duplicate(void** state);
// -------------------------------------------------------------------------------- 

void test_insert_float_dict_null(void** state);
// -------------------------------------------------------------------------------- 

void test_get_float_dict_value_basic(void** state);
// -------------------------------------------------------------------------------- 

void test_get_float_dict_value_missing(void** state);
// -------------------------------------------------------------------------------- 

void test_update_float_dict_basic(void** state);
// -------------------------------------------------------------------------------- 

void test_update_float_dict_missing(void** state);
// -------------------------------------------------------------------------------- 

void test_pop_float_dict_basic(void** state);
// -------------------------------------------------------------------------------- 

void test_pop_float_dict_missing(void** state);
// -------------------------------------------------------------------------------- 

void test_resize_behavior(void** state);
// -------------------------------------------------------------------------------- 

void test_get_keys_float_dict(void** state);
// -------------------------------------------------------------------------------- 

void test_get_values_float_dict(void** state);
// -------------------------------------------------------------------------------- 

int teardown(void **state);
// -------------------------------------------------------------------------------- 

int setup(void **state);
// -------------------------------------------------------------------------------- 

void test_foreach_float_dict_basic(void** state);
// -------------------------------------------------------------------------------- 

void test_foreach_float_dict_empty(void** state);
// -------------------------------------------------------------------------------- 

void test_foreach_float_dict_null(void** state);
// -------------------------------------------------------------------------------- 

void test_dictionary_float_gbc(void **state);
// ================================================================================ 
// ================================================================================ 

void test_vector_dictionary(void **state);
// -------------------------------------------------------------------------------- 

void test_vector_dictionary_resize(void **state);
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
    void test_vector_dictionary_gbc(void **state);
#endif
// -------------------------------------------------------------------------------- 

void test_pop_vector_dictionary(void **state);
// -------------------------------------------------------------------------------- 

void test_insert_floatv_dict_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_floatv_size_macros(void **state);
// -------------------------------------------------------------------------------- 

void test_copy_floatv_dict_success(void **state);
// -------------------------------------------------------------------------------- 

void test_copy_floatv_dict_null_input(void **state);
// -------------------------------------------------------------------------------- 

void test_copy_floatv_dict_static_vector(void **state);
// -------------------------------------------------------------------------------- 

void test_copy_floatv_dict_multiple_entries(void **state);
// -------------------------------------------------------------------------------- 

void test_copy_floatv_dict_independence(void **state);
// -------------------------------------------------------------------------------- 

void test_merge_floatv_dict_no_overwrite(void **state);
// -------------------------------------------------------------------------------- 

void test_merge_floatv_dict_overwrite(void **state);
// -------------------------------------------------------------------------------- 

void test_merge_floatv_dict_reject_static(void **state);
// -------------------------------------------------------------------------------- 

void test_merge_floatv_dict_null_inputs(void **state);
// -------------------------------------------------------------------------------- 

void test_merge_floatv_dict_unique_keys(void **state);
// -------------------------------------------------------------------------------- 

void test_clear_floatv_dict_basic(void **state);
// -------------------------------------------------------------------------------- 

void test_clear_floatv_dict_empty(void **state);
// -------------------------------------------------------------------------------- 

void test_clear_floatv_dict_reuse_after_clear(void **state);
// -------------------------------------------------------------------------------- 

void test_foreach_floatv_dict_counts_keys(void **state);
// -------------------------------------------------------------------------------- 

void test_foreach_floatv_dict_with_null_dict(void **state);
// -------------------------------------------------------------------------------- 

void test_foreach_floatv_dict_with_null_callback(void **state);
// -------------------------------------------------------------------------------- 

void test_foreach_floatv_dict_accumulates_sum(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST MATRIX 

void test_create_dense_float_matrix(void **state);
// -------------------------------------------------------------------------------- 

void test_insert_and_get_dense_float_matrix(void **state);
// -------------------------------------------------------------------------------- 

void test_invalid_dense_float_insert(void **state);
// -------------------------------------------------------------------------------- 

void test_float_coo_conversion_from_dense(void **state);
// -------------------------------------------------------------------------------- 

void test_insert_float_and_get_coo_matrix(void **state);
// --------------------------------------------------------------------------------

void test_duplicate_flaot_insert_coo_disallowed(void **state);
// -------------------------------------------------------------------------------- 

void test_float_csr_conversion_from_coo(void **state);
// -------------------------------------------------------------------------------- 

void test_get_after_float_csr_conversion(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_after_float_csr_conversion(void **state);
// -------------------------------------------------------------------------------- 

void test_get_float_matrix_bounds(void **state);
// -------------------------------------------------------------------------------- 

void test_insert_float_matrix_bounds(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_float_matrix_bounds(void **state);
// -------------------------------------------------------------------------------- 

void test_get_float_matrix_null(void **state);
// -------------------------------------------------------------------------------- 

void test_insert_float_matrix_null(void **state);
// -------------------------------------------------------------------------------- 

void test_pop_float_matrix_null(void **state);
// -------------------------------------------------------------------------------- 

#if defined(__GNUC__) || defined(__clang__)
    void test_float_matrix_gbc(void **state);
#endif
// -------------------------------------------------------------------------------- 

void test_identity_inverse(void **state); 
// -------------------------------------------------------------------------------- 

void test_known_inverse(void **state); 
// -------------------------------------------------------------------------------- 

void test_singular_matrix(void **state); 
// -------------------------------------------------------------------------------- 

void test_null_input(void **state); 
// -------------------------------------------------------------------------------- 

void test_non_square_matrix(void **state);
// -------------------------------------------------------------------------------- 

void test_transpose_dense_identity(void **state);
// -------------------------------------------------------------------------------- 

void test_transpose_dense_general(void **state);
// -------------------------------------------------------------------------------- 

void test_transpose_dense_rectangular(void **state);
// -------------------------------------------------------------------------------- 

void test_transpose_coo_identity(void **state);
// -------------------------------------------------------------------------------- 

void test_transpose_coo_rectangular(void **state);
// -------------------------------------------------------------------------------- 

void test_transpose_csr_identity(void **state);
// -------------------------------------------------------------------------------- 

void test_transpose_csr_rectangular(void **state);
// -------------------------------------------------------------------------------- 

void test_copy_dense_matrix(void **state);
// -------------------------------------------------------------------------------- 

void test_copy_coo_matrix(void **state);
// -------------------------------------------------------------------------------- 

void test_copy_csr_matrix(void **state);
// -------------------------------------------------------------------------------- 

void test_det_identity_3x3(void **state);
// -------------------------------------------------------------------------------- 

void test_det_singular_2x2(void **state);
// -------------------------------------------------------------------------------- 

void test_det_known_2x2(void **state);
// -------------------------------------------------------------------------------- 

void test_det_known_3x3(void **state);
// ================================================================================ 
// ================================================================================ 
#endif /* test_H */
// ================================================================================
// ================================================================================
// eof
