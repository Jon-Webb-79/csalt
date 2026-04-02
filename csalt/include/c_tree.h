// ================================================================================
// ================================================================================
// - File:    c_tree.h
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    April 02, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================

#ifndef c_tree_H
#define c_tree_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
 
#include "c_allocator.h"
#include "c_error.h"
#include "c_dtypes.h"
// ================================================================================ 
// ================================================================================ 

#ifdef __cplusplus
extern "C" {
#endif
// ================================================================================ 
// ================================================================================ 

/**
 * @brief A single node in the AVL tree.
 *
 * Each node stores its element bytes inline via a flexible array member,
 * avoiding a secondary allocation per node. The height field is cached and
 * updated on every insert and remove so that the balance factor at any
 * ancestor can be computed in O(1) without a subtree scan.
 *
 * The left pointer is repurposed as a free-list next pointer when the node
 * has been removed and returned to the slab's internal free list. Callers
 * must never read or write node fields directly.
 */
typedef struct avl_node_t avl_node_t;
struct avl_node_t {
    avl_node_t* left;    /**< Left child, or next free-list slot when recycled. */
    avl_node_t* right;   /**< Right child.                                      */
    int         height;  /**< Cached height of this subtree. Leaf height == 1.  */
    uint8_t     data[];  /**< Inline element bytes. Size == tree->data_size.     */
};
 
// ================================================================================
// ================================================================================
 
/**
 * @brief A generic, type-safe AVL binary search tree.
 *
 * avl_t stores elements as fixed-size inline byte buffers inside avl_node_t
 * nodes. All node memory comes from a pre-allocated contiguous slab carved
 * at initialisation. When the slab is exhausted and overflow is enabled,
 * additional nodes are allocated individually through the allocator vtable.
 * Removed nodes are returned to an internal free list (reusing the left
 * pointer field) so that slab slots are recycled rather than abandoned.
 *
 * Tree ordering and duplicate policy are both fixed at initialisation:
 *   - The comparator defines the sort order and follows the qsort(3) convention.
 *   - allow_duplicates controls whether equal elements are accepted (inserted
 *     into the right subtree of their match) or rejected with INVALID_ARG.
 *
 * In-order traversal via avl_foreach always visits elements in sorted order,
 * which is the primary advantage of an AVL tree over a heap.
 *
 * Do not modify any field directly — use the provided API functions.
 */
typedef struct {
    avl_node_t*        root;              /**< Root of the tree; NULL when empty.          */
    avl_node_t*        slab;              /**< Flat pre-allocated node block.              */
    avl_node_t*        free_list;         /**< Head of the recycled-node free list.        */
    size_t             slab_cap;          /**< Total nodes in the slab.                   */
    size_t             slab_used;         /**< Next uncarved slab index (bump pointer).    */
    size_t             len;               /**< Cached number of elements in the tree.      */
    size_t             data_size;         /**< Size of one element in bytes.               */
    size_t             node_size;         /**< sizeof(avl_node_t) + data_size, cached.     */
    dtype_id_t         dtype;             /**< Runtime type identity. Fixed at init time.  */
    bool               overflow;          /**< If true, allocate beyond slab when full.    */
    bool               allow_duplicates;  /**< If true, equal elements go to right child.  */
    allocator_vtable_t alloc_v;           /**< Allocator vtable for all memory operations. */
    int              (*cmp)(const void*, const void*); /**< Element comparator.            */
} avl_t;
 
// ================================================================================
// Expected return type
// ================================================================================
 
/**
 * @brief Expected return type for AVL tree initialisation and copy operations.
 *
 * On success, has_value is true and u.value points to a valid avl_t.
 * On failure, has_value is false and u.error contains the error code.
 */
typedef struct {
    bool has_value;
    union {
        avl_t*       value;
        error_code_t error;
    } u;
} avl_expect_t;
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
/**
 * @brief Initialise a new AVL tree.
 *
 * Allocates the avl_t struct and a contiguous slab of capacity nodes through
 * the provided allocator vtable. The dtype must be registered in the dtype
 * registry before calling this function. The comparator and duplicate policy
 * are fixed for the lifetime of the tree.
 *
 * The comparator follows the qsort(3) convention:
 *   - Returns < 0 if *a should sort before *b.
 *   - Returns   0 if *a and *b are equal.
 *   - Returns > 0 if *a should sort after *b.
 *
 * When allow_duplicates is true, elements that compare equal to an existing
 * node are inserted into that node's right subtree. When false, avl_insert
 * returns INVALID_ARG without modifying the tree.
 *
 * When overflow is true and the slab is exhausted, additional nodes are
 * allocated individually through alloc_v. When false, avl_insert returns
 * CAPACITY_OVERFLOW once the slab is full and no free-list slots remain.
 *
 * @param capacity         Number of nodes to pre-allocate in the slab. Must be > 0.
 * @param dtype            Type identifier. Must be registered in the dtype registry.
 * @param overflow         If true, allocate beyond the slab when it is exhausted.
 * @param allow_duplicates If true, equal elements are accepted; if false, they
 *                         are rejected with INVALID_ARG.
 * @param alloc_v          Allocator vtable for all memory operations.
 * @param cmp              Comparator defining sort order. Must not be NULL.
 *
 * @return avl_expect_t with has_value true and a valid avl_t* on success.
 *         On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER    if alloc_v.allocate or cmp is NULL
 *         - INVALID_ARG     if capacity is 0 or dtype is UNKNOWN_TYPE
 *         - ILLEGAL_STATE   if the dtype registry could not be initialised
 *         - TYPE_MISMATCH   if dtype is not registered in the dtype registry
 *         - LENGTH_OVERFLOW if capacity * node_size would overflow size_t
 *         - BAD_ALLOC       if the allocator fails to allocate the avl_t struct
 *         - OUT_OF_MEMORY   if the allocator fails to allocate the node slab
 *
 * @code
 *     static int cmp_int32(const void* a, const void* b) {
 *         int32_t va = *(const int32_t*)a, vb = *(const int32_t*)b;
 *         return (va > vb) - (va < vb);
 *     }
 *     allocator_vtable_t alloc = heap_allocator();
 *     avl_expect_t r = init_avl(64, INT32_TYPE, true, false, alloc, cmp_int32);
 *     if (!r.has_value) { handle_error(r.u.error); }
 *     avl_t* tree = r.u.value;
 * @endcode
 */
avl_expect_t init_avl(size_t             capacity,
                      dtype_id_t         dtype,
                      bool               overflow,
                      bool               allow_duplicates,
                      allocator_vtable_t alloc_v,
                      int              (*cmp)(const void*, const void*));
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Return an AVL tree's memory back to its allocator.
 *
 * Frees every overflow node individually through the allocator vtable, then
 * releases the node slab and the avl_t struct. Slab nodes do not require
 * individual deallocation since the entire slab is freed as one block. After
 * this call the pointer must not be used.
 *
 * @param tree  Pointer to the tree to return. Silently ignored if NULL.
 */
void return_avl(avl_t* tree);
 
// ================================================================================
// Insertion and removal
// ================================================================================
 
/**
 * @brief Insert one element into the tree and restore AVL balance.
 *
 * Descends the tree using cmp to find the insertion point. If an equal
 * element is found and allow_duplicates is false, returns INVALID_ARG without
 * modifying the tree. If allow_duplicates is true, the new element is placed
 * in the right subtree of the matching node. After insertion, the path from
 * the new node back to the root is rebalanced via single and double rotations
 * as needed. The element is copied by value; the caller retains ownership of
 * the buffer pointed to by data.
 *
 * Node allocation prefers the free list, then the slab bump pointer, then
 * overflow allocation if enabled.
 *
 * @param tree  Pointer to the target tree. Must not be NULL.
 * @param data  Pointer to the element to insert. Must not be NULL.
 *              Must point to exactly tree->data_size bytes.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER      if tree or data is NULL
 *         - INVALID_ARG       if the element compares equal to an existing node
 *                             and allow_duplicates is false
 *         - CAPACITY_OVERFLOW if the slab is full, free list is empty, and
 *                             overflow is false
 *         - OUT_OF_MEMORY     if overflow is true but allocation fails
 *
 * @code
 *     int32_t v = 42;
 *     error_code_t err = avl_insert(tree, &v);
 * @endcode
 */
error_code_t avl_insert(avl_t* tree, const void* data);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Remove one element from the tree and restore AVL balance.
 *
 * Searches for an element equal to key under the tree's comparator. If found,
 * optionally copies it into out, unlinks the node using the standard BST
 * deletion algorithm (replacing with in-order successor when the node has two
 * children), rebalances the path back to the root, and returns the node to
 * the internal free list for reuse. If allow_duplicates is true and multiple
 * equal elements exist, the first one encountered in the descent is removed.
 *
 * @param tree  Pointer to the target tree. Must not be NULL.
 * @param key   Pointer to a value to match against. Must not be NULL.
 *              Must point to exactly tree->data_size bytes.
 * @param out   Caller-provided buffer to receive the removed element, or NULL
 *              to discard. If non-NULL must be at least tree->data_size bytes.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if tree or key is NULL
 *         - EMPTY        if the tree contains no elements
 *         - NOT_FOUND    if no element equal to key exists in the tree
 *
 * @code
 *     int32_t key = 42, removed;
 *     error_code_t err = avl_remove(tree, &key, &removed);
 * @endcode
 */
error_code_t avl_remove(avl_t* tree, const void* key, void* out);
 
// ================================================================================
// Search and access
// ================================================================================
 
/**
 * @brief Test whether an element equal to key exists in the tree.
 *
 * Descends the tree in O(log n) comparisons. Does not modify the tree.
 *
 * @param tree  Pointer to the tree to search. Must not be NULL.
 * @param key   Pointer to the value to search for. Must not be NULL.
 *
 * @return true if a matching element is found, false otherwise. Also returns
 *         false if tree or key is NULL.
 *
 * @code
 *     int32_t key = 42;
 *     if (avl_contains(tree, &key)) { ... }
 * @endcode
 */
bool avl_contains(const avl_t* tree, const void* key);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Copy the element equal to key into a caller-supplied buffer.
 *
 * Descends the tree in O(log n) comparisons and copies exactly
 * tree->data_size bytes into out if a match is found. The tree is not
 * modified.
 *
 * @param tree  Pointer to the tree to search. Must not be NULL.
 * @param key   Pointer to the value to search for. Must not be NULL.
 * @param out   Caller-provided buffer to receive the found element.
 *              Must not be NULL. Must be at least tree->data_size bytes.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if tree, key, or out is NULL
 *         - EMPTY        if the tree contains no elements
 *         - NOT_FOUND    if no element equal to key exists in the tree
 *
 * @code
 *     int32_t key = 42, result;
 *     error_code_t err = avl_find(tree, &key, &result);
 * @endcode
 */
error_code_t avl_find(const avl_t* tree, const void* key, void* out);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Copy the minimum element (leftmost node) into a caller-supplied buffer.
 *
 * Traverses left pointers from the root to the leftmost node in O(log n).
 * The tree is not modified.
 *
 * @param tree  Pointer to the tree to query. Must not be NULL.
 * @param out   Caller-provided buffer to receive the minimum element.
 *              Must not be NULL. Must be at least tree->data_size bytes.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if tree or out is NULL
 *         - EMPTY        if the tree contains no elements
 *
 * @code
 *     int32_t min_val;
 *     error_code_t err = avl_min(tree, &min_val);
 * @endcode
 */
error_code_t avl_min(const avl_t* tree, void* out);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Copy the maximum element (rightmost node) into a caller-supplied buffer.
 *
 * Traverses right pointers from the root to the rightmost node in O(log n).
 * The tree is not modified.
 *
 * @param tree  Pointer to the tree to query. Must not be NULL.
 * @param out   Caller-provided buffer to receive the maximum element.
 *              Must not be NULL. Must be at least tree->data_size bytes.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if tree or out is NULL
 *         - EMPTY        if the tree contains no elements
 *
 * @code
 *     int32_t max_val;
 *     error_code_t err = avl_max(tree, &max_val);
 * @endcode
 */
error_code_t avl_max(const avl_t* tree, void* out);
 
// ================================================================================
// Traversal
// ================================================================================
 
/**
 * @brief Visit every element in sorted order via an in-order traversal.
 *
 * Performs a recursive in-order traversal (left subtree, node, right subtree),
 * calling fn once per element in ascending order as defined by the tree's
 * comparator. When allow_duplicates is true, equal elements appear
 * contiguously in the output in an unspecified relative order.
 *
 * The callback signature is:
 *   void fn(const void* element, void* ctx)
 * where element points into the node's inline data buffer (valid only for the
 * duration of the callback) and ctx is the opaque pointer supplied by the
 * caller. The tree must not be mutated during traversal.
 *
 * @param tree  Pointer to the tree to traverse. Must not be NULL.
 * @param fn    Callback invoked once per element in sorted order. Must not be NULL.
 * @param ctx   Opaque pointer forwarded to every fn call. May be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if tree or fn is NULL
 *         - EMPTY        if the tree contains no elements
 *
 * @code
 *     static void print_int(const void* elem, void* ctx) {
 *         (void)ctx;
 *         printf("%d\n", *(const int32_t*)elem);
 *     }
 *     avl_foreach(tree, print_int, NULL);
 * @endcode
 */
error_code_t avl_foreach(const avl_t* tree,
                         void       (*fn)(const void* element, void* ctx),
                         void*        ctx);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Visit every element within [low, high] in sorted order.
 *
 * Performs a selective in-order traversal, calling fn only for elements e
 * where cmp(low, e) <= 0 and cmp(e, high) <= 0. The subtree descent is
 * pruned: left subtrees are skipped when the current node is already below
 * low, and right subtrees are skipped when the current node is already above
 * high, making this significantly faster than a full traversal followed by
 * filtering when the range is narrow relative to the tree size.
 *
 * Both low and high are inclusive bounds. Passing the same pointer for both
 * is valid and visits exactly the elements equal to that value (useful for
 * duplicate-enabled trees).
 *
 * @param tree  Pointer to the tree to traverse. Must not be NULL.
 * @param low   Pointer to the lower bound (inclusive). Must not be NULL.
 * @param high  Pointer to the upper bound (inclusive). Must not be NULL.
 * @param fn    Callback invoked once per in-range element. Must not be NULL.
 * @param ctx   Opaque pointer forwarded to every fn call. May be NULL.
 *
 * @return NO_ERROR on success, or one of:
 *         - NULL_POINTER if tree, low, high, or fn is NULL
 *         - EMPTY        if the tree contains no elements
 *         - INVALID_ARG  if cmp(low, high) > 0 (inverted range)
 *
 * @code
 *     int32_t lo = 10, hi = 50;
 *     avl_foreach_range(tree, &lo, &hi, print_int, NULL);
 * @endcode
 */
error_code_t avl_foreach_range(const avl_t* tree,
                               const void*  low,
                               const void*  high,
                               void       (*fn)(const void* element, void* ctx),
                               void*        ctx);
 
// ================================================================================
// Introspection
// ================================================================================
 
/**
 * @brief Return the number of elements currently stored in the tree.
 *
 * Returns the cached len field. O(1). Returns 0 if tree is NULL.
 *
 * @param tree  Pointer to the tree to query.
 * @return Number of elements in the tree, or 0 if tree is NULL.
 */
size_t avl_size(const avl_t* tree);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Return the height of the tree.
 *
 * Returns the cached height of the root node, which equals the height of the
 * entire tree. An empty tree has height 0; a single-node tree has height 1.
 * O(1). Returns 0 if tree is NULL or the tree is empty.
 *
 * For a perfectly balanced tree of n nodes, height is approximately log2(n).
 * The AVL invariant guarantees height never exceeds 1.44 * log2(n + 2).
 *
 * @param tree  Pointer to the tree to query.
 * @return Height of the tree, or 0 if tree is NULL or empty.
 */
int avl_height(const avl_t* tree);
 
// --------------------------------------------------------------------------------
 
/**
 * @brief Return true if the tree contains no elements.
 *
 * Equivalent to avl_size(tree) == 0. Returns true if tree is NULL.
 *
 * @param tree  Pointer to the tree to query.
 * @return true if empty or NULL, false otherwise.
 */
bool avl_is_empty(const avl_t* tree);
 
// ================================================================================
// Copy
// ================================================================================
 
/**
 * @brief Create a deep copy of an AVL tree using a (possibly different) allocator.
 *
 * Allocates a new avl_t struct and a new node slab through alloc_v, then
 * copies all elements from src into the new tree via a pre-order traversal.
 * The copy shares the same dtype, data_size, comparator, overflow flag, and
 * allow_duplicates policy as src. The comparator function pointer is
 * shallow-copied — function pointers carry no ownership.
 *
 * The returned tree is fully independent of src: mutations to either tree do
 * not affect the other, and each must be individually returned via return_avl.
 *
 * The copy's slab is sized to src->len rather than src->slab_cap, so the copy
 * is compact. If the caller expects to insert more elements, they should
 * pre-size their own tree or enable overflow.
 *
 * @param src      Pointer to the source tree. Must not be NULL.
 * @param alloc_v  Allocator vtable for all memory operations on the new tree.
 *                 May differ from src's allocator. alloc_v.allocate must not
 *                 be NULL.
 *
 * @return avl_expect_t with has_value true and a valid avl_t* on success.
 *         On failure, has_value is false and u.error is one of:
 *         - NULL_POINTER  if src is NULL or alloc_v.allocate is NULL
 *         - BAD_ALLOC     if the allocator fails to allocate the new avl_t
 *         - OUT_OF_MEMORY if the allocator fails to allocate the new node slab
 *
 * @code
 *     allocator_vtable_t alloc = heap_allocator();
 *     avl_expect_t r = copy_avl(tree, alloc);
 *     if (!r.has_value) { handle_error(r.u.error); }
 *     avl_t* copy = r.u.value;
 *     // ... use copy ...
 *     return_avl(copy);
 * @endcode
 */
avl_expect_t copy_avl(const avl_t* src, allocator_vtable_t alloc_v);
// ================================================================================ 
// ================================================================================ 
#ifdef __cplusplus
}
#endif /* cplusplus */
#endif /* c_tree_H */
// ================================================================================
// ================================================================================
// eof
