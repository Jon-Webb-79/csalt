.. _c_dict:

****************
dict_t Overview
****************

The ``dict_t`` family provides a generic, allocator-agnostic hash dictionary
that maps arbitrary byte-span keys to fixed-size values.  Keys and values are
both stored as raw bytes, making the engine suitable for any value type: a
scalar such as ``float`` or ``uint8_t``, a struct copied by value, or a
pointer to a heap object.  Type-specific wrappers cast in and out of the
``void*`` interface without the engine needing to know anything about the
concrete type.

Keys are represented as a :c:struct:`dict_key_t` — a ``(const void* data,
size_t len)`` byte-span pair.  This handles both null-terminated C-strings
and arbitrary binary keys uniformly.  The convenience macro
:c:macro:`DICT_KEY` builds a :c:struct:`dict_key_t` from a C-string literal
in a single expression.  The dict always deep-copies key bytes into its own
allocator-managed storage on insert, so the caller may free or reuse the key
immediately after the call returns.

All memory is managed through an :c:type:`allocator_vtable_t` supplied by
the caller.  :c:func:`init_dict` and :c:func:`insert_dict` each accept an
allocator argument; all other operations use the allocator stored in the
:c:struct:`dict_t` at initialisation time.  This split allows nodes to be
allocated from a different arena than the dict struct itself if needed.  See
:ref:`allocator_file` for the full allocator API and available
implementations.

Error handling follows the *expected value* pattern: operations that produce
a new object return a :c:struct:`dict_expect_t` whose ``has_value`` field
distinguishes success from failure, while in-place mutations return an
:c:type:`error_code_t` directly.  Callers must always check the result
before using the value.

.. code-block:: c

   #include "c_dict.h"

   allocator_vtable_t a = heap_allocator();

   /* Create a dict mapping C-string keys to float values. */
   dict_expect_t r = init_dict(16, sizeof(float), FLOAT_TYPE, true, a);
   if (!r.has_value) { /* handle r.u.error */ }
   dict_t* d = r.u.value;

   float pi = 3.14159f;
   insert_dict(d, DICT_KEY("pi"), &pi, a);

   float v;
   if (get_dict_value(d, DICT_KEY("pi"), &v) == NO_ERROR)
       printf("pi = %f\n", v);

   return_dict(d);

Recommended Allocators
----------------------

Different allocators are appropriate depending on how dictionaries are used
and how frequently elements are inserted and removed:

* **HeapAllocator**

  Best for:

  * general-purpose hash maps
  * development and testing
  * workloads with unpredictable insertion and removal patterns

  The heap allocator is flexible and easy to debug, making it the default
  choice for most use cases.

* **ArenaAllocator**

  Best for:

  * build-once dictionaries
  * read-only lookup tables
  * configuration or initialization data

  This is highly efficient when the dictionary is constructed once and not
  modified or individually freed.

* **BuddyAllocator**

  Best for:

  * dynamic dictionaries with frequent insertions and deletions
  * systems requiring controlled fragmentation
  * long-running applications

  Buddy allocation helps manage fragmentation when many nodes are allocated
  and returned over time.

* **PoolAllocator / SlabAllocator**

  Best for:

  * high-frequency insert/remove workloads
  * uniform key/value sizes
  * performance-critical systems

  Dictionary nodes are typically uniform in size, making them an excellent
  match for pool or slab allocators. This can significantly reduce allocation
  overhead and improve cache locality.

Writing a Type-Specific Wrapper
================================

The recommended way to use ``dict_t`` is through a thin type-specific wrapper
that hides the ``void*`` casts.  The wrapper fixes ``data_size`` and
``dtype`` at initialisation and exposes a typed API to callers.  No type
registration is required — the dict engine uses only ``memcpy`` and
``memcmp`` internally and never inspects the value bytes.

.. code-block:: c

   /* Example: a dict mapping C-string keys to int32_t values. */

   typedef dict_t         int32_dict_t;
   typedef dict_expect_t  int32_dict_expect_t;

   static inline int32_dict_expect_t
   init_int32_dict(size_t cap, bool growth, allocator_vtable_t a) {
       return init_dict(cap, sizeof(int32_t), INT32_TYPE, growth, a);
   }

   static inline error_code_t
   insert_int32_dict(int32_dict_t* d, const char* key,
                     int32_t value, allocator_vtable_t a) {
       return insert_dict(d, DICT_KEY(key), &value, a);
   }

   static inline error_code_t
   get_int32_dict_value(const int32_dict_t* d, const char* key,
                        int32_t* out) {
       return get_dict_value(d, DICT_KEY(key), out);
   }

Hash Function
=============

Keys are hashed with a MurmurHash3-inspired function operating on the raw
key bytes.  The hash is seeded with a compile-time constant.  The table size
is always rounded up to the next power of two and collisions are resolved by
chaining.  The table resizes automatically when the load factor exceeds 0.75,
doubling capacity for small tables and growing by a fixed step for large ones,
provided ``growth`` was set to ``true`` at initialisation.

Internal Structure
==================

Each entry in the hash table is a :c:struct:`dict_node_t` allocated as a
single block whose layout is:

.. code-block:: text

   [ dict_node_t header ][ value buffer (data_size bytes) ]

The value buffer is accessed through :c:func:`dict_node_value` and
:c:func:`dict_node_value_c`.  Nodes are singly-linked within each bucket via
the ``next`` pointer.  Each bucket slot in the backing array is a
:c:struct:`dict_bucket_t` sentinel whose ``next`` pointer is the head of the
chain for that bucket.

Structs
=======

.. doxygenstruct:: dict_key_t
   :members:

.. doxygenstruct:: dict_node_t
   :members:

.. doxygenstruct:: dict_bucket_t
   :members:

.. doxygenstruct:: dict_t
   :members:

.. doxygenstruct:: dict_expect_t
   :members:

.. doxygenstruct:: dict_entry_t
   :members:

Key Construction Macro
======================

.. doxygendefine:: DICT_KEY

Value Access Helpers
====================

These ``static inline`` helpers return a pointer to the inline value buffer
of a node.  They are intended for use inside type-specific wrappers and are
not typically called directly by application code.

.. doxygenfunction:: dict_node_value
.. doxygenfunction:: dict_node_value_c

Iterator Type
=============

.. doxygentypedef:: dict_iter_fn

Initialisation and Teardown
============================

.. doxygenfunction:: init_dict
.. doxygenfunction:: return_dict

Insert / Remove / Update
=========================

.. doxygenfunction:: insert_dict
.. doxygenfunction:: pop_dict
.. doxygenfunction:: update_dict

Lookup
======

.. doxygenfunction:: get_dict_value
.. doxygenfunction:: get_dict_value_ptr
.. doxygenfunction:: has_dict_key

Utility Operations
==================

.. doxygenfunction:: clear_dict
.. doxygenfunction:: copy_dict
.. doxygenfunction:: merge_dict

Iteration
=========

:c:func:`foreach_dict` visits every entry in bucket order, which is not
guaranteed to match insertion order.  The callback receives a read-only
:c:struct:`dict_entry_t` view of the current key and value.  The callback
must not insert or remove entries during traversal.

.. code-block:: c

   static void print_entry(dict_entry_t e, void* ud) {
       (void)ud;
       float v;
       memcpy(&v, e.value, sizeof(float));
       printf("%.*s = %f\n", (int)e.key_len, (const char*)e.key, v);
   }

   foreach_dict(d, print_entry, NULL);

.. doxygenfunction:: foreach_dict

Introspection
=============

.. doxygenfunction:: dict_size
.. doxygenfunction:: dict_hash_size
.. doxygenfunction:: dict_alloc
.. doxygenfunction:: dict_data_size
.. doxygenfunction:: is_dict_empty
