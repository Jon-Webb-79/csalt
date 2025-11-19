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
// INTERNAL ARENA 

typedef struct iarena_t iarena_t;
// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a sub-arena (iarena_t) inside an existing parent arena.
 *
 * This function reserves a single, contiguous, bump-allocated slice of memory
 * from a parent `arena_t`, constructs an `iarena_t` header at the beginning of
 * that slice, and returns a lightweight sub-arena capable of fast bump
 * allocation (`alloc_iarena`, `alloc_iarena_aligned`, `realloc_iarena`,
 * `ralloc_iarena_aligned`, etc.).
 *
 * @param parent       Parent arena from which memory is carved.
 * @param bytes        Total bytes to reserve (header + padding + payload).
 * @param alignment    Requested payload alignment (power-of-two). If zero,
 *                     the parent arena's alignment is used.
 *
 * @return Pointer to a newly created iarena_t, or NULL on error.*
 *
 * The sub-arena:
 *  - does **not** grow dynamically (its slice is fixed-size)
 *  - is always **contiguous** (no Chunk list)
 *  - maintains its own cursor, alignment, and statistics
 *  - never frees memory; `reset_iarena()` simply rewinds the cursor
 *
 * Alignment semantics:
 *  - `alignment == 0` → use parent->alignment.
 *  - Otherwise, effective payload alignment is:
 *
 *        max(alignment, parent->alignment)
 *
 *  - The iarena header itself is aligned to:
 *
 *        max(effective_payload_alignment, alignof(iarena_t))
 *
 * Capacity semantics:
 *  - `bytes` is the total reserved size, including:
 *      - the iarena_t header,
 *      - padding after the header,
 *      - and the user payload region.
 *  - The usable payload size is stored in `ia->alloc`.
 *
 * Error handling:
 *  - Returns NULL and sets errno on failure:
 *      - `EINVAL`: invalid arguments, invalid alignment, or `bytes` too small.
 *      - `ENOMEM`: parent arena cannot provide a `bytes`-sized slice.
 *
 * Lifetime:
 *  - The returned iarena_t is allocated *inside* the parent arena.
 *  - Do **not** free it; it becomes invalid only if the parent arena is reset
 *    past the slice or destroyed.
 *
 * @code{.c}
 * // Example: Creating an iarena from a parent arena and allocating from it
 *
 * #include <stdalign.h>
 *
 * // Create a parent arena of 8 KiB
 * arena_t* parent = init_dynamic_arena(
 *     8192,              // total bytes
 *     false,             // resizing disabled
 *     4096,              // min_chunk
 *     alignof(max_align_t)
 * );
 *
 * if (!parent) {
 *     perror("init_dynamic_arena");
 *     return;
 * }
 *
 * // Create a 2 KiB sub-arena with 64-byte alignment
 * iarena_t* ia = init_iarena_with_arena(parent, 2048, 64);
 * if (!ia) {
 *     perror("init_iarena_with_arena");
 *     free_arena(parent);
 *     return;
 * }
  * @endcode
  */
iarena_t* init_iarena_with_arena(arena_t* parent,
                                 size_t bytes,
                                 size_t alignment);
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate memory from an iarena_t using fast bump allocation.
 *
 * This function allocates a block of `bytes` from the sub-arena `ia` using a
 * single pointer bump (`cur += pad + bytes`).  The returned block begins at an
 * address aligned to `ia->alignment`, which is fixed when the iarena is
 * created via `init_iarena_with_arena()`.
 *
 * Allocation semantics:
 *  - All allocations are contiguous and linear.
 *  - No free operation exists; memory is reclaimed only through
 *    `reset_iarena()` or by destroying/resetting the parent arena.
 *  - The iarena is a **fixed-size slice**. If the requested block does not fit,
 *    the function fails with `ENOMEM`.
 *
 * Zeroing semantics:
 *  - If `zeroed == true`, the returned block is cleared using `memset()`.
 *  - Otherwise, the memory contains whatever values were present previously.
 *
 * Charging semantics:
 *  - The internal statistic `ia->len` is incremented by:
 *
 *        (alignment padding) + bytes
 *
 *    This mirrors the behavior of the parent arena implementation.
 *
 * Error handling:
 *  - Returns NULL and sets errno on error:
 *      - `EINVAL`: ia == NULL, bytes == 0, or invalid alignment.
 *      - `ENOMEM`: allocation exceeds the fixed capacity of the iarena.
 *
 * @code{.c}
 * // Example: Using alloc_iarena() inside a sub-arena
 *
 * arena_t* parent = init_dynamic_arena(
 *     8192,               // total bytes
 *     false,              // resizing disabled
 *     4096,               // min_chunk
 *     alignof(max_align_t)
 * );
 *
 * iarena_t* ia = init_iarena_with_arena(parent, 2048, 64);
 *
 * // Allocate a 128-byte block, zero-initialized
 * void* p = alloc_iarena(ia, 128, true);
 * if (!p) {
 *     perror("alloc_iarena");
 * } else {
 *     memset(p, 0xAB, 128);   // safe to write
 * }
 *
 * // Allocate a non-zeroed struct
 * typedef struct { int id; float mass; } Object;
 * Object* obj = (Object*)alloc_iarena(ia, sizeof(Object), false);
 *
 * // Reset sub-arena when done
 * reset_iarena(ia);
 *
 * free_arena(parent);
 * @endcode
 *
 * @param ia       Pointer to a valid iarena_t.
 * @param bytes    Number of bytes to allocate (must be > 0).
 * @param zeroed   If true, newly allocated memory is zero-filled.
 *
 * @return Pointer to the aligned allocated block, or NULL on failure.
 */

void* alloc_iarena(iarena_t* ia, size_t bytes, bool zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate an aligned memory block from an `iarena_t`.
 *
 * This function allocates `bytes` from a sub-arena (`iarena_t`) with a required
 * alignment that may exceed the sub-arena's default alignment.
 *
 * Alignment semantics:
 *  - The sub-arena has a built-in base alignment (`ia->alignment`), which is
 *    always a power of two and is established by `init_iarena_with_arena()`.
 *  - The caller may request a stronger alignment via `align`.
 *  - If `align == 0`, the sub-arena’s base alignment is used.
 *  - Otherwise `align` must be a power of two.
 *  - The final effective alignment used is:
 *
 *        A = max( align, ia->alignment )
 *
 * Allocation semantics:
 *  - `alloc_iarena_aligned()` performs a bump allocation:
 *
 *        p = align_up(cur, A)
 *        next = p + bytes
 *
 *  - If `next` exceeds the iarena’s capacity (`ia->end`), the allocation fails
 *    with `ENOMEM`.
 *  - On success:
 *        - `ia->cur` is advanced to `next`
 *        - `ia->len` is increased by `(padding + bytes)`
 *        - The returned pointer is guaranteed to be A-aligned
 *
 * Zeroing semantics:
 *  - If `zeroed == true`, the allocated block is cleared with `memset()`.
 *
 * Error handling:
 *  - Returns NULL and sets errno:
 *      - `EINVAL` if `ia` is NULL, `bytes == 0`, or alignment is invalid.
 *      - `ENOMEM` if insufficient capacity remains in the fixed-size iarena.
 *
 * @code{.c}
 * // Example: Allocating aligned memory from a sub-arena
 *
 * arena_t* parent = init_dynamic_arena(
 *     8192,               // total bytes
 *     false,              // resizing disabled
 *     4096,               // min_chunk
 *     alignof(max_align_t)
 * );
 *
 * // Create a 4 KiB sub-arena with 32-byte base alignment
 * iarena_t* ia = init_iarena_with_arena(parent, 4096, 32);
 *
 * // Allocate a 256-byte block aligned to 128 bytes (stronger than base)
 * void* p = alloc_iarena_aligned(ia, 256, 128, true);
 *
 * if (!p) {
 *     perror("alloc_iarena_aligned");
 * } else {
 *     // Guaranteed: ((uintptr_t)p % 128) == 0
 *     memset(p, 0xCD, 256);
 * }
 *
 * // Normal allocations still respect the sub-arena's base alignment
 * void* q = alloc_iarena_aligned(ia, 64, 0, false); // uses ia->alignment==32
 *
 * reset_iarena(ia);   // rewind; fast reuse
 * free_arena(parent); // destroys everything including the iarena slice
 * @endcode
 *
 * @param ia       Pointer to an initialized `iarena_t`.
 * @param bytes    Size of the block to allocate (must be > 0).
 * @param align    Requested alignment (power-of-two). If 0, defaults to the
 *                 sub-arena's base alignment.
 * @param zeroed   If true, the allocated block is set to zero.
 *
 * @return Aligned allocated block pointer on success, or NULL on failure.
 */

void* alloc_iarena_aligned(iarena_t* ia,
                           size_t bytes,
                           size_t align,
                           bool zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reallocate an existing iarena allocation to a new size.
 *
 * This function provides a `realloc`-like interface for memory that was
 * previously allocated from an `iarena_t`. Because an iarena is a
 * monotonic (bump) allocator, *in-place* growth or shrink is impossible.
 *
 * Instead, `realloc_iarena()` always behaves as:
 *
 *   1. Validate that `var` belongs to this iarena (if non-NULL).
 *   2. Allocate a new block of size `new_size`.
 *   3. Copy `min(old_size, new_size)` bytes from the old block.
 *   4. Return the new pointer.
 *
 * The old block is **not freed** and becomes dead space inside the
 * sub-arena. This is normal and expected for bump allocators.
 *
 * Behavior with NULL pointers:
 *  - If `var == NULL`, the call is equivalent to:
 *
 *        alloc_iarena(ia, new_size, zeroed)
 *
 * Zeroing semantics:
 *  - If `zeroed == true`, the *new* block is zero-initialized before the copy.
 *
 * Error handling:
 *  - Returns NULL and sets errno:
 *      - `EINVAL` if `ia == NULL`
 *      - `EPERM`  if `var` does not belong to this iarena
 *      - `ENOMEM` if the iarena does not have enough remaining capacity
 *  - On failure, the original block remains valid and untouched.
 *
 * Limitations:
 *  - This function cannot shrink in place.
 *  - It cannot free previous allocations.
 *  - It may fragment the iarena if used heavily.
 *
 * @code{.c}
 * // Example: Growing a struct inside an iarena
 *
 * arena_t* parent = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
 * iarena_t* ia   = init_iarena_with_arena(parent, 4096, 64);
 *
 * // Allocate an initial 32-byte block
 * size_t old_size = 32;
 * uint8_t* p = alloc_iarena(ia, old_size, false);
 * memset(p, 0xAB, old_size);
 *
 * // Grow it to 96 bytes
 * size_t new_size = 96;
 * uint8_t* q = realloc_iarena(ia, p, old_size, new_size, false);
 * if (!q) {
 *     perror("realloc_iarena");
 *     // p is still valid
 * } else {
 *     // q now holds the first 32 bytes copied from p
 *     // ((uintptr_t)q % ia->alignment) == 0
 * }
 *
 * // Reset the entire sub-arena to reclaim all bump allocations
 * reset_iarena(ia);
 * free_arena(parent);
 * @endcode
 *
 * @param ia        Pointer to the sub-arena.
 * @param var       Existing block pointer or NULL.
 * @param old_size  Size of the existing block (required for safe copying).
 * @param new_size  Desired new size (must be > 0).
 * @param zeroed    Whether the newly allocated block should be zeroed.
 *
 * @return Pointer to the newly allocated block, or NULL on error.
 */
void* realloc_iarena(iarena_t* ia,
                     void* var,
                     size_t old_size,
                     size_t new_size,
                     bool zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reallocate an iarena allocation with an explicit alignment requirement.
 *
 * This function behaves like a `realloc()` variant for memory obtained from an
 * `iarena_t`, but with control over the alignment of the *new* allocation.
 *
 * Because an iarena is a monotonic (bump) allocator, it **cannot** shrink or
 * grow an existing allocation in place. Therefore, this function *always*
 * allocates a fresh block and copies data into it.
 *
 * Reallocation semantics:
 *  - If `var == NULL`, the call behaves exactly like:
 *
 *        alloc_iarena_aligned(ia, new_size, alignment, zeroed)
 *
 *  - If `var != NULL`, the function checks that the pointer belongs to this
 *    iarena (`is_iarena_ptr_sized()`).
 *  - A new block is allocated with effective alignment:
 *
 *        A = max( alignment, ia->alignment )
 *
 *  - The first `min(old_size, new_size)` bytes are copied from `var`
 *    into the new block.
 *  - The old block remains valid but becomes unreachable dead space
 *    inside the bump allocator.
 *
 * Alignment semantics:
 *  - If `alignment == 0`, the iarena's base alignment is used.
 *  - Otherwise, `alignment` must be a power-of-two.
 *  - Final alignment is always:
 *
 *        max(alignment, ia->alignment)
 *
 * Zeroing semantics:
 *  - If `zeroed == true`, the newly allocated block is zero-initialized
 *    *before* the copy is performed.
 *
 * Error handling:
 *  - Returns NULL and sets errno:
 *      - `EINVAL` if `ia == NULL` or alignment is invalid.
 *      - `EPERM`  if `var` does not belong to this iarena.
 *      - `ENOMEM` if insufficient space remains for the new allocation.
 *  - On error, `var` remains valid and unmodified.
 *
 * @code{.c}
 * // Example: Reallocating a block with stronger alignment
 *
 * arena_t* parent = init_dynamic_arena(
 *     8192, false, 4096, alignof(max_align_t)
 * );
 *
 * iarena_t* ia = init_iarena_with_arena(parent, 4096, 64);
 *
 * // Initial allocation: 48 bytes, 64-byte alignment
 * size_t old_size = 48;
 * uint8_t* p = alloc_iarena_aligned(ia, old_size, 64, false);
 * memset(p, 0xEE, old_size);
 *
 * // Reallocate with stronger 128-byte alignment and larger size
 * size_t new_size = 128;
 * uint8_t* q = ralloc_iarena_aligned(
 *     ia,
 *     p,
 *     old_size,
 *     new_size,
 *     false,
 *     128
 * );
 *
 * if (!q) {
 *     perror("ralloc_iarena_aligned");
 *     // p remains valid
 * } else {
 *     // q is 128-byte aligned
 *     // q contains the first 48 bytes copied from p
 * }
 *
 * reset_iarena(ia);   // Reclaim all bump allocations
 * free_arena(parent); // Destroys entire parent + all sub-arenas
 * @endcode
 *
 * @param ia         Sub-arena from which memory is allocated.
 * @param var        Existing block pointer or NULL.
 * @param old_size   Size of the existing block.
 * @param new_size   Requested size of the new block.
 * @param zeroed     Whether newly allocated memory should be zero-filled.
 * @param alignment  Requested alignment (power-of-two). Zero means "use
 *                   iarena's base alignment".
 *
 * @return Pointer to the new allocation, or NULL on failure.
 */
void* realloc_iarena_aligned(iarena_t* ia,
                             void* var,
                             size_t old_size,
                             size_t new_size,
                             bool zeroed,
                             size_t alignment);
// -------------------------------------------------------------------------------- 

/**
 * @brief Check whether a pointer lies within the payload region of an iarena.
 *
 * This function verifies whether `ptr` is a valid address *inside* the memory
 * slice owned by an `iarena_t`. It does **not** check the size of any
 * particular allocation—only whether the pointer lies within the range:
 *
 *      [ ia->begin , ia->end )
 *
 * This is commonly used for defensive programming, debugging, and validating
 * inputs passed to functions like `realloc_iarena()` or custom container
 * implementations that rely on iarena allocations.
 *
 * Error handling:
 *  - Returns `false` and sets errno = `EINVAL` if:
 *      - `ia == NULL`, or
 *      - `ptr == NULL`.
 *
 * Notes:
 *  - This function cannot determine whether `ptr` points to the *start* of a
 *    valid allocation—only that it resides somewhere inside the usable region.
 *  - To validate a pointer *and* its size, use `is_iarena_ptr_sized()`.
 *
 * @code{.c}
 * // Example: Checking whether a pointer belongs to an iarena
 *
 * iarena_t* ia = init_iarena_with_arena(parent, 4096, 32);
 * void* p = alloc_iarena(ia, 128, false);
 *
 * assert_true(is_iarena_ptr(ia, p));            // inside
 * assert_false(is_iarena_ptr(ia, (void*)0x1));  // outside
 *
 * reset_iarena(ia);
 * @endcode
 *
 * @param ia   The sub-arena to check against.
 * @param ptr  The pointer to validate.
 *
 * @return `true` if ptr lies inside the iarena's usable region, otherwise `false`.
 */

bool is_iarena_ptr(const iarena_t* ia, const void *ptr);
// -------------------------------------------------------------------------------- 

/**
 * @brief Check whether a pointer and its size lie entirely within an iarena.
 *
 * This function validates not only that the pointer `ptr` lies within the
 * usable region of an `iarena_t`, but also that a block of `size` bytes
 * beginning at `ptr` is fully contained inside the iarena slice:
 *
 *      ptr ∈ [ ia->begin , ia->end )
 *      ptr + size ≤ ia->end
 *
 * It also protects against integer overflow when computing `ptr + size`.
 *
 * This is the safest way to validate an existing allocation before performing
 * operations such as:
 *  - `realloc_iarena()`
 *  - `ralloc_iarena_aligned()`
 *  - custom container modifications
 *
 * Error handling:
 *  - Returns `false` and sets errno = `EINVAL` if:
 *      - `ia == NULL`, or
 *      - `ptr == NULL`.
 *
 * Notes:
 *  - The function does **not** verify that the memory at `ptr` was created by
 *    a *single* call to `alloc_iarena()`—only that `[ptr, ptr+size)` lies
 *    entirely inside the bump-allocation region.
 *  - Overflow is explicitly checked: if `ptr + size` overflows `uintptr_t`,
 *    the function safely returns `false`.
 *
 * @code{.c}
 * // Example: Validating a block before reallocating it
 *
 * iarena_t* ia = init_iarena_with_arena(parent, 4096, 64);
 *
 * uint8_t* buf = alloc_iarena(ia, 200, false);
 * assert_non_null(buf);
 *
 * // Safe: entire 200-byte region lies inside the sub-arena.
 * assert_true(is_iarena_ptr_sized(ia, buf, 200));
 *
 * // Not safe: too large or outside bounds.
 * assert_false(is_iarena_ptr_sized(ia, buf, 5000));
 *
 * // Attempting to reallocate only after confirming safety:
 * if (is_iarena_ptr_sized(ia, buf, 200)) {
 *     uint8_t* new_buf = realloc_iarena(ia, buf, 200, 400, false);
 * }
 *
 * reset_iarena(ia);
 * @endcode
 *
 * @param ia     The sub-arena to check against.
 * @param ptr    The starting address of the region.
 * @param size   The length of the region in bytes.
 *
 * @return `true` if the entire `[ptr, ptr+size)` region lies inside the iarena,
 *         otherwise `false`.
 */

bool is_iarena_ptr_sized(const iarena_t* ia, const void* ptr, size_t size);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reset an iarena_t to an empty state.
 *
 * This function rewinds the sub-arena's bump pointer (`cur`) back to the start
 * of its payload region (`begin`), effectively discarding **all** allocations
 * previously made within the iarena. It also resets the internal accounting
 * field (`len`) to zero.
 *
 * No memory is freed, and no bytes are overwritten. The allocated slice inside
 * the parent arena remains intact; only the bump pointer is reset. This makes
 * the operation extremely fast—typically just two assignments and no loops.
 *
 * Typical use cases:
 *  - Reusing a sub-arena as a temporary workspace.
 *  - Efficient multi-stage algorithms that allocate scratch storage and then
 *    discard all temporary state at once.
 *  - Resetting an iarena between frames in high-performance systems.
 *
 * Safety:
 *  - It is safe to call `reset_iarena()` multiple times.
 *  - Any pointers returned by prior `alloc_iarena()` or
 *    `alloc_iarena_aligned()` calls become invalid immediately after the reset.
 *
 * Error handling:
 *  - If `ia == NULL`, the function sets `errno = EINVAL` and performs no action.
 *
 * @code{.c}
 * // Example: Using an iarena as a temporary workspace
 *
 * arena_t* parent = init_dynamic_arena(8192, false, 4096, alignof(max_align_t));
 *
 * iarena_t* ia = init_iarena_with_arena(parent, 4096, 64);
 *
 * // Allocate some objects
 * void* a = alloc_iarena(ia, 128, false);
 * void* b = alloc_iarena(ia, 256, false);
 *
 * // Reset the sub-arena: a and b are now invalid
 * reset_iarena(ia);
 *
 * // New temporary allocations
 * void* c = alloc_iarena(ia, 512, true);
 *
 * reset_iarena(ia);   // discard c
 * free_arena(parent); // destroys everything, including iarena header + memory
 * @endcode
 *
 * @param ia   Pointer to the iarena to reset.
 */
void reset_iarena(iarena_t* ia);
// -------------------------------------------------------------------------------- 

typedef struct {
    const void* owner;     /* identity of the subarena (opaque pointer to iarena_t) */
    size_t      used;      /* bytes used at save (cur - begin) */
    size_t      capacity;  /* total usable capacity at save (end - begin) */
    size_t      alignment; /* effective alignment at save */
    uint32_t    magic;     /* guard for basic corruption detection */
} iArenaCheckPoint;

/**
 * @brief Create a checkpoint capturing the current state of an iarena.
 *
 * This function records enough information about an `iarena_t` to later restore
 * its allocation state using `restore_iarena()`. The checkpoint captures:
 *
 *   - the iarena pointer (`owner`)
 *   - the number of bytes currently used (`used`)
 *   - the total usable capacity (`capacity`)
 *   - the current alignment requirement (`alignment`)
 *   - a magic value (`IARENA_CP_MAGIC`) for integrity checking
 *
 * A checkpoint behaves like a "bookmark" into the bump-allocator’s state. It
 * allows undoing a sequence of allocations *without* resetting the entire
 * sub-arena.
 *
 * Checkpoint semantics:
 *  - The checkpoint is **immutable** after creation.
 *  - A checkpoint is valid only for the **same iarena instance**.
 *  - The underlying `iarena_t` must not be reset or invalidated before
 *    restoration.
 *
 * Error handling:
 *  - If `ia == NULL`, the function sets `errno = EINVAL` and returns an
 *    all-zero checkpoint (which is guaranteed to fail validation).
 *
 * @code{.c}
 * // Example: Saving and restoring an iarena checkpoint
 *
 * iarena_t* ia = init_iarena_with_arena(parent, 4096, 64);
 *
 * void* a = alloc_iarena(ia, 128, false);
 *
 * // Save state after one allocation
 * iArenaCheckPoint cp = save_iarena(ia);
 *
 * void* b = alloc_iarena(ia, 256, false);
 * void* c = alloc_iarena(ia, 512, false);
 *
 * // Restore: b and c become invalid; next allocation will reuse their space
 * restore_iarena(ia, cp);
 *
 * void* d = alloc_iarena(ia, 300, false);  // allocated from where b was
 * @endcode
 *
 * @param ia  The iarena to checkpoint.
 *
 * @return A checkpoint describing the current state of `ia`. Valid only for
 *         restoring this same iarena.
 */
iArenaCheckPoint save_iarena(const iarena_t* ia);
// -------------------------------------------------------------------------------- 

/**
 * @brief Restore an iarena_t to a previously saved checkpoint.
 *
 * This function restores the allocation state of an iarena to a checkpoint
 * created by `save_iarena()`. All allocations made after the checkpoint are
 * discarded, and the bump pointer (`cur`) is rewound accordingly.
 *
 * Restoration semantics:
 *  - The checkpoint must have been created from the **same iarena instance**.
 *  - The iarena must not have changed size or alignment since the checkpoint
 *    (fixed-slice design ensures this under normal use).
 *  - Any pointers obtained after the checkpoint immediately become invalid.
 *
 * Integrity validation:
 *  - The checkpoint is validated using:
 *      - `magic` (must equal `IARENA_CP_MAGIC`)
 *      - `owner` (must match the `ia` pointer)
 *      - `capacity` (must match the current iarena capacity)
 *      - `alignment` (must equal the iarena's current alignment)
 *      - `used <= capacity`
 *
 * Error handling:
 *  - On failure, the function returns `false` and sets errno:
 *      - `EINVAL` if the checkpoint is invalid or incompatible.
 *
 * @code{.c}
 * // Example: Restoring an iarena checkpoint
 *
 * iArenaCheckPoint cp = save_iarena(ia);
 *
 * void* tmp1 = alloc_iarena(ia, 400, false);
 * void* tmp2 = alloc_iarena(ia, 200, false);
 *
 * // Undo tmp1 and tmp2:
 * if (!restore_iarena(ia, cp)) {
 *     perror("restore_iarena");
 * }
 *
 * // Now ia->cur is exactly where it was when cp was created
 * void* p = alloc_iarena(ia, 128, false);
 * @endcode
 *
 * @param ia  The iarena whose state is being restored.
 * @param cp  A checkpoint previously returned by `save_iarena()`.
 *
 * @return `true` on success, or `false` if validation fails.
 */
bool restore_iarena(iarena_t *ia, iArenaCheckPoint cp);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the number of bytes remaining in an iarena_t.
 *
 * This function computes the remaining capacity of an `iarena_t` by subtracting
 * the current bump pointer (`cur`) from the fixed end of the slice (`end`):
 *
 *      remaining = end - cur
 *
 * Because an iarena is a fixed-size, contiguous bump allocator, this value
 * represents the exact number of bytes available for future allocations using
 * `alloc_iarena()` or `alloc_iarena_aligned()`.
 *
 * Error handling:
 *  - If `ia == NULL`, returns 0 and sets `errno = EINVAL`.
 *  - If the internal invariant `cur <= end` is violated, returns 0 and sets
 *    `errno = EFAULT`. (This condition should never occur in correct code.)
 *
 * Notes:
 *  - This function does *not* consider alignment padding required by the next
 *    allocation. A request for fewer than `iarena_remaining()` bytes may still
 *    fail if alignment padding causes the bump pointer to exceed `end`.
 *  - For actual availability, callers should perform allocation and check for
 *    `NULL` rather than rely solely on this function.
 *
 * @code{.c}
 * // Example: Checking remaining capacity in an iarena
 *
 * iarena_t* ia = init_iarena_with_arena(parent, 4096, 64);
 *
 * size_t remaining_before = iarena_remaining(ia);
 *
 * void* p = alloc_iarena(ia, 256, false);
 *
 * size_t remaining_after  = iarena_remaining(ia);
 *
 * printf("Used: %zu bytes\n", remaining_before - remaining_after);
 *
 * reset_iarena(ia);   // Restores remaining == full capacity
 * @endcode
 *
 * @param ia   Pointer to an initialized `iarena_t`.
 *
 * @return The number of bytes available for additional allocations, or 0 on error.
 */
size_t iarena_remaining(const iarena_t* ia);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the memory type (STATIC or DYNAMIC) of the parent arena.
 *
 * An `iarena_t` does not itself have a memory type, since it always uses
 * memory carved from its parent `arena_t`. This function simply forwards the
 * query to the parent arena and returns the same value that the parent reports:
 *
 *      STATIC  → the parent arena was created with fixed storage
 *      DYNAMIC → the parent arena may grow by allocating additional chunks
 *
 * This is primarily useful for diagnostics, statistics, and determining
 * whether behaviors such as parent-side growth or chunk allocation are enabled.
 *
 * Error handling:
 *  - If `ia == NULL` or `ia->arena == NULL`:
 *        errno = EINVAL
 *        return ALLOC_INVALID
 *
 * Notes:
 *  - This does *not* reflect properties of the iarena slice, which is always
 *    fixed-size and never grows.
 *
 * @code{.c}
 * // Example: Checking memory type of an iarena's parent arena
 *
 * iarena_t* ia = init_iarena_with_arena(parent, 4096, 64);
 *
 * alloc_t type = iarena_mtype(ia);
 * if (type == STATIC) {
 *     printf("Parent arena is STATIC.\n");
 * } else if (type == DYNAMIC) {
 *     printf("Parent arena is DYNAMIC.\n");
 * } else {
 *     printf("Invalid iarena or parent.\n");
 * }
 *
 * @endcode
 *
 * @param ia  The sub-arena whose parent memory type should be queried.
 *
 * @return The parent's memory type, or `ALLOC_INVALID` on error.
 */
alloc_t iarena_mtype(const iarena_t* ia);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the number of bytes consumed so far inside an iarena.
 *
 * This function returns the internal accounting field `ia->len`, which tracks
 * the total number of bytes charged against the sub-arena. Each allocation
 * increments this value by:
 *
 *      (alignment padding required) + (bytes requested)
 *
 * Therefore, `iarena_size()` yields the number of bytes *logically consumed*,
 * not necessarily the sum of user-requested payload sizes.
 *
 * This value is analogous to `arena_size()` in the parent allocator.
 *
 * Error handling:
 *  - If `ia == NULL`, sets `errno = EINVAL` and returns 0.
 *
 * Notes:
 *  - This function does *not* represent how much memory is *remaining*. Use
 *    `iarena_remaining()` for that.
 *  - `iarena_size()` does *not* shrink when objects go out of scope; it grows
 *    monotonically until a call to `reset_iarena()` or `restore_iarena()`.
 *
 * @code{.c}
 * // Example: Tracking usage inside a sub-arena
 *
 * iarena_t* ia = init_iarena_with_arena(parent, 4096, 64);
 *
 * printf("Initial size: %zu\n", iarena_size(ia));   // 0
 *
 * void* p = alloc_iarena(ia, 128, false);
 * printf("After alloc: %zu\n", iarena_size(ia));
 *
 * void* q = alloc_iarena_aligned(ia, 200, 128, false);
 * printf("After aligned alloc: %zu\n", iarena_size(ia));
 *
 * reset_iarena(ia);
 * printf("After reset: %zu\n", iarena_size(ia));    // 0
 * @endcode
 *
 * @param ia  Pointer to the iarena.
 *
 * @return Number of bytes consumed inside the iarena, or 0 on error.
 */
size_t iarena_size(const iarena_t* ia);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the usable payload capacity of an iarena_t.
 *
 * This function returns `ia->alloc`, which represents the total number of bytes
 * available for user allocations inside the sub-arena’s payload region.
 * 
 * This corresponds to:
 *
 *      alloc = end - begin
 *
 * where:
 *   - `begin` is the first usable payload byte after the iarena header
 *   - `end`   is the end of the slice carved from the parent arena
 *
 * Unlike `iarena_size()`, which tracks consumption, `iarena_alloc()` reports the
 * fixed maximum usable size of the sub-arena. It never changes after
 * initialization because an iarena is always a fixed-size slice.
 *
 * Error handling:
 *  - If `ia == NULL`, sets `errno = EINVAL` and returns 0.
 *
 * @code{.c}
 * iarena_t* ia = init_iarena_with_arena(parent, 4096, 64);
 *
 * size_t cap = iarena_alloc(ia);
 * printf("Usable payload capacity: %zu bytes\n", cap);
 *
 * // After allocations, iarena_alloc() does not change:
 * alloc_iarena(ia, 200, false);
 * printf("%zu\n", iarena_alloc(ia));  // same value
 * @endcode
 *
 * @param ia  Pointer to the sub-arena.
 * @return The total usable payload capacity, or 0 on error.
 */
size_t iarena_alloc(const iarena_t* ia);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the total number of bytes reserved for the iarena slice.
 *
 * This function returns `ia->tot_alloc`, the number of bytes consumed in the
 * *parent arena* to construct the sub-arena. This includes:
 *
 *    sizeof(iarena_t)              → header
 *  + alignment padding after header
 *  + usable payload capacity
 *
 * Therefore:
 *
 *      tot_alloc >= iarena_alloc()
 *
 * This function is useful for diagnostics, statistics, and allocator
 * introspection but is not commonly used during allocation.
 *
 * Error handling:
 *  - If `ia == NULL`, sets `errno = EINVAL` and returns 0.
 *
 * @code{.c}
 * iarena_t* ia = init_iarena_with_arena(parent, 4096, 64);
 *
 * printf("Total slice reserved: %zu bytes\n",
 *        total_iarena_alloc(ia));   // typically 4096
 *
 * printf("Usable payload: %zu bytes\n",
 *        iarena_alloc(ia));
 *
 * // tot_alloc >= alloc always holds
 * @endcode
 *
 * @param ia  Pointer to the sub-arena.
 * @return Total reserved bytes (header + padding + payload), or 0 on error.
 */
size_t total_iarena_alloc(const iarena_t* ia);
// -------------------------------------------------------------------------------- 

/**
 * @brief Return the effective payload alignment for allocations in an iarena.
 *
 * This function returns `ia->alignment`, which is the minimum alignment
 * guaranteed for all allocations made through:
 *
 *    alloc_iarena()
 *    alloc_iarena_aligned()
 *    realloc_iarena()
 *    ralloc_iarena_aligned()
 *
 * This value is determined when the iarena is created via
 * `init_iarena_with_arena()` and is equal to:
 *
 *      max(requested_alignment, parent->alignment)
 *
 * Alignment is always a power-of-two.
 *
 * Error handling:
 *  - If `ia == NULL`, sets `errno = EINVAL` and returns 0.
 *
 * @code{.c}
 * iarena_t* ia = init_iarena_with_arena(parent, 4096, 64);
 *
 * size_t A = iarena_alignment(ia);
 * printf("Base alignment: %zu bytes\n", A);   // e.g., 64
 *
 * // alloc_iarena() will always produce A-aligned pointers
 * void* p = alloc_iarena(ia, 128, false);
 * assert(((uintptr_t)p % A) == 0);
 *
 * @endcode
 *
 * @param ia  Pointer to the sub-arena.
 * @return The alignment (power-of-two) used by the sub-arena, or 0 on error.
 */
size_t iarena_alignment(const iarena_t* ia);
// -------------------------------------------------------------------------------- 

/**
 * @brief Write human-readable statistics for an iarena_t into a text buffer.
 *
 * This function prints a formatted summary describing the internal state of an
 * `iarena_t`. The output is appended to `buffer` using `_buf_appendf()`, which
 * ensures the buffer is never overrun and truncates safely if needed.
 *
 * The statistics include:
 *  - Whether the iarena has a parent `arena_t`
 *  - Bytes used  (`cur - begin`)
 *  - Capacity    (`end - begin`, equivalent to `iarena_alloc()`)
 *  - Total bytes reserved from the parent (`tot_alloc`)
 *  - Remaining space (`end - cur`)
 *  - Alignment used for all allocations
 *  - Utilization percentage (`used / capacity * 100`)
 *
 * Unlike the parent arena (which may have multiple chunks), an iarena is always
 * a **single contiguous slice**. Therefore, this function does not print a
 * chunk list.
 *
 * Error handling:
 *  - If `buffer == NULL` or `buffer_size == 0`, sets `errno = EINVAL` and
 *    returns false.
 *  - If `ia == NULL`, writes `"iArena: NULL\n"` into the buffer and returns true.
 *  - If `_buf_appendf()` fails due to truncation or formatting, returns false.
 *
 * Notes:
 *  - The caller is responsible for providing a sufficiently large buffer
 *    (typically 256–512 bytes).
 *  - The function never writes outside the provided buffer.
 *
 * @code{.c}
 * // Example: Printing iarena statistics
 *
 * arena_t* parent = init_dynamic_arena(
 *     8192, false, 4096, alignof(max_align_t));
 *
 * iarena_t* ia = init_iarena_with_arena(parent, 2048, 64);
 *
 * // Perform some allocations
 * alloc_iarena(ia, 128, false);
 * alloc_iarena(ia, 256, false);
 *
 * char buf[512];
 * if (iarena_stats(ia, buf, sizeof(buf))) {
 *     printf("%s\n", buf);
 * } else {
 *     perror("iarena_stats");
 * }
 *
 * reset_iarena(ia);
 * free_arena(parent);
 * @endcode
 *
 * @param ia           Pointer to the sub-arena to describe.
 * @param buffer       Output buffer for the formatted text.
 * @param buffer_size  Size of the output buffer in bytes.
 *
 * @return `true` on success, or `false` if an error occurred (errno set).
 */

bool iarena_stats(const iarena_t* arena, char* buffer, size_t buffer_size);
// -------------------------------------------------------------------------------- 

#if ARENA_USE_CONVENIENCE_MACROS

/**
 * @def iarena_alloc_type
 * @brief Allocate a single object of the specified type from an iarena_t.
 *
 * This macro is a convenience wrapper around `alloc_iarena()`. It allocates
 * `sizeof(type)` bytes from the sub-arena without zero-initializing the block.
 *
 * Equivalent to:
 *
 *      (type*)alloc_iarena(ia, sizeof(type), false)
 *
 * Notes:
 *  - The returned pointer is aligned according to the iarena's base alignment.
 *  - On failure, returns NULL and errno is set by `alloc_iarena()`.
 *
 * @param arena  A pointer to the `iarena_t` allocator.
 * @param type   The C type to allocate.
 *
 * @return A pointer to a `type` object, or NULL on failure.
 *
 * @code{.c}
 * iarena_t* ia = init_iarena_with_arena(parent, 4096, 64);
 * MyStruct* obj = iarena_alloc_type(ia, MyStruct);
 * @endcode
 */
    #define iarena_alloc_type(arena, type) \
        (type*)alloc_iarena((arena), sizeof(type), false)

/**
 * @def iarena_alloc_array
 * @brief Allocate an array of `count` objects of the specified type.
 *
 * This macro allocates `sizeof(type) * count` bytes from an iarena without
 * zeroing the memory.
 *
 * Equivalent to:
 *
 *      (type*)alloc_iarena(ia, sizeof(type) * count, false)
 *
 * Notes:
 *  - No overflow protection is performed by the macro; callers should ensure
 *    that `sizeof(type) * count` does not overflow `size_t`.
 *  - On failure, returns NULL and errno is set appropriately.
 *
 * @param arena  A pointer to the `iarena_t` allocator.
 * @param type   The C type of each array element.
 * @param count  Number of elements to allocate.
 *
 * @return A pointer to an array of `type`, or NULL on failure.
 *
 * @code{.c}
 * float* values = iarena_alloc_array(ia, float, 128);
 * @endcode
 */
    #define iarena_alloc_array(arena, type, count) \
        (type*)alloc_iarena((arena), sizeof(type) * (count), false)

/**
 * @def iarena_alloc_type_zeroed
 * @brief Allocate a single zero-initialized object of the specified type.
 *
 * This macro behaves like `iarena_alloc_type()` but ensures that the allocated
 * memory is filled with zero bytes prior to returning.
 *
 * Equivalent to:
 *
 *      (type*)alloc_iarena(ia, sizeof(type), true)
 *
 * Notes:
 *  - Useful for POD structs where initialization to zero is desired.
 *  - On failure, returns NULL.
 *
 * @param arena  A pointer to the `iarena_t` allocator.
 * @param type   The C type to allocate and initialize.
 *
 * @return A zero-initialized `type*`, or NULL on failure.
 *
 * @code{.c}
 * Vec3* v = iarena_alloc_type_zeroed(ia, Vec3);
 * // v->x == v->y == v->z == 0.0f
 * @endcode
 */
    #define iarena_alloc_type_zeroed(arena, type) \
        (type*)alloc_iarena((arena), sizeof(type), true)

/**
 * @def iarena_alloc_array_zeroed
 * @brief Allocate a zero-initialized array of `count` objects.
 *
 * This macro allocates `sizeof(type) * count` bytes and zeroes the entire
 * allocation.
 *
 * Equivalent to:
 *
 *      (type*)alloc_iarena(ia, sizeof(type) * count, true)
 *
 * Notes:
 *  - No overflow check is performed on the multiplication; callers should
 *    ensure the product fits in `size_t`.
 *  - On failure, returns NULL.
 *
 * @param arena  A pointer to the `iarena_t` allocator.
 * @param type   The element type.
 * @param count  The number of objects to allocate and zero.
 *
 * @return A zero-initialized array pointer, or NULL on failure.
 *
 * @code{.c}
 * int* counts = iarena_alloc_array_zeroed(ia, int, 64);
 * // counts[i] == 0 for all i
 * @endcode
 */
    #define iarena_alloc_array_zeroed(arena, type, count) \
        (type*)alloc_iarena((arena), sizeof(type) * (count), true)
#endif
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for immutable arena (iarena) allocation.
 *
 * Allocates a block of memory of size @p size from the immutable arena
 * referenced by @p ctx. If @p zeroed is true, the returned memory will be
 * zero-initialized according to the semantics of alloc_iarena().
 *
 * On error, errno is set (typically to EINVAL or ENOMEM) and NULL is returned.
 *
 * This function implements the @ref alloc_prototype interface for
 * iarena-backed allocators.
 *
 * @param ctx    Pointer to an iarena_t instance.
 * @param size   Number of bytes to allocate.
 * @param zeroed Whether the memory should be zero-initialized.
 *
 * @retval void* Pointer to a block of at least @p size bytes.
 * @retval NULL  On failure, with errno set.
 */
static inline void* iarena_v_alloc(void* ctx, size_t size, bool zeroed) {
    iarena_t* iarena = (iarena_t*)ctx;
    if (!iarena) {
        errno = EINVAL;
        return NULL;
    }
    return alloc_iarena(iarena, size, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for aligned immutable arena (iarena) allocation.
 *
 * Allocates a block of memory of size @p size from the immutable arena
 * referenced by @p ctx, with a minimum alignment of @p align. The alignment
 * must be a power of two. If @p zeroed is true, the returned memory will be
 * zero-initialized.
 *
 * On error, errno is set and NULL is returned.
 *
 * This function implements the @ref alloc_aligned_prototype interface
 * for iarena-backed allocators.
 *
 * @param ctx    Pointer to an iarena_t instance.
 * @param size   Number of bytes to allocate.
 * @param align  Required alignment (power of two).
 * @param zeroed Whether the memory should be zero-initialized.
 *
 * @retval void* Pointer to an aligned block of at least @p size bytes.
 * @retval NULL  On failure, with errno set.
 */
static inline void* iarena_v_alloc_aligned(void* ctx, size_t size, size_t align,
                                           bool zeroed) {
    iarena_t* iarena = (iarena_t*)ctx;
    if (!iarena) {
        errno = EINVAL;
        return NULL;
    }
    return alloc_iarena_aligned(iarena, size, align, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for immutable arena (iarena) reallocation.
 *
 * Resizes a block of memory previously allocated from the immutable arena
 * referenced by @p ctx. The block is identified by @p old_ptr and its
 * previous size @p old_size; the new requested size is @p new_size.
 *
 * The contents are preserved up to min(@p old_size, @p new_size). If
 * @p zeroed is true and the allocation grows, any newly added region must
 * be zero-initialized according to the semantics of realloc_iarena().
 *
 * On error, errno is set and NULL is returned. In that case, the caller
 * must continue to use @p old_ptr unchanged.
 *
 * This function implements the @ref realloc_prototype interface for
 * iarena-backed allocators.
 *
 * @param ctx       Pointer to an iarena_t instance.
 * @param old_ptr   Pointer to the existing allocation (may be NULL).
 * @param old_size  Size of the existing allocation.
 * @param new_size  Requested new size.
 * @param zeroed    Whether any expanded portion must be zero-initialized.
 *
 * @retval void* New pointer to the resized allocation on success.
 * @retval NULL  On failure, with errno set (caller keeps @p old_ptr).
 */
static inline void* iarena_v_realloc(void* ctx, void* old_ptr,
                                     size_t old_size, size_t new_size,
                                     bool zeroed) {
    iarena_t* iarena = (iarena_t*)ctx;
    if (!iarena) {
        errno = EINVAL;
        return NULL;
    }
    return realloc_iarena(iarena, old_ptr, old_size, new_size, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for aligned immutable arena (iarena) reallocation.
 *
 * Resizes a block of memory previously allocated from the immutable arena
 * referenced by @p ctx, enforcing a minimum alignment of @p align for the
 * resulting block. The previous allocation is described by @p old_ptr and
 * @p old_size; the new requested size is @p new_size.
 *
 * The contents are preserved up to min(@p old_size, @p new_size). If
 * @p zeroed is true and the allocation grows, any newly added bytes must
 * be zero-initialized according to the semantics of realloc_iarena_aligned().
 *
 * On error, errno is set and NULL is returned. In that case, the caller
 * must continue to use @p old_ptr unchanged.
 *
 * This function implements the @ref realloc_aligned_prototype interface for
 * iarena-backed allocators.
 *
 * @param ctx       Pointer to an iarena_t instance.
 * @param old_ptr   Pointer to the existing allocation.
 * @param old_size  Size of the existing allocation.
 * @param new_size  Requested new size.
 * @param zeroed    Whether any expanded portion must be zero-initialized.
 * @param align     Required alignment (power of two).
 *
 * @retval void* Pointer to the resized, aligned allocation on success.
 * @retval NULL  On failure, with errno set (caller keeps @p old_ptr).
 */
static inline void* iarena_v_realloc_aligned(void* ctx, void* old_ptr,
                                             size_t old_size, size_t new_size,
                                             bool zeroed, size_t align) {
    iarena_t* iarena = (iarena_t*)ctx;
    if (!iarena) {
        errno = EINVAL;
        return NULL;
    }
    return realloc_iarena_aligned(iarena, old_ptr, old_size,
                                  new_size, zeroed, align);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for returning an element to an immutable arena.
 *
 * Immutable arenas do not support returning individual blocks to the
 * allocator; memory is typically reclaimed only when the entire arena
 * is reset or destroyed. As a result, this function is effectively a
 * no-op for valid contexts.
 *
 * If @p ctx is NULL, errno is set to EINVAL and the function returns.
 * The @p ptr parameter is otherwise ignored.
 *
 * This function implements the @ref return_prototype interface for
 * iarena-backed allocators.
 *
 * @param ctx Pointer to an iarena_t instance.
 * @param ptr Pointer to an allocated block (ignored).
 */
static inline void iarena_v_return(void* ctx, void* ptr) {
    (void)ptr;  // arenas don’t support returning individual blocks
    iarena_t* iarena = (iarena_t*)ctx;
    if (!iarena) {
        errno = EINVAL;
        return;
    }
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Vtable adapter for freeing an immutable arena.
 *
 * Immutable arenas are often stack-allocated or externally managed,
 * and may not own their backing storage in a way that allows this
 * interface to release it. In this implementation, @p ctx is validated
 * and the function performs no further action.
 *
 * If @p ctx is NULL, errno is set to EINVAL and the function returns.
 *
 * This function implements the @ref free_prototype interface for
 * iarena-backed allocators, but is currently a no-op for valid contexts.
 *
 * @param ctx Pointer to an iarena_t instance.
 */
static inline void iarena_v_free(void* ctx) {
    iarena_t* iarena = (iarena_t*)ctx;
    if (!iarena) {
        errno = EINVAL;
        return;
    }
    // NO OP
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Constructs an allocator_vtable_t for a given immutable arena.
 *
 * This helper initializes an allocator_vtable_t that exposes an
 * iarena_t instance through the generic allocator interface. All
 * operations (allocate, reallocate, etc.) are forwarded to the
 * corresponding iarena-backed vtable adapter functions.
 *
 * The returned vtable is typically used to parameterize components that
 * depend on the generic allocator API, without requiring direct knowledge
 * of the underlying iarena_t implementation.
 *
 * @param a Pointer to an iarena_t instance to use as the allocator backend.
 *
 * @return A fully initialized allocator_vtable_t bound to @p a.
 */
static inline allocator_vtable_t iarena_allocator(iarena_t* a) {
    allocator_vtable_t v = {
        .allocate           = iarena_v_alloc,
        .allocate_aligned   = iarena_v_alloc_aligned,
        .reallocate         = iarena_v_realloc,
        .reallocate_aligned = iarena_v_realloc_aligned,
        .return_element     = iarena_v_return,
        .deallocate         = iarena_v_free,
        .ctx                = a
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
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* arena_H */
// ================================================================================
// ================================================================================
// eof
//
// TODO: Look at changes necessary for multi-core and multi-processing systems
// TODO: ADD free_list allocator 
// TODO: Add Buddy Allocator 
// TODO: Add Slab allocator 
// TODO: ADD two level segregated fit allocator 
