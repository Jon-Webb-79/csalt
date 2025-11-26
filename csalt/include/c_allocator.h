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
#include <string.h>  // memset
#include <stddef.h> // max_aling_t
#include <stdalign.h> // alignof
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

typedef struct {
    uintptr_t _priv[6];
} PoolCheckPoint;
// -------------------------------------------------------------------------------- 

#ifndef ALLOC_H 
#define ALLOC_H 

    typedef enum {
        ALLOC_INVALID = 0,
        STATIC = 1,
        DYNAMIC = 2
    } alloc_t;

#endif /*ALLOC_H*/
// -------------------------------------------------------------------------------- 
// CONTEXT POINTER STRUCT

/**
 * @typedef alloc_prototype
 * @brief Prototype for allocating a block of memory.
 *
 * This function allocates `size` bytes using the allocator identified
 * by `ctx`. If `zeroed` is true, the returned memory must be
 * zero-initialized. If allocation fails, the implementation should set
 * errno (typically to ENOMEM) and return NULL.
 *
 * @param ctx     Allocator context (arena, pool, heap wrapper, etc.)
 * @param size    Number of bytes to allocate
 * @param zeroed  If true, memory must be zero-initialized
 *
 * @return Pointer to a block of at least `size` bytes, or NULL on failure.
 */
typedef void* (*alloc_prototype)(void* ctx, size_t size, bool zeroed);

/**
 * @typedef alloc_aligned_prototype
 * @brief Prototype for aligned allocation.
 *
 * This function allocates `size` bytes with at least `align` alignment.
 * Alignment must be a power of two. If `zeroed` is true, the returned
 * memory must be zero-initialized. On failure, errno must be set and
 * NULL returned.
 *
 * @param ctx     Allocator context
 * @param size    Number of bytes to allocate
 * @param align   Required alignment (power of two)
 * @param zeroed  Whether the returned memory must be zero-initialized
 *
 * @return Pointer to an aligned memory block, or NULL on failure.
 */
typedef void* (*alloc_aligned_prototype)(void* ctx, size_t size, size_t align, bool zeroed);

/**
 * @typedef realloc_prototype
 * @brief Prototype for resizing an existing allocation.
 *
 * This function resizes a previously allocated block. Implementations
 * may move the allocation; if they do, the old contents up to
 * `min(old_size, new_size)` must remain intact. If `zeroed` is true and
 * `new_size > old_size`, the newly added region must be zeroed.
 *
 * If reallocation fails, errno must be set and NULL returned. In that
 * case, the caller must retain `old_ptr` unchanged.
 *
 * @param ctx       Allocator context
 * @param old_ptr   Pointer to existing allocation (may be NULL)
 * @param old_size  Previously allocated size
 * @param new_size  Requested new size
 * @param zeroed    Whether any expanded memory must be zero-initialized
 *
 * @return New pointer on success, or NULL on failure (caller must keep old_ptr).
 */
typedef void* (*realloc_prototype)(void* ctx, void* old_ptr,
                                   size_t old_size, size_t new_size, bool zeroed);

/**
 * @typedef realloc_aligned_prototype
 * @brief Prototype for aligned reallocation.
 *
 * This function behaves like realloc_prototype but also enforces a
 * minimum alignment requirement for the resulting allocation.
 * `align` must be a power of two. If expanded, new memory must be zeroed
 * when `zeroed` is true.
 *
 * On failure, errno must be set and NULL returned, and the caller must
 * continue to use the original block (`old_ptr`).
 *
 * @param ctx       Allocator context
 * @param old_ptr   Pointer to an existing allocation
 * @param old_size  Previous allocation size
 * @param new_size  Requested new size
 * @param zeroed    Whether expanded memory must be zero-initialized
 * @param align     Required alignment (power of two)
 *
 * @return Pointer to a resized & aligned block, or NULL on failure.
 */
typedef void* (*realloc_aligned_prototype)(void* ctx, void* old_ptr,
                                           size_t old_size, size_t new_size,
                                           bool zeroed, size_t align);

/**
 * @typedef return_prototype
 * @brief Prototype for returning (or releasing) memory back to the allocator.
 *
 * Some allocators may support returning individual blocks for reuse
 * (e.g., free lists, object pools). Others may ignore this operation.
 *
 * This function must not free the memory using the system allocator
 * unless that is appropriate for the backing implementation.
 *
 * @param ctx  Allocator context
 * @param ptr  Memory block previously allocated by this allocator
 */
typedef void  (*return_prototype)(void* ctx, void* ptr);

/**
 * @typedef free_prototype
 * @brief Prototype for tearing down or freeing an allocator.
 *
 * This function releases any resources held by the allocator identified
 * by `ctx`. For arena- or pool-style allocators, this typically frees
 * all chunks in bulk. After this call, the allocator context must no
 * longer be used.
 *
 * @param ctx  Allocator context to destroy
 */
typedef void (*free_prototype)(void* ctx);

typedef struct {
    alloc_prototype           allocate;
    alloc_aligned_prototype   allocate_aligned;
    realloc_prototype         reallocate;
    realloc_aligned_prototype reallocate_aligned;
    return_prototype          return_element;
    free_prototype            deallocate;
    void*                     ctx;      // backing arena, pool, system heap, etc.
} allocator_vtable_t;
// ================================================================================ 
// ================================================================================ 
// INITIALIZE AND DEALLOCATE FUNCTIONS

/**
 * @brief Initialize a dynamically growing arena allocator.
 *
 * Allocates an arena that obtains its initial storage via @c malloc() and, if
 * permitted by @p resize, may grow by allocating new chunks on demand. The
 * arena header, first chunk header, and initial data region are all placed
 * contiguously in the first allocation. Additional chunks (if growth is
 * enabled) are allocated separately and linked into the arena.
 *
 * @param bytes          Requested initial total size in bytes. This is the
 *                       minimum storage footprint to allocate. If @p bytes is
 *                       smaller than @p min_chunk_in (when non-zero), the
 *                       larger value is used instead.
 * @param resize         Whether the arena may grow when out of space.
 *                       If @c false, the arena behaves like a static arena that
 *                       simply lives in heap memory.
 * @param min_chunk_in   Minimum data chunk size to allocate on growth
 *                       (0 allowed). If non-zero and not a power-of-two, it is
 *                       rounded up to the next power-of-two. When growing, the
 *                       arena will allocate at least this many bytes.
 * @param base_align_in  Required minimum base alignment for all allocations
 *                       within the arena (0 => use @c alignof(max_align_t)).
 *                       Rounded up to the next power-of-two if needed and
 *                       never less than @c alignof(max_align_t).
 *
 * @return Pointer to a new dynamic arena on success, or @c NULL on failure
 *         with @c errno set.
 *
 * @retval NULL, errno=EINVAL
 *      If @p bytes is too small to contain the arena header and first chunk
 *      header, if @p base_align_in or @p min_chunk_in has an invalid value that
 *      cannot be normalized, or alignment calculations fail.
 * @retval NULL, errno=ENOMEM
 *      If memory allocation fails or arithmetic overflow is detected during
 *      pointer computations.
 * @retval NULL, errno=ENOTSUP
 *      If dynamic arenas are disabled at compile time
 *      (@c ARENA_ENABLE_DYNAMIC == 0).
 *
 * @note The returned arena’s first chunk is fully initialized and ready for
 *       allocation via @c alloc_arena() or @c alloc_arena_aligned().
 *
 * @note The initial data region begins at an address aligned to
 *       @c max(base_align_in, alignof(max_align_t)).
 *
 * @warning The returned arena must be released with @c free_arena(). Individual
 *          allocations from the arena must not be freed with @c free().
 *
 * @pre  @c ARENA_ENABLE_DYNAMIC must be enabled.  This occurs by default compilation process
 * @post @c arena->mem_type == DYNAMIC and @c arena->resize == (resize ? 1 : 0).
 * @post @c arena->head == arena->tail and @c arena->cur begins at the start of
 *       the aligned initial data region.
 *
 * @sa init_static_arena(), free_arena(), alloc_arena(), alloc_arena_aligned(),
 *     reset_arena(), arena_remaining()
 *
 * @par Example
 * @code{.c}
 * // Create a dynamic arena with 4 KiB initial storage,
 * // allow future growth, minimum chunk of 4 KiB, default alignment.
 * arena_t* a = init_dynamic_arena(4096,        // initial bytes
 *                                 true,        // resize enabled
 *                                 4096,        // minimum chunk size for growth
 *                                 alignof(max_align_t));
 *
 * void* p = alloc_arena(a, 128, true);   // allocate 128 zeroed bytes
 * assert(p != NULL);
 *
 * // ... use arena allocations ...
 *
 * free_arena(a);  // releases all chunks and the arena header itself
 * @endcode
 */
arena_t* init_dynamic_arena(size_t bytes, bool resize, size_t min_chunk_in, size_t base_align_in);
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
 * @code{.c}
 * uint8_t buf[16 * 24];  
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
 * @code{.c}
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
 * @brief Create a sub-arena carved from a parent arena with single allocation.
 *
 * Allocates a contiguous region from @p parent and constructs an arena header,
 * chunk header, and data region within it (single alloc_arena call). The 
 * sub-arena is fixed-capacity (cannot grow) and does NOT own its backing memory.
 *
 * Memory layout: [ arena_t | padding | Chunk | padding | usable data ]
 *
 * @param[in,out] parent    Parent arena to allocate from (must not be NULL).
 * @param[in]     bytes     Total bytes to carve from parent (including headers).
 * @param[in]     alignment Base alignment for sub-arena data allocations.
 *
 * @return Pointer to new arena_t on success; NULL on failure with errno set.
 *
 * @retval NULL, errno=EINVAL
 *      - parent is NULL
 *      - bytes too small for headers + at least 1 byte of data
 *      - alignment is invalid
 *
 * @retval NULL, errno=ENOMEM
 *      - parent cannot satisfy the request
 *
 * @note The sub-arena has:
 *       - owns_memory = false (parent owns the backing region)
 *       - resize = false (fixed capacity, cannot grow)
 *       - mem_type inherited from parent
 *
 * @note Calling free_arena() on the sub-arena only nulls it out (no actual free).
 *       The backing memory is released when the parent is freed/reset.
 *
 * @warning If parent is reset or freed, all sub-arenas become invalid.
 *
 * @par Example: Scoped temporary allocations
 * @code{.c}
 * arena_t* main = init_darena(1024*1024, true);
 * 
 * // Carve 8KB sub-arena (includes headers)
 * arena_t* temp = init_arena_with_arena(main, 8192, alignof(max_align_t));
 * assert_non_null(temp);
 * 
 * void* data = alloc_arena(temp, 1024, false);  // Same API as main arena
 * // ... use data ...
 * 
 * free_arena(temp);   // Only nulls it out
 * free_arena(main);   // Frees everything including temp's region
 * @endcode
 */
arena_t* init_arena_with_arena(arena_t* parent,
                                size_t bytes,
                                size_t alignment_in);
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
 *        after the arena is no longer used.  Although the user is encouraged to use stack 
 *        memory buffers for this function which are freed when the scope ends.
 *
 * @sa    init_static_arena(), alloc_arena(), reset_arena(), arena_remaining()
 *
 * @par Example: Typical usage with caller-managed storage
 * @code{.c}
 * enum { BUF = 16 * 1024 };
 * uint8_t* buf[16 * 24];
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
 */
arena_t* init_sarena(void* buffer, size_t bytes);
// -------------------------------------------------------------------------------- 

/**
 * @brief Destroy a dynamically allocated arena and frees all of its heap memory.
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
 * @code{.c}
 * arena_t *a = init_darena(4096, true);
 * void *p1 = alloc_arena(a, 1024, false);
 * // ... use p1 ...
 * free_arena(a);   // releases all chunks and 'a' itself
 * a = NULL;        // prevent accidental reuse
 * @endcode
 *
 * @par Example (static arena)
 * @code{.c}
 * enum { BUF = 8192 };
 * void *buf = malloc(alignof(max_align_t));
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
 * @code{.c}
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
 * @code{.c}
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
 * @brief Reallocate an object within an arena allocator.
 *
 * The arena allocator does not support in-place reallocation. This function
 * provides a `realloc`-like interface by allocating a new block from the arena,
 * copying the original object into it, and returning the new pointer. The
 * original memory remains in the arena until the arena is reset; it cannot be
 * individually freed.
 *
 * If @p realloc_size is less than or equal to @p var_size, this function
 * performs a no-op and simply returns @p variable, avoiding unnecessary arena
 * consumption.
 *
 * If @p zeroed is true and a new block is allocated, only the newly added tail
 * region (bytes [var_size, realloc_size)) will be zero-initialized. The first
 * @p var_size bytes preserve their original contents.
 *
 * @param arena
 *     Pointer to an initialized ::arena_t instance. Must not be NULL.
 *
 * @param variable
 *     Pointer to the existing object previously allocated from @p arena. Must
 *     not be NULL. Passing memory not allocated by the arena results in
 *     undefined behavior.
 *
 * @param var_size
 *     Size in bytes of the existing object. Must match the size originally
 *     allocated for @p variable.
 *
 * @param realloc_size
 *     New allocation size in bytes. If greater than @p var_size, a new block
 *     is allocated and the contents copied. If less than or equal to
 *     @p var_size, the function returns @p variable.
 *
 * @param zeroed
 *     If true, the newly added tail region is zero-filled. If false, the tail
 *     contains uninitialized memory.
 *
 * @return
 *     - On success, a pointer to a valid memory block:
 *         - The original pointer if no growth was required.
 *         - A new pointer if a larger block was allocated.
 *     - On failure, NULL is returned and `errno` is set to:
 *         - `EINVAL` if @p arena or @p variable is NULL.
 *         - Any error code raised by ::arena_alloc during allocation.
 *
 * @note
 *     - This function never frees memory; old blocks accumulate until the
 *       arena is reset.
 *     - Alignment of the new block follows ::arena_alloc rules.
 *     - Not thread-safe unless externally synchronized.
 *
 * @code{.c}
 * // Example: growing an integer array inside an arena
 * arena_t* arena = init_dynamic_arena(4096);
 * if (!arena) {
 *     perror("Arena initialization failed");
 *     exit(EXIT_FAILURE);
 * }
 *
 * size_t n = 4;
 * int* arr = arena_alloc(arena, n * sizeof(int), true);
 * if (!arr) {
 *     perror("Initial allocation failed");
 *     exit(EXIT_FAILURE);
 * }
 *
 * // Populate initial values
 * for (size_t i = 0; i < n; ++i)
 *     arr[i] = (int)i;
 *
 * // Grow the array to 8 integers
 * size_t new_n = 8;
 * int* new_arr = realloc_arena(arena, arr,
 *                              n * sizeof(int),
 *                              new_n * sizeof(int),
 *                              true);  // zero-fill the new region
 *
 * if (!new_arr) {
 *     perror("Arena reallocation failed");
 *     exit(EXIT_FAILURE);
 * }
 *
 * // new_arr now contains: [0, 1, 2, 3, 0, 0, 0, 0]
 *
 * // Use the new space
 * for (size_t i = n; i < new_n; ++i)
 *     new_arr[i] = (int)(i * 10);
 *
 * // When done:
 * reset_arena(arena);  // frees all arena memory at once
 * free_arena(arena);   // destroys the allocator
 * @endcode
 *
 * @see arena_alloc
 * @see reset_arena
 */

void* realloc_arena(arena_t* arena, void* variable, size_t var_size, size_t realloc_size,
                    bool zeroed);
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
 * @code{.c}
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
 * @code{.c}
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
// -------------------------------------------------------------------------------- 

/**
 * @brief Reallocate an object within an arena allocator with a specified alignment.
 *
 * This function behaves like a `realloc` operation adapted for arena semantics:
 * arenas cannot grow an allocation in place, so reallocation is implemented as:
 *
 *      1. Allocate a new block of @p realloc_size bytes with the requested
 *         alignment @p aligned using ::arena_alloc_aligned.
 *      2. Copy the first @p var_size bytes from @p variable into the new block.
 *      3. Optionally zero-fill the new tail region (bytes [var_size, realloc_size)).
 *      4. Return the new pointer.
 *
 * The old memory is not freed and remains part of the arena until a call to
 * ::reset_arena() or until its parent chunk is discarded.
 *
 * If @p realloc_size is less than or equal to @p var_size, the function performs
 * a no-op and returns @p variable unchanged. This avoids unnecessary arena
 * consumption and matches typical arena semantics.
 *
 * @param arena
 *     Pointer to an initialized ::arena_t. Must not be NULL.
 *
 * @param variable
 *     Pointer to the existing object previously allocated from @p arena.
 *     Must not be NULL. The function does not validate arena ownership.
 *
 * @param var_size
 *     Size of the existing object in bytes. Must be the exact size originally
 *     allocated. Passing an incorrect size results in undefined behavior.
 *
 * @param realloc_size
 *     New size in bytes for the reallocated object. If greater than @p var_size,
 *     a new block is allocated with alignment @p aligned. Otherwise, this
 *     function returns @p variable unchanged.
 *
 * @param zeroed
 *     If true, the newly added tail region is zero-filled. The initial
 *     @p var_size bytes are always preserved exactly.
 *
 * @param aligned
 *     Required alignment for the new allocation. Must be a power of two and
 *     supported by ::arena_alloc_aligned. The returned pointer will satisfy:
 *
 *         (uintptr_t)ptr % aligned == 0
 *
 * @return
 *     - The original @p variable if no growth is required.
 *     - A newly allocated, aligned pointer if growth is required.
 *     - NULL on failure, with `errno` set to:
 *         - `EINVAL` if @p arena or @p variable is NULL.
 *         - Errors propagated from ::arena_alloc_aligned when allocation fails.
 *
 * @note
 *     - Alignment of the returned block *may differ from* the original block.
 *     - This function never frees memory; the old block persists.
 *     - Misaligned @p aligned values (non-power-of-two) result in undefined behavior.
 *     - Not thread-safe unless the arena is externally synchronized.
 *
 * @code
 * // Example: aligned growth of a struct inside an arena
 * typedef struct {
 *     float x, y, z;
 * } vec3;
 *
 * arena_t* arena = init_dynamic_arena(8192);
 *
 * // Allocate a vec3 with 16-byte alignment
 * vec3* v = arena_alloc_aligned(arena, sizeof(vec3), 16, true);
 * if (!v) {
 *     perror("aligned allocation failed");
 *     exit(EXIT_FAILURE);
 * }
 *
 * v->x = 1.0f;
 * v->y = 2.0f;
 * v->z = 3.0f;
 *
 * // Reallocate with 32-byte alignment and additional trailing space
 * size_t old_size = sizeof(vec3);
 * size_t new_size = sizeof(vec3) + 32;
 *
 * vec3* v2 = realloc_arena_aligned(arena, v,
 *                                  old_size,
 *                                  new_size,
 *                                  true,   // zero the new region
 *                                  32);    // new alignment requirement
 *
 * if (!v2) {
 *     perror("aligned realloc failed");
 *     exit(EXIT_FAILURE);
 * }
 *
 * // v2 preserves the vec3 values and now has a zeroed 32-byte tail.
 * // The returned pointer satisfies (uintptr_t)v2 % 32 == 0.
 *
 * // Old object v is still in the arena; it is not freed.
 *
 * reset_arena(arena);   // Free all arena memory at once
 * free_arena(arena);
 * @endcode
 *
 * @see arena_alloc_aligned
 * @see arena_alloc
 * @see reset_arena
 */

void* realloc_arena_aligned(arena_t* arena, void* variable, size_t var_size, size_t realloc_size,
                            bool zeroed, size_t alignment);
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.c}
 * char buf[512];
 * if (!arena_stats(a, buf, sizeof buf)) {
 *     perror("arena_stats");
 * } else {
 *     fputs(buf, stdout);
 * }
 * @endcode
 */
bool arena_stats(const arena_t* arena, char* buffer, size_t buffer_size);
// -------------------------------------------------------------------------------- 

/**
 * @brief Check whether an arena owns its backing memory.
 *
 * Determines the ownership model of the arena's memory allocation:
 * - **true**: Arena owns its memory and free_arena() will release it
 * - **false**: Arena borrows memory from another source (parent arena or caller buffer)
 *
 * This is useful for understanding memory lifetime and determining safe
 * operations on an arena.
 *
 * @param[in] arena  Pointer to the arena to query. May be NULL.
 *
 * @return true if the arena owns its backing memory; false otherwise.
 *
 * @retval false, errno=EINVAL  if @p arena is NULL
 * @retval true   Arena created with init_darena() or init_dynamic_arena()
 *                (owns its dynamically allocated memory)
 * @retval true   Arena created with init_sarena() or init_static_arena()
 *                (owns the arena header but NOT the caller's buffer)
 * @retval false  Arena created with init_arena_with_arena()
 *                (sub-arena borrowing from parent, owns nothing)
 *
 * @note For static arenas, owns_memory=true means the arena owns its header
 *       structure, but the caller still owns the buffer itself. This is why
 *       free_arena() returns EPERM for static arenas.
 *
 * @note This function is useful for:
 *       - Determining if toggle_arena_resize() is allowed (requires owns_memory=true)
 *       - Understanding cleanup responsibilities
 *       - Validating arena relationships before operations
 *
 * @par Example: Checking ownership before operations
 * @code{.c}
 * arena_t* parent = init_darena(64*1024, true);
 * arena_t* sub = init_arena_with_arena(parent, 8192, 8);
 * 
 * if (arena_owns_memory(parent)) {
 *     printf("Parent owns its memory\n");          // Prints
 * }
 * 
 * if (!arena_owns_memory(sub)) {
 *     printf("Sub-arena borrows from parent\n");   // Prints
 * }
 * 
 * // Only owned arenas can have resize toggled
 * if (arena_owns_memory(parent)) {
 *     toggle_arena_resize(parent, false);  // OK
 * }
 * 
 * if (!arena_owns_memory(sub)) {
 *     // toggle_arena_resize(sub, true);  // Would fail with EPERM
 * }
 * 
 * free_arena(parent);
 * @endcode
 *
 * @par Example: Ownership by arena type
 * @code{.c}
 * // Dynamic arena: owns memory
 * arena_t* dyn = init_darena(4096, true);
 * assert_true(arena_owns_memory(dyn));
 * free_arena(dyn);  // Frees all memory
 * 
 * // Static arena: owns header, caller owns buffer
 * uint8_t buf[8192];
 * arena_t* sta = init_sarena(buf, sizeof(buf));
 * assert_true(arena_owns_memory(sta));
 * free_arena(sta);  // Returns EPERM (caller must manage buf)
 * 
 * // Sub-arena: owns nothing
 * arena_t* parent = init_darena(64*1024, true);
 * arena_t* sub = init_arena_with_arena(parent, 8192, 8);
 * assert_false(arena_owns_memory(sub));
 * free_arena(sub);     // Just nulls it out
 * free_arena(parent);  // Frees everything including sub's region
 * @endcode
 *
 * @sa init_arena_with_arena(), toggle_arena_resize(), free_arena()
 */
bool arena_owns_memory(const arena_t* arena);
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.c}
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
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for arena allocation.
 *
 * Allocates a block of memory of size @p size from the arena referenced
 * by @p ctx. If @p zeroed is true, the returned memory will be
 * zero-initialized according to the semantics of alloc_arena().
 *
 * On error, errno is set (typically to ENOMEM or EINVAL) and NULL is
 * returned.
 *
 * This function implements the @ref alloc_prototype interface for
 * arena-backed allocators.
 *
 * @param ctx    Pointer to an arena_t instance.
 * @param size   Number of bytes to allocate.
 * @param zeroed Whether the memory should be zero-initialized.
 *
 * @retval void* Pointer to a block of at least @p size bytes.
 * @retval NULL  On failure, with errno set.
 */
static inline void* arena_v_alloc(void* ctx, size_t size, bool zeroed) {
    arena_t* arena = (arena_t*)ctx;
    if (!arena) {
        errno = EINVAL;
        return NULL;
    }
    return alloc_arena(arena, size, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for aligned arena allocation.
 *
 * Allocates a block of memory of size @p size from the arena referenced
 * by @p ctx, with a minimum alignment of @p align. The alignment must
 * be a power of two. If @p zeroed is true, the returned memory will be
 * zero-initialized.
 *
 * On error, errno is set (typically to ENOMEM or EINVAL) and NULL is
 * returned.
 *
 * This function implements the @ref alloc_aligned_prototype interface
 * for arena-backed allocators.
 *
 * @param ctx    Pointer to an arena_t instance.
 * @param size   Number of bytes to allocate.
 * @param align  Required alignment (power of two).
 * @param zeroed Whether the memory should be zero-initialized.
 *
 * @retval void* Pointer to an aligned block of at least @p size bytes.
 * @retval NULL  On failure, with errno set.
 */
static inline void* arena_v_alloc_aligned(void* ctx, size_t size,
                                          size_t align, bool zeroed) {
    arena_t* arena = (arena_t*)ctx;
    if (!arena) {
        errno = EINVAL;
        return NULL;
    }
    return alloc_arena_aligned(arena, size, align, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for arena reallocation.
 *
 * Resizes a block of memory previously allocated from the arena
 * referenced by @p ctx. The block is identified by @p old_ptr and its
 * previous size @p old_size. The new requested size is @p new_size.
 *
 * The contents of the allocation are preserved up to
 * min(@p old_size, @p new_size). If @p zeroed is true and the
 * allocation grows, any newly added region must be zero-initialized
 * according to the semantics of realloc_arena().
 *
 * On error, errno is set and NULL is returned. In that case, the caller
 * must continue to use @p old_ptr unchanged.
 *
 * This function implements the @ref realloc_prototype interface for
 * arena-backed allocators.
 *
 * @param ctx       Pointer to an arena_t instance.
 * @param old_ptr   Pointer to the existing allocation (may be NULL).
 * @param old_size  Size of the existing allocation.
 * @param new_size  Requested new size.
 * @param zeroed    Whether any expanded portion must be zero-initialized.
 *
 * @retval void* New pointer to the resized allocation on success.
 * @retval NULL  On failure, with errno set (caller keeps @p old_ptr).
 */
static inline void* arena_v_realloc(void* ctx, void* old_ptr,
                                    size_t old_size, size_t new_size,
                                    bool zeroed) {
    arena_t* arena = (arena_t*)ctx;
    if (!arena) {
        errno = EINVAL;
        return NULL;
    }
    return realloc_arena(arena, old_ptr, old_size, new_size, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for aligned arena reallocation.
 *
 * Resizes a block of memory previously allocated from the arena
 * referenced by @p ctx, enforcing a minimum alignment of @p align for
 * the resulting block. The previous allocation is described by
 * @p old_ptr and @p old_size; the new requested size is @p new_size.
 *
 * The contents of the allocation are preserved up to
 * min(@p old_size, @p new_size). If @p zeroed is true and the
 * allocation grows, any newly added region must be zero-initialized.
 *
 * On error, errno is set and NULL is returned. In that case, the caller
 * must continue to use @p old_ptr unchanged.
 *
 * This function implements the @ref realloc_aligned_prototype interface
 * for arena-backed allocators.
 *
 * @param ctx       Pointer to an arena_t instance.
 * @param old_ptr   Pointer to the existing allocation.
 * @param old_size  Size of the existing allocation.
 * @param new_size  Requested new size.
 * @param zeroed    Whether any expanded portion must be zero-initialized.
 * @param align     Required alignment (power of two).
 *
 * @retval void* Pointer to the resized, aligned allocation on success.
 * @retval NULL  On failure, with errno set (caller keeps @p old_ptr).
 */
static inline void* arena_v_realloc_aligned(void* ctx, void* old_ptr,
                                            size_t old_size, size_t new_size,
                                            bool zeroed, size_t align) {
    arena_t* arena = (arena_t*)ctx;
    if (!arena) {
        errno = EINVAL;
        return NULL;
    }
    return realloc_arena_aligned(arena, old_ptr, old_size,
                                 new_size, zeroed, align);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for returning an element to an arena allocator.
 *
 * Arena allocators do not support returning individual blocks to the
 * allocator; all memory is typically released in bulk via a reset or
 * destroy operation. As a result, this function is effectively a no-op.
 *
 * If @p ctx is NULL, errno is set to EINVAL and the function returns.
 * The @p ptr parameter is ignored.
 *
 * This function implements the @ref return_prototype interface for
 * arena-backed allocators.
 *
 * @param ctx Pointer to an arena_t instance.
 * @param ptr Pointer to an allocated block (ignored).
 */
static inline void arena_v_return(void* ctx, void* ptr) {
    (void)ptr;  // arenas don’t support returning individual blocks
    arena_t* arena = (arena_t*)ctx;
    if (!arena) {
        errno = EINVAL;
        return;
    }
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for freeing an arena allocator.
 *
 * Releases all memory owned by the arena referenced by @p ctx. After
 * this call, the arena must not be used again.
 *
 * If @p ctx is NULL, errno is set to EINVAL and the function returns
 * without performing any action.
 *
 * This function implements the @ref free_prototype interface for
 * arena-backed allocators.
 *
 * @param ctx Pointer to an arena_t instance.
 */
static inline void arena_v_free(void* ctx) {
    arena_t* arena = (arena_t*)ctx;
    if (!arena) {
        errno = EINVAL;
        return;
    }
    free_arena(arena);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Constructs an allocator_vtable_t for a given arena.
 *
 * This helper initializes an allocator_vtable_t that exposes an
 * arena_t instance through the generic allocator interface. All
 * operations (allocate, reallocate, deallocate, etc.) are forwarded
 * to the arena-backed vtable adapter functions.
 *
 * The returned vtable is typically used to parameterize other
 * components that depend on the generic allocator API rather than
 * directly on arena_t.
 *
 * @param a Pointer to an arena_t instance to use as the allocator backend.
 *
 * @return A fully initialized allocator_vtable_t bound to @p a.
 */
static inline allocator_vtable_t arena_allocator(arena_t* a) {
    allocator_vtable_t v = {
        .allocate           = arena_v_alloc,
        .allocate_aligned   = arena_v_alloc_aligned,
        .reallocate         = arena_v_realloc,
        .reallocate_aligned = arena_v_realloc_aligned,
        .return_element     = arena_v_return,
        .deallocate         = arena_v_free,
        .ctx                = a
    };
    return v;
}
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
 * @code{.c}
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
 * @brief Create a pool allocator that owns its own dynamically allocated arena.
 *
 * This function constructs a @c pool_t backed by a newly allocated dynamic
 * arena. The pool header is allocated inside the arena itself (no
 * external @c malloc for pool metadata), and the pool may optionally
 * reserve its first slice of memory immediately (“prewarm”).
 *
 * The pool supports two operation modes:
 * - **Grow mode** (@p grow_enabled = true): additional slices are carved from
 *   the arena on demand. If the arena runs out of backing memory, it may
 *   allocate new chunks (subject to @p min_chunk_bytes).
 * - **Fixed-capacity mode** (@p grow_enabled = false): no new slices may be
 *   allocated after initialization. If @p prewarm_one_chunk = false, the first
 *   call to @c alloc_pool() will fail immediately with @c EPERM.
 *
 * @param block_size        User-visible block payload size in bytes (must be > 0).
 * @param alignment         Requested alignment for returned blocks. If 0,
 *                          @c alignof(max_align_t) is used. Rounded up to at
 *                          least @c alignof(void*) to support the intrusive
 *                          free-list pointer.
 * @param blocks_per_chunk  Number of blocks to allocate per pool slice. Must be > 0.
 * @param arena_seed_bytes  Initial arena allocation size in bytes. Must be > 0.
 *                          Must be large enough to hold the pool header and,
 *                          if @p prewarm_one_chunk is true, at least one slice.
 * @param min_chunk_bytes   Minimum data bytes per arena expansion when growing
 *                          (0 allowed; normalized upward if needed).
 * @param grow_enabled      Whether the pool may request additional slices after
 *                          initialization.
 * @param prewarm_one_chunk Whether to allocate the first slice immediately.
 *                          If @c true and insufficient space is available,
 *                          initialization fails.
 *
 * @return A pointer to a fully initialized pool on success, or @c NULL on
 *         failure with @c errno set.
 *
 * @retval NULL, errno=EINVAL
 *      If any argument is invalid (@p block_size == 0,
 *      @p blocks_per_chunk == 0, @p arena_seed_bytes == 0).
 * @retval NULL, errno=EOVERFLOW
 *      If @c block_size * blocks_per_chunk would overflow @c size_t.
 * @retval NULL, errno=ENOMEM
 *      If memory for the arena or pool header cannot be allocated, or if
 *      @p prewarm_one_chunk is true and the slice cannot be carved.
 * @retval NULL, errno=EPERM
 *      If @p grow_enabled is false, the arena has insufficient initial space,
 *      and @p prewarm_one_chunk is false (first allocation would fail).
 * @retval NULL, errno=ENOTSUP
 *      If dynamic arenas are disabled via compile-time configuration.
 *
 * @note The pool takes ownership of the created arena. Call @c free_pool()
 *       (not @c free_arena()) to destroy it.
 *
 * @note Returned blocks have at least @c alignment alignment and are at least
 *       @c sizeof(void*) bytes to support the intrusive free list.
 *
 * @warning In fixed-capacity mode (@p grow_enabled = false), failure to
 *          prewarm results in a pool that cannot allocate any blocks until
 *          reset.
 *
 * @pre @c ARENA_ENABLE_DYNAMIC must be enabled at compile time.
 * @post On success, the pool owns a dynamic arena and can allocate blocks.
 *
 * @sa init_pool_with_arena(), alloc_pool(), return_pool_element(),
 *     reset_pool(), free_pool(), init_dynamic_arena()
 *
 * @par Example: Grow-enabled pool
 * @code{.c}
 * pool_t* p = init_dynamic_pool(
 *     64,                   // block_size
 *     0,                    // alignment (default)
 *     32,                   // blocks_per_chunk
 *     8192,                 // arena_seed_bytes
 *     4096,                 // min_chunk_bytes
 *     true,                 // grow_enabled
 *     true                  // prewarm_one_chunk
 * );
 *
 * void* b1 = alloc_pool(p);  // OK
 * void* b2 = alloc_pool(p);  // Grows when necessary
 *
 * free_pool(p);             // Frees arena and pool
 * @endcode
 *
 * @par Example: Fixed-capacity pool
 * @code{.c}
 * pool_t* p = init_dynamic_pool(
 *     128,                  // block_size
 *     0,                    // default alignment
 *     16,                   // blocks_per_chunk
 *     4096,                 // arena_seed_bytes
 *     0,                    // min_chunk_bytes (not used, no growth)
 *     false,                // grow_enabled = false
 *     true                  // prewarm_one_chunk (allocate first slice now)
 * );
 *
 * // Exactly 16 allocations succeed:
 * for (int i = 0; i < 16; i++) assert(alloc_pool(p) != NULL);
 * assert(alloc_pool(p) == NULL); // EPERM (fixed size reached)
 *
 * free_pool(p);
 * @endcode
 */
pool_t* init_dynamic_pool(size_t block_size,
                          size_t alignment,
                          size_t blocks_per_chunk,
                          size_t arena_seed_bytes,
                          size_t min_chunk_bytes,
                          bool   grow_enabled,
                          bool   prewarm_one_chunk);
// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a fixed-capacity memory pool over a caller-supplied buffer.
 *
 * This function creates a @c pool_t allocator backed by a static arena built
 * inside @p buffer. The pool metadata and the backing slice are allocated from
 * the caller-provided memory region—no heap allocations are performed. The
 * pool eagerly carves a single contiguous slice of memory and divides it into
 * fixed-size blocks. Its capacity is therefore
 *
 *     floor( (buffer_bytes - header_overhead) / stride )
 *
 * where @c stride is @p block_size rounded up to the chosen alignment, and
 * at least @c sizeof(void*) to support the intrusive free list.
 *
 * @p alignment controls the required alignment of each returned element.
 * If zero, @c alignof(max_align_t) is used. The effective alignment is clamped
 * to be at least @c alignof(void*) so freed blocks can store a next-pointer.
 *
 * The caller retains ownership of @p buffer itself; @c free_pool() only tears
 * down the pool and arena metadata and does not free @p buffer.
 *
 * @param buffer        Pointer to caller-owned memory region used for the pool
 *                      and its arena bookkeeping. Must be non-NULL.
 * @param buffer_bytes  Size of @p buffer in bytes. Must be > 0.
 * @param block_size    Size of each allocated block (payload bytes). Must be > 0.
 * @param alignment     Desired alignment for returned blocks (0 → default =
 *                      @c alignof(max_align_t)). The effective alignment will
 *                      be @c max(alignment, alignof(void*)) rounded up by the
 *                      implementation if power-of-two alignment is required.
 *
 * @return A pointer to an initialized fixed-capacity @c pool_t on success, or
 *         @c NULL on failure with @c errno set.
 *
 * @retval NULL, errno=EINVAL
 *      If @p buffer is NULL, or @p buffer_bytes == 0, or @p block_size == 0.
 * @retval NULL, errno=ENOMEM
 *      If the static arena cannot reserve space for the pool header, or if the
 *      remaining capacity cannot fit at least one stride-aligned block.
 *
 * @note This pool never grows. After initialization,
 *       @c grow_enabled == false and @c total_blocks is fixed.
 *       When all blocks are in use, @c alloc_pool() returns NULL with
 *       @c errno == EPERM.
 *
 * @note @c return_pool_element() returns blocks to the pool’s free list.
 *       Returned blocks are reused in LIFO order.
 *
 * @warning Returned memory must not be passed to @c free(). Only @c free_pool()
 *          may be used to destroy the pool metadata. The caller remains
 *          responsible for @p buffer.
 *
 * @sa init_dynamic_pool(), init_pool_with_arena(), init_static_arena(),
 *     alloc_pool(), return_pool_element(), free_pool(), pool_total_blocks(),
 *     pool_stride()
 *
 * @par Example
 * @code{.c}
 * // Statically allocate backing storage
 * enum { BUF = 64 * 1024 };
 * static unsigned char buf[BUF] __attribute__((aligned(64)));
 *
 * // Create a pool of 64-byte blocks inside caller-owned memory
 * pool_t* p = init_static_pool(buf, BUF, 64,  64);
 * assert(p != NULL);
 *
 * size_t cap = pool_total_blocks(p);
 *
 * // Allocate all blocks
 * for (size_t i = 0; i < cap; ++i) {
 *     void *b = alloc_pool(p);
 *     assert(b != NULL);
 * }
 *
 * // Pool is exhausted
 * errno = 0;
 * void *x = alloc_pool(p);
 * assert(x == NULL && errno == EPERM);
 *
 * free_pool(p); // does NOT free 'buf'
 * @endcode
 */
pool_t* init_static_pool(void*  buffer,
                         size_t buffer_bytes,
                         size_t block_size,
                         size_t alignment);
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
 * @param zeroed true if element is to be zeroed
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
 * void* a = alloc_pool(pool, false);   // OK
 * void* b = alloc_pool(pool, false);   // OK
 *
 * return_pool_element(pool, a); // returns 'a' to free-list
 *
 * void* c = alloc_pool(pool, true);   // likely returns 'a' again (LIFO reuse)
 *
 * free_arena(arena);            // releases pool header + all slices
 * @endcode
 */
void* alloc_pool(pool_t* pool, bool zeroed);
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
 * void* x = alloc_pool(p, false);
 * void* y = alloc_pool(p, false);
 *
 * return_pool_element(p, x);      // x goes to the head of the free list
 * void* z = alloc_pool(p, false);        // z == x (LIFO reuse)
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
 * void* x = alloc_pool(p, false);
 * void* y = alloc_pool(p, false);
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
 * void* x = alloc_pool(p, false);
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
 * void* x = alloc_pool(p, true);
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
size_t pool_block_size(const pool_t* pool);
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

size_t pool_stride(const pool_t* pool);
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

size_t pool_total_blocks(const pool_t* pool);
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
size_t pool_free_blocks(const pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the effective alignment used for blocks in this pool.
 *
 * Returns the alignment value applied when allocating blocks from the pool.
 * This is derived from the pool's requested alignment and the minimum
 * required alignment for storing an intrusive free-list pointer.
 *
 * @param pool  Valid pool handle.
 *
 * @return The pool's alignment in bytes on success.
 * @retval 0 and @c errno=EINVAL if @p pool is NULL or has no arena.
 *
 * @note The returned alignment is always >= alignof(void*).
 * @note Does not modify errno on success.
 *
 * @sa pool_block_size(), pool_stride()
 */
size_t pool_alignment(const pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the number of blocks still available in the active bump slice.
 *
 * Computes how many blocks remain before the pool must grow (dynamic pools),
 * or before further allocation fails (static pools).
 *
 * @param pool  Valid pool handle.
 *
 * @return Remaining blocks available in the current slice.
 * @retval 0 and @c errno=EINVAL if @p pool is NULL or has no arena.
 *
 * @note This value counts *only* unused bump-region blocks. Additional free
 *       blocks may exist on the free list; see @c pool_free_blocks().
 * @note Does not trigger any growth; this is a pure query.
 *
 * @sa alloc_pool(), pool_free_blocks(), pool_total_blocks()
 */
size_t pool_bump_remaining_blocks(const pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the number of blocks currently in use (not on the free list).
 *
 * @param pool  Valid pool handle.
 *
 * @return Number of allocated (live) blocks owned by the caller.
 * @retval 0 and @c errno=EINVAL if @p pool is NULL or has no arena.
 *
 * @note Computed as:
 *       @code
 *       pool_in_use = pool_total_blocks(pool) - pool_free_blocks(pool)
 *       @endcode
 *
 * @sa pool_total_blocks(), pool_free_blocks()
 */
size_t pool_in_use_blocks(const pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return whether this pool owns the underlying arena.
 *
 * @param pool  Valid pool handle.
 *
 * @return @c true if the pool owns its arena (created via init_dynamic_pool()
 *         or init_static_pool()), @c false otherwise.
 *
 * @warning When @c false, destroying the pool must *not* deallocate the arena.
 *          The caller remains responsible for arena lifetime management.
 *
 * @sa init_dynamic_pool(), init_static_pool(), free_pool()
 */
bool pool_owns_arena(const pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Query whether the pool is currently allowed to grow.
 *
 * @param pool  Valid pool handle.
 *
 * @return @c true if the pool may allocate new slices from the arena.
 * @return @c false if the pool is fixed-capacity.
 *
 * @note Growth may still fail at runtime if the arena is exhausted or
 *       if its internal resize policy disallows expansion.
 *
 * @sa toggle_pool_growth(), alloc_pool()
 */
bool pool_grow_enabled(const pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the memory type of the underlying arena.
 *
 * Indicates whether the arena backing the pool uses STATIC or DYNAMIC
 * allocation semantics, matching @c alloc_t from the arena API.
 *
 * @param pool  Valid pool handle.
 *
 * @return @c STATIC or @c DYNAMIC depending on the arena's memory type.
 * @retval a valid @c alloc_t value.
 *
 * @note Does not set errno. If @p pool is NULL, behavior is implementation-defined;
 *       you may choose to return @c STATIC as a safe default or assert in debug builds.
 *
 * @sa init_static_pool(), init_dynamic_pool(), arena_mtype()
 */
alloc_t pool_mtype(const pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Enable or disable pool growth (dynamic slice allocation).
 *
 * Controls whether future @c alloc_pool() calls may request additional slices
 * from the underlying arena. This function only updates policy; it does not
 * allocate memory itself.
 *
 * Behavior:
 *  - Disabling growth (@p toggle=false) always succeeds.
 *  - Enabling growth (@p toggle=true) succeeds only if:
 *        * Dynamic allocation support is enabled (ARENA_ENABLE_DYNAMIC),
 *        * The pool's arena is @c DYNAMIC,
 *        * The arena's own resize flag is enabled (@c arena->resize != 0).
 *    Otherwise, the pool remains non-growing and @c errno is set.
 *
 * @param pool   Pointer to a valid pool instance.
 * @param toggle @c true to enable pool growth, @c false to disable it.
 *
 * @retval void  On success or no-op condition.
 *
 * @note On failure, the pool growth setting remains unchanged.
 *
 * @par Errors
 * @c errno is set to:
 *   - @c EINVAL if @p pool or @c pool->arena is NULL.
 *   - @c ENOTSUP if the library was built without dynamic arena support.
 *   - @c EPERM if enabling growth on a static arena or on a dynamic arena
 *              whose own resize flag is disabled.
 *
 * @warning Enabling growth does not guarantee future allocations will succeed;
 *          it only permits @c alloc_pool() to request more memory. Allocation
 *          may still fail due to arena exhaustion or system allocation failure.
 *
 * @sa pool_grow_enabled(), alloc_pool(), init_dynamic_pool(), init_static_pool()
 */

void toggle_pool_growth(pool_t* pool, bool toggle);
// -------------------------------------------------------------------------------- 

/**
 * @brief Format a human-readable summary of a pool into @p buffer.
 *
 * Mirrors arena_stats() style and errno behavior:
 * - On invalid arguments (NULL buffer or size==0) => false, errno=EINVAL.
 * - If @p pool is NULL, writes "Pool: NULL\n" and returns true.
 * - All formatting uses the internal _buf_appendf() which protects against
 *   truncation and sets errno accordingly (ERANGE, EINVAL).
 *
 * The report includes:
 *  - Kind (STATIC/DYNAMIC) from underlying arena
 *  - Ownership (whether pool owns the arena object)
 *  - Growth policy (enabled/disabled)
 *  - Block size, stride, and effective alignment
 *  - Totals: total/free/in-use/bump-remaining blocks
 *  - Derived utilization (in-use / total)
 *  - Optional: slice list with [start,end) when DEBUG and slices are tracked
 */
bool pool_stats(const pool_t *pool, char *buffer, size_t buffer_size);
// -------------------------------------------------------------------------------- 

/**
 * @brief Capture the current state of a pool for later restoration.
 *
 * Creates a lightweight checkpoint of the pool's allocation state. The
 * checkpoint can later be passed to restore_pool() to rewind allocations
 * to this point in time.
 *
 * @param[in] pool  Pointer to the pool to checkpoint. May be NULL.
 *
 * @return Opaque PoolCheckpoint value. If @p pool is NULL, returns an
 *         empty checkpoint that restore_pool() will treat as a no-op.
 *
 * @note Checkpoints do not pin memory. They store only metadata (pointers
 *       and counters) for later validation and restoration.
 *
 * @note After restoring to a checkpoint, any pointers returned by
 *       alloc_pool() after the save point become invalid and must not
 *       be used.
 *
 * @warning Not thread-safe. If multiple threads access the pool while
 *          a checkpoint is being saved or restored, behavior is undefined.
 *
 * @sa restore_pool()
 *
 * @par Example: Transactional allocation
 * @code{.c}
 * pool_t* pool = init_dynamic_pool(64, 8, 128, 4096, 4096, true, true);
 * 
 * PoolCheckpoint cp = save_pool(pool);
 * 
 * // Allocate some objects for a transaction
 * void* obj1 = alloc_pool(pool, false);
 * void* obj2 = alloc_pool(pool, false);
 * 
 * if (!try_operation(obj1, obj2)) {
 *     // Operation failed - restore to checkpoint
 *     restore_pool(pool, cp);
 *     // obj1 and obj2 are now invalid
 * } else {
 *     // Success - keep the allocations
 * }
 * 
 * free_pool(pool);
 * @endcode
 */
PoolCheckPoint save_pool(const pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Restore a pool to a previously saved checkpoint state.
 *
 * Rewinds the pool's allocation state to the point captured by
 * save_pool(). Any blocks allocated after the checkpoint become
 * available for reuse (returned to the bump pointer region or
 * free list, depending on implementation).
 *
 * @param[in,out] pool  Pointer to the pool to restore. Must not be NULL.
 * @param[in]     cp    Checkpoint previously returned by save_pool().
 *
 * @return true on success, false on failure with errno set.
 *
 * @retval true  Pool successfully restored to checkpoint state.
 * @retval false Restoration failed (see errno).
 *
 * @retval false, errno=EINVAL
 *      - @p pool is NULL
 *      - Checkpoint is invalid or corrupted
 *      - Checkpoint's bump pointer is out of bounds
 *      - Pool has been reset or freed since checkpoint was saved
 *
 * @note After successful restoration:
 *       - All blocks allocated after the checkpoint are reclaimed
 *       - Pointers obtained after save_pool() must not be used
 *       - The free list may contain blocks that existed at save time
 *       - Statistics (total_blocks, free_blocks) are updated
 *
 * @note Empty checkpoints (from save_pool(NULL)) are treated as
 *       successful no-ops.
 *
 * @warning This function does NOT free memory back to the arena in
 *          dynamic pools. Slices allocated after the checkpoint remain
 *          allocated but become available for bump allocation.
 *
 * @warning Not thread-safe. External synchronization required if
 *          multiple threads access the pool.
 *
 * @sa save_pool()
 *
 * @par Example: Nested checkpoints
 * @code{.c}
 * pool_t* pool = init_dynamic_pool(32, 4, 64, 4096, 4096, true, true);
 * 
 * PoolCheckpoint cp1 = save_pool(pool);
 * void* a = alloc_pool(pool, false);
 * 
 * PoolCheckpoint cp2 = save_pool(pool);
 * void* b = alloc_pool(pool, false);
 * void* c = alloc_pool(pool, false);
 * 
 * // Restore to cp2: only 'a' remains allocated
 * restore_pool(pool, cp2);
 * 
 * // Restore to cp1: all allocations reclaimed
 * restore_pool(pool, cp1);
 * 
 * free_pool(pool);
 * @endcode
 */
bool restore_pool(pool_t* pool, PoolCheckPoint cp);
// -------------------------------------------------------------------------------- 

/**
 * @brief Get the total number of bytes currently in use.
 *
 * Returns the sum of all block payload bytes that are allocated
 * and not on the free list. This does NOT include:
 * - Alignment padding between blocks (stride - block_size)
 * - Blocks available in the bump region
 * - Blocks on the free list
 *
 * @param[in] pool  Pointer to the pool to query.
 *
 * @return Number of payload bytes in use, or 0 on error with errno set.
 *
 * @retval 0, errno=EINVAL  if @p pool is NULL
 */
size_t pool_size(const pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Get the total pool capacity in bytes.
 *
 * Returns the sum of all block payload bytes that have been made
 * available to this pool (including in-use, free, and bump regions).
 * This represents the usable capacity, NOT the total memory footprint.
 *
 * To get the actual memory footprint including alignment padding:
 * @code
 * size_t footprint = pool_total_blocks(pool) * pool_stride(pool);
 * @endcode
 *
 * @param[in] pool  Pointer to the pool to query.
 *
 * @return Total capacity in payload bytes, or 0 on error with errno set.
 *
 * @retval 0, errno=EINVAL  if @p pool is NULL
 */
size_t pool_alloc(const pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Get the total memory footprint of the pool in bytes.
 *
 * Returns the total bytes consumed by all pool blocks, including
 * alignment padding. This is the actual memory taken from the arena.
 *
 * Formula: total_blocks × stride
 *
 * Compare with pool_alloc() which returns only usable payload bytes.
 *
 * @param[in] pool  Pointer to the pool to query.
 *
 * @return Total memory footprint in bytes, or 0 on error with errno set.
 *
 * @retval 0, errno=EINVAL  if @p pool is NULL
 */
size_t pool_footprint(const pool_t* pool);
// -------------------------------------------------------------------------------- 

/**
 * @brief Verify whether a pointer belongs to a given pool.
 *
 * Determines if @p ptr falls within memory currently owned by @p pool.
 * In debug builds, this check validates against the pool’s tracked slices.
 * In release builds, it falls back to an arena-level containment check.
 *
 * @param pool  Pointer to a valid pool_t.
 * @param ptr   Pointer to check.
 *
 * @retval true  If @p ptr lies within the pool’s managed memory.
 * @retval false If @p ptr is outside the pool or invalid.
 *
 * @note In release builds, this function cannot distinguish between two pools
 *       sharing the same arena; it only verifies arena-level containment.
 *
 * @warning This does not validate alignment or active allocation state; it only
 *          checks spatial containment.
 */
bool is_pool_ptr(const pool_t* pool, const void* ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Query whether a pool owns the underlying arena memory.
 *
 * A pool may either:
 * - **Own its arena** — as in the case of `init_dynamic_pool()` or
 *   `init_static_pool()` where the pool internally creates or embeds
 *   an `arena_t`, *or*
 * - **Borrow an arena** — as in `init_pool_with_arena()`, where the
 *   caller provides an existing arena to carve the pool header and
 *   slices from.
 *
 * This function distinguishes those cases:
 *
 * - If the pool **owns** its arena, calling `free_pool(pool)` will free the arena
 *   and all memory associated with the pool.
 * - If the pool **borrows** its arena, calling `free_pool(pool)` will *not*
 *   free the arena; the pool header is invalidated, but the caller remains
 *   responsible for destroying the arena.
 *
 * @param pool
 *     Pointer to a valid `pool_t` instance.
 *
 * @return `true` if the pool owns its arena;  
 *         `false` if it borrows an external arena or if `pool == NULL`.
 *
 * @retval false, errno = EINVAL  
 *     If `pool` is `NULL`.
 *
 * @code{.c}
 * // Example:
 * pool_t *p1 = init_dynamic_pool(64, 0, 16, 8192, 4096, true, true);
 * assert(pool_owns_memory(p1) == true);   // owns arena
 *
 * arena_t *a = init_dynamic_arena(8192, true, 4096, 0);
 * pool_t *p2 = init_pool_with_arena(a, 64, 0, 16, true, true);
 * assert(pool_owns_memory(p2) == false);  // borrows arena
 *
 * free_pool(p1);
 * free_pool(p2);   // does NOT free 'a'
 * free_arena(a);
 * @endcode
 */
bool pool_owns_memory(const pool_t* pool);
// ================================================================================ 
// ================================================================================ 
// POOL MACROS 

#if ARENA_USE_CONVENIENCE_MACROS
/**
 * \def alloc_pool_type(T, pool)
 * @brief Allocate one object of type `T` from a memory pool.
 *
 * This macro wraps ::alloc_pool and casts the result to `T*`, improving
 * readability and reducing repetitive casting in caller code.
 *
 * - Obtains one block from `pool` via `alloc_pool(pool)`.
 * - Casts the result to `T*`.
 * - Returns `NULL` on failure; `errno` is set by the allocator.
 *
 * @note
 * - Caller must initialize the returned object.
 * - Memory must be returned with ::return_pool_element.
 * - Only valid when the pool's block size ≥ `sizeof(T)`.
 *
 * @param T     The C type to allocate (e.g., `MyStruct`)
 * @param pool  Pointer to an initialized `pool_t` allocator
 *
 * @return `T*` on success, or `NULL` on failure.
 *
 * @code{.c}
 * typedef struct { float x, y, z; } vec3;
 *
 * pool_t* p = init_dynamic_pool(sizeof(vec3), 0, 64, 8192, 4096, true, true);
 * vec3* v = alloc_pool_type(vec3, p);
 * if (!v) { perror("alloc_pool_type"); exit(EXIT_FAILURE); }
 *
 * v->x = 1; v->y = 2; v->z = 3;
 *
 * return_pool_element(p, v);
 * free_pool(p);
 * @endcode
 */
#define alloc_pool_type(T, pool) ((T*)alloc_pool(pool, false))
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for pool allocation.
 *
 * Allocates a single fixed-size element from the pool referenced by @p ctx.
 * Pool allocators do not support variable-sized allocation; therefore
 * @p size is validated only to ensure it does not exceed the pool's block size.
 *
 * If @p zeroed is true, the returned block will be zero-initialized.
 *
 * @param ctx   Pointer to a pool_t instance.
 * @param size  Requested size (must be <= pool_block_size()).
 * @param zeroed Whether the element should be zero-initialized.
 *
 * @retval void* Pointer to the allocated pool element.
 * @retval NULL  On error (errno set to EINVAL).
 */
static inline void* pool_v_alloc(void* ctx, size_t size, bool zeroed) {
    (void) size;
    pool_t* pool = (pool_t*)ctx;
    if (!pool) {
        errno = EINVAL;
        return NULL;
    }
    if (size > pool_block_size(pool)) {
        errno = EINVAL;   // requested more than a pool block can hold
        return NULL;
    }
    return alloc_pool(pool, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for aligned pool allocation.
 *
 * Pool allocators inherently provide alignment guarantees based on the
 * block size and internal structure, and do not support arbitrary alignment
 * requests. Therefore, @p align is ignored.
 *
 * If @p zeroed is true, the returned block will be zero-initialized.
 *
 * @param ctx     Pointer to a pool_t instance.
 * @param size    Requested size (ignored except for compatibility).
 * @param align   Requested alignment (ignored).
 * @param zeroed  Whether the element should be zero-initialized.
 *
 * @return Pointer to a pool element, or NULL on error.
 */
static inline void* pool_v_alloc_aligned(void* ctx, size_t size,
                                         size_t align, bool zeroed) {
    (void) size;
    (void) align;
    pool_t* pool = (pool_t*)ctx;
    return alloc_pool(pool, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for pool reallocation.
 *
 * Reallocation is not supported by pool allocators because block sizes
 * are fixed. This function simply returns @p old_ptr unchanged.
 *
 * @param ctx       Pointer to a pool_t instance (unused).
 * @param old_ptr   The previously allocated block.
 * @param old_size  Previous allocation size (ignored).
 * @param new_size  Requested new size (ignored).
 * @param zeroed    Whether to zero-init expanded memory (ignored).
 *
 * @return Always returns @p old_ptr.
 */
static inline void* pool_v_realloc(void* ctx, void* old_ptr,
                                   size_t old_size, size_t new_size,
                                   bool zeroed) {
    (void)ctx; (void)old_size; (void)new_size; (void)zeroed;
    return old_ptr;
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for aligned pool reallocation.
 *
 * Like pool_v_realloc(), pool allocators do not support resizing or
 * alignment-based reallocation. The function simply returns @p old_ptr.
 *
 * @param ctx       Pointer to a pool_t instance (unused).
 * @param old_ptr   Previously allocated pool element.
 * @param old_size  Previous size (ignored).
 * @param new_size  Requested size (ignored).
 * @param zeroed    Zero-initialize flag (ignored).
 * @param align     Alignment (ignored).
 *
 * @return Always returns @p old_ptr.
 */
static inline void* pool_v_realloc_aligned(void* ctx, void* old_ptr,
                                           size_t old_size, size_t new_size,
                                           bool zeroed, size_t align) {
    (void)ctx; (void)old_size; (void)new_size; (void)zeroed; (void)align;
    return old_ptr;
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Returns a previously allocated pool element to the pool.
 *
 * Pool allocators support element-level return operations. This returns
 * @p ptr to the pool referenced by @p ctx for reuse.
 *
 * If either @p ctx or @p ptr is NULL, errno is set to EINVAL and the
 * function has no effect.
 *
 * @param ctx  Pointer to a pool_t instance.
 * @param ptr  Pool element previously returned by alloc_pool().
 */
static inline void pool_v_return(void* ctx, void* ptr) {
    pool_t* pool = (pool_t*)ctx;
    if (!pool || !ptr) {
        errno = EINVAL;
        return;
    }
    return_pool_element(pool, ptr);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Frees all memory owned by the pool allocator.
 *
 * Destroys the pool referenced by @p ctx, releasing all associated
 * memory back to the system. After this call, the pool must not be used.
 *
 * If @p ctx is NULL, errno is set to EINVAL.
 *
 * @param ctx Pointer to a pool_t instance.
 */
static inline void pool_v_free(void* ctx) {
    pool_t* pool = (pool_t*)ctx;
    if (!pool) {
        errno = EINVAL;
        return;
    }
    free_pool(pool);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Constructs an allocator_vtable_t for a given pool.
 *
 * This helper initializes a vtable that exposes the pool allocator
 * through the generic allocator interface. All allocation operations
 * forward to pool-compatible functions.
 *
 * @param p Pointer to a pool_t instance used as the allocator backend.
 *
 * @return A fully initialized allocator_vtable_t referring to @p p.
 */
static inline allocator_vtable_t pool_allocator(pool_t* p) {
    allocator_vtable_t v = {
        .allocate          = pool_v_alloc,
        .allocate_aligned  = pool_v_alloc_aligned,
        .reallocate        = pool_v_realloc,
        .reallocate_aligned= pool_v_realloc_aligned,
        .return_element    = pool_v_return,
        .deallocate        = pool_v_free,
        .ctx               = p
    };
    return v;
}
// ================================================================================ 
// ================================================================================ 
// FREE LIST IMPLEMENTATION 

typedef struct freelist_t freelist_t;
// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a freelist allocator using memory obtained from an existing arena.
 *
 * This function creates a `freelist_t` inside the caller-provided `arena_t`.
 * The freelist itself does not own the memory; the arena remains responsible
 * for its lifetime. All allocations made through the freelist draw from a
 * single contiguous region carved out of the arena via a single `alloc_arena()`
 * call.
 *
 * The freelist memory layout consists of:
 *
 *     [ freelist_t struct | aligned user memory region ]
 *
 * Within the memory region, allocations are managed using variable-sized
 * blocks. Returned blocks are coalesced whenever possible to limit external
 * fragmentation.
 *
 * @param arena
 *        Pointer to an initialized `arena_t`. The freelist is allocated
 *        directly from this arena. Must not be NULL.
 *
 * @param size
 *        Requested number of usable bytes for the freelist (not including
 *        internal metadata). Must be at least `freelist_min_request`. If the
 *        region rounds up to an alignment boundary, the actual usable size may
 *        be slightly larger than requested.
 *
 * @param alignment
 *        Desired alignment for both the freelist structure and its internal
 *        memory region. If zero, `alignof(max_align_t)` is used. Non–power-of-two
 *        values are rounded up. The final alignment is always at least
 *        `alignof(max_align_t)`.
 *
 * @return
 *        Pointer to a fully initialized `freelist_t` on success, or NULL on
 *        error. In case of failure, `errno` is set to:
 *
 *        - `EINVAL`  if inputs are invalid  
 *        - `ENOMEM`  if the arena does not have enough space  
 *
 * @note
 *        The freelist does not grow. Its capacity is fully determined by the
 *        initial `size` request and available arena space.
 *
 * @note
 *        The returned freelist shares the arena's lifetime. When the arena is
 *        reset or destroyed, all freelist allocations become invalid.
 *
 * @par Example
 * Creating a freelist inside an arena:
 *
 * @code{.c}
 * #include "arena.h"
 * #include "freelist.h"
 *
 * int main(void) {
 *     uint8_t buffer[2048];
 *
 *     // Create a static arena over a fixed buffer.
 *     arena_t* arena =
 *         init_static_arena(buffer, sizeof buffer, alignof(max_align_t));
 *     if (!arena) return 1;
 *
 *     // Create a freelist that uses 1024 bytes from the arena.
 *     freelist_t* fl = init_freelist_with_arena(arena, 1024, 0);
 *     if (!fl) return 1;
 *
 *     // Allocate memory from the freelist
 *     const size_t N = 64;
 *     void* p = alloc_freelist(fl, N, true);
 *
 *     // ... use p ...
 *
 *     // Free the memory back to the freelist
 *     return_freelist_element(fl, p);
 *
 *     // Freelist is automatically invalidated when the arena is reset/destroyed
 *     reset_arena(arena);
 *     return 0;
 * }
 * @endcode
 */
freelist_t* init_freelist_with_arena(arena_t* arena, 
                                     size_t size, 
                                     size_t alignment);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a dynamically backed freelist allocator.
 *
 * This function allocates a new `arena_t` and constructs a `freelist_t` inside
 * it. The freelist *owns* the arena and will release it when
 * `free_freelist()` is called. All memory used by the freelist—including the
 * freelist structure itself and all future allocations—resides inside this
 * dedicated arena.
 *
 * The caller specifies a desired usable payload size (`bytes`). The allocator
 * then calculates the minimum total memory required to store:
 *
 *   - an aligned `freelist_t` structure  
 *   - at least one free block (`free_block_t`)  
 *   - the requested usable region  
 *
 * The underlying dynamic arena may provide more than the requested minimum;
 * any extra space is incorporated into the freelist.
 *
 * @param bytes
 *        Requested minimum usable payload size (not counting internal
 *        metadata). Must be at least `freelist_min_request`. If the value is
 *        too small to hold control structures, initialization fails and
 *        `errno` is set to `EINVAL`.
 *
 * @param alignment
 *        Desired alignment for the freelist header and all internal
 *        allocations. If zero, `alignof(max_align_t)` is used. Non–power-of-two
 *        values are rounded up. The resulting alignment is guaranteed to be at
 *        least `alignof(max_align_t)`.
 *
 * @param resize
 *        Whether the underlying dynamic arena is permitted to grow. In the
 *        current freelist design, the freelist itself remains *fixed-size* once
 *        constructed, but future versions may allow growth when this flag is
 *        enabled. Passing `false` ensures a strictly fixed-size allocator.
 *
 * @return
 *        A pointer to a fully initialized `freelist_t` on success, or NULL on
 *        failure. In case of error, `errno` is set to:
 *
 *        - `EINVAL`  — invalid arguments or size too small  
 *        - `ENOMEM`  — underlying arena could not allocate sufficient space  
 *        - `ENOTSUP` — dynamic arenas disabled (`ARENA_ENABLE_DYNAMIC = 0`)  
 *
 * @note
 *        The freelist takes ownership of the newly created `arena_t`. Call
 *        `free_freelist()` to release all memory associated with the freelist.
 *
 * @note
 *        The actual usable size (`freelist_alloc(fl)`) may be larger than
 *        requested, depending on how the underlying dynamic arena chooses its
 *        chunk sizes.
 *
 * @par Example
 * Creating a standalone freelist with dynamic allocation:
 *
 * @code{.c}
 * #include "arena.h"
 * #include "freelist.h"
 *
 * int main(void) {
 *     // Request at least 4096 bytes of usable space
 *     freelist_t* fl = init_dynamic_freelist(4096, 0, false);
 *     if (!fl) {
 *         fprintf(stderr, "Freelist init failed: %s\n", strerror(errno));
 *         return 1;
 *     }
 *
 *     // Allocate memory from the freelist
 *     void* p = alloc_freelist(fl, 128, true);
 *     if (!p) return 1;
 *
 *     // Use memory...
 *
 *     // Free the element back to the freelist
 *     return_freelist_element(fl, p);
 *
 *     // Destroy the freelist and its arena
 *     free_freelist(fl);
 *     return 0;
 * }
 * @endcode
 */
freelist_t* init_dynamic_freelist(size_t bytes, size_t alignment, bool resize);
// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a freelist allocator over a user-supplied static buffer.
 *
 * This function constructs a `freelist_t` using memory provided entirely by the
 * caller. No heap allocation occurs. The freelist does **not** take ownership
 * of the buffer; the caller retains responsibility for ensuring that the buffer
 * remains valid for the freelist’s lifetime.
 *
 * A static freelist is ideal for embedded systems, scratch buffers, fixed-size
 * workspaces, or any environment where dynamic allocation is restricted.
 *
 * The memory layout inside the buffer is:
 *
 *     [ aligned freelist_t | aligned free region for allocations ]
 *
 * The free region is placed immediately after the freelist header and begins as
 * one large free block. All allocations and frees operate within this region.
 *
 * @param buffer
 *        Pointer to the start of the user-supplied memory region. This buffer
 *        must be at least `min_freelist_alloc()` bytes *plus* enough space for
 *        a `freelist_t` header. Must not be NULL.
 *
 * @param bytes
 *        Total size of the buffer. Must be large enough to contain:
 *
 *            sizeof(freelist_t) + sizeof(free_block_t)
 *
 *        and preferably more, to provide usable allocation space. If `bytes` is
 *        smaller than the minimum required size, initialization fails with
 *        `errno = EINVAL`.
 *
 * @param alignment
 *        Required alignment for the freelist header and all internal blocks.
 *        If zero, `alignof(max_align_t)` is used. Non–power-of-two values are
 *        rounded up. The final alignment always satisfies:
 *
 *            alignment >= alignof(max_align_t)
 *
 * @return
 *        Pointer to an initialized `freelist_t` on success, or NULL on failure.
 *        In case of failure:
 *
 *        - `EINVAL`  — invalid buffer, size too small, or bad alignment  
 *        - `ENOMEM`  — unexpected allocation failure inside arena creation  
 *
 * @note
 *        The freelist *does not* own the memory region. Destroying the freelist
 *        does not free the buffer.
 *
 * @note
 *        The `arena_t` created internally wraps the caller’s buffer and is also
 *        non-owning. The freelist simply carves out its own region from that
 *        static arena.
 *
 * @par Example
 * Creating a freelist over a static buffer:
 *
 * @code{.c}
 * #include "arena.h"
 * #include "freelist.h"
 *
 * int main(void) {
 *     // Allocate a static buffer
 *     uint8_t buffer[2048];
 *
 *     // Create a freelist using the buffer
 *     freelist_t* fl = init_static_freelist(buffer, sizeof buffer, 0);
 *     if (!fl) {
 *         fprintf(stderr, "Static freelist init failed: %s\n", strerror(errno));
 *         return 1;
 *     }
 *
 *     // Allocate memory from the freelist
 *     void* p = alloc_freelist(fl, 128, true);
 *     if (!p) return 1;
 *
 *     // Use memory...
 *
 *     // Return memory to freelist
 *     return_freelist_element(fl, p);
 *
 *     // Because the freelist is static, no destruction call is required.
 *     // The caller owns the buffer and may reuse or discard it at will.
 *
 *     return 0;
 * }
 * @endcode
 */
freelist_t* init_static_freelist(void* buffer, size_t bytes, size_t alignment);
// -------------------------------------------------------------------------------- 

/**
 * @brief Destroy a dynamically allocated freelist and its underlying arena.
 *
 * This function frees a freelist created with `init_dynamic_freelist()`.  
 * Only dynamic freelists own their underlying memory; static freelists do not.
 *
 * After calling this function:
 *   - All memory allocated from the freelist becomes invalid.
 *   - The `freelist_t` object itself is invalid, because it lives inside
 *     the freed arena.
 *
 * Static freelists (those created with `init_freelist_with_arena()` or
 * `init_static_freelist()`) must not be passed to this function.
 *
 * @param fl
 *        Pointer to a freelist. Must refer to a dynamically allocated freelist.
 *
 * @retval void
 *         The function does not return a value. On failure it sets errno:
 *
 *         - `EINVAL` : `fl` is NULL or its parent arena pointer is NULL  
 *         - `EPERM`  : freelist does not own its memory (i.e., static freelist)
 *
 * @note
 *        This function only applies to freelists that own their arena.
 *        Attempting to free a static freelist does nothing and sets errno.
 *
 * @code{.c}
 * freelist_t* fl = init_dynamic_freelist(2048, 0, false);
 * if (!fl) return 1;
 *
 * void* p = alloc_freelist(fl, 128, false);
 * if (!p) return 1;
 *
 *
 * free_freelist(fl);
 *
 * @endcode
 */

void free_freelist(freelist_t* fl);
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate a block of memory from a freelist.
 *
 * This function allocates `bytes` of user-visible memory from a freelist,
 * returning a pointer that is aligned to the freelist's effective alignment.
 * Internally, the allocator may consume more than `bytes` due to:
 *
 *   - a small header (`freelist_header_t`) stored immediately before the
 *     returned user pointer
 *   - alignment padding between the free block and the user region
 *   - full-block consumption if the remaining tail fragment is too small
 *     to form another free block
 *
 * These internal details are tracked automatically and do not affect the
 * pointer returned to the caller.
 *
 * Memory returned by this function can later be freed with
 * `return_freelist_element()`, which restores the block to the free list
 * and performs coalescing to reduce external fragmentation.
 *
 * @param fl
 *        Pointer to a valid `freelist_t`. Must not be NULL.
 *
 * @param bytes
 *        Number of user bytes requested. Must be greater than zero.
 *
 * @param zeroed
 *        If true, the returned block's user region is zero-initialized.
 *
 * @return
 *        Pointer to an aligned user region on success, or NULL on error.  
 *        In case of failure, `errno` is set to:
 *
 *        - `EINVAL` — invalid arguments or overflow in size computation  
 *        - `ENOMEM` — no suitable free block is available  
 *
 * @note
 *        The returned pointer is always aligned to at least
 *        `freelist_alignment(fl)`, but internal padding may increase the
 *        actual size charged against freelist capacity.
 *
 * @note
 *        The freelist may internally split free blocks or consume them fully
 *        to minimize unusable tail fragments.
 *
 * @code{.c}
 * freelist_t* fl = init_dynamic_freelist(2048, 0, false);
 * if (!fl) return 1;
 *
 * void* p = alloc_freelist(fl, 128, true);
 * if (!p) return 1;
 *
 * // p is aligned and safe to use
 *
 * // Free the memory back to the freelist
 * return_freelist_element(fl, p);
 *
 * // Destroy freelist and its arena
 * free_freelist(fl);
 * @endcode
 */
void* alloc_freelist(freelist_t* fl, size_t size, bool zero);
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate an aligned block of memory from a freelist.
 *
 * This function behaves like ::alloc_freelist, but allows the caller to
 * request a specific alignment for the returned pointer. The effective
 * alignment is computed as:
 *
 *   - if @p alignment is zero, the freelist's base alignment is used
 *   - if @p alignment is not a power of two, it is rounded up
 *   - the final alignment is at least ::freelist_alignment(fl)
 *
 * Internally, the allocator may consume more memory than the requested
 * @p bytes due to:
 *
 *   - the freelist header stored immediately before the user pointer
 *   - padding required to satisfy the requested alignment
 *   - full-block consumption when the tail fragment would be too small
 *     to form another free block
 *
 * The caller only sees a pointer to a region of size @p bytes, aligned
 * to the requested (or stricter) alignment.
 *
 * @param fl
 *        Pointer to a valid ::freelist_t. Must not be NULL.
 *
 * @param bytes
 *        Number of user bytes requested. Must be greater than zero.
 *
 * @param alignment
 *        Desired alignment for the returned pointer. If zero, the freelist's
 *        base alignment is used. Non–power-of-two values are rounded up.
 *        The resulting alignment is always at least ::freelist_alignment(fl).
 *
 * @param zeroed
 *        If true, the user region is zero-initialized before being returned.
 *
 * @return
 *        Pointer to an aligned user region on success, or NULL on error.
 *        On failure, errno is set to:
 *
 *        - EINVAL  if arguments are invalid or alignment normalization fails
 *        - ENOMEM  if no suitable free block is available
 *
 * @note
 *        The returned pointer can be freed with ::return_freelist_element.
 *        The alignment requested here does not change the freelist's global
 *        accounting semantics.
 *
 * @code{.c}
 * freelist_t* fl = init_dynamic_freelist(4096, 0, false);
 * if (!fl) return 1;
 *
 * size_t align = 64;
 * void* p = alloc_freelist_aligned(fl, 256, align, true);
 * if (!p) return 1;
 *
 * return_freelist_element(fl, p);
 * free_freelist(fl);
 * @endcode
 */
void* alloc_freelist_aligned(freelist_t* fl,
                             size_t      bytes,
                             size_t      alignment,
                             bool        zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Resize an allocation managed by a freelist.
 *
 * This function behaves similarly to the standard C library `realloc()`,
 * but operates on memory obtained from a ::freelist_t. The behavior is:
 *
 *   - If @p ptr is NULL, this is equivalent to ::alloc_freelist.
 *   - If @p new_size <= @p old_size, the existing pointer is returned
 *     unchanged (the block is not shrunk).
 *   - If the block must grow, a new block is allocated, the first
 *     @p old_size bytes are copied, the old block is freed via
 *     ::return_freelist_element, and the new pointer is returned.
 *
 * The returned pointer is always aligned to ::freelist_alignment(fl).
 *
 * @param fl
 *        Pointer to a valid freelist. Must not be NULL.
 *
 * @param ptr
 *        Existing user pointer previously returned by ::alloc_freelist.
 *        May be NULL.
 *
 * @param old_size
 *        Number of user bytes originally allocated at @p ptr.
 *        The caller must supply this value; it is not stored by the freelist.
 *
 * @param new_size
 *        Number of user bytes requested for the resized allocation.
 *
 * @param zeroed
 *        If true, any newly added region (beyond old_size) is zero-filled.
 *
 * @return
 *        Pointer to a resized allocation on success, or NULL on failure.
 *        On error, errno is set to:
 *
 *        - EINVAL : invalid freelist pointer or size arguments  
 *        - ENOMEM : insufficient free space to grow the block  
 *
 * @note
 *        The freelist does not currently support in-place growth; all expansions
 *        require allocation+copy+free.
 *
 * @note
 *        The caller must track old_size accurately. The freelist does not
 *        store user-requested sizes internally.
 *
 * @code{.c}
 * freelist_t* fl = init_dynamic_freelist(4096, 0, false);
 * void* p = alloc_freelist(fl, 64, false);
 *
 * // Grow from 64 bytes to 200 bytes
 * void* q = realloc_freelist(fl, p, 64, 200, true);
 * if (!q) return 1;
 *
 * return_freelist_element(fl, q);
 * free_freelist(fl);
 * @endcode
 */
void* realloc_freelist(freelist_t* fl, void* variable, size_t old_size, size_t new_size, bool zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Resize an aligned freelist allocation.
 *
 * This function extends ::realloc_freelist by allowing the caller to specify
 * a desired alignment for the new allocation. The behavior mirrors standard
 * `realloc()` semantics with the following rules:
 *
 *   - If @p ptr is NULL, the call behaves like ::alloc_freelist_aligned.
 *   - If @p new_size <= @p old_size, the original pointer is returned
 *     unchanged (no shrinking occurs).
 *   - If the block must grow, a new aligned block is allocated, the first
 *     @p old_size bytes are copied, and the old block is freed.
 *
 * The returned pointer is aligned to at least:
 *
 *     max(requested_alignment, freelist_alignment(fl))
 *
 * @param fl
 *        Pointer to a valid freelist. Must not be NULL.
 *
 * @param ptr
 *        Existing aligned pointer previously returned by
 *        ::alloc_freelist_aligned or ::alloc_freelist.
 *
 * @param old_size
 *        Number of user bytes originally allocated at @p ptr.
 *
 * @param new_size
 *        Number of user bytes requested for the resized allocation.
 *
 * @param zeroed
 *        If true, any newly added region (beyond old_size) is zero-filled.
 *
 * @param alignment
 *        Desired alignment for the returned pointer. Non–power-of-two values
 *        are rounded up; final alignment is always at least ::freelist_alignment(fl).
 *
 * @return
 *        Pointer to a resized, aligned block on success, or NULL on failure.
 *        On error, errno is set to:
 *
 *        - EINVAL : invalid freelist or alignment  
 *        - ENOMEM : insufficient free space  
 *
 * @note
 *        Like ::realloc_freelist, this function never shrinks blocks.
 *
 * @note
 *        If @p ptr was allocated with a lower alignment than requested here,
 *        the returned block may move.
 *
 * @code{.c}
 * freelist_t* fl = init_dynamic_freelist(4096, 0, false);
 * void* p = alloc_freelist_aligned(fl, 128, 64, false);
 *
 * // Grow to 512 bytes, keeping 64-byte alignment
 * void* q = realloc_freelist_aligned(fl, p, 128, 512, true, 64);
 * if (!q) return 1;
 *
 * return_freelist_element(fl, q);
 * free_freelist(fl);
 * @endcode
 */
void* realloc_freelist_aligned(freelist_t* fl,
                               void*       ptr,
                               size_t      old_size,
                               size_t      new_size,
                               bool        zeroed,
                               size_t      alignment);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a previously allocated block to the freelist.
 *
 * This function frees a block allocated by ::alloc_freelist or
 * ::alloc_freelist_aligned and reinserts it into the freelist as a
 * free block. If adjacent free blocks exist immediately before or after
 * the returned region, they are automatically coalesced, reducing
 * external fragmentation.
 *
 * The function verifies:
 *
 *   - @p ptr is non-NULL  
 *   - @p ptr lies inside the freelist's managed memory region  
 *   - A valid freelist header exists immediately before @p ptr  
 *   - The block's recorded @c block_size and @c offset are sane  
 *   - The block fits entirely within the freelist memory region  
 *
 * On successful return, the freelist's internal accounting (`len`) is
 * decreased by exactly the block size originally charged during allocation.
 *
 * @param fl
 *        Pointer to the freelist that originally allocated @p ptr.
 *        Must not be NULL.
 *
 * @param ptr
 *        Pointer previously returned by a freelist allocation. Must not
 *        be NULL and must refer to a currently allocated block.
 *
 * @return
 *        Nothing. On error, `errno` is set to:
 *
 *        - EINVAL : @p fl is NULL  
 *        - EINVAL : @p ptr is NULL  
 *        - EINVAL : @p ptr does not lie inside the freelist region  
 *        - EINVAL : allocation header is corrupt or inconsistent  
 *        - EINVAL : block extends beyond freelist bounds  
 *        - EINVAL : freeing more bytes than the freelist thinks were used  
 *
 * @note
 *        Double frees, frees of foreign pointers, and corrupted metadata
 *        are all detected and rejected.
 *
 * @note
 *        This function performs in-place free-block coalescing whenever
 *        adjacent blocks are physically contiguous.
 *
 * @warning
 *        After calling this function, @p ptr becomes invalid and must not
 *        be dereferenced.
 *
 * @code{.c}
 * freelist_t* fl = init_dynamic_freelist(4096, 0, false);
 * void* p = alloc_freelist(fl, 128, false);
 *
 * // Return p to the freelist
 * return_freelist_element(fl, p);
 *
 * // p must not be used after this point
 *
 * free_freelist(fl);
 * @endcode
 */
void return_freelist_element(freelist_t* fl, void* ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reset a freelist to its initial empty state.
 *
 * This function clears all allocation state within a ::freelist_t, restoring
 * it to a pristine, fully empty allocator. All previously allocated blocks
 * become invalid and may not be used after the reset.
 *
 * The freelist’s internal region is rebuilt as a single large free block:
 *
 *     [ free_block_t(size = fl->alloc) ]
 *
 * No memory is freed back to an arena in this operation; it only resets
 * the freelist’s internal bookkeeping. If the freelist was created inside
 * an arena (via ::init_freelist_with_arena or ::init_static_freelist),
 * the arena itself remains unchanged.
 *
 * @param fl
 *        Pointer to an initialized freelist. Must not be NULL. The freelist
 *        must have a valid `memory` region and non-zero capacity.
 *
 * @return
 *        Nothing. On error, `errno` is set to:
 *
 *        - EINVAL : `fl` is NULL or not properly initialized
 *
 * @note
 *        This operation invalidates all outstanding freelist allocations.
 *
 * @code{.c}
 * freelist_t* fl = init_dynamic_freelist(4096, 0, false);
 * void* p = alloc_freelist(fl, 128, false);
 *
 * // Reset the freelist — p becomes invalid
 * reset_freelist(fl);
 *
 * void* q = alloc_freelist(fl, 128, false);
 * return_freelist_element(fl, q);
 * free_freelist(fl);
 * @endcode
 */
void reset_freelist(freelist_t* fl);
// -------------------------------------------------------------------------------- 

/**
 * @brief Validate whether a pointer was allocated by this freelist.
 *
 * This function performs a strict verification that @p ptr corresponds to a
 * currently active allocation originating from the given ::freelist_t.
 *
 * Validation includes:
 *
 *   - Pointer lies inside the freelist’s managed memory region  
 *   - There is a valid allocation header immediately preceding the pointer  
 *   - The recorded block size and offset form a consistent block layout  
 *   - The full block fits entirely inside the freelist region  
 *
 * This check detects:
 *
 *   - Pointers from other allocators  
 *   - Off-by-one or misaligned pointers  
 *   - Double frees  
 *   - Corrupted block metadata  
 *
 * @param fl
 *        The freelist to validate against. Must not be NULL.
 *
 * @param ptr
 *        The candidate pointer. Must not be NULL.
 *
 * @return
 *        true  if the pointer appears valid and belongs to @p fl  
 *        false otherwise  
 *
 * @note
 *        This function does not check whether the block is still allocated
 *        or already freed; it only checks if @p ptr could have been returned
 *        by the freelist.
 *
 * @code{.c}
 * freelist_t* fl = init_dynamic_freelist(2048, 0, false);
 * void* p = alloc_freelist(fl, 64, false);
 *
 * bool ok1 = is_freelist_ptr(fl, p);       // true
 * bool ok2 = is_freelist_ptr(fl, p + 1);   // false
 *
 * return_freelist_element(fl, p);
 * free_freelist(fl);
 * @endcode
 */
bool is_freelist_ptr(const freelist_t* fl, const void* ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Validate that a freelist pointer is valid and large enough for a size.
 *
 * This function extends ::is_freelist_ptr by additionally ensuring that the
 * memory block referenced by @p ptr contains at least @p size user bytes.
 *
 * The freelist records, for each block:
 *
 *     block_size = total block size (header + padding + user)
 *     offset     = distance from block_start to user_ptr
 *
 * The available user payload is:
 *
 *     user_data_size = block_size - offset
 *
 * This function verifies that:
 *
 *   - @p ptr is a plausible freelist allocation  
 *   - @p size does not exceed @c user_data_size  
 *   - @p ptr + size remains inside the freelist memory region  
 *
 * @param fl
 *        Freelist allocator to validate against. Must not be NULL.
 *
 * @param ptr
 *        Candidate pointer previously returned by a freelist allocation.
 *
 * @param size
 *        Number of bytes the caller intends to use starting at @p ptr.
 *
 * @return
 *        true  if @p ptr is valid for the freelist and has at least @p size bytes  
 *        false otherwise  
 *
 * @code{.c}
 * freelist_t* fl = init_dynamic_freelist(4096, 0, false);
 * void* p = alloc_freelist(fl, 128, false);
 *
 * bool ok1 = is_freelist_ptr_sized(fl, p, 64);   // true
 * bool ok2 = is_freelist_ptr_sized(fl, p, 200);  // false
 *
 * return_freelist_element(fl, p);
 * free_freelist(fl);
 * @endcode
 */
bool is_freelist_ptr_sized(const freelist_t* fl, const void* ptr, size_t size);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the remaining capacity (in bytes) of a freelist.
 *
 * This function reports how many bytes of the freelist's usable region
 * are still available for future allocations, according to the internal
 * accounting `alloc - len`.
 *
 * Note that `len` counts the *full block size* of allocated blocks
 * (including header and padding), not just the user-requested payload.
 * Therefore, `freelist_remaining(fl)` is a logical capacity measure,
 * not simply "sum of free fragments".
 *
 * @param fl
 *        Pointer to a valid freelist. Must not be NULL.
 *
 * @return
 *        Number of remaining bytes (`size_t`) that can be consumed by new
 *        allocations, or 0 on error. On failure, errno is set to:
 *
 *        - EINVAL : @p fl is NULL
 *
 * @code{.c}
 * size_t before = freelist_remaining(fl);
 * void* p = alloc_freelist(fl, 128, false);
 * size_t after  = freelist_remaining(fl);
 * @endcode
 */
size_t freelist_remaining(const freelist_t* fl);
// -------------------------------------------------------------------------------- 

/**
 * @brief Query the underlying allocation type used by the freelist's arena.
 *
 * This function forwards to ::arena_mtype on the freelist's parent arena.
 * It allows callers to distinguish between static and dynamic backing
 * stores (or other allocation modes defined by ::alloc_t).
 *
 * @param fl
 *        Pointer to a freelist. Must not be NULL.
 *
 * @return
 *        An ::alloc_t value describing the arena's allocation mode, or
 *        ::ALLOC_INVALID if @p fl is NULL. On error, errno is set to:
 *
 *        - EINVAL : @p fl is NULL
 *
 * @note
 *        Typical values include STATIC and DYNAMIC, depending on how
 *        the arena was initialized.
 */
alloc_t freelist_mtype(const freelist_t* fl);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the total number of bytes currently consumed by the freelist.
 *
 * This function reports how many bytes of the freelist's usable region are
 * currently in use, according to the internal accounting field `len`.
 *
 * Important: `len` counts the *full block size* of each allocation
 * (header + padding + user payload), not just the user-requested bytes.
 * As a result:
 *
 *     freelist_size(fl) + freelist_remaining(fl) == freelist_alloc(fl)
 *
 * holds by construction.
 *
 * @param fl
 *        Pointer to a freelist. Must not be NULL.
 *
 * @return
 *        Number of bytes accounted as in use, or 0 on error.
 *        On failure, errno is set to:
 *
 *        - EINVAL : @p fl is NULL
 */
size_t freelist_size(const freelist_t* fl);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the total usable capacity of the freelist region.
 *
 * This function reports the size, in bytes, of the freelist's managed
 * memory region. It represents the maximum amount of space that can be
 * consumed by allocations plus internal metadata and padding.
 *
 * Equivalently, it is the constant part of:
 *
 *     freelist_size(fl) + freelist_remaining(fl) == freelist_alloc(fl)
 *
 * @param fl
 *        Pointer to a freelist. Must not be NULL.
 *
 * @return
 *        Total usable freelist capacity in bytes, or 0 on error.
 *        On failure, errno is set to:
 *
 *        - EINVAL : @p fl is NULL
 */
size_t freelist_alloc(const freelist_t* fl);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the total number of bytes carved from the backing arena.
 *
 * This function returns the total size of the contiguous chunk obtained
 * from the parent arena for this freelist. It may be larger than the
 * freelist's usable capacity (see ::freelist_alloc), because it includes
 * space consumed by the freelist header and any internal alignment padding.
 *
 * Conceptually:
 *
 *   - ::total_freelist_alloc(fl)  ≥  ::freelist_alloc(fl)
 *
 * @param fl
 *        Pointer to a freelist. Must not be NULL.
 *
 * @return
 *        Total bytes reserved from the arena for this freelist, or 0 on error.
 *        On failure, errno is set to:
 *
 *        - EINVAL : @p fl is NULL
 */
size_t total_freelist_alloc(const freelist_t* fl);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the base alignment guarantee of the freelist.
 *
 * This function reports the alignment, in bytes, that the freelist
 * guarantees for allocations when using ::alloc_freelist. Aligned
 * variants such as ::alloc_freelist_aligned may request stricter
 * alignments, but will never return pointers with alignment less than
 * this base value.
 *
 * @param fl
 *        Pointer to a freelist. Must not be NULL.
 *
 * @return
 *        The freelist's base alignment in bytes, or 0 on error.
 *        On failure, errno is set to:
 *
 *        - EINVAL : @p fl is NULL
 */
size_t freelist_alignment(const freelist_t* fl);
// -------------------------------------------------------------------------------- 

/**
 * @brief Report whether the freelist owns its backing arena.
 *
 * This function indicates whether the freelist is responsible for
 * destroying its underlying arena when ::free_freelist is called.
 *
 * Ownership rules:
 *
 *   - Dynamic freelists (created via ::init_dynamic_freelist)  
 *     typically return true.
 *
 *   - Static freelists (created via ::init_freelist_with_arena or
 *     ::init_static_freelist) return false, since the caller or some
 *     other subsystem owns the arena and its memory.
 *
 * @param fl
 *        Pointer to a freelist. Must not be NULL.
 *
 * @return
 *        true  if the freelist owns its arena and is allowed to free it  
 *        false if the freelist is non-owning, or if @p fl is NULL  
 *
 *        When @p fl is NULL, errno is set to:
 *
 *        - EINVAL : @p fl is NULL
 */
bool freelist_owns_arena(const freelist_t* fl);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the minimum usable allocation size required to construct a freelist.
 *
 * A freelist requires that its initial usable memory region be large enough to
 * hold at least one `free_block_t` structure. This represents the smallest
 * possible free block that the allocator can manage.
 *
 * This function exposes that minimum requirement so callers can query it when:
 *
 *  - allocating static buffers for `init_static_freelist()`
 *  - determining whether a requested freelist size is valid
 *  - performing compile-time or runtime capacity checks
 *
 * The value returned is constant for the lifetime of the program and is
 * typically:
 *
 *     sizeof(free_block_t)
 *
 * @return
 *     The minimum number of usable bytes (`size_t`) required to initialize a
 *     freelist. Any attempt to construct a freelist with a smaller size will
 *     fail with `errno = EINVAL`.
 *
 * @par Example
 * @code{.c}
 * size_t min_bytes = min_freelist_alloc();
 *
 * // Create a static buffer large enough for the freelist and its metadata.
 * uint8_t buffer[1024];
 *
 * // Safe: 1024 >= min_bytes
 * freelist_t* fl = init_static_freelist(buffer, sizeof buffer, 0);
 *
 * if (!fl) {
 *     fprintf(stderr, "Freelist creation failed: %s\n", strerror(errno));
 * }
 * @endcode
 */
size_t min_freelist_alloc();
// -------------------------------------------------------------------------------- 

/**
 * @brief Produce a human-readable diagnostic summary of a freelist allocator.
 *
 * This function writes a formatted, multi-line textual report describing the
 * internal state of a `freelist_t` allocator. It mirrors the behavior of
 * `arena_stats()` and is intended for debugging, logging, and verification
 * during development.
 *
 * The report includes:
 *   - freelist type (STATIC or DYNAMIC)
 *   - whether the freelist owns the underlying arena
 *   - used bytes (total block sizes consumed)
 *   - remaining bytes
 *   - usable capacity of the freelist region
 *   - total bytes carved from the parent arena (including freelist header)
 *   - utilization percentage of the freelist region
 *   - alignment requirements
 *   - enumeration of free blocks (address and size)
 *
 * The string is written into the caller-provided buffer using the internal
 * `_buf_appendf()` utility. Output is always null-terminated as long as the
 * buffer is at least 1 byte in size.
 *
 * @param fl
 *        Pointer to the freelist instance to query. When NULL, the function
 *        writes the string "Freelist: NULL" and returns true.
 *
 * @param buffer
 *        Destination character buffer. Must not be NULL.
 *
 * @param buffer_size
 *        Size of `buffer` in bytes. Must be greater than zero.
 *
 * @retval true
 *         The report was successfully generated (even if truncated to fit
 *         within the provided buffer).
 *
 * @retval false
 *         An error occurred. In this case:
 *           - `EINVAL` is set if `buffer` is NULL or size is zero.
 *           - Any internal `_buf_appendf()` failure (buffer too small)
 *             results in `false`, but the buffer may contain partial output.
 *
 * @note
 *        This function never allocates memory. All output is performed
 *        using the caller-provided buffer.
 *
 * @code{.c}
 * char buf[512];
 *
 * freelist_t *fl = init_freelist_with_arena(arena, 1024, 0);
 * if (!fl) { // handle error }
 *
 * if (freelist_stats(fl, buf, sizeof buf)) {
 *     printf("%s\n", buf);
 * } else {
 *     perror("freelist_stats");
 * }
 * @endcode
 */
bool freelist_stats(const freelist_t *fl, char *buffer, size_t buffer_size);
// ================================================================================ 
// ================================================================================ 

/**
 * @brief Allocate a block of memory using `malloc()`.
 *
 * This is the standard-library backend for the allocator vtable.  
 * The function allocates `size` bytes using `malloc()` and optionally
 * zero-initializes the memory if `zeroed` is true.
 *
 * The function performs the minimal safety checks expected of a compliant
 * allocator:
 *  - `size == 0` is rejected (`EINVAL`)
 *  - `malloc()` failure returns NULL and leaves `errno` unchanged
 *
 * @param ctx
 *        Unused. Present only to satisfy the allocator interface.
 *
 * @param size
 *        Number of bytes to allocate. Must be greater than zero.
 *
 * @param zeroed
 *        If true, the allocated memory is cleared to zero.
 *
 * @return
 *        Pointer to newly allocated memory on success, or NULL on failure.
 */
static inline void* v_alloc(void* ctx, size_t size, bool zeroed) {
    (void)ctx;

    if (size == 0) {
        errno = EINVAL;
        return NULL;
    }

    void* p = malloc(size);
    if (!p) {
        /* malloc sets errno (usually ENOMEM) */
        return NULL;
    }

    if (zeroed) {
        memset(p, 0, size);
    }
    return p;
}

// ----------------------------------------------------------------------------- 

/**
 * @brief Allocate memory with a requested alignment using the malloc backend.
 *
 * This backend does not provide true platform-specific aligned allocation.
 * Instead, it enforces the following policy:
 *
 *  - If `alignment == 0`, the alignment defaults to `alignof(max_align_t)`.
 *  - If `alignment` exceeds `alignof(max_align_t)`, the request is rejected
 *    with `errno = EINVAL`, because the `malloc()` backend cannot guarantee
 *    stricter alignment.
 *
 * When alignment is acceptable, the function behaves exactly like
 * `v_alloc()`.
 *
 * @param ctx
 *        Unused.
 *
 * @param size
 *        Number of bytes to allocate. Must be greater than zero.
 *
 * @param alignment
 *        Required alignment. Must be zero or a power of two, and must not
 *        exceed `alignof(max_align_t)`.
 *
 * @param zeroed
 *        If true, the returned buffer is set to zero.
 *
 * @return
 *        Pointer to allocated memory on success, or NULL on failure.
 */
static inline void* v_alloc_aligned(void* ctx,
                                    size_t size,
                                    size_t align,
                                    bool   zeroed) {
    (void)ctx;

    if (size == 0) {
        errno = EINVAL;
        return NULL;
    }

    if (align == 0) {
        align = alignof(max_align_t);
    }

    /* For this simple backend, refuse alignments stricter than max_align_t. */
    if (align > alignof(max_align_t)) {
        errno = EINVAL;
        return NULL;
    }

    /* We also assume align is a power of two or caller-normalized. */

    return v_alloc(NULL, size, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Resize a memory block using `realloc()`.
 *
 * This backend follows the standard `realloc()` contract:
 *
 *  - If `old_ptr == NULL`, the call behaves like `v_alloc()`.
 *  - On success, a new pointer is returned and the contents of the old
 *    block are preserved.
 *  - On failure, NULL is returned and the original pointer remains valid.
 *
 * If `zeroed` is true and the block is grown, only the newly-added region
 * (`new_size - old_size`) is zero-initialized.
 *
 * @param ctx
 *        Unused.
 *
 * @param old_ptr
 *        Pointer to previously allocated memory, or NULL.
 *
 * @param old_size
 *        Size of the old allocation. Used only for zero-initialization
 *        when growing.
 *
 * @param new_size
 *        Requested new allocation size.
 *
 * @param zeroed
 *        Whether to zero-initialize any newly allocated portion.
 *
 * @return
 *        New pointer on success, NULL on allocation failure.
 *        On failure, `old_ptr` remains valid.
 */
static inline void* v_realloc(void* ctx,
                              void* old_ptr,
                              size_t old_size,
                              size_t new_size,
                              bool  zeroed) {
    (void)ctx;

    /* NULL behaves like alloc */
    if (!old_ptr) {
        return v_alloc(NULL, new_size, zeroed);
    }

    void* p = realloc(old_ptr, new_size);
    if (!p) {
        /* realloc failed; old_ptr is still valid, caller must handle */
        return NULL;
    }

    if (zeroed && new_size > old_size) {
        memset((uint8_t*)p + old_size, 0, new_size - old_size);
    }
    return p;
}
// ----------------------------------------------------------------------------- 

/**
 * @brief Aligned version of `v_realloc()` for the malloc backend.
 *
 * This function applies the same alignment policy as
 * `v_alloc_aligned()`:
 *
 *  - If alignment is zero, the default alignment is used.
 *  - If alignment exceeds `alignof(max_align_t)`, the request is rejected.
 *
 * For acceptable alignments, the operation behaves exactly like
 * `v_realloc()`.
 *
 * @param ctx
 *        Unused.
 *
 * @param old_ptr
 *        Previously allocated pointer, or NULL.
 *
 * @param old_size
 *        Size of the original allocation.
 *
 * @param new_size
 *        Requested new size.
 *
 * @param zeroed
 *        Whether to zero newly allocated bytes when growing.
 *
 * @param alignment
 *        Desired alignment. Must not exceed `alignof(max_align_t)`.
 *
 * @return
 *        New pointer on success, NULL on failure.
 */
static inline void* v_realloc_aligned(void* ctx,
                                      void* old_ptr,
                                      size_t old_size,
                                      size_t new_size,
                                      bool   zeroed,
                                      size_t align) {
    (void)ctx;

    if (!old_ptr) {
        return v_alloc_aligned(NULL, new_size, align, zeroed);
    }

    /* For this backend, we don't actually enforce extra alignment
       beyond max_align_t; we just reuse v_realloc. */
    if (align == 0) {
        align = alignof(max_align_t);
    }
    if (align > alignof(max_align_t)) {
        errno = EINVAL;
        return NULL;
    }

    return v_realloc(NULL, old_ptr, old_size, new_size, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a previously-allocated block to the system allocator.
 *
 * This is a simple wrapper for `free()`.  
 * Unlike some custom allocators, freeing a NULL pointer is legal and
 * results in a no-op.
 *
 * @param ctx
 *        Unused.
 *
 * @param ptr
 *        Pointer to memory allocated by the malloc backend. May be NULL.
 */
static inline void v_return(void* ctx, void* ptr) {
    (void)ctx;
    /* free(NULL) is defined as a no-op; no need to set errno. */
    free(ptr);
}

// ----------------------------------------------------------------------------- 

/**
 * @brief Finalizer for the malloc allocator backend.
 *
 * For malloc-backed allocators, there is no global allocator state
 * to clean up. This function exists only to satisfy the vtable interface.
 *
 * @param ctx
 *        Unused.
 */
static inline void v_free(void* ctx) {
    (void)ctx;
    /* Nothing to do for a malloc-backed allocator. */
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Create an allocator vtable backed by the C standard library.
 *
 * This function constructs an ::allocator_vtable_t whose operations are
 * implemented using the standard C library memory functions (`malloc`,
 * `realloc`, and `free`). It provides a convenient plug-in backend for
 * APIs expecting a vtable-style allocator interface.
 *
 * The returned vtable provides the following semantics:
 *
 *   - `allocate`  
 *        Uses `malloc()`. Optionally zeroes the memory.
 *
 *   - `allocate_aligned`  
 *        Accepts alignment requests up to `alignof(max_align_t)`.  
 *        Larger alignments are rejected with `EINVAL`, since `malloc()`
 *        cannot guarantee stricter alignment.
 *
 *   - `reallocate`  
 *        Wraps `realloc()`. On failure, returns NULL and leaves the input
 *        pointer valid.
 *
 *   - `reallocate_aligned`  
 *        Behaves like `reallocate`, but with the same alignment policy as
 *        `allocate_aligned`.
 *
 *   - `return_element`  
 *        A thin wrapper over `free()`.
 *
 *   - `deallocate`  
 *        No-op. Included only for interface consistency.
 *
 * The `.ctx` field is always NULL, because this backend does not require
 * a context object.
 *
 * @return
 *        A fully-populated ::allocator_vtable_t initialized to use the
 *        standard library allocator backend.
 *
 * @code{.c}
 * allocator_vtable_t alloc = malloc_allocator();
 *
 * void* p = alloc.allocate(alloc.ctx, 128, true);
 * if (!p) {
 *     perror("malloc_allocator allocate");
 *     return 1;
 * }
 *
 * // Resize the block
 * p = alloc.reallocate(alloc.ctx, p, 128, 256, false);
 *
 * // Free it
 * alloc.return_element(alloc.ctx, p);
 * @endcode
 */
static inline allocator_vtable_t malloc_allocator(void) {
    allocator_vtable_t v = {
        .allocate           = v_alloc,
        .allocate_aligned   = v_alloc_aligned,
        .reallocate         = v_realloc,
        .reallocate_aligned = v_realloc_aligned,
        .return_element     = v_return,
        .deallocate         = v_free,
        .ctx                = NULL
    };
    return v;
}
// ================================================================================ 
// ================================================================================ 
// BUDDY ALLOCATOR 

typedef struct buddy_t buddy_t;
// ================================================================================ 
// ================================================================================ 

/**
 * @brief Initialize a buddy allocator with a fixed-size memory pool.
 *
 * This function constructs a new ::buddy_t allocator backed by a
 * power-of-two–sized memory pool obtained from the OS using
 * `buddy_os_alloc()` (typically `mmap` on POSIX or `VirtualAlloc` on Windows).
 *
 * The allocator divides the pool into blocks whose sizes are powers of two,
 * ranging from @p min_block_size up to @p pool_size. All allocation requests
 * are rounded upward to the nearest block size that can hold:
 *
 *   - the internal allocation header (`buddy_header_t`), and  
 *   - the requested user payload aligned to @p base_align.
 *
 * The following normalization rules apply:
 *
 *   - If @p base_align is `0`, it defaults to `alignof(max_align_t)`.
 *   - If @p base_align is not a power of two, it is rounded up to the next
 *     power of two.
 *   - If @p min_block_size is too small to hold the header plus alignment
 *     padding, it is increased to the minimum size that can.
 *   - Both @p pool_size and @p min_block_size are rounded up to powers of two.
 *   - @p min_block_size must not exceed the (adjusted) @p pool_size.
 *
 * The top-level free list is initialized with a single block representing the
 * entire pool.
 *
 * @param pool_size
 *     Requested total pool size in bytes. Must be non-zero. Will be rounded
 *     upward to the next power of two.
 *
 * @param min_block_size
 *     Minimum block size in bytes. Must be non-zero. Will be rounded upward to
 *     the next power of two *after* being raised (if necessary) to accommodate
 *     the internal header + alignment padding.
 *
 * @param base_align
 *     Required alignment for returned user pointers. May be zero to indicate
 *     natural alignment (`alignof(max_align_t)`). Must be a power of two, or it
 *     will be rounded up to one.
 *
 * @return
 *     A newly allocated ::buddy_t instance on success, or `NULL` on failure.
 *
 * @retval NULL
 *     The function returns `NULL` and sets @c errno under these conditions:
 *
 *     - `EINVAL`  
 *         - @p pool_size is zero  
 *         - @p min_block_size is zero  
 *         - @p min_block_size exceeds @p pool_size after normalization  
 *         - @p base_align cannot be normalized (overflow in `_next_pow2()`)  
 *         - Power-of-two order computation fails (invalid values)
 *
 *     - `ENOMEM`  
 *         - System memory for the control structure or free-list array cannot be allocated  
 *         - The OS-level allocation (`buddy_os_alloc`) for the backing pool fails
 *
 * @note
 *     The resulting allocator is *not* resizable; its pool size is fixed for
 *     the lifetime of the object. Use ::free_buddy to destroy it.
 *
 * @warning
 *     Returned pointers must be freed only via ::return_buddy(); using `free()`
 *     on them is undefined behavior.
 *
 * @code{.c}
 * #include "buddy.h"
 * #include <stdio.h>
 * #include <errno.h>
 *
 * int main(void) {
 *     size_t pool      = 4096;                 // 4 KiB requested
 *     size_t min_block = 64;                   // smallest allocation is 64 bytes
 *     size_t align     = 32;                   // user pointers aligned to 32 bytes
 *
 *     buddy_t *buddy = init_buddy_allocator(pool, min_block, align);
 *     if (!buddy) {
 *         perror("init_buddy_allocator");
 *         return 1;
 *     }
 *
 *     // Allocate 100 bytes (rounded to next block size)
 *     void *ptr = alloc_buddy(buddy, 100, false);
 *     if (!ptr) {
 *         perror("alloc_buddy");
 *         free_buddy(buddy);
 *         return 1;
 *     }
 *
 *     // Use memory ...
 *
 *     // Return memory
 *     return_buddy(buddy, ptr);
 *
 *     // Release the entire buddy allocator
 *     free_buddy(buddy);
 *     return 0;
 * }
 * @endcode
 */
buddy_t *init_buddy_allocator(size_t pool_size, size_t min_block_size, size_t base_align);
// -------------------------------------------------------------------------------- 

/**
 * @brief Destroy a buddy allocator and release all associated resources.
 *
 * This function frees the OS-backed memory pool created by
 * ::init_buddy_allocator(), releases internal metadata such as the free-list
 * array, clears the allocator structure, and finally frees the ::buddy_t
 * object itself.
 *
 * All memory allocated through this buddy allocator becomes invalid after this
 * call. Accessing pointers returned by ::alloc_buddy(), ::alloc_buddy_aligned(),
 * ::realloc_buddy(), or ::realloc_buddy_aligned() after invoking
 * ::free_buddy() results in undefined behavior.
 *
 * The function is safe to call with a NULL pointer, in which case it performs
 * no action.
 *
 * @param b
 *     Pointer to a ::buddy_t instance previously created by
 *     ::init_buddy_allocator(). May be NULL.
 *
 * @note
 *     This call implicitly frees *all* blocks allocated from @p b, regardless
 *     of whether they were returned via ::return_buddy(). Users do not need to
 *     free individual allocations before calling this function.
 *
 * @warning
 *     After calling ::free_buddy(), the pointer @p b must not be reused.
 *     The function clears the contents of the structure before freeing it.
 *
 * @code{.c}
 * buddy_t *buddy = init_buddy_allocator(4096, 64, 32);
 * if (!buddy) {
 *     perror("init_buddy_allocator");
 *     return 1;
 * }
 *
 * void *p = alloc_buddy(buddy, 128, false);
 * // ... use p ... 
 *
 * free_buddy(buddy);   // releases pool + metadata + allocator itself
 * @endcode
 */
void free_buddy(buddy_t *b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate a block of memory from a buddy allocator.
 *
 * This function allocates @p size bytes from the buddy allocator @p b.
 * The request is rounded upward to accommodate the internal
 * ::buddy_header_t followed by power-of-two block sizing. The returned
 * user pointer refers to the memory *after* the allocation header.
 *
 * The allocation proceeds as follows:
 *
 *   1. The user size is increased by the size of ::buddy_header_t.
 *   2. The result is rounded up to at least the minimum block size
 *      (``2^min_order``).
 *   3. The total size is rounded to the next power of two.
 *   4. A suitable free block is located; if necessary, larger blocks are
 *      repeatedly split into buddies until the target order is reached.
 *   5. A header is written at the start of the block, recording the
 *      block's order and pool offset.
 *   6. If @p zeroed is true, the user portion of the block is cleared.
 *
 * The returned pointer is always at least ``sizeof(buddy_header_t)`` bytes
 * into the allocated block. Use ::alloc_buddy_aligned if strict alignment
 * beyond this implicit header offset is required.
 *
 * @param b
 *     Pointer to a valid ::buddy_t allocator created by
 *     ::init_buddy_allocator(). Must not be NULL.
 *
 * @param size
 *     Requested number of user bytes. Must be non-zero.
 *
 * @param zeroed
 *     If true, the user portion of the block will be filled with zeros.
 *     The internal header is never zeroed.
 *
 * @return
 *     A pointer to the user portion of the allocated block, or NULL on
 *     failure.
 *
 * @retval NULL
 *     The function returns NULL and sets @c errno if:
 *
 *       - @c EINVAL  
 *         - @p b is NULL  
 *         - @p size is zero  
 *
 *       - @c ENOMEM  
 *         - no block of adequate size exists  
 *         - the allocation would exceed the total pool size
 *
 * @note
 *     Returned blocks must be released using ::return_buddy(). Calling
 *     ``free()`` on a pointer returned by this function is undefined
 *     behavior.
 *
 * @note
 *     This function guarantees only the minimum alignment implied by the
 *     block size and header placement. Use ::alloc_buddy_aligned() when
 *     explicit user-defined alignment is required.
 *
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(4096, 64, 32);
 * if (!b) {
 *     perror("init_buddy_allocator");
 *     return 1;
 * }
 *
 * // Allocate 100 bytes (rounded to next power-of-two block)
 * void *ptr = alloc_buddy(b, 100, true);
 * if (!ptr) {
 *     perror("alloc_buddy");
 *     free_buddy(b);
 *     return 1;
 * }
 *
 * // Use memory...
 *
 * return_buddy(b, ptr);   // free block back to buddy system
 * free_buddy(b);          // destroy allocator
 * @endcode
 */
void *alloc_buddy(buddy_t *b, size_t size, bool zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize an arena_t backed by a buddy allocator region.
 *
 * This function creates an arena_t that lives entirely inside a single
 * allocation obtained from the given @p buddy allocator. The buddy allocation
 * is used to store:
 *
 *   - the arena_t header itself,
 *   - a single Chunk header, and
 *   - the arena's usable data region.
 *
 * The pointer returned by this function is:
 *
 *   - the address of the arena_t object, and
 *   - exactly the same pointer that was returned by alloc_buddy().
 *
 * This is important: when the arena is no longer needed, the entire region
 * (arena_t + Chunk + data) must be returned to the buddy allocator using
 * return_arena_with_buddy(), which internally calls return_buddy_element()
 * with the arena pointer.
 *
 * The @p bytes parameter specifies how many bytes of the buddy pool should
 * be reserved for this arena. Within that region, the arena places its
 * metadata and aligns the remaining data area to @p base_align_in. The
 * arena's @c alloc field reflects the size of the usable data region, while
 * @c tot_alloc records the full sub-region size (@p bytes).
 *
 * The @p base_align_in parameter specifies the per-arena alignment for
 * allocations made via alloc_arena(). If @p base_align_in is zero, it
 * defaults to alignof(max_align_t). Non–power-of-two values are rounded up
 * to the next power of two, and a minimum of alignof(max_align_t) is
 * enforced.
 *
 * On success, the returned arena_t has:
 *
 *   - @c mem_type set to @c DYNAMIC (or a dedicated "buddy-backed" tag if
 *     you define one),
 *   - @c resize set to 0 (fixed capacity, no growth), and
 *   - @c owns_memory set to 0 (the buddy allocator owns the underlying
 *     memory).
 *
 * The caller must not free this arena via free_arena(). Instead, the entire
 * region must be returned using return_arena_with_buddy().
 *
 * On failure, this function returns NULL and sets errno:
 *
 *   - EINVAL if:
 *       - @p buddy is NULL,
 *       - @p bytes is zero,
 *       - the requested alignment cannot be normalized to a valid value,
 *       - the requested sub-region cannot accommodate arena_t, Chunk, and
 *         at least one usable data byte, or
 *       - internal size/alignment checks fail.
 *   - ENOMEM if alloc_buddy() fails to provide the requested region.
 *
 * If a buddy allocation was acquired but a subsequent layout check fails,
 * the region is returned to the buddy allocator via return_buddy_element()
 * before NULL is returned.
 *
 * @param buddy         Pointer to a valid buddy_t allocator instance.
 * @param bytes         Size of the sub-region to reserve from @p buddy for
 *                      this arena (including metadata and data).
 * @param base_align_in Desired per-arena alignment for allocations made via
 *                      alloc_arena(); 0 means "use alignof(max_align_t)".
 *
 * @retval arena_t* Pointer to a fully initialized arena_t on success.
 * @retval NULL     On failure, with errno set as described above.
 *
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(4096u, 64u, alignof(max_align_t));
 * if (!b) { // handle error }
 *
 * arena_t *a = init_arena_with_buddy(b, 1024u, alignof(max_align_t));
 * if (!a) { // handle error }
 *
 * // Use 'a' as a normal arena:
 * void *p = alloc_arena(a, 128u, true);
 * if (!p) { // handle error }
 *
 * // When done with this arena region:
 * if (!return_arena_with_buddy(a, b)) {
 *     // handle error
 * }
 *
 * free_buddy(b);
 * @endcode
 */
arena_t* init_arena_with_buddy(buddy_t* buddy, size_t bytes, size_t base_align_in);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a buddy-backed arena region to its buddy allocator.
 *
 * This function returns the entire memory region associated with an arena
 * created by init_arena_with_buddy() back to the underlying buddy allocator.
 *
 * The @p arena pointer must be the same pointer that was originally returned
 * by init_arena_with_buddy(), which is also the user pointer obtained from
 * alloc_buddy(). The function treats @p arena as a single buddy allocation
 * and returns it via return_buddy_element().
 *
 * After a successful call:
 *
 *   - the memory containing @p arena, its Chunk header, and the arena's
 *     data region is returned to @p buddy, and
 *   - the @p arena pointer becomes invalid and must not be dereferenced.
 *
 * This function is intended only for arenas that do not own their memory
 * independently. If @p arena->owns_memory is non-zero, the function refuses
 * to proceed and fails with EPERM.
 *
 * On failure, the function returns false and sets errno:
 *
 *   - EINVAL if @p arena or @p buddy is NULL.
 *   - EPERM if @p arena->owns_memory is non-zero, indicating the arena was
 *     not intended to be freed via the buddy allocator.
 *   - Any errno set by is_buddy_ptr_sized() if that validation fails. In
 *     this case, no memory is returned to the buddy allocator.
 *
 * On success, return_buddy_element() is called with @p arena, and its return
 * value is propagated to the caller. In that case, the memory region is
 * considered freed from the arena's perspective.
 *
 * @param arena Pointer to an arena_t instance that was created by
 *              init_arena_with_buddy().
 * @param buddy Pointer to the buddy_t allocator that originally provided
 *              the backing memory.
 *
 * @retval true  On success; the region has been returned to @p buddy and
 *               @p arena is no longer valid.
 * @retval false On failure, with errno set as described above.
 *
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(4096u, 64u, alignof(max_align_t));
 * if (!b) { // handle error }
 *
 * arena_t *a = init_arena_with_buddy(b, 1024u, alignof(max_align_t));
 * if (!a) { // handle error }
 *
 * // Use 'a' for various arena allocations...
 *
 * if (!return_arena_with_buddy(a, b)) {
 *     // handle error; memory not returned to buddy
 * }
 *
 * free_buddy(b);
 * @endcode
 */
bool return_arena_with_buddy(arena_t *arena, buddy_t *buddy);
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate an aligned memory block from a buddy allocator.
 *
 * This function allocates @p size bytes from the buddy allocator @p b,
 * guaranteeing that the returned user pointer is aligned to @p align bytes.
 *
 * The request is expanded to accommodate:
 *
 *   - the internal ::buddy_header_t (stored immediately before the user
 *     pointer),
 *   - the requested user payload,
 *   - extra padding required to achieve the requested alignment.
 *
 * Allocation proceeds similarly to ::alloc_buddy(), but with additional
 * alignment logic:
 *
 *   1. If @p align is zero, it defaults to `alignof(max_align_t)`.
 *   2. If @p align is not a power of two, it is rounded up to the next power
 *      of two.
 *   3. The total required bytes (header + payload + alignment padding) are
 *      rounded up to at least the minimum buddy block size, then to the next
 *      power of two.
 *   4. A sufficiently large block is selected; larger blocks are split until
 *      the target order is reached.
 *   5. An aligned user pointer is chosen within the block. The internal header
 *      is written immediately before this aligned pointer.
 *   6. If @p zeroed is true, the entire usable payload region behind the user
 *      pointer is cleared.
 *
 * @param b
 *     Pointer to a valid ::buddy_t allocator created by
 *     ::init_buddy_allocator(). Must not be NULL.
 *
 * @param size
 *     Number of user bytes requested. Must be non-zero.
 *
 * @param align
 *     Required alignment for the returned user pointer.  
 *     Must be a power of two, or will be rounded upward to the next power of
 *     two. A value of zero is interpreted as `alignof(max_align_t)`.
 *
 * @param zeroed
 *     If true, the allocated user portion of the block is filled with zeros.
 *
 * @return
 *     A pointer to the aligned user memory, or NULL on failure.
 *
 * @retval NULL
 *     The function returns NULL and sets @c errno under these conditions:
 *
 *       - @c EINVAL  
 *         - @p b is NULL  
 *         - @p size is zero  
 *         - @p align cannot be normalized to a valid power of two  
 *
 *       - @c ENOMEM  
 *         - no suitable block is available  
 *         - alignment padding exceeds block boundaries  
 *         - @p size cannot fit in the pool (after rounding and header overhead)
 *
 * @note
 *     Returned pointers must be freed with ::return_buddy(). Calling `free()`
 *     on memory allocated by this function results in undefined behavior.
 *
 * @note
 *     Alignment is guaranteed only for the *user* pointer; the internal header
 *     may be unaligned and should not be accessed by user code.
 *
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(4096, 64, 32);
 * if (!b) {
 *     perror("init_buddy_allocator");
 *     return 1;
 * }
 *
 * // Allocate 128 bytes aligned to 64 bytes.
 * void *ptr = alloc_buddy_aligned(b, 128, 64, true);
 * if (!ptr) {
 *     perror("alloc_buddy_aligned");
 *     free_buddy(b);
 *     return 1;
 * }
 *
 * // ptr is now 64-byte aligned and zero-initialized
 *
 * return_buddy(b, ptr);  // free the block
 * free_buddy(b);         // destroy allocator
 * @endcode
 */
void *alloc_buddy_aligned(buddy_t *b,
                          size_t  size,
                          size_t  align,
                          bool    zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return a previously allocated block to the buddy allocator.
 *
 * This function frees a block allocated by ::alloc_buddy(),
 * ::alloc_buddy_aligned(), ::realloc_buddy(), or
 * ::realloc_buddy_aligned(), restoring it to the buddy system's free lists.
 * After freeing, the block may be coalesced with its buddy if the adjacent
 * block of the same order is also free.
 *
 * The user pointer @p ptr must be one returned by one of the allocation
 * functions of this buddy allocator. The corresponding block header
 * (::buddy_header_t) is assumed to reside immediately before the user
 * pointer.
 *
 * Freeing proceeds as follows:
 *
 *   1. If @p b is NULL, the function fails with @c errno = EINVAL.
 *   2. If @p ptr is NULL, the call is treated as a no-op (like `free(NULL)`).
 *   3. The block header is read to determine the block's order and offset.
 *   4. The allocator’s accounting value ``len`` is decreased by the block size.
 *   5. If the buddy block (same order, XOR offset) is free, the two blocks are
 *      coalesced into a larger block, repeating until no further buddy merge
 *      is possible.
 *   6. The resulting block is inserted into the appropriate free list.
 *
 * @param b
 *     Pointer to a ::buddy_t allocator previously created by
 *     ::init_buddy_allocator(). Must not be NULL.
 *
 * @param ptr
 *     A user pointer returned by the buddy allocator's allocation functions.
 *     May be NULL, in which case the function succeeds without action.
 *
 * @return
 *     `true` on success, or `false` on failure.
 *
 * @retval false
 *     The function returns `false` and sets @c errno under these conditions:
 *
 *       - @c EINVAL  
 *         - @p b is NULL  
 *         - The header preceding @p ptr is invalid  
 *         - The block order is outside the range [`min_order`, `max_order`]  
 *         - The recorded block offset/size lies outside the allocator’s pool  
 *
 * @note
 *     After this operation, @p ptr and any derived pointers become invalid.
 *
 * @warning
 *     Passing a pointer that was not allocated by this allocator, or one that
 *     was already freed, results in undefined behavior.
 *
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(4096, 64, 32);
 * void *p = alloc_buddy(b, 200, false);
 *
 * if (!p) {
 *     perror("alloc_buddy");
 *     free_buddy(b);
 *     return 1;
 * }
 *
 * // ... use p ...
 *
 * if (!return_buddy_element(b, p)) {
 *     perror("return_buddy_element");
 * }
 *
 * free_buddy(b);
 * @endcode
 */
bool return_buddy_element(buddy_t *b, void *ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Resize a buddy allocation, preserving existing data when possible.
 *
 * This function adjusts the size of a block previously allocated from the
 * buddy allocator @p buddy. It follows the familiar `realloc`-style semantics:
 *
 *   - `realloc_buddy(buddy, NULL, 0, new_size, zeroed)` behaves like
 *     ::alloc_buddy(buddy, new_size, zeroed).
 *   - `realloc_buddy(buddy, old_ptr, old_size, 0, zeroed)` frees @p old_ptr and
 *     returns NULL.
 *   - If @p new_size is less than or equal to the usable space in the existing
 *     block, the block is reused in place and the original pointer is returned.
 *   - Otherwise, a new block is allocated, up to `new_size` bytes are copied,
 *     and the old block is returned to the allocator.
 *
 * The usable capacity of the old block is derived from the internal
 * ::buddy_header_t stored immediately before @p old_ptr. The capacity is:
 *
 *   `usable_old = block_size - sizeof(buddy_header_t)`
 *
 * where @c block_size is the power-of-two size of the underlying buddy block.
 *
 * If @p zeroed is true and the reallocation occurs in place (i.e., the same
 * block is reused) and @p new_size is greater than @p old_size, the newly
 * exposed tail region `[old_size, new_size)` is zero-initialized.
 *
 * @param buddy
 *     Pointer to a ::buddy_t allocator previously created by
 *     ::init_buddy_allocator(). Must not be NULL.
 *
 * @param old_ptr
 *     Pointer to a block previously allocated from @p buddy, or NULL.
 *
 * @param old_size
 *     Logical size of the existing allocation (in bytes) as understood by the
 *     caller. Must be non-zero if @p old_ptr is non-NULL.
 *
 * @param new_size
 *     Requested new size in bytes. May be zero to indicate free.
 *
 * @param zeroed
 *     If true, any newly added bytes (in the in-place grow case) or the entire
 *     freshly allocated block (when moving) will be zero-initialized.
 *
 * @return
 *     A pointer to the resized allocation, which may be the same as @p old_ptr,
 *     or NULL on failure or when @p new_size is zero.
 *
 * @retval NULL
 *     The function returns NULL and sets @c errno under these conditions:
 *
 *       - @c EINVAL  
 *         - @p buddy is NULL  
 *         - @p old_ptr is non-NULL but @p old_size is zero  
 *
 *       - @c ENOMEM  
 *         - A larger block is required and ::alloc_buddy fails.
 *           In this case, @p old_ptr remains valid and unchanged.
 *
 *     When @p old_ptr is NULL and @p new_size is zero, the function returns
 *     NULL without setting @c errno (no-op case).
 *
 * @note
 *     On success, if a new block is allocated, exactly
 *     `min(old_size, usable_old)` bytes are copied from the old block to the
 *     new block. The old block is then freed via ::return_buddy_element().
 *
 * @warning
 *     Passing a pointer not obtained from ::alloc_buddy(),
 *     ::alloc_buddy_aligned(), ::realloc_buddy(), or ::realloc_buddy_aligned()
 *     on the same allocator instance results in undefined behavior.
 *
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(4096, 64, 32);
 * if (!b) {
 *     perror("init_buddy_allocator");
 *     return 1;
 * }
 *
 * size_t size = 128;
 * void *p = alloc_buddy(b, size, false);
 * if (!p) {
 *     perror("alloc_buddy");
 *     free_buddy(b);
 *     return 1;
 * }
 *
 * // Grow to 256 bytes, zero-initializing the new tail. 
 * void *p2 = realloc_buddy(b, p, size, 256, true);
 * if (!p2) {
 *     perror("realloc_buddy");
 *     // p is still valid on failure 
 *     return_buddy_element(b, p);
 *     free_buddy(b);
 *     return 1;
 * }
 *
 * // Use p2 ... 
 *
 * return_buddy_element(b, p2);
 * free_buddy(b);
 * @endcode
 */
void* realloc_buddy(buddy_t* buddy, void* old_ptr, size_t old_size, size_t new_size, bool zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Resize an aligned buddy allocation, preserving alignment and data.
 *
 * This function resizes a block previously allocated from @p b, enforcing a
 * specified alignment requirement on the resulting pointer. It combines the
 * semantics of ::realloc_buddy() and ::alloc_buddy_aligned():
 *
 *   - `realloc_buddy_aligned(b, NULL, 0, new_size, align, zeroed)` behaves like
 *     ::alloc_buddy_aligned(b, new_size, align, zeroed).
 *   - `realloc_buddy_aligned(b, old_ptr, old_size, 0, align, zeroed)` frees
 *     @p old_ptr and returns NULL.
 *   - If @p new_size fits within the existing block's usable capacity and
 *     @p old_ptr already satisfies the (normalized) alignment requirement,
 *     the block is reused in place and @p old_ptr is returned.
 *   - Otherwise, a new aligned block is allocated, up to
 *     `min(old_size, usable_old)` bytes are copied, and the old block is
 *     returned to the allocator.
 *
 * Alignment handling:
 *
 *   - If @p align is zero, it defaults to `alignof(max_align_t)`.
 *   - If @p align is not a power of two, it is rounded up to the next power
 *     of two.
 *   - The in-place reuse path is taken only if the existing @p old_ptr
 *     satisfies the normalized alignment.
 *
 * The usable capacity of the old block is conservatively taken as:
 *
 *   `usable_old = block_size - sizeof(buddy_header_t)`
 *
 * where @c block_size is the power-of-two size of the underlying buddy block.
 *
 * If @p zeroed is true and the reallocation occurs in place and @p new_size is
 * greater than @p old_size, the newly exposed tail `[old_size, new_size)` is
 * zero-initialized. When moving to a new block, zeroing semantics follow
 * ::alloc_buddy_aligned().
 *
 * @param b
 *     Pointer to a ::buddy_t allocator. Must not be NULL.
 *
 * @param old_ptr
 *     Pointer to a previously allocated aligned block, or NULL.
 *
 * @param old_size
 *     Logical size of the existing allocation (in bytes) as understood by the
 *     caller. Must be non-zero if @p old_ptr is non-NULL.
 *
 * @param new_size
 *     Requested new size in bytes. May be zero to indicate free.
 *
 * @param align
 *     Required alignment for the resulting user pointer. May be zero to
 *     indicate natural alignment (`alignof(max_align_t)`). If not a power of
 *     two, it is rounded up to the next power of two.
 *
 * @param zeroed
 *     If true, any newly added bytes (in-place grow) or the newly allocated
 *     block (when moving) will be zero-initialized.
 *
 * @return
 *     A pointer to the resized, properly aligned allocation, which may be the
 *     same as @p old_ptr, or NULL on failure or when @p new_size is zero.
 *
 * @retval NULL
 *     The function returns NULL and sets @c errno under these conditions:
 *
 *       - @c EINVAL  
 *         - @p b is NULL  
 *         - @p old_ptr is non-NULL but @p old_size is zero  
 *         - @p align cannot be normalized (overflow in `_next_pow2`)  
 *
 *       - @c ENOMEM  
 *         - A larger aligned block is required and
 *           ::alloc_buddy_aligned() fails. In this case, @p old_ptr remains
 *           valid and unchanged.
 *
 *     When @p old_ptr is NULL and @p new_size is zero, the function returns
 *     NULL without setting @c errno (no-op case).
 *
 * @note
 *     On successful move, exactly `min(old_size, usable_old)` bytes are copied
 *     from the old block to the new block before the old block is freed via
 *     ::return_buddy_element().
 *
 * @warning
 *     Passing a pointer not obtained from the buddy allocator, or reusing a
 *     pointer after it has been freed, results in undefined behavior.
 *
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(8192, 64, 32);
 * if (!b) {
 *     perror("init_buddy_allocator");
 *     return 1;
 * }
 *
 * size_t size  = 128;
 * size_t align = 64;
 *
 * void *p = alloc_buddy_aligned(b, size, align, false);
 * if (!p) {
 *     perror("alloc_buddy_aligned");
 *     free_buddy(b);
 *     return 1;
 * }
 *
 * // Grow to 512 bytes, maintaining 64-byte alignment and zeroing new tail.
 * void *p2 = realloc_buddy_aligned(b, p, size, 512, align, true);
 * if (!p2) {
 *     perror("realloc_buddy_aligned");
 *     // p is still valid on failure
 *     return_buddy_element(b, p);
 *     free_buddy(b);
 *     return 1;
 * }
 *
 * // Use p2 ...
 *
 * return_buddy_element(b, p2);
 * free_buddy(b);
 * @endcode
 */
void *realloc_buddy_aligned(buddy_t *b,
                            void   *old_ptr,
                            size_t  old_size,
                            size_t  new_size,
                            size_t  align,
                            bool    zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Validate that a pointer comes from a specific buddy allocator.
 *
 * This function performs a series of structural checks to determine whether
 * @p ptr is a valid user pointer for the buddy allocator @p b. It does not
 * check whether the pointer currently refers to an allocated or freed block;
 * it only verifies that the pointer is *structurally consistent* with the
 * allocator’s layout and header conventions.
 *
 * The validation steps are:
 *
 *   1. @p b and @p ptr must be non-NULL.
 *   2. The internal ::buddy_header_t must reside immediately before @p ptr.
 *   3. The header's @c order must be within [`min_order`, `max_order`].
 *   4. The header's @c block_offset plus the block size must lie within the
 *      allocator's pool size.
 *   5. The block offset must be correctly aligned for the block size
 *      (i.e., multiple of `2^order`).
 *   6. The user pointer @p ptr must point inside the block, at or after the
 *      header and strictly before the block end.
 *
 * If all checks succeed, the function returns `true` and leaves @c errno
 * unchanged. On failure it returns `false` and sets @c errno.
 *
 * @param b
 *     Pointer to a ::buddy_t allocator. Must not be NULL.
 *
 * @param ptr
 *     Candidate user pointer to validate. Must not be NULL.
 *
 * @return
 *     `true` if @p ptr is structurally valid for @p b, `false` otherwise.
 *
 * @retval false
 *     The function returns `false` and sets @c errno under these conditions:
 *
 *       - @c EINVAL  
 *         - @p b is NULL  
 *         - @p ptr is NULL  
 *         - The header is not located immediately before @p ptr  
 *         - The header @c order is outside the valid range  
 *         - The header @c block_offset plus block size exceeds the pool  
 *         - The block offset is not properly aligned for its size  
 *         - @p ptr does not lie within the computed block range
 *
 * @note
 *     This function does *not* track allocation state; it cannot distinguish
 *     between allocated and freed blocks. It only verifies that @p ptr looks
 *     like a pointer produced by this allocator instance.
 *
 * @warning
 *     Passing arbitrary pointers (e.g., stack addresses or pointers from
 *     another allocator) is safe but will cause the function to return
 *     `false` with @c errno = EINVAL. Misuse of the buddy allocator API
 *     (such as double frees) may still result in undefined behavior elsewhere.
 *
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(4096, 64, 32);
 * void *p    = alloc_buddy(b, 128, false);
 *
 * if (!p) {
 *     perror("alloc_buddy");
 *     free_buddy(b);
 *     return 1;
 * }
 *
 * if (!is_buddy_ptr(b, p)) {
 *     fprintf(stderr, "Pointer p is not a valid buddy pointer!\n");
 * }
 *
 * int local = 42;
 * if (!is_buddy_ptr(b, &local)) {
 *     // expected to fail: &local is not from buddy allocator
 * }
 *
 * return_buddy_element(b, p);
 * free_buddy(b);
 * @endcode
 */
bool is_buddy_ptr(const buddy_t *b, const void *ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Validate that a pointer and size fit within a buddy block.
 *
 * This function extends ::is_buddy_ptr() by additionally checking that a
 * requested size @p size can be safely stored within the underlying buddy
 * block corresponding to @p ptr.
 *
 * The following conditions are required for success:
 *
 *   1. ::is_buddy_ptr(@p b, @p ptr) must succeed (i.e., @p ptr must be a
 *      structurally valid buddy pointer for @p b).
 *   2. The user-visible size @p size must be less than or equal to the
 *      usable capacity of the block, defined as:
 *
 *         `usable = block_size - sizeof(buddy_header_t)`
 *
 *      where @c block_size is `2^order` derived from the internal header.
 *
 * If both conditions hold, the function returns `true`. Otherwise, it returns
 * `false` and sets @c errno appropriately.
 *
 * @param b
 *     Pointer to a ::buddy_t allocator. Must not be NULL.
 *
 * @param ptr
 *     Candidate user pointer to validate. Must not be NULL and must satisfy
 *     ::is_buddy_ptr(@p b, @p ptr).
 *
 * @param size
 *     Requested logical size in bytes. Must fit within the usable region of
 *     the block.
 *
 * @return
 *     `true` if @p ptr is a valid buddy pointer for @p b and @p size fits
 *     within the underlying block; `false` otherwise.
 *
 * @retval false
 *     The function returns `false` and sets @c errno under these conditions:
 *
 *       - @c EINVAL  
 *         - ::is_buddy_ptr(@p b, @p ptr) fails (invalid pointer or allocator)  
 *
 *       - @c ERANGE  
 *         - @p size is larger than the usable capacity of the underlying block
 *
 * @note
 *     This function is particularly useful for debug assertions and defensive
 *     checks in higher-level code built on top of the buddy allocator, such as
 *     verifying that a client-provided size is consistent with an allocated
 *     block's capacity.
 *
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(4096, 64, 32);
 * void *p    = alloc_buddy(b, 128, false);
 *
 * if (!p) {
 *     perror("alloc_buddy");
 *     free_buddy(b);
 *     return 1;
 * }
 *
 * // This should succeed: we requested 128 bytes.
 * if (!is_buddy_ptr_sized(b, p, 128)) {
 *     perror("is_buddy_ptr_sized");
 * }
 *
 * // This should fail with ERANGE if 4096 exceeds the block capacity.
 * if (!is_buddy_ptr_sized(b, p, 4096)) {
 *     if (errno == ERANGE) {
 *         fprintf(stderr, "Requested size is larger than block capacity.\n");
 *     }
 * }
 *
 * return_buddy_element(b, p);
 * free_buddy(b);
 * @endcode
 */
bool is_buddy_ptr_sized(const buddy_t *b, const void *ptr, size_t size);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reset a buddy allocator to its initial empty state.
 *
 * This function restores the buddy allocator @p b to the same effective
 * state it had immediately after ::init_buddy_allocator(), without releasing
 * or reallocating the underlying OS-backed memory pool. All outstanding
 * allocations are logically discarded, all free lists are cleared, and the
 * allocator is rebuilt with a single large free block covering the entire
 * pool.
 *
 * This is analogous to a “phase reset” or “arena-style clear” operation:
 * the allocator retains its pool, metadata arrays, alignment configuration,
 * and block-order structure, but *all allocated blocks become invalid*.
 *
 * The reset process performs the following steps:
 *
 *   1. Validate the allocator structure (non-NULL fields, non-zero pool size,
 *      valid order range).
 *   2. Clear all free lists.
 *   3. Insert one free block spanning the entire pool at the highest order.
 *   4. Reset `b->len` to 0, indicating no bytes are currently in use.
 *
 * After this call, all previously returned user pointers become invalid and
 * must not be dereferenced or passed to ::return_buddy_element().
 *
 * @param b
 *     Pointer to a ::buddy_t allocator to reset. Must not be NULL.
 *
 * @return
 *     `true` on success, or `false` on failure.
 *
 * @retval false
 *     The function returns `false` and sets @c errno under the following
 *     conditions:
 *
 *       - @c EINVAL  
 *         - @p b is NULL  
 *         - The allocator structure has an invalid state  
 *           (e.g., missing pool, zero pool size, invalid min/max order values,
 *            or zero free-list levels)
 *
 * @note
 *     This operation does **not** release or reallocate OS memory; the pool is
 *     preserved as-is. Use ::free_buddy() to fully destroy the allocator and
 *     release all associated resources.
 *
 * @note
 *     Unlike ::return_buddy_element(), this function frees *all* allocations at
 *     once, regardless of their order or lifetime.
 *
 * @warning
 *     Calling ::return_buddy_element() on any pointer obtained before the
 *     reset is undefined behavior.
 *
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(4096, 64, 32);
 * void *p1 = alloc_buddy(b, 100, false);
 * void *p2 = alloc_buddy(b, 128, false);
 *
 * // Use memory ...
 *
 * // Completely discard all state and restore allocator to fresh condition 
 * if (!reset_buddy(b)) {
 *     perror("reset_buddy");
 *     free_buddy(b);
 *     return 1;
 * }
 *
 * // Now the pool is empty and alloc_buddy() can be used again.
 * void *p3 = alloc_buddy(b, 200, true);
 *
 * return_buddy_element(b, p3);
 * free_buddy(b);
 * @endcode
 */
bool reset_buddy(buddy_t *b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the total number of bytes currently consumed from the buddy pool.
 *
 * This function returns the allocator’s internal accounting value `b->len`,
 * which represents the sum of the *full block sizes* (power-of-two blocks)
 * of all active allocations. This value includes internal fragmentation and
 * header overhead.
 *
 * This function does not compute logical user-requested bytes; it reflects
 * raw pool consumption.
 *
 * @param b
 *     Pointer to a ::buddy_t allocator. Must not be NULL.
 *
 * @return
 *     The total number of bytes consumed from the pool, or `0` on error.
 *
 * @retval 0
 *     Returned when @p b is NULL, with @c errno set to @c EINVAL.
 *
 * @note
 *     This is analogous to "arena used" or "pool used" metrics in other
 *     allocators.
 *
 * @code{.c}
 * size_t used = buddy_alloc(b);
 * printf("Bytes currently consumed: %zu\n", used);
 * @endcode
 */
size_t buddy_alloc(const buddy_t *b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the total memory footprint of the buddy allocator including overhead.
 *
 * This function returns `b->total_alloc`, which includes:
 *
 *   - the OS-backed pool (`pool_size`),
 *   - the free-lists array,
 *   - the ::buddy_t struct itself,
 *
 * All of these represent the full memory cost of the allocator’s existence.
 *
 * @param b
 *     Pointer to a ::buddy_t allocator. Must not be NULL.
 *
 * @return
 *     The total footprint in bytes, or `0` on error.
 *
 * @retval 0
 *     Returned if @p b is NULL, with @c errno set to @c EINVAL.
 *
 * @note
 *     This value does *not* change during normal allocation or free
 *     operations; it only changes if the allocator’s metadata layout changes.
 *
 * @code{.c}
 * printf("Allocator footprint (bytes): %zu\n", buddy_alloc_total(b));
 * @endcode
 */
size_t total_buddy_alloc(const buddy_t *b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the overall memory size occupied by the buddy allocator.
 *
 * This function simply returns the same value as ::buddy_alloc_total().
 * It exists to provide a naming parallel to `arena_size()` and similar
 * conventions in other allocators.
 *
 * @param b
 *     Pointer to a ::buddy_t allocator. Must not be NULL.
 *
 * @return
 *     Total size in bytes, or `0` on error.
 *
 * @retval 0
 *     Returned if @p b is NULL, with @c errno set to @c EINVAL.
 *
 * @note
 *     Equivalent to calling ::buddy_alloc_total().
 *
 * @code{.c}
 * printf("Buddy allocator size: %zu\n", buddy_size(b));
 * @endcode
 */
size_t buddy_size(const buddy_t *b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the total amount of free memory remaining in the buddy pool.
 *
 * This function reports how many bytes of the underlying buddy allocator's
 * memory pool are still unconsumed, regardless of fragmentation. The value is
 * computed as:
 *
 *      remaining = pool_size - len
 *
 * where:
 *   - `pool_size` is the total allocatable memory in the buddy pool.
 *   - `len` is the sum of the *full block sizes* of all active allocations.
 *
 * Because `len` tracks block sizes (powers of two) rather than user-requested
 * sizes, the returned value may differ from the sum of user-visible free space,
 * but accurately reflects physical pool consumption.
 *
 * @warning
 *     This value does **not** guarantee that a single allocation of the same
 *     size can succeed. Fragmentation may prevent large contiguous blocks from
 *     being available. To determine the largest contiguous block currently
 *     allocatable, use ::buddy_largest_block().
 *
 * @param b
 *     Pointer to a valid ::buddy_t allocator. Must not be NULL.
 *
 * @return
 *     The number of free bytes remaining in the pool, or `0` on error.
 *
 * @retval 0
 *     Returned if @p b is NULL, with @c errno set to @c EINVAL.
 *
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(4096, 64, 32);
 * if (!b) {
 *     perror("init_buddy_allocator");
 *     return 1;
 * }
 *
 * size_t free_bytes = buddy_remaining(b);
 * printf("Remaining free memory: %zu bytes\n", free_bytes);
 *
 * free_buddy(b);
 * @endcode
 */
size_t buddy_remaining(const buddy_t *b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the size (in bytes) of the largest contiguous free block.
 *
 * This function scans the buddy allocator's free lists from the highest
 * order (largest blocks) down to the lowest order, and returns the size of
 * the largest block that is currently available for allocation. The size
 * returned is a power of two and represents the maximum contiguous memory
 * block that can be allocated *without* triggering a split or coalescing
 * operation.
 *
 * Unlike ::buddy_remaining(), which returns total free memory regardless of
 * fragmentation, this function measures how large a single allocation may
 * be in the allocator’s current state. If all free memory is fragmented into
 * small blocks, this function will return a small value even if the total
 * free memory is large.
 *
 * @param b
 *     Pointer to a valid ::buddy_t allocator. Must not be NULL.
 *
 * @return
 *     The size in bytes of the largest free block, or `0` if no free blocks
 *     exist, or on error.
 *
 * @retval 0
 *     Returned if:
 *       - @p b is NULL (with @c errno set to @c EINVAL)
 *       - All free lists are empty (allocator fully consumed)
 *
 * @note
 *     A return value of `0` does not always mean the allocator is out of
 *     memory—it may simply be completely fragmented into blocks smaller than
 *     any requested size.
 *
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(4096, 64, 32);
 * if (!b) {
 *     perror("init_buddy_allocator");
 *     return 1;
 * }
 *
 * void *p1 = alloc_buddy(b, 2000, false);  // Splits large blocks
 * size_t largest = buddy_largest_block(b);
 * printf("Largest free block: %zu bytes\n", largest);
 *
 * return_buddy_element(b, p1);
 * free_buddy(b);
 * @endcode
 */
size_t buddy_largest_block(const buddy_t *b);
// -------------------------------------------------------------------------------- 

/**
 * @brief Format human-readable statistics for a buddy allocator.
 *
 * This function writes a textual summary of the state of @p buddy into
 * @p buffer, up to @p buffer_size bytes. The output is suitable for logging,
 * debugging, or interactive diagnostics and mirrors the style of
 * ::arena_stats().
 *
 * When @p buddy is non-NULL, the report includes:
 *
 *   - Total pool size in bytes.
 *   - Minimum and maximum block sizes (in bytes).
 *   - Bytes currently “used” (sum of allocated block sizes).
 *   - Remaining bytes (pool size minus used).
 *   - Total memory including allocator overhead (`total_alloc`).
 *   - The size of the largest free block.
 *   - Utilization percentage (`used / pool_size * 100`).
 *   - Per-level free list details:
 *       - level index
 *       - block order and size
 *       - number of free blocks
 *       - total free bytes at that level
 *   - A summary line of the total free bytes across all free lists.
 *
 * If @p buddy is NULL, the function writes the line:
 *
 *   `"Buddy: NULL\n"`
 *
 * and returns `true` as long as there is enough space in @p buffer.
 *
 * @param buddy
 *     Pointer to the ::buddy_t allocator to inspect, or NULL. If NULL, a
 *     minimal `"Buddy: NULL\n"` message is written.
 *
 * @param buffer
 *     Destination character buffer for the formatted statistics. Must not
 *     be NULL.
 *
 * @param buffer_size
 *     Size of @p buffer in bytes. Must be greater than zero.
 *
 * @return
 *     `true` if all output was successfully written to @p buffer, `false`
 *     if an error occurred (including insufficient buffer space).
 *
 * @retval false
 *     The function returns `false` and sets @c errno under these conditions:
 *
 *       - @c EINVAL  
 *         - @p buffer is NULL  
 *         - @p buffer_size is zero
 *
 *     If ::_buf_appendf() fails (e.g., due to insufficient buffer capacity),
 *     this function returns `false`. In that case, @c errno is not directly
 *     modified here; consult ::_buf_appendf()’s behavior if needed.
 *
 * @note
 *     The function uses ::buddy_largest_block() to compute the largest free
 *     block and iterates each free list to derive per-level statistics.
 *
 * @note
 *     The string written into @p buffer is **not** null-terminated if the
 *     underlying ::_buf_appendf() helper does not ensure this or if the
 *     buffer is exactly filled. Ensure @p buffer has sufficient size for
 *     typical diagnostic output.
 *
 * @code{.c}
 * char buf[1024];
 *
 * buddy_t *b = init_buddy_allocator(4096, 64, 32);
 * if (!b) {
 *     perror("init_buddy_allocator");
 *     return 1;
 * }
 *
 * // Perform some allocations
 * void *p1 = alloc_buddy(b, 128, false);
 * void *p2 = alloc_buddy(b, 256, false);
 *
 * if (!buddy_stats(b, buf, sizeof(buf))) {
 *     perror("buddy_stats");
 * } else {
 *     fputs(buf, stdout);
 * }
 *
 * return_buddy_element(b, p1);
 * return_buddy_element(b, p2);
 * free_buddy(b);
 * @endcode
 */
bool buddy_stats(const buddy_t *buddy, char *buffer, size_t buffer_size);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the default alignment used by the buddy allocator.
 *
 * This function returns the per-allocator base alignment value that was
 * specified (or normalized) during ::init_buddy_allocator(). This alignment
 * represents the minimum guarantee for all user pointers returned by
 * ::alloc_buddy() and ::alloc_buddy_aligned().
 *
 * The returned value is always a power of two, and is never less than
 * `alignof(max_align_t)`. For strictly aligned allocations beyond this
 * default, use ::alloc_buddy_aligned() or ::realloc_buddy_aligned().
 *
 * @param buddy
 *     Pointer to a valid ::buddy_t allocator. Must not be NULL.
 *
 * @return
 *     The allocator’s default alignment in bytes, or `0` on error.
 *
 * @retval 0
 *     Returned when @p buddy is NULL, with @c errno set to @c EINVAL.
 *
 * @note
 *     The default alignment applies to normal allocations made via
 *     ::alloc_buddy(). Aligned allocations may request larger alignment
 *     values.
 *
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(4096, 64, 32);
 * if (!b) {
 *     perror("init_buddy_allocator");
 *     return 1;
 * }
 *
 * size_t a = buddy_alignment(b);
 * printf("Buddy allocator default alignment: %zu bytes\n", a);
 *
 * free_buddy(b);
 * @endcode
 */
size_t buddy_alignment(const buddy_t* buddy);
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for buddy allocation.
 *
 * Allocates a block of memory of size @p size from the buddy allocator
 * referenced by @p ctx. If @p zeroed is true, the returned memory will be
 * zero-initialized according to the semantics of ::alloc_buddy().
 *
 * On error, errno is set (typically to ENOMEM or EINVAL) and NULL is
 * returned.
 *
 * This function implements the @ref alloc_prototype interface for
 * buddy-backed allocators.
 *
 * @param ctx    Pointer to a buddy_t instance.
 * @param size   Number of bytes to allocate.
 * @param zeroed Whether the memory should be zero-initialized.
 *
 * @retval void* Pointer to a block of at least @p size bytes.
 * @retval NULL  On failure, with errno set.
 */
static inline void* buddy_v_alloc(void* ctx, size_t size, bool zeroed) {
    buddy_t* buddy = (buddy_t*)ctx;
    if (!buddy) {
        errno = EINVAL;
        return NULL;
    }
    return alloc_buddy(buddy, size, zeroed);
}
// --------------------------------------------------------------------------------

/**
 * @brief Vtable adapter for aligned buddy allocation.
 *
 * Allocates a block of memory of size @p size from the buddy allocator
 * referenced by @p ctx, with a minimum alignment of @p align. The alignment
 * must be a power of two (or it will be rounded up to the next power of two)
 * according to the semantics of ::alloc_buddy_aligned(). If @p zeroed is true,
 * the returned memory will be zero-initialized.
 *
 * On error, errno is set (typically to ENOMEM or EINVAL) and NULL is
 * returned.
 *
 * This function implements the @ref alloc_aligned_prototype interface
 * for buddy-backed allocators.
 *
 * @param ctx    Pointer to a buddy_t instance.
 * @param size   Number of bytes to allocate.
 * @param align  Required alignment (power of two, or 0 for natural alignment).
 * @param zeroed Whether the memory should be zero-initialized.
 *
 * @retval void* Pointer to an aligned block of at least @p size bytes.
 * @retval NULL  On failure, with errno set.
 */
static inline void* buddy_v_alloc_aligned(void* ctx, size_t size,
                                          size_t align, bool zeroed) {
    buddy_t* buddy = (buddy_t*)ctx;
    if (!buddy) {
        errno = EINVAL;
        return NULL;
    }
    return alloc_buddy_aligned(buddy, size, align, zeroed);
}
// --------------------------------------------------------------------------------

/**
 * @brief Vtable adapter for buddy reallocation.
 *
 * Resizes a block of memory previously allocated from the buddy allocator
 * referenced by @p ctx. The block is identified by @p old_ptr and its
 * previous size @p old_size. The new requested size is @p new_size.
 *
 * Semantics follow ::realloc_buddy():
 *
 *  - `realloc_buddy(b, NULL, 0, new_size, zeroed)` behaves like
 *    ::alloc_buddy().
 *  - `realloc_buddy(b, old_ptr, old_size, 0, zeroed)` frees @p old_ptr and
 *    returns NULL.
 *  - If the new size fits in the existing block, the pointer is reused in
 *    place.
 *  - Otherwise, a new block is allocated, data is copied, and the old block
 *    is returned to the allocator.
 *
 * On error, errno is set and NULL is returned. In that case, the caller must
 * continue to use @p old_ptr unchanged.
 *
 * This function implements the @ref realloc_prototype interface for
 * buddy-backed allocators.
 *
 * @param ctx       Pointer to a buddy_t instance.
 * @param old_ptr   Pointer to the existing allocation (may be NULL).
 * @param old_size  Size of the existing allocation.
 * @param new_size  Requested new size.
 * @param zeroed    Whether any expanded portion must be zero-initialized.
 *
 * @retval void* New pointer to the resized allocation on success.
 * @retval NULL  On failure, with errno set (caller keeps @p old_ptr).
 */
static inline void* buddy_v_realloc(void* ctx, void* old_ptr,
                                    size_t old_size, size_t new_size,
                                    bool zeroed) {
    buddy_t* buddy = (buddy_t*)ctx;
    if (!buddy) {
        errno = EINVAL;
        return NULL;
    }
    return realloc_buddy(buddy, old_ptr, old_size, new_size, zeroed);
}
// --------------------------------------------------------------------------------

/**
 * @brief Vtable adapter for aligned buddy reallocation.
 *
 * Resizes a block of memory previously allocated from the buddy allocator
 * referenced by @p ctx, enforcing a minimum alignment of @p align for the
 * resulting block. The previous allocation is described by @p old_ptr and
 * @p old_size; the new requested size is @p new_size.
 *
 * Semantics follow ::realloc_buddy_aligned():
 *
 *  - `realloc_buddy_aligned(b, NULL, 0, new_size, align, zeroed)` behaves
 *    like ::alloc_buddy_aligned().
 *  - `realloc_buddy_aligned(b, old_ptr, old_size, 0, align, zeroed)` frees
 *    @p old_ptr and returns NULL.
 *  - If the new size fits in the existing block and the pointer already
 *    satisfies the requested alignment, the pointer is reused in place.
 *  - Otherwise, a new aligned block is allocated, data is copied, and the
 *    old block is returned to the allocator.
 *
 * On error, errno is set and NULL is returned. In that case, the caller must
 * continue to use @p old_ptr unchanged.
 *
 * This function implements the @ref realloc_aligned_prototype interface for
 * buddy-backed allocators.
 *
 * @param ctx       Pointer to a buddy_t instance.
 * @param old_ptr   Pointer to the existing allocation (may be NULL).
 * @param old_size  Size of the existing allocation.
 * @param new_size  Requested new size.
 * @param zeroed    Whether any expanded portion must be zero-initialized.
 * @param align     Required alignment (power of two, or 0 for natural).
 *
 * @retval void* Pointer to the resized, aligned allocation on success.
 * @retval NULL  On failure, with errno set (caller keeps @p old_ptr).
 */
static inline void* buddy_v_realloc_aligned(void* ctx, void* old_ptr,
                                            size_t old_size, size_t new_size,
                                            bool zeroed, size_t align) {
    buddy_t* buddy = (buddy_t*)ctx;
    if (!buddy) {
        errno = EINVAL;
        return NULL;
    }
    return realloc_buddy_aligned(buddy, old_ptr, old_size,
                                 new_size, align, zeroed);
}
// --------------------------------------------------------------------------------

/**
 * @brief Vtable adapter for returning an element to a buddy allocator.
 *
 * Returns a block previously allocated from the buddy allocator referenced
 * by @p ctx back to the allocator. This is a thin adapter over
 * ::return_buddy_element().
 *
 * If @p ctx is NULL, errno is set to EINVAL and the function returns without
 * performing any deallocation. If @p ptr is NULL, the call is treated as
 * a no-op (like `free(NULL)`).
 *
 * This function implements the @ref return_prototype interface for
 * buddy-backed allocators.
 *
 * @param ctx Pointer to a buddy_t instance.
 * @param ptr Pointer to an allocated block (may be NULL).
 */
static inline void buddy_v_return(void* ctx, void* ptr) {
    buddy_t* buddy = (buddy_t*)ctx;
    if (!buddy) {
        errno = EINVAL;
        return;
    }
    (void)return_buddy_element(buddy, ptr);
}
// --------------------------------------------------------------------------------

/**
 * @brief Vtable adapter for freeing a buddy allocator.
 *
 * Releases all memory owned by the buddy allocator referenced by @p ctx,
 * including the OS-backed pool and all internal metadata. After this call,
 * the allocator must not be used again.
 *
 * If @p ctx is NULL, errno is set to EINVAL and the function returns
 * without performing any action.
 *
 * This function implements the @ref free_prototype interface for
 * buddy-backed allocators.
 *
 * @param ctx Pointer to a buddy_t instance.
 */
static inline void buddy_v_free(void* ctx) {
    buddy_t* buddy = (buddy_t*)ctx;
    if (!buddy) {
        errno = EINVAL;
        return;
    }
    free_buddy(buddy);
}
// --------------------------------------------------------------------------------

/**
 * @brief Constructs an allocator_vtable_t for a given buddy allocator.
 *
 * This helper initializes an allocator_vtable_t that exposes a ::buddy_t
 * instance through the generic allocator interface. All operations
 * (allocate, reallocate, deallocate, etc.) are forwarded to the
 * buddy-backed vtable adapter functions.
 *
 * The returned vtable is typically used to parameterize other components
 * that depend on the generic allocator API rather than directly on
 * ::buddy_t.
 *
 * @param b Pointer to a buddy_t instance to use as the allocator backend.
 *
 * @return A fully initialized allocator_vtable_t bound to @p b.
 */
static inline allocator_vtable_t buddy_allocator(buddy_t* b) {
    allocator_vtable_t v = {
        .allocate           = buddy_v_alloc,
        .allocate_aligned   = buddy_v_alloc_aligned,
        .reallocate         = buddy_v_realloc,
        .reallocate_aligned = buddy_v_realloc_aligned,
        .return_element     = buddy_v_return,
        .deallocate         = buddy_v_free,
        .ctx                = b
    };
    return v;
}
// ================================================================================ 
// ================================================================================ 
// SLAB ALLOCATOR 

typedef struct slab_t slab_t;
// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a slab allocator backed by a buddy allocator.
 *
 * Constructs a new ``slab_t`` instance whose control structure is allocated
 * from the buddy allocator referenced by @p buddy. The slab allocator manages
 * fixed-size objects of size @p obj_size, with per-object alignment of at
 * least @p align bytes.
 *
 * The slab allocator does **not** own the underlying memory pool; instead, it
 * draws all slab pages from the caller-provided ::buddy_t instance. The slab
 * allocator therefore remains valid only as long as @p buddy remains valid.
 *
 * The @p align parameter controls the minimum alignment of objects returned by
 * ::alloc_slab():
 *
 * - If @p align is 0, the default ``alignof(max_align_t)`` is used.
 * - If @p align is not a power of two, it is rounded up to the next power of
 *   two using ::_next_pow2().
 * - If normalization of @p align fails, the function returns NULL with errno
 *   set to EINVAL.
 *
 * Each slab page contains:
 *
 * 1. A page header of size at least ``sizeof(slab_page_t)``, aligned to
 *    @p align.
 * 2. A sequence of fixed-size slots, each of size ``max(obj_size,
 *    sizeof(slab_slot_t))`` rounded up to @p align.
 *
 * The @p slab_bytes_hint parameter influences the total size of each slab
 * page:
 *
 * - If 0, a default minimum is chosen (at least 64 slots or 4 KiB).
 * - If too small to contain at least one slot, the size is automatically
 *   increased.
 * - The final slab size is adjusted so the slot region contains an integer
 *   number of slots (no tail fragments).
 *
 * On success, the returned ``slab_t`` has:
 *
 * - No allocated pages yet (pages are created lazily).
 * - An empty free list.
 * - Zero bytes in use (``slab->len == 0``).
 *
 * This function fails and returns NULL if:
 *
 * - @p buddy is NULL
 * - @p obj_size is 0
 * - @p align cannot be normalized to a valid power of two
 * - The underlying ::alloc_buddy_aligned() fails (e.g., ENOMEM)
 *
 * In all error cases, errno is set appropriately.
 *
 *
 * The following example creates a buddy allocator, constructs a slab allocator
 * for objects of type ``my_object_t``, allocates several objects, and then
 * returns them to the slab:
 *
 * @code{.c}
 * #include "buddy.h"
 * #include "slab.h"
 *
 * typedef struct {
 *     int   id;
 *     float value;
 * } my_object_t;
 *
 * int main(void) {
 *     // 1) Create a buddy allocator with a 1 MiB pool.
 *     buddy_t *buddy = init_buddy_allocator(1 << 20,   // pool size
 *                                           64,        // minimum block size
 *                                           0);        // natural alignment
 *     if (!buddy) {
 *         perror("init_buddy_allocator");
 *         return 1;
 *     }
 *
 *     // 2) Create a slab for fixed-size objects.
 *     slab_t *slab = init_slab_allocator(buddy,
 *                                        sizeof(my_object_t),
 *                                        alignof(my_object_t),
 *                                        0);  // let the system choose page size
 *     if (!slab) {
 *         perror("init_slab_allocator");
 *         free_buddy(buddy);
 *         return 1;
 *     }
 *
 *     // 3) Allocate objects from the slab.
 *     my_object_t *a = alloc_slab(slab, true);
 *     my_object_t *b = alloc_slab(slab, true);
 *
 *     if (!a || !b) {
 *         perror("alloc_slab");
 *     } else {
 *         a->id = 1;
 *         b->id = 2;
 *     }
 *
 *     // 4) Return objects to the slab.
 *     return_slab(slab, a);
 *     return_slab(slab, b);
 *
 *     // 5) Reset slab (optional).
 *     reset_slab(slab);
 *
 *     // 6) Destroy the buddy allocator
 *     //    (this implicitly destroys all slab pages).
 *     free_buddy(buddy);
 *
 *     return 0;
 * }
 * @endcode
 *
 * @param buddy           Pointer to a valid ::buddy_t allocator
 *                        supplying memory for slab pages.
 * @param obj_size        Size (in bytes) of each managed object.
 * @param align           Required alignment for stored objects.
 *                        If 0, ``alignof(max_align_t)`` is used.
 * @param slab_bytes_hint Optional hint for the size (in bytes) of each
 *                        slab page. If 0, a default size is computed.
 *
 * @retval slab_t* Pointer to a newly constructed slab allocator on success.
 * @retval NULL    On failure, with errno set to EINVAL or ENOMEM.
 */
slab_t *init_slab_allocator(buddy_t *buddy,
                            size_t   obj_size,
                            size_t   align,
                            size_t   slab_bytes_hint);
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate a fixed-size object from a slab allocator.
 *
 * Obtains a single object from the slab allocator referenced by @p slab.
 * Slab allocators manage objects of a fixed size (specified during
 * ::init_slab_allocator()) and maintain an internal free list of available
 * slots. If no free slots remain, the allocator automatically attempts to
 * grow by acquiring a new slab page from the backing buddy allocator via
 * ::_slab_grow().
 *
 * On success, this function:
 *  - Removes one slot from the free list.
 *  - Optionally zeroes the object if @p zeroed is true.
 *  - Increments the internal usage counter by ``slab->obj_size``.
 *
 * The returned pointer is aligned according to the slab's configured
 * alignment, and must later be returned using ::return_slab().
 *
 * @par Error Handling
 * The function returns NULL and sets errno in the following cases:
 *  - @p slab is NULL (errno = EINVAL)
 *  - Growing the slab via ::_slab_grow() fails  
 *    (errno propagated from underlying buddy allocator, typically ENOMEM)
 *
 * In all failure cases, the internal state of the slab remains unchanged,
 * except for any partial allocation attempts inside ::_slab_grow().
 *
 * @par Example
 * The following example illustrates creating a slab allocator for a
 * user-defined struct, allocating several objects, and returning them:
 *
 * @code{.c}
 * #include "buddy.h"
 * #include "slab.h"
 *
 * typedef struct {
 *     int id;
 *     float value;
 * } my_object_t;
 *
 * int main(void) {
 *     // 1) Create a buddy allocator with a 1 MiB pool.
 *     buddy_t *buddy = init_buddy_allocator(
 *         1 << 20,   // pool size
 *         64,        // minimum block size
 *         0          // natural alignment
 *     );
 *
 *     if (!buddy) {
 *         perror("init_buddy_allocator");
 *         return 1;
 *     }
 *
 *     // 2) Create a slab for my_object_t objects.
 *     slab_t *slab = init_slab_allocator(
 *         buddy,
 *         sizeof(my_object_t),
 *         alignof(my_object_t),
 *         0                 // let the allocator choose slab page size
 *     );
 *
 *     if (!slab) {
 *         perror("init_slab_allocator");
 *         free_buddy(buddy);
 *         return 1;
 *     }
 *
 *     // 3) Allocate two objects.
 *     my_object_t *a = alloc_slab(slab, true);   // zeroed
 *     my_object_t *b = alloc_slab(slab, false);  // uninitialized
 *
 *     if (!a || !b) {
 *         perror("alloc_slab");
 *     } else {
 *         a->id = 1;
 *         b->id = 2;
 *     }
 *
 *     // 4) Return them when finished.
 *     return_slab(slab, a);
 *     return_slab(slab, b);
 *
 *     // 5) Optional: reset the entire slab (all objects freed).
 *     reset_slab(slab);
 *
 *     // 6) Destroy buddy allocator (releases all slab memory).
 *     free_buddy(buddy);
 *
 *     return 0;
 * }
 * @endcode
 *
 * @param slab   Pointer to an initialized ::slab_t instance.
 * @param zeroed If true, the object is zero-initialized to the slab’s object size.
 *
 * @retval void* Pointer to a newly allocated object on success.
 * @retval NULL  On failure, with errno set to EINVAL or a value propagated
 *               from the buddy allocator during slab growth.
 */
void *alloc_slab(slab_t *slab, bool zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return an object to a slab allocator.
 *
 * Returns a previously allocated object back to the slab allocator referenced
 * by @p slab. The pointer must have been obtained from ::alloc_slab() on the
 * same slab allocator instance; otherwise the function fails with errno set
 * to EINVAL.
 *
 * Returning an object:
 *  - Pushes the slot back onto the slab’s global free list.
 *  - Decrements the internal usage counter (``slab->len``) by the object size.
 *
 * This function performs strict pointer validation to ensure that @p ptr
 * refers to a valid slot inside a slab page managed by @p slab. This includes:
 *  - Checking that the page exists and belongs to this slab.
 *  - Checking that @p ptr lies inside the slot region of the page.
 *  - Verifying that @p ptr is aligned on a slot boundary.
 *
 * @par Special Cases
 *  - If @p slab is NULL, the function returns false and sets errno = EINVAL.
 *  - If @p ptr is NULL, the call is treated like ``free(NULL)`` and returns
 *    true with no effect.
 *
 * @par Error Handling
 * The function returns false and sets errno = EINVAL when:
 *  - @p ptr does not belong to any page owned by the slab allocator.
 *  - @p ptr falls outside the slot region of a matching page.
 *  - @p ptr is misaligned (not on a slot boundary).
 *
 * When returning an object, ``slab->len`` is decreased by the object size.
 * If the counter underflows (should not happen), it is clamped to zero
 * defensively.
 *
 * @par Example
 * The following example allocates then frees several objects:
 *
 * @code{.c}
 * typedef struct {
 *     int   id;
 *     float value;
 * } my_object_t;
 *
 * buddy_t *buddy = init_buddy_allocator(1<<20, 64, 0);
 * if (!buddy) {
 *     perror("init_buddy_allocator");
 *     return 1;
 * }
 *
 * slab_t *slab = init_slab_allocator(
 *     buddy,
 *     sizeof(my_object_t),
 *     alignof(my_object_t),
 *     0                  // default page size
 * );
 *
 * my_object_t *a = alloc_slab(slab, true);
 * my_object_t *b = alloc_slab(slab, true);
 *
 * // Use objects a and b...
 *
 * // Return them to the slab allocator:
 * if (!return_slab(slab, a)) {
 *     perror("return_slab(a)");
 * }
 *
 * if (!return_slab(slab, b)) {
 *     perror("return_slab(b)");
 * }
 *
 * // Reset the slab or destroy the buddy allocator when finished.
 * reset_slab(slab);
 * free_buddy(buddy);
 * @endcode
 *
 * @param slab Pointer to an initialized ::slab_t.
 * @param ptr  Pointer to an object previously returned by ::alloc_slab().
 *             May be NULL.
 *
 * @retval true  Object successfully returned to the slab allocator.
 * @retval false Invalid pointer or invalid slab argument; errno is set to EINVAL.
 */
bool return_slab(slab_t *slab, void *ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Query the number of logical bytes currently in use by a slab allocator.
 *
 * Returns the total number of bytes of user payload currently allocated from
 * the slab allocator referenced by @p slab. This value is computed as the
 * number of live objects multiplied by the configured object size
 * (not the internal slot stride).
 *
 * In other words, this reports how many bytes of user data are "in use" from
 * the slab allocator’s point of view, not counting internal metadata,
 * page headers, or unused slots.
 *
 * @par Error Handling
 * If @p slab is NULL, the function returns 0 and sets errno to EINVAL.
 *
 * @param slab Pointer to an initialized ::slab_t instance.
 *
 * @retval size_t Number of logical payload bytes currently in use,
 *                or 0 on error (with errno set to EINVAL).
 */
size_t slab_alloc(const slab_t *slab);
// -------------------------------------------------------------------------------- 

/**
 * @brief Query the total payload capacity of all pages in a slab allocator.
 *
 * Returns the total number of bytes reserved for slots across all pages
 * managed by @p slab. This is computed as:
 *
 * - the number of pages currently allocated, multiplied by
 * - the configured per-page size (``slab->slab_bytes``).
 *
 * This value represents the gross capacity of the slab pages, including
 * both used and free slots, but excluding the memory occupied by the
 * ::slab_t struct itself.
 *
 * @par Relationship to Other Queries
 * - ::slab_alloc() reports the subset of this capacity currently in use by
 *   live objects.
 * - ::total_slab_alloc() includes both page capacity and the control
 *   structure overhead.
 *
 * @par Error Handling
 * If @p slab is NULL, the function returns 0 and sets errno to EINVAL.
 *
 * @param slab Pointer to an initialized ::slab_t instance.
 *
 * @retval size_t Total size in bytes of all slab pages currently allocated,
 *                or 0 on error (with errno set to EINVAL).
 */
size_t slab_size(const slab_t *slab);
// -------------------------------------------------------------------------------- 

/**
 * @brief Query the full memory footprint of a slab allocator.
 *
 * Returns the total number of bytes consumed by the slab allocator itself,
 * including:
 *
 * - the aligned size of the ::slab_t control structure, and
 * - the size of all currently allocated pages (slabs).
 *
 * This is the quantity you would typically use to answer "how much memory
 * does this slab allocator occupy in total?", including metadata and
 * capacity for future allocations.
 *
 * @par Relationship to Other Queries
 * - ::slab_alloc() reports the logical payload bytes in use.
 * - ::slab_size() reports only the total size of the pages, excluding the
 *   control structure.
 * - ::total_slab_alloc() includes both the control structure and all pages.
 *
 * @par Error Handling
 * If @p slab is NULL, the function returns 0 and sets errno to EINVAL.
 *
 * @param slab Pointer to an initialized ::slab_t instance.
 *
 * @retval size_t Total bytes consumed by the slab allocator, including
 *                metadata and pages, or 0 on error (with errno = EINVAL).
 */
size_t total_slab_alloc(const slab_t *slab);
// -------------------------------------------------------------------------------- 

/**
 * @brief Query the per-slot stride (capacity) of a slab allocator.
 *
 * Returns the internal slot size used by @p slab. This stride is:
 *
 * - at least as large as the configured object size, and
 * - large enough to hold internal free-list linkage (e.g., ::slab_slot_t).
 *
 * The slot stride may therefore be greater than the nominal object size
 * if additional padding or metadata is required.
 *
 * @par Usage
 * This function is useful for:
 * - Validating that requested logical sizes do not exceed the slot capacity
 *   when using a generic allocator interface.
 * - Understanding internal memory layout for debugging or statistics.
 *
 * @par Error Handling
 * If @p slab is NULL, the function returns 0 and sets errno to EINVAL.
 *
 * @param slab Pointer to an initialized ::slab_t instance.
 *
 * @retval size_t Slot stride in bytes, or 0 on error (with errno = EINVAL).
 */
size_t slab_stride(const slab_t *slab);
// -------------------------------------------------------------------------------- 

/**
 * @brief Query the total number of slots managed by a slab allocator.
 *
 * Returns the total capacity of @p slab in units of objects (slots), across
 * all currently allocated pages. This is computed as:
 *
 * - the number of pages currently allocated, multiplied by
 * - the number of objects per page (``objs_per_slab``).
 *
 * In other words, this is the maximum number of objects that could be
 * simultaneously allocated from the slab without growing (adding new pages).
 *
 * @par Relationship to Other Queries
 * - ::slab_total_blocks() reports the total number of slots (used + free).
 * - ::slab_alloc() / object size gives an upper bound on the number of
 *   currently allocated objects.
 * - A complementary function (e.g., ::slab_free_blocks()) can be used to
 *   compute the number of free slots:  
 *   ``free_slots = slab_total_blocks(slab) - in_use_slots``.
 *
 * @par Error Handling
 * If @p slab is NULL, the function returns 0 and sets errno to EINVAL.
 *
 * @param slab Pointer to an initialized ::slab_t instance.
 *
 * @retval size_t Total number of slots (objects) available across all pages,
 *                or 0 on error (with errno = EINVAL).
 */
size_t slab_total_blocks(const slab_t *slab);
// -------------------------------------------------------------------------------- 

/**
 * @brief Query the number of free (unallocated) slots in a slab allocator.
 *
 * Returns the number of currently available allocation slots in @p slab.
 * This is computed by walking the slab’s global free list and counting the
 * number of entries. Each entry corresponds to one free slot within some slab
 * page.
 *
 * @par Relationship to Other Queries
 * - ::slab_total_blocks() returns the total slot capacity.
 * - ::slab_in_use_blocks() returns the number of allocated slots.
 * - Therefore:
 *   @code
 *   free_blocks == slab_total_blocks(slab) - slab_in_use_blocks(slab)
 *   @endcode
 *
 * @par Performance
 * This function runs in **O(n)** time, where *n* is the number of free slots.
 * It is intended for diagnostics, assertions, and statistics—not hot paths.
 *
 * @par Error Handling
 * If @p slab is NULL, the function returns 0 and sets errno = EINVAL.
 *
 * @param slab Pointer to an initialized ::slab_t instance.
 *
 * @retval size_t Number of free slots, or 0 on error with errno = EINVAL.
 */
size_t slab_free_blocks(const slab_t *slab);
// -------------------------------------------------------------------------------- 

/**
 * @brief Query the object alignment of a slab allocator.
 *
 * Returns the minimum alignment (in bytes) guaranteed for all objects
 * allocated from @p slab. This alignment is determined when the slab
 * allocator is created via ::init_slab_allocator().
 *
 * @par Usage
 * - Useful when integrating the slab allocator with generic allocator APIs.
 * - Ensures that allocated objects meet the required ABI alignment.
 *
 * @par Error Handling
 * If @p slab is NULL, the function returns 0 and sets errno = EINVAL.
 *
 * @param slab Pointer to an initialized ::slab_t instance.
 *
 * @retval size_t Alignment in bytes, or 0 on error (with errno = EINVAL).
 */
size_t slab_alignment(const slab_t *slab);
// -------------------------------------------------------------------------------- 

/**
 * @brief Query the number of currently allocated objects (slots) in a slab.
 *
 * Returns the number of slots currently in use by dividing ``slab->len``—
 * the total logical bytes in use—by the object size configured when the slab
 * was created.
 *
 * This gives the number of *currently allocated objects*, not counting the
 * free list or empty slots in partially used pages.
 *
 * @par Relationship to Other Queries
 * - ::slab_total_blocks() gives the total slot capacity.
 * - ::slab_free_blocks() gives the number of free slots.
 * - ::slab_in_use_blocks() reports how many are actively allocated.
 *
 * @par Error Handling
 * - If @p slab is NULL, returns 0 and sets errno = EINVAL.
 * - If the slab’s object size is 0 (should never happen), returns 0 and
 *   sets errno = EINVAL.
 *
 * @param slab Pointer to an initialized ::slab_t instance.
 *
 * @retval size_t Number of allocated objects, or 0 on error.
 */
size_t slab_in_use_blocks(const slab_t *slab);
// -------------------------------------------------------------------------------- 

/**
 * @brief Determine whether a pointer was allocated by a given slab allocator.
 *
 * Returns true if @p ptr refers to a valid object allocated from the slab
 * allocator referenced by @p slab. The function performs strict validation:
 *
 * @par Validation Rules
 *   1. The pointer must belong to one of the slab’s pages.
 *   2. It must fall **after** the page header region and **before** the end
 *      of the page.
 *   3. It must be aligned on a slot boundary:
 *      @code
 *      (ptr - slots_start) % slot_size == 0
 *      @endcode
 *   4. The pointer must not lie in the header region of the page.
 *
 * These checks ensure that the pointer was returned by ::alloc_slab() and
 * has not been corrupted or fabricated.
 *
 * @par Error Handling
 * The function returns false and sets errno = EINVAL when:
 *   - @p slab is NULL
 *   - @p ptr is NULL
 *   - @p ptr does not belong to any known slab page
 *   - @p ptr is inside a page but not inside the slot region
 *   - @p ptr is misaligned (not on a slot boundary)
 *
 * @param slab Pointer to an initialized ::slab_t instance.
 * @param ptr  Pointer to validate.
 *
 * @retval true  @p ptr is a valid object allocated from @p slab.
 * @retval false Pointer is invalid or does not belong to this slab (errno set).
 */
bool is_slab_ptr(const slab_t *slab, const void *ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reset a slab allocator, returning all slots to the free list.
 *
 * Reinitializes the internal state of the slab allocator referenced by
 * @p slab so that:
 *
 *  - All pages remain allocated.
 *  - All slots in every page are placed back onto the global free list.
 *  - No objects are considered "in use" (``slab->len == 0``).
 *
 * This is a bulk "clear" operation: it does not release any memory to the
 * backing allocator, but makes all capacity immediately available for reuse.
 *
 * @par Geometry Checks
 * The function first verifies that the slab geometry is valid:
 *  - ``obj_size != 0``
 *  - ``slot_size != 0``
 *  - ``slab_bytes >= page_hdr_bytes + slot_size``
 *
 * If any of these conditions fail, the function returns false and sets
 * errno = EINVAL.
 *
 * @par Behavior
 * For each page in the slab:
 *  - The slots region is recomputed as the range
 *    ``[page_base + page_hdr_bytes, page_base + slab_bytes)``.
 *  - This region is carved into chunks of ``slot_size`` bytes.
 *  - Each chunk is pushed onto the slab’s global free list.
 *
 * After completion:
 *  - ``slab->len == 0``
 *  - ``slab->free_list`` references all slots across all pages.
 *
 * @par Error Handling
 * - If @p slab is NULL, returns false and sets errno = EINVAL.
 * - If the geometry checks fail, returns false and sets errno = EINVAL.
 *
 * @par Example
 * @code{.c}
 * // Assume slab has been created and used previously:
 * slab_t *slab = init_slab_allocator(buddy,
 *                                    sizeof(my_object_t),
 *                                    alignof(my_object_t),
 *                                    0);
 *
 * my_object_t *a = alloc_slab(slab, true);
 * my_object_t *b = alloc_slab(slab, true);
 *
 * // Use a, b...
 *
 * // Now discard all live objects and recycle all slots:
 * if (!reset_slab(slab)) {
 *     perror("reset_slab");
 * }
 *
 * // After reset, slab_in_use_blocks(slab) == 0 and all slots are free.
 * @endcode
 *
 * @param slab Pointer to an initialized ::slab_t instance.
 *
 * @retval true  Reset succeeded; all slots are free and slab->len == 0.
 * @retval false Invalid slab or geometry; errno set to EINVAL.
 */
bool reset_slab(slab_t *slab);
// -------------------------------------------------------------------------------- 

/**
 * @brief Serialize the state of a slab allocator into a caller-provided buffer.
 *
 * Captures a snapshot of the slab allocator referenced by @p slab into
 * a contiguous memory region provided by the caller. The snapshot consists of:
 *
 *  1. A copy of the ::slab_t control structure.
 *  2. A copy of each slab page (in list order), each of size ``slab->slab_bytes``.
 *
 * The snapshot is intended for in-process checkpointing and later restoration
 * via ::restore_slab(). It is **not** a stable, portable serialization format:
 * it contains raw pointers and is only valid within the same process and
 * allocator lifetime.
 *
 * @par Size Query Pattern
 * The function always computes the number of bytes required to store the
 * snapshot and writes it to @p bytes_needed. This allows a two-pass pattern:
 *
 *  @code{.c}
 *  size_t bytes_needed = 0;
 *
 *  if (!save_slab(slab, NULL, 0, &bytes_needed)) {
 *      // Expect errno == ERANGE: use bytes_needed to allocate a buffer.
 *      if (errno != ERANGE) {
 *          perror("save_slab (size query)");
 *      }
 *  }
 *
 *  void *buffer = malloc(bytes_needed);
 *  if (!buffer) { // handle error }
 *
 *  if (!save_slab(slab, buffer, bytes_needed, &bytes_needed)) {
 *      perror("save_slab (snapshot)");
 *  }
 *  @endcode
 *
 * @par Error Handling
 * - If @p slab is NULL or @p bytes_needed is NULL, returns false and sets
 *   errno = EINVAL.
 * - If @p buffer is NULL or @p buffer_size is smaller than the required size,
 *   returns false, sets errno = ERANGE, and still fills @p bytes_needed with
 *   the required size.
 *
 * On success, the snapshot is written to @p buffer and the function returns true.
 *
 * @param slab         Pointer to an initialized ::slab_t instance.
 * @param buffer       Destination buffer for the snapshot (may be NULL for
 *                     size query only).
 * @param buffer_size  Size of @p buffer in bytes.
 * @param bytes_needed Output: number of bytes required to store the snapshot.
 *
 * @retval true  Snapshot successfully written to @p buffer.
 * @retval false Invalid arguments or insufficient buffer; errno set to EINVAL
 *               or ERANGE.
 */
bool save_slab(const slab_t *slab,
               void *buffer,
               size_t buffer_size,
               size_t *bytes_needed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Restore a slab allocator from a previously saved snapshot.
 *
 * Restores the state of the slab allocator referenced by @p slab from the
 * snapshot stored in @p buffer. The snapshot must have been created by
 * ::save_slab() for the **same slab instance**, within the **same process**,
 * and without changes to the underlying page layout since the snapshot.
 *
 * @par Important Limitations
 * - The snapshot is **not portable** across processes, architectures, or
 *   allocator instances; it contains raw pointers.
 * - The snapshot assumes that:
 *   - The same pages are still allocated at the same addresses.
 *   - The slab geometry (object size, alignment, page size, etc.) has not
 *     changed between save and restore.
 * - This function is suitable for in-process checkpoint/rollback
 *   scenarios, not for persistent on-disk serialization.
 *
 * @par Restore Procedure (High-Level)
 *  1. Copy a temporary snapshot header (``slab_t``) from @p buffer.
 *  2. Use the header’s page list (``snap_header.pages``) to count the number
 *     of pages that must be restored.
 *  3. Verify that @p buffer contains enough data for the header and all pages.
 *  4. Verify that the current @p slab has the same geometry as the snapshot
 *     (object size, slot size, alignment, slab bytes, header bytes, objects
 *     per slab).
 *  5. For each page in the snapshot’s page list, copy the saved page contents
 *     into the live page at the same address.
 *  6. Finally, overwrite the live ::slab_t with the snapshot header.
 *
 * @par Error Handling
 * The function returns false and sets errno in the following cases:
 *  - @p slab is NULL or @p buffer is NULL (errno = EINVAL).
 *  - @p buffer_size is too small to contain a full snapshot header
 *    (errno = ERANGE).
 *  - @p buffer_size is too small to contain all pages indicated by the
 *    snapshot (errno = ERANGE).
 *  - The snapshot’s geometry does not match the current slab’s geometry
 *    (errno = EINVAL).
 *
 * On success, @p slab’s internal state (including pages and free list) is
 * restored to the moment when ::save_slab() was called.
 *
 * @par Example
 * @code{.c}
 * // Assume slab has been created and used:
 * slab_t *slab = init_slab_allocator(buddy,
 *                                    sizeof(my_object_t),
 *                                    alignof(my_object_t),
 *                                    0);
 *
 * // ... allocate and use objects ...
 *
 * // 1) Take a snapshot:
 * size_t bytes_needed = 0;
 * (void)save_slab(slab, NULL, 0, &bytes_needed); // expect ERANGE
 *
 * void *snapshot = malloc(bytes_needed);
 * if (!snapshot) {
 *     perror("malloc snapshot");
 *     // handle error...
 * }
 *
 * if (!save_slab(slab, snapshot, bytes_needed, &bytes_needed)) {
 *     perror("save_slab");
 * }
 *
 * // ... mutate slab: allocate/free some objects ...
 *
 * // 2) Restore previous state:
 * if (!restore_slab(slab, snapshot, bytes_needed)) {
 *     perror("restore_slab");
 * }
 *
 * free(snapshot);
 * @endcode
 *
 * @param slab        Pointer to the live ::slab_t instance to restore.
 * @param buffer      Pointer to the snapshot buffer previously filled by
 *                    ::save_slab().
 * @param buffer_size Size of @p buffer in bytes.
 *
 * @retval true  Slab successfully restored from the snapshot.
 * @retval false Invalid arguments, insufficient buffer, or geometry mismatch;
 *               errno set to EINVAL or ERANGE.
 */
bool restore_slab(slab_t *slab,
                  const void *buffer,
                  size_t buffer_size);
// -------------------------------------------------------------------------------- 

/**
 * @brief Format human-readable statistics for a slab allocator.
 *
 * Writes a textual summary of the slab allocator referenced by @p slab into
 * the caller-provided character buffer @p buffer. The summary includes
 * geometry (object size, slot stride, alignment, page size), capacity,
 * usage, block counts, utilization, and a per-page listing.
 *
 * The output is intended for debugging, logging, and diagnostic tools. It
 * is not meant to be parsed programmatically.
 *
 * @par Buffer Semantics
 * - @p buffer must be non-NULL.
 * - @p buffer_size must be greater than 0.
 * - Output is appended using ::_buf_appendf(), which ensures that the buffer
 *   is not overrun and that it is always NUL-terminated on success.
 *
 * If @p slab is NULL, the function writes:
 *
 * @code
 * Slab: NULL\n
 * @endcode
 *
 * to @p buffer and returns true.
 *
 * @par Reported Fields
 * The function reports, among others:
 *
 *  - Object size (bytes)
 *  - Slot stride (bytes)
 *  - Alignment (bytes)
 *  - Page size (bytes) and page header bytes
 *  - Page count
 *  - Blocks per page and total blocks
 *  - In-use blocks
 *  - Free blocks (geometric, derived from totals)
 *  - Free blocks (counted from free list, for cross-check)
 *  - Used bytes, capacity bytes, remaining bytes
 *  - Total footprint (including slab_t struct)
 *  - Utilization percentage (used / capacity)
 *  - Per-page summary: size and block count for each page
 *
 * @par Error Handling
 * - If @p buffer is NULL or @p buffer_size is 0, the function returns false
 *   and sets errno = EINVAL.
 * - If @p slab is NULL, a simple "Slab: NULL" line is written and the
 *   function returns true (no error).
 * - If any call to ::_buf_appendf() fails (e.g., due to insufficient space),
 *   the function returns false. errno is not modified by ::slab_stats()
 *   in that case; callers may consult their own logging or wrap _buf_appendf.
 *
 * @par Example
 * @code{.c}
 * #include <stdio.h>
 *
 * void dump_slab_stats(const slab_t *slab) {
 *     char buf[1024];
 *
 *     if (!slab_stats(slab, buf, sizeof(buf))) {
 *         perror("slab_stats");
 *         return;
 *     }
 *
 *     // Print the formatted statistics.
 *     fputs(buf, stdout);
 * }
 *
 * int main(void) {
 *     buddy_t *buddy = init_buddy_allocator(1<<20, 64, 0);
 *     if (!buddy) {
 *         perror("init_buddy_allocator");
 *         return 1;
 *     }
 *
 *     slab_t *slab = init_slab_allocator(
 *         buddy,
 *         sizeof(int),           // object size
 *         alignof(int),          // alignment
 *         0                      // let slab choose page size
 *     );
 *     if (!slab) {
 *         perror("init_slab_allocator");
 *         free_buddy(buddy);
 *         return 1;
 *     }
 *
 *     // Use the slab: allocate a few ints.
 *     int *a = alloc_slab(slab, true);
 *     int *b = alloc_slab(slab, true);
 *     if (a) *a = 42;
 *     if (b) *b = 7;
 *
 *     // Dump statistics to stdout.
 *     dump_slab_stats(slab);
 *
 *     // Clean up.
 *     return_slab(slab, a);
 *     return_slab(slab, b);
 *     reset_slab(slab);
 *     free_buddy(buddy);
 *
 *     return 0;
 * }
 * @endcode
 *
 * @param slab         Pointer to an initialized ::slab_t instance, or NULL.
 * @param buffer       Output buffer for the formatted statistics string.
 * @param buffer_size  Size of @p buffer in bytes.
 *
 * @retval true  Statistics successfully written into @p buffer.
 * @retval false Invalid arguments or buffer too small for _buf_appendf();
 *               errno is set to EINVAL when arguments are invalid.
 */
bool slab_stats(const slab_t *slab, char *buffer, size_t buffer_size);
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for slab allocation.
 *
 * Allocates a fixed-size object from the slab allocator referenced by @p ctx.
 * The slab has a fixed capacity per slot, exposed via ::slab_stride(); the
 * requested @p size must be greater than 0 and less than or equal to that
 * stride.
 *
 * If @p zeroed is true, the returned memory will be zero-initialized according
 * to the semantics of ::alloc_slab().
 *
 * On error, errno is set (typically to ENOMEM or EINVAL) and NULL is returned.
 *
 * @param ctx    Pointer to a slab_t instance.
 * @param size   Number of bytes requested (must be <= ::slab_stride()).
 * @param zeroed Whether the memory should be zero-initialized.
 *
 * @retval void* Pointer to a block of at least @p size bytes.
 * @retval NULL  On failure, with errno set.
 */
static inline void *slab_v_alloc(void *ctx, size_t size, bool zeroed) {
    slab_t *slab = (slab_t *)ctx;
    if (!slab) {
        errno = EINVAL;
        return NULL;
    }

    if (size == 0u) {
        errno = EINVAL;
        return NULL;
    }

    /* Maximum capacity per slot (exposed via getter). */
    size_t const capacity = slab_stride(slab);
    if (capacity == 0u) {
        /* slab_stride() already sets errno appropriately. */
        if (errno == 0) errno = EINVAL;
        return NULL;
    }

    if (size > capacity) {
        errno = ENOMEM;
        return NULL;
    }

    return alloc_slab(slab, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for aligned slab allocation.
 *
 * Allocates a fixed-size object from the slab allocator referenced by @p ctx,
 * with a minimum alignment requirement of @p align. The slab's alignment is
 * exposed via ::slab_alignment(); if @p align is 0, the slab's natural
 * alignment is used, otherwise @p align must not exceed the slab alignment.
 *
 * The requested @p size must be greater than 0 and less than or equal to the
 * slot stride reported by ::slab_stride().
 *
 * On error, errno is set (typically to ENOMEM or EINVAL) and NULL is returned.
 *
 * @param ctx    Pointer to a slab_t instance.
 * @param size   Number of bytes requested (must be <= ::slab_stride()).
 * @param align  Required alignment (0 for natural, or <= ::slab_alignment()).
 * @param zeroed Whether the memory should be zero-initialized.
 *
 * @retval void* Pointer to an aligned block of at least @p size bytes.
 * @retval NULL  On failure, with errno set.
 */
static inline void *slab_v_alloc_aligned(void *ctx, size_t size,
                                         size_t align, bool zeroed) {
    slab_t *slab = (slab_t *)ctx;
    if (!slab) {
        errno = EINVAL;
        return NULL;
    }

    if (size == 0u) {
        errno = EINVAL;
        return NULL;
    }

    size_t const capacity = slab_stride(slab);
    if (capacity == 0u) {
        if (errno == 0) errno = EINVAL;
        return NULL;
    }
    if (size > capacity) {
        errno = ENOMEM;
        return NULL;
    }

    size_t const slab_align = slab_alignment(slab);
    if (slab_align == 0u) {
        if (errno == 0) errno = EINVAL;
        return NULL;
    }

    if (align != 0u && align > slab_align) {
        errno = EINVAL;
        return NULL;
    }

    /* All slab allocations are at least slab_align-aligned, so they satisfy
       any requested align <= slab_align. */
    return alloc_slab(slab, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for slab reallocation (not supported).
 *
 * Slab allocators manage fixed-size objects and do not support resizing
 * individual allocations. This adapter exists only to satisfy the
 * @ref realloc_prototype interface; it does not perform any reallocation.
 *
 * Semantics:
 *
 *  - If @p old_ptr is NULL, the function returns NULL and performs no action.
 *  - If @p old_ptr is non-NULL, the function simply returns @p old_ptr
 *    unchanged, regardless of @p old_size, @p new_size, or @p zeroed.
 *  - No new memory is allocated, no memory is freed, and errno is not
 *    modified by this function.
 *
 * Callers must not rely on standard realloc semantics when using a slab
 * allocator via this vtable. To change the logical size associated with
 * an object, the caller is responsible for allocating a new object via
 * ::slab_v_alloc() (or the underlying ::alloc_slab()) and performing any
 * data copying and deallocation explicitly.
 *
 * @param ctx       Pointer to a slab_t instance (unused).
 * @param old_ptr   Pointer to the existing allocation (may be NULL).
 * @param old_size  Logical size of the existing allocation (ignored).
 * @param new_size  Requested new logical size (ignored).
 * @param zeroed    Whether any expanded portion must be zero-initialized
 *                  (ignored).
 *
 * @retval void* @p old_ptr, unchanged. May be NULL if @p old_ptr was NULL.
 */
static inline void *slab_v_realloc(void *ctx, void *old_ptr,
                                   size_t old_size, size_t new_size,
                                   bool zeroed) {
    (void)ctx; (void)old_size; (void)new_size; (void)zeroed;
    return old_ptr;
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for aligned slab reallocation (not supported).
 *
 * As with ::slab_v_realloc(), slab allocators do not support resizing
 * existing allocations, even when an explicit alignment is requested.
 * This function exists solely to satisfy the @ref realloc_aligned_prototype
 * interface for slab-backed allocators.
 *
 * Semantics:
 *
 *  - If @p old_ptr is NULL, the function returns NULL and performs no action.
 *  - If @p old_ptr is non-NULL, the function simply returns @p old_ptr
 *    unchanged, regardless of @p old_size, @p new_size, @p zeroed, or
 *    @p align.
 *  - No new memory is allocated, no memory is freed, alignment is not
 *    changed, and errno is not modified by this function.
 *
 * Callers that require true reallocation semantics must implement them
 * explicitly on top of the slab interface (allocate a new object of the
 * desired size/alignment, copy data as needed, and return the old object
 * via ::slab_v_return() / ::return_slab()).
 *
 * @param ctx       Pointer to a slab_t instance (unused).
 * @param old_ptr   Pointer to the existing allocation (may be NULL).
 * @param old_size  Logical size of the existing allocation (ignored).
 * @param new_size  Requested new size (ignored).
 * @param zeroed    Whether any expanded portion must be zero-initialized
 *                  (ignored).
 * @param align     Required alignment (ignored).
 *
 * @retval void* @p old_ptr, unchanged. May be NULL if @p old_ptr was NULL.
 */
static inline void *slab_v_realloc_aligned(void *ctx, void *old_ptr,
                                           size_t old_size, size_t new_size,
                                           bool zeroed, size_t align) {
    (void)ctx; (void)old_size; (void)new_size; (void)zeroed; (void)align;
    return old_ptr;
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for returning an element to a slab allocator.
 *
 * Returns a block previously allocated from the slab allocator referenced
 * by @p ctx back to the allocator. This is a thin adapter over ::return_slab().
 *
 * If @p ctx is NULL, errno is set to EINVAL and the function returns without
 * performing any deallocation. If @p ptr is NULL, the call is treated as
 * a no-op (like `free(NULL)`).
 *
 * @param ctx Pointer to a slab_t instance.
 * @param ptr Pointer to an allocated block (may be NULL).
 */
static inline void slab_v_return(void *ctx, void *ptr) {
    slab_t *slab = (slab_t *)ctx;
    if (!slab) {
        errno = EINVAL;
        return;
    }
    (void)return_slab(slab, ptr);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for logically resetting a slab allocator.
 *
 * For slab-backed allocators, the slab does not own its underlying memory
 * pool; instead, it carves fixed-size objects from a backing allocator.
 * As such, this function does not release the underlying pool; it simply
 * resets the slab's internal state via ::reset_slab(), returning all
 * objects to the free list.
 *
 * After this call, the slab remains valid and may be used again.
 *
 * If @p ctx is NULL, errno is set to EINVAL and the function returns
 * without performing any action.
 *
 * @param ctx Pointer to a slab_t instance.
 */
static inline void slab_v_free(void *ctx) {
    slab_t *slab = (slab_t *)ctx;
    if (!slab) {
        errno = EINVAL;
        return;
    }
    (void)reset_slab(slab);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Constructs an allocator_vtable_t for a given slab allocator.
 *
 * This helper initializes an allocator_vtable_t that exposes a ::slab_t
 * instance through the generic allocator interface. All operations
 * (allocate, reallocate, deallocate, etc.) are forwarded to the
 * slab-backed vtable adapter functions.
 *
 * Because a slab has fixed slot capacity and alignment, allocation and
 * reallocation requests are subject to additional constraints:
 *
 *  - The requested size must be > 0 and <= ::slab_stride().
 *  - The requested alignment must be 0 or <= ::slab_alignment().
 *  - Reallocation cannot grow blocks beyond the slot capacity; attempts
 *    to do so fail with errno = ENOMEM and return NULL (the caller keeps
 *    the original pointer).
 *
 * @param slab Pointer to a slab_t instance to use as the allocator backend.
 *
 * @return A fully initialized allocator_vtable_t bound to @p slab.
 */
static inline allocator_vtable_t slab_allocator(slab_t *slab) {
    allocator_vtable_t v = {
        .allocate           = slab_v_alloc,
        .allocate_aligned   = slab_v_alloc_aligned,
        .reallocate         = slab_v_realloc,
        .reallocate_aligned = slab_v_realloc_aligned,
        .return_element     = slab_v_return,
        .deallocate         = slab_v_free,
        .ctx                = slab
    };
    return v;
}

// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* arena_H */
// ================================================================================
// ================================================================================
// eof
