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

Example 
-------
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
       return 0;
   }

.. code-block:: bash 

   >> Vector: [ 2.1, 7.4, 1.1, 43.5, 13.8, 7.7 ]
   >> Vector Length: 6

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
