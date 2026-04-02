.. _avl_tree:

********
AVL Tree
********

``c_tree.h`` provides a generic, type-erased AVL (self-balancing binary
search tree) built on top of the csalt allocator and dtype systems.  It is
designed for predictable performance, allocator control, and efficient
ordered data access while remaining fully agnostic to the stored data type.

Unlike :ref:`array`, this module does not provide primitive typed wrappers.
Instead, it is intended to serve as a foundation for user-defined data
structures, where the caller defines the stored type via
:c:type:`dtype_id_t` and builds thin wrappers around the generic API.

Overview
--------

The AVL tree stores values as raw byte buffers of fixed size determined at
initialisation.  Each node embeds its value inline, avoiding additional
pointer indirection and improving cache locality.

The tree maintains the AVL invariant:

- For every node, the height difference between left and right subtrees
  is at most 1
- Insertions and removals automatically rebalance the tree using rotations

This guarantees:

- O(log n) insertion
- O(log n) removal
- O(log n) search
- Ordered traversal in O(n)

A hybrid allocation model is used:

- A contiguous *slab* of nodes is pre-allocated at initialisation
- Nodes are recycled through an internal free list
- Additional nodes may optionally be allocated individually if overflow is enabled

All memory management is performed through a caller-supplied
:c:type:`allocator_vtable_t`.

.. code-block:: c

   #include "c_tree.h"

   static int cmp_int32(const void* a, const void* b) {
       int32_t x = *(const int32_t*)a;
       int32_t y = *(const int32_t*)b;
       return (x > y) - (x < y);
   }

   allocator_vtable_t alloc = heap_allocator();

   avl_expect_t r = init_avl(64, INT32_TYPE, true, false, alloc, cmp_int32);
   if (!r.has_value) { /* handle r.u.error */ }

   avl_t* tree = r.u.value;

   int32_t v = 42;
   avl_insert(tree, &v);

   int32_t out = 0;
   avl_find(tree, &v, &out);

   return_avl(tree);

.. note::

   The generic API performs no implicit type conversions.  The caller is
   responsible for ensuring that all inserted values match the declared
   ``dtype`` and ``data_size``.

Memory Model
------------

Each node stores its value inline:

.. code-block:: c

   struct avl_node_t {
       struct avl_node_t* left;
       struct avl_node_t* right;
       int                height;
       uint8_t            data[];
   };

This layout provides:

- Single allocation per node
- Improved spatial locality for slab-allocated nodes
- No double indirection for value access

The tree uses three allocation strategies:

1. **Free list reuse (primary)**
   - Removed nodes are recycled
   - Avoids unnecessary allocations

2. **Slab allocation**
   - Nodes are carved from a contiguous block
   - Improves cache performance

3. **Overflow allocation (optional)**
   - Enabled via ``overflow = true``
   - Falls back to allocator when slab is exhausted

Ownership Rules
---------------

- The tree owns:
  - its header
  - its slab allocation
  - any overflow nodes
- All memory is released via :c:func:`return_avl`
- Individual nodes must never be freed by the caller

Allocator Integration
---------------------

All allocations are performed through :c:type:`allocator_vtable_t`.

This allows the tree to be backed by:

- heap allocators
- arena allocators
- pool allocators
- buddy allocators
- custom user-defined allocators

The tree does not assume ownership semantics beyond what is defined
by the allocator implementation.

See :ref:`allocator_file` for details.

Recommended Allocators
----------------------

Different allocators are appropriate depending on how the tree is used:

* **HeapAllocator**
  
  Best for:
  
  * general-purpose use
  * unit testing
  * host-side applications
  * debugging tree logic independent of allocator complexity

* **ArenaAllocator**
  
  Best for:
  
  * build-then-discard workflows
  * temporary ordered data structures
  * scenarios where the tree lifetime matches a larger arena lifetime

* **BuddyAllocator**
  
  Best for:
  
  * deterministic allocation patterns
  * systems requiring coalescing and reuse
  * long-lived trees with dynamic growth and shrink

* **PoolAllocator / SlabAllocator**
  
  Best for:
  
  * fixed-size node workloads
  * high-frequency insert/remove operations
  * performance-critical paths

Error Handling
--------------

The tree follows the csalt *expected value* pattern:

- Constructors return :c:type:`avl_expect_t`
- Mutating operations return :c:type:`error_code_t`

Callers must always check return values.

.. code-block:: c

   avl_expect_t r = init_avl(...);
   if (!r.has_value) {
       error_code_t err = r.u.error;
       /* handle error */
   }

Type System and User-Defined Types
----------------------------------

The tree relies on :c:type:`dtype_id_t` to describe stored data.

Built-in types are defined in ``c_dtypes.h``:

- ``INT32_TYPE``
- ``FLOAT_TYPE``
- ``DOUBLE_TYPE``
- etc.

For user-defined types, define a unique type ID and register it with
the dtype registry.

.. code-block:: c

   typedef struct {
       float x, y, z;
   } vec3_t;

   #define VEC3_TYPE (USER_BASE_TYPE + 1u)

   static const dtype_t vec3_desc = {
       VEC3_TYPE,
       sizeof(vec3_t),
       "vec3"
   };

   ensure_dtype_registered(&vec3_desc);

   avl_expect_t r = init_avl(32, VEC3_TYPE, false, false, alloc, cmp_vec3);

.. note::

   User-defined dtype IDs must be >= ``USER_BASE_TYPE`` and must be unique.

Typed Wrapper Pattern (Recommended)
-----------------------------------

Because the AVL API operates on ``void*`` buffers and requires a comparator,
it is recommended to define thin wrappers for specific data types.

Example wrapper for ``int32_t``:

.. code-block:: c

   static inline error_code_t int32_avl_insert(avl_t* tree, int32_t value) {
       return avl_insert(tree, &value);
   }

   static inline error_code_t int32_avl_find(const avl_t* tree,
                                             int32_t key,
                                             int32_t* out) {
       return avl_find(tree, &key, out);
   }

This provides:

- Type safety at the call site
- Cleaner API usage
- No runtime overhead

Core API
--------

Structs
~~~~~~~

.. doxygenstruct:: avl_t
   :members:

.. doxygenstruct:: avl_expect_t
   :members:

Initialisation and Teardown
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_avl

.. doxygenfunction:: return_avl

Insertion and Removal
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: avl_insert

.. doxygenfunction:: avl_remove

Search and Access
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: avl_contains

.. doxygenfunction:: avl_find

.. doxygenfunction:: avl_min

.. doxygenfunction:: avl_max

Traversal
~~~~~~~~~

.. doxygenfunction:: avl_foreach

.. doxygenfunction:: avl_foreach_range

Introspection
~~~~~~~~~~~~~

All introspection functions are O(1) read-only operations.

.. doxygenfunction:: avl_size

.. doxygenfunction:: avl_height

.. doxygenfunction:: avl_is_empty

Copy
~~~~

.. doxygenfunction:: copy_avl
