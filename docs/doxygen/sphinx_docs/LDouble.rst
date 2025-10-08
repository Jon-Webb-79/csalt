.. _ldouble_vector_file:

**********************
C Long Double Overview 
**********************
The ``c_ldouble.h`` header file contains implementations for statically 
and dynamically allocated arrays of `long double` data types.  In addition, the 
``c_ldouble.h`` header file contains an implementation for a dictionary of 
float values with a string keyword as well as various matrices to include 
``Dense``, ``CSC``, ``CSR``, and ``COO`` matrix types.


Long Double Vector Overview
===========================

A long double vector provides both dynamic and static array containers for long double values, offering
efficient management of numerical data with automatic memory handling for dynamic arrays
and safe access for static arrays. This implementation combines the flexibility of dynamic
arrays with the safety of bounds checking and proper memory management.  All of the functions 
described in this section can be found in the ``c_ldouble.h`` header file.

The vector implementation within this ``c_ldouble.h`` header file contains these 
key features for the implementation of ``ldouble_v`` data types.

* Dynamic and static array support: Choose between automatic resizing or fixed-size arrays
* Memory safety: Proper encapsulation and memory management
* Bounds checking: Safe access to array elements
* Efficient access: O(1) access to any value in the vector
* Automatic cleanup: Optional garbage collection support with LDBLEVEC_GBC

Double vectors are particularly useful when:

* Managing collections of long double values that may grow or shrink
* Working with an unknown number of long value values at compile time
* Requiring sequential access to multiple long double values
* Needing efficient random access to long double values by index
* Managing related numerical long double as a single unit
* Implementing numerical algorithms or data processing pipelines

the functions for the ``ldouble_v`` data type have the following performance 
characteristics.

* Access time: O(1) for index-based access
* Insertion time: O(1) amortized for push_back operations
* Memory efficiency: Dynamic vectors grow geometrically to minimize reallocations
* Memory overhead: Minimal per-element overhead

Data Types
----------

The follwing enums and structs can be found in the ``c_ldouble.h`` file to produce 
derived data types.

iter_dir enum 
~~~~~~~~~~~~~

.. code:: c

   typedef enum {
       FORWARD = 0, // visit elements in forward order 
       REVERSE = 1  // visit elements in reverse order 
   } iter_dir;

alloc_t enum 
~~~~~~~~~~~~

.. code:: c

   typedef enum {
       STATIC = 0  // Indicates static memory allocation on the stack 
       DYNAMIC = 1 // Indicates dynamic memory allocation on the heap 
   } alloc_t;

ldouble_v struct 
~~~~~~~~~~~~~~~~

.. code:: c

   typedef struct {
       long double* data;    // Pointer to c-style array of long double values
       size_t len;          // The populated length of the array 
       size_t alloc;        // The allocated length of the array 
       ErrorCode error;     // ErrorCode enum describing relevant errors 
       alloc_t alloc_type   // DYNAMIC or STATIC memory allocation
   } ldouble_v;

.. note:: ``ErrorCode`` enum can be found in the :ref:`c_error.h <error_code_file>` file.

Initialization and Memory Management
------------------------------------

init_ldouble_vector 
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_ldouble_vector 
   :project: csalt

init_ldouble_array 
~~~~~~~~~~~~~~~~~~

.. doxygendefine:: init_ldouble_array
   :project: csalt

wrap_ldouble_array 
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: wrap_ldouble_array
   :project: csalt

free_ldouble_vector
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: free_ldouble_vector
   :project: csalt

LDBLEVEC_GBC 
~~~~~~~~~~~~

.. doxygendefine:: LDBLEVEC_GBC
   :project: csalt

Utility Functions 
-----------------

ldouble_vector_size 
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: ldouble_vector_size
   :project: csalt

ldouble_vector_alloc 
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: ldouble_vector_alloc
   :project: csalt

get_ldouble_vector_error
~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: get_ldouble_vector_error
   :project: csalt

Vector Manipulation 
-------------------

push_back_ldouble_vector 
~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: push_back_ldouble_vector
   :project: csalt

push_front_ldouble_vector 
~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: push_front_ldouble_vector
   :project: csalt

insert_ldouble_vector 
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: insert_ldouble_vector
   :project: csalt

pop_back_ldouble_vector 
~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pop_back_ldouble_vector
   :project: csalt

pop_front_ldouble_vector 
~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pop_front_ldouble_vector
   :project: csalt

pop_any_ldouble_vector 
~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pop_any_ldouble_vector
   :project: csalt

Search Vector 
-------------

ldouble_vector_index 
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: ldouble_vector_index
   :project: csalt

Vector Iterator 
---------------

Double Matrix Overview 
======================

Data Type 
---------

Initialization and Memory Management 
------------------------------------

Utility Functions 
-----------------

Matrix Manipulation 
-------------------

Matrix Operations 
-----------------

Long Double Dictionary Overview
===============================

A long double dictionary provides a hash table implementation for mapping string keys to long double values, 
offering efficient key-value storage and retrieval. This implementation uses chained hashing for 
collision resolution and features automatic table resizing for optimal performance.

The key features of a float dictionary are the following;

* Dynamic resizing: Automatic growth when load factor threshold is reached
* Efficient lookup: O(1) average case access time
* Memory safety: Proper encapsulation and memory management
* String key support: Automatic key duplication and management
* Collision handling: Chained hashing for robust collision resolution
* Automatic cleanup: Optional garbage collection support with FDICT_GBC

A long double dicitonary should be used for the following reasons.

Long Double dictionaries are particularly useful when:

* Mapping strings to numerical values
* Requiring fast key-value lookups
* Managing relationships between text identifiers and measurements
* Implementing caches or lookup tables with string keys
* Building numerical mapping tables
* Creating frequency counters for string data

A long double matrix has the following performance characteristics.

* Access time: O(1) average case for lookups and insertions
* Space efficiency: Adaptive growth strategy for memory efficiency
* Collision handling: Chained hashing for reliable performance under high load
* Memory overhead: Small per-entry overhead for key storage and chain pointers

Data Types
----------

The following are derived data types used in the implementation of long double dictionaries.

General Long Double Functions 
=============================

swap_ldouble 
------------

.. doxygenfunction:: swap_ldouble
   :project: csalt

Generic Macro Overview
======================



