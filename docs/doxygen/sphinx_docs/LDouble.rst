.. _ldouble:

******************
ldouble_t Overview
******************

The ``ldouble`` family of data structures in csalt provides type-safe,
allocator-agnostic containers for long double typed data. Every
structure wraps the generic ``tensor_t`` engine from ``c_tensor.h`` with
the element type fixed to ``LDOUBLE_TYPE`` at initialisation, eliminating
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
that produce a new object return a ``ldouble_tensor_expect_t`` whose
``has_value`` field distinguishes success from failure, while in-place
mutations return an :c:type:`error_code_t` directly. Callers must always
check the result before using the value.

.. code-block:: c

   ldouble_tensor_expect_t r = init_ldouble_array(8, true, heap_allocator());
   if (!r.has_value) {
       fprintf(stderr, "%s\n", error_to_string(r.u.error));
   }
   ldouble_tensor_t* arr = r.u.value;
   /* ... use arr ... */
   return_ldouble_tensor(arr);

.. note::

   Functions documented under :ref:`ldouble_tensor` operate on any
   ``ldouble_tensor_t`` regardless of how it was initialised. Functions
   documented under :ref:`ldouble_array` are only valid for tensors
   initialised via ``init_ldouble_array`` — they require
   ``mode == ARRAY_STRUCT`` and will return ``PRECONDITION_FAIL`` if
   called on a tensor created by ``init_ldouble_tensor``.

.. _ldouble_tensor:

ldouble_t Tensor
================

The following types and functions operate on any ``ldouble_tensor_t``
regardless of dimensionality or mode. This includes fixed-shape N-D tensors
created with ``init_ldouble_tensor`` and dynamic 1-D arrays created with
``init_ldouble_array``.

Structs
-------

.. doxygenstruct:: ldouble_tensor_t
   :members:

.. doxygenstruct:: ldouble_tensor_expect_t
   :members:

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_ldouble_tensor

.. doxygenfunction:: return_ldouble_tensor

.. doxygenfunction:: copy_ldouble_tensor

Get and Set
-----------

.. doxygenfunction:: get_ldouble_tensor_index

.. doxygenfunction:: get_ldouble_tensor_nd_index

.. doxygenfunction:: set_ldouble_tensor_index

.. doxygenfunction:: set_ldouble_tensor_nd_index

Utility Operations
------------------

.. doxygenfunction:: clear_ldouble_tensor

.. doxygenfunction:: slice_ldouble_tensor_array

.. doxygenfunction:: reverse_ldouble_tensor

.. doxygenfunction:: sort_ldouble_tensor

.. doxygenfunction:: ldouble_tensor_lsearch

.. doxygenfunction:: ldouble_tensor_bsearch

.. doxygenfunction:: ldouble_tensor_bbsearch

.. doxygenfunction:: ldouble_tensors_equal

.. doxygenfunction:: min_ldouble_tensor

Introspection
-------------

.. doxygenfunction:: ldouble_tensor_size

.. doxygenfunction:: ldouble_tensor_alloc

.. doxygenfunction:: ldouble_tensor_data_size

.. doxygenfunction:: ldouble_tensor_dtype

.. doxygenfunction:: ldouble_tensor_ndim

.. doxygenfunction:: ldouble_tensor_shape_dim

.. doxygenfunction:: ldouble_tensor_shape

.. doxygenfunction:: ldouble_tensor_shape_ptr

.. doxygenfunction:: ldouble_tensor_strides_ptr

.. doxygenfunction:: ldouble_tensor_shape_str

.. doxygenfunction:: is_ldouble_tensor_empty

.. doxygenfunction:: is_ldouble_tensor_full

.. doxygenfunction:: is_ldouble_tensor_ptr

.. _ldouble_array:

ldouble_t Array
===============

A ``ldouble_tensor_t`` initialised via ``init_ldouble_array`` operates in
``ARRAY_STRUCT`` mode, providing a dynamic 1-D ordered sequence of
``ldouble_t`` values backed by a contiguous allocator-managed buffer. In this
mode ``len`` tracks the number of populated elements and ``alloc`` tracks
the total allocated capacity. The functions in this section are only valid
for tensors in ``ARRAY_STRUCT`` mode — calling them on a tensor created by
``init_ldouble_tensor`` returns ``PRECONDITION_FAIL``.

.. code-block:: c

   #include "c_ldouble.h"

   /* Choose an allocator — see :ref:`allocator_file` for all options. */
   allocator_vtable_t alloc = heap_allocator();

   ldouble_tensor_expect_t r = init_ldouble_array(8, true, alloc);
   if (!r.has_value) { /* handle r.u.error */ }
   ldouble_tensor_t* arr = r.u.value;

   push_back_ldouble_tensor(arr, 30u);
   push_back_ldouble_tensor(arr, 10u);
   push_back_ldouble_tensor(arr, 20u);
   /* arr = [30, 10, 20], len == 3 */

   ldouble_t val;
   pop_front_ldouble_tensor(arr, &val);   /* val == 30, arr = [10, 20] */

   return_ldouble_tensor(arr);

Initialisation and Teardown
---------------------------

.. doxygenfunction:: init_ldouble_array

Push Operations
---------------

.. doxygenfunction:: push_back_ldouble_array

.. doxygenfunction:: push_front_ldouble_array

.. doxygenfunction:: push_at_ldouble_array

Pop Operations
--------------

.. doxygenfunction:: pop_back_ldouble_array

.. doxygenfunction:: pop_front_ldouble_array

.. doxygenfunction:: pop_at_ldouble_array

Utility Operations
------------------

.. doxygenfunction:: concat_ldouble_tensor_array
