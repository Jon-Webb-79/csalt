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
#include "c_arena.h"
// ================================================================================ 
// ================================================================================ 
// CONSTANTS 

/* Deterministic growth knobs (choose values once for your build) */
static const size_t k_growth_limit = (size_t)1u << 20; /* 1 MiB: switch from 2x to 1.5x */
static const size_t k_max_chunk    = (size_t)1u << 24; /* 16 MiB: cap single chunk size */
// ================================================================================ 
// ================================================================================ 
// DATA STRUCTURES 

// 25 bytes
struct Chunk{
    uint8_t *chunk;     // Pointer to beginning of memory
    size_t len;         // Populated length of memory within struct in bytes
    size_t alloc;       // Allocated length of memory within struct in bytes
    struct Chunk* next; // Pointer to next memory chunk
};
// -------------------------------------------------------------------------------- 

// 72 bytes
struct Arena{
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

static inline size_t _align_up_size(size_t x, size_t a) {
    /* a must be power-of-two */
    return (x + (a - 1)) & ~(a - 1);
}
// -------------------------------------------------------------------------------- 

static inline uintptr_t _align_up_uintptr(uintptr_t p, size_t a) {
    return (p + (a - 1)) & ~(a - 1);
}
// -------------------------------------------------------------------------------- 

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

static const Chunk* find_chunk_in_chain(const Arena* a, const Chunk* target, Chunk const** prev_out) {
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

Arena* init_dynamic_arena(size_t bytes, bool resize, size_t min_chunk_in, size_t base_align_in) {
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
    if (total < (sizeof(Arena) + sizeof(Chunk))) { errno = EINVAL; return NULL; }

    void* base = malloc(total);
    if (!base) { errno = ENOMEM; return NULL; }
    uintptr_t const b = (uintptr_t)base;

    uintptr_t p_arena   = b;
    uintptr_t arena_end = p_arena + sizeof(Arena);
    if (arena_end < p_arena) { free(base); errno = ENOMEM; return NULL; }

    uintptr_t p_chunk   = _align_up_uintptr(arena_end, alignof(Chunk));
    uintptr_t chunk_end = p_chunk + sizeof(Chunk);
    if (chunk_end < p_chunk || chunk_end > b + total) { free(base); errno = ENOMEM; return NULL; }

    // Data starts aligned to *base_align* (not default_arena_alignment)
    uintptr_t p_data    = _align_up_uintptr(chunk_end, base_align);
    if (p_data > b + total) { free(base); errno = EINVAL; return NULL; }

    size_t usable = (size_t)((b + total) - p_data);
    if (!usable) { free(base); errno = EINVAL; return NULL; }

    Arena* a = (Arena*)p_arena;
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
}
// -------------------------------------------------------------------------------- 

Arena* init_static_arena(void* buffer, size_t bytes, size_t alignment_in) {
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
    uintptr_t p_arena = _align_up_uintptr(b, alignof(Arena));
    if (p_arena > b_end) { errno = EINVAL; return NULL; }

    /* must have room for Arena + Chunk at minimum */
    if ((b_end - p_arena) < (sizeof(Arena) + sizeof(Chunk))) { errno = EINVAL; return NULL; }

    uintptr_t arena_end = p_arena + sizeof(Arena);
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
    Arena* a = (Arena*)p_arena;
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

Arena* init_darena(size_t bytes, bool resize) {
    return init_dynamic_arena(bytes, resize, 4096u, alignof(max_align_t));
}
// --------------------------------------------------------------------------------

Arena* init_sarena(void* buffer, size_t bytes) {
    return init_static_arena(buffer, bytes, alignof(max_align_t));
}
// -------------------------------------------------------------------------------- 

void free_arena(Arena* arena) {
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

void* alloc_arena(Arena* arena, size_t bytes, bool zeroed) {
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
}
// ================================================================================ 
// ================================================================================ 
// UTILITY FUNCTIONS

bool is_arena_ptr(const Arena* arena, const void* ptr) {
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

bool is_arena_ptr_sized(const Arena* arena, const void* ptr, size_t size) {
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

void reset_arena(Arena *arena, bool trim_extra_chunks) {
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

ArenaCheckPoint save_arena(const Arena* arena) {
    ArenaCheckPoint pub = {0};
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

bool restore_arena(Arena* arena, ArenaCheckPoint cp) {
    if (!arena) { errno = EINVAL; return false; }

    ArenaCheckPointRep rep = {0};
    _cp_unpack(&cp, &rep);

    if (!rep.chunk) return true; // empty checkpoint OK

    const Chunk* prev = NULL;
    const Chunk* hit  = find_chunk_in_chain(arena, rep.chunk, &prev);
    if (!hit) { errno = EINVAL; return false; }

    if (!rep.chunk->chunk) { errno = EINVAL; return false; }
    uintptr_t s  = (uintptr_t)rep.chunk->chunk;
    uintptr_t pc = (uintptr_t)rep.cur;
    uintptr_t e  = s + rep.chunk->alloc;  // exclusive
    if (e < s || pc < s || pc > e) { errno = EINVAL; return false; }

    if (arena->mem_type == (uint8_t)DYNAMIC) {
        Chunk* c = rep.chunk->next;
        while (c) { Chunk* nxt = c->next; free(c); c = nxt; }
        ((Chunk*)rep.chunk)->next = NULL;
    }

    // Set tail chunk's used bytes to the checkpoint cursor
    ((Chunk*)rep.chunk)->len = (size_t)(pc - s);
    arena->tail = (Chunk*)rep.chunk;
    arena->cur  = (uint8_t*)rep.cur;

    // Recompute accounting under current policy
    size_t a_policy = arena->alignment;
    if (a_policy == 0u || (a_policy & (a_policy - 1u)) != 0u) { errno = EINVAL; return false; }

    size_t total_used = 0, total_cap = 0, total_foot = _align_up_size(sizeof(Arena), a_policy);
    for (Chunk* k = arena->head; k; k = k->next) {
        size_t used = (k->len <= k->alloc) ? k->len : k->alloc;
        total_used += used;
        total_cap  += k->alloc;
        total_foot += _align_up_size(sizeof(Chunk), a_policy) + k->alloc;
    }

    arena->len       = total_used;
    arena->alloc     = total_cap;
    arena->tot_alloc = total_foot;

    return true;
}
// ================================================================================ 
// ================================================================================ 
// GETTER TUNCTIONS

size_t arena_remaining(const Arena* arena) {
    if (!arena || !arena->tail || !arena->tail->chunk) return 0u;
    size_t used = arena->tail->len;
    if (used > arena->tail->alloc) used = arena->tail->alloc;  // clamp
    return arena->tail->alloc - used;
}
// -------------------------------------------------------------------------------- 

size_t arena_chunk_count(const Arena* arena) {
    if (!arena) return 0u;
    
    size_t count = 0u;
    Chunk* current = arena->head;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}
// -------------------------------------------------------------------------------- 

alloc_t arena_mtype(const Arena* arena) {
    if (!arena) {
        errno = EINVAL;
        return ALLOC_INVALID;
    }

    return (alloc_t)arena->mem_type;
}
// -------------------------------------------------------------------------------- 

size_t arena_size(const Arena* arena) {
    if (!arena) return 0;
    return arena->len;
}
// -------------------------------------------------------------------------------- 

size_t arena_alloc(const Arena* arena) {
    if (!arena) return 0;
    return arena->alloc;
}
// -------------------------------------------------------------------------------- 

size_t total_arena_alloc(const Arena* arena) {
    if (!arena) return 0;
    return arena->tot_alloc;
}
// -------------------------------------------------------------------------------- 

size_t arena_alignment(Arena* arena) {
    if (!arena || !arena->head) {
        errno = EINVAL;
        return 0;
    }
    return arena->alignment;
}
// -------------------------------------------------------------------------------- 

size_t arena_min_chunk_size(Arena* arena) {
    if (!arena || !arena->head) {
        errno = EINVAL;
        return 0;
    }
    return arena->min_chunk;
}
// ================================================================================ 
// ================================================================================ 
// SETTER FUNCTIONS 

void toggle_arena_resize(Arena* arena, bool toggle) {
    if (arena->mem_type == STATIC) {
        errno = EPERM;
        return;
    }
    arena->resize = (uint8_t)(toggle ? 1 : 0);
}
// ================================================================================ 
// ================================================================================ 
// LOG FUNCTIONS 

bool arena_stats(const Arena *arena, char *buffer, size_t buffer_size) {
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
// eof
