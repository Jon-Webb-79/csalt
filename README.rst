*************
CSalt Library 
*************

Working with real and integer values in C presents several challenges in modern software development:

* Manual memory management for arrays can lead to memory leaks and buffer overflows
* Static arrays require compile-time size decisions that may not suit runtime needs
* No built-in support for dynamic resizing of collections
* Lack of associative containers for values requires complex custom implementations
* Error handling for operations often requires boilerplate code

The ``csalt`` library addresses these challenges by providing four primary containers 
for all basic c data types:

* A set of custom allocator types to support ease of memory management with a reduction in overhead calls to ``malloc``, ``calloc``, ``realloc`` and ``free``
* A dynamically allocated vector that automatically manages memory and resizing
* A safe wrapper for static arrays with bounds checking
* A dictionary implementation for mapping strings to primitive data types 
* A dictionary implementation for mapping strings to arrays of primitive data types
* A matrix implementation for dense and sparse storage methods
* Build time config flags ``STATIC_ONLY`` and ``NO_FUNCTION_MACROS`` to support MISRA compliant builds

When to Use This Library
########################

All of the functionality in this library can be accessed from the ``c_float.h``,
``c_double.h``, ``c_ldouble.h``, ``c_int.h``, and ``c_string.h`` header files.

This library is particularly useful when:

* The use of intuitive and safe data structures is required
* Requiring safe bounds-checked access to arrays, matrices, and dictionaries
* Managing collections of named values
* Performing numerical computations with dynamic and static data sets

The library's encapsulated design prevents common array manipulation 
errors while maintaining the performance characteristics expected in C programs.

This project encapsulates its functionality, which is wrapped in the header 
guard ``#ifdef __cplusplus`` to allow compilation with both C and C++. 

Performance Acceleration
------------------------

Dynamically allocated vectors in ``csalt`` are optimized with hardware acceleration
using SIMD (Single Instruction, Multiple Data) instruction sets where available.
The library automatically dispatches to the best implementation supported by the
target platform, including:

* **AVX-512**, **AVX2**, and **AVX** on modern x86 processors
* **SSE4.1**, **SSE3**, and **SSE2** on legacy x86 processors
* **SVE2** and **SVE** on ARMv9/ARMv8-A processors
* **NEON** on ARMv8-A (AArch64) processors

Implementation Details
######################

The library provides these container types:

Allocators (``allocator_vtable_v``)
-----------------------------------

* Fast linear bump-pointer allocation (``arena_t``)
* Fixed-size block allocator for small, uniform objects (``pool_t``)
* Variable-sized returnable free list allocator (``free_list_t``)
* Power-of-two buddy allocator (``buddy_t``) 
* High-throughput slab allocator for fixed-size objects (``slab_t``)

String (``string_t``)
---------------------
* A dynamically allocated string implementation 
* Automatically manages memory during string operations 
* Provides functions for standard string operations 
* Provides an iterator for access methods

Array (``array_t``)
-------------------
* Utilizes allocators from the ``c_allocator.h`` file or user developed custom 
  allocators to manage memory.
* Automatically manages memory allocation and resizing
* Maintains size and capacity information
* Provides safe element access with bounds checking
* Supports efficient append and insert operations
* Functions to support advanced options such as sort, binary search, min, max, etc.

Dictionary (``dict_t``)
-----------------------
* Utilizes allocators from the ``c_allocator.h`` file or user developed custom
  allocators to manage memory.
* Hash table using MurmurHash3-inspired byte-span hashing over arbitrary key
  lengths, with power-of-two bucket counts and separate chaining for collision
  resolution.
* Supports any fixed-size value type via a generic ``data_size`` parameter;
  typed wrappers (``uint8_dict_t``, ``int32_dict_t``, ``float_dict_t``, etc.)
  fix the value type at initialisation and enforce it through the API.
* Keys are null-terminated C-strings copied into internal allocator-managed
  storage on every insert; the caller may free or reuse key memory immediately.
* Both plain (null-terminated) and ``_n`` (explicit-length) variants are
  provided for every key-taking function.
* Automatically resizes when the load factor exceeds 0.75, provided the dict
  was initialised with ``growth = true``; fixed-capacity mode is also
  supported.
* Supports insert, pop, update, value retrieval by copy, direct pointer access,
  membership test, clear, deep copy, merge (with or without overwrite), and
  bucket-order iteration via a typed callback.

Matrix
------
* To be developed

Work Forward
============
The following are areas for future improvement in the code base 

* Test on a wider array of platforms and compilers to exercise all SIMD instruction sets 
* Increase robustness of edge cases for unit tests 
* Refactor string_t type to handle non ASCI data 

Contributing
############
Pull requests are welcome.  For major changes, please open an issue first to discuss
EADME.rst

as well as relevant doc-string and sphinx updates.

License
#######
This software is developed under a simple MIT license.

Requirements
############
This library is developed and tested on Macintosh and Arch Linux Operating
Systems.  It is developed with ``gcc 14.2.1`` and ``clang 16.0.6`` compilers. In
addition, this code base requires the use of ``CMake 3.31.3``, ``cmocka``, and 
``valgrind``. This code is compiled and written with the C17 standard; however, this 
should work with any compiler using C11 or later versions.

Installation and Build Guide
############################

Requirements
------------
- Git
- CMake (version 3.31.3 or later)
- C compiler (GCC, Clang, or MSVC)

For unit testing:
- Linux: valgrind (optional, for memory leak checking)
- All platforms: cmocka testing framework

Getting the Code
----------------
Clone the repository:

.. code-block:: bash

  git clone https://github.com/Jon-Webb-79/csalt.git
  cd csalt

Debug Build (with tests)
------------------------

Use the appropriate script for your platform:

**Linux/macOS (bash)**:

.. code-block:: bash

  cd scripts/bash
  ./debug.sh

**Linux/macOS (zsh)**:

.. code-block:: bash

  cd scripts/zsh
  ./debug.zsh

**Windows**:

.. code-block:: batch

  cd scripts\Windows
  debug.bat

Run tests:

**Linux (with valgrind)**:

.. code-block:: bash

  cd build/debug
  valgrind ./unit_tests

**macOS/Windows**:

.. code-block:: bash

  cd build/debug
  ./unit_tests

Static Library Build
--------------------
Creates a static library without tests:

**Linux/macOS (bash)**:

.. code-block:: bash

  cd scripts/bash
  ./static.sh

**Linux/macOS (zsh)**:

.. code-block:: bash

  cd scripts/zsh
  ./static.zsh

**Windows**:

.. code-block:: batch

  cd scripts\Windows
  static.bat

System Installation
-------------------
Installs library files to system directories for use in other projects:

**Linux/macOS (requires sudo)**:

.. code-block:: bash

  cd scripts/bash  # or scripts/zsh
  sudo ./install.sh  # or sudo ./install.zsh

**Windows (requires Administrator)**:

1. Right-click ``scripts\Windows\install.bat``
2. Select "Run as Administrator"

Usage in Projects
-----------------
After installation, you can use the library in three ways:

1. **As System Library**:

  After installation, include in your C files:

  .. code-block:: c

     #include <c_float.h> // Or whichever header file you wish to use

2. **As Static Library**:

  Link against the static library created in the build/static directory.

3. **Direct Integration**:

  Copy any files you wish to your project and compile directly.  Ensure that you have the ``.h`` and ``.c`` files.  Each file requires that the ``c_string.h`` and ``c_string.c`` file also be present.

Troubleshooting
---------------
- If tests fail, ensure all dependencies are properly installed
- For Windows builds, ensure you're using an appropriate Visual Studio version
- For installation issues, verify you have appropriate system permissions

Contribute to Code Base 
-----------------------
#. Establish a pull request with the git repository owner.

#. Once the package has been downloade, you will also need to install
   Python3.10 or later version to support documentation with Sphinx.

EADME.rst


#. Create a Python virtual environment with the following command.

   .. code-block:: bash 

      python -m venv .venv 

#. Activate the virtual environment with the following command.

.. table:: Activation Commands for Virtual Environments

   +----------------------+------------------+-------------------------------------------+
   | Platform             | Shell            | Command to activate virtual environment   |
   +======================+==================+===========================================+
   | POSIX                | bash/zsh         | ``$ source <venv>/bin/activate``          |
   +                      +------------------+-------------------------------------------+
   |                      | fish             | ``$ source <venv>/bin/activate.fish``     |
   +                      +------------------+-------------------------------------------+
   |                      | csh/tcsh         | ``$ source <venv>/bin/activate.csh``      |
   +                      +------------------+-------------------------------------------+
   |                      | Powershell       | ``$ <venv>/bin/Activate.ps1``             |
   +----------------------+------------------+-------------------------------------------+
   | Windows              | cmd.exe          | ``C:\> <venv>\\Scripts\\activate.bat``    |
   +                      +------------------+-------------------------------------------+
   |                      | PowerShell       | ``PS C:\\> <venv>\\Scripts\\Activate.ps1``|
   +----------------------+------------------+-------------------------------------------+

#. Install packages to virtual environments from ``requirements.txt`` file

   .. code-block:: bash 

      pip install -r requirements.txt

#. At this point you can build the files in the same way described in the 
   previous section and contribute to documentation.


Documentation 
=============
This code in this repository is further documented at the 
`Read The Docs <https://csalt-lib.readthedocs.io/en/latest/>`_
website
