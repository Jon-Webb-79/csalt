// ================================================================================
// ================================================================================
// - File:    c_tree.c
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    April 02, 2026
// - Version: 1.0
// - Copyright: Copyright 2026, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "c_tree.h"
// ================================================================================ 
// ================================================================================ 

/**
 * Return a pointer to the data buffer of a node.
 * The data field sits immediately after the fixed fields of avl_node_t.
 */
static inline uint8_t* _node_data(avl_node_t* n) {
    return n->data;
}
 
// --------------------------------------------------------------------------------
 
/**
 * Return the cached height of a node, or 0 for NULL.
 */
static inline int _height(const avl_node_t* n) {
    return n ? n->height : 0;
}
 
// --------------------------------------------------------------------------------
 
/**
 * Recompute and cache the height of a node from its children.
 * Must be called after any structural change to n's subtree.
 */
static inline void _update_height(avl_node_t* n) {
    int lh = _height(n->left);
    int rh = _height(n->right);
    n->height = 1 + (lh > rh ? lh : rh);
}
 
// --------------------------------------------------------------------------------
 
/**
 * Return the balance factor of a node: height(right) - height(left).
 * A value outside [-1, 1] means the node is unbalanced.
 */
static inline int _balance(const avl_node_t* n) {
    return _height(n->right) - _height(n->left);
}
 
// ================================================================================
// Internal helpers — node allocation and deallocation
// ================================================================================
 
/**
 * Carve a fresh node from the tree's slab or free list.
 * Returns NULL when the slab is exhausted and overflow is disabled.
 * Overflow nodes are allocated individually through the allocator vtable
 * and are distinguished from slab nodes by being outside the slab's address
 * range — this is used by return_avl to decide which nodes need individual
 * deallocation.
 */
static avl_node_t* _alloc_node(avl_t* tree) {
    avl_node_t* node = NULL;
 
    /* Prefer the free list first (recycled slab slots) */
    if (tree->free_list != NULL) {
        node            = tree->free_list;
        tree->free_list = (avl_node_t*)node->left;  /* pop free list */
        memset(node, 0, tree->node_size);
        return node;
    }
 
    /* Bump-pointer allocation from the slab */
    if (tree->slab_used < tree->slab_cap) {
        node = (avl_node_t*)((uint8_t*)tree->slab +
                             tree->slab_used * tree->node_size);
        tree->slab_used++;
        memset(node, 0, tree->node_size);
        return node;
    }
 
    /* Overflow path — individual allocation through the allocator vtable */
    if (tree->overflow && tree->alloc_v.allocate != NULL) {
        void_ptr_expect_t r = tree->alloc_v.allocate(
            tree->alloc_v.ctx, tree->node_size, true
        );
        if (r.has_value) node = (avl_node_t*)r.u.value;
    }
 
    return node;  /* NULL if slab full, no free list, overflow disabled or failed */
}
 
// --------------------------------------------------------------------------------
 
/**
 * Return a node to the internal free list.
 * Overflow nodes (outside the slab's address range) are also returned to the
 * free list rather than being freed immediately; they will be individually
 * deallocated during return_avl if the allocator supports return_element.
 *
 * Repurposing the left pointer as the free-list next pointer is safe because
 * a removed node is no longer part of the tree structure.
 */
static void _free_node(avl_t* tree, avl_node_t* node) {
    node->left      = (avl_node_t*)tree->free_list;
    tree->free_list = node;
}
 
// --------------------------------------------------------------------------------
 
/**
 * Return true if node is an overflow node (allocated outside the slab).
 * Used by return_avl to decide which nodes to pass to return_element.
 */
static inline bool _is_overflow_node(const avl_t* tree,
                                     const avl_node_t* node) {
    const uint8_t* slab_start = (const uint8_t*)tree->slab;
    const uint8_t* slab_end   = slab_start + tree->slab_cap * tree->node_size;
    const uint8_t* p          = (const uint8_t*)node;
    return (p < slab_start || p >= slab_end);
}
 
// ================================================================================
// Internal helpers — AVL rotations
// ================================================================================
 
/**
 * Right rotation around pivot y.
 *
 *       y                x
 *      / \              / \
 *     x   C    -->     A   y
 *    / \                  / \
 *   A   B                B   C
 *
 * Returns the new root of the rotated subtree (x).
 */
static avl_node_t* _rotate_right(avl_node_t* y) {
    avl_node_t* x = y->left;
    avl_node_t* B = x->right;
 
    x->right = y;
    y->left  = B;
 
    _update_height(y);
    _update_height(x);
    return x;
}
 
// --------------------------------------------------------------------------------
 
/**
 * Left rotation around pivot x.
 *
 *     x                  y
 *    / \                / \
 *   A   y    -->       x   C
 *      / \            / \
 *     B   C          A   B
 *
 * Returns the new root of the rotated subtree (y).
 */
static avl_node_t* _rotate_left(avl_node_t* x) {
    avl_node_t* y = x->right;
    avl_node_t* B = y->left;
 
    y->left  = x;
    x->right = B;
 
    _update_height(x);
    _update_height(y);
    return y;
}
 
// --------------------------------------------------------------------------------
 
/**
 * Rebalance the subtree rooted at n if its balance factor is out of range.
 * Applies single or double rotations as required by the AVL invariant.
 * Returns the (possibly new) root of the subtree after rebalancing.
 */
static avl_node_t* _rebalance(avl_node_t* n) {
    _update_height(n);
    int bf = _balance(n);
 
    /* Left-heavy */
    if (bf < -1) {
        if (_balance(n->left) > 0)               /* Left-Right case */
            n->left = _rotate_left(n->left);
        return _rotate_right(n);                  /* Left-Left case  */
    }
 
    /* Right-heavy */
    if (bf > 1) {
        if (_balance(n->right) < 0)               /* Right-Left case */
            n->right = _rotate_right(n->right);
        return _rotate_left(n);                   /* Right-Right case */
    }
 
    return n;  /* already balanced */
}
 
// ================================================================================
// Internal helpers — recursive insert, remove, traversal
// ================================================================================
 
/**
 * Recursive insert helper. Returns the (possibly rotated) subtree root.
 * err is set on failure; on success it remains NO_ERROR.
 */
static avl_node_t* _insert(avl_t*       tree,
                            avl_node_t*  n,
                            const void*  data,
                            error_code_t* err) {
    /* Base case: empty subtree — allocate and initialise a new leaf */
    if (n == NULL) {
        avl_node_t* node = _alloc_node(tree);
        if (node == NULL) {
            *err = tree->overflow ? OUT_OF_MEMORY : CAPACITY_OVERFLOW;
            return NULL;
        }
        memcpy(_node_data(node), data, tree->data_size);
        node->height = 1;
        tree->len++;
        return node;
    }
 
    int cmp = tree->cmp(data, _node_data(n));
 
    if (cmp < 0) {
        /* Descend left */
        n->left = _insert(tree, n->left, data, err);
    } else if (cmp > 0 || tree->allow_duplicates) {
        /* Descend right for larger values or equal values when duplicates
           are allowed (orthodox: equal goes right) */
        n->right = _insert(tree, n->right, data, err);
    } else {
        /* Equal element and duplicates are not allowed */
        *err = INVALID_ARG;
        return n;
    }
 
    if (*err != NO_ERROR) return n;  /* propagate failure without rebalancing */
 
    return _rebalance(n);
}
 
// --------------------------------------------------------------------------------
 
/**
 * Find and detach the leftmost (minimum) node in a subtree.
 * Used during two-child removal to find the in-order successor.
 * *min_out receives the detached node; returns the rebalanced subtree root.
 */
static avl_node_t* _detach_min(avl_node_t*  n,
                               avl_node_t** min_out) {
    if (n->left == NULL) {
        *min_out = n;
        return n->right;  /* splice out this node */
    }
    n->left = _detach_min(n->left, min_out);
    return _rebalance(n);
}
 
// --------------------------------------------------------------------------------
 
/**
 * Recursive remove helper. Returns the (possibly rotated) subtree root.
 * err is set on failure; removed is set to the detached node on success so
 * the caller can copy data and return it to the free list.
 */
static avl_node_t* _remove(avl_t*        tree,
                            avl_node_t*   n,
                            const void*   key,
                            avl_node_t**  removed,
                            error_code_t* err) {
    if (n == NULL) {
        *err = NOT_FOUND;
        return NULL;
    }
 
    int cmp = tree->cmp(key, _node_data(n));
 
    if (cmp < 0) {
        n->left  = _remove(tree, n->left,  key, removed, err);
    } else if (cmp > 0) {
        n->right = _remove(tree, n->right, key, removed, err);
    } else {
        /* Found — remove this node */
        *removed = n;
 
        if (n->left == NULL)  return n->right;
        if (n->right == NULL) return n->left;
 
        /* Two children: replace with in-order successor */
        avl_node_t* successor = NULL;
        avl_node_t* new_right = _detach_min(n->right, &successor);
 
        successor->left  = n->left;
        successor->right = new_right;
        return _rebalance(successor);
    }
 
    if (*err != NO_ERROR) return n;
    return _rebalance(n);
}
 
// --------------------------------------------------------------------------------
 
/**
 * Recursive in-order traversal. Visits left subtree, node, right subtree.
 */
static void _inorder(const avl_node_t* n,
                     size_t            data_size,
                     void            (*fn)(const void*, void*),
                     void*             ctx) {
    if (n == NULL) return;
    _inorder(n->left,  data_size, fn, ctx);
    fn((const void*)n->data, ctx);
    _inorder(n->right, data_size, fn, ctx);
}
 
// --------------------------------------------------------------------------------
 
/**
 * Selective in-order traversal for avl_foreach_range.
 * Prunes left branches when the current node is already <= low, and right
 * branches when the current node is already >= high.
 */
static void _inorder_range(const avl_node_t* n,
                           const avl_t*      tree,
                           const void*       low,
                           const void*       high,
                           void            (*fn)(const void*, void*),
                           void*             ctx) {
    if (n == NULL) return;
 
    int cmp_low  = tree->cmp(low,  n->data);
    int cmp_high = tree->cmp(high, n->data);
 
    /* Descend left only if current node > low (there may be in-range nodes) */
    if (cmp_low < 0)
        _inorder_range(n->left, tree, low, high, fn, ctx);
 
    /* Visit this node if it falls within [low, high] */
    if (cmp_low <= 0 && cmp_high >= 0)
        fn((const void*)n->data, ctx);
 
    /* Descend right only if current node < high */
    if (cmp_high > 0)
        _inorder_range(n->right, tree, low, high, fn, ctx);
}
 
// --------------------------------------------------------------------------------
 
/**
 * Pre-order traversal used by copy_avl and return_avl.
 * Visits node before children, allowing callers to process parents before
 * their descendants are invalidated.
 */
typedef void (*_node_visitor_fn)(avl_t*, avl_node_t*, void*);
 
static void _preorder(avl_t*            tree,
                      avl_node_t*       n,
                      _node_visitor_fn  visit,
                      void*             ctx) {
    if (n == NULL) return;
    /* Save children before visit potentially modifies the node */
    avl_node_t* left  = n->left;
    avl_node_t* right = n->right;
    visit(tree, n, ctx);
    _preorder(tree, left,  visit, ctx);
    _preorder(tree, right, visit, ctx);
}
 
// ================================================================================
// Initialisation and teardown
// ================================================================================
 
avl_expect_t init_avl(size_t             capacity,
                      dtype_id_t         dtype,
                      bool               overflow,
                      bool               allow_duplicates,
                      allocator_vtable_t alloc_v,
                      int              (*cmp)(const void*, const void*)) {
    if (alloc_v.allocate == NULL || cmp == NULL)
        return (avl_expect_t){ .has_value = false, .u.error = NULL_POINTER };
    if (capacity == 0u || dtype == UNKNOWN_TYPE)
        return (avl_expect_t){ .has_value = false, .u.error = INVALID_ARG };
    if (!init_dtype_registry())
        return (avl_expect_t){ .has_value = false, .u.error = ILLEGAL_STATE };
 
    const dtype_t* desc = lookup_dtype(dtype);
    if (desc == NULL)
        return (avl_expect_t){ .has_value = false, .u.error = TYPE_MISMATCH };
 
    /* node_size = struct header + inline data, rounded up so each node is
       naturally aligned for the worst-case member (pointer-sized). */
    size_t data_size = desc->data_size;
    size_t node_size = sizeof(avl_node_t) + data_size;
 
    /* Overflow guard on the slab allocation */
    if (capacity > SIZE_MAX / node_size)
        return (avl_expect_t){ .has_value = false, .u.error = LENGTH_OVERFLOW };
 
    /* Allocate the avl_t struct */
    void_ptr_expect_t sr = alloc_v.allocate(alloc_v.ctx, sizeof(avl_t), true);
    if (!sr.has_value)
        return (avl_expect_t){ .has_value = false, .u.error = BAD_ALLOC };
 
    avl_t* tree = (avl_t*)sr.u.value;
 
    /* Allocate the node slab */
    void_ptr_expect_t slab_r = alloc_v.allocate(
        alloc_v.ctx, capacity * node_size, true
    );
    if (!slab_r.has_value) {
        alloc_v.return_element(alloc_v.ctx, tree);
        return (avl_expect_t){ .has_value = false, .u.error = OUT_OF_MEMORY };
    }
 
    tree->root             = NULL;
    tree->slab             = (avl_node_t*)slab_r.u.value;
    tree->free_list        = NULL;
    tree->slab_cap         = capacity;
    tree->slab_used        = 0u;
    tree->len              = 0u;
    tree->data_size        = data_size;
    tree->node_size        = node_size;
    tree->dtype            = dtype;
    tree->overflow         = overflow;
    tree->allow_duplicates = allow_duplicates;
    tree->alloc_v          = alloc_v;
    tree->cmp              = cmp;
 
    return (avl_expect_t){ .has_value = true, .u.value = tree };
}
 
// --------------------------------------------------------------------------------
 
/**
 * Visitor used by return_avl: frees overflow nodes individually.
 * Slab nodes are freed in bulk when the slab itself is released.
 */
static void _return_node_visitor(avl_t* tree, avl_node_t* n, void* ctx) {
    (void)ctx;
    if (_is_overflow_node(tree, n))
        tree->alloc_v.return_element(tree->alloc_v.ctx, n);
}
 
void return_avl(avl_t* tree) {
    if (tree == NULL) return;
 
    /* Free any overflow nodes encountered during pre-order traversal.
       Slab nodes need not be freed individually. */
    if (tree->overflow && tree->alloc_v.return_element != NULL)
        _preorder(tree, tree->root, _return_node_visitor, NULL);
 
    /* Also walk the free list and free any overflow nodes sitting there */
    if (tree->overflow && tree->alloc_v.return_element != NULL) {
        avl_node_t* fl = tree->free_list;
        while (fl != NULL) {
            avl_node_t* next = (avl_node_t*)fl->left;
            if (_is_overflow_node(tree, fl))
                tree->alloc_v.return_element(tree->alloc_v.ctx, fl);
            fl = next;
        }
    }
 
    /* Release the slab and the tree struct */
    tree->alloc_v.return_element(tree->alloc_v.ctx, tree->slab);
    tree->slab = NULL;
    tree->alloc_v.return_element(tree->alloc_v.ctx, tree);
}
 
// ================================================================================
// Insertion and removal
// ================================================================================
 
error_code_t avl_insert(avl_t* tree, const void* data) {
    if (tree == NULL || data == NULL) return NULL_POINTER;
 
    error_code_t err = NO_ERROR;
    tree->root = _insert(tree, tree->root, data, &err);
    return err;
}
 
// --------------------------------------------------------------------------------
 
error_code_t avl_remove(avl_t* tree, const void* key, void* out) {
    if (tree == NULL || key == NULL) return NULL_POINTER;
    if (tree->len == 0u)             return EMPTY;
 
    error_code_t err     = NO_ERROR;
    avl_node_t*  removed = NULL;
 
    tree->root = _remove(tree, tree->root, key, &removed, &err);
 
    if (err != NO_ERROR) return err;
 
    /* Copy element to caller's buffer before returning the node */
    if (out != NULL)
        memcpy(out, _node_data(removed), tree->data_size);
 
    _free_node(tree, removed);
    tree->len--;
    return NO_ERROR;
}
 
// ================================================================================
// Search and access
// ================================================================================
 
bool avl_contains(const avl_t* tree, const void* key) {
    if (tree == NULL || key == NULL) return false;
 
    const avl_node_t* n = tree->root;
    while (n != NULL) {
        int cmp = tree->cmp(key, (const void*)n->data);
        if      (cmp < 0) n = n->left;
        else if (cmp > 0) n = n->right;
        else              return true;
    }
    return false;
}
 
// --------------------------------------------------------------------------------
 
error_code_t avl_find(const avl_t* tree, const void* key, void* out) {
    if (tree == NULL || key == NULL || out == NULL) return NULL_POINTER;
    if (tree->len == 0u)                            return EMPTY;
 
    const avl_node_t* n = tree->root;
    while (n != NULL) {
        int cmp = tree->cmp(key, (const void*)n->data);
        if (cmp < 0)       n = n->left;
        else if (cmp > 0)  n = n->right;
        else {
            memcpy(out, (const void*)n->data, tree->data_size);
            return NO_ERROR;
        }
    }
    return NOT_FOUND;
}
 
// --------------------------------------------------------------------------------
 
error_code_t avl_min(const avl_t* tree, void* out) {
    if (tree == NULL || out == NULL) return NULL_POINTER;
    if (tree->len == 0u)             return EMPTY;
 
    const avl_node_t* n = tree->root;
    while (n->left != NULL) n = n->left;
    memcpy(out, (const void*)n->data, tree->data_size);
    return NO_ERROR;
}
 
// --------------------------------------------------------------------------------
 
error_code_t avl_max(const avl_t* tree, void* out) {
    if (tree == NULL || out == NULL) return NULL_POINTER;
    if (tree->len == 0u)             return EMPTY;
 
    const avl_node_t* n = tree->root;
    while (n->right != NULL) n = n->right;
    memcpy(out, (const void*)n->data, tree->data_size);
    return NO_ERROR;
}
 
// ================================================================================
// Traversal
// ================================================================================
 
error_code_t avl_foreach(const avl_t* tree,
                         void       (*fn)(const void* element, void* ctx),
                         void*        ctx) {
    if (tree == NULL || fn == NULL) return NULL_POINTER;
    if (tree->len == 0u)            return EMPTY;
 
    _inorder(tree->root, tree->data_size, fn, ctx);
    return NO_ERROR;
}
 
// --------------------------------------------------------------------------------
 
error_code_t avl_foreach_range(const avl_t* tree,
                               const void*  low,
                               const void*  high,
                               void       (*fn)(const void* element, void* ctx),
                               void*        ctx) {
    if (tree == NULL || low == NULL || high == NULL || fn == NULL)
        return NULL_POINTER;
    if (tree->len == 0u)
        return EMPTY;
    if (tree->cmp(low, high) > 0)
        return INVALID_ARG;
 
    _inorder_range(tree->root, tree, low, high, fn, ctx);
    return NO_ERROR;
}
 
// ================================================================================
// Introspection
// ================================================================================
 
size_t avl_size(const avl_t* tree) {
    return tree ? tree->len : 0u;
}
 
// --------------------------------------------------------------------------------
 
int avl_height(const avl_t* tree) {
    if (tree == NULL) return 0;
    return _height(tree->root);
}
 
// --------------------------------------------------------------------------------
 
bool avl_is_empty(const avl_t* tree) {
    return tree == NULL || tree->len == 0u;
}
 
// ================================================================================
// Copy
// ================================================================================
 
/**
 * Visitor context for copy_avl: holds the destination tree and a running
 * error code so the pre-order visitor can signal failure back to the caller.
 */
typedef struct {
    avl_t*       dst;
    error_code_t err;
} _copy_ctx_t;
 
/**
 * Pre-order visitor: inserts each source node's data into the destination
 * tree. Uses avl_insert so that the copied tree is properly balanced and
 * all invariants hold.
 */
static void _copy_visitor(avl_t* src_tree, avl_node_t* n, void* ctx) {
    (void)src_tree;
    _copy_ctx_t* c = (_copy_ctx_t*)ctx;
    if (c->err != NO_ERROR) return;   /* short-circuit on first failure */
    c->err = avl_insert(c->dst, (const void*)n->data);
}
 
avl_expect_t copy_avl(const avl_t* src, allocator_vtable_t alloc_v) {
    if (src == NULL || alloc_v.allocate == NULL)
        return (avl_expect_t){ .has_value = false, .u.error = NULL_POINTER };
 
    /* Size the copy's slab to src->len (compact) rather than src->slab_cap */
    size_t cap = src->len > 0u ? src->len : 1u;
 
    avl_expect_t r = init_avl(cap,
                              src->dtype,
                              src->overflow,
                              src->allow_duplicates,
                              alloc_v,
                              src->cmp);
    if (!r.has_value) return r;
 
    _copy_ctx_t ctx = { r.u.value, NO_ERROR };
    _preorder((avl_t*)src, src->root, _copy_visitor, &ctx);
 
    if (ctx.err != NO_ERROR) {
        return_avl(r.u.value);
        return (avl_expect_t){ .has_value = false, .u.error = ctx.err };
    }
 
    return (avl_expect_t){ .has_value = true, .u.value = r.u.value };
}
// ================================================================================
// ================================================================================
// eof
