// ================================================================================
// ================================================================================
// - File:    test_suite.h
// - Purpose: Test suite declarations for arena allocator
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    October 26, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include <stdarg.h>  // va_list (needed by cmocka)
#include <setjmp.h>  // jmp_buf (needed by cmocka)
#include <stddef.h>  // size_t
#include <cmocka.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================================================================================
// ================================================================================
// ARENA ALLOCATOR TEST SUITE

/**
 * @brief Test suite for arena allocator functionality
 * 
 * Covers:
 * - Dynamic and static arena initialization
 * - Memory allocation and deallocation
 * - Growth and resize strategies
 * - Pointer validation functions
 * - Reset and checkpoint/restore operations
 * - Convenience macros
 */
extern const struct CMUnitTest test_arena[];
extern const size_t test_arena_count;
// ================================================================================ 
// ================================================================================ 

/**
 * @brief Test suite for error code functionality
 * 
 * Covers:
 * - error code initialization
 * - error code to errno mapping
 * - error code mapping to logging messages
 */
extern const struct CMUnitTest test_error[];
extern const size_t test_error_count;
// ================================================================================ 
// ================================================================================ 

/**
 * @brief Test suite for pool allocator functionality
 * 
 * Covers:
 * - Test of pool initialization with an non owned arena
 */
extern const struct CMUnitTest test_pool[];
extern const size_t test_pool_count;
// ================================================================================
// ================================================================================

/**
 * @brief Test suite for iarena allocator functionality
 * 
 * Covers:
 * - Test of pool initialization with an non owned arena
 */
extern const struct CMUnitTest test_iarena[];
extern const size_t test_iarena_count;
// ================================================================================
// ================================================================================

#ifdef __cplusplus
}
#endif

#endif /* TEST_SUITE_H */

// ================================================================================
// ================================================================================
// eof
