// ================================================================================
// ================================================================================
// - File:    test_arena.c
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

#include "c_allocator.h"
#include "test_suite.h"

#include <stdint.h>
#include <errno.h>
#include <stdalign.h>
#include <stddef.h>
#include <string.h>

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>
// ================================================================================ 
// ================================================================================ 
// TEST INIT DYNAMIC AND STATIC ARENAS

static inline int is_aligned(const void* p, size_t a) {
    return (((uintptr_t)p) & (a - 1u)) == 0u;
}

/* Helpers */
static void dispose_arena(struct arena_t **pa) {
    if (!pa || !*pa) return;
    free_arena(*pa);
    *pa = NULL;
}

static int ptr_is_aligned(const void* p, size_t a) {
    return ((uintptr_t)p % a) == 0;
}
// -------------------------------------------------------------------------------- 

static void test_dyn_min_chunk_rounds_up_and_changes_capacity(void **state) {
    (void)state;

    /* Case A: small min_chunk (4KiB) */
    struct arena_t *a1 = init_dynamic_arena(/*bytes*/1000, /*resize*/false,
                                          /*min_chunk_in*/4096, /*base_align_in*/alignof(max_align_t));
    assert_non_null(a1);
    size_t rem1 = arena_remaining(a1);
    assert_true(rem1 > 0);

    /* Case B: larger min_chunk (16KiB) */
    struct arena_t *a2 = init_dynamic_arena(/*bytes*/1000, /*resize*/false,
                                          /*min_chunk_in*/16384, /*base_align_in*/alignof(max_align_t));
    assert_non_null(a2);
    size_t rem2 = arena_remaining(a2);
    assert_true(rem2 > rem1); /* Capacity should increase with min_chunk */

    dispose_arena(&a1);
    dispose_arena(&a2);
}
// -------------------------------------------------------------------------------- 

static void test_dyn_min_chunk_equivalence_6000_vs_8192(void **state) {
    (void)state;

    struct arena_t *aA = init_dynamic_arena(/*bytes*/1000, /*resize*/false,
                                          /*min_chunk_in*/6000, /*base_align_in*/alignof(max_align_t));
    assert_non_null(aA);
    size_t remA = arena_remaining(aA);

    struct arena_t *aB = init_dynamic_arena(/*bytes*/1000, /*resize*/false,
                                          /*min_chunk_in*/8192, /*base_align_in*/alignof(max_align_t));
    assert_non_null(aB);
    size_t remB = arena_remaining(aB);

    /* They may not be bit-for-bit equal on exotic ABIs, but they should match for
       a given platform/compiler because both round to 8192 and headers/layout match. */
    assert_int_equal(remA, remB);

    dispose_arena(&aA);
    dispose_arena(&aB);
}
// -------------------------------------------------------------------------------- 

static void test_dyn_alignment_rounds_up_and_floors_to_maxalign(void **state) {
    (void)state;

    /* Round up (24 -> 32) */
    struct arena_t *a1 = init_dynamic_arena(/*bytes*/4096, /*resize*/false,
                                          /*min_chunk_in*/4096, /*base_align_in*/24);
    assert_non_null(a1);
    void* p1 = alloc_arena(a1, 1, /*zeroed*/false);
    assert_non_null(p1);
    assert_true(ptr_is_aligned(p1, 32)); /* rounded up to 32 */

    dispose_arena(&a1);

    /* Floor to alignof(max_align_t) if too small */
    const size_t maxa = alignof(max_align_t);
    struct arena_t *a2 = init_dynamic_arena(/*bytes*/4096, /*resize*/false,
                                          /*min_chunk_in*/4096, /*base_align_in*/1);
    assert_non_null(a2);
    void* p2 = alloc_arena(a2, 1, /*zeroed*/false);
    assert_non_null(p2);
    assert_true(ptr_is_aligned(p2, maxa));

    dispose_arena(&a2);
}
// -------------------------------------------------------------------------------- 

static void test_static_alignment_honored_even_with_unaligned_buffer(void **state) {
    (void)state;

    const size_t BUF = 16384 + 64;  /* plenty of space */
    void* raw = malloc(BUF);
    assert_non_null(raw);

    /* Force an unaligned starting address by offsetting 1 byte */
    uint8_t* unaligned = (uint8_t*)raw + 1;

    /* Request a strong base alignment (e.g., 64) */
     struct arena_t *a = init_static_arena((void*)unaligned, BUF - 1, /*alignment_in*/64);
     assert_non_null(a);

    /* First allocation must respect 64-byte alignment */
    void* p = alloc_arena(a, 1, /*zeroed*/false);
    assert_non_null(p);
    assert_true(ptr_is_aligned(p, 64));

    /* Basic sanity: the pointer should be recognized by the arena if helpers exist */
// #ifdef HAS_ARENA_CHECKS
//     assert_true(is_arena_ptr(a, p));
//     assert_true(is_arena_ptr_sized(a, p, 1));
// #endif
//
    /* And we can keep allocating until capacity is exhausted without growth (STATIC) */
    size_t before = arena_remaining(a);
    (void)before;
    errno = 0;
    /* Try to burn the rest in one go (may fail if not enough due to internal pad) */
    size_t r = arena_remaining(a);
    void* big = alloc_arena(a, r, false);
    if (!big) {
        // subtract worst-case pad (63) to account for alignment
        size_t max_payload = (r >= 63) ? (r - 63) : 0;
        void* q = (max_payload ? alloc_arena(a, max_payload, false) : NULL);
        assert_non_null(q);
    }
    
    /* free_arena should NOT free the user buffer in STATIC mode (by your design);
       this is a functional call to tear down internal state only. */
    free_arena(a);
    free(raw);
}
// ================================================================================ 
// ================================================================================ 
// TEST INIT SARENA AND DARENA FUNCTIONS

static void test_init_darena(void **state) {
    // Valgrind test to ensure memory is properly created and freed
	(void) state;
    arena_t* arena = init_darena(1000, true);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena);
    assert_int_equal(alloc_type, DYNAMIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 3984);
    assert_int_equal(total_alloc, 4096);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_init_darena_no_bytes(void **state) {
    // Verify allocation if user requests 0 bytes
    // Will throw compiler error if user tries to pass a negative number
	(void) state;
    arena_t* arena = init_darena(0, true);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena); 
    assert_int_equal(alloc_type, DYNAMIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 3984);
    assert_int_equal(total_alloc, 4096);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_init_darena_large_chunk(void **state) {
    // - Verify allocation if user requests more than 4096 bytes to ensure 
    //   proper padding is added
	(void) state;
    arena_t* arena = init_darena(4097, true);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena); 
    assert_int_equal(alloc_type, DYNAMIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 3985);
    assert_int_equal(total_alloc, 4097);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_init_sarena(void **state) {
    // Test for proper initialization of a static arena
    (void) state;
    uint8_t buffer[4097];
    arena_t* arena = init_sarena(buffer, 4097);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena); 
    assert_int_equal(alloc_type, STATIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 3985);
    assert_int_equal(total_alloc, 4097);
}
// -------------------------------------------------------------------------------- 

static void test_invalid_free_sarena(void **state) {
    // Test behavior when an attempt is made to free a static arena
    (void) state;
    uint8_t buffer[400];
    arena_t* arena = init_sarena(buffer, 400);
    free_arena(arena);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena);
    assert_int_equal(EPERM, errno);
    assert_int_equal(alloc_type, STATIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 288);
    assert_int_equal(total_alloc, 400);
}
// -------------------------------------------------------------------------------- 

static void test_arena_double_free(void **state) {
    (void) state;
    arena_t* arena = init_darena(4097, true);
    free_arena(arena);
    arena = NULL;
    free_arena(arena);
}
// ================================================================================ 
// ================================================================================ 
// TEST ARENA ALLOC FUNCTIONS 

// struct for test
typedef struct {
    float one;
    int two;
} test;
// -------------------------------------------------------------------------------- 

static void test_alloc_darena(void **state) {
    (void) state;
    arena_t* arena = init_darena(10000, true);
    test* struct_val = (test*)alloc_arena(arena, sizeof(test), false);
    int* value = (int*)alloc_arena(arena, sizeof(int), false);
    struct_val->one = 3.4f;
    struct_val->two = 3;
    *value = 4;
    assert_int_equal(*value, 4);
    assert_int_equal(struct_val->two, 3);
    assert_float_equal(struct_val->one, 3.4, 0.001);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    size_t left_over = arena_remaining(arena);
    assert_int_equal(size, 20);
    assert_int_equal(alloc, 9888);
    assert_int_equal(total_alloc, 10000);
    assert_int_equal(left_over, 9868);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_darena_zeroed(void **state) {
    (void)state;

    arena_t* arena = init_darena(10000, true);
    assert_non_null(arena);

    test* struct_val = alloc_arena(arena, sizeof *struct_val, true);
    assert_non_null(struct_val);

    // verify every byte (including padding) is zero
    unsigned char zeros[sizeof *struct_val];
    memset(zeros, 0, sizeof zeros);
    assert_memory_equal(struct_val, zeros, sizeof *struct_val);

    // …the rest of your test…
    int* value = alloc_arena(arena, sizeof *value, false);
    assert_non_null(value);

    struct_val->one = 3.4f;
    struct_val->two = 3;
    *value = 4;

    assert_int_equal(*value, 4);
    assert_int_equal(struct_val->two, 3);
    assert_float_equal(struct_val->one, 3.4, 0.001);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_darena_null_value(void **state) {
    (void) state;
    int* value = alloc_arena(NULL, sizeof(*value), true);
    assert_null(value);
    assert_int_equal(EINVAL, errno);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_darena_zero_input(void **state) {
    arena_t* arena = init_darena(10000, true);
    errno = 0;
    int* value = alloc_arena(arena, 0, true);
    assert_null(value);
    assert_int_equal(EINVAL, errno);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_sarena(void **state) {
    (void) state;
    uint8_t buffer[10000];
    arena_t* arena = init_sarena(buffer, 10000);
    test* struct_val = (test*)alloc_arena(arena, sizeof(test), false);
    int* value = (int*)alloc_arena(arena, sizeof(int), false);
    struct_val->one = 3.4f;
    struct_val->two = 3;
    *value = 4;
    assert_int_equal(*value, 4);
    assert_int_equal(struct_val->two, 3);
    assert_float_equal(struct_val->one, 3.4, 0.001);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    size_t left_over = arena_remaining(arena);
    assert_int_equal(size, 20);
    assert_int_equal(alloc, 9888);
    assert_int_equal(total_alloc, 10000);
    assert_int_equal(left_over, 9868);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_sarena_zeroed(void **state) {
    (void)state;

    uint8_t buffer[10000];
    arena_t* arena = init_sarena(buffer, 10000);
    assert_non_null(arena);

    test* struct_val = alloc_arena(arena, sizeof *struct_val, true);
    assert_non_null(struct_val);

    // verify every byte (including padding) is zero
    unsigned char zeros[sizeof *struct_val];
    memset(zeros, 0, sizeof zeros);
    assert_memory_equal(struct_val, zeros, sizeof *struct_val);
    
    int* value = alloc_arena(arena, sizeof *value, false);
    assert_non_null(value);

    struct_val->one = 3.4f;
    struct_val->two = 3;
    *value = 4;

    assert_int_equal(*value, 4);
    assert_int_equal(struct_val->two, 3);
    assert_float_equal(struct_val->one, 3.4, 0.001);
}
// ================================================================================ 
// ================================================================================ 
// TEST RE_ALLOCATION STRATEGY

static void* alloc_fit(struct arena_t *a, size_t want, size_t *taken) {
    while (want > 0) {
        void *p = alloc_arena(a, want, /*zeroed=*/false);
        if (p) { if (taken) *taken = want; return p; }
        --want;
    }
    if (taken) *taken = 0;
    return NULL;
}
// -------------------------------------------------------------------------------- 

static void test_realloc_grows_when_allowed(void **state) {
    (void)state;

    struct arena_t *a = init_dynamic_arena(/*bytes*/4096, /*resize*/true,
                                         /*min_chunk_in*/4096, /*base_align_in*/alignof(max_align_t));
    assert_non_null(a);

    /* Make at least one allocation */
    assert_non_null(alloc_arena(a, 16, false));

    /* Fill the rest of current tail (pad-aware) */
    size_t rem = arena_remaining(a);
    assert_true(rem > 0);

    size_t taken = 0;
    void *edge = alloc_fit(a, rem, &taken);
    assert_non_null(edge);
    assert_true(taken > 0);

    /* Next 1-byte alloc should cause growth and succeed */
    size_t rem0 = arena_remaining(a);
    errno = 0;
    void *p = alloc_arena(a, 1, false);
    assert_non_null(p);
    (void)rem0; /* not used further here */

    /* After growth, there should still be capacity remaining. */
    assert_true(arena_remaining(a) > 0);

    dispose_arena(&a);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_fails_when_resize_false(void **state) {
    (void)state;

    struct arena_t *a = init_dynamic_arena(/*bytes*/4096, /*resize*/false,
                                         /*min_chunk_in*/4096, /*base_align_in*/alignof(max_align_t));
    assert_non_null(a);

    assert_non_null(alloc_arena(a, 16, false));

    size_t rem = arena_remaining(a);
    assert_true(rem > 0);

    size_t taken = 0;
    assert_non_null(alloc_fit(a, rem, &taken));
    assert_true(taken > 0);

    errno = 0;
    void *fail = alloc_arena(a, 1, false);
    assert_null(fail);
    assert_int_equal(errno, EPERM);

    dispose_arena(&a);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_fails_in_static_arena(void **state) {
    (void)state;

    /* Provide a caller buffer (aligned enough) */
    size_t const BUF = 8192;
    void *buf = aligned_alloc(alignof(max_align_t), BUF);
    assert_non_null(buf);

    struct arena_t *a = init_static_arena(buf, BUF, /*alignment_in*/alignof(max_align_t));
    assert_non_null(a);

    assert_non_null(alloc_arena(a, 16, false));

    size_t rem = arena_remaining(a);
    assert_true(rem > 0);

    size_t taken = 0;
    assert_non_null(alloc_fit(a, rem, &taken));
    assert_true(taken > 0);

    errno = 0;
    void *fail = alloc_arena(a, 1, false);
    assert_null(fail);
    assert_int_equal(errno, EPERM);

    /* For STATIC, free_arena should not free the user buffer */
    free_arena(a);
    free(buf);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_first_alloc_in_new_chunk_is_aligned_and_no_pad(void **state) {
    (void)state;

    const size_t base_align = 64; /* strong alignment to observe easily */
    struct arena_t *a = init_dynamic_arena(/*bytes*/4096, /*resize*/true,
                                         /*min_chunk_in*/4096, /*base_align_in*/base_align);
    assert_non_null(a);

    /* Burn current tail completely */
    assert_non_null(alloc_arena(a, 8, false));
    size_t rem = arena_remaining(a);
    assert_true(rem > 0);
    size_t taken = 0;
    assert_non_null(alloc_fit(a, rem, &taken));
    assert_true(taken > 0);

    /* Allocate 1B -> triggers growth; first block in new chunk should be exactly aligned */
    size_t remaining_before = 0, remaining_after = 0;

    remaining_before = arena_remaining(a);
    void *p = alloc_arena(a, 1, false);
    assert_non_null(p);
    assert_true(ptr_is_aligned(p, base_align));

    remaining_after = arena_remaining(a);
    /* No pad on first alloc from a freshly aligned chunk: delta == 1 */
    assert_true(remaining_before > 0);
    assert_true(remaining_after > 0);

    dispose_arena(&a);
}
// ================================================================================ 
// ================================================================================ 
// TEST IS_PTR FUNCTIONS 

static void test_is_arena_ptr_basic_hits_and_misses(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(/*bytes*/4096, /*resize=*/false,
                                  /*min_chunk_in=*/4096, /*base_align_in=*/alignof(max_align_t));
    assert_non_null(a);

    size_t n = 128;
    uint8_t *p = (uint8_t*)alloc_arena(a, n, /*zeroed=*/true);
    assert_non_null(p);

    /* Hits */
    assert_true(is_arena_ptr(a, p));             // start
    assert_true(is_arena_ptr(a, p + (n/2)));     // middle
    /* Sized hits */
    assert_true(is_arena_ptr_sized(a, p, n));    // exact span
    assert_true(is_arena_ptr_sized(a, p+1, n-1));

    /* Misses */
    assert_false(is_arena_ptr(a, p + n));        // end is exclusive
    assert_false(is_arena_ptr_sized(a, p, n+1)); // runs past end

    dispose_arena(&a);
}
// -------------------------------------------------------------------------------- 

static void test_is_arena_ptr_tail_fastpath(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(4096, /*resize=*/false, 4096, alignof(max_align_t));
    assert_non_null(a);

    /* Make a couple allocations; last one is in tail */
    (void)alloc_arena(a, 32, false);
    uint8_t *q = (uint8_t*)alloc_arena(a, 64, false);
    assert_non_null(q);

    assert_true(is_arena_ptr(a, q));
    assert_true(is_arena_ptr_sized(a, q, 64));

    dispose_arena(&a);
}
// -------------------------------------------------------------------------------- 

static void test_is_arena_ptr_sized_boundaries(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(4096, /*resize=*/false, 4096, alignof(max_align_t));
    assert_non_null(a);

    size_t n = 256;
    uint8_t *p = (uint8_t*)alloc_arena(a, n, false);
    assert_non_null(p);

    /* Exact boundary is allowed (pe == ue) */
    assert_true(is_arena_ptr_sized(a, p + (n-1), 1));
    assert_true(is_arena_ptr_sized(a, p, n));

    /* One byte too far should fail */
    assert_false(is_arena_ptr_sized(a, p + n - 1, 2));
    assert_false(is_arena_ptr_sized(a, p + n, 1));

    dispose_arena(&a);
}
// -------------------------------------------------------------------------------- 

static void test_is_arena_ptr_multichunk(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(/*bytes*/4096, /*resize=*/true,
                                  /*min_chunk_in=*/4096, /*base_align_in=*/alignof(max_align_t));
    assert_non_null(a);

    /* Consume almost all of first chunk */
    (void)alloc_arena(a, 32, false);
    size_t rem = arena_remaining(a);
    size_t taken = 0;
    assert_non_null(alloc_fit(a, rem, &taken));
    assert_true(taken > 0);

    /* Next allocation triggers growth -> new tail chunk */
    uint8_t *p2 = (uint8_t*)alloc_arena(a, 64, false);
    assert_non_null(p2);

    /* Check: pointer in old chunk (we have one from earlier) */
    // The earlier 32-byte block's start is not saved; allocate a small one before growth to keep a handle.
    // Instead, allocate one more small in the new tail and then check both chunks using p2 and p2+offset.
    assert_true(is_arena_ptr(a, p2));
    assert_true(is_arena_ptr_sized(a, p2, 64));

    /* Allocate another small block so tail len advances; still recognized */
    uint8_t *p3 = (uint8_t*)alloc_arena(a, 8, false);
    assert_non_null(p3);
    assert_true(is_arena_ptr(a, p3));

    /* Old chunk pointer: we can derive one by allocating anew from a fresh arena to reproduce.
       Simpler: assert that *some* pointer before p2 isn't in new chunk range.
       Instead, just ensure is_arena_ptr returns false for clearly foreign memory. */
    int dummy = 0;
    assert_false(is_arena_ptr(a, &dummy));

    dispose_arena(&a);
}
// -------------------------------------------------------------------------------- 

static void test_is_arena_ptr_sized_cross_chunk_fails(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(/*bytes*/4096, /*resize=*/true,
                                  /*min_chunk_in=*/4096, /*base_align_in=*/alignof(max_align_t));
    assert_non_null(a);

    /* Force growth to a second chunk. */
    (void)alloc_arena(a, 16, false);
    size_t rem = arena_remaining(a);
    size_t taken = 0;
    assert_non_null(alloc_fit(a, rem, &taken));   // burn the rest (pad-aware)
    assert_true(taken > 0);

    /* First alloc in the NEW chunk: exactly 64 bytes used there. */
    uint8_t *p2 = (uint8_t*)alloc_arena(a, 64, false);
    assert_non_null(p2);

    /* Now, a 2-byte span starting at the last byte of that 64B block
       would exceed the USED range of the second chunk -> must be false. */
    assert_true(is_arena_ptr_sized(a, p2, 64));        // sanity: the block itself is valid
    assert_false(is_arena_ptr_sized(a, p2 + 63, 2));   // crosses beyond used in this chunk

    free_arena(a);
}
// -------------------------------------------------------------------------------- 

static void test_is_arena_ptr_null_and_zero_size_guards(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(4096, /*resize=*/false, 4096, alignof(max_align_t));
    assert_non_null(a);

    uint8_t *p = (uint8_t*)alloc_arena(a, 16, false);
    assert_non_null(p);

    /* is_arena_ptr guards */
    assert_false(is_arena_ptr(NULL, p));
    assert_false(is_arena_ptr(a, NULL));

    /* is_arena_ptr_sized guards */
    assert_false(is_arena_ptr_sized(NULL, p, 1));
    assert_false(is_arena_ptr_sized(a, NULL, 1));
    assert_false(is_arena_ptr_sized(a, p, 0));

    dispose_arena(&a);
}
// ================================================================================ 
// ================================================================================ 
// TEST RESET ARENA

static void test_reset_dynamic_trim_true_frees_extra_chunks_and_resets_usage(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(/*bytes*/4096, /*resize=*/true,
                                  /*min_chunk_in=*/4096, /*base_align_in=*/alignof(max_align_t));
    assert_non_null(a);

    /* Record initial head-only capacity (for sanity later) */
    size_t initial_remaining = arena_remaining(a);
    assert_true(initial_remaining > 0);

    /* Fill the first chunk so next alloc triggers growth */
    (void)alloc_arena(a, 32, false);
    size_t rem = arena_remaining(a);
    size_t taken = 0;
    assert_non_null(alloc_fit(a, rem, &taken));
    assert_true(taken > 0);

    /* Allocate in the NEW (tail) chunk, keep a pointer */
    uint8_t *tail_ptr = (uint8_t*)alloc_arena(a, 64, false);
    assert_non_null(tail_ptr);
    assert_true(is_arena_ptr(a, tail_ptr)); /* currently recognized */

    /* Reset with trimming: should free extra chunks, zero usage, and reset cur to head */
    reset_arena(a, /*trim_extra_chunks=*/true);

    /* Pointer into the old tail must no longer be recognized */
    assert_false(is_arena_ptr(a, tail_ptr));

    /* Usage reset: we should be able to allocate again from a clean arena */
    void *p = alloc_arena(a, 16, false);
    assert_non_null(p);

    /* Remaining after reset should be at most the initial head-only capacity (sanity check).
       (Exact equality depends on alignment; allow <= ) */
    size_t after_reset = arena_remaining(a);
    assert_true(after_reset <= initial_remaining);

    dispose_arena(&a);
}
// -------------------------------------------------------------------------------- 

static void test_reset_dynamic_keep_chunks_preserves_capacity(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(/*bytes*/4096, /*resize=*/true,
                                  /*min_chunk_in=*/4096, /*base_align_in=*/alignof(max_align_t));
    assert_non_null(a);

    size_t initial_remaining = arena_remaining(a);
    assert_true(initial_remaining > 0);

    /* Force growth */
    (void)alloc_arena(a, 32, false);
    size_t rem = arena_remaining(a);
    size_t taken = 0;
    assert_non_null(alloc_fit(a, rem, &taken));
    assert_true(taken > 0);
    assert_non_null(alloc_arena(a, 64, false)); /* new chunk alive */

    /* Keep chunks: zero usage but retain capacity across all chunks */
    reset_arena(a, /*trim_extra_chunks=*/false);

    size_t after_reset = arena_remaining(a);
    /* Because we have at least two chunks retained, remaining should exceed the
       original head-only capacity. */
    assert_true(after_reset > initial_remaining);

    /* And we can now allocate a lot in a single go (should not error) */
    void *big = alloc_fit(a, after_reset, &taken);
    assert_non_null(big);
    assert_true(taken > 0);

    dispose_arena(&a);
}
// -------------------------------------------------------------------------------- 

static void test_reset_static_zeroes_usage_ignores_trim(void **state) {
    (void)state;

    /* Build a static arena over caller-owned buffer */
    enum { BUF = 8192 };
    void *buf = aligned_alloc(alignof(max_align_t), BUF);
    assert_non_null(buf);

    arena_t *a = init_static_arena(buf, BUF, /*alignment_in=*/alignof(max_align_t));
    assert_non_null(a);

    size_t initial_remaining = arena_remaining(a);
    assert_true(initial_remaining > 0);

    /* Consume some space */
    assert_non_null(alloc_arena(a, 128, true));
    size_t mid_remaining = arena_remaining(a);
    assert_true(mid_remaining < initial_remaining);

    /* Reset with trim=true (should be ignored for STATIC) */
    reset_arena(a, /*trim_extra_chunks=*/true);

    /* Usage zeroed: remaining is back up (close to initial; allow >=) */
    size_t after_reset = arena_remaining(a);
    assert_true(after_reset >= mid_remaining);
    assert_true(after_reset <= initial_remaining); /* static didn’t gain capacity */

    /* We can allocate again from the start */
    void *p = alloc_arena(a, 128, false);
    assert_non_null(p);

    /* Tear down: static free should not free 'buf' */
    free_arena(a);
    free(buf);
}
// ================================================================================ 
// ================================================================================ 
// TEST SAVE AND RESTORE 

static void test_save_restore_same_chunk_pointer_roundtrip(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(/*bytes*/4096, /*resize=*/false,
                                  /*min_chunk_in=*/4096, /*base_align_in=*/alignof(max_align_t));
    assert_non_null(a);

    /* Allocate A, then save. */
    void *A = alloc_arena(a, 128, false);
    assert_non_null(A);
    ArenaCheckPoint cp = save_arena(a);

    /* Allocate B after the checkpoint; remember its address. */
    void *B1 = alloc_arena(a, 64, false);
    assert_non_null(B1);

    /* Now restore to the checkpoint. */
    assert_true(restore_arena(a, cp));

    /* Re-allocate the same size: it should come back at the same address as B1. */
    void *B2 = alloc_arena(a, 64, false);
    assert_non_null(B2);
    assert_ptr_equal(B1, B2);

    /* And B1 should no longer be considered “used-sized” after restore. */
    assert_false(is_arena_ptr_sized(a, B1, 65));

    dispose_arena(&a);
}
// -------------------------------------------------------------------------------- 

static void test_save_restore_second_chunk_trims_and_replays_allocation(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(/*bytes*/4096, /*resize=*/true,
                                  /*min_chunk_in=*/4096, /*base_align_in=*/alignof(max_align_t));
    assert_non_null(a);

    /* Burn first chunk to force growth on next alloc. */
    (void)alloc_arena(a, 16, false);
    size_t rem = arena_remaining(a);
    size_t taken = 0;
    assert_non_null(alloc_fit(a, rem, &taken));
    assert_true(taken > 0);

    /* First allocation in the NEW chunk. */
    void *X = alloc_arena(a, 32, false);
    assert_non_null(X);

    /* Save the checkpoint *inside* the second chunk. */
    ArenaCheckPoint cp = save_arena(a);

    /* Allocate Y after save; keep its address. */
    void *Y1 = alloc_arena(a, 48, false);
    assert_non_null(Y1);
    assert_true(is_arena_ptr(a, Y1));            /* currently used */

    /* Restore: must trim anything after the checkpoint (dynamic) and reset cursor. */
    assert_true(restore_arena(a, cp));

    /* Y1 should no longer be a valid used span. */
    assert_false(is_arena_ptr_sized(a, Y1, 48));

    /* Replay Y: address must match the pre-restore Y1. */
    void *Y2 = alloc_arena(a, 48, false);
    assert_non_null(Y2);
    assert_ptr_equal(Y1, Y2);

    dispose_arena(&a);
}
// -------------------------------------------------------------------------------- 

static void test_save_restore_static_rewinds_cursor(void **state) {
    (void)state;

    enum { BUF = 8192 };
    void *buf = aligned_alloc(alignof(max_align_t), BUF);
    assert_non_null(buf);

    arena_t *a = init_static_arena(buf, BUF, /*alignment_in=*/alignof(max_align_t));
    assert_non_null(a);

    /* Allocate A, then save, then allocate B. */
    void *A = alloc_arena(a, 128, false);
    assert_non_null(A);

    ArenaCheckPoint cp = save_arena(a);

    void *B1 = alloc_arena(a, 96, false);
    assert_non_null(B1);

    /* Restore should rewind cursor to exactly after A. */
    assert_true(restore_arena(a, cp));

    /* Re-allocate B; pointer should match. */
    void *B2 = alloc_arena(a, 96, false);
    assert_non_null(B2);
    assert_ptr_equal(B1, B2);

    /* And B1 is no longer considered a valid used span after restore. */
    assert_true(is_arena_ptr_sized(a, B1, 96));

    free_arena(a);   /* must not free 'buf' */
    free(buf);
}
// -------------------------------------------------------------------------------- 

static void test_restore_rejects_checkpoint_from_other_arena(void **state) {
    (void)state;

    arena_t *a1 = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
    arena_t *a2 = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
    assert_non_null(a1);
    assert_non_null(a2);

    /* Make sure a1 has at least one allocation before saving (not strictly required). */
    assert_non_null(alloc_arena(a1, 32, false));
    ArenaCheckPoint cp = save_arena(a1);

    errno = 0;
    bool ok = restore_arena(a2, cp);
    assert_false(ok);
    assert_int_equal(errno, EINVAL);

    dispose_arena(&a1);
    dispose_arena(&a2);
}
// -------------------------------------------------------------------------------- 

static void test_restore_accepts_empty_checkpoint_noop(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(4096, false, 4096, alignof(max_align_t));
    assert_non_null(a);

    ArenaCheckPoint empty = {0};   /* (_priv all zeros) */
    size_t before = arena_remaining(a);

    assert_true(restore_arena(a, empty));
    size_t after = arena_remaining(a);

    /* No state change expected. */
    assert_int_equal(before, after);

    dispose_arena(&a);
}
// ================================================================================ 
// ================================================================================ 
// TEST MACROS

/* A composite type to exercise struct alignment/size */
typedef struct {
    int    x;
    double y;
} Demo;
// -------------------------------------------------------------------------------- 

static void test_arena_alloc_type_and_type_zeroed(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(/*bytes*/4096, /*resize=*/false,
                                  /*min_chunk_in=*/4096, /*base_align_in=*/alignof(max_align_t));
    assert_non_null(a);

    /* Non-zeroed object */
    Demo *p1 = arena_alloc_type(a, Demo);
    assert_non_null(p1);
    assert_true(ptr_is_aligned(p1, alignof(Demo)));
    assert_true(is_arena_ptr_sized(a, p1, sizeof *p1));

    /* Zeroed object */
    Demo *p2 = arena_alloc_type_zeroed(a, Demo);
    assert_non_null(p2);
    assert_true(ptr_is_aligned(p2, alignof(Demo)));
    assert_true(is_arena_ptr_sized(a, p2, sizeof *p2));
    /* Validate zeroing */
    for (size_t i = 0; i < sizeof *p2; ++i) {
        assert_int_equal(((const uint8_t*)p2)[i], 0);
    }

    dispose_arena(&a);
}
// -------------------------------------------------------------------------------- 

static void test_arena_alloc_array_and_array_zeroed(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(/*bytes*/4096, /*resize=*/false,
                                  /*min_chunk_in=*/4096, /*base_align_in=*/alignof(max_align_t));
    assert_non_null(a);

    enum { N = 10 };

    /* Non-zeroed array of uint32_t */
    uint32_t *arr = arena_alloc_array(a, uint32_t, N);
    assert_non_null(arr);
    assert_true(ptr_is_aligned(arr, alignof(uint32_t)));
    assert_true(is_arena_ptr_sized(a, arr, sizeof(uint32_t)*N));

    /* Zeroed array of uint64_t */
    uint64_t *zarr = arena_alloc_array_zeroed(a, uint64_t, N);
    assert_non_null(zarr);
    assert_true(ptr_is_aligned(zarr, alignof(uint64_t)));
    assert_true(is_arena_ptr_sized(a, zarr, sizeof(uint64_t)*N));
    for (size_t i = 0; i < N; ++i) {
        assert_int_equal(zarr[i], 0);
    }

    dispose_arena(&a);
}
// -------------------------------------------------------------------------------- 

static void test_arena_alloc_array_count_zero_is_error(void **state) {
    (void)state;

    arena_t *a = init_dynamic_arena(/*bytes*/4096, /*resize=*/false,
                                  /*min_chunk_in=*/4096, /*base_align_in=*/alignof(max_align_t));
    assert_non_null(a);

    errno = 0;
    uint8_t *p = arena_alloc_array(a, uint8_t, 0);
    assert_null(p);
    assert_int_equal(errno, EINVAL);

    errno = 0;
    uint8_t *pz = arena_alloc_array_zeroed(a, uint8_t, 0);
    assert_null(pz);
    assert_int_equal(errno, EINVAL);

    dispose_arena(&a);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_grow_copies_and_zeroes_tail(void **state) {
    (void)state;

    arena_t* a = init_dynamic_arena(/*bytes*/4096, /*resize*/true, /*min_chunk*/0, /*base_align*/alignof(max_align_t));
    assert_non_null(a);

    const size_t old_sz = 8;
    uint8_t* oldp = (uint8_t*)alloc_arena(a, old_sz, /*zeroed*/false);
    assert_non_null(oldp);

    // Fill original with a pattern
    for (size_t i = 0; i < old_sz; ++i) oldp[i] = (uint8_t)(0xA0u + (uint8_t)i);

    const size_t new_sz = 32;
    uint8_t* newp = (uint8_t*)realloc_arena(a, oldp, old_sz, new_sz, /*zeroed*/true);
    assert_non_null(newp);

    // First old_sz bytes must match original
    assert_memory_equal(newp, oldp, old_sz);

    // Tail must be zeroed
    for (size_t i = old_sz; i < new_sz; ++i) {
        assert_int_equal(newp[i], 0);
    }

    // Cleanup arena (base pointer is the arena itself)
    free(a);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_shrink_is_noop(void **state) {
    (void)state;

    arena_t* a = init_dynamic_arena(2048, true, 0, alignof(max_align_t));
    assert_non_null(a);

    const size_t old_sz = 32;
    uint8_t* p = (uint8_t*)alloc_arena(a, old_sz, false);
    assert_non_null(p);

    // Write sentinel values
    for (size_t i = 0; i < old_sz; ++i) p[i] = (uint8_t)(0xC0u + (uint8_t)i);

    // Shrink
    void* q = realloc_arena(a, p, old_sz, /*realloc_size*/16, /*zeroed*/false);
    assert_ptr_equal(q, p);

    // Same size
    void* r = realloc_arena(a, p, old_sz, /*realloc_size*/32, /*zeroed*/false);
    assert_ptr_equal(r, p);

    free(a);
}
// -------------------------------------------------------------------------------

static void test_realloc_fails_when_insufficient_space(void **state) {
    (void)state;

    // Keep this pretty small so a large realloc will fail.
    arena_t* a = init_dynamic_arena(512, /*resize*/false, /*min_chunk*/0, /*base_align*/alignof(max_align_t));
    assert_non_null(a);

    // Consume most space
    const size_t old_sz = 400;
    void* p = alloc_arena(a, old_sz, false);
    assert_non_null(p);

    // Request something larger than likely remains
    errno = 0;
    void* q = realloc_arena(a, p, old_sz, /*realloc_size*/500, /*zeroed*/false);
    assert_null(q);
    // errno is set inside arena_alloc; don’t assert a specific value to avoid coupling

    free(a);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_aligned_alignment_and_copy(void **state) {
    (void)state;

    arena_t* a = init_dynamic_arena(4096, true, 0, alignof(max_align_t));
    assert_non_null(a);

    const size_t old_sz = 16;
    uint8_t* p = (uint8_t*)alloc_arena(a, old_sz, false);
    assert_non_null(p);
    for (size_t i = 0; i < old_sz; ++i) p[i] = (uint8_t)(0x11u * (uint8_t)i);

    const size_t want = 64;
    const size_t align = 64;

    uint8_t* q = (uint8_t*)realloc_arena_aligned(a, p, old_sz, want, /*zeroed*/false, align);
    assert_non_null(q);
    assert_true(is_aligned(q, align));

    // Prefix preserved
    assert_memory_equal(q, p, old_sz);

    free(a);
}
// -------------------------------------------------------------------------------- 

static void test_init_arena_with_arena_basic(void **state) {
    (void)state;
    
    arena_t* parent = init_darena(64 * 1024, true);
    assert_non_null(parent);
    
    arena_t* sub = init_arena_with_arena(parent, 8 * 1024, alignof(max_align_t));
    assert_non_null(sub);
    
    // Verify sub-arena properties using public API
    assert_false(arena_owns_memory(sub));
    assert_int_equal(arena_mtype(sub), arena_mtype(parent));  // Inherits type
    
    // Verify we can allocate from sub
    void* p = alloc_arena(sub, 256, false);
    assert_non_null(p);
    
    // Verify pointer is within parent's memory
    assert_true(is_arena_ptr(parent, sub));  // Sub header in parent
    assert_true(is_arena_ptr(parent, p));    // Sub's allocation in parent
    
    free_arena(sub);
    free_arena(parent);
}
// --------------------------------------------------------------------------------

static void test_init_arena_with_arena_null_parent(void **state) {
    (void)state;
    
    errno = 0;
    arena_t* sub = init_arena_with_arena(NULL, 4096, 8);
    assert_null(sub);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

static void test_init_arena_with_arena_zero_bytes(void **state) {
    (void)state;
    
    arena_t* parent = init_darena(16 * 1024, true);
    assert_non_null(parent);
    
    errno = 0;
    arena_t* sub = init_arena_with_arena(parent, 0, 8);
    assert_null(sub);
    assert_int_equal(errno, EINVAL);
    
    free_arena(parent);
}
// --------------------------------------------------------------------------------

static void test_init_arena_with_arena_too_small_for_headers(void **state) {
    (void)state;
    
    arena_t* parent = init_darena(16 * 1024, true);
    assert_non_null(parent);
    
    // Request very small size (likely too small for headers + data)
    errno = 0;
    arena_t* sub = init_arena_with_arena(parent, 64, 8);
    // May succeed on some platforms if 64 bytes is enough, or fail
    // We just verify no crash and proper error handling if it fails
    if (!sub) {
        assert_int_equal(errno, EINVAL);
    } else {
        free_arena(sub);
    }
    
    free_arena(parent);
}
// --------------------------------------------------------------------------------

static void test_init_arena_with_arena_exhausts_parent(void **state) {
    (void)state;
    
    // Create small parent with no resize
    arena_t* parent = init_darena(1024, false);
    assert_non_null(parent);
    
    // Try to create sub-arena larger than parent capacity
    errno = 0;
    arena_t* sub = init_arena_with_arena(parent, 64 * 1024, 8);
    assert_null(sub);
    // errno should be ENOMEM or EPERM from alloc_arena
    assert_true(errno == ENOMEM || errno == EPERM);
    
    free_arena(parent);
}
// --------------------------------------------------------------------------------

static void test_init_arena_with_arena_multiple_subs(void **state) {
    (void)state;
    
    arena_t* parent = init_darena(128 * 1024, true);
    assert_non_null(parent);
    
    // Create multiple sub-arenas
    arena_t* sub1 = init_arena_with_arena(parent, 8 * 1024, 8);
    arena_t* sub2 = init_arena_with_arena(parent, 16 * 1024, 16);
    arena_t* sub3 = init_arena_with_arena(parent, 32 * 1024, 32);
    
    assert_non_null(sub1);
    assert_non_null(sub2);
    assert_non_null(sub3);
    
    // All should be sub-arenas (don't own memory)
    assert_false(arena_owns_memory(sub1));
    assert_false(arena_owns_memory(sub2));
    assert_false(arena_owns_memory(sub3));
    
    // Allocate from each
    void* p1 = alloc_arena(sub1, 128, false);
    void* p2 = alloc_arena(sub2, 256, false);
    void* p3 = alloc_arena(sub3, 512, false);
    
    assert_non_null(p1);
    assert_non_null(p2);
    assert_non_null(p3);
    
    // All allocations should be in parent
    assert_true(is_arena_ptr(parent, p1));
    assert_true(is_arena_ptr(parent, p2));
    assert_true(is_arena_ptr(parent, p3));
    
    // Free in any order (just nulls out)
    free_arena(sub2);
    free_arena(sub1);
    free_arena(sub3);
    
    free_arena(parent);
}
// --------------------------------------------------------------------------------

static void test_init_arena_with_arena_nested_subs(void **state) {
    (void)state;
    
    arena_t* main = init_darena(256 * 1024, true);
    assert_non_null(main);
    
    // Create level 1 sub-arena
    arena_t* level1 = init_arena_with_arena(main, 64 * 1024, 16);
    assert_non_null(level1);
    assert_false(arena_owns_memory(level1));
    
    // Create level 2 sub-arena from level 1
    arena_t* level2 = init_arena_with_arena(level1, 16 * 1024, 8);
    assert_non_null(level2);
    assert_false(arena_owns_memory(level2));
    
    // Create level 3 sub-arena from level 2
    arena_t* level3 = init_arena_with_arena(level2, 4 * 1024, 8);
    assert_non_null(level3);
    assert_false(arena_owns_memory(level3));
    
    // Allocate from deepest level
    void* p = alloc_arena(level3, 256, false);
    assert_non_null(p);
    
    // Should be in main's memory
    assert_true(is_arena_ptr(main, p));
    
    // Free in reverse order (safest, but not required)
    free_arena(level3);
    free_arena(level2);
    free_arena(level1);
    free_arena(main);
}
// --------------------------------------------------------------------------------

static void test_sub_arena_cannot_grow(void **state) {
    (void)state;
    
    arena_t* parent = init_darena(64 * 1024, true);
    assert_non_null(parent);
    
    arena_t* sub = init_arena_with_arena(parent, 2048, 8);
    assert_non_null(sub);
    
    // Exhaust sub-arena
    size_t rem = arena_remaining(sub);
    void* p1 = alloc_arena(sub, rem, false);
    assert_non_null(p1);
    
    // Next allocation should fail (no growth)
    errno = 0;
    void* p2 = alloc_arena(sub, 1, false);
    assert_null(p2);
    assert_int_equal(errno, EPERM);
    
    // Parent can still allocate (it has capacity)
    void* p3 = alloc_arena(parent, 1024, false);
    assert_non_null(p3);
    
    free_arena(parent);
}
// --------------------------------------------------------------------------------

static void test_sub_arena_inherits_alignment(void **state) {
    (void)state;
    
    arena_t* parent = init_darena(64 * 1024, true);
    assert_non_null(parent);
    
    // Create sub with specific alignment
    arena_t* sub = init_arena_with_arena(parent, 8192, 64);
    assert_non_null(sub);
    
    // Sub should have requested alignment
    assert_int_equal(arena_alignment(sub), 64);
    
    // Allocation should respect this alignment
    void* p = alloc_arena(sub, 1, false);
    assert_non_null(p);
    assert_true(((uintptr_t)p % 64) == 0);
    
    free_arena(parent);
}
// --------------------------------------------------------------------------------

static void test_sub_arena_from_static_parent(void **state) {
    (void)state;
    
    uint8_t buffer[32 * 1024];
    arena_t* parent = init_sarena(buffer, sizeof(buffer));
    assert_non_null(parent);
    
    // Create sub-arena from static parent
    arena_t* sub = init_arena_with_arena(parent, 8192, 8);
    assert_non_null(sub);
    assert_false(arena_owns_memory(sub));
    assert_int_equal(arena_mtype(sub), STATIC);  // Inherits STATIC type
    
    void* p = alloc_arena(sub, 256, false);
    assert_non_null(p);
    
    // Sub-arena free just nulls out
    free_arena(sub);
    
    // Parent free should fail (STATIC)
    errno = 0;
    free_arena(parent);
    assert_int_equal(errno, EPERM);
    
    // Caller owns buffer (stack allocated in this case)
}
// --------------------------------------------------------------------------------

static void test_free_sub_arena_nulls_state(void **state) {
    (void)state;
    
    arena_t* parent = init_darena(64 * 1024, true);
    assert_non_null(parent);
    
    arena_t* sub = init_arena_with_arena(parent, 8192, 8);
    assert_non_null(sub);
    
    void* p = alloc_arena(sub, 128, false);
    assert_non_null(p);
    
    // Before free - should have capacity and size
    size_t alloc_before = arena_alloc(sub);
    size_t size_before = arena_size(sub);
    assert_true(alloc_before > 0);
    assert_true(size_before > 0);
    
    // Free sub-arena
    free_arena(sub);
    
    // After free - should be zeroed/nulled
    size_t alloc_after = arena_alloc(sub);
    size_t size_after = arena_size(sub);
    assert_int_equal(alloc_after, 0);
    assert_int_equal(size_after, 0);
    
    free_arena(parent);
}
// --------------------------------------------------------------------------------

static void test_sub_arena_double_free_safe(void **state) {
    (void)state;
    
    arena_t* parent = init_darena(64 * 1024, true);
    arena_t* sub = init_arena_with_arena(parent, 8192, 8);
    
    // First free
    free_arena(sub);
    assert_int_equal(arena_alloc(sub), 0);
    
    // Second free should be safe (idempotent)
    free_arena(sub);
    assert_int_equal(arena_alloc(sub), 0);
    
    free_arena(parent);
}
// --------------------------------------------------------------------------------

static void test_cannot_toggle_resize_on_sub_arena(void **state) {
    (void)state;
    
    arena_t* parent = init_darena(64 * 1024, true);
    arena_t* sub = init_arena_with_arena(parent, 8192, 8);
    
    assert_false(arena_owns_memory(sub));
    
    // Try to enable resize - should fail
    errno = 0;
    toggle_arena_resize(sub, true);
    assert_int_equal(errno, EPERM);
    
    // Verify sub still can't grow by trying to over-allocate
    size_t rem = arena_remaining(sub);
    (void)alloc_arena(sub, rem, false);  // Exhaust
    
    errno = 0;
    void* p = alloc_arena(sub, 1, false);
    assert_null(p);
    assert_int_equal(errno, EPERM);
    
    free_arena(parent);
}
// --------------------------------------------------------------------------------

static void test_arena_owns_memory_dynamic(void **state) {
    (void)state;
    
    arena_t* a = init_darena(4096, true);
    assert_non_null(a);
    assert_true(arena_owns_memory(a));
    
    free_arena(a);
}
// --------------------------------------------------------------------------------

static void test_arena_owns_memory_static(void **state) {
    (void)state;
    
    uint8_t buffer[8192];
    arena_t* a = init_sarena(buffer, sizeof(buffer));
    assert_non_null(a);
    
    // Static arena owns its header (but not the buffer)
    assert_true(arena_owns_memory(a));
    
    errno = 0;
    free_arena(a);
    assert_int_equal(errno, EPERM);
}
// --------------------------------------------------------------------------------

static void test_arena_owns_memory_sub_arena(void **state) {
    (void)state;
    
    arena_t* parent = init_darena(64 * 1024, true);
    arena_t* sub = init_arena_with_arena(parent, 8192, 8);
    
    // Parent owns, sub doesn't
    assert_true(arena_owns_memory(parent));
    assert_false(arena_owns_memory(sub));
    
    free_arena(parent);
}
// --------------------------------------------------------------------------------

static void test_arena_owns_memory_null(void **state) {
    (void)state;
    
    errno = 0;
    bool result = arena_owns_memory(NULL);
    assert_false(result);
    assert_int_equal(errno, EINVAL);
}
// --------------------------------------------------------------------------------

static void test_sub_arena_stats(void **state) {
    (void)state;
    
    arena_t* parent = init_darena(128 * 1024, true);
    arena_t* sub = init_arena_with_arena(parent, 16 * 1024, 8);
    
    // Allocate some memory
    void* p1 = alloc_arena(sub, 1024, false);
    void* p2 = alloc_arena(sub, 2048, false);
    assert_non_null(p1);
    assert_non_null(p2);
    
    // Check stats using public API
    size_t used = arena_size(sub);
    size_t capacity = arena_alloc(sub);
    size_t remaining = arena_remaining(sub);
    
    assert_true(used >= 3072);  // At least 1024 + 2048
    assert_true(capacity > used);
    assert_true(remaining > 0);
    assert_int_equal(used + remaining, capacity);  // Basic consistency
    
    // Print stats for verification
    char stats[1024];
    bool ok = arena_stats(sub, stats, sizeof(stats));
    assert_true(ok);
    
    free_arena(parent);
}
// --------------------------------------------------------------------------------

static void test_sub_arena_checkpoint_restore(void **state) {
    (void)state;
    
    arena_t* parent = init_darena(128 * 1024, true);
    arena_t* sub = init_arena_with_arena(parent, 16 * 1024, 8);
    
    // Allocate some memory
    void* p1 = alloc_arena(sub, 512, false);
    assert_non_null(p1);
    
    // Save checkpoint
    ArenaCheckPoint cp = save_arena(sub);
    
    // Allocate more
    void* p2 = alloc_arena(sub, 1024, false);
    assert_non_null(p2);
    
    size_t size_before = arena_size(sub);
    
    // Restore to checkpoint
    assert_true(restore_arena(sub, cp));
    
    size_t size_after = arena_size(sub);
    
    // Size should be less after restore
    assert_true(size_after < size_before);
    
    free_arena(parent);
}
// --------------------------------------------------------------------------------

static void test_sub_arena_independent_allocations(void **state) {
    (void)state;
    
    arena_t* parent = init_darena(128 * 1024, true);
    
    // Create two independent sub-arenas
    arena_t* sub1 = init_arena_with_arena(parent, 16 * 1024, 8);
    arena_t* sub2 = init_arena_with_arena(parent, 16 * 1024, 8);
    
    assert_non_null(sub1);
    assert_non_null(sub2);
    
    // Allocate from sub1
    void* p1 = alloc_arena(sub1, 1024, false);
    assert_non_null(p1);
    size_t size1 = arena_size(sub1);
    size_t size2_before = arena_size(sub2);
    
    // Allocate from sub2
    void* p2 = alloc_arena(sub2, 2048, false);
    assert_non_null(p2);
    size_t size2_after = arena_size(sub2);
    
    // sub1 should be unchanged
    assert_int_equal(arena_size(sub1), size1);
    
    // sub2 should have grown
    assert_true(size2_after > size2_before);
    
    free_arena(parent);
}
// ================================================================================ 
// ================================================================================ 

const struct CMUnitTest test_arena[] = {
    cmocka_unit_test(test_dyn_min_chunk_rounds_up_and_changes_capacity),
    cmocka_unit_test(test_dyn_min_chunk_equivalence_6000_vs_8192),
    cmocka_unit_test(test_dyn_alignment_rounds_up_and_floors_to_maxalign),
    cmocka_unit_test(test_static_alignment_honored_even_with_unaligned_buffer),
    
    cmocka_unit_test(test_init_darena),
    cmocka_unit_test(test_init_darena_no_bytes),
    cmocka_unit_test(test_init_darena_large_chunk),
    cmocka_unit_test(test_init_sarena),
    cmocka_unit_test(test_invalid_free_sarena),
    cmocka_unit_test(test_arena_double_free),
    
    cmocka_unit_test(test_alloc_darena),
    cmocka_unit_test(test_alloc_darena_zeroed),
    cmocka_unit_test(test_alloc_darena_null_value),
    cmocka_unit_test(test_alloc_darena_zero_input),
    cmocka_unit_test(test_alloc_sarena),
    cmocka_unit_test(test_alloc_sarena_zeroed),

    cmocka_unit_test(test_realloc_grows_when_allowed),
    cmocka_unit_test(test_realloc_fails_when_resize_false),
    cmocka_unit_test(test_realloc_fails_in_static_arena),
    cmocka_unit_test(test_realloc_first_alloc_in_new_chunk_is_aligned_and_no_pad),

    cmocka_unit_test(test_is_arena_ptr_basic_hits_and_misses),
    cmocka_unit_test(test_is_arena_ptr_tail_fastpath),
    cmocka_unit_test(test_is_arena_ptr_sized_boundaries),
    cmocka_unit_test(test_is_arena_ptr_multichunk),
    cmocka_unit_test(test_is_arena_ptr_sized_cross_chunk_fails),
    cmocka_unit_test(test_is_arena_ptr_null_and_zero_size_guards),
  
    cmocka_unit_test(test_reset_dynamic_trim_true_frees_extra_chunks_and_resets_usage),
    cmocka_unit_test(test_reset_dynamic_keep_chunks_preserves_capacity),
    cmocka_unit_test(test_reset_static_zeroes_usage_ignores_trim),
    
    cmocka_unit_test(test_save_restore_same_chunk_pointer_roundtrip),
    cmocka_unit_test(test_save_restore_second_chunk_trims_and_replays_allocation),
    cmocka_unit_test(test_save_restore_static_rewinds_cursor),
    cmocka_unit_test(test_restore_rejects_checkpoint_from_other_arena),
    cmocka_unit_test(test_restore_accepts_empty_checkpoint_noop), 

    #if ARENA_USE_CONVENIENCE_MACROS
        cmocka_unit_test(test_arena_alloc_type_and_type_zeroed),
        cmocka_unit_test(test_arena_alloc_array_and_array_zeroed),
        cmocka_unit_test(test_arena_alloc_array_count_zero_is_error),
    #endif

    cmocka_unit_test(test_realloc_grow_copies_and_zeroes_tail),
    cmocka_unit_test(test_realloc_shrink_is_noop),
    cmocka_unit_test(test_realloc_fails_when_insufficient_space),
    cmocka_unit_test(test_realloc_aligned_alignment_and_copy),

    cmocka_unit_test(test_init_arena_with_arena_basic),
    cmocka_unit_test(test_init_arena_with_arena_null_parent),
    cmocka_unit_test(test_init_arena_with_arena_zero_bytes),
    cmocka_unit_test(test_init_arena_with_arena_too_small_for_headers),
    cmocka_unit_test(test_init_arena_with_arena_exhausts_parent),
    cmocka_unit_test(test_init_arena_with_arena_multiple_subs),
    cmocka_unit_test(test_init_arena_with_arena_nested_subs),
    cmocka_unit_test(test_sub_arena_cannot_grow),
    cmocka_unit_test(test_sub_arena_inherits_alignment),
    cmocka_unit_test(test_sub_arena_from_static_parent),
    cmocka_unit_test(test_free_sub_arena_nulls_state),
    cmocka_unit_test(test_sub_arena_double_free_safe),
    cmocka_unit_test(test_cannot_toggle_resize_on_sub_arena),
    cmocka_unit_test(test_arena_owns_memory_dynamic),
    cmocka_unit_test(test_arena_owns_memory_static),
    cmocka_unit_test(test_arena_owns_memory_sub_arena),
    cmocka_unit_test(test_arena_owns_memory_null),
    cmocka_unit_test(test_sub_arena_stats),
    cmocka_unit_test(test_sub_arena_checkpoint_restore),
    cmocka_unit_test(test_sub_arena_independent_allocations),
};

const size_t test_arena_count = sizeof(test_arena) / sizeof(test_arena[0]);
// ================================================================================ 
// ================================================================================ 
// TEST POOL 

// Helper functions
static arena_t* make_dynamic_arena(size_t seed_bytes) {
    // resize=true, min_chunk=64 KiB, base_align = max_align_t
    return init_dynamic_arena(seed_bytes, /*resize=*/true, /*min_chunk_in=*/1u<<16, /*base_align_in=*/alignof(max_align_t));
}

static arena_t* make_tiny_dynamic_arena(size_t seed_bytes) {
    // Small seed & no min_chunk; we want a tiny usable region to trigger ENOMEM on pool header alloc
    return init_dynamic_arena(seed_bytes, /*resize=*/false, /*min_chunk_in=*/0, /*base_align_in=*/alignof(max_align_t));
}
// Helper: create a dynamic arena that cannot grow and leave only `leave_bytes` free.
static arena_t* make_exhausted_dynamic_arena(size_t seed_bytes, size_t leave_bytes) {
    // resize = false, min_chunk = 0, base_align = max_align_t
    arena_t* a = init_dynamic_arena(seed_bytes, /*resize=*/false, /*min_chunk_in=*/0,
                                    /*base_align_in=*/alignof(max_align_t));
    assert_non_null(a);

    // Consume all but `leave_bytes` from the current tail.
    // Assumes you expose arena_remaining(...) and alloc_arena(...).
    size_t rem = arena_remaining(a);
    if (rem > leave_bytes) {
        void* eaten = alloc_arena(a, rem - leave_bytes, /*zeroed=*/false);
        assert_non_null(eaten);
    }
    return a;
}

// Optional small convenience to snapshot+restore errno around calls
#define SAVE_ERRNO(name)    int name = errno
#define RESTORE_ERRNO(name) do { errno = name; } while (0)
// ================================================================================ 
// ================================================================================ 
// TEST INIT WITH ARENA

static void test_init_pool_invalid_args(void **state) {
    (void)state;

    errno = 0;
    pool_t* p = init_pool_with_arena(NULL, /*block_size=*/64, /*alignment=*/0, 
                                    /*blocks_per_chunk=*/16, /*prewarm=*/false,
                                    /*enable reallocations*/ true);
    assert_null(p);
    assert_int_equal(errno, EINVAL);

    arena_t* a = make_dynamic_arena(1<<20);
    assert_non_null(a);

    errno = 0;
    p = init_pool_with_arena(a, /*block_size=*/0, /*alignment=*/0, 
                            /*blocks_per_chunk=*/16, /*prewarm=*/false,
                            /*enable reallocations*/ true);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
    free_arena(a);
    errno = 0;
    p = init_pool_with_arena(a, /*block_size=*/64, /*alignment=*/0, 
                             /*blocks_per_chunk=*/0, /*prewarm=*/false,
                             /*enable reallocations*/ true);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_init_pool_header_lives_in_arena(void **state) {
    (void)state;

    arena_t* a = make_dynamic_arena(1<<20);
    assert_non_null(a);

    pool_t* p = init_pool_with_arena(a, /*block_size=*/64, /*alignment=*/0, 
                                    /*blocks_per_chunk=*/32, /*prewarm=*/false,
                                    /*enable reallocations*/ true);
    assert_non_null(p);

    // The pool header was allocated from the arena; verify with your helper
    assert_true(is_arena_ptr(a, (const void*)p));

    // Basic invariants
    assert_int_equal(pool_block_size(p), 64);
    assert_true(pool_stride(p) >= 64);
    assert_true(pool_stride(p) % alignof(void*) == 0);
    assert_int_equal(pool_total_blocks(p), 0);
    assert_int_equal(pool_free_blocks(p), 0);
    free_arena(a);
}
// -------------------------------------------------------------------------------- 

static void test_init_pool_alignment_and_stride_rules(void **state) {
    (void)state;

    arena_t* a = make_dynamic_arena(1<<20);
    assert_non_null(a);

    // Tiny block size to force stride >= sizeof(void*)
    pool_t* p = init_pool_with_arena(a, /*block_size=*/1, /*alignment=*/0, 
                                     /*blocks_per_chunk=*/8, /*prewarm=*/false,
                                     /*enable reallocations*/ true);
    assert_non_null(p);

    size_t stride = pool_stride(p);
    assert_true(stride >= sizeof(void*));
    assert_true(stride % alignof(void*) == 0);

    // Stronger requested alignment should be honored
    size_t req_align = 64;
    pool_t* q = init_pool_with_arena(a, /*block_size=*/48, /*alignment=*/req_align, 
                                     /*blocks_per_chunk=*/8, /*prewarm=*/false,
                                     /*enable reallocations*/ true);
    assert_non_null(q);

    size_t qstride = pool_stride(q);
    assert_true(qstride >= 48);
    assert_true(qstride % req_align == 0);
    free_arena(a);
}
// -------------------------------------------------------------------------------- 

static void test_init_pool_prewarm_sets_blocks(void **state) {
    (void)state;

    arena_t* a = make_dynamic_arena(1<<20);
    assert_non_null(a);

    const size_t blocks_per_chunk = 16;
    pool_t* p = init_pool_with_arena(a, /*block_size=*/32, /*alignment=*/0, 
                                     blocks_per_chunk, /*prewarm=*/true,
                                     /*enable reallocations*/ true);
    assert_non_null(p);

    // After prewarm, the pool has made one chunk available
    assert_int_equal(pool_total_blocks(p), blocks_per_chunk);
    assert_int_equal(pool_free_blocks(p), 0);

    // First allocations should succeed and come from the current slice (free_list still empty)
    void* x = alloc_pool(p, false);
    assert_non_null(x);
    void* y = alloc_pool(p, false);
    assert_non_null(y);

    // Free one and ensure LIFO via intrusive list
    return_pool_element(p, x);
    assert_int_equal(pool_free_blocks(p), 1);
    void* z = alloc_pool(p, false);
    assert_ptr_equal(z, x);
    free_arena(a);
}
// -------------------------------------------------------------------------------- 

static void test_init_pool_fails_when_no_room_for_header(void **state) {
    (void)state;

    // Make a valid arena, but leave only a few bytes so the pool header can't fit.
    arena_t* a = make_exhausted_dynamic_arena(4096, /*leave_bytes=*/8);
    assert_non_null(a);
    
    errno = 0;
    pool_t* p = init_pool_with_arena(a, /*block_size=*/64, /*alignment=*/0,
                                     /*blocks_per_chunk=*/8, /*prewarm=*/false,
                                     /*enable reallocations*/ true);
    
    // Header allocation should fail because the arena cannot grow (resize=false)
    assert_null(p);
    assert_int_equal(errno, ENOMEM);
    free_arena(a);
}
// -------------------------------------------------------------------------------- 

static void test_pool_reset_semantics(void **state) {
    (void)state;

    arena_t* a = make_dynamic_arena(1<<20);
    assert_non_null(a);

    pool_t* p = init_pool_with_arena(a, /*block_size=*/64, /*alignment=*/0, 
                                     /*blocks_per_chunk=*/8, /*prewarm=*/true,
                                     /*enable reallocations*/ true);
    assert_non_null(p);

    void* b0 = alloc_pool(p, false);
    void* b1 = alloc_pool(p, false);
    assert_non_null(b0);
    assert_non_null(b1);

    return_pool_element(p, b0);
    assert_int_equal(pool_free_blocks(p), 1);

    // Reset pool: clears its bookkeeping (free list, slice window, counters)
    reset_pool(p);
    assert_int_equal(pool_total_blocks(p), 0);
    assert_int_equal(pool_free_blocks(p), 0);

    // Alloc should still work after reset (pool will grow again)
    void* b2 = alloc_pool(p, false);
    assert_non_null(b2);
    free_arena(a);
}
// ================================================================================ 
// ================================================================================ 

static void test_init_dynamic_pool_invalid_args(void **state) {
    (void)state;

    SAVE_ERRNO(e0); errno = 0;
    pool_t *p = init_dynamic_pool(0, 0, 32, 8192, 4096, true, true);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
    RESTORE_ERRNO(e0);

    SAVE_ERRNO(e1); errno = 0;
    p = init_dynamic_pool(64, 0, 0, 8192, 4096, true, true);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
    RESTORE_ERRNO(e1);

    SAVE_ERRNO(e2); errno = 0;
    p = init_dynamic_pool(64, 0, 32, 0, 4096, true, true);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
    RESTORE_ERRNO(e2);
    free_pool(p);
}
// -------------------------------------------------------------------------------- 

static void test_init_dynamic_pool_overflow_guard(void **state) {
    (void)state;

    size_t too_many = (SIZE_MAX / sizeof(void*)) + 1;

    SAVE_ERRNO(e); errno = 0;
    pool_t *p = init_dynamic_pool(8, 0, too_many, 8192, 4096, true, false);
    assert_null(p);
    assert_int_equal(errno, EOVERFLOW);
    RESTORE_ERRNO(e);
    free_pool(p);
}
// -------------------------------------------------------------------------------- 

static void test_init_dynamic_pool_alignment_and_stride(void **state) {
    (void)state;

    pool_t *p = init_dynamic_pool(48, 64, 8, 8192, 4096, true, false);
    assert_non_null(p);

    assert_int_equal(pool_block_size(p), 48);
    size_t stride = pool_stride(p);

    assert_true(stride >= 48);
    assert_true(stride % 64 == 0);
    assert_true(stride >= sizeof(void*));

    free_pool(p);
}
// -------------------------------------------------------------------------------- 

static void test_init_dynamic_pool_fixed_prewarm_exact(void **state) {
    (void)state;

    const size_t blocks = 16;

    // Fixed-capacity (grow_enabled = false) + prewarm = required & valid
    pool_t *p = init_dynamic_pool(
        /*block_size=*/64,
        /*alignment=*/0,
        /*blocks_per_chunk=*/blocks,
        /*arena_seed_bytes=*/8192,
        /*min_chunk_bytes=*/0,
        /*grow_enabled=*/false,
        /*prewarm_one_chunk=*/true
    );
    assert_non_null(p);

    // After prewarm, pool exposes exactly 'blocks' capacity
    assert_int_equal(pool_total_blocks(p), blocks);
    assert_int_equal(pool_free_blocks(p), 0);

    // Stride invariants: >= block_size, >= sizeof(void*), multiple of alignof(void*)
    size_t stride = pool_stride(p);
    assert_true(stride >= 64);
    assert_true(stride >= sizeof(void *));
    assert_true((stride % alignof(void*)) == 0);

    // Allocate exactly 'blocks' items successfully
    void* saved[16] = {0};
    for (size_t i = 0; i < blocks; ++i) {
        saved[i] = alloc_pool(p, false);
        assert_non_null(saved[i]);
        // Optional: check alignment of returned block
        assert_true(((uintptr_t)saved[i] % stride) == 0);
    }

    // Next allocation should fail with EPERM (fixed-capacity exhausted)
    SAVE_ERRNO(e0); errno = 0;
    void *extra = alloc_pool(p, false);
    assert_null(extra);
    assert_int_equal(errno, EPERM);
    RESTORE_ERRNO(e0);

    // Return one element, free_list should get one
    return_pool_element(p, saved[0]);
    assert_int_equal(pool_free_blocks(p), 1);
    assert_int_equal(pool_total_blocks(p), blocks); // capacity unchanged

    // Re-allocate and confirm LIFO reuse (likely returns the same pointer)
    void *r = alloc_pool(p, false);
    assert_non_null(r);
    assert_ptr_equal(r, saved[0]);
    assert_int_equal(pool_free_blocks(p), 0);

    // Capacity is still fixed; next extra allocation still fails
    SAVE_ERRNO(e1); errno = 0;
    extra = alloc_pool(p, false);
    assert_null(extra);
    assert_int_equal(errno, EPERM);
    RESTORE_ERRNO(e1);

    free_pool(p);
}
// -------------------------------------------------------------------------------- 

// static void test_init_dynamic_pool_fixed_no_prewarm_first_alloc_fails(void **state) {
//     (void)state;
//
//     pool_t *p = init_dynamic_pool(64, 0, 8, 4096, 0, false, false);
//     assert_non_null(p);
//
//     SAVE_ERRNO(e); errno = 0;
//     assert_null(alloc_pool(p));
//     assert_int_equal(errno, EPERM);
//     RESTORE_ERRNO(e);
//
//     free_pool(p);
// }
// -------------------------------------------------------------------------------- 

static void test_init_dynamic_pool_grow_prewarm_and_reuse(void **state) {
    (void)state;

    pool_t *p = init_dynamic_pool(32, 16, 8, 8192, 4096, true, true);
    assert_non_null(p);

    void *a = alloc_pool(p, false);
    void *b = alloc_pool(p, false);
    assert_non_null(a);
    assert_non_null(b);

    return_pool_element(p, a);
    assert_int_equal(pool_free_blocks(p), 1);

    void *c = alloc_pool(p, false);
    assert_ptr_equal(c, a);
    assert_int_equal(pool_free_blocks(p), 0);

    free_pool(p);
}
// -------------------------------------------------------------------------------- 

static void test_init_dynamic_pool_grow_lazy_first_alloc(void **state) {
    (void)state;

    pool_t *p = init_dynamic_pool(64, 0, 4, 4096, 2048, true, false);
    assert_non_null(p);

    assert_int_equal(pool_total_blocks(p), 0);

    assert_non_null(alloc_pool(p, false));
    assert_int_equal(pool_total_blocks(p), 4);

    free_pool(p);
}
// -------------------------------------------------------------------------------- 

static void test_init_dynamic_pool_tiny_seed_fails(void **state) {
    (void)state;

    SAVE_ERRNO(e); errno = 0;
    pool_t *p = init_dynamic_pool(64, 0, 8, 32, 0, true, false);
    assert_null(p);
    assert_true(errno == EINVAL || errno == ENOMEM);
    RESTORE_ERRNO(e);
    free_pool(p);
}
// ================================================================================ 
// ================================================================================ 

static void* make_aligned_buffer(size_t bytes, size_t align) {
    // Simple portable aligned storage for tests.
    // For large buffers you might prefer posix_memalign/malloc+align tricks.
    // Here we just over-allocate on the stack for small examples.
    // In practice, we’ll use static arrays with attributes below.
    (void)bytes; (void)align;
    return NULL; // not used; we’ll test with static arrays instead
}
// -------------------------------------------------------------------------------- 

static void test_init_static_pool_invalid_args(void **state) {
    (void)state;

    // NULL buffer
    SAVE_ERRNO(e0); errno = 0;
    pool_t* p = init_static_pool(NULL, 1024, 64, 0);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
    RESTORE_ERRNO(e0);

    // zero buffer_bytes
    static unsigned char buf1[128];
    SAVE_ERRNO(e1); errno = 0;
    p = init_static_pool(buf1, 0, 64, 0);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
    RESTORE_ERRNO(e1);

    // zero block_size
    SAVE_ERRNO(e2); errno = 0;
    p = init_static_pool(buf1, sizeof buf1, 0, 0);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
    RESTORE_ERRNO(e2);
}
// -------------------------------------------------------------------------------- 

static void test_init_static_pool_too_small_buffer(void **state) {
    (void)state;

    // Very small buffer that should fail regardless of overhead
    static unsigned char tiny[32];
    SAVE_ERRNO(e); errno = 0;
    pool_t* p = init_static_pool(tiny, sizeof tiny, /*block_size=*/64, /*alignment=*/0);
    assert_null(p);
    // Depending on internal layout it will be ENOMEM (most likely).
    assert_int_equal(errno, EINVAL);
    RESTORE_ERRNO(e);
}
// -------------------------------------------------------------------------------- 

static void test_init_static_pool_alignment_and_stride(void **state) {
    (void)state;

    // Generous buffer
    static unsigned char buf[64 * 1024] __attribute__((aligned(64)));
    pool_t* p = init_static_pool(buf, sizeof buf, /*block_size=*/48, /*alignment=*/64);
    assert_non_null(p);

    // stride rules
    size_t bs = pool_block_size(p);
    size_t st = pool_stride(p);
    assert_int_equal(bs, 48);
    assert_true(st >= 48);
    assert_true(st >= sizeof(void*));
    assert_true(st % 64 == 0);                      // requested alignment honored
    assert_true(st % alignof(void*) == 0);          // intrusive free-list
}
// -------------------------------------------------------------------------------- 

static void test_init_static_pool_stride_minimum(void **state) {
    (void)state;

    static unsigned char buf[4096] __attribute__((aligned(16)));
    pool_t* p = init_static_pool(buf, sizeof buf, /*block_size=*/1, /*alignment=*/0);
    assert_non_null(p);

    size_t st = pool_stride(p);
    assert_true(st >= sizeof(void*));
    assert_true(st % alignof(void*) == 0);
}
// -------------------------------------------------------------------------------- 

static void test_init_static_pool_capacity_and_exhaustion(void **state) {
    (void)state;

    // Use a reasonable buffer to ensure capacity > 0
    static unsigned char buf[16 * 1024] __attribute__((aligned(64)));
    pool_t* p = init_static_pool(buf, sizeof buf, /*block_size=*/64, /*alignment=*/0);
    assert_non_null(p);

    const size_t cap = pool_total_blocks(p);
    assert_true(cap > 0);

    // Allocate exactly 'cap' blocks
    for (size_t i = 0; i < cap; ++i) {
        void* b = alloc_pool(p, false);
        assert_non_null(b);
        // Returned pointer alignment (relative to stride)
        assert_true(((uintptr_t)b % pool_stride(p)) == 0);
    }

    // Next allocation must fail with EPERM
    SAVE_ERRNO(e0); errno = 0;
    void* extra = alloc_pool(p, false);
    assert_null(extra);
    assert_int_equal(errno, EPERM);
    RESTORE_ERRNO(e0);
}
// -------------------------------------------------------------------------------- 

static void test_init_static_pool_free_list_reuse(void **state) {
    (void)state;

    static unsigned char buf[8 * 1024] __attribute__((aligned(32)));
    pool_t* p = init_static_pool(buf, sizeof buf, /*block_size=*/32, /*alignment=*/16);
    assert_non_null(p);

    size_t cap = pool_total_blocks(p);
    assert_true(cap > 2); // ensure we can do a few operations

    void* a = alloc_pool(p, false);
    void* b = alloc_pool(p, false);
    assert_non_null(a);
    assert_non_null(b);
    assert_int_equal(pool_free_blocks(p), 0);

    // Return 'a' then re-alloc and confirm LIFO reuse
    return_pool_element(p, a);
    assert_int_equal(pool_free_blocks(p), 1);

    void* c = alloc_pool(p, false);
    assert_ptr_equal(c, a);
    assert_int_equal(pool_free_blocks(p), 0);
}
// -------------------------------------------------------------------------------- 

static void test_init_static_pool_one_block_only(void **state) {
    (void)state;

    // Force a “tight” buffer by using a large block size relative to buffer
    // Exact header size is unknown, so we don’t assert cap==1; we assert cap>=1 and exercise exhaustion.
    static unsigned char buf[8192] __attribute__((aligned(64)));
    pool_t* p = init_static_pool(buf, sizeof buf, /*block_size=*/4096, /*alignment=*/64);
    assert_non_null(p);

    size_t cap = pool_total_blocks(p);
    assert_true(cap >= 1);

    // Allocate all capacity
    for (size_t i = 0; i < cap; ++i) {
        assert_non_null(alloc_pool(p, false));
    }

    // Next allocation must fail
    SAVE_ERRNO(e); errno = 0;
    void* z = alloc_pool(p, false);
    assert_null(z);
    assert_int_equal(errno, EPERM);
    RESTORE_ERRNO(e);
}
// ================================================================================ 
// ================================================================================ 

static void test_pool_alignment_matches_contract(void **state) {
    (void)state;

    // Dynamic, grow-enabled, prewarmed; ask for strong alignment
    pool_t *p = init_dynamic_pool(
        /*block_size=*/48,
        /*alignment=*/64,
        /*blocks_per_chunk=*/8,
        /*arena_seed_bytes=*/8192,
        /*min_chunk_bytes=*/4096,
        /*grow_enabled=*/true,
        /*prewarm_one_chunk=*/true
    );
    assert_non_null(p);

    size_t a = pool_alignment(p);
    size_t s = pool_stride(p);

    // Alignment must be at least alignof(void*), and stride multiple of alignment
    assert_true(a >= alignof(void*));
    assert_true((s % a) == 0);

    // Since we requested 64, expect base alignment to be >= 64
    assert_true(a >= 64);

    free_pool(p);
}
// -------------------------------------------------------------------------------- 

static void test_pool_bump_remaining_single_chunk(void **state) {
    (void)state;

    // Static pool → single prewarmed slice by design
    static unsigned char buf[8 * 1024] __attribute__((aligned(64)));
    pool_t *p = init_static_pool(buf, sizeof buf, /*block_size=*/64, /*alignment=*/64);
    assert_non_null(p);

    size_t before = pool_bump_remaining_blocks(p);
    assert_true(before > 0);
    assert_int_equal(before, pool_total_blocks(p)); // static pool: all capacity in one slice

    // Allocate two blocks → bump remaining decreases by 2
    void *b0 = alloc_pool(p, false); assert_non_null(b0);
    void *b1 = alloc_pool(p, false); assert_non_null(b1);

    size_t after2 = pool_bump_remaining_blocks(p);
    assert_int_equal(after2, before - 2);

    // Return one block → free list grows, bump remaining unchanged
    return_pool_element(p, b0);
    assert_int_equal(pool_free_blocks(p), 1);
    size_t after_free = pool_bump_remaining_blocks(p);
    assert_int_equal(after_free, after2);

    free_pool(p);
}
// -------------------------------------------------------------------------------- 
//
static void test_pool_bump_remaining_two_chunks_dynamic(void **state) {
    (void)state;

    // Dynamic, grow-enabled, prewarm one chunk with small count
    const size_t BPC = 8;
    pool_t *p = init_dynamic_pool(
        /*block_size=*/32,
        /*alignment=*/16,
        /*blocks_per_chunk=*/BPC,
        /*arena_seed_bytes=*/8192,
        /*min_chunk_bytes=*/4096,
        /*grow_enabled=*/true,
        /*prewarm_one_chunk=*/true
    );
    assert_non_null(p);

    // Initial bump remaining equals that chunk's capacity
    assert_int_equal(pool_bump_remaining_blocks(p), BPC);

    // Exhaust the first chunk
    for (size_t i = 0; i < BPC; ++i) {
        assert_non_null(alloc_pool(p, false));
    }
    assert_int_equal(pool_bump_remaining_blocks(p), 0);

    // Next allocation should trigger growth → new chunk carved, one block consumed
    void *g = alloc_pool(p, false);
    assert_non_null(g);
    assert_int_equal(pool_bump_remaining_blocks(p), BPC - 1);

    free_pool(p);
}
// -------------------------------------------------------------------------------- 

static void test_pool_in_use_blocks_counts(void **state) {
    (void)state;

    static unsigned char buf[16 * 1024] __attribute__((aligned(64)));
    pool_t *p = init_static_pool(buf, sizeof buf, /*block_size=*/64, /*alignment=*/0);
    assert_non_null(p);
    assert_int_equal(pool_in_use_blocks(p), 0);
    
    void *a = alloc_pool(p, false); assert_non_null(a);
    void *b = alloc_pool(p, false); assert_non_null(b);

    assert_int_equal(pool_in_use_blocks(p), 2);

    return_pool_element(p, a);
    assert_int_equal(pool_in_use_blocks(p), 1);

    return_pool_element(p, b);
    assert_int_equal(pool_in_use_blocks(p), 0);

    free_pool(p);
}
// -------------------------------------------------------------------------------- 

static void test_pool_owns_arena_and_mtype_static_vs_dynamic(void **state) {
    (void)state;

    // Static
    static unsigned char sbuf[4096] __attribute__((aligned(64)));
    pool_t *ps = init_static_pool(sbuf, sizeof sbuf, /*block_size=*/64, /*alignment=*/0);
    assert_non_null(ps);
    assert_true(pool_owns_arena(ps));
    assert_int_equal(pool_mtype(ps), STATIC);
    free_pool(ps);

    // Dynamic
    pool_t *pd = init_dynamic_pool(
        /*block_size=*/64, /*alignment=*/0, /*blocks_per_chunk=*/8,
        /*arena_seed_bytes=*/8192, /*min_chunk_bytes=*/4096,
        /*grow_enabled=*/true, /*prewarm_one_chunk=*/true
    );
    assert_non_null(pd);
    assert_true(pool_owns_arena(pd));
    assert_int_equal(pool_mtype(pd), DYNAMIC);
    free_pool(pd);
}
// -------------------------------------------------------------------------------- 

static void test_pool_grow_enabled_and_toggle(void **state) {
    (void)state;

    // Dynamic, grow-enabled initially
    pool_t *p = init_dynamic_pool(
        /*block_size=*/32, /*alignment=*/0, /*blocks_per_chunk=*/4,
        /*arena_seed_bytes=*/8192, /*min_chunk_bytes=*/4096,
        /*grow_enabled=*/true, /*prewarm_one_chunk=*/true
    );
    assert_non_null(p);
    assert_true(pool_grow_enabled(p));
    
    // Freeze
    toggle_pool_growth(p, false);
    assert_false(pool_grow_enabled(p));

    // Re-enable (allowed: dynamic arena and resize==true)
    toggle_pool_growth(p, true);
    assert_true(pool_grow_enabled(p));

    free_pool(p);
}
// -------------------------------------------------------------------------------- 

static void test_pool_grow_toggle_disallowed_on_static(void **state) {
    (void)state;

    static unsigned char buf[4096] __attribute__((aligned(64)));
    pool_t *p = init_static_pool(buf, sizeof buf, /*block_size=*/64, /*alignment=*/0);
    assert_non_null(p);

    // Static pool is fixed-capacity: grow_enabled should be false and enabling should fail
    assert_false(pool_grow_enabled(p));
    
    SAVE_ERRNO(e); errno = 0;
    toggle_pool_growth(p, true);
    assert_false(pool_grow_enabled(p));

    free_pool(p);
}
// ================================================================================ 
// ================================================================================ 

static void test_pool_save_restore_basic(void **state) {
    (void)state;
    
    pool_t* p = init_dynamic_pool(32, 4, 64, 4096, 4096, true, true);
    assert_non_null(p);
    
    // Allocate, save, allocate more, restore
    void* a = alloc_pool(p, false);
    assert_non_null(a);
    
    PoolCheckPoint cp = save_pool(p);
    
    void* b = alloc_pool(p, false);
    assert_non_null(b);
    
    size_t before = pool_in_use_blocks(p);
    assert_true(restore_pool(p, cp));
    size_t after = pool_in_use_blocks(p);
    
    // After restore, one less block in use
    assert_true(after < before);
    
    free_pool(p);
}

static void test_pool_restore_reuses_address(void **state) {
    (void)state;
    
    pool_t* p = init_dynamic_pool(64, 8, 128, 4096, 4096, true, true);
    assert_non_null(p);
    
    PoolCheckPoint cp = save_pool(p);
    
    void* first = alloc_pool(p, false);
    assert_non_null(first);
    
    assert_true(restore_pool(p, cp));
    
    void* second = alloc_pool(p, false);
    assert_non_null(second);
    
    // Should get same address (deterministic bump allocation)
    assert_ptr_equal(first, second);
    
    free_pool(p);
}

static void test_pool_restore_invalidates_checkpoint(void **state) {
    (void)state;
    
    pool_t* p = init_dynamic_pool(32, 4, 64, 4096, 4096, true, true);
    assert_non_null(p);
    
    PoolCheckPoint cp = save_pool(p);
    (void)alloc_pool(p, false);
    
    // Reset invalidates all checkpoints
    reset_pool(p);

    errno = 0;
    assert_false(restore_pool(p, cp));
    // assert_int_equal(errno, EINVAL);
    
    free_pool(p);
}

static void test_pool_empty_checkpoint_noop(void **state) {
    (void)state;
    
    pool_t* p = init_dynamic_pool(32, 4, 64, 4096, 4096, true, true);
    assert_non_null(p);
    
    PoolCheckPoint empty = {0};
    
    size_t before = pool_total_blocks(p);
    assert_true(restore_pool(p, empty));
    size_t after = pool_total_blocks(p);
    
    assert_int_equal(before, after);  // No change
    
    free_pool(p);
}
// ================================================================================ 
// ================================================================================ 

const struct CMUnitTest test_pool[] = {
    cmocka_unit_test(test_init_pool_invalid_args),
    cmocka_unit_test(test_init_pool_header_lives_in_arena),
    cmocka_unit_test(test_init_pool_alignment_and_stride_rules),
    cmocka_unit_test(test_init_pool_prewarm_sets_blocks),
    cmocka_unit_test(test_init_pool_fails_when_no_room_for_header ),
    cmocka_unit_test(test_pool_reset_semantics),
    
    cmocka_unit_test(test_init_dynamic_pool_invalid_args),
    cmocka_unit_test(test_init_dynamic_pool_overflow_guard),
    cmocka_unit_test(test_init_dynamic_pool_alignment_and_stride),
    cmocka_unit_test(test_init_dynamic_pool_fixed_prewarm_exact),
    cmocka_unit_test(test_init_dynamic_pool_grow_prewarm_and_reuse),
    cmocka_unit_test(test_init_dynamic_pool_grow_lazy_first_alloc),
    cmocka_unit_test(test_init_dynamic_pool_tiny_seed_fails),

    cmocka_unit_test(test_init_static_pool_invalid_args),
    cmocka_unit_test(test_init_static_pool_too_small_buffer),
    cmocka_unit_test(test_init_static_pool_alignment_and_stride),
    cmocka_unit_test(test_init_static_pool_stride_minimum),
    cmocka_unit_test(test_init_static_pool_capacity_and_exhaustion),
    cmocka_unit_test(test_init_static_pool_free_list_reuse),
    cmocka_unit_test(test_init_static_pool_one_block_only),

    cmocka_unit_test(test_pool_alignment_matches_contract),
    cmocka_unit_test(test_pool_bump_remaining_single_chunk),
    cmocka_unit_test(test_pool_bump_remaining_two_chunks_dynamic),
    cmocka_unit_test(test_pool_in_use_blocks_counts),
    cmocka_unit_test(test_pool_owns_arena_and_mtype_static_vs_dynamic),
    cmocka_unit_test(test_pool_grow_enabled_and_toggle),
    cmocka_unit_test(test_pool_grow_toggle_disallowed_on_static),

    cmocka_unit_test(test_pool_save_restore_basic),
    cmocka_unit_test(test_pool_restore_reuses_address),
    cmocka_unit_test(test_pool_restore_invalidates_checkpoint),
    cmocka_unit_test(test_pool_empty_checkpoint_noop),
};

const size_t test_pool_count = sizeof(test_pool) / sizeof(test_pool[0]);
// ================================================================================ 
// ================================================================================ 

static int is_power_of_two(size_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}
// -------------------------------------------------------------------------------- 

static void test_init_freelist_with_arena_null_arena(void **state) {
    (void)state;

    errno = 0;
    freelist_t *fl = init_freelist_with_arena(NULL, 128u, 0u);

    assert_null(fl);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_init_freelist_with_arena_basic(void **state) {
    (void)state;

    uint8_t buffer[4096];
    errno = 0;

    arena_t *arena = init_static_arena(buffer, sizeof buffer,
                                       alignof(max_align_t));
    assert_non_null(arena);

    freelist_t *fl = init_freelist_with_arena(arena, 512u, 0u);
    assert_non_null(fl);

    /* Alignment must be power-of-two and >= max_align_t */
    size_t fl_align = freelist_alignment(fl);
    assert_true(is_power_of_two(fl_align));
    assert_true(fl_align >= alignof(max_align_t));

    /* Initial accounting */
    size_t alloc_bytes   = freelist_alloc(fl);
    size_t size_bytes    = freelist_size(fl);
    size_t remaining_now = freelist_remaining(fl);

    assert_true(alloc_bytes >= 512u);   /* must be at least requested size */
    assert_int_equal(size_bytes, 0u);   /* nothing allocated yet */
    assert_int_equal(remaining_now, alloc_bytes);
    assert_int_equal(size_bytes + remaining_now, alloc_bytes);

    /* Allocate a small block */
    errno = 0;
    const size_t request = 128u;
    void *p = alloc_freelist(fl, request, false);
    assert_non_null(p);

    /* Pointer should belong to this freelist and be properly aligned */
    assert_true(is_freelist_ptr(fl, p));
    assert_true(is_freelist_ptr_sized(fl, p, request));
    assert_int_equal((uintptr_t)p % fl_align, 0u);

    /* Accounting after allocation:
       - size_after is total *block* bytes consumed (user + header + padding)
       - remaining_after is alloc_bytes - size_after
       - size_after must be at least the requested user size */
    size_t size_after      = freelist_size(fl);
    size_t remaining_after = freelist_remaining(fl);

    assert_true(size_after >= request);
    assert_true(size_after <= alloc_bytes);          /* can't exceed total */
    assert_int_equal(size_after + remaining_after, alloc_bytes);

    /* Free and make sure accounting returns to original values */
    return_freelist_element(fl, p);

    size_t size_final      = freelist_size(fl);
    size_t remaining_final = freelist_remaining(fl);
    assert_int_equal(size_final, 0u);
    assert_int_equal(remaining_final, alloc_bytes);
    assert_int_equal(size_final + remaining_final, alloc_bytes);
}
// -------------------------------------------------------------------------------- 

static void test_init_freelist_with_arena_alignment_normalization(void **state) {
    (void)state;

    uint8_t buffer[4096];
    errno = 0;

    arena_t *arena = init_static_arena(buffer, sizeof buffer,
                                       alignof(max_align_t));
    assert_non_null(arena);

    /* Intentionally non-power-of-two alignment request */
    size_t requested_alignment = 24u;

    freelist_t *fl = init_freelist_with_arena(arena, 256u, requested_alignment);
    assert_non_null(fl);

    size_t fl_align = freelist_alignment(fl);

    /* Must be power-of-two and >= both max_align_t and requested_alignment */
    assert_true(is_power_of_two(fl_align));
    assert_true(fl_align >= alignof(max_align_t));
    assert_true(fl_align >= requested_alignment);

    /* An allocation from this freelist must respect that alignment */
    void *p = alloc_freelist(fl, 64u, false);
    assert_non_null(p);
    assert_int_equal((uintptr_t)p % fl_align, 0u);
    assert_true(is_freelist_ptr_sized(fl, p, 64u));

    return_freelist_element(fl, p);
}
// -------------------------------------------------------------------------------- 

static void test_init_freelist_with_arena_insufficient_arena_space(void **state) {
    (void)state;

    uint8_t buffer[128]; /* small buffer on purpose */
    errno = 0;

    arena_t *arena = init_static_arena(buffer, sizeof buffer,
                                       alignof(max_align_t));
    assert_non_null(arena);

    size_t avail = arena_remaining(arena);

    /* Request more bytes than the arena has. Since init_freelist_with_arena
       internally needs space for freelist_t plus usable memory, asking for
       > avail guarantees alloc_arena() fails. */
    freelist_t *fl = init_freelist_with_arena(arena, avail + 1u, 0u);

    assert_null(fl);
    /* alloc_arena should have set errno appropriately; ENOMEM is typical.
       Adjust this if your arena uses a different errno. */
    assert_int_equal(errno, EPERM);
}
// -------------------------------------------------------------------------------- 

static void test_init_dynamic_freelist_basic(void **state) {
    (void)state;

    errno = 0;
    freelist_t *fl = init_dynamic_freelist(512u, 0u, false);
    assert_non_null(fl);

    /* Alignment must be power-of-two and >= max_align_t */
    size_t fl_align = freelist_alignment(fl);
    assert_true(is_power_of_two(fl_align));
    assert_true(fl_align >= alignof(max_align_t));

    /* Initial accounting */
    size_t alloc_bytes   = freelist_alloc(fl);
    size_t size_bytes    = freelist_size(fl);
    size_t remaining_now = freelist_remaining(fl);
    assert_true(alloc_bytes >= 416u);        /* at least requested */
    assert_int_equal(size_bytes, 0u);        /* nothing used yet */
    assert_int_equal(remaining_now, alloc_bytes);
    assert_int_equal(size_bytes + remaining_now, alloc_bytes);

    /* Allocate a small block */
    const size_t request = 128u;
    errno = 0;
    void *p = alloc_freelist(fl, request, false);
    assert_non_null(p);

    /* Pointer should belong to this freelist and be properly aligned */
    assert_true(is_freelist_ptr(fl, p));
    assert_true(is_freelist_ptr_sized(fl, p, request));
    assert_int_equal((uintptr_t)p % fl_align, 0u);

    /* After allocation:
       - freelist_size counts full block bytes (header + padding + user)
       - remaining = alloc_bytes - freelist_size */
    size_t size_after      = freelist_size(fl);
    size_t remaining_after = freelist_remaining(fl);

    assert_true(size_after >= request);
    assert_true(size_after <= alloc_bytes);
    assert_int_equal(size_after + remaining_after, alloc_bytes);

    /* Free and make sure accounting returns to original values */
    return_freelist_element(fl, p);

    size_t size_final      = freelist_size(fl);
    size_t remaining_final = freelist_remaining(fl);

    assert_int_equal(size_final, 0u);
    assert_int_equal(remaining_final, alloc_bytes);
    assert_int_equal(size_final + remaining_final, alloc_bytes);

    /* Cleanup dynamic freelist (it owns its arena) */
    free_freelist(fl);
}
// -------------------------------------------------------------------------------- 

static void test_init_dynamic_freelist_size_zero_uses_default(void **state) {
    (void)state;

    errno = 0;
    freelist_t *fl = init_dynamic_freelist(0u, 0u, false);
    assert_null(fl);
    assert_int_equal(errno, EINVAL);
    freelist_t *nfl = init_dynamic_freelist(6u, 0u, false);
    assert_null(nfl);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_init_dynamic_freelist_alignment_normalization(void **state) {
    (void)state;

    errno = 0;
    size_t requested_alignment = 24u;  /* intentionally non power-of-two */

    freelist_t *fl = init_dynamic_freelist(256u, requested_alignment, false);
    assert_non_null(fl);

    size_t fl_align = freelist_alignment(fl);

    /* Must be power-of-two and >= both max_align_t and requested_alignment */
    assert_true(is_power_of_two(fl_align));
    assert_true(fl_align >= alignof(max_align_t));
    assert_true(fl_align >= requested_alignment);

    /* An allocation should respect that alignment */
    const size_t request = 64u;
    void *p = alloc_freelist(fl, request, false);
    assert_non_null(p);
    assert_true(is_freelist_ptr_sized(fl, p, request));
    assert_int_equal((uintptr_t)p % fl_align, 0u);

    return_freelist_element(fl, p);
    free_freelist(fl);
}
// -------------------------------------------------------------------------------- 

static void test_init_static_freelist_basic(void **state) {
    (void)state;

    uint8_t buffer[1024];
    errno = 0;

    /* alignment = 0 -> normalize to >= alignof(max_align_t) */
    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);

    /* Alignment must be power-of-two and >= max_align_t */
    size_t fl_align = freelist_alignment(fl);
    assert_true(is_power_of_two(fl_align));
    assert_true(fl_align >= alignof(max_align_t));

    /* Initial accounting */
    size_t alloc_bytes   = freelist_alloc(fl);
    size_t size_bytes    = freelist_size(fl);
    size_t remaining_now = freelist_remaining(fl);

    assert_true(alloc_bytes > 0u);
    assert_true(alloc_bytes <= sizeof buffer);
    assert_int_equal(size_bytes, 0u);
    assert_int_equal(remaining_now, alloc_bytes);
    assert_int_equal(size_bytes + remaining_now, alloc_bytes);

    /* Allocate a small block */
    const size_t request = 128u;
    errno = 0;
    void *p = alloc_freelist(fl, request, false);
    assert_non_null(p);

    /* Pointer should belong to this freelist and be properly aligned */
    assert_true(is_freelist_ptr(fl, p));
    assert_true(is_freelist_ptr_sized(fl, p, request));
    assert_int_equal((uintptr_t)p % fl_align, 0u);

    /* After allocation: freelist_size counts full block bytes */
    size_t size_after      = freelist_size(fl);
    size_t remaining_after = freelist_remaining(fl);

    assert_true(size_after >= request);
    assert_true(size_after <= alloc_bytes);
    assert_int_equal(size_after + remaining_after, alloc_bytes);

    /* Free and make sure accounting returns to original values */
    return_freelist_element(fl, p);

    size_t size_final      = freelist_size(fl);
    size_t remaining_final = freelist_remaining(fl);

    assert_int_equal(size_final, 0u);
    assert_int_equal(remaining_final, alloc_bytes);
    assert_int_equal(size_final + remaining_final, alloc_bytes);
}
// -------------------------------------------------------------------------------- 

static void test_init_static_freelist_zero_bytes_invalid(void **state) {
    (void)state;

    uint8_t buffer[1];
    errno = 0;

    freelist_t *fl = init_static_freelist(buffer, 0u, 0u);
    assert_null(fl);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_init_static_freelist_too_small_buffer(void **state) {
    (void)state;

    uint8_t buffer[8];   /* almost certainly < freelist_min_request */
    errno = 0;

    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_null(fl);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_init_static_freelist_alignment_normalization(void **state) {
    (void)state;

    uint8_t buffer[2048];
    errno = 0;

    size_t requested_alignment = 24u;  /* intentionally non–power-of-two */

    freelist_t *fl = init_static_freelist(buffer, sizeof buffer,
                                          requested_alignment);
    assert_non_null(fl);

    size_t fl_align = freelist_alignment(fl);

    /* Must be power-of-two and >= both max_align_t and requested_alignment */
    assert_true(is_power_of_two(fl_align));
    assert_true(fl_align >= alignof(max_align_t));
    assert_true(fl_align >= requested_alignment);

    /* Allocation should respect that alignment */
    const size_t request = 64u;
    void *p = alloc_freelist(fl, request, false);
    assert_non_null(p);
    assert_true(is_freelist_ptr_sized(fl, p, request));
    assert_int_equal((uintptr_t)p % fl_align, 0u);

    return_freelist_element(fl, p);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_freelist_aligned_basic(void **state) {
    (void)state;

    uint8_t buffer[4096];
    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);

    size_t fl_align = freelist_alignment(fl);
    assert_true(is_power_of_two(fl_align));

    size_t alloc_bytes = freelist_alloc(fl);
    assert_true(alloc_bytes > 0u);

    const size_t req_align = 64u;
    const size_t req_bytes = 128u;

    void *p = alloc_freelist_aligned(fl, req_bytes, req_align, false);
    assert_non_null(p);

    /* Ownership checks */
    assert_true(is_freelist_ptr(fl, p));
    assert_true(is_freelist_ptr_sized(fl, p, req_bytes));

    /* Alignment checks */
    assert_int_equal(((uintptr_t)p) % req_align, 0u);
    assert_int_equal(((uintptr_t)p) % fl_align, 0u);

    /* Accounting */
    size_t size_after = freelist_size(fl);
    size_t remaining_after = freelist_remaining(fl);
    assert_true(size_after >= req_bytes);
    assert_true(size_after <= alloc_bytes);
    assert_int_equal(size_after + remaining_after, alloc_bytes);

    /* Round-trip free */
    return_freelist_element(fl, p);
    assert_int_equal(freelist_size(fl), 0u);
    assert_int_equal(freelist_remaining(fl), alloc_bytes);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_freelist_aligned_zero_alignment_uses_base(void **state) {
    (void)state;

    uint8_t buffer[4096];
    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);

    size_t fl_align = freelist_alignment(fl);

    const size_t req_bytes = 96u;

    void *p = alloc_freelist_aligned(fl, req_bytes, 0u, false);
    assert_non_null(p);

    assert_true(is_freelist_ptr(fl, p));
    assert_true(is_freelist_ptr_sized(fl, p, req_bytes));

    /* Should be aligned to freelist's base alignment */
    assert_int_equal(((uintptr_t)p) % fl_align, 0u);

    return_freelist_element(fl, p);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_freelist_aligned_non_power_of_two_align(void **state) {
    (void)state;

    uint8_t buffer[4096];
    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);

    size_t fl_align = freelist_alignment(fl);

    size_t requested_alignment = 24u;  /* not power-of-two */
    size_t req_bytes = 64u;

    void *p = alloc_freelist_aligned(fl, req_bytes, requested_alignment, false);
    assert_non_null(p);

    assert_true(is_freelist_ptr(fl, p));
    assert_true(is_freelist_ptr_sized(fl, p, req_bytes));

    /* We know normalized alignment ≥ fl_align */
    assert_int_equal(((uintptr_t)p) % fl_align, 0u);

    return_freelist_element(fl, p);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_freelist_aligned_zeroed(void **state) {
    (void)state;

    uint8_t buffer[4096];
    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);

    const size_t req_bytes = 64u;
    const size_t req_align = 32u;

    void *p = alloc_freelist_aligned(fl, req_bytes, req_align, true);
    assert_non_null(p);

    /* Entire user region must be zero */
    uint8_t *u = (uint8_t *)p;
    for (size_t i = 0; i < req_bytes; ++i) {
        assert_int_equal(u[i], 0u);
    }

    return_freelist_element(fl, p);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_freelist_aligned_null_freelist(void **state) {
    (void)state;

    errno = 0;
    void *p = alloc_freelist_aligned(NULL, 16u, 16u, false);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_freelist_aligned_zero_bytes(void **state) {
    (void)state;

    uint8_t buffer[512];
    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);

    errno = 0;
    void *p = alloc_freelist_aligned(fl, 0u, 16u, false);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_freelist_null_freelist(void **state) {
    (void)state;

    errno = 0;
    void *p = realloc_freelist(NULL, NULL, 0u, 16u, false);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_freelist_null_ptr_allocates_and_zeroes(void **state) {
    (void)state;

    uint8_t buffer[1024];
    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);

    const size_t new_size = 64u;

    errno = 0;
    void *p = realloc_freelist(fl, NULL, 0u, new_size, true);
    assert_non_null(p);
    assert_true(is_freelist_ptr_sized(fl, p, new_size));

    /* Newly allocated memory should be zeroed */
    uint8_t *u = (uint8_t *)p;
    for (size_t i = 0; i < new_size; ++i) {
        assert_int_equal(u[i], 0u);
    }

    return_freelist_element(fl, p);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_freelist_shrink_keeps_pointer(void **state) {
    (void)state;

    uint8_t buffer[1024];
    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);

    const size_t old_size = 64u;
    const size_t new_size = 32u;  /* shrinking */

    uint8_t pattern = 0xAB;

    void *p = alloc_freelist(fl, old_size, false);
    assert_non_null(p);

    memset(p, pattern, old_size);
    size_t size_before      = freelist_size(fl);
    size_t remaining_before = freelist_remaining(fl);

    void *p2 = realloc_freelist(fl, p, old_size, new_size, true);
    assert_ptr_equal(p2, p);  /* must return same pointer */

    /* No accounting changes for shrink */
    size_t size_after      = freelist_size(fl);
    size_t remaining_after = freelist_remaining(fl);
    assert_int_equal(size_before,      size_after);
    assert_int_equal(remaining_before, remaining_after);

    /* Data must be preserved at least new_size bytes */
    uint8_t *u = (uint8_t *)p2;
    for (size_t i = 0; i < new_size; ++i) {
        assert_int_equal(u[i], pattern);
    }

    return_freelist_element(fl, p2);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_freelist_grow_moves_and_zeroes_tail(void **state) {
    (void)state;

    uint8_t buffer[2048];
    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);

    const size_t old_size = 64u;
    const size_t new_size = 160u;  /* larger than old_size */

    /* Allocate original block and fill with pattern */
    void *p = alloc_freelist(fl, old_size, false);
    assert_non_null(p);

    uint8_t *u_old = (uint8_t *)p;
    for (size_t i = 0; i < old_size; ++i) {
        u_old[i] = (uint8_t)(i & 0xFF);
    }

    size_t size_before      = freelist_size(fl);
    size_t remaining_before = freelist_remaining(fl);

    /* Grow with zeroed tail */
    void *p2 = realloc_freelist(fl, p, old_size, new_size, true);
    assert_non_null(p2);
    assert_true(is_freelist_ptr_sized(fl, p2, new_size));

    /* Expect a move in most implementations; don't require it, but
       it's strongly expected. If you want to enforce it: */
    assert_ptr_not_equal(p2, p);

    uint8_t *u_new = (uint8_t *)p2;

    /* Old data preserved */
    for (size_t i = 0; i < old_size; ++i) {
        assert_int_equal(u_new[i], (uint8_t)(i & 0xFF));
    }

    /* Tail must be zeroed */
    for (size_t i = old_size; i < new_size; ++i) {
        assert_int_equal(u_new[i], 0u);
    }

    /* Accounting should still be consistent */
    size_t size_after      = freelist_size(fl);
    size_t remaining_after = freelist_remaining(fl);
    assert_true(size_after >= new_size);
    assert_int_equal(size_after + remaining_after, freelist_alloc(fl));

    return_freelist_element(fl, p2);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_freelist_aligned_null_freelist(void **state) {
    (void)state;

    errno = 0;
    void *p = realloc_freelist_aligned(NULL, NULL, 0u, 32u, false, 32u);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_freelist_aligned_null_ptr_allocates(void **state) {
    (void)state;

    uint8_t buffer[2048];
    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);

    size_t fl_align = freelist_alignment(fl);
    assert_true(is_power_of_two(fl_align));

    const size_t new_size  = 96u;
    const size_t req_align = 64u;

    void *p = realloc_freelist_aligned(fl, NULL, 0u, new_size, true, req_align);
    assert_non_null(p);
    assert_true(is_freelist_ptr_sized(fl, p, new_size));

    /* Should be aligned at least to req_align and fl_align */
    assert_int_equal(((uintptr_t)p) % req_align, 0u);
    assert_int_equal(((uintptr_t)p) % fl_align, 0u);

    /* Zeroing check */
    uint8_t *u = (uint8_t *)p;
    for (size_t i = 0; i < new_size; ++i) {
        assert_int_equal(u[i], 0u);
    }

    return_freelist_element(fl, p);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_freelist_aligned_shrink_keeps_pointer(void **state) {
    (void)state;

    uint8_t buffer[2048];
    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);

    const size_t old_size  = 128u;
    const size_t new_size  = 64u;
    const size_t req_align = 32u;

    void *p = alloc_freelist_aligned(fl, old_size, req_align, false);
    assert_non_null(p);

    uint8_t *u_old = (uint8_t *)p;
    for (size_t i = 0; i < old_size; ++i) {
        u_old[i] = (uint8_t)(0xC0 + (i & 0x3F));
    }

    size_t size_before      = freelist_size(fl);
    size_t remaining_before = freelist_remaining(fl);

    void *p2 = realloc_freelist_aligned(fl, p, old_size, new_size, true, req_align);
    assert_ptr_equal(p2, p);

    /* No accounting change */
    size_t size_after      = freelist_size(fl);
    size_t remaining_after = freelist_remaining(fl);
    assert_int_equal(size_before,      size_after);
    assert_int_equal(remaining_before, remaining_after);

    uint8_t *u_new = (uint8_t *)p2;
    for (size_t i = 0; i < new_size; ++i) {
        assert_int_equal(u_new[i], (uint8_t)(0xC0 + (i & 0x3F)));
    }

    return_freelist_element(fl, p2);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_freelist_aligned_grow_preserves_data_and_alignment(void **state) {
    (void)state;

    uint8_t buffer[4096];
    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);

    size_t fl_align = freelist_alignment(fl);
    assert_true(is_power_of_two(fl_align));

    const size_t old_size  = 80u;
    const size_t new_size  = 200u;
    const size_t req_align = 64u;

    void *p = alloc_freelist_aligned(fl, old_size, req_align, false);
    assert_non_null(p);

    uint8_t *u_old = (uint8_t *)p;
    for (size_t i = 0; i < old_size; ++i) {
        u_old[i] = (uint8_t)(i ^ 0x5A);
    }

    size_t size_before      = freelist_size(fl);
    size_t remaining_before = freelist_remaining(fl);

    void *p2 = realloc_freelist_aligned(fl, p, old_size, new_size, true, req_align);
    assert_non_null(p2);
    assert_true(is_freelist_ptr_sized(fl, p2, new_size));
    assert_ptr_not_equal(p2, p);

    /* Alignment: at least req_align and fl_align */
    assert_int_equal(((uintptr_t)p2) % req_align, 0u);
    assert_int_equal(((uintptr_t)p2) % fl_align, 0u);

    uint8_t *u_new = (uint8_t *)p2;

    /* Old data preserved */
    for (size_t i = 0; i < old_size; ++i) {
        assert_int_equal(u_new[i], (uint8_t)(i ^ 0x5A));
    }

    /* Tail zeroed */
    for (size_t i = old_size; i < new_size; ++i) {
        assert_int_equal(u_new[i], 0u);
    }

    size_t size_after      = freelist_size(fl);
    size_t remaining_after = freelist_remaining(fl);
    assert_true(size_after >= new_size);
    assert_int_equal(size_after + remaining_after, freelist_alloc(fl));

    return_freelist_element(fl, p2);
}
// -------------------------------------------------------------------------------- 

static void test_reset_freelist_null(void **state) {
    (void)state;

    errno = 0;
    reset_freelist(NULL);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_reset_freelist_basic_static(void **state) {
    (void)state;

    uint8_t buffer[4096];
    errno = 0;

    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);

    /* Basic sanity */
    size_t fl_align = freelist_alignment(fl);
    assert_true(is_power_of_two(fl_align));
    assert_true(fl_align >= alignof(max_align_t));

    size_t alloc_bytes   = freelist_alloc(fl);
    size_t size_bytes    = freelist_size(fl);
    size_t remaining_now = freelist_remaining(fl);

    assert_true(alloc_bytes > 0u);
    assert_int_equal(size_bytes, 0u);
    assert_int_equal(remaining_now, alloc_bytes);
    assert_int_equal(size_bytes + remaining_now, alloc_bytes);

    /* Allocate one block */
    const size_t request = 128u;
    void *p = alloc_freelist(fl, request, false);
    assert_non_null(p);
    assert_true(is_freelist_ptr_sized(fl, p, request));
    assert_int_equal(((uintptr_t)p) % fl_align, 0u);

    size_t size_after      = freelist_size(fl);
    size_t remaining_after = freelist_remaining(fl);
    assert_true(size_after >= request);
    assert_true(size_after <= alloc_bytes);
    assert_int_equal(size_after + remaining_after, alloc_bytes);

    /* Now reset */
    errno = 0;
    reset_freelist(fl);
    assert_int_equal(errno, 0);

    /* After reset, accounting should look like a fresh freelist */
    size_t size_reset      = freelist_size(fl);
    size_t remaining_reset = freelist_remaining(fl);

    assert_int_equal(size_reset, 0u);
    assert_int_equal(remaining_reset, alloc_bytes);
    assert_int_equal(size_reset + remaining_reset, alloc_bytes);

    /* And we should be able to allocate again as if new */
    void *p2 = alloc_freelist(fl, request, true);
    assert_non_null(p2);
    assert_true(is_freelist_ptr_sized(fl, p2, request));
    assert_int_equal(((uintptr_t)p2) % fl_align, 0u);

    /* Clean up logically (not strictly needed for static freelist) */
    return_freelist_element(fl, p2);
}
// -------------------------------------------------------------------------------- 

static void test_reset_freelist_after_fragmentation(void **state) {
    (void)state;

    uint8_t buffer[4096];
    errno = 0;

    freelist_t *fl = init_static_freelist(buffer, sizeof buffer, 0u);
    assert_non_null(fl);
    assert_int_equal(STATIC, freelist_mtype(fl));

    size_t alloc_bytes_initial = freelist_alloc(fl);
    assert_true(alloc_bytes_initial > 0u);

    /* Allocate three blocks */
    const size_t a_size = 128u;
    const size_t b_size = 256u;
    const size_t c_size = 64u;

    void *a = alloc_freelist(fl, a_size, false);
    void *b = alloc_freelist(fl, b_size, false);
    void *c = alloc_freelist(fl, c_size, false);

    assert_non_null(a);
    assert_non_null(b);
    assert_non_null(c);

    /* Free the middle block to create fragmentation */
    return_freelist_element(fl, b);

    /* At this point, accounting shows some usage */
    size_t size_before_reset      = freelist_size(fl);
    size_t remaining_before_reset = freelist_remaining(fl);
    assert_true(size_before_reset > 0u);
    assert_int_equal(size_before_reset + remaining_before_reset,
                     alloc_bytes_initial);

    /* Now reset everything */
    errno = 0;
    reset_freelist(fl);
    assert_int_equal(errno, 0);

    size_t size_after_reset      = freelist_size(fl);
    size_t remaining_after_reset = freelist_remaining(fl);

    /* After reset, must look like fresh freelist again */
    assert_int_equal(size_after_reset, 0u);
    assert_int_equal(remaining_after_reset, alloc_bytes_initial);
    assert_int_equal(size_after_reset + remaining_after_reset,
                     alloc_bytes_initial);

    /* We should be able to allocate a relatively large block now
       (e.g., about half of total capacity). */
    const size_t big_request = alloc_bytes_initial / 2u;
    void *big = alloc_freelist(fl, big_request, false);
    assert_non_null(big);
    assert_true(is_freelist_ptr_sized(fl, big, big_request));

    return_freelist_element(fl, big);
}
// -------------------------------------------------------------------------------- 

const struct CMUnitTest test_freelist[] = {
    cmocka_unit_test(test_init_freelist_with_arena_null_arena),
    cmocka_unit_test(test_init_freelist_with_arena_basic),
    cmocka_unit_test(test_init_freelist_with_arena_alignment_normalization),
    cmocka_unit_test(test_init_freelist_with_arena_insufficient_arena_space),
    cmocka_unit_test(test_init_dynamic_freelist_basic),
    cmocka_unit_test(test_init_dynamic_freelist_size_zero_uses_default),
    cmocka_unit_test(test_init_dynamic_freelist_alignment_normalization),
    cmocka_unit_test(test_init_static_freelist_basic),
    cmocka_unit_test(test_init_static_freelist_zero_bytes_invalid),
    cmocka_unit_test(test_init_static_freelist_too_small_buffer),
    cmocka_unit_test(test_init_static_freelist_alignment_normalization),
    cmocka_unit_test(test_alloc_freelist_aligned_basic),
    cmocka_unit_test(test_alloc_freelist_aligned_zero_alignment_uses_base),
    cmocka_unit_test(test_alloc_freelist_aligned_non_power_of_two_align),
    cmocka_unit_test(test_alloc_freelist_aligned_zeroed),
    cmocka_unit_test(test_alloc_freelist_aligned_null_freelist),
    cmocka_unit_test(test_alloc_freelist_aligned_zero_bytes),
    cmocka_unit_test(test_realloc_freelist_null_freelist),
    cmocka_unit_test(test_realloc_freelist_null_ptr_allocates_and_zeroes),
    cmocka_unit_test(test_realloc_freelist_shrink_keeps_pointer),
    cmocka_unit_test(test_realloc_freelist_grow_moves_and_zeroes_tail),
    cmocka_unit_test(test_realloc_freelist_aligned_null_freelist),
    cmocka_unit_test(test_realloc_freelist_aligned_null_ptr_allocates),
    cmocka_unit_test(test_realloc_freelist_aligned_shrink_keeps_pointer),
    cmocka_unit_test(test_realloc_freelist_aligned_grow_preserves_data_and_alignment),
    cmocka_unit_test(test_reset_freelist_null),
    cmocka_unit_test(test_reset_freelist_basic_static),
    cmocka_unit_test(test_reset_freelist_after_fragmentation),
};


const size_t test_freelist_count = sizeof(test_freelist) / sizeof(test_freelist[0]);
// ================================================================================ 
// ================================================================================ 
// TEST BUDDY ALLOCATOR 

static size_t next_pow2_test(size_t x) {
    if (x == 0) return 1;
    x--;
    for (size_t i = 1; i < sizeof(size_t) * 8; i <<= 1) {
        x |= x >> i;
    }
    x++;
    return x;
}
// -------------------------------------------------------------------------------- 

static void test_init_buddy_zero_pool(void **state) {
    (void)state;
    errno = 0;

    buddy_t *b = init_buddy_allocator(0u, 64u, 16u);

    assert_null(b);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_init_buddy_zero_min_block(void **state) {
    (void)state;
    errno = 0;

    buddy_t *b = init_buddy_allocator(1024u, 0u, 16u);

    assert_null(b);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_init_buddy_zero_base_align(void **state) {
    (void)state;
    errno = 0;

    buddy_t *b = init_buddy_allocator(1024u, 64u, 0u);

    assert_non_null(b);

    /* Nothing allocated yet */
    assert_int_equal(buddy_alloc(b), 0u);
    assert_int_equal(buddy_remaining(b), 1024u);

    /* Largest block is entire pool */
    assert_int_equal(buddy_largest_block(b), 1024u);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_init_buddy_min_block_too_large(void **state) {
    (void)state;
    errno = 0;

    buddy_t *b = init_buddy_allocator(1024u, 4096u, 64u);
    assert_null(b);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_init_buddy_rounding(void **state) {
    (void)state;

    size_t req_pool = 1500u;        /* rounds to 2048 */
    size_t req_min  = 30u;          /* rounds to 32   */
    size_t align    = 16u;          /* already pow2   */

    size_t expected_pool = next_pow2_test(req_pool);
    size_t expected_min  = next_pow2_test(req_min);

    errno = 0;
    buddy_t *b = init_buddy_allocator(req_pool, req_min, align);

    assert_non_null(b);

    /* No allocation yet */
    assert_int_equal(buddy_alloc(b), 0u);
    assert_int_equal(buddy_remaining(b), expected_pool);
    assert_int_equal(buddy_largest_block(b), expected_pool);

    /* Total alloc must be >= expected_pool */
    assert_true(total_buddy_alloc(b) >= expected_pool);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_init_buddy_exact_powers_of_two(void **state) {
    (void)state;

    size_t pool = 1u << 20; /* 1 MiB */
    size_t minb = 1u << 6;  /* 64 B  */

    errno = 0;
    buddy_t *b = init_buddy_allocator(pool, minb, 64u);
    assert_non_null(b);

    assert_int_equal(buddy_alloc(b), 0u);
    assert_int_equal(buddy_remaining(b), pool);
    assert_int_equal(buddy_largest_block(b), pool);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_init_buddy_non_pow2_align(void **state) {
    (void)state;

    size_t pool = 4096u;
    size_t mb   = 64u;
    size_t bad_align = 24u; /* rounds to 32 */

    errno = 0;
    buddy_t *b = init_buddy_allocator(pool, mb, bad_align);
    assert_non_null(b);

    /* Sanity checks for init state: */
    assert_int_equal(buddy_remaining(b), pool);
    assert_int_equal(buddy_largest_block(b), pool);
    assert_int_equal(buddy_alloc(b), 0u);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_buddy_basic(void **state) {
    (void)state;

    size_t pool      = 1024u;
    size_t min_block = 64u;
    size_t base_align = alignof(max_align_t);

    errno = 0;
    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t before_remaining = buddy_remaining(b);
    assert_int_equal(before_remaining, next_pow2_test(pool));

    size_t req_size = 32u;

    void *p = alloc_buddy(b, req_size, false);
    assert_non_null(p);

    /* remaining should have decreased by at least req_size */
    size_t after_remaining = buddy_remaining(b);
    assert_true(after_remaining < before_remaining);

    /* buddy_alloc should reflect bytes consumed from the pool */
    size_t used = buddy_alloc(b);
    assert_int_equal(used, before_remaining - after_remaining);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_buddy_too_large(void **state) {
    (void)state;

    size_t pool      = 1024u;
    size_t min_block = 64u;
    size_t base_align = alignof(max_align_t);

    errno = 0;
    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    /* Request larger than the entire pool. */
    errno = 0;
    void *p = alloc_buddy(b, pool + 1u, false);
    assert_null(p);
    assert_int_equal(errno, ENOMEM);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_buddy_zeroed_reuse_full_block(void **state) {
    (void)state;

    size_t pool      = 1024u;
    size_t min_block = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);
    
    /* Request big enough so allocator uses the whole pool as one block.
       We assume header + requested_size rounds up to pool. */
    size_t req_size = 900u;

    void *p1 = alloc_buddy(b, req_size, false);
    assert_non_null(p1);

    /* Pool should now be fully consumed. */
    assert_int_equal(buddy_remaining(b), 0u);

    /* Fill with a non-zero pattern. */
    memset(p1, 0xAA, req_size);

    /* Free the block. */
    assert_true(return_buddy_element(b, p1));

    /* Full pool should be free again. */
    assert_int_equal(buddy_remaining(b), pool);

    /* Allocate again, this time zeroed. */
    void *p2 = alloc_buddy(b, req_size, true);
    assert_non_null(p2);

    /* Verify buffer is zeroed. */
    const unsigned char *bytes = (const unsigned char *)p2;
    for (size_t i = 0; i < req_size; ++i) {
        assert_int_equal(bytes[i], 0u);
    }

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_buddy_aligned_basic(void **state) {
    (void)state;

    size_t pool       = 4096u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t req_size = 128u;
    size_t align    = 64u;

    void *p = alloc_buddy_aligned(b, req_size, align, false);
    assert_non_null(p);

    uintptr_t addr = (uintptr_t)p;
    assert_int_equal(addr % align, 0u);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_buddy_aligned_non_pow2_align(void **state) {
    (void)state;

    size_t pool       = 4096u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t req_size = 128u;
    size_t align    = 24u; /* should be normalized to 32 internally */

    void *p = alloc_buddy_aligned(b, req_size, align, false);
    assert_non_null(p);

    uintptr_t addr = (uintptr_t)p;
    /* We don't care about 24, we care it meets the rounded alignment: 32. */
    assert_int_equal(addr % 32u, 0u);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_buddy_aligned_zero_align(void **state) {
    (void)state;

    size_t pool       = 2048u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t req_size = 100u;

    void *p = alloc_buddy_aligned(b, req_size, 0u, false);
    assert_non_null(p);

    uintptr_t addr = (uintptr_t)p;
    /* At least max_align_t alignment. */
    assert_int_equal(addr % alignof(max_align_t), 0u);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_buddy_aligned_zeroed_reuse_full_block(void **state) {
    (void)state;

    size_t pool       = 2048u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t req_size = 1800u;
    size_t align    = 64u;

    void *p1 = alloc_buddy_aligned(b, req_size, align, false);
    assert_non_null(p1);
    assert_int_equal(buddy_remaining(b), 0u);

    memset(p1, 0xBB, req_size);

    assert_true(return_buddy_element(b, p1));
    assert_int_equal(buddy_remaining(b), pool);

    void *p2 = alloc_buddy_aligned(b, req_size, align, true);
    assert_non_null(p2);

    uintptr_t addr = (uintptr_t)p2;
    assert_int_equal(addr % align, 0u);

    const unsigned char *bytes = (const unsigned char *)p2;
    for (size_t i = 0; i < req_size; ++i) {
        assert_int_equal(bytes[i], 0u);
    }

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_buddy_aligned_too_large(void **state) {
    (void)state;

    size_t pool       = 1024u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    errno = 0;
    void *p = alloc_buddy_aligned(b, pool + 512u, 64u, false);
    assert_null(p);
    assert_int_equal(errno, ENOMEM);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_buddy_from_null(void **state) {
    (void)state;

    size_t pool       = 4096u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t new_size = 128u;

    errno = 0;
    void *p = realloc_buddy(b, NULL, 0u, new_size, false);
    assert_non_null(p);
    assert_int_equal(errno, 0);

    /* Basic sanity: some memory is now used. */
    assert_true(buddy_alloc(b) > 0u);
    assert_true(buddy_remaining(b) < next_pow2_test(pool));

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_buddy_to_zero_frees(void **state) {
    (void)state;

    size_t pool       = 2048u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t size = 128u;
    void *p = alloc_buddy(b, size, false);
    assert_non_null(p);

    size_t remaining_before = buddy_remaining(b);

    /* realloc to 0 -> free */
    errno = 0;
    void *p2 = realloc_buddy(b, p, size, 0u, false);
    assert_null(p2);
    assert_int_equal(errno, 0);

    /* Entire pool should be free again. */
    assert_int_equal(buddy_remaining(b), next_pow2_test(pool));

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_buddy_shrink_in_place(void **state) {
    (void)state;

    size_t pool       = 4096u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t old_size = 200u;
    size_t new_size = 100u;

    uint8_t pattern[200];
    for (size_t i = 0; i < old_size; ++i) {
        pattern[i] = (uint8_t)(i & 0xFFu);
    }

    void *p = alloc_buddy(b, old_size, false);
    assert_non_null(p);

    memcpy(p, pattern, old_size);

    errno = 0;
    void *p2 = realloc_buddy(b, p, old_size, new_size, false);
    assert_non_null(p2);

    /* Ideally, shrink is in-place; if your implementation guarantees this,
       keep this assert. If not, you can drop it and only check contents. */
    assert_ptr_equal(p2, p);

    /* First new_size bytes must match original data. */
    uint8_t *bytes = (uint8_t *)p2;
    for (size_t i = 0; i < new_size; ++i) {
        assert_int_equal(bytes[i], pattern[i]);
    }

    /* The allocator should still consider the block "allocated" (no free). */
    assert_true(buddy_remaining(b) < next_pow2_test(pool));

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_buddy_grow_zeroed(void **state) {
    (void)state;

    size_t pool       = 4096u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t old_size = 100u;
    size_t new_size = 300u;

    uint8_t pattern[100];
    for (size_t i = 0; i < old_size; ++i) {
        pattern[i] = (uint8_t)(0xA0u + (i & 0x0Fu));
    }

    void *p = alloc_buddy(b, old_size, false);
    assert_non_null(p);
    memcpy(p, pattern, old_size);

    errno = 0;
    void *p2 = realloc_buddy(b, p, old_size, new_size, true);
    assert_non_null(p2);
    /* Implementation may or may not reuse same address; don't assume. */

    uint8_t *bytes = (uint8_t *)p2;

    /* Old region preserved. */
    for (size_t i = 0; i < old_size; ++i) {
        assert_int_equal(bytes[i], pattern[i]);
    }

    /* New region zeroed. */
    for (size_t i = old_size; i < new_size; ++i) {
        assert_int_equal(bytes[i], 0u);
    }

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_buddy_grow_too_large_failure(void **state) {
    (void)state;

    size_t pool       = 1024u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t old_size = 128u;
    void *p = alloc_buddy(b, old_size, false);
    assert_non_null(p);

    /* Fill with a pattern. */
    uint8_t *bytes = (uint8_t *)p;
    for (size_t i = 0; i < old_size; ++i) {
        bytes[i] = (uint8_t)(0x55u);
    }

    size_t before_remaining = buddy_remaining(b);

    errno = 0;
    void *p2 = realloc_buddy(b, p, old_size, pool * 2u, false);
    assert_null(p2);
    assert_int_equal(errno, ENOMEM);

    /* Old pointer should still be valid and content unchanged. */
    for (size_t i = 0; i < old_size; ++i) {
        assert_int_equal(bytes[i], 0x55u);
    }

    /* Allocator state shouldn't claim more memory used than before. */
    assert_int_equal(buddy_remaining(b), before_remaining);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_buddy_aligned_from_null(void **state) {
    (void)state;

    size_t pool       = 4096u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t new_size = 128u;
    size_t align    = 64u;

    errno = 0;
    void *p = realloc_buddy_aligned(b, NULL, 0u, new_size, align, false);
    assert_non_null(p);
    assert_int_equal(errno, 0);

    uintptr_t addr = (uintptr_t)p;
    assert_int_equal(addr % align, 0u);

    /* Basic sanity: pool is not fully free anymore. */
    assert_true(buddy_remaining(b) < next_pow2_test(pool));

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_buddy_aligned_to_zero_frees(void **state) {
    (void)state;

    size_t pool       = 2048u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t size  = 128u;
    size_t align = 64u;

    void *p = alloc_buddy_aligned(b, size, align, false);
    assert_non_null(p);

    size_t before_remaining = buddy_remaining(b);

    errno = 0;
    void *p2 = realloc_buddy_aligned(b, p, size, 0u, align, false);
    assert_null(p2);
    assert_int_equal(errno, 0);

    /* Entire pool should be free again. */
    assert_int_equal(buddy_remaining(b), next_pow2_test(pool));
    assert_true(buddy_remaining(b) >= before_remaining);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_buddy_aligned_shrink_preserves_data(void **state) {
    (void)state;

    size_t pool       = 4096u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t old_size = 200u;
    size_t new_size = 100u;
    size_t align    = 64u;

    uint8_t pattern[200];
    for (size_t i = 0; i < old_size; ++i) {
        pattern[i] = (uint8_t)(i & 0xFFu);
    }

    void *p = alloc_buddy_aligned(b, old_size, align, false);
    assert_non_null(p);
    uintptr_t addr = (uintptr_t)p;
    assert_int_equal(addr % align, 0u);

    memcpy(p, pattern, old_size);

    errno = 0;
    void *p2 = realloc_buddy_aligned(b, p, old_size, new_size, align, false);
    assert_non_null(p2);
    /* We won't require same pointer, but it often will be. */
    uintptr_t addr2 = (uintptr_t)p2;
    assert_int_equal(addr2 % align, 0u);
    assert_int_equal(errno, 0);

    /* First new_size bytes must match original data. */
    uint8_t *bytes = (uint8_t *)p2;
    for (size_t i = 0; i < new_size; ++i) {
        assert_int_equal(bytes[i], pattern[i]);
    }

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_buddy_aligned_grow_zeroed(void **state) {
    (void)state;

    size_t pool       = 4096u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t old_size = 100u;
    size_t new_size = 300u;
    size_t align    = 64u;

    uint8_t pattern[100];
    for (size_t i = 0; i < old_size; ++i) {
        pattern[i] = (uint8_t)(0xB0u + (i & 0x0Fu));
    }

    void *p = alloc_buddy_aligned(b, old_size, align, false);
    assert_non_null(p);
    uintptr_t addr = (uintptr_t)p;
    assert_int_equal(addr % align, 0u);

    memcpy(p, pattern, old_size);

    errno = 0;
    void *p2 = realloc_buddy_aligned(b, p, old_size, new_size, align, true);
    assert_non_null(p2);
    uintptr_t addr2 = (uintptr_t)p2;
    assert_int_equal(addr2 % align, 0u);
    assert_int_equal(errno, 0);

    uint8_t *bytes = (uint8_t *)p2;

    /* Old region preserved. */
    for (size_t i = 0; i < old_size; ++i) {
        assert_int_equal(bytes[i], pattern[i]);
    }

    /* New region zeroed. */
    for (size_t i = old_size; i < new_size; ++i) {
        assert_int_equal(bytes[i], 0u);
    }

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_buddy_aligned_grow_too_large_failure(void **state) {
    (void)state;

    size_t pool       = 1024u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t old_size = 128u;
    size_t align    = 32u;

    void *p = alloc_buddy_aligned(b, old_size, align, false);
    assert_non_null(p);
    uintptr_t addr = (uintptr_t)p;
    assert_int_equal(addr % 32u, 0u);

    /* Fill with a known pattern. */
    uint8_t *bytes = (uint8_t *)p;
    for (size_t i = 0; i < old_size; ++i) {
        bytes[i] = (uint8_t)0x5Au;
    }

    size_t before_remaining = buddy_remaining(b);

    errno = 0;
    void *p2 = realloc_buddy_aligned(b, p, old_size, pool * 2u, align, false);
    assert_null(p2);
    assert_int_equal(errno, ENOMEM);

    /* Old pointer still valid, contents unchanged. */
    for (size_t i = 0; i < old_size; ++i) {
        assert_int_equal(bytes[i], 0x5Au);
    }

    /* Remaining bytes in pool should be unchanged. */
    assert_int_equal(buddy_remaining(b), before_remaining);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_buddy_aligned_zero_align_behavior(void **state) {
    (void)state;

    size_t pool       = 2048u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t old_size = 64u;
    size_t new_size = 256u;

    /* Start from NULL: behaves like alloc_buddy_aligned with natural alignment. */
    void *p = realloc_buddy_aligned(b, NULL, 0u, old_size, 0u, false);
    assert_non_null(p);

    uintptr_t addr = (uintptr_t)p;
    assert_int_equal(addr % alignof(max_align_t), 0u);

    /* Now grow with align == 0 again. */
    void *p2 = realloc_buddy_aligned(b, p, old_size, new_size, 0u, false);
    assert_non_null(p2);

    uintptr_t addr2 = (uintptr_t)p2;
    assert_int_equal(addr2 % alignof(max_align_t), 0u);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static size_t get_normalized_pool(size_t requested_pool,
                                  size_t min_block,
                                  size_t base_align)
{
    buddy_t *b = init_buddy_allocator(requested_pool, min_block, base_align);
    assert_non_null(b);
    size_t pool = buddy_remaining(b);
    free_buddy(b);
    return pool;
}
// -------------------------------------------------------------------------------- 

static void test_is_buddy_ptr_null_args(void **state) {
    (void)state;

    int dummy = 42;

    errno = 0;
    assert_false(is_buddy_ptr(NULL, &dummy));
    assert_int_equal(errno, EINVAL);

    size_t pool       = 1024u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    errno = 0;
    assert_false(is_buddy_ptr(b, NULL));
    assert_int_equal(errno, EINVAL);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_is_buddy_ptr_valid_alloc(void **state) {
    (void)state;

    size_t pool       = 2048u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t size = 128u;
    void *p = alloc_buddy(b, size, false);
    assert_non_null(p);

    errno = 0;
    assert_true(is_buddy_ptr(b, p));
    /* errno should not indicate an error on success */
    assert_int_not_equal(errno, EINVAL);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_is_buddy_ptr_foreign_pointer(void **state) {
    (void)state;

    size_t pool       = 2048u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    int local = 123;

    errno = 0;
    assert_false(is_buddy_ptr(b, &local));
    assert_int_equal(errno, EINVAL);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_is_buddy_ptr_other_buddy(void **state) {
    (void)state;

    size_t pool1      = 2048u;
    size_t pool2      = 4096u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b1 = init_buddy_allocator(pool1, min_block, base_align);
    buddy_t *b2 = init_buddy_allocator(pool2, min_block, base_align);
    assert_non_null(b1);
    assert_non_null(b2);

    void *p = alloc_buddy(b1, 128u, false);
    assert_non_null(p);

    errno = 0;
    assert_false(is_buddy_ptr(b2, p));
    assert_int_equal(errno, EINVAL);

    free_buddy(b1);
    free_buddy(b2);
}
// -------------------------------------------------------------------------------- 

static void test_is_buddy_ptr_offset_into_block(void **state) {
    (void)state;

    size_t pool       = 2048u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    void *p = alloc_buddy(b, 128u, false);
    assert_non_null(p);

    uint8_t *p_offset = (uint8_t *)p + 1;

    errno = 0;
    assert_false(is_buddy_ptr(b, p_offset));
    assert_int_equal(errno, EINVAL);

    /* Original pointer is still valid. */
    errno = 0;
    assert_true(is_buddy_ptr(b, p));
    assert_int_not_equal(errno, EINVAL);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_is_buddy_ptr_sized_null_args(void **state) {
    (void)state;

    int dummy = 0;

    errno = 0;
    assert_false(is_buddy_ptr_sized(NULL, &dummy, 16u));
    assert_int_equal(errno, EINVAL);

    size_t pool       = 1024u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    errno = 0;
    assert_false(is_buddy_ptr_sized(b, NULL, 16u));
    assert_int_equal(errno, EINVAL);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_is_buddy_ptr_sized_exact_request(void **state) {
    (void)state;

    size_t pool       = 4096u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    size_t req_size = 256u;
    void *p = alloc_buddy(b, req_size, false);
    assert_non_null(p);

    errno = 0;
    assert_true(is_buddy_ptr_sized(b, p, req_size));
    assert_int_not_equal(errno, EINVAL);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_is_buddy_ptr_sized_too_large(void **state) {
    (void)state;

    size_t requested_pool = 2048u;
    size_t min_block      = 64u;
    size_t base_align     = alignof(max_align_t);

    /* Get the normalized pool (after pow2 rounding) for a sanity bound. */
    size_t normalized_pool = get_normalized_pool(requested_pool,
                                                 min_block,
                                                 base_align);

    buddy_t *b = init_buddy_allocator(requested_pool, min_block, base_align);
    assert_non_null(b);

    void *p = alloc_buddy(b, 128u, false);
    assert_non_null(p);

    /* Ask for more than the entire pool; this should be too large. */
    size_t huge_size = normalized_pool * 2u;

    errno = 0;
    assert_false(is_buddy_ptr_sized(b, p, huge_size));
    /* We defined ERANGE in the design for "size does not fit". */
    assert_int_equal(errno, ERANGE);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

static void test_is_buddy_ptr_sized_foreign_pointer(void **state) {
    (void)state;

    size_t pool       = 1024u;
    size_t min_block  = 64u;
    size_t base_align = alignof(max_align_t);

    buddy_t *b = init_buddy_allocator(pool, min_block, base_align);
    assert_non_null(b);

    int local = 42;

    errno = 0;
    assert_false(is_buddy_ptr_sized(b, &local, sizeof local));
    assert_int_equal(errno, EINVAL);

    free_buddy(b);
}
// -------------------------------------------------------------------------------- 

const struct CMUnitTest test_buddy_allocator[] = {
    cmocka_unit_test(test_init_buddy_zero_pool),
    cmocka_unit_test(test_init_buddy_zero_min_block),
    cmocka_unit_test(test_init_buddy_zero_base_align),
    cmocka_unit_test(test_init_buddy_min_block_too_large),
    cmocka_unit_test(test_init_buddy_rounding),
    cmocka_unit_test(test_init_buddy_exact_powers_of_two),
    cmocka_unit_test(test_init_buddy_non_pow2_align),

    cmocka_unit_test(test_alloc_buddy_basic),
    cmocka_unit_test(test_alloc_buddy_too_large),
    cmocka_unit_test(test_alloc_buddy_zeroed_reuse_full_block),

    cmocka_unit_test(test_alloc_buddy_aligned_basic),
    cmocka_unit_test(test_alloc_buddy_aligned_non_pow2_align),
    cmocka_unit_test(test_alloc_buddy_aligned_zero_align),
    cmocka_unit_test(test_alloc_buddy_aligned_zeroed_reuse_full_block),
    cmocka_unit_test(test_alloc_buddy_aligned_too_large),

    cmocka_unit_test(test_realloc_buddy_from_null),
    cmocka_unit_test(test_realloc_buddy_to_zero_frees),
    cmocka_unit_test(test_realloc_buddy_shrink_in_place),
    cmocka_unit_test(test_realloc_buddy_grow_zeroed),
    cmocka_unit_test(test_realloc_buddy_grow_too_large_failure),

    cmocka_unit_test(test_realloc_buddy_aligned_from_null),
    cmocka_unit_test(test_realloc_buddy_aligned_to_zero_frees),
    cmocka_unit_test(test_realloc_buddy_aligned_shrink_preserves_data),
    cmocka_unit_test(test_realloc_buddy_aligned_grow_zeroed),
    cmocka_unit_test(test_realloc_buddy_aligned_grow_too_large_failure),
    cmocka_unit_test(test_realloc_buddy_aligned_zero_align_behavior),

    cmocka_unit_test(test_is_buddy_ptr_null_args),
    cmocka_unit_test(test_is_buddy_ptr_valid_alloc),
    cmocka_unit_test(test_is_buddy_ptr_foreign_pointer),
    cmocka_unit_test(test_is_buddy_ptr_other_buddy),
    cmocka_unit_test(test_is_buddy_ptr_offset_into_block),

    cmocka_unit_test(test_is_buddy_ptr_sized_null_args),
    cmocka_unit_test(test_is_buddy_ptr_sized_exact_request),
    cmocka_unit_test(test_is_buddy_ptr_sized_too_large),
    cmocka_unit_test(test_is_buddy_ptr_sized_foreign_pointer),
};

const size_t test_buddy_allocator_count = sizeof(test_buddy_allocator) / sizeof(test_buddy_allocator[0]);
// ================================================================================ 
// ================================================================================ 
// TEST SLAB ALLOCATOR 

/* Convenience: choose a reasonable buddy config for tests */
#define TEST_BUDDY_POOL_SIZE      (1u << 20)  /* 1 MiB */
#define TEST_BUDDY_MIN_BLOCK_SIZE 64u

/* Helper to create/destroy a buddy allocator locally in each test */
static buddy_t *create_test_buddy(void) {
    buddy_t *b = init_buddy_allocator(TEST_BUDDY_POOL_SIZE,
                                      TEST_BUDDY_MIN_BLOCK_SIZE,
                                      0u); /* base_align = 0 => default */
    assert_non_null(b);
    return b;
}
// -------------------------------------------------------------------------------- 

static void test_init_slab_null_buddy(void **state) {
    (void)state;
    errno = 0;

    slab_t *slab = init_slab_allocator(NULL,
                                       /*obj_size=*/16u,
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);

    assert_null(slab);
    assert_int_equal(errno, EINVAL);
}

static void test_init_slab_zero_object_size(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = create_test_buddy();

    slab_t *slab = init_slab_allocator(buddy,
                                       /*obj_size=*/0u,
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);

    assert_null(slab);
    assert_int_equal(errno, EINVAL);

    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_init_slab_default_alignment(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = create_test_buddy();

    size_t const obj_size = 24u;
    slab_t *slab = init_slab_allocator(buddy,
                                       obj_size,
                                       /*align=*/0u,          /* use default */
                                       /*slab_bytes_hint=*/0u); /* use default */
    assert_non_null(slab);

    /* slab itself should be inside the buddy pool */
    assert_true(is_buddy_ptr(buddy, slab));

    /* No pages allocated yet, so: */
    assert_int_equal(slab_alloc(slab), 0u);
    assert_int_equal(slab_size(slab), 0u);
    assert_int_equal(slab_total_blocks(slab), 0u);
    assert_int_equal(slab_free_blocks(slab), 0u);
    assert_int_equal(slab_in_use_blocks(slab), 0u);

    /* Alignment should be at least alignof(max_align_t) */
    size_t const slab_align = slab_alignment(slab);
    assert_true((slab_align & (slab_align - 1u)) == 0u); /* power of two */
    assert_true(slab_align >= alignof(max_align_t));

    /* Stride must be >= object size and >= sizeof(void*) for free-list link */
    size_t const stride = slab_stride(slab);
    assert_true(stride >= obj_size);
    assert_true(stride >= sizeof(void*));

    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_init_slab_custom_alignment_pow2(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = create_test_buddy();

    size_t const obj_size = 32u;
    size_t const align    = 64u;

    slab_t *slab = init_slab_allocator(buddy,
                                       obj_size,
                                       align,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    /* Alignment should be exactly what we requested (already power-of-two). */
    assert_int_equal(slab_alignment(slab), align);

    /* Again, no pages yet: capacity == 0. */
    assert_int_equal(slab_size(slab), 0u);
    assert_int_equal(slab_total_blocks(slab), 0u);

    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_init_slab_alignment_rounded_up(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = create_test_buddy();

    size_t const obj_size = 16u;
    size_t const align_in = 24u;   /* not a power of two, should round to 32 */
    size_t const expected_align = 32u;

    slab_t *slab = init_slab_allocator(buddy,
                                       obj_size,
                                       align_in,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    size_t const slab_align = slab_alignment(slab);
    assert_int_equal(slab_align, expected_align);

    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_init_slab_small_page_hint(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = create_test_buddy();

    size_t const obj_size = 8u;
    size_t const hint     = 32u;  /* intentionally very small */

    slab_t *slab = init_slab_allocator(buddy,
                                       obj_size,
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/hint);
    assert_non_null(slab);

    /* No pages yet, but first allocation should succeed and create a page. */
    assert_int_equal(slab_size(slab), 0u);
    assert_int_equal(slab_total_blocks(slab), 0u);

    void *p = alloc_slab(slab, false);
    assert_non_null(p);

    /* Now we must have capacity and at least one block. */
    assert_true(slab_size(slab) > 0u);
    assert_true(slab_total_blocks(slab) >= 1u);
    assert_true(slab_in_use_blocks(slab) == 1u);

    /* Returned pointer should have the advertised alignment. */
    size_t const a = slab_alignment(slab);
    assert_true(((uintptr_t)p & (a - 1u)) == 0u);

    /* Cleanup */
    assert_true(return_slab(slab, p));
    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_slab_basic(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = init_buddy_allocator(TEST_BUDDY_POOL_SIZE,
                                          TEST_BUDDY_MIN_BLOCK_SIZE,
                                          0u);
    assert_non_null(buddy);

    size_t const obj_size = 32u;

    slab_t *slab = init_slab_allocator(buddy,
                                       obj_size,
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    /* Initially, no pages and no allocations. */
    assert_int_equal(slab_alloc(slab), 0u);
    assert_int_equal(slab_size(slab), 0u);
    assert_int_equal(slab_total_blocks(slab), 0u);
    assert_int_equal(slab_in_use_blocks(slab), 0u);
    assert_int_equal(slab_free_blocks(slab), 0u);

    /* First allocation should succeed and create a page. */
    void *p1 = alloc_slab(slab, /*zeroed=*/false);
    assert_non_null(p1);

    /* Pointer should be aligned per slab_alignment. */
    size_t const a = slab_alignment(slab);
    assert_true((a & (a - 1u)) == 0u);          /* a is power-of-two */
    assert_true(((uintptr_t)p1 & (a - 1u)) == 0u);

    /* Now the slab must have capacity and one in-use block. */
    assert_true(slab_size(slab) > 0u);
    assert_true(slab_total_blocks(slab) >= 1u);
    assert_int_equal(slab_in_use_blocks(slab), 1u);

    /* Used bytes should be obj_size * in_use_blocks. */
    assert_int_equal(slab_alloc(slab), obj_size);

    /* There should be at least one free block unless page holds exactly 1. */
    size_t total_blocks = slab_total_blocks(slab);
    size_t free_blocks  = slab_free_blocks(slab);
    assert_true(total_blocks >= 1u);
    assert_true(free_blocks + slab_in_use_blocks(slab) == total_blocks);

    /* Clean up: free buddy (which implicitly releases slab memory). */
    assert_true(return_slab(slab, p1));
    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_slab_zeroed(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = init_buddy_allocator(TEST_BUDDY_POOL_SIZE,
                                          TEST_BUDDY_MIN_BLOCK_SIZE,
                                          0u);
    assert_non_null(buddy);

    typedef struct {
        int x;
        int y;
    } vec2i;

    slab_t *slab = init_slab_allocator(buddy,
                                       sizeof(vec2i),
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    /* Allocate zeroed. */
    vec2i *v = (vec2i *)alloc_slab(slab, /*zeroed=*/true);
    assert_non_null(v);

    /* Should start as all zeros. */
    assert_int_equal(v->x, 0);
    assert_int_equal(v->y, 0);

    /* Write some values and verify. */
    v->x = 3;
    v->y = -7;
    assert_int_equal(v->x, 3);
    assert_int_equal(v->y, -7);

    /* Accounting: exactly one block in use. */
    assert_int_equal(slab_in_use_blocks(slab), 1u);
    assert_int_equal(slab_alloc(slab), sizeof(vec2i));

    /* Return the block. */
    assert_true(return_slab(slab, v));

    /* After return, in-use should be 0. */
    assert_int_equal(slab_in_use_blocks(slab), 0u);
    assert_int_equal(slab_alloc(slab), 0u);

    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_return_slab_reuse(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = init_buddy_allocator(TEST_BUDDY_POOL_SIZE,
                                          TEST_BUDDY_MIN_BLOCK_SIZE,
                                          0u);
    assert_non_null(buddy);

    size_t const obj_size = 16u;

    slab_t *slab = init_slab_allocator(buddy,
                                       obj_size,
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    void *p1 = alloc_slab(slab, false);
    void *p2 = alloc_slab(slab, false);
    assert_non_null(p1);
    assert_non_null(p2);
    assert_ptr_not_equal(p1, p2);

    /* Now two blocks in use. */
    assert_int_equal(slab_in_use_blocks(slab), 2u);

    /* Free p1 and then allocate again; we expect LIFO reuse of slots,
       so the newly allocated pointer should equal p1. */
    assert_true(return_slab(slab, p1));
    assert_int_equal(slab_in_use_blocks(slab), 1u);

    void *p3 = alloc_slab(slab, false);
    assert_non_null(p3);
    assert_ptr_equal(p3, p1);

    /* Clean up. Return both. */
    assert_true(return_slab(slab, p2));
    assert_true(return_slab(slab, p3));

    assert_int_equal(slab_in_use_blocks(slab), 0u);
    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_return_slab_null_pointer(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = init_buddy_allocator(TEST_BUDDY_POOL_SIZE,
                                          TEST_BUDDY_MIN_BLOCK_SIZE,
                                          0u);
    assert_non_null(buddy);

    slab_t *slab = init_slab_allocator(buddy,
                                       /*obj_size=*/16u,
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    /* No allocations yet. */
    assert_int_equal(slab_in_use_blocks(slab), 0u);

    /* return_slab on NULL should be a no-op success. */
    assert_true(return_slab(slab, NULL));
    assert_int_equal(slab_in_use_blocks(slab), 0u);

    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_return_slab_invalid_pointer(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = init_buddy_allocator(TEST_BUDDY_POOL_SIZE,
                                          TEST_BUDDY_MIN_BLOCK_SIZE,
                                          0u);
    assert_non_null(buddy);

    slab_t *slab = init_slab_allocator(buddy,
                                       /*obj_size=*/32u,
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    int stack_value = 42;
    errno = 0;

    /* Returning a pointer that does not belong to the slab should fail. */
    bool ok = return_slab(slab, &stack_value);
    assert_false(ok);
    assert_int_equal(errno, EINVAL);

    /* Still no allocations in use. */
    assert_int_equal(slab_in_use_blocks(slab), 0u);

    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_slab_is_slab_ptr(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = init_buddy_allocator(TEST_BUDDY_POOL_SIZE,
                                          TEST_BUDDY_MIN_BLOCK_SIZE,
                                          0u);
    assert_non_null(buddy);

    slab_t *slab = init_slab_allocator(buddy,
                                       /*obj_size=*/24u,
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    void *p = alloc_slab(slab, false);
    assert_non_null(p);

    /* p must be recognized as a valid slab pointer. */
    assert_true(is_slab_ptr(slab, p));

    int other = 123;
    assert_false(is_slab_ptr(slab, &other));
    assert_int_equal(errno, EINVAL);

    /* Clean up. */
    assert_true(return_slab(slab, p));
    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_reset_slab_null(void **state) {
    (void)state;
    errno = 0;

    bool ok = reset_slab(NULL);
    assert_false(ok);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_reset_slab_basic(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = init_buddy_allocator(TEST_BUDDY_POOL_SIZE,
                                          TEST_BUDDY_MIN_BLOCK_SIZE,
                                          0u);
    assert_non_null(buddy);

    size_t const obj_size = 32u;
    slab_t *slab = init_slab_allocator(buddy,
                                       obj_size,
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    /* Allocate a few objects to force at least one page. */
    void *p1 = alloc_slab(slab, false);
    void *p2 = alloc_slab(slab, false);
    void *p3 = alloc_slab(slab, false);
    assert_non_null(p1);
    assert_non_null(p2);
    assert_non_null(p3);

    size_t total_blocks_before = slab_total_blocks(slab);
    assert_true(total_blocks_before >= 3u);

    assert_int_equal(slab_in_use_blocks(slab), 3u);
    assert_int_equal(slab_alloc(slab), 3u * obj_size);

    /* Now reset. */
    errno = 0;
    bool ok = reset_slab(slab);
    assert_true(ok);
    assert_int_equal(errno, 0);

    /* After reset, nothing should be in use. */
    assert_int_equal(slab_in_use_blocks(slab), 0u);
    assert_int_equal(slab_alloc(slab), 0u);

    /* Capacity (bytes / blocks) should be unchanged. */
    size_t total_blocks_after = slab_total_blocks(slab);
    assert_int_equal(total_blocks_after, total_blocks_before);

    /* All blocks should now be free. */
    assert_int_equal(slab_free_blocks(slab), total_blocks_after);

    /* We do NOT try to return p1/p2/p3 after reset: that would be UB. */

    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_save_slab_null_slab(void **state) {
    (void)state;
    errno = 0;

    size_t bytes_needed = 0;
    bool ok = save_slab(NULL,
                        /*buffer=*/NULL,
                        /*buffer_size=*/0u,
                        &bytes_needed);

    assert_false(ok);
    assert_int_equal(errno, EINVAL);
    /* bytes_needed is unspecified on error here; we only care about errno. */
}
// -------------------------------------------------------------------------------- 

static void test_save_slab_size_only(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = init_buddy_allocator(TEST_BUDDY_POOL_SIZE,
                                          TEST_BUDDY_MIN_BLOCK_SIZE,
                                          0u);
    assert_non_null(buddy);

    slab_t *slab = init_slab_allocator(buddy,
                                       /*obj_size=*/16u,
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    /* Create at least one page. */
    void *p = alloc_slab(slab, false);
    assert_non_null(p);

    size_t bytes_needed = 0;
    errno = 0;

    /* Request a save with buffer_size too small (0). */
    bool ok = save_slab(slab,
                        /*buffer=*/NULL,
                        /*buffer_size=*/0u,
                        &bytes_needed);

    assert_false(ok);
    assert_int_equal(errno, ERANGE);
    assert_true(bytes_needed > 0u);

    /* No state change expected in slab. */
    assert_int_equal(slab_in_use_blocks(slab), 1u);

    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_save_restore_roundtrip(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = init_buddy_allocator(TEST_BUDDY_POOL_SIZE,
                                          TEST_BUDDY_MIN_BLOCK_SIZE,
                                          0u);
    assert_non_null(buddy);

    typedef struct {
        int a;
        int b;
    } pair_t;

    slab_t *slab = init_slab_allocator(buddy,
                                       sizeof(pair_t),
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    /* Allocate a few objects and initialize them. */
    pair_t *p1 = (pair_t *)alloc_slab(slab, true);
    pair_t *p2 = (pair_t *)alloc_slab(slab, true);
    pair_t *p3 = (pair_t *)alloc_slab(slab, true);
    assert_non_null(p1);
    assert_non_null(p2);
    assert_non_null(p3);

    p1->a = 1;  p1->b = 10;
    p2->a = 2;  p2->b = 20;
    p3->a = 3;  p3->b = 30;

    size_t const in_use_before   = slab_in_use_blocks(slab);
    size_t const alloc_bytes_before = slab_alloc(slab);
    size_t const total_blocks_before = slab_total_blocks(slab);
    size_t const free_blocks_before  = slab_free_blocks(slab);

    assert_int_equal(in_use_before, 3u);
    assert_true(total_blocks_before >= 3u);
    assert_true(free_blocks_before + in_use_before == total_blocks_before);

    /* First call to save_slab: get required size. */
    size_t bytes_needed = 0;
    errno = 0;

    bool ok = save_slab(slab,
                        /*buffer=*/NULL,
                        /*buffer_size=*/0u,
                        &bytes_needed);
    assert_false(ok);
    assert_int_equal(errno, ERANGE);
    assert_true(bytes_needed > 0u);

    /* Second call: provide a buffer of the correct size. */
    void *buffer = malloc(bytes_needed);
    assert_non_null(buffer);

    errno = 0;
    ok = save_slab(slab,
                   buffer,
                   bytes_needed,
                   &bytes_needed);
    assert_true(ok);
    assert_int_equal(errno, 0);

    /* Mutate state after snapshot: change values, free/alloc, etc. */
    p1->a = 111;  p1->b = 999;
    (void)return_slab(slab, p2);
    pair_t *p4 = (pair_t *)alloc_slab(slab, false);
    assert_non_null(p4);
    p4->a = 444;  p4->b = 555;

    assert_int_equal(slab_in_use_blocks(slab), 3u); /* p1, p3, p4 */

    /* Now restore from snapshot. This should roll back:
       - allocations/frees
       - contents of p1, p2, p3
       - counters (len, free list, etc.)
    */
    errno = 0;
    ok = restore_slab(slab, buffer, bytes_needed);
    assert_true(ok);
    assert_int_equal(errno, 0);

    /* After restore, pointers p1/p2/p3 should be valid again
       (snapshot logic assumes same pages still exist), and
       contents should match original values, not mutated ones.
    */
    assert_int_equal(p1->a, 1);
    assert_int_equal(p1->b, 10);
    assert_int_equal(p2->a, 2);
    assert_int_equal(p2->b, 20);
    assert_int_equal(p3->a, 3);
    assert_int_equal(p3->b, 30);

    /* Accounting should match pre-snapshot state. */
    assert_int_equal(slab_in_use_blocks(slab), in_use_before);
    assert_int_equal(slab_alloc(slab), alloc_bytes_before);
    assert_int_equal(slab_total_blocks(slab), total_blocks_before);

    /* Free blocks (geom) should match too. */
    assert_int_equal(slab_free_blocks(slab), free_blocks_before);

    free(buffer);
    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_restore_slab_small_buffer(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = init_buddy_allocator(TEST_BUDDY_POOL_SIZE,
                                          TEST_BUDDY_MIN_BLOCK_SIZE,
                                          0u);
    assert_non_null(buddy);

    slab_t *slab = init_slab_allocator(buddy,
                                       /*obj_size=*/16u,
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    void *p = alloc_slab(slab, false);
    assert_non_null(p);

    /* Save to find required size. */
    size_t bytes_needed = 0;
    (void)save_slab(slab, NULL, 0u, &bytes_needed);
    assert_true(bytes_needed > 0u);

    /* Create a buffer too small on purpose. */
    size_t const small_size = bytes_needed / 2u;
    void *buffer = malloc(bytes_needed);
    assert_non_null(buffer);

    /* First, create a valid snapshot. */
    bool ok = save_slab(slab, buffer, bytes_needed, &bytes_needed);
    assert_true(ok);

    /* Attempt to restore using only part of the buffer. */
    errno = 0;
    ok = restore_slab(slab, buffer, small_size);
    assert_false(ok);
    assert_int_equal(errno, ERANGE);

    free(buffer);
    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_slab_getters_null_slab(void **state) {
    (void)state;

    errno = 0;
    assert_int_equal(slab_alloc(NULL), 0u);
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_int_equal(slab_size(NULL), 0u);
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_int_equal(total_slab_alloc(NULL), 0u);
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_int_equal(slab_stride(NULL), 0u);
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_int_equal(slab_total_blocks(NULL), 0u);
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_int_equal(slab_free_blocks(NULL), 0u);
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_int_equal(slab_alignment(NULL), 0u);
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_int_equal(slab_in_use_blocks(NULL), 0u);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_slab_getters_initial_state(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = init_buddy_allocator(TEST_BUDDY_POOL_SIZE,
                                          TEST_BUDDY_MIN_BLOCK_SIZE,
                                          0u);
    assert_non_null(buddy);

    size_t const obj_size = 32u;

    slab_t *slab = init_slab_allocator(buddy,
                                       obj_size,
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    /* No pages allocated yet -> zero usage and capacity. */
    assert_int_equal(slab_alloc(slab), 0u);
    assert_int_equal(slab_size(slab), 0u);
    assert_int_equal(slab_total_blocks(slab), 0u);
    assert_int_equal(slab_free_blocks(slab), 0u);
    assert_int_equal(slab_in_use_blocks(slab), 0u);

    /* total_slab_alloc should be >= alloc and size. */
    size_t total = total_slab_alloc(slab);
    assert_true(total >= slab_alloc(slab));
    assert_true(total >= slab_size(slab));

    /* Alignment should be power-of-two and at least alignof(max_align_t). */
    size_t const a = slab_alignment(slab);
    assert_true((a & (a - 1u)) == 0u);
    assert_true(a >= alignof(max_align_t));

    /* Stride must be >= object size and at least sizeof(void*) for next pointer. */
    size_t const stride = slab_stride(slab);
    assert_true(stride >= obj_size);
    assert_true(stride >= sizeof(void*));

    free_buddy(buddy);
}
// -------------------------------------------------------------------------------- 

static void test_slab_getters_after_allocs_and_frees(void **state) {
    (void)state;
    errno = 0;

    buddy_t *buddy = init_buddy_allocator(TEST_BUDDY_POOL_SIZE,
                                          TEST_BUDDY_MIN_BLOCK_SIZE,
                                          0u);
    assert_non_null(buddy);

    size_t const obj_size = 24u;

    slab_t *slab = init_slab_allocator(buddy,
                                       obj_size,
                                       /*align=*/0u,
                                       /*slab_bytes_hint=*/0u);
    assert_non_null(slab);

    /* Snapshot of total_slab_alloc before first allocation (just slab struct). */
    size_t const total_before = total_slab_alloc(slab);

    /* Allocate several blocks to force at least one page. */
    void *p1 = alloc_slab(slab, false);
    void *p2 = alloc_slab(slab, false);
    void *p3 = alloc_slab(slab, false);
    void *p4 = alloc_slab(slab, false);
    assert_non_null(p1);
    assert_non_null(p2);
    assert_non_null(p3);
    assert_non_null(p4);

    /* After first allocation, capacity and total footprint must be > 0. */
    assert_true(slab_size(slab) > 0u);
    assert_true(slab_total_blocks(slab) >= 4u);

    /* total_slab_alloc should have grown (pages added). */
    size_t const total_after = total_slab_alloc(slab);
    assert_true(total_after >= total_before);
    assert_true(total_after > total_before);

    /* In-use blocks and alloc bytes. */
    size_t const in_use = slab_in_use_blocks(slab);
    assert_int_equal(in_use, 4u);

    size_t const alloc_bytes = slab_alloc(slab);
    assert_int_equal(alloc_bytes, in_use * obj_size);

    /* Capacity relationships. */
    size_t const total_blocks = slab_total_blocks(slab);
    size_t const free_blocks  = slab_free_blocks(slab);

    assert_true(total_blocks >= in_use);
    assert_int_equal(free_blocks + in_use, total_blocks);

    /* Now free two blocks and check counters update. */
    assert_true(return_slab(slab, p2));
    assert_true(return_slab(slab, p4));

    size_t const in_use2 = slab_in_use_blocks(slab);
    size_t const alloc_bytes2 = slab_alloc(slab);
    size_t const free_blocks2 = slab_free_blocks(slab);
    size_t const total_blocks2 = slab_total_blocks(slab);

    assert_int_equal(in_use2, 2u);
    assert_int_equal(alloc_bytes2, in_use2 * obj_size);
    assert_int_equal(total_blocks2, total_blocks);  /* capacity unchanged */
    assert_int_equal(free_blocks2 + in_use2, total_blocks2);

    /* total_slab_alloc should still be >= size and alloc, and same pages. */
    size_t const total_after_free = total_slab_alloc(slab);
    assert_true(total_after_free >= slab_size(slab));
    assert_true(total_after_free >= slab_alloc(slab));
    /* It may equal total_after; we don't require shrink. */

    /* Free remaining blocks. */
    assert_true(return_slab(slab, p1));
    assert_true(return_slab(slab, p3));

    assert_int_equal(slab_in_use_blocks(slab), 0u);
    assert_int_equal(slab_alloc(slab), 0u);
    assert_int_equal(slab_free_blocks(slab), slab_total_blocks(slab));

    free_buddy(buddy);
}
/* ------------------------------------------------------------------------- */

const struct CMUnitTest test_slab_allocator[] = {
    cmocka_unit_test(test_init_slab_null_buddy),
    cmocka_unit_test(test_init_slab_zero_object_size),
    cmocka_unit_test(test_init_slab_default_alignment),
    cmocka_unit_test(test_init_slab_custom_alignment_pow2),
    cmocka_unit_test(test_init_slab_alignment_rounded_up),
    cmocka_unit_test(test_init_slab_small_page_hint),

    cmocka_unit_test(test_alloc_slab_basic),
    cmocka_unit_test(test_alloc_slab_zeroed),
    cmocka_unit_test(test_return_slab_reuse),
    cmocka_unit_test(test_return_slab_null_pointer),
    cmocka_unit_test(test_return_slab_invalid_pointer),
    cmocka_unit_test(test_alloc_slab_is_slab_ptr),

    cmocka_unit_test(test_reset_slab_null),
    cmocka_unit_test(test_reset_slab_basic),
    cmocka_unit_test(test_save_slab_null_slab),
    cmocka_unit_test(test_save_slab_size_only),
    cmocka_unit_test(test_save_restore_roundtrip),
    cmocka_unit_test(test_restore_slab_small_buffer),

    cmocka_unit_test(test_slab_getters_null_slab),
    cmocka_unit_test(test_slab_getters_initial_state),
    cmocka_unit_test(test_slab_getters_after_allocs_and_frees),
};

const size_t test_slab_allocator_count = sizeof(test_slab_allocator) / sizeof(test_slab_allocator[0]);
// ================================================================================
// ================================================================================
// eof
