.. _int64:

****************
int64_t Overview
****************

The ``int64_t`` family of data structures in csalt provides type-safe,
allocator-agnostic containers for 64-bit integer data. Every
structure wraps the generic ``tensor_t`` engine from ``c_tensor.h`` with
the element type fixed to ``INT64_TYPE`` at initialisation, eliminating
the need for the caller to pass a ``dtype_id_t`` argument to any operation.

All memory is managed through an :c:type:`allocator_vtable_t` supplied by
the caller at initialisation time. csalt does not assume a default
allocator — the caller must explicitly provide one at every call site that
allocates memory (initialisation, copy, and slice). The library ships
several ready-made implementations and the interface is open so that custom
allocators can be substituted without changing any call sites. See
:ref:`allocator_file` for the full allocator API, the list of available
implementations, and guidance on writing your own.

Error handling follows the *expected value* pattern throughout: operations
that produce a new object return a ``int64_tensor_expect_t`` whose
``has_value`` field distinguishes success from failure, while in-place
mutations return an :c:type:`error_code_t` directly. Callers must always
check the result before using the value.

.. code-block:: c

   int64_tensor_expect_t r = init_int64_array(8, true, heap_allocator());
   if (!r.has_value) {
       fprintf(stderr, "%s\n", error_to_string(r.u.error));
   }
   int64_tensor_t* arr = r.u.value;
   /* ... use arr ... */
   return_int64_tensor(arr);

.. note::

   Functions documented under :ref:`int64_tensor` operate on any
   ``int64_tensor_t`` regardless of how it was initialised. Functions
   documented under :ref:`int64_array` are only valid for tensors
   initialised via ``init_int64_array`` — they require
   ``mode == ARRAY_STRUCT`` and will return ``PRECONDITION_FAIL`` if
   called on a tensor created by ``init_int64_tensor``.

.. _int64_tensor:

int64_t Tensor
==============

The following types and functions operate on any ``int64_tensor_t``
regardless of dimensionality or mode. This includes fixed-shape N-D tensors
created with ``init_int64_tensor`` and dynamic 1-D arrays created with
``init_int64_array``.

Structs
-------

.. doxygenstruct:: int64_tensor_t
   :members:

.. doxygenstruct:: int64_tensor_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_int64_tensor

.. doxygenfunction:: return_int64_tensor

.. doxygenfunction:: copy_int64_tensor

Get and Set
-----------

.. doxygenfunction:: get_int64_tensor_index

.. doxygenfunction:: get_int64_tensor_nd_index

.. doxygenfunction:: set_int64_tensor_index

.. doxygenfunction:: set_int64_tensor_nd_index

Utility Operations
------------------

.. doxygenfunction:: clear_int64_tensor

.. doxygenfunction:: slice_int64_tensor_array

.. doxygenfunction:: reverse_int64_tensor

.. doxygenfunction:: sort_int64_tensor

.. doxygenfunction:: int64_tensor_lsearch

.. doxygenfunction:: int64_tensor_bsearch

.. doxygenfunction:: int64_tensor_bbsearch

Introspection
-------------

.. doxygenfunction:: int64_tensor_size

.. doxygenfunction:: int64_tensor_alloc

.. doxygenfunction:: int64_tensor_data_size

.. doxygenfunction:: int64_tensor_dtype

.. doxygenfunction:: int64_tensor_ndim

.. doxygenfunction:: int64_tensor_shape_dim

.. doxygenfunction:: int64_tensor_shape

.. doxygenfunction:: int64_tensor_shape_ptr

.. doxygenfunction:: int64_tensor_strides_ptr

.. doxygenfunction:: int64_tensor_shape_str

.. doxygenfunction:: is_int64_tensor_empty

.. doxygenfunction:: is_int64_tensor_full

.. doxygenfunction:: is_int64_tensor_ptr

.. _int64_array:

int64_t Array
=============

A ``int64_tensor_t`` initialised via ``init_int64_array`` operates in
``ARRAY_STRUCT`` mode, providing a dynamic 1-D ordered sequence of
``int64_t`` values backed by a contiguous allocator-managed buffer. In this
mode ``len`` tracks the number of populated elements and ``alloc`` tracks
the total allocated capacity. The functions in this section are only valid
for tensors in ``ARRAY_STRUCT`` mode — calling them on a tensor created by
``init_int64_tensor`` returns ``PRECONDITION_FAIL``.

.. code-block:: c

   #include "c_int64.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   int64_tensor_expect_t r = init_int64_array(8, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   int64_tensor_t* arr = r.u.value;

   push_back_int64_tensor(arr, 30u);
   push_back_int64_tensor(arr, 10u);
   push_back_int64_tensor(arr, 20u);
   /* arr = [30, 10, 20], len == 3 */

   int64_t val;
   pop_front_int64_tensor(arr, &val);   /* val == 30, arr = [10, 20] */

   return_int64_tensor(arr);

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_int64_array

Push Operations
---------------

.. doxygenfunction:: push_back_int64_array

.. doxygenfunction:: push_front_int64_array

.. doxygenfunction:: push_at_int64_array

Pop Operations
--------------

.. doxygenfunction:: pop_back_int64_array

.. doxygenfunction:: pop_front_int64_array

.. doxygenfunction:: pop_at_int64_array

Utility Operations
------------------

.. doxygenfunction:: concat_int64_tensor_array
