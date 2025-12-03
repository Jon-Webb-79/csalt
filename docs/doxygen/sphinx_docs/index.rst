.. Core Utilities documentation master file, created by
   sphinx-quickstart
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to CSalt documentation!
===============================
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
* A dictionary implementation for dense and sparse storage methods

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

When to Use This Library
########################

All of the functionality in this library can be accessed from the ``c_float.h``,
``c_double.h``, ``c_ldouble.h``, ``c_int.h``, ``c_string.h``, ``c_bool.h``,
and ``c_binary.h`` files.  Other data types will be included to this library 
at a later date.

This library is particularly useful when:

* Working with arrays of unknown or varying size
* Requiring safe bounds-checked access to arrays
* Managing collections of named values
* Performing numerical computations with dynamic data sets
* Implementing algorithms that require flexible storage

The library's encapsulated design prevents common array manipulation 
errors while maintaining the performance characteristics expected in C programs.

This project encapsulates its functionality, which is wrapped in the header 
guard ``#ifdef __cplusplus`` to allow compilation with both C and C++. 

Implementation Details
######################

The library provides these main container types:

Allocators
----------

* Fast linear bump-pointer allocation (``arena_t``)
* Fixed-size block allocator for small, uniform objects (``pool_t``)
* Variable-sized returnable free list allocator (``free_list_t``)
* Power-of-two buddy allocator (``buddy_t``) 
* High-throughput slab allocator for fixed-size objects (``slab_t``)

String  (``string_t``)
----------------------
* A dynamically allocated string implementation 
* Automatically manages memory during string operations 
* Provides functions for standard string operations 
* Provides an iterator for access methods

Dynamic Vector
--------------
* Automatically manages memory allocation and resizing
* Maintains size and capacity information
* Provides safe element access with bounds checking
* Supports efficient append and insert operations

Static Array Wrapper
--------------------
* Provides bounds-checked access to fixed-size arrays
* Prevents buffer overflows through runtime checks
* Maintains actual element count for partially filled arrays
* Offers safe iteration and access methods

Dictionary
----------
* Maps string keys to values
* Handles memory management for both keys and values
* Provides efficient key lookup
* Supports dynamic addition and removal of entries
* a dictionary for management of data primitives and a dictionary for arrays of primitives

Matrix
------

**NOTE:** The matrix implementation in this package is being updated for a more intuitive interface.

The matrix module provides adaptive storage for two-dimensional floating-point matrices, with
automatic conversion between dense and sparse formats based on size and sparsity characteristics.

Supported formats:

* **Dense Matrix** — row-major, with optional initialization tracking for missing values
* **Coordinate List (COO)** — stores only non-zero entries with explicit row and column indices
* **Compressed Sparse Row (CSR)** — optimized for fast row-based access and memory efficiency
* **Compressed Sparse Column (CSC)** - optimized for dast column-based access and memory efficiency

Typical use cases include large engineering calculations (e.g., PDEs, FEM solvers) where the 
storage and access pattern of matrices must adapt to the structure of the data.

Work Forward 
------------
The following are areas for future improvement in the code base 

* Update all documentation to link to Doxygen doc strings (IN PROGRESS)
* Incorporate ErrorCode enums and related error handling into all data structures and functions (IN PROGRESS)
* Refactor entire Matrix code base
* Test on a wider array of platforms and compilers to exercise all SIMD instruction sets 
* Increase robustness of edge cases for unit tests 
* Refactor string_t type to handle non ASCI data 

.. toctree::
   :maxdepth: 1
   :caption: Modules:

    c_error <Error>
    c_allocator <Allocator>
    c_string <Strings>
    c_ldouble <LDouble>
    c_double <Double>
    c_float <Float>
    c_int <Int>
    
Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

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

#. Navigate to the ``csalt/docs/doxygen`` directory.

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
