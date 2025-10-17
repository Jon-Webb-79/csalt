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

struct Chunk{
    uint8_t *chunk;     // Pointer to beginning of memory
    size_t len;         // Populated length of memory within struct in bytes
    size_t alloc;       // Allocated length of memory within struct in bytes
    struct Chunk* next; // Pointer to next memory chunk
};
// -------------------------------------------------------------------------------- 

struct Arena{
    Chunk* head;       // Pointer to head of memory chunk linked list
    Chunk* tail;       // Pointer to the tail of memory chunks for the linked list
    uint8_t *cur;      // A Pointer to the next available memory slot
    size_t len;        // Total memory used in bytes between all memory chunks
    size_t alloc;      // Total memory allocated in bytes between all memory chunks
    size_t tot_alloc;  // Total memory allocated to include containers
    alloc_t mem_type;  // Type of memory used
    bool resize;       // allows resizing if true with mem_type == DYNAMIC
};
// -------------------------------------------------------------------------------- 

typedef struct {
    Chunk* chunk;     // Pointer to saved memory chunk
    uint8_t* cur;     // Current pointer position in saved chunk
    size_t len;       // Length of saved chunk
} ArenaCheckPointRep;
// ================================================================================ 
// ================================================================================ 

static const size_t k_min_chunk = 4096u;  /* internal default minimum chunk size */

/* alignment policy (power-of-two). 0 => use alignof(max_align_t) */
static _Atomic size_t g_default_alignment = alignof(max_align_t);

// -------------------------------------------------------------------------------- 

void set_default_arena_alignment(size_t alignment) {
    if (alignment == 0) { atomic_store_explicit(&g_default_alignment, alignof(max_align_t), memory_order_release); return; }
    if ((alignment & (alignment - 1)) != 0) { return; }
    atomic_store_explicit(&g_default_alignment, alignment, memory_order_release);
}
// -------------------------------------------------------------------------------- 

size_t default_arena_alignment(void) {
    return atomic_load_explicit(&g_default_alignment, memory_order_acquire);
}
// -------------------------------------------------------------------------------- 

void reset_default_arena_alignment(void) {
    atomic_store_explicit(&g_default_alignment, alignof(max_align_t), memory_order_relaxed);
}
// -------------------------------------------------------------------------------- 

static inline size_t _align_up_size(size_t x, size_t a) {
    /* a must be power-of-two */
    return (x + (a - 1)) & ~(a - 1);
}

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

    /* Optional: exact footprint for tot_alloc (uncomment if you want to use it)
    if (footprint_out != NULL) {
        *footprint_out = (size_t)((data_p - base) + (uintptr_t)data_bytes);
    }
    */

    return ch; /* free(ch) later frees header+pad+data in one go */
}
// -------------------------------------------------------------------------------- 

Arena* init_dynamic_arena(size_t bytes, bool resize) {
    size_t const a = default_arena_alignment(); /* must be power-of-two */
    if ((a == 0U) || ((a & (a - 1U)) != 0U)) {
        errno = EINVAL;
        return NULL;
    }

    /* NOTE: If you enforce a minimum (k_min_chunk), that can change total_alloc.
       If you truly want total_alloc == user 'bytes', either remove the min
       or document that 'bytes' will be raised to k_min_chunk. */
    if (bytes < k_min_chunk) {
        bytes = k_min_chunk;  /* If you prefer exact user total, remove this. */
    }

    /* Must fit Arena + Chunk inside 'bytes' */
    if (bytes < (sizeof(struct Arena) + sizeof(struct Chunk))) {
        errno = EINVAL;
        return NULL;
    }

    void* base = malloc(bytes);
    if (base == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    uintptr_t const base_u = (uintptr_t)base;

    /* Place Arena at malloc base (malloc guarantees max_align_t alignment) */
    uintptr_t p_arena = base_u;
    uintptr_t const arena_end = p_arena + (uintptr_t)sizeof(struct Arena);
    if (arena_end < p_arena) { free(base); errno = ENOMEM; return NULL; }

    /* Place Chunk immediately after Arena, aligned for Chunk */
    uintptr_t p_chunk = _align_up_uintptr(arena_end, alignof(struct Chunk));
    uintptr_t const chunk_end = p_chunk + (uintptr_t)sizeof(struct Chunk);
    if ((chunk_end < p_chunk) || (chunk_end > (base_u + bytes))) {
        free(base); errno = ENOMEM; return NULL;
    }

    /* Place data aligned to allocator policy; NO ROUND-UP of total */
    uintptr_t p_data = _align_up_uintptr(chunk_end, a);
    if (p_data > (base_u + bytes)) {
        free(base); errno = EINVAL; return NULL;
    }

    size_t const usable = (size_t)((base_u + bytes) - p_data);
    if (usable == 0U) {
        free(base); errno = EINVAL; return NULL;
    }

    /* Stitch it together */
    struct Arena* arena = (struct Arena*)p_arena;
    struct Chunk* head  = (struct Chunk*)p_chunk;

    head->chunk = (uint8_t*)p_data;
    head->len   = 0U;
    head->alloc = usable;
    head->next  = NULL;

    arena->head      = head;
    arena->tail      = head;
    arena->cur       = (uint8_t*)p_data;
    arena->len       = 0U;
    arena->alloc     = usable;    /* usable capacity in head chunk */
    arena->tot_alloc = bytes;     /* EXACTLY what caller asked for */
    arena->mem_type  = DYNAMIC;
    arena->resize    = resize;

    /* Because malloc base == arena pointer, free_arena() must free(arena) */
    return arena;
}
// -------------------------------------------------------------------------------- 

Arena* init_static_arena(void* buffer, size_t bytes) {
    if (!buffer) {
        errno = EINVAL;
        return NULL;
    }

    uintptr_t base = (uintptr_t)buffer;

    // Require buffer alignment for max_align_t
    if ((base & (alignof(max_align_t) - 1)) != 0) {
        errno = EINVAL;
        return NULL;
    }

    // Place Arena at buffer start (aligned)
    uintptr_t p = _align_up_uintptr(base, alignof(Arena));
    uintptr_t arena_end = p + sizeof(Arena);
    if (arena_end < p) { errno = ENOMEM; return NULL; }

    // Place Chunk immediately after Arena (aligned)
    uintptr_t c = _align_up_uintptr(arena_end, alignof(struct Chunk));
    uintptr_t chunk_end = c + sizeof(struct Chunk);
    if (chunk_end < c) { errno = ENOMEM; return NULL; }

    // Align data region to arena’s alignment policy
    uintptr_t data = _align_up_uintptr(chunk_end, default_arena_alignment());

    if (data > base + bytes) {
        errno = EINVAL;
        return NULL;
    }

    size_t usable = (size_t)((base + bytes) - data);
    if (usable == 0) {
        errno = EINVAL;
        return NULL;
    }

    Arena* arena = (Arena*)p;
    struct Chunk* head = (struct Chunk*)c;

    // Initialize Chunk
    head->chunk = (uint8_t*)data;
    head->len   = 0;
    head->alloc = usable;
    head->next  = NULL;

    // Initialize Arena
    arena->head      = head;
    arena->tail      = head;
    arena->cur       = (uint8_t*)data;
    arena->len       = 0;
    arena->alloc     = usable;
    arena->tot_alloc = bytes;   // total buffer footprint
    arena->mem_type  = STATIC;
    arena->resize    = false;

    return arena;
}
// -------------------------------------------------------------------------------- 

/* Deterministic growth knobs (choose values once for your build) */
static const size_t k_growth_limit = (size_t)1u << 20; /* 1 MiB: switch from 2x to 1.5x */
static const size_t k_max_chunk    = (size_t)1u << 24; /* 16 MiB: cap single chunk size */

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

/* Decide data capacity for the *new* chunk (usable bytes, not footprint). */
static size_t _next_chunk_size(size_t prev_data_alloc, size_t need, size_t align) {
    /* meet the request at minimum */
    size_t grow = (need > prev_data_alloc) ? need : prev_data_alloc;

    /* geometric target: 2x until growth_limit, then 1.5x */
    size_t doubled = (prev_data_alloc <= (SIZE_MAX / 2u)) ? (prev_data_alloc << 1) : SIZE_MAX;
    size_t onefive = _mul_div_ceil(prev_data_alloc, 3u, 2u); /* 1.5x */

    size_t target = (prev_data_alloc < k_growth_limit) ? doubled : onefive;
    if (target > grow) { grow = target; }

    /* clamp to floor/ceiling */
    if (grow < k_min_chunk) { grow = k_min_chunk; }
    if (grow > k_max_chunk) { grow = k_max_chunk; }

    /* align capacity to alignment to keep chunk->chunk naturally aligned */
    grow = _align_up_size(grow, align);

    /* final safety: ensure capacity still covers need after rounding */
    if (grow < need) { grow = need; }

    return grow;
}

static inline size_t pad_up(uintptr_t p, size_t a) {
    size_t const mask = a - 1u;                // a is power-of-two
    return (size_t)(((p + mask) & ~mask) - p);  // 0..a-1
}

void* alloc_arena(Arena* arena, size_t bytes, bool zeroed) {
    if ((arena == NULL) || (bytes == 0u)) { errno = EINVAL; return NULL; }

    size_t const a = default_arena_alignment();                   // must be pow2
    if (a == 0u || (a & (a - 1u)) != 0u) { errno = EINVAL; return NULL; }

    struct Chunk* tail = arena->tail;
    if (tail == NULL) { errno = EINVAL; return NULL; }

    uintptr_t const cur = (uintptr_t)arena->cur;
    size_t const pad = pad_up(cur, a);                            // NEW
    size_t const need = pad + bytes;

    /* fast path: current tail has space, no extra pad required here */
    size_t const avail = (tail->alloc >= tail->len) ? (tail->alloc - tail->len) : 0u;
    if (avail >= need) {
        uint8_t* p = (uint8_t*)(cur + pad);                       // aligned
        arena->cur  = p + bytes;
        tail->len  += need;                                       // count pad+bytes
        arena->len += need;
        if (zeroed) memset(p, 0, bytes);
        return p;
    }

    /* STATIC arenas cannot grow */
    if (arena->mem_type == STATIC || arena->resize == false) { errno = EPERM; return NULL; }

    size_t const grow_data = _next_chunk_size(tail->alloc, need, a);

    struct Chunk* nc = _chunk_new_ex(grow_data, a);
    if (nc == NULL) { return NULL; } /* errno set */

    /* link */
    tail->next   = nc;
    arena->tail  = nc;

    /* accounting (approximate header footprint; see note below) */
    arena->alloc     += nc->alloc; /* usable data capacity added */
    arena->tot_alloc += _align_up_size(sizeof(struct Chunk), a) + nc->alloc;

    /* allocate from new chunk */
    void* p = (void*)nc->chunk;
    arena->cur  = nc->chunk + bytes;
    nc->len     = bytes;
    arena->len += bytes;

    if (zeroed) { (void)memset(p, 0, bytes); }
    return p;
}
// -------------------------------------------------------------------------------- 

void* alloc_arena_aligned(Arena* arena, size_t bytes, size_t alignment, bool zeroed) {
    if (!arena || bytes == 0u) { errno = EINVAL; return NULL; }
    if (alignment == 0u || (alignment & (alignment - 1u)) != 0u) {
        errno = EINVAL; return NULL;  // requested alignment must be pow2
    }
    struct Chunk* tail = arena->tail;
    if (!tail) { errno = EINVAL; return NULL; }

    // Effective alignment = max(requested, arena policy)
    size_t const a_policy = default_arena_alignment();
    if (a_policy == 0u || (a_policy & (a_policy - 1u)) != 0u) { errno = EINVAL; return NULL; }
    size_t const eff_a = (alignment > a_policy) ? alignment : a_policy;

    // Per-allocation padding from current cursor
    uintptr_t const cur = (uintptr_t)arena->cur;
    size_t const pad = pad_up(cur, eff_a);
    if (pad > SIZE_MAX - bytes) { errno = ENOMEM; return NULL; }
    size_t const need = pad + bytes;

    // Available space in tail
    size_t const avail = (tail->alloc >= tail->len) ? (tail->alloc - tail->len) : 0u;

    // Fast path: fits in current chunk
    if (avail >= need) {
        uint8_t* p = (uint8_t*)(cur + pad);   // eff_a-aligned
        arena->cur  = p + bytes;
        tail->len  += need;                   // charge pad+bytes
        arena->len += need;
        if (zeroed) memset(p, 0, bytes);
        return p;
    }

    // Grow path
    if (arena->mem_type == STATIC || !arena->resize) { errno = EPERM; return NULL; }

    size_t const grow_data = _next_chunk_size(tail->alloc, need, eff_a);
    struct Chunk* nc = _chunk_new_ex(grow_data, eff_a);  // must return eff_a-aligned data
    if (!nc) return NULL; // errno set by helper

    // Link new chunk
    tail->next  = nc;
    arena->tail = nc;

    // Accounting
    size_t const hdr = _align_up_size(sizeof(struct Chunk), eff_a);
    if (arena->alloc > SIZE_MAX - nc->alloc) { errno = ENOMEM; return NULL; }
    arena->alloc += nc->alloc;
    if (arena->tot_alloc > SIZE_MAX - (hdr + nc->alloc)) { errno = ENOMEM; return NULL; }
    arena->tot_alloc += hdr + nc->alloc;

    // First alloc from fresh chunk: base is eff_a-aligned -> no pad
    void* p = (void*)nc->chunk;
    nc->len     = bytes;
    arena->len += bytes;
    arena->cur  = nc->chunk + bytes;
    if (zeroed) memset(p, 0, bytes);
    return p;
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
// -------------------------------------------------------------------------------- 

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

alloc_t arena_mtype(const Arena* arena) {
    if (!arena) {
        errno = EINVAL;
        return ALLOC_INVALID;
    }
    return arena->mem_type;
}
// -------------------------------------------------------------------------------- 

void reset_arena(Arena *arena, bool trim_extra_chunks)
{
    if (arena == NULL) {
        return;
    }

    // No chunks: normalize and return
    if (arena->head == NULL) {
        arena->cur  = NULL;
        arena->len  = 0U;
        // leave alloc/tot_alloc unchanged
        arena->tail = NULL;
        return;
    }

    // Zero usage counters on all chunks
    for (Chunk *cur = arena->head; cur != NULL; cur = cur->next) {
        cur->len = 0U;
    }
    arena->len = 0U;

    if (trim_extra_chunks && arena->mem_type == DYNAMIC) {
        // Free all chunks after head
        Chunk *to_free = arena->head->next;
        while (to_free != NULL) {
            Chunk *next = to_free->next;
            free(to_free->chunk);
            free(to_free);
            to_free = next;
        }

        // Detach and normalize to a single head chunk
        arena->head->next = NULL;
        arena->tail       = arena->head;
        arena->cur        = arena->head->chunk;

        // Recompute accounting for one-chunk state
        arena->alloc     = arena->head->alloc;
        arena->tot_alloc = arena->head->alloc + sizeof(Arena) + sizeof(Chunk);
    } else {
        // Not trimming (or STATIC arena): keep all chunks allocated.
        // Ensure cur/tail refer to the SAME chunk to preserve invariants.
        arena->tail = (arena->tail != NULL) ? arena->tail : arena->head;
        arena->cur  = (arena->tail->chunk != NULL) ? arena->tail->chunk
                                                   : arena->head->chunk;
        // alloc/tot_alloc remain unchanged (capacity retained)
    }
}
// -------------------------------------------------------------------------------- 

static inline void cp_pack(ArenaCheckPoint* pub, const ArenaCheckPointRep* rep) {
    memcpy(pub, rep, sizeof *rep);
}
static inline void cp_unpack(const ArenaCheckPoint* pub, ArenaCheckPointRep* rep) {
    memcpy(rep, pub, sizeof *rep);
}
// --------------------------------------------------------------------------------

ArenaCheckPoint save_arena(const Arena* arena) {
    ArenaCheckPoint pub = {0};
    ArenaCheckPointRep rep = {0};
    if (arena) {
        rep.chunk = arena->tail;
        rep.cur   = arena->cur;
        rep.len   = arena->len;
    }
    cp_pack(&pub, &rep);
    return pub;
}
// -------------------------------------------------------------------------------- 

bool restore_arena(Arena* arena, ArenaCheckPoint cp) {
    if (!arena) { errno = EINVAL; return false; }

    ArenaCheckPointRep rep;
    cp_unpack(&cp, &rep);

    if (!rep.chunk) return true;                 // restoring to "empty" OK

    // Validate rep.cur within chunk
    if (!rep.chunk->chunk) { errno = EINVAL; return false; }
    uintptr_t s = (uintptr_t)rep.chunk->chunk;
    uintptr_t e = s + rep.chunk->alloc;         // end (exclusive)
    uintptr_t pc = (uintptr_t)rep.cur;
    if (e < s || pc < s || pc > e) {            // overflow or out of range
        errno = EINVAL; 
        return false;
    }

    // If dynamic, free every chunk after the checkpoint chunk
    if (arena->mem_type == DYNAMIC) {
        Chunk* cur = rep.chunk->next;
        while (cur) {
            Chunk* next = cur->next;
            free(cur->chunk);
            free(cur);
            cur = next;
        }
        rep.chunk->next = NULL;
    }

    // Restore pointers and per-chunk used
    rep.chunk->len = (size_t)(pc - s);          // used bytes in tail chunk
    arena->tail    = rep.chunk;
    arena->cur     = rep.cur;

    // Restore global used bytes
    arena->len     = rep.len;

    // Recompute capacity & footprint from remaining chain (defensive)
    size_t cap = 0, foot = sizeof(Arena);
    for (const Chunk* k = arena->head; k; k = k->next) {
        cap  += k->alloc;
        foot += sizeof(Chunk) + k->alloc;
    }
    arena->alloc     = cap;
    arena->tot_alloc = foot;

    return true;
}

// -------------------------------------------------------------------------------- 

static bool buf_appendf(char *buffer,
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

bool arena_stats(const Arena *arena, char *buffer, size_t buffer_size) {
    size_t offset = 0U;

    if ((buffer == NULL) || (buffer_size == 0U)) {
        errno = EINVAL;
        return false;
    }

    if (arena == NULL) {
        (void)buf_appendf(buffer, buffer_size, &offset, "%s", "Arena: NULL\n");
        return true;
    }

    if (!buf_appendf(buffer, buffer_size, &offset, "%s", "Arena Statistics:\n")) {
        return false;
    }

    if (!buf_appendf(buffer, buffer_size, &offset,
                     "  Type: %s\n",
                     (arena->mem_type == STATIC) ? "STATIC" : "DYNAMIC")) {
        return false;
    }

    if (!buf_appendf(buffer, buffer_size, &offset,
                     "  Used: %zu bytes\n", arena->len)) {
        return false;
    }

    if (!buf_appendf(buffer, buffer_size, &offset,
                     "  Capacity: %zu bytes\n", arena->alloc)) {
        return false;
    }

    if (!buf_appendf(buffer, buffer_size, &offset,
                     "  Total (with overhead): %zu bytes\n", arena->tot_alloc)) {
        return false;
    }

    /* Utilization with divide-by-zero guard */
    if (arena->alloc == 0U) {
        if (!buf_appendf(buffer, buffer_size, &offset,
                         "%s", "  Utilization: N/A (capacity is 0)\n")) {
            return false;
        }
    } else {
        double const util = (100.0 * (double)arena->len) / (double)arena->alloc;
        if (!buf_appendf(buffer, buffer_size, &offset,
                         "  Utilization: %.1f%%\n", util)) {
            return false;
        }
        /* If you prefer to avoid floating point in MISRA contexts,
           see the integer alternative below. */
    }

    /* List chunks */
    {
        int chunk_count = 0;
        Chunk *current = arena->head;
        while (current != NULL) {
            chunk_count += 1;
            if (!buf_appendf(buffer, buffer_size, &offset,
                             "  Chunk %d: %zu/%zu bytes\n",
                             chunk_count, current->len, current->alloc)) {
                return false;
            }
            current = current->next;
        }
    }

    return true;
}
// -------------------------------------------------------------------------------- 

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
// ================================================================================
// ================================================================================
// eof
