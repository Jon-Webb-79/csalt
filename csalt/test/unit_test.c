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
// MAIN TEST RUNNER

int main(void) {
    printf("\n========================================\n");
    printf("  CSalt Test Suite\n");
    printf("========================================\n\n");
    
    // Use the low-level function that accepts a count
    int result = _cmocka_run_group_tests(
        "Arena Tests",
        test_arena,
        test_arena_count,  // ← Pass the count explicitly
        NULL,  // group setup
        NULL   // group teardown
    );
    
    printf("\n========================================\n");
    if (result == 0) {
        printf("  All tests PASSED ✓\n");
    } else {
        printf("  Some tests FAILED ✗\n");
    }
    printf("========================================\n\n");
    
    return result;
}
// ================================================================================
// ================================================================================
// eof

