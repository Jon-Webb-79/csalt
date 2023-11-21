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

Pushing Data to Arrays
======================
The ``push_array`` macro in the ``array.h`` header file offers a method to 
insert data into static arrays. Unlike stack operations that typically follow 
a Last In, First Out (LIFO) approach, this macro allows data insertion at any 
specified index within the bounds of the array, providing flexibility in 
managing array elements.

The macro uses the ``_Generic`` keyword to determine the correct function 
based on the data type of the array, making ``push_array`` a convenient and 
type-safe tool for working with different data types.

.. code-block:: c

   #define push_array(arr, dat, index) ( /* Expression to add data to array */ )

Parameters
----------
- :c:`arr`: An array container structure as defined in :ref:`Array Data Types <array_dat_type>`.
- :c:`dat`: The data to be inserted, compatible with the array's data type.
- :c:`index`: The index at which to insert the data.

Returns
-------
- Returns ``true`` if the data is successfully inserted into the array, ``false`` otherwise.

Error Handling
--------------
The ``push_array`` macro may encounter errors such as invalid input or index out 
of bounds. In these cases, underlying functions set the global variable 
``errno`` to indicate the specific error.

Possible error codes:
- ``EINVAL``: Invalid argument was passed to the function.
- ``ERANGE``: Index is out of bounds.

Example 1
---------
An example of adding data to an integer array at various locations.

.. code-block:: c

   #include "print.h"
   #include "array.h"

   int main() {
       int dat[20];
       int_arr arr = init_array(dat, 20, 0);
       push_array(arr, 1, array_length(arr));
       push_array(arr, 2, array_length(arr));
       push_array(arr, 3, array_length(arr));
       push_array(arr, 4, array_length(arr));
       push_array(arr, 5, 0);
       print(arr);
       return 0;
   }

.. code-block:: bash

   >> [ 5, 1, 2, 3, 4 ]

Example 2
---------
An example demonstrating a failure caused by attempting to push data to an 
out-of-bounds index.

.. code-block:: c

   #include "print.h"
   #include "array.h"

   int main() {
       int dat[20];
       int_arr arr = init_array(dat, 20, 0);
       push_array(arr, 1, array_length(arr));
       push_array(arr, 2, array_length(arr));
       push_array(arr, 3, array_length(arr));
       bool test = push_array(arr, 4, 21);  // Index 21 is out of bounds
       if (!test) {
           print("Failed to push data: ", strerror(errno));
       }
       print(arr);
       return 0;
   }

.. code-block:: bash

   >> Failed to push data: Range error in math function.
   >> [ 1, 2, 3 ]

Underlying Functions
--------------------
The ``push_array`` macro employs the ``_Generic`` keyword to select from several 
potential functions. While using the macro is recommended for type safety and 
generic capabilities, users may choose to directly utilize one of the underlying 
functions listed below.

.. code-block:: c

   bool push_char_array(char_arr* arr, char var, size_t index);
   bool push_uchar_array(uchar_arr* arr, unsigned char var, size_t index);
   bool push_short_array(short_arr* arr, short int var, size_t index);
   bool push_ushort_array(ushort_arr* arr, unsigned short int var, size_t index);
   bool push_int_array(int_arr* arr, int var, size_t index);
   bool push_uint_array(uint_arr* arr, unsigned int var, size_t index);
   bool push_long_array(long_arr* arr, long int var, size_t index);
   bool push_ulong_array(ulong_arr* arr, unsigned long int var, size_t index);
   bool push_llong_array(llong_arr* arr, long long int var, size_t index);
   bool push_ullong_array(ullong_arr* arr, unsigned long long int var, size_t index);
   bool push_float_array(float_arr* arr, float var, size_t index);
   bool push_double_array(double_arr* arr, double var, size_t index);
   bool push_ldouble_array(ldouble_arr* arr, long double var, size_t index);
   bool push_bool_array(bool_arr* arr, bool var, size_t index);

Get Array Data 
==============
Retrieving data from a statically allocated vector based on an index is a common 
operation.  Direct access to the ``data`` attribute is possible but risky,
as it might lead to accidentally overwriting of data.  To standardize data 
access and mitigate these risks, the ``get_array`` macro was developed.
This macro uses the ``_Generic`` keyword to intelligently select the appropriate 
function based on the arrays data type.

.. code-block:: c

   #define get_array(arr, index) (/* Expression to retrieve data */)

Parameters 
----------

- :c:`arr`: A vector data structure as defined in :ref:`Array Data Types <array_dat_type>`.
- :c:`index`: The index from which to retrieve data.

Returns 
-------

- The value at the specified index in the vector. The return type matches the vector's data type.

Error Handling
--------------
The ``get_array`` macro may encounter several error conditions during its 
execution. In such cases, the function sets the ``errno`` global variable to 
indicate the specific error. Users of this function should check ``errno`` 
immediately after the function call to determine if an error occurred and to 
understand the nature of the error.

The possible error codes set by ``get_array`` include:

- ``EINVAL``: Indicates an invalid argument was passed to the function. This error is set when the input parameters are out of the expected range or format.
- ``ERANGE``: Suggests that the operation resulted in a value that is outside the range of representable values for the specified data type.

Example 1
---------
Demonstrating how to safely access data from a vector using the ``get_array`` macro:

.. code-block:: c

   #include "print.h"
   #include "array.h"

   int main() {
       unsigned int a[5];
       uint_arr* vec = init_array(arr, 5, 0);
       push_array(arr, 2, array_length(arr));
       push_array(arr, 12, array_length(arr));
       push_array(arr, 22, array_length(arr));
       push_array(arr, 1, array_length(arr));
       push_array(arr, 80, array_length(arr));
       print("Index 2: ", get_array(arr, 2));
       print("Index 0: ", get_array(arr, 0));
       // This method works, but should be avoided for safety
       print("Index 3: ", arr->data[3]);
       free_arrtor(arr);
       return 0;
   }

.. code-block:: bash 

   >> Index 2: 22
   >> Index 0: 0
   >> Index 3: 1

Example 2
---------
Error handling in `get_vector` includes detecting null pointers and out-of-bounds 
indices:

.. code-block:: c 

   #include "array.h"
   #include "print.h"

   int main() {
       bool a[3];
       bool_arr* arr = init_array(arr, 3, 0)
       push_array(arr, true, array_length(arr));
       push_array(arr, true, array_length(arr));
       push_array(arr, false, array_length(arr));
       bool len = get_array(arr, 6);
       if (errno == ERANGE) print("Failure");
   }

.. code-block:: bash 

   >> Error: Index out of bounds in get_bool_vector
   >> Failure

Underlying Functions 
--------------------
The ``get_array`` macro utilizes ``_Generic`` for type-safe and convenient data 
access. These underlying functions can be used directly for more control:

.. code-block:: c

   char get_char_array(char_arr* arr, size_t index);
   unsigned char get_uchar_array(uchar_arr* arr, size_t index);
   short int get_short_array(short_arr* arr, size_t index);
   unsigned short int get_ushort_array(ushort_arr* arr, size_t index);
   int get_int_array(int_arr* arr, size_t index);
   unsigned int get_uint_array(uint_arr* arr, size_t index);
   long int get_long_array(long_arr* arr, size_t index);
   unsigned long int get_ulong_array(ulong_arr* arr, size_t index);
   long long int get_llong_array(llong_arr* arr, size_t index);
   unsigned long long int get_ullong_array(ullong_arr* arr, size_t index);
   float get_float_array(float_arr* arr, size_t index);
   double get_double_array(double_arr* arr, size_t index);
   long double get_ldouble_array(ldouble_arr* arr, size_t index);
   bool get_bool_array(bool_arr* arr, size_t index);
