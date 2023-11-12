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

.. _type_enum:

Enum Data Types 
===============
This ``vector.h`` header file relies on the ``dtype`` enum stored in ``admin.h``
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

Initialize Vector 
=================
The ``vector.h`` header file provides the ``init_vector`` function for 
initializing dynamically allocated arrays. This function is essentially a 
function pointer, intelligently selecting the appropriate initialization 
function based on the specified data type.

.. code-block:: c

   type init_vector(dtype dat_type)(size_t buff);

Parameters 
----------

- :c:`dat_type`: The data type for the vector, represented by a ``dtype`` enum keyword as described in :ref:`Enum Data Types <type_enum>`.
- :c:`buff`: An initial buffer size, indicating the number of indices to be allocated.

Returns
-------

- :c:`type`: A struct representing the vector of the specified type, as outlined in the :ref:`Derived Data Types <vector_dat_type>` section.

.. note:: The buffer size is only a guess, if the user exceeds this value, the underyling functions will allocate more memory in a geometric fashion, until the arrays becomes to large and then it will allocate in a linear fashion.

Example 
-------
Below is an example of using the ``init_vector`` function to create a dynamically 
allocated vector of type ``float_v`` with an initial capacity of 5 indices.

.. code-block:: c 

   #include "vector.h"

   int main() {
       float_v* vec = init_vector(dFloat)(5);
       // Operations on the vector...
       // Remember to free dynamically allocated memory
       free_vector(vec);
       return 0;
   }

.. note:: The ``init_vector`` function abstracts the complexity of vector initialization, providing a straightforward way to instantiate vectors. It is crucial to manage the memory of these vectors properly, which includes freeing them after use.

Underlying Functions 
--------------------
The ``init_vector`` selects from one of the functions below to initialize 
a dynamically allocated array.  If the user desires, they can directly select 
one of these functions instead of using the ``init_vector`` function.

.. code-block:: c

   char_v* init_char_vector(size_t buff);
   uchar_v* init_uchar_vector(size_t buff);
   short_v* init_short_vector(size_t buff);
   ushort_v* init_ushort_vector(size_t buff);
   int_v* init_int_vector(size_t buff);
   uint_v* init_uint_vector(size_t buff);
   long_v* init_long_vector(size_t buff);
   ulong_v* init_ulong_vector(size_t buff);
   llong_v* init_llong_vector(size_t buff);
   ullong_v* init_ullong_vector(size_t buff);
   float_v* init_float_vector(size_t buff);
   double_v* init_double_vector(size_t buff);
   ldouble_v* init_ldouble_vector(size_t buff);
   bool_v* init_bool_vector(size_t buff);
   string_v* init_string_vector(size_t buff);

Garbage Collection with Vectors
===============================
Dynamically allocated vectors require explicit deallocation to free memory. 
While developers often manually manage this using functions like ``free_vector``, 
those compiling with GCC or Clang compilers have an alternative: automatic 
garbage collection using a macro. This feature leverages the `cleanup` 
attribute available in these compilers and is not part of the standard C language.

The macro follows the naming convention ``gbc_<type>``, where ``<type>`` 
corresponds to the derived data types mentioned in :ref:`Derived Data Types <vector_dat_type>`.

Example 
-------
Below is an example demonstrating the use of garbage collection with a 
``float_v`` vector. Notice the absence of a manual ``free_vector`` 
call; the ``gbc_float_v`` macro ensures automatic deallocation when the 
variable goes out of scope.

.. code-block:: c

   #include "vector.h"

   int main() {
       float_v* vec gbc_float_v = init_vector(dFloat)(4);
       push_vector(vec, 1.1, vector_length(vec));
       push_vector(vec, 2.2, vector_length(vec)); 
       push_vector(vec, 3.3, vector_length(vec));
       push_vector(vec, 4.4, vector_length(vec));
       // Automatic cleanup happens here when vec goes out of scope
       return 0;
   }

Pushing Data to Vectors
=======================
The ``push_vector`` macro in the ``vector.h`` header file provides a versatile 
way to insert data into dynamically allocated vectors. Unlike traditional 
stack operations that adhere to a LIFO approach, this macro allows inserting 
data at any specified index, offering greater flexibility in managing vector 
elements. 

The macro employs the ``_Generic`` keyword to determine the correct function for the 
data type being inserted. This design makes ``push_vector`` a convenient and 
type-safe way to work with vectors of different data types.

.. code-block:: c

   #define push_vector(dat_type, dat, index)

Parameters 
----------

- :c:`dat_type`: The vector struct instance.
- :c:`dat`: The data to be inserted, compatible with the vector's data type.
- :c:`index`: The index at which to insert the data.

Returns 
-------

- Returns `true` if the data is successfully inserted into the vector, `false` otherwise.

.. note:: The function returns ``false`` and prints an error message to ``stderr`` in cases such as NULL vector pointers, index out of bounds, or memory allocation failures.

Example 1
---------
This example demonstrates inserting unsigned integers into a vector using the 
``len`` attribute to append data at the end or insert at a specific index.

.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
       uint_v* vec = init_vector(dUInt)(4);
       push_vector(vec, 1, vec->len);
       push_vector(vec, 2, vec->len);
       push_vector(vec, 3, vec->len);
       push_vector(vec, 4, 0);
       print(vec);
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> [ 4, 1, 2, 3 ]

Example 2
---------
An example showing how to insert string literals into a dynamically allocated 
vector.

.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
       string_v* vec = init_vector(dString)(4);
       push_vector(vec, "One", vec->len);
       push_vector(vec, "Two", vec->len);
       push_vector(vec, "Three", vec->len);
       push_vector(vec, "Four", 0);
       print(vec);
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> [ Four, One, Two, Three ]

.. _push_str_dat:

Example 3
---------
An example that demonstrates how to add ``str`` data to a dynamically allocated 
vector.

.. code-block:: c

   #include "print.h"
   #include "vector.h"
   #include "str.h"

   int main() {
       str one =  {.data = "One", .len = 3, .alloc = 4};
       str two = {.data = "Two", .len = 3, .alloc = 4};
       str three = {.data = "Three", .len = 5, .alloc = 6};
       str four = {.data = "Four", .len = 4, .alloc = 5};

       string_v* vec = init_vector(dString)(4);
       push_str_vector(vec, &one, vec->len);
       push_str_vector(vec, &two, vec->len);
       push_str_vector(vec, &three, vec->len);
       push_str_vector(vec, &four, 0);
       print(vec);
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> [ Four, One, Two, Three ]


Underlying Functions 
--------------------
While the ``push_vector`` macro is the recommended approach due to its ease of 
use and type safety, direct use of underlying functions is also possible. 
These functions are particularly useful in scenarios where more control or 
specific behavior is required.

.. code-block:: c

   bool push_char_vector(char_v* vec, char var, size_t index);
   bool push_uchar_vector(uchar_v* vec, unsigned char var, size_t index);
   bool push_short_vector(short_v* vec, short int var, size_t index);
   bool push_ushort_vector(ushort_v* vec, unsigned short int var, size_t index);
   bool push_int_vector(int_v* vec, int var, size_t index);
   bool push_uint_vector(uint_v* vec, unsigned int var, size_t index);
   bool push_long_vector(long_v* vec, long int var, size_t index);
   bool push_ulong_vector(ulong_v* vec, unsigned long int var, size_t index);
   bool push_llong_vector(llong_v* vec, long long int var, size_t index);
   bool push_ullong_vector(ullong_v* vec, unsigned long long int var, size_t index);
   bool push_float_vector(float_v* vec, float var, size_t index);
   bool push_double_vector(double_v* vec, double var, size_t index);
   bool push_ldouble_vector(ldouble_v* vec, long double var, size_t index);
   bool push_bool_vector(bool_v* vec, bool var, size_t index);
   bool push_string_vector(string_v* vec, const char* var, size_t index);
   bool push_str_vector(string_v* vec, str* var, size_t index);

Free Vector 
===========
The ``free_vector`` macro is designed to facilitate the proper freeing of 
dynamically allocated vector data structures. It employs the ``_Generic`` 
keyword to intelligently select the appropriate deallocation function based 
on the vector's data type.  The underlying functions return a void data type.

.. note:: **Caution:** Avoid freeing a vector more than once. While the underlying functions in this library are designed to handle double-free attempts gracefully, double freeing is generally considered bad practice and can lead to undefined behavior in C.

.. code-block:: c

   #define free_vector(vec)

Parameters 
----------

- :c:`vec`: A vector data structure of any type listed in the :ref:`Vector Data Types <vector_dat_type>` section.

Usage Examples
--------------

Example 1: Proper Use
^^^^^^^^^^^^^^^^^^^^^
This example demonstrates the correct way to free a vector data structure.

.. code-block:: c 

   #include "vector.h"

   int main() {
       int_v* vec = init_vector(dInt)(4);
       push_vector(vec, 1, vector_length(vec));
       push_vector(vec, 2, vector_length(vec));
       push_vector(vec, 3, vector_length(vec));
       push_vector(vec, 4, vector_length(vec));
       free_vector(vec);
       return 0;
   }

Example 2: What to Avoid
^^^^^^^^^^^^^^^^^^^^^^^^
The following is an example of improper usage, where the vector is freed twice. 
This is **not** recommended and is shown here for educational purposes only.

.. code-block:: c 

   #include "vector.h"

   int main() {
       int_v* vec = init_vector(dInt)(4);
       push_vector(vec, 1, vector_length(vec));
       push_vector(vec, 2, vector_length(vec));
       push_vector(vec, 3, vector_length(vec));
       push_vector(vec, 4, vector_length(vec));
       free_vector(vec);
       // Improper usage: Avoid double freeing a vector
       free_vector(vec);
       return 0;
   }

Underlying Functions 
--------------------
The ``free_vector`` macro uses the ``_Generic`` keyword to select the proper 
function based on the type of the input variable.  While it is recommended that 
a developer use the ``free_vector`` macro, they can also directly interact with 
the functions themselves which are shown below.

.. code-block:: c

   void free_char_vector(char_v* vec);
   void free_uchar_vector(uchar_v* vec);
   void free_short_vector(short_v* vec);
   void free_ushort_vector(ushort_v* vec);
   void free_int_vector(int_v* vec);
   void free_uint_vector(uint_v* vec);
   void free_long_vector(long_v* vec);
   void free_ulong_vector(ulong_v* vec);
   void free_llong_vector(llong_v* vec);
   void free_ullong_vector(ullong_v* vec);
   void free_float_vector(float_v* vec);
   void free_double_vector(double_v* vec);
   void free_ldouble_vector(ldouble_v* vec);
   void free_bool_vector(bool_v* vec);
   void free_string_vector(string_v* vec);

Get Vector Data 
===============
Retrieving data from a dynamically allocated vector based on an index is a 
common operation. Direct access to the ``data`` attribute is possible but risky, 
as it might lead to accidental overwriting or misinterpretation of data. To 
standardize data access and mitigate these risks, the ``get_vector`` macro was 
developed. This macro uses the ``_Generic`` keyword to intelligently select the 
appropriate function based on the vector's data type.

.. code-block:: c

   #define get_vector(vec, index) (/* Expression to retrieve data */)

Parameters 
----------

- :c:`vec`: A vector data structure as defined in :ref:`Vector Data Types <vector_dat_type>`.
- :c:`index`: The index from which to retrieve data.

Returns 
-------

- The value at the specified index in the vector. The return type matches the vector's data type.

Error Handling
--------------
The ``get_vector`` macro may encounter several error conditions during its 
execution. In such cases, the function sets the ``errno`` global variable to 
indicate the specific error. Users of this function should check ``errno`` 
immediately after the function call to determine if an error occurred and to 
understand the nature of the error.

The possible error codes set by ``get_vector`` include:

- ``EINVAL``: Indicates an invalid argument was passed to the function. This error is set when the input parameters are out of the expected range or format.

- ``ERANGE``: Suggests that the operation resulted in a value that is outside the range of representable values for the specified data type.

Example 1
---------
Demonstrating how to safely access data from a vector using the `get_vector` macro:

.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
       uint_v* vec = init_vector(dUInt)(5);
       push_vector(vec, 2, vector_length(vec));
       push_vector(vec, 12, vector_length(vec));
       push_vector(vec, 22, vector_length(vec));
       push_vector(vec, 1, vector_length(vec));
       push_vector(vec, 80, vector_length(vec));
       print("Index 2: ", get_vector(vec, 2));
       print("Index 0: ", get_vector(vec, 0));
       // This method works, but should be avoided for safety
       print("Index 3: ", vec->data[3]);
       free_vector(vec);
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

   #include "vector.h"

   int main() {
       bool_v* vec = init_vector(dBool)(3)
       push_vector(vec, true, vector_length(vec));
       push_vector(vec, true, vector_length(vec));
       push_vector(vec, false, vector_length(vec));
       bool len = get_vector(vec, 6);
       if (errno == ERANGE) print("Failure");
   }

.. code-block:: bash 

   >> Error: Index out of bounds in get_bool_vector
   >> Failure

Underlying Functions 
--------------------
The ``get_vector`` macro utilizes ``_Generic`` for type-safe and convenient data 
access. These underlying functions can be used directly for more control:

.. code-block:: c

   char get_char_vector(char_v* vec, size_t index);
   unsigned char get_uchar_vector(uchar_v* vec, size_t index);
   short int get_short_vector(short_v* vec, size_t index);
   unsigned short int get_ushort_vector(ushort_v* vec, size_t index);
   int get_int_vector(int_v* vec, size_t index);
   unsigned int get_uint_vector(uint_v* vec, size_t index);
   long int get_long_vector(long_v* vec, size_t index);
   unsigned long int get_ulong_vector(ulong_v* vec, size_t index);
   long long int get_llong_vector(llong_v* vec, size_t index);
   unsigned long long int get_ullong_vector(ullong_v* vec, size_t index);
   float get_float_vector(float_v* vec, size_t index);
   double get_double_vector(double_v* vec, size_t index);
   long double get_ldouble_vector(ldouble_v* vec, size_t index);
   bool get_bool_vector(bool_v* vec, size_t index);
   char* get_string_vector(string_v* vec, size_t index);

Vector Length 
=============
The length of a dynamically allocated vector is maintained in the ``len`` 
attribute of the vector struct. While it's technically possible to access 
this attribute directly, doing so can be risky as it might lead to accidental 
modification of the length. To safely retrieve the vector's length without 
exposing the internal attribute for modification, the ``vector_length`` macro 
is provided.

.. code-block:: c

   #define vector_length(vec) (/* Expression to retrieve length */)

Parameters 
----------

- :c:`vec`: A vector data structure from the :ref:`Vector Data Types <vector_dat_type>` section.
- :c:`index`: The position within the index where data will be retrieved from.

Returns 
-------

- The length of the actively populated vector, returned as a ``size_t`` type.

Error Handling
--------------
The ``vector_length`` macro may encounter several error conditions during its 
execution. In such cases, the function sets the ``errno`` global variable to 
indicate the specific error. Users of this function should check ``errno`` 
immediately after the function call to determine if an error occurred and to 
understand the nature of the error.

The possible error codes set by ``vector_length`` include:

- ``EINVAL``: Indicates an invalid argument was passed to the function. This error is set when the input parameters are out of the expected range or format.

Example 1
---------
This example demonstrates how to access the vector length using the ``vector_length`` 
macro, compared to directly accessing the struct attribute. The latter should 
be avoided to reduce the risk of unintentional modifications.

.. code-block:: c

   #include "vector.h"
   #include "print.h"

   int main() {
       float_v* vec = init_vector(dFloat)(5);
       push_vector(vec, 2.1f, vector_length(vec));
       // Avoid directly accessing vec->len like below.
       push_vector(vec, 7.4f, vec->len);
       push_vector(vec, 1.1f, vector_length(vec));
       push_vector(vec, 43.5f, vec->len);
       push_vector(vec, 13.8f, vector_length(vec));
       push_vector(vec, 7.7f, vec->len);
       print("Vector: ", vec);
       print("Vector Length: ", vector_length(vec));
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> Vector: [ 2.1, 7.4, 1.1, 43.5, 13.8, 7.7 ]
   >> Vector Length: 6

Example 2
---------
It is possible to pass a NULL pointer to the ``vector_length`` macro 
or a struct with a NULL pointer to data.  In this case, the ``vector_length``
macro will throw a value of ``EINVAL`` to ``errno`` which can be checked
to handle the error.  In this instance, the underlying functions will 
return a value of 0, false, or a string with nothing but a null terminator.
In addition, the function will print an error to ``stderr``.

.. code-block:: c 

   #include "vector.h"

   int main() {
       bool_v vec = {.data = NULL, .len = 0, .alloc = 0};
       size_t len = vector_length(&vec);
       if (errno == EINVAL) print("Failure");

.. code-block:: bash 

   >> Error: Null pointer passed to bool_vector_length 
   >> Failure

Underlying Functions 
--------------------
The ``vector_length`` macro utilizes the ``_Generic`` keyword to select the 
appropriate function based on the vector's data type. While the macro is the 
recommended way to access the vector's length, developers can use the underlying 
functions directly in advanced scenarios.

.. code-block:: c 

   size_t char_vector_length(char_v* vec);
   size_t uchar_vector_length(uchar_v* vec);
   size_t short_vector_length(short_v* vec);
   size_t ushort_vector_length(ushort_v* vec);
   size_t int_vector_length(int_v* vec);
   size_t uint_vector_length(uint_v* vec);
   size_t long_vector_length(long_v* vec);
   size_t ulong_vector_length(ulong_v* vec);
   size_t llong_vector_length(llong_v* vec);
   size_t ullong_vector_length(ullong_v* vec);
   size_t float_vector_length(float_v* vec);
   size_t double_vector_length(double_v* vec);
   size_t ldouble_vector_length(ldouble_v* vec);
   size_t bool_vector_length(bool_v* vec);
   size_t string_vector_length(string_v* vec);

Get Vector Memory 
=================
Retrieving the memory allocation for a vector, measured by the number of 
allocated indices, can be done through the ``alloc`` attribute of the vector 
struct. Direct access to this attribute, however, poses a risk of accidental 
overwriting, which could lead to unintended behavior. The ``vector_memory`` 
macro provides a safe way to access this information without directly exposing 
the ``alloc`` attribute.

.. code-block:: c

   #define vector_memory(vec) (/* Expression to retrieve memory allocation */) 

Parameters 
----------

- :c:`vec`: A vector data structure as defined in :ref:`Vector Data Types <vector_dat_type>`.

Returns 
-------

- The number of indices allocated in memory for the vector, returned as a `size_t`.

Error Handling
--------------
The ``vector_memory`` macro may encounter several error conditions during its 
execution. In such cases, the function sets the ``errno`` global variable to 
indicate the specific error. Users of this function should check ``errno`` 
immediately after the function call to determine if an error occurred and to 
understand the nature of the error.

The possible error codes set by ``vector_memory`` include:

- ``EINVAL``: Indicates an invalid argument was passed to the function. This error is set when the input parameters are out of the expected range or format.

Example 1
---------
Demonstrating how to retrieve the memory allocation using the ``vector_memory`` macro:

.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
       float_v* vec = init_vector(dFloat)(15);
       // ...pushing data into vec...
       print("Vector Memory: ", vector_memory(vec));
       free_vector(vec); 
       return 0;
   }

.. code-block:: bash

   >> Vector Memory: 15 

Example 2
---------
Error handling for scenarios where a NULL pointer is passed:

.. code-block:: c 

   #include "vector.h"

   int main() {
       bool_v* vec = NULL;
       errno = 0; // Reset errno before calling vector_memory
       size_t mem = vector_memory(vec);
       if (errno == EINVAL) print("Failure: Null pointer error.");
       return 0;
   }

.. code-block:: bash 

   >> Failure: Null pointer error.

Underlying Functions 
--------------------
The ``vector_memory`` macro employs the ``_Generic`` keyword to select the 
appropriate function based on the vector's data type. While using the macro is 
recommended, developers have the option to directly use the underlying functions 
for specific requirements.

.. code-block:: c 

   size_t char_vector_memory(char_v* vec);
   size_t uchar_vector_memory(uchar_v* vec);
   size_t short_vector_memory(short_v* vec);
   size_t ushort_vector_memory(ushort_v* vec);
   size_t int_vector_memory(int_v* vec);
   size_t uint_vector_memory(uint_v* vec);
   size_t long_vector_memory(long_v* vec);
   size_t ulong_vector_memory(ulong_v* vec);
   size_t llong_vector_memory(llong_v* vec);
   size_t ullong_vector_memory(ullong_v* vec);
   size_t float_vector_memory(float_v* vec);
   size_t double_vector_memory(double_v* vec);
   size_t ldouble_vector_memory(ldouble_v* vec);
   size_t bool_vector_memory(bool_v* vec);
   size_t string_vector_memory(string_v* vec);

Pop Vector 
==========
The ``pop_vector`` macro in this library allows for extracting data from a vector 
at any index. While removing data from the end of the vector is an :math:`O(1)` 
operation, typical of LIFO stack behavior, extracting from any other position 
has an :math:`O(n)` time complexity due to the need to shift remaining elements.

.. code-block:: c 

   #define pop_vector(vec, index) (/* Expression to pop data from a vector */) 

Parameters 
----------

- :c:`vec`: A vector data structure as defined in :ref:`Vector Data Types <vector_dat_type>`.
- :c:`index`: The index from which data will be retrieved.

Returns 
-------

- The popped value's data type corresponds with the vector's data type. For ``str*`` data types, the returned string must be manually freed.

Error Handling
--------------
The ``pop_vector`` macro may encounter several error conditions during its 
execution. In such cases, the function sets the ``errno`` global variable to 
indicate the specific error. Users of this function should check ``errno`` 
immediately after the function call to determine if an error occurred and to 
understand the nature of the error.

The possible error codes set by ``pop_vector`` include:

- ``EINVAL``: Indicates an invalid argument was passed to the function. This error is set when the input parameters are out of the expected range or format.

- ``ERANGE``: Suggests that the operation resulted in a value that is outside the range of representable values for the specified data type.


.. note:: When popping any other data type, the user can choose not to catch the returned variable int a variable namespace. However, for ``str`` data, the user must catch and free the string data. 

Example 1 
---------
Extracting integer data from a vector:

.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
       int_v* vec = init_vector(dInt)(5);
       push_vector(vec, 1, vector_length(vec));
       push_vector(vec, 2, vector_length(vec));
       push_vector(vec, 3, vector_length(vec));
       push_vector(vec, 4, vector_length(vec));
       push_vector(vec, 5, vector_length(vec));
       int var_one = pop_vector(vec, 4);
       int var_two = pop_vector(vec, 0);
       print("Popped variables: ", var_one, " and ", var_two);
       print("Remaining vector: ", vec);
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> Popped variables: 5 and 1
   >> Remaining vector: [ 2, 3, 4 ]

Example 2
---------
Extracting string data from a vector, demonstrating the need to free the returned ``str*``:


.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
       string_v* vec = init_vector(dString)(5);
       push_vector(vec, "One", vector_length(vec));
       push_vector(vec, "Two", vector_length(vec));
       push_vector(vec, "Three", vector_length(vec));
       push_vector(vec, "Four", vector_length(vec));
       push_vector(vec, "Five", vector_length(vec));
       str* var_one = pop_vector(vec, 4);
       str* var_two = pop_vector(vec, 0);
       print("Popped variables: ", var_one, " and ", var_two);
       print("Remaining vector: ", vec);
       free_vector(vec);
       free_string(var_one);
       free_string(var_two);
       return 0;
   }

.. code-block:: bash 

   >> Popped variables: Five and One
   >> Remaining vector: [ Two, Three, Four ]

Underlying Functions 
--------------------
The ``pop_vector`` macro leverages ``_Generic`` to select the appropriate function 
based on the vector's data type. While using the macro is recommended, 
these underlying functions can be directly used for more specific control:

.. code-block:: c

   char pop_char_vector(char_v* vec, size_t index);
   unsigned char pop_uchar_vector(uchar_v* vec, size_t index);
   short int pop_short_vector(short_v* vec, size_t index);
   unsigned short int pop_ushort_vector(ushort_v* vec, size_t index);
   int pop_int_vector(int_v* vec, size_t index);
   unsigned int pop_uint_vector(uint_v* vec, size_t index);
   long int pop_long_vector(long_v* vec, size_t index);
   unsigned long int pop_ulong_vector(ulong_v* vec, size_t index);
   long long int pop_llong_vector(llong_v* vec, size_t index);
   unsigned long long int pop_ullong_vector(ullong_v* vec, size_t index);
   float pop_float_vector(float_v* vec, size_t index);
   double pop_double_vector(double_v* vec, size_t index);
   long double pop_ldouble_vector(ldouble_v* vec, size_t index);
   bool pop_bool_vector(bool_v* vec, size_t index);
   str* pop_string_vector(string_v* vec, size_t index);
