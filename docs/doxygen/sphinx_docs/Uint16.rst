.. _uint16:

*****************
uint16_t Overview
*****************
The ``uint16_t`` family of data structures in csalt provides type-safe,
allocator-agnostic containers for 8-bit unsigned integer data.  Every
structure wraps the generic ``array_t`` engine from ``c_array.h`` with the
element type fixed to ``UINT8_TYPE`` at initialisation, eliminating the need
for the caller to pass a ``dtype_id_t`` argument to any operation.

All memory is managed through an :c:type:`allocator_vtable_t` supplied by
the caller at initialisation time.  csalt does not assume a default
allocator — the caller must explicitly provide one.  The library ships
several ready-made implementations and the interface is open so that custom
allocators can be substituted without changing any call sites.  See
:ref:`allocator_file` for the full allocator API, the list of available
implementations, and guidance on writing your own.

Error handling follows the *expected value* pattern throughout: operations
that produce a new object return a ``*_expect_t`` struct whose ``has_value``
field distinguishes success from failure, while in-place mutations return an
:c:type:`error_code_t` directly.  Callers must always check the result
before using the value.

uint16_t Array
==============
A :c:type:`uint16_array_t` is an ordered sequence of ``uint16_t`` values
backed by a contiguous heap-allocated buffer.  It supports O(1) amortised
append, O(n) insert and remove at arbitrary positions, SIMD-accelerated
reverse and search, and an in-place comparison sort.  It is implemented as
a thin type-safe wrapper around the generic :ref:`array` engine described
in ``c_array.h``.

The array does not have a default allocator — an :c:type:`allocator_vtable_t`
must be supplied at every call site that allocates memory (initialisation,
copy, and slice).  See :ref:`allocator_file` for available allocators and
the trade-offs between them.

.. code-block:: c

   #include "c_uint16.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   uint16_array_expect_t r = init_uint16_array(8, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   uint16_array_t* arr = r.u.value;

   push_back_uint16_array(arr, 30);
   push_back_uint16_array(arr, 10);
   push_back_uint16_array(arr, 20);

   sort_uint16_array(arr, FORWARD);   /* arr is now [10, 20, 30] */

   return_uint16_array(arr);

Structs
-------

.. doxygenstruct:: uint16_array_t
   :members:

.. doxygenstruct:: uint16_array_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_uint16_array

.. doxygenfunction:: return_uint16_array

Push Operations
---------------

.. doxygenfunction:: push_back_uint16_array

.. doxygenfunction:: push_front_uint16_array

.. doxygenfunction:: push_at_uint16_array

Get and Set
-----------

.. doxygenfunction:: get_uint16_array_index

.. doxygenfunction:: set_uint16_array_index

Pop Operations
--------------

.. doxygenfunction:: pop_back_uint16_array

.. doxygenfunction:: pop_front_uint16_array

.. doxygenfunction:: pop_any_uint16_array

Utility Operations
------------------

.. doxygenfunction:: clear_uint16_array

.. doxygenfunction:: copy_uint16_array

.. doxygenfunction:: concat_uint16_array

.. doxygenfunction:: slice_uint16_array

Algorithms
----------

.. doxygenfunction:: reverse_uint16_array

.. doxygenfunction:: sort_uint16_array

Search
------

.. doxygenfunction:: uint16_array_contains

.. doxygenfunction:: uint16_array_binary_search 

.. doxygenfunction:: uint16_array_binary_bracket

Introspection
-------------

.. doxygenfunction:: uint16_array_size

.. doxygenfunction:: uint16_array_alloc

.. doxygenfunction:: uint16_array_data_size

.. doxygenfunction:: is_uint16_array_empty

.. doxygenfunction:: is_uint16_array_full

.. doxygenfunction:: is_uint16_array_ptr

.. doxygenfunction:: uint16_array_max

.. doxygenfunction:: uint16_array_min

uint16_t Dictionary 
===================
A ``uint16_dict_t`` is a hash dictionary that maps C-string keys to
``uint16_t`` values.  It is a type-safe wrapper around the generic
dict engine described in :ref:`c_dict`, with the value type fixed to
``sizeof(uint16_t)`` and ``dtype`` fixed to ``UINT16_TYPE`` at initialisation.
 
Keys are null-terminated C-strings.  Every function is available in two
forms: a plain variant that measures the key length with ``strlen``, and an
``_n`` variant that accepts an explicit ``size_t key_len`` argument.  The
``_n`` variants are useful when the key is a sub-string of a larger buffer,
when its length is already known and the ``strlen`` scan is unnecessary, or
when the key contains embedded null bytes.
 
The dict does not store a pointer to the caller's key — it copies the key
bytes into its own allocator-managed storage on every insert.  The caller
may free or reuse the key memory immediately after any dict call returns.
 
The dict does not have a default allocator.  An
:c:type:`allocator_vtable_t` must be supplied to :c:func:`init_uint16_dict`
and to every :c:func:`insert_uint16_dict` call.  All other operations use
the allocator that was stored at initialisation time.  See
:ref:`allocator_file` for available allocators and the trade-offs between
them.
 
.. code-block:: c
 
   #include "uint16_dict.h"
 
   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t a = heap_allocator();
 
   uint16_dict_expect_t r = init_uint16_dict(16, true, a);
   if (!r.has_value) { /* handle r.u.error */ }
   uint16_dict_t* d = r.u.value;
 
   insert_uint16_dict(d, "width",  1920u, a);
   insert_uint16_dict(d, "height", 1080u, a);
   insert_uint16_dict(d, "depth",    32u, a);
 
   uint16_t v;
   get_uint16_dict_value(d, "width", &v);   /* v == 1920 */
 
   update_uint16_dict(d, "width", 2560u);
 
   pop_uint16_dict(d, "depth", NULL);       /* removes "depth", discards value */
 
   return_uint16_dict(d);
 
Plain vs ``_n`` Variants
------------------------
 
Every function that takes a key is available in two forms.
 
.. list-table::
   :header-rows: 1
   :widths: 45 55
 
   * - Plain variant
     - ``_n`` variant
   * - ``insert_uint16_dict(d, key, value, a)``
     - ``insert_uint16_dict_n(d, key, key_len, value, a)``
   * - ``pop_uint16_dict(d, key, out)``
     - ``pop_uint16_dict_n(d, key, key_len, out)``
   * - ``update_uint16_dict(d, key, value)``
     - ``update_uint16_dict_n(d, key, key_len, value)``
   * - ``get_uint16_dict_value(d, key, out)``
     - ``get_uint16_dict_value_n(d, key, key_len, out)``
   * - ``get_uint16_dict_ptr(d, key)``
     - ``get_uint16_dict_ptr_n(d, key, key_len)``
   * - ``has_uint16_dict_key(d, key)``
     - ``has_uint16_dict_key_n(d, key, key_len)``
 
The ``_n`` variants are particularly useful for splitting on sub-strings
without constructing a null-terminated copy:
 
.. code-block:: c
 
   /* Buffer holds "width_max" but we only want "width" (5 bytes). */
   const char* buf = "width_max";
   insert_uint16_dict_n(d, buf, 5, 1920u, a);
 
   uint16_t v;
   get_uint16_dict_value_n(d, buf, 5, &v);   /* v == 1920 */
   get_uint16_dict_value(d, "width", &v);     /* same key — also v == 1920 */
 
Structs
-------
 
.. note::
 
   ``uint16_dict_t`` is a ``typedef`` alias for :c:struct:`dict_t`.  All
   internal fields are documented under :c:struct:`dict_t` in
   :ref:`c_dict`.  The ``data_size`` field is always ``sizeof(uint16_t)``
   and the ``dtype`` field is always ``UINT16_TYPE``.
 
.. doxygenstruct:: uint16_dict_expect_t
   :members:
 
Initialisation and Teardown
---------------------------
 
.. doxygenfunction:: init_uint16_dict
.. doxygenfunction:: return_uint16_dict
 
Insert
------
 
.. doxygenfunction:: insert_uint16_dict
.. doxygenfunction:: insert_uint16_dict_n
 
Pop
---
 
.. doxygenfunction:: pop_uint16_dict
.. doxygenfunction:: pop_uint16_dict_n
 
Update
------
 
.. doxygenfunction:: update_uint16_dict
.. doxygenfunction:: update_uint16_dict_n
 
Lookup
------
 
.. doxygenfunction:: get_uint16_dict_value
.. doxygenfunction:: get_uint16_dict_value_n
.. doxygenfunction:: get_uint16_dict_ptr
.. doxygenfunction:: get_uint16_dict_ptr_n
.. doxygenfunction:: has_uint16_dict_key
.. doxygenfunction:: has_uint16_dict_key_n
 
Utility Operations
------------------
 
.. doxygenfunction:: clear_uint16_dict
.. doxygenfunction:: copy_uint16_dict
.. doxygenfunction:: merge_uint16_dict
 
Iteration
---------
 
:c:func:`foreach_uint16_dict` visits every entry in bucket order (which is
not guaranteed to match insertion order).  The callback receives the key as
a null-terminated ``const char*`` pointer into the dict's internal storage,
the key length, the ``uint16_t`` value, and an optional caller-supplied
context pointer.  The callback must not insert or remove entries during
traversal.
 
.. code-block:: c
 
   static void print_entry(const char* key, size_t key_len,
                            uint16_t value, void* ud) {
       (void)key_len; (void)ud;
       printf("  %s = %u\n", key, (unsigned)value);
   }
 
   foreach_uint16_dict(d, print_entry, NULL);
 
.. doxygenfunction:: foreach_uint16_dict
 
Introspection
-------------
 
.. doxygenfunction:: uint16_dict_size
.. doxygenfunction:: uint16_dict_hash_size
.. doxygenfunction:: uint16_dict_alloc
.. doxygenfunction:: is_uint16_dict_empty

uint16_t Matrix 
===============
