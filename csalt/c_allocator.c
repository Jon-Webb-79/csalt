// ================================================================================
// ================================================================================
// - File:    arena.c
// - Purpose: This file contains the implementation of generalized C allocators 
//            defined in the arena.h file
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    October 15, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include <stdlib.h>  // size_t, uintptr_t
#include <stdint.h>  // uint8_t
#include <errno.h>   // errno and macros
#include <string.h>  // memset
#include <stdio.h>   // printf
#include <stdarg.h>  // va_list, va_start
#include <stddef.h>  // max_align_t
#include <stdalign.h> // alignof
#include <stdatomic.h> // _Atomic
#define _GNU_SOURCE
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#include "c_allocator.h"
// ================================================================================ 
// ================================================================================ 
// CONSTANTS 

#if ARENA_ENABLE_DYNAMIC
/* Deterministic growth knobs (choose values once for your build) */
static const size_t k_growth_limit = (size_t)1u << 20; /* 1 MiB: switch from 2x to 1.5x */
static const size_t k_max_chunk    = (size_t)1u << 24; /* 16 MiB: cap single chunk size */
#endif
// ================================================================================ 
// ================================================================================ 
// DATA STRUCTURES 

// 32 bytes with padding
typedef struct Chunk{
    uint8_t *chunk;     // Pointer to beginning of memory
    size_t len;         // Populated length of memory within struct in bytes
    size_t alloc;       // Allocated length of memory within struct in bytes
    struct Chunk* next; // Pointer to next memory chunk
} Chunk;
// -------------------------------------------------------------------------------- 

// 72 bytes
struct arena_t{
    uint8_t* cur;      // A pointer to the next available memory slot
    Chunk* head;       // Pointer to head of memory chunk linked list
    Chunk* tail;       // Pointer to the tail of memory chunks for the linked list

    size_t alignment;  // alignment in bytes 
    size_t len;        // Total memory used in bytes between all memory chunks
    size_t alloc;      // Total memory allocated in bytes between all memory chunks
    size_t tot_alloc;  // Total memory allocated to include containers
    size_t min_chunk;  // The minimum chunk size in bytes

    uint8_t mem_type;  // type of memory used
    uint8_t resize;    // allows resizing if true with mem_type == DYNAMIC
    uint8_t owns_memory; // true if struct owns memory false otherwise
    uint8_t _pad[5];   // keep 8 byte passing
};
// -------------------------------------------------------------------------------- 

typedef struct {
    Chunk* chunk;     // Pointer to saved memory chunk
    uint8_t* cur;     // Current pointer position in saved chunk
    size_t len;       // Length of saved chunk
} ArenaCheckPointRep;
// ================================================================================ 
// ================================================================================ 
// STATIC HELPER FUNCTIONS 

static bool _buf_appendf(char *buffer,
                        size_t buffer_size,
                        size_t *p_offset,
                        const char *fmt, ...) {
    if ((buffer == NULL) || (p_offset == NULL) || (fmt == NULL)) {
        return false;
    }

    size_t const offset = *p_offset;
    if (offset > buffer_size) {      /* defensive: offset should never exceed size */
        return false;
    }

    size_t const remaining = buffer_size - offset;
    if (remaining == 0U) {
        return false;
    }

    va_list args;
    va_start(args, fmt);
    int const n = vsnprintf(&buffer[offset], remaining, fmt, args);
    va_end(args);

    if (n < 0) {                     /* encoding/format failure */
        return false;
    }

    /* If n >= remaining, output would be truncated */
    if ((size_t)n >= remaining) {
        return false;
    }

    *p_offset = offset + (size_t)n;
    return true;
}
// -------------------------------------------------------------------------------- 

static inline size_t max_size(size_t a, size_t b) { return a > b ? a : b; }
// -------------------------------------------------------------------------------- 

static inline size_t _align_up_size(size_t x, size_t a) {
    /* a must be power-of-two */
    return (x + (a - 1)) & ~(a - 1);
}
// -------------------------------------------------------------------------------- 

static inline uintptr_t _align_up_uintptr(uintptr_t p, size_t a) {
    return (p + (a - 1)) & ~(a - 1);
}
// -------------------------------------------------------------------------------- 

#if ARENA_ENABLE_DYNAMIC
static struct Chunk* _chunk_new_ex(size_t data_bytes, size_t data_align){
    if ((data_bytes == 0U) || (data_align == 0U) || ((data_align & (data_align - 1U)) != 0U)) {
        errno = EINVAL;
        return NULL;
    }

    /* Worst-case header+pad before data: sizeof(Chunk) + (data_align - 1) */
    if (data_bytes > SIZE_MAX - (sizeof(struct Chunk) + (data_align - 1U))) {
        errno = ENOMEM;
        return NULL;
    }
    size_t const total = sizeof(struct Chunk) + (data_align - 1U) + data_bytes;

    struct Chunk* ch = (struct Chunk*)malloc(total);
    if (ch == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    uintptr_t const base   = (uintptr_t)ch;
    uintptr_t const before = base + (uintptr_t)sizeof(struct Chunk);
    uintptr_t const data_p = _align_up_uintptr(before, data_align);

    ch->chunk = (uint8_t*)data_p;      /* now truly aligned to data_align */
    ch->len   = 0U;
    ch->alloc = data_bytes;            /* usable data capacity */
    ch->next  = NULL;

    return ch; /* free(ch) later frees header+pad+data in one go */
}
#endif
// -------------------------------------------------------------------------------- 

static inline int _is_pow2(size_t x){ return x && !(x & (x-1)); }
// -------------------------------------------------------------------------------- 

static inline size_t _next_pow2(size_t x){
    if (x <= 1) return x ? 1 : 0;
    if (x > (SIZE_MAX>>1)) return 0;
    x--; for (size_t s=1; s<8*sizeof(size_t); s<<=1) x|=x>>s; return x+1;
}
// -------------------------------------------------------------------------------- 

static inline size_t _mul_div_ceil(size_t x, size_t mul, size_t div) {
    /* assumes div > 0, caller ensures small constants like 2 or 3 */
    size_t q = x / div;
    size_t r = x % div;
    size_t hi = r * mul;
    size_t add = (hi + (div - 1u)) / div;
    size_t t = q * mul;
    size_t y = t + add;
    /* very defensive overflow clamp */
    if (y < t) { return SIZE_MAX; }
    return y;
}
// -------------------------------------------------------------------------------- 

#if ARENA_ENABLE_DYNAMIC
static size_t _next_chunk_size(size_t prev_data_alloc, size_t need, size_t align, size_t min_chunk) {
    /* meet the request at minimum */
    size_t grow = (need > prev_data_alloc) ? need : prev_data_alloc;

    /* geometric target: 2x until growth_limit, then 1.5x */
    size_t doubled = (prev_data_alloc <= (SIZE_MAX / 2u)) ? (prev_data_alloc << 1) : SIZE_MAX;
    size_t onefive = _mul_div_ceil(prev_data_alloc, 3u, 2u); /* 1.5x */

    size_t target = (prev_data_alloc < k_growth_limit) ? doubled : onefive;
    if (target > grow) { grow = target; }

    /* clamp to floor/ceiling */
    if (grow < min_chunk) { grow = min_chunk; }
    if (grow > k_max_chunk) { grow = k_max_chunk; }

    /* align capacity to alignment to keep chunk->chunk naturally aligned */
    grow = _align_up_size(grow, align);

    /* final safety: ensure capacity still covers need after rounding */
    if (grow < need) { grow = need; }

    return grow;
}
#endif
// -------------------------------------------------------------------------------- 

static inline size_t _pad_up(uintptr_t p, size_t a) {
    size_t const mask = a - 1u;                // a is power-of-two
    return (size_t)(((p + mask) & ~mask) - p);  // 0..a-1
}
// -------------------------------------------------------------------------------- 

static inline void _cp_pack(ArenaCheckPoint* pub, const ArenaCheckPointRep* rep) {
    memcpy(pub, rep, sizeof *rep);
}
static inline void _cp_unpack(const ArenaCheckPoint* pub, ArenaCheckPointRep* rep) {
    memcpy(rep, pub, sizeof *rep);
}
// --------------------------------------------------------------------------------

static const Chunk* find_chunk_in_chain(const arena_t* a, const Chunk* target, Chunk const** prev_out) {
    const Chunk* prev = NULL;
    for (const Chunk* c = a->head; c; c = c->next) {
        if (c == target) {
            if (prev_out) *prev_out = prev;
            return c;
        }
        prev = c;
    }
    return NULL;
}
// ================================================================================ 
// ================================================================================ 
// INITIALIZE AND DEALLOCATE FUNCTIONS 

static inline arena_expect_t arena_ok(arena_t *a) {
    return (arena_expect_t){
        .has_value = true,
        .u.value   = a
    };
}
// -------------------------------------------------------------------------------- 

static inline arena_expect_t arena_err(ErrorCode e) {
    /* If you want errno set, do it here: */
    // errno = set_errno_from_error(e);
    return (arena_expect_t){
        .has_value = false,
        .u.error   = e
    };
}
// -------------------------------------------------------------------------------- 

arena_expect_t init_dynamic_arena(size_t bytes,
                                  bool   resize,
                                  size_t min_chunk_in,
                                  size_t base_align_in) {
#if ARENA_ENABLE_DYNAMIC

    /* Normalize min_chunk (0 allowed) */
    size_t min_chunk = min_chunk_in;
    if (min_chunk && !_is_pow2(min_chunk)) {
        min_chunk = _next_pow2(min_chunk);
        if (!min_chunk) {
            return arena_err(INVALID_ARG);      /* overflow when normalizing */
        }
    }

    /* Normalize base alignment; enforce ABI floor */
    size_t base_align = base_align_in ? base_align_in : alignof(max_align_t);
    if (!_is_pow2(base_align)) {
        base_align = _next_pow2(base_align);
        if (!base_align) {
            return arena_err(ALIGNMENT_ERROR);  /* overflow when normalizing */
        }
    }
    if (base_align < alignof(max_align_t)) {
        base_align = alignof(max_align_t);
    }

    /* Initial total buffer size */
    size_t total = bytes;
    if (min_chunk && total < min_chunk) {
        total = min_chunk;
    }

    if (total < (sizeof(arena_t) + sizeof(Chunk))) {
        /* User asked for too little memory to even hold headers */
        return arena_err(INVALID_ARG);
    }

    void *base = malloc(total);
    if (!base) {
        return arena_err(BAD_ALLOC);
    }

    uintptr_t const b = (uintptr_t)base;

    /* Layout: [arena][padding][Chunk][padding][data...] */

    uintptr_t p_arena   = b;
    uintptr_t arena_end = p_arena + sizeof(arena_t);
    if (arena_end < p_arena) {
        /* overflow in addition */
        free(base);
        return arena_err(LENGTH_OVERFLOW);
    }

    uintptr_t p_chunk   = _align_up_uintptr(arena_end, alignof(Chunk));
    uintptr_t chunk_end = p_chunk + sizeof(Chunk);
    if (chunk_end < p_chunk || chunk_end > b + total) {
        /* overflow or not enough room */
        free(base);
        return arena_err(LENGTH_OVERFLOW);
    }

    /* Data starts aligned to base_align */
    uintptr_t p_data = _align_up_uintptr(chunk_end, base_align);
    if (p_data > b + total) {
        free(base);
        return arena_err(ALIGNMENT_ERROR);
    }

    size_t usable = (size_t)((b + total) - p_data);
    if (!usable) {
        free(base);
        return arena_err(OUT_OF_MEMORY);
    }

    arena_t *a = (arena_t *)p_arena;
    Chunk   *h = (Chunk *)p_chunk;

    h->chunk = (uint8_t *)p_data;
    h->len   = 0;
    h->alloc = usable;
    h->next  = NULL;

    a->head        = h;
    a->tail        = h;
    a->cur         = (uint8_t *)p_data;
    a->len         = 0;
    a->alloc       = usable;
    a->tot_alloc   = total;
    a->min_chunk   = min_chunk;
    a->alignment   = base_align;
    a->mem_type    = (uint8_t)DYNAMIC;
    a->resize      = (uint8_t)(resize ? 1u : 0u);
    a->owns_memory = (uint8_t)1u;

    return arena_ok(a);

#else
    /* Feature compiled out */
    (void)bytes;
    (void)resize;
    (void)min_chunk_in;
    (void)base_align_in;
    return arena_err(FEATURE_DISABLED);
#endif
}
// -------------------------------------------------------------------------------- 

arena_expect_t init_static_arena(void *buffer,
                                 size_t bytes,
                                 size_t alignment_in) {
    /* Validate buffer */
    if (buffer == NULL) {
        return arena_err(NULL_POINTER);
    }

    /* Normalize per-arena base alignment (>= alignof(max_align_t)) */
    size_t base_align = (alignment_in != 0u)
                        ? alignment_in
                        : alignof(max_align_t);

    if (!_is_pow2(base_align)) {
        base_align = _next_pow2(base_align);
        if (base_align == 0u) {
            return arena_err(ALIGNMENT_ERROR);
        }
    }

    if (base_align < alignof(max_align_t)) {
        base_align = alignof(max_align_t);
    }

    /* Convert buffer to integer space */
    uintptr_t const b     = (uintptr_t)buffer;
    uintptr_t const b_end = b + bytes;

    /* Detect overflow in b + bytes */
    if (b_end < b) {
        return arena_err(LENGTH_OVERFLOW);
    }

    /* Align Arena header itself */
    uintptr_t p_arena = _align_up_uintptr(b, alignof(arena_t));
    if (p_arena > b_end) {
        return arena_err(INVALID_ARG);
    }

    /* Must have room for Arena + Chunk at minimum */
    if ((b_end - p_arena) < (sizeof(arena_t) + sizeof(Chunk))) {
        return arena_err(INVALID_ARG);
    }

    uintptr_t arena_end = p_arena + sizeof(arena_t);
    if ((arena_end < p_arena) || (arena_end > b_end)) {
        return arena_err(LENGTH_OVERFLOW);
    }

    /* Place first Chunk header aligned for Chunk */
    uintptr_t p_chunk = _align_up_uintptr(arena_end, alignof(Chunk));
    uintptr_t chunk_end = p_chunk + sizeof(Chunk);
    if ((chunk_end < p_chunk) || (chunk_end > b_end)) {
        return arena_err(LENGTH_OVERFLOW);
    }

    /* Place data aligned to per-arena base alignment */
    uintptr_t p_data = _align_up_uintptr(chunk_end, base_align);
    if (p_data > b_end) {
        return arena_err(ALIGNMENT_ERROR);
    }

    size_t usable = (size_t)(b_end - p_data);
    if (usable == 0u) {
        return arena_err(OUT_OF_MEMORY);
    }

    /* Stitch in-place */
    arena_t *a = (arena_t *)p_arena;
    Chunk   *h = (Chunk   *)p_chunk;

    h->chunk = (uint8_t *)p_data;
    h->len   = 0u;
    h->alloc = usable;
    h->next  = NULL;

    a->head      = h;
    a->tail      = h;
    a->cur       = (uint8_t *)p_data;
    a->len       = 0u;
    a->alloc     = usable;
    a->tot_alloc = (size_t)(b_end - b);
    a->min_chunk = 0u;
    a->alignment = base_align;

    a->mem_type    = (uint8_t)STATIC;
    a->resize      = (uint8_t)0u;
    a->owns_memory = (uint8_t)1u;

    return arena_ok(a);
}
// -------------------------------------------------------------------------------- 

arena_expect_t init_arena_with_arena(arena_t *parent,
                                     size_t   bytes,
                                     size_t   alignment_in) {
    /* Basic argument validation */
    if (parent == NULL) {
        return arena_err(NULL_POINTER);
    }
    if (bytes == 0u) {
        return arena_err(INVALID_ARG);
    }

    /* Normalize per-arena base alignment (>= alignof(max_align_t)) */
    size_t base_align = (alignment_in != 0u)
                        ? alignment_in
                        : alignof(max_align_t);

    if (!_is_pow2(base_align)) {
        base_align = _next_pow2(base_align);
        if (base_align == 0u) {
            return arena_err(ALIGNMENT_ERROR);
        }
    }

    if (base_align < alignof(max_align_t)) {
        base_align = alignof(max_align_t);
    }

    /* Single allocation from parent for entire sub-arena (header + chunk + data) */
    void_ptr_expect_t expect = alloc_arena(parent, bytes, false);
    if (!expect.has_value) {
        return arena_err(OUT_OF_MEMORY);
    }
    void* buffer = expect.u.value;

    uintptr_t const b     = (uintptr_t)buffer;
    uintptr_t const b_end = b + bytes;

    /* Detect overflow in b + bytes */
    if (b_end < b) {
        return arena_err(LENGTH_OVERFLOW);
    }

    /* Align arena_t header */
    uintptr_t p_arena = _align_up_uintptr(b, alignof(arena_t));
    if (p_arena > b_end) {
        return arena_err(INVALID_ARG);
    }

    /* Must have room for arena_t + Chunk at minimum */
    if ((b_end - p_arena) < (sizeof(arena_t) + sizeof(Chunk))) {
        return arena_err(INVALID_ARG);
    }

    uintptr_t arena_end = p_arena + sizeof(arena_t);
    if ((arena_end < p_arena) || (arena_end > b_end)) {
        return arena_err(LENGTH_OVERFLOW);
    }

    /* Place Chunk header aligned for Chunk */
    uintptr_t p_chunk   = _align_up_uintptr(arena_end, alignof(Chunk));
    uintptr_t chunk_end = p_chunk + sizeof(Chunk);
    if ((chunk_end < p_chunk) || (chunk_end > b_end)) {
        return arena_err(LENGTH_OVERFLOW);
    }

    /* Place data aligned to per-arena base alignment */
    uintptr_t p_data = _align_up_uintptr(chunk_end, base_align);
    if (p_data > b_end) {
        return arena_err(ALIGNMENT_ERROR);
    }

    size_t usable = (size_t)(b_end - p_data);
    if (usable == 0u) {
        return arena_err(OUT_OF_MEMORY);
    }

    /* Stitch sub-arena in-place */
    arena_t *a = (arena_t *)p_arena;
    Chunk   *h = (Chunk   *)p_chunk;

    h->chunk = (uint8_t *)p_data;
    h->len   = 0u;
    h->alloc = usable;
    h->next  = NULL;

    a->head      = h;
    a->tail      = h;
    a->cur       = (uint8_t *)p_data;
    a->len       = 0u;
    a->alloc     = usable;
    a->tot_alloc = (size_t)(b_end - b);   /* full region footprint */
    a->min_chunk = 0u;                    /* not used (no growth) */
    a->alignment = base_align;

    /* Key differences from init_static_arena: */
    a->mem_type    = parent->mem_type;    /* inherit from parent */
    a->resize      = (uint8_t)0u;         /* fixed capacity, cannot grow */
    a->owns_memory = (uint8_t)0u;         /* does NOT own backing memory */

    return arena_ok(a);
}
// -------------------------------------------------------------------------------- 

arena_expect_t init_darena(size_t bytes, bool resize) {
    return init_dynamic_arena(bytes, resize, 4096u, alignof(max_align_t));
}
// --------------------------------------------------------------------------------

arena_expect_t init_sarena(void* buffer, size_t bytes) {
    return init_static_arena(buffer, bytes, alignof(max_align_t));
}
// -------------------------------------------------------------------------------- 

arena_expect_t init_arena_with_buddy(buddy_t *buddy,
                                     size_t   bytes,
                                     size_t   base_align_in) {
    /* Basic argument validation */
    if (buddy == NULL) {
        return arena_err(NULL_POINTER);
    }
    if (bytes == 0u) {
        return arena_err(INVALID_ARG);
    }

    /* Normalize per-arena base alignment (>= alignof(max_align_t), power of two) */
    size_t base_align = (base_align_in != 0u)
                        ? base_align_in
                        : alignof(max_align_t);

    if (!_is_pow2(base_align)) {
        base_align = _next_pow2(base_align);
        if (base_align == 0u) {
            return arena_err(ALIGNMENT_ERROR);
        }
    }

    if (base_align < alignof(max_align_t)) {
        base_align = alignof(max_align_t);
    }

    /* Single allocation from buddy for entire arena region. */
    void *buffer = alloc_buddy(buddy, bytes, false);
    if (buffer == NULL) {
        /* Buddy could not satisfy the request (pool exhausted, etc.) */
        return arena_err(OUT_OF_MEMORY);
    }

    uintptr_t const b     = (uintptr_t)buffer;
    uintptr_t const b_end = b + bytes;

    /* Detect overflow in b + bytes */
    if (b_end < b) {
        (void)return_buddy_element(buddy, buffer);
        return arena_err(LENGTH_OVERFLOW);
    }

    /* Place arena_t exactly at the buddy user pointer. */
    uintptr_t p_arena = b;

    /* Must have room for arena_t + Chunk at minimum. */
    if ((b_end - p_arena) < (sizeof(arena_t) + sizeof(Chunk))) {
        (void)return_buddy_element(buddy, buffer);
        return arena_err(INVALID_ARG);
    }

    uintptr_t arena_end = p_arena + sizeof(arena_t);
    if ((arena_end < p_arena) || (arena_end > b_end)) {
        (void)return_buddy_element(buddy, buffer);
        return arena_err(LENGTH_OVERFLOW);
    }

    /* Place Chunk header aligned for Chunk */
    uintptr_t p_chunk   = _align_up_uintptr(arena_end, alignof(Chunk));
    uintptr_t chunk_end = p_chunk + sizeof(Chunk);
    if ((chunk_end < p_chunk) || (chunk_end > b_end)) {
        (void)return_buddy_element(buddy, buffer);
        return arena_err(LENGTH_OVERFLOW);
    }

    /* Place data aligned to per-arena base alignment */
    uintptr_t p_data = _align_up_uintptr(chunk_end, base_align);
    if (p_data > b_end) {
        (void)return_buddy_element(buddy, buffer);
        return arena_err(ALIGNMENT_ERROR);
    }

    size_t usable = (size_t)(b_end - p_data);
    if (usable == 0u) {
        (void)return_buddy_element(buddy, buffer);
        return arena_err(OUT_OF_MEMORY);
    }

    /* Stitch in-place */
    arena_t *a = (arena_t *)p_arena;
    Chunk   *h = (Chunk   *)p_chunk;

    h->chunk = (uint8_t *)p_data;
    h->len   = 0u;
    h->alloc = usable;
    h->next  = NULL;

    a->head      = h;
    a->tail      = h;
    a->cur       = (uint8_t *)p_data;
    a->len       = 0u;
    a->alloc     = usable;     /* total usable data bytes */
    a->tot_alloc = bytes;      /* entire region visible to the arena */
    a->min_chunk = 0u;         /* no dynamic growth from this arena */
    a->alignment = base_align;

    a->mem_type    = (uint8_t)DYNAMIC; /* memory ultimately from a dynamic source */
    a->resize      = 0u;               /* fixed-capacity arena */
    a->owns_memory = 0u;               /* buddy owns the underlying memory */

    /* IMPORTANT: 'a' == 'buffer', so later you can call return_buddy_element(buddy, a). */

    return arena_ok(a);
}
// -------------------------------------------------------------------------------- 

bool return_arena_with_buddy(arena_t *arena, buddy_t *buddy) {
    if (!arena || !buddy) {
        return false;
    }

    // This arena is supposed to be backed by a buddy allocator,
    // and must *not* own its memory independently.
    if (arena->owns_memory) {
        return false;
    }

    if (!is_buddy_ptr_sized(buddy, (const void *)arena, arena->tot_alloc)) {
        return false;
    }

    return return_buddy_element(buddy, (void *)arena);
}
// -------------------------------------------------------------------------------- 

void free_arena(arena_t* arena) {
    if (arena == NULL) { return; }

    if (!arena->owns_memory) {
        // Null out pointers to prevent use-after-free
        arena->cur   = NULL;
        arena->head  = NULL;
        arena->tail  = NULL;
        arena->alloc = 0;
        arena->len   = 0;
        arena->tot_alloc = 0;
        // Note: We cannot free the arena header itself because it lives
        // in the parent's memory. The parent will reclaim it.
        return;
    }

    if (arena->mem_type == STATIC) {
        return;
    }

    /* Free extra chunks (if any) first */
    if ((arena->head != NULL) && (arena->head->next != NULL)) {
        struct Chunk* cur = arena->head->next;
        while (cur != NULL) {
            struct Chunk* next = cur->next;
            free(cur);                  /* single malloc frees header+data */
            cur = next;
        }
        arena->head->next = NULL;
    }

    free(arena);
}
// ================================================================================ 
// ================================================================================ 
// ALLOCATION FUNCTIONS

static inline void_ptr_expect_t vp_ok(void *p) {
    return (void_ptr_expect_t){
        .has_value = true,
        .u.value   = p
    };
}
// -------------------------------------------------------------------------------- 

static inline void_ptr_expect_t vp_err(ErrorCode e) {
    return (void_ptr_expect_t){
        .has_value = false,
        .u.error   = e
    };
}
// -------------------------------------------------------------------------------- 

inline void_ptr_expect_t alloc_arena(arena_t *arena, size_t bytes, bool zeroed) {
    /* Basic argument validation */
    if (arena == NULL) {
        return vp_err(NULL_POINTER);
    }
    if (bytes == 0u) {
        return vp_err(INVALID_ARG);
    }

    /* Per-arena base alignment must be a nonzero power-of-two */
    size_t const a = arena->alignment;
    if (a == 0u || (a & (a - 1u)) != 0u) {
        return vp_err(ALIGNMENT_ERROR);
    }

    Chunk *tail = arena->tail;
    if (tail == NULL) {
        return vp_err(ILLEGAL_STATE); /* corrupted or uninitialized arena state */
    }

    /* Per-allocation pad to 'a' */
    uintptr_t const cur = (uintptr_t)arena->cur;
    size_t    const pad = _pad_up(cur, a);

    /* Check for overflow in pad + bytes */
    if (bytes > (SIZE_MAX - pad)) {
        return vp_err(LENGTH_OVERFLOW);
    }

    size_t const need  = pad + bytes;
    size_t const avail = (tail->alloc >= tail->len) ? (tail->alloc - tail->len) : 0u;

    /* Fast path: fits in current tail */
    if (avail >= need) {
        uint8_t *p = (uint8_t *)(cur + pad);  /* a-aligned */
        arena->cur  = p + bytes;
        tail->len  += need;                   /* charge pad + bytes */
        arena->len += need;

        if (zeroed) {
            memset(p, 0, bytes);
        }
        return vp_ok(p);
    }

#if ARENA_ENABLE_DYNAMIC
    /* No space -> grow only when allowed */
    if (arena->mem_type == STATIC || !arena->resize) {
        return vp_err(OPERATION_UNAVAILABLE); /* cannot grow this arena */
    }

    size_t const grow_data = _next_chunk_size(tail->alloc, need, a, arena->min_chunk);
    if (grow_data == 0u) {
        return vp_err(LENGTH_OVERFLOW);
    }

    Chunk *nc = _chunk_new_ex(grow_data, a);
    if (nc == NULL) {
        return vp_err(BAD_ALLOC);
    }

    /* Link new chunk as tail */
    tail->next  = nc;
    arena->tail = nc;

    /* Accounting: usable capacity + footprint */
    arena->alloc     += nc->alloc;
    arena->tot_alloc += _align_up_size(sizeof(Chunk), a) + nc->alloc;

    /* First allocation from fresh chunk: base is a-aligned -> no pad */
    void *p = (void *)nc->chunk;
    arena->cur  = nc->chunk + bytes;
    nc->len     = bytes;
    arena->len += bytes;

    if (zeroed) {
        memset(p, 0, bytes);
    }
    return vp_ok(p);
#else
    /* Dynamic growth support is compiled out */
    (void)zeroed; /* only relevant if allocation could succeed */
    return vp_err(UNSUPPORTED);
#endif
}
// -------------------------------------------------------------------------------- 

void* realloc_arena(arena_t* arena,
                    void*   variable,
                    size_t  var_size,
                    size_t  realloc_size,
                    bool    zeroed)
{
    if (!arena || !variable) {
        errno = EINVAL;
        return NULL;
    }

    // No-op on shrink or same size; keeps arena space.
    if (realloc_size <= var_size) {
        return variable;
    }

    // Allocate new block (no zeroing; we’ll zero only the tail if requested).
    void_ptr_expect_t expect = alloc_arena(arena, realloc_size, false);
    if (!expect.has_value) {
        return NULL;
    }
    void* ptr = expect.u.value;

    memcpy(ptr, variable, var_size);

    if (zeroed) {
        memset((uint8_t*)ptr + var_size, 0, realloc_size - var_size);
    }

    return ptr;
}
// -------------------------------------------------------------------------------- 

void* alloc_arena_aligned(arena_t* arena, size_t bytes, size_t alignment, bool zeroed) {
    if (!arena || bytes == 0u) { return NULL; }

    // Choose effective alignment: at least the arena base alignment.
    size_t a_req = (alignment ? alignment : arena->alignment);
    // Must be power-of-two; clamp to arena base if smaller.
    if (a_req == 0u || (a_req & (a_req - 1u)) != 0u) {return NULL; }
    size_t const a = (a_req < arena->alignment) ? arena->alignment : a_req;

    Chunk* tail = arena->tail;
    if (!tail) { return NULL; }

    // Pad current cursor up to 'a'
    uintptr_t const cur  = (uintptr_t)arena->cur;
    size_t    const pad  = _pad_up(cur, a);
    size_t    const need = pad + bytes;

    // Fast path: fits in current tail
    size_t const avail = (tail->alloc >= tail->len) ? (tail->alloc - tail->len) : 0u;
    if (avail >= need) {
        uint8_t* p = (uint8_t*)(cur + pad);   // a-aligned
        arena->cur  = p + bytes;
        tail->len  += need;                   // charge pad + bytes
        arena->len += need;
        if (zeroed) memset(p, 0, bytes);
        return p;
    }

#if ARENA_ENABLE_DYNAMIC
    // No space -> grow only when allowed
    if (arena->mem_type == STATIC || !arena->resize) { return NULL; }

    // Ask for a new chunk whose data region is aligned to 'a'.
    // For a fresh chunk, first allocation will have zero pad.
    size_t const grow_data = _next_chunk_size(tail->alloc, /*need=*/bytes, /*align=*/a, arena->min_chunk);
    Chunk* nc = _chunk_new_ex(grow_data, a);
    if (!nc) return NULL; // helper sets errno

    // Link new chunk
    tail->next  = nc;
    arena->tail = nc;

    // Accounting: usable capacity (alloc) is nc->alloc; footprint adds aligned header + data.
    arena->alloc     += nc->alloc;
    arena->tot_alloc += _align_up_size(sizeof(Chunk), a) + nc->alloc;

    // First allocation from fresh chunk: nc->chunk is already a-aligned.
    void* p = (void*)nc->chunk;
    arena->cur  = nc->chunk + bytes;
    nc->len     = bytes;
    arena->len += bytes;

    if (zeroed) memset(p, 0, bytes);
    return p;
#else
    return NULL;
#endif
}
// -------------------------------------------------------------------------------- 

void* realloc_arena_aligned(arena_t* arena,
                            void*   variable,
                            size_t  var_size,
                            size_t  realloc_size,
                            bool    zeroed,
                            size_t  algined) {
    if (!arena || !variable) {
        return NULL;
    }

    // No-op on shrink or same size; keeps arena space.
    if (realloc_size <= var_size) {
        return variable;
    }

    // Allocate new block (no zeroing; we’ll zero only the tail if requested).
    void* ptr = alloc_arena_aligned(arena, realloc_size, algined, false);
    if (!ptr) {
        return NULL;
    }

    memcpy(ptr, variable, var_size);

    if (zeroed) {
        memset((uint8_t*)ptr + var_size, 0, realloc_size - var_size);
    }

    return ptr;
}
// ================================================================================ 
// ================================================================================ 
// UTILITY FUNCTIONS

bool is_arena_ptr(const arena_t* arena, const void* ptr) {
    if (!arena || !ptr) return false;

    const uintptr_t p = (uintptr_t)ptr;

    // Fast check: tail first
    const Chunk* c = arena->tail;
    if (c && c->chunk && c->len <= c->alloc) {
        uintptr_t s = (uintptr_t)c->chunk;
        uintptr_t e = s + c->len;               // end is exclusive
        if (e >= s && p >= s && p < e) return true;  // e>=s guards overflow
    }

    // Walk remaining chunks
    for (const Chunk* cur = arena->head; cur; cur = cur->next) {
        if (!cur->chunk) continue;
        size_t used = cur->len;
        if (used > cur->alloc) used = cur->alloc;     // defensive clamp

        uintptr_t s = (uintptr_t)cur->chunk;
        uintptr_t e = s + used;
        if (e < s) continue;                          // overflow guard

        if (p >= s && p < e) return true;
    }

    return false;
}
// -------------------------------------------------------------------------------- 

bool is_arena_ptr_sized(const arena_t* arena, const void* ptr, size_t size) {
    if (!arena || !ptr || size == 0) return false;

    const uintptr_t p  = (uintptr_t)ptr;
    const uintptr_t pe = p + size;                 // end (exclusive)

    if (pe < p) return false;                      // overflow

    // Tail fast-path
    const Chunk* c = arena->tail;
    if (c && c->chunk && c->len <= c->alloc) {
        const uintptr_t s  = (uintptr_t)c->chunk;
        const uintptr_t ue = s + c->len;
        if (ue >= s && p >= s && pe <= ue) return true;
    }

    // Walk remaining chunks
    for (const Chunk* cur = arena->head; cur; cur = cur->next) {
        if (!cur->chunk) continue;

        size_t used = cur->len;                    // clamp if corrupted
        if (used > cur->alloc) used = cur->alloc;

        const uintptr_t s  = (uintptr_t)cur->chunk;
        const uintptr_t ue = s + used;             // end (exclusive)

        if (ue < s) continue;                      // overflow in chunk arithmetic
        if (p >= s && pe <= ue) return true;
    }

    return false;
}
// -------------------------------------------------------------------------------- 

bool reset_arena(arena_t *arena, bool trim_extra_chunks) {
    if (!arena) {
        return false;
    }

    if (!arena->head) {
        arena->cur  = NULL;
        arena->len  = 0u;
        arena->tail = NULL;
        return false;
    }

    /* Zero usage counters on all chunks */
    for (Chunk *cur = arena->head; cur; cur = cur->next) {
        cur->len = 0u;
    }
    arena->len = 0u;

    if (trim_extra_chunks && arena->mem_type == (uint8_t)DYNAMIC) {
        /* Free all chunks after head (each is a single-malloc block).
           Adjust tot_alloc by mirroring the addition you did at growth:
           tot_alloc += align_up(sizeof(Chunk), alignment) + chunk->alloc; */
        const size_t hdr_rounded = _align_up_size(sizeof(Chunk), arena->alignment);

        Chunk *to_free = arena->head->next;
        while (to_free) {
            Chunk *next = to_free->next;

            /* subtract this chunk's contribution safely */
            size_t contrib = hdr_rounded + to_free->alloc;
            if (arena->tot_alloc >= contrib) {
                arena->tot_alloc -= contrib;
            } else {
                arena->tot_alloc = 0; /* defensive clamp */
            }

            /* IMPORTANT: free the header pointer (owns the whole block),
               NOT to_free->chunk which is interior. */
            free(to_free);
            to_free = next;
        }

        /* Detach and normalize to a single head chunk */
        arena->head->next = NULL;
        arena->tail       = arena->head;
        arena->cur        = arena->head->chunk;

        /* Usable capacity now equals the head's data capacity */
        arena->alloc = arena->head->alloc;
        /* tot_alloc already adjusted above; it now equals the initial malloc size */
    } else {
        /* Keep all chunks allocated: zero usage but preserve capacity/footprint */
        arena->tail = arena->tail ? arena->tail : arena->head;
        arena->cur  = arena->tail->chunk ? arena->tail->chunk : arena->head->chunk;
        /* alloc/tot_alloc unchanged */
    }
    return true;
}
// -------------------------------------------------------------------------------- 

ArenaCheckPoint save_arena(const arena_t* arena) {
    ArenaCheckPoint pub = {0};
    if (!arena) {
        return pub;
    }
    ArenaCheckPointRep rep = {0};
    if (arena) {
        rep.chunk = arena->tail;   // save tail chunk (point-in-time tail)
        rep.cur   = arena->cur;    // save cursor within that chunk
        rep.len   = arena->len;    // optional: total used at save time
    }
    _cp_pack(&pub, &rep);
    return pub;
}
// -------------------------------------------------------------------------------- 

bool restore_arena(arena_t* arena, ArenaCheckPoint cp) {
    if (!arena) { 
        return false; 
    }

    // Unpack the checkpoint
    ArenaCheckPointRep rep = {0};
    _cp_unpack(&cp, &rep);

    // Empty checkpoint is a no-op (success)
    if (!rep.chunk) {
        return true;
    }

    // Validate that the checkpoint's chunk still exists in the chain
    const Chunk* prev = NULL;
    const Chunk* hit  = find_chunk_in_chain(arena, rep.chunk, &prev);
    if (!hit) { 
        return false; 
    }

    // Validate the chunk has a valid data region
    if (!rep.chunk->chunk) { 
        return false; 
    }

    // Validate the checkpoint cursor is within bounds
    uintptr_t chunk_start = (uintptr_t)rep.chunk->chunk;
    uintptr_t cursor_pos  = (uintptr_t)rep.cur;
    uintptr_t chunk_end   = chunk_start + rep.chunk->alloc;  // exclusive
    
    // Check for overflow and valid cursor position
    if (chunk_end < chunk_start || cursor_pos < chunk_start || cursor_pos > chunk_end) { 
        return false; 
    }

    // For DYNAMIC arenas: free all chunks after the checkpoint chunk
    if (arena->mem_type == (uint8_t)DYNAMIC) {
        const size_t hdr_rounded = _align_up_size(sizeof(Chunk), arena->alignment);
        
        Chunk* to_free = rep.chunk->next;
        while (to_free) {
            Chunk* next = to_free->next;
            
            // Subtract this chunk's contribution from tot_alloc
            // (mirrors what was added during growth)
            size_t contrib = hdr_rounded + to_free->alloc;
            if (arena->tot_alloc >= contrib) {
                arena->tot_alloc -= contrib;
            } else {
                arena->tot_alloc = 0;  // defensive clamp
            }
            
            free(to_free);  // Free the chunk header (owns the entire block)
            to_free = next;
        }
        
        // Detach the freed chunks from the list
        ((Chunk*)rep.chunk)->next = NULL;
    }
    // For STATIC arenas: we can't free chunks, just validate they still exist
    // (the validation already happened via find_chunk_in_chain above)

    // Update the tail chunk's used length to match the checkpoint cursor
    ((Chunk*)rep.chunk)->len = (size_t)(cursor_pos - chunk_start);
    
    // Update arena state
    arena->tail = (Chunk*)rep.chunk;
    arena->cur  = (uint8_t*)rep.cur;

    // Recompute accounting for the remaining chain
    size_t alignment = arena->alignment;
    
    // Validate alignment is a power of two
    if (alignment == 0u || (alignment & (alignment - 1u)) != 0u) { 
        return false; 
    }

    // Walk the chain and recompute totals
    size_t total_used = 0;
    size_t total_cap  = 0;
    size_t total_foot = _align_up_size(sizeof(arena_t), alignment);
    
    for (Chunk* k = arena->head; k; k = k->next) {
        // Clamp used to allocation (defensive)
        size_t used = (k->len <= k->alloc) ? k->len : k->alloc;
        
        total_used += used;
        total_cap  += k->alloc;
        
        // Add this chunk's overhead + data to footprint
        total_foot += _align_up_size(sizeof(Chunk), alignment) + k->alloc;
    }

    arena->len   = total_used;
    arena->alloc = total_cap;
    
    // Only recompute tot_alloc for STATIC arenas
    // (DYNAMIC already adjusted it during the freeing loop)
    if (arena->mem_type == (uint8_t)STATIC) {
        arena->tot_alloc = total_foot;
    }
    // For DYNAMIC, tot_alloc was incrementally adjusted during chunk freeing,
    // so we keep that value (it should match total_foot, but the incremental
    // approach is more precise if there were any rounding differences)

    return true;
}
// ================================================================================ 
// ================================================================================ 
// GETTER TUNCTIONS

size_t arena_remaining(const arena_t* arena) {
    if (!arena) {
        return SIZE_MAX;
    }
    if (!arena || !arena->tail || !arena->tail->chunk) return 0u;
    size_t used = arena->tail->len;
    if (used > arena->tail->alloc) used = arena->tail->alloc;  // clamp
    return arena->tail->alloc - used;
}
// -------------------------------------------------------------------------------- 

size_t arena_chunk_count(const arena_t* arena) {
    if (!arena) {
        return SIZE_MAX;
    }
    
    size_t count = 0u;
    Chunk* current = arena->head;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}
// -------------------------------------------------------------------------------- 

inline alloc_t arena_mtype(const arena_t* arena) {
    if (!arena) {
        return ALLOC_INVALID;
    }

    return (alloc_t)arena->mem_type;
}
// -------------------------------------------------------------------------------- 

inline size_t arena_size(const arena_t* arena) {
    if (!arena) {
        return SIZE_MAX;
    }
    return arena->len;
}
// -------------------------------------------------------------------------------- 

inline size_t arena_alloc(const arena_t* arena) {
    if (!arena) {
        return SIZE_MAX;
    }
    return arena->alloc;
}
// -------------------------------------------------------------------------------- 

inline size_t total_arena_alloc(const arena_t* arena) {
    if (!arena) {
        return SIZE_MAX;
    }
    return arena->tot_alloc;
}
// -------------------------------------------------------------------------------- 

inline size_t arena_alignment(const arena_t* arena) {
    if (!arena || !arena->head) {
        return SIZE_MAX;
    }
    return arena->alignment;
}
// -------------------------------------------------------------------------------- 

inline size_t arena_min_chunk_size(const arena_t* arena) {
    if (!arena || !arena->head) {
        return SIZE_MAX;
    }
    return arena->min_chunk;
}
// -------------------------------------------------------------------------------- 

inline bool arena_owns_memory(const arena_t* arena) {
    if (!arena) {
        return false;
    }
    return (bool)arena->owns_memory;
}
// ================================================================================ 
// ================================================================================ 
// SETTER FUNCTIONS 

inline void toggle_arena_resize(arena_t* arena, bool toggle) {
    if (!arena) {
        errno = EINVAL;
        return;
    }

#if ARENA_ENABLE_DYNAMIC
    // Cannot toggle resize for static arenas
    if (arena->mem_type == STATIC) {
        errno = EPERM;
        return;
    }
    
    // Cannot toggle resize for sub-arenas (borrowed memory)
    // Sub-arenas are always fixed-size by design
    if (!arena->owns_memory) {
        errno = EPERM;
        return;
    }
    
    // Only dynamic arenas that own their memory can have resize toggled
    arena->resize = (uint8_t)(toggle ? 1 : 0);
    return;
    
#else 
    (void)toggle;  // Suppress unused parameter warning
    errno = ENOTSUP;
    return;
#endif
}
// ================================================================================ 
// ================================================================================ 
// LOG FUNCTIONS 

bool arena_stats(const arena_t *arena, char *buffer, size_t buffer_size) {
    size_t offset = 0U;

    if ((buffer == NULL) || (buffer_size == 0U)) {
        return false;
    }

    if (arena == NULL) {
        (void)_buf_appendf(buffer, buffer_size, &offset, "%s", "Arena: NULL\n");
        return true;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset, "%s", "Arena Statistics:\n")) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                     "  Type: %s\n",
                     (arena->mem_type == STATIC) ? "STATIC" : "DYNAMIC")) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                     "  Used: %zu bytes\n", arena->len)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                     "  Capacity: %zu bytes\n", arena->alloc)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                     "  Total (with overhead): %zu bytes\n", arena->tot_alloc)) {
        return false;
    }

    /* Utilization with divide-by-zero guard */
    if (arena->alloc == 0U) {
        if (!_buf_appendf(buffer, buffer_size, &offset,
                         "%s", "  Utilization: N/A (capacity is 0)\n")) {
            return false;
        }
    } else {
        double const util = (100.0 * (double)arena->len) / (double)arena->alloc;
        if (!_buf_appendf(buffer, buffer_size, &offset,
                         "  Utilization: %.1f%%\n", util)) {
            return false;
        }
    }

    /* List chunks */
    {
        int chunk_count = 0;
        Chunk *current = arena->head;
        while (current != NULL) {
            chunk_count += 1;
            if (!_buf_appendf(buffer, buffer_size, &offset,
                             "  Chunk %d: %zu/%zu bytes\n",
                             chunk_count, current->len, current->alloc)) {
                return false;
            }
            current = current->next;
        }
    }

    return true;
}
// ================================================================================ 
// ================================================================================ 
// POOL ALLOCATORS 

#ifdef DEBUG
typedef struct pool_slice {
    uint8_t* start;
    uint8_t* end;
    struct pool_slice* next;
} pool_slice_t;
#endif

struct pool_t {
    arena_t* arena;          // Backing arena supplying memory (owned or borrowed)
    bool     owns_arena;     // true if this pool allocated and must destroy the arena
    size_t   block_size;     // User-requested block payload size (bytes)
    size_t   stride;         // Actual block size = block_size rounded up to required alignment
    size_t   blocks_per_chunk; // Number of blocks to allocate in each arena slice (growth granularity)
    uint8_t* cur;            // Pointer to next free byte in current arena slice (bump pointer)
    uint8_t* end;            // End of current arena slice (cur == end means next grow needed)
    void*    free_list;      // Head of intrusive free list (pointer stored in first word of freed blocks)
    size_t   total_blocks;   // Total number of blocks ever made available to this pool (including freed)
    size_t   free_blocks;    // Number of blocks currently in free_list (available to reuse)
    bool     grow_enabled;   // If false, pool cannot request new slices from arena (fixed-size mode)
#ifdef DEBUG
    pool_slice_t* slices;    // Linked list of all memory slices obtained from arena (for debug verification)
#endif
};
// -------------------------------------------------------------------------------- 

typedef struct {
    void*    free_list;      // Head of free list at checkpoint time
    size_t   free_blocks;    // Number of blocks in free list
    uint8_t* cur;            // Bump pointer position in current slice
    uint8_t* end;            // End of current slice at checkpoint time
    size_t   total_blocks;   // Total blocks available at checkpoint time
#ifdef DEBUG
    pool_slice_t* slices;    // First slice at checkpoint time (for validation)
#endif
} PoolCheckpointRep;
// ================================================================================ 
// ================================================================================ 
// STATIC HELPER FUNCTIONS 

static inline void _cp_pack_pool(PoolCheckPoint* pub, const PoolCheckpointRep* rep) {
    // Ensure internal representation fits in public opaque storage
    _Static_assert(sizeof(PoolCheckpointRep) <= sizeof(pub->_priv),
                   "PoolCheckpointRep too large for PoolCheckpoint");
    memcpy(pub, rep, sizeof *rep);
}
// -------------------------------------------------------------------------------- 

static inline void _cp_unpack_pool(const PoolCheckPoint* pub, PoolCheckpointRep* rep) {
    memcpy(rep, pub, sizeof *rep);
}
// ================================================================================ 
// ================================================================================ 
// POOL STATIC FUNCTIONS 

static bool pool_grow(pool_t* p) {
    if (!p->grow_enabled) { errno = EPERM; return false; }
    const size_t bytes = p->stride * p->blocks_per_chunk;
    uint8_t* base = (uint8_t*)alloc_arena_aligned(p->arena, bytes, p->stride, false);
    if (!base) { errno = ENOMEM; return false; }
    p->cur = base;
    p->end = base + bytes;
    p->total_blocks += p->blocks_per_chunk;

#ifdef DEBUG
    // record this slice; allocate node from arena to avoid malloc
    pool_slice_t* s = (pool_slice_t*)arena_alloc_aligned(p->arena, sizeof *s, alignof(pool_slice_t));
    if (!s) { errno = ENOMEM; return false; } // extremely unlikely
    s->start = base;
    s->end   = base + bytes;
    s->next  = p->slices;
    p->slices = s;
#endif

    return true;
}
// -------------------------------------------------------------------------------- 

static inline void* pool_pop_free(pool_t* p) {
    void* blk = p->free_list;
    if (blk) {
        p->free_list = *(void**)blk;
        p->free_blocks--;
    }
    return blk;
}
// -------------------------------------------------------------------------------- 

static inline void pool_push_free(pool_t* p, void* blk) {
#ifdef DEBUG
    assert(is_arena_ptr_sized(p->arena, blk, p->block_size));
    assert(((uintptr_t)blk % p->stride) == 0);
    bool in_pool = false;
    for (pool_slice_t* s = p->slices; s; s = s->next) {
        if ((uint8_t*)blk >= s->start && (uint8_t*)blk < s->end) { in_pool = true; break; }
    }
    assert(in_pool && "free(): pointer not from this pool");
#endif
    *(void**)blk = p->free_list;
    p->free_list = blk;
    p->free_blocks++;
}
// ================================================================================ 
// ================================================================================ 
// INITIALIZE AND DEALLOCATE FUNCTIONS 

pool_t* init_pool_with_arena(arena_t* arena,
                             size_t   block_size,
                             size_t   alignment,
                             size_t   blocks_per_chunk,
                             bool     prewarm_one_chunk,
                             bool     grow_enabled) {
    if (!arena || block_size == 0 || blocks_per_chunk == 0) {
        errno = EINVAL;
        return NULL;
    }

    // Effective alignment must be at least alignof(void*) for the intrusive next*
    size_t eff_align = alignment ? alignment : alignof(max_align_t);
    // Round alignment to a power-of-two if your project requires; at minimum, clamp:
    if (eff_align < alignof(void*)) eff_align = alignof(void*);

    // Compute stride; ensure a freed block can hold a pointer
    size_t stride = _align_up_size(block_size, eff_align);
    if (stride < sizeof(void*)) stride = sizeof(void*);

    // Allocate the pool header from the arena so there’s no external malloc
    pool_t* p = (pool_t*)alloc_arena_aligned(arena, sizeof *p, alignof(pool_t), false);
    if (!p) { errno = ENOMEM; return NULL; }

    p->arena           = arena;
    p->owns_arena      = false;
    p->block_size      = block_size;
    p->stride          = stride;
    p->blocks_per_chunk= blocks_per_chunk;
    p->cur             = NULL;
    p->end             = NULL;
    p->free_list       = NULL;
    p->total_blocks    = 0;
    p->free_blocks     = 0;

    p->grow_enabled    = grow_enabled;

#ifdef DEBUG
    p->slices           = NULL;
#endif
    // Optional “prewarm” so the first allocation is O(1)
    if (prewarm_one_chunk) {
        if (!pool_grow(p)) return NULL; // errno set by pool_grow
    }
    return p;
}
// -------------------------------------------------------------------------------- 

pool_t* init_dynamic_pool(size_t block_size,
                          size_t alignment,
                          size_t blocks_per_chunk,
                          size_t arena_seed_bytes,
                          size_t min_chunk_bytes,
                          bool   grow_enabled,
                          bool   prewarm_one_chunk)
{
#if !ARENA_ENABLE_DYNAMIC
    errno = ENOTSUP;
    return NULL;
#else
    // -------- argument validation
    if (block_size == 0 || blocks_per_chunk == 0 || arena_seed_bytes == 0) {
        errno = EINVAL; return NULL;
    }

    // NEW: fixed-capacity pools must be prewarmed, otherwise they’d be unusable
    if (!grow_enabled && !prewarm_one_chunk) {
        errno = EINVAL; return NULL;
    }

    // -------- compute effective alignment and stride
    size_t eff_align = alignment ? alignment : alignof(max_align_t);
    // if (!_is_pow2(eff_align)) eff_align = _next_pow2(eff_align);
    if (eff_align < alignof(void*)) eff_align = alignof(void*);

    size_t stride = _align_up_size(block_size, eff_align);
    if (stride < sizeof(void*)) stride = sizeof(void*);

    // Overflow guard: bytes = stride * blocks_per_chunk
    if (blocks_per_chunk > SIZE_MAX / stride) {
        errno = EOVERFLOW; return NULL;
    }
    const size_t slice_bytes = stride * blocks_per_chunk;

    // Arena base alignment should never be less than max_align_t
    size_t arena_base_align = eff_align;
    if (arena_base_align < alignof(max_align_t)) arena_base_align = alignof(max_align_t);

    // -------- create the owned arena
    arena_expect_t a = init_dynamic_arena(/*bytes=*/arena_seed_bytes,
                                          /*resize=*/grow_enabled,
                                          /*min_chunk_in=*/min_chunk_bytes,
                                          /*base_align_in=*/arena_base_align);
    if (!a.has_value) {
        return NULL;
    }
    arena_t* arena = a.u.value;
    if (!arena) {
        return NULL; // errno set by init_dynamic_arena
    }

    // -------- allocate pool header inside arena (no external malloc)
    pool_t* p = (pool_t*)alloc_arena_aligned(arena, sizeof *p, alignof(pool_t), /*zeroed=*/false);
    if (!p) {
        free_arena(arena); // errno set by alloc_arena_aligned
        return NULL;
    }

    // -------- initialize pool fields
    p->arena            = arena;
    p->owns_arena       = true;
    p->block_size       = block_size;
    p->stride           = stride;
    p->blocks_per_chunk = blocks_per_chunk;
    p->cur = p->end     = NULL;
    p->free_list        = NULL;
    p->total_blocks     = 0;
    p->free_blocks      = 0;
    p->grow_enabled     = grow_enabled;
#ifdef DEBUG
    p->slices           = NULL;
#endif

    // -------- prewarm behavior
    if (prewarm_one_chunk) {
        uint8_t* base = (uint8_t*)alloc_arena_aligned(arena, slice_bytes, stride, /*zeroed=*/false);
        if (!base) {
            free_arena(arena); // errno already set (ENOMEM / EPERM)
            return NULL;
        }
        p->cur = base;
        p->end = base + slice_bytes;
        p->total_blocks += blocks_per_chunk;

#ifdef DEBUG
        pool_slice_t* s = (pool_slice_t*)alloc_arena_aligned(arena, sizeof *s, alignof(pool_slice_t), false);
        if (s) { s->start = base; s->end = base + slice_bytes; s->next = p->slices; p->slices = s; }
#endif
    }

    return p;
#endif // ARENA_ENABLE_DYNAMIC
}

// -------------------------------------------------------------------------------- 

pool_t* init_static_pool(void*  buffer,
                         size_t buffer_bytes,
                         size_t block_size,
                         size_t alignment) {
    // ---- validate inputs
    if (!buffer || buffer_bytes == 0 || block_size == 0) {
        errno = EINVAL; return NULL;
    }

    // ---- compute effective block alignment and stride
    size_t eff_align = alignment ? alignment : alignof(max_align_t);
    // If your project enforces power-of-two, normalize with your helpers:
    // if (!_is_pow2(eff_align)) eff_align = _next_pow2(eff_align);
    if (eff_align < alignof(void*)) eff_align = alignof(void*);

    size_t stride = _align_up_size(block_size, eff_align);
    if (stride < sizeof(void*)) stride = sizeof(void*);

    // Arena base alignment should be >= max_align_t (and >= eff_align)
    size_t arena_base_align = eff_align;
    if (arena_base_align < alignof(max_align_t)) arena_base_align = alignof(max_align_t);

    // ---- create the owned static arena (header lives in caller buffer)
    arena_expect_t a = init_static_arena(buffer, buffer_bytes, arena_base_align);
    if (!a.has_value) {
        return NULL;
    }
    arena_t* arena = a.u.value;
    if (!arena) {
        // errno set by init_static_arena
        return NULL;
    }

    // ---- allocate pool header inside the arena (no external malloc)
    pool_t* p = (pool_t*)alloc_arena_aligned(arena, sizeof *p, alignof(pool_t), /*zeroed=*/false);
    if (!p) {
        // Not enough room even for the pool header within the caller buffer
        free_arena(arena);
        // errno set by alloc_arena_aligned (likely ENOMEM)
        return NULL;
    }

    // ---- compute capacity from remaining tail space
    size_t rem = arena_remaining(arena);             // usable bytes after header
    size_t blocks = rem / stride;                    // floor to integral blocks
    if (blocks == 0) {
        // No space to hold even one block at requested alignment/stride
        free_arena(arena);
        errno = ENOMEM;
        return NULL;
    }

    // Carve the single (and only) slice now (prewarm is required by policy)
    size_t slice_bytes = blocks * stride;
    uint8_t* base = (uint8_t*)alloc_arena_aligned(arena, slice_bytes, stride, /*zeroed=*/false);
    if (!base) {
        // Alignment could reduce usable capacity below our estimate
        free_arena(arena);
        // errno set by alloc_arena_aligned
        return NULL;
    }

    // ---- initialize pool fields (fixed-capacity)
    p->arena            = arena;
    p->owns_arena       = true;     // owns arena object; caller still owns raw buffer per your static-arena contract
    p->block_size       = block_size;
    p->stride           = stride;
    p->blocks_per_chunk = 0;        // not used in static pools
    p->cur              = base;
    p->end              = base + slice_bytes;
    p->free_list        = NULL;
    p->total_blocks     = blocks;
    p->free_blocks      = 0;
    p->grow_enabled     = false;    // static pools never grow
#ifdef DEBUG
    p->slices           = NULL;
    // Track the single slice for debug-time ownership checks
    pool_slice_t* s = (pool_slice_t*)alloc_arena_aligned(arena, sizeof *s, alignof(pool_slice_t), false);
    if (s) { s->start = base; s->end = base + slice_bytes; s->next = p->slices; p->slices = s; }
#endif

    return p;
}
// -------------------------------------------------------------------------------- 

inline void* alloc_pool(pool_t* pool, bool zeroed) {
    if (!pool) { errno = EINVAL; return NULL; }

    // 1) Reuse from free list if available
    void* blk = pool_pop_free(pool);
    if (blk) return blk;

    // 2) Carve from the current slice; grow if necessary
    if (pool->cur == pool->end) {
        if (!pool_grow(pool)) return NULL; // errno set
    }
    blk  = pool->cur;
    pool->cur += pool->stride;
    if (zeroed) memset(blk, 0, pool->block_size);
    return blk;
}
// -------------------------------------------------------------------------------- 

inline void return_pool_element(pool_t* pool, void* ptr) {
    if (!pool || !ptr) return;
    pool_push_free(pool, ptr);
}
// -------------------------------------------------------------------------------- 

void reset_pool(pool_t* pool) {
    if (!pool) return;
    pool->free_list = NULL;
    //pool->cur = pool->end = NULL;
    pool->free_blocks = 0;
    pool->total_blocks = 0;
#ifdef DEBUG
    pool->slices = NULL; // slice nodes live in arena; a later arena_reset reclaims them
#endif
}
// -------------------------------------------------------------------------------- 

void free_pool(pool_t* pool) {
    if (!pool) return;

    if (pool->owns_arena) {
        // The pool header lives inside the arena; free_arena will release it
        free_arena(pool->arena);
        return;
    }

    // Borrowed arena case: pool header is inside arena memory,
    // so we cannot free it directly — we only null it out so
    // accidental reuse fails loudly.
    pool->arena = NULL;
    pool->cur   = NULL;
    pool->end   = NULL;
    pool->free_list = NULL;
    pool->total_blocks = 0;
    pool->free_blocks  = 0;

#ifdef DEBUG
    pool->slices = NULL;
#endif
}
// -------------------------------------------------------------------------------- 

inline size_t pool_block_size(const pool_t* pool) {
    if (!pool) {
        errno = EINVAL;
        return 0;
    }
    return pool->block_size;
}
// -------------------------------------------------------------------------------- 

inline size_t pool_stride(const pool_t* pool) {
    if (!pool) {
        errno = EINVAL;
        return 0;
    }
    return pool->stride;
}
// -------------------------------------------------------------------------------- 

inline size_t pool_total_blocks(const pool_t* pool) {
    if (!pool || !pool->arena) {
        errno = EINVAL;
        return 0;
    }
    return pool->total_blocks;
}
// -------------------------------------------------------------------------------- 

inline size_t pool_free_blocks(const pool_t* pool) {
    if (!pool || !pool->arena) {
        errno = EINVAL;
        return 0;
    }
    return pool->free_blocks;
}
// -------------------------------------------------------------------------------- 

inline size_t pool_alignment(const pool_t* pool) {
    if (!pool || !pool->arena) {
        errno = EINVAL;
        return 0;
    }
    return pool->arena->alignment;
}
// -------------------------------------------------------------------------------- 

size_t pool_bump_remaining_blocks(const pool_t* pool) {
    if (!pool || !pool->arena) {
        errno = EINVAL;
        return 0;
    }
    // Defensive guards: stride must be non-zero and the slice must exist.
    if (pool->stride == 0 || pool->cur == NULL || pool->end == NULL) {
        return 0; // no active slice or misconfigured; treat as empty
    }

    // Compute remaining bytes, guarding against pointer wrap (shouldn’t happen).
    const uintptr_t cur = (uintptr_t)pool->cur;
    const uintptr_t end = (uintptr_t)pool->end;
    if (end <= cur) return 0;

    const size_t rem_bytes = (size_t)(end - cur);
    return rem_bytes / pool->stride;  // floor division
}
// -------------------------------------------------------------------------------- 

inline size_t pool_in_use_blocks(const pool_t* pool) {
    if (!pool) {
        errno = EINVAL;
        return 0;
    }

    const size_t total = pool->total_blocks;
    const size_t free  = pool->free_blocks;
    const size_t bump  = pool_bump_remaining_blocks(pool);

    // Guard against any accidental underflow if counters drift.
    if (total <= free + bump) return 0;
    return total - free - bump;
}
// -------------------------------------------------------------------------------- 

inline bool pool_owns_arena(const pool_t* pool) {
    if (!pool) {
        errno = EINVAL;
        return false;
    }
    return pool->owns_arena;
}
// -------------------------------------------------------------------------------- 

inline bool pool_grow_enabled(const pool_t* pool) {
    if (!pool) {
        errno = EINVAL;
        return false;
    }
    return pool->grow_enabled;
}
// -------------------------------------------------------------------------------- 

inline alloc_t pool_mtype(const pool_t* pool) {
    if (!pool || !pool->arena) {
        errno = EINVAL;
        return ALLOC_INVALID;
    }
    return arena_mtype(pool->arena);
}
// -------------------------------------------------------------------------------- 

inline void toggle_pool_growth(pool_t* pool, bool toggle) {
    if (!pool || !pool->arena) {
        errno = EINVAL;
        return;
    }
    
#if !ARENA_ENABLE_DYNAMIC
    errno = ENOTSUP;
    return;
#else
    if (pool->arena->mem_type == STATIC) {
        errno = EPERM;
        return;
    }
    pool->grow_enabled = toggle;
    return;
#endif
}
// -------------------------------------------------------------------------------- 

bool pool_stats(const pool_t *pool, char *buffer, size_t buffer_size) {
    size_t offset = 0U;

    if (!buffer || buffer_size == 0U) {
        errno = EINVAL;
        return false;
    }

    if (!pool) {
        (void)_buf_appendf(buffer, buffer_size, &offset, "%s", "Pool: NULL\n");
        return true;
    }

    // Gather fields via getters so we preserve opacity and any validation
    const bool   owns  = pool_owns_arena(pool);
    const bool   grow  = pool_grow_enabled(pool);
    const size_t bsz   = pool_block_size(pool);
    const size_t stride= pool_stride(pool);
    const size_t align = pool_alignment(pool);
    const size_t total = pool_total_blocks(pool);
    const size_t freeb = pool_free_blocks(pool);
    const size_t bump  = pool_bump_remaining_blocks(pool);
    const size_t inuse = pool_in_use_blocks(pool);
    const alloc_t mt   = pool_mtype(pool);

    // Header
    if (!_buf_appendf(buffer, buffer_size, &offset, "%s", "Pool Statistics:\n")) {
        return false;
    }

    // Kind / ownership / growth
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Kind: %s\n", (mt == STATIC) ? "STATIC" : "DYNAMIC")) {
        return false;
    }
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Owns arena: %s\n", owns ? "yes" : "no")) {
        return false;
    }
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Growth enabled: %s\n", grow ? "yes" : "no")) {
        return false;
    }

    // Geometry
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Block size: %zu bytes\n", bsz)) {
        return false;
    }
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Stride: %zu bytes\n", stride)) {
        return false;
    }
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Alignment: %zu bytes\n", align)) {
        return false;
    }

    // Capacity (in blocks)
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Total blocks: %zu\n", total)) {
        return false;
    }
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Free-list blocks: %zu\n", freeb)) {
        return false;
    }
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Bump-remaining blocks: %zu\n", bump)) {
        return false;
    }
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  In-use blocks: %zu\n", inuse)) {
        return false;
    }

    // Utilization (in-use / total)
    if (total == 0U) {
        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "%s", "  Utilization: N/A (total is 0)\n")) {
            return false;
        }
    } else {
        double util = (100.0 * (double)inuse) / (double)total;
        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "  Utilization: %.1f%%\n", util)) {
            return false;
        }
    }

#ifdef DEBUG
    // Slice list (only when tracked)
    {
        size_t idx = 0;
        if (!_buf_appendf(buffer, buffer_size, &offset, "%s", "  Slices:\n")) {
            return false;
        }
        for (const pool_slice_t *s = pool->slices; s; s = s->next) {
            ++idx;
            if (!_buf_appendf(buffer, buffer_size, &offset,
                              "    [%zu] %p .. %p  (%zu bytes)\n",
                              idx, (void*)s->start, (void*)s->end,
                              (size_t)(s->end - s->start))) {
                return false;
            }
        }
        if (idx == 0) {
            if (!_buf_appendf(buffer, buffer_size, &offset, "%s", "    (none)\n")) {
                return false;
            }
        }
    }
#endif

    return true;
}
// -------------------------------------------------------------------------------- 

PoolCheckPoint save_pool(const pool_t* pool) {
    PoolCheckPoint pub = {0};
    
    if (!pool) {
        return pub;
    }
    
    PoolCheckpointRep rep = {0};
    rep.free_list    = pool->free_list;
    rep.free_blocks  = pool->free_blocks;
    rep.cur          = pool->cur;
    rep.end          = pool->end;
    rep.total_blocks = pool->total_blocks;
    
#ifdef DEBUG
    rep.slices = pool->slices;  // Capture slice list head for validation
#endif
    
    _cp_pack_pool(&pub, &rep);
    return pub;
}
// -------------------------------------------------------------------------------- 

bool restore_pool(pool_t* pool, PoolCheckPoint cp) {
    if (!pool) {
        errno = EINVAL;
        return false;
    }
    
    PoolCheckpointRep rep = {0};
    _cp_unpack_pool(&cp, &rep);
    
    // Empty checkpoint is a successful no-op
    if (!rep.cur && !rep.end && rep.total_blocks == 0) {
        return true;
    }
    
    // ---- Validation: detect if pool was reset ----
    
    // If checkpoint has pointers but pool is completely reset, reject
    if (rep.cur && rep.end) {
        // Pool was reset if both cur and end are NULL
        if (!pool->cur && !pool->end && pool->total_blocks == 0) {
            errno = EINVAL;  // Pool was reset since checkpoint
            return false;
        }
        
        // Basic sanity checks
        if (rep.cur > rep.end) {
            errno = EINVAL;
            return false;
        }
        
        // Verify the checkpoint's slice is within our arena's memory
        if (!is_arena_ptr(pool->arena, rep.cur) || 
            !is_arena_ptr(pool->arena, rep.end - 1)) {
            errno = EINVAL;
            return false;
        }
    }
    
#ifdef DEBUG
    // Debug build: check if slice tracking was reset
    if (rep.slices && !pool->slices) {
        errno = EINVAL;  // Pool was reset (slice list cleared)
        return false;
    }
    
    // Verify the checkpoint's slice still exists in current list
    if (rep.slices) {
        bool found = false;
        for (pool_slice_t* s = pool->slices; s; s = s->next) {
            if (s == rep.slices) {
                found = true;
                break;
            }
        }
        if (!found) {
            errno = EINVAL;
            return false;
        }
    }
#endif
    
    // ---- Additional check: pool state consistency ----
    
    // If checkpoint thinks pool had blocks, but current pool has zero total_blocks,
    // it likely was reset
    if (rep.total_blocks > 0 && pool->total_blocks == 0) {
        errno = EINVAL;  // Pool state incompatible with checkpoint
        return false;
    }
    
    // ---- Restore pool state ----
    
    pool->cur          = rep.cur;
    pool->end          = rep.end;
    pool->free_list    = rep.free_list;
    pool->free_blocks  = rep.free_blocks;
    pool->total_blocks = rep.total_blocks;
    
    return true;
}
// -------------------------------------------------------------------------------- 

inline size_t pool_size(const pool_t* pool) {
    if (!pool) {
        errno = EINVAL;
        return 0;
    }
    size_t blocks = pool_in_use_blocks(pool);
    return blocks * pool->block_size;
}
// -------------------------------------------------------------------------------- 

inline size_t pool_alloc(const pool_t* pool) {
    if (!pool) {
        errno = EINVAL;
        return 0;
    }
    return pool->total_blocks;
}
// -------------------------------------------------------------------------------- 

inline size_t pool_footprint(const pool_t* pool) {
    if (!pool) {
        errno = EINVAL;
        return 0;
    }
    return pool->total_blocks * pool->stride;
}
// -------------------------------------------------------------------------------- 

inline bool pool_owns_memory(const pool_t* pool) {
    if (!pool) {
        errno = EINVAL;
        return false;
    }
    return pool->owns_arena;
}
// ================================================================================ 
// ================================================================================ 
// FREE LIST IMPLEMENTATION 

static size_t FREELIST_DEFAULT_MIN_ALLOC = 16;

typedef struct free_block {
    size_t size;
    struct free_block* next;
} free_block_t;
// -------------------------------------------------------------------------------- 

typedef struct freelist_header {
    size_t block_size;  // total size of the allocated block (from block_start)
    size_t offset;      // (uint8_t*)user_ptr - (uint8_t*)block_start
} freelist_header_t;
// -------------------------------------------------------------------------------- 

struct freelist_t {
    free_block_t* head;     // Head of free list - accessed first in alloc
    uint8_t*      cur;      // High-water mark - updated on alloc
    size_t        len;      // Current usage - updated on alloc/free
    size_t        alignment;// Checked on every alloc
    void*         memory;   // Start of memory region (for reset/bounds checking)
    size_t        alloc;    // Total usable memory
    size_t        tot_alloc;// Total including overhead
    arena_t*      parent_arena;  // Parent arena reference
    bool          owns_memory;   // Ownership flag
    uint8_t       _pad[7];       // Explicit padding to 8-byte boundary
};
// -------------------------------------------------------------------------------- 

static size_t  freelist_min_request = sizeof(free_block_t);
// -------------------------------------------------------------------------------- 

freelist_t* init_freelist_with_arena(arena_t* arena,
                                     size_t  size,
                                     size_t  alignment)
{
    if (!arena) {
        errno = EINVAL;
        return NULL;
    }

    if (size < freelist_min_request) {
        errno = EINVAL;
        return NULL;
    }

    // Normalize alignment (0 => at least max_align_t, power of two)
    if (alignment == 0) alignment = alignof(max_align_t);
    if (!_is_pow2(alignment)) {
        alignment = _next_pow2(alignment);
        if (!alignment) { errno = EINVAL; return NULL; }
    }
    if (alignment < alignof(max_align_t)) {
        alignment = alignof(max_align_t);
    }

    // Calculate total allocation: freelist_t struct + usable memory
    size_t struct_size = _align_up_size(sizeof(freelist_t), alignment);
    size_t usable_size = _align_up_size(size, alignment);
    size_t total_alloc = struct_size + usable_size;

    // Ensure usable_size is at least large enough for free_block_t
    if (usable_size < sizeof(free_block_t)) {
        usable_size = sizeof(free_block_t);
        total_alloc = struct_size + usable_size;
    }

    // Single allocation from arena for everything
    void_ptr_expect_t expect = alloc_arena(arena, total_alloc, false);
    if (!expect.has_value) {
        return NULL;
    }
    void* base = expect.u.value;

    // freelist_t struct at the beginning
    freelist_t* fl = (freelist_t*)base;
    memset(fl, 0, sizeof *fl);

    // Usable memory starts after the struct (aligned)
    void* memory = (uint8_t*)base + struct_size;

    // Initialize the freelist structure
    fl->memory       = memory;
    fl->cur          = (uint8_t*)memory;    // Next available slot (initially at start)
    fl->len          = 0;
    fl->alloc        = usable_size;
    fl->tot_alloc    = total_alloc;
    fl->alignment    = alignment;
    fl->owns_memory  = false;               // Underlying arena is owned by caller
    fl->parent_arena = arena;

    // Initialize with one large free block
    fl->head         = (free_block_t*)memory;
    fl->head->size   = usable_size;
    fl->head->next   = NULL;

    return fl;
}
// -------------------------------------------------------------------------------- 

inline size_t min_freelist_alloc() {
    return freelist_min_request;
}
// -------------------------------------------------------------------------------- 

freelist_t* init_dynamic_freelist(size_t bytes, size_t alignment, bool resize) {
#if ARENA_ENABLE_DYNAMIC
    /* Treat `bytes` as the desired minimum usable payload. */
    if (bytes < freelist_min_request) {
        errno = EINVAL;
        return NULL;
    } 

    if (bytes == 0) {
        bytes = FREELIST_DEFAULT_MIN_ALLOC;
    }

    /* Normalize alignment (0 => at least max_align_t, power of two). */
    if (alignment == 0) alignment = alignof(max_align_t);
    if (!_is_pow2(alignment)) {
        alignment = _next_pow2(alignment);
        if (!alignment) {
            errno = EINVAL;
            return NULL;
        }
    }
    if (alignment < alignof(max_align_t)) {
        alignment = alignof(max_align_t);
    }

    /* We know we at least need space for:
       - aligned freelist_t
       - at least one free_block_t
       - and hopefully `bytes` worth of payload.

       We'll ask the arena for a bit more than that to give it room
       for its own internal bookkeeping. The arena may round this up
       or down; we'll query the actual available bytes afterward. */
    size_t struct_size_aligned = _align_up_size(sizeof(freelist_t), alignment);
    size_t min_free_region     = sizeof(free_block_t);
    size_t requested_payload   = bytes;

    /* Guard against overflow in the sums. */
    if (struct_size_aligned > SIZE_MAX - min_free_region ||
        struct_size_aligned + min_free_region > SIZE_MAX - requested_payload) {
        errno = EINVAL;
        return NULL;
    }

    size_t min_total_user = struct_size_aligned + min_free_region + requested_payload;

    /* Ask the arena to manage at least min_total_user bytes of user space.
       Let it decide the actual chunk size; we will adapt to the real
       arena_remaining() afterward. */
    bool   arena_resize = resize;     /* currently we still treat freelist as fixed-size;
                                         you can keep this false if you prefer */
    size_t min_chunk    = 0;          /* or some policy if you plan to grow later */

    arena_expect_t a = init_dynamic_arena(min_total_user,
                                        arena_resize,
                                        min_chunk,
                                        alignment);  
    if (!a.has_value) {
        return NULL;
    }
    arena_t* arena = a.u.value;
    if (!arena) {
        return NULL;
    }

    /* Now ask the arena how many bytes it actually exposes as usable. */
    size_t available = arena_remaining(arena);
    if (available < struct_size_aligned + min_free_region) {
        /* Not enough room even for control structures. */
        free_arena(arena);
        errno = ENOMEM;
        return NULL;
    }

    /* Carve `available` bytes from the arena in one shot. */
    void_ptr_expect_t expect2 = alloc_arena(arena, available, false);
    if (!expect2.has_value) {
        free_arena(arena);
        return NULL;
    }
    void* base = expect2.u.value;

    /* freelist_t at the beginning (aligned by construction). */
    freelist_t* fl = (freelist_t*)base;
    memset(fl, 0, sizeof *fl);

    /* Usable memory starts after the aligned freelist_t. */
    void*  memory     = (uint8_t*)base + struct_size_aligned;
    size_t usable_sz  = available - struct_size_aligned;

    /* Initialize freelist */
    fl->memory       = memory;
    fl->cur          = (uint8_t*)memory;
    fl->len          = 0;
    fl->alloc        = usable_sz;      /* actual usable region we got */
    fl->tot_alloc    = available;      /* total bytes carved from arena */
    fl->alignment    = alignment;
    fl->parent_arena = arena;
    fl->owns_memory  = true;

    /* One large free block spans the entire usable region. */
    fl->head       = (free_block_t*)memory;
    fl->head->size = usable_sz;
    fl->head->next = NULL;

    return fl;
#else
    errno = ENOTSUP;
    return NULL;
#endif
}
// -------------------------------------------------------------------------------- 

freelist_t* init_static_freelist(void* buffer, size_t bytes, size_t alignment) {
    if (bytes < freelist_min_request) {
        errno = EINVAL;
        return NULL;
    }

    if (bytes == 0) {
        bytes = FREELIST_DEFAULT_MIN_ALLOC;  // or treat as error
    }

    // Early obvious minimum: struct + at least one free block
    {
        size_t min_needed_raw = sizeof(freelist_t) + sizeof(free_block_t);
        if (bytes < min_needed_raw) {
            errno = EINVAL;
            return NULL;
        }
    }

    // Normalize alignment (0 => at least max_align_t, power of two)
    if (alignment == 0) alignment = alignof(max_align_t);
    if (!_is_pow2(alignment)) {
        alignment = _next_pow2(alignment);
        if (!alignment) { errno = EINVAL; return NULL; }
    }
    if (alignment < alignof(max_align_t)) {
        alignment = alignof(max_align_t);
    }

    // Create the underlying static arena over the user buffer
    arena_expect_t a = init_static_arena(buffer, bytes, alignment);
    if (!a.has_value) {
        return NULL;
    }
    arena_t* arena = a.u.value;
    if (!arena) {
        return NULL;
    }

    // Calculate space needed for freelist struct (aligned)
    size_t freelist_size = _align_up_size(sizeof(freelist_t), alignment);

    // Check if we have enough space for at least freelist + one free block
    size_t arena_bytes = arena_alloc(arena);  // or arena->alloc if that's your API

    size_t min_needed = freelist_size + sizeof(free_block_t);
    if (arena_bytes < min_needed) {
        errno = EINVAL;
        return NULL;  // arena is too small for freelist + first block
    }

    // Usable space is what remains after the freelist header
    size_t usable_size  = arena_bytes - freelist_size;
    size_t total_needed = freelist_size + usable_size;

    // Single allocation from arena for freelist struct + usable memory
    void_ptr_expect_t expect = alloc_arena(arena, total_needed, false);
    if (!expect.has_value) {
        return NULL; 
    }
    void* base = expect.u.value;

    // freelist_t at the beginning
    freelist_t* fl = (freelist_t*)base;
    memset(fl, 0, sizeof *fl);

    // Usable memory starts after the struct (already aligned)
    void* memory = (uint8_t*)base + freelist_size;

    // Initialize freelist
    fl->memory       = memory;              // Start of memory region
    fl->cur          = (uint8_t*)memory;    // Next available slot
    fl->len          = 0;
    fl->alloc        = usable_size;
    fl->tot_alloc    = total_needed;
    fl->alignment    = alignment;
    fl->parent_arena = arena;
    fl->owns_memory  = false;               // user owns the static buffer

    // One large initial free block
    fl->head         = (free_block_t*)memory;
    fl->head->size   = usable_size;
    fl->head->next   = NULL;

    return fl;
}
// -------------------------------------------------------------------------------- 

void free_freelist(freelist_t* fl) {
    if (fl == NULL) { 
        errno = EINVAL; 
        return; 
    }
    
    // Check if we own the memory (dynamic) or not (static)
    if (!fl->owns_memory || freelist_mtype(fl) == STATIC) {
        errno = EPERM;
        return;
    }
    
    if (fl->parent_arena == NULL) {
        errno = EINVAL;
        return;
    }
    
    free_arena(fl->parent_arena);
}
// -------------------------------------------------------------------------------- 

static void* alloc_freelist_internal(freelist_t* fl,
                                     size_t      bytes,
                                     size_t      eff_align,
                                     bool        zeroed) {
    if (!fl || bytes == 0) {
        errno = EINVAL;
        return NULL;
    }

    const size_t header_size = sizeof(freelist_header_t);

    if (bytes > SIZE_MAX - header_size - (eff_align - 1u)) {
        errno = EINVAL;
        return NULL;
    }

    free_block_t** current = &fl->head;

    while (*current) {
        free_block_t* block      = *current;
        uintptr_t     block_addr = (uintptr_t)block;
        uintptr_t     block_end  = block_addr + block->size;

        uintptr_t after_header = block_addr + header_size;
        uintptr_t user_addr    = _align_up_uintptr(after_header, eff_align);
        uintptr_t user_end     = user_addr + bytes;

        if (user_end > block_end) {
            current = &block->next;
            continue;
        }

        size_t offset    = (size_t)(user_addr - block_addr);
        size_t used_size = (size_t)(user_end - block_addr);
        size_t remaining = block->size - used_size;

        size_t block_size_for_hdr;

        if (remaining >= sizeof(free_block_t)) {
            // Split: front portion used for this allocation
            free_block_t* new_block =
                (free_block_t*)((uint8_t*)block + used_size);
            new_block->size = remaining;
            new_block->next = block->next;

            block->size = used_size;
            *current    = new_block;

            block_size_for_hdr = used_size;
        } else {
            // Use entire block
            block_size_for_hdr = block->size;
            user_end           = block_addr + block_size_for_hdr;
            *current           = block->next;
        }

        uint8_t*           user_ptr = (uint8_t*)user_addr;
        freelist_header_t* hdr =
            (freelist_header_t*)(user_ptr - header_size);

        hdr->block_size = block_size_for_hdr;
        hdr->offset     = offset;

        // IMPORTANT: count full block, not just user bytes
        fl->len += block_size_for_hdr;

        uint8_t* block_used_end = (uint8_t*)block + block_size_for_hdr;
        if (block_used_end > fl->cur) {
            fl->cur = block_used_end;
        }

        if (zeroed) {
            memset(user_ptr, 0, bytes);
        }

        return user_ptr;
    }

    errno = ENOMEM;
    return NULL;
}
// -------------------------------------------------------------------------------- 

void* alloc_freelist(freelist_t* fl, size_t bytes, bool zeroed) {
    if (!fl) { errno = EINVAL; return NULL; }
    return alloc_freelist_internal(fl, bytes, fl->alignment, zeroed);
}
// -------------------------------------------------------------------------------- 

void* alloc_freelist_aligned(freelist_t* fl,
                             size_t      bytes,
                             size_t      alignment,
                             bool        zeroed) {
    if (!fl) { errno = EINVAL; return NULL; }
    if (bytes == 0) { errno = EINVAL; return NULL; }

    if (alignment == 0) alignment = fl->alignment;
    if (!_is_pow2(alignment)) {
        alignment = _next_pow2(alignment);
        if (!alignment) { errno = EINVAL; return NULL; }
    }
    if (alignment < fl->alignment) {
        alignment = fl->alignment;
    }

    return alloc_freelist_internal(fl, bytes, alignment, zeroed);
}
// -------------------------------------------------------------------------------- 

void return_freelist_element(freelist_t* fl, void* ptr) {
    if (!fl || !ptr) {
        errno = EINVAL;
        return;
    }

    const size_t header_size = sizeof(freelist_header_t);

    uint8_t* user_ptr   = (uint8_t*)ptr;
    uint8_t* mem_start8 = (uint8_t*)fl->memory;
    uint8_t* mem_end8   = mem_start8 + fl->alloc;

    // Basic bounds: user pointer must be inside region and leave room for header
    if (user_ptr < mem_start8 + header_size || user_ptr > mem_end8) {
        errno = EINVAL;
        return;
    }

    // Header sits immediately before user pointer
    freelist_header_t* hdr =
        (freelist_header_t*)(user_ptr - header_size);

    size_t   block_size = hdr->block_size;
    size_t   offset     = hdr->offset;

    // Reconstruct block start
    uint8_t* block_start = user_ptr - offset;

    uintptr_t block_addr = (uintptr_t)block_start;
    uintptr_t mem_start  = (uintptr_t)fl->memory;
    uintptr_t mem_end    = mem_start + fl->alloc;

    // Sanity checks on block size and bounds
    if (block_size < sizeof(free_block_t) || block_size > fl->alloc) {
        errno = EINVAL;
        return;
    }
    if (block_addr < mem_start || block_addr + block_size > mem_end) {
        errno = EINVAL;
        return;
    }

    // Also ensure offset is sane (block_size must cover offset + header at least)
    if (offset > block_size) {
        errno = EINVAL;
        return;
    }

    // Accounting: we charged block_size on alloc, so undo exactly that
    if (fl->len < block_size) {
        errno = EINVAL;
        return;
    }
    fl->len -= block_size;

    // Turn region back into a free block
    free_block_t* block = (free_block_t*)block_start;
    block->size = block_size;

    // Insert into free list in address order
    free_block_t* prev = NULL;
    free_block_t* curr = fl->head;

    while (curr && curr < block) {
        prev = curr;
        curr = curr->next;
    }

    block->next = curr;
    if (prev) {
        prev->next = block;
    } else {
        fl->head = block;
    }

    // Coalesce with next block if adjacent
    if (block->next) {
        uint8_t* block_end = (uint8_t*)block + block->size;
        if (block_end == (uint8_t*)block->next) {
            block->size += block->next->size;
            block->next  = block->next->next;
        }
    }

    // Coalesce with previous block if adjacent
    if (prev) {
        uint8_t* prev_end = (uint8_t*)prev + prev->size;
        if (prev_end == (uint8_t*)block) {
            prev->size += block->size;
            prev->next  = block->next;
        }
    }
}
// -------------------------------------------------------------------------------- 

void* realloc_freelist(freelist_t* fl,
                       void*       ptr,
                       size_t      old_size,
                       size_t      new_size,
                       bool        zeroed) {
    if (!fl) {
        errno = EINVAL;
        return NULL;
    }

    // NULL ptr behaves like alloc
    if (!ptr) {
        return alloc_freelist(fl, new_size, zeroed);
    }

    // Shrink or same size: keep pointer
    if (new_size <= old_size) {
        return ptr;
    }

    // Need to grow
    void* new_ptr = alloc_freelist(fl, new_size, false);
    if (!new_ptr) {
        return NULL;    // errno set by alloc_freelist
    }

    memcpy(new_ptr, ptr, old_size);

    if (zeroed) {
        memset((uint8_t*)new_ptr + old_size, 0, new_size - old_size);
    }

    return_freelist_element(fl, ptr);
    return new_ptr;
}
// -------------------------------------------------------------------------------- 

void* realloc_freelist_aligned(freelist_t* fl,
                               void*       ptr,
                               size_t      old_size,
                               size_t      new_size,
                               bool        zeroed,
                               size_t      alignment) {
    if (!fl) {
        errno = EINVAL;
        return NULL;
    }

    // NULL ptr behaves like aligned alloc
    if (!ptr) {
        return alloc_freelist_aligned(fl, new_size, alignment, zeroed);
    }

    // Shrink or same size: keep pointer
    if (new_size <= old_size) {
        return ptr;
    }

    // Need to grow with requested alignment
    void* new_ptr = alloc_freelist_aligned(fl, new_size, alignment, false);
    if (!new_ptr) {
        return NULL;    // errno set by alloc_freelist_aligned
    }

    memcpy(new_ptr, ptr, old_size);

    if (zeroed) {
        memset((uint8_t*)new_ptr + old_size, 0, new_size - old_size);
    }

    return_freelist_element(fl, ptr);
    return new_ptr;
}
// -------------------------------------------------------------------------------- 

inline void reset_freelist(freelist_t* fl) {
    if (!fl) {
        errno = EINVAL;
        return;
    }

    if (!fl->memory || fl->alloc == 0) {
        // Freelist not properly initialized or already torn down
        errno = EINVAL;
        return;
    }

    // Reset accounting
    fl->cur = (uint8_t*)fl->memory;
    fl->len = 0;

    // Recreate a single large free block covering the entire region
    free_block_t* head = (free_block_t*)fl->memory;
    head->size = fl->alloc;
    head->next = NULL;

    fl->head = head;
}
// -------------------------------------------------------------------------------- 

bool is_freelist_ptr(const freelist_t* fl, const void* ptr) {
    if (!fl || !ptr) {
        return false;
    }

    const size_t header_size = sizeof(freelist_header_t);

    uintptr_t mem_start = (uintptr_t)fl->memory;
    uintptr_t mem_end   = mem_start + fl->alloc;

    uintptr_t ptr_addr  = (uintptr_t)ptr;

    // Pointer must be within the managed region and leave room for the header
    if (ptr_addr < mem_start + header_size || ptr_addr > mem_end) {
        return false;
    }

    // Header sits immediately before the user pointer
    const uint8_t*        user_ptr = (const uint8_t*)ptr;
    const freelist_header_t* hdr =
        (const freelist_header_t*)(user_ptr - header_size);

    size_t block_size = hdr->block_size;
    size_t offset     = hdr->offset;

    // Reconstruct block start
    const uint8_t* block_start = user_ptr - offset;
    uintptr_t      block_addr  = (uintptr_t)block_start;

    // Basic sanity on offset and size
    // block_size must be large enough to cover the offset and at least a free_block_t
    if (offset > block_size) {
        return false;
    }
    if (block_size < sizeof(free_block_t) || block_size > fl->alloc) {
        return false;
    }

    // block_start must be within the freelist region
    if (block_addr < mem_start || block_addr >= mem_end) {
        return false;
    }

    // Check that the full block fits within the region, overflow-safe:
    // block_addr + block_size <= mem_end  <=>  block_size <= mem_end - block_addr
    if (block_size > (size_t)(mem_end - block_addr)) {
        return false;
    }

    // Also check that ptr lies within [block_start, block_start + block_size)
    if (ptr_addr < block_addr || ptr_addr >= block_addr + block_size) {
        return false;
    }

    return true;
}
// -------------------------------------------------------------------------------- 

bool is_freelist_ptr_sized(const freelist_t* fl, const void* ptr, size_t size) {
    if (!fl || !ptr || size == 0) {
        return false;
    }

    // First check if it's at least a plausible freelist pointer
    if (!is_freelist_ptr(fl, ptr)) {
        return false;
    }

    const size_t header_size = sizeof(freelist_header_t);
    const uint8_t* user_ptr  = (const uint8_t*)ptr;

    const freelist_header_t* hdr =
        (const freelist_header_t*)(user_ptr - header_size);

    size_t block_size = hdr->block_size;
    size_t offset     = hdr->offset;

    if (offset > block_size) {
        return false;
    }

    // User data size = block_size - offset
    size_t user_data_size = block_size - offset;

    // Requested size must fit within the user data region
    if (size > user_data_size) {
        return false;
    }

    // Also verify that ptr + size doesn't run off the freelist region
    uintptr_t ptr_addr  = (uintptr_t)ptr;
    uintptr_t mem_start = (uintptr_t)fl->memory;
    uintptr_t mem_end   = mem_start + fl->alloc;

    // Overflow-safe: ptr_addr + size <= mem_end  <=>  size <= mem_end - ptr_addr
    if (size > (size_t)(mem_end - ptr_addr)) {
        return false;
    }

    return true;
}
// -------------------------------------------------------------------------------- 

inline size_t freelist_remaining(const freelist_t* fl) {
    if (!fl) {
        errno = EINVAL;
        return 0;
    }
    
    // Calculate remaining as total usable minus currently used
    return fl->alloc - fl->len;
}
// -------------------------------------------------------------------------------- 

inline alloc_t freelist_mtype(const freelist_t* fl) {
    if (!fl) {
        errno = EINVAL;
        return ALLOC_INVALID;
    }
    return arena_mtype(fl->parent_arena);
}
// -------------------------------------------------------------------------------- 

inline size_t freelist_size(const freelist_t* fl) {
    if (!fl) {
        errno = EINVAL;
        return 0;
    }
    return fl->len;
}
// -------------------------------------------------------------------------------- 

inline size_t freelist_alloc(const freelist_t* fl) {
    if (!fl) {
        errno = EINVAL;
        return 0;
    }
    return fl->alloc;
}
// -------------------------------------------------------------------------------- 

inline size_t total_freelist_alloc(const freelist_t* fl) {
    if (!fl) {
        errno = EINVAL;
        return 0;
    }
    return fl->tot_alloc;
}
// -------------------------------------------------------------------------------- 

inline size_t freelist_alignment(const freelist_t* fl) {
    if (!fl) {
        errno = EINVAL;
        return 0;
    }
    return fl->alignment;
}
// -------------------------------------------------------------------------------- 

bool freelist_owns_arena(const freelist_t* fl) {
    if (!fl) {
        errno = EINVAL;
        return false;
    }
    return fl->owns_memory;
}
// -------------------------------------------------------------------------------- 

bool freelist_stats(const freelist_t *fl, char *buffer, size_t buffer_size) {
    size_t offset = 0u;

    if ((buffer == NULL) || (buffer_size == 0u)) {
        errno = EINVAL;
        return false;
    }

    if (fl == NULL) {
        (void)_buf_appendf(buffer, buffer_size, &offset, "%s", "Freelist: NULL\n");
        return true;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "%s", "Freelist Statistics:\n")) {
        return false;
    }

    /* Type / ownership information */
    {
        alloc_t mtype = freelist_mtype(fl);
        const char *type_str = "UNKNOWN";
        switch (mtype) {
            case STATIC:  type_str = "STATIC";  break;
            case DYNAMIC: type_str = "DYNAMIC"; break;
            default:      type_str = "UNKNOWN"; break;
        }

        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "  Type: %s\n", type_str)) {
            return false;
        }

        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "  Owns arena: %s\n",
                          freelist_owns_arena(fl) ? "yes" : "no")) {
            return false;
        }
    }

    /* Basic accounting */
    {
        size_t used      = freelist_size(fl);        /* fl->len */
        size_t capacity  = freelist_alloc(fl);       /* fl->alloc */
        size_t total     = total_freelist_alloc(fl); /* fl->tot_alloc */
        size_t remaining = freelist_remaining(fl);   /* alloc - len */

        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "  Used (accounted): %zu bytes\n", used)) {
            return false;
        }

        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "  Remaining: %zu bytes\n", remaining)) {
            return false;
        }

        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "  Capacity (usable region): %zu bytes\n", capacity)) {
            return false;
        }

        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "  Total (with header/overhead): %zu bytes\n", total)) {
            return false;
        }

        /* Utilization of the usable freelist region */
        if (capacity == 0u) {
            if (!_buf_appendf(buffer, buffer_size, &offset,
                              "%s", "  Utilization: N/A (capacity is 0)\n")) {
                return false;
            }
        } else {
            double util = (100.0 * (double)used) / (double)capacity;
            if (!_buf_appendf(buffer, buffer_size, &offset,
                              "  Utilization: %.1f%%\n", util)) {
                return false;
            }
        }

        /* Alignment info */
        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "  Base alignment: %zu bytes\n",
                          freelist_alignment(fl))) {
            return false;
        }
    }

    /* Free list layout */
    {
        const free_block_t *current = fl->head;
        int   block_count           = 0;
        size_t free_bytes           = 0u;

        while (current != NULL) {
            block_count += 1;
            free_bytes  += current->size;

            if (!_buf_appendf(buffer, buffer_size, &offset,
                              "  Free block %d: %p, %zu bytes\n",
                              block_count,
                              (const void *)current,
                              current->size)) {
                return false;
            }

            current = current->next;
        }

        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "  Free blocks: %d, total free bytes (raw): %zu\n",
                          block_count, free_bytes)) {
            return false;
        }
    }

    return true;
}
// ================================================================================ 
// ================================================================================ 
// BUDDY ALLOCATOR 

/* Free block node (stored in the block memory itself when free). */
typedef struct buddy_block {
    struct buddy_block *next;
} buddy_block_t;

/* Per-block header (stored at the start of every allocated block). */
typedef struct buddy_header {
    uint32_t order;   /* log2(block_size) */
    size_t   block_offset;
} buddy_header_t;

/* Internal definition of buddy_t (opaque to users). */ 
struct buddy_t {
    /* Hot-ish fields used in most operations */
    void           *base;
    buddy_block_t **free_lists;

    /* Usage / capacity */
    size_t          pool_size;
    size_t          len;
    size_t          alloc;
    size_t          total_alloc;

    /* Alignment policy */
    size_t          base_align;   /* per-allocator minimum alignment  */
    size_t          user_offset;  /* offset from block start to user  */

    /* Small integers / counts */
    uint32_t        min_order;
    uint32_t        max_order;
    uint32_t        num_levels;
    uint8_t         _pad[4];      /* keep sizeof(buddy_t) % 8 == 0    */
};
// -------------------------------------------------------------------------------- 

/* --- OS abstraction layer: get/release a big contiguous region --- */

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    static void *buddy_os_alloc(size_t size) {
        /* Round to page size if you want to be strict; VirtualAlloc
           already works in page granularity. */
        return VirtualAlloc(NULL, size,
                            MEM_RESERVE | MEM_COMMIT,
                            PAGE_READWRITE);
    }

    static void buddy_os_free(void *ptr, size_t size) {
        (void)size; /* not needed for MEM_RELEASE */
        if (ptr) {
            VirtualFree(ptr, 0, MEM_RELEASE);
        }
    }

#else  /* POSIX: Linux, BSD, macOS, etc. */

    #define _GNU_SOURCE
    #include <sys/mman.h>
    #include <unistd.h>

    static void *buddy_os_alloc(size_t size) {
        void *p = mmap(NULL, size,
                       PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS,
                       -1, 0);
        return (p == MAP_FAILED) ? NULL : p;
    }

    static void buddy_os_free(void *ptr, size_t size) {
        if (ptr && size) {
            (void)munmap(ptr, size);
        }
    }

#endif
// -------------------------------------------------------------------------------- 

static uint32_t _ilog2_size(size_t x) {
    uint32_t r = 0;
    while (x > 1) {
        x >>= 1;
        r++;
    }
    return r;
}
// -------------------------------------------------------------------------------- 

static inline uint32_t _order_to_level(const buddy_t *b, uint32_t order) {
    return order - b->min_order;
}
// -------------------------------------------------------------------------------- 

static int32_t _find_nonempty_level(const buddy_t *b, uint32_t desired_level) {
    for (uint32_t lvl = desired_level; lvl < b->num_levels; ++lvl) {
        if (b->free_lists[lvl] != NULL) {
            return (int32_t)lvl;
        }
    }
    return -1; /* none available */
}
// -------------------------------------------------------------------------------- 

static void _freelist_push(buddy_block_t **head, buddy_block_t *blk) {
    blk->next = *head;
    *head = blk;
}
// -------------------------------------------------------------------------------- 

static bool _freelist_remove(buddy_block_t **head, buddy_block_t *blk) {
    buddy_block_t *prev = NULL;
    buddy_block_t *cur  = *head;
    while (cur) {
        if (cur == blk) {
            if (prev) prev->next = cur->next;
            else      *head = cur->next;
            return true;
        }
        prev = cur;
        cur  = cur->next;
    }
    return false;
}
// -------------------------------------------------------------------------------- 

static buddy_block_t *_freelist_find(buddy_block_t *head, void *addr) {
    while (head) {
        if ((void *)head == addr) return head;
        head = head->next;
    }
    return NULL;
}
// -------------------------------------------------------------------------------- 

static inline uint32_t _level_to_order(const buddy_t *b, uint32_t level) {
    return b->min_order + level;
}
// -------------------------------------------------------------------------------- 

buddy_t *init_buddy_allocator(size_t pool_size,
                              size_t min_block_size,
                              size_t base_align) {
    if (pool_size == 0 || min_block_size == 0) {
        errno = EINVAL;
        return NULL;
    }

    buddy_t *b = calloc(1, sizeof(*b));
    if (!b) { errno = ENOMEM; return NULL; }

    /* 1) Normalize base alignment */
    if (base_align == 0) {
        base_align = alignof(max_align_t);
    }
    if ((base_align & (base_align - 1u)) != 0u) {
        base_align = _next_pow2(base_align);
        if (!base_align) { free(b); errno = EINVAL; return NULL; }
    }

    /* 2) Compute header-aligned user offset */
    size_t header_size = sizeof(buddy_header_t);

    /* Align the user pointer to base_align relative to block start */
    size_t user_offset = (header_size + (base_align - 1u)) & ~(base_align - 1u);

    /* 3) Ensure min_block_size is at least big enough to hold header+alignment */
    if (min_block_size < user_offset) {
        min_block_size = user_offset;
    }

    /* 4) Round min_block_size and pool_size up to powers of two */
    size_t min_blk = _next_pow2(min_block_size);
    size_t pool    = _next_pow2(pool_size);

    if (min_blk > pool) {
        errno = EINVAL;
        free(b);
        return NULL;
    }

    uint32_t min_order  = _ilog2_size(min_blk);
    uint32_t max_order  = _ilog2_size(pool);
    uint32_t num_levels = max_order - min_order + 1;

    if (num_levels == 0) {
        errno = EINVAL;
        free(b);
        return NULL;
    }

    void *base = buddy_os_alloc(pool);
    if (!base) {
        errno = ENOMEM;
        free(b);
        return NULL;
    }

    buddy_block_t **free_lists =
        calloc(num_levels, sizeof(buddy_block_t *));
    if (!free_lists) {
        buddy_os_free(base, pool);
        free(b);
        errno = ENOMEM;
        return NULL;
    }

    b->base       = base;
    b->pool_size  = pool;
    b->min_order  = min_order;
    b->max_order  = max_order;
    b->num_levels = num_levels;
    b->free_lists = free_lists;

    b->alloc       = pool;
    b->len         = 0;
    b->total_alloc = pool
                   + num_levels * sizeof(buddy_block_t *)
                   + sizeof(*b);

    b->base_align  = base_align;
    b->user_offset = user_offset;

    buddy_block_t *initial_block = (buddy_block_t *)base;
    initial_block->next = NULL;

    uint32_t top_level = _order_to_level(b, max_order);
    b->free_lists[top_level] = initial_block;

    return b;
}
// -------------------------------------------------------------------------------- 

void free_buddy(buddy_t *b) {
    if (!b) return;

    if (b->base && b->pool_size) {
        buddy_os_free(b->base, b->pool_size);
    }

    free(b->free_lists);
    memset(b, 0, sizeof(*b));
    free(b);
}
// -------------------------------------------------------------------------------- 

void *alloc_buddy(buddy_t *b, size_t size, bool zeroed) {
    if (!b || size == 0) {
        errno = EINVAL;
        return NULL;
    }

    /* Include header overhead. */
    size_t total = size + sizeof(buddy_header_t);

    /* Ensure at least min block size. */
    size_t min_block = (size_t)1u << b->min_order;
    if (total < min_block) {
        total = min_block;
    } else {
        total = _next_pow2(total);
    }

    if (total > b->pool_size) {
        errno = ENOMEM;
        return NULL;
    }

    uint32_t order = _ilog2_size(total);
    if (order < b->min_order) order = b->min_order;
    if (order > b->max_order) {
        errno = ENOMEM;
        return NULL;
    }

    uint32_t desired_level = _order_to_level(b, order);
    int32_t  lvl           = _find_nonempty_level(b, desired_level);

    if (lvl < 0) {
        errno = ENOMEM;
        return NULL;
    }

    /* Take a block from level 'lvl'. */
    buddy_block_t *block = b->free_lists[lvl];
    b->free_lists[lvl]   = block->next;
    block->next          = NULL;

    uint32_t current_order = _level_to_order(b, (uint32_t)lvl);
    size_t   current_size  = (size_t)1u << current_order;

    /* Split down until we reach the desired order. */
    while (current_order > order) {
        current_order--;
        current_size >>= 1;

        buddy_block_t *split_block =
            (buddy_block_t *)((uint8_t *)block + current_size);
        split_block->next = NULL;

        uint32_t split_level = _order_to_level(b, current_order);
        _freelist_push(&b->free_lists[split_level], split_block);
    }

    /* Now 'block' is the final block of size 2^order. */
    uint8_t *block_bytes = (uint8_t *)block;

    /* Header lives at block start in the unaligned case. */
    buddy_header_t *hdr = (buddy_header_t *)block_bytes;
    hdr->order        = order;
    hdr->block_offset = (size_t)(block_bytes - (uint8_t *)b->base);

    size_t block_size = (size_t)1u << order;
    b->len += block_size;

    /* User pointer is just after the header. */
    uint8_t *user_ptr = block_bytes + sizeof(buddy_header_t);

    if (zeroed) {
        memset(user_ptr, 0, block_size - sizeof(buddy_header_t));
    }

    return (void *)user_ptr;
}
// -------------------------------------------------------------------------------- 

void *alloc_buddy_aligned(buddy_t *b,
                          size_t  size,
                          size_t  align,
                          bool    zeroed) {
    if (!b || size == 0) {
        errno = EINVAL;
        return NULL;
    }

    if (align == 0) {
        /* Treat 0 as "natural" alignment. */
        align = alignof(max_align_t);
    }

    /* Require power-of-two alignment. */
    if (align & (align - 1u)) {
        align = _next_pow2(align);
        if (!align) { errno = EINVAL; return NULL; }
    }

    /* We need room for:
       - header
       - requested payload
       - worst-case alignment padding (align-1)
    */
    size_t total = size + sizeof(buddy_header_t) + (align - 1u);

    size_t min_block = (size_t)1u << b->min_order;
    if (total < min_block) {
        total = min_block;
    } else {
        total = _next_pow2(total);
    }

    if (total > b->pool_size) {
        errno = ENOMEM;
        return NULL;
    }

    uint32_t order = _ilog2_size(total);
    if (order < b->min_order) order = b->min_order;
    if (order > b->max_order) {
        errno = ENOMEM;
        return NULL;
    }

    uint32_t desired_level = _order_to_level(b, order);
    int32_t  lvl           = _find_nonempty_level(b, desired_level);

    if (lvl < 0) {
        errno = ENOMEM;
        return NULL;
    }

    /* Take a block from level 'lvl'. */
    buddy_block_t *block = b->free_lists[lvl];
    b->free_lists[lvl]   = block->next;
    block->next          = NULL;

    uint32_t current_order = _level_to_order(b, (uint32_t)lvl);
    size_t   current_size  = (size_t)1u << current_order;

    /* Split down until we reach the desired order. */
    while (current_order > order) {
        current_order--;
        current_size >>= 1;

        buddy_block_t *split_block =
            (buddy_block_t *)((uint8_t *)block + current_size);
        split_block->next = NULL;

        uint32_t split_level = _order_to_level(b, current_order);
        _freelist_push(&b->free_lists[split_level], split_block);
    }

    size_t   block_size  = (size_t)1u << order;
    uint8_t *block_bytes = (uint8_t *)block;

    /* Find an aligned user pointer inside this block.
       We want user_ptr aligned to 'align', with enough room
       immediately before it for the header.
    */

    uintptr_t block_addr = (uintptr_t)block_bytes;

    /* Minimal place we can put the user pointer: after header. */
    uintptr_t min_user = block_addr + sizeof(buddy_header_t);

    /* Candidate aligned user pointer. */
    uintptr_t aligned_user =
        (min_user + (align - 1u)) & ~(uintptr_t)(align - 1u);

    /* Ensure the header + payload fit inside this block. */
    if (aligned_user + size > block_addr + block_size) {
        /* In theory this shouldn't happen because we over-allocated with +align-1,
           but be defensive: treat as ENOMEM. */
        errno = ENOMEM;
        /* Return block to free list. */
        uint32_t lvl_final = _order_to_level(b, order);
        _freelist_push(&b->free_lists[lvl_final], block);
        return NULL;
    }

    uint8_t *user_ptr = (uint8_t *)aligned_user;

    /* Header lives immediately before user_ptr. */
    buddy_header_t *hdr =
        (buddy_header_t *)(user_ptr - sizeof(buddy_header_t));
    hdr->order        = order;
    hdr->block_offset = (size_t)(block_bytes - (uint8_t *)b->base);

    b->len += block_size;

    if (zeroed) {
        /* Zero the entire usable payload region behind user_ptr.
           You could also just zero [0, size) if you prefer.
         */
        size_t payload_bytes =
            (block_bytes + block_size) - user_ptr;
        memset(user_ptr, 0, payload_bytes);
    }

    return (void *)user_ptr;
}

// --------------------------------------------------------------------------------

bool return_buddy_element(buddy_t *b, void *ptr) {
    if (!b) {
        errno = EINVAL;
        return false;
    }
    if (!ptr) {
        /* Like free(NULL): no-op, success. */
        return true;
    }

    uint8_t *user = (uint8_t *)ptr;

    /* Header is immediately before the user pointer. */
    buddy_header_t *hdr =
        (buddy_header_t *)(user - sizeof(buddy_header_t));

    uint32_t order = hdr->order;
    if (order < b->min_order || order > b->max_order) {
        errno = EINVAL;
        return false;
    }

    size_t block_size = (size_t)1u << order;

    /* Block start is base + block_offset. */
    uint8_t *base  = (uint8_t *)b->base;
    size_t   off   = hdr->block_offset;
    if (off + block_size > b->pool_size) {
        errno = EINVAL;
        return false;
    }

    buddy_block_t *block = (buddy_block_t *)(base + off);

    /* ---- accounting: undo the allocation ---- */
    if (b->len >= block_size) {
        b->len -= block_size;
    } else {
        /* should never happen, but be defensive */
        b->len = 0;
    }

    /* ---- coalesce as usual ---- */
    size_t cur_off = off;
    uint32_t cur_order = order;

    while (cur_order < b->max_order) {
        size_t buddy_off  = cur_off ^ ((size_t)1u << cur_order);
        uint8_t *buddy_addr = base + buddy_off;

        uint32_t lvl = _order_to_level(b, cur_order);

        buddy_block_t *buddy_in_list =
            _freelist_find(b->free_lists[lvl], (void *)buddy_addr);

        if (!buddy_in_list) {
            break;
        }

        /* Remove buddy from free list. */
        _freelist_remove(&b->free_lists[lvl], buddy_in_list);

        /* New merged block starts at the lower address. */
        if (buddy_off < cur_off) {
            cur_off = buddy_off;
            block   = (buddy_block_t *)(base + cur_off);
        }

        cur_order++;
    }

    uint32_t final_level = _order_to_level(b, cur_order);
    _freelist_push(&b->free_lists[final_level], block);

    return true;
}
// -------------------------------------------------------------------------------- 

void* realloc_buddy(buddy_t* buddy,
                    void*    old_ptr,
                    size_t   old_size,
                    size_t   new_size,
                    bool     zeroed) {
    if (!buddy) {
        errno = EINVAL;
        return NULL;
    }

    /* realloc(NULL, n) => malloc-like behavior */
    if (!old_ptr) {
        if (new_size == 0) {
            /* realloc(NULL, 0) is allowed to return NULL. */
            return NULL;
        }
        return alloc_buddy(buddy, new_size, zeroed);
    }

    /* realloc(ptr, 0) => free(ptr), return NULL */
    if (new_size == 0) {
        (void)return_buddy_element(buddy, old_ptr);
        return NULL;
    }

    if (old_size == 0) {
        /* Caller claims old_size is 0 but passed a non-NULL pointer.
           Treat as programmer error. */
        errno = EINVAL;
        return NULL;
    }

    /* Determine how much usable space is actually behind old_ptr. */
    buddy_header_t *hdr =
        (buddy_header_t *)((uint8_t *)old_ptr - sizeof(buddy_header_t));
    uint32_t order      = hdr->order;
    size_t   block_size = (size_t)1u << order;
    size_t   usable_old = block_size - sizeof(buddy_header_t);

    /* If new_size fits in the existing block, no need to move. */
    if (new_size <= usable_old) {
        /* Optionally, if zeroed == true and new_size > old_size,
           you can zero the extension region. */
        if (zeroed && new_size > old_size) {
            size_t extra = new_size - old_size;
            memset((uint8_t *)old_ptr + old_size, 0, extra);
        }
        return old_ptr;
    }

    /* Need a bigger block. */
    void *new_ptr = alloc_buddy(buddy, new_size, zeroed);
    if (!new_ptr) {
        /* errno set by alloc_buddy; old_ptr is still valid. */
        return NULL;
    }

    /* Copy min(logical old size, usable old capacity). */
    size_t copy_bytes = old_size < usable_old ? old_size : usable_old;
    memcpy(new_ptr, old_ptr, copy_bytes);

    /* Return the old block. */
    (void)return_buddy_element(buddy, old_ptr);

    return new_ptr;
}
// -------------------------------------------------------------------------------- 

void *realloc_buddy_aligned(buddy_t *b,
                            void   *old_ptr,
                            size_t  old_size,
                            size_t  new_size,
                            size_t  align,
                            bool    zeroed) {
    if (!b) {
        errno = EINVAL;
        return NULL;
    }

    /* realloc(NULL, n) => alloc */
    if (!old_ptr) {
        if (new_size == 0) {
            return NULL;
        }
        return alloc_buddy_aligned(b, new_size, align, zeroed);
    }

    /* realloc(p, 0) => free(p), return NULL */
    if (new_size == 0) {
        (void)return_buddy_element(b, old_ptr);
        return NULL;
    }

    if (old_size == 0) {
        /* Caller passed non-NULL ptr but claims size 0. Treat as error. */
        errno = EINVAL;
        return NULL;
    }

    if (align == 0) {
        align = alignof(max_align_t);
    }
    if (align & (align - 1u)) {
        align = _next_pow2(align);
        if (!align) { errno = EINVAL; return NULL; }
    }

    /* Introspect old block. */
    buddy_header_t *hdr =
        (buddy_header_t *)((uint8_t *)old_ptr - sizeof(buddy_header_t));
    uint32_t order      = hdr->order;
    size_t   block_size = (size_t)1u << order;

    size_t usable_old =
        block_size - ((size_t)((uint8_t *)old_ptr - (uint8_t *)b->base) - hdr->block_offset)
        - sizeof(buddy_header_t);

    /* Simpler & safer: just treat usable_old as block_size - sizeof(header)
       for both unaligned and aligned allocations. We know user_ptr is always
       within the block and header sits immediately before it. So:
    */
    usable_old = block_size - sizeof(buddy_header_t);

    /* If it fits and alignment is already OK, reuse in place. */
    if (new_size <= usable_old &&
        (((uintptr_t)old_ptr & (align - 1u)) == 0u)) {

        if (zeroed && new_size > old_size) {
            size_t extra = new_size - old_size;
            memset((uint8_t *)old_ptr + old_size, 0, extra);
        }
        return old_ptr;
    }

    /* Need a new block. */
    void *new_ptr = alloc_buddy_aligned(b, new_size, align, zeroed);
    if (!new_ptr) {
        /* errno set by alloc_buddy_aligned; old_ptr is still valid. */
        return NULL;
    }

    size_t copy_bytes = (old_size < usable_old) ? old_size : usable_old;
    memcpy(new_ptr, old_ptr, copy_bytes);

    (void)return_buddy_element(b, old_ptr);

    return new_ptr;
}
// -------------------------------------------------------------------------------- 

bool is_buddy_ptr(const buddy_t *b, const void *ptr) {
    if (!b || !ptr) {
        errno = EINVAL;
        return false;
    }

    const uint8_t *p          = (const uint8_t *)ptr;
    const uint8_t *pool_start = (const uint8_t *)b->base;
    const uint8_t *pool_end   = pool_start + b->pool_size;

    /* Step 0: ptr must lie inside the buddy pool's user range. */
    if (p < pool_start + sizeof(buddy_header_t) || p >= pool_end) {
        errno = EINVAL;
        return false;
    }

    /* Now it's safe to look at the header, which must be in the pool. */
    const buddy_header_t *hdr =
        (const buddy_header_t *)(p - sizeof(buddy_header_t));

    /* Step 2: order must be valid */
    if (hdr->order < b->min_order || hdr->order > b->max_order) {
        errno = EINVAL;
        return false;
    }

    size_t block_size = (size_t)1u << hdr->order;

    /* Step 3: block_offset must be within pool */
    if (hdr->block_offset + block_size > b->pool_size) {
        errno = EINVAL;
        return false;
    }

    /* Step 4: the block must be aligned correctly */
    if (hdr->block_offset & (block_size - 1u)) {
        errno = EINVAL;
        return false;
    }

    /* Step 5: user pointer must lie inside the block */
    const uint8_t *block_start = pool_start + hdr->block_offset;
    const uint8_t *block_end   = block_start + block_size;

    if (p < block_start + sizeof(buddy_header_t) || p >= block_end) {
        errno = EINVAL;
        return false;
    }

    /* Everything checks out */
    return true;
}
//
// bool is_buddy_ptr(const buddy_t *b, const void *ptr) {
//     if (!b || !ptr) {
//         errno = EINVAL;
//         return false;
//     }
//
//     const uint8_t *p = (const uint8_t *)ptr;
//
//     /* Step 1: Header must be directly before user pointer */
//     if (p < (uint8_t *)b->base + sizeof(buddy_header_t)) {
//         errno = EINVAL;
//         return false;
//     }
//
//     const buddy_header_t *hdr =
//         (const buddy_header_t *)(p - sizeof(buddy_header_t));
//
//     /* Step 2: order must be valid */
//     if (hdr->order < b->min_order || hdr->order > b->max_order) {
//         errno = EINVAL;
//         return false;
//     }
//
//     size_t block_size = (size_t)1u << hdr->order;
//
//     /* Step 3: block_offset must be within pool */
//     if (hdr->block_offset + block_size > b->pool_size) {
//         errno = EINVAL;
//         return false;
//     }
//
//     /* Step 4: the block must be aligned correctly */
//     if (hdr->block_offset & (block_size - 1u)) {
//         errno = EINVAL;
//         return false;
//     }
//
//     /* Step 5: user pointer must lie inside the block */
//     const uint8_t *block_start = (const uint8_t *)b->base + hdr->block_offset;
//     const uint8_t *block_end   = block_start + block_size;
//
//     if (p < block_start + sizeof(buddy_header_t) || p >= block_end) {
//         errno = EINVAL;
//         return false;
//     }
//
//     /* Everything checks out */
//     return true;
// }
// -------------------------------------------------------------------------------- 

bool is_buddy_ptr_sized(const buddy_t *b, const void *ptr, size_t size) {
    if (!is_buddy_ptr(b, ptr)) {
        return false;  /* errno already set */
    }

    const buddy_header_t *hdr =
        (const buddy_header_t *)((const uint8_t *)ptr - sizeof(buddy_header_t));
    size_t block_size = (size_t)1u << hdr->order;

    size_t usable = block_size - sizeof(buddy_header_t);

    if (size > usable) {
        errno = ERANGE; /* size does not fit in underlying block */
        return false;
    }

    return true;
}
// -------------------------------------------------------------------------------- 

bool reset_buddy(buddy_t *b) {
    if (!b) {
        errno = EINVAL;
        return false;
    }

    if (!b->base || b->pool_size == 0 ||
        b->num_levels == 0 ||
        b->max_order < b->min_order) {
        errno = EINVAL;
        return false;
    }

    /* Clear all free lists. */
    memset(b->free_lists, 0, b->num_levels * sizeof(buddy_block_t*));

    /* Single big free block spanning the whole pool. */
    buddy_block_t *initial_block = (buddy_block_t *)b->base;
    initial_block->next = NULL;

    uint32_t top_level = _order_to_level(b, b->max_order);
    b->free_lists[top_level] = initial_block;

    /* No bytes “in use” from the pool any more. */
    b->len = 0;

    return true;
}
// -------------------------------------------------------------------------------- 

inline size_t buddy_alloc(const buddy_t *b) {
    if (!b) {
        errno = EINVAL;
        return 0;
    }
    /* b->len counts actual bytes consumed from the pool. */
    return b->len;
}
// -------------------------------------------------------------------------------- 

inline size_t total_buddy_alloc(const buddy_t *b) {
    if (!b) {
        errno = EINVAL;
        return 0;
    }
    /* b->total_alloc = pool_size + metadata */
    return b->total_alloc;
}
// -------------------------------------------------------------------------------- 

inline size_t buddy_size(const buddy_t *b) {
    if (!b) {
        errno = EINVAL;
        return 0;
    }
    /* Current footprint of the entire allocator in memory. */
    return b->total_alloc;
}
// -------------------------------------------------------------------------------- 

inline size_t buddy_remaining(const buddy_t *b) {
    if (!b) {
        errno = EINVAL;
        return 0;
    }

    /* Total free memory in the pool, even if fragmented */
    return (b->pool_size > b->len) ? (b->pool_size - b->len) : 0;
}
// -------------------------------------------------------------------------------- 

inline size_t buddy_largest_block(const buddy_t *b) {
    if (!b) {
        errno = EINVAL;
        return 0;
    }

    /* Scan from highest order (largest blocks) down to smallest. */
    for (int32_t lvl = (int32_t)b->num_levels - 1; lvl >= 0; --lvl) {
        if (b->free_lists[lvl] != NULL) {
            /* This level has at least one free block. */
            uint32_t order = b->min_order + (uint32_t)lvl;
            return (size_t)1u << order;
        }
    }

    return 0;
}
// -------------------------------------------------------------------------------- 

bool buddy_stats(const buddy_t *buddy, char *buffer, size_t buffer_size) {
    size_t offset = 0U;

    if ((buffer == NULL) || (buffer_size == 0U)) {
        errno = EINVAL;
        return false;
    }

    if (buddy == NULL) {
        (void)_buf_appendf(buffer, buffer_size, &offset, "%s", "Buddy: NULL\n");
        return true;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "%s", "Buddy Statistics:\n")) {
        return false;
    }

    /* Basic capacity / usage numbers */
    size_t const pool_size      = buddy->pool_size;
    size_t const used           = buddy->len;
    size_t const remaining      = (pool_size > used) ? (pool_size - used) : 0U;
    size_t const total_overhead = buddy->total_alloc;

    size_t const min_block_size = (size_t)1u << buddy->min_order;
    size_t const max_block_size = (size_t)1u << buddy->max_order;
    size_t const largest_block  = buddy_largest_block(buddy);

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Pool size: %zu bytes\n", pool_size)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Min block size: %zu bytes\n", min_block_size)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Max block size: %zu bytes\n", max_block_size)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Used: %zu bytes\n", used)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Remaining: %zu bytes\n", remaining)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Total (with overhead): %zu bytes\n", total_overhead)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Largest free block: %zu bytes\n", largest_block)) {
        return false;
    }

    /* Utilization with divide-by-zero guard */
    if (pool_size == 0U) {
        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "%s", "  Utilization: N/A (pool size is 0)\n")) {
            return false;
        }
    } else {
        double const util = (100.0 * (double)used) / (double)pool_size;
        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "  Utilization: %.1f%%\n", util)) {
            return false;
        }
    }

    /* Per-level free list stats */
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "%s", "  Free lists by level:\n")) {
        return false;
    }

    size_t total_free_bytes_from_lists = 0U;

    for (uint32_t level = 0U; level < buddy->num_levels; ++level) {
        uint32_t const order      = buddy->min_order + level;
        size_t   const block_size = (size_t)1u << order;

        size_t count = 0U;
        for (buddy_block_t *blk = buddy->free_lists[level];
             blk != NULL;
             blk = blk->next) {
            count++;
        }

        size_t level_free_bytes = count * block_size;
        total_free_bytes_from_lists += level_free_bytes;

        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "    Level %u (order %u, block %zu bytes): "
                          "%zu blocks, %zu bytes free\n",
                          level, order, block_size,
                          count, level_free_bytes)) {
            return false;
        }
    }

    /* Optional cross-check of free bytes vs remaining */
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Free bytes (sum of free lists): %zu bytes\n",
                      total_free_bytes_from_lists)) {
        return false;
    }

    return true;
}
// -------------------------------------------------------------------------------- 

size_t buddy_alignment(const buddy_t* buddy) {
    if (!buddy) {
        errno = EINVAL;
        return 0;
    }
    return buddy->base_align;
}
// ================================================================================ 
// ================================================================================ 
// SLAB ALLOCATOR 

/* Intrusive free-list node stored in each free slot. */
typedef struct slab_slot {
    struct slab_slot *next;
} slab_slot_t;
// -------------------------------------------------------------------------------- 

/* Per-page header, stored at the beginning of each slab page. */
typedef struct slab_page {
    struct slab_page *next;
    /* You could add free-count, etc., here later if you want. */
} slab_page_t;
// -------------------------------------------------------------------------------- 

typedef struct slab_t {
    buddy_t *buddy;        /* backing buddy allocator */

    size_t   obj_size;     /* user-visible object size */
    size_t   slot_size;    /* internal slot size (>= obj_size and >= sizeof(slab_slot_t)) */
    size_t   align;        /* slot alignment (power of two) */

    size_t   slab_bytes;   /* total bytes in each slab page (buddy allocation size we carve up) */
    size_t   page_hdr_bytes;/* bytes reserved for slab_page header (aligned) */
    size_t   objs_per_slab;/* slots per page */

    size_t   len;          /* bytes currently in-use by this slab (obj_size * live objects) */

    slab_page_t *pages;    /* linked list of pages (headers sit at page base) */
    slab_slot_t *free_list;/* global free-list of free slots */
} slab_t;
// -------------------------------------------------------------------------------- 

/* helper: align x up to a (a must be power of two) */
static inline size_t _slab_align_up(size_t x, size_t a) {
    return (x + (a - 1u)) & ~(a - 1u);
}
// -------------------------------------------------------------------------------- 

static bool _slab_grow(slab_t *slab) {
    if (!slab || !slab->buddy) {
        errno = EINVAL;
        return false;
    }

    /* Allocate one slab page from the buddy allocator. */
    void *mem = alloc_buddy_aligned(slab->buddy,
                                    slab->slab_bytes,
                                    slab->align,
                                    false);
    if (!mem) {
        /* errno set by alloc_buddy_aligned */
        return false;
    }

    uint8_t    *raw  = (uint8_t *)mem;
    slab_page_t *page = (slab_page_t *)raw;

    /* Link page into page list. */
    page->next = slab->pages;
    slab->pages = page;

    /* Slots start after the (aligned) page header. */
    uint8_t *slots_base = raw + slab->page_hdr_bytes;

    /* Carve page into slots and push them on the global free-list. */
    uint8_t *p = slots_base;
    for (size_t i = 0; i < slab->objs_per_slab; ++i) {
        slab_slot_t *slot = (slab_slot_t *)p;
        slot->next = slab->free_list;
        slab->free_list = slot;

        p += slab->slot_size;
    }

    return true;
}
// -------------------------------------------------------------------------------- 

static slab_page_t *_slab_find_page(slab_t *slab, void *ptr) {
    if (!slab || !ptr) return NULL;

    uint8_t *p = (uint8_t *)ptr;

    for (slab_page_t *page = slab->pages; page != NULL; page = page->next) {
        uint8_t *base = (uint8_t *)page;                  /* page starts at header */
        uint8_t *end  = base + slab->slab_bytes;          /* we only use slab_bytes */

        if (p >= base && p < end) {
            return page;
        }
    }
    return NULL;
}
// -------------------------------------------------------------------------------- 

static size_t _slab_snapshot_size(const slab_t *slab) {
    size_t page_count = 0;

    for (const slab_page_t *p = slab->pages; p != NULL; p = p->next) {
        ++page_count;
    }

    /* slab_t struct + all pages */
    return sizeof(*slab) + page_count * slab->slab_bytes;
}
// -------------------------------------------------------------------------------- 

/* Uses _next_pow2(size_t) from your buddy code. */
slab_t *init_slab_allocator(buddy_t *buddy,
                            size_t   obj_size,
                            size_t   align,
                            size_t   slab_bytes_hint) {
    if (!buddy || obj_size == 0u) {
        errno = EINVAL;
        return NULL;
    }

    /* Normalize alignment. */
    if (align == 0u) {
        align = alignof(max_align_t);
    }
    if (align & (align - 1u)) {
        align = _next_pow2(align);
        if (!align) {
            errno = EINVAL;
            return NULL;
        }
    }

    /* Allocate the slab_t itself from the buddy allocator. */
    size_t   slab_struct_bytes = _slab_align_up(sizeof(slab_t), alignof(max_align_t));
    slab_t *slab = (slab_t *)alloc_buddy_aligned(buddy,
                                                 slab_struct_bytes,
                                                 alignof(max_align_t),
                                                 true /* zeroed */);
    if (!slab) {
        /* errno set by alloc_buddy_aligned */
        return NULL;
    }

    slab->buddy = buddy;
    slab->obj_size = obj_size;
    slab->align    = align;

    /* Determine slot size: must hold object and slab_slot_t linkage. */
    size_t slot_size = _slab_align_up(obj_size, align);
    if (slot_size < sizeof(slab_slot_t)) {
        slot_size = _slab_align_up(sizeof(slab_slot_t), align);
    }
    slab->slot_size = slot_size;

    /* Determine page header bytes (aligned). */
    size_t page_hdr_bytes = _slab_align_up(sizeof(slab_page_t), align);
    slab->page_hdr_bytes  = page_hdr_bytes;

    /* Choose slab page size. */
    size_t slab_bytes = slab_bytes_hint;

    if (slab_bytes == 0u) {
        /* Default: at least 4 KiB or enough for 64 objects, plus header. */
        size_t min_slots_bytes = slot_size * 64u;
        size_t min_total       = page_hdr_bytes + min_slots_bytes;
        size_t default_min     = 4096u;

        if (min_total < default_min) {
            min_total = default_min;
        }
        slab_bytes = min_total;
    }

    /* Ensure we can fit at least one slot. */
    if (slab_bytes < page_hdr_bytes + slot_size) {
        slab_bytes = page_hdr_bytes + slot_size;
    }

    /* Make slab_bytes a multiple of slot_size after header, so no tail fragment. */
    size_t usable_for_slots = slab_bytes - page_hdr_bytes;
    size_t objs_per_slab    = usable_for_slots / slot_size;
    if (objs_per_slab == 0u) {
        objs_per_slab       = 1u;
        usable_for_slots    = slot_size;
        slab_bytes          = page_hdr_bytes + usable_for_slots;
    } else {
        usable_for_slots    = objs_per_slab * slot_size;
        slab_bytes          = page_hdr_bytes + usable_for_slots;
    }

    slab->slab_bytes    = slab_bytes;
    slab->objs_per_slab = objs_per_slab;

    slab->len       = 0u;
    slab->pages     = NULL;
    slab->free_list = NULL;

    return slab;
}
// -------------------------------------------------------------------------------- 

void *alloc_slab(slab_t *slab, bool zeroed) {
    if (!slab) {
        errno = EINVAL;
        return NULL;
    }

    /* Grow if no free slots. */
    if (!slab->free_list) {
        if (!_slab_grow(slab)) {
            /* errno set by _slab_grow/buddy allocator */
            return NULL;
        }
    }

    /* Pop from global free list. */
    slab_slot_t *slot = slab->free_list;
    slab->free_list   = slot->next;

    void *user_ptr = (void *)slot;

    if (zeroed) {
        memset(user_ptr, 0, slab->obj_size);
    }

    /* Track payload bytes currently in use. */
    slab->len += slab->obj_size;

    return user_ptr;
}
// -------------------------------------------------------------------------------- 

bool return_slab(slab_t *slab, void *ptr) {
    if (!slab) {
        errno = EINVAL;
        return false;
    }

    if (!ptr) {
        /* Like free(NULL): no-op, success. */
        return true;
    }

    slab_page_t *page = _slab_find_page(slab, ptr);
    if (!page) {
        /* Pointer not from this slab allocator. */
        errno = EINVAL;
        return false;
    }

    uint8_t *page_base   = (uint8_t *)page;  /* header is at base */
    uint8_t *slots_start = page_base + slab->page_hdr_bytes;
    uint8_t *slots_end   = page_base + slab->slab_bytes;

    uint8_t *p = (uint8_t *)ptr;

    /* Must be within the slots region. */
    if (p < slots_start || p >= slots_end) {
        errno = EINVAL;
        return false;
    }

    size_t offset = (size_t)(p - slots_start);

    /* Must be aligned on a slot boundary. */
    if ((offset % slab->slot_size) != 0u) {
        errno = EINVAL;
        return false;
    }

    /* Return to free list. */
    slab_slot_t *slot = (slab_slot_t *)ptr;
    slot->next        = slab->free_list;
    slab->free_list   = slot;

    if (slab->len >= slab->obj_size) {
        slab->len -= slab->obj_size;
    } else {
        slab->len = 0u; /* defensive */
    }

    return true;
}
// -------------------------------------------------------------------------------- 

inline size_t slab_alloc(const slab_t *slab) {
    if (!slab) {
        errno = EINVAL;
        return 0;
    }
    return slab->len;   /* logical bytes in use */
}
// -------------------------------------------------------------------------------- 

inline size_t slab_size(const slab_t *slab) {
    if (!slab) {
        errno = EINVAL;
        return 0;
    }

    size_t total = 0;
    for (slab_page_t *p = slab->pages; p != NULL; p = p->next) {
        total += slab->slab_bytes;
    }

    return total;
}
// -------------------------------------------------------------------------------- 

inline size_t total_slab_alloc(const slab_t *slab) {
    if (!slab) {
        errno = EINVAL;
        return 0;
    }

    /* Start with the slab_t struct itself */
    size_t total = _slab_align_up(sizeof(slab_t), alignof(max_align_t));

    /* Add each page */
    for (slab_page_t *p = slab->pages; p != NULL; p = p->next) {
        total += slab->slab_bytes;
    }

    return total;
}
// -------------------------------------------------------------------------------- 

inline size_t slab_stride(const slab_t *slab) {
    if (!slab) {
        errno = EINVAL;
        return 0;
    }

    /* Internal slot stride, may be >= obj_size to hold free-list linkage. */
    return slab->slot_size;
}
// --------------------------------------------------------------------------------

inline size_t slab_total_blocks(const slab_t *slab) {
    if (!slab) {
        errno = EINVAL;
        return 0;
    }

    /* Count pages and multiply by objs_per_slab: total capacity in slots. */
    size_t page_count = 0;
    for (const slab_page_t *p = slab->pages; p != NULL; p = p->next) {
        ++page_count;
    }

    return page_count * slab->objs_per_slab;
}
// -------------------------------------------------------------------------------- 

inline size_t slab_free_blocks(const slab_t *slab) {
    if (!slab) {
        errno = EINVAL;
        return 0;
    }

    size_t count = 0;
    const slab_slot_t *slot = slab->free_list;

    while (slot) {
        ++count;
        slot = slot->next;
    }

    return count;
}
// -------------------------------------------------------------------------------- 

inline size_t slab_alignment(const slab_t *slab) {
    if (!slab) {
        errno = EINVAL;
        return 0;
    }

    return slab->align;
}
// -------------------------------------------------------------------------------- 

inline size_t slab_in_use_blocks(const slab_t *slab) {
    if (!slab) {
        errno = EINVAL;
        return 0;
    }

    if (slab->obj_size == 0) {
        errno = EINVAL;
        return 0;
    }

    return slab->len / slab->obj_size;
}
// -------------------------------------------------------------------------------- 

bool is_slab_ptr(const slab_t *slab, const void *ptr) {
    if (!slab || !ptr) {
        errno = EINVAL;
        return false;
    }

    uint8_t *p = (uint8_t *)ptr;

    /* Step 1: find which page this pointer belongs to */
    for (slab_page_t *page = slab->pages; page != NULL; page = page->next) {

        uint8_t *page_base   = (uint8_t *)page;            /* header at base */
        uint8_t *slots_start = page_base + slab->page_hdr_bytes;
        uint8_t *page_end    = page_base + slab->slab_bytes;

        /* Step 2: check if it lies in the page at all */
        if (p < page_base || p >= page_end) {
            continue;
        }

        /* Step 3: reject pointers inside the header region */
        if (p < slots_start) {
            errno = EINVAL;
            return false;
        }

        /* Step 4: must be aligned to slot boundaries */
        size_t offset = (size_t)(p - slots_start);

        if ((offset % slab->slot_size) != 0u) {
            errno = EINVAL;
            return false;
        }

        /* Pointer is valid */
        return true;
    }

    /* Pointer not found in any page */
    errno = EINVAL;
    return false;
}
// -------------------------------------------------------------------------------- 

bool reset_slab(slab_t *slab) {
    if (!slab) {
        errno = EINVAL;
        return false;
    }

    /* Basic sanity checks on geometry */
    if (slab->obj_size == 0u ||
        slab->slot_size == 0u ||
        slab->slab_bytes < slab->page_hdr_bytes + slab->slot_size) {
        errno = EINVAL;
        return false;
    }

    /* After reset, nothing is in use and free_list is rebuilt. */
    slab->len       = 0u;
    slab->free_list = NULL;

    /* Walk all pages and re-carve them into free slots. */
    for (slab_page_t *page = slab->pages; page != NULL; page = page->next) {
        uint8_t *page_base   = (uint8_t *)page;              /* header at base */
        uint8_t *slots_start = page_base + slab->page_hdr_bytes;
        uint8_t *slots_end   = page_base + slab->slab_bytes;

        /* Carve the slots region into slot_size chunks. */
        for (uint8_t *p = slots_start;
             p + slab->slot_size <= slots_end;
             p += slab->slot_size) {

            slab_slot_t *slot = (slab_slot_t *)p;
            slot->next        = slab->free_list;
            slab->free_list   = slot;
        }
    }

    return true;
}
// -------------------------------------------------------------------------------- 

bool save_slab(const slab_t *slab,
               void *buffer,
               size_t buffer_size,
               size_t *bytes_needed) {
    if (!slab || !bytes_needed) {
        errno = EINVAL;
        return false;
    }

    size_t needed = _slab_snapshot_size(slab);
    *bytes_needed = needed;

    if (!buffer || buffer_size < needed) {
        errno = ERANGE;
        return false;
    }

    uint8_t *dst = (uint8_t *)buffer;

    /* 1) Copy the slab_t struct itself */
    memcpy(dst, slab, sizeof(*slab));
    dst += sizeof(*slab);

    /* 2) Copy each page in list order */
    for (const slab_page_t *page = slab->pages;
         page != NULL;
         page = page->next) {

        memcpy(dst, (const void *)page, slab->slab_bytes);
        dst += slab->slab_bytes;
    }

    return true;
}
// -------------------------------------------------------------------------------- 

bool restore_slab(slab_t *slab,
                  const void *buffer,
                  size_t buffer_size) {
    if (!slab || !buffer) {
        errno = EINVAL;
        return false;
    }

    const uint8_t *src = (const uint8_t *)buffer;

    /* 1) Peek at the serialized slab_t */
    slab_t snap_header;
    if (buffer_size < sizeof(snap_header)) {
        errno = ERANGE;
        return false;
    }

    memcpy(&snap_header, src, sizeof(snap_header));
    src += sizeof(snap_header);

    /* 2) Count pages from the snapshot's perspective */
    size_t page_count = 0;
    const slab_page_t *snap_page = snap_header.pages;

    while (snap_page != NULL) {
        ++page_count;
        snap_page = snap_page->next;
    }

    size_t needed = sizeof(snap_header) + page_count * snap_header.slab_bytes;
    if (buffer_size < needed) {
        errno = ERANGE;
        return false;
    }

    /* 3) Sanity-check geometry vs the current slab */
    if (snap_header.obj_size      != slab->obj_size      ||
        snap_header.slot_size     != slab->slot_size     ||
        snap_header.align         != slab->align         ||
        snap_header.slab_bytes    != slab->slab_bytes    ||
        snap_header.page_hdr_bytes!= slab->page_hdr_bytes||
        snap_header.objs_per_slab != slab->objs_per_slab) {

        errno = EINVAL;
        return false;
    }

    /* 4) Restore pages using snapshot's page list.
       NOTE: the pointers in snap_header.pages are the addresses
       of the real page headers in memory at the time of save_slab.
       We assume the slab has not grown/shrunk pages since then, so
       those addresses are still valid.
    */
    snap_page = snap_header.pages;

    for (size_t i = 0; i < page_count && snap_page != NULL; ++i) {
        memcpy((void *)snap_page, src, snap_header.slab_bytes);
        src += snap_header.slab_bytes;

        /* Important: use the *snapshot's* next pointer from the buffer,
           not the live one, which we just overwrote.
           We already loaded the snapshot header, so snap_page->next
           is still the next pointer from the snapshot's perspective.
         */
        snap_page = snap_page->next;
    }

    /* 5) Finally restore the slab_t itself */
    memcpy(slab, &snap_header, sizeof(*slab));

    return true;
}
// -------------------------------------------------------------------------------- 

bool slab_stats(const slab_t *slab, char *buffer, size_t buffer_size) {
    size_t offset = 0U;

    if ((buffer == NULL) || (buffer_size == 0U)) {
        errno = EINVAL;
        return false;
    }

    if (slab == NULL) {
        (void)_buf_appendf(buffer, buffer_size, &offset, "%s", "Slab: NULL\n");
        return true;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "%s", "Slab Statistics:\n")) {
        return false;
    }

    /* Count pages */
    size_t page_count = 0U;
    for (const slab_page_t *p = slab->pages; p != NULL; p = p->next) {
        ++page_count;
    }

    /* Capacity / usage in bytes */
    size_t const capacity_bytes = page_count * slab->slab_bytes;
    size_t const used_bytes     = slab->len;
    size_t const remaining      =
        (capacity_bytes > used_bytes) ? (capacity_bytes - used_bytes) : 0U;

    /* Total footprint including slab_t struct itself */
    size_t const slab_struct_bytes =
        _slab_align_up(sizeof(slab_t), alignof(max_align_t));
    size_t const total_overhead = slab_struct_bytes + capacity_bytes;

    /* Blocks info */
    size_t const total_blocks   = page_count * slab->objs_per_slab;
    size_t const in_use_blocks  =
        (slab->obj_size != 0U) ? (slab->len / slab->obj_size) : 0U;

    size_t const free_blocks_geom =
        (total_blocks > in_use_blocks) ? (total_blocks - in_use_blocks) : 0U;

    /* Free blocks counted from the free list (cross-check) */
    size_t free_blocks_list = 0U;
    for (const slab_slot_t *slot = slab->free_list;
         slot != NULL;
         slot = slot->next) {
        ++free_blocks_list;
    }

    /* Basic geometry info */
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Object size: %zu bytes\n", slab->obj_size)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Slot stride: %zu bytes\n", slab->slot_size)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Alignment: %zu bytes\n", slab->align)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Page size: %zu bytes\n", slab->slab_bytes)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Page header bytes: %zu\n", slab->page_hdr_bytes)) {
        return false;
    }

    /* Capacity / usage summary */
    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Pages: %zu\n", page_count)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Blocks per page: %zu\n", slab->objs_per_slab)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Total blocks: %zu\n", total_blocks)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  In-use blocks: %zu\n", in_use_blocks)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Free blocks (geom): %zu\n", free_blocks_geom)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Free blocks (free list): %zu\n", free_blocks_list)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Used: %zu bytes\n", used_bytes)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Capacity: %zu bytes\n", capacity_bytes)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Remaining: %zu bytes\n", remaining)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Total (with overhead): %zu bytes\n", total_overhead)) {
        return false;
    }

    /* Utilization with divide-by-zero guard */
    if (capacity_bytes == 0U) {
        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "%s", "  Utilization: N/A (capacity is 0)\n")) {
            return false;
        }
    } else {
        double const util = (100.0 * (double)used_bytes) / (double)capacity_bytes;
        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "  Utilization: %.1f%%\n", util)) {
            return false;
        }
    }

    /* Per-page listing, similar to arena chunk listing */
    {
        size_t page_index = 0U;
        const slab_page_t *current = slab->pages;

        while (current != NULL) {
            ++page_index;

            if (!_buf_appendf(buffer, buffer_size, &offset,
                              "  Page %zu: %zu bytes, %zu blocks\n",
                              page_index,
                              slab->slab_bytes,
                              slab->objs_per_slab)) {
                return false;
            }

            current = current->next;
        }
    }

    return true;
}
// ================================================================================
// ================================================================================
// eof
