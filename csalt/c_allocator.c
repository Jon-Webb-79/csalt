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

void* alloc_arena(arena_t* arena, size_t bytes, bool zeroed) {
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

alloc_t arena_mtype(const arena_t* arena) {
    if (!arena) {
        errno = EINVAL;
        return ALLOC_INVALID;
    }

    return (alloc_t)arena->mem_type;
}
// -------------------------------------------------------------------------------- 

size_t arena_size(const arena_t* arena) {
    if (!arena) {
        errno = EINVAL;
        return 0;
    }
    return arena->len;
}
// -------------------------------------------------------------------------------- 

size_t arena_alloc(const arena_t* arena) {
    if (!arena) {
        errno = EINVAL;
        return 0;
    }
    return arena->alloc;
}
// -------------------------------------------------------------------------------- 

size_t total_arena_alloc(const arena_t* arena) {
    if (!arena) {
        errno = EINVAL;
        return 0;
    }
    return arena->tot_alloc;
}
// -------------------------------------------------------------------------------- 

size_t arena_alignment(const arena_t* arena) {
    if (!arena || !arena->head) {
        errno = EINVAL;
        return 0;
    }
    return arena->alignment;
}
// -------------------------------------------------------------------------------- 

size_t arena_min_chunk_size(const arena_t* arena) {
    if (!arena || !arena->head) {
        errno = EINVAL;
        return 0;
    }
    return arena->min_chunk;
}
// ================================================================================ 
// ================================================================================ 
// SETTER FUNCTIONS 

void toggle_arena_resize(arena_t* arena, bool toggle) {
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
    // Optional “prewarm” so the first allocation is O(1)
    if (prewarm_one_chunk) {
        if (!pool_grow(p)) return NULL; // errno set by pool_grow
    }
    return p;
}
// -------------------------------------------------------------------------------- 

void* alloc_pool(pool_t* pool) {
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

void return_pool_element(pool_t* pool, void* ptr) {
    if (!pool || !ptr) return;
    pool_push_free(pool, ptr);
}
// -------------------------------------------------------------------------------- 

void reset_pool(pool_t* pool) {
    if (!pool) return;
    pool->free_list = NULL;
    pool->cur = pool->end = NULL;
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

size_t pool_block_size(pool_t* pool) {
    if (!pool) {
        errno = EINVAL;
        return 0;
    }
    return pool->block_size;
}
// -------------------------------------------------------------------------------- 
size_t pool_stride(pool_t* pool) {
    if (!pool) {
        errno = EINVAL;
        return 0;
    }
    return pool->stride;
}
// -------------------------------------------------------------------------------- 

size_t pool_total_blocks(pool_t* pool) {
    if (!pool) {
        errno = EINVAL;
        return 0;
    }
    return pool->total_blocks;
}
// -------------------------------------------------------------------------------- 

size_t pool_free_blocks(pool_t* pool) {
    if (!pool) {
        errno = EINVAL;
        return 0;
    }
    return pool->free_blocks;
}
// ================================================================================
// ================================================================================
// eof
