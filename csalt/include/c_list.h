// ================================================================================
// ================================================================================
// - File:    c_list.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    March 30, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_list_H
#define c_list_H

#include "c_allocator.h"
#include "c_error.h"
#include "c_dtypes.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
// ================================================================================ 
// ================================================================================ 

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

// ================================================================================
// Node — inline data, no second pointer dereference
// ================================================================================
 
/**
 * @brief A singly-linked list node with its value stored inline.
 *
 * Each node occupies exactly @c sizeof(sNode) + @c data_size bytes.
 * Within the pre-allocated slab these nodes are laid out contiguously at
 * a fixed stride, which improves cache behaviour during traversal because
 * sequential @c ->next dereferences are likely to hit the same or adjacent
 * cache lines.  Overflow nodes allocated individually do not carry this
 * guarantee.
 *
 * The flexible array member @c data holds the value; never access it
 * directly — use @c memcpy through the push/get API.
 */
typedef struct sNode {
    struct sNode* next;
    uint8_t       data[];   /**< Inline value buffer — sizeof(sNode) + data_size bytes total */
} sNode;
 
// ================================================================================
// List
// ================================================================================
 
/**
 * @brief A singly-linked list backed by a pre-allocated contiguous node slab.
 *
 * This is a pure linked list.  All operations traverse the @c ->next chain
 * and the list makes no assumptions about physical node order or adjacency.
 * The slab is an allocation optimisation only: pre-allocating @c slab_cap
 * nodes in a single contiguous block reduces allocator pressure and improves
 * cache behaviour during sequential traversal, because nodes handed out from
 * the slab in push order tend to be physically adjacent in memory.
 *
 * Once the slab is exhausted, behaviour is controlled by @c allow_overflow:
 *
 *   - @c true  — fall back to individual allocator calls.  Cache locality is
 *                no longer guaranteed beyond the slab boundary.  The caller
 *                may migrate to a new, larger list to restore locality.
 *   - @c false — any push beyond @c slab_cap returns CAPACITY_OVERFLOW and
 *                leaves the list unchanged.
 *
 * Do not manipulate any fields directly; use the provided API.
 */
typedef struct {
    // -------------------------------------------------------------------------
    // Core list structure (hot path — accessed most frequently)
    // -------------------------------------------------------------------------
    sNode* head;            /**< First node, or NULL if empty.                     */
    sNode* tail;            /**< Last node, or NULL if empty.                      */

    // -------------------------------------------------------------------------
    // Slab memory management (also hot path)
    // -------------------------------------------------------------------------
    uint8_t* slab;          /**< Contiguous slab storage for primary nodes.        */
    sNode*   slab_free;     /**< Recycled slab nodes available for reuse.          */

    // -------------------------------------------------------------------------
    // Size and capacity tracking
    // -------------------------------------------------------------------------
    size_t len;             /**< Number of live nodes currently in the list.       */
    size_t slab_cap;        /**< Total number of nodes the slab can hold.          */
    size_t slab_used;       /**< Fresh slab slots ever consumed.                  */
    size_t slab_free_count; /**< Number of nodes on slab_free.                    */
    size_t overflow_live;   /**< Number of currently live overflow nodes.         */

    // -------------------------------------------------------------------------
    // Data description
    // -------------------------------------------------------------------------
    size_t     data_size;   /**< Bytes per element stored in each node.           */
    dtype_id_t dtype;       /**< Type identifier (UINT8_TYPE, FLOAT_TYPE, etc.). */

    // -------------------------------------------------------------------------
    // Configuration flags
    // -------------------------------------------------------------------------
    bool allow_overflow;    /**< true = allocator fallback, false = hard cap.     */

    // -------------------------------------------------------------------------
    // Allocator interface
    // -------------------------------------------------------------------------
    allocator_vtable_t alloc_v; /**< Allocator used for slab and overflow nodes. */
} slist_t;
// ================================================================================
// Expected return types
// ================================================================================
 
/**
 * @brief Expected return type for @ref init_slist and @ref copy_slist.
 *
 * Check @c has_value before using @c u.value.  On failure @c u.error
 * contains the relevant @ref error_code_t.
 */
typedef struct {
    bool has_value;
    union {
        slist_t*     value;
        error_code_t error;
    } u;
} slist_expect_t;
 
/**
 * @brief Expected return type for @ref contains_slist.
 *
 * On success @c u.value is the zero-based index of the first matching node.
 * On failure @c u.error is NOT_FOUND or another error code.
 */
typedef struct {
    bool has_value;
    union {
        size_t       value;
        error_code_t error;
    } u;
} slist_index_expect_t;
 
// ================================================================================
// Iterator type
// ================================================================================
 
/**
 * @brief Callback type for @ref foreach_slist.
 *
 * @param value      Read-only pointer to the node's inline value buffer
 *                   (@c data_size bytes).  Do not store this pointer — it
 *                   points into internal node storage and must not be used
 *                   after the callback returns.
 * @param index      Zero-based position of this node in the list.
 * @param user_data  Caller-supplied context pointer; may be NULL.
 */
typedef void (*slist_iter_fn)(const void* value, size_t index, void* user_data);
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
/**
 * @brief Allocate and initialise a singly-linked list with a pre-allocated
 *        node slab.
 *
 * All @p num_nodes nodes are allocated as a single contiguous block of
 * @c num_nodes * _node_stride(data_size) bytes.  This reduces allocator
 * pressure and improves cache behaviour for sequential traversal when nodes
 * are pushed in order.  All traversal is performed via @c ->next pointers.
 *
 * @param num_nodes      Number of nodes to pre-allocate.  Must be > 0.
 * @param data_size      Size in bytes of each stored value.  Must be > 0.
 * @param dtype          Type tag identifying the stored type.
 * @param allow_overflow If true, pushes beyond @p num_nodes fall back to
 *                       individual allocator calls.  If false, returns
 *                       CAPACITY_OVERFLOW.
 * @param alloc_v        Allocator.  @c alloc_v.allocate must not be NULL.
 *
 * @return @ref slist_expect_t with @c has_value true on success.
 *
 * @code
 *     allocator_vtable_t a = heap_allocator();
 *     slist_expect_t r = init_slist(64, sizeof(float), FLOAT_TYPE, false, a);
 *     if (!r.has_value) { // handle r.u.error }
 *     slist_t* l = r.u.value;
 *     float v = 3.14f;
 *     push_back_slist(l, &v);
 *     return_slist(l);
 * @endcode
 */
slist_expect_t init_slist(size_t             num_nodes,
                          size_t             data_size,
                          dtype_id_t         dtype,
                          bool               allow_overflow,
                          allocator_vtable_t alloc_v);
 
/**
 * @brief Release all memory owned by the list.
 *
 * Overflow nodes are returned individually via @c alloc_v.return_element.
 * The slab and list header are returned in single calls.
 * Passing NULL is safe and performs no action.
 *
 * @param list  List to release.
 */
void return_slist(slist_t* list);
 
// ================================================================================
// Get and peek
// ================================================================================
 
/**
 * @brief Copy the value at zero-based @p index into @p out_value.
 *
 * O(n) — traverses the @c ->next chain from @c head.
 *
 * @param list       Must not be NULL.
 * @param index      Zero-based position (0 … list->len - 1).
 * @param out_value  Destination buffer of at least @c data_size bytes.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND (index out of range).
 */
error_code_t get_slist(const slist_t* list, size_t index, void* out_value);
 
/**
 * @brief Copy the value at the head without removing the node.
 *
 * O(1).
 *
 * @param list       Must not be NULL.
 * @param out_value  Destination buffer of at least @c data_size bytes.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND (list is empty).
 */
error_code_t peek_front_slist(const slist_t* list, void* out_value);
 
/**
 * @brief Copy the value at the tail without removing the node.
 *
 * O(1).
 *
 * @param list       Must not be NULL.
 * @param out_value  Destination buffer of at least @c data_size bytes.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND (list is empty).
 */
error_code_t peek_back_slist(const slist_t* list, void* out_value);
 
// ================================================================================
// Push operations
// ================================================================================
 
/**
 * @brief Append a copy of @p value to the tail.
 *
 * O(1).
 *
 * @param list   Must not be NULL.
 * @param value  Pointer to exactly @c data_size bytes.  Must not be NULL.
 *
 * @return NO_ERROR, NULL_POINTER, CAPACITY_OVERFLOW, or OUT_OF_MEMORY.
 */
error_code_t push_back_slist(slist_t* list, const void* value);
 
/**
 * @brief Prepend a copy of @p value to the head.
 *
 * O(1).
 *
 * @param list   Must not be NULL.
 * @param value  Pointer to exactly @c data_size bytes.  Must not be NULL.
 *
 * @return NO_ERROR, NULL_POINTER, CAPACITY_OVERFLOW, or OUT_OF_MEMORY.
 */
error_code_t push_front_slist(slist_t* list, const void* value);
 
/**
 * @brief Insert a copy of @p value before the node at zero-based @p index.
 *
 * O(n).  @p index == 0 delegates to @ref push_front_slist.
 * @p index == @c list->len delegates to @ref push_back_slist.
 *
 * @param list   Must not be NULL.
 * @param index  Zero-based insertion position (0 … list->len inclusive).
 * @param value  Pointer to exactly @c data_size bytes.  Must not be NULL.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, CAPACITY_OVERFLOW,
 *         or OUT_OF_MEMORY.
 */
error_code_t push_at_slist(slist_t* list, size_t index, const void* value);
 
// ================================================================================
// Pop operations
// ================================================================================
 
/**
 * @brief Remove the head node and optionally copy its value.
 *
 * O(1).  Pass NULL for @p out_value to discard.
 *
 * @param list       Must not be NULL.
 * @param out_value  Destination buffer of at least @c data_size bytes, or NULL.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND (list is empty).
 */
error_code_t pop_front_slist(slist_t* list, void* out_value);
 
/**
 * @brief Remove the tail node and optionally copy its value.
 *
 * O(n) — must walk to find the predecessor of the tail.
 *
 * @param list       Must not be NULL.
 * @param out_value  Destination buffer of at least @c data_size bytes, or NULL.
 *
 * @return NO_ERROR, NULL_POINTER, or NOT_FOUND (list is empty).
 */
error_code_t pop_back_slist(slist_t* list, void* out_value);
 
/**
 * @brief Remove the node at zero-based @p index and optionally copy its value.
 *
 * O(n).  Boundary cases delegate to @ref pop_front_slist and
 * @ref pop_back_slist.
 *
 * @param list       Must not be NULL.
 * @param index      Zero-based position (0 … list->len - 1).
 * @param out_value  Destination buffer of at least @c data_size bytes, or NULL.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG, or NOT_FOUND.
 */
error_code_t pop_at_slist(slist_t* list, size_t index, void* out_value);
 
// ================================================================================
// Utility operations
// ================================================================================
 
/**
 * @brief Remove all nodes and return them to the allocator, leaving the slab
 *        intact and ready for reuse.
 *
 * O(n).  Overflow nodes are returned individually via @c return_element.
 * Slab nodes are reclaimed in a single @c return_element call on the slab
 * block.  A fresh slab of the same capacity is then allocated so the list
 * can be reused immediately.  The @c head, @c tail, @c len, and
 * @c slab_used fields are all reset to zero.
 *
 * @param list  Must not be NULL.
 *
 * @return NO_ERROR, NULL_POINTER, or OUT_OF_MEMORY (fresh slab allocation
 *         failed; the list is left empty but without a slab — subsequent
 *         pushes will use overflow allocation if @c allow_overflow is true).
 *
 * @code
 *     clear_slist(l);
 *     // l is empty and ready to reuse with full slab locality
 * @endcode
 */
error_code_t clear_slist(slist_t* list);
 
/**
 * @brief Allocate a deep copy of @p src.
 *
 * All nodes are pushed into a new list with a fresh slab of the same
 * capacity as @p src's slab.  Values are copied via @c memcpy.  The new
 * list uses @p alloc_v for all allocations.
 *
 * O(n).
 *
 * @param src     Must not be NULL.
 * @param alloc_v Allocator for the new list.
 *
 * @return @ref slist_expect_t with @c has_value true on success.
 *
 * @code
 *     slist_expect_t cr = copy_slist(src, a);
 *     if (!cr.has_value) { // handle error }
 *     slist_t* dst = cr.u.value;
 *     // src and dst are independent
 *     return_slist(dst);
 * @endcode
 */
slist_expect_t copy_slist(const slist_t* src, allocator_vtable_t alloc_v);
 
/**
 * @brief Search for the first node whose value matches @p needle.
 *
 * Comparison is performed by @c memcmp over @c data_size bytes.
 * O(n).
 *
 * @param list    Must not be NULL.
 * @param needle  Pointer to exactly @c data_size bytes to search for.
 *                Must not be NULL.
 *
 * @return @ref slist_index_expect_t with @c has_value true and @c u.value
 *         set to the zero-based index of the first match.  On failure
 *         @c u.error is NULL_POINTER or NOT_FOUND.
 *
 * @code
 *     float target = 2.0f;
 *     slist_index_expect_t r = contains_slist(l, &target);
 *     if (r.has_value)
 *         printf("found at index %zu\n", r.u.value);
 * @endcode
 */
slist_index_expect_t contains_slist(const slist_t* list, const void* needle);
 
/**
 * @brief Reverse the list in-place.
 *
 * Reverses the @c ->next chain in O(n) with O(1) extra space.  No data
 * is moved or copied.  The @c head and @c tail pointers are swapped.
 *
 * @param list  Must not be NULL.
 *
 * @return NO_ERROR or NULL_POINTER.
 *
 * @code
 *     reverse_slist(l);   // [1, 2, 3] becomes [3, 2, 1]
 * @endcode
 */
error_code_t reverse_slist(slist_t* list);
 
/**
 * @brief Append every node from @p src to the tail of @p dst.
 *
 * Values are copied node-by-node via @c memcpy into newly allocated nodes
 * in @p dst.  @p src is not modified.  Both lists must have the same
 * @c data_size; if they differ INVALID_ARG is returned.
 *
 * O(n) in the length of @p src.
 *
 * @param dst     Destination list.  Must not be NULL.
 * @param src     Source list.  Must not be NULL.
 *
 * @return NO_ERROR, NULL_POINTER, INVALID_ARG (data_size mismatch),
 *         CAPACITY_OVERFLOW, or OUT_OF_MEMORY.
 *
 * @code
 *     concat_slist(dst, src);   // all of src appended to dst; src unchanged
 * @endcode
 */
error_code_t concat_slist(slist_t* dst, const slist_t* src);
 
/**
 * @brief Call @p fn once for every node in the list, in head-to-tail order.
 *
 * The callback receives a read-only pointer to the node's inline value
 * buffer, the zero-based node index, and the caller-supplied @p user_data
 * pointer.  The callback must not insert or remove nodes during traversal.
 *
 * O(n).
 *
 * @param list       Must not be NULL.
 * @param fn         Callback.  Must not be NULL.
 * @param user_data  Passed unchanged to @p fn; may be NULL.
 *
 * @return NO_ERROR or NULL_POINTER.
 *
 * @code
 *     static void print_float(const void* v, size_t i, void* ud) {
 *         (void)ud;
 *         printf("[%zu] = %f\n", i, *(const float*)v);
 *     }
 *     foreach_slist(l, print_float, NULL);
 * @endcode
 */
error_code_t foreach_slist(const slist_t* list,
                            slist_iter_fn  fn,
                            void*          user_data);
 
// ================================================================================
// Introspection — static inline
// ================================================================================
 
/** @brief Total number of nodes. Returns 0 if @p list is NULL. */
static inline size_t slist_size(const slist_t* list) {
    return list ? list->len : 0;
}
 
/** @brief Maximum nodes the slab can hold. Returns 0 if @p list is NULL. */
static inline size_t slist_slab_alloc(const slist_t* list) {
    return list ? list->slab_cap : 0;
}
 
/** @brief Slab slots consumed so far. Returns 0 if @p list is NULL. */
static inline size_t slist_slab_used(const slist_t* list) {
    return list ? list->slab_used : 0;
}
 
/** @brief Slab slots still available. Returns 0 if @p list is NULL or slab is full. */
static inline size_t slist_slab_remaining(const slist_t* list) {
    if (!list) return 0;
    return (list->slab_cap > list->slab_used)
           ? list->slab_cap - list->slab_used : 0;
}
 
/** @brief Number of individually allocated overflow nodes. Returns 0 if NULL or no overflow. */
static inline size_t slist_overflow_count(const slist_t* list) {
    if (!list || list->len <= list->slab_cap) return 0;
    return list->len - list->slab_cap;
}
 
/** @brief Size in bytes of each stored value. Returns 0 if @p list is NULL. */
static inline size_t slist_data_size(const slist_t* list) {
    return list ? list->data_size : 0;
}
 
/** @brief Type tag of the stored type. Returns 0 if @p list is NULL. */
static inline dtype_id_t slist_dtype(const slist_t* list) {
    return list ? list->dtype : 0;
}
 
/** @brief True if @p list is NULL or contains no nodes. */
static inline bool is_slist_empty(const slist_t* list) {
    return !list || list->len == 0;
}
 
/** @brief True if @p list is non-NULL and no nodes have been pushed yet. */
static inline bool is_slist_slab_empty(const slist_t* list) {
    return list && list->slab_used == 0;
}
 
/** @brief True if all slab nodes have been consumed. */
static inline bool is_slist_slab_full(const slist_t* list) {
    return !list || list->slab_used >= list->slab_cap;
}
 
/** @brief True if any nodes in the list reside outside the slab. */
static inline bool slist_in_overflow(const slist_t* list) {
    return list && list->len > list->slab_cap;
}
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_list_H */
// ================================================================================
// ================================================================================
// eof
