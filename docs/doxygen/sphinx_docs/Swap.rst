****
Swap 
****
The ``swap.h`` header file contains functions that allow a user to swap 
data of similar types in memory.  This header file can be used to swap data 
two variables of type ``char``, ``unsigned char``, ``short int``, ``insigned short int``,
``int``, ``unisnged int``, ``long int``, ``unsigned long int``, ``long long int``,
``unsigned long long int``, ``float``, ``double``, ``long double``, ``bool``
and the ``str`` derived data type described in the :ref:`String <string_struct>`
Section of this documentation.  The following demonstrate the functions that 
can be used from this header file.

.. code-block:: c 

   void swap_char(char *a, char *b);
   void swap_uchar(unsigned char *a, unsigned char *b);
   void swap_short(short int *a, short int *b);
   void swap_ushort(unsigned short int *a, unsigned short int *b);
   void swap_int(int *a, int *b);
   void swap_uint(unsigned int *a, unsigned int *b);
   void swap_long(long int *a, long int *b);
   void swap_ulong(unsigned long int *a, unsigned long int *b);
   void swap_llong(long long int *a, long long int *b);
   void swap_ullong(unsigned long long int *a, unsigned long long int *b);
   void swap_float(float *a, float *b);
   void swap_double(double *a, double *b);
   void swap_ldouble(long double *a, long double *b);
   void swap_bool(bool *a, bool *b);
   void swap_string(str *a, str *b);

Parameters
----------

- ``a``: The first parameter to be swapped in memory.
- ``b``: The second parameter to be swapped in memory.

Example
-------
An example for swapping two string data types in memory.

.. code-block:: c

   #include "print.h"
   #include "swap.h"
   #include "string.h"

   int main() {
       str* one = init_string("Hello");
       str* two = init_string("Goodbye");
       print("String a before: ", one);
       print("String b before: ", two);
       swap_string(one, two);
       print("String a after: ", one);
       print("String b after: ", two);
       free_string(one);
       free_string(two);
   }

.. code-block:: bash 

   >> String a before: Hello
   >> String b before: Goodbye 
   >> String a after: Goodbye 
   >> String b after: Hello

SWAP MACRO 
==========
For ease of use, this library has implemented a ``SWAP`` macro that works with 
all data types, but the ``str`` type.  The following is an example for how to 
use the ``SWAP`` macro for float data.  The ``SWAP`` macro uses the 
``_Generic`` keyword to intelligently select the functin to use, based on 
the data type of the variables passed to the macro.

.. code-block:: c

   #include "print.h"
   #include "swap.h"

   int main() {
       float one = 4.3 
       float two = 21.7
       print("a before: ", one);
       print("b before: ", two);
       SWAP(one, two);
       print("a after: ", one);
       print("b after: ", two);
   }

.. code-block:: bash 

   >> a before: 4.3
   >> b before: 21.7 
   >> a after: 21.7 
   >> b after: 4.3

