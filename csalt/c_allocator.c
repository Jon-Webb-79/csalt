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
    uint8_t _pad[6];   // keep 8 byte passing
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
        errno = EINVAL;
        return false;
    }

    size_t const offset = *p_offset;
    if (offset > buffer_size) {      /* defensive: offset should never exceed size */
        errno = ERANGE;
        return false;
    }

    size_t const remaining = buffer_size - offset;
    if (remaining == 0U) {
        errno = ERANGE;
        return false;
    }

    va_list args;
    va_start(args, fmt);
    int const n = vsnprintf(&buffer[offset], remaining, fmt, args);
    va_end(args);

    if (n < 0) {                     /* encoding/format failure */
        errno = EINVAL;
        return false;
    }

    /* If n >= remaining, output would be truncated */
    if ((size_t)n >= remaining) {
        errno = ERANGE;
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

arena_t* init_dynamic_arena(size_t bytes, bool resize, size_t min_chunk_in, size_t base_align_in) {
#if ARENA_ENABLE_DYNAMIC
    // Normalize min_chunk (0 allowed)
    size_t min_chunk = min_chunk_in;
    if (min_chunk && !_is_pow2(min_chunk)) {
        min_chunk = _next_pow2(min_chunk);
        if (!min_chunk) { errno = EINVAL; return NULL; }
    }

    // Normalize base alignment; enforce ABI floor
    size_t base_align = base_align_in ? base_align_in : alignof(max_align_t);
    if (!_is_pow2(base_align)) {
        base_align = _next_pow2(base_align);
        if (!base_align) { errno = EINVAL; return NULL; }
    }
    if (base_align < alignof(max_align_t)) base_align = alignof(max_align_t);

    // Initial total
    size_t total = bytes;
    if (min_chunk && total < min_chunk) total = min_chunk;
    if (total < (sizeof(arena_t) + sizeof(Chunk))) { errno = EINVAL; return NULL; }

    void* base = malloc(total);
    if (!base) { errno = ENOMEM; return NULL; }
    uintptr_t const b = (uintptr_t)base;

    uintptr_t p_arena   = b;
    uintptr_t arena_end = p_arena + sizeof(arena_t);
    if (arena_end < p_arena) { free(base); errno = ENOMEM; return NULL; }

    uintptr_t p_chunk   = _align_up_uintptr(arena_end, alignof(Chunk));
    uintptr_t chunk_end = p_chunk + sizeof(Chunk);
    if (chunk_end < p_chunk || chunk_end > b + total) { free(base); errno = ENOMEM; return NULL; }

    // Data starts aligned to *base_align* (not default_arena_alignment)
    uintptr_t p_data    = _align_up_uintptr(chunk_end, base_align);
    if (p_data > b + total) { free(base); errno = EINVAL; return NULL; }

    size_t usable = (size_t)((b + total) - p_data);
    if (!usable) { free(base); errno = EINVAL; return NULL; }

    arena_t* a = (arena_t*)p_arena;
    Chunk* h = (Chunk*)p_chunk;
    h->chunk = (uint8_t*)p_data; h->len = 0; h->alloc = usable; h->next = NULL;

    a->head = a->tail = h;
    a->cur  = (uint8_t*)p_data;
    a->len = 0; a->alloc = usable; a->tot_alloc = total;
    a->min_chunk = min_chunk;
    a->alignment = base_align;

    a->mem_type = (uint8_t)DYNAMIC;          /* was alloc_t; now a byte */
    a->resize   = (uint8_t)(resize ? 1 : 0);

    return a;
#else 
    errno = ENOTSUP;
    return NULL;
#endif
}
// -------------------------------------------------------------------------------- 

arena_t* init_static_arena(void* buffer, size_t bytes, size_t alignment_in) {
    if (!buffer) { errno = EINVAL; return NULL; }

    /* normalize per-arena base alignment (>= max_align_t) */
    size_t base_align = alignment_in ? alignment_in : alignof(max_align_t);
    if (!_is_pow2(base_align)) {
        base_align = _next_pow2(base_align);
        if (!base_align) { errno = EINVAL; return NULL; }
    }
    if (base_align < alignof(max_align_t)) base_align = alignof(max_align_t);

    uintptr_t const b      = (uintptr_t)buffer;
    uintptr_t const b_end  = b + bytes;

    /* align Arena header itself */
    uintptr_t p_arena = _align_up_uintptr(b, alignof(arena_t));
    if (p_arena > b_end) { errno = EINVAL; return NULL; }

    /* must have room for Arena + Chunk at minimum */
    if ((b_end - p_arena) < (sizeof(arena_t) + sizeof(Chunk))) { errno = EINVAL; return NULL; }

    uintptr_t arena_end = p_arena + sizeof(arena_t);
    if (arena_end < p_arena || arena_end > b_end) { errno = EINVAL; return NULL; }

    /* place first Chunk header aligned for Chunk */
    uintptr_t p_chunk  = _align_up_uintptr(arena_end, alignof(Chunk));
    uintptr_t chunk_end = p_chunk + sizeof(Chunk);
    if (chunk_end < p_chunk || chunk_end > b_end) { errno = EINVAL; return NULL; }

    /* place data aligned to per-arena base alignment */
    uintptr_t p_data = _align_up_uintptr(chunk_end, base_align);
    if (p_data > b_end) { errno = EINVAL; return NULL; }

    size_t usable = (size_t)(b_end - p_data);
    if (!usable) { errno = EINVAL; return NULL; }

    /* stitch in-place */
    arena_t* a = (arena_t*)p_arena;
    Chunk* h = (Chunk*)p_chunk;

    h->chunk = (uint8_t*)p_data;
    h->len   = 0;
    h->alloc = usable;
    h->next  = NULL;

    a->head      = h;
    a->tail      = h;
    a->cur       = (uint8_t*)p_data;
    a->len       = 0;
    a->alloc     = usable;
    a->tot_alloc = (size_t)(b_end - b);   /* full caller buffer footprint */
    a->min_chunk = 0;                     /* not used for STATIC */
    a->alignment = base_align;            /* per-arena base alignment */

    a->mem_type  = (uint8_t)STATIC;       /* <-- keep STATIC */
    a->resize    = (uint8_t)0;            /* <-- cannot grow */

    return a;
}
// -------------------------------------------------------------------------------- 

#if ARENA_ENABLE_DYNAMIC
arena_t* init_darena(size_t bytes, bool resize) {
    return init_dynamic_arena(bytes, resize, 4096u, alignof(max_align_t));
}
#endif
// --------------------------------------------------------------------------------

arena_t* init_sarena(void* buffer, size_t bytes) {
    return init_static_arena(buffer, bytes, alignof(max_align_t));
}
// -------------------------------------------------------------------------------- 

void free_arena(arena_t* arena) {
    if (arena == NULL) { errno = EINVAL; return; }

    if (arena->mem_type == STATIC) {
        errno = EPERM;
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

inline void* alloc_arena(arena_t* arena, size_t bytes, bool zeroed) {
    if (!arena || bytes == 0u) { errno = EINVAL; return NULL; }

    // Use per-arena base alignment (power-of-two)
    size_t const a = arena->alignment;
    if (a == 0u || (a & (a - 1u)) != 0u) { errno = EINVAL; return NULL; }

    Chunk* tail = arena->tail;
    if (!tail) { errno = EINVAL; return NULL; }

    // Per-allocation pad to 'a'
    uintptr_t const cur  = (uintptr_t)arena->cur;
    size_t    const pad  = _pad_up(cur, a);
    size_t    const need = pad + bytes;

    // Fast path: fits in current tail
    size_t const avail = (tail->alloc >= tail->len) ? (tail->alloc - tail->len) : 0u;
    if (avail >= need) {
        uint8_t* p = (uint8_t*)(cur + pad);  // a-aligned
        arena->cur  = p + bytes;
        tail->len  += need;                  // charge pad + bytes
        arena->len += need;
        if (zeroed) memset(p, 0, bytes);
        return p;
    }
#if ARENA_ENABLE_DYNAMIC
    // No space -> grow only when allowed
    if (arena->mem_type == STATIC || !arena->resize) { errno = EPERM; return NULL; }

    size_t const grow_data = _next_chunk_size(tail->alloc, need, a, arena->min_chunk);
    Chunk* nc = _chunk_new_ex(grow_data, a);
    if (!nc) return NULL; // errno set by helper

    // Link
    tail->next  = nc;
    arena->tail = nc;

    // Accounting (approximate footprint: header rounded to 'a' + data bytes)
    arena->alloc     += nc->alloc;                          // usable capacity added
    arena->tot_alloc += _align_up_size(sizeof(Chunk), a) + nc->alloc;

    // First allocation from fresh chunk: base is a-aligned -> no pad
    void* p = (void*)nc->chunk;
    arena->cur  = nc->chunk + bytes;
    nc->len     = bytes;
    arena->len += bytes;

    if (zeroed) memset(p, 0, bytes);
    return p;
#else 
    errno = EPERM;
    return NULL;
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
    void* ptr = alloc_arena(arena, realloc_size, false);
    if (!ptr) {
        // errno set by arena_alloc
        return NULL;
    }

    memcpy(ptr, variable, var_size);

    if (zeroed) {
        memset((uint8_t*)ptr + var_size, 0, realloc_size - var_size);
    }

    return ptr;
}
// -------------------------------------------------------------------------------- 

void* alloc_arena_aligned(arena_t* arena, size_t bytes, size_t alignment, bool zeroed) {
    if (!arena || bytes == 0u) { errno = EINVAL; return NULL; }

    // Choose effective alignment: at least the arena base alignment.
    size_t a_req = (alignment ? alignment : arena->alignment);
    // Must be power-of-two; clamp to arena base if smaller.
    if (a_req == 0u || (a_req & (a_req - 1u)) != 0u) { errno = EINVAL; return NULL; }
    size_t const a = (a_req < arena->alignment) ? arena->alignment : a_req;

    Chunk* tail = arena->tail;
    if (!tail) { errno = EINVAL; return NULL; }

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
    if (arena->mem_type == STATIC || !arena->resize) { errno = EPERM; return NULL; }

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
    errno = EPERM;
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
        errno = EINVAL;
        return NULL;
    }

    // No-op on shrink or same size; keeps arena space.
    if (realloc_size <= var_size) {
        return variable;
    }

    // Allocate new block (no zeroing; we’ll zero only the tail if requested).
    void* ptr = alloc_arena_aligned(arena, realloc_size, algined, false);
    if (!ptr) {
        // errno set by arena_alloc
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

void reset_arena(arena_t *arena, bool trim_extra_chunks) {
    if (!arena) {
        errno = EINVAL;
        return;
    }

    if (!arena->head) {
        arena->cur  = NULL;
        arena->len  = 0u;
        arena->tail = NULL;
        return;
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
}
// -------------------------------------------------------------------------------- 

ArenaCheckPoint save_arena(const arena_t* arena) {
    ArenaCheckPoint pub = {0};
    if (!arena) {
        errno = EINVAL;
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
        errno = EINVAL; 
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
        errno = EINVAL; 
        return false; 
    }

    // Validate the chunk has a valid data region
    if (!rep.chunk->chunk) { 
        errno = EINVAL; 
        return false; 
    }

    // Validate the checkpoint cursor is within bounds
    uintptr_t chunk_start = (uintptr_t)rep.chunk->chunk;
    uintptr_t cursor_pos  = (uintptr_t)rep.cur;
    uintptr_t chunk_end   = chunk_start + rep.chunk->alloc;  // exclusive
    
    // Check for overflow and valid cursor position
    if (chunk_end < chunk_start || cursor_pos < chunk_start || cursor_pos > chunk_end) { 
        errno = EINVAL; 
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
        errno = EINVAL; 
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
        errno = EINVAL;
        return 0u;
    }
    if (!arena || !arena->tail || !arena->tail->chunk) return 0u;
    size_t used = arena->tail->len;
    if (used > arena->tail->alloc) used = arena->tail->alloc;  // clamp
    return arena->tail->alloc - used;
}
// -------------------------------------------------------------------------------- 

size_t arena_chunk_count(const arena_t* arena) {
    if (!arena) {
        errno = EINVAL;
        return 0u;
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
        errno = EINVAL;
        return ALLOC_INVALID;
    }

    return (alloc_t)arena->mem_type;
}
// -------------------------------------------------------------------------------- 

inline size_t arena_size(const arena_t* arena) {
    if (!arena) {
        errno = EINVAL;
        return 0;
    }
    return arena->len;
}
// -------------------------------------------------------------------------------- 

inline size_t arena_alloc(const arena_t* arena) {
    if (!arena) {
        errno = EINVAL;
        return 0;
    }
    return arena->alloc;
}
// -------------------------------------------------------------------------------- 

inline size_t total_arena_alloc(const arena_t* arena) {
    if (!arena) {
        errno = EINVAL;
        return 0;
    }
    return arena->tot_alloc;
}
// -------------------------------------------------------------------------------- 

inline size_t arena_alignment(const arena_t* arena) {
    if (!arena || !arena->head) {
        errno = EINVAL;
        return 0;
    }
    return arena->alignment;
}
// -------------------------------------------------------------------------------- 

inline size_t arena_min_chunk_size(const arena_t* arena) {
    if (!arena || !arena->head) {
        errno = EINVAL;
        return 0;
    }
    return arena->min_chunk;
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
    if (arena->mem_type == STATIC) {
        errno = EPERM;
        return;
    }
    arena->resize = (uint8_t)(toggle ? 1 : 0);
    return;
#else 
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
        errno = EINVAL;
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
    arena_t* arena = init_dynamic_arena(/*bytes=*/arena_seed_bytes,
                                        /*resize=*/grow_enabled,
                                        /*min_chunk_in=*/min_chunk_bytes,
                                        /*base_align_in=*/arena_base_align);
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
    arena_t* arena = init_static_arena(buffer, buffer_bytes, arena_base_align);
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

inline void* alloc_pool(pool_t* pool) {
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
// ================================================================================ 
// ================================================================================ 
// INTERNAL ARENA 

struct iarena_t {
    // Hot path
    uint8_t* cur;      // next write
    uint8_t* end;      // bump limit (exclusive)
    uint8_t* begin;    // base for stats/reset

    // Parent ref
    arena_t* arena;    // Pointer to an arena not owned by this truct

    // Accounting
    size_t len;        // charged pad+bytes total
    size_t alloc;      // usable payload capacity (end - begin)
    size_t tot_alloc;  // total reserved from parent (header + padding + payload)

    size_t alignment;  // effective payload alignment for this subarena
};
// -------------------------------------------------------------------------------- 

iarena_t* init_iarena_with_arena(arena_t* parent,
                                 size_t bytes,
                                 size_t alignment) {
    if (!parent || bytes == 0u) { errno = EINVAL; return NULL; }

    const size_t parent_align = parent->alignment;
    if ((parent_align & (parent_align - 1u)) != 0u) { errno = EINVAL; return NULL; }
    if (alignment && ((alignment & (alignment - 1u)) != 0u)) { errno = EINVAL; return NULL; }

    // Effective payload & base (header) alignment
    const size_t eff_payload_align = alignment ? (alignment > parent_align ? alignment : parent_align)
                                               : parent_align;
    const size_t hdr_align  = alignof(iarena_t);
    const size_t base_align = (eff_payload_align > hdr_align) ? eff_payload_align : hdr_align;

    // --- Preflight like init_dynamic_arena does before malloc ---
    // Worst-case pad after header to reach payload alignment
    const size_t worst_pad = eff_payload_align - 1u;
    // Need at least header + worst_pad + 1 byte of payload
    if (bytes <= sizeof(iarena_t) + worst_pad) {
        errno = EINVAL;          // not enough total for any payload
        return NULL;
    }
    // ------------------------------------------------------------

    // Carve total slice from parent; base is aligned to base_align
    uint8_t* base = (uint8_t*)alloc_arena_aligned(parent, bytes, base_align, /*zero=*/false);
    if (!base) { errno = ENOMEM; return NULL; }

    iarena_t* ia = (iarena_t*)base;

    uint8_t* raw_after_hdr = base + sizeof(iarena_t);
    uint8_t* begin = (uint8_t*)_align_up_uintptr((uintptr_t)raw_after_hdr, eff_payload_align);
    uint8_t* end   = base + bytes;

    if (begin >= end) { errno = ENOMEM; return NULL; }

    ia->arena     = parent;
    ia->begin     = begin;
    ia->cur       = begin;
    ia->end       = end;
    ia->len       = 0;
    ia->alloc     = (size_t)(end - begin); // usable payload capacity
    ia->tot_alloc = bytes;                 // total reserved (header + padding + payload)
    ia->alignment = eff_payload_align;
    return ia;
}
// -------------------------------------------------------------------------------- 

inline void* alloc_iarena(iarena_t* ia, size_t bytes, bool zeroed) {
    if (!ia || bytes == 0u) { errno = EINVAL; return NULL; }

    const size_t A = ia->alignment;
    if (A == 0u || (A & (A - 1u)) != 0u) { errno = EINVAL; return NULL; }

    uintptr_t cur  = (uintptr_t)ia->cur;
    uintptr_t p    = _align_up_uintptr(cur, A);
    uintptr_t next = p + (uintptr_t)bytes;

    if (next > (uintptr_t)ia->end) {
        errno = ENOMEM; // fixed-size slice; no growth
        return NULL;
    }

    ia->cur  = (uint8_t*)next;
    ia->len += (size_t)(next - cur); // charge pad + bytes

    void* out = (void*)p;
    if (zeroed) memset(out, 0, bytes);
    return out;
}
// -------------------------------------------------------------------------------- 

inline void* alloc_iarena_aligned(iarena_t* ia,
                                  size_t bytes,
                                  size_t align,
                                  bool zeroed) {
    if (!ia || bytes == 0u) {
        errno = EINVAL;
        return NULL;
    }

    // Validate alignment argument
    const size_t base_align = ia->alignment;
    if (base_align == 0u || (base_align & (base_align - 1u)) != 0u) {
        errno = EINVAL;
        return NULL;
    }

    // Normalize requested alignment
    if (align == 0u) {
        align = base_align;
    } else if (align & (align - 1u)) { // must be power of two
        errno = EINVAL;
        return NULL;
    }

    // Effective alignment = max(requested, subarena base)
    const size_t A = (align > base_align) ? align : base_align;

    uintptr_t cur  = (uintptr_t)ia->cur;
    uintptr_t p    = _align_up_uintptr(cur, A);
    uintptr_t next = p + (uintptr_t)bytes;

    // Capacity check
    if (next > (uintptr_t)ia->end) {
        errno = ENOMEM;
        return NULL;
    }

    ia->cur  = (uint8_t*)next;
    ia->len += (size_t)(next - cur); // track pad + payload

    void* out = (void*)p;
    if (zeroed) memset(out, 0, bytes);
    return out;
}
// -------------------------------------------------------------------------------- 

void* realloc_iarena(iarena_t* ia,
                     void* var,
                     size_t old_size,
                     size_t new_size,
                     bool zeroed) {
    if (!ia) {
        errno = EINVAL;
        return NULL;
    }

    if (!var) {
        return alloc_iarena(ia, new_size, zeroed);
    }

    if (!is_iarena_ptr_sized(ia, var, old_size)) {
        errno = EPERM;      /* or EINVAL if you prefer */
        return NULL;
    }

    void* new_ptr = alloc_iarena(ia, new_size, zeroed);
    if (!new_ptr) {
        /* alloc_iarena already set errno (likely ENOMEM or EPERM) */
        return NULL;
    }

    size_t copy_size = (old_size < new_size) ? old_size : new_size;
    memcpy(new_ptr, var, copy_size);

    return new_ptr;  /* caller updates their variable */
}
// -------------------------------------------------------------------------------- 

void* ralloc_iarena_aligned(iarena_t* ia,
                            void* var,
                            size_t old_size,
                            size_t new_size,
                            bool zeroed,
                            size_t alignment) {
    if (!ia) {
        errno = EINVAL;
        return NULL;
    }

    if (!var) {
        return alloc_iarena_aligned(ia, new_size, alignment, zeroed);
    }

    if (!is_iarena_ptr_sized(ia, var, old_size)) {
        errno = EPERM;  /* does not belong to this iarena */
        return NULL;
    }

    size_t base_align = ia->alignment;
    size_t A = (alignment == 0 ? base_align :
                (alignment > base_align ? alignment : base_align));

    if (A == 0 || (A & (A - 1u))) {
        errno = EINVAL;
        return NULL;
    }

    void* new_ptr = alloc_iarena_aligned(ia, new_size, A, zeroed);
    if (!new_ptr) {
        return NULL;
    }

    /* Copy only the overlapping portion */
    size_t copy_size = (old_size < new_size) ? old_size : new_size;
    memcpy(new_ptr, var, copy_size);

    return new_ptr; 
}
// -------------------------------------------------------------------------------- 

bool is_iarena_ptr(const iarena_t* ia, const void *ptr) {
    if (!ia || !ptr) {
        errno = EINVAL;
        return false;
    }

    const uint8_t* p = (const uint8_t*)ptr;
    return (p >= ia->begin && p < ia->end);
}
// -------------------------------------------------------------------------------- 

bool is_iarena_ptr_sized(const iarena_t* ia, const void* ptr, size_t size) {
    if (!ia || !ptr) {
        errno = EINVAL;
        return false;
    }

    const uintptr_t p = (uintptr_t)ptr;
    const uintptr_t b = (uintptr_t)ia->begin;
    const uintptr_t e = (uintptr_t)ia->end;

    // overflow-safe check: p + size must not wrap around
    if (size > (uintptr_t)(-1) - p) return false;  // overflow => false

    const uintptr_t p_end = p + size;
    return (p >= b && p_end <= e);
}
// -------------------------------------------------------------------------------- 

inline size_t iarena_remaining(const iarena_t* ia) {
    if (!ia) { errno = EINVAL; return 0; }
    if (ia->cur > ia->end) { errno = EFAULT; return 0; }  // invariant guard
    return (size_t)(ia->end - ia->cur);
}
// -------------------------------------------------------------------------------- 

void reset_iarena(iarena_t* ia) {
    if (!ia) {
        errno = EINVAL;
        return;
    }
    ia->cur = ia->begin;
    ia->len = 0;
}
// -------------------------------------------------------------------------------- 

#define IARENA_CP_MAGIC ((uint32_t)0x49415243u)

iArenaCheckPoint save_iarena(const iarena_t* ia) {
    iArenaCheckPoint cp = {0};  /* zero-init → invalid by default */
    if (!ia) { errno = EINVAL; return cp; }

    /* In your allocator, len is charged as (next - cur) each bump, so len == (cur - begin). */
    const size_t used     = (size_t)(ia->cur - ia->begin);
    const size_t capacity = (size_t)(ia->end - ia->begin);

    cp.owner     = (const void*)ia;
    cp.used      = used;
    cp.capacity  = capacity;       /* sticky for validation */
    cp.alignment = ia->alignment;  /* sticky for validation */
    cp.magic     = IARENA_CP_MAGIC;
    return cp;
}
// -------------------------------------------------------------------------------- 

bool restore_iarena(iarena_t *ia, iArenaCheckPoint cp) {
    if (!ia) { errno = EINVAL; return false; }

    /* Basic checkpoint sanity */
    if (cp.magic != IARENA_CP_MAGIC) { errno = EINVAL; return false; }
    if (cp.owner != (const void*)ia) { errno = EINVAL; return false; }

    /* Current capacity/alignment must match the saved view (fixed-slice design) */
    const size_t cur_capacity = (size_t)(ia->end - ia->begin);
    if (cp.capacity != cur_capacity) { errno = EINVAL; return false; }
    if (cp.alignment != ia->alignment) { errno = EINVAL; return false; }

    /* Bounds check: used ≤ capacity */
    if (cp.used > cur_capacity) { errno = EINVAL; return false; }

    /* Restore cursor and accounting */
    ia->cur = ia->begin + cp.used;
    ia->len = cp.used;  /* equals cur - begin given your charging policy */

    return true;
}
// -------------------------------------------------------------------------------- 

inline alloc_t iarena_mtype(const iarena_t* ia) {
    if (!ia || !ia->arena) {
        errno = EINVAL;
        return ALLOC_INVALID;
    }
    return arena_mtype(ia->arena);
}
// -------------------------------------------------------------------------------- 

inline size_t iarena_size(const iarena_t* ia) {
    if (!ia) {
        errno = EINVAL;
        return 0;
    }
    return ia->len;
}
// -------------------------------------------------------------------------------- 

inline size_t iarena_alloc(const iarena_t* ia) {
    if (!ia) {
        errno = EINVAL;
        return 0;
    }
    return ia->alloc;
}
// -------------------------------------------------------------------------------- 

inline size_t total_iarena_alloc(const iarena_t* ia) {
    if (!ia) {
        errno = EINVAL;
        return 0;
    }
    return ia->tot_alloc;
}
// -------------------------------------------------------------------------------- 

inline size_t iarena_alignment(const iarena_t* ia) {
    if (!ia) {
        errno = EINVAL;
        return 0;
    }
    return ia->alignment;
}
// -------------------------------------------------------------------------------- 

bool iarena_stats(const iarena_t *ia, char *buffer, size_t buffer_size) {
    size_t offset = 0u;

    if (!buffer || buffer_size == 0u) {
        errno = EINVAL;
        return false;
    }

    if (!ia) {
        (void)_buf_appendf(buffer, buffer_size, &offset, "%s", "iArena: NULL\n");
        return true;
    }

    /* Derive quantities (begin/end/cur are uint8_t*) */
    const size_t used      = (size_t)(ia->cur   - ia->begin);
    const size_t capacity  = (size_t)(ia->end   - ia->begin);  /* should match ia->alloc */
    const size_t remaining = (size_t)(ia->end   - ia->cur);
    const size_t alignment = ia->alignment;
    const char*  parent    = ia->arena ? "Yes" : "No";

    if (!_buf_appendf(buffer, buffer_size, &offset, "%s", "iArena Statistics:\n")) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset, "  Parent: %s\n", parent)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset, "  Used: %zu bytes\n", used)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset, "  Capacity: %zu bytes\n", capacity)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset,
                      "  Total (with overhead): %zu bytes\n", ia->tot_alloc)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset, "  Remaining: %zu bytes\n", remaining)) {
        return false;
    }

    if (!_buf_appendf(buffer, buffer_size, &offset, "  Alignment: %zu bytes\n", alignment)) {
        return false;
    }

    /* Utilization with divide-by-zero guard (mirror arena_stats behavior) */
    if (capacity == 0u) {
        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "%s", "  Utilization: N/A (capacity is 0)\n")) {
            return false;
        }
    } else {
        double const util = (100.0 * (double)used) / (double)capacity;
        if (!_buf_appendf(buffer, buffer_size, &offset,
                          "  Utilization: %.1f%%\n", util)) {
            return false;
        }
    }

    /* iArena is a single contiguous slice; no chunk list */
    return true;
}
// ================================================================================
// ================================================================================
// eof
