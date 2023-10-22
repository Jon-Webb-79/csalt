************
Project Name
************
For some time I have been interested in creating a C library that provides users
with common utility functions that enable a range of data structures, 
more robust random number generators, and common format file readers.  In
addition to the best extent sensible for C, I am also interested in creating 
interfaces that are more common to objects.  When describing my interface 
idea to someone who was very seasoned in the C language, he described the concept
as being so alien to the C language, that it was like puring salt into the wounds
of old C programmers.  Hen the name C Salt (i.e.csalt).

Contributing
############
Pull requests are welcome.  For major changes, please open an issue first to discuss
what you would like to change.  Please make sure to include and update tests
as well as relevant doc-string and sphinx updates.

License
#######
This software is developed under a simple MIT license.

Requirements
############
This library is developed and tested on Macintosh and Arch Linux Operating
Systems.  It is developed with ``gcc 13.2.1`` and ``clang 16.0.6`` compilers. In
addition, this code base requires the use of ``CMake 3.27.7``, ``cmocka``, and 
``valgrind``.


Installation
############
This project is covered under a basic MIT license which allows anyone to use 
this code base or to contribute to it with the express permsission of the 
git project owner.

Use Code Base 
-------------
In order to download this repository from github, follow these instructions

#. Ensure you have ``.git`` installed on your computer

#. Ensure you have ``cmake`` installed on your computer.  This code-base requires 
   cmake version 3.27.7 or later versions.

#. Download this repository to your preferred directory with the following command;

   .. code-block:: bash 

      git clone https://github.com/Jon-Webb-79/csalt.git csalt 

#. Navigate to either the bash or zshell scripts directory depending on your 
   environment with one of the following commands.

   .. code-block:: bash 

      cd csalt/scripts/bash 
      cd csalt/scripts/zsh 

#. Build the code base with the following command.

   .. code-block:: bash 

      # If using bash
      bash build.sh  
      # If using zsh 
      zsh build.zsh

#. Compile the code base with the following command.

   .. code-block:: bash 

      # If using bash 
      bash compile.sh 
      # If using zsh 
      zsh compile.zsh

#. Navigate to ``csalt/cstal/build`` to run unit tests 

   .. code-block:: bash 

      valgrind ./unit_tests 
      
#. If all unit tests do not pass with no memory leaks, you may need to contact 
   the administrator of this git repository.  If they do pass, then you are set 
   to transform this code-base into a static or dynamic library, or just 
   copy the ``.c`` and ``.h`` files to your project.

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
