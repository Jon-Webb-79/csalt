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
for allocating memory which can be time consuming.  This method has a
space complexity of :math:`O(1)` and a time complexity of :math:`O(n)`.

.. code-block:: c

   str* init_string(char* string, size_t buff);

Parameters 
----------

- :c:`string`: A string literal.  If the user passes a character string, the function will attempt to add a null terminator, but it could lead to undefined behavior.
- :c:`buff`: The number of characters to be allocated to store the ``string`` and any excess necessary.  This value is defaulted to the length of ``string``

Returns
-------

- :c:`str`: A string containter of type ``str``. Will return NULL and print to stderr if a memory allocation error occurs.

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
       print(string_memory(one));
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
       print(string_memory(one));
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

.. code-block:: c 
   
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
``free_string`` function is not called.  While this initializing function will 
automate the process of freeing memory, a user can still use the ``free_string``
function if they decide to free memory manually. This method has a
space complexity of :math:`O(1)` and a time complexity of :math:`O(n)`. 

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
       str *one init_string_gbc("Hello World!");
       print(one);
       print(string_length(one));
       print(string_memory(one));
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
       str *one init_string_gbc("Hello World!", 30);
       print(one);
       print(string_length(one));
       print(string_memory(one));
       return 0;
   }

.. code-block:: bash 

   >> Hello World! 
   >> 11 
   >> 30

Free String 
===========
The ``free_string`` function can be used to free all memory in an ``str``
Struct to include the Struct itself. This method has a
space complexity of :math:`O(1)` and a time complexity of :math:`O(1)`. 

.. code-block:: c

   void free_string(str str_struct);

Parameters 
----------

- :c:`str_struct`: A string struct of type ``str``

Example 
-------

.. code-block:: c 

   #include "str.h"
   #include "print.h"

   int main() {
       str *one init_string_gbc("Hello World!", 30);
       free_string(one);
       return 0;
   }

Get String
==========
While the user can directly interface with the ``str`` struct, it is not wise 
to do so, since it enables the user to accidentally change an attribute that
could cause undefined behavior.  The ``get_string`` function allows a user 
to access the string variable in the ``str`` struct. This method has a
space complexity of :math:`O(1)` and a time complexity of :math:`O(1)`. 

.. code-block:: c

   char* get_string(str *str_struct);

Parameters 
----------

- :c:`str_struct`: A string container of type ``str``.

Returns 
-------

- :c:`string`: A string of type ``char*``. Will return NULL and print to stderr if input points to NULL struct or string.

Example 
-------
The following example shows how the ``get_string`` function can be used to 
retrieve a string.

.. code-block:: c 

   #include "str.h"
   #include "print.h"

   int main() {
       str *one init_string("Hello World!");
       print(get_string(one));
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> Hello World!

String Length 
=============
While the user can directly interface with the ``str`` struct, it is not wise 
to do so, since it enables the user to accidentally change an attribute that
could cause undefined behavior.  The ``string_length`` function allows a user 
to access the length of the string variable in the ``str`` struct. Unlike 
string literals in the C language, this container does not rely on a null 
terminator to determine the string length, but instead an attribute of the 
``str`` struct. This method has a
space complexity of :math:`O(1)` and a time complexity of :math:`O(1)`. 

.. code-block:: c

   size_t string_length(str *str_struct);

Parameters 
----------

- :c:`str_struct`: A string container of type ``str``.  Will return -1 if input points to NULL struct or string.

Returns 
-------

- :c:`len`: The length of the string in the ``str`` container minus the null terminator.

Example 
-------
The following example shows how the ``get_string`` function can be used to 
retrieve a string.

.. code-block:: c 

   #include "str.h"
   #include "print.h"

   int main() {
       str *one init_string("Hello World!");
       print(string_length(one));
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> 11

String Memory 
=============
The ``string_memory`` function returns to a user the memory allocation for the 
string in units of ``chars``.  The user can also access the memory via the
``struct->alloc`` attribute; however, it can be dangerous to directly access 
a struct attribute.  If a user were to accidentally change a value in a struct 
attribute it could lead to undefined behavior.  This function will return a -1 
and print to ``stderr`` if the user passes a NULL struct or a struct with 
a NULL pointer to ``data``.

.. code-block:: c 

   size_t string_memory(str *str_struct);

Parameters 
----------

- :c:`str_struct`: A struct of type str 

Returns 
-------

- :c:`alloc`: The memory allocation in units of ``chars``.

Example 
-------

.. code-block:: c 

   #define "print.h"
   #define "str.h"

   int main() {
       str *one init_string("Hello", 20);
       print("The string size is: ", string_memory(one));
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> The string size is: 20

Insert String 
=============
The ``insert_string`` macro allows a user to insert a string literal 
or another ``str`` container into a ``str`` container.  The underlying 
functions will allow a user to insert the struct anywhere into the string.
This function will return a false if the user supplies data that points to 
NULL values, or if it is not able to allocate sufficient memory for the string 
concatenations.  This macro and its underlying functions have a time 
complexity of :math:`O(a+b)` where :math:`a` and :math:`b` are the lengths
of the first and second strings.  However, if you insert at the end of string 
:math:`a` this method will only be of order :math:`O(b)`.  The function also
has a memory complexity of :math:`O(a+b)`. In addition, if the buffer does 
original and inserted strings together, doubling that number plus one, and 
it will re-allocate memory for that buffer size.

.. code-block:: c 

   bool insert_string(str *str_one, char* || str* str_two, size_t index);

Parameters
----------

- :c:`str_one`: A string container of type ``str``. This is the string that will be inserted into.
- :c:`str_two`: A string literal or string container of type ``str``.
- :c:`index`: The index where ``str_two`` will be inserted into ``str_one``.

Returns
-------

- :c:`err_code`: true if the function executes succesfully, false otherwise with a ``stderr`` print out.

Example 1 
---------
This example shows a use where a string literal is inserted into a ``str`` container.

.. code-block:: c 

   #define "print.h"
   #define "str.h"

   int main() {
       str *a init_string("Hello");
       bool result = insert_string(a, " World!", get_length(one));
       print(result);
       print(a);
       print(string_length(one));
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> true
   >> Hello World!
   >> 11

Example 2
---------
This example shows a user where a ``str`` container is inserted into another 
``str`` container.

.. code-block:: c 

   #define "print.h"
   #define "str.h"

   int main() {
       str *a init_string("Hello");
       str *b init_string(" World!");
       bool result = insert_string(a, b, 2);
       print(result);
       print(a);
       print(string_length(one));
       free_string(one);
       free_string(two);
       return 0;
   }

.. code-block:: bash 

   >> true
   >> He World!llo
   >> 11

Example 3
---------
This example shows how the function fails when an index out of bounds is
selected.  The function can also fail for a failure to reallocate memory 
if required, or if the user passes a NULL ``str`` container or string literal,
or if one of the ``str`` containers has a NULL pointer to its string.


.. code-block:: c 

   #define "print.h"
   #define "str.h"

   int main() {
       str *a init_string("Hello");
       str *b init_string(" World!");
       bool result = insert_string(a, b, 50);
       print(result);
       print(a);
       print(string_length(one));
       free_string(one);
       free_string(two);
       return 0;
   }

.. code-block:: bash 

   >> String insert location out of bounds
   >> false
   >> Hello
   >> 5

Underlying Functions 
--------------------
The ``insert_string`` macro relies on a ``_Generic`` operator that connects
the following two functions which can be used in place of the ``insert_string``
macro.

.. code-block:: bash 

   bool insert_string_lit(str *str_struct, char *string, size_t index);
   bool insert_string_str(str *str_struct_one, str *str_struct_two, size_t index);

.. code-block:: c 

   #define "print.h"
   #define "str.h"

   int main() {
       str *a init_string("Hello");
       str *b init_string(" World!");
       bool result = insert_string(a, b, 2);
       print(result);
       print(a);
       print(string_length(one));
       free_string(one);
       free_string(two);
       return 0;
   }

.. code-block:: bash 

   >> true
   >> He World!llo
   >> 11

Trim String 
===========
The process of initializing a string can lead to an oversized memory allocation 
that is later deemed un-necessary.  The ``trim_string`` function will downsize 
the memory to the minimum necessary allocation.  This function will return ``true``
if succesfully executed and ``false`` if unsuccesful.  The function may return 
false if the memory is undersized, or if the user passes a NULL struct or string to 
the function.

.. code-block:: c 

   bool trim_string(str *str_struct);

Parameters 
----------

- :c:`str_struct`: A string container of type ``str``.

Returns 
-------

- :c:`err_code`: true if the function executes succesfully, false otherwise with a stderr print out.

Example 1
---------
Example for an oversized string 

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       // String is oversized in memory
       str *one init_string("Hello", 30);
       print(string_length(one));
       print(string_memory(one));
       bool val = trim_string(one);
       print(val);
       print(string_length(one));
       print(string_memory(one));
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> 5
   >> 30
   >> true 
   >> 5 
   >> 6

Example 2 
---------
Example for a properly sized string 

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       // String is properly sized in memory
       str *one init_string("Hello");
       print(string_length(one));
       print(string_memory(one));
       bool val = trim_string(one);
       print(val);
       print(string_length(one));
       print(string_memory(one));
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> 5
   >> 6
   >> true 
   >> 5 
   >> 6

Example 3 
---------
Example where a NULL struct is passed to function.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       // Passing a NULL struct
       bool val = trim_string(NULL);
       print(val);
       return 0;
   }

.. code-block:: bash 

   >> Null pointer provided to trim_string
   >> false

Resize String 
=============
TBD

Copy String 
===========
TBD

Compare String 
==============
TBD 


