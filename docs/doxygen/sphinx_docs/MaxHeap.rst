********
Max Heap
********

The ``heap.h`` header file provides an implementation of the Max-Heap data 
structure in C. A Max-Heap is a specialized binary tree-based data structure 
that ensures that the parent nodes always have values greater than their 
child nodes. This fundamental property makes Max-Heaps valuable in various 
algorithms and applications.

Common Uses of Max-Heaps
-------------------------

- **Priority Queues**: Max-Heaps efficiently maintain a collection of elements with associated priorities, ensuring that the element with the highest priority (largest value) can be quickly accessed and removed.

- **Heap Sort**: Max-Heaps serve as a core component of the heap sort algorithm, an efficient in-place sorting technique that can be used to sort an array of elements in descending order.

- **Graph Algorithms**: Max-Heaps are often employed in graph algorithms, such as Prim's algorithm for finding the minimum spanning tree in a weighted graph.

Key Properties of Max-Heaps
---------------------------

Within this library, a Max-Heap exhibits the following properties:

- Max-Heaps are binary trees where each parent node's value is greater than those of its child nodes.

- The largest element in the Max-Heap is always located at the root, making it accessible in constant time.

- Max-Heaps can be efficiently used to add elements with higher values at the top and remove the largest element, making them suitable for priority queue operations.

Implementation Details
----------------------

In the context of this library, a Max-Heap is implemented as an array, where the child nodes exist at indices :math:`2i+1` and :math:`2i+2` relative to the parent node at index :math:`i`.

This library offers functions to create, manage, and interact with Max-Heaps, allowing developers to utilize this data structure for solving a wide range of problems efficiently.

For detailed usage and function references, refer to the library documentation.

Structure
=========
The ``heap.h`` header file utilizes the following structure to manage 
dynamically allocated heap structures. The struct is encoded as a macro that can be 
easily expanded for various data types.

.. role:: c(code)
   :language: c

.. code-block:: c

   #define HEAP_STRUCT(type, dat_type)           \
       typedef struct {                            \
           type *data;                             \
           size_t len;                             \
           size_t alloc;                           \
       } dat_type;

Parameters
----------

- :c:`type`: The data type that the struct will be instantiated for.
- :c:`dat_type`: The derived data type that will describe the struct.

Attributes
----------

- :c:`data`: A pointer to the array of type ``type``.
- :c:`len`: The length of the vector measured in the number of indices.
- :c:`alloc`: The number of indices allocated in memory.

.. _max_heap_dat_type:

Derived Data Types 
------------------
The ``HEAP_STRUCT`` macro is pre-instantiated within the ``heap.h``
header file for these data types. The code example below shows the 
derived data type name for each data type and the corresponding data type 
that will be contained in array form within the struct. The ``str``
data type is further described in the :ref:`String <string_struct>` section of this document.

.. code-block:: c 

   HEAP_STRUCT(char, char_max_hp);
   HEAP_STRUCT(unsigned char, uchar_max_hp);
   HEAP_STRUCT(short int, short_max_hp);
   HEAP_STRUCT(unsigned short int, ushort_max_hp);
   HEAP_STRUCT(int, int_max_hp);
   HEAP_STRUCT(unsigned int, uint_max_hp);
   HEAP_STRUCT(long int, long_max_hp);
   HEAP_STRUCT(unsigned long int, ulong_max_hp);
   HEAP_STRUCT(long long int, llong_max_hp);
   HEAP_STRUCT(unsigned long long int, ullong_max_hp);
   HEAP_STRUCT(float, float_max_hp);
   HEAP_STRUCT(double, double_max_hp);
   HEAP_STRUCT(long double, ldouble_max_hp);
   HEAP_STRUCT(bool, bool_max_hp);
   HEAP_STRUCT(str, string_max_hp);

.. _max_heap_type_enum:

Enum Data Types 
===============
This ``heap.h`` header file relies on the ``dtype`` enum stored in ``admin.h``
to identify the data type of an instantiated vector.  The possible enum values
that can be used are shown below and are further described in the upcoming sections 
of this document.

.. code-block:: c

   typedef enum {
       dChar,  // Keyword for a char data type as an integer data type 
       dUChar,  // keyword for unsigned char data type as an integer data type 
       dShort,  // Keyword for short int data type 
       dUShort,  // Keyword for unsigned short int data type 
       dInt,  // Keyword for int data type 
       dUInt,  // Keyword for unsigned int data type 
       dLong,  // Keyword for long int data type 
       dULong,  // Keyword for unsigned long int data type 
       dLongLong,  // Keyword for long long int data type 
       dULongLong,  // Keyword for unsigned long long int data type 
       dFloat,  // Keyword for float data type 
       dDouble,  // Keyword for double data type 
       dLDouble,  // Keyword for long double data type 
       dBool,  // Keyword for bool data type 
       dString  // Keyword for str data type
   } dtype;
