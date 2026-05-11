.. _int16:

****************
int16_t Overview
****************

The ``int16_t`` family of data structures in csalt provides type-safe,
allocator-agnostic containers for 16-bit integer data. Every
structure wraps the generic ``tensor_t`` engine from ``c_tensor.h`` with
the element type fixed to ``INT16_TYPE`` at initialisation, eliminating
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
that produce a new object return a ``int16_tensor_expect_t`` whose
``has_value`` field distinguishes success from failure, while in-place
mutations return an :c:type:`error_code_t` directly. Callers must always
check the result before using the value.

.. code-block:: c

   int16_tensor_expect_t r = init_int16_array(8, true, heap_allocator());
   if (!r.has_value) {
       fprintf(stderr, "%s\n", error_to_string(r.u.error));
   }
   int16_tensor_t* arr = r.u.value;
   /* ... use arr ... */
   return_int16_tensor(arr);

.. note::

   Functions documented under :ref:`int16_tensor` operate on any
   ``int16_tensor_t`` regardless of how it was initialised. Functions
   documented under :ref:`int16_array` are only valid for tensors
   initialised via ``init_int16_array`` — they require
   ``mode == ARRAY_STRUCT`` and will return ``PRECONDITION_FAIL`` if
   called on a tensor created by ``init_int16_tensor``.

.. _int16_tensor:

int16_t Tensor
==============

The following types and functions operate on any ``int16_tensor_t``
regardless of dimensionality or mode. This includes fixed-shape N-D tensors
created with ``init_int16_tensor`` and dynamic 1-D arrays created with
``init_int16_array``.

Structs
-------

.. doxygenstruct:: int16_tensor_t
   :members:

.. doxygenstruct:: int16_tensor_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_int16_tensor

.. doxygenfunction:: return_int16_tensor

.. doxygenfunction:: copy_int16_tensor

Get and Set
-----------

.. doxygenfunction:: get_int16_tensor_index

.. doxygenfunction:: get_int16_tensor_nd_index

.. doxygenfunction:: set_int16_tensor_index

.. doxygenfunction:: set_int16_tensor_nd_index

Utility Operations
------------------

.. doxygenfunction:: clear_int16_tensor

.. doxygenfunction:: slice_int16_tensor_array

.. doxygenfunction:: reverse_int16_tensor

.. doxygenfunction:: sort_int16_tensor

.. doxygenfunction:: int16_tensor_lsearch

.. doxygenfunction:: int16_tensor_bsearch

.. doxygenfunction:: int16_tensor_bbsearch

.. doxygenfunction:: int16_tensors_equal

Introspection
-------------

.. doxygenfunction:: int16_tensor_size

.. doxygenfunction:: int16_tensor_alloc

.. doxygenfunction:: int16_tensor_data_size

.. doxygenfunction:: int16_tensor_dtype

.. doxygenfunction:: int16_tensor_ndim

.. doxygenfunction:: int16_tensor_shape_dim

.. doxygenfunction:: int16_tensor_shape

.. doxygenfunction:: int16_tensor_shape_ptr

.. doxygenfunction:: int16_tensor_strides_ptr

.. doxygenfunction:: int16_tensor_shape_str

.. doxygenfunction:: is_int16_tensor_empty

.. doxygenfunction:: is_int16_tensor_full

.. doxygenfunction:: is_int16_tensor_ptr

.. _int16_array:

int16_t Array
=============

A ``int16_tensor_t`` initialised via ``init_int16_array`` operates in
``ARRAY_STRUCT`` mode, providing a dynamic 1-D ordered sequence of
``int16_t`` values backed by a contiguous allocator-managed buffer. In this
mode ``len`` tracks the number of populated elements and ``alloc`` tracks
the total allocated capacity. The functions in this section are only valid
for tensors in ``ARRAY_STRUCT`` mode — calling them on a tensor created by
``init_int16_tensor`` returns ``PRECONDITION_FAIL``.

.. code-block:: c

   #include "c_int16.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   int16_tensor_expect_t r = init_int16_array(8, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   int16_tensor_t* arr = r.u.value;

   push_back_int16_tensor(arr, 30u);
   push_back_int16_tensor(arr, 10u);
   push_back_int16_tensor(arr, 20u);
   /* arr = [30, 10, 20], len == 3 */

   int16_t val;
   pop_front_int16_tensor(arr, &val);   /* val == 30, arr = [10, 20] */

   return_int16_tensor(arr);

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_int16_array

Push Operations
---------------

.. doxygenfunction:: push_back_int16_array

.. doxygenfunction:: push_front_int16_array

.. doxygenfunction:: push_at_int16_array

Pop Operations
--------------

.. doxygenfunction:: pop_back_int16_array

.. doxygenfunction:: pop_front_int16_array

.. doxygenfunction:: pop_at_int16_array

Utility Operations
------------------

.. doxygenfunction:: concat_int16_tensor_array
