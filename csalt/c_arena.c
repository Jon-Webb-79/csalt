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
    uint8_t *chunk;    // Pointer to beginning of memory
    size_t len;     // Populated length of memory within struct in bytes
    size_t alloc;   // Allocated length of memory within struct in bytes
    struct Chunk* next; // Pointer to next memory chunk
};
// -------------------------------------------------------------------------------- 

struct Arena{
    Chunk* head;   // Pointer to head of memory chunk linked list
    Chunk* tail;   // Pointer to the tail of memory chunks for the linked list
    uint8_t *cur;     // A Pointer to the next available memory slot
    size_t len;       // Total memory used in bytes between all memory chunks
    size_t alloc;     // Total memory allocated in bytes between all memory chunks
    size_t tot_alloc; // Total memory allocated to include containers
    alloc_t mem_type;  // Type of memory used
};
// -------------------------------------------------------------------------------- 

typedef struct {
    Chunk* chunk;  // Pointer to saved memory chunk
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
// -------------------------------------------------------------------------------- 

static Chunk* _create_memory_chunk(size_t bytes) {
    if (bytes == 0) { errno = EINVAL; return NULL; }

    Chunk *mc = malloc(sizeof(*mc));
    if (!mc) { errno = ENOMEM; return NULL; }

    uint8_t *blk = (uint8_t*)malloc(bytes); /* byte granularity */
    if (!blk) { errno = ENOMEM; free(mc); return NULL; }

    mc->chunk = blk;
    mc->len   = 0;
    mc->alloc = bytes;
    mc->next  = NULL;
    return mc;
}
// -------------------------------------------------------------------------------- 

Arena* init_dynamic_arena(size_t bytes)
{
    /* Policy: round up to minimum + alignment to reduce immediate spillover */
    size_t const a = default_arena_alignment();
    if ((a & (a - 1)) != 0 || a == 0) {
        /* Should never happen if setter validated, but be defensive */
        errno = EINVAL; 
        return NULL;
    }

    if (bytes < k_min_chunk) bytes = k_min_chunk;
    bytes = _align_up_size(bytes, a);

    /* Guard tot_alloc overflow: bytes + sizeof(Arena)+sizeof(MemChunk) */
    size_t const overhead = sizeof(struct Arena) + sizeof(Chunk);
    if (bytes > SIZE_MAX - overhead) {
        errno = ENOMEM;
        return NULL;
    }

    Chunk *mc = _create_memory_chunk(bytes);
    if (!mc) return NULL;

    struct Arena *arena = (struct Arena*)malloc(sizeof *arena);
    if (!arena) {
        errno = ENOMEM;
        free(mc->chunk);
        free(mc);
        return NULL;
    }

    arena->head      = mc;
    arena->tail      = mc;
    arena->cur       = mc->chunk;
    arena->len       = 0;
    arena->alloc     = bytes;
    arena->tot_alloc = bytes + overhead;
    arena->mem_type  = DYNAMIC;

    return arena;
}
// -------------------------------------------------------------------------------- 

static inline uintptr_t align_up_uintptr(uintptr_t p, size_t a) {
    return (p + (a - 1)) & ~(a - 1);
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
    uintptr_t p = align_up_uintptr(base, alignof(Arena));
    uintptr_t arena_end = p + sizeof(Arena);
    if (arena_end < p) { errno = ENOMEM; return NULL; }

    // Place Chunk immediately after Arena (aligned)
    uintptr_t c = align_up_uintptr(arena_end, alignof(struct Chunk));
    uintptr_t chunk_end = c + sizeof(struct Chunk);
    if (chunk_end < c) { errno = ENOMEM; return NULL; }

    // Align data region to arena’s alignment policy
    uintptr_t data = align_up_uintptr(chunk_end, default_arena_alignment());

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

    return arena;
}
// -------------------------------------------------------------------------------- 

static inline size_t align_up_size(size_t x, size_t a) {
    return (x + (a - 1)) & ~(a - 1);
}
// -------------------------------------------------------------------------------- 

static size_t next_chunk_size(size_t prev_alloc, size_t need, size_t align)
{
    /* Guard inputs */
    if (align == 0 || (align & (align - 1)) != 0) return 0;   /* invalid align */
    if (need == 0) return 0;

    /* Tunables: adjust to taste */
    const size_t MIN_SLACK = 16u * 1024u;        /* 16 KiB */
    const size_t MAX_SLACK = 8u  * 1024u * 1024u;/* 8 MiB  */

    /* Heuristic: decide if this is a "big" request relative to current chunk. */
    int big_request = (prev_alloc == 0) ? 1 : (need >= (prev_alloc * 3) / 4);

    size_t slack;
    size_t target;

    if (big_request) {
        /* Allocate exactly what we need, plus bounded slack. */
        slack = prev_alloc / 2;
        if (slack < MIN_SLACK) slack = MIN_SLACK;
        if (slack > MAX_SLACK) slack = MAX_SLACK;

        /* target = need + slack (with overflow clamp) */
        if (need > SIZE_MAX - slack) target = need; else target = need + slack;
    } else {
        /* Geometric growth for small/medium total sizes.
           Use 2x for very small chunks, 1.5x up to ~16 MiB, then 1.25x. */
        size_t factor_num, factor_den;
        if (prev_alloc < (64u * 1024u)) {              /* < 64 KiB */
            factor_num = 2;  factor_den = 1;           /* 2.0x */
        } else if (prev_alloc < (16u * 1024u * 1024u)) {
            factor_num = 3;  factor_den = 2;           /* 1.5x */
        } else {
            factor_num = 5;  factor_den = 4;           /* 1.25x */
        }

        /* candidate = prev_alloc * factor (overflow-safe) */
        size_t candidate;
        if (prev_alloc != 0 && prev_alloc > SIZE_MAX / factor_num) candidate = SIZE_MAX;
        else candidate = (prev_alloc * factor_num) / factor_den;

        /* Ensure we still satisfy the immediate need */
        if (candidate < need) candidate = need;
        target = candidate;
    }

    /* Align up; if it wraps, fall back to need aligned. */
    size_t aligned = align_up_size(target, align);
    if (aligned < target) aligned = align_up_size(need, align);
    return aligned;
}
// -------------------------------------------------------------------------------- 

void* alloc_arena(Arena* arena, size_t bytes, bool zeroed) {
    if (!arena || bytes == 0) { errno = EINVAL; return NULL; }

    Chunk* c = arena->tail;
    if (!c) { errno = EINVAL; return NULL; }

    /* NB: pick one name and use it everywhere: arena_default_alignment() */
    size_t const a = default_arena_alignment();
    if (a == 0 || (a & (a - 1))) { errno = EINVAL; return NULL; }  /* must be pow2 */

    /* Compute how much we need in the CURRENT chunk (pad + bytes) */
    uintptr_t cur     = (uintptr_t)arena->cur;
    uintptr_t aligned = align_up_uintptr(cur, a);
    size_t    pad     = (size_t)(aligned - cur);

    if (pad > SIZE_MAX - bytes) { errno = ENOMEM; return NULL; }
    size_t need = pad + bytes;

    /* Fast path: fits in current chunk */
    size_t available = (c->alloc >= c->len) ? (c->alloc - c->len) : 0;
    if (available >= need) {
        /* consume padding */
        arena->cur  = (uint8_t*)aligned;
        c->len     += pad;
        arena->len += pad;

        /* allocate */
        void* p = (void*)arena->cur;
        arena->cur  += bytes;
        c->len      += bytes;
        arena->len  += bytes;

        if (zeroed) memset(p, 0, bytes);
        return p;
    }

    /* Need a new chunk */
    if (arena->mem_type == STATIC) { errno = EPERM; return NULL; }

    /* Ask policy for the next chunk size. It must be >= need and aligned. */
    size_t new_size = next_chunk_size(c->alloc, need, a);
    if (new_size == 0) { errno = EINVAL; return NULL; }

    /* Guard total-accounting overflow (optional but nice) */
    if (new_size > SIZE_MAX - sizeof(Chunk)) { errno = ENOMEM; return NULL; }
    if (arena->alloc > SIZE_MAX - new_size)    { errno = ENOMEM; return NULL; }
    if (arena->tot_alloc > SIZE_MAX - (new_size + sizeof(Chunk))) {
        errno = ENOMEM; return NULL;
    }

    Chunk* n = _create_memory_chunk(new_size);
    if (!n) return NULL; /* errno set */

    /* Link it */
    c->next     = n;
    arena->tail = n;

    /* Align the first allocation inside THIS new chunk */
    uintptr_t basep    = (uintptr_t)n->chunk;
    uintptr_t aligned2 = align_up_uintptr(basep, a);
    size_t    pad2     = (size_t)(aligned2 - basep);

    /* Sanity: we should fit bytes after padding inside this chunk */
    if (pad2 > n->alloc || (n->alloc - pad2) < bytes) {
        /* Should not happen if next_chunk_size did its job */
        errno = ENOMEM;
        /* unlink to be extra safe */
        c->next     = NULL;
        arena->tail = c;
        free(n->chunk);
        free(n);
        return NULL;
    }

    /* Account for the new chunk */
    arena->alloc     += n->alloc;
    arena->tot_alloc += n->alloc + sizeof(Chunk);

    /* Consume the internal padding in the new chunk */
    n->len      = pad2;
    arena->len += pad2;
    arena->cur  = (uint8_t*)aligned2;

    /* Place the object */
    void* p = (void*)arena->cur;
    arena->cur  += bytes;
    n->len      += bytes;
    arena->len  += bytes;

    if (zeroed) memset(p, 0, bytes);
    return p;
}
// -------------------------------------------------------------------------------- 

void* alloc_alloc_aligned(Arena* arena, size_t bytes, size_t alignment, bool zeroed) {
    if (!arena || bytes == 0) { errno = EINVAL; return NULL; }
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) { errno = EINVAL; return NULL; }

    Chunk* c = arena->tail;
    if (!c) { errno = EINVAL; return NULL; }

    /* Compute required space in current chunk: padding + payload */
    uintptr_t cur     = (uintptr_t)arena->cur;
    uintptr_t aligned = align_up_uintptr(cur, alignment);
    size_t    pad     = (size_t)(aligned - cur);

    if (pad > SIZE_MAX - bytes) { errno = ENOMEM; return NULL; }
    size_t need = pad + bytes;

    /* Fast path: fits in current tail chunk */
    size_t available = (c->alloc >= c->len) ? (c->alloc - c->len) : 0;
    if (available >= need) {
        /* consume padding */
        arena->cur  = (uint8_t*)aligned;
        c->len     += pad;
        arena->len += pad;

        /* place object */
        void* p = (void*)arena->cur;
        arena->cur  += bytes;
        c->len      += bytes;
        arena->len  += bytes;

        if (zeroed) memset(p, 0, bytes);
        return p;
    }

    /* Need a new chunk */
    if (arena->mem_type == STATIC) { errno = EPERM; return NULL; }

    size_t new_size = next_chunk_size(c->alloc, need, alignment);
    if (new_size == 0) { errno = EINVAL; return NULL; }

    /* Global accounting overflow guards (defensive) */
    if (new_size > SIZE_MAX - sizeof(Chunk)) { errno = ENOMEM; return NULL; }
    if (arena->alloc > SIZE_MAX - new_size) { errno = ENOMEM; return NULL; }
    if (arena->tot_alloc > SIZE_MAX - (new_size + sizeof(Chunk))) { errno = ENOMEM; return NULL; }

    Chunk* n = _create_memory_chunk(new_size);
    if (!n) return NULL; /* errno set by helper */

    /* Link new tail */
    c->next     = n;
    arena->tail = n;

    /* Align inside the new chunk */
    uintptr_t basep    = (uintptr_t)n->chunk;
    uintptr_t aligned2 = align_up_uintptr(basep, alignment);
    size_t    pad2     = (size_t)(aligned2 - basep);

    /* Sanity: ensure payload fits after pad in the new chunk */
    if (pad2 > n->alloc || (n->alloc - pad2) < bytes) {
        /* Shouldn’t happen if next_chunk_size sized correctly */
        errno = ENOMEM;
        c->next     = NULL;
        arena->tail = c;
        free(n->chunk);
        free(n);
        return NULL;
    }

    /* Update arena accounting for the new chunk */
    arena->alloc     += n->alloc;
    arena->tot_alloc += n->alloc + sizeof(Chunk);

    /* Consume padding within new chunk */
    n->len      = pad2;
    arena->len += pad2;
    arena->cur  = (uint8_t*)aligned2;

    /* Place the object */
    void* p = (void*)arena->cur;
    arena->cur  += bytes;
    n->len      += bytes;
    arena->len  += bytes;

    if (zeroed) memset(p, 0, bytes);
    return p;
}
// -------------------------------------------------------------------------------- 

void free_arena(Arena* arena) {
    if (!arena) return;

    if (arena->mem_type == STATIC) {
        errno = EINVAL;
        return;
    }

    Chunk* cur = arena->head;
    while (cur) {
        Chunk* next = cur->next;

        free(cur->chunk);
        free(cur);

        cur = next;
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
