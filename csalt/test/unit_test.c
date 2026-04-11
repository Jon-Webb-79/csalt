// ================================================================================
// ================================================================================
// - File:    unit_test.c
// - Purpose: This file contains an implementation of the cmocka unit test frameword 
//            for the c_string library
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    December 30, 2024
// - Version: 0.1
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
#include "test_suite.h"

// ================================================================================
// ================================================================================
// TEST SUITE REGISTRY

typedef struct {
    const char *name;
    const struct CMUnitTest *tests;
    size_t count;
} TestSuite;

// ================================================================================
// ================================================================================
// MAIN TEST RUNNER

int main(void) {
    // Define all test suites to run
    const TestSuite suites[] = {
        {"Error Handling",  test_error, test_error_count},
        {"Arena Allocator", test_arena, test_arena_count},
        {"Pool Allocator", test_pool, test_pool_count},
        {"Freelist Allocator",  test_freelist, test_freelist_count},
        {"Buddy Allocator",  test_buddy_allocator, test_buddy_allocator_count},
        {"Slab Allocator",  test_slab_allocator, test_slab_allocator_count},
        {"String Implementation", test_string, test_string_count},
        {"Types Implementation", test_dtypes, test_dtypes_count},
        {"Uint8 Array", test_uint8_array, test_uint8_array_count},
        {"Int8 Array", test_int8_array, test_int8_array_count},
        {"Uint16 Array", test_uint16_array, test_uint16_array_count},
        {"Int16 Array", test_int16_array, test_int16_array_count},
        {"Uint32 Array", test_uint32_array, test_uint32_array_count},
        {"Int32 Array", test_int32_array, test_int32_array_count},
        {"Uint64 Array", test_uint64_array, test_uint64_array_count},
        {"Int64 Array", test_int64_array, test_int64_array_count},
        {"Float Array", test_float_array, test_float_array_count},
        {"Double Array", test_double_array, test_double_array_count},
        {"Long Double Array", test_ldouble_array, test_ldouble_array_count},
        {"String Array", test_string_array, test_string_array_count},
        {"Uint8 Dict", test_uint8_dict, test_uint8_dict_count},
        {"Int8 Dict", test_int8_dict, test_int8_dict_count},
        {"Uint16 Dict", test_uint16_dict, test_uint16_dict_count},
        {"Int16 Dict", test_int16_dict, test_int16_dict_count},
        {"UInt32 Dict", test_uint32_dict, test_uint32_dict_count},
        {"Int32 Dict", test_int32_dict, test_int32_dict_count},
        {"Uint64 Dict", test_uint64_dict, test_uint64_dict_count},
        {"Int64 Dict", test_int64_dict, test_int64_dict_count},
        {"Float Dict", test_float_dict, test_float_dict_count},
        {"Double Dict", test_double_dict, test_double_dict_count},
        {"LDouble Dict", test_ldouble_dict, test_ldouble_dict_count},
        {"Singly Linked List", test_slist, test_slist_count},
        {"Heap", test_heap, test_heap_count},
        {"AVL", test_avl, test_avl_count},
        {"Generic Matrix", test_matrix, test_matrix_count},
        {"Float Matrix", test_float_matrix, test_float_matrix_count},
        {"Double Matrix", test_double_matrix, test_double_matrix_count},
        {"LDouble Matrix", test_ldouble_matrix, test_ldouble_matrix_count},
        {"UInt8 Matrix", test_uint8_matrix, test_uint8_matrix_count},
        {"Int8 Matrix", test_int8_matrix, test_int8_matrix_count},
    };
    
    const size_t num_suites = sizeof(suites) / sizeof(suites[0]);
    int total_failed = 0;
    
    printf("\n========================================\n");
    printf("  CSalt Test Suite\n");
    printf("========================================\n");
    
    // Run each suite
    for (size_t i = 0; i < num_suites; i++) {
        printf("\n>>> Running: %s\n", suites[i].name);
        
        int result = _cmocka_run_group_tests(
            suites[i].name,
            suites[i].tests,
            suites[i].count,
            NULL,  // group setup
            NULL   // group teardown
        );
        
        if (result != 0) {
            total_failed++;
            printf("FAILED: %s\n", suites[i].name);
        }
    }
    
    // Print summary
    printf("\n========================================\n");
    printf("  Test Summary\n");
    printf("========================================\n");
    printf("Total suites run:    %zu\n", num_suites);
    printf("Suites passed:       %zu\n", num_suites - total_failed);
    printf("Suites failed:       %d\n", total_failed);
    printf("========================================\n\n");
    
    return (total_failed > 0) ? 1 : 0;
}

// ================================================================================
// ================================================================================
// eof

