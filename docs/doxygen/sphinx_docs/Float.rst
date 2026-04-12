.. _float_type:

**************
float Overview
**************
The ``float`` family of data structures in csalt provides type-safe,
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

float Array
===========
A :c:type:`float_array_t` is an ordered sequence of ``float`` values
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

   #include "c_float.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   float_array_expect_t r = init_float_array(8, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   float_array_t* arr = r.u.value;

   push_back_float_array(arr, 30);
   push_back_float_array(arr, 10);
   push_back_float_array(arr, 20);

   sort_float_array(arr, FORWARD);   /* arr is now [10, 20, 30] */

   return_float_array(arr);

Structs
-------

.. doxygenstruct:: float_array_t
   :members:

.. doxygenstruct:: float_array_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_float_array

.. doxygenfunction:: return_float_array

.. doxygenfunction:: cumulative_float_array

Push Operations
---------------

.. doxygenfunction:: push_back_float_array

.. doxygenfunction:: push_front_float_array

.. doxygenfunction:: push_at_float_array

Get and Set
-----------

.. doxygenfunction:: get_float_array_index

.. doxygenfunction:: set_float_array_index

Pop Operations
--------------

.. doxygenfunction:: pop_back_float_array

.. doxygenfunction:: pop_front_float_array

.. doxygenfunction:: pop_any_float_array

Utility Operations
------------------

.. doxygenfunction:: clear_float_array

.. doxygenfunction:: copy_float_array

.. doxygenfunction:: concat_float_array

.. doxygenfunction:: slice_float_array

.. doxygenfunction:: print_float_array

Algorithms
----------

.. doxygenfunction:: reverse_float_array

.. doxygenfunction:: sort_float_array

Search
------

.. doxygenfunction:: float_array_contains

.. doxygenfunction:: float_array_binary_search 

.. doxygenfunction:: float_array_binary_bracket

Introspection
-------------

.. doxygenfunction:: float_array_size

.. doxygenfunction:: float_array_alloc

.. doxygenfunction:: float_array_data_size

.. doxygenfunction:: is_float_array_empty

.. doxygenfunction:: is_float_array_full

.. doxygenfunction:: is_float_array_ptr

.. doxygenfunction:: float_array_max

.. doxygenfunction:: float_array_min

float Dictionary 
================
A ``float_dict_t`` is a hash dictionary that maps C-string keys to ``float``
values.  It is a type-safe wrapper around the generic dict engine described
in :ref:`c_dict`, with the value type fixed to ``sizeof(float)`` and
``dtype`` fixed to ``FLOAT_TYPE`` at initialisation.
 
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
:c:type:`allocator_vtable_t` must be supplied to :c:func:`init_float_dict`
and to every :c:func:`insert_float_dict` call.  All other operations use
the allocator that was stored at initialisation time.  See
:ref:`allocator_file` for available allocators and the trade-offs between
them.
 
.. code-block:: c
 
   #include "float_dict.h"
 
   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t a = heap_allocator();
 
   float_dict_expect_t r = init_float_dict(16, true, a);
   if (!r.has_value) { /* handle r.u.error */ }
   float_dict_t* d = r.u.value;
 
   insert_float_dict(d, "pi",      3.14159265f, a);
   insert_float_dict(d, "gravity", 9.80665f,    a);
   insert_float_dict(d, "zero",    0.0f,        a);
 
   float v;
   get_float_dict_value(d, "pi", &v);   /* v == 3.14159265f */
 
   update_float_dict(d, "zero", -1.0f);
 
   pop_float_dict(d, "gravity", NULL);  /* removes "gravity", discards value */
 
   return_float_dict(d);
 
Plain vs ``_n`` Variants
------------------------
 
Every function that takes a key is available in two forms.
 
.. list-table::
   :header-rows: 1
   :widths: 45 55
 
   * - Plain variant
     - ``_n`` variant
   * - ``insert_float_dict(d, key, value, a)``
     - ``insert_float_dict_n(d, key, key_len, value, a)``
   * - ``pop_float_dict(d, key, out)``
     - ``pop_float_dict_n(d, key, key_len, out)``
   * - ``update_float_dict(d, key, value)``
     - ``update_float_dict_n(d, key, key_len, value)``
   * - ``get_float_dict_value(d, key, out)``
     - ``get_float_dict_value_n(d, key, key_len, out)``
   * - ``get_float_dict_ptr(d, key)``
     - ``get_float_dict_ptr_n(d, key, key_len)``
   * - ``has_float_dict_key(d, key)``
     - ``has_float_dict_key_n(d, key, key_len)``
 
The ``_n`` variants are particularly useful for splitting on sub-strings
without constructing a null-terminated copy:
 
.. code-block:: c
 
   /* Buffer holds "scale_factor" but we only want "scale" (5 bytes). */
   const char* buf = "scale_factor";
   insert_float_dict_n(d, buf, 5, 0.5f, a);
 
   float v;
   get_float_dict_value_n(d, buf, 5, &v);   /* v == 0.5f */
   get_float_dict_value(d, "scale", &v);     /* same key — also v == 0.5f */
 
Structs
-------
 
.. note::
 
   ``float_dict_t`` is a ``typedef`` alias for :c:struct:`dict_t`.  All
   internal fields are documented under :c:struct:`dict_t` in
   :ref:`c_dict`.  The ``data_size`` field is always ``sizeof(float)``
   (4 bytes) and the ``dtype`` field is always ``FLOAT_TYPE``.
 
.. doxygenstruct:: float_dict_expect_t
   :members:
 
Initialisation and Teardown
---------------------------
 
.. doxygenfunction:: init_float_dict
.. doxygenfunction:: return_float_dict
 
Insert
------
 
.. doxygenfunction:: insert_float_dict
.. doxygenfunction:: insert_float_dict_n
 
Pop
---
 
.. doxygenfunction:: pop_float_dict
.. doxygenfunction:: pop_float_dict_n
 
Update
------
 
.. doxygenfunction:: update_float_dict
.. doxygenfunction:: update_float_dict_n
 
Lookup
------
 
.. doxygenfunction:: get_float_dict_value
.. doxygenfunction:: get_float_dict_value_n
.. doxygenfunction:: get_float_dict_ptr
.. doxygenfunction:: get_float_dict_ptr_n
.. doxygenfunction:: has_float_dict_key
.. doxygenfunction:: has_float_dict_key_n
 
Utility Operations
------------------
 
.. doxygenfunction:: clear_float_dict
.. doxygenfunction:: copy_float_dict
.. doxygenfunction:: merge_float_dict
.. doxygenfunction:: print_float_dict
 
Iteration
---------
 
:c:func:`foreach_float_dict` visits every entry in bucket order (which is
not guaranteed to match insertion order).  The callback receives the key as
a null-terminated ``const char*`` pointer into the dict's internal storage,
the key length, the ``float`` value, and an optional caller-supplied context
pointer.  The callback must not insert or remove entries during traversal.
 
.. note::
 
   Values are stored and retrieved as exact IEEE 754 bit patterns via
   ``memcpy``.  No epsilon tolerance is applied — equality comparisons
   between stored and retrieved values are exact.  If you insert a NaN,
   you will retrieve that same NaN bit pattern; however, NaN keys cannot
   be looked up reliably because ``NaN != NaN`` by definition, so avoid
   using NaN as a stored value if equality checks matter downstream.
 
   When accumulating ``float`` values inside an iterator callback, use a
   ``double`` accumulator to reduce rounding error.
 
.. code-block:: c
 
   static void print_entry(const char* key, size_t key_len,
                            float value, void* ud) {
       (void)key_len; (void)ud;
       printf("  %s = %f\n", key, value);
   }
 
   foreach_float_dict(d, print_entry, NULL);
 
.. doxygenfunction:: foreach_float_dict
 
Introspection
-------------
 
.. doxygenfunction:: float_dict_size
.. doxygenfunction:: float_dict_hash_size
.. doxygenfunction:: float_dict_alloc
.. doxygenfunction:: is_float_dict_empty

float Matrix
============
A ``float_matrix_t`` is a type-safe wrapper around the generic
:c:type:`matrix_t` container with the runtime ``dtype`` fixed to
``FLOAT_TYPE``.  It provides float-specific construction, element access,
conversion, comparison, and helper functions while reusing the generic
matrix engine implemented in ``c_matrix.h`` and ``c_matrix.c``.

The wrapper exists for APIs that either:

* fix the ``dtype`` to ``FLOAT_TYPE``
* replace ``void*`` element access with ``float`` / ``float*``
* provide float-specialised helper behaviour such as dense SIMD fast paths

All memory is managed through a caller-supplied
:c:type:`allocator_vtable_t`.  The library does not assume a default
allocator.

Storage Formats
---------------

``float_matrix_t`` supports the same storage formats as
:c:type:`matrix_t`:

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Format
     - Description
   * - ``DENSE_MATRIX``
     - Row-major contiguous float buffer.
   * - ``COO_MATRIX``
     - Coordinate list with parallel ``row_idx``, ``col_idx``, and
       ``values`` arrays.  Intended for sparse matrix assembly.
   * - ``CSR_MATRIX``
     - Compressed Sparse Row format for sparse row-oriented operations.
   * - ``CSC_MATRIX``
     - Compressed Sparse Column format for sparse column-oriented operations.

.. note::

   ``set_float_matrix`` supports dense and COO matrices.  CSR and CSC are
   treated as derived sparse formats and are not directly mutable through
   element-wise set operations.

Float Comparison and Zero Semantics
-----------------------------------

The generic matrix layer supports semantic comparison and semantic zero
testing through callback-style predicates such as
:c:type:`matrix_equal_fn` and :c:type:`matrix_zero_fn`.  These are
documented in ``c_matrix.h`` and are the correct place to describe
custom comparison behaviour. :contentReference[oaicite:7]{index=7}

For the float wrapper specifically:

* ``float_matrix_equal`` performs exact logical equality and uses a
  dense SIMD fast path when both inputs are dense float matrices.
* Sparse and mixed-format comparisons fall back to generic element-wise
  comparison through the generic matrix engine.
* ``float_matrix_is_zero`` uses a dense SIMD fast path for dense float
  matrices and falls back to the generic zero test for sparse formats.

.. note::

   Exact float equality is appropriate for structural container checks,
   but applications that require tolerance-based numerical comparison
   should document and apply their own comparison policy at a higher
   level.

SIMD Fast Paths
---------------

Several dense float matrix operations use SIMD-specialised implementations
when supported by the target platform.  Mixed-format and sparse cases
fall back to the generic matrix implementation.

Current dense-float SIMD fast paths include:

* ``convert_float_matrix`` for dense-to-CSR conversion
* ``transpose_float_matrix`` for dense transpose
* ``fill_float_matrix`` for dense fill
* ``float_matrix_is_zero`` for dense zero detection
* ``float_matrix_equal`` for dense equality checks 

Example Usage
-------------

.. code-block:: c

   #include "c_float.h"

   allocator_vtable_t a = heap_allocator();

   float_matrix_expect_t r = init_float_dense_matrix(3, 4, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   float_matrix_t* m = r.u.value;

   set_float_matrix(m, 0, 0, 1.0f);
   set_float_matrix(m, 1, 2, 5.5f);
   set_float_matrix(m, 2, 3, -3.0f);

   float out = 0.0f;
   get_float_matrix(m, 1, 2, &out);

   float_matrix_expect_t tr = transpose_float_matrix(m, a);
   float_matrix_expect_t csr = convert_float_matrix(m, CSR_MATRIX, a);

   return_float_matrix(csr.u.value);
   return_float_matrix(tr.u.value);
   return_float_matrix(m);

.. code-block:: c

   float_matrix_expect_t r = init_float_coo_matrix(100, 100, 16, true, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   float_matrix_t* sp = r.u.value;

   push_back_float_coo_matrix(sp, 0, 5, 3.14f);
   push_back_float_coo_matrix(sp, 42, 99, -1.0f);
   sort_float_coo_matrix(sp);

   float v = 0.0f;
   get_float_matrix(sp, 10, 10, &v);   /* implicit zero */

   return_float_matrix(sp);

Structs
-------

.. note::

   ``float_matrix_t`` is a ``typedef`` alias for :c:struct:`matrix_t`.
   The underlying generic storage layout is documented in :ref:`matrix`.

.. doxygenstruct:: float_matrix_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_float_dense_matrix
.. doxygenfunction:: init_float_coo_matrix
.. doxygenfunction:: return_float_matrix

Element Access
--------------

.. doxygenfunction:: get_float_matrix
.. doxygenfunction:: set_float_matrix

COO Assembly
------------

These functions are only valid for COO-format matrices.

.. doxygenfunction:: reserve_float_coo_matrix
.. doxygenfunction:: push_back_float_coo_matrix
.. doxygenfunction:: sort_float_coo_matrix

Lifecycle / Structural Operations
---------------------------------

.. doxygenfunction:: clear_float_matrix
.. doxygenfunction:: copy_float_matrix
.. doxygenfunction:: convert_float_matrix
.. doxygenfunction:: convert_float_matrix_zero
.. doxygenfunction:: transpose_float_matrix

Fill
----

.. doxygenfunction:: fill_float_matrix

Shape and Compatibility
-----------------------

.. doxygenfunction:: float_matrix_has_same_shape
.. doxygenfunction:: float_matrix_is_square
.. doxygenfunction:: float_matrix_is_sparse
.. doxygenfunction:: float_matrix_is_zero
.. doxygenfunction:: float_matrix_equal
.. doxygenfunction:: float_matrix_is_add_compatible
.. doxygenfunction:: float_matrix_is_multiply_compatible

Row and Column Swaps
--------------------

.. doxygenfunction:: swap_float_matrix_rows
.. doxygenfunction:: swap_float_matrix_cols

Special Constructors
--------------------

.. doxygenfunction:: init_float_identity_matrix
.. doxygenfunction:: init_float_row_vector
.. doxygenfunction:: init_float_col_vector

Vector Queries
--------------

.. doxygenfunction:: float_matrix_is_row_vector
.. doxygenfunction:: float_matrix_is_col_vector
.. doxygenfunction:: float_matrix_is_vector
.. doxygenfunction:: float_matrix_vector_length

Print 
-----

.. doxygenfunction:: print_float_matrix

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
   float-specific APIs in the public documentation.
