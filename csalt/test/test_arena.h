// ================================================================================
// ================================================================================
// - File:    test_arena.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 31, 2022
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef test_arena_H
#define test_arena_H

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
// ================================================================================ 
// ================================================================================ 
// TEST INIT DYNAMIC AND STATIC ARENAS 

void test_dyn_min_chunk_rounds_up_and_changes_capacity(void **state);
void test_dyn_min_chunk_equivalence_6000_vs_8192(void **state);
void test_dyn_alignment_rounds_up_and_floors_to_maxalign(void **state);
void test_static_alignment_honored_even_with_unaligned_buffer(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST INIT SARENA AND DARENA FUNCTIONS

void test_init_darena(void **state);
void test_init_darena_no_bytes(void **state);
void test_init_darena_large_chunk(void **state);
void test_init_sarena(void **state);
void test_invalid_free_sarena(void **state);
void test_arena_double_free(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST ARENA ALLOC FUNCTIONS

void test_alloc_darena(void **state);
void test_alloc_darena_zeroed(void **state);
void test_alloc_darena_null_value(void **state);
void test_alloc_darena_zero_input(void **state);

void test_alloc_sarena(void **state);
void test_alloc_sarena_zeroed(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST RE-ALLOCATION STRATEGY 

void test_realloc_grows_when_allowed(void **state);
void test_realloc_fails_when_resize_false(void **state);
void test_realloc_fails_in_static_arena(void **state);
void test_realloc_first_alloc_in_new_chunk_is_aligned_and_no_pad(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST IS_PTR FUNCTIONS 

void test_is_arena_ptr_basic_hits_and_misses(void **state);
void test_is_arena_ptr_tail_fastpath(void **state);
void test_is_arena_ptr_sized_boundaries(void **state);
void test_is_arena_ptr_multichunk(void **state);
void test_is_arena_ptr_sized_cross_chunk_fails(void **state);
void test_is_arena_ptr_null_and_zero_size_guards(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST RESET ARENA 

void test_reset_dynamic_trim_true_frees_extra_chunks_and_resets_usage(void **state);
void test_reset_dynamic_keep_chunks_preserves_capacity(void **state);
void test_reset_static_zeroes_usage_ignores_trim(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST SAVE AND RESTORE 

void test_save_restore_same_chunk_pointer_roundtrip(void **state);
void test_save_restore_second_chunk_trims_and_replays_allocation(void **state);
void test_save_restore_static_rewinds_cursor(void **state);
void test_restore_rejects_checkpoint_from_other_arena(void **state);
void test_restore_accepts_empty_checkpoint_noop(void **state);
// ================================================================================ 
// ================================================================================ 
// TEST TRIM ARENA 

// ================================================================================ 
// ================================================================================ 
// TEST MACROS 

#if !defined(__NO_GENERICS__)
void test_arena_alloc_type_and_type_zeroed(void **state);
void test_arena_alloc_array_and_array_zeroed(void **state);
void test_arena_alloc_array_count_zero_is_error(void **state);
#endif
// ================================================================================ 
// ================================================================================ 
#endif /* test_arena_H */
// ================================================================================
// ================================================================================
// eof
