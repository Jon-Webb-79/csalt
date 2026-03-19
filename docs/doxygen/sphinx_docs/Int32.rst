.. _int32:

****************
int32_t Overview
****************
The ``int32_t`` family of data structures in csalt provides type-safe,
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

int32_t Array
=============
A :c:type:`int32_array_t` is an ordered sequence of ``int32_t`` values
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

   #include "c_int32.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   int32_array_expect_t r = init_int32_array(8, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   int32_array_t* arr = r.u.value;

   push_back_int32_array(arr, 30);
   push_back_int32_array(arr, 10);
   push_back_int32_array(arr, 20);

   sort_int32_array(arr, FORWARD);   /* arr is now [10, 20, 30] */

   return_int32_array(arr);

Structs
-------

.. doxygenstruct:: int32_array_t
   :members:

.. doxygenstruct:: int32_array_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_int32_array

.. doxygenfunction:: return_int32_array

.. doxygenfunction:: cumulative_int32_array

Push Operations
---------------

.. doxygenfunction:: push_back_int32_array

.. doxygenfunction:: push_front_int32_array

.. doxygenfunction:: push_at_int32_array

Get and Set
-----------

.. doxygenfunction:: get_int32_array_index

.. doxygenfunction:: set_int32_array_index

Pop Operations
--------------

.. doxygenfunction:: pop_back_int32_array

.. doxygenfunction:: pop_front_int32_array

.. doxygenfunction:: pop_any_int32_array

Utility Operations
------------------

.. doxygenfunction:: clear_int32_array

.. doxygenfunction:: copy_int32_array

.. doxygenfunction:: concat_int32_array

.. doxygenfunction:: slice_int32_array

Algorithms
----------

.. doxygenfunction:: reverse_int32_array

.. doxygenfunction:: sort_int32_array

Search
------

.. doxygenfunction:: int32_array_contains

.. doxygenfunction:: int32_array_binary_search 

.. doxygenfunction:: int32_array_binary_bracket

Introspection
-------------

.. doxygenfunction:: int32_array_size

.. doxygenfunction:: int32_array_alloc

.. doxygenfunction:: int32_array_data_size

.. doxygenfunction:: is_int32_array_empty

.. doxygenfunction:: is_int32_array_full

.. doxygenfunction:: is_int32_array_ptr

.. doxygenfunction:: int32_array_max

.. doxygenfunction:: int32_array_min

int32_t Dictionary 
==================
An ``int32_dict_t`` is a hash dictionary that maps C-string keys to
``int32_t`` values.  It is a type-safe wrapper around the generic
dict engine described in :ref:`c_dict`, with the value type fixed to
``sizeof(int32_t)`` and ``dtype`` fixed to ``INT32_TYPE`` at initialisation.
 
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
:c:type:`allocator_vtable_t` must be supplied to :c:func:`init_int32_dict`
and to every :c:func:`insert_int32_dict` call.  All other operations use
the allocator that was stored at initialisation time.  See
:ref:`allocator_file` for available allocators and the trade-offs between
them.
 
.. code-block:: c
 
   #include "int32_dict.h"
 
   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t a = heap_allocator();
 
   int32_dict_expect_t r = init_int32_dict(16, true, a);
   if (!r.has_value) { /* handle r.u.error */ }
   int32_dict_t* d = r.u.value;
 
   insert_int32_dict(d, "altitude",  -10000, a);   /* metres below sea level */
   insert_int32_dict(d, "score",    -2147483648, a);   /* INT32_MIN */
   insert_int32_dict(d, "offset",           0, a);
 
   int32_t v;
   get_int32_dict_value(d, "altitude", &v);   /* v == -10000 */
 
   update_int32_dict(d, "offset", -100000);
 
   pop_int32_dict(d, "score", NULL);           /* removes "score", discards value */
 
   return_int32_dict(d);
 
Plain vs ``_n`` Variants
------------------------
 
Every function that takes a key is available in two forms.
 
.. list-table::
   :header-rows: 1
   :widths: 45 55
 
   * - Plain variant
     - ``_n`` variant
   * - ``insert_int32_dict(d, key, value, a)``
     - ``insert_int32_dict_n(d, key, key_len, value, a)``
   * - ``pop_int32_dict(d, key, out)``
     - ``pop_int32_dict_n(d, key, key_len, out)``
   * - ``update_int32_dict(d, key, value)``
     - ``update_int32_dict_n(d, key, key_len, value)``
   * - ``get_int32_dict_value(d, key, out)``
     - ``get_int32_dict_value_n(d, key, key_len, out)``
   * - ``get_int32_dict_ptr(d, key)``
     - ``get_int32_dict_ptr_n(d, key, key_len)``
   * - ``has_int32_dict_key(d, key)``
     - ``has_int32_dict_key_n(d, key, key_len)``
 
The ``_n`` variants are particularly useful for splitting on sub-strings
without constructing a null-terminated copy:
 
.. code-block:: c
 
   /* Buffer holds "delta_total" but we only want "delta" (5 bytes). */
   const char* buf = "delta_total";
   insert_int32_dict_n(d, buf, 5, -500000, a);
 
   int32_t v;
   get_int32_dict_value_n(d, buf, 5, &v);   /* v == -500000 */
   get_int32_dict_value(d, "delta", &v);     /* same key — also v == -500000 */
 
Structs
-------
 
.. note::
 
   ``int32_dict_t`` is a ``typedef`` alias for :c:struct:`dict_t`.  All
   internal fields are documented under :c:struct:`dict_t` in
   :ref:`c_dict`.  The ``data_size`` field is always ``sizeof(int32_t)``
   (4 bytes) and the ``dtype`` field is always ``INT32_TYPE``.
 
.. doxygenstruct:: int32_dict_expect_t
   :members:
 
Initialisation and Teardown
---------------------------
 
.. doxygenfunction:: init_int32_dict
.. doxygenfunction:: return_int32_dict
 
Insert
------
 
.. doxygenfunction:: insert_int32_dict
.. doxygenfunction:: insert_int32_dict_n
 
Pop
---
 
.. doxygenfunction:: pop_int32_dict
.. doxygenfunction:: pop_int32_dict_n
 
Update
------
 
.. doxygenfunction:: update_int32_dict
.. doxygenfunction:: update_int32_dict_n
 
Lookup
------
 
.. doxygenfunction:: get_int32_dict_value
.. doxygenfunction:: get_int32_dict_value_n
.. doxygenfunction:: get_int32_dict_ptr
.. doxygenfunction:: get_int32_dict_ptr_n
.. doxygenfunction:: has_int32_dict_key
.. doxygenfunction:: has_int32_dict_key_n
 
Utility Operations
------------------
 
.. doxygenfunction:: clear_int32_dict
.. doxygenfunction:: copy_int32_dict
.. doxygenfunction:: merge_int32_dict
 
Iteration
---------
 
:c:func:`foreach_int32_dict` visits every entry in bucket order (which is
not guaranteed to match insertion order).  The callback receives the key as
a null-terminated ``const char*`` pointer into the dict's internal storage,
the key length, the ``int32_t`` value, and an optional caller-supplied
context pointer.  The callback must not insert or remove entries during
traversal.
 
.. note::
 
   When accumulating ``int32_t`` values inside an iterator callback, use an
   ``int64_t`` accumulator to avoid overflow — summing values near
   ``INT32_MIN`` or ``INT32_MAX`` can exceed the 32-bit signed range.
 
.. code-block:: c
 
   static void print_entry(const char* key, size_t key_len,
                            int32_t value, void* ud) {
       (void)key_len; (void)ud;
       printf("  %s = %d\n", key, value);
   }
 
   foreach_int32_dict(d, print_entry, NULL);
 
.. doxygenfunction:: foreach_int32_dict
 
Introspection
-------------
 
.. doxygenfunction:: int32_dict_size
.. doxygenfunction:: int32_dict_hash_size
.. doxygenfunction:: int32_dict_alloc
.. doxygenfunction:: is_int32_dict_empty

int32_t Matrix 
==============
