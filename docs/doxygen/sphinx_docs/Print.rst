*****
Print
*****
The C language natively includes the ``printf`` function for formatted output 
to ``stdout``. However, for simpler use cases where formatting isn't needed, 
the ``print`` macro in the ``print.h`` header file offers an easy alternative. 
It allows printing up to 8 variables of any standard data type, including those 
defined in this library, without explicit format specifiers.

Standard Data Types
===================
The ``print`` macro can handle any standard C data type, including but not 
limited to ``char``, ``unsigned char``, ``int``, ``unsigned int``, ``long int``, 
``unsigned long int``, ``long long int``, ``unsigned long long int``, 
``float``, ``double``, ``long double``, ``bool``, and strings (``char*``). 
Below is an example demonstrating its usage.

Eaxmple
-------
A basic example for the use of the ``print`` macro.

.. code-block:: c 

   #include "print.h"

   int main() {
       int a = 1;
       long int b = 2;
       double c = 3.4;
       print("An int: ", a, ", A long int: ", b, ", A double: ", c);
       return 0;
   }

.. code-block:: bash 

   >> An int: 1, A long int: 2, A double 3.40000

String Container
================
The library introduces the ``str`` data type, a dynamically allocated structure 
that encapsulates a string literal along with its metadata. This struct offers 
a more robust and flexible way of handling strings in C. The ``str`` struct is 
elaborated in the :ref:`String <string_struct>` section of this document. 
The ``print`` macro is capable of printing the ``str`` data type, treating it 
as one of the up to eight variables it can handle in a single call.

Example 1
---------
This example demonstrates using the ``print`` macro to print a literal string 
alongside a ``str`` data type.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       char* literal = "This is a string literal.\n";
       str* container = init_string("This is a string container!");
       print(literal, container);
       // Remember to free dynamically allocated memory
       free_string(container);
       return 0;
   }

.. code-block:: bash 

   >> This is a string literal.
   >> This is a string container!

Example 2
---------
The ``str`` data type is dynamically allocated and may be freed during program 
execution. It's crucial to ensure that a `str` object is not printed after it 
has been freed. The ``print`` macro includes logic to handle such scenarios 
gracefully by checking for NULL pointers and providing informative messages.

.. note:: While most compilers detect and prevent operations on freed memory during compilation, accessing freed memory is undefined behavior in C. The `print` macro's checks serve as an additional safeguard.

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       str* container = init_string("This is a string container!");
       free_string(container);
       // Attempt to print a null struct 
       print(container);
        
       // Attempt to print a struct with a null string
       str empty = { .data = NULL, .len = 0, .alloc = 0 }; 
       print(&empty);

       return 0;
   }

.. code-block:: bash 

   >> Error: The struct is a NULL pointer.
   >> Error: The string is a NULL pointer.

Vector
======
This library contains derived data types for dynamically allocated vectors 
described in the :ref:`Vector <vector_lib>` section of this document. the
``print`` macro is programmed to recognize the vector data types and print them
on a single line or multiple lines if the vector is long enough.  Any print out
of the vector data types will be encapsulated in a set of square brackets 
(i.e. ``[ ]``). 

Example 1
---------
An example showing the print out of a vector data type after a string.

.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
       int_v* vec = init_vector(dInt)(5);
       push_vector(vec, 1, vec->len);
       push_vector(vec, 2, vec->len);
       push_vector(vec, 3, vec->len);
       push_vector(vec, 4, 0);
       print("This is a vector: ", vec);
       free_vector(vec);
       return 0;
   }

.. code-block:: bash 

   >> This is a vector: [ 4, 1, 2, 3 ]

The C language out of the box comes with the ``printf`` function that can be used
to print to ``stdout`` with formatting.  However, sometimes a user may want to print
to ``stdout`` and not have to worry about formatting.  The ``print`` statement in 
the ``print.h`` header file can be an easy to use alternative to the ``printf`` 
function and allows a user the ability to print up to 8 variables of any standard data 
type in the function, to include all data types created in this library.

Standard Data 
=============
The ``print`` macro can print any standard data type to include ``char``, 
``unsigned char``, ``int``, ``unsigned int``, ``long int``, ``unsigned long int``,
``long long int``, ``unsigned long long int``, ``float``, ``double``, 
``long double``, ``bool``, and ``char*``. An example is shown below.

Eaxmple
-------
A basic example for the use of the ``print`` macro.

.. code-block:: c 

   #include "print.h"

   int main() {
       int a = 1;
       long int b = 2;
       double c = 3.4;
       print("An int: ", a, ", A long int: ", b, ", A double: ", c);
       return 0;
   }

.. code-block:: bash 

   >> An int: 1, A long int: 2, A double 3.40000

String Container
================
This library introductes the ``str`` data type, which is a dynamically allocated
struct which contains a string literal and its metadata.  The ``str`` struct 
is further described in the :ref:`String <string_struct>` section of this document.
The ``str`` data type can be printed as one of or all of the 8 variables in the 
``print`` macro.

Example 1
---------
This example demonstrates the use of the ``print`` macro to print a ``str`` 
data type.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       char* one = "This is a string literal.\n";
       str* two gbc_str init_string("This is a string container!");
       print(one, two);
       return 0;
   }

.. code-block:: bash 

   >> This is a string literal.
   >> This is a string container!

Example 2
---------
The string container is dynamically allocated.  It is possible that a
developer may try to print the container after it has been freed from 
memory.  The ``print`` statement has logic to catch this error and will
print a message to ``stdout`` to let the user know of the error.

**NOTE:** Most compilers should catch any attempt to print memory that has 
been freed and prevent compilation.  However, in case this does not occur,
this example demonstrates the logic for a fail nicely approach.

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       
       str* one init_string("This is a string container!");
       free_string(one);
       // Attempt to print a null struct 
       print(one);
        
       // Attempt to print a null string
       str two = { .data = NULL, .len = 0, .alloc = 0 }; 
       print(&two);

       return 0;
   }

.. code-block:: bash 

   >> The struct is a NULL pointer
   >> The string is a NULL pointer

Vector
======
The library enhances its functionality with derived data types for dynamically 
allocated vectors, as detailed in the :ref:`Vector <vector_lib>` section. 
The ``print`` macro is specially designed to handle these vector data types. 
It prints the contents of a vector, encapsulated within square brackets 
(i.e., ``[ ]``), either on a single line or across multiple lines for longer 
vectors, providing a clear and concise representation of the vector's contents.

Example 1
---------
This example demonstrates printing a vector data type alongside a string. The 
macro automatically formats the vector output for readability.

.. code-block:: c

   #include "print.h"
   #include "vector.h"

   int main() {
       int_v* vec = init_vector(dInt)(5);
       push_vector(vec, 1, vec->len);
       push_vector(vec, 2, vec->len);
       push_vector(vec, 3, vec->len);
       push_vector(vec, 4, 0);
       print("This is a vector: ", vec);
       free_vector(vec); // Always remember to free dynamically allocated memory
       return 0;
   }

.. code-block:: bash 

   >> This is a vector: [ 4, 1, 2, 3 ]

.. note:: The `print` macro adapts its output format based on the vector's length, ensuring optimal readability. For longer vectors, the output may span multiple lines to avoid overly long single-line outputs.

