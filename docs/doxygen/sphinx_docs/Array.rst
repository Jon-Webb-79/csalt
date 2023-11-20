.. _array_lib:

******
Arrays 
******
The ``array.h`` header file offers a comprehensive suite of functions for 
managing statically allocated arrays in C. Unlike dynamic arrays, these static 
arrays have a fixed size, determined at compile-time, making them a key tool 
in scenarios where data sizes are known and constant.

Statically allocated arrays are stored on the stack, leading to faster access 
for small and fixed-size data sets. They are ideal for applications where 
deterministic memory usage and efficiency are paramount, albeit with the 
trade-off of a fixed capacity.  Some of the key features of an array are;

- **Array Initialization and Population:** Simplify the process of initializing and populating arrays with values.
- **Searching and Sorting:** Tailored efficient algorithms for searching and sorting fixed-size data sets.
- **Data Manipulation:** Facilitate the insertion, deletion, and modification of elements within fixed-size constraints.
- **Utility Functions:** Various helper functions for common array tasks, such as finding minimum/maximum values or calculating averages.

These utilities enhance the usability of statically allocated arrays, making 
them more practical and effective for a range of programming tasks in C. 
The ``array.h`` file is especially useful in contexts where memory efficiency 
and performance are critical, and the data size is known and consistent.

Structure
=========
The ``array.h`` header file utilizes the following structure to manage 
dynamically allocated arrays. The struct is encoded as a macro that can be 
easily expanded for various data types.

.. role:: c(code)
   :language: c

.. code-block:: c

   #define ARRAY_STRUCT(type, dat_type)            \
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

.. _array_dat_type:

Derived Data Types 
==================
The ``ARRAY_STRUCT`` macro is pre-instantiated within the ``array.h``
header file for these data types. The code example below shows the 
derived data type name for each data type and the corresponding data type 
that will be contained in array form within the struct.

.. code-block:: c 

   ARRAY_STRUCT(char, char_arr);
   ARRAY_STRUCT(unsigned char, uchar_arr);
   ARRAY_STRUCT(short int, short_arr);
   ARRAY_STRUCT(unsigned short int, ushort_arr);
   ARRAY_STRUCT(int, int_arr);
   ARRAY_STRUCT(unsigned int, uint_arr);
   ARRAY_STRUCT(long int, long_arr);
   ARRAY_STRUCT(unsigned long int, ulong_arr);
   ARRAY_STRUCT(long long int, llong_arr);
   ARRAY_STRUCT(unsigned long long int, ullong_arr);
   ARRAY_STRUCT(float, float_arr);
   ARRAY_STRUCT(double, double_arr);
   ARRAY_STRUCT(long double, ldouble_arr);
   ARRAY_STRUCT(bool, bool_arr);

.. note:: There is no string data type defined, because strings by nature are dynamically allocated in this library


Initialize Array
================
The ``array.h`` header file includes the ``init_array`` macro, leveraging the 
``_Generic`` keyword to appropriately initialize a static array container. 
Since arrays in C are defined at compile time, the user is responsible for 
creating an array and passing it to the function. It is crucial to accurately 
provide the values for ``buff`` and ``len`` to avoid undefined behavior.

.. note:: Statically allocated arrays are automatically cleaned up when they go out of scope and do not require manual memory management.

.. code-block:: c

   #define init_array(arr, buff, len) ( /* Expression to initialize an array */ )

Parameters
----------

- :c:`arr`: A traditional C array.
- :c:`buff`: The total number of indices allocated for the array.
- :c:`len`: The number of currently populated indices in the array.

Returns
-------

- Returns an array container struct as defined in :ref:`Array Data Types <array_dat_type>`.

Example 1
---------

An example demonstrating the initialization of a float array.

.. code-block:: c

   #include "array.h"
   #include "print.h"

   int main() {
       float a[10] = {1.1, 2.2, 3.3};
       float_arr arr = init_array(a, 10, 3);
       print(arr);  // Assuming print_array is a function for printing array contents
       return 0;
   }

.. code-block:: bash

   >> [ 1.1, 2.2, 3.3 ]

Example 2
---------

An example illustrating incorrect instantiation of an array data structure.

.. code-block:: c

   #include "array.h"
   #include "print.h"

   int main() {
       float a[10] = {1.1, 2.2, 3.3};
       // Incorrectly passing len as 0 instead of 3
       // This may lead to undefined behavior.
       float_arr arr = init_array(a, 10, 0);
       print(arr);
       return 0;
   }

Underlying Functions
--------------------

The ``init_array`` macro utilizes the ``_Generic`` keyword to select from 
several potential functions. While the macro is recommended for its type 
safety and generic capabilities, users may opt to directly use one of the 
underlying functions listed below.

.. code-block:: c

   char_arr init_char_array(char* arr, size_t buff, size_t len);
   uchar_arr init_uchar_array(unsigned char* arr, size_t buff, size_t len);
   short_arr init_short_array(short int* arr, size_t buff, size_t len);
   ushort_arr init_ushort_array(unsigned short int* arr, size_t buff, size_t len);
   int_arr init_int_array(int* arr, size_t buff, size_t len);
   uint_arr init_uint_array(unsigned int* arr, size_t buff, size_t len);
   long_arr init_long_array(long int* arr, size_t buff, size_t len);
   ulong_arr init_ulong_array(unsigned long int* arr, size_t buff, size_t len);
   llong_arr init_llong_array(long long int* arr, size_t buff, size_t len);
   ullong_arr init_ullong_array(unsigned long long int* arr, size_t buff, size_t len);
   float_arr init_float_array(float* arr, size_t buff, size_t len);
   double_arr init_double_array(double* arr, size_t buff, size_t len);
   ldouble_arr init_ldouble_array(long double* arr, size_t buff, size_t len);
   bool_arr init_bool_array(bool* arr, size_t buff, size_t len);

