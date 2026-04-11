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
#include <setjmp.h>
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
 * @brief Test suite for free list allocator functionality
 * 
 * Covers:
 * - Test of pool initialization with an non owned arena
 */
extern const struct CMUnitTest test_freelist[];
extern const size_t test_freelist_count;
// ================================================================================ 
// ================================================================================ 

/**
 * @brief Test suite for buddy allocator functionality
 * 
 * Covers:
 * - Test of buddy allocation initialization with an non owned arena
 */
extern const struct CMUnitTest test_buddy_allocator[];
extern const size_t test_buddy_allocator_count;
// ================================================================================
// ================================================================================

/**
 * @brief Test suite for slab allocator functionality
 * 
 * Covers:
 * - Test of slab allocation initialization with an non owned arena
 */
extern const struct CMUnitTest test_slab_allocator[];
extern const size_t test_slab_allocator_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_string[];
extern const size_t test_string_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_dtypes[];
extern const size_t test_dtypes_count;
// ================================================================================
// ================================================================================

extern const struct CMUnitTest test_uint8_array[];
extern const size_t test_uint8_array_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_int8_array[];
extern const size_t test_int8_array_count;
// ================================================================================
// ================================================================================

extern const struct CMUnitTest test_uint16_array[];
extern const size_t test_uint16_array_count;
// ================================================================================
// ================================================================================

extern const struct CMUnitTest test_int16_array[];
extern const size_t test_int16_array_count;
// ================================================================================
// ================================================================================

extern const struct CMUnitTest test_uint32_array[];
extern const size_t test_uint32_array_count;
// ================================================================================
// ================================================================================

extern const struct CMUnitTest test_int32_array[];
extern const size_t test_int32_array_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_uint64_array[];
extern const size_t test_uint64_array_count;
// ================================================================================
// ================================================================================

extern const struct CMUnitTest test_int64_array[];
extern const size_t test_int64_array_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_float_array[];
extern const size_t test_float_array_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_double_array[];
extern const size_t test_double_array_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_ldouble_array[];
extern const size_t test_ldouble_array_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_string_array[];
extern const size_t test_string_array_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_uint8_dict[];
extern const size_t test_uint8_dict_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_int8_dict[];
extern const size_t test_int8_dict_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_uint16_dict[];
extern const size_t test_uint16_dict_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_int16_dict[];
extern const size_t test_int16_dict_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_uint32_dict[];
extern const size_t test_uint32_dict_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_int32_dict[];
extern const size_t test_int32_dict_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_uint64_dict[];
extern const size_t test_uint64_dict_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_int64_dict[];
extern const size_t test_int64_dict_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_float_dict[];
extern const size_t test_float_dict_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_double_dict[];
extern const size_t test_double_dict_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_ldouble_dict[];
extern const size_t test_ldouble_dict_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_slist[];
extern const size_t test_slist_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_heap[];
extern const size_t test_heap_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_avl[];
extern const size_t test_avl_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_matrix[];
extern const size_t test_matrix_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_float_matrix[];
extern const size_t test_float_matrix_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_double_matrix[];
extern const size_t test_double_matrix_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_ldouble_matrix[];
extern const size_t test_ldouble_matrix_count;
// ================================================================================ 
// ================================================================================ 

extern const struct CMUnitTest test_uint8_matrix[];
extern const size_t test_uint8_matrix_count;
// ================================================================================
// ================================================================================/
#ifdef __cplusplus
}
#endif

#endif /* TEST_SUITE_H */

// ================================================================================
// ================================================================================
// eof
