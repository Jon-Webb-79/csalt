*******
Strings
*******
The standard C library does not have a string data type and instead allows 
users to create string literals, or they can create an array of ``char``
characters like shown.  While the C implementation for string literals 
is easy to use and is fairly efficient, it can lead to security issues,
most noteably a buffer overflow attack.  This chapter outlines a ``str``
data type created for this library, that can help minimize the security issues 
inherent with C and can also increase the speed with which the C code and 
process strings.  All functionality described in this Chapter can be
accessed through the ``str.h`` header file.

.. role:: c(code)
   :language: c

Structure
=========
The ``str`` library relies on the following struct to contain information

.. code-block:: c

   typedef struct
   {
       char *data;
       size_t len;
       size_t alloc;
   } str;

Attributes
----------

- :c:`data`: A Pointer to the string
- :c:`len`: The length of the string in number of bytes, minus the null terminator ``\0``
- :c:`alloc`: The number of bytes allocated to the ``data`` pointer.

.. _init_string:

Initialize String 
=================
All strings in this library are encapsulated in a dynamically allocated 
``str`` struct.  In addition, the memory required to store the string 
within the struct is also dynamically allocated.  This means that the 
string container and string needs to be freed after it has served its 
purpose.  There are two methods that can be used to initialize a string 
variable of type ``str``.

The first method for initializing a string data type uses the ``init_string``
macro described below.  In the implementation shown below, the variable 
``buff`` is optional.  If it is not provided, the underlying function will 
allocate enough memory for the string and the null terminator.  If the value 
of ``buff`` is less than the size of ``string`` plus the null terminator,
the function will allocate enoug memory for the string and its null terminator.
A user may decide to invoke a value of ``buff`` that is larger than the initial 
string, if the string may grow in the future.  This will prevent the need 
for allocating memory which can be time consuming.

.. code-block:: bash 

   str* init_string(char* string, size_t buff);

Parameters 
----------

- :c:`string`: A string literal.  If the user passes a character string, the function will attempt to add a null terminator, but it could lead to undefined behavior.
- :c:`buff`: The number of characters to be allocated to store the ``string`` and any excess necessary.  This value is defaulted to the length of ``string``

Returns
-------

- :c:`str`: A string containter of type ``str``.

Example 1
---------
In this example we will initialize a string with a memory allocation just for
the string. Notice that we have to manually free the memory in the struct 
and the string with the ``free_string`` function.  You can also use the 
``FREE_DAT`` macro which can be invoked from the ``dat_struct.h`` file.
**NOTICE:** the fact that their is no equal sign between the variable 
name and the macro.  The equal sign is accounted for in the macro.

.. code-block:: c 

   #include "str.h"
   #include "print.h"

   int main() {
       str *one init_string("Hello World!");
       print(one);
       print(string_length(one));
       // - This command is only used for demonstration.  Users should not try
       //   to directly access struct attributes.
       print(one->alloc);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> Hello World!
   >> 11
   >> 12


Example 2 
---------
In this example we will initialize a string with a larger than necessary buffer 
in order to reduce the time necessary to add characters to the string if 
necessary later in the program.  This removes the need to reallocate memory.
**NOTICE:** the fact that their is no equal sign between the variable 
name and the macro.  The equal sign is accounted for in the macro.

.. code-block:: c 

   #include "str.h"
   #include "print.h"

   int main() {
       str *one init_string("Hello World!", 30);
       print(one);
       print(string_length(one));
       // - This command is only used for demonstration.  Users should not try
       //   to directly access struct attributes.
       print(one->alloc);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> Hello World! 
   >> 11 
   >> 30

Underlying Functions
--------------------
The ``init_string`` macro uses the ``_Generic`` operator to wrap two 
functions, which can be accessed in their place.  These functions are 
described below.

.. code-block:: bash 

   str init_string_nol(char *string);
   str init_string_len(char *string, size_t buff);

The example below shows how to use these functions.

.. code-block:: bash 
   
   #include "print.h"
   #include "str.h"
  
   int main() {
       str *one = init_string_nol("Hello!");
       str *two = init_string_len("Hello!", 30);
       free_string(one);
       free_string(two);
       return 0;
   }



Initialize String with Garbage Collection
=========================================
The user can also initialize a string such that it will be automatically 
garbage collected when it goes out of scope with the ``init_string_gbc``
macro.  This macro uses the same underlying functions as the ``init_string``
macro, but they are invoked with the ``__attribute__(cleanup))`` operator 
that only works with ``gcc`` and ``clang`` compilers.  

See the :ref:`init string <init_string>` Section for a description of all 
attributes.  The examples below show how to use this macro, notice that the 
``free_string`` function is not called.

Example 1
---------
In this example we will initialize a string with a memory allocation just for
the string. Notice that we have to manually free the memory in the struct 
and the string with the ``free_string`` function.  You can also use the 
``FREE_DAT`` macro which can be invoked from the ``dat_struct.h`` file.
**NOTICE:** the fact that their is no equal sign between the variable 
name and the macro.  The equal sign is accounted for in the macro.

.. code-block:: c 

   #include "str.h"
   #include "print.h"

   int main() {
       str *one init_string("Hello World!");
       print(one);
       print(string_length(one));
       // - This command is only used for demonstration.  Users should not try
       //   to directly access struct attributes.
       print(one->alloc);
       return 0;
   }

.. code-block:: bash 

   >> Hello World!
   >> 11
   >> 12


Example 2 
---------
In this example we will initialize a string with a larger than necessary buffer 
in order to reduce the time necessary to add characters to the string if 
necessary later in the program.  This removes the need to reallocate memory.
**NOTICE:** the fact that their is no equal sign between the variable 
name and the macro.  The equal sign is accounted for in the macro.

.. code-block:: c 

   #include "str.h"
   #include "print.h"

   int main() {
       str *one init_string("Hello World!", 30);
       print(one);
       print(string_length(one));
       // - This command is only used for demonstration.  Users should not try
       //   to directly access struct attributes.
       print(one->alloc);
       return 0;
   }

.. code-block:: bash 

   >> Hello World! 
   >> 11 
   >> 30



