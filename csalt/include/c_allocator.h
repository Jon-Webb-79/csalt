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
#include <errno.h>

#include "c_error.h"
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
 * @return void_ptr_expect_t data type with a pointer to data or errors
 */
typedef void_ptr_expect_t (*alloc_prototype)(void* ctx, size_t size, bool zeroed);

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
 * @return void_ptr_expect_t data type with a pointer to data or errors
 */
typedef void_ptr_expect_t (*alloc_aligned_prototype)(void* ctx, size_t size, size_t align, bool zeroed);

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
 * @return void_ptr_expect_t data type with a pointer to data or errors
 */
typedef void_ptr_expect_t (*realloc_prototype)(void* ctx, void* old_ptr,
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
 * @return void_ptr_expect_t data type with a pointer to data or errors
 */
typedef void_ptr_expect_t (*realloc_aligned_prototype)(void* ctx, void* old_ptr,
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
// ARENA FUNCTIONS 

typedef struct {
    bool has_value;
    union {
        arena_t* value;
        ErrorCode error;
    } u;
} arena_expect_t;
// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a dynamically growing arena allocator.
 *
 * Allocates an arena that obtains its initial storage via @c malloc() and, if
 * permitted by @p resize, may grow by allocating new chunks on demand. The
 * arena header, first chunk header, and initial data region are all placed
 * contiguously in the first allocation. Additional chunks (if growth is
 * enabled) are allocated separately and linked into the arena.
 *
 * This function uses an "expected" style return type. On success, the returned
 * @c arena_expect_t has @c has_value set to @c true and @c u.value points to
 * a fully initialized @c arena_t. On failure, @c has_value is @c false and
 * @c u.error contains an appropriate ::ErrorCode value describing the cause.
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
 * @return An ::arena_expect_t result describing either success or failure.
 *
 * On success:
 *   - @c result.has_value == true
 *   - @c result.u.value   is a pointer to the newly created @c arena_t.
 *
 * On failure:
 *   - @c result.has_value == false
 *   - @c result.u.error   contains one of the following ::ErrorCode values:
 *
 * @retval INVALID_ARG
 *      If @p bytes is too small to contain the arena header and first chunk
 *      header, or if @p base_align_in or @p min_chunk_in has an invalid value
 *      that cannot be normalized (e.g., overflow while rounding up to a
 *      power-of-two).
 * @retval ALIGNMENT_ERROR
 *      If alignment normalization fails or the computed aligned data region
 *      cannot be placed within the allocated buffer.
 * @retval LENGTH_OVERFLOW
 *      If arithmetic overflow is detected during pointer or size computations
 *      for the header, chunk, or data regions.
 * @retval OUT_OF_MEMORY
 *      If the computed usable data region size is zero after layout, indicating
 *      that no space remains for allocations.
 * @retval BAD_ALLOC
 *      If the underlying @c malloc() call for the initial arena buffer fails.
 * @retval FEATURE_DISABLED
 *      If dynamic arenas are disabled at compile time
 *      (@c ARENA_ENABLE_DYNAMIC == 0).
 *
 * If your implementation maps ::ErrorCode values to @c errno (e.g., via
 * @c set_errno_from_error()), then @c errno will be set consistently with
 * @c result.u.error on failure.
 *
 * @note On success, the returned arena’s first chunk is fully initialized and
 *       ready for allocation via @c alloc_arena() or @c alloc_arena_aligned().
 *
 * @note The initial data region begins at an address aligned to
 *       @c max(base_align_in, alignof(max_align_t)) after normalization.
 *
 * @warning On success, the arena returned in @c result.u.value must be released
 *          with @c free_arena(). Individual allocations from the arena must not
 *          be freed with @c free().
 *
 * @pre  @c ARENA_ENABLE_DYNAMIC must be enabled (non-zero) at compile time.
 * @post On success (@c result.has_value == true), the arena @c a =
 *       @c result.u.value satisfies:
 *       - @c a->mem_type == DYNAMIC
 *       - @c a->resize == (resize ? 1 : 0)
 *       - @c a->head == a->tail
 *       - @c a->cur begins at the start of the aligned initial data region.
 *
 * @sa init_static_arena(), free_arena(), alloc_arena(), alloc_arena_aligned(),
 *     reset_arena(), arena_remaining()
 *
 * @par Example
 * @code{.c}
 * // Create a dynamic arena with 4 KiB initial storage,
 * // allow future growth, minimum chunk of 4 KiB, default alignment.
 * arena_expect_t res = init_dynamic_arena(4096,        // initial bytes
 *                                         true,        // resize enabled
 *                                         4096,        // minimum chunk size for growth
 *                                         alignof(max_align_t));
 *
 * if (!res.has_avlue) {
 *     // Initialization failed; inspect the error code
 *     ErrorCode ec = res.u.error;
 *     // handle error (log, abort, fall back, etc.)
 *     return;
 * }
 *
 * arena_t* a = res.u.value;
 *
 * void_ptr_expect_t expect = alloc_arena(a, 128, true) // allocate 128 zeroed bytes 
 * if (!expect.has_value) {
 *    // Handle error 
 * }
 * void *p = expect.u.value;
 *
 * // ... use arena allocations ...
 *
 * free_arena(a);  // releases all chunks and the arena header itself
 * @endcode
 */
arena_expect_t init_dynamic_arena(size_t bytes, bool resize, size_t min_chunk_in, size_t base_align_in);
// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a STATIC (non-growing) arena inside a caller-supplied buffer.
 *
 * This function constructs an arena *in place* within the memory range
 * ``[buffer, buffer + bytes)``. No heap allocations occur. The memory layout is:
 *
 *     [ arena_t header | padding | struct Chunk | padding | data... ]
 *
 * The data region for the head chunk is aligned to @p alignment_in (rounded up
 * to a power of two and never less than @c alignof(max_align_t)). Any alignment
 * padding reduces the usable capacity available for allocations.
 *
 * @param buffer        Caller-supplied backing store for the arena. Must remain
 *                      valid for the entire arena lifetime. The arena header and
 *                      the first chunk header are written directly into @p buffer.
 *
 * @param bytes         Total size in bytes of @p buffer. This must be large
 *                      enough to hold:
 *                      - one @c arena_t,
 *                      - one @c Chunk,
 *                      - at least one byte of data after alignment.
 *
 * @param alignment_in  Requested base alignment for the chunk's data region.
 *                      If zero, defaults to @c alignof(max_align_t).
 *                      If not a power of two, it is rounded up to the next
 *                      power of two. The final alignment is always at least
 *                      @c alignof(max_align_t).
 *
 * @return An ::arena_expect_t describing success or failure.
 *
 * On success:
 *   - @c result.has_value == true
 *   - @c result.u.value   is a pointer to the in-place @c arena_t
 *
 * On failure:
 *   - @c result.has_value == false
 *   - @c result.u.error   contains one of the following ::ErrorCode values:
 *
 * @retval NULL_POINTER
 *      If @p buffer is NULL.
 *
 * @retval INVALID_ARG
 *      If @p bytes is too small to contain @c arena_t + @c Chunk after
 *      alignment, or if the aligned Arena header would lie beyond the buffer.
 *
 * @retval ALIGNMENT_ERROR
 *      If @p alignment_in cannot be normalized to a valid power-of-two, or if
 *      the aligned data region cannot fit inside the provided buffer.
 *
 * @retval LENGTH_OVERFLOW
 *      If arithmetic overflow is detected while computing pointer offsets or
 *      buffer boundaries.
 *
 * @retval OUT_OF_MEMORY
 *      If no usable bytes remain for the data region after alignment.
 *
 * @note  The arena is created in STATIC mode:
 *        - @c mem_type = STATIC  
 *        - @c resize   = 0 (growth is not permitted)
 *        Attempts to grow via @c alloc_arena() beyond remaining space should
 *        return an appropriate ::ErrorCode (often @c OUT_OF_MEMORY or @c OPERATION_UNAVAILABLE).
 *
 * @note  @c tot_alloc is set to the full caller-supplied buffer footprint
 *        (i.e., ``bytes``). @c alloc is the usable data capacity after all
 *        required alignment and header placement.
 *
 * @warning Do **not** free @p buffer while the arena is in use.  
 *          The arena header lives *inside* @p buffer, so freeing @p buffer
 *          instantly invalidates the arena.  
 *          The public @c free_arena() implementation must not attempt to free
 *          the backing store in STATIC mode.
 *
 * @pre  @p buffer points to at least @p bytes of valid writable memory.
 * @pre  The lifetime of @p buffer strictly dominates the lifetime of the arena.
 *
 * @post On success:
 *       - @c a->head and @c a->tail reference the single in-place head chunk
 *       - @c a->cur begins at the start of the aligned data region
 *       - @c a->len == 0
 *       - @c a->alloc == usable aligned data bytes
 *       - @c a->tot_alloc == bytes
 *
 * @sa   alloc_arena(), alloc_arena_aligned(), reset_arena(),
 *       arena_remaining(), arena_stats()
 *
 * @par Example: Using a fixed-size stack buffer
 * @code{.c}
 * uint8_t buf[4096];
 *
 * arena_expect_t r = init_static_arena(buf, sizeof(buf), alignof(max_align_t));
 * if (!r.has_value) {
 *     ErrorCode ec = r.u.error;
 *     // handle error...
 * }
 *
 * arena_t *a = r.u.value;
 *
 * void_ptr_expect_t expect = alloc_arena(a, 128, false) // ok 
 * if (!expect.has_value) {
 *     // Handle error 
 * } 
 * void *p1 = except.u.value;
 * expect = alloc_arena(a, 9000, false);  
 * if (!expect.has_value) {
 *     // This will fail since capacity has been exceeded 
 *     printf(error_to_string(expect.error));
 * }
 *
 * reset_arena(a, false);   // discards allocations, capacity unchanged
 *
 * // No need to free buf; STATIC arenas do not own their backing store.
 * @endcode
 *
 * @par Example: Caller buffer with forced misalignment
 * @code{.c}
 * enum { RAW = 16384 };
 * uint8_t *raw = malloc(RAW);
 * uint8_t *unaligned = raw + 1;  // deliberately unaligned start
 *
 * arena_expect_t r = init_static_arena(unaligned, RAW - 1, 64);
 * if (!r.has_value) {
 *     // handle error...
 * }
 *
 * arena_t *a = r.u.value;
 * void_ptr_expect_t expect = alloc_arena(a, 256, true);
 * if (!expect.has_value) {
 *     // Handle error 
 * }
 * void *p = expect.u.value;
 *
 * // Guaranteed 64-byte alignment:
 * assert(((uintptr_t)p % 64u) == 0u);
 *
 * free(raw);  // frees original allocation, not the arena
 * @endcode
 */
arena_expect_t init_static_arena(void* buffer, size_t bytes, size_t passing_in);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a fixed-capacity sub-arena carved from a parent arena.
 *
 * This function allocates a contiguous memory region from @p parent using a
 * single call to @c alloc_arena(), and constructs an entire arena in place
 * within that region:
 *
 *     [ arena_t header | padding | Chunk | padding | usable data ]
 *
 * The resulting sub-arena:
 *   - **does not own its memory** (owned by the parent)
 *   - **cannot grow** (fixed capacity)
 *   - **inherits @c mem_type** from the parent arena
 *
 * @param[in,out] parent        Pointer to a valid parent arena. Must not be NULL.
 * @param[in]     bytes         Total number of bytes to carve from the parent,
 *                              including headers, padding, and data region.
 * @param[in]     alignment_in  Requested base alignment for the sub-arena's
 *                              data region (0 => @c alignof(max_align_t)).
 *                              Rounded up to the next power of two if needed.
 *
 * @return An ::arena_expect_t describing success or failure.
 *
 * On success:
 *   - @c result.has_value == true  
 *   - @c result.u.value   points to the newly constructed sub-arena
 *
 * On failure:
 *   - @c result.has_value == false  
 *   - @c result.u.error   contains one of the following ::ErrorCode values:
 *
 * @retval NULL_POINTER
 *      If @p parent is NULL.
 *
 * @retval INVALID_ARG
 *      If @p bytes is zero or too small to contain @c arena_t + @c Chunk +
 *      at least one data byte, or if alignment normalization fails.
 *
 * @retval LENGTH_OVERFLOW
 *      If pointer arithmetic overflows when computing internal layout.
 *
 * @retval ALIGNMENT_ERROR
 *      If the aligned data region cannot fit within the carved block.
 *
 * @retval OUT_OF_MEMORY
 *      If the parent arena cannot satisfy the @p bytes request.
 *
 * @note  The sub-arena has:
 *        - @c mem_type    = parent->mem_type  
 *        - @c resize      = 0 (cannot grow)  
 *        - @c owns_memory = 0 (parent owns the region)
 *
 * @note  @c free_arena() on a sub-arena performs only shallow cleanup
 *        (no memory is freed). The carved region is released only when the
 *        parent arena is reset or freed.
 *
 * @warning Resetting or freeing @p parent immediately invalidates all
 *          sub-arenas carved from it.
 *
 * @post On success:
 *       - The sub-arena's header and first chunk reside entirely within the
 *         carved parent region.
 *       - @c a->cur begins at the aligned start of the data region.
 *       - @c a->alloc == usable aligned data capacity.
 *       - @c a->tot_alloc == bytes.
 *
 * @sa alloc_arena(), reset_arena(), init_static_arena(), init_dynamic_arena()
 *
 * @par Example: Scoped sub-arena for temporary allocations
 * @code{.c}
 * arena_expect_t main_r = init_dynamic_arena(1024 * 1024, true, 4096, 0);
 * assert(main_r.has_value);
 *
 * arena_t *main = main_r.u.value;
 *
 * // Carve an 8 KiB sub-arena (headers included)
 * arena_expect_t sub_r = init_arena_with_arena(main, 8192, alignof(max_align_t));
 * assert(sub_r.has_value);
 *
 * arena_t *temp = sub_r.u.value;
 *
 * void_ptr_expect_t expect = alloc_arena(temp, 1024, false);
 * if (!expect.has_value) {
 *     // Hanlde error 
 * }
 * void *data = expect.u.value;
 *
 * // Sub-arena teardown: no memory is freed; parent still owns everything.
 * free_arena(temp);
 *
 * // Parent teardown releases the entire carved region.
 * free_arena(main);
 * @endcode
 */
arena_expect_t init_arena_with_arena(arena_t* parent,
                                     size_t bytes,
                                     size_t alignment_in);
// -------------------------------------------------------------------------------- 

/**
 * @brief Convenience initializer for a dynamic arena with common defaults.
 *
 * This helper constructs a dynamic (heap-backed, optionally growing) arena
 * using typical defaults by forwarding to ::init_dynamic_arena():
 *
 * @code{.c}
 *   init_dynamic_arena(bytes,
 *                      resize,
 *                      4096u,
 *                      alignof(max_align_t));
 * @endcode
 *
 * Use it when you want a dynamic arena with:
 *   - a reasonable minimum chunk size (4 KiB), and
 *   - a base alignment of at least @c alignof(max_align_t),
 * without manually specifying growth chunk size or base alignment.
 *
 * If you need custom growth tuning or a different base alignment, call
 * ::init_dynamic_arena() directly instead.
 *
 * @param bytes   Initial allocation footprint requested for the arena’s first
 *                region. The implementation may increase this internally to
 *                satisfy header placement and the minimum chunk rule.
 *
 * @param resize  If @c true, the arena may grow by allocating additional
 *                chunks as needed. If @c false, allocations that exceed the
 *                remaining capacity will fail with an appropriate ::ErrorCode.
 *
 * @return An ::arena_expect_t describing success or failure.
 *
 * On success:
 *   - @c result.has_value == true
 *   - @c result.u.value   points to the newly created @c arena_t.
 *
 * On failure:
 *   - @c result.has_value == false
 *   - @c result.u.error   is the ::ErrorCode propagated from
 *     ::init_dynamic_arena(), typically one of:
 *
 *   - @c INVALID_ARG       — invalid size or layout (e.g., too small for headers)
 *   - @c ALIGNMENT_ERROR   — base alignment normalization or placement failed
 *   - @c LENGTH_OVERFLOW   — arithmetic overflow in size/layout calculations
 *   - @c OUT_OF_MEMORY     — no usable data space after layout
 *   - @c BAD_ALLOC         — underlying @c malloc() for the initial buffer failed
 *   - @c FEATURE_DISABLED  — dynamic arenas disabled at compile time
 *
 * @note Minimum chunk size is fixed at 4096 bytes (4 KiB) in this wrapper.
 *       This is a conventional page-like default, not a guarantee of OS page
 *       size. For workloads that benefit from different growth steps, call
 *       ::init_dynamic_arena() directly.
 *
 * @note The arena’s base alignment is @c alignof(max_align_t) in this wrapper.
 *       Per-allocation alignment follows this base unless you use the aligned
 *       allocation variant.
 *
 * @warning Dynamic arenas allocate from the heap. Release resources via
 *          ::free_arena() when done. Individual allocations from the arena
 *          must not be freed with @c free().
 *
 * @sa init_dynamic_arena(), init_static_arena(), free_arena()
 *
 * @par Example
 * @code{.c}
 * arena_expect_t r = init_darena(4096u,  true);
 * if (!r.has_value) {
 *     ErrorCode ec = r.u.error;
 *     // handle error (log, abort, fallback, etc.)
 *     return;
 * }
 *
 * arena_t *a = r.u.value;
 *
 * void_ptr_expect_t expect = alloc_arena(a, 128u, true);
 * if (!expect.has_value) {
 *     // Handle error 
 * }
 * void* p = expect.u.value;
 *
 * // ... use arena allocations ...
 *
 * free_arena(a);
 * @endcode
 */
arena_expect_t init_darena(size_t bytes, bool resize);
// -------------------------------------------------------------------------------- 

/**
 * @brief Convenience initializer for a STATIC (non-growing) arena with default alignment.
 *
 * This helper constructs a STATIC (non-growing, in-place) arena inside a
 * caller-supplied buffer by forwarding to ::init_static_arena():
 *
 * @code{.c}
 *   init_static_arena(buffer,
 *                     bytes,
 *                     alignof(max_align_t));
 * @endcode
 *
 * Use it when you want an in-place arena built inside a caller-managed buffer
 * with the data region aligned to at least @c alignof(max_align_t) and do not
 * need a custom base alignment. For different alignment requirements, call
 * ::init_static_arena() directly.
 *
 * @param buffer  Caller-supplied backing store where the arena header, first
 *                chunk header, and data region will be constructed in place.
 *                Must remain valid and writable for the entire arena lifetime.
 *
 * @param bytes   Total size in bytes of @p buffer. Must be large enough to
 *                contain one @c arena_t, one @c Chunk, and at least one byte
 *                of usable data after alignment and padding.
 *
 * @return An ::arena_expect_t describing success or failure.
 *
 * On success:
 *   - @c result.has_value == true
 *   - @c result.u.value   points to the in-place @c arena_t inside @p buffer.
 *
 * On failure:
 *   - @c result.has_value == false
 *   - @c result.u.error   is the ::ErrorCode propagated from
 *     ::init_static_arena(), typically one of:
 *
 *   - @c NULL_POINTER     — @p buffer is @c NULL
 *   - @c INVALID_ARG      — @p bytes too small for header + chunk + data
 *   - @c ALIGNMENT_ERROR  — base alignment normalization or data placement failed
 *   - @c LENGTH_OVERFLOW  — arithmetic overflow during layout computation
 *   - @c OUT_OF_MEMORY    — no usable data region after layout
 *
 * @note The arena is created in STATIC mode (no growth):
 *       - @c mem_type    = STATIC
 *       - @c resize      = 0 (fixed capacity)
 *       Calls to ::alloc_arena() that exceed remaining capacity will fail and
 *       return an appropriate ::ErrorCode.
 *
 * @note Ownership: the arena does **not** own @p buffer. ::free_arena() for a
 *       STATIC arena must not attempt to free @p buffer. Typically, the caller
 *       uses stack or statically allocated storage for @p buffer, which is
 *       released when its scope or program lifetime ends.
 *
 * @sa init_static_arena(), alloc_arena(), reset_arena(), arena_remaining()
 *
 * @par Example: Caller-managed stack buffer
 * @code{.c}
 * enum { BUF = 16u * 1024u };
 * uint8_t buf[BUF];   // stack- or static-allocated backing store
 *
 * arena_expect_t r = init_sarena(buf, BUF);
 * if (!r.has_value) {
 *     ErrorCode ec = r.u.error;
 *     // handle error...
 *     return;
 * }
 *
 * arena_t *a = r.u.value;
 *
 * void_ptr_expect_t expect = alloc_arena(a, 1024u, false);
 * if (!expect.has_value) {
 *     // Hanlde error 
 * }
 * void *p = expect.u.value;
 *
 * // Reset discards allocations but keeps capacity.
 * reset_arena(a, false);
 *
 * // No explicit free for 'buf' is needed if it is stack or static storage.
 * // ::free_arena(a) should perform only logical cleanup in STATIC mode.
 * @endcode
 */
arena_expect_t init_sarena(void *buffer, size_t bytes);
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
 * arena_expect_t expect = init_darena(4096, true);
 * if (!expect.has_value) {
 *     // Handle error 
 * }
 * arena_t *a = expect.u.value; 
 * void_ptr_expect_t aexpect = alloc_arena(a, 1024, false);
 * if (!aexpect.has_value) {
 *     // Handle error 
 * }
 * void *p1 = aexpect.u.value;
 * // ... use p1 ...
 * free_arena(a);   // releases all chunks and 'a' itself
 * a = NULL;        // prevent accidental reuse
 * @endcode
 *
 * @par Example (static arena)
 * @code{.c}
 * enum { BUF = 8192 };
 * void *buf = malloc(alignof(max_align_t));
 * arena_expect_t expect = init_sarena(buf, BUF);
 * if (!expect.has_value) {
 *     // Handle error 
 * }
 * arena_t *a = expect.u.value;
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
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate a block from an arena with its base alignment, returning an
 *        explicit success-or-error result.
 *
 * Performs a bump allocation from the arena’s current tail chunk. The returned
 * block is aligned to the arena’s base alignment (see ::arena_t::alignment).
 *
 * This function does **not** signal errors through `errno`. Instead it returns
 * a ::void_ptr_expect_t structure containing either:
 *
 *   - `.has_value = true` and `.u.value = void*` on success, or
 *   - `.has_value = false` and `.u.error = ErrorCode` describing the failure.
 *
 * - If enough space exists in the current tail chunk, the block is carved out
 *   immediately (with any required leading padding).
 * - If not enough space exists:
 *     - **STATIC** arenas fail immediately.
 *     - **DYNAMIC** arenas grow by allocating a new chunk (if dynamic support is
 *       compiled in and growth is enabled).
 *     - If growth is not permitted or the new chunk cannot be allocated,
 *       the call fails.
 *
 * @param arena
 *     Pointer to an initialized ::arena_t. Must not be NULL.
 *
 * @param bytes
 *     Requested payload size in bytes. Must be > 0.
 *
 * @param zeroed
 *     If true, the returned payload is zero-initialized. Otherwise it is left
 *     uninitialized.
 *
 * @return
 *     A ::void_ptr_expect_t struct:
 *       - On success: `.has_value = true`, `.u.value = pointer`.
 *       - On failure: `.has_value = false`, `.u.error` containing:
 *
 * The function returns an error with one of the following codes:
 *
 * | Condition | Returned ErrorCode |
 * |----------|--------------------|
 * | `arena == NULL` | `NULL_POINTER` |
 * | `bytes == 0` | `INVALID_ARG` |
 * | `arena->alignment` is zero or not a power-of-two | `ALIGNMENT_ERROR` |
 * | `arena->tail == NULL` (invalid or corrupted arena state) | `ILLEGAL_STATE` |
 * | Padding + size overflows `size_t` | `LENGTH_OVERFLOW` |
 * | Insufficient space & arena is STATIC | `OPERATION_UNAVAILABLE` |
 * | Insufficient space & growth disabled (`resize == 0`) | `OPERATION_UNAVAILABLE` |
 * | Growth required but dynamic support compiled out | `UNSUPPORTED` |
 * | `_next_chunk_size()` reports an invalid/overflow size | `LENGTH_OVERFLOW` |
 * | `_chunk_new_ex()` fails to allocate a new chunk | `BAD_ALLOC` |
 *
 * - The arena charges both the payload and any leading padding needed to reach
 *   alignment. Internal usage counters (`tail->len`, `arena->len`) increase by:
 *
 *       pad + bytes
 *
 *   where `pad ∈ [0, alignment-1]`.
 *
 * - For the first allocation from a newly grown chunk, no leading padding is
 *   required because the chunk’s data base is already aligned.
 *
 * - The returned pointer must **not** be freed with `free()`. Memory is released
 *   only when the arena is reset with ::reset_arena() or destroyed via
 *   ::free_arena().
 *
 * @pre
 *     - `arena` must be initialized.
 *     - `arena->alignment` must be a non-zero power-of-two.
 *
 * @post On success:
 *     - The returned pointer is aligned to `arena->alignment`.
 *     - `arena->cur` advances by `pad + bytes`.
 *     - Usage counters (`tail->len`, `arena->len`) reflect the allocation.
 *
 * @sa
 *     init_static_arena(), init_dynamic_arena(), alloc_arena_aligned(),
 *     reset_arena(), free_arena()
 *
 * @par Example: STATIC arena (no growth)
 * @code{.c}
 * uint8_t buf[8192];
 *
 * arena_expect_t ar = init_static_arena(buf, sizeof(buf), alignof(max_align_t));
 * assert(ar.has_value);
 *
 * arena_t *a = ar.u.value;
 *
 * void_ptr_expect_t r1 = alloc_arena(a, 256, true);
 * assert(r1.has_value);
 * void *p1 = r1.u.value;
 *
 * // Oversized request -> error (OPERATION_UNAVAILABLE)
 * void_ptr_expect_t r2 = alloc_arena(a, 9000, false);
 * if (!r2.has_value) {
 *     printf("alloc failed: %d\n", r2.u.error);
 * }
 * @endcode
 *
 * @par Example: DYNAMIC arena (growth allowed)
 * @code{.c}
 * arena_expect_t ar = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
 * assert(ar.has_value);
 *
 * arena_t *a = ar.u.value;
 *
 * // Fill current chunk
 * (void)alloc_arena(a, 4095, false);
 *
 * // This allocation will grow a new chunk
 * void_ptr_expect_t r = alloc_arena(a, 2, false);
 * if (!r.has_value) {
 *     printf("alloc failed: %d\n", r.u.error);
 * }
 *
 * free_arena(a);
 * @endcode
 */
void_ptr_expect_t alloc_arena(arena_t* arena, size_t bytes, bool zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reallocate an object within an arena, returning a ::void_ptr_expect_t.
 *
 * Arenas cannot grow an allocation in place. This function provides a
 * `realloc`-like interface that implements reallocation as:
 *
 *   1. If @p realloc_size <= @p var_size:
 *          → No-op. Return the original pointer in a success expect.
 *
 *   2. Otherwise:
 *          - Allocate a new block of @p realloc_size bytes using ::alloc_arena().
 *          - Copy the first @p var_size bytes from @p variable into the new block.
 *          - If @p zeroed is true, zero-fill bytes [@p var_size, @p realloc_size).
 *          - Return the new pointer in a success expect.
 *
 * The original memory is *not* freed and remains part of the arena until a reset.
 * This function never reclaims memory on failure.
 *
 * @param arena
 *     Pointer to an initialized ::arena_t instance. Must not be NULL.
 *
 * @param variable
 *     Pointer to an existing block previously allocated from @p arena.
 *     Must not be NULL. Ownership is not validated; passing a foreign pointer
 *     causes undefined behavior.
 *
 * @param var_size
 *     Size in bytes of the existing object. Must be exactly the size originally
 *     allocated; incorrect sizes result in undefined behavior (e.g., buffer
 *     overrun during memcpy).
 *
 * @param realloc_size
 *     Requested new size. If <= @p var_size, no new allocation occurs.
 *     If > @p var_size, a new block is allocated and old contents copied.
 *
 * @param zeroed
 *     If true and a new block is allocated, only the tail region
 *     (bytes [@p var_size, @p realloc_size)) is zero-filled. The leading
 *     @p var_size bytes are preserved.
 *
 * @return ::void_ptr_expect_t
 *     - On success:
 *         - expect.has_value = true
 *         - expect.u.value  = original pointer (no-op case)
 *           OR the newly allocated pointer (growth case)
 *
 *     - On failure:
 *         - expect.has_value = false
 *         - expect.u.error   = one of the ErrorCode values described below
 *
 * @par Failure conditions and ErrorCode meanings
 * A failure expect (has_value = false) may contain any of the following:
 *
 * **1. INVALID_ARG**
 *      - @p arena is NULL  
 *      - @p variable is NULL  
 *
 * **2. LENGTH_OVERFLOW**
 *      - Internal size arithmetic (e.g., var_size + (realloc_size - var_size))
 *        overflows size_t
 *
 * **3. BAD_ALLOC** (propagated from ::alloc_arena)
 *      - Arena lacks remaining capacity AND:
 *          * arena is STATIC, or
 *          * arena->resize == 0 (growth disabled), or
 *          * dynamic support is compiled out
 *      - Or dynamic growth attempted but chunk allocation failed
 *
 * **4. ALIGNMENT_ERROR** (propagated from ::alloc_arena)
 *      - Arena’s base alignment invalid or corrupted
 *
 * **5. ILLEGAL_STATE** (propagated from ::alloc_arena)
 *      - Arena internal pointers (e.g., tail) are invalid
 *
 * @note
 *     - This function never frees memory; old blocks accumulate until the arena
 *       is reset via ::reset_arena().
 *     - Alignment of the new block is determined by ::alloc_arena() rules.
 *     - Passing a pointer not allocated by the arena results in undefined
 *       behavior (this function cannot detect foreign pointers).
 *     - Not thread-safe unless externally synchronized.
 *
 * @code{.c}
 * // Example: growing an integer array inside an arena
 * arena_expect_t r = init_dynamic_arena(4096u, true, 4096u, alignof(max_align_t));
 * if (!r.has_value) {
 *     fprintf(stderr, "Arena init failed: %d\n", r.u.error);
 *     return;
 * }
 *
 * arena_t *arena = r.u.value;
 *
 * size_t n = 4u;
 * void_ptr_expect_t e0 = alloc_arena(arena, n * sizeof(int), true);
 * if (!e0.has_value) {
 *     fprintf(stderr, "Initial alloc failed: %d\n", e0.u.error);
 *     return;
 * }
 *
 * int *arr = e0.u.value;
 * for (size_t i = 0; i < n; ++i) arr[i] = (int)i;
 *
 * // Grow from 4 to 8 ints
 * size_t new_n = 8u;
 * void_ptr_expect_t er = realloc_arena(arena,
 *                                      arr,
 *                                      n * sizeof(int),
 *                                      new_n * sizeof(int),
 *                                      true); // zero-fill new region
 *
 * if (!er.has_value) {
 *     fprintf(stderr, "Realloc failed: %d\n", er.u.error);
 *     return;
 * }
 *
 * int *new_arr = er.u.value;
 * // new_arr now holds: [0, 1, 2, 3, 0, 0, 0, 0]
 *
 * reset_arena(arena);
 * free_arena(arena);
 * @endcode
 *
 * @see alloc_arena
 * @see reset_arena
 */
void_ptr_expect_t realloc_arena(arena_t* arena, void* variable, size_t var_size, 
                                size_t realloc_size, bool zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate a block from an arena with a caller-specified alignment,
 *        returning a ::void_ptr_expect_t for structured error reporting.
 *
 * Performs a bump allocation from the arena’s current tail chunk. The returned
 * pointer is aligned to the *effective alignment*, defined as:
 *
 *      max(requested_alignment, arena->alignment)
 *
 * If @p alignment is zero, the arena’s base alignment is used. Any non-zero
 * alignment must be a power-of-two.
 *
 * If there is insufficient space in the current tail chunk:
 *   - In **STATIC** arenas or when growth is disabled, the call fails.
 *   - In **DYNAMIC** arenas with growth enabled, a new chunk is allocated whose
 *     data region is naturally aligned to the effective alignment, and the
 *     block is carved from that fresh chunk with no leading pad.
 *
 * Unlike earlier versions, this function no longer returns NULL. Instead it
 * returns a ::void_ptr_expect_t:
 *
 *      - expect.has_value = true   → allocation succeeded
 *      - expect.has_value = false  → allocation failed; expect.u.error
 *                                    contains an ::ErrorCode describing why
 *
 * @param arena      Arena to allocate from (must not be NULL).
 * @param bytes      Requested payload size in bytes (must be > 0).
 * @param alignment  Desired alignment (0 → use arena default). Must be a
 *                   non-zero power-of-two if non-zero.
 * @param zeroed     If true, the returned memory is zero-initialized.
 *
 * @return ::void_ptr_expect_t
 *      - On success:    expect.has_value = true,  expect.u.value = pointer
 *      - On failure:    expect.has_value = false, expect.u.error = ErrorCode
 *
 * @par Failure conditions and ErrorCode values
 * An expect with has_value = false may contain any of the following errors:
 *
 * **1. INVALID_ARG**
 *      - @p arena is NULL  
 *      - @p bytes == 0  
 *      - @p alignment is non-zero and not a power-of-two  
 *
 * **2. ALIGNMENT_ERROR**
 *      - Arena base alignment is zero  
 *      - Arena alignment is not a power-of-two  
 *
 * **3. ILLEGAL_STATE**
 *      - @c arena->tail is NULL (corrupted or uninitialized arena)
 *
 * **4. OPERATION_UNAVAILABLE**
 *      - Insufficient space in the current tail AND:
 *          * arena is STATIC, or  
 *          * arena->resize == 0 (growth disabled), or  
 *          * dynamic growth support was compiled out  
 *
 * **5. BAD_ALLOC**
 *      - Dynamic growth is allowed but allocating a new chunk fails
 *
 * **6. LENGTH_OVERFLOW**
 *      - Internal size arithmetic (pad + bytes) overflows size_t
 *
 * @note On success, the arena charges both the payload and the required padding.
 *       That is, @c tail->len and @c arena->len increase by (pad + bytes).
 *
 * @note On first allocation in a freshly grown chunk, no leading pad is added
 *       because the chunk’s base is already aligned.
 *
 * @warning The returned pointer must not be freed with @c free(). Memory is
 *          reclaimed only through ::reset_arena() or ::free_arena().
 *
 * @pre  @p arena is a valid, initialized arena.
 * @pre  @p alignment is zero or a non-zero power-of-two.
 * @post On success:
 *       - The returned pointer is aligned to max(alignment, arena->alignment).
 *       - @c arena->cur advances by @p bytes (plus pad if allocated from tail).
 *       - Chunk and arena usage counters are updated accordingly.
 *
 * @sa   alloc_arena(), init_static_arena(), init_dynamic_arena(),
 *       reset_arena(), free_arena()
 *
 * @par Example: Requesting a stricter alignment than the arena’s default
 * @code{.c}
 * arena_expect_t r = init_dynamic_arena(4096u, true, 4096u, alignof(max_align_t));
 * if (!r.has_value) {
 *     fprintf(stderr, "arena init failed: %d\n", r.u.error);
 *     return;
 * }
 * arena_t *a = r.u.value;
 *
 * void_ptr_expect_t e = alloc_arena_aligned(a, 128, 64, false);
 * if (!e.has_value) {
 *     fprintf(stderr, "alloc failed: %d\n", e.u.error);
 *     return;
 * }
 *
 * void *p = e.u.value;
 * assert(((uintptr_t)p % 64) == 0);
 *
 * free_arena(a);
 * @endcode
 *
 * @par Example: Falling back to arena base alignment
 * @code{.c}
 * arena_expect_t r = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
 * if (!r.has_value) {
 *     fprintf(stderr, "arena init failed: %d\n", r.u.error);
 *     return;
 * }
 *
 * arena_t *a = r.u.value;
 *
 * void_ptr_expect_t e2 = alloc_arena_aligned(a, 64, 0, true);
 * if (!e2.has_value) {
 *     fprintf(stderr, "alloc failed: %d\n", e2.u.error);
 *     return;
 * }
 *
 * void *p = e2.u.value;
 * assert(((uintptr_t)p % alignof(max_align_t)) == 0);
 *
 * free_arena(a);
 * @endcode
 */

void_ptr_expect_t alloc_arena_aligned(arena_t* arena, size_t bytes, size_t alignment, bool zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Reallocate an object within an arena using a specified alignment.
 *
 * This function provides a `realloc`-like interface adapted to arena
 * semantics. Because arenas cannot grow an allocation in place, reallocation
 * is performed using the following steps:
 *
 *   1. Allocate a new block of @p realloc_size bytes with the requested
 *      alignment @p aligned using ::alloc_arena_aligned().
 *   2. Copy the first @p var_size bytes from @p variable into the new block.
 *   3. If @p zeroed is true, zero-fill the newly added region
 *      (bytes [var_size, realloc_size)).
 *   4. Return the new block.
 *
 * The original memory remains owned by the arena until the arena is reset.
 * It is never individually freed.
 *
 * If @p realloc_size is less than or equal to @p var_size, the function
 * performs a no-op and returns @p variable unchanged. This avoids unnecessary
 * arena consumption and is consistent with arena-based allocation semantics.
 *
 * @param arena
 *     Pointer to a valid ::arena_t. Must not be NULL.
 *
 * @param variable
 *     Pointer to an object previously allocated from @p arena. Must not be
 *     NULL. Ownership is not validated; passing foreign pointers results in
 *     undefined behavior.
 *
 * @param var_size
 *     Size in bytes of the existing object. Must match the size originally
 *     allocated for @p variable. Incorrect sizes lead to undefined behavior.
 *
 * @param realloc_size
 *     New size in bytes. If greater than @p var_size, a new allocation is
 *     made; otherwise, the original pointer is returned unchanged.
 *
 * @param zeroed
 *     If true and a new block is allocated, only the newly added tail region is
 *     zero-filled. The first @p var_size bytes preserve their original values.
 *
 * @param aligned
 *     Required alignment for the new block. Must be zero (use arena default) or
 *     a non-zero power-of-two supported by ::alloc_arena_aligned(). The returned
 *     pointer will satisfy:
 *
 *         (uintptr_t)ptr % aligned == 0
 *
 * @return
 *     - @p variable itself if @p realloc_size <= @p var_size.
 *     - A newly allocated, properly aligned pointer if growth succeeds.
 *     - @c NULL if allocation of the new block fails.
 *
 * @par Reasons this function may return NULL
 * A NULL return indicates that the attempt to allocate a new, larger block
 * failed. Common causes include:
 *
 *   1. **Invalid inputs**
 *        - @p arena is NULL  
 *        - @p variable is NULL  
 *
 *   2. **Invalid alignment** (checked by ::alloc_arena_aligned)
 *        - @p aligned is non-zero and not a power-of-two  
 *        - The arena’s base alignment is invalid  
 *
 *   3. **Arena internal state invalid**
 *        - @c arena->tail is NULL  
 *        - Corrupted or uninitialized arena state  
 *
 *   4. **Insufficient space and no ability to grow**
 *        - STATIC arena  
 *        - Or DYNAMIC arena with growth disabled (@c arena->resize == 0)  
 *        - And the requested block does not fit in the current chunk  
 *
 *   5. **Dynamic growth attempted but chunk allocation failed**
 *        - Growth is allowed but ::alloc_arena_aligned() cannot allocate a
 *          sufficiently large chunk (e.g., underlying allocation failure)
 *
 * In all cases, *only* the NULL return value signals failure; no diagnostic
 * information is stored or returned.
 *
 * @note
 *     - The function never frees memory. Old blocks accumulate until the arena
 *       is reset.
 *     - Alignment of the new block may differ from the original.
 *     - Passing a pointer not allocated from @p arena is undefined behavior.
 *     - Not thread-safe unless externally synchronized.
 *
 * @code{.c}
 * // Example: aligned growth of a struct inside an arena
 * typedef struct {
 *     float x, y, z;
 * } vec3;
 *
 * arena_expect_t r = init_dynamic_arena(4096u, true, 4096u, alignof(max_align_t));
 * if (!r.has_value) {
 *     // handle arena initialization error via r.u.error
 *     return;
 * }
 *
 * arena_t *arena = r.u.value;*
 *
 * // Allocate with 16-byte alignment
 * vec3* v = alloc_arena_aligned(arena, sizeof(vec3), 16, true);
 *
 * size_t old_size = sizeof(vec3);
 * size_t new_size = sizeof(vec3) + 32;
 *
 * // Grow the block with stricter alignment and zero tail
 * vec3* v2 = realloc_arena_aligned(arena, v,
 *                                  old_size,
 *                                  new_size,
 *                                  true,   // zero tail
 *                                  32);    // stricter alignment
 *
 * // v2 is 32-byte aligned and contains a zeroed extension region.
 * // v still exists in the arena and is not freed.
 *
 * reset_arena(arena);   // Reclaims all arena allocations at once
 * free_arena(arena);
 * @endcode
 *
 * @see alloc_arena_aligned
 * @see alloc_arena
 * @see reset_arena
 */
void_ptr_expect_t realloc_arena_aligned(arena_t* arena, void* variable, size_t var_size, size_t realloc_size,
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
 * @brief Reset an arena to an empty state, optionally trimming dynamic chunks.
 *
 * This function rewinds an arena to a fresh “empty” state. All chunk usage
 * counters are cleared, @c arena->len becomes 0, and the cursor @c arena->cur
 * is repositioned appropriately.
 *
 * Two reset modes are supported:
 *
 * 1. Non-trimming reset (`trim_extra_chunks == false`)
 * - All chunks remain allocated.
 * - Only usage counters are cleared.
 * - Total capacity (`arena->alloc`) and total allocation footprint
 *   (`arena->tot_alloc`) are preserved.
 * - The cursor is moved to the start of the current tail chunk
 *   (or the head if no tail exists).
 *
 * 2. Trimming reset (`trim_extra_chunks == true`)
 * Applies **only to DYNAMIC arenas**:
 * - Frees all growth chunks after the head.  
 * - Resets the chunk list to a single head chunk.  
 * - The cursor is reset to the head’s data region.  
 * - `arena->alloc` becomes exactly the head chunk’s usable size.  
 * - `arena->tot_alloc` is reduced to reflect the removal of those extra chunks.
 *
 * STATIC arenas always behave as if `trim_extra_chunks == false`; they never
 * free additional chunks because their storage is caller-owned.
 *
 * @param arena
 *     Pointer to an initialized ::arena_t. Must not be NULL.
 *
 * @param trim_extra_chunks
 *     If true and the arena is dynamic, all chunks after the head are freed.
 *     If false, all chunks are retained and only usage is cleared.
 *
 * @return
 *     - `true` on success.  
 *     - `false` if @p arena is NULL.
 *
 * @par Reasons this function may return false
 * - @p arena is NULL.
 *
 * No other failure conditions exist. A NULL arena is the only error state.
 *
 * @note
 * - STATIC arenas never trim: their chunk chain is caller-supplied storage.
 * - When trimming dynamic arenas, each chunk’s footprint was originally added as:
 *
 *       align_up(sizeof(Chunk), arena->alignment) + chunk->alloc
 *
 *   This same quantity is subtracted when the chunk is removed.
 *
 * @note
 *   This function is safe to call on arenas created via ::init_dynamic_arena()
 *   or ::init_static_arena(). When using ::init_dynamic_arena(), remember that
 *   the initializer returns an `arena_expect_t`. Always check `.has_value`
 *   before using the returned arena.
 *
 * @warning
 *   Not thread-safe unless externally synchronized.
 *
 * @par Complexity
 *   O(N) over the number of chunks.  
 *   When trimming, freeing the extra chunks is also O(N_tail).
 *
 * @par Examples
 * @code{.c}
 * // Dynamic arena created via arena_expect_t:
 * arena_expect_t exp = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
 * if (!exp.has_value) { / handle error }
 * arena_t *a = exp.u.value;
 *
 * // Non-trimming reset preserves all allocated chunks:
 * (void)alloc_arena(a, 1024, false);
 * reset_arena(a, false);   // usage cleared, capacity preserved
 *
 * // Force the arena to grow:
 * size_t rem = arena_remaining(a);
 * (void)alloc_arena(a, rem, false);    // exhaust head
 * (void)alloc_arena(a, 64,  false);    // causes a new tail chunk
 *
 * // Trimming reset collapses back to a single head chunk:
 * reset_arena(a, true);
 *
 * free_arena(a);  // dynamic arena teardown
 * @endcode
 */
bool reset_arena(arena_t *arena, bool trim_extra_chunks);
// -------------------------------------------------------------------------------- 

/**
 * @brief Capture a lightweight checkpoint of the arena’s current position.
 *
 * Creates an opaque ::ArenaCheckPoint value that encodes the arena’s current
 * cursor position, the active tail chunk, and the arena’s global used-byte
 * count at the moment of capture. No memory is copied; only structural
 * addresses and counters are saved.
 *
 * The returned checkpoint may later be passed to ::restore_arena() to rewind
 * the arena to this exact allocation point. Checkpoints provide a fast,
 * allocation-free mechanism for implementing temporary “scoped” arena usage.
 *
 * If @p arena is NULL, this function returns a *zeroed* ::ArenaCheckPoint.  
 * This value is treated as a **no-op checkpoint** by ::restore_arena() and
 * indicates that no valid state was captured.
 *
 * Apart from a NULL arena, this function has **no other failure modes**.
 *
 * Internally (in a private representation), the checkpoint stores:
 * - the pointer to the current tail chunk at the time of capture,
 * - the cursor position within that chunk, and
 * - the arena’s total used length (`arena->len`) for diagnostics.
 *
 * A checkpoint is valid **only as long as** the tail chunk it references
 * remains part of the arena’s chunk list. Operations that remove or free the
 * recorded chunk invalidate the checkpoint. In particular:
 *
 * - A **trimming reset** (see ::reset_arena with `trim_extra_chunks == true`)
 *   may free tail chunks, invalidating older checkpoints.
 *
 * - A **dynamic growth** does *not* invalidate a checkpoint—the tail grows,
 *   but the checkpoint remains valid unless that tail is later removed.
 *
 * - STATIC arenas never remove chunks, so checkpoints remain valid until an
 *   arena-wide reset that overwrites usage information.
 *
 * @param arena
 *     Pointer to an initialized ::arena_t. If NULL, a zeroed checkpoint is
 *     returned.
 *
 * @return
 *     - A nonzero ::ArenaCheckPoint that can be safely passed to
 *       ::restore_arena() for rewinding.
 *     - A zeroed ::ArenaCheckPoint if @p arena is NULL.
 *
 * @note
 *     Checkpoints do not “pin” or protect memory. They merely store pointers
 *     and counters. Removing or freeing the underlying chunk chain invalidates
 *     them.
 *
 * @warning
 *     Not thread-safe unless externally synchronized. Concurrent mutation of
 *     the arena while saving or restoring invalidates checkpoints.
 *
 * @sa restore_arena(), reset_arena(), alloc_arena(), alloc_arena_aligned()
 */
ArenaCheckPoint save_arena(const arena_t* arena);
// -------------------------------------------------------------------------------- 

/**
 * @brief Rewind an arena to a previously saved checkpoint.
 *
 * Restores the arena’s allocation state to the point captured by
 * ::save_arena(). The function rewinds the cursor, tail chunk, and internal
 * usage counters, optionally freeing dynamic growth chunks depending on the
 * arena type.
 *
 * If @p cp represents an “empty” checkpoint (as returned when ::save_arena()
 * is called with a NULL arena), this function performs a **no-op** and returns
 * @c true.
 *
 * A checkpoint is valid only if:
 * - the referenced chunk still exists in the arena’s chunk chain,
 * - the saved cursor lies within that chunk’s data region,
 * - the arena has a valid power-of-two alignment,
 * - the checkpoint was produced from this arena (caller responsibility).
 *
 * If any of these validations fail, the function returns @c false and the arena
 * remains unchanged.
 *
 * **Dynamic arenas**
 * - All chunks *after* the checkpoint’s chunk are freed (each corresponds to a
 *   single-growth allocation).
 * - The chunk list is truncated at the checkpoint chunk, which becomes the new
 *   tail.
 * - Usage counters (`len`, `alloc`) and footprint counters (`tot_alloc`) are
 *   recomputed or adjusted based on the surviving chain.
 *
 * **Static arenas**
 * - No chunks are freed; storage is caller-owned.
 * - The cursor and tail chunk are rewound to the checkpoint.
 * - Usage/footprint accounting is recomputed.
 *
 * The function returns @c false if any of the following occur:
 *
 * - @p arena is @c NULL.
 * - The checkpoint’s chunk does not appear in the arena’s current chunk chain
 *   (e.g., checkpoint is stale due to trimming or external modifications).
 * - The checkpoint’s saved cursor lies outside the referenced chunk’s data
 *   bounds.
 * - The arena’s base alignment is zero or not a power-of-two.
 * - Internal invariants fail (e.g., chunk missing its data pointer).
 *
 * In all failure cases, the arena is left unchanged.
 *
 * - The checkpoint is empty (treated as a no-op).
 * - The checkpoint is valid and the arena is rewound successfully.
 *
 * @param arena
 *     Pointer to a valid ::arena_t instance. Must not be NULL.
 *
 * @param cp
 *     A checkpoint previously returned by ::save_arena().
 *
 * @return
 *     - @c true  if the checkpoint is valid and the arena is successfully
 *                rewound (or the checkpoint is empty).
 *     - @c false if validation fails or the operation cannot be performed.
 *
 * @note
 *     After a successful restore, any pointers obtained from the arena
 *     **after** the saved checkpoint become invalid:
 *     - In dynamic arenas, their chunks may have been freed.
 *     - In static arenas, their memory is still present but considered
 *       “unused” again.
 *
 * @warning
 *     Checkpoints do not pin memory. If the arena shrinks, trims, or mutates
 *     after saving a checkpoint, the checkpoint may become invalid.
 *
 * @warning
 *     Not thread-safe. External synchronization is required if other threads
 *     could interact with the arena concurrently.
 *
 * @sa save_arena(), reset_arena(), alloc_arena(), alloc_arena_aligned(),
 *     free_arena()
 */
bool restore_arena(arena_t *arena, ArenaCheckPoint cp);
// ================================================================================ 
// ================================================================================ 
// GETTER FUNCTIONS 

/**
 * @brief Return the number of *immediately usable* bytes remaining in the arena’s
 *        current tail chunk.
 *
 * Computes the free space available in the tail chunk only, using:
 *
 *      remaining = tail->alloc - min(tail->len, tail->alloc)
 *
 * This reflects how many bytes can be allocated *without* triggering arena
 * growth (in dynamic arenas) and without traversing earlier chunks.
 *
 * If @p arena is NULL, has no tail chunk, or the tail chunk lacks a valid data
 * region, the function returns 0.
 *
 * @param arena
 *     Pointer to an initialized ::arena_t, or NULL.
 *
 * @return
 *     The number of bytes available in the current tail chunk.
 *     Returns 0 on invalid input or if no space remains. Returns 
 *     SIZE_MAX if a NULL pointer is passed to the function
 *
 * @note
 *     This is **not** the total free space across all chunks. It reports only
 *     what the current tail can satisfy directly.
 *
 * @note
 *     The reported value does **not** include any per-allocation alignment
 *     padding that may be required by ::alloc_arena() or
 *     ::alloc_arena_aligned().  
 *     An allocation of @c n bytes succeeds from the tail iff:
 *
 *         arena_remaining(a) >= pad + n
 *
 *     where @c pad is the alignment padding required to bring the cursor up to
 *     the arena’s base or requested alignment.
 *
 * @note
 *     For dynamic arenas, if the remaining space is insufficient, a subsequent
 *     allocation *may* still succeed by causing the arena to grow a new chunk.
 *
 * @par Example
 * @code{.c}
 * size_t rem = arena_remaining(a);
 *
 * if (rem == SIZE_MAX) {
 *    // ascertain if result is real or a NULL pointer was passed to function
 * }
 *
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
 * @return The number of chunks in the arena; SIZE_MAX if @p arena is NULL.
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
 * @return Total used bytes (including pad) across the arena; SIZE_MAX if @p arena is NULL.
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
 * @return Total usable capacity; SIZE_MAX if @p arena is NULL.
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
 *         padding as accounted by the implementation); SIZE_MAX if @p arena is NULL.
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
 *         Returns SIZE_MAX arena is NULL or uninitialized.
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
 *         Returns SIZE_MAX if @p arena is NULL or uninitialized.
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
 *       structure, but the caller still owns the buffer itself.
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

/**
 * @brief Vtable adapter for arena allocation.
 *
 * Allocates a block of memory of size @p size from the arena referenced
 * by @p ctx. If @p zeroed is true, the returned memory will be
 * zero-initialized according to the semantics of alloc_arena().
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
static inline void_ptr_expect_t arena_v_alloc(void* ctx, size_t size, bool zeroed) {
    arena_t* arena = (arena_t*)ctx;
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
static inline void_ptr_expect_t arena_v_alloc_aligned(void* ctx, size_t size,
                                          size_t align, bool zeroed) {
    arena_t* arena = (arena_t*)ctx;
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
static inline void_ptr_expect_t arena_v_realloc(void* ctx, void* old_ptr,
                                    size_t old_size, size_t new_size,
                                    bool zeroed) {
    arena_t* arena = (arena_t*)ctx;
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
static inline void_ptr_expect_t arena_v_realloc_aligned(void* ctx, void* old_ptr,
                                            size_t old_size, size_t new_size,
                                            bool zeroed, size_t align) {
    arena_t* arena = (arena_t*)ctx;
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
// -------------------------------------------------------------------------------- 

typedef struct {
    bool has_value;
    union {
        pool_t* value;
        ErrorCode error;
    } u;
} pool_expect_t;
// ================================================================================ 
// ================================================================================ 
// INITIALIZE AND DEALLOCATE FUNCTIONS

/**
 * @brief Initialize a fixed-size memory pool backed by an existing arena.
 *
 * Creates a pool allocator that dispenses fixed-size blocks from @p arena.
 * The pool acquires memory in “chunks” (slices) carved from the arena,
 * optionally maintains an intrusive free list for recycled blocks, and may
 * operate in either fixed-capacity or grow-on-demand mode.
 *
 * The pool header itself is allocated from @p arena using
 * ::alloc_arena_aligned(), so no external @c malloc is performed.
 *
 * This function uses the ::pool_expect_t result type:
 * - @c result.has_value == true  → success; the pool pointer is available in
 *   @c result.u.value
 * - @c result.has_value == false → failure; the reason is encoded in
 *   @c result.u.error (an ::ErrorCode)
 *
 * @param arena
 *     Existing arena to supply memory. Must not be NULL.
 *
 * @param block_size
 *     User payload size (in bytes) per block. Must be > 0.
 *
 * @param alignment
 *     Desired alignment for each block (0 → use arena’s default alignment).
 *     If non-zero, it must be a power-of-two.  
 *     The effective alignment is:
 *
 *         max(alignment, alignof(void*))
 *
 *     ensuring each freed block can store a next-pointer for the free list.
 *
 * @param blocks_per_chunk
 *     Number of blocks the pool allocates per arena slice. Must be > 0.
 *
 * @param prewarm_one_chunk
 *     If true, the pool immediately acquires one slice during initialization
 *     so that the first allocation is O(1). If false, the first allocation
 *     may invoke a grow operation.
 *
 * @param grow_enabled
 *     Whether the pool may request additional slices from the arena when
 *     capacity is exhausted. If false, the pool is fixed-capacity and
 *     allocations fail once all blocks are consumed.
 *
 * @return ::pool_expect_t
 *     - On success:  
 *           @code{.c}
 *           result.has_value == true  
 *           pool_t *p = result.u.value;
 *           @endcode
 *
 *     - On failure:  
 *           @code{.c}
 *           result.has_value == false  
 *           ErrorCode e = result.u.error;
 *           @endcode
 *
 * @par ErrorCode meanings
 * The function may return the following ::ErrorCode values:
 *
 * - ::INVALID_ARG  
 *       If @p arena is NULL, @p block_size == 0, @p blocks_per_chunk == 0,
 *       or @p alignment is non-zero and not a power-of-two.
 *
 * - ::ALIGNMENT_ERROR  
 *       If the requested alignment exceeds the allocator’s supported limits
 *       or violates power-of-two requirements.
 *
 * - ::BAD_ALLOC  
 *       If the pool header could not be allocated from @p arena.
 *
 * - ::OUT_OF_MEMORY  
 *       If @p prewarm_one_chunk is true and the pool is unable to acquire its
 *       initial slice (e.g., arena cannot satisfy the request).
 *
 * Additional errors may be returned via propagated ::ErrorCode values from
 * ::alloc_arena_aligned() or the underlying grow path.
 *
 * @note Freed blocks return to an intrusive free list and are reused in LIFO
 *       order. Only blocks allocated from this pool may be returned to it.
 *
 * @note Memory owned by the pool is not individually freed; all storage is
 *       reclaimed only when the underlying arena is reset or destroyed.
 *
 * @pre
 *     - @p arena was created via ::init_static_arena or ::init_dynamic_arena.  
 *     - @p alignment is zero or a power-of-two.
 *
 * @post On success:
 *     - The pool header resides inside @p arena.  
 *     - If @p prewarm_one_chunk is true:  
 *           @c pool_total_blocks(p) == @p blocks_per_chunk.
 *
 * @sa alloc_pool(), return_pool_element(), reset_pool(), free_arena()
 *
 * @par Example: Fixed-capacity pool inside a static arena
 * @code{.c}
 * enum { BUF = 64 * 1024 };
 * void *buf = aligned_alloc(alignof(max_align_t), BUF);
 *
 * arena_expect_t ar = init_static_arena(buf, BUF, alignof(max_align_t));
 * assert(ar.has_value);
 *
 * pool_expect_t pr = init_pool_with_arena(ar.u.value,
 *                                         64,     // block size
 *                                         0,      // arena default alignment
 *                                         128,    // blocks per slice
 *                                         true,   // prewarm slice
 *                                         false); // fixed capacity
 *
 * if (!pr.has_value) {
 *     // handle pr.u.error
 * }
 * pool_t *p = pr.u.value;
 *
 * void_ptr_expect_t expect = alloc_pool(p);
 * if (!expect.has_value) {
 *    // Handle error 
 * }
 * void *x = expect.u.value; 
 * return_pool_element(p, x);
 *
 * // After all 128 blocks are consumed, further allocations fail:
 * for (int i = 0; i < 128; ++i) assert(alloc_pool(p));
 * assert(!alloc_pool(p)); // no errno; inspect return code only
 *
 * reset_pool(p);
 * free(buf); // static arena storage was caller-owned
 * @endcode
 *
 * @par Example: Grow-on-demand pool inside a dynamic arena
 * @code{.c}
 * arena_expect_t ar = init_dynamic_arena(4096, true, 4096, alignof(max_align_t));
 * assert(ar.has_value);
 *
 * pool_expect_t pr = init_pool_with_arena(ar.u.value,
 *                                         32,
 *                                         16,
 *                                         64,
 *                                         false, // no prewarm
 *                                         true); // allow growth
 *
 * if (!pr.has_value) {
 *     // handle pr.u.error
 * }
 * 
 * pool_t *p = pr.u.value;
 * void_ptr_expect_t expect = alloc_pool(p);
 * if (!expect.has_value) {
 *     // Handle error 
 * }
 * void *b = expect.u.value;
 * return_pool_element(p, b); // returned to free list
 *
 * free_arena(ar.u.value); // destroys pool header and all slices
 * @endcode
 */
pool_expect_t init_pool_with_arena(arena_t* arena,
                                   size_t   block_size,
                                   size_t   alignment,
                                   size_t   blocks_per_chunk,
                                   bool     prewarm_one_chunk,
                                   bool     grow_enabled);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a grow-capable fixed-size memory pool backed by an internally
 *        owned dynamic arena.
 *
 * This constructor creates a pool allocator whose storage is obtained from a
 * freshly created dynamic arena. The arena is fully owned by the pool and
 * destroyed automatically when the pool is destroyed via @c free_pool().
 *
 * The pool allocates fixed-size blocks (“elements”) of @p block_size bytes,
 * using an effective alignment computed from @p alignment and internal
 * constraints. Blocks are dispensed sequentially from arena slices; freed
 * blocks are returned to an intrusive free list for fast O(1) reuse.
 *
 * The pool may operate in:
 *   - **Fixed-capacity mode** (`grow_enabled == false`):  
 *       Only the initial slice is available, and it must be prewarmed
 *       (i.e., @p prewarm_one_chunk must be true).
 *
 *   - **Grow-on-demand mode** (`grow_enabled == true`):  
 *       Additional slices are allocated from the arena automatically when the
 *       pool becomes exhausted.
 *
 * The pool header itself is allocated inside the owned arena, so no external
 * `malloc()` is used.
 *
 * @param block_size
 *      Size in bytes of each user allocation. Must be > 0.
 *
 * @param alignment
 *      Desired block alignment.  
 *      - If 0, defaults to `alignof(max_align_t)`  
 *      - The effective alignment is clamped to at least `alignof(void*)`
 *        to allow storing a next-pointer inside freed blocks.  
 *      - Must be a power-of-two if nonzero.
 *
 * @param blocks_per_chunk
 *      Number of blocks allocated in each arena slice. Must be > 0.
 *
 * @param arena_seed_bytes
 *      Initial size in bytes for the internal dynamic arena. Must be > 0.
 *
 * @param min_chunk_bytes
 *      Minimum slice size requested from the dynamic arena when growth occurs.
 *      Passed directly to ::init_dynamic_arena().
 *
 * @param grow_enabled
 *      If true, the pool may grow by acquiring additional slices.  
 *      If false, the pool is fixed-capacity and **requires**
 *      @p prewarm_one_chunk == true.
 *
 * @param prewarm_one_chunk
 *      If true, the pool eagerly allocates its first slice so the first call to
 *      ::alloc_pool() is O(1).  
 *      If false and the pool is fixed-capacity, initialization fails.
 *
 * @return @c pool_expect_t
 *      Structured success/failure result:
 *      - On success:  
 *          @code
 *          (pool_expect_t){ .has_value = true, .u.value = pool_pointer }
 *          @endcode
 *      - On failure:  
 *          @code
 *          (pool_expect_t){ .has_value = false, .u.error = <ErrorCode> }
 *          @endcode
 *
 * @par Failure conditions (returned as ErrorCode)
 * This function may fail with:
 *
 * 1. **INVALID_ARG**  
 *      - @p block_size == 0  
 *      - @p blocks_per_chunk == 0  
 *      - @p arena_seed_bytes == 0  
 *      - @p alignment is non-zero and not a power-of-two  
 *      - fixed-capacity pool requested (`grow_enabled == false`) but
 *        @p prewarm_one_chunk == false  
 *
 * 2. **LENGTH_OVERFLOW**  
 *      - Computation of slice size overflows:  
 *        @code stride * blocks_per_chunk @endcode
 *
 * 3. **BAD_ALLOC**  
 *      - Dynamic arena creation failed  
 *      - Allocation of the pool header failed  
 *      - Prewarm slice allocation failed  
 *
 * 4. **FEATURE_DISABLED**  
 *      - Dynamic arenas are disabled at compile time
 *        (`ARENA_ENABLE_DYNAMIC == 0`).
 *
 * 5. **STATE_CORRUPT**  
 *      - init_dynamic_arena() reported success but returned a NULL arena
 *        pointer (should not normally occur; defensive condition).
 *
 * @note
 *   - The pool owns its internal arena. Destroying the pool destroys the arena
 *     and all memory allocated within it.  
 *   - Individual blocks returned by ::alloc_pool() must **not** be freed with
 *     `free()`.  
 *   - Freed blocks are recycled through an intrusive free list; memory is not
 *     returned to the arena until pool destruction.
 *
 * @pre
 *   - @c ARENA_ENABLE_DYNAMIC must be enabled at build time.
 *
 * @post On success:
 *   - The pool header resides inside a newly created arena.
 *   - If @p prewarm_one_chunk is true, @c total_blocks == blocks_per_chunk.
 *   - The pool is ready for use with ::alloc_pool() and ::return_pool_element().
 *
 * @sa init_pool_with_arena(), alloc_pool(), return_pool_element(),
 *     free_pool(), init_dynamic_arena()
 *
 * @par Example (grow-enabled pool)
 * @code{.c}
 * pool_expect_t r =
 *     init_dynamic_pool(32,
 *                       16,
 *                       64,
 *                       4096,
 *                       4096,
 *                       true,
 *                       false);
 *
 * if (!r.has_value) {
 *     printf("Pool creation failed: %d\n", r.u.error);
 *     return;
 * }
 *
 * pool_t *p = r.u.value;
 * void_ptr_expect_t expect = alloc_pool(p);
 * if (!expect.has_value) { 
 *     // Handle error
 * }
 * void *x = expect.u.value;
 * return_pool_element(p, x); // returns block to free list
 *
 * free_pool(p);  // destroys pool and its arena
 * @endcode
 */
pool_expect_t init_dynamic_pool(size_t block_size,
                                size_t alignment,
                                size_t blocks_per_chunk,
                                size_t arena_seed_bytes,
                                size_t min_chunk_bytes,
                                bool   grow_enabled,
                                bool   prewarm_one_chunk);
// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a fixed-capacity memory pool backed by a caller-supplied
 *        static buffer.
 *
 * This constructor builds a pool allocator whose storage is carved from a
 * caller-provided memory region @p buffer. Internally, it creates a STATIC
 * arena in-place inside @p buffer via ::init_static_arena(), then allocates
 * the pool header and a single slice of pool blocks from that arena.
 *
 * The resulting pool:
 *   - Has a fixed capacity determined by the remaining space in @p buffer
 *     after arena and pool headers are placed.
 *   - Never grows: when all blocks are consumed and the free list is empty,
 *     further allocations fail.
 *   - Reuses freed blocks via an intrusive free list stored inside the blocks
 *     themselves.
 *
 * The caller remains the ultimate owner of @p buffer. Destroying the pool
 * (e.g., via @c free_pool()) destroys the internal arena and pool metadata,
 * but must not free @p buffer itself; that remains the caller’s responsibility.
 *
 * @param buffer
 *      Caller-supplied backing storage for the static arena and pool. Must
 *      point to at least @p buffer_bytes bytes of writable memory and remain
 *      valid for the entire lifetime of the pool.
 *
 * @param buffer_bytes
 *      Total size in bytes of @p buffer. Must be large enough to contain:
 *        - the arena header,
 *        - the pool header,
 *        - and at least one usable block at the computed stride.
 *
 * @param block_size
 *      Size of the user payload for each block (in bytes). Must be > 0.
 *
 * @param alignment
 *      Desired alignment for pool blocks.  
 *      - If 0, defaults to `alignof(max_align_t)`.  
 *      - The effective alignment is clamped to at least `alignof(void*)` so
 *        that freed blocks can store a next-pointer.  
 *      - Must be a power-of-two if non-zero; otherwise the function fails
 *        with ::ALIGNMENT_ERROR.
 *
 * @return @c pool_expect_t
 *      Structured success/failure result:
 *      - On success:
 *        @code
 *        (pool_expect_t){ .has_value = true, .u.value = pool_pointer }
 *        @endcode
 *      - On failure:
 *        @code
 *        (pool_expect_t){ .has_value = false, .u.error = <ErrorCode> }
 *        @endcode
 *
 * @par Failure conditions (ErrorCode)
 *
 * 1. ::NULL_POINTER  
 *      - @p buffer is NULL.
 *
 * 2. ::INVALID_ARG  
 *      - @p buffer_bytes == 0  
 *      - @p block_size == 0  
 *
 * 3. ::ALIGNMENT_ERROR  
 *      - @p alignment is non-zero and not a power-of-two.
 *
 * 4. Errors propagated from ::init_static_arena()  
 *      - For example, the caller buffer is too small to hold the arena header,
 *        chunk header, and at least one data byte, or normalization of the
 *        static arena layout fails. These appear as whatever ::init_static_arena()
 *        encodes in @c arena_expect_t.u.error.
 *
 * 5. Errors from ::alloc_arena_aligned() when allocating:
 *      - the pool header, or
 *      - the single pool slice:
 *        - Typically ::OUT_OF_MEMORY if there is insufficient usable space
 *          remaining in the static arena to satisfy the request.
 *
 * 6. ::OUT_OF_MEMORY  
 *      - There is enough space for the arena and pool header, but the computed
 *        capacity allows zero blocks (i.e., the slice cannot contain even one
 *        block at the chosen stride).
 *
 * 7. ::STATE_CORRUPT  
 *      - Defensive: ::init_static_arena() reports success but returns a
 *        NULL arena pointer.
 *
 * @note
 *   - This pool is strictly fixed-capacity: no additional slices will ever be
 *     allocated. ::alloc_pool() will eventually fail once all blocks are
 *     allocated and the free list is empty.
 *   - Individual blocks must not be passed to `free()`. They are reclaimed
 *     only via ::return_pool_element() or when the pool (and its arena) are
 *     torn down.
 *   - The underlying buffer @p buffer is not freed by this API; its lifetime
 *     is entirely managed by the caller.
 *
 * @pre
 *   - @p buffer points to at least @p buffer_bytes bytes of writable memory.
 *   - The lifetime of @p buffer strictly dominates the lifetime of the pool.
 *
 * @post On success:
 *   - A STATIC arena is constructed in-place inside @p buffer.
 *   - The pool header resides inside that arena.
 *   - A single slice covering @c total_blocks * stride bytes is carved out and
 *     ready to serve allocations.
 *
 * @sa init_dynamic_pool(), init_pool_with_arena(),
 *     alloc_pool(), return_pool_element(), free_pool(),
 *     init_static_arena()
 *
 * @par Example
 * @code{.c}
 * enum { BUF = 64 * 1024 };  // 64 KiB caller-owned buffer
 * static uint8_t buf[BUF];
 *
 * pool_expect_t r =
 *     init_static_pool(buf,
 *                      BUF,
 *                      64,
 *                      0);  // use default
 *
 * if (!r.has_value) {
 *     printf("static pool init failed: %d\n", r.u.error);
 *     return;
 * }
 *
 * pool_t *p = r.u.value;
 *
 * void_ptr_expect_t expect = alloc_pool(p);
 * if (!expect.has_value) {
 *     // Handle error 
 * }
 * void *x = expect.u.value;
 * expect = alloc_pool(p);
 * if (!expect.has_value) {
 *     // Handle error 
 * }
 * void *y = expect.u.value;      // ok
 * return_pool_element(p, x);    // block reclaimed for reuse
 *
 * // When done:
 * free_pool(p);   // destroys pool and its internal arena
 * // 'buf' remains caller-owned and is not freed here.
 * @endcode
 */
pool_expect_t init_static_pool(void*  buffer,
                               size_t buffer_bytes,
                               size_t block_size,
                               size_t alignment);
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate a fixed-size block from a memory pool.
 *
 * Retrieves a block from the pool using the following strategy:
 *
 *  1. **Free-list reuse (fast path):**  
 *     If the pool contains previously freed blocks, the most recently
 *     released block is popped and returned immediately.
 *
 *  2. **Carve from current slice:**  
 *     If the active slice still has unused space (`cur < end`), the next block
 *     is carved from the slice and returned.
 *
 *  3. **Grow-on-demand (dynamic pools only):**  
 *     If the active slice is full and @c grow_enabled is true, a new slice is
 *     requested from the pool’s backing arena, and the block is allocated from
 *     that fresh slice.
 *
 * If @p zeroed is true, the block’s user-visible region (`block_size` bytes)
 * is zero-filled. Any stride padding is left uninitialized.
 *
 * This function does **not** use errno. Errors are reported through a
 * ::void_ptr_expect_t return value.
 *
 * @param pool
 *     Pointer to a valid ::pool_t instance. Must not be NULL.
 *
 * @param zeroed
 *     If true, the returned block's payload is zero-initialized.
 *
 * @return ::void_ptr_expect_t describing success or failure:
 *
 *   - **Success:**  
 *       @code
 *       expect.has_value == true
 *       expect.u.value   -> pointer to allocated block
 *       @endcode
 *
 *   - **Failure:**  
 *       @code
 *       expect.has_value == false
 *       expect.u.error   == one of:
 *           NULL_POINTER      — @p pool is NULL
 *           CAPACITY_OVERFLOW — pool exhausted and growth disabled
 *           BAD_ALLOC         — arena failed to grow a new slice
 *           STATE_CORRUPT     — inconsistent state after growth
 *       @endcode
 *
 * @par Failure Conditions
 * Failure occurs when:
 *  1. @p pool is NULL → ::NULL_POINTER  
 *  2. No blocks remain and growth is disabled → ::CAPACITY_OVERFLOW  
 *  3. Growth is enabled but arena allocation fails → ::BAD_ALLOC  
 *  4. Growth succeeded but internal state invalid → ::STATE_CORRUPT  
 *
 * @note
 *   - The returned block must be released using ::return_pool_element(),
 *     not @c free().
 *   - Memory is reclaimed only when the underlying arena is reset or destroyed.
 *   - Blocks are reused in **LIFO** order (stack behavior).
 *
 * @sa init_pool_with_arena(), init_static_pool(), pool_grow(), return_pool_element()
 *
 * @par Example
 * @code{.c}
 * // Create a dynamic arena and pool
 * arena_expect_t ar = init_dynamic_arena(4096u, true, 4096u, alignof(max_align_t));
 * if (!ar.has_value) {
 *     fprintf(stderr, "Arena init failed: %d\n", ar.u.error);
 *     return;
 * }
 * arena_t *arena = ar.u.value;
 *
 * // Initialize a pool where each block holds 32 bytes
 * pool_expect_t pr = init_pool_with_arena(arena,
 *                                         32,          // block size
 *                                         0,           // alignment = arena default
 *                                         64,          // blocks per chunk
 *                                         true,        // prewarm first slice
 *                                         true);       // allow dynamic growth
 * if (!pr.has_value) {
 *     fprintf(stderr, "Pool init failed: %d\n", pr.u.error);
 *     free_arena(arena);
 *     return;
 * }
 * pool_t *pool = pr.u.value;
 *
 * // Allocate a zeroed block
 * void_ptr_expect_t b1 = alloc_pool(pool, true);
 * if (!b1.has_value) {
 *     fprintf(stderr, "alloc_pool failed: %d\n", b1.u.error);
 *     return;
 * }
 *
 * // Allocate a second block
 * void_ptr_expect_t b2 = alloc_pool(pool, false);
 *
 * // Return first block to the free list
 * return_pool_element(pool, b1.u.value);
 *
 * // Next allocation will reuse the returned block (LIFO behavior)
 * void_ptr_expect_t reused = alloc_pool(pool, false);
 * assert(reused.has_value && reused.u.value == b1.u.value);
 *
 * // Cleanup
 * free_arena(arena); // destroys the pool and arena
 * @endcode
 */
void_ptr_expect_t alloc_pool(pool_t* pool, bool zeroed);
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
 * @brief Allocate a block from a pool via the generic allocator interface.
 *
 * This function adapts ::alloc_pool() to the allocator vtable model.
 * The caller supplies a pool via @p ctx, and requests a block of size @p size.
 *
 * Pool allocators dispense *fixed-size* blocks. Therefore, the request is valid
 * only if @p size is less than or equal to the pool's configured block size.
 *
 * On success, this returns a ::void_ptr_expect_t with:
 *   - @c has_value = true,
 *   - @c u.value   = pointer to a valid pool block.
 *
 * On failure, @c has_value is false and @c u.error describes the reason:
 *   - ::NULL_POINTER     — @p ctx was NULL.
 *   - ::INVALID_ARG      — requested @p size exceeds pool block capacity.
 *   - ::BAD_ALLOC        — pool is exhausted and cannot grow.
 *
 * @param ctx     Pointer to a ::pool_t instance.
 * @param size    Number of bytes requested (must not exceed pool block size).
 * @param zeroed  If true, the returned block is zero-initialized.
 *
 * @return A ::void_ptr_expect_t containing either an allocated pointer or
 *         an error code.
 */
static inline void_ptr_expect_t pool_v_alloc(void *ctx, size_t size, bool zeroed) {
    pool_t *pool = (pool_t *)ctx;

    if (!pool) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }

    /* Requested size must fit within a single pool block. */
    if (size > pool_block_size(pool)) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = INVALID_ARG   /* asking pool for more than one block can hold */
        };
    }

    /* Delegate to the pool’s allocator (LIFO free list, slices, growth, etc.). */
    return alloc_pool(pool, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate a block from a pool with an alignment request.
 *
 * Pool allocators have a *fixed* alignment determined at initialization.
 * Therefore @p align is ignored, and alignment errors cannot occur here as long
 * as @p size fits in a single pool block.
 *
 * Behavior is identical to ::pool_v_alloc(), except that @p align is ignored.
 *
 * @param ctx     Pointer to a ::pool_t instance.
 * @param size    Number of bytes requested (must not exceed pool block size).
 * @param align   Ignored; pool alignment is fixed.
 * @param zeroed  If true, the returned block is zero-initialized.
 *
 * @return A ::void_ptr_expect_t with either:
 *         - valid block pointer, or
 *         - error: ::NULL_POINTER, ::INVALID_ARG, or ::BAD_ALLOC.
 */
static inline void_ptr_expect_t pool_v_alloc_aligned(void *ctx,
                                                     size_t size,
                                                     size_t align,
                                                     bool   zeroed) {
    (void)align;  /* Per-call alignment is ignored; pool has a fixed block alignment. */

    pool_t *pool = (pool_t *)ctx;

    if (!pool) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }

    /* Same “fits in one block” rule as pool_v_alloc. */
    if (size > pool_block_size(pool)) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = INVALID_ARG
        };
    }

    /* Alignment is determined by the pool’s configuration; we just allocate. */
    return alloc_pool(pool, zeroed);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Reallocate memory from a pool via the vtable interface.
 *
 * Memory pools cannot resize blocks. Therefore, reallocation follows strict
 * rules:
 *
 *   - If @p old_ptr is NULL → returns an error (::INVALID_ARG).
 *   - If @p new_size exceeds the pool’s block size → returns ::CAPACITY_OVERFLOW.
 *   - Otherwise, the reallocation is a **no-op**:
 *       the original pointer is returned unchanged.
 *
 * No copying occurs, and no new block is allocated.
 *
 * @param ctx        Pointer to a ::pool_t instance.
 * @param old_ptr    Existing block allocated from the pool (must not be NULL).
 * @param old_size   Ignored (block size is fixed).
 * @param new_size   Requested new size (must not exceed block size).
 * @param zeroed     Ignored (no new memory is created).
 *
 * @return A ::void_ptr_expect_t with:
 *         - success: @p old_ptr unchanged,
 *         - failure: ::NULL_POINTER, ::INVALID_ARG, or ::CAPACITY_OVERFLOW.
 */
static inline void_ptr_expect_t pool_v_realloc(void *ctx,
                                               void *old_ptr,
                                               size_t old_size,
                                               size_t new_size,
                                               bool  zeroed) {
    (void)old_size;
    (void)zeroed;

    pool_t *pool = (pool_t *)ctx;

    if (!pool) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }

    if (!old_ptr) {
        /* For this backend, realloc with NULL is treated as invalid. */
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = INVALID_ARG
        };
    }

    /* Pool blocks are fixed-size: we cannot grow beyond a single block. */
    if (new_size > pool_block_size(pool)) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = CAPACITY_OVERFLOW
        };
    }

    /* No-op: caller must treat this as “same block, same capacity”. */
    return (void_ptr_expect_t){
        .has_value = true,
        .u.value   = old_ptr
    };
}
// --------------------------------------------------------------------------------

/**
 * @brief Reallocate memory from a pool with an alignment request.
 *
 * Alignment does not affect reallocation since pools cannot resize blocks and
 * have fixed alignment. The semantics mirror ::pool_v_realloc():
 *
 *   - @p old_ptr must not be NULL,
 *   - @p new_size must not exceed pool block size,
 *   - otherwise, return @p old_ptr unchanged.
 *
 * @p align is ignored.
 *
 * @param ctx        Pointer to a ::pool_t instance.
 * @param old_ptr    Existing block allocated from the pool.
 * @param old_size   Ignored.
 * @param new_size   New size (must not exceed pool block size).
 * @param zeroed     Ignored.
 * @param align      Ignored.
 *
 * @return A ::void_ptr_expect_t containing either @p old_ptr or an error code.
 */
static inline void_ptr_expect_t pool_v_realloc_aligned(void *ctx,
                                                       void *old_ptr,
                                                       size_t old_size,
                                                       size_t new_size,
                                                       bool   zeroed,
                                                       size_t align) {
    (void)align;   /* Per-call alignment is ignored; pool has fixed alignment. */
    (void)old_size;
    (void)zeroed;

    pool_t *pool = (pool_t *)ctx;

    if (!pool) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = NULL_POINTER
        };
    }

    if (!old_ptr) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = INVALID_ARG
        };
    }

    if (new_size > pool_block_size(pool)) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = CAPACITY_OVERFLOW
        };
    }

    /* Same no-op semantics as pool_v_realloc. */
    return (void_ptr_expect_t){
        .has_value = true,
        .u.value   = old_ptr
    };
}
// --------------------------------------------------------------------------------

/**
 * @brief Return a previously allocated block to the pool.
 *
 * This inserts @p ptr into the pool’s intrusive free list for later reuse.
 *
 * Invalid inputs result in a silent no-op (consistent with MISRA and
 * allocator-vtable expectations). No error is returned.
 *
 * @param ctx   Pointer to the ::pool_t instance.
 * @param ptr   Block previously returned by alloc_pool(). Must belong to
 *              this pool; the function does not validate ownership.
 *
 * @note This function never frees memory; it only recycles blocks.
 */
static inline void pool_v_return(void *ctx, void *ptr) {
    pool_t *pool = (pool_t *)ctx;

    if (!pool || !ptr) {
        /* Silent no-op on invalid input; no errno side-effects. */
        return;
    }

    return_pool_element(pool, ptr);
}
// --------------------------------------------------------------------------------

/**
 * @brief Destroy a pool via the vtable interface.
 *
 * This invokes ::free_pool() on the underlying pool instance. The pool must
 * have been allocated using a pool initializer such as ::init_pool_with_arena()
 * or ::init_dynamic_pool().
 *
 * Invalid input is ignored by design (no error reporting).
 *
 * @param ctx  Pointer to the ::pool_t instance to destroy.
 *
 * @note If the pool owns its arena, that arena is destroyed as well.
 */
static inline void pool_v_free(void *ctx) {
    pool_t *pool = (pool_t *)ctx;

    if (!pool) {
        return; /* nothing to do */
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

typedef struct {
    bool has_value;
    union {
        freelist_t* value;
        ErrorCode error;
    } u;
} freelist_expect_t;
// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a freelist allocator using memory obtained from an existing arena.
 *
 * Creates a ::freelist_t object entirely within the storage provided by an
 * existing ::arena_t. The freelist does not own the underlying memory; the
 * arena remains responsible for its lifetime. All memory managed by the
 * freelist is carved from the arena via a single allocation.
 *
 * The internal memory layout is:
 *
 *     [ freelist_t | aligned usable memory region ]
 *
 * Within the usable region, the freelist manages variable-sized blocks using
 * a linked free-block structure. Returned blocks are reinserted into the free
 * list and coalesced when possible to reduce external fragmentation.
 *
 * @param arena
 *        Pointer to an initialized ::arena_t. Must not be NULL.
 *
 * @param size
 *        Requested number of usable bytes for the freelist (excluding internal
 *        metadata). Must be at least ::freelist_min_request. The actual usable
 *        capacity may be rounded up to satisfy alignment constraints.
 *
 * @param alignment
 *        Desired alignment for both the freelist structure and its internal
 *        memory region. If zero, defaults to @c alignof(max_align_t).
 *        Non–power-of-two values are rounded up. The effective alignment is
 *        always at least @c alignof(max_align_t).
 *
 * @return
 *        A ::freelist_expect_t describing the result:
 *
 *        - @c has_value == true  
 *          @c u.value points to a fully initialized ::freelist_t.
 *
 *        - @c has_value == false  
 *          @c u.error contains an ::ErrorCode describing the failure.
 *
 * @par Failure conditions
 * Initialization fails if any of the following occur:
 *
 * - @p arena is NULL.
 * - @p size is smaller than ::freelist_min_request.
 * - The requested alignment cannot be normalized to a valid power-of-two.
 * - Arithmetic overflow occurs while computing internal layout sizes.
 * - The arena cannot supply a contiguous block large enough to hold the
 *   freelist header and usable memory region.
 *
 * @note
 * The freelist has fixed capacity. It does not grow after initialization.
 * All capacity is determined by the initial @p size request and the available
 * space in the arena.
 *
 * @note
 * The freelist shares the lifetime of the parent arena. When the arena is
 * reset or destroyed, all allocations obtained from the freelist become
 * invalid.
 *
 * @note
 * Allocation functions operating on this freelist (e.g.,
 * ::alloc_freelist()) return a ::void_ptr_expect_t to explicitly report
 * allocation success or failure.
 *
 * @warning
 * The freelist does not perform ownership validation. Passing pointers not
 * allocated by this freelist to ::return_freelist_element() results in
 * undefined behavior.
 *
 * @par Example
 * Creating and using a freelist inside a static arena:
 *
 * @code{.c}
 * #include "c_allocator.h"
 *
 * int main(void) {
 *     uint8_t buffer[2048];
 *
 *     arena_expect_t a =
 *         init_static_arena(buffer, sizeof buffer, alignof(max_align_t));
 *     if (!a.has_value) {
 *         return 1;
 *     }
 *
 *     freelist_expect_t f =
 *         init_freelist_with_arena(a.u.value, 1024, 0);
 *     if (!f.has_value) {
 *         // handle initialization error via f.u.error
 *         return 1;
 *     }
 *
 *     freelist_t* fl = f.u.value;
 *
 *     void_ptr_expect_t p = alloc_freelist(fl, 64, true);
 *     if (!p.has_value) {
 *         // allocation failed
 *         return 1;
 *     }
 *
 *     // use p.u.value
 *
 *     return_freelist_element(fl, p.u.value);
 *
 *     reset_arena(a.u.value);
 *     return 0;
 * }
 * @endcode
 */
freelist_expect_t init_freelist_with_arena(arena_t* arena, 
                                     size_t size, 
                                     size_t alignment);
// -------------------------------------------------------------------------------- 

/**
 * @brief Create a dynamically backed freelist allocator.
 *
 * Constructs a `freelist_t` inside a newly created dynamic `arena_t`. The
 * freelist *owns* the arena and is responsible for its lifetime. All memory
 * used by the freelist—including the freelist structure itself and all future
 * allocations—resides entirely within this dedicated arena.
 *
 * The caller specifies a desired minimum usable payload size (`bytes`). This
 * function computes the minimum arena space required to accommodate:
 *
 *   - an aligned `freelist_t` control structure,
 *   - at least one `free_block_t`, and
 *   - the requested usable memory region.
 *
 * The underlying dynamic arena may allocate more space than requested; any
 * additional capacity is incorporated into the freelist automatically.
 *
 * @param bytes
 *        Requested minimum usable payload size in bytes (excluding internal
 *        metadata). Must be at least `freelist_min_request`.
 *
 * @param alignment
 *        Desired alignment for the freelist header and all internal allocations.
 *        If zero, `alignof(max_align_t)` is used. Non–power-of-two values are
 *        rounded up. The effective alignment is always at least
 *        `alignof(max_align_t)`.
 *
 * @param resize
 *        Controls whether the underlying dynamic arena is permitted to grow.
 *        In the current design, the freelist itself remains fixed-size after
 *        construction; this flag primarily affects arena behavior and future
 *        extensibility.
 *
 * @return
 *        A `freelist_expect_t` result:
 *
 *        - `has_value == true`  
 *          `u.value` points to a fully initialized `freelist_t`.
 *
 *        - `has_value == false`  
 *          `u.error` describes the failure condition.
 *
 * @par Failure conditions
 * This function returns `has_value == false` for any of the following reasons:
 *
 *   - **Invalid arguments**
 *       - `bytes` is smaller than `freelist_min_request`
 *       - arithmetic overflow occurs during size calculations
 *
 *   - **Invalid alignment**
 *       - requested alignment cannot be normalized to a valid power-of-two
 *
 *   - **Insufficient memory**
 *       - the dynamic arena cannot allocate the required space
 *
 *   - **Unsupported configuration**
 *       - dynamic arenas are disabled at compile time
 *         (`ARENA_ENABLE_DYNAMIC == 0`)
 *
 * @note
 *        The freelist owns the arena created by this function. Call
 *        `free_freelist()` to release all associated memory.
 *
 * @note
 *        The actual usable capacity of the freelist may exceed the requested
 *        `bytes`, depending on how the underlying arena rounds allocations.
 *
 * @warning
 *        All pointers obtained from this freelist become invalid once
 *        `free_freelist()` is called.
 *
 * @par Example
 * Creating a standalone freelist with dynamic backing:
 *
 * @code{.c}
 * #include "c_allocator.h"
 *
 * int main(void) {
 *     freelist_expect_t r = init_dynamic_freelist(4096, 0, false);
 *     if (!r.has_value) {
 *         // handle error via r.u.error
 *         return 1;
 *     }
 *
 *     freelist_t* fl = r.u.value;
 *
 *     // Allocate memory from the freelist
 *     void_ptr_expect_t m = alloc_freelist(fl, 128, true);
 *     if (!m.has_value) {
 *         // allocation failed (out of space, etc.)
 *         free_freelist(fl);
 *         return 1;
 *     }
 *
 *     void* p = m.u.value;
 *     // ... use p ...
 *
 *     // Return memory to the freelist
 *     return_freelist_element(fl, p);
 *
 *     // Destroy the freelist and its arena
 *     free_freelist(fl);
 *     return 0;
 * }
 * @endcode
 *
 * @sa init_freelist_with_arena(), alloc_freelist(), return_freelist_element(),
 *     free_freelist()
 */
freelist_expect_t init_dynamic_freelist(size_t bytes, size_t alignment, bool resize);
// -------------------------------------------------------------------------------- 

/**
 * @brief Initialize a freelist allocator over a user-supplied static buffer.
 *
 * Constructs a `freelist_t` entirely within a caller-provided memory buffer.
 * No heap allocation is performed. The freelist does **not** take ownership of
 * the buffer; the caller is responsible for ensuring that the buffer remains
 * valid for the full lifetime of the freelist.
 *
 * A static freelist is well suited for embedded systems, scratch allocators,
 * fixed-capacity workspaces, or environments where dynamic allocation is
 * restricted or forbidden.
 *
 * Internally, this function creates a non-owning STATIC arena over the supplied
 * buffer and then carves a freelist allocator out of that arena.
 *
 * The in-buffer memory layout is:
 *
 *     [ aligned freelist_t | aligned free region for allocations ]
 *
 * The free region begins as a single large free block and is managed using
 * variable-sized blocks with coalescing on free.
 *
 * @param buffer
 *     Pointer to the start of the user-supplied memory region.
 *     Must not be NULL.
 *
 * @param bytes
 *     Total size of the buffer in bytes. Must be large enough to contain at
 *     least:
 *
 *         sizeof(freelist_t) + sizeof(free_block_t)
 *
 *     and preferably more to provide usable allocation capacity. If insufficient,
 *     initialization fails.
 *
 * @param alignment
 *     Required alignment for the freelist header and all internal allocations.
 *     If zero, `alignof(max_align_t)` is used. Non–power-of-two values are
 *     rounded up. The effective alignment always satisfies:
 *
 *         alignment >= alignof(max_align_t)
 *
 * @return
 *     A `freelist_expect_t` result:
 *
 *     - `has_value == true`
 *         - `u.value` points to a fully initialized `freelist_t`
 *
 *     - `has_value == false`
 *         - `u.error` indicates the reason initialization failed
 *
 * @par Failure reasons
 * Initialization fails with `has_value == false` for any of the following:
 *
 * 1. **Invalid arguments**
 *      - @p buffer is NULL
 *      - @p bytes is smaller than the minimum required control structures
 *
 * 2. **Alignment errors**
 *      - Requested alignment cannot be normalized to a valid power-of-two
 *
 * 3. **Insufficient buffer capacity**
 *      - The buffer cannot accommodate the freelist header and at least one
 *        free block after alignment
 *
 * 4. **Arena initialization failure**
 *      - Internal static arena setup fails (propagated from
 *        ::init_static_arena)
 *
 * 5. **Arena allocation failure**
 *      - Internal call to ::alloc_arena fails due to layout constraints
 *
 * @note
 *     The freelist does **not** own the underlying memory. Destroying or
 *     resetting the freelist does not free the buffer.
 *
 * @note
 *     The internal arena created by this function is also non-owning and
 *     exists solely to support allocator composition.
 *
 * @note
 *     All memory returned by `alloc_freelist()` becomes invalid if the
 *     underlying arena is reset or destroyed.
 *
 * @par Example
 * Creating a freelist over a static buffer:
 *
 * @code{.c}
 * #include "c_allocator.h"
 *
 * int main(void) {
 *     uint8_t buffer[2048];
 *
 *     freelist_expect_t r =
 *         init_static_freelist(buffer, sizeof buffer, 0);
 *
 *     if (!r.has_value) {
 *         // Handle initialization error via r.u.error
 *         return 1;
 *     }
 *
 *     freelist_t* fl = r.u.value;
 *
 *     void_ptr_expect_t a = alloc_freelist(fl, 128, true);
 *     if (!a.has_value) {
 *         // Allocation failed (capacity exhausted, etc.)
 *         return 1;
 *     }
 *
 *     void* p = a.u.value;
 *
 *     // Use memory...
 *
 *     return_freelist_element(fl, p);
 *
 *     // No destruction required; caller owns the buffer.
 *     return 0;
 * }
 * @endcode
 *
 * @see init_dynamic_freelist
 * @see alloc_freelist
 * @see return_freelist_element
 * @see init_static_arena
 */
freelist_expect_t init_static_freelist(void* buffer, size_t bytes, size_t alignment);
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
 * @brief Allocate a block of memory from a freelist allocator.
 *
 * This function allocates @p bytes of user-visible memory from the freelist,
 * returning a pointer aligned to the freelist’s effective alignment. Internally,
 * the allocator may consume more memory than requested due to:
 *
 *   - a small allocation header (`freelist_header_t`) stored immediately
 *     before the returned user pointer
 *   - alignment padding between the free block and the user region
 *   - full-block consumption when the remaining fragment is too small to
 *     form another free block
 *
 * These internal details are fully managed by the allocator and are invisible
 * to the caller.
 *
 * Memory returned by this function must be released back to the freelist using
 * ::return_freelist_element(). The allocator performs block coalescing where
 * possible to reduce external fragmentation.
 *
 * @param fl
 *     Pointer to a valid ::freelist_t instance. Must not be NULL.
 *
 * @param bytes
 *     Number of user bytes requested. Must be greater than zero.
 *
 * @param zeroed
 *     If @c true, the returned user region is zero-initialized. If @c false,
 *     the contents of the block are left uninitialized.
 *
 * @return
 *     A ::void_ptr_expect_t result:
 *
 *     - @c has_value == true  
 *         - @c u.value points to an aligned user-accessible memory region
 *
 *     - @c has_value == false  
 *         - @c u.error indicates the reason for failure
 *
 * @retval INVALID_ARG
 *     The freelist pointer is NULL, the requested size is zero, or internal
 *     argument validation fails.
 *
 * @retval ALIGNMENT_ERROR
 *     The freelist alignment is invalid (not a power-of-two).
 *
 * @retval CAPACITY_OVERFLOW
 *     No suitable free block exists to satisfy the request, or an internal
 *     size computation overflow was detected.
 *
 * @note
 *     The returned pointer is always aligned to at least
 *     `freelist_alignment(fl)`. Alignment padding may increase the amount of
 *     freelist capacity consumed beyond @p bytes.
 *
 * @note
 *     The freelist may split free blocks or consume them entirely to avoid
 *     leaving unusable tail fragments.
 *
 * @note
 *     This function is not thread-safe unless externally synchronized.
 *
 * @par Example
 * Allocating and returning memory from a freelist:
 *
 * @code{.c}
 * freelist_expect_t fe = init_dynamic_freelist(2048, 0, false);
 * if (!fe.has_value) {
 *     // handle freelist initialization error via fe.u.error
 *     return;
 * }
 *
 * freelist_t* fl = fe.u.value;
 *
 * void_ptr_expect_t r = alloc_freelist(fl, 128, true);
 * if (!r.has_value) {
 *     // handle allocation failure via r.u.error
 *     free_freelist(fl);
 *     return;
 * }
 *
 * void* p = r.u.value;
 *
 * // p is aligned and safe to use
 *
 * return_freelist_element(fl, p);
 *
 * free_freelist(fl);
 * @endcode
 *
 * @see return_freelist_element
 * @see init_freelist_with_arena
 * @see init_dynamic_freelist
 * @see init_static_freelist
 */
void_ptr_expect_t alloc_freelist(freelist_t* fl, size_t size, bool zero);
// -------------------------------------------------------------------------------- 

/**
 * @brief Allocate an aligned block of memory from a freelist.
 *
 * This function behaves like ::alloc_freelist(), but allows the caller to
 * request a specific alignment for the returned pointer. The effective
 * alignment is computed as follows:
 *
 *   - If @p alignment is zero, the freelist’s base alignment is used.
 *   - If @p alignment is not a power-of-two, it is rounded up.
 *   - The final alignment is always at least ::freelist_alignment(fl).
 *
 * Internally, the allocator may consume more memory than the requested
 * @p bytes due to:
 *
 *   - a freelist header stored immediately before the user pointer
 *   - padding required to satisfy the effective alignment
 *   - full-block consumption when the remaining fragment would be too small
 *     to form another free block
 *
 * These details are tracked internally and do not affect the size or alignment
 * guarantees of the pointer returned to the caller.
 *
 * @param fl
 *        Pointer to a valid ::freelist_t. Must not be NULL.
 *
 * @param bytes
 *        Number of user bytes requested. Must be greater than zero.
 *
 * @param alignment
 *        Desired alignment for the returned pointer. If zero, the freelist’s
 *        base alignment is used. Non–power-of-two values are rounded up. The
 *        effective alignment is guaranteed to be at least
 *        ::freelist_alignment(fl).
 *
 * @param zeroed
 *        If true, the user-visible region is zero-initialized before being
 *        returned.
 *
 * @return
 *        A ::void_ptr_expect_t describing the result:
 *
 *        - @c has_value == true  
 *          @c u.value points to an aligned user region of @p bytes.
 *
 *        - @c has_value == false  
 *          @c u.error indicates the reason for failure:
 *
 *          - ::INVALID_ARG  
 *              if @p fl is NULL or @p bytes is zero
 *
 *          - ::ALIGNMENT_ERROR  
 *              if alignment normalization fails or overflows
 *
 *          - ::CAPACITY_OVERFLOW  
 *              if no suitable free block is available
 *
 * @note
 *        The returned pointer must be released using
 *        ::return_freelist_element(). Individual blocks must not be freed
 *        with @c free().
 *
 * @note
 *        Requesting a stricter alignment may increase internal fragmentation
 *        but does not otherwise affect freelist accounting semantics.
 *
 * @par Example
 * @code{.c}
 * freelist_expect_t fr = init_dynamic_freelist(4096, 0, false);
 * if (!fr.has_value) {
 *     return 1;
 * }
 *
 * freelist_t *fl = fr.u.value;
 *
 * void_ptr_expect_t r =
 *     alloc_freelist_aligned(fl, 256, 64, true);
 *
 * if (!r.has_value) {
 *     // handle allocation failure via r.u.error
 *     free_freelist(fl);
 *     return 1;
 * }
 *
 * void *p = r.u.value;
 *
 * // use p ...
 *
 * return_freelist_element(fl, p);
 * free_freelist(fl);
 * @endcode
 *
 * @see alloc_freelist
 * @see return_freelist_element
 */

void_ptr_expect_t alloc_freelist_aligned(freelist_t* fl,
                                         size_t      bytes,
                                         size_t      alignment,
                                         bool        zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Resize an allocation managed by a freelist.
 *
 * This function provides a `realloc`-like interface for memory obtained from
 * a ::freelist_t. Because freelists do not support in-place growth, resizing
 * follows these rules:
 *
 *   - If @p ptr is @c NULL, the call behaves like ::alloc_freelist().
 *   - If @p new_size is less than or equal to @p old_size, the existing
 *     pointer is returned unchanged (the block is not shrunk).
 *   - If the block must grow, a new block is allocated, the first
 *     @p old_size bytes are copied, the old block is returned to the freelist
 *     via ::return_freelist_element(), and the new pointer is returned.
 *
 * The returned pointer is always aligned to at least
 * ::freelist_alignment(fl).
 *
 * @param fl
 *        Pointer to a valid ::freelist_t. Must not be NULL.
 *
 * @param ptr
 *        Existing user pointer previously returned by ::alloc_freelist or
 *        ::alloc_freelist_aligned. May be NULL.
 *
 * @param old_size
 *        Number of user bytes originally allocated at @p ptr.
 *        The freelist does not store user sizes internally; the caller must
 *        supply an accurate value.
 *
 * @param new_size
 *        Number of user bytes requested for the resized allocation.
 *        Must be greater than zero.
 *
 * @param zeroed
 *        If true and the allocation grows, the newly added region
 *        (@c [old_size, new_size)) is zero-initialized.
 *
 * @return
 *        A ::void_ptr_expect_t describing the result:
 *
 *        - @c has_value == true  
 *              @c u.value points to:
 *                - the original pointer if no growth was required, or
 *                - a newly allocated block if growth succeeded.
 *
 *        - @c has_value == false  
 *              @c u.error contains the reason for failure, typically:
 *                - ::INVALID_ARG        — invalid arguments (e.g. NULL freelist,
 *                                          zero @p new_size)
 *                - ::CAPACITY_OVERFLOW  — insufficient free space to grow
 *
 * @note
 *        The freelist never frees memory implicitly. Old blocks remain valid
 *        until explicitly returned to the freelist.
 *
 * @note
 *        In-place growth is not supported; all expansions require
 *        allocate–copy–return semantics.
 *
 * @note
 *        Passing a pointer not allocated from this freelist results in
 *        undefined behavior.
 *
 * @code{.c}
 * freelist_expect_t flr = init_dynamic_freelist(4096, 0, false);
 * if (!flr.has_value) {
 *     // handle freelist initialization error
 *     return;
 * }
 *
 * freelist_t* fl = flr.u.value;
 *
 * void_ptr_expect_t a = alloc_freelist(fl, 64, false);
 * if (!a.has_value) return;
 *
 * // Grow from 64 bytes to 200 bytes
 * void_ptr_expect_t r =
 *     realloc_freelist(fl, a.u.value, 64, 200, true);
 *
 * if (!r.has_value) {
 *     // handle resize failure via r.u.error
 *     return;
 * }
 *
 * return_freelist_element(fl, r.u.value);
 * free_freelist(fl);
 * @endcode
 *
 * @see alloc_freelist
 * @see alloc_freelist_aligned
 * @see return_freelist_element
 */
void_ptr_expect_t realloc_freelist(freelist_t* fl, void* variable, size_t old_size, size_t new_size, bool zeroed);
// -------------------------------------------------------------------------------- 

/**
 * @brief Resize an aligned allocation managed by a freelist.
 *
 * This function is the alignment-aware counterpart to
 * ::realloc_freelist. It provides `realloc`-like behavior for memory
 * obtained from a ::freelist_t, while allowing the caller to request a
 * specific alignment for the resized allocation.
 *
 * Because freelists do not support in-place growth, expansion is always
 * implemented as:
 *
 *   1. Allocate a new block of @p new_size bytes with the requested
 *      alignment using ::alloc_freelist_aligned().
 *   2. Copy the first @p old_size bytes from @p ptr into the new block.
 *   3. Optionally zero-fill the newly added tail region.
 *   4. Return the old block to the freelist via ::return_freelist_element().
 *
 * If @p new_size is less than or equal to @p old_size, the function performs
 * a no-op and returns the original pointer unchanged.
 *
 * The effective alignment of the returned block is:
 *
 *     max(requested_alignment, freelist_alignment(fl))
 *
 * @param fl
 *        Pointer to a valid ::freelist_t. Must not be NULL.
 *
 * @param ptr
 *        Existing pointer previously returned by ::alloc_freelist or
 *        ::alloc_freelist_aligned. May be NULL, in which case this function
 *        behaves like ::alloc_freelist_aligned.
 *
 * @param old_size
 *        Number of user bytes originally allocated at @p ptr. The freelist
 *        does not track user sizes internally; the caller must supply this
 *        value accurately.
 *
 * @param new_size
 *        Number of user bytes requested for the resized allocation.
 *        Must be greater than zero.
 *
 * @param zeroed
 *        If true and a new block is allocated, the newly added tail region
 *        (bytes [@p old_size, @p new_size)) is zero-filled.
 *
 * @param alignment
 *        Desired alignment for the returned pointer. If zero, the freelist’s
 *        base alignment is used. Non–power-of-two values are rounded up.
 *
 * @return
 *        A ::void_ptr_expect_t with the following semantics:
 *
 *        - @c has_value == true  
 *          @c u.value points to the resized allocation (which may be the
 *          original pointer if no growth was required).
 *
 *        - @c has_value == false  
 *          @c u.error indicates the reason for failure, typically one of:
 *            - ::INVALID_ARG        — invalid freelist, size, or alignment
 *            - ::ALIGNMENT_ERROR   — requested alignment cannot be satisfied
 *            - ::CAPACITY_OVERFLOW — insufficient free space to grow
 *
 * @note
 *        This function never shrinks allocations. Requests with
 *        @p new_size <= @p old_size return the original pointer unchanged.
 *
 * @note
 *        If the original block was allocated with a weaker alignment than
 *        requested here, the returned block will necessarily move.
 *
 * @warning
 *        Any pointers returned by earlier allocations that overlap the freed
 *        block become invalid after a successful grow operation.
 *
 * @code{.c}
 * freelist_expect_t f = init_dynamic_freelist(4096, 0, false);
 * if (!f.has_value) return 1;
 *
 * freelist_t* fl = f.u.value;
 *
 * // Initial allocation with 64-byte alignment
 * void_ptr_expect_t a = alloc_freelist_aligned(fl, 128, 64, false);
 * if (!a.has_value) return 1;
 *
 * void* p = a.u.value;
 *
 * // Grow to 512 bytes, preserving alignment
 * void_ptr_expect_t r =
 *     realloc_freelist_aligned(fl, p, 128, 512, true, 64);
 *
 * if (!r.has_value) {
 *     // handle error via r.u.error
 *     return 1;
 * }
 *
 * void* q = r.u.value;
 *
 * return_freelist_element(fl, q);
 * free_freelist(fl);
 * @endcode
 *
 * @see realloc_freelist
 * @see alloc_freelist_aligned
 * @see return_freelist_element
 */
void_ptr_expect_t realloc_freelist_aligned(freelist_t* fl,
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
 * @brief Allocate memory from a freelist via allocator vtable.
 *
 * Requests a block of at least @p size bytes from the freelist associated
 * with @p ctx. On success, returns a valid pointer wrapped in
 * ::void_ptr_expect_t. On failure, returns an error code.
 *
 * @param ctx    Allocator context; must point to a valid ::freelist_t.
 * @param size   Number of bytes requested (> 0).
 * @param zeroed If true, the returned memory is zero-initialized.
 *
 * @return ::void_ptr_expect_t containing a pointer on success or an error
 *         code on failure.
 */
static inline void_ptr_expect_t
freelist_v_alloc(void *ctx, size_t size, bool zeroed) {  
    freelist_t *fl = (freelist_t *)ctx;
    if (fl == NULL) {
        return (void_ptr_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }
    if (size == 0u) {
        return (void_ptr_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    }

    /* delegate */
    return alloc_freelist(fl, size, zeroed);
}

// ----------------------------------------------------------------------------- 

/**
 * @brief Allocate aligned memory from a freelist via allocator vtable.
 *
 * Requests a block of at least @p size bytes aligned to @p align. The
 * effective alignment is validated and normalized by the freelist allocator.
 *
 * @param ctx    Allocator context; must point to a valid ::freelist_t.
 * @param size   Number of bytes requested (> 0).
 * @param align  Requested alignment in bytes (0 => freelist default).
 * @param zeroed If true, the returned memory is zero-initialized.
 *
 * @return ::void_ptr_expect_t containing a pointer on success or an error
 *         code on failure.
 */
static inline void_ptr_expect_t
freelist_v_alloc_aligned(void *ctx, size_t size, size_t align, bool zeroed) {
    freelist_t *fl = (freelist_t *)ctx;
    if (fl == NULL) {
        return (void_ptr_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }
    if (size == 0u) {
        return (void_ptr_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    }

    /* Let alloc_freelist_aligned normalize/validate alignment if that's its policy.
       If you prefer to pre-check here, add power-of-two validation + min clamp. */
    return alloc_freelist_aligned(fl, size, align, zeroed);
}

// ----------------------------------------------------------------------------- 

/**
 * @brief Reallocate a freelist allocation via allocator vtable.
 *
 * Resizes a previously allocated block. If @p old_ptr is NULL, behaves
 * like an allocation. Shrinking does not release memory.
 *
 * @param ctx       Allocator context; must point to a valid ::freelist_t.
 * @param old_ptr   Existing allocation or NULL.
 * @param old_size  Size of the existing allocation in bytes.
 * @param new_size  Requested new size in bytes (> 0).
 * @param zeroed    If true, newly added memory is zero-initialized.
 *
 * @return ::void_ptr_expect_t containing the resized pointer on success
 *         or an error code on failure.
 */
static inline void_ptr_expect_t
freelist_v_realloc(void *ctx,
                   void *old_ptr,
                   size_t old_size,
                   size_t new_size,
                   bool zeroed) {
    freelist_t *fl = (freelist_t *)ctx;
    if (fl == NULL) {
        return (void_ptr_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }

    /* Policy: new_size == 0 is invalid (mirrors your v_realloc backend). */
    if (new_size == 0u) {
        return (void_ptr_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    }

    return realloc_freelist(fl, old_ptr, old_size, new_size, zeroed);
}

// ----------------------------------------------------------------------------- 

/**
 * @brief Reallocate a freelist allocation with alignment via allocator vtable.
 *
 * Resizes a previously allocated block while enforcing a requested alignment.
 * If the block must grow, a new aligned block is allocated and the old block
 * is returned to the freelist.
 *
 * @param ctx       Allocator context; must point to a valid ::freelist_t.
 * @param old_ptr   Existing allocation or NULL.
 * @param old_size  Size of the existing allocation in bytes.
 * @param new_size  Requested new size in bytes (> 0).
 * @param zeroed    If true, newly added memory is zero-initialized.
 * @param align     Requested alignment in bytes (0 => freelist default).
 *
 * @return ::void_ptr_expect_t containing the resized pointer on success
 *         or an error code on failure.
 */
static inline void_ptr_expect_t
freelist_v_realloc_aligned(void *ctx,
                           void *old_ptr,
                           size_t old_size,
                           size_t new_size,
                           bool zeroed,
                           size_t align) {
    freelist_t *fl = (freelist_t *)ctx;
    if (fl == NULL) {
        return (void_ptr_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    }

    if (new_size == 0u) {
        return (void_ptr_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    }

    return realloc_freelist_aligned(fl, old_ptr, old_size, new_size, zeroed, align);
}

// ----------------------------------------------------------------------------- 

/**
 * @brief Return a previously allocated block to a freelist.
 *
 * Restores a block obtained from the freelist back into its free list.
 * The pointer must have been returned by this allocator.
 *
 * @param ctx Allocator context; must point to a valid ::freelist_t.
 * @param ptr Pointer previously obtained from this allocator.
 */

static inline void
freelist_v_return(void *ctx, void *ptr) {
    freelist_t *fl = (freelist_t *)ctx;
    if (fl == NULL || ptr == NULL) {
        return;
    }

    /* If your return_freelist_element currently returns void, change this wrapper
       to call it and return true. If it returns bool, propagate it. */
    return return_freelist_element(fl, ptr);
}

// ----------------------------------------------------------------------------- 

/**
 * @brief Destroy a freelist allocator via allocator vtable.
 *
 * Releases all resources owned by the freelist. If the freelist owns its
 * backing arena, the arena is destroyed as well.
 *
 * @param ctx Allocator context; must point to a valid ::freelist_t.
 */

static inline void
freelist_v_free(void *ctx) {
    freelist_t *fl = (freelist_t *)ctx;
    if (fl == NULL) {
        return;
    }

    /* If your free_freelist returns void, call it and return true. */
    return free_freelist(fl);
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Obtain an allocator vtable backed by a freelist allocator.
 *
 * Returns an ::allocator_vtable_t whose function pointers implement allocation,
 * reallocation, aligned allocation, return, and destruction operations using
 * a ::freelist_t as the underlying allocator.
 *
 * The returned vtable is stateless and does not own any memory. It may be
 * reused for multiple freelist instances; the actual allocator state is
 * provided at call time via the @c ctx parameter passed to each vtable
 * function.
 *
 * @return An ::allocator_vtable_t configured for freelist-based allocation.
 *
 * @note The caller is responsible for ensuring that the @c ctx argument passed
 *       to the vtable functions points to a valid ::freelist_t.
 *
 * @note This function does not allocate, initialize, or destroy a freelist.
 *       It only provides the function bindings required to use a freelist
 *       through the generic allocator interface.
 *
 * @warning Passing a context pointer that is not a valid ::freelist_t to any
 *          function in the returned vtable results in undefined behavior.
 *
 * @par Example
 * Using a freelist through the generic allocator interface:
 *
 * @code{.c}
 * freelist_expect_t flr = init_dynamic_freelist(4096, 0, false);
 * if (!flr.has_value) {
 *     // handle error
 *     return;
 * }
 *
 * freelist_t *fl = flr.u.value;
 *
 * allocator_vtable_t alloc = freelist_allocator();
 *
 * void_ptr_expect_t r = alloc.alloc(fl, 128, true);
 * if (!r.has_value) {
 *     // handle allocation error
 * }
 *
 * alloc.ret(fl, r.u.value);
 * alloc.free(fl);
 * @endcode
 */
static inline allocator_vtable_t
freelist_allocator(void) {
    allocator_vtable_t vt;

    vt.alloc           = freelist_v_alloc;
    vt.alloc_aligned   = freelist_v_alloc_aligned;
    vt.realloc         = freelist_v_realloc;
    vt.realloc_aligned = freelist_v_realloc_aligned;
    vt.ret             = freelist_v_return;   /* or .return_fn depending on your struct */
    vt.free            = freelist_v_free;

    return vt;
}

// ================================================================================ 
// ================================================================================ 

/**
 * @brief Allocate a heap block and return it as a ::void_ptr_expect_t.
 *
 * This is a thin wrapper around @c malloc() that returns a structured
 * ::void_ptr_expect_t result instead of a raw pointer. It provides:
 *
 *   - @c INVALID_ARG when @p size == 0.
 *   - @c BAD_ALLOC   when @c malloc() fails.
 *
 * On success, the block is optionally zero-initialized.
 *
 * The @p ctx parameter is accepted for compatibility with generic allocator
 * vtable interfaces but is ignored in this simple backend.
 *
 * @param ctx
 *     Optional allocator context (unused by this implementation).
 *
 * @param size
 *     Number of bytes to allocate. Must be greater than zero. A request of
 *     @c size == 0 is treated as invalid and returns an error expect.
 *
 * @param zeroed
 *     If true, the allocated block is fully zero-initialized via @c memset().
 *     If false, the contents are uninitialized.
 *
 * @return ::void_ptr_expect_t
 *     - On success:
 *         - @c has_value = true
 *         - @c u.value   = pointer returned by @c malloc()
 *     - On failure:
 *         - @c has_value = false
 *         - @c u.error   = one of:
 *             - ::INVALID_ARG  (size == 0)
 *             - ::BAD_ALLOC    (underlying @c malloc() failed)
 *
 * @note
 *     - The caller is responsible for eventually freeing the returned block
 *       with @c free(), or managing it according to the higher-level system
 *       that uses this backend.
 *     - This function does not track or validate ownership.
 *
 * @code{.c}
 * void_ptr_expect_t r = v_alloc(NULL, 256u, true);
 * if (!r.has_value) {
 *     fprintf(stderr, "alloc failed: %d\n", r.u.error);
 *     return;
 * }
 *
 * void *p = r.u.value;
 * // use p ...
 * free(p);
 * @endcode
 */
static inline void_ptr_expect_t v_alloc(void* ctx, size_t size, bool zeroed) {
    (void)ctx;

    if (size == 0) {
        return (void_ptr_expect_t){.has_value = false, .u.error = INVALID_ARG};
    }

    void* p = malloc(size);
    if (!p) {
        return (void_ptr_expect_t){.has_value = false, .u.error = BAD_ALLOC};
    }

    if (zeroed) {
        memset(p, 0, size);
    }
    return (void_ptr_expect_t){.has_value = true, .u.value = p};
}
// ----------------------------------------------------------------------------- 

/**
 * @brief Allocate a heap block with an alignment constraint, using ::void_ptr_expect_t.
 *
 * This function implements an alignment-aware interface atop ::v_alloc().
 * It validates the requested alignment and then delegates to ::v_alloc(),
 * relying on the guarantee that this backend provides at least
 * @c alignof(max_align_t) alignment for all allocations.
 *
 * Alignment semantics:
 *   - If @p align == 0, the effective alignment becomes @c alignof(max_align_t).
 *   - If @p align > alignof(max_align_t), the request is rejected.
 *   - If @p align is non-power-of-two, the request is rejected.
 *
 * Because this simple backend does not provide actual over-aligned allocations,
 * any request with @p align <= alignof(max_align_t) is treated as satisfied
 * by a normal ::v_alloc() call.
 *
 * @param ctx
 *     Optional allocator context (unused by this implementation).
 *
 * @param size
 *     Number of bytes to allocate. Must be greater than zero. A request of
 *     @c size == 0 yields an ::INVALID_ARG error.
 *
 * @param align
 *     Requested alignment in bytes. May be:
 *       - 0               → use @c alignof(max_align_t)
 *       - power-of-two ≤ alignof(max_align_t)
 *
 *     Any value > @c alignof(max_align_t) or non-power-of-two produces
 *     ::ALIGNMENT_ERROR.
 *
 * @param zeroed
 *     If true, the allocated block is fully zero-initialized; otherwise its
 *     contents are uninitialized.
 *
 * @return ::void_ptr_expect_t
 *     - On success:
 *         - @c has_value = true
 *         - @c u.value   = pointer returned by ::v_alloc()
 *     - On failure:
 *         - @c has_value = false
 *         - @c u.error   = one of:
 *             - ::INVALID_ARG     (size == 0)
 *             - ::ALIGNMENT_ERROR (invalid or overly strict alignment)
 *             - ::BAD_ALLOC       (underlying ::v_alloc / @c malloc failed)
 *
 * @note
 *     - This backend does not provide “true” over-aligned allocations; it only
 *       enforces that @p align does not demand more than @c alignof(max_align_t).
 *     - Ownership and eventual @c free() are the caller’s responsibility.
 *
 * @code{.c}
 * // Request 16-byte alignment, which is ≤ alignof(max_align_t) on many ABIs.
 * void_ptr_expect_t r = v_alloc_aligned(NULL, 128u, 16u, true);
 * if (!r.has_value) {
 *     fprintf(stderr, "aligned alloc failed: %d\n", r.u.error);
 *     return;
 * }
 * void *p = r.u.value;
 * // use p ...
 * free(p);
 * @endcode
 */
static inline void_ptr_expect_t v_alloc_aligned(void *ctx,
                                                size_t size,
                                                size_t align,
                                                bool   zeroed) {
    /* Validate size */
    if (size == 0u) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = INVALID_ARG
        };
    }

    /* Normalize alignment: 0 → alignof(max_align_t) */
    size_t eff_align = (align != 0u) ? align : alignof(max_align_t);

    /* For this backend, refuse alignments stricter than max_align_t */
    if (eff_align > alignof(max_align_t)) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = ALIGNMENT_ERROR
        };
    }

    /* Require power-of-two alignment (defensive) */
    if ((eff_align & (eff_align - 1u)) != 0u) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = ALIGNMENT_ERROR
        };
    }

    /* Delegate to the underlying allocator.
       This backend guarantees at least alignof(max_align_t), so any
       eff_align <= alignof(max_align_t) is satisfied. */
    void_ptr_expect_t expect = v_alloc(ctx, size, zeroed);
    return expect;  /* propagate success or error unchanged */
}
// -------------------------------------------------------------------------------- 

/**
 * @brief Resize a heap block using realloc semantics, returning ::void_ptr_expect_t.
 *
 * This is a thin wrapper around @c realloc() that reports success or failure
 * via ::void_ptr_expect_t and preserves the usual C realloc rules, with one
 * important difference: a @p new_size of zero is treated as an error
 * (::INVALID_ARG), not as a “free” request.
 *
 * Semantics:
 *   - If @p new_size == 0:
 *       → return error expect (::INVALID_ARG).
 *   - If @p old_ptr == NULL:
 *       → behave like an allocation and delegate to ::v_alloc().
 *   - Otherwise:
 *       → call @c realloc(old_ptr, new_size).
 *         - On success: optionally zero-fill the tail if the block grew and
 *           @p zeroed is true.
 *         - On failure: return ::BAD_ALLOC and leave @p old_ptr valid.
 *
 * @param ctx
 *     Optional allocator context (unused by this implementation).
 *
 * @param old_ptr
 *     Pointer to an existing allocation obtained from this backend, or NULL to
 *     request a fresh allocation. Passing a foreign pointer yields undefined
 *     behavior (this function does not validate ownership).
 *
 * @param old_size
 *     Previous size of the allocation in bytes. Used only to determine the
 *     tail region to zero when @p zeroed is true and the block grows. Passing
 *     an incorrect size leads to undefined behavior (e.g., memset past end).
 *
 * @param new_size
 *     Requested new size in bytes. Must be > 0. If greater than @p old_size,
 *     the block may grow; if smaller, it may shrink in place.
 *
 * @param zeroed
 *     If true and @p new_size > @p old_size and reallocation succeeds, the
 *     newly added region [@p old_size, @p new_size) is zero-filled.
 *
 * @return ::void_ptr_expect_t
 *     - On success:
 *         - @c has_value = true
 *         - @c u.value   = the (possibly moved) pointer returned by @c realloc(),
 *                         or a freshly allocated pointer if @p old_ptr == NULL.
 *     - On failure:
 *         - @c has_value = false
 *         - @c u.error   = one of:
 *             - ::INVALID_ARG (new_size == 0)
 *             - ::BAD_ALLOC   (underlying @c realloc() or ::v_alloc() failed)
 *
 * @note
 *     - On @c BAD_ALLOC from @c realloc(), the original @p old_ptr remains
 *       valid and is not freed.
 *     - This function does not attempt to free memory on failure.
 *     - Ownership and eventual @c free() remain the caller’s responsibility.
 *
 * @code{.c}
 * void_ptr_expect_t r = v_alloc(NULL, 64u, false);
 * if (!r.has_value) { // handle error }
 * void *p = r.u.value;
 *
 * void_ptr_expect_t rr = v_realloc(NULL, p, 64u, 128u, true);
 * if (!rr.has_value) {
 *     fprintf(stderr, "realloc failed: %d\n", rr.u.error);
 *     // p is still valid here
 * } else {
 *     p = rr.u.value;
 * }
 * @endcode
 */
static inline void_ptr_expect_t v_realloc(void *ctx,
                                          void *old_ptr,
                                          size_t old_size,
                                          size_t new_size,
                                          bool  zeroed)
{
    /* Reject zero-size realloc requests explicitly */
    if (new_size == 0u) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = INVALID_ARG
        };
    }

    /* If old_ptr is NULL, this behaves like an allocation. */
    if (!old_ptr) {
        /* Delegate to v_alloc so we stay within the same backend policy. */
        return v_alloc(ctx, new_size, zeroed);
    }

    /* Standard realloc semantics for a non-NULL old_ptr. */
    void *p = realloc(old_ptr, new_size);
    if (!p) {
        /* realloc failed; old_ptr is still valid, caller decides what to do */
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = BAD_ALLOC
        };
    }

    /* Zero-fill the extended region if requested and we actually grew. */
    if (zeroed && new_size > old_size) {
        memset((uint8_t *)p + old_size, 0, new_size - old_size);
    }

    return (void_ptr_expect_t){
        .has_value = true,
        .u.value   = p
    };
}
// ----------------------------------------------------------------------------- 

/**
 * @brief Resize a heap block with an alignment requirement, returning ::void_ptr_expect_t.
 *
 * This function combines the semantics of ::v_realloc() with the alignment
 * validation rules of ::v_alloc_aligned(). Because this simple backend does
 * not provide true over-aligned allocations, any alignment requirement is
 * restricted to @c alignof(max_align_t).
 *
 * Alignment semantics:
 *   - If @p align == 0, the effective alignment is @c alignof(max_align_t).
 *   - If @p align is non-zero and not a power-of-two, the call fails with
 *     ::ALIGNMENT_ERROR.
 *   - If @p align (after normalization) is greater than @c alignof(max_align_t),
 *     the call fails with ::ALIGNMENT_ERROR.
 *
 * Reallocation semantics:
 *   - If @p old_ptr == NULL:
 *       → behave like ::v_alloc_aligned(ctx, new_size, eff_align, zeroed).
 *   - Otherwise:
 *       → behave like ::v_realloc(ctx, old_ptr, old_size, new_size, zeroed).
 *
 * The alignment check is performed up front; no attempt is made to use
 * platform-specific over-aligned allocation APIs.
 *
 * @param ctx
 *     Optional allocator context (currently unused).
 *
 * @param old_ptr
 *     Pointer to an existing allocation, or NULL to request an aligned
 *     allocation. Foreign pointers produce undefined behavior.
 *
 * @param old_size
 *     Previous size in bytes of @p old_ptr, used to determine how much of the
 *     block may be zero-filled on growth. Must be accurate when @p old_ptr is
 *     non-NULL.
 *
 * @param new_size
 *     Requested new size in bytes. For the @p old_ptr != NULL case, the
 *     behavior is that of ::v_realloc(). For @p old_ptr == NULL, it is passed
 *     through to ::v_alloc_aligned().
 *
 * @param zeroed
 *     If true and @p new_size > @p old_size in the @p old_ptr != NULL case,
 *     the extended region is zero-filled. For the allocation case
 *     (@p old_ptr == NULL), the entire block is zeroed if true.
 *
 * @param align
 *     Requested alignment in bytes. May be:
 *       - 0               → effective alignment = @c alignof(max_align_t)
 *       - power-of-two ≤ alignof(max_align_t)
 *
 * @return ::void_ptr_expect_t
 *     - On success:
 *         - @c has_value = true
 *         - @c u.value   = allocation/reallocation result pointer
 *     - On failure:
 *         - @c has_value = false
 *         - @c u.error   = one of:
 *             - ::ALIGNMENT_ERROR (invalid or overly strict alignment)
 *             - ::INVALID_ARG     (from ::v_realloc when new_size == 0)
 *             - ::BAD_ALLOC       (allocation or reallocation failure)
 *
 * @note
 *     - This backend does not provide real over-aligned allocations; it only
 *       enforces that requested alignments do not exceed @c alignof(max_align_t).
 *     - Ownership and @c free() are still the caller’s responsibility.
 *
 * @code{.c}
 * // Allocate aligned
 * void_ptr_expect_t a = v_realloc_aligned(NULL, NULL, 0u, 128u, true, 16u);
 * if (!a.has_value) { // handle error }
 * void *p = a.u.value;
 *
 * // Grow with the same alignment requirement
 * void_ptr_expect_t r = v_realloc_aligned(NULL, p, 128u, 256u, true, 16u);
 * if (!r.has_value) { // handle error }
 * p = r.u.value;
 *
 * free(p);
 * @endcode
 */
static inline void_ptr_expect_t v_realloc_aligned(void *ctx,
                                                  void *old_ptr,
                                                  size_t old_size,
                                                  size_t new_size,
                                                  bool   zeroed,
                                                  size_t align)
{
    /* Normalize alignment: 0 → alignof(max_align_t) */
    size_t eff_align = (align != 0u) ? align : alignof(max_align_t);

    /* For this simple backend, refuse alignments stricter than max_align_t. */
    if (eff_align > alignof(max_align_t)) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = ALIGNMENT_ERROR
        };
    }

    /* Require power-of-two alignment (defensive). */
    if ((eff_align & (eff_align - 1u)) != 0u) {
        return (void_ptr_expect_t){
            .has_value = false,
            .u.error   = ALIGNMENT_ERROR
        };
    }

    /* If old_ptr is NULL, this behaves like an aligned allocation. */
    if (!old_ptr) {
        return v_alloc_aligned(ctx, new_size, eff_align, zeroed);
    }

    /* Backend only guarantees up to max_align_t; eff_align <= max_align_t
       is already validated, so we can just reuse v_realloc. */
    return v_realloc(ctx, old_ptr, old_size, new_size, zeroed);
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
 * @brief Initialize an arena_t backed by a region allocated from a buddy allocator.
 *
 * This function creates an arena whose entire lifetime and storage are contained inside
 * a *single allocation* obtained from the provided @p buddy allocator. The allocated
 * region is used to store:
 *
 *   - the @c arena_t header,
 *   - exactly one @c Chunk header, and
 *   - the arena’s data region aligned to @p base_align_in.
 *
 * The returned arena pointer is:
 *
 *   - the address of the arena_t object itself, and
 *   - **exactly equal** to the pointer returned by @c alloc_buddy().
 *
 * This is important because the backing memory must later be released via
 * ::return_arena_with_buddy(), which internally calls @c return_buddy_element()
 * using the arena pointer.
 *
 * @param buddy          Valid buddy allocator instance that provides the backing memory.
 * @param bytes          Total size of the sub-region to reserve from @p buddy.
 *                       This includes the arena header, the Chunk header, and all
 *                       aligned/usable data space.
 * @param base_align_in  Desired per-arena base alignment for allocations made via
 *                       ::alloc_arena(). If zero, defaults to @c alignof(max_align_t).
 *
 * @return An ::arena_expect_t describing success or failure.
 *
 * @par Successful Result
 * On success:
 *   - @c result.has_value == true
 *   - @c result.u.value   points to a fully constructed @c arena_t located inside
 *     the buddy-allocated region.
 *
 * The initialized arena has:
 *   - @c mem_type    = DYNAMIC        (memory originates from a dynamic source)
 *   - @c resize      = 0              (fixed-capacity sub-arena, no growth)
 *   - @c owns_memory = 0              (buddy allocator owns the backing memory)
 *   - @c alloc       = usable aligned data bytes
 *   - @c tot_alloc   = @p bytes
 *
 * @par Failure Result
 * On failure:
 *   - @c result.has_value == false
 *   - @c result.u.error   contains an appropriate ::ErrorCode.
 *
 * Possible error codes:
 *   - @c NULL_POINTER       — @p buddy is NULL.
 *   - @c INVALID_ARG        — @p bytes is zero, or region too small to hold
 *                             @c arena_t + @c Chunk + at least one byte of data.
 *   - @c ALIGNMENT_ERROR    — @p base_align_in cannot be normalized to a valid
 *                             power-of-two alignment ≥ @c alignof(max_align_t).
 *   - @c LENGTH_OVERFLOW    — arithmetic overflow detected when computing layout.
 *   - @c OUT_OF_MEMORY      — @c alloc_buddy() could not satisfy the request.
 *
 * If an allocation was obtained from the buddy allocator but a subsequent
 * layout/alignment check fails, the region is **returned automatically** to
 * @c buddy via @c return_buddy_element() before the error is returned.
 *
 * @warning
 * Do **not** call @c free_arena() on an arena created by this function.
 * Release it only through ::return_arena_with_buddy(), which returns the entire
 * sub-region to the buddy allocator.
 *
 * @sa alloc_buddy(), return_buddy_element(), return_arena_with_buddy(),
 *     init_dynamic_arena(), init_static_arena()
 *
 * @par Example
 * @code{.c}
 * buddy_t *b = init_buddy_allocator(4096u, 64u, alignof(max_align_t));
 * if (!b) {
 *     // handle error
 * }
 *
 * arena_expect_t r = init_arena_with_buddy(b, 1024u, alignof(max_align_t));
 * if (!r.has_value) {
 *     ErrorCode ec = r.u.error;
 *     // handle error
 * }
 *
 * arena_t *a = r.u.value;
 *
 * void *p = alloc_arena(a, 128u, true).u.value;
 * assert(p != NULL);
 *
 * // When finished with this arena:
 * (void)return_arena_with_buddy(a, b);
 *
 * free_buddy(b);
 * @endcode
 */
arena_expect_t init_arena_with_buddy(buddy_t *buddy,
                                     size_t   bytes,
                                     size_t   base_align_in);
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
 * Failure is only apprant based in false value return.  Possible failure 
 * reasons are the following 
 *
 *   - NULL pointers for arena or buddy passed to function 
 *   - The arena does not own its memory 
 *   - The arena structure is not a pointer within the buddy memory allocation
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
// static inline void* buddy_v_realloc(void* ctx, void* old_ptr,
//                                     size_t old_size, size_t new_size,
//                                     bool zeroed) {
//     buddy_t* buddy = (buddy_t*)ctx;
//     if (!buddy) {
//         errno = EINVAL;
//         return NULL;
//     }
//     return realloc_buddy(buddy, old_ptr, old_size, new_size, zeroed);
// }
// // --------------------------------------------------------------------------------
//
// /**
//  * @brief Vtable adapter for aligned buddy reallocation.
//  *
//  * Resizes a block of memory previously allocated from the buddy allocator
//  * referenced by @p ctx, enforcing a minimum alignment of @p align for the
//  * resulting block. The previous allocation is described by @p old_ptr and
//  * @p old_size; the new requested size is @p new_size.
//  *
//  * Semantics follow ::realloc_buddy_aligned():
//  *
//  *  - `realloc_buddy_aligned(b, NULL, 0, new_size, align, zeroed)` behaves
//  *    like ::alloc_buddy_aligned().
//  *  - `realloc_buddy_aligned(b, old_ptr, old_size, 0, align, zeroed)` frees
//  *    @p old_ptr and returns NULL.
//  *  - If the new size fits in the existing block and the pointer already
//  *    satisfies the requested alignment, the pointer is reused in place.
//  *  - Otherwise, a new aligned block is allocated, data is copied, and the
//  *    old block is returned to the allocator.
//  *
//  * On error, errno is set and NULL is returned. In that case, the caller must
//  * continue to use @p old_ptr unchanged.
//  *
//  * This function implements the @ref realloc_aligned_prototype interface for
//  * buddy-backed allocators.
//  *
//  * @param ctx       Pointer to a buddy_t instance.
//  * @param old_ptr   Pointer to the existing allocation (may be NULL).
//  * @param old_size  Size of the existing allocation.
//  * @param new_size  Requested new size.
//  * @param zeroed    Whether any expanded portion must be zero-initialized.
//  * @param align     Required alignment (power of two, or 0 for natural).
//  *
//  * @retval void* Pointer to the resized, aligned allocation on success.
//  * @retval NULL  On failure, with errno set (caller keeps @p old_ptr).
//  */
// static inline void* buddy_v_realloc_aligned(void* ctx, void* old_ptr,
//                                             size_t old_size, size_t new_size,
//                                             bool zeroed, size_t align) {
//     buddy_t* buddy = (buddy_t*)ctx;
//     if (!buddy) {
//         errno = EINVAL;
//         return NULL;
//     }
//     return realloc_buddy_aligned(buddy, old_ptr, old_size,
//                                  new_size, align, zeroed);
// }
// // --------------------------------------------------------------------------------
//
// /**
//  * @brief Vtable adapter for returning an element to a buddy allocator.
//  *
//  * Returns a block previously allocated from the buddy allocator referenced
//  * by @p ctx back to the allocator. This is a thin adapter over
//  * ::return_buddy_element().
//  *
//  * If @p ctx is NULL, errno is set to EINVAL and the function returns without
//  * performing any deallocation. If @p ptr is NULL, the call is treated as
//  * a no-op (like `free(NULL)`).
//  *
//  * This function implements the @ref return_prototype interface for
//  * buddy-backed allocators.
//  *
//  * @param ctx Pointer to a buddy_t instance.
//  * @param ptr Pointer to an allocated block (may be NULL).
//  */
// static inline void buddy_v_return(void* ctx, void* ptr) {
//     buddy_t* buddy = (buddy_t*)ctx;
//     if (!buddy) {
//         errno = EINVAL;
//         return;
//     }
//     (void)return_buddy_element(buddy, ptr);
// }
// // --------------------------------------------------------------------------------
//
// /**
//  * @brief Vtable adapter for freeing a buddy allocator.
//  *
//  * Releases all memory owned by the buddy allocator referenced by @p ctx,
//  * including the OS-backed pool and all internal metadata. After this call,
//  * the allocator must not be used again.
//  *
//  * If @p ctx is NULL, errno is set to EINVAL and the function returns
//  * without performing any action.
//  *
//  * This function implements the @ref free_prototype interface for
//  * buddy-backed allocators.
//  *
//  * @param ctx Pointer to a buddy_t instance.
//  */
// static inline void buddy_v_free(void* ctx) {
//     buddy_t* buddy = (buddy_t*)ctx;
//     if (!buddy) {
//         errno = EINVAL;
//         return;
//     }
//     free_buddy(buddy);
// }
// // --------------------------------------------------------------------------------
//
// /**
//  * @brief Constructs an allocator_vtable_t for a given buddy allocator.
//  *
//  * This helper initializes an allocator_vtable_t that exposes a ::buddy_t
//  * instance through the generic allocator interface. All operations
//  * (allocate, reallocate, deallocate, etc.) are forwarded to the
//  * buddy-backed vtable adapter functions.
//  *
//  * The returned vtable is typically used to parameterize other components
//  * that depend on the generic allocator API rather than directly on
//  * ::buddy_t.
//  *
//  * @param b Pointer to a buddy_t instance to use as the allocator backend.
//  *
//  * @return A fully initialized allocator_vtable_t bound to @p b.
//  */
// static inline allocator_vtable_t buddy_allocator(buddy_t* b) {
//     allocator_vtable_t v = {
//         .allocate           = buddy_v_alloc,
//         .allocate_aligned   = buddy_v_alloc_aligned,
//         .reallocate         = buddy_v_realloc,
//         .reallocate_aligned = buddy_v_realloc_aligned,
//         .return_element     = buddy_v_return,
//         .deallocate         = buddy_v_free,
//         .ctx                = b
//     };
//     return v;
// }
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
// static inline void *slab_v_alloc(void *ctx, size_t size, bool zeroed) {
//     slab_t *slab = (slab_t *)ctx;
//     if (!slab) {
//         errno = EINVAL;
//         return NULL;
//     }
//
//     if (size == 0u) {
//         errno = EINVAL;
//         return NULL;
//     }
//
//     /* Maximum capacity per slot (exposed via getter). */
//     size_t const capacity = slab_stride(slab);
//     if (capacity == 0u) {
//         /* slab_stride() already sets errno appropriately. */
//         if (errno == 0) errno = EINVAL;
//         return NULL;
//     }
//
//     if (size > capacity) {
//         errno = ENOMEM;
//         return NULL;
//     }
//
//     return alloc_slab(slab, zeroed);
// }
// // -------------------------------------------------------------------------------- 
//
// /**
//  * @brief Vtable adapter for aligned slab allocation.
//  *
//  * Allocates a fixed-size object from the slab allocator referenced by @p ctx,
//  * with a minimum alignment requirement of @p align. The slab's alignment is
//  * exposed via ::slab_alignment(); if @p align is 0, the slab's natural
//  * alignment is used, otherwise @p align must not exceed the slab alignment.
//  *
//  * The requested @p size must be greater than 0 and less than or equal to the
//  * slot stride reported by ::slab_stride().
//  *
//  * On error, errno is set (typically to ENOMEM or EINVAL) and NULL is returned.
//  *
//  * @param ctx    Pointer to a slab_t instance.
//  * @param size   Number of bytes requested (must be <= ::slab_stride()).
//  * @param align  Required alignment (0 for natural, or <= ::slab_alignment()).
//  * @param zeroed Whether the memory should be zero-initialized.
//  *
//  * @retval void* Pointer to an aligned block of at least @p size bytes.
//  * @retval NULL  On failure, with errno set.
//  */
// static inline void *slab_v_alloc_aligned(void *ctx, size_t size,
//                                          size_t align, bool zeroed) {
//     slab_t *slab = (slab_t *)ctx;
//     if (!slab) {
//         errno = EINVAL;
//         return NULL;
//     }
//
//     if (size == 0u) {
//         errno = EINVAL;
//         return NULL;
//     }
//
//     size_t const capacity = slab_stride(slab);
//     if (capacity == 0u) {
//         if (errno == 0) errno = EINVAL;
//         return NULL;
//     }
//     if (size > capacity) {
//         errno = ENOMEM;
//         return NULL;
//     }
//
//     size_t const slab_align = slab_alignment(slab);
//     if (slab_align == 0u) {
//         if (errno == 0) errno = EINVAL;
//         return NULL;
//     }
//
//     if (align != 0u && align > slab_align) {
//         errno = EINVAL;
//         return NULL;
//     }
//
//     /* All slab allocations are at least slab_align-aligned, so they satisfy
//        any requested align <= slab_align. */
//     return alloc_slab(slab, zeroed);
// }
// // -------------------------------------------------------------------------------- 
//
// /**
//  * @brief Vtable adapter for slab reallocation (not supported).
//  *
//  * Slab allocators manage fixed-size objects and do not support resizing
//  * individual allocations. This adapter exists only to satisfy the
//  * @ref realloc_prototype interface; it does not perform any reallocation.
//  *
//  * Semantics:
//  *
//  *  - If @p old_ptr is NULL, the function returns NULL and performs no action.
//  *  - If @p old_ptr is non-NULL, the function simply returns @p old_ptr
//  *    unchanged, regardless of @p old_size, @p new_size, or @p zeroed.
//  *  - No new memory is allocated, no memory is freed, and errno is not
//  *    modified by this function.
//  *
//  * Callers must not rely on standard realloc semantics when using a slab
//  * allocator via this vtable. To change the logical size associated with
//  * an object, the caller is responsible for allocating a new object via
//  * ::slab_v_alloc() (or the underlying ::alloc_slab()) and performing any
//  * data copying and deallocation explicitly.
//  *
//  * @param ctx       Pointer to a slab_t instance (unused).
//  * @param old_ptr   Pointer to the existing allocation (may be NULL).
//  * @param old_size  Logical size of the existing allocation (ignored).
//  * @param new_size  Requested new logical size (ignored).
//  * @param zeroed    Whether any expanded portion must be zero-initialized
//  *                  (ignored).
//  *
//  * @retval void* @p old_ptr, unchanged. May be NULL if @p old_ptr was NULL.
//  */
// static inline void *slab_v_realloc(void *ctx, void *old_ptr,
//                                    size_t old_size, size_t new_size,
//                                    bool zeroed) {
//     (void)ctx; (void)old_size; (void)new_size; (void)zeroed;
//     return old_ptr;
// }
// // -------------------------------------------------------------------------------- 
//
// /**
//  * @brief Vtable adapter for aligned slab reallocation (not supported).
//  *
//  * As with ::slab_v_realloc(), slab allocators do not support resizing
//  * existing allocations, even when an explicit alignment is requested.
//  * This function exists solely to satisfy the @ref realloc_aligned_prototype
//  * interface for slab-backed allocators.
//  *
//  * Semantics:
//  *
//  *  - If @p old_ptr is NULL, the function returns NULL and performs no action.
//  *  - If @p old_ptr is non-NULL, the function simply returns @p old_ptr
//  *    unchanged, regardless of @p old_size, @p new_size, @p zeroed, or
//  *    @p align.
//  *  - No new memory is allocated, no memory is freed, alignment is not
//  *    changed, and errno is not modified by this function.
//  *
//  * Callers that require true reallocation semantics must implement them
//  * explicitly on top of the slab interface (allocate a new object of the
//  * desired size/alignment, copy data as needed, and return the old object
//  * via ::slab_v_return() / ::return_slab()).
//  *
//  * @param ctx       Pointer to a slab_t instance (unused).
//  * @param old_ptr   Pointer to the existing allocation (may be NULL).
//  * @param old_size  Logical size of the existing allocation (ignored).
//  * @param new_size  Requested new size (ignored).
//  * @param zeroed    Whether any expanded portion must be zero-initialized
//  *                  (ignored).
//  * @param align     Required alignment (ignored).
//  *
//  * @retval void* @p old_ptr, unchanged. May be NULL if @p old_ptr was NULL.
//  */
// static inline void *slab_v_realloc_aligned(void *ctx, void *old_ptr,
//                                            size_t old_size, size_t new_size,
//                                            bool zeroed, size_t align) {
//     (void)ctx; (void)old_size; (void)new_size; (void)zeroed; (void)align;
//     return old_ptr;
// }
// // -------------------------------------------------------------------------------- 
//
// /**
//  * @brief Vtable adapter for returning an element to a slab allocator.
//  *
//  * Returns a block previously allocated from the slab allocator referenced
//  * by @p ctx back to the allocator. This is a thin adapter over ::return_slab().
//  *
//  * If @p ctx is NULL, errno is set to EINVAL and the function returns without
//  * performing any deallocation. If @p ptr is NULL, the call is treated as
//  * a no-op (like `free(NULL)`).
//  *
//  * @param ctx Pointer to a slab_t instance.
//  * @param ptr Pointer to an allocated block (may be NULL).
//  */
// static inline void slab_v_return(void *ctx, void *ptr) {
//     slab_t *slab = (slab_t *)ctx;
//     if (!slab) {
//         errno = EINVAL;
//         return;
//     }
//     (void)return_slab(slab, ptr);
// }
// // -------------------------------------------------------------------------------- 
//
// /**
//  * @brief Vtable adapter for logically resetting a slab allocator.
//  *
//  * For slab-backed allocators, the slab does not own its underlying memory
//  * pool; instead, it carves fixed-size objects from a backing allocator.
//  * As such, this function does not release the underlying pool; it simply
//  * resets the slab's internal state via ::reset_slab(), returning all
//  * objects to the free list.
//  *
//  * After this call, the slab remains valid and may be used again.
//  *
//  * If @p ctx is NULL, errno is set to EINVAL and the function returns
//  * without performing any action.
//  *
//  * @param ctx Pointer to a slab_t instance.
//  */
// static inline void slab_v_free(void *ctx) {
//     slab_t *slab = (slab_t *)ctx;
//     if (!slab) {
//         errno = EINVAL;
//         return;
//     }
//     (void)reset_slab(slab);
// }
// // -------------------------------------------------------------------------------- 
//
// /**
//  * @brief Constructs an allocator_vtable_t for a given slab allocator.
//  *
//  * This helper initializes an allocator_vtable_t that exposes a ::slab_t
//  * instance through the generic allocator interface. All operations
//  * (allocate, reallocate, deallocate, etc.) are forwarded to the
//  * slab-backed vtable adapter functions.
//  *
//  * Because a slab has fixed slot capacity and alignment, allocation and
//  * reallocation requests are subject to additional constraints:
//  *
//  *  - The requested size must be > 0 and <= ::slab_stride().
//  *  - The requested alignment must be 0 or <= ::slab_alignment().
//  *  - Reallocation cannot grow blocks beyond the slot capacity; attempts
//  *    to do so fail with errno = ENOMEM and return NULL (the caller keeps
//  *    the original pointer).
//  *
//  * @param slab Pointer to a slab_t instance to use as the allocator backend.
//  *
//  * @return A fully initialized allocator_vtable_t bound to @p slab.
//  */
// static inline allocator_vtable_t slab_allocator(slab_t *slab) {
//     allocator_vtable_t v = {
//         .allocate           = slab_v_alloc,
//         .allocate_aligned   = slab_v_alloc_aligned,
//         .reallocate         = slab_v_realloc,
//         .reallocate_aligned = slab_v_realloc_aligned,
//         .return_element     = slab_v_return,
//         .deallocate         = slab_v_free,
//         .ctx                = slab
//     };
//     return v;
// }
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
// TODO: Made it to realloc_arena with updates 
// TODO: Complete arena with new data types 
