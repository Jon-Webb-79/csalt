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
        {"IArena Allocator", test_iarena, test_iarena_count},
        {"Freelist Allocator",  test_freelist, test_freelist_count},
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

