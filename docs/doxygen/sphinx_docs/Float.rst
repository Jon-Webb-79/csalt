.. _float_vector_file:

****************
C Float Overview 
****************
The ``c_float.h`` header file contains implementations for statically 
and dynamically allocated arrays of `float` data types.  In addition, the 
``c_float.h`` header file contains an implementation for a dictionary of 
float values with a string keyword as well as various matrices to include 
``Dense``, ``CSC``, ``CSR``, and ``COO`` matrix types.


Float Vector Overview
=====================

A float vector provides both dynamic and static array containers for float values, offering
efficient management of numerical data with automatic memory handling for dynamic arrays
and safe access for static arrays. This implementation combines the flexibility of dynamic
arrays with the safety of bounds checking and proper memory management.  All of the functions 
described in this section can be found in the ``c_float.h`` header file.

The vector implementation within this ``c_float.h`` header file contains these 
key features for the implementation of ``float_v`` data types.

* Dynamic and static array support: Choose between automatic resizing or fixed-size arrays
* Memory safety: Proper encapsulation and memory management
* Bounds checking: Safe access to array elements
* Efficient access: O(1) access to any value in the vector
* Automatic cleanup: Optional garbage collection support with FLTVEC_GBC

Float vectors are particularly useful when:

* Managing collections of float values that may grow or shrink
* Working with an unknown number of float values at compile time
* Requiring sequential access to multiple float values
* Needing efficient random access to float values by index
* Managing related numerical float as a single unit
* Implementing numerical algorithms or data processing pipelines

the functions for the ``float_v`` data type have the following performance 
characteristics.

* Access time: O(1) for index-based access
* Insertion time: O(1) amortized for push_back operations
* Memory efficiency: Dynamic vectors grow geometrically to minimize reallocations
* Memory overhead: Minimal per-element overhead

Data Types
----------
The follwing enums and structs can be found in the ``c_float.h`` file to produce 
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

float_v struct 
~~~~~~~~~~~~~~

.. code:: c

   typedef struct {
       float* data;       // Pointer to c-style array of float values
       size_t len;        // The populated length of the array 
       size_t alloc;      // The allocated length of the array 
       ErrorCode error;   // ErrorCode enum describing relevant errors 
       alloc_t alloc_type // DYNAMIC or STATIC memory allocation
   } float_v;

.. note:: ``ErrorCode`` enum can be found in the :ref:`c_error.h <error_code_file>` file.

Initialization and Memory Management
------------------------------------
The functions and macros documented in this section can be used to initialize 
a ``float_v`` data structure and manage its memory.

init_float_vector 
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: init_float_vector 
   :project: csalt

init_float_array 
~~~~~~~~~~~~~~~~

.. doxygendefine:: init_float_array
   :project: csalt

wrap_float_array 
~~~~~~~~~~~~~~~~

.. doxygenfunction:: wrap_float_array
   :project: csalt

free_float_vector
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: free_float_vector
   :project: csalt

FLTVEC_GBC 
~~~~~~~~~~

.. doxygendefine:: FLTVEC_GBC
   :project: csalt

Utility Functions 
-----------------

get_float_vector_error
~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: get_float_vector_error
   :project: csalt

Vector Manipulation 
-------------------

push_back_float_vector 
~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: push_back_float_vector
   :project: csalt

push_front_float_vector 
~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: push_front_float_vector
   :project: csalt

insert_float_vector 
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: insert_float_vector
   :project: csalt

pop_back_float_vector 
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pop_back_float_vector
   :project: csalt

pop_front_float_vector 
~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pop_front_float_vector
   :project: csalt

pop_any_float_vector 
~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: pop_any_float_vector
   :project: csalt

Search Vector 
-------------

Vector Iterator 
---------------

Float Matrix Overview 
=====================

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

Float Dictionary Overview
==========================

A float dictionary provides a hash table implementation for mapping string keys to float values, 
offering efficient key-value storage and retrieval. This implementation uses chained hashing for 
collision resolution and features automatic table resizing for optimal performance.

The key features of a float dictionary are the following;

* Dynamic resizing: Automatic growth when load factor threshold is reached
* Efficient lookup: O(1) average case access time
* Memory safety: Proper encapsulation and memory management
* String key support: Automatic key duplication and management
* Collision handling: Chained hashing for robust collision resolution
* Automatic cleanup: Optional garbage collection support with FDICT_GBC

A float dicitonary should be used for the following reasons.

Float dictionaries are particularly useful when:

* Mapping strings to numerical values
* Requiring fast key-value lookups
* Managing relationships between text identifiers and measurements
* Implementing caches or lookup tables with string keys
* Building numerical mapping tables
* Creating frequency counters for string data

A float matrix has the following performance characteristics.

* Access time: O(1) average case for lookups and insertions
* Space efficiency: Adaptive growth strategy for memory efficiency
* Collision handling: Chained hashing for reliable performance under high load
* Memory overhead: Small per-entry overhead for key storage and chain pointers

Data Types
----------

The following are derived data types used in the implementation of float dictionaries.

General Float Functions 
=======================

Generic Macro Overview
======================


