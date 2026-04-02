.. _slist:

******************
Singly Linked List
******************

``c_list.h`` provides a generic, type-erased singly linked list built on top
of the csalt allocator and dtype systems.  It is designed for flexibility,
allocator control, and predictable performance, while remaining fully
agnostic to the stored data type.

Unlike :ref:`array`, this module does not provide primitive typed wrappers.
Instead, it is intended to serve as a foundation for user-defined data
structures, where the caller defines the stored type via
:c:type:`dtype_id_t` and builds thin wrappers around the generic API.

Overview
--------

The list stores values as raw byte buffers of fixed size determined at
initialisation.  Each node embeds its value inline, avoiding additional
pointer indirection and improving cache locality during traversal.

A hybrid allocation model is used:

- A contiguous *slab* of nodes is pre-allocated at initialisation
- Additional nodes may optionally be allocated individually if overflow is enabled

All memory management is performed through a caller-supplied
:c:type:`allocator_vtable_t`.  No assumptions are made about the backing
allocator (arena, pool, heap, etc.).

.. code-block:: c

   #include "c_list.h"

   allocator_vtable_t alloc = heap_allocator();

   slist_expect_t r = init_slist(64, sizeof(int32_t),
                                INT32_TYPE, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }

   slist_t* list = r.u.value;

   int32_t v = 10;
   push_back_slist(list, &v);

   int32_t out = 0;
   get_slist(list, 0, &out);   /* out == 10 */

   return_slist(list);

.. note::

   The generic API performs no implicit type conversions.  The caller is
   responsible for ensuring that all inserted values match the declared
   ``data_size`` and ``dtype``.

Memory Model
------------

Each node stores its value inline:

.. code-block:: c

   struct sNode {
       struct sNode* next;
       uint8_t       data[];
   };

This layout provides:

- Single allocation per node
- Improved spatial locality for slab-allocated nodes
- No double indirection for value access

The list uses two allocation strategies:

1. **Slab allocation (primary)**
   - Nodes are carved from a contiguous block
   - Improves cache performance

2. **Overflow allocation (optional)**
   - Enabled via ``allow_overflow = true``
   - Falls back to allocator when slab is exhausted

Ownership Rules
---------------

- The list owns:
  - its header
  - its slab allocation
  - any overflow nodes
- All memory is released via :c:func:`return_slist`
- Individual nodes must never be freed by the caller

Allocator Integration
---------------------

All allocations are performed through :c:type:`allocator_vtable_t`.

This allows the list to be backed by:

- heap allocators
- arena allocators
- pool allocators
- custom user-defined allocators

The list does not assume ownership semantics beyond what is defined
by the allocator implementation.

See :ref:`allocator_file` for details.

Recommended Allocators
----------------------

Different allocators are appropriate depending on how the list is used:

* **HeapAllocator**
  
  Best for:
  
  * general-purpose use
  * unit testing
  * host-side applications
  * debugging container logic independent of allocator complexity

  This is usually the best allocator to use first because failure modes are
  simple and easy to reason about.

* **ArenaAllocator**
  
  Best for:
  
  * build-then-discard workflows
  * temporary lists used during parsing or transformation passes
  * scenarios where the full list lifetime matches a larger arena lifetime

  This works especially well when overflow is disabled or rare. If overflow
  is frequent, the arena must still have enough capacity to satisfy those
  extra node allocations.

* **BuddyAllocator**
  
  Best for:
  
  * deterministic dynamic allocation
  * environments where memory should be returnable and coalesced
  * systems requiring more structure than a heap allocator

  This is a reasonable choice when the list may grow dynamically and later
  release memory through normal destruction.

* **PoolAllocator / SlabAllocator**
  
  Best for:
  
  * overflow-heavy workloads with uniform node sizes
  * fixed-type deployments where the node footprint is stable
  * performance-sensitive paths with many push/pop operations

  These allocators can be particularly effective because list nodes are
  fixed-size objects for a given ``T``. They are less useful when only the
  primary slab is used and overflow is disabled.

Error Handling
--------------

The list follows the csalt *expected value* pattern:

- Constructors return :c:type:`slist_expect_t`
- Mutating operations return :c:type:`error_code_t`

Callers must always check return values before use.

.. code-block:: c

   slist_expect_t r = init_slist(...);
   if (!r.has_value) {
       error_code_t err = r.u.error;
       /* handle error */
   }

Type System and User-Defined Types
----------------------------------

The list relies on :c:type:`dtype_id_t` to describe stored data.

Built-in types are defined in ``c_dtypes.h``:

- ``INT32_TYPE``
- ``FLOAT_TYPE``
- ``STRING_TYPE``
- etc.

For user-defined types, define a unique type ID and register it with
the dtype registry.

.. code-block:: c

   typedef struct {
       float x, y, z;
   } vec3_t;

   /* Define a unique dtype ID */
   #define VEC3_TYPE (USER_BASE_TYPE + 1u)

   static const dtype_t vec3_desc = {
       VEC3_TYPE,
       sizeof(vec3_t),
       "vec3"
   };

   ensure_dtype_registered(&vec3_desc);

   slist_expect_t r = init_slist(32, sizeof(vec3_t),
                                VEC3_TYPE, false, alloc);

.. note::

   User-defined dtype IDs must be >= ``USER_BASE_TYPE`` and must be unique
   within the application.  It is recommended to define them as named macros
   rather than using raw numeric values.

Typed Wrapper Pattern (Recommended)
-----------------------------------

Because the generic API operates on ``void*`` buffers, it is recommended
to define thin wrappers for your specific data types.

Example wrapper for ``vec3_t``:

.. code-block:: c

   static inline error_code_t push_back_vec3(slist_t* list,
                                             const vec3_t* v) {
       return push_back_slist(list, v);
   }

   static inline error_code_t get_vec3(const slist_t* list,
                                       size_t index,
                                       vec3_t* out) {
       return get_slist(list, index, out);
   }

This provides:

- Type safety at the call site
- Cleaner API usage
- No runtime overhead

Core API
--------

Structs
~~~~~~~

.. doxygenstruct:: slist_t
   :members:

.. doxygenstruct:: slist_expect_t
   :members:

.. doxygenstruct:: slist_index_expect_t
   :members:

Initialisation and Teardown
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_slist

.. doxygenfunction:: return_slist

Push Operations
~~~~~~~~~~~~~~~

.. doxygenfunction:: push_back_slist

.. doxygenfunction:: push_front_slist

.. doxygenfunction:: push_at_slist

Access Operations
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: get_slist

.. doxygenfunction:: peek_front_slist

.. doxygenfunction:: peek_back_slist

Pop Operations
~~~~~~~~~~~~~~

.. doxygenfunction:: pop_front_slist

.. doxygenfunction:: pop_back_slist

.. doxygenfunction:: pop_at_slist

Iteration
~~~~~~~~~

.. doxygenfunction:: foreach_slist

Search
~~~~~~

.. doxygenfunction:: contains_slist

Introspection
~~~~~~~~~~~~~

All introspection functions are O(1) read-only operations.  Functions return
safe default values when passed a NULL pointer.

.. doxygenfunction:: slist_size

.. doxygenfunction:: is_slist_empty

.. doxygenfunction:: slist_slab_alloc

.. doxygenfunction:: slist_data_size
