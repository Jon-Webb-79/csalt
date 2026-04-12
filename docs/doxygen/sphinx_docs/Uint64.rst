.. _uint64:

*****************
uint64_t Overview
*****************
The ``uint64_t`` family of data structures in csalt provides type-safe,
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

uint64_t Array
==============
A :c:type:`uint64_array_t` is an ordered sequence of ``uint64_t`` values
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

   #include "c_uint64.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   uint64_array_expect_t r = init_uint64_array(8, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   uint64_array_t* arr = r.u.value;

   push_back_uint64_array(arr, 30);
   push_back_uint64_array(arr, 10);
   push_back_uint64_array(arr, 20);

   sort_uint64_array(arr, FORWARD);   /* arr is now [10, 20, 30] */

   return_uint64_array(arr);

Structs
-------

.. doxygenstruct:: uint64_array_t
   :members:

.. doxygenstruct:: uint64_array_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_uint64_array

.. doxygenfunction:: return_uint64_array

.. doxygenfunction:: cumulative_uint64_array

Push Operations
---------------

.. doxygenfunction:: push_back_uint64_array

.. doxygenfunction:: push_front_uint64_array

.. doxygenfunction:: push_at_uint64_array

Get and Set
-----------

.. doxygenfunction:: get_uint64_array_index

.. doxygenfunction:: set_uint64_array_index

Pop Operations
--------------

.. doxygenfunction:: pop_back_uint64_array

.. doxygenfunction:: pop_front_uint64_array

.. doxygenfunction:: pop_any_uint64_array

Utility Operations
------------------

.. doxygenfunction:: clear_uint64_array

.. doxygenfunction:: copy_uint64_array

.. doxygenfunction:: concat_uint64_array

.. doxygenfunction:: slice_uint64_array

.. doxygenfunction:: print_uint64_array

Algorithms
----------

.. doxygenfunction:: reverse_uint64_array

.. doxygenfunction:: sort_uint64_array

Search
------

.. doxygenfunction:: uint64_array_contains

.. doxygenfunction:: uint64_array_binary_search 

.. doxygenfunction:: uint64_array_binary_bracket

Introspection
-------------

.. doxygenfunction:: uint64_array_size

.. doxygenfunction:: uint64_array_alloc

.. doxygenfunction:: uint64_array_data_size

.. doxygenfunction:: is_uint64_array_empty

.. doxygenfunction:: is_uint64_array_full

.. doxygenfunction:: is_uint64_array_ptr

.. doxygenfunction:: uint64_array_max

.. doxygenfunction:: uint64_array_min

uint64_t Dictionary 
===================
A ``uint64_dict_t`` is a hash dictionary that maps C-string keys to
``uint64_t`` values.  It is a type-safe wrapper around the generic
dict engine described in :ref:`c_dict`, with the value type fixed to
``sizeof(uint64_t)`` and ``dtype`` fixed to ``UINT64_TYPE`` at initialisation.
 
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
:c:type:`allocator_vtable_t` must be supplied to :c:func:`init_uint64_dict`
and to every :c:func:`insert_uint64_dict` call.  All other operations use
the allocator that was stored at initialisation time.  See
:ref:`allocator_file` for available allocators and the trade-offs between
them.
 
.. code-block:: c
 
   #include "uint64_dict.h"
 
   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t a = heap_allocator();
 
   uint64_dict_expect_t r = init_uint64_dict(16, true, a);
   if (!r.has_value) { /* handle r.u.error */ }
   uint64_dict_t* d = r.u.value;
 
   insert_uint64_dict(d, "bytes", 1099511627776ULL,    a);   /* 1 TiB */
   insert_uint64_dict(d, "hash",  0xDEADBEEFCAFEBABEULL, a);
   insert_uint64_dict(d, "count", 0ULL,                a);
 
   uint64_t v;
   get_uint64_dict_value(d, "bytes", &v);   /* v == 1099511627776 */
 
   update_uint64_dict(d, "count", 1000000000ULL);
 
   pop_uint64_dict(d, "hash", NULL);        /* removes "hash", discards value */
 
   return_uint64_dict(d);
 
Plain vs ``_n`` Variants
------------------------
 
Every function that takes a key is available in two forms.
 
.. list-table::
   :header-rows: 1
   :widths: 45 55
 
   * - Plain variant
     - ``_n`` variant
   * - ``insert_uint64_dict(d, key, value, a)``
     - ``insert_uint64_dict_n(d, key, key_len, value, a)``
   * - ``pop_uint64_dict(d, key, out)``
     - ``pop_uint64_dict_n(d, key, key_len, out)``
   * - ``update_uint64_dict(d, key, value)``
     - ``update_uint64_dict_n(d, key, key_len, value)``
   * - ``get_uint64_dict_value(d, key, out)``
     - ``get_uint64_dict_value_n(d, key, key_len, out)``
   * - ``get_uint64_dict_ptr(d, key)``
     - ``get_uint64_dict_ptr_n(d, key, key_len)``
   * - ``has_uint64_dict_key(d, key)``
     - ``has_uint64_dict_key_n(d, key, key_len)``
 
The ``_n`` variants are particularly useful for splitting on sub-strings
without constructing a null-terminated copy:
 
.. code-block:: c
 
   /* Buffer holds "bytes_total" but we only want "bytes" (5 bytes). */
   const char* buf = "bytes_total";
   insert_uint64_dict_n(d, buf, 5, 1099511627776ULL, a);
 
   uint64_t v;
   get_uint64_dict_value_n(d, buf, 5, &v);   /* v == 1099511627776 */
   get_uint64_dict_value(d, "bytes", &v);     /* same key — also v == 1099511627776 */
 
Structs
-------
 
.. note::
 
   ``uint64_dict_t`` is a ``typedef`` alias for :c:struct:`dict_t`.  All
   internal fields are documented under :c:struct:`dict_t` in
   :ref:`c_dict`.  The ``data_size`` field is always ``sizeof(uint64_t)``
   (8 bytes) and the ``dtype`` field is always ``UINT64_TYPE``.
 
.. doxygenstruct:: uint64_dict_expect_t
   :members:
 
Initialisation and Teardown
---------------------------
 
.. doxygenfunction:: init_uint64_dict
.. doxygenfunction:: return_uint64_dict
 
Insert
------
 
.. doxygenfunction:: insert_uint64_dict
.. doxygenfunction:: insert_uint64_dict_n
 
Pop
---
 
.. doxygenfunction:: pop_uint64_dict
.. doxygenfunction:: pop_uint64_dict_n
 
Update
------
 
.. doxygenfunction:: update_uint64_dict
.. doxygenfunction:: update_uint64_dict_n
 
Lookup
------
 
.. doxygenfunction:: get_uint64_dict_value
.. doxygenfunction:: get_uint64_dict_value_n
.. doxygenfunction:: get_uint64_dict_ptr
.. doxygenfunction:: get_uint64_dict_ptr_n
.. doxygenfunction:: has_uint64_dict_key
.. doxygenfunction:: has_uint64_dict_key_n
 
Utility Operations
------------------
 
.. doxygenfunction:: clear_uint64_dict
.. doxygenfunction:: copy_uint64_dict
.. doxygenfunction:: merge_uint64_dict
.. doxygenfunction:: print_uint64_dict
 
Iteration
---------
 
:c:func:`foreach_uint64_dict` visits every entry in bucket order (which is
not guaranteed to match insertion order).  The callback receives the key as
a null-terminated ``const char*`` pointer into the dict's internal storage,
the key length, the ``uint64_t`` value, and an optional caller-supplied
context pointer.  The callback must not insert or remove entries during
traversal.
 
.. note::
 
   ``uint64_t`` values cannot be safely passed to ``printf`` as ``%u`` — use
   the ``%llu`` specifier with a cast to ``unsigned long long``, or the
   ``PRIu64`` macro from ``<inttypes.h>``.  Similarly, when accumulating
   values inside an iterator callback be aware that two ``UINT64_MAX`` values
   cannot fit in any standard unsigned type — keep accumulator ranges within
   bounds or use checked arithmetic.
 
.. code-block:: c
 
   #include <inttypes.h>
 
   static void print_entry(const char* key, size_t key_len,
                            uint64_t value, void* ud) {
       (void)key_len; (void)ud;
       printf("  %s = %" PRIu64 "\n", key, value);
   }
 
   foreach_uint64_dict(d, print_entry, NULL);
 
.. doxygenfunction:: foreach_uint64_dict
 
Introspection
-------------
 
.. doxygenfunction:: uint64_dict_size
.. doxygenfunction:: uint64_dict_hash_size
.. doxygenfunction:: uint64_dict_alloc
.. doxygenfunction:: is_uint64_dict_empty
 

uint64_t Matrix 
===============
A ``uint64_matrix_t`` is a type-safe wrapper around the generic
:c:type:`matrix_t` container with the runtime ``dtype`` fixed to
``UINT64_TYPE``.  It provides uint64-specific construction, element access,
conversion, comparison, and helper functions while reusing the generic
matrix engine implemented in ``c_matrix.h`` and ``c_matrix.c``.

The wrapper exists for APIs that either:

* fix the ``dtype`` to ``UINT64_TYPE``
* replace ``void*`` element access with ``uint64_t`` / ``uint64_t*``
* provide uint64-specialised helper behaviour such as dense SIMD fast paths

All memory is managed through a caller-supplied
:c:type:`allocator_vtable_t`.  The library does not assume a default
allocator.

Storage Formats
---------------

``uint64_matrix_t`` supports the same storage formats as
:c:type:`matrix_t`:

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Format
     - Description
   * - ``DENSE_MATRIX``
     - Row-major contiguous uint64 buffer.
   * - ``COO_MATRIX``
     - Coordinate list with parallel ``row_idx``, ``col_idx``, and
       ``values`` arrays.  Intended for sparse matrix assembly.
   * - ``CSR_MATRIX``
     - Compressed Sparse Row format for sparse row-oriented operations.
   * - ``CSC_MATRIX``
     - Compressed Sparse Column format for sparse column-oriented operations.

.. note::

   ``set_uint64_matrix`` supports dense and COO matrices.  CSR and CSC are
   treated as derived sparse formats and are not directly mutable through
   element-wise set operations.

uint64 Comparison and Zero Semantics
-----------------------------------

The generic matrix layer supports semantic comparison and semantic zero
testing through callback-style predicates such as
:c:type:`matrix_equal_fn` and :c:type:`matrix_zero_fn`.  These are
documented in ``c_matrix.h`` and are the correct place to describe
custom comparison behaviour. :contentReference[oaicite:7]{index=7}

For the uint64 wrapper specifically:

* ``uint64_matrix_equal`` performs exact logical equality and uses a
  dense SIMD fast path when both inputs are dense uint64 matrices.
* Sparse and mixed-format comparisons fall back to generic element-wise
  comparison through the generic matrix engine.
* ``uint64_matrix_is_zero`` uses a dense SIMD fast path for dense uint64
  matrices and falls back to the generic zero test for sparse formats.

SIMD Fast Paths
---------------

Several dense uint64 matrix operations use SIMD-specialised implementations
when supported by the target platform.  Mixed-format and sparse cases
fall back to the generic matrix implementation.

Current dense-uint64 SIMD fast paths include:

* ``convert_uint64_matrix`` for dense-to-CSR conversion
* ``transpose_uint64_matrix`` for dense transpose
* ``fill_uint64_matrix`` for dense fill
* ``uint64_matrix_is_zero`` for dense zero detection
* ``uint64_matrix_equal`` for dense equality checks 

Example Usage
-------------

.. code-block:: c

   #include "c_uint64.h"

   allocator_vtable_t a = heap_allocator();

   uint64_matrix_expect_t r = init_uint64_dense_matrix(3, 4, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   uint64_matrix_t* m = r.u.value;

   set_uint64_matrix(m, 0, 0, 1u);
   set_uint64_matrix(m, 1, 2, 5u);
   set_uint64_matrix(m, 2, 3, 3u);

   uint64_t out = 0u;
   get_uint64_matrix(m, 1, 2, &out);

   uint64_matrix_expect_t tr = transpose_uint64_matrix(m, a);
   uint64_matrix_expect_t csr = convert_uint64_matrix(m, CSR_MATRIX, a);

   return_uint64_matrix(csr.u.value);
   return_uint64_matrix(tr.u.value);
   return_uint64_matrix(m);

.. code-block:: c

   uint64_matrix_expect_t r = init_uint64_coo_matrix(100, 100, 16, true, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   uint64_matrix_t* sp = r.u.value;

   push_back_uint64_coo_matrix(sp, 0, 5, 3u);
   push_back_uint64_coo_matrix(sp, 42, 99, 1u);
   sort_uint64_coo_matrix(sp);

   uint64_t v = 0u;
   get_uint64_matrix(sp, 10, 10, &v);   /* implicit zero */

   return_uint64_matrix(sp);

Structs
-------

.. note::

   ``uint64_matrix_t`` is a ``typedef`` alias for :c:struct:`matrix_t`.
   The underlying generic storage layout is documented in :ref:`matrix`.

.. doxygenstruct:: uint64_matrix_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_uint64_dense_matrix
.. doxygenfunction:: init_uint64_coo_matrix
.. doxygenfunction:: return_uint64_matrix

Element Access
--------------

.. doxygenfunction:: get_uint64_matrix
.. doxygenfunction:: set_uint64_matrix

COO Assembly
------------

These functions are only valid for COO-format matrices.

.. doxygenfunction:: reserve_uint64_coo_matrix
.. doxygenfunction:: push_back_uint64_coo_matrix
.. doxygenfunction:: sort_uint64_coo_matrix

Lifecycle / Structural Operations
---------------------------------

.. doxygenfunction:: clear_uint64_matrix
.. doxygenfunction:: copy_uint64_matrix
.. doxygenfunction:: convert_uint64_matrix
.. doxygenfunction:: convert_uint64_matrix_zero
.. doxygenfunction:: transpose_uint64_matrix

Fill
----

.. doxygenfunction:: fill_uint64_matrix

Shape and Compatibility
-----------------------

.. doxygenfunction:: uint64_matrix_has_same_shape
.. doxygenfunction:: uint64_matrix_is_square
.. doxygenfunction:: uint64_matrix_is_sparse
.. doxygenfunction:: uint64_matrix_is_zero
.. doxygenfunction:: uint64_matrix_equal
.. doxygenfunction:: uint64_matrix_is_add_compatible
.. doxygenfunction:: uint64_matrix_is_multiply_compatible

Row and Column Swaps
--------------------

.. doxygenfunction:: swap_uint64_matrix_rows
.. doxygenfunction:: swap_uint64_matrix_cols

Special Constructors
--------------------

.. doxygenfunction:: init_uint64_identity_matrix
.. doxygenfunction:: init_uint64_row_vector
.. doxygenfunction:: init_uint64_col_vector

Vector Queries
--------------

.. doxygenfunction:: uint64_matrix_is_row_vector
.. doxygenfunction:: uint64_matrix_is_col_vector
.. doxygenfunction:: uint64_matrix_is_vector
.. doxygenfunction:: uint64_matrix_vector_length

Print 
-----

.. doxygenfunction:: print_uint64_matrix

Generic Introspection
---------------------

Matrix introspection is documented in ``c_matrix.h`` rather than repeated
for each typed wrapper.  Users should call the generic matrix functions
directly:

.. doxygenfunction:: matrix_rows
.. doxygenfunction:: matrix_cols
.. doxygenfunction:: matrix_data_size
.. doxygenfunction:: matrix_dtype
.. doxygenfunction:: matrix_format
.. doxygenfunction:: matrix_nnz

.. note::

   These functions are type-agnostic and therefore are not duplicated as
   uint64-specific APIs in the public documentation.
