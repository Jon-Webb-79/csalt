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
       print(&arr);  // Assuming print_array is a function for printing array contents
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
       print(&arr);
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
       print(&arr);
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
       print(&arr);
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

Array Length 
============
The length of a statically allocated array is maintained in the ``len`` 
attribute of the vector struct. While it's technically possible to access 
this attribute directly, doing so can be risky as it might lead to accidental 
modification of the length. To safely retrieve the vector's length without 
exposing the internal attribute for modification, the ``array_length`` macro 
is provided.

.. code-block:: c

   #define array_length(arr) (/* Expression to retrieve length */)

Parameters 
----------

- :c:`arr`: An array data structure from the :ref:`Array Data Types <array_dat_type>` section.

Returns 
-------

- The length of the actively populated vector, returned as a ``size_t`` type.

Error Handling
--------------
The ``array_length`` macro may encounter several error conditions during its 
execution. In such cases, the function sets the ``errno`` global variable to 
indicate the specific error. Users of this function should check ``errno`` 
immediately after the function call to determine if an error occurred and to 
understand the nature of the error.

The possible error codes set by ``array_length`` include:

- ``EINVAL``: Indicates an invalid argument was passed to the function. This error is set when the input parameters are out of the expected range or format.

Example 1
---------
This example demonstrates how to access the vector length using the ``array_length`` 
macro, compared to directly accessing the struct attribute. The latter should 
be avoided to reduce the risk of unintentional modifications.

.. code-block:: c

   #include "array.h"
   #include "print.h"

   int main() {
       float a[5];
       float_arr* arr = init_array(arr, 5, 0);
       push_array(arr, 2.1f, array_length(arr));
       // Avoid directly accessing arr->len like below.
       push_array(arr, 7.4f, arr->len);
       push_array(arr, 1.1f, array_length(arr));
       push_array(arr, 43.5f, arr->len);
       push_array(arr, 13.8f, array_length(arr));
       push_array(arr, 7.7f, arr->len);
       print("Array: ", &arr);
       print("Array Length: ", array_length(arr));
       return 0;
   }

.. code-block:: bash 

   >> Array: [ 2.1, 7.4, 1.1, 43.5, 13.8, 7.7 ]
   >> Array Length: 6

Example 2
---------
It is possible to pass a NULL pointer to the ``array_length`` macro 
or a struct with a NULL pointer to data.  In this case, the ``array_length``
macro will throw a value of ``EINVAL`` to ``errno`` which can be checked
to handle the error.  In this instance, the underlying functions will 
return a value of 0, false, or a string with nothing but a null terminator.
In addition, the function will print an error to ``stderr``.

.. code-block:: c 

   #include "array.h"

   int main() {
       bool_arr arr = {.data = NULL, .len = 0, .alloc = 0};
       size_t len = array_length(&vec);
       if (errno == EINVAL) print("Failure");

.. code-block:: bash 

   >> Error: Null pointer passed to bool_vector_length 
   >> Failure

Underlying Functions 
--------------------
The ``array_length`` macro utilizes the ``_Generic`` keyword to select the 
appropriate function based on the vector's data type. While the macro is the 
recommended way to access the vector's length, developers can use the underlying 
functions directly in advanced scenarios.

.. code-block:: c 

   size_t char_array_length(char_arr* arr);
   size_t uchar_array_length(uchar_arr* arr);
   size_t short_array_length(short_arr* arr);
   size_t ushort_array_length(ushort_arr* arr);
   size_t int_array_length(int_arr* arr);
   size_t uint_array_length(uint_arr* arr);
   size_t long_array_length(long_arr* arr);
   size_t ulong_array_length(ulong_arr* arr);
   size_t llong_array_length(llong_arr* arr);
   size_t ullong_array_length(ullong_arr* arr);
   size_t float_array_length(float_arr* arr);
   size_t double_array_length(double_arr* arr);
   size_t ldouble_array_length(ldouble_arr* arr);
   size_t bool_array_length(bool_arr* arr);
   size_t string_array_length(string_arr* arr);

Array Memory 
============
Retrieving the memory allocation for an array, measured by the number of 
allocated indices, can be done through the ``alloc`` attribute of the vector 
struct. Direct access to this attribute, however, poses a risk of accidental 
overwriting, which could lead to unintended behavior. The ``array_memory`` 
macro provides a safe way to access this information without directly exposing 
the ``alloc`` attribute.

.. code-block:: c

   #define array_memory(arr) (/* Expression to retrieve memory allocation */) 

Parameters 
----------

- :c:`arr`: A vector data structure as defined in :ref:`Array Data Types <array_dat_type>`.

Returns 
-------

- The number of indices allocated in memory for the vector, returned as a `size_t`.

Error Handling
--------------
The ``array_memory`` macro may encounter several error conditions during its 
execution. In such cases, the function sets the ``errno`` global variable to 
indicate the specific error. Users of this function should check ``errno`` 
immediately after the function call to determine if an error occurred and to 
understand the nature of the error.

The possible error codes set by ``array_memory`` include:

- ``EINVAL``: Indicates an invalid argument was passed to the function. This error is set when the input parameters are out of the expected range or format.

Example 1
---------
Demonstrating how to retrieve the memory allocation using the ``array_memory`` macro:

.. code-block:: c

   #include "print.h"
   #include "array.h"

   int main() {
       float a[15];
       float_arr* vec = init_array(arr, 15, 0);
       // ...pushing data into vec...
       print("Array Memory: ", array_memory(vec));
       return 0;
   }

.. code-block:: bash

   >> Array Memory: 15 

Example 2
---------
Error handling for scenarios where a NULL pointer is passed:

.. code-block:: c 

   #include "array.h"

   int main() {
       bool_arr* vec = NULL;
       errno = 0; // Reset errno before calling vector_memory
       size_t mem = array_memory(vec);
       if (errno == EINVAL) print("Failure: Null pointer error.");
       return 0;
   }

.. code-block:: bash 

   >> Failure: Null pointer error.

Underlying Functions 
--------------------
The ``array_memory`` macro employs the ``_Generic`` keyword to select the 
appropriate function based on the vector's data type. While using the macro is 
recommended, developers have the option to directly use the underlying functions 
for specific requirements.

.. code-block:: c 

   size_t char_array_memory(chararr* arr);
   size_t uchar_array_memory(uchararr* arr);
   size_t short_array_memory(shortarr* arr);
   size_t ushort_array_memory(ushortarr* arr);
   size_t int_array_memory(intarr* arr);
   size_t uint_array_memory(uintarr* arr);
   size_t long_array_memory(longarr* arr);
   size_t ulong_array_memory(ulongarr* arr);
   size_t llong_array_memory(llongarr* arr);
   size_t ullong_array_memory(ullongarr* arr);
   size_t float_array_memory(floatarr* arr);
   size_t double_array_memory(doublearr* arr);
   size_t ldouble_array_memory(ldoublearr* arr);
   size_t bool_array_memory(boolarr* arr);
   size_t string_array_memory(stringarr* arr);

Pop Array 
=========
The ``pop_array`` macro in this library allows for extracting of data from a vector at
any index.  While removing data from the end of the vector is an :math:`O(1)` operation,
typical of LIFO stack behavior, extracting data from any other position has an :math:`O(n)`
time complexity due to the need to shift remaining elements.

.. code-block:: c 

   #define pop_array(arr, index) ( /*Expression to pop data from an array */) 

Parameters 
----------

- :c:`arr`: A vector data structure as defined in :ref:`Array Data Types <array_dat_type>`.
- :c:`index`: The index from which data shall be retrieved.

Returns 
-------

- The popped values data type corresponds with the vector's data type.

Error Handling
--------------
The ``pop_array`` macro may encounter several error conditions during its 
execution. In such cases, the function sets the ``errno`` global variable to 
indicate the specific error. Users of this function should check ``errno`` 
immediately after the function call to determine if an error occurred and to 
understand the nature of the error.

The possible error codes set by ``pop_array`` include:

- ``EINVAL``: Indicates an invalid argument was passed to the function. This error is set when the input parameters are out of the expected range or format.
- ``ERANGE``: Suggests that the value of ``index`` was outside a valid range.

Example 1
---------
An example showing how to pop and catch data, or choose not to catch data.

.. code-block:: c 

   #include "print.h"
   #include "array.h"

   int main() {
       int a[6];
       int_arr arr = init_array(a, 6, 0);
       push_array(arr, 1, 0);
       push_array(arr, 2, 1);
       push_array(arr, 3, 2);
       push_array(arr, 4, 3);
       push_array(arr, 5, 4);
       push_array(arr, 6, 5);
       int var = pop_array(arr, 0);
       pop_array(arr, 4);
       print(var);
       print(&arr);
       return 0;
   }

.. code-block:: bash 

   >> 1
   >> [ 1, 2, 3, 4 ]

Example 2 
---------
Example showing the results of an out of bounds index.

.. code-block:: c 

   #include "print.h"
   #include "array.h"

   int main() {
       int a[6];
       int_arr arr = init_array(a, 6, 0);
       push_array(arr, 1, 0);
       push_array(arr, 2, 1);
       push_array(arr, 3, 2);
       push_array(arr, 4, 3);
       push_array(arr, 5, 4);
       push_array(arr, 6, 5);
       int var = pop_array(arr, 0);
       pop_array(arr, 14);
       if (errno == ERANGE) print("Index out of range")
       print(&var);
       print(arr);
       return 0;
   }

.. code-block:: bash 

   >> 1
   >> Index out of range
   >> [ 1, 2, 3, 4, 5 ]

Underlying Functions 
--------------------
The ``pop_array`` macro employs the ``_Generic`` keyword to select the 
appropriate function based on the vector's data type. While using the macro is 
recommended, developers have the option to directly use the underlying functions 
for specific requirements.

.. code-block:: c 

   char pop_char_array(char_arr* arr, size_t index);
   unsigned char pop_uchar_array(uchar_arr* arr, size_t index);
   short int pop_short_array(short_arr* arr, size_t index);
   unsigned short int pop_ushort_array(ushort_arr* arr, size_t index);
   int pop_int_array(int_arr* arr, size_t index);
   unsigned int pop_uint_array(uint_arr* arr, size_t index);
   long int pop_long_array(long_arr* arr, size_t index);
   unsigned long int pop_ulong_array(ulong_arr* arr, size_t index);
   long long int pop_llong_array(llong_arr* arr, size_t index);
   unsigned long long int pop_ullong_array(ullong_arr* arr, size_t index);
   float pop_float_array(float_arr* arr, size_t index);
   double pop_double_array(double_arr* arr, size_t index);
   long double pop_ldouble_array(ldouble_arr* arr, size_t index);
   bool pop_bool_array(bool_arr* arr, size_t index);

Reverse Array 
=============
The ``reverse_array`` macro leverages the ``_Generic`` keyword to select from 
several functions in order to reverse the order of data in a statically
allocated array data structure.  The form and parameters of the ``reverse_array``
are shown below.

.. code-block:: c

   #define reverse_array(arr) (/*Expression to reverse vector here*/) 

Parameters 
----------

- :c:`arr`: A vector data structure as defined in :ref:`Array Data Types <array_dat_type>`.

Error Handling
--------------
The ``reverse_array`` macro may encounter several error conditions during its 
execution. In such cases, the function sets the ``errno`` global variable to 
indicate the specific error. Users of this function should check ``errno`` 
immediately after the function call to determine if an error occurred and to 
understand the nature of the error.

The possible error codes set by ``reverse_array`` include:

- ``EINVAL``: Indicates an invalid argument was passed to the function. This error is set when the input parameters are out of the expected range or format.

Example 
-------
Example showing how to revserse the order of a string vector.

.. code-block:: c

   #include "array.h"
   #include "print.h"

   int main() {
       int a[5];
       int_arr arr = init_array(a, 5, 0);
       push_array(arr, 1, array_length(arr));
       push_array(arr, 2, array_length(arr));
       push_array(arr, 3, array_length(arr));
       push_array(arr, 4, array_length(arr));
       push_array(arr, 5, array_length(arr));
       print("Before reverse operation: ", arr);
       reverse_array(arr);
       print("After reverse operation: ", arr);
       return 0;
   }

.. code-block:: bash 

   >> Before reverse operation: [ 1, 2, 3, 4 ]
   >> After reverse operation: [ 4, 3, 2, 1 ]

Underlying Functions 
--------------------
The ``reverse_array`` macro leverages ``_Generic`` to select the appropriate function 
based on the array's data type. While using the macro is recommended, 
these underlying functions can be directly used for more specific control:

.. code-block:: c

   void reverse_char_array(char_arr* arr);
   void reverse_uchar_array(uchar_arr* arr);
   void reverse_short_array(short_arr* arr);
   void reverse_ushort_array(ushort_arr* arr);
   void reverse_int_array(int_arr* arr);
   void reverse_uint_array(uint_arr* arr);
   void reverse_long_array(long_arr* arr);
   void reverse_ulong_array(ulong_arr* arr);
   void reverse_llong_array(llong_arr* arr);
   void reverse_ullong_array(ullong_arr* arr);
   void reverse_float_array(float_arr* arr);
   void reverse_double_array(double_arr* arr);
   void reverse_ldouble_array(ldouble_arr* arr);
   void reverse_bool_array(bool_arr* arr);
   void reverse_string_array(string_arr* arr);

Sort Array
==========
The ``sort_array`` macro is a versatile tool for sorting array data structures 
in either ``FORWARD`` or ``REVERSE`` direction. It intelligently selects the 
appropriate sorting function based on the vector type and the specified sorting 
algorithm, ensuring type safety through the use of the ``_Generic`` keyword.

.. code-block:: c 

   #define sort_array(arr, stype, direction) ( /* Expressions to sort vector */)   

Parameters 
----------

- :c:`arr`: An array data structure defined in :ref:`Array Data Types <array_dat_type>`.
- :c:`stype`: An ``enum`` of type ``sort_type`` including ``BUBBLE``, ``INSERT``, ``MERGE``, ``QUICK``, and ``TIM``, representing various sorting algorithms.
- :c:`direction`: An ``enum`` of type ``iter_dir`` with possible values ``FORWARD`` or ``REVERSE``, representing the sorting direction.

The following table describes the parameters of the various sorting algorithms.

.. list-table:: Sorting Algorithms Complexity and Stability
   :widths: 25 25 25 25
   :header-rows: 1

   * - Algorithm
     - Time Complexity (Average/Worst)
     - Space Complexity
     - Stability
   * - Bubble Sort
     - O(n^2) / O(n^2)
     - O(1)
     - Stable
   * - Insertion Sort
     - O(n^2) / O(n^2)
     - O(1)
     - Stable
   * - Merge Sort
     - O(n log n) / O(n log n)
     - O(n)
     - Stable
   * - Quick Sort
     - O(n log n) / O(n^2)
     - O(log n)
     - Not Stable
   * - Tim Sort
     - O(n log n) / O(n log n)
     - O(n)
     - Stable


Error Handling
--------------
The macro sets the ``errno`` global variable to indicate errors, such as:

- ``EINVAL``: Passed if ``vec`` is NULL or if an unsupported ``stype`` is provided.

Example 
-------
An example for sorting a array using the quick sort algorithm in the ``FORWARD`` direction.

.. code-block:: c

   #include "array.h"
   #include "print.h"

   int main() {
       int a[5];
       int_arr arr = init_array(a, 5, 0);
       push_array(arr, 5, array_length(arr));
       push_array(arr, 1, array_length(arr));
       push_array(arr, 3, array_length(arr));
       push_array(arr, 2, array_length(arr));
       push_array(arr, -4, array_length(arr));
       print("Before sorting operation: ", arr);
       sort_array(arr, QUICK, FORWARD);
       print("After sorting operation: ", arr);
       return 0;
   }

.. code-block:: bash 

   >> Before sorting operation: [ 5, 1, 3, 2, -4 ]
   >> After sorting operation: [ -4, 1, 2, 3, 5 ]

Underlying Functions 
--------------------
While the ``sort_array`` macro is recommended for its ease of use and type safety, 
developers may also directly call the specific sorting functions:

.. code-block:: c 

   void sort_char_array(char_arr* arr, sort_type stype, iter_dir direction);
   void sort_uchar_array(uchar_arr* arr, sort_type stype, iter_dir direction);
   void sort_short_array(short_arr* arr, sort_type stype, iter_dir direction);
   void sort_ushort_array(ushort_arr* arr, sort_type stype, iter_dir direction);
   void sort_int_array(int_arr* arr, sort_type stype, iter_dir direction);
   void sort_uint_array(uint_arr* arr, sort_type stype, iter_dir direction);
   void sort_long_array(long_arr* arr, sort_type stype, iter_dir direction);
   void sort_ulong_array(ulong_arr* arr, sort_type stype, iter_dir direction);
   void sort_llong_array(llong_arr* arr, sort_type stype, iter_dir direction);
   void sort_ullong_array(ullong_arr* arr, sort_type stype, iter_dir direction);
   void sort_float_array(float_arr* arr, sort_type stype, iter_dir direction);
   void sort_double_array(double_arr* arr, sort_type stype, iter_dir direction);
   void sort_ldouble_array(ldouble_arr* arr, sort_type stype, iter_dir direction);
   void sort_bool_array(bool_arr* arr, sort_type stype, iter_dir direction);
   void sort_string_array(string_arr* arr, sort_type stype, iter_dir direction);

Min and Max Array Value
=======================
The ``min_array`` and ``max_array`` macros leverage the ``_Generic`` 
keyword to select the appropriate function for determining the minimum and 
maximum values in a statically allocated array data structure. These macros 
abstract the complexity of choosing the correct function based on the vector's 
data type. These macros do not work on ``bool_arr`` and ``string_arr`` derived 
data types.

.. code-block:: c

   #define min_array(arr) (/* Expression to find minimum value in vector */) 
   #define max_array(arr) (/* Expression to find maximum value in vector */)

Parameters 
----------

- :c:`arr`: A vector data structure defined in :ref:`Array Data Types <array_dat_type>`.

Returns 
-------

- Returns the minimum or maximum value in the vector, consistent with the vector's data type.

Error Handling
--------------
The ``max_array`` and ``min_array`` macros handle several error conditions, 
setting the ``errno`` global variable to indicate specific errors. Users 
should check ``errno`` after the function call to identify and understand 
any errors.

Possible error codes:

- ``EINVAL``: Indicates an invalid argument was passed to the function.

Example 
-------
Example to find min and max values in vector data structure.

.. code-block:: c

   #include "print.h"
   #include "array.h"

   int main() {
       float a[5];
       float_v vec = init_array(a, 5, 0);
       push_array(vec, 12.1f, array_length(vec));
       push_array(vec, 8.7f, array_length(vec));
       push_array(vec, 22.18f, array_length(vec));
       push_array(vec, 1.1f, array_length(vec));
       push_array(vec, -27.6f, array_length(vec));
       print("Min Value: ", min_array(vec));
       print("Max Value: ", max_array(vec));
       return 0;
   }

.. code-block:: bash 

   >> Min Value: -27.6000
   >> Max Value: 22.18000

Underlying Functions 
--------------------
The ``min_array`` and ``max_array`` macros use ``_Generic`` to select the 
right function based on the vector's data type. For specific control, these 
underlying functions can be used directly:

.. code-block:: c

   char max_char_array(char_arr* arr);
   unsigned char max_uchar_array(uchar_arr* arr);
   short int max_short_array(short_arr* arr);
   unsigned short int max_ushort_array(ushort_arr* arr);
   int max_int_array(int_arr* arr);
   unsigned int max_uint_array(uint_arr* arr);
   long int max_long_array(long_arr* arr);
   unsigned long int max_ulong_array(ulong_arr* arr);
   long long int max_llong_array(llong_arr* arr);
   unsigned long long int max_ullong_array(ullong_arr* arr);
   float max_float_array(float_arr* arr);
   double max_double_array(double_arr* arr);
   long double max_ldouble_array(ldouble_arr* arr);
   char min_char_array(char_arr* arr);
   unsigned char min_uchar_array(uchar_arr* arr);
   short int min_short_array(short_arr* arr);
   unsigned short int min_ushort_array(ushort_arr* arr);
   int min_int_array(int_arr* arr);
   unsigned int min_uint_array(uint_arr* arr);
   long int min_long_array(long_arr* arr);
   unsigned long int min_ulong_array(ulong_arr* arr);
   long long int min_llong_array(llong_arr* arr);
   unsigned long long int min_ullong_array(ullong_arr* arr);
   float min_float_array(float_arr* arr);
   double min_double_array(double_arr* arr);
   long double min_ldouble_array(ldouble_arr* arr);

Binary Search 
=============
The ``bsearch_array`` macro leverages the ``_Generic`` keyword to select the 
appropriate function for conducting a binary search of a statically allocated 
array to find the index where a value resides.

.. note:: The input vector must be sorted for this function to work properly.  If the function has not been previously sorted, the user should enter ``false`` for the ``sorted`` variable.

.. code-block:: c

   #define bsearch_array(arr, val, sorted) ( /*Expression to search a vector */) 

Parameters 
----------

- :c:`arr`: A vector data structure defined in :ref:`Array Data Types <array_dat_type>`.
- :c:`val`: The value being searched for.
- :c:`sorted`: ``true`` if the vector is already sorted, ``false`` otherwise.

Returns 
-------

- Returns the index associated with the value of ``val``, or -1 if the value is not found.

Error Handling
--------------
The macro sets the ``errno`` global variable to indicate errors, such as:

- ``EINVAL``: Passed if ``vec`` is NULL or if ``val`` does not exist in the vector

Example 1 
---------
An example showing how to conduct a binary search of an unsorted vector.

.. code-block:: c

   #include "print.h"
   #include "array.h"

   int main() {
       int a[5];
       string_v arr = init_array(a, 5, 0);
       int a[5] = {1, 2, 3, 4, 5};
       for (size_t i = 0; i < 5; i++) {
           push_array(arr, a[i], array_length(vec));
       }
       int b = bsearch_array(arr, "3, false);
       print("This is the value: ", get_array(b));
       return 0;
   }

.. code-block:: bash 

   >> This is the value: 3

Underlying Functions 
--------------------
While the ``bsearch_array`` macro is recommended for its ease of use and type safety, 
developers may also directly call the specific sorting functions:
Max and Min Vector Values 

.. code-block:: c 

   int bsearch_char_array(char_arr* arr, char val, bool sorted);
   int bsearch_uchar_array(uchar_arr* arr, unsigned char val, bool sorted);
   int bsearch_short_array(short_arr* arr, short int val, bool sorted);
   int bsearch_ushort_array(ushort_arr* arr, unsigned short int val, bool sorted);
   int bsearch_int_array(int_arr* arr, int val, bool sorted);
   int bsearch_uint_array(uint_arr* arr, unsigned int val, bool sorted);
   int bsearch_long_array(long_arr* arr, long int val, bool sorted);
   int bsearch_ulong_array(ulong_arr* arr, unsigned long int val, bool sorted);
   int bsearch_llong_array(llong_arr* arr, long long int val, bool sorted);
   int bsearch_ullong_array(ullong_arr* arr, unsigned long long int val, bool sorted);
   int bsearch_float_array(float_arr* arr, float val, bool sorted);
   int bsearch_double_array(double_arr* arr, double val, bool sorted);
   int bsearch_ldouble_array(ldouble_arr* arr, long double val, bool sorted);
   int bsearch_bool_array(bool_arr* arr, bool val, bool sorted);
   int bsearch_string_array(string_arr* arr, char* val, bool sorted);
   int bsearch_string_array(string_arr* arr, char* val, bool sorted);
   int bsearch_str_array(string_arr* arr, str* val, bool sorted);

Sum of Array Values 
===================
The ``sum_array`` macros leverage the ``_Generic`` 
keyword to select the appropriate function for determining the minimum and 
maximum values in a statically allocated array data structure. These macros 
abstract the complexity of choosing the correct function based on the vector's 
data type. These macros do not work on ``bool_v`` and ``string_v`` derived 
data types.

.. code-block:: c

   #define sum_array(arr) (/* Expression to find sum of values in a vector */) 

Parameters 
----------

- :c:`arr`: A vector data structure defined in :ref:`Array Data Types <array_dat_type>`.

Returns 
-------

- Returns the summation of valuea in the vector, consistent with the vector's data type.

Error Handling
--------------
The ``sum_array`` macros handle several error conditions, 
setting the ``errno`` global variable to indicate specific errors. Users 
should check ``errno`` after the function call to identify and understand 
any errors.

Possible error codes:

- ``EINVAL``: Indicates an invalid argument was passed to the function.

Example 
-------
Example to find min and max values in vector data structure.

.. code-block:: c

   #include "print.h"
   #include "array.h"

   int main() {
       float a[5];
       float_v arr = init_arrtor(a, 5, 0);
       push_array(arr, 12.1f, array_length(arr));
       push_array(arr, 8.7f, array_length(arr));
       push_array(arr, 22.18f, array_length(arr));
       push_array(arr, 1.1f, array_length(arr));
       push_array(arr, -27.6f, array_length(arr));
       print("Sum Value: ", sum_array(arr));
       return 0;
   }

.. code-block:: bash 

   >> Sum Value: 16.48

Underlying Functions 
--------------------
The ``sum_array`` macro use ``_Generic`` to select the 
right function based on the array's data type. For specific control, these 
underlying functions can be used directly:

.. code-block:: c

   char sum_char_array(char_arr* arr);
   unsigned char sum_uchar_array(uchar_arr* arr);
   short int sum_short_array(short_arr* arr);
   unsigned short int sum_ushort_array(ushort_arr* arr);
   int sum_int_array(int_arr* arr);
   unsigned int sum_uint_array(uint_arr* arr);
   long int sum_long_array(long_arr* arr);
   unsigned long int sum_ulong_array(ulong_arr* arr);
   long long int sum_llong_array(llong_arr* arr);
   unsigned long long int sum_ullong_array(ullong_arr* arr);
   float sum_float_array(float_arr* arr);
   double sum_double_array(double_arr* arr);
   long double sum_ldouble_array(ldouble_arr* arr);

Average Array Value 
===================
The ``array_average`` macro uses the ``_Generic`` keyword to intelligently 
select from several functions to determine the average value within an array
data structure. The form of the ``average_average`` macro is shown below.

.. code-block:: c 

   #define array_average(arr) (/* Expressions to determine the vector average */)

Parameters 
----------

- :c:`arr`: A vector data structure defined in :ref:`Array Data Types <array_dat_type>`.

Returns 
-------

- Calculates and returns the arithmetic mean of the elements within ``arr``.

.. note:: The return type varies based on the input vector's data type:
   
   - For ``char_arr``, ``uchar_arr``, ``short_arr``, ``ushort_arr``, and ``float_arr``, the return type is ``float``.
   - For ``int_arr``, ``uint_arr``, ``long_arr``, ``ulong_arr``, and ``double_arr``, the return type is ``double``.
   - For all other inputs, the return type is ``long double``.

Error Handling
--------------
The ``array_average`` macros handle several error conditions, 
setting the ``errno`` global variable to indicate specific errors. Users 
should check ``errno`` after the function call to identify and understand 
any errors.

Possible error codes:

- ``EINVAL``: Indicates an invalid argument was passed to the function.

Example 
-------
An example with ``int_arr`` data types.

.. code-block:: c

   #include "print.h"
   #include "array.h"

   int main() {
       int a[5];
       int_arr arr = init_array(a, 5, 0);
       push_array(arr, 12, array_length(arr));
       push_array(arr, 8, array_length(arr));
       push_array(arr, 22, array_length(arr));
       push_array(arr, 1, array_length(arr));
       push_array(arr, -27, array_length(arr));
       print("Average Value: ", array_average(arr));
       return 0;
   }

.. code-block:: bash 

   >> Average Value: 3.2000

Underlying Functions 
--------------------
The ``array_average`` macro uses ``_Generic`` to select the right function 
based on the vector's data type. For specific control, these underlying 
functions can be used directly:

.. code-block:: c

   char average_char_array(char_arr* arr);
   unsigned char average_uchar_array(uchar_arr* arr);
   short int average_short_array(short_arr* arr);
   unsigned short int average_ushort_array(ushort_arr* arr);
   int average_int_array(int_arr* arr);
   unsigned int average_uint_array(uint_arr* arr);
   long int average_long_array(long_arr* arr);
   unsigned long int average_ulong_array(ulong_arr* arr);
   long long int average_llong_array(llong_arr* arr);
   unsigned long long int average_ullong_array(ullong_arr* arr);
   float average_float_array(float_arr* arr);
   double average_double_array(double_arr* arr);
   long double average_ldouble_array(ldouble_arr* arr);

Standard Deviation of an Array
==============================
The ``array_stdev`` macro uses the ``_Generic`` keyword to select the 
appropriate function to calculate and return the standard deviation of
the values in an array data structure.  The parameters of the ``array_stdev``
macro are shown below.

.. code-block:: c 

   #define array_stdev(arr) ( /*Expressions to calculate standard deviation */)  

Parameters 
----------

- :c:`arr`: A vector data structure defined in :ref:`Array Data Types <array_dat_type>`.

Returns 
-------

- Calculates and returns the arithmetic mean of the elements within ``arr``.

.. note:: The return type varies based on the input vector's data type:
   
   - For ``char_arr``, ``uchar_arr``, ``short_arr``, ``ushort_arr``, and ``float_arr``, the return type is ``float``.
   - For ``int_arr``, ``uint_arr``, ``long_arr``, ``ulong_arr``, and ``double_arr``, the return type is ``double``.
   - For all other inputs, the return type is ``long double``.

Error Handling
--------------
The ``array_stdev`` macros handle several error conditions, 
setting the ``errno`` global variable to indicate specific errors. Users 
should check ``errno`` after the function call to identify and understand 
any errors.

Possible error codes:

- ``EINVAL``: Indicates an invalid argument was passed to the function.

Example 
------- 
An example to calculate the standard deviation of a ``float_arr`` type.

.. code-block:: c 

   #include "print.h"
   #include "array.h"

   int main() {
       float a[5];
       float_v arr = init_array(a, 5, 0);
       push_array(vec, 1.f, array_length(vec));
       push_array(vec, 2.f, array_length(vec));
       push_array(vec, 10.3f, array_length(vec));
       push_array(vec, 4.f, array_length(vec));
       push_array(vec, 5.f, array_length(vec));
       float stdev = array_stdev(vec);
       print("Stdev: ", stdev);
       return 0;
   }

.. code-block:: bash 

   >> Stdev: 3.2444414

Underlying Functions 
--------------------
The ``array_average`` macro uses ``_Generic`` to select the right function 
based on the array's data type. For specific control, these underlying 
functions can be used directly:

.. code-block:: c

   char stdev_char_array(char_arr* arr);
   unsigned char stdev_uchar_array(uchar_arr* arr);
   short int stdev_short_array(short_arr* arr);
   unsigned short int stdev_ushort_array(ushort_arr* arr);
   int stdev_int_array(int_arr* arr);
   unsigned int stdev_uint_array(uint_arr* arr);
   long int stdev_long_array(long_arr* arr);
   unsigned long int stdev_ulong_array(ulong_arr* arr);
   long long int stdev_llong_array(llong_arr* arr);
   unsigned long long int stdev_ullong_array(ullong_arr* arr);
   float stdev_float_array(float_arr* arr);
   double stdev_double_array(double_arr* arr);
   long double stdev_ldouble_array(ldouble_arr* arr);
