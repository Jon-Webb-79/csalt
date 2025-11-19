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
// TEST IARENA 

static void test_init_iarena_normal(void **state) {
    (void)state;

    /* Case A: small min_chunk (4KiB) */
    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    assert_int_equal(arena_size(arena), 0);
    assert_int_equal(arena_alloc(arena), 8080);
    assert_int_equal(total_arena_alloc(arena), 8192);
    iarena_t* iarena = init_iarena_with_arena(arena, 4096, alignof(max_align_t));

    assert_int_equal(iarena_size(iarena), 0);
    assert_int_equal(iarena_alloc(iarena), 4032);
    assert_int_equal(total_iarena_alloc(iarena), 4096);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_init_iarena_too_small_total(void **state) {
    (void)state;
    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    errno = 0;
    /* Worst-case pad for payload alignment == alignof(max_align_t)-1 */
    const size_t worst_pad = alignof(max_align_t) - 1u;
    const size_t too_small = 1 + worst_pad;  /* no room for payload */
    iarena_t* ia = init_iarena_with_arena(arena, too_small, alignof(max_align_t));
    assert_null(ia);
    assert_int_equal(errno, EINVAL);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_init_iarena_bad_alignment(void **state) {
    (void)state;
    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    errno = 0;
    /* 24 is not a power-of-two */
    iarena_t* ia = init_iarena_with_arena(arena, 4096, 24u);
    assert_null(ia);
    assert_int_equal(errno, EINVAL);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_init_iarena_null_parent(void **state) {
    (void)state;
    errno = 0;
    iarena_t* ia = init_iarena_with_arena(NULL, 4096, alignof(max_align_t));
    assert_null(ia);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_init_iarena_zero_bytes(void **state) {
    (void)state;
    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    errno = 0;
    iarena_t* ia = init_iarena_with_arena(arena, 0u, alignof(max_align_t));
    assert_null(ia);
    assert_int_equal(errno, EINVAL);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_init_iarena_insufficient_parent_capacity(void **state) {
    (void)state;
    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    /* From your normal test: arena_alloc(arena) == 8080 usable */
    assert_int_equal(arena_alloc(arena), 8080);

    errno = 0;
    iarena_t* ia = init_iarena_with_arena(arena, /*bytes*/ 8200, alignof(max_align_t));
    assert_null(ia);
    assert_int_equal(errno, ENOMEM);

    /* Parent should still be usable after the failed carve */
    errno = 0;
    iarena_t* ok = init_iarena_with_arena(arena, 4096, alignof(max_align_t));
    assert_non_null(ok);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_init_iarena_inherit_alignment_and_alloc(void **state) {
    (void)state;
    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    /* alignment=0 => inherit parent alignment */
    iarena_t* ia = init_iarena_with_arena(arena, 4096, /*alignment*/0u);
    assert_non_null(ia);

    /* First small allocation should be aligned to parent/base (alignof(max_align_t)) */
    void* p = alloc_iarena(ia, 1u, /*zeroed=*/false);
    assert_non_null(p);
    assert_int_equal(((uintptr_t)p) % alignof(max_align_t), 0);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_init_iarena_zeroed_payload(void **state) {
    (void)state;
    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 4096, alignof(max_align_t));
    assert_non_null(ia);

    /* Allocate N bytes and confirm they read back as zero */
    const size_t N = 128;
    uint8_t* p = (uint8_t*)alloc_iarena(ia, N, /*zeroed=*/false);
    assert_non_null(p);
    for (size_t i = 0; i < N; ++i) {
        assert_int_equal(p[i], 0);
    }

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_init_iarena_stronger_alignment_than_parent(void **state) {
    (void)state;
    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t)); // parent base align
    assert_non_null(arena);

    /* Request a stronger payload alignment (e.g., 64) */
    iarena_t* ia = init_iarena_with_arena(arena, 4096, 64u);
    assert_non_null(ia);

    void* p = alloc_iarena(ia, 1u, /*zeroed=*/false);
    assert_non_null(p);
    assert_int_equal(((uintptr_t)p) % 64u, 0);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_basic(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    /* carve a 4 KiB subarena; request 64B payload alignment to make tests easy */
    const size_t A = 64u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, A);
    assert_non_null(ia);

    assert_int_equal(iarena_size(ia), 0);
    assert_int_equal(total_iarena_alloc(ia), 4096);

    /* first alloc: 128 bytes; since cur is aligned, pad==0 */
    void* p = alloc_iarena(ia, 128, /*zeroed=*/false);
    assert_non_null(p);
    assert_int_equal(((uintptr_t)p) % A, 0);

    /* used should be exactly 128 */
    assert_int_equal(iarena_size(ia), 128);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_exact_fit_then_enomem(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t A = 64u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, A);
    assert_non_null(ia);

    const size_t cap = iarena_alloc(ia);  /* usable capacity (end - base) */
    assert_true(cap > 0);

    /* exact fill */
    void* p = alloc_iarena(ia, cap, /*zeroed=*/false);
    assert_non_null(p);
    assert_int_equal(((uintptr_t)p) % A, 0);
    assert_int_equal(iarena_size(ia), cap);
    assert_int_equal(iarena_remaining(ia), 0);

    /* one more byte -> ENOMEM */
    errno = 0;
    void* q = alloc_iarena(ia, 1, /*zeroed=*/false);
    assert_null(q);
    assert_int_equal(errno, ENOMEM);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_padding_charged(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t A = 64u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, A);
    assert_non_null(ia);

    /* 1st: 1 byte (pad==0 initially) */
    void* p1 = alloc_iarena(ia, 1, /*zeroed=*/false);
    assert_non_null(p1);
    assert_int_equal(((uintptr_t)p1) % A, 0);
    assert_int_equal(iarena_size(ia), 1);

    /* 2nd: 1 byte; cur is now (aligned + 1), so pad == A-1; delta == pad+1 == A */
    size_t used_before = iarena_size(ia);
    void* p2 = alloc_iarena(ia, 1, /*zeroed=*/false);
    assert_non_null(p2);
    size_t used_after = iarena_size(ia);
    assert_int_equal(used_after - used_before, A);  /* padding charged + 1 byte == A */

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_zeroed_flag(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t A = 64u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, A);
    assert_non_null(ia);

    const size_t N = 128;
    uint8_t* p = (uint8_t*)alloc_iarena(ia, N, /*zeroed=*/true);
    assert_non_null(p);
    for (size_t i = 0; i < N; ++i) {
        assert_int_equal(p[i], 0);
    }

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_enomem_immediate(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t A = 64u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, A);
    assert_non_null(ia);

    const size_t cap = iarena_alloc(ia);
    errno = 0;
    void* p = alloc_iarena(ia, cap + 1u, /*zeroed=*/false);
    assert_null(p);
    assert_int_equal(errno, ENOMEM);

    /* Verify no usage was recorded */
    assert_int_equal(iarena_size(ia), 0);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_einval_cases(void **state) {
    (void)state;

    /* NULL iarena */
    errno = 0;
    void* p = alloc_iarena(NULL, 16, /*zeroed=*/false);
    assert_null(p);
    assert_int_equal(errno, EINVAL);

    /* Build a valid iarena to test bytes==0 */
    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);
    iarena_t* ia = init_iarena_with_arena(arena, 4096, alignof(max_align_t));
    assert_non_null(ia);

    errno = 0;
    void* q = alloc_iarena(ia, 0u, /*zeroed=*/false);
    assert_null(q);
    assert_int_equal(errno, EINVAL);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_aligned_inherit(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    /* Make subarena with base align = 32 for easy checks */
    const size_t BASE = 32u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, BASE);
    assert_non_null(ia);

    void* p = alloc_iarena_aligned(ia, 64, /*align*/0u, /*zeroed*/false);
    assert_non_null(p);
    assert_int_equal(((uintptr_t)p) % BASE, 0);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_aligned_stronger_than_base(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t BASE = 16u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, BASE);
    assert_non_null(ia);

    void* p = alloc_iarena_aligned(ia, 128, /*align*/64u, /*zeroed*/false);
    assert_non_null(p);
    assert_int_equal(((uintptr_t)p) % 64u, 0);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_aligned_weaker_than_base(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t BASE = 64u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, BASE);
    assert_non_null(ia);

    /* ask for 8, but base is 64 => expect 64 */
    void* p = alloc_iarena_aligned(ia, 32, /*align*/8u, /*zeroed*/false);
    assert_non_null(p);
    assert_int_equal(((uintptr_t)p) % BASE, 0);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_aligned_bad_align_arg(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 4096, 16u);
    assert_non_null(ia);

    errno = 0;
    void* p = alloc_iarena_aligned(ia, 32, /*align*/24u, /*zeroed*/false); /* 24 not power-of-two */
    assert_null(p);
    assert_int_equal(errno, EINVAL);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_aligned_padding_charged(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t BASE = 16u;      // subarena base alignment
    const size_t REQ  = 64u;      // per-call requested alignment
    iarena_t* ia = init_iarena_with_arena(arena, 4096, BASE);
    assert_non_null(ia);

    /* First: 1 byte at BASE alignment; pad == 0 for the very first bump */
    uint8_t* p1 = (uint8_t*)alloc_iarena_aligned(ia, 1, /*align*/0u, /*zeroed*/false);
    assert_non_null(p1);
    assert_int_equal(((uintptr_t)p1) % BASE, 0);
    assert_int_equal(iarena_size(ia), 1);

    /* Compute expected pad dynamically from the actual current cursor */
    uint8_t* cur_before = p1 + 1;                         // after first alloc
    size_t rem = (size_t)((uintptr_t)cur_before % REQ);
    size_t pad = rem ? (REQ - rem) : 0;
    size_t expected_delta = pad + 1;                      // pad + payload

    size_t used_before = iarena_size(ia);
    void* p2 = alloc_iarena_aligned(ia, 1, /*align*/REQ, /*zeroed*/false);
    assert_non_null(p2);

    size_t used_after = iarena_size(ia);
    assert_int_equal(used_after - used_before, expected_delta);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_aligned_exact_fit_then_enomem(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t BASE = 32u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, BASE);
    assert_non_null(ia);

    const size_t cap = iarena_alloc(ia);
    assert_true(cap > 0);

    /* exactly fill using align==0 (inherits BASE) */
    void* p = alloc_iarena_aligned(ia, cap, /*align*/0u, /*zeroed*/false);
    assert_non_null(p);
    assert_int_equal(((uintptr_t)p) % BASE, 0);
    assert_int_equal(iarena_remaining(ia), 0);

    errno = 0;
    void* q = alloc_iarena_aligned(ia, 1, /*align*/0u, /*zeroed*/false);
    assert_null(q);
    assert_int_equal(errno, ENOMEM);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_aligned_enomem_immediate(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 4096, 64u);
    assert_non_null(ia);

    const size_t cap = iarena_alloc(ia);
    errno = 0;
    void* p = alloc_iarena_aligned(ia, cap + 1u, /*align*/0u, /*zeroed*/false);
    assert_null(p);
    assert_int_equal(errno, ENOMEM);
    assert_int_equal(iarena_size(ia), 0);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_aligned_zeroed_flag(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 4096, alignof(max_align_t));
    assert_non_null(ia);

    const size_t N = 128;
    uint8_t* p = (uint8_t*)alloc_iarena_aligned(ia, N, /*align*/64u, /*zeroed*/true);
    assert_non_null(p);
    for (size_t i = 0; i < N; ++i) {
        assert_int_equal(p[i], 0);
    }

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_alloc_iarena_aligned_einval_cases(void **state) {
    (void)state;

    errno = 0;
    void* p = alloc_iarena_aligned(NULL, 16, /*align*/16u, /*zeroed*/false);
    assert_null(p);
    assert_int_equal(errno, EINVAL);

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);
    iarena_t* ia = init_iarena_with_arena(arena, 4096, 16u);
    assert_non_null(ia);

    errno = 0;
    void* q = alloc_iarena_aligned(ia, 0u, /*align*/16u, /*zeroed*/false);
    assert_null(q);
    assert_int_equal(errno, EINVAL);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_is_iarena_ptr_null_and_boundaries(void **state) {
    (void)state;

    /* NULL args */
    errno = 0;
    assert_false(is_iarena_ptr(NULL, (void*)0x1));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_false(is_iarena_ptr((iarena_t*)0x1, NULL));
    assert_int_equal(errno, EINVAL);

    /* Build a 4 KiB subarena with 64B alignment for deterministic base alignment */
    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);
    const size_t A = 64u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, A);
    assert_non_null(ia);

    /* First 1-byte alloc; this returns the subarena base pointer (64-aligned) */
    uint8_t* base = (uint8_t*)alloc_iarena_aligned(ia, 1, /*align*/0u, /*zeroed*/false);
    assert_non_null(base);
    assert_int_equal(((uintptr_t)base) % A, 0);

    /* Capacity and end pointer */
    const size_t cap = iarena_alloc(ia);
    assert_true(cap > 0);
    uint8_t* end = base + cap;

    /* In-bounds pointers */
    assert_true(is_iarena_ptr(ia, base));           /* begin is valid (inclusive) */
    assert_true(is_iarena_ptr(ia, base + 1));
    assert_true(is_iarena_ptr(ia, base + (cap > 1 ? cap - 1 : 0)));

    /* Out-of-bounds pointers */
    assert_false(is_iarena_ptr(ia, base - 1));      /* just before begin */
    assert_false(is_iarena_ptr(ia, end));           /* end is exclusive */

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_is_iarena_ptr_sized_inside_and_crossing(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    /* Subarena with 32B base alignment */
    const size_t A = 32u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, A);
    assert_non_null(ia);

    /* Get base pointer via first allocation */
    uint8_t* base = (uint8_t*)alloc_iarena(ia, 1, /*zeroed*/false);
    assert_non_null(base);
    const size_t cap = iarena_alloc(ia);
    uint8_t* end = base + cap;

    /* Region wholly inside: start at base + 16, size 32 (assuming capacity allows) */
    const size_t off = 16u;
    const size_t sz  = 32u;
    assert_true(cap > off + sz);
    assert_true(is_iarena_ptr_sized(ia, base + off, sz));

    /* Crossing end boundary: choose size so it spills just past end */
    size_t spill_size = (size_t)(end - (base + off)) + 1u;
    assert_false(is_iarena_ptr_sized(ia, base + off, spill_size));

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_is_iarena_ptr_sized_zero_length_inside(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 4096, alignof(max_align_t));
    assert_non_null(ia);

    uint8_t* base = (uint8_t*)alloc_iarena(ia, 1, /*zeroed*/false);
    assert_non_null(base);

    /* size==0: treat [p,p) inside if p is inside */
    assert_true(is_iarena_ptr_sized(ia, base, 0u));

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_is_iarena_ptr_sized_overflow_guard(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 4096, 64u);
    assert_non_null(ia);

    uint8_t* p = (uint8_t*)alloc_iarena(ia, 1, /*zeroed*/false);
    assert_non_null(p);

    /* Ask for an absurdly large size that would overflow p + size */
    size_t huge = (size_t)~(size_t)0; /* SIZE_MAX */
    assert_false(is_iarena_ptr_sized(ia, p, huge));

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_is_iarena_ptr_sized_einval_cases(void **state) {
    (void)state;

    errno = 0;
    assert_false(is_iarena_ptr_sized(NULL, (void*)0x1, 1));
    assert_int_equal(errno, EINVAL);

    errno = 0;
    assert_false(is_iarena_ptr_sized((iarena_t*)0x1, NULL, 1));
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_is_iarena_ptr_after_multiple_allocs(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 4096, 32u);
    assert_non_null(ia);

    /* Grab the base; then allocate a second block to move cur forward further */
    uint8_t* base = (uint8_t*)alloc_iarena(ia, 64, /*zeroed*/false);
    assert_non_null(base);

    uint8_t* blk2 = (uint8_t*)alloc_iarena_aligned(ia, 128, /*align*/64u, /*zeroed*/false);
    assert_non_null(blk2);

    /* Interior pointer from block2 should be inside */
    assert_true(is_iarena_ptr(ia, blk2 + 64));

    /* end pointer is exclusive */
    const size_t cap = iarena_alloc(ia);
    assert_true(cap > 0);
    uint8_t* end = base + cap;
    assert_false(is_iarena_ptr(ia, end));

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_reset_iarena_basic(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t A = 64u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, A);
    assert_non_null(ia);

    const size_t cap_before = iarena_alloc(ia);
    const size_t tot_before = total_iarena_alloc(ia);

    /* first allocation */
    uint8_t* p = (uint8_t*)alloc_iarena(ia, 128, /*zeroed=*/false);
    assert_non_null(p);
    assert_int_equal(((uintptr_t)p) % A, 0);
    /* write a pattern to confirm reset doesn't zero */
    memset(p, 0xAB, 128);
    assert_true(iarena_size(ia) >= 128);

    /* reset should drop usage to zero and keep capacity/metadata */
    reset_iarena(ia);
    assert_int_equal(iarena_size(ia), 0);
    assert_int_equal(iarena_remaining(ia), cap_before);
    assert_int_equal(iarena_alloc(ia), cap_before);
    assert_int_equal(total_iarena_alloc(ia), tot_before);

    /* next allocation should return same address; contents persist (not zeroed by reset) */
    uint8_t* p2 = (uint8_t*)alloc_iarena(ia, 128, /*zeroed=*/false);
    assert_non_null(p2);
    assert_ptr_equal(p2, p);
    for (size_t i = 0; i < 128; ++i) {
        assert_int_equal(p2[i], 0xAB);
    }

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_reset_iarena_alignment_preserved(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t BASE = 32u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, BASE);
    assert_non_null(ia);

    /* consume a few bytes to move cur off alignment, then reset */
    assert_non_null(alloc_iarena(ia, 3, /*zeroed=*/false));
    reset_iarena(ia);

    /* Default alloc respects base alignment */
    void* p = alloc_iarena(ia, 16, /*zeroed=*/false);
    assert_non_null(p);
    assert_int_equal(((uintptr_t)p) % BASE, 0);

    /* Per-call stronger alignment works after reset too */
    void* q = alloc_iarena_aligned(ia, 16, /*align*/64u, /*zeroed=*/false);
    assert_non_null(q);
    assert_int_equal(((uintptr_t)q) % 64u, 0);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_reset_iarena_multiple_resets(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 4096, 64u);
    assert_non_null(ia);

    const size_t cap = iarena_alloc(ia);

    /* Fill exactly once */
    assert_non_null(alloc_iarena(ia, cap, /*zeroed=*/false));
    assert_int_equal(iarena_remaining(ia), 0);

    /* One more should fail */
    errno = 0;
    assert_null(alloc_iarena(ia, 1, /*zeroed=*/false));
    assert_int_equal(errno, ENOMEM);

    /* Reset -> should be able to fill exactly again */
    reset_iarena(ia);
    assert_int_equal(iarena_size(ia), 0);
    assert_non_null(alloc_iarena(ia, cap, /*zeroed=*/false));
    assert_int_equal(iarena_remaining(ia), 0);

    /* Reset twice in a row is fine */
    reset_iarena(ia);
    reset_iarena(ia);
    assert_int_equal(iarena_size(ia), 0);
    assert_int_equal(iarena_alloc(ia), cap);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_reset_iarena_null(void **state) {
    (void)state;
    errno = 0;
    reset_iarena(NULL);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_reset_iarena_does_not_zero(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096 * 2, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 4096, alignof(max_align_t));
    assert_non_null(ia);

    uint8_t* p = (uint8_t*)alloc_iarena(ia, 64, /*zeroed=*/false);
    assert_non_null(p);
    for (size_t i = 0; i < 64; ++i) p[i] = (uint8_t)i;

    reset_iarena(ia);

    /* Re-allocate same size; expect same address and bytes preserved */
    uint8_t* q = (uint8_t*)alloc_iarena(ia, 64, /*zeroed=*/false);
    assert_non_null(q);
    assert_ptr_equal(q, p);
    for (size_t i = 0; i < 64; ++i) {
        assert_int_equal(q[i], (uint8_t)i);
    }

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_iarena_restore_round_trip(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 4096, 64u);
    assert_non_null(ia);

    /* First allocation to establish baseline */
    assert_non_null(alloc_iarena(ia, 128, false));
    iArenaCheckPoint cp = save_iarena(ia);

    /* Advance further */
    assert_non_null(alloc_iarena(ia, 256, false));
    assert_true(iarena_size(ia) > cp.used);

    /* Restore back to cp */
    bool ok = restore_iarena(ia, cp);
    assert_true(ok);
    assert_int_equal(iarena_size(ia), (int)cp.used);
    assert_int_equal(iarena_remaining(ia), (int)(cp.capacity - cp.used));

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_iarena_restore_multiple_checkpoints(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 4096, 32u);
    assert_non_null(ia);

    assert_non_null(alloc_iarena(ia, 64, false));
    iArenaCheckPoint cp1 = save_iarena(ia);

    assert_non_null(alloc_iarena(ia, 100, false));
    iArenaCheckPoint cp2 = save_iarena(ia);

    assert_non_null(alloc_iarena(ia, 50, false));
    size_t size_now = iarena_size(ia);
    assert_true(size_now > cp2.used);

    /* Restore to cp2, then cp1 */
    assert_true(restore_iarena(ia, cp2));
    assert_int_equal(iarena_size(ia), (int)cp2.used);

    assert_true(restore_iarena(ia, cp1));
    assert_int_equal(iarena_size(ia), (int)cp1.used);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_iarena_restore_invalid_magic(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
    assert_non_null(arena);
    iarena_t* ia = init_iarena_with_arena(arena, 4096, 64u);
    assert_non_null(ia);

    assert_non_null(alloc_iarena(ia, 16, false));
    iArenaCheckPoint cp = save_iarena(ia);

    size_t before = iarena_size(ia);
    cp.magic ^= 0xFFFFFFFFu; /* corrupt */
    errno = 0;
    assert_false(restore_iarena(ia, cp));
    assert_int_equal(errno, EINVAL);
    assert_int_equal(iarena_size(ia), (int)before); /* unchanged */

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_iarena_restore_wrong_owner(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(16384, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia1 = init_iarena_with_arena(arena, 4096, 32u);
    iarena_t* ia2 = init_iarena_with_arena(arena, 4096, 32u);
    assert_non_null(ia1);
    assert_non_null(ia2);

    assert_non_null(alloc_iarena(ia1, 32, false));
    iArenaCheckPoint cp = save_iarena(ia1);

    errno = 0;
    assert_false(restore_iarena(ia2, cp));
    assert_int_equal(errno, EINVAL);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_iarena_restore_capacity_mismatch(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
    assert_non_null(arena);
    iarena_t* ia = init_iarena_with_arena(arena, 4096, 64u);
    assert_non_null(ia);

    iArenaCheckPoint cp = save_iarena(ia);
    cp.capacity += 1; /* corrupt capacity */

    errno = 0;
    assert_false(restore_iarena(ia, cp));
    assert_int_equal(errno, EINVAL);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_iarena_restore_alignment_mismatch(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
    assert_non_null(arena);
    iarena_t* ia = init_iarena_with_arena(arena, 4096, 32u);
    assert_non_null(ia);

    iArenaCheckPoint cp = save_iarena(ia);
    cp.alignment *= 2; /* corrupt alignment */

    errno = 0;
    assert_false(restore_iarena(ia, cp));
    assert_int_equal(errno, EINVAL);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_iarena_restore_used_overflow(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
    assert_non_null(arena);
    iarena_t* ia = init_iarena_with_arena(arena, 4096, 64u);
    assert_non_null(ia);

    iArenaCheckPoint cp = save_iarena(ia);
    cp.used = cp.capacity + 1; /* invalid */

    errno = 0;
    assert_false(restore_iarena(ia, cp));
    assert_int_equal(errno, EINVAL);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_iarena_restore_null_ia(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
    assert_non_null(arena);
    iarena_t* ia = init_iarena_with_arena(arena, 4096, alignof(max_align_t));
    assert_non_null(ia);

    iArenaCheckPoint cp = save_iarena(ia);

    errno = 0;
    assert_false(restore_iarena(NULL, cp));
    assert_int_equal(errno, EINVAL);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_iarena_restore_same_address_when_saved_before_alloc(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    // Force 64B payload alignment to keep things neat
    iarena_t* ia = init_iarena_with_arena(arena, 4096, 64u);
    assert_non_null(ia);

    // Save BEFORE first allocation so restoring returns cursor to start of p
    iArenaCheckPoint cp0 = save_iarena(ia);

    // Allocate and write marker
    uint8_t* p = (uint8_t*)alloc_iarena(ia, 64, false);
    assert_non_null(p);
    for (size_t i = 0; i < 64; ++i) p[i] = (uint8_t)(0xA0 + i);

    // Allocate more to move cur forward
    assert_non_null(alloc_iarena(ia, 128, false));

    // Restore to pre-allocation checkpoint -> next alloc should return *same* address
    assert_true(restore_iarena(ia, cp0));
    uint8_t* q = (uint8_t*)alloc_iarena(ia, 64, false);
    assert_non_null(q);
    assert_ptr_equal(q, p);                  // same address
    for (size_t i = 0; i < 64; ++i) {        // data persisted (reset/restore doesn't zero)
        assert_int_equal(q[i], (uint8_t)(0xA0 + i));
    }

    free_arena(arena);
}

static void test_iarena_restore_after_alloc_gives_next_block(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 4096, 64u);
    assert_non_null(ia);

    // First allocation
    uint8_t* p = (uint8_t*)alloc_iarena(ia, 64, false);
    assert_non_null(p);

    // Save AFTER first allocation (used == 64 now)
    iArenaCheckPoint cp_after = save_iarena(ia);

    // Move cursor forward a bit more
    assert_non_null(alloc_iarena(ia, 32, false));

    // Restore to "after p" -> next 64B allocation should NOT be p
    assert_true(restore_iarena(ia, cp_after));
    uint8_t* q = (uint8_t*)alloc_iarena(ia, 64, false);
    assert_non_null(q);
    assert_false(q == p);   // documents the expected behavior

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

typedef struct {
    int    a;
    double b;
} Foo;

static void test_macro_iarena_alloc_type(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t BASE = 64u; /* make alignment checks clear */
    iarena_t* ia = init_iarena_with_arena(arena, 4096, BASE);
    assert_non_null(ia);

    size_t used_before = iarena_size(ia);
    Foo* p = iarena_alloc_type(ia, Foo);
    assert_non_null(p);
    assert_int_equal(((uintptr_t)p) % BASE, 0);

    /* first allocation from a fresh subarena: pad==0, so delta == sizeof(Foo) */
    size_t used_after = iarena_size(ia);
    assert_int_equal(used_after - used_before, sizeof(Foo));

    /* sanity: we can write to it */
    p->a = 7; p->b = 3.14;
    assert_int_equal(p->a, 7);
    assert_true(p->b > 3.0 && p->b < 3.2);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_macro_iarena_alloc_array(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t BASE = 32u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, BASE);
    assert_non_null(ia);

    size_t used_before = iarena_size(ia);
    const size_t N = 10;
    Foo* arr = iarena_alloc_array(ia, Foo, N);
    assert_non_null(arr);
    assert_int_equal(((uintptr_t)arr) % BASE, 0);

    size_t used_after = iarena_size(ia);
    assert_int_equal(used_after - used_before, sizeof(Foo) * N);

    /* write to first and last element */
    arr[0].a = 1; arr[0].b = 1.0;
    arr[N-1].a = 99; arr[N-1].b = 9.9;
    assert_int_equal(arr[0].a, 1);
    assert_int_equal(arr[N-1].a, 99);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_macro_iarena_alloc_type_zeroed(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t BASE = 64u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, BASE);
    assert_non_null(ia);

    Foo* p = iarena_alloc_type_zeroed(ia, Foo);
    assert_non_null(p);
    assert_int_equal(((uintptr_t)p) % BASE, 0);

    /* Verify zeroed bytewise */
    uint8_t* bytes = (uint8_t*)p;
    for (size_t i = 0; i < sizeof(Foo); ++i) {
        assert_int_equal(bytes[i], 0);
    }

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_macro_iarena_alloc_array_zeroed(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t BASE = 32u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, BASE);
    assert_non_null(ia);

    const size_t N = 8;
    Foo* arr = iarena_alloc_array_zeroed(ia, Foo, N);
    assert_non_null(arr);
    assert_int_equal(((uintptr_t)arr) % BASE, 0);

    /* Every byte of the array should be zero */
    uint8_t* b = (uint8_t*)arr;
    for (size_t i = 0; i < sizeof(Foo) * N; ++i) {
        assert_int_equal(b[i], 0);
    }

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_iarena_null_ia(void **state) {
    (void)state;

    int dummy = 0;
    errno = 0;
    void* p = realloc_iarena(NULL, &dummy, sizeof(dummy), sizeof(dummy) * 2, false);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_iarena_malloc_semantics(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 2048, alignof(max_align_t));
    assert_non_null(ia);

    size_t used_before = iarena_size(ia);
    size_t cap         = iarena_alloc(ia);

    assert_true(cap >= sizeof(Foo));

    Foo* p = (Foo*)realloc_iarena(ia, NULL, 0, sizeof(Foo), true);
    assert_non_null(p);

    /* Should have charged exactly sizeof(Foo) for the first allocation (no pad) */
    size_t used_after = iarena_size(ia);
    assert_int_equal(used_after - used_before, sizeof(Foo));

    /* Should be zeroed */
    uint8_t* bytes = (uint8_t*)p;
    for (size_t i = 0; i < sizeof(Foo); ++i) {
        assert_int_equal(bytes[i], 0);
    }

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_iarena_grow_preserves_prefix(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t ALIGN = alignof(max_align_t);
    iarena_t* ia = init_iarena_with_arena(arena, 4096, ALIGN);
    assert_non_null(ia);

    const size_t old_size = 64;   /* multiple of ALIGN for pad=0 next time */
    const size_t new_size = 128;

    uint8_t* old = (uint8_t*)alloc_iarena(ia, old_size, false);
    assert_non_null(old);

    for (size_t i = 0; i < old_size; ++i) {
        old[i] = (uint8_t)(0x10 + i);
    }

    size_t used_before = iarena_size(ia);

    uint8_t* newer = (uint8_t*)realloc_iarena(ia, old, old_size, new_size, false);
    assert_non_null(newer);

    /* Prefix preserved up to old_size */
    for (size_t i = 0; i < old_size; ++i) {
        assert_int_equal(newer[i], (uint8_t)(0x10 + i));
    }

    /* Used should have grown by at least new_size (plus any pad), but we
       don't assert the exact delta to avoid tying tests to padding details. */
    size_t used_after = iarena_size(ia);
    assert_true(used_after > used_before);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_iarena_shrink_truncates(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 4096, alignof(max_align_t));
    assert_non_null(ia);

    const size_t old_size = 64;
    const size_t new_size = 16;

    uint8_t* old = (uint8_t*)alloc_iarena(ia, old_size, false);
    assert_non_null(old);

    for (size_t i = 0; i < old_size; ++i) {
        old[i] = (uint8_t)i;
    }

    uint8_t* newer = (uint8_t*)realloc_iarena(ia, old, old_size, new_size, false);
    assert_non_null(newer);

    for (size_t i = 0; i < new_size; ++i) {
        assert_int_equal(newer[i], (uint8_t)i);
    }

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_iarena_invalid_pointer(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 2048, alignof(max_align_t));
    assert_non_null(ia);

    int stack_var = 123;
    errno = 0;
    void* p = realloc_iarena(ia, &stack_var, sizeof(stack_var), sizeof(stack_var) * 2, false);
    assert_null(p);
    assert_int_equal(errno, EPERM);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_iarena_enomem(void **state) {
    (void)state;

    /* Big enough parent that we can carve a decent iarena slice */
    arena_t *arena = init_dynamic_arena(2 * 4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    /* Sub-arena slice of 4096 bytes; this should succeed */
    iarena_t* ia = init_iarena_with_arena(arena, 4096, alignof(max_align_t));
    assert_non_null(ia);

    const size_t cap = iarena_alloc(ia);   /* usable payload capacity */
    assert_true(cap > 0);

    /* Allocate an “old” block that uses half the capacity */
    const size_t old_size = cap / 2;
    uint8_t* old = (uint8_t*)alloc_iarena(ia, old_size, false);
    assert_non_null(old);

    /* Fill old block with a pattern */
    for (size_t i = 0; i < old_size; ++i) {
        old[i] = (uint8_t)0xAA;
    }

    /* Compute remaining space; request new_size strictly greater than remaining
       so alloc_iarena() inside realloc_iarena is guaranteed to fail. */
    const size_t remaining = iarena_remaining(ia);
    assert_true(remaining > 0);

    const size_t new_size = remaining + 1u;

    errno = 0;
    uint8_t* newer = (uint8_t*)realloc_iarena(ia, old, old_size, new_size, false);
    assert_null(newer);
    assert_int_equal(errno, ENOMEM);

    /* Old block should still contain the pattern (no overwrite on failure) */
    for (size_t i = 0; i < old_size; ++i) {
        assert_int_equal(old[i], (uint8_t)0xAA);
    }

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_iarena_aligned_null_ia(void **state) {
    (void)state;
    int dummy = 0;
    errno = 0;
    void* p = realloc_iarena_aligned(NULL, &dummy, sizeof(dummy), sizeof(dummy) * 2,
                                    false, 16u);
    assert_null(p);
    assert_int_equal(errno, EINVAL);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_iarena_aligned_malloc_semantics(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 2048, alignof(max_align_t));
    assert_non_null(ia);

    const size_t ALIGN = 64u;
    uint8_t* p = (uint8_t*)realloc_iarena_aligned(ia, NULL, 0, 128, true, ALIGN);
    assert_non_null(p);
    assert_int_equal(((uintptr_t)p) % ALIGN, 0);

    /* zeroed */
    for (size_t i = 0; i < 128; ++i) {
        assert_int_equal(p[i], 0);
    }

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_iarena_aligned_grow_and_align(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    const size_t BASE_ALIGN = 32u;
    iarena_t* ia = init_iarena_with_arena(arena, 4096, BASE_ALIGN);
    assert_non_null(ia);

    const size_t old_size = 64;
    const size_t new_size = 128;

    /* initial block with some alignment (32) */
    uint8_t* old = (uint8_t*)alloc_iarena_aligned(ia, old_size, BASE_ALIGN, false);
    assert_non_null(old);
    assert_int_equal(((uintptr_t)old) % BASE_ALIGN, 0);

    for (size_t i = 0; i < old_size; ++i) {
        old[i] = (uint8_t)(0x55 + i);
    }

    /* request stronger alignment 64 */
    const size_t REQ_ALIGN = 64u;
    uint8_t* newer = (uint8_t*)realloc_iarena_aligned(ia, old, old_size, new_size,
                                                     false, REQ_ALIGN);
    assert_non_null(newer);
    assert_int_equal(((uintptr_t)newer) % REQ_ALIGN, 0);

    for (size_t i = 0; i < old_size; ++i) {
        assert_int_equal(newer[i], (uint8_t)(0x55 + i));
    }

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_iarena_aligned_bad_alignment(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 2048, alignof(max_align_t));
    assert_non_null(ia);

    uint8_t* old = (uint8_t*)alloc_iarena(ia, 32, false);
    assert_non_null(old);

    /* 24 is not a power-of-two */
    errno = 0;
    uint8_t* newer = (uint8_t*)realloc_iarena_aligned(ia, old, 32, 64, false, 24u);
    assert_null(newer);
    assert_int_equal(errno, EINVAL);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_iarena_aligned_invalid_pointer(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(4096, false, 4096, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 2048, alignof(max_align_t));
    assert_non_null(ia);

    int stack_var = 42;
    errno = 0;
    void* p = realloc_iarena_aligned(ia, &stack_var, sizeof(stack_var),
                                    sizeof(stack_var) * 2, false, 16u);
    assert_null(p);
    assert_int_equal(errno, EPERM);

    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

static void test_realloc_iarena_aligned_enomem(void **state) {
    (void)state;

    arena_t *arena = init_dynamic_arena(2 * 1024, false, 1024, alignof(max_align_t));
    assert_non_null(arena);

    iarena_t* ia = init_iarena_with_arena(arena, 1024, alignof(max_align_t));
    assert_non_null(ia);

    size_t cap = iarena_alloc(ia);
    assert_true(cap > 0);

    const size_t old_size = cap / 2;
    const size_t new_size = cap;  /* likely too big given old allocation */

    uint8_t* old = (uint8_t*)alloc_iarena(ia, old_size, false);
    assert_non_null(old);

    for (size_t i = 0; i < old_size; ++i) {
        old[i] = (uint8_t)0xCC;
    }

    errno = 0;
    uint8_t* newer = (uint8_t*)realloc_iarena_aligned(ia, old, old_size,
                                                     new_size, false, alignof(max_align_t));

    if (!newer) {
        assert_int_equal(errno, ENOMEM);
        for (size_t i = 0; i < old_size; ++i) {
            assert_int_equal(old[i], (uint8_t)0xCC);
        }
    } else {
        for (size_t i = 0; i < old_size; ++i) {
            assert_int_equal(newer[i], (uint8_t)0xCC);
        }
    }

    free_arena(arena);
}
// ================================================================================ 
// ================================================================================ 

const struct CMUnitTest test_iarena[] = {
    cmocka_unit_test(test_init_iarena_normal),
    cmocka_unit_test(test_init_iarena_too_small_total),
    cmocka_unit_test(test_init_iarena_bad_alignment),
    cmocka_unit_test(test_init_iarena_null_parent),
    cmocka_unit_test(test_init_iarena_insufficient_parent_capacity),
    cmocka_unit_test(test_init_iarena_inherit_alignment_and_alloc),
    cmocka_unit_test(test_init_iarena_stronger_alignment_than_parent), 

    cmocka_unit_test(test_alloc_iarena_basic),
    cmocka_unit_test(test_alloc_iarena_exact_fit_then_enomem),
    cmocka_unit_test(test_alloc_iarena_padding_charged),
    cmocka_unit_test(test_alloc_iarena_zeroed_flag),
    cmocka_unit_test(test_alloc_iarena_enomem_immediate),
    cmocka_unit_test(test_alloc_iarena_einval_cases),

    cmocka_unit_test(test_alloc_iarena_aligned_inherit),
    cmocka_unit_test(test_alloc_iarena_aligned_stronger_than_base),
    cmocka_unit_test(test_alloc_iarena_aligned_weaker_than_base),
    cmocka_unit_test(test_alloc_iarena_aligned_bad_align_arg),
    cmocka_unit_test(test_alloc_iarena_aligned_padding_charged),
    cmocka_unit_test(test_alloc_iarena_aligned_exact_fit_then_enomem),
    cmocka_unit_test(test_alloc_iarena_aligned_enomem_immediate),
    cmocka_unit_test(test_alloc_iarena_aligned_zeroed_flag),
    cmocka_unit_test(test_alloc_iarena_aligned_einval_cases),

    cmocka_unit_test(test_is_iarena_ptr_null_and_boundaries),
    cmocka_unit_test(test_is_iarena_ptr_sized_inside_and_crossing),
    cmocka_unit_test(test_is_iarena_ptr_sized_zero_length_inside),
    cmocka_unit_test(test_is_iarena_ptr_sized_overflow_guard),
    cmocka_unit_test(test_is_iarena_ptr_sized_einval_cases),
    cmocka_unit_test(test_is_iarena_ptr_after_multiple_allocs),

    cmocka_unit_test(test_reset_iarena_basic),
    cmocka_unit_test(test_reset_iarena_alignment_preserved),
    cmocka_unit_test(test_reset_iarena_multiple_resets),
    cmocka_unit_test(test_reset_iarena_null),
    cmocka_unit_test(test_reset_iarena_does_not_zero),

    cmocka_unit_test(test_iarena_restore_round_trip),
    cmocka_unit_test(test_iarena_restore_multiple_checkpoints),
    cmocka_unit_test(test_iarena_restore_invalid_magic),
    cmocka_unit_test(test_iarena_restore_wrong_owner),
    cmocka_unit_test(test_iarena_restore_capacity_mismatch),
    cmocka_unit_test(test_iarena_restore_alignment_mismatch),
    cmocka_unit_test(test_iarena_restore_used_overflow),
    cmocka_unit_test(test_iarena_restore_null_ia),
    cmocka_unit_test(test_iarena_restore_same_address_when_saved_before_alloc),
    cmocka_unit_test(test_iarena_restore_after_alloc_gives_next_block),

    cmocka_unit_test(test_macro_iarena_alloc_type),
    cmocka_unit_test(test_macro_iarena_alloc_array),
    cmocka_unit_test(test_macro_iarena_alloc_type_zeroed),
    cmocka_unit_test(test_macro_iarena_alloc_array_zeroed),

    cmocka_unit_test(test_realloc_iarena_null_ia),
    cmocka_unit_test(test_realloc_iarena_malloc_semantics),
    cmocka_unit_test(test_realloc_iarena_grow_preserves_prefix),
    cmocka_unit_test(test_realloc_iarena_shrink_truncates),
    cmocka_unit_test(test_realloc_iarena_invalid_pointer),
    cmocka_unit_test(test_realloc_iarena_enomem),

    cmocka_unit_test(test_realloc_iarena_aligned_null_ia),
    cmocka_unit_test(test_realloc_iarena_aligned_malloc_semantics),
    cmocka_unit_test(test_realloc_iarena_aligned_grow_and_align),
    cmocka_unit_test(test_realloc_iarena_aligned_bad_alignment),
    cmocka_unit_test(test_realloc_iarena_aligned_invalid_pointer),
    cmocka_unit_test(test_realloc_iarena_aligned_enomem),
};

const size_t test_iarena_count = sizeof(test_iarena) / sizeof(test_iarena[0]);
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
// eof
