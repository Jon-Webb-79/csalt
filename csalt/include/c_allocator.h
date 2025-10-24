// ================================================================================
// ================================================================================
// - File:    arena.h
// - Purpose: This file contains prototypes for generalized C allocators
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    October 15, 2025
// - Version: 1.0
// - Copyright: Copyright 2025, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef arena_H
#define arena_H

// Compile for static or dynamic memory allocation for MISRA compliance
#ifndef ARENA_ENABLE_DYNAMIC
#  ifdef STATIC_ONLY
#    define ARENA_ENABLE_DYNAMIC 0
#  else
#    define ARENA_ENABLE_DYNAMIC 1
#  endif
#endif

#if defined(STATIC_ONLY) && defined(ARENA_ENABLE_DYNAMIC) && (ARENA_ENABLE_DYNAMIC+0)!=0
#  error "STATIC_ONLY set but ARENA_ENABLE_DYNAMIC != 0"
#endif

/* ================= Convenience macro switch =================
   Define -DARENA_USE_CONVENIENCE_MACROS=0 (or -DNO_ARENA_MACROS)
   to prevent these function-like macros from being defined. */
#ifndef ARENA_USE_CONVENIENCE_MACROS
#  ifdef NO_FUNCITON_MACROS
#    define ARENA_USE_CONVENIENCE_MACROS 0
#  else
#    define ARENA_USE_CONVENIENCE_MACROS 1
#  endif
#endif

/* Sanity: if someone set both, ensure consistency */
#if defined(NO_ARENA_MACROS) && (ARENA_USE_CONVENIENCE_MACROS+0)!=0
#  error "NO_ARENA_MACROS set but ARENA_USE_CONVENIENCE_MACROS != 0"
#endif
// ================================================================================ 
// ================================================================================ 

#include <stdlib.h>  // size_t 
#include <stdbool.h> // true and false
#include <stdint.h>  // unit8_t
#include <errno.h>   // errno
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 
// DATA STRUCTURS 

// Forward declarations for opaque data types
typedef struct arena_t arena_t;
typedef struct Chunk Chunk;
// -------------------------------------------------------------------------------- 

typedef struct {
    uintptr_t _priv[3];
} ArenaCheckPoint;
// -------------------------------------------------------------------------------- 

#ifndef ALLOC_H 
#define ALLOC_H 

    typedef enum {
        ALLOC_INVALID = 0,
        STATIC = 1,
        DYNAMIC = 2
    } alloc_t;

#endif /*ALLOC_H*/
// ================================================================================ 
// ================================================================================ 
// INITIALIZE AND DEALLOCATE FUNCTIONS

#if ARENA_ENABLE_DYNAMIC
/**
 * @brief Allocate a single-malloc growth Chunk with its data region aligned to @p data_align.
 *
 * This **internal** helper allocates one contiguous block laid out as:
 *
 *     [ struct Chunk header | padding (0..data_align-1) | data_bytes ]
 *
 * The returned pointer is the base of the allocation (@c struct Chunk*). The
 * @c chunk->chunk member points to the aligned start of the data region and is
 * guaranteed to satisfy @p data_align (power-of-two). The usable data capacity
 * is exactly @p data_bytes (not including header or pad).
 *
 * @param data_bytes  Usable data capacity for this chunk (must be > 0).
 * @param data_align  Alignment for the chunk's data region; must be a nonzero power-of-two.
 *
 * @return Pointer to a newly allocated @c struct Chunk on success; @c NULL on failure
 *         with @c errno set.
 *
 * @retval NULL, errno=EINVAL
 *      if @p data_bytes == 0, or @p data_align == 0, or @p data_align is not
 *      a power-of-two, or if the computed total size would overflow @c size_t.
 * @retval NULL, errno=ENOMEM
 *      if the underlying allocation fails.
 *
 * **ERRORS:**
 * Sets @c errno to EINVAL or ENOMEM as described above.
 *
 * @note  **Total footprint (growth chunks)** allocated by this function is:
 *        @c sizeof(struct Chunk) + (data_align - 1) + data_bytes,
 *        where the @c (data_align - 1) term accounts for worst-case padding
 *        to reach an aligned data base.
 *
 * @note  **Initial base allocation** (performed by @c init_dynamic_arena) is
 *        different: it places @c Arena + first @c Chunk + pad + data in a single
 *        block. That base footprint is approximately:
 *        @c sizeof(Arena) + sizeof(struct Chunk) + (base_align - 1) + first_data_bytes.
 *        Subsequent growth uses @_chunk_new_ex() and therefore does **not**
 *        include an @c Arena header in its footprint.
 *
 * @note  if compiled with the DSTATIC_ONLY flag this function will return NULL and set the 
 *        errno = ENOTSUP flag
 *
 * @warning This is an internal function. Callers must never free individual
 *          chunks or @c chunk->chunk. Use the public API @c free_arena() to
 *          release the entire Arena and all of its chunks. Internally, the owner
 *          of the chunk header is freed with @c free(ch).
 *
 * @pre ARENA_ENABLE_DYNAMIC == 1.
 * @pre @p data_align is a nonzero power-of-two.
 * @post On success: @c ch->chunk is @p data_align-aligned, @c ch->alloc == @p data_bytes,
 *       @c ch->len == 0, @c ch->next == NULL.
 *
 * @sa alloc_arena(), free_arena(), init_static_arena()
 *
 * @par Example: User code (dynamic build) that triggers growth
 * @code
 * // Dynamic build:
 * Arena* a = init_dynamic_arena(4096,true,4096, alignof(max_align_t));
 * void* p1 = alloc_arena(a, 4000, false);   // ok in first (head) chunk
 * void* p2 = alloc_arena(a, 2000, false);   // not enough space -> growth
 *                                           // internally calls: _chunk_new_ex(need, alignment)
 * // ...
 * free_arena(a);                            // frees base + all growth chunks
 * @endcode
 *
 * @par Example: Build with a 2 page minimum chunk allocation (8192 bytes)
 * Arena* a = init_dynamic_arena(4096,true, 8192, alignof(max_align_t));
 * void* p1 = alloc_arena(a, 4000, false);   // ok in first (head) chunk
 * void* p2 = alloc_arena(a, 2000, false);   // not enough space -> growth
 *                                           // internally calls: _chunk_new_ex(need, alignment)
 * // ...
 * free_arena(a);                            // frees base + all growth chunks*
 * @endcode
 */
arena_t* init_dynamic_arena(size_t bytes, bool resize, size_t min_chunk_in, size_t padding_in);
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a STATIC (non-growing on the stack) arena inside a caller-supplied buffer.
 *
 * This function constructs an arena *in place* within the memory range
 * ``[buffer, buffer + bytes)``. No heap allocations are performed. The memory
 * layout is:
 *
 *     [ arena_t header | padding/alignment | struct Chunk | pad | data... ]
 *
 * The initial (head) chunk’s data base is aligned to @p alignment_in (rounded
 * up to a power-of-two and at least @c alignof(max_align_t)). Any space lost
 * to aligning the @c arena_t, @c struct Chunk, and the data region reduces the
 * usable capacity for allocations.
 *
 * @param buffer        Caller-supplied backing store for the arena.
 *                      Must remain valid for the entire arena lifetime.
 * @param bytes         Total size in bytes of @p buffer.
 *                      Must be large enough to hold @c arena_t, one @c Chunk,
 *                      and at least one byte of data after alignment/padding.
 * @param alignment_in  Requested base alignment for the chunk’s data region.
 *                      If zero, defaults to @c alignof(max_align_t). If not a
 *                      power of two, it is rounded up to the next power of two.
 *                      Final alignment is always at least @c alignof(max_align_t).
 *
 * @return Pointer to the in-place @c arena_t on success; @c NULL on failure
 *         with @c errno set.
 *
 * @retval NULL, errno=EINVAL
 *      if @p buffer is NULL,
 *      or if @p bytes is too small to fit @c arena_t + @c Chunk after alignment,
 *      or if post-alignment usable capacity is zero,
 *      or if a normalization step detects an invalid range.
 *
 * @note  The arena’s @c tot_alloc is set to the full caller footprint:
 *        ``bytes`` (i.e., ``(uintptr_t)buffer_end - (uintptr_t)buffer``).
 *        The arena’s @c alloc is the usable data capacity after all required
 *        alignments/padding have been applied.
 *
 * @note  The arena is created in STATIC mode:
 *        - @c mem_type = STATIC
 *        - @c resize   = 0 (growth is not permitted)
 *        Calls to @c alloc_arena() that exceed remaining capacity will fail
 *        with @c errno = EPERM.
 *
 * @warning Do **not** call @c free() on @p buffer while the arena is in use.
 *          The arena header lives inside @p buffer; freeing @p buffer invalidates
 *          the arena immediately. The public @c free_arena() must not attempt
 *          to free @p buffer in STATIC mode.
 *
 * @pre  @p buffer points to at least @p bytes of writable memory.
 * @pre  The lifetime of @p buffer strictly dominates the lifetime of the arena.
 * @post On success:
 *       - @c a->head, @c a->tail initialized to a single head chunk
 *       - @c a->cur points to the start of the (aligned) data region
 *       - @c a->len == 0
 *       - @c a->alloc == usable bytes in the head chunk
 *       - @c a->tot_alloc == bytes
 *
 * @sa   alloc_arena(), reset_arena(), arena_remaining(), arena_stats()
 *
 * @par Example: Aligned caller buffer (typical case)
 * @code
 
 * arena_t *a = init_static_arena(buf, 16 * 24, alignof(max_align_t));
 * assert(a != NULL);
 *
 * void *p1 = alloc_arena(a, 1024, false);  // ok
 * void *p2 = alloc_arena(a, 8192, false);  // may fail if capacity exhausted
 *
 * // Reset discards all allocations but keeps the same capacity.
 * reset_arena(a, false);
 *
 * // Tear-down: STATIC arena does not own 'buf'.
 * // free_arena(a) should NOT free 'buf'; many implementations make it a no-op or set errno=EPERM.
 * free(buf);
 * @endcode
 *
 * @par Example: Intentionally unaligned buffer
 * @code
 * enum { RAW = 16384 + 1 };
 * void *raw = malloc(RAW);
 * assert(raw != NULL);
 *
 * // Force an unaligned start:
 * uint8_t *unaligned = (uint8_t*)raw + 1;
 *
 * // init_static_arena() will align the internal structures and data base.
 * // Usable capacity will be RAW-1 minus header/padding.
 * arena_t *a = init_static_arena(unaligned, RAW - 1, 64);
 * assert(a != NULL);
 *
 * void *p = alloc_arena(a, 256, true);
 * assert(p != NULL);
 * assert(((uintptr_t)p % 64) == 0); // data pointer respects requested alignment
 *
 * free(raw);
 * @endcode
 */
arena_t* init_static_arena(void* buffer, size_t bytes, size_t passing_in);
// -------------------------------------------------------------------------------- 

#if ARENA_ENABLE_DYNAMIC
/**
 * @brief Convenience initializer for a dynamic arena with common defaults.
 *
 * This helper calls:
 *
 *   init_dynamic_arena(bytes,
 *                      resize,
 *                      4096u,
 *                      alignof(max_align_t));
 *
 * Use it when you want a dynamic (heap-growing) arena with a reasonable
 * minimum chunk size (4 KiB) and a base alignment of at least
 * @c alignof(max_align_t). If you need different tuning for growth chunk size
 * or alignment, call @c init_dynamic_arena() directly.
 *
 * @param bytes   Initial allocation footprint requested for the arena’s first
 *                block. The implementation may increase it to satisfy header
 *                placement and the minimum chunk rule.
 * @param resize  If @c true, the arena may grow by allocating additional
 *                chunks as needed; if @c false, allocations that exceed the
 *                remaining capacity will fail with @c errno = EPERM.
 *
 * @return Pointer to a newly created @c arena_t on success; @c NULL on failure
 *         with @c errno set by @c init_dynamic_arena().
 *
 * @retval NULL, errno=EINVAL  Propagated from @c init_dynamic_arena() if
 *                             input or computed layout is invalid.
 * @retval NULL, errno=ENOMEM  If the initial allocation fails.
 *
 * @note Minimum chunk size is fixed at 4096 bytes (4 KiB) in this wrapper.
 *       This is a conventional page-sized default, not a guarantee of OS
 *       page size. Choose a different value via @c init_dynamic_arena()
 *       if your workload benefits from larger/smaller growth steps.
 *
 * @note The arena’s base alignment is @c alignof(max_align_t). Per-allocation
 *       alignment within the arena follows this base unless you use the
 *       aligned allocation variant (if provided in your API).
 *
 * @warning Dynamic arenas allocate from the heap. Release resources via
 *          @c free_arena() when done. Do not @c free() the returned pointer
 *          directly.
 *
 * @sa init_dynamic_arena(), init_static_arena(), free_arena()
 */
arena_t* init_darena(size_t bytes, bool resize);
#endif

// Stubs to return error values if user calls functions with DYNAMIC MEMORY allocation turned off
#if !ARENA_ENABLE_DYNAMIC
Arena* init_dynamic_arena(size_t, bool, size_t, size_t) { errno = ENOTSUP; return NULL; }
Arena* init_darena(size_t, bool) { errno = ENOTSUP; return NULL; }
void toggle_arena_resize(Arena*, bool) { errno = ENOTSUP; }
#endif

// -------------------------------------------------------------------------------- 

/**
 * @brief Convenience initializer for a STATIC (non-growing) arena with a safe default alignment.
 *
 * This helper calls:
 *
 *   init_static_arena(buffer,
 *                     bytes,
 *                     alignof(max_align_t));
 *
 * Use it when you want an in-place, non-heap (STATIC) arena built inside a caller-supplied
 * buffer, with the data region aligned to at least @c alignof(max_align_t). If you require
 * a different base alignment, call @c init_static_arena() directly.
 *
 * @param buffer  Caller-supplied backing store where the arena header, first chunk header,
 *                and the data region will be constructed in place. Must stay valid for
 *                the entire lifetime of the arena.
 * @param bytes   Total size in bytes of @p buffer.
 *
 * @return Pointer to the in-place @c arena_t on success; @c NULL on failure with @c errno set
 *         by @c init_static_arena().
 *
 * @retval NULL, errno=EINVAL  Propagated if @p buffer is NULL; or @p bytes cannot fit the
 *                             arena header + chunk header + at least 1 byte of data after
 *                             required alignments; or if normalization detects an invalid range.
 *
 * @note  The arena is created in STATIC mode (no growth). Calls to @c alloc_arena() that
 *        exceed remaining capacity will fail with @c errno = EPERM.
 *
 * @note  Ownership: the arena does **not** own @p buffer. Do not expect @c free_arena()
 *        to free @p buffer in STATIC mode. The caller is responsible for releasing @p buffer
 *        after the arena is no longer used.
 *
 * @sa    init_static_arena(), alloc_arena(), reset_arena(), arena_remaining()
 *
 * @par Example: Typical usage with caller-managed storage
 * @code
 * enum { BUF = 16 * 1024 };
 * void *buf = aligned_alloc(alignof(max_align_t), BUF);
 * assert(buf != NULL);
 *
 * arena_t *a = init_sarena(buf, BUF);
 * assert(a != NULL);
 *
 * void *p = alloc_arena(a, 1024, false);
 * assert(p != NULL);
 *
 * // Reset discards allocations but keeps the same capacity.
 * reset_arena(a, false);
 *
 * // Tear-down: STATIC arena does not free 'buf'.
 * // free_arena(a) should not free 'buf' in STATIC mode.
 * free(buf);
 * @endcode
 *
 * @par Example: If you need a different base alignment
 * @code
 * // Prefer an explicit 64-byte base alignment for the data region:
 * arena_t *a = init_static_arena(buf, BUF, 64);
 * @endcode
 */
arena_t* init_sarena(void* buffer, size_t bytes);
// -------------------------------------------------------------------------------- 

/**
 * @brief Destroy a dynamically allocated arena and free all of its heap memory.
 *
 * This function releases the entire allocation associated with a **dynamic**
 * arena created by @c init_dynamic_arena() / @c init_darena(). It walks and
 * frees any growth chunks after the head, then frees the base block that
 * contains the @c arena_t header (and the head chunk). After this call,
 * all pointers previously returned by @c alloc_arena() become invalid.
 *
 * @param arena  Pointer to a dynamic arena instance to destroy.
 *
 * @return void  (errors are reported via @c errno and early-return)
 *
 * @retval (no return), errno=EINVAL
 *      if @p arena is @c NULL.
 * @retval (no return), errno=EPERM
 *      if @p arena was created as @c STATIC. Static arenas are built inside
 *      caller-owned buffers and must not be freed here.
 *
 * **ERRORS:**
 * Sets @c errno to EINVAL or EPERM as described above.
 *
 * @note   For **STATIC** arenas (from @c init_static_arena()/@c init_sarena()),
 *         the arena header lives inside the caller’s buffer; freeing it here
 *         would be invalid. In STATIC mode the caller manages the buffer’s
 *         lifetime separately and may optionally provide a no-op teardown.
 *
 * @pre  @p arena was allocated by @c init_dynamic_arena() / @c init_darena().
 * @pre  @p arena->mem_type == @c DYNAMIC.
 * @post All heap memory owned by the arena is released; @p arena itself is freed.
 *
 * @warning This function is **not** idempotent. Calling @c free_arena()
 *          twice on the same pointer is undefined behavior (typically a
 *          double-free). After calling, set your variable to @c NULL or
 *          use a helper like @c dispose_arena(&arena) to null the caller’s
 *          pointer.
 *
 * @par Ownership model
 * - Dynamic: a single @c free(arena) releases the base block (arena header +
 *   head chunk). Each growth chunk is a single-malloc block freed with
 *   @c free(chunk_header). Internal pointers such as @c chunk->chunk must
 *   never be freed directly.
 * - Static: @c free_arena() must not free the caller’s buffer; it returns
 *   with @c errno = EPERM.
 *
 * @par Example (dynamic arena)
 * @code
 * arena_t *a = init_darena(4096, true);
 * void *p1 = alloc_arena(a, 1024, false);
 * // ... use p1 ...
 * free_arena(a);   // releases all chunks and 'a' itself
 * a = NULL;        // prevent accidental reuse
 * @endcode
 *
 * @par Example (static arena)
 * @code
 * enum { BUF = 8192 };
 * void *buf = aligned_alloc(alignof(max_align_t), BUF);
 * arena_t *a = init_sarena(buf, BUF);
 *
 * // Later:
 * free_arena(a);   // ERROR: sets errno=EPERM (static arenas are caller-owned)
 * // Correct teardown:
 * // (Optionally) reset_arena(a, false);
 * free(buf);       // caller frees its own buffer
 * @endcode
 *
 * @sa init_dynamic_arena(), init_static_arena(), reset_arena(), alloc_arena()
 */
void free_arena(arena_t* arena);
// ================================================================================ 
// ================================================================================ 
// ALLOCATION FUNCTIONS 

/**
 * @brief Allocate a block from an arena with per-arena base alignment.
 *
 * Performs a bump allocation from the arena’s current tail chunk. The returned
 * pointer is aligned to the arena’s base alignment (see @c arena->alignment).
 * If there is not enough space in the tail:
 * - In **STATIC** arenas or when growth is disabled, the call fails.
 * - In **DYNAMIC** arenas with growth enabled, a new chunk is allocated and
 *   linked as the new tail; the block is then carved from that fresh chunk.
 *
 * @param arena   Arena to allocate from (must be a valid pointer).
 * @param bytes   Requested payload size in bytes (must be > 0).
 * @param zeroed  If @c true, the returned block is zero-initialized via
 *                @c memset; otherwise it is left uninitialized.
 *
 * @return Pointer to an @c arena->alignment-aligned block of @p bytes on success;
 *         @c NULL on failure with @c errno set.
 *
 * @retval NULL, errno=EINVAL
 *      if @p arena is NULL, or @p bytes == 0, or the arena’s @c alignment is
 *      zero/non–power-of-two, or the internal tail state is invalid.
 * @retval NULL, errno=EPERM
 *      if there is insufficient space in the current tail and either
 *      the arena is @c STATIC or growth is disabled (@c arena->resize == 0).
 * @retval NULL, errno=ENOMEM
 *      (dynamic builds only) if growth is allowed but allocating a new chunk
 *      fails; propagated from the internal growth helper.
 *
 * @note The arena charges both the returned payload and any leading padding
 *       required to reach the alignment. That is, the internal usage counters
 *       (@c tail->len and @c arena->len) increase by
 *       @c pad + @p bytes where @c pad ∈ [0, alignment-1].
 *
 * @note On the first allocation from a freshly grown chunk, no leading padding
 *       is required because the chunk’s data base is created aligned; the
 *       delta in remaining capacity equals exactly @p bytes.
 *
 * @warning The returned pointer must not be freed with @c free(). Memory is
 *          reclaimed only when the entire arena is @c reset_arena() or
 *          destroyed (for dynamic arenas) via @c free_arena().
 *
 * @pre  @p arena points to a properly initialized arena.
 * @pre  @c arena->alignment is a nonzero power-of-two.
 * @post On success:
 *       - The returned pointer is @c arena->alignment-aligned.
 *       - @c arena->cur advances by @p bytes (plus any pad in the prior chunk).
 *       - @c tail->len and @c arena->len are updated to include pad+bytes.
 *
 * @sa   init_static_arena(), init_dynamic_arena(), reset_arena(), free_arena()
 *
 * @par Example: Allocating from a STATIC arena (no growth)
 * @code
 * enum { BUF = 8192 };
 * void *buf = aligned_alloc(alignof(max_align_t), BUF);
 * arena_t *a = init_static_arena(buf, BUF, alignof(max_align_t));
 *
 * void *p1 = alloc_arena(a, 256, true);   // ok, zeroed
 * // ... consume capacity ...
 * void *p2 = alloc_arena(a, BUF, false);  // likely fails: errno == EPERM
 *
 * // Caller remains owner of 'buf'; free_arena(a) must not free it.
 * free(buf);
 * @endcode
 *
 * @par Example: Allocating from a DYNAMIC arena (growth allowed)
 * @code
 * arena_t *a = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
 *
 * // Fill current chunk, then one more byte to trigger growth:
 * (void)alloc_arena(a, 4095, false);
 * void *p = alloc_arena(a, 2, false);     // grows via new chunk, then returns aligned block
 * assert(p != NULL);
 *
 * free_arena(a); // releases all chunks and the arena header block
 * @endcode
 */
void* alloc_arena(arena_t* arena, size_t bytes, bool zeroed);
// ================================================================================ 
// ================================================================================ 
// UTILITY FUNCTIONS 

bool is_arena_ptr(const arena_t* arena, const void *ptr);
// -------------------------------------------------------------------------------- 

bool is_arena_ptr_sized(const arena_t* arena, const void* ptr, size_t size);
// -------------------------------------------------------------------------------- 

void reset_arena(arena_t* arena, bool trim_extra_chunks);
// -------------------------------------------------------------------------------- 

ArenaCheckPoint save_arena(const arena_t* arena);
// -------------------------------------------------------------------------------- 

bool restore_arena(arena_t *arena, ArenaCheckPoint cp);
// ================================================================================ 
// ================================================================================ 
// GETTER FUNCTIONS 

size_t arena_remaining(const arena_t* arena);
// -------------------------------------------------------------------------------- 

size_t arena_chunk_count(const arena_t* arena);
// -------------------------------------------------------------------------------- 

alloc_t arena_mtype(const arena_t* arena);
// -------------------------------------------------------------------------------- 

size_t arena_size(const arena_t* arena);
// -------------------------------------------------------------------------------- 

size_t arena_alloc(const arena_t* arena);
// -------------------------------------------------------------------------------- 

size_t total_arena_alloc(const arena_t* arena);
// -------------------------------------------------------------------------------- 

size_t arena_alignment(const arena_t* arena);
// -------------------------------------------------------------------------------- 

size_t arena_min_chunk_size(const arena_t* arena);
// ================================================================================ 
// ================================================================================ 
// SETTER FUNCTIONS 

#if ARENA_ENABLE_DYNAMIC
void toggle_arena_resize(arena_t* arena, bool toggle);
#endif
// ================================================================================ 
// ================================================================================ 
// LOG FUNCTIONS 

bool arena_stats(const arena_t* arena, char* buffer, size_t buffer_size);
// ================================================================================ 
// ================================================================================ 
// MACROS 

#if ARENA_USE_CONVENIENCE_MACROS
    #define arena_alloc_type(arena, type) \
        (type*)alloc_arena((arena), sizeof(type), false)
    #define arena_alloc_array(arena, type, count) \
        (type*)alloc_arena((arena), sizeof(type) * (count), false)
    #define arena_alloc_type_zeroed(arena, type) \
        (type*)alloc_arena((arena), sizeof(type), true)
    #define arena_alloc_array_zeroed(arena, type, count) \
        (type*)alloc_arena((arena), sizeof(type) * (count), true)
#endif
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* arena_H */
// ================================================================================
// ================================================================================
// eof
//
// TODO: ADD FUNCTION TO TURN resize on 
// TODO: ADD FUNCTION TO TURN resize off
