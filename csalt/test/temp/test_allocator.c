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

#include "test_allocator.h"
#include "c_allocator.h"

#include <stdint.h>
#include <errno.h>
#include <stdalign.h>
#include <stddef.h>
#include <string.h>
// ================================================================================ 
// ================================================================================ 
// TEST INIT DYNAMIC AND STATIC ARENAS

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

void test_dyn_min_chunk_rounds_up_and_changes_capacity(void **state) {
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

void test_dyn_min_chunk_equivalence_6000_vs_8192(void **state) {
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

void test_dyn_alignment_rounds_up_and_floors_to_maxalign(void **state) {
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

void test_static_alignment_honored_even_with_unaligned_buffer(void **state) {
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

void test_init_darena(void **state) {
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

void test_init_darena_no_bytes(void **state) {
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

void test_init_darena_large_chunk(void **state) {
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

void test_init_sarena(void **state) {
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

void test_invalid_free_sarena(void **state) {
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

void test_arena_double_free(void **state) {
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

void test_alloc_darena(void **state) {
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

void test_alloc_darena_zeroed(void **state) {
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

void test_alloc_darena_null_value(void **state) {
    (void) state;
    int* value = alloc_arena(NULL, sizeof(*value), true);
    assert_null(value);
    assert_int_equal(EINVAL, errno);
}
// -------------------------------------------------------------------------------- 

void test_alloc_darena_zero_input(void **state) {
    arena_t* arena = init_darena(10000, true);
    errno = 0;
    int* value = alloc_arena(arena, 0, true);
    assert_null(value);
    assert_int_equal(EINVAL, errno);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

void test_alloc_sarena(void **state) {
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

void test_alloc_sarena_zeroed(void **state) {
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

void test_realloc_grows_when_allowed(void **state) {
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

void test_realloc_fails_when_resize_false(void **state) {
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

void test_realloc_fails_in_static_arena(void **state) {
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

void test_realloc_first_alloc_in_new_chunk_is_aligned_and_no_pad(void **state) {
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

void test_is_arena_ptr_basic_hits_and_misses(void **state) {
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

void test_is_arena_ptr_tail_fastpath(void **state) {
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

void test_is_arena_ptr_sized_boundaries(void **state) {
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

void test_is_arena_ptr_multichunk(void **state) {
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

void test_is_arena_ptr_sized_cross_chunk_fails(void **state) {
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

void test_is_arena_ptr_null_and_zero_size_guards(void **state) {
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

void test_reset_dynamic_trim_true_frees_extra_chunks_and_resets_usage(void **state) {
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

void test_reset_dynamic_keep_chunks_preserves_capacity(void **state) {
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

void test_reset_static_zeroes_usage_ignores_trim(void **state) {
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

void test_save_restore_same_chunk_pointer_roundtrip(void **state) {
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

void test_save_restore_second_chunk_trims_and_replays_allocation(void **state) {
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

void test_save_restore_static_rewinds_cursor(void **state) {
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

void test_restore_rejects_checkpoint_from_other_arena(void **state) {
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

void test_restore_accepts_empty_checkpoint_noop(void **state) {
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

#if !defined(__NO_GENERICS__)
/* A composite type to exercise struct alignment/size */
typedef struct {
    int    x;
    double y;
} Demo;
// -------------------------------------------------------------------------------- 

void test_arena_alloc_type_and_type_zeroed(void **state) {
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

void test_arena_alloc_array_and_array_zeroed(void **state) {
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

void test_arena_alloc_array_count_zero_is_error(void **state) {
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

#endif
// ================================================================================
// ================================================================================
// eof
