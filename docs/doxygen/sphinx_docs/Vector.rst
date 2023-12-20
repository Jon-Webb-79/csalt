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

Error Handling
--------------
The ``init_vector`` macro has one primary failure mechanism, which is a failure 
to allocate memory.  If this occurs, the underlying functions will return a 
NULL pointer in place of the data struct, and will also set the value of 
``errno`` to ``ENOMEM``.  The developer can check for any of these two 
conditions to determine if an error occurred.

Possible error codes:

- ``ENOMEM``: Indicates a failure to allocate memory.

Example 
-------
Below is an example of using the ``init_vector`` function to create a dynamically 
allocated vector of type ``float_v`` with an initial capacity of 5 indices.

.. code-block:: c 

   #include "vector.h"

   int main() {
       float_v* vec = init_vector(dFloat)(5);
       if (vec == NULL) {
           fprintf(stderr, "Error: Memory allocation failure\n");
           return EXIT_FAILURE;
       }
       // Operations on the vector...
       // Remember to free dynamically allocated memory
       free_vector(vec);
       return EXIT_SUCCESS;
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

- Returns ``true`` if the data is successfully inserted into the vector, ``false`` otherwise.

Error Handling
--------------
The ``push_vector`` macro has three primary failure mechanisms to include an 
index out of bounds, failure to allocate memory, and an invalid argument.
If any of these occure, the following error macros will be passed to 
errno.  In addition, the function will return a value false.

Possible error codes:

- ``ENOMEM``: Indicates a failure to allocate memory.
- ``EINVAL``: Indicates an invalid argument passed for ``vec``.
- ``ERANGE``: Indicates the value of ``index`` was out of bounds.

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

Example 4 
---------
Example with error code processing.

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
       push_str_vector(vec, &four, 10);
       if (errno == ERANGE) fprintf(stderr, "Error, index out of bounds\n");
       print(vec);
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> [ One, Two, Three ]
   >> Error: Index out of bounds

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

Error Handling
--------------
The ``free_vector`` macro has one primary failure mechanism, which is a the 
use of an invalid variable. If this occurs the function will return a 
void as it would if the function executes succesfully; however, it will 
also write a macro value of ``EINVAL`` to ``errno`` which can be checked in 
the calling program.

Possible error codes:

- ``EINVAL``: Indicates an invalid value (i.e. NULL pointer) for the value of ``vec``.

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
Demonstrating how to safely access data from a vector using the ``get_vector`` macro:

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

Reverse Vector 
==============
The ``reverse_vector`` macro leverages the ``_Generic`` keyword to select from 
several functions in order to reverse the order of data in a dynamically 
allocated vector data structure.  The form and parameters of the ``reverse_vector``
are shown below.

.. code-block:: c

   #define reverse_vector(vec) (/*Expression to reverse vector here*/) 

Parameters 
----------

- :c:`vec`: A vector data structure as defined in :ref:`Vector Data Types <vector_dat_type>`.

Error Handling
--------------
The ``reverse_vector`` macro may encounter several error conditions during its 
execution. In such cases, the function sets the ``errno`` global variable to 
indicate the specific error. Users of this function should check ``errno`` 
immediately after the function call to determine if an error occurred and to 
understand the nature of the error.

The possible error codes set by ``reverse_vector`` include:

- ``EINVAL``: Indicates an invalid argument was passed to the function. This error is set when the input parameters are out of the expected range or format.

Example 
-------
Example showing how to revserse the order of a string vector.

.. code-block:: c

   #include "vector.h"
   #include "print.h"

   int main() {
       string_v* vec = init_vector(dString)(5);
       push_vector(vec, "One", vector_length(vec));
       push_vector(vec, "Two", vector_length(vec));
       push_vector(vec, "Three", vector_length(vec));
       push_vector(vec, "Four", vector_length(vec));
       push_vector(vec, "Five", vector_length(vec));
       print("Before reverse operation: ", vec);
       reverse_vector(vec);
       print("After reverse operation: ", vec);
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> Before reverse operation: [ One, Two, Three, Four, Five ]
   >> After reverse operation: [ Five, Four, Three, Two, One ]

Underlying Functions 
--------------------
The ``reverse_vector`` macro leverages ``_Generic`` to select the appropriate function 
based on the vector's data type. While using the macro is recommended, 
these underlying functions can be directly used for more specific control:

.. code-block:: c

   void reverse_char_vector(char_v* vec);
   void reverse_uchar_vector(uchar_v* vec);
   void reverse_short_vector(short_v* vec);
   void reverse_ushort_vector(ushort_v* vec);
   void reverse_int_vector(int_v* vec);
   void reverse_uint_vector(uint_v* vec);
   void reverse_long_vector(long_v* vec);
   void reverse_ulong_vector(ulong_v* vec);
   void reverse_llong_vector(llong_v* vec);
   void reverse_ullong_vector(ullong_v* vec);
   void reverse_float_vector(float_v* vec);
   void reverse_double_vector(double_v* vec);
   void reverse_ldouble_vector(ldouble_v* vec);
   void reverse_bool_vector(bool_v* vec);
   void reverse_string_vector(string_v* vec);

Sort Vector 
===========
The ``sort_vector`` macro is a versatile tool for sorting vector data structures 
in either ``FORWARD`` or ``REVERSE`` direction. It intelligently selects the 
appropriate sorting function based on the vector type and the specified sorting 
algorithm, ensuring type safety through the use of the ``_Generic`` keyword.

.. code-block:: c 

   #define sort_vector(vec, stype, direction) ( /* Expressions to sort vector */)   

Parameters 
----------

- :c:`vec`: A vector data structure defined in :ref:`Vector Data Types <vector_dat_type>`.
- :c:`stype`: An ``enum`` of type ``sort_type`` including ``BUBBLE``, ``SELECTION``, ``INSERT``, ``HEAP``, ``MERGE``, ``QUICK``, and ``TIM``, representing various sorting algorithms.
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
   * - Heap Sort
     - O(n log n) / O(n log n)
     - O(1)
     - Not Stable
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
An example for sorting a vector using the quick sort algorithm in the ``FORWARD`` direction.

.. code-block:: c

   #include "vector.h"
   #include "print.h"

   int main() {
       int_v* vec = init_vector(dInt)(5);
       push_vector(vec, 5, vector_length(vec));
       push_vector(vec, 1, vector_length(vec));
       push_vector(vec, 3, vector_length(vec));
       push_vector(vec, 2, vector_length(vec));
       push_vector(vec, -4, vector_length(vec));
       print("Before sorting operation: ", vec);
       sort_vector(vec, QUICK, FORWARD);
       print("After sorting operation: ", vec);
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> Before sorting operation: [ 5, 1, 3, 2, -4 ]
   >> After sorting operation: [ -4, 1, 2, 3, 5 ]

Underlying Functions 
--------------------
While the ``sort_vector`` macro is recommended for its ease of use and type safety, 
developers may also directly call the specific sorting functions:

.. code-block:: c 

   void sort_char_vector(char_v* vec, sort_type stype, iter_dir direction);
   void sort_uchar_vector(uchar_v* vec, sort_type stype, iter_dir direction);
   void sort_short_vector(short_v* vec, sort_type stype, iter_dir direction);
   void sort_ushort_vector(ushort_v* vec, sort_type stype, iter_dir direction);
   void sort_int_vector(int_v* vec, sort_type stype, iter_dir direction);
   void sort_uint_vector(uint_v* vec, sort_type stype, iter_dir direction);
   void sort_long_vector(long_v* vec, sort_type stype, iter_dir direction);
   void sort_ulong_vector(ulong_v* vec, sort_type stype, iter_dir direction);
   void sort_llong_vector(llong_v* vec, sort_type stype, iter_dir direction);
   void sort_ullong_vector(ullong_v* vec, sort_type stype, iter_dir direction);
   void sort_float_vector(float_v* vec, sort_type stype, iter_dir direction);
   void sort_double_vector(double_v* vec, sort_type stype, iter_dir direction);
   void sort_ldouble_vector(ldouble_v* vec, sort_type stype, iter_dir direction);
   void sort_bool_vector(bool_v* vec, sort_type stype, iter_dir direction);
   void sort_string_vector(string_v* vec, sort_type stype, iter_dir direction);

Binary Search 
=============
The ``bsearch_vector`` macro leverages the ``_Generic`` keyword to select the 
appropriate function for conducting a binary search of a dynamically allocated 
vector to find the index where a value resides.

.. note:: The input vector must be sorted for this function to work properly.  If the function has not been previously sorted, the user should enter ``false`` for the ``sorted`` variable.

.. code-block:: c

   #define bsearch_vector(vec, val, sorted) ( /*Expression to search a vector */) 

Parameters 
----------

- :c:`vec`: A vector data structure defined in :ref:`Vector Data Types <vector_dat_type>`.
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
   #include "vector.h"

   int main() {
       string_v* vec = init_vector(dString)(5);
       char *a[5] = {"One", "Two", "Three", "Four", "Five"};
       for (size_t i = 0; i < 5; i++) {
           push_vector(vec, a[i], vector_length(vec));
       }
       int b = bsearch_vector(vec, "Three", false);
       print("This is the value: ", get_vector(b));
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> This is the value: Three 

Example 2 
---------
An example showing how to search for a ``str`` data type which can not be accomplished
with the ``bsearch_vector`` macro.

.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
       string_v* vec = init_vector(dString)(5);
       char *a[5] = {"One", "Two", "Three", "Four", "Five"};
       for (size_t i = 0; i < 5; i++) {
           push_vector(vec, a[i], vector_length(vec));
       }
       str* c = init_string("Three");
       int b = bsearch_str_vector(vec, c, false);
       print("This is the value: ", get_vector(b));
       free_vector(vec);
       free_string(c);
       return 0;
   }

.. code-block:: bash 

   >> This is the value: Three

Underlying Functions 
--------------------
While the ``bsearch_vector`` macro is recommended for its ease of use and type safety, 
developers may also directly call the specific sorting functions:
Max and Min Vector Values 

.. code-block:: c 

   int bsearch_char_vector(char_v* vec, char val, bool sorted);
   int bsearch_uchar_vector(uchar_v* vec, unsigned char val, bool sorted);
   int bsearch_short_vector(short_v* vec, short int val, bool sorted);
   int bsearch_ushort_vector(ushort_v* vec, unsigned short int val, bool sorted);
   int bsearch_int_vector(int_v* vec, int val, bool sorted);
   int bsearch_uint_vector(uint_v* vec, unsigned int val, bool sorted);
   int bsearch_long_vector(long_v* vec, long int val, bool sorted);
   int bsearch_ulong_vector(ulong_v* vec, unsigned long int val, bool sorted);
   int bsearch_llong_vector(llong_v* vec, long long int val, bool sorted);
   int bsearch_ullong_vector(ullong_v* vec, unsigned long long int val, bool sorted);
   int bsearch_float_vector(float_v* vec, float val, bool sorted);
   int bsearch_double_vector(double_v* vec, double val, bool sorted);
   int bsearch_ldouble_vector(ldouble_v* vec, long double val, bool sorted);
   int bsearch_bool_vector(bool_v* vec, bool val, bool sorted);
   int bsearch_string_vector(string_v* vec, char* val, bool sorted);
   int bsearch_string_vector(string_v* vec, char* val, bool sorted);
   int bsearch_str_vector(string_v* vec, str* val, bool sorted);

Min and Max Vector Value
========================
The ``min_vector`` and ``max_vector`` macros leverage the ``_Generic`` 
keyword to select the appropriate function for determining the minimum and 
maximum values in a dynamically allocated vector data structure. These macros 
abstract the complexity of choosing the correct function based on the vector's 
data type. These macros do not work on ``bool_v`` and ``string_v`` derived 
data types.

.. code-block:: c

   #define min_vector(vec) (/* Expression to find minimum value in vector */) 
   #define max_vector(vec) (/* Expression to find maximum value in vector */)

Parameters 
----------

- :c:`vec`: A vector data structure defined in :ref:`Vector Data Types <vector_dat_type>`.

Returns 
-------

- Returns the minimum or maximum value in the vector, consistent with the vector's data type.

Error Handling
--------------
The ``max_vector`` and ``min_vector`` macros handle several error conditions, 
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
   #include "vector.h"

   int main() {
       float_v* vec = init_vector(dFloat)(5);
       push_vector(vec, 12.1f, vector_length(vec));
       push_vector(vec, 8.7f, vector_length(vec));
       push_vector(vec, 22.18f, vector_length(vec));
       push_vector(vec, 1.1f, vector_length(vec));
       push_vector(vec, -27.6f, vector_length(vec));
       print("Min Value: ", min_vector(vec));
       print("Max Value: ", max_vector(vec));
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> Min Value: -27.6000
   >> Max Value: 22.18000

Underlying Functions 
--------------------
The ``min_vector`` and ``max_vector`` macros use ``_Generic`` to select the 
right function based on the vector's data type. For specific control, these 
underlying functions can be used directly:

.. code-block:: c

   char max_char_vector(char_v* vec);
   unsigned char max_uchar_vector(uchar_v* vec);
   short int max_short_vector(short_v* vec);
   unsigned short int max_ushort_vector(ushort_v* vec);
   int max_int_vector(int_v* vec);
   unsigned int max_uint_vector(uint_v* vec);
   long int max_long_vector(long_v* vec);
   unsigned long int max_ulong_vector(ulong_v* vec);
   long long int max_llong_vector(llong_v* vec);
   unsigned long long int max_ullong_vector(ullong_v* vec);
   float max_float_vector(float_v* vec);
   double max_double_vector(double_v* vec);
   long double max_ldouble_vector(ldouble_v* vec);
   char min_char_vector(char_v* vec);
   unsigned char min_uchar_vector(uchar_v* vec);
   short int min_short_vector(short_v* vec);
   unsigned short int min_ushort_vector(ushort_v* vec);
   int min_int_vector(int_v* vec);
   unsigned int min_uint_vector(uint_v* vec);
   long int min_long_vector(long_v* vec);
   unsigned long int min_ulong_vector(ulong_v* vec);
   long long int min_llong_vector(llong_v* vec);
   unsigned long long int min_ullong_vector(ullong_v* vec);
   float min_float_vector(float_v* vec);
   double min_double_vector(double_v* vec);
   long double min_ldouble_vector(ldouble_v* vec);

Sum of Vector Values 
====================
The ``sum_vector`` macros leverage the ``_Generic`` 
keyword to select the appropriate function for determining the minimum and 
maximum values in a dynamically allocated vector data structure. These macros 
abstract the complexity of choosing the correct function based on the vector's 
data type. These macros do not work on ``bool_v`` and ``string_v`` derived 
data types.

.. code-block:: c

   #define sum_vector(vec) (/* Expression to find sum of values in a vector */) 

Parameters 
----------

- :c:`vec`: A vector data structure defined in :ref:`Vector Data Types <vector_dat_type>`.

Returns 
-------

- Returns the summation of valuea in the vector, consistent with the vector's data type.

Error Handling
--------------
The ``sum_vector`` macros handle several error conditions, 
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
   #include "vector.h"

   int main() {
       float_v* vec = init_vector(dFloat)(5);
       push_vector(vec, 12.1f, vector_length(vec));
       push_vector(vec, 8.7f, vector_length(vec));
       push_vector(vec, 22.18f, vector_length(vec));
       push_vector(vec, 1.1f, vector_length(vec));
       push_vector(vec, -27.6f, vector_length(vec));
       print("Sum Value: ", sum_vector(vec));
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> Sum Value: 16.48

Underlying Functions 
--------------------
The ``sum_vector`` macro use ``_Generic`` to select the 
right function based on the vector's data type. For specific control, these 
underlying functions can be used directly:

.. code-block:: c

   char sum_char_vector(char_v* vec);
   unsigned char sum_uchar_vector(uchar_v* vec);
   short int sum_short_vector(short_v* vec);
   unsigned short int sum_ushort_vector(ushort_v* vec);
   int sum_int_vector(int_v* vec);
   unsigned int sum_uint_vector(uint_v* vec);
   long int sum_long_vector(long_v* vec);
   unsigned long int sum_ulong_vector(ulong_v* vec);
   long long int sum_llong_vector(llong_v* vec);
   unsigned long long int sum_ullong_vector(ullong_v* vec);
   float sum_float_vector(float_v* vec);
   double sum_double_vector(double_v* vec);
   long double sum_ldouble_vector(ldouble_v* vec);

Average Vector Value 
====================
The ``vector_average`` macro uses the ``_Generic`` keyword to intelligently 
select from several functions to determine the average value within a vector 
data structure. The form of the ``vector_average`` macro is shown below.

.. code-block:: c 

   #define vector_average(vec) (/* Expressions to determine the vector average */)

Parameters 
----------

- :c:`vec`: A vector data structure defined in :ref:`Vector Data Types <vector_dat_type>`.

Returns 
-------

- Calculates and returns the arithmetic mean of the elements within ``vec``.

.. note:: The return type varies based on the input vector's data type:
   
   - For ``char_v``, ``uchar_v``, ``short_v``, ``ushort_v``, and ``float_v``, the return type is ``float``.
   - For ``int_v``, ``uint_v``, ``long_v``, ``ulong_v``, and ``double_v``, the return type is ``double``.
   - For all other inputs, the return type is ``long double``.

Error Handling
--------------
The ``vector_average`` macros handle several error conditions, 
setting the ``errno`` global variable to indicate specific errors. Users 
should check ``errno`` after the function call to identify and understand 
any errors.

Possible error codes:

- ``EINVAL``: Indicates an invalid argument was passed to the function.

Example 
-------
An example with ``int_v`` data types.

.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
       int_v* vec = init_vector(dInt)(5);
       push_vector(vec, 12, vector_length(vec));
       push_vector(vec, 8, vector_length(vec));
       push_vector(vec, 22, vector_length(vec));
       push_vector(vec, 1, vector_length(vec));
       push_vector(vec, -27, vector_length(vec));
       print("Average Value: ", vector_average(vec));
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> Average Value: 3.2000

Underlying Functions 
--------------------
The ``vector_average`` macro uses ``_Generic`` to select the right function 
based on the vector's data type. For specific control, these underlying 
functions can be used directly:

.. code-block:: c

   char average_char_vector(char_v* vec);
   unsigned char average_uchar_vector(uchar_v* vec);
   short int average_short_vector(short_v* vec);
   unsigned short int average_ushort_vector(ushort_v* vec);
   int average_int_vector(int_v* vec);
   unsigned int average_uint_vector(uint_v* vec);
   long int average_long_vector(long_v* vec);
   unsigned long int average_ulong_vector(ulong_v* vec);
   long long int average_llong_vector(llong_v* vec);
   unsigned long long int average_ullong_vector(ullong_v* vec);
   float average_float_vector(float_v* vec);
   double average_double_vector(double_v* vec);
   long double average_ldouble_vector(ldouble_v* vec);

Standard Deviation of a Vector 
==============================
The ``vector_stdev`` macro uses the ``_Generic`` keyword to select the 
appropriate function to calculate and return the standard deviation of
the values in a vector data structure.  The parameters of the ``vector_stdev``
macro are shown below.

.. code-block:: c 

   #define vector_stdev(vec) ( /*Expressions to calculate standard deviation */)  

Parameters 
----------

- :c:`vec`: A vector data structure defined in :ref:`Vector Data Types <vector_dat_type>`.

Returns 
-------

- Calculates and returns the arithmetic mean of the elements within ``vec``.

.. note:: The return type varies based on the input vector's data type:
   
   - For ``char_v``, ``uchar_v``, ``short_v``, ``ushort_v``, and ``float_v``, the return type is ``float``.
   - For ``int_v``, ``uint_v``, ``long_v``, ``ulong_v``, and ``double_v``, the return type is ``double``.
   - For all other inputs, the return type is ``long double``.

Error Handling
--------------
The ``vector_stdev`` macros handle several error conditions, 
setting the ``errno`` global variable to indicate specific errors. Users 
should check ``errno`` after the function call to identify and understand 
any errors.

Possible error codes:

- ``EINVAL``: Indicates an invalid argument was passed to the function.

Example 
------- 
An example to calculate the standard deviation of a ``float_v`` type.

.. code-block:: c 

   #include "print.h"
   #include "vector.h"

   int main() {
       float_v* vec = init_vector(dFloat)(5);
       push_vector(vec, 1.f, vector_length(vec));
       push_vector(vec, 2.f, vector_length(vec));
       push_vector(vec, 10.3f, vector_length(vec));
       push_vector(vec, 4.f, vector_length(vec));
       push_vector(vec, 5.f, vector_length(vec));
       float stdev = vector_stdev(vec);
       print("Stdev: ", stdev);
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> Stdev: 3.2444414

Underlying Functions 
--------------------
The ``vector_average`` macro uses ``_Generic`` to select the right function 
based on the vector's data type. For specific control, these underlying 
functions can be used directly:

.. code-block:: c

   char stdev_char_vector(char_v* vec);
   unsigned char stdev_uchar_vector(uchar_v* vec);
   short int stdev_short_vector(short_v* vec);
   unsigned short int stdev_ushort_vector(ushort_v* vec);
   int stdev_int_vector(int_v* vec);
   unsigned int stdev_uint_vector(uint_v* vec);
   long int stdev_long_vector(long_v* vec);
   unsigned long int stdev_ulong_vector(ulong_v* vec);
   long long int stdev_llong_vector(llong_v* vec);
   unsigned long long int stdev_ullong_vector(ullong_v* vec);
   float stdev_float_vector(float_v* vec);
   double stdev_double_vector(double_v* vec);
   long double stdev_ldouble_vector(ldouble_v* vec);

Cumulative Sum of Vector 
========================
The ``vector_cumsum`` macro utilizes the ``_Generic`` keyword to select the  
appropriate function to build the cumulative sum of an input vector as 
another vector data structure.  The parameters of the ``vector_cumsum`` 
macro is shown below.

.. code-block:: c 

   #define vector_cumsum(vec) ( /* Expression to develop a cumulative sum */ )

Parameters 
----------

- :c:`vec`: A vector data structure defined in :ref:`Vector Data Types <vector_dat_type>`, except for ``bool_v`` and ``string_v``..

Returns 
-------

- Returns a vector data structure as the same type as ``vec`` with the cumulative sum of the values in ``vec``.

Error Handling
--------------
The ``vector_cumsum`` macros handle several error conditions, 
setting the ``errno`` global variable to indicate specific errors. Users 
should check ``errno`` after the function call to identify and understand 
any errors.

Possible error codes:

- ``EINVAL``: Indicates an invalid argument was passed to the function.

Example 
-------
An example with a ``double_v`` derived data type. 

.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
       float_v* vec = init_vector(dFloat)(5);
       push_vector(vec, 1.f, vector_length(vec));
       push_vector(vec, 2.f, vector_length(vec));
       push_vector(vec, 3.f, vector_length(vec));
       push_vector(vec, 4.f, vector_length(vec));
       push_vector(vec, 5.f, vector_length(vec));
       float_v* cumulative = vector_cumsum(vec);
       print("Cumulative: ", cumulative);
       free_vector(vec);
       free_vector(cumulative);
       return 0;
   }

.. code-block:: bash 

   >> Cumulative: [ 1.000000, 3.000000, 6.000000, 10.000000, 15.000000 ]

Underlying Functions 
--------------------
The ``vector_cumsum`` macro uses ``_Generic`` to select the right function 
based on the vector's data type. For specific control, these underlying 
functions can be used directly:

.. code-block:: c

   char cumsum_char_vector(char_v* vec);
   unsigned char cumsum_uchar_vector(uchar_v* vec);
   short int cumsum_short_vector(short_v* vec);
   unsigned short int cumsum_ushort_vector(ushort_v* vec);
   int cumsum_int_vector(int_v* vec);
   unsigned int cumsum_uint_vector(uint_v* vec);
   long int cumsum_long_vector(long_v* vec);
   unsigned long int cumsum_ulong_vector(ulong_v* vec);
   long long int cumsum_llong_vector(llong_v* vec);
   unsigned long long int cumsum_ullong_vector(ullong_v* vec);
   float cumsum_float_vector(float_v* vec);
   double cumsum_double_vector(double_v* vec);
   long double cumsum_ldouble_vector(ldouble_v* vec);

Copy Vector 
===========
The ``vector_copy`` macro uses the ``_Generic`` keyword to intelligently select 
the best function to create a deep copy of the input vector.  The parameters 
of the ``vector_copy`` macro are described below.

.. code-block:: c 

   #define vector_copy(vec) ( /* Expression to copy vector */ )  

Parameters 
----------

- :c:`vec`: A vector data structure defined in :ref:`Vector Data Types <vector_dat_type>`.

Returns 
-------

- Returns a vector data structure as the same type as ``vec`` with a deep copy of the values in ``vec`` to included allocated length.

Error Handling
--------------
The ``vector_copy`` macros handle several error conditions, 
setting the ``errno`` global variable to indicate specific errors. Users 
should check ``errno`` after the function call to identify and understand 
any errors.

Possible error codes:

- ``EINVAL``: Indicates an invalid argument was passed to the function.

Example 
-------
An example of ``vector_copy`` creating a deep copy of a vector.

.. code-block:: c 

   #include "print.h"
   #include "vector.h"

   int main() {
       float_v* vec = init_vector(dFloat)(5);
       push_vector(vec, 1.f, vector_length(vec));
       push_vector(vec, 2.f, vector_length(vec));
       push_vector(vec, 10.3f, vector_length(vec));
       push_vector(vec, 4.f, vector_length(vec));
       push_vector(vec, 5.f, vector_length(vec));
       float copy = vector_copy(vec);
       print("Original: ", vec);
       print("New: ", copy);
       free_vector(vec);
       free_vector(copy);
       return 0;
   }

.. code-block:: bash 

   >> Original: [ 1.00000, 2.00000, 10.30000, 4.00000, 5.00000 ]
   >> New: [ 1.00000, 2.00000, 10.30000, 4.00000, 5.00000 ] 

Underlying Functions 
--------------------
The ``vector_copy`` macro uses ``_Generic`` to select the right function 
based on the vector's data type. For specific control, these underlying 
functions can be used directly:

.. code-block:: c

   char copy_char_vector(char_v* vec);
   unsigned char copy_uchar_vector(uchar_v* vec);
   short int copy_short_vector(short_v* vec);
   unsigned short int copy_ushort_vector(ushort_v* vec);
   int copy_int_vector(int_v* vec);
   unsigned int copy_uint_vector(uint_v* vec);
   long int copy_long_vector(long_v* vec);
   unsigned long int copy_ulong_vector(ulong_v* vec);
   long long int copy_llong_vector(llong_v* vec);
   unsigned long long int copy_ullong_vector(ullong_v* vec);
   float copy_float_vector(float_v* vec);
   double copy_double_vector(double_v* vec);
   long double copy_ldouble_vector(ldouble_v* vec);
   double copy_bool_vector(double_v* vec);
   long double copy_string_vector(ldouble_v* vec);

Trim Vector
===========
It is very possible the a vector data structure may over-allocate memory,
which needs to be reduced later in the lifecycle of the vector.  The 
``trim_vector`` macro can be used to reduce the allocated memory to the minimum 
size necessary to container an array.

.. code-block:: c 

   #define trim_memory(vec)  ( /* Expression to reduce the memory allocation */) 

Parameters 
----------

- :c:`vec`: A vector data structure defined in :ref:`Vector Data Types <vector_dat_type>`.

Returns 
--------

- Returns ``true`` if function executes sucessfully, ``false`` otherwise.

Error Handling
--------------
The ``trim_vector`` macro may fail if it is unable to reallocate memory.
If this occures, the function will set the globale variable ``errno``
to a value of ``ENOMEN``.  In addition, if the user passes an invalid 
value for ``vec`` (i.e. a NULL pointer).  The function will set ``errno``
to a value of ``EINVAL``.

Possible error codes:

- ``EINVAL``: Indicates an invalid argument was passed to the function.
- ``ENOMEM``: Indicates a failure to allocate memory.

Example 
-------
An example with a ``double_v`` derived data type. 

.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
        double_v* vec = init_vector(dDouble)(15);
        push_vector(vec, 1, vector_length(vec));
        push_vector(vec, 2, vector_length(vec));
        push_vector(vec, 3, vector_length(vec));
        push_vector(vec, 4, vector_length(vec));
        push_vector(vec, 5, vector_length(vec));
        trim_vector(vec); 
        print(vec->alloc);
        free_vector(vec);
        return 0;
    }

.. code-block:: bash 

   >> 5

Underlying Functions 
--------------------
The ``trim_vector`` macro uses ``_Generic`` to select the right function 
based on the vector's data type. For specific control, these underlying 
functions can be used directly:

.. code-block:: c

   char trim_char_vector(char_v* vec);
   unsigned char trim_uchar_vector(uchar_v* vec);
   short int trim_short_vector(short_v* vec);
   unsigned short int trim_ushort_vector(ushort_v* vec);
   int trim_int_vector(int_v* vec);
   unsigned int trim_uint_vector(uint_v* vec);
   long int trim_long_vector(long_v* vec);
   unsigned long int trim_ulong_vector(ulong_v* vec);
   long long int trim_llong_vector(llong_v* vec);
   unsigned long long int trim_ullong_vector(ullong_v* vec);
   float trim_float_vector(float_v* vec);
   double trim_double_vector(double_v* vec);
   long double trim_ldouble_vector(ldouble_v* vec);
   double trim_bool_vector(double_v* vec);
   long double trim_string_vector(ldouble_v* vec);

Replace Vector Index 
====================
The ``replace_vector_index`` macro utilizes the ``_Generic`` keyword to select 
from several functions that replace data at a user-specified index in a vector 
data structure. 

.. note:: This macro supports string literals for replacing string values in a ``string_v`` array. To replace a string value with a ``str`` data type, use the ``replace_str_vector_index`` function.

.. code-block:: c 

   #define replace_vector_index(vec, dat, index) ( /* Expression to replace index value */ )  
   bool replace_str_vector_index(string_v* vec, str* dat, size_t index);

Parameters 
----------

- :c:`vec`: A vector data structure defined in :ref:`Vector Data Types <vector_dat_type>`.
- :c:`dat`: The data to replace the current value at ``index``.
- :c:`index`: The index where data will be replaced.

Return 
------

- Returns ``true`` if the operation is successful, ``false`` otherwise.

Error Handling
--------------
The ``replace_vector_index`` macro may encounter errors such as memory 
allocation failure or invalid input. If these occur, underlying functions set 
the global variable ``errno`` to indicate the specific error.

Possible error codes:

- ``EINVAL``: Invalid argument was passed to the function.
- ``ENOMEM``: Memory allocation failure.
- ``ERANGE``: Index is out of bounds.

Example 1 
---------
Replacing a string value with a string literal:

.. code-block:: c 

   #include "print.h"
   #include "vector.h"

   int main() {
       string_v* vec = init_vector(dString)(4);
       push_vector(vec, "This", 0);
       push_vector(vec, "Is", 1);
       push_vector(vec, "a", 2);
       push_vector(vec, "Test", 3);
       print(vec);
       replace_vector_index(vec, "Trap", 3);
       print(vec);
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> This is a Test 
   >> This is a Trap

Example 2
---------
An example showing the replacement of a string value with a ``str`` data container.


.. code-block:: c 

   #include "print.h"
   #include "vector.h"

   int main() {
       string_v* vec = init_vector(dString)(4);
       push_vector(vec, "This", 0);
       push_vector(vec, "Is", 1);
       push_vector(vec, "a", 2);
       push_vector(vec, "Test", 3);
       print(vec);
       str* a = init_string("Trap");
       replace_str_vector_index(vec, a, 3);
       print(vec);
       free_vector(vec);
       free_string(a);
       return 0;
   }

.. code-block:: bash 

   >> This is a Test 
   >> This is a Trap

Underlying Functions 
--------------------
The ``replace_vector_index`` macro uses ``_Generic`` to select the right function 
based on the vector's data type. For specific control, these underlying 
functions can be used directly:

.. code-block:: c 

   bool replace_char_vector_index(char_v* vec, char dat, size_t index);
   bool replace_uchar_vector_index(uchar_v* vec, unsigned char dat, size_t index);
   bool replace_short_vector_index(short_v* vec, short int dat, size_t index);
   bool replace_ushort_vector_index(ushort_v* vec, unsigned short int dat, size_t index);
   bool replace_int_vector_index(int_v* vec, int dat, size_t index);
   bool replace_uint_vector_index(uint_v* vec, unsigned int dat, size_t index);
   bool replace_long_vector_index(long_v* vec, long int dat, size_t index);
   bool replace_ulong_vector_index(ulong_v* vec, unsigned long int dat, size_t index);
   bool replace_llong_vector_index(llong_v* vec, long long int dat, size_t index);
   bool replace_ullong_vector_index(ullong_v* vec, unsigned long long int dat, size_t index);
   bool replace_float_vector_index(float_v* vec, float dat, size_t index);
   bool replace_double_vector_index(double_v* vec, double dat, size_t index);
   bool replace_ldouble_vector_index(ldouble_v* vec, long double dat, size_t index);
   bool replace_bool_vector_index(bool_v* vec, bool dat, size_t index);
   bool replace_string_vector_index(string_v* vec, char* dat, size_t index);
   bool replace_str_vector_index(string_v* vec, str* dat, size_t index);

.. _vector_iterator:

Vector Iterator Struct 
======================
While vectors are stored in contiguous memory and can be accessed using for 
loops, other data structures may not support such direct access. To provide a 
uniform method for traversing different data structures, this library offers 
a generic iterator mechanism.

The core of this mechanism is a struct of function pointers, each corresponding 
to a typical iterator operation. These function pointers allow for traversing 
and accessing elements within the data structure. The general form of this 
iterator struct is defined using the following macro:

.. code-block:: c 

   #define VECTOR_ITERATOR(type_one, type_two) \
       typedef struct { \
           type_one* (*begin) (type_two *s); \
           type_one* (*end) (type_two *s); \
           void (*next) (type_one** current); \
           void (*prev) (type_one** current); \
           type_one (*get) (type_one** current); \
       } type_two##_iterator;

Parameters 
----------

- :c:`type_one`: The derived vector data type consistent with :ref:`Vector Data Types <vector_dat_type>`. 
- :c:`type_two`: The C data type that is consistent with :ref:`Vector Data Types <vector_dat_type>`. 

Derived Iterator Types 
----------------------
The `VECTOR_ITERATOR` macro is used to create specific iterator types for 
various data structures, including a specialized iterator for `string_v`, 
which is slightly different in its `get` function:

.. code-block:: c 

   VECTOR_ITERATOR(char, char_v)
   VECTOR_ITERATOR(unsigned char, uchar_v)
   VECTOR_ITERATOR(short int, short_v)
   VECTOR_ITERATOR(unsigned short int, ushort_v)
   VECTOR_ITERATOR(int, int_v)
   VECTOR_ITERATOR(unsigned int, uint_v)
   VECTOR_ITERATOR(long int, long_v)
   VECTOR_ITERATOR(unsigned long int, ulong_v)
   VECTOR_ITERATOR(long long int, llong_v)
   VECTOR_ITERATOR(unsigned long long int, ullong_v)
   VECTOR_ITERATOR(float, float_v)
   VECTOR_ITERATOR(double, double_v)
   VECTOR_ITERATOR(long double, ldouble_v)
   VECTOR_ITERATOR(bool, bool_v)

   typedef struct {
       str* (*begin) (string_v *s);
       str* (*end) (string_v* s);
       void (*next) (str** current);
       void (*prev) (str** current);
       char* (*get) (str** current);
   } string_v_iterator;

Vector Iterator 
===============
While vectors are stored in contiguous memory and can be accessed using for 
loops, not all data structures support such direct access. To provide a 
uniform method for traversing different data structures, this library offers 
a generic iterator mechanism. The `vector_iterator` macro allows easy access 
to the appropriate iterator for a given vector type.

.. code-block:: c 

   #define vector_iterator(vec) ( /* Expression to select iterator */) 
   
Parameters 
----------

- :c:`vec`: A vector data structure defined in :ref:`Vector Data Types <vector_dat_type>`.

Return 
------

- A struct of types described in :ref:`Vector Iterator <vector_iterator>` that contains function pointers to applicable iterator functions.

.. note:: Passing the value of ``vec`` to the ``vector_iterator`` macro does **NOT** associate the data of the vector with the iterator, and instead this only informs the macro of the data type to use.

Error Handling
--------------
The `vector_iterator` macro selects the appropriate iterator based on the 
vector's data type. If an error occurs, such as an invalid vector type or 
memory allocation failure, the underlying functions set `errno` to indicate 
the specific error.

Possible error codes:

- ``EINVAL``: Invalid argument was passed to the function.

Example 1 
---------
An example using a ``string_v_iterator``.

.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
       string_v* vec = init_vector(dString)(4);
       push_vector(vec, "One", vector_length(vec));
       push_vector(vec, "Two", vector_length(vec));
       push_vector(vec, "Three", vector_length(vec));
       push_vector(vec, "Four", vector_length(vec));
       string_v_iterator it = init_vector(vec);
       str* begin = it.begin(vec);
       str* end = it.end(vec);
       for (str* i = begin; i != end; it.next(&i)) {
           print(it.get(&i));
       }
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> One 
   >> Two 
   >> Three 
   >> Four

Underlying Functions 
--------------------
The ``vector_iterator`` macro uses ``_Generic`` to select the right function 
based on the vector's data type. For specific control, these underlying 
functions can be used directly:

.. code-block:: c 

   char_v_iterator init_char_vector_iterator();
   uchar_v_iterator init_uchar_vector_iterator();
   short_v_iterator init_short_vector_iterator();
   ushort_v_iterator init_ushort_vector_iterator();
   int_v_iterator init_int_vector_iterator();
   uint_v_iterator init_uint_vector_iterator();
   long_v_iterator init_long_vector_iterator();
   ulong_v_iterator init_ulong_vector_iterator();
   llong_v_iterator init_llong_vector_iterator();
   ullong_v_iterator init_ullong_vector_iterator();
   float_v_iterator init_float_vector_iterator();
   double_v_iterator init_double_vector_iterator();
   ldouble_v_iterator init_ldouble_vector_iterator();
   bool_v_iterator init_bool_vector_iterator();
   string_v_iterator init_string_vector_iterator();
