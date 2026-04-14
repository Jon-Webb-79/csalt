.. _double_type:

***************
double Overview
***************
The ``double`` family of data structures in csalt provides type-safe,
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

double Array
============
A :c:type:`double_array_t` is an ordered sequence of ``double`` values
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

   #include "c_double.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   double_array_expect_t r = init_double_array(8, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   double_array_t* arr = r.u.value;

   push_back_double_array(arr, 30);
   push_back_double_array(arr, 10);
   push_back_double_array(arr, 20);

   sort_double_array(arr, FORWARD);   /* arr is now [10, 20, 30] */

   return_double_array(arr);

Structs
-------

.. doxygenstruct:: double_array_t
   :members:

.. doxygenstruct:: double_array_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_double_array

.. doxygenfunction:: return_double_array

.. doxygenfunction:: cumulative_double_array

Push Operations
---------------

.. doxygenfunction:: push_back_double_array

.. doxygenfunction:: push_front_double_array

.. doxygenfunction:: push_at_double_array

Get and Set
-----------

.. doxygenfunction:: get_double_array_index

.. doxygenfunction:: set_double_array_index

Pop Operations
--------------

.. doxygenfunction:: pop_back_double_array

.. doxygenfunction:: pop_front_double_array

.. doxygenfunction:: pop_any_double_array

Utility Operations
------------------

.. doxygenfunction:: clear_double_array

.. doxygenfunction:: copy_double_array

.. doxygenfunction:: concat_double_array

.. doxygenfunction:: slice_double_array

.. doxygenfunction:: print_double_array

.. doxygenfunction:: double_array_equal

Algorithms
----------

.. doxygenfunction:: reverse_double_array

.. doxygenfunction:: sort_double_array

Search
------

.. doxygenfunction:: double_array_contains

.. doxygenfunction:: double_array_binary_search 

.. doxygenfunction:: double_array_binary_bracket

Introspection
-------------

.. doxygenfunction:: double_array_size

.. doxygenfunction:: double_array_alloc

.. doxygenfunction:: double_array_data_size

.. doxygenfunction:: is_double_array_empty

.. doxygenfunction:: is_double_array_full

.. doxygenfunction:: is_double_array_ptr

.. doxygenfunction:: double_array_max

.. doxygenfunction:: double_array_min

double Dictionary 
=================
A ``double_dict_t`` is a hash dictionary that maps C-string keys to ``double``
values.  It is a type-safe wrapper around the generic dict engine described
in :ref:`c_dict`, with the value type fixed to ``sizeof(double)`` and
``dtype`` fixed to ``DOUBLE_TYPE`` at initialisation.
 
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
:c:type:`allocator_vtable_t` must be supplied to :c:func:`init_double_dict`
and to every :c:func:`insert_double_dict` call.  All other operations use
the allocator that was stored at initialisation time.  See
:ref:`allocator_file` for available allocators and the trade-offs between
them.
 
.. code-block:: c
 
   #include "double_dict.h"
 
   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t a = heap_allocator();
 
   double_dict_expect_t r = init_double_dict(16, true, a);
   if (!r.has_value) { /* handle r.u.error */ }
   double_dict_t* d = r.u.value;
 
   insert_double_dict(d, "pi",      3.141592653589793, a);
   insert_double_dict(d, "gravity", 9.80665,           a);
   insert_double_dict(d, "zero",    0.0,               a);
 
   double v;
   get_double_dict_value(d, "pi", &v);   /* v == 3.141592653589793 */
 
   update_double_dict(d, "zero", -1.0);
 
   pop_double_dict(d, "gravity", NULL);  /* removes "gravity", discards value */
 
   return_double_dict(d);
 
Plain vs ``_n`` Variants
------------------------
 
Every function that takes a key is available in two forms.
 
.. list-table::
   :header-rows: 1
   :widths: 45 55
 
   * - Plain variant
     - ``_n`` variant
   * - ``insert_double_dict(d, key, value, a)``
     - ``insert_double_dict_n(d, key, key_len, value, a)``
   * - ``pop_double_dict(d, key, out)``
     - ``pop_double_dict_n(d, key, key_len, out)``
   * - ``update_double_dict(d, key, value)``
     - ``update_double_dict_n(d, key, key_len, value)``
   * - ``get_double_dict_value(d, key, out)``
     - ``get_double_dict_value_n(d, key, key_len, out)``
   * - ``get_double_dict_ptr(d, key)``
     - ``get_double_dict_ptr_n(d, key, key_len)``
   * - ``has_double_dict_key(d, key)``
     - ``has_double_dict_key_n(d, key, key_len)``
 
The ``_n`` variants are particularly useful for splitting on sub-strings
without constructing a null-terminated copy:
 
.. code-block:: c
 
   /* Buffer holds "scale_factor" but we only want "scale" (5 bytes). */
   const char* buf = "scale_factor";
   insert_double_dict_n(d, buf, 5, 0.123456789012345, a);
 
   double v;
   get_double_dict_value_n(d, buf, 5, &v);   /* v == 0.123456789012345 */
   get_double_dict_value(d, "scale", &v);     /* same key — also v == 0.123456789012345 */
 
Structs
-------
 
.. note::
 
   ``double_dict_t`` is a ``typedef`` alias for :c:struct:`dict_t`.  All
   internal fields are documented under :c:struct:`dict_t` in
   :ref:`c_dict`.  The ``data_size`` field is always ``sizeof(double)``
   (8 bytes) and the ``dtype`` field is always ``DOUBLE_TYPE``.
 
.. doxygenstruct:: double_dict_expect_t
   :members:
 
Initialisation and Teardown
---------------------------
 
.. doxygenfunction:: init_double_dict
.. doxygenfunction:: return_double_dict
 
Insert
------
 
.. doxygenfunction:: insert_double_dict
.. doxygenfunction:: insert_double_dict_n
 
Pop
---
 
.. doxygenfunction:: pop_double_dict
.. doxygenfunction:: pop_double_dict_n
 
Update
------
 
.. doxygenfunction:: update_double_dict
.. doxygenfunction:: update_double_dict_n
 
Lookup
------
 
.. doxygenfunction:: get_double_dict_value
.. doxygenfunction:: get_double_dict_value_n
.. doxygenfunction:: get_double_dict_ptr
.. doxygenfunction:: get_double_dict_ptr_n
.. doxygenfunction:: has_double_dict_key
.. doxygenfunction:: has_double_dict_key_n
 
Utility Operations
------------------
 
.. doxygenfunction:: clear_double_dict
.. doxygenfunction:: copy_double_dict
.. doxygenfunction:: merge_double_dict
.. doxygenfunction:: print_double_dict
 
Iteration
---------
 
:c:func:`foreach_double_dict` visits every entry in bucket order (which is
not guaranteed to match insertion order).  The callback receives the key as
a null-terminated ``const char*`` pointer into the dict's internal storage,
the key length, the ``double`` value, and an optional caller-supplied context
pointer.  The callback must not insert or remove entries during traversal.
 
.. note::
 
   Values are stored and retrieved as exact IEEE 754 bit patterns via
   ``memcpy``.  No epsilon tolerance is applied — equality comparisons
   between stored and retrieved values are exact.  As with :ref:`float`, NaN
   values can be stored and retrieved bitwise, but cannot be looked up
   reliably because ``NaN != NaN`` by definition.
 
   Use ``%.15g`` or the ``%g`` specifier with sufficient precision when
   printing ``double`` values, or the ``PRId64``-style macros are not
   applicable — use ``printf("%.15g", v)`` directly.  A ``double``
   accumulator is sufficient for summing ``double`` values, but be aware
   that adding many values of the same sign can accumulate rounding error;
   prefer Kahan summation for high-precision work.
 
.. code-block:: c
 
   static void print_entry(const char* key, size_t key_len,
                            double value, void* ud) {
       (void)key_len; (void)ud;
       printf("  %s = %.15g\n", key, value);
   }
 
   foreach_double_dict(d, print_entry, NULL);
 
.. doxygenfunction:: foreach_double_dict
 
Introspection
-------------
 
.. doxygenfunction:: double_dict_size
.. doxygenfunction:: double_dict_hash_size
.. doxygenfunction:: double_dict_alloc
.. doxygenfunction:: is_double_dict_empty


double Matrix
=============
A ``double_matrix_t`` is a type-safe wrapper around the generic
:c:type:`matrix_t` container with the runtime ``dtype`` fixed to
``double_TYPE``.  It provides double-specific construction, element access,
conversion, comparison, and helper functions while reusing the generic
matrix engine implemented in ``c_matrix.h`` and ``c_matrix.c``.

The wrapper exists for APIs that either:

* fix the ``dtype`` to ``double_TYPE``
* replace ``void*`` element access with ``double`` / ``double*``
* provide double-specialised helper behaviour such as dense SIMD fast paths

All memory is managed through a caller-supplied
:c:type:`allocator_vtable_t`.  The library does not assume a default
allocator.

Storage Formats
---------------

``double_matrix_t`` supports the same storage formats as
:c:type:`matrix_t`:

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Format
     - Description
   * - ``DENSE_MATRIX``
     - Row-major contiguous double buffer.
   * - ``COO_MATRIX``
     - Coordinate list with parallel ``row_idx``, ``col_idx``, and
       ``values`` arrays.  Intended for sparse matrix assembly.
   * - ``CSR_MATRIX``
     - Compressed Sparse Row format for sparse row-oriented operations.
   * - ``CSC_MATRIX``
     - Compressed Sparse Column format for sparse column-oriented operations.

.. note::

   ``set_double_matrix`` supports dense and COO matrices.  CSR and CSC are
   treated as derived sparse formats and are not directly mutable through
   element-wise set operations.

double Comparison and Zero Semantics
------------------------------------

The generic matrix layer supports semantic comparison and semantic zero
testing through callback-style predicates such as
:c:type:`matrix_equal_fn` and :c:type:`matrix_zero_fn`.  These are
documented in ``c_matrix.h`` and are the correct place to describe
custom comparison behaviour. :contentReference[oaicite:7]{index=7}

For the double wrapper specifically:

* ``double_matrix_equal`` performs exact logical equality and uses a
  dense SIMD fast path when both inputs are dense double matrices.
* Sparse and mixed-format comparisons fall back to generic element-wise
  comparison through the generic matrix engine.
* ``double_matrix_is_zero`` uses a dense SIMD fast path for dense double
  matrices and falls back to the generic zero test for sparse formats.

.. note::

   Exact double equality is appropriate for structural container checks,
   but applications that require tolerance-based numerical comparison
   should document and apply their own comparison policy at a higher
   level.

SIMD Fast Paths
---------------

Several dense double matrix operations use SIMD-specialised implementations
when supported by the target platform.  Mixed-format and sparse cases
fall back to the generic matrix implementation.

Current dense-double SIMD fast paths include:

* ``convert_double_matrix`` for dense-to-CSR conversion
* ``transpose_double_matrix`` for dense transpose
* ``fill_double_matrix`` for dense fill
* ``double_matrix_is_zero`` for dense zero detection
* ``double_matrix_equal`` for dense equality checks 

Example Usage
-------------

.. code-block:: c

   #include "c_double.h"

   allocator_vtable_t a = heap_allocator();

   double_matrix_expect_t r = init_double_dense_matrix(3, 4, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   double_matrix_t* m = r.u.value;

   set_double_matrix(m, 0, 0, 1.0);
   set_double_matrix(m, 1, 2, 5.5);
   set_double_matrix(m, 2, 3, -3.0);

   double out = 0.0f;
   get_double_matrix(m, 1, 2, &out);

   double_matrix_expect_t tr = transpose_double_matrix(m, a);
   double_matrix_expect_t csr = convert_double_matrix(m, CSR_MATRIX, a);

   return_double_matrix(csr.u.value);
   return_double_matrix(tr.u.value);
   return_double_matrix(m);

.. code-block:: c

   double_matrix_expect_t r = init_double_coo_matrix(100, 100, 16, true, a);
   if (!r.has_value) {
       /* handle r.u.error */
   }

   double_matrix_t* sp = r.u.value;

   push_back_double_coo_matrix(sp, 0, 5, 3.14);
   push_back_double_coo_matrix(sp, 42, 99, -1.0);
   sort_double_coo_matrix(sp);

   double v = 0.0f;
   get_double_matrix(sp, 10, 10, &v);   /* implicit zero */

   return_double_matrix(sp);

Structs
-------

.. note::

   ``double_matrix_t`` is a ``typedef`` alias for :c:struct:`matrix_t`.
   The underlying generic storage layout is documented in :ref:`matrix`.

.. doxygenstruct:: double_matrix_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_double_dense_matrix
.. doxygenfunction:: init_double_coo_matrix
.. doxygenfunction:: return_double_matrix

Element Access
--------------

.. doxygenfunction:: get_double_matrix
.. doxygenfunction:: set_double_matrix

COO Assembly
------------

These functions are only valid for COO-format matrices.

.. doxygenfunction:: reserve_double_coo_matrix
.. doxygenfunction:: push_back_double_coo_matrix
.. doxygenfunction:: sort_double_coo_matrix

Lifecycle / Structural Operations
---------------------------------

.. doxygenfunction:: clear_double_matrix
.. doxygenfunction:: copy_double_matrix
.. doxygenfunction:: convert_double_matrix
.. doxygenfunction:: convert_double_matrix_zero
.. doxygenfunction:: transpose_double_matrix

Fill
----

.. doxygenfunction:: fill_double_matrix

Shape and Compatibility
-----------------------

.. doxygenfunction:: double_matrix_has_same_shape
.. doxygenfunction:: double_matrix_is_square
.. doxygenfunction:: double_matrix_is_sparse
.. doxygenfunction:: double_matrix_is_zero
.. doxygenfunction:: double_matrix_equal
.. doxygenfunction:: double_matrix_is_add_compatible
.. doxygenfunction:: double_matrix_is_multiply_compatible

Row and Column Swaps
--------------------

.. doxygenfunction:: swap_double_matrix_rows
.. doxygenfunction:: swap_double_matrix_cols

Special Constructors
--------------------

.. doxygenfunction:: init_double_identity_matrix
.. doxygenfunction:: init_double_row_vector
.. doxygenfunction:: init_double_col_vector

Vector Queries
--------------

.. doxygenfunction:: double_matrix_is_row_vector
.. doxygenfunction:: double_matrix_is_col_vector
.. doxygenfunction:: double_matrix_is_vector
.. doxygenfunction:: double_matrix_vector_length

Print 
-----

.. doxygenfunction:: print_double_matrix

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
   double-specific APIs in the public documentation.
