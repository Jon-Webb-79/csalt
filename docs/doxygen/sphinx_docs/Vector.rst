.. _vector_lib:

******
Vector
******
The ``vector.h`` header file provides a comprehensive suite of functions for 
managing dynamically allocated arrays in C. Unlike static arrays, these dynamic 
arrays can grow and shrink at runtime, offering more flexibility and efficiency 
for data storage and manipulation. This flexibility is particularly useful in 
scenarios where the size of the data set cannot be determined in advance or 
when dealing with large datasets that might otherwise exceed stack size limits.

Dynamically allocated arrays are a fundamental component in C programming, 
enabling developers to handle data more effectively in various applications. 
The ``vector.h`` file includes utilities that facilitate common array 
operations, such as adding or removing elements, thereby expanding the 
practical utility of dynamically allocated arrays beyond their basic 
implementation in standard C.

Structure
=========
The ``vector.h`` header file utilizes the following structure to manage 
dynamically allocated arrays. The struct is encoded as a macro that can be 
easily expanded for various data types.

.. role:: c(code)
   :language: c

.. code-block:: c

   #define VECTOR_STRUCT(type, dat_type)           \
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

.. _vector_dat_type:

Derived Data Types 
------------------
The ``VECTOR_STRUCT`` macro is pre-instantiated within the ``vector.h``
header file for these data types. The code example below shows the 
derived data type name for each data type and the corresponding data type 
that will be contained in array form within the struct. The ``str``
data type is further described in the :ref:`String <string_struct>` section of this document.

.. code-block:: c 

   VECTOR_STRUCT(char, char_v);
   VECTOR_STRUCT(unsigned char, uchar_v);
   VECTOR_STRUCT(short int, short_v);
   VECTOR_STRUCT(unsigned short int, ushort_v);
   VECTOR_STRUCT(int, int_v);
   VECTOR_STRUCT(unsigned int, uint_v);
   VECTOR_STRUCT(long int, long_v);
   VECTOR_STRUCT(unsigned long int, ulong_v);
   VECTOR_STRUCT(long long int, llong_v);
   VECTOR_STRUCT(unsigned long long int, ullong_v);
   VECTOR_STRUCT(float, float_v);
   VECTOR_STRUCT(double, double_v);
   VECTOR_STRUCT(long double, ldouble_v);
   VECTOR_STRUCT(bool, bool_v);
   VECTOR_STRUCT(str, string_v);
