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

// Forward declarations for opaque arena_t data type
typedef struct arena_t arena_t;
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
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate a block from an arena with a caller-specified alignment.
 *
 * Performs a bump allocation from the arena’s current tail chunk, returning a
 * pointer aligned to @p alignment. The effective alignment is the maximum of
 * the requested @p alignment and the arena’s base alignment (see
 * @c arena->alignment). If @p alignment is zero, the arena’s base alignment is
 * used. The requested alignment must be a non-zero power-of-two (or zero to
 * request the arena default).
 *
 * If there is insufficient space in the current tail chunk:
 * - In **STATIC** arenas or when growth is disabled, the call fails.
 * - In **DYNAMIC** arenas with growth enabled, a new chunk is allocated with
 *   data naturally aligned to the effective alignment; the block is carved
 *   from that fresh chunk with no leading pad.
 *
 * @param arena      Arena to allocate from (must be a valid pointer).
 * @param bytes      Requested payload size in bytes (must be > 0).
 * @param alignment  Desired alignment in bytes (0 => use arena->alignment).
 *                   Must be a power-of-two if non-zero.
 * @param zeroed     If @c true, the returned block is zero-initialized via
 *                   @c memset; otherwise it is left uninitialized.
 *
 * @return Pointer to an @p alignment-aligned (or better) block of @p bytes on
 *         success; @c NULL on failure with @c errno set.
 *
 * @retval NULL, errno=EINVAL
 *      if @p arena is NULL, or @p bytes == 0, or @p alignment is non-zero and
 *      not a power-of-two, or if the arena’s base alignment is zero or not a
 *      power-of-two, or if the arena tail state is invalid.
 * @retval NULL, errno=EPERM
 *      if there is insufficient space in the current chunk and either the
 *      arena is @c STATIC or growth is disabled (@c arena->resize == 0).
 * @retval NULL, errno=ENOMEM
 *      (dynamic builds only) if growth is allowed but allocating a new chunk
 *      fails; propagated from the internal chunk initialization helper.
 *
 * @note The arena charges both the returned payload and any leading padding
 *       required to reach the alignment. That is, the internal usage counters
 *       (@c tail->len and @c arena->len) increase by @c pad + @p bytes where
 *       @c pad ∈ [0, effective_alignment-1].
 *
 * @note On the first allocation from a freshly grown chunk, no leading padding
 *       is required because the chunk’s data pointer is itself created aligned
 *       to the effective alignment; the delta in remaining capacity equals
 *       exactly @p bytes.
 *
 * @warning The returned pointer must not be freed with @c free(). Memory is
 *          reclaimed only when the entire arena is reset via @c reset_arena()
 *          or destroyed via @c free_arena() (for dynamic arenas).
 *
 * @pre  @p arena points to a properly initialized arena.
 * @pre  @p alignment is zero (use arena default) or a non-zero power-of-two.
 * @post On success:
 *       - The returned pointer is aligned to @c max(alignment, arena->alignment).
 *       - @c arena->cur advances by @p bytes (plus any pad in the prior chunk).
 *       - @c tail->len and @c arena->len are updated to include pad+bytes.
 *
 * @sa   alloc_arena(), init_static_arena(), init_dynamic_arena(),
 *       reset_arena(), free_arena()
 *
 * @par Example: Requesting a stricter alignment than the arena’s default
 * @code
 * arena_t *a = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
 *
 * // Request 64-byte alignment for SIMD data
 * void *p = alloc_arena_aligned(a, 128, 64, false);
 * assert(((uintptr_t)p % 64) == 0);
 *
 * free_arena(a);
 * @endcode
 *
 * @par Example: Falling back to arena base alignment
 * @code
 * arena_t *a = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
 *
 * // Zero alignment means "use arena->alignment"
 * void *p = alloc_arena_aligned(a, 64, 0, true);
 * assert(((uintptr_t)p % alignof(max_align_t)) == 0);
 *
 * free_arena(a);
 * @endcode
 */
void* alloc_arena_aligned(arena_t* arena, size_t bytes, size_t alignment, bool zeroed);
// ================================================================================ 
// ================================================================================ 
// UTILITY FUNCTIONS 

/**
 * @brief Check whether a pointer falls inside the *used* region of any chunk in an arena.
 *
 * This helper answers: “Does @p ptr point into bytes that have been *allocated* from the
 * arena (i.e., within the currently used portion of some chunk)?” It first checks the
 * tail chunk (fast path) and then walks the remaining chunks. For each chunk it treats
 * the valid range as:
 *
 *     [ chunk->chunk , chunk->chunk + chunk->len )
 *
 * so only memory that has been handed out (including any leading pad charged to the
 * chunk) is considered “in-arena.” Memory in the *unused* tail of a chunk is not.
 *
 * @param arena  Arena to query (must be a valid, initialized arena).
 * @param ptr    Pointer to test (may be any address).
 *
 * @return @c true if @p ptr lies within the used region of any chunk, otherwise @c false.
 *
 * @note This is a *geometric* test against the arena’s current accounting (len/alloc).
 *       It cannot tell whether @p ptr actually points to the start of a specific
 *       allocation—only that it falls somewhere inside the used span of a chunk.
 *
 * @note Defensive clamping is applied when a chunk appears corrupted
 *       (e.g., @c len > @c alloc). In that case the check uses @c min(len, alloc).
 *
 * @note Overflow guards are used on address arithmetic (e.g., end >= start) and
 *       the function returns @c false if a suspicious range is detected.
 *
 * @warning Not thread-safe against concurrent mutation of the arena without external
 *          synchronization. If another thread grows/resets the arena while this
 *          function runs, results are undefined.
 *
 * @par Complexity
 * Average O(1) for the tail fast-path, O(N) worst-case across N chunks.
 *
 * @par Example
 * @code
 * arena_t *a = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
 * void *p = alloc_arena(a, 128, false);
 * assert_true(is_arena_ptr(a, p));          // inside used region
 * uint8_t *q = (uint8_t*)p + 127;
 * assert_true(is_arena_ptr(a, q));          // still inside the same used span
 * uint8_t *r = (uint8_t*)p + 128;
 * assert_false(is_arena_ptr(a, r));         // exactly 1 past -> outside used span
 * free_arena(a);
 * @endcode
 */
bool is_arena_ptr(const arena_t* arena, const void *ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Check whether a [ptr, ptr+size) range lies fully inside the *used* region of the arena.
 *
 * This is a stricter variant of @c is_arena_ptr: it verifies that the entire half-open
 * interval @c [ptr, ptr+size) is contained within the used portion of exactly one chunk.
 * It returns @c false if the span crosses a chunk boundary or extends beyond the used
 * length of a chunk.
 *
 * @param arena  Arena to query (must be a valid, initialized arena).
 * @param ptr    Start of the span to test.
 * @param size   Length in bytes of the span (must be > 0).
 *
 * @return @c true if the full span @c [ptr, ptr+size) is inside a single chunk’s used region,
 *         otherwise @c false.
 *
 * @retval false  if @p arena is NULL, @p ptr is NULL, @p size == 0, or an overflow in
 *                @c ptr + size is detected.
 *
 * @note The test uses per-chunk used lengths (like @c is_arena_ptr) and clamps
 *       @c used to @c min(len, alloc) for robustness when corruption is suspected.
 *
 * @note Cross-chunk spans return @c false even if each byte is “in-arena” individually.
 *       The function requires containment within a *single* chunk’s used interval.
 *
 * @warning Not thread-safe against concurrent mutation (growth/reset) without external
 *          synchronization.
 *
 * @par Complexity
 * Average O(1) for the tail fast-path, O(N) worst-case across N chunks.
 *
 * @par Examples
 * @code
 * arena_t *a = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
 * uint8_t *p = (uint8_t*)alloc_arena(a, 128, false);
 *
 * // Fully contained span:
 * assert_true(is_arena_ptr_sized(a, p + 64, 32));      // [p+64, p+96) is within p’s block
 *
 * // One-past-the-end is not contained:
 * assert_false(is_arena_ptr_sized(a, p + 120, 9));     // would extend beyond used region
 *
 * // Force a new chunk, then ask for a cross-chunk span:
 * size_t rem = arena_remaining(a);
 * (void)alloc_arena(a, rem, false);                    // exhaust tail -> next alloc grows
 * uint8_t *q = (uint8_t*)alloc_arena(a, 64, false);    // q is in the new chunk
 *
 * // Span starting near end of first block that would cross into the gap/new chunk:
 * assert_false(is_arena_ptr_sized(a, p + 127, 2));
 *
 * free_arena(a);
 * @endcode
 */
bool is_arena_ptr_sized(const arena_t* arena, const void* ptr, size_t size);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reset an arena’s usage counters, with an option to trim extra chunks.
 *
 * Rewinds the arena to an “empty” state by zeroing the used-length of every
 * chunk and resetting global usage counters. Two modes are supported:
 *
 * - **Non-trimming reset** (@p trim_extra_chunks == false): all existing chunks
 *   are retained; capacity and @c tot_alloc are unchanged. The cursor @c cur is
 *   moved to the start of the current @c tail (or @c head if @c tail is NULL).
 *
 * - **Trimming reset** (@p trim_extra_chunks == true, dynamic arenas only):
 *   frees every growth chunk after the head (each was a single @c malloc),
 *   detaches the list to a single head chunk, and resets the cursor to the
 *   head’s data base. Usable capacity becomes exactly the head’s @c alloc.
 *   @c tot_alloc is reduced to reflect the removal of those extra chunks.
 *
 * @param arena             Arena to reset (must be a valid pointer).
 * @param trim_extra_chunks If @c true and the arena is dynamic, free all chunks
 *                          after the head; if @c false, keep the current chunk
 *                          chain and only clear usage.
 *
 * @return void (errors are reported via @c errno and early-return)
 *
 * @retval (no return), errno=EINVAL
 *      if @p arena is @c NULL.
 *
 * @note STATIC arenas ignore the trim path: their chunk chain is caller-owned
 *       storage; this function will only clear usage and preserve capacity.
 *
 * @note Accounting details when trimming (dynamic only):
 *       - Each growth chunk contributed
 *         @c align_up(sizeof(Chunk), alignment) + chunk->alloc
 *         to @c tot_alloc at link time; this function subtracts the same
 *         contribution before freeing the chunk header.
 *       - After trimming, @c alloc == head->alloc and @c tot_alloc reflects
 *         the initial base allocation footprint.
 *
 * @warning Do not call this concurrently with other operations on the same
 *          arena without external synchronization.
 *
 * @par Complexity
 * O(N) over the number of chunks; when trimming, freeing is also O(N_tail).
 *
 * @par Examples
 * @code
 * // Non-trimming reset (keeps capacity and all chunks):
 * arena_t *a = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
 * (void)alloc_arena(a, 1024, false);
 * reset_arena(a, false);  // capacity retained
 *
 * // Trimming reset (drop extra chunks, return to a single head):
 * // Force growth:
 * size_t rem = arena_remaining(a);
 * (void)alloc_arena(a, rem, false);             // exhaust head
 * (void)alloc_arena(a, 64,  false);             // creates a new tail chunk
 * reset_arena(a, true);   // frees tail(s), cursor at head
 *
 * free_arena(a); // dynamic arena teardown
 * @endcode
 */
void reset_arena(arena_t* arena, bool trim_extra_chunks);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a lightweight checkpoint of the arena’s current cursor/tail.
 *
 * Captures just enough information to later rewind the arena to the same “point
 * in time” without copying any payload data. The checkpoint is an opaque,
 * fixed-size public struct (ABI-stable) that internally records:
 *
 * - the current tail chunk pointer,
 * - the current cursor within that tail,
 * - the arena’s total used bytes at save time (for diagnostics).
 *
 * The checkpoint remains valid only as long as the recorded tail chunk continues
 * to exist in the arena’s chunk chain. Operations that *remove* or *free* that
 * tail (e.g., trimming resets) may invalidate previously saved checkpoints.
 *
 * @param arena  Arena whose state to capture. If @c NULL, an "empty" checkpoint
 *               is returned that @c restore_arena() will treat as a no-op.
 *
 * @return Opaque @c ArenaCheckPoint value which can be passed to
 *         @c restore_arena() to rewind to this position.
 *
 * @note Checkpoints do not pin memory. They do not keep chunks alive; they
 *       merely store addresses and counters for later validation/rewind.
 *
 * @warning This function is not synchronization-safe: if other threads grow,
 *          reset, or free the arena while the checkpoint is being saved or
 *          later restored, behavior is undefined.
 *
 * @sa restore_arena(), reset_arena(), alloc_arena()
 */
ArenaCheckPoint save_arena(const arena_t* arena);
// -------------------------------------------------------------------------------- 

/**
 * @brief Rewind an arena to a previously saved checkpoint.
 *
 * Attempts to restore the arena’s cursor and tail to the state captured by
 * @c save_arena(). Validation ensures the checkpoint’s chunk still exists in
 * the current chain and that its saved cursor lies within that chunk’s capacity.
 *
 * Behavior differs by arena type:
 * - **DYNAMIC**: all chunks *after* the checkpoint chunk are freed (each was a
 *   single-malloc “growth” block), and the list is relinked so the checkpoint
 *   chunk becomes the tail. Accounting fields (@c len, @c alloc, @c tot_alloc)
 *   are recomputed for the remaining chain.
 * - **STATIC**: no chunks are freed (storage is caller-owned). The cursor and
 *   tail are rewound; accounting is recomputed. If the checkpoint referred to a
 *   chunk that no longer exists, the restore fails.
 *
 * @param arena  Arena to rewind (must be a valid pointer).
 * @param cp     Checkpoint previously returned by @c save_arena().
 *
 * @return @c true on success; @c false on failure with @c errno set.
 *
 * @retval true
 *      if the checkpoint is empty (no chunk recorded) — treated as a no-op.
 *
 * @retval false, errno=EINVAL
 *      if @p arena is @c NULL, or the checkpoint’s chunk is not found in the
 *      current chain (stale checkpoint), or the checkpoint cursor is out of
 *      bounds for that chunk, or the arena alignment is invalid.
 *
 * @note After a successful restore, any pointers returned by @c alloc_arena()
 *       *after* the checkpoint position become invalid (dynamic arenas will
 *       have freed those growth chunks; static arenas will consider their
 *       contents “unused” again).
 *
 * @note @c tot_alloc is recomputed based on the remaining chain using the
 *       convention: @c align_up(sizeof(arena_t), alignment) +
 *       sum( align_up(sizeof(Chunk), alignment) + chunk->alloc ).
 *
 * @warning Not thread-safe; external synchronization is required if other
 *          threads interact with the arena concurrently.
 *
 * @sa save_arena(), reset_arena(), alloc_arena(), free_arena()
 */
bool restore_arena(arena_t *arena, ArenaCheckPoint cp);
// ================================================================================ 
// ================================================================================ 
// GETTER FUNCTIONS 

/**
 * @brief Return *immediately usable* bytes remaining in the current tail chunk.
 *
 * Computes the free space only in the arena’s @b tail chunk as:
 *
 *     tail->alloc - min(tail->len, tail->alloc)
 *
 * If the arena is NULL, has no tail, or the tail has no data base, returns 0.
 *
 * @param arena  Arena to query (may be NULL).
 *
 * @return Number of bytes that can be satisfied from the current tail chunk
 *         without growth; 0 if none or on invalid input.
 *
 * @note This is @b not the total free space across all chunks; it reports
 *       only the space left in the @e current tail. If you need additional
 *       capacity (dynamic arenas), @c alloc_arena() may grow the arena.
 *
 * @note The value ignores any per-allocation padding that might be required to
 *       satisfy the arena’s base alignment. A subsequent allocation of @c n
 *       bytes may require up to @c (alignment-1) extra pad, so an allocation
 *       of size @c n succeeds from the tail iff @c arena_remaining() >= pad + n.
 *
 * @par Example
 * @code
 * size_t r = arena_remaining(a);
 * if (r >= needed + pad_up((uintptr_t)arena_cursor(a), arena_alignment(a))) {
 *     // fits in current tail without growth
 * }
 * @endcode
 */
size_t arena_remaining(const arena_t* arena);
// -------------------------------------------------------------------------------- 

/**
 * @brief Count the number of chunks currently linked in the arena.
 *
 * Walks the chunk list from @c head to @c NULL and returns the number of
 * chunk headers encountered.
 *
 * @param arena  Arena to query (may be NULL).
 *
 * @return The number of chunks in the arena; 0 if @p arena is NULL.
 *
 * @note For a freshly initialized arena there is always at least one chunk
 *       (the head). After @c reset_arena(..., trim=true) on a dynamic arena,
 *       the count returns to 1.
 *
 * @par Complexity
 * O(N) where N is the number of chunks.
 */
size_t arena_chunk_count(const arena_t* arena);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the arena’s memory type as an @c alloc_t enum.
 *
 * Converts the arena’s internal 8-bit storage to the public @c alloc_t
 * (e.g., @c STATIC or @c DYNAMIC).
 *
 * @param arena  Arena to query.
 *
 * @return The @c alloc_t value for this arena. If @p arena is NULL, sets
 *         @c errno to @c EINVAL and returns @c ALLOC_INVALID.
 *
 * @retval STATIC   The arena uses caller-supplied storage and cannot grow.
 * @retval DYNAMIC  The arena was dynamically allocated and may grow (subject
 *                  to @c resize flag).
 * @retval ALLOC_INVALID  On NULL input or corrupted state.
 *
 * @note The return value reflects configuration time. Whether a dynamic arena
 *       will actually grow also depends on @c arena->resize (see
 *       @c toggle_arena_resize when available).
 */
alloc_t arena_mtype(const arena_t* arena);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the total bytes currently *consumed* from the arena.
 *
 * This is the logical usage counter aggregated across all chunks. It includes
 * any per-allocation padding that was charged to satisfy alignment, so it may
 * be larger than the sum of the user-requested sizes.
 *
 * @param arena  Arena to query (may be NULL).
 *
 * @return Total used bytes (including pad) across the arena; 0 if @p arena is NULL.
 *
 * @note This value never exceeds @c arena_alloc(arena).
 */
size_t arena_size(const arena_t* arena);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the total *usable capacity* (in bytes) across all chunks.
 *
 * This is the sum of @c alloc fields for every chunk currently linked in the
 * arena. It does not include header or padding bytes and may increase when the
 * arena grows (dynamic) or decrease when trimmed.
 *
 * @param arena  Arena to query (may be NULL).
 *
 * @return Total usable capacity; 0 if @p arena is NULL.
 *
 * @see total_arena_alloc() for capacity including metadata overhead.
 */
size_t arena_alloc(const arena_t* arena);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the arena’s total footprint, including metadata overhead.
 *
 * For dynamic arenas, this approximates:
 *
 *   align_up(sizeof(arena_t), alignment)
 *   + Σ over chunks { align_up(sizeof(Chunk), alignment) + chunk->alloc }
 *
 * For static arenas initialized over a caller buffer, this typically equals the
 * full buffer size provided at initialization.
 *
 * @param arena  Arena to query (may be NULL).
 *
 * @return Total bytes attributed to the arena (data + headers + alignment
 *         padding as accounted by the implementation); 0 if @p arena is NULL.
 *
 * @note This is an accounting value; it is not necessarily the exact number of
 *       bytes returned by underlying allocation calls in all configurations.
 */
size_t total_arena_alloc(const arena_t* arena);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the arena’s base alignment policy in bytes.
 *
 * The base alignment is applied to the start of the data region in each chunk,
 * and to each allocation’s placement within a chunk (for the default allocator).
 *
 * @param arena  Arena to query.
 *
 * @return The power-of-two alignment (≥ alignof(max_align_t)) used by this arena.
 *         Returns 0 and sets @c errno=EINVAL if @p arena is NULL or uninitialized.
 *
 * @note A nonzero return is guaranteed to be a power-of-two by construction.
 */
size_t arena_alignment(const arena_t* arena);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the arena’s minimum growth chunk size (bytes).
 *
 * For dynamic arenas, this value (if nonzero) is used as a floor when
 * computing the size of new growth chunks. For static arenas, this is 0.
 *
 * @param arena  Arena to query.
 *
 * @return The minimum growth size in bytes; 0 for static arenas or on error.
 *         Returns 0 and sets @c errno=EINVAL if @p arena is NULL or uninitialized.
 */
size_t arena_min_chunk_size(const arena_t* arena);
// ================================================================================ 
// ================================================================================ 
// SETTER FUNCTIONS 

#if ARENA_ENABLE_DYNAMIC
/**
 * @brief Enable or disable geometric growth for a dynamic arena at runtime.
 *
 * Controls whether @c alloc_arena() is allowed to allocate new “growth” chunks
 * when the current tail chunk has insufficient free space.
 *
 * - When @p toggle is @c true, subsequent allocations that do not fit in the
 *   current tail may grow the arena (subject to other limits such as
 *   @c min_chunk).
 * - When @p toggle is @c false, allocations that do not fit in the current tail
 *   will fail with @c errno=EPERM (no growth); the arena remains usable for
 *   allocations that do fit in the existing capacity.
 *
 * @param arena   Arena to modify. Must be a valid pointer to a @b dynamic arena.
 * @param toggle  @c true to allow growth; @c false to forbid growth.
 *
 * @return void  (errors reported via @c errno and early return)
 *
 * @retval (no return), errno=EPERM
 *         if @p arena is a STATIC arena (growth is never permitted for static).
 *
 * @note This function does not shrink or free existing chunks. To drop extra
 *       chunks and return to a single head chunk, use @c reset_arena(arena, true).
 *
 * @note Has no effect on STATIC arenas; calling it on a STATIC arena sets
 *       @c errno=EPERM and returns without modifying the arena.
 *
 * @warning Not thread-safe; coordinate with other threads that allocate from or
 *          modify the same arena.
 *
 * @par Example
 * @code
 * arena_t *a = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
 * // Temporarily freeze growth for a phase that must not allocate extra memory:
 * toggle_arena_resize(a, false);
 * void *p = alloc_arena(a, 8192, false); // will fail with EPERM if it doesn't fit
 * // Re-enable growth:
 * toggle_arena_resize(a, true);
 * @endcode
 */
void toggle_arena_resize(arena_t* arena, bool toggle);
#endif
// ================================================================================ 
// ================================================================================ 
// LOG FUNCTIONS 

/**
 * @brief Render a human-readable snapshot of arena state into a caller buffer.
 *
 * Writes a multi-line textual report describing the arena’s configuration and
 * current usage. The format is stable enough for logs and diagnostics, e.g.:
 *
 * @code
 * Arena Statistics:
 *   Type: STATIC
 *   Used: 1024 bytes
 *   Capacity: 4096 bytes
 *   Total (with overhead): 8192 bytes
 *   Utilization: 25.0%
 *   Chunk 1: 1024/4096 bytes
 *   Chunk 2: 0/2048 bytes
 * @endcode
 *
 * If @p arena is NULL, the function writes: "Arena: NULL\n" and returns @c true.
 *
 * The function guarantees no truncation: it uses a bounded formatter and
 * returns @c false if the output would exceed @p buffer_size (leaving @c errno
 * set). On success, the buffer is NUL-terminated.
 *
 * @param arena         Arena to describe. May be NULL (prints "Arena: NULL\n").
 * @param buffer        Destination character buffer for the report. Must not be NULL.
 * @param buffer_size   Size in bytes of @p buffer, including space for the
 *                      terminating NUL. Must be > 0.
 *
 * @return @c true on success (fully written, NUL-terminated);
 *         @c false on error with @c errno set.
 *
 * @retval false, errno=EINVAL
 *         if @p buffer is NULL, or @p buffer_size is 0.
 *
 * @retval false, errno=EINVAL
 *         if an internal formatting error is detected.
 *
 * @retval false, errno=ERANGE
 *         if there is not enough space in @p buffer for the full report (the
 *         function avoids partial/truncated output).
 *
 * @note The report includes:
 *       - Type: "STATIC" or "DYNAMIC" (derived from @c arena->mem_type).
 *       - Used: @c arena->len (bytes consumed, including per-allocation padding).
 *       - Capacity: @c arena->alloc (sum of usable data bytes across chunks).
 *       - Total (with overhead): @c arena->tot_alloc (implementation accounting of
 *         headers + alignment + data).
 *       - Utilization: 100*Used/Capacity, or "N/A" if capacity is zero.
 *       - One line per chunk: "Chunk i: len/alloc bytes".
 *
 * @note Thread-safety: the function does not lock the arena; if other threads
 *       mutate the arena concurrently, the snapshot may be transient or
 *       inconsistent. Coordinate externally if needed.
 *
 * @par Example
 * @code
 * char buf[512];
 * if (!arena_stats(a, buf, sizeof buf)) {
 *     perror("arena_stats");
 * } else {
 *     fputs(buf, stdout);
 * }
 * @endcode
 */
bool arena_stats(const arena_t* arena, char* buffer, size_t buffer_size);
// ================================================================================ 
// ================================================================================ 
// MACROS 

#if ARENA_USE_CONVENIENCE_MACROS
/**
 * @def arena_alloc_type(arena, type)
 * @brief Allocate one object of @p type from @p arena (uninitialized).
 *
 * Expands to:
 * @code
 * (type*)alloc_arena((arena), sizeof(type), false)
 * @endcode
 *
 * The returned pointer is aligned to the arena’s base alignment
 * (see ::arena_alignment()) and points to uninitialized storage of size
 * @c sizeof(type).
 *
 * @param arena  An ::arena_t* previously created with ::init_static_arena()
 *               or ::init_dynamic_arena().
 * @param type   The complete object type to allocate (e.g., @c int, @c struct Foo).
 *
 * @return @c type* on success; @c NULL on failure with @c errno set by
 *         ::alloc_arena() (e.g., @c EINVAL, @c EPERM, @c ENOMEM).
 *
 * @note This macro only wraps ::alloc_arena(); no additional behavior is added.
 * @note Do not pass an incomplete type.
 *
 * @par Example
 * @code
 * typedef struct { float x, y, z; } vec3;
 * arena_t* a = init_darena(4096, true);
 * vec3* p = arena_alloc_type(a, vec3);
 * if (!p) { perror("arena_alloc_type"); }
 * // ...
 * free_arena(a);
 * @endcode
 */
    #define arena_alloc_type(arena, type) \
        (type*)alloc_arena((arena), sizeof(type), false)

/**
 * @def arena_alloc_array(arena, type, count)
 * @brief Allocate an array of @p count objects of @p type from @p arena (uninitialized).
 *
 * Expands to:
 * @code
 * (type*)alloc_arena((arena), sizeof(type) * (count), false)
 * @endcode
 *
 * The returned pointer is aligned to the arena’s base alignment and provides
 * uninitialized storage for exactly @p count elements of @p type in a single
 * contiguous block.
 *
 * @param arena  An ::arena_t* previously created with ::init_static_arena()
 *               or ::init_dynamic_arena().
 * @param type   The element type (must be complete).
 * @param count  Number of elements to allocate. Evaluated exactly once.
 *
 * @return @c type* on success; @c NULL on failure with @c errno set by ::alloc_arena().
 *
 * @warning Very large @p count values can overflow the size computation
 *          @c sizeof(type) * (count), causing ::alloc_arena() to fail.
 * @warning Avoid side effects in @p count (e.g., do not pass @c i++).
 *
 * @par Example
 * @code
 * arena_t* a = init_darena(4096, true);
 * double* samples = arena_alloc_array(a, double, 1024);
 * if (!samples) { perror("arena_alloc_array"); }
 * // ...
 * free_arena(a);
 * @endcode
 */
    #define arena_alloc_array(arena, type, count) \
        (type*)alloc_arena((arena), sizeof(type) * (count), false)

/**
 * @def arena_alloc_type_zeroed(arena, type)
 * @brief Allocate one object of @p type from @p arena and zero-initialize it.
 *
 * Expands to:
 * @code
 * (type*)alloc_arena((arena), sizeof(type), true)
 * @endcode
 *
 * The returned pointer is aligned to the arena’s base alignment and the
 * @c sizeof(type) bytes are set to zero.
 *
 * @param arena  An ::arena_t* previously created with ::init_static_arena()
 *               or ::init_dynamic_arena().
 * @param type   The complete object type to allocate and zero.
 *
 * @return @c type* on success; @c NULL on failure with @c errno set by ::alloc_arena().
 *
 * @note Zero-initialization uses @c memset(ptr, 0, sizeof(type)).
 *
 * @par Example
 * @code
 * typedef struct { int id; char name[32]; } user;
 * arena_t* a = init_darena(4096, true);
 * user* u = arena_alloc_type_zeroed(a, user);  // all fields = 0
 * if (!u) { perror("arena_alloc_type_zeroed"); }
 * // ...
 * free_arena(a);
 * @endcode
 */
    #define arena_alloc_type_zeroed(arena, type) \
        (type*)alloc_arena((arena), sizeof(type), true)

/**
 * @def arena_alloc_array_zeroed(arena, type, count)
 * @brief Allocate an array of @p count objects of @p type and zero-initialize the block.
 *
 * Expands to:
 * @code
 * (type*)alloc_arena((arena), sizeof(type) * (count), true)
 * @endcode
 *
 * The returned pointer is aligned to the arena’s base alignment and the entire
 * allocation ( @c sizeof(type) * @p count bytes ) is set to zero.
 *
 * @param arena  An ::arena_t* previously created with ::init_static_arena()
 *               or ::init_dynamic_arena().
 * @param type   Element type (must be complete).
 * @param count  Number of elements to allocate. Evaluated exactly once.
 *
 * @return @c type* on success; @c NULL on failure with @c errno set by ::alloc_arena().
 *
 * @warning Very large @p count values can overflow the size computation.
 * @warning Avoid side effects in @p count (e.g., do not pass @c i++).
 *
 * @par Example
 * @code
 * arena_t* a = init_darena(8192, true);
 * uint8_t* buf = arena_alloc_array_zeroed(a, uint8_t, 512); // 512 zeroed bytes
 * if (!buf) { perror("arena_alloc_array_zeroed"); }
 * // ...
 * free_arena(a);
 * @endcode
 */
    #define arena_alloc_array_zeroed(arena, type, count) \
        (type*)alloc_arena((arena), sizeof(type) * (count), true)
#endif
// ================================================================================ 
// ================================================================================ 
// POOL ALLOCATOR 

// Forward declarations for opaque pool_t data type
typedef struct pool_t pool_t;
// ================================================================================ 
// ================================================================================ 
// INITIALIZE AND DEALLOCATE FUNCTIONS

/**
 * @brief Initialize a fixed-size memory pool backed by an existing arena.
 *
 * Creates a pool allocator that dispenses fixed-size blocks from @p arena.
 * The pool performs bump allocation from arena slices, optionally reuses freed
 * blocks via an intrusive free list, and can operate in fixed-capacity or 
 * grow-on-demand mode.
 *
 * The pool header itself is allocated from the provided arena; there is no
 * external @c malloc.
 *
 * @param arena             Existing arena to supply memory (must be valid).
 * @param block_size        Requested user payload size in bytes (> 0).
 * @param alignment         Desired block alignment (0 => arena default).
 *                          Must be a power-of-two if nonzero. The effective
 *                          alignment is @c max(alignment, alignof(void*)),
 *                          ensuring freed blocks can store a next-pointer.
 * @param blocks_per_chunk  Number of blocks to allocate per arena slice.
 *                          Must be > 0.
 * @param prewarm_one_chunk If @c true, the pool eagerly acquires one slice
 *                          during initialization so the first allocation is O(1).
 * @param grow_enabled      If @c true, the pool may request additional slices
 *                          from the arena when exhausted; otherwise the pool
 *                          has fixed capacity and further allocations fail once
 *                          that capacity is consumed.
 *
 * @return
 *   Pointer to a newly initialized pool object on success,
 *   or @c NULL with @c errno set on failure.
 *
 * @retval NULL, errno=EINVAL
 *      If @p arena is NULL, @p block_size == 0, @p blocks_per_chunk == 0,
 *      or @p alignment is non-power-of-two.
 * @retval NULL, errno=ENOMEM
 *      If the pool header could not be allocated from @p arena.
 * @retval NULL, errno=EPERM
 *      If @p prewarm_one_chunk or a later grow request is attempted but the
 *      arena cannot grow (STATIC arena or resize disabled).
 *
 * @note Freed blocks are returned to an intrusive free list and reused in LIFO
 *       order. The caller must only return blocks obtained from this pool.
 *
 * @note Memory is reclaimed only when the underlying arena is reset or destroyed.
 *       Individual blocks must not be freed with @c free().
 *
 * @pre  @p arena is initialized via @c init_static_arena or @c init_dynamic_arena.
 * @post On success:
 *       - The pool header resides inside @p arena.
 *       - If @p prewarm_one_chunk is true, @c pool_total_blocks() equals
 *         @p blocks_per_chunk.
 *
 * @sa alloc_pool(), return_pool_element(), reset_pool(), free_arena()
 *
 * @par Example: Fixed-capacity pool inside a static arena
 * @code{.c}
 * enum { BUF = 64 * 1024 };  // 64 KiB backing buffer
 * void *buf = aligned_alloc(alignof(max_align_t), BUF);
 * arena_t *a = init_static_arena(buf, BUF, alignof(max_align_t));
 *
 * // Each block holds 64 bytes, 128 blocks per slice, no arena growth allowed
 * pool_t *p = init_pool_with_arena(a,
 *                                  64,
 *                                  0,
 *                                  128,
 *                                  true,
 *                                  false);
 * assert(p);
 *
 * void *x = alloc_pool(p);        // ok
 * void *y = alloc_pool(p);        // ok
 * return_pool_element(p, x);      // block reclaimed for reuse
 *
 * // After 128 successful allocs, subsequent calls fail with errno=EPERM:
 * for (int i = 0; i < 128; ++i) assert(alloc_pool(p));
 * assert(!alloc_pool(p) && errno == EPERM);
 *
 * reset_pool(p);   // pool bookkeeping reset (arena storage not reclaimed here)
 * free(buf);       // static arena buffer was caller-owned
 * @endcode
 *
 * @par Example: Grow-on-demand pool inside a dynamic arena
 * @code
 * arena_t *a = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
 *
 * pool_t *p = init_pool_with_arena(a,
 *                                  32,
 *                                  16,
 *                                  64,
 *                                  false,
 *                                  true);
 *
 * void *b = alloc_pool(p);   // first call triggers one chunk allocation
 * return_pool_element(p, b); // returned to free list
 *
 * free_arena(a); // destroys pool header and all chunks
 * @endcode
 */
pool_t* init_pool_with_arena(arena_t* arena,
                             size_t   block_size,
                             size_t   alignment,
                             size_t   blocks_per_chunk,
                             bool     prewarm_one_chunk,
                             bool     grow_enabled);
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate one fixed-size block from a pool.
 *
 * Obtains a block from the pool in O(1) time. If the pool has previously freed
 * blocks, they are returned first via an intrusive free-list. Otherwise the
 * allocation is carved from the current arena slice (bump pointer). If the
 * current slice is exhausted and pool growth is enabled, a new slice is
 * requested from the backing arena.
 *
 * The returned pointer is aligned to the pool’s effective alignment, and the
 * block size is equal to @c pool->block_size bytes. The internal stride
 * (possibly larger than @c block_size) ensures proper alignment and space for
 * the free-list pointer on returned blocks.
 *
 * @param pool  Pool to allocate from (must be a valid pointer).
 *
 * @return Pointer to a block on success, or @c NULL on failure with @c errno set.
 *
 * @retval NULL, errno=EINVAL
 *      If @p pool is @c NULL.
 * @retval NULL, errno=EPERM
 *      If the pool is exhausted and @c grow_enabled == false, or if the backing
 *      arena is STATIC or otherwise cannot grow.
 * @retval NULL, errno=ENOMEM
 *      If growth is permitted but the underlying arena could not allocate a new
 *      slice (propagated from @c alloc_arena_aligned).
 *
 * @note Returned pointers must be released with
 *       @c return_pool_element(pool, ptr), not @c free().
 *
 * @note When @c grow_enabled == false and the pool becomes full, all subsequent
 *       calls fail until @c reset_pool() is called.
 *
 * @pre  @p pool was initialized via @c init_pool_with_arena().
 * @post On success:
 *       - The returned pointer is suitably aligned and points to a block of
 *         size @c pool->block_size bytes.
 *       - If carved from the current slice, @c pool->cur advances by @c stride.
 *       - If from the free-list, @c pool->free_blocks decreases by 1.
 *
 * @sa return_pool_element(), reset_pool(), init_pool_with_arena()
 *
 * @par Example
 * @code{.c}
 * arena_t* arena = init_dynamic_arena(8192, true, 8192, alignof(max_align_t));
 *
 * pool_t* pool = init_pool_with_arena(arena,
 *                                     64,
 *                                     0,
 *                                     32,
 *                                     true,
 *                                     true);
 *
 * void* a = alloc_pool(pool);   // OK
 * void* b = alloc_pool(pool);   // OK
 *
 * return_pool_element(pool, a); // returns 'a' to free-list
 *
 * void* c = alloc_pool(pool);   // likely returns 'a' again (LIFO reuse)
 *
 * free_arena(arena);            // releases pool header + all slices
 * @endcode
 */
void* alloc_pool(pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a previously allocated block to the pool’s free list.
 *
 * Places @p ptr onto the pool’s intrusive LIFO free list in O(1) time.
 * The block must have been obtained from the same @p pool via
 * @c alloc_pool() and must not already be on the free list (no double free).
 *
 * In debug builds, the internal helper validates that @p ptr:
 *  - lies within the backing arena’s used region,
 *  - is aligned to the pool’s @c stride,
 *  - belongs to one of this pool’s recorded slices.
 * Failing any of these conditions triggers an assertion.
 *
 * @param pool  Target pool (may be NULL; treated as a no-op).
 * @param ptr   Pointer previously returned by @c alloc_pool() (may be NULL; no-op).
 *
 * @return void
 *
 * @note This function does not set @c errno and never allocates.
 * @note The free list is *intrusive*: the first @c sizeof(void*) bytes of a
 *       freed block are repurposed to store the next pointer until reallocated.
 *
 * @warning Passing a pointer not obtained from this pool, a pointer belonging
 *          to another pool, or double-freeing a block is undefined behavior in
 *          release builds and will corrupt the free list. Debug builds attempt
 *          to catch these errors via assertions.
 *
 * @pre  @p pool was initialized via @c init_pool_with_arena().
 * @pre  @p ptr is either NULL or a live block from @p pool.
 * @post The block is available for immediate reuse by @c alloc_pool() and
 *       will typically be the next block returned (LIFO behavior).
 *
 * @sa alloc_pool(), reset_pool(), init_pool_with_arena()
 *
 * @par Example
 * @code{.c}
 * arena_t* a = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
 * pool_t*  p = init_pool_with_arena(a, 64, 0, 32, true, true);
 *
 * void* x = alloc_pool(p);
 * void* y = alloc_pool(p);
 *
 * return_pool_element(p, x);      // x goes to the head of the free list
 * void* z = alloc_pool(p);        // z == x (LIFO reuse)
 *
 * free_arena(a); // destroys all pool memory
 * @endcode
 */
void return_pool_element(pool_t* pool, void* ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reset a pool to its initial empty state without releasing arena memory.
 *
 * Clears the pool’s internal bookkeeping — including its free list, bump
 * pointer state, and block counters — effectively returning the pool to a
 * freshly-initialized state *without reclaiming any memory from the underlying
 * arena*. The arena-owned slices previously used by the pool remain reserved
 * until the arena is reset or destroyed.
 *
 * After @c reset_pool(), subsequent calls to @c alloc_pool() behave as if no
 * blocks have been allocated yet: if the pool was prewarmed or previously
 * grew, the first allocation after reset will trigger growth again unless the
 * initial slice is still present and @c cur!=end.
 *
 * In debug builds, per-slice debug metadata is cleared; the slice headers
 * themselves remain in arena memory and will be reclaimed if and when the
 * arena is reset or freed.
 *
 * @param pool  Pool to reset (may be @c NULL; treated as a no-op).
 *
 * @note This does not free or reuse arena storage. It only resets the pool’s
 *       bookkeeping. Call @c reset_arena() or @c free_arena() to reclaim
 *       memory globally.
 *
 * @warning Any outstanding blocks obtained from @c alloc_pool() become invalid
 *          after @c reset_pool(). Using them after reset is undefined behavior.
 *
 * @post
 *   - @c pool_free_blocks(pool) == 0
 *   - @c pool_total_blocks(pool) == 0
 *   - Free list is empty
 *   - Bump state is cleared (@c cur == @c end == NULL)
 *
 * @sa alloc_pool(), return_pool_element(), init_pool_with_arena(),
 *     reset_arena(), free_arena()
 *
 * @par Example
 * @code{.c}
 * arena_t* a = init_dynamic_arena(16 * 1024, true, 4096, alignof(max_align_t));
 * pool_t*  p = init_pool_with_arena(a,
 *                                   64,
 *                                   0,
 *                                   64,
 *                                   true,
 *                                   true);
 *
 * void* x = alloc_pool(p);
 * void* y = alloc_pool(p);
 *
 * return_pool_element(p, x);
 * assert(pool_free_blocks(p) == 1);
 *
 * reset_pool(p);   // invalidate x, y, and all other outstanding blocks
 *
 * assert(pool_free_blocks(p) == 0);
 * assert(pool_total_blocks(p) == 0);
 *
 * void* z = alloc_pool(p);  // triggers a fresh slice allocation if needed
 *
 * free_arena(a); // pool header + slices reclaimed
 * @endcode
 */
void reset_pool(pool_t* pool); 
// -------------------------------------------------------------------------------- 

/**
 * @brief Destroy a pool and release its resources.
 *
 * Frees the memory associated with a pool allocator. If the pool owns its
 * backing arena (created via a convenience constructor that allocates the
 * arena), then this function will call @c free_arena() and all memory for the
 * pool and its slices is reclaimed.
 *
 * If the pool does *not* own the arena (i.e., the arena was supplied by the
 * caller), the pool object is invalidated but the arena is left untouched.
 * This allows a pool to be used as a transient allocator layered on top of a
 * longer-lived arena.
 *
 * After calling @c free_pool(), the @p pool pointer must not be used again.
 *
 * @param pool  Pool to destroy (may be @c NULL, treated as no-op).
 *
 * @note This function never sets @c errno.
 *
 * @warning Outstanding allocations obtained from @c alloc_pool() become invalid
 *          after @c free_pool(). Accessing them is undefined behavior.
 *
 * @warning When a pool does not own its arena, this function does *not*
 *          return arena memory to the system. Use @c reset_pool() to reuse the
 *          arena space, or @c reset_arena() / @c free_arena() when the arena
 *          itself should be reclaimed.
 *
 * @pre  @p pool was initialized via @c init_pool_with_arena() or a
 *       pool creation helper.
 * @post If @c pool->owns_arena == true, its arena and slices are freed.
 * @post If @c pool->owns_arena == false, pool metadata is invalidated but the
 *       arena remains usable for other allocators.
 *
 * @sa init_pool_with_arena(), alloc_pool(), return_pool_element(),
 *     reset_pool(), reset_arena(), free_arena()
 *
 * @par Example: Borrowed arena
 * @code{.c}
 * arena_t* a = init_dynamic_arena(8192, true, 4096, alignof(max_align_t));
 * pool_t*  p = init_pool_with_arena(a, 64, 0, 32, false, false);
 *
 * void* x = alloc_pool(p);
 * return_pool_element(p, x);
 *
 * free_pool(p);    // pool invalid, arena 'a' still valid
 *
 * void* y = alloc_arena(a, 128, false);  // still works
 * free_arena(a);
 * @endcode
 *
 * @par Example: Pool owns arena
 * @code
 * pool_t* p = init_pool_dynamic(8192,
 *                               64,
 *                               0,
 *                               32,
 *                               true);
 *
 * void* x = alloc_pool(p);
 *
 * free_pool(p);   // frees arena and pool header
 * // p is now invalid
 * @endcode
 */
void free_pool(pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the user-visible block size for a pool.
 *
 * Reports the size in bytes of the usable payload of each block returned by
 * @c alloc_pool(). This is the value originally passed as @c block_size to
 * @c init_pool_with_arena().
 *
 * Note that the actual internal footprint of each block may be larger than
 * @c block_size due to alignment and intrusive free-list requirements; see
 * @c pool_stride() for the full internal size.
 *
 * @param pool  Pool to query (must be non-NULL).
 *
 * @return The payload size in bytes, or @c 0 if @p pool is @c NULL (and
 *         @c errno is set to @c EINVAL).
 *
 * @sa pool_stride(), alloc_pool(), return_pool_element(),
 *     init_pool_with_arena(), pool_total_blocks(), pool_free_blocks()
 *
 * @par Example
 * @code{.c}
 * size_t user_size = pool_block_size(p);
 * printf("Pool block payload size: %zu bytes\n", user_size);
 * @endcode
 */
size_t pool_block_size(pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Obtain the stride (internal block size) of a pool.
 *
 * Returns the number of bytes consumed by each allocation made from the pool,
 * including any padding required to satisfy the pool’s alignment guarantee and
 * to hold the intrusive next-pointer when blocks are returned to the free list.
 * This value may be larger than the user-requested @c block_size.
 *
 * @param pool  Pool to query (must be non-NULL).
 *
 * @return The stride in bytes, or @c 0 if @p pool is @c NULL (and @c errno is
 *         set to @c EINVAL).
 *
 * @note The stride is @c max(block_size, sizeof(void*)), rounded up to the
 *       pool’s effective alignment.
 *
 * @sa init_pool_with_arena(), alloc_pool(), return_pool_element(),
 *     pool_total_blocks(), pool_free_blocks()
 *
 * @par Example
 * @code{.c}
 * size_t stride = pool_stride(p);
 * printf("Each pool block consumes %zu bytes\n", stride);
 * @endcode
 */

size_t pool_stride(pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the total number of blocks ever made available by the pool.
 *
 * Counts all blocks that have been provisioned from arena slices, regardless of
 * whether they are currently allocated or returned to the free list. This value
 * increases when the pool prewarms or grows, and is reset to zero by
 * @c reset_pool().
 *
 * @param pool  Pool to query (must be non-NULL).
 *
 * @return Total number of blocks the pool has made available, or @c 0 if
 *         @p pool is @c NULL (and @c errno is set to @c EINVAL).
 *
 * @note This does not necessarily equal the number of currently usable blocks
 *       if @c reset_pool() was called without resetting the backing arena.
 *
 * @sa alloc_pool(), return_pool_element(), reset_pool(),
 *     pool_free_blocks(), pool_stride()
 *
 * @par Example
 * @code{.c}
 * printf("Blocks created so far: %zu\n", pool_total_blocks(p));
 * @endcode
 */

size_t pool_total_blocks(pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the number of blocks currently available for reuse.
 *
 * Counts the number of blocks on the pool’s intrusive free list. Each call to
 * @c return_pool_element() increments this value, and each call to
 * @c alloc_pool() that reuses a freed block decrements it. This counter is
 * reset to zero by @c reset_pool().
 *
 * @param pool  Pool to query (must be non-NULL).
 *
 * @return Number of free blocks, or @c 0 if @p pool is @c NULL (and @c errno is
 *         set to @c EINVAL).
 *
 * @sa alloc_pool(), return_pool_element(), reset_pool(),
 *     pool_total_blocks(), pool_stride()
 *
 * @par Example
 * @code{.c}
 * printf("Currently free blocks: %zu\n", pool_free_blocks(p));
 * @endcode
 */

size_t pool_free_blocks(pool_t* pool);
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
