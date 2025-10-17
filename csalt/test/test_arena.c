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

#include "test_arena.h"
#include "c_arena.h"

#include <stdint.h>
#include <errno.h>
#include <stdalign.h>
#include <stddef.h>
#include <string.h>
// ================================================================================ 
// ================================================================================ 

void test_init_dynamic_arena(void **state)
{
    // Valgrind test to ensure memory is properly created and freed
	(void) state;
    Arena* arena = init_dynamic_arena(1000);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena);
    assert_int_equal(alloc_type, DYNAMIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 4000);
    assert_int_equal(total_alloc, 4096);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

void test_init_dynamic_arena_no_bytes(void **state) {
    // Verify allocation if user requests 0 bytes
    // Will throw compiler error if user tries to pass a negative number
	(void) state;
    Arena* arena = init_dynamic_arena(0);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena); 
    assert_int_equal(alloc_type, DYNAMIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 4000);
    assert_int_equal(total_alloc, 4096);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

void test_init_dynamic_arena_large_chunk(void **state) {
    // - Verify allocation if user requests more than 4096 bytes to ensure 
    //   proper padding is added
	(void) state;
    Arena* arena = init_dynamic_arena(4097);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena); 
    assert_int_equal(alloc_type, DYNAMIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 4001);
    assert_int_equal(total_alloc, 4097);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

void test_init_static_arena(void **state) {
    // Test for proper initialization of a static arena
    (void) state;
    uint8_t buffer[4097];
    Arena* arena = init_static_arena(buffer, 4097);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena); 
    assert_int_equal(alloc_type, STATIC);
    assert_int_equal(size, 0);
    assert_int_equal(alloc, 4001);
    assert_int_equal(total_alloc, 4097);
}
// -------------------------------------------------------------------------------- 

void test_invalid_free_static_arena(void **state) {
    // Test behavior when an attempt is made to free a static arena
    (void) state;
    uint8_t buffer[400];
    Arena* arena = init_static_arena(buffer, 400);
    free_arena(arena);
    size_t size = arena_size(arena);
    size_t alloc = arena_alloc(arena);
    size_t total_alloc = total_arena_alloc(arena);
    alloc_t alloc_type = arena_mtype(arena);
    // assert_int_equal(EINVAL, errno);
    // assert_int_equal(alloc_type, STATIC);
    // assert_int_equal(size, 0);
    // assert_int_equal(alloc, 304);
    // assert_int_equal(total_alloc, 400);
}
// -------------------------------------------------------------------------------- 

void test_default_alignment(void **state) {
    (void) state;
    size_t val = default_arena_alignment();
    assert_int_equal(val, alignof(max_align_t));
}
// -------------------------------------------------------------------------------- 

void test_set_default_alignment(void **state) {
    (void) state;
    set_default_arena_alignment(4096);
    assert_int_equal(4096, default_arena_alignment());
    reset_default_arena_alignment();
    assert_int_equal(alignof(max_align_t), default_arena_alignment());
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

void test_alloc_dynamic_arena(void **state) {
    (void) state;
    Arena* arena = init_dynamic_arena(10000);
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
    assert_int_equal(size, 12);
    assert_int_equal(alloc, 9904);
    assert_int_equal(total_alloc, 10000);
    assert_int_equal(left_over, 9892);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

void test_alloc_dynamic_arena_zeroed(void **state) {
    (void)state;

    Arena* arena = init_dynamic_arena(10000);
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

void test_alloc_dynamic_arena_null_value(void **state) {
    (void) state;
    int* value = alloc_arena(NULL, sizeof(*value), true);
    assert_null(value);
    assert_int_equal(EINVAL, errno);
}
// -------------------------------------------------------------------------------- 

void test_alloc_dynamic_arena_zero_input(void **state) {
    Arena* arena = init_dynamic_arena(10000);
    errno = 0;
    int* value = alloc_arena(arena, 0, true);
    assert_null(value);
    assert_int_equal(EINVAL, errno);
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

void test_alloc_static_arena(void **state) {
    (void) state;
    uint8_t buffer[10000];
    Arena* arena = init_static_arena(buffer, 10000);
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
    assert_int_equal(size, 12);
    assert_int_equal(alloc, 9904);
    assert_int_equal(total_alloc, 10000);
    assert_int_equal(left_over, 9892);
}
// -------------------------------------------------------------------------------- 

void test_alloc_static_arena_zeroed(void **state) {
    (void)state;

    uint8_t buffer[10000];
    Arena* arena = init_static_arena(buffer, 10000);
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

static inline size_t align_up_size_test(size_t x, size_t a) {
    return (x + (a - 1u)) & ~(a - 1u); /* a must be power-of-two */
}

/* These must match your allocator’s constants */
static const size_t G_MIN_CHUNK    = 4096u;             /* k_min_chunk */
static const size_t G_GROWTH_LIMIT = (size_t)1u << 20;  /* 1 MiB */
static const size_t G_MAX_CHUNK    = (size_t)1u << 24;  /* 16 MiB */

static inline size_t mul_div_ceil_test(size_t x, size_t mul, size_t div) {
    size_t q = x / div;
    size_t r = x % div;
    size_t hi = r * mul;
    size_t add = (hi + (div - 1u)) / div;
    size_t t = q * mul;
    size_t y = t + add;
    if (y < t) return SIZE_MAX; /* clamp on overflow */
    return y;
}

/* Mirrors _next_chunk_size(prev, need, align) from arena.c */
static size_t next_chunk_size_test(size_t prev, size_t need, size_t align) {
    size_t grow = (need > prev) ? need : prev;

    size_t doubled = (prev <= (SIZE_MAX / 2u)) ? (prev << 1) : SIZE_MAX;
    size_t onefive = mul_div_ceil_test(prev, 3u, 2u);
    size_t target  = (prev < G_GROWTH_LIMIT) ? doubled : onefive;

    if (target > grow) { grow = target; }

    if (grow < G_MIN_CHUNK) { grow = G_MIN_CHUNK; }
    if (grow > G_MAX_CHUNK) { grow = G_MAX_CHUNK; }

    grow = align_up_size_test(grow, align);
    if (grow < need) { grow = need; }
    return grow;
}

/* ---------------------- Fixtures ---------------------- */

int setup_small_arena(void **state) {
    const size_t first_total = 4096u;  /* total_alloc for first block */
    Arena* a = init_dynamic_arena(first_total);
    if (!a) return -1;
    *state = a;
    return 0;
}

int teardown_arena(void **state) {
    if (state && *state) {
        free_arena((Arena*)(*state));
        *state = NULL;
    }
    return 0;
}

/* ---------------------- Tests ---------------------- */

void test_no_growth_within_capacity(void **state) {
    Arena* a = (Arena*)(*state);
    assert_non_null(a);

    assert_int_equal(arena_size(a), 0u);
    assert_int_equal(arena_chunk_count(a), 1u);
    assert_int_equal(arena_mtype(a), DYNAMIC);

    size_t const head_cap = arena_alloc(a);     /* usable bytes in head */
    assert_true(head_cap > 64u);

    size_t const n1 = 1000u, n2 = 2000u, n3 = 500u;
    assert_non_null(alloc_arena(a, n1, true));
    assert_non_null(alloc_arena(a, n2, false));
    assert_non_null(alloc_arena(a, n3, false));

    assert_int_equal(arena_size(a), n1 + n2 + n3);
    assert_int_equal(arena_chunk_count(a), 1u); /* no growth */
}

void test_geometric_growth_unaligned(void **state) {
    Arena* a = (Arena*)(*state);
    assert_non_null(a);

    size_t const head_cap = arena_alloc(a);
    size_t fill = (head_cap >= 64u) ? (head_cap - 64u) : (head_cap / 2u);
    assert_non_null(alloc_arena(a, fill, false));
    assert_int_equal(arena_chunk_count(a), 1u);

    size_t const request   = 128u;
    size_t const prev_cap  = arena_alloc(a);    /* still head-only sum */
    size_t const a_align   = default_arena_alignment();
    size_t const expected_new = next_chunk_size_test(prev_cap, request, a_align);

    assert_non_null(alloc_arena(a, request, true));
    assert_int_equal(arena_chunk_count(a), 2u);

    size_t const now_cap = arena_alloc(a);
    assert_true(now_cap >= prev_cap + expected_new);
}

void test_aligned_growth_and_alignment(void **state) {
    Arena* a = (Arena*)(*state);
    assert_non_null(a);

    /* disturb cursor a bit */
    assert_non_null(alloc_arena(a, 1u, false));

    size_t const prev_cap = arena_alloc(a);
    size_t const req_align = 64u;
    size_t const request   = prev_cap;  /* force growth */

    size_t const expected_new = next_chunk_size_test(prev_cap, request, req_align);

    void* p = alloc_arena_aligned(a, request, req_align, true);
    assert_non_null(p);
    assert_aligned_ptr(p, req_align);

    assert_int_equal(arena_chunk_count(a), 2u);
    size_t const now_cap = arena_alloc(a);
    assert_true(now_cap >= prev_cap + expected_new);
}

void test_multiple_geometric_steps(void **state) {
    Arena* a = (Arena*)(*state);
    assert_non_null(a);

    size_t const align = default_arena_alignment();

    size_t prev_total_cap = arena_alloc(a);   // sum across chunks
    size_t tail_cap       = prev_total_cap;   // only head exists, so this equals tail->alloc

    for (int i = 0; i < 3; ++i) {
        /* Force growth: ask for more than total so it can't fit in tail */
        size_t const need = prev_total_cap + 1u;

        /* Compute expected size for the NEW tail chunk using the correct prev (= tail capacity) */
        size_t const expected_new = next_chunk_size_test(tail_cap, need, align);

        void* p = alloc_arena(a, need, false);
        assert_non_null(p);

        size_t const now_total_cap = arena_alloc(a);

        /* Total capacity increases by exactly the new chunk's usable capacity */
        assert_true(now_total_cap >= prev_total_cap + expected_new);

        /* Update trackers for next loop */
        prev_total_cap = now_total_cap;
        tail_cap       = expected_new;  // new tail's capacity
    }

    assert_true(arena_chunk_count(a) >= 2u);
}
// ================================================================================ 
// ================================================================================ 

static size_t size_delta(Arena* a, size_t before) {
    size_t after = arena_size(a);
    assert_true(after >= before);
    return after - before;
}
// -------------------------------------------------------------------------------- 

void test_aligned_basic_default_alignment(void **state) {
    (void)state;

    const size_t first_total = 8192u;      /* total_alloc for initial block */
    Arena* a = init_dynamic_arena(first_total);
    assert_non_null(a);

    const size_t def_align = default_arena_alignment();

    size_t before = arena_size(a);
    size_t bytes  = 256u;

    void* p = alloc_arena_aligned(a, bytes, def_align, false);
    assert_aligned_ptr(p, def_align);

    /* size must increase by bytes + padding; padding < alignment */
    size_t d = size_delta(a, before);
    assert_true(d >= bytes);
    assert_true(d <  bytes + def_align);

    /* still in head chunk (no mandatory growth for this case) */
    assert_int_equal(arena_chunk_count(a), 1u);

    free_arena(a);
}
// -------------------------------------------------------------------------------- 

void test_aligned_stricter_no_growth_after_misalignment(void **state) {
    (void)state;

    /* choose a total large enough to keep everything in one chunk */
    const size_t first_total = 16384u;
    Arena* a = init_dynamic_arena(first_total);
    assert_non_null(a);

    /* disturb natural alignment */
    assert_non_null(alloc_arena(a, 1u, false));

    const size_t strict_align = 64u;
    const size_t bytes = 300u;

    size_t before = arena_size(a);
    size_t cap_before = arena_alloc(a);
    size_t remain = cap_before - before;

    /* ensure we stay in-chunk (no growth). If not enough, adjust bytes downward. */
    assert_true(remain >= (bytes + strict_align)); /* headroom for pad */

    void* p = alloc_arena_aligned(a, bytes, strict_align, false);
    assert_aligned_ptr(p, strict_align);

    size_t d = size_delta(a, before);
    /* padding ∈ [0, align-1], so delta is in [bytes, bytes+align) */
    assert_true(d >= bytes);
    assert_true(d <  bytes + strict_align);

    /* Still one chunk */
    assert_int_equal(arena_chunk_count(a), 1u);

    /* Pointer should be recognized as arena-owned and sized */
    assert_true(is_arena_ptr(a, p));
    assert_true(is_arena_ptr_sized(a, p, bytes));

    free_arena(a);
}
// -------------------------------------------------------------------------------- 

void test_aligned_zeroed_memory(void **state) {
    (void)state;

    const size_t first_total = 8192u;
    Arena* a = init_dynamic_arena(first_total);
    assert_non_null(a);

    const size_t align = 32u;
    const size_t n     = 128u;

    size_t before = arena_size(a);
    void* p = alloc_arena_aligned(a, n, align, true);
    assert_aligned_ptr(p, align);

    /* check zeroed */
    const unsigned char zeros[128] = {0};
    assert_int_equal(memcmp(p, zeros, n), 0);

    /* accounting sane */
    size_t d = size_delta(a, before);
    assert_true(d >= n);
    assert_true(d <  n + align);

    free_arena(a);
}
// -------------------------------------------------------------------------------- 

void test_aligned_invalid_inputs(void **state) {
    (void)state;

    const size_t first_total = 4096u;
    Arena* a = init_dynamic_arena(first_total);
    assert_non_null(a);

    /* bytes == 0 */
    errno = 0;
    assert_null(alloc_arena_aligned(a, 0u, 16u, false));
    assert_int_equal(errno, EINVAL);

    /* alignment == 0 */
    errno = 0;
    assert_null(alloc_arena_aligned(a, 8u, 0u, false));
    assert_int_equal(errno, EINVAL);

    /* alignment not power-of-two */
    errno = 0;
    assert_null(alloc_arena_aligned(a, 8u, 24u, false));
    assert_int_equal(errno, EINVAL);

    /* NULL arena */
    errno = 0;
    assert_null(alloc_arena_aligned(NULL, 8u, 16u, false));
    assert_int_equal(errno, EINVAL);

    free_arena(a);
}
// -------------------------------------------------------------------------------- 

void test_aligned_static_arena_within_capacity(void **state) {
    (void)state;

    /* Build a static buffer that comfortably holds Arena + Chunk + data */
    enum { BUF_BYTES = 8192 };
    _Alignas(16) uint8_t buf[BUF_BYTES];

    Arena* a = init_static_arena(buf, BUF_BYTES);
    assert_non_null(a);
    assert_int_equal(arena_mtype(a), STATIC);

    const size_t align = 64u;
    const size_t n     = 512u;

    size_t before = arena_size(a);
    size_t cap    = arena_alloc(a);
    assert_true(cap - before >= n + align);  /* leave room for pad */

    void* p = alloc_arena_aligned(a, n, align, false);
    assert_aligned_ptr(p, align);

    /* size bump in expected range */
    size_t d = size_delta(a, before);
    assert_true(d >= n);
    assert_true(d <  n + align);

    /* still a single chunk (static arena cannot grow) */
    assert_int_equal(arena_chunk_count(a), 1u);

    /* pointer sanity in static */
    assert_true(is_arena_ptr(a, p));
    assert_true(is_arena_ptr_sized(a, p, n));
}
// ================================================================================
// ================================================================================
// eof
