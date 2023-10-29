*****
Print
*****
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

   #define "print.h"

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

   #define "print.h"
   #define "str.h"

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

   #define "print.h"
   #define "str.h"

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


