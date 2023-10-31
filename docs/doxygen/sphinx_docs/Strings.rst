.. _string_struct:

*******
Strings
*******
The standard C library does not have a string data type and instead allows 
users to create string literals, or they can create an array of ``char``
characters.  While the C implementation for string literals 
is easy to use and is fairly efficient, it can lead to security issues,
most noteably a buffer overflow attack.  This chapter outlines a ``str``
data type created for this library, that can help minimize the security issues 
inherent with C and can also increase the speed with which the C code 
process strings.  All functionality described in this Chapter can be
accessed through the ``str.h`` header file.

**DISCLAIMER:** This section of the csalt library will be updated with more
functionality in the future.  In addition, their are certain macros that 
enable garbage collection that only work on gcc and clang compilers.  The 
software should still compile on other compilers, but it will have slightly
reduced functionality.

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
purpose.

The ``init_string`` macro is used to instantiate and initialize a string 
container.  The ``init_string`` macro wraps two functions in order to 
provide an default values. In the implementation shown below, the variable 
``buff`` is optional.  If ``buff`` is not provided, the underlying function will 
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
and the string with the ``free_string`` function.

.. code-block:: c 

   #include "str.h"
   #include "print.h"

   int main() {
       str *one = init_string("Hello World!");
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

.. code-block:: c 

   #include "str.h"
   #include "print.h"

   int main() {
       str *one = init_string("Hello World!", 30);
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
Their are serval functions in this library that return a ``str*`` data 
type to include the ``init_string`` macro.  When a function returns a 
``str*`` data type, the developer is then obligated to free the memory 
allocations at some point in the program.  Instead of manually freeing the 
memory, a developer can also choose to utilyze the ``gbc_str`` macro that 
wraps the ``__attribute__(cleanup)`` operator.  This will assign the 
returned variable of type ``str*`` for cleanup and eventual garbage collection.
**NOTE:** This macro only works with gcc and clang compilers. In addition, 
developers can also use the ``free_dat`` macro from the ``dat_struct.h``
library, which has the advantage of providing a common interface for freeing 
all data types in this library.

See the :ref:`init string <init_string>` Section for a description of all 
attributes.  The examples below show how to use this macro, notice that the 
``free_string`` function is not called.  While this initializing function will 
automate the process of freeing memory, a user can still use the ``free_string``
macro if they decide to free memory manually. This method has a
space complexity of :math:`O(1)` and a time complexity of :math:`O(n)`. 
**NOTE:** Once memory has been freed, the user should try to avoid accidentally 
freeing the memory again; however, the underling function does have checks 
to prevent accidentally freeing memory that has already been freed.

Example 1
---------
In this example we will initialize a string with a memory allocation just for
the string. Notice that we do not have to manually free the memory in the struct 
and the string with the ``free_string`` function.

.. code-block:: c 

   #include "str.h"
   #include "print.h"

   int main() {
       str *one gbc_str = init_string("Hello World!");
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
       str *one gbc_str = init_string("Hello World!", 30);
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
The ``free_string`` macro can be used to free all memory in an ``str``
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
       str *one = init_string("Hello World!", 30);
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
       str *one = init_string("Hello World!");
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
       str *one = init_string("Hello World!");
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
       str *one = init_string("Hello", 20);
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
       str *a = init_string("Hello");
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
       str *a = init_string("Hello");
       str *b = init_string(" World!");
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
       str *a = init_string("Hello");
       str *b = init_string(" World!");
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

.. code-block:: c

   bool insert_string_lit(str *str_struct, char *string, size_t index);
   bool insert_string_str(str *str_struct_one, str *str_struct_two, size_t index);

.. code-block:: c 

   #define "print.h"
   #define "str.h"

   int main() {
       str *a = init_string("Hello");
       str *b = init_string(" World!");
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
       str *one = init_string("Hello", 30);
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
       str *one = init_string("Hello");
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

Copy String 
===========
The ``copy_string`` function will create a deep copy of a string container to
include the memory allocation.

.. code-block:: c 

   str* copy_string(str *str_struct);

Parameters 
----------

- :c:`str_struct`: A string container of type ``str``

Returns
-------

- :c:`str_copy`: A copy of the input ``str`` struct.

Example 
-------

.. code-block:: c 

   #include "str.h"
   #include "print.h"

   int main() {
       str *one = init_string("Hello", 20);
       str *two = copy_string(one);
       print(get_string(two));
       print(string_length(two));
       print(string_memory(two));
       return 0;
   }
.. code-block:: bash 

   >> Hello 
   >> 5 
   >> 20

Compare String 
==============
The ``compare_strings`` macro can be used to compare a string container to
another string container or a string literal. This macro utilizes the ``_Generic``
operator to select from one of two functions that allows a user to compare 
a string container of type ``str`` with another ``str`` container or a string 
literal.  The function will compare each ``char`` to see if they match.  The 
difference between the first non matching characters will be returned as an
integer difference, unless all ``char``'s match in which case it will return 
0.  If the strings have a different length, the function will return the 
difference in the lengths of the strings with the first container being the 
basis for the difference.  Finally, if the user passes a NULL pointer as the 
container, string literal, or if the container possesses a NULL pointer to 
its contained string, the function will return 0 and write an message to 
``stderr``.  Both underlying functions are safer to use than the ``strcmp``
function in the ``string.h`` header file; however, comparing two string 
containers is the safest option to ensure that a string literal is not 
null terminated.

.. code-block:: c

   int compare_strings(str *str_one, str* || char* str_two);

Parameters 
----------

- :c:`str_one`: A string container of type ``str``
- :c:`str_two`: A string container of type ``str`` or a string literal

Returns 
-------

- :c:`cmp`: 0 if strings are equal, < 0 if ``str_one`` is greater than ``str_two``, > 0 otherwise.

Example 1
---------
This example shows the comparison between a string container and a string 
literal.  This option is safer than the ``strcmp`` function in ``string.h``,
however, it still runs the risk that the string literal is not null terminated,
which could lead to an incorrect result.

.. code-block:: c 

   #define "print.h"
   #define "str.h"

   int main() {
       str *one = init_string("Hello");
       int val = compare_strings(one, "Helloo");
       print(val);
       free_string(one);
   return 0;
   }

.. code-block:: bash 

   >> -1

Example 2 
---------
This example shows the comparison between two string containers which is the 
safest opton for comparing two strings.

.. code-block:: c 

   #define "print.h"
   #define "str.h"

   int main() {
       str *one = init_string("Hello");
       str *two = init_string("Hello");
       int val = compare_strings(one, two);
       print(val);
       free_string(one);
       free_string(two);
   return 0;
   }

.. code-block:: bash 

   >> 0

Underlying Functions 
--------------------
The ``compare_strings`` macro uses the ``_Generic`` operator to select 
from one of the two following functions that can be used in its place. 

.. code-block:: c 

   int compare_strings_lit(str *str_struct, char *string);
   int compare_strings_str(str *str_struct_one, str *str_struct_two);

Find Char 
=========
The ``first_char`` and ``last_char`` functions will find the first or last 
occurance of a ``char`` value between two pointers.to find a char. The functions
will also check to ensure that the ``min_ptr`` is smaller than the ``max_ptr``
and return a NULL value with a ``stderr`` message if it is not.

.. code-block:: c 

   char* first_char(char a, char* min_ptr, char* max_ptr);
   char* last_char(char a, char* min_ptr, char* max_ptr);

Parameters 
----------

- :c:`a` The char being searched for 
- :c:`min_ptr`: A pointer to the minimum position within a string being searched 
- :c:`max_ptr`: A pointer to the maximum position within a string being searched

Returns 
-------

- :c:`ptr`: A pointer to the character `a` or a NULL value.

Example 1
---------
An example where the function is used to earch a string literal, and a string 
container, beginning to end to find an instance of a character.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       char* one = "Hello this is a string function with another string in it";
       str* two gbc_str = init_string(one);
       char* ptr1 = first_char("string", one, one + strlen(one));
       char* ptr2 = last_char("string"), get_string(two), get_string(two) + string_length(two));
       print(ptr1);
       print(ptr2);
       return 0;
   }

.. code-block:: bash 

   >> string function with another string in it 
   >> string in it

Example 2 
---------
An example where the ``min_ptr`` skips the first few characters of a string.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       char* a = "ababcdefg";
       char* b = first_char('b', a + 2, a + strlen(a));
       print(b);
       return 0;
   }

.. code-block:: bash 

   >> bcdefg

Example 3
---------
User passes a value of ``min_ptr`` that is greater than ``max_ptr``

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       char* one = "Hello this is a string function with another string in it";
       char* ptr1 = first_char("string", one + strlen(one), one);
       print(ptr1);
       return 0;
   }

.. code-block:: bash 

   >> min_ptr is not smaller than max_ptr in first_char 
   >> NULL

Find Strings 
============
TBD

Pop Char 
========
The ``pop_string_char`` macro selects from one of two functions to pop data 
from a string container.  The data is returned to the user and errors such as 
null pointers or an out of bounds selection return a null terminator ``\0``
and print a message to ``stderr``.

.. code-block:: c 

   char pop_string_char(str *str_struct, size_t index);

Parameters 
----------

- :c:`str_struct`: A string container of type ``str``
- :c:`index`: The index where data will be popped.  This variable is defaulted to the length of the string.

Returns 
-------

- :c:`char_val`: The ``char`` value popped from the string.

Example 1
---------
Pop data using the default value for index.

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       str *one = init_string("Goodbye");
       char val = pop_string_char(one);
       print(val);
       print(one);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> e
   >> Goodby 

Example 2 
---------
Pop data from a specific index.  Note, this operation will consume more execution
time than popping from the last index.

.. code-block:: c

   #include "print.h"
   #include "str.h"

   int main() {
       str *one = init_string("Goodbye");
       char val = pop_string_char(one, 3);
       print(val);
       print(one);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> b
   >> Goodye

Underlying Functions 
--------------------
The ``pop_string_char`` macro selects from one of two functions to allow the 
appearance of overloading.  The functions can be used in place of the macro and 
they are shown below.

.. code-block:: c

   char pop_str_char(str *str_struct);
   char pop_str_index(str *str_struct, size_t index);

Pop String Token
================
The ``pop_string_token`` macro wraps two functions that allow a user to pop 
all data from a string to the right of the right most token.  If the function 
recieved NULL pointers for the ``str`` struct, or the struct data, it will 
return a NULL pointer, and write a message to ``stderr``.  The macro also 
allows a user to return the string as a container that must be manually free 
or as a string that will be automatically collected by a garbage collector and 
freed.

.. code-block:: c 

   str* pop_string_token(str *str_struct, char token, bool gbc);

Parameters
----------

- :c:`str_struct`: A string container of type ``str``
- :c:`token`: A token that divides data to be popped 
- :c:`gbc`: true of returned string is to be garbage collected, false otherwise.  Variable is defaulted to false.

Returns 
-------

- :c:`str_struct`: A string container of type struct 

Example 1
---------
Return a string that must be manually freed 

.. code-block:: c

   #define "print.h"
   #define "str.h"

   int main() {
       str *one = init_string("2023/10/24");
       str *two = pop_string_token(one, '/');
       // The same as str *two = pop_string_token(one, '/', false);
       print(one);
       print(two);

       free_string(one);
       free_string(two);
       return 0;
   }

.. code-block:: bash 

   >> 2023/10 
   >> 24 

Example 2
---------
In this example we will create to strings that are garbage collected and 
do not need to be manually freed.

.. code-block:: c 


   #define "print.h"
   #define "str.h"

   int main() {
       str *one = init_string_gbc("2023/10/24");
       str *two gbc_str = pop_string_token(one, '/');
       print(one);
       print(two);
       return 0;
   }

.. code-block:: bash 

   >> 2023/10 
   >> 24

Underlying Functions 
--------------------
The ``pop_string_token`` macro wraps two functions that can be used in place of
the macro.  The functions are shown below.

.. code-block:: c 

   str* string_pop_token_wogbc(str *str_struct, char token);
   str* string_pop_token_wgbc(str *str_struct, char token, bool gdb);

Test Pointer 
============
In order to enable many functions such as an iterator, it is necessary 
to pass ``char`` pointers to a function.  It is necessary to check a pointer 
before it is passed to a function to ensure that it exists within the bounds 
of a string container or a string literal.  The functions 
``ptr_in_str_container`` and ``ptr_in_literal`` can be used to check if a pointer 
exists within the correct bounds.

.. code-block:: c 

   bool ptr_in_str_container(str* str_struct, char* ptr);
   bool ptr_in_literal(char* ptr, char* min_ptr, char* max_ptr);

Parameters 
----------

- :c:`str_struct`: A string container of type ``str``.
- :c:`ptr`: The ``char`` pointer to be checked
- :c:`min_ptr`: A pointer that represents the minimum bounds.
- :c:`max_pt`: A pointer that represents the maximum bounds.

Returns 
-------

- :c:`status`: true if ``ptr`` is in bounds, false otherwise.

Example 1
---------
Example to check if a pointer is in the bounds of a literal before using 
the pointer.  This example will print a string one character at a time.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       char* one = "Hello Again!";
       char* begin = one + 1;
       char* end = one + strlen(one);
       if(pt_int_literal(begin, one, end)) {
           for (char* i =  begin; i != end; i++) {
               printf("%c", *i);
           }
        }
       printf("%s", one);
       printf("\n");
       return 0;
   }

.. code-block:: bash 

   >> Hello Again!

Example 2
---------
Example to check if a pointer is in the bounds of a string container 
using the pointer.  This example will print a string one character at 
at a time.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       char* one = init_string("Hello Again!");
       char* begin = one->data + 1;
       char* end = one + string_length(one);
       if(pt_int_str_container(one, begin)) {
           for (char* i =  begin; i != end; i++) {
               printf("%c", *i);
           }
        }
       printf("%s", one);
       printf("\n");
       return 0;
   }

.. code-block:: bash 

   >> Hello Again!


Iterator
========
A string literal is in essence a statically allocated ``char`` array with a null 
terminator, and the data stored in a ``str`` container is a dynamically 
allocated ``char`` vector with a null terminator.  In both instances, a 
developer can iterate over the contiguous indices to extract data from each 
indice.  However, some data structures such as Binary Trees and Linked Lists 
do not use contiguous memory.  An Iterator is a method that allows a developer 
to use a common method to iterate through data structures with or without 
continguous memory allocation to ensure commonality in how data is extracted 
and processed.  In essence an iterator is a for or while loop that iterates
through pointer references instead of indices.

While a developer can use a for loop to iterate through 
the data in a ``str`` container, it is highly encoruaged to use the 
iterator method supplied in this section, which have a standard interface 
for applications to other data structures.

The ``str`` iterator relies on the use of the ``str_iterator`` struct which 
contains pointers to several different functions.  The function namespace 
within this struct is identical to iterators for other data structures in this 
library.  The ``str_iterator`` is shown below.

.. code-block:: c 

   typedef struct {
       char* (*begin) (str* s);  // Linked to _str_begin
       char* (*end) (str* s);  // Linked to _str_end
       void (*next) (char** current);  // Linked to _str_next
       void (*prev) (char** current);  // Linked to _str_prev
       char (*get) (char** current);  // Linked to __str_get
   } str_iterator;

Attributes 
----------

- :c:`begin`: A pointer to a function that will return a pointer to the first psuedo-index in the data structure. 
- :c:`end`: A pointer to a function that will return a pointer to the last pseudo-index in the data structure.
- :c:`next`: A pointer to a function that updates the address passed to it to the address of the next pseudo-index.
- :c:`prev`: A pointer to a function that updates the address passed to it to the address of the previous pseudo-index.
- :c:`get`: A pointer to a function that will return that ``char`` value contained at a pseudo-index.

This struct is initialized in the ``init_str_iterator`` function which returns 
a copy of an initialized ``str_iterator`` struct with the function pointers 
linked to the appropriate functions in the ``str.c`` file.  The function 
which are shown in the comments above, are static functions that are not 
available outside of the ``str.c`` file.

Example 1
---------
This example shows how to use the iterator with a for loop to iterate forwards 
through data in a ``str`` container and Capitalize the values within the 
string.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       str *one = init_string("This is a Long String");
       str_iterator it = init_str_iterator();
       char* begin = it.begin(one);
       char* end = it.end(one);
       char a;
       for (char* i =  begin; i != end; it.next(&i)) {
           a = it.get(&i);
           if (a >= 'a' && a <= 'z') *i -= 32;
       }
       print(one);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> THIS IS A LONG STRING

Example 2
---------
In this example we use an while loop to iterate through the desired
data.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main () {
       str *one = init_string("This is a Long String");
       str_iterator it = init_str_iterator();
       char* begin = it.begin(one);
       char* end = it.end(one);
       char a; 
       while(begin != end) {
           a = it.get(&begin);
           if (a >= 'a' && a <= 'z') *begin -= 32;
           it.next(&begin);
       }
       print(one);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> THIS IS A LONG STRING

Decorate Iterator 
=================
The ``dec_str_iter`` function utilizes the principles of an iterator and a 
istrategy pattern to decouple the logic of iteration from the problem being
solved.  The can decorate the ``dec_str_iter`` function with another function 
of type ``void func(char*)`` to modify char variables in memory within the 
``str`` container.

.. code-block:: c 

   void dec_str_iter(char* begin, char* end,
                     str_iter_dir direction, str_decorator decorator);

Parameters
----------

- :c:`begin`: A char pointer to a position in the data within ``str_struct``.
- :c:`end`: A char pointer to a position in the data within ``str_struct``.
- :c:`direction`: An enum of type ``str_iter_dir`` which can be ``FORWARD`` or ``REVERSE``.
- :c:`decorator`: A function of type ``str_decorator`` which is an alias for ``void func(char*)``.

Example 1
---------
An example where we iterate forward through a string to transform some of
the characters into capital characters.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   void uppercase_char(char* a) {
       if (*a >= 'a' && *a <= 'z') {
           *a -= 32;
       }
   }

   int main() {
       str *one = init_string("This is a Long String");
       str_iterator it = init_str_iterator();
       char* begin = it.begin(one);
       char* end = it.end(one); 
       dec_str_iter(begin + 3, end, FORWARD, uppercase_char);
       print(one);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> ThiS IS A LONG STRING

Uppercase 
=========
The ``to_uppercase`` function will convert an entire string to uppercase 
characters.

.. code-block:: c 

   void to_uppercase(str *s);

Parameters 
----------

- :c:`s` A string container of type ``str``

Example 
-------
Convert a string to uppercase in memory.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       str* one = init_string("this will be uppercase");
       to_uppercase(one);
       print(one);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> THIS WILL BE UPPERCASE 

Lowercase 
=========
The ``to_lowercase`` function will convert an entire string to lowercase 
characters.

.. code-block:: c 

   void to_lowercase(str *s);

Parameters 
----------

- :c:`s` A string container of type ``str``

Example 
-------
Convert a string to lowercase in memory.

.. code-block:: c 

   #include "print.h"
   #include "str.h"

   int main() {
       str* one = init_string("THIS WILL BE LOWERCASE");
       to_lowercase(one);
       print(one);
       free_string(one);
       return 0;
   }

.. code-block:: bash 

   >> this will be lowercase

Delete Substring 
================
To be filled in 

Replace Substring 
=================
To be filled in

