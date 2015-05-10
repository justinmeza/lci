# lci

![Build status](https://travis-ci.org/justinmeza/lci.svg?branch=future)

lci is a LOLCODE (http://lolcode.org) interpreter written in C and is designed
to be correct, portable, fast, and precisely documented.

- Correct: Every effort has been made to test lci's conformance to the LOLCODE
  language specification. Unit tests come packaged with the lci source code.

- Portable: lci follows the widely ported ANSI C specification allowing it to
  compile on a broad range of systems.

- Fast: Much effort has gone into producing simple and efficient code whenever
  possible to the extent that the above points are not compromized.

- Precisely documented: lci uses Doxygen to generate literate code
  documentation, browsable here.

This project's homepage is at http://lolcode.org.  For help, visit
http://groups.google.com/group/lci-general.  To report a bug, go to
http://github.com/justinmeza/lci/issues.

Created and maintained by Justin J. Meza (http://justinmeza.com).

### Prerequisites

1. You must have CMake installed (http://www.cmake.org).

  - If you're using a Linux distro with package managment CMake should be in
    your repositories.

1. Python 2.7+ or Python 2.x with the argparse module installed.

### Installation:  The Easy Way on Linux or OSX

Run the script install.py. Note that

`$ ./install.py -h`

will display a list of relavent install options. For example, to install lci to
the directory `/home/foo/opt`, run:

`$ ./install.py --prefix="/home/foo/opt"`

### Installation:  The More Involved Way on Linux or OSX

1. Configure lci using CMake. This can be as simple as opening up the terminal,
   navigating to the directory containing lci and typing:

  `$ cmake .`

  You can also provide any other argument to the CMake configuration process
you'd like. To enable Memory testing turn the `PERFORM_MEM_TESTS` option on
like so:

  `$ cmake -DPERFORM_MEM_TESTS:BOOL=ON .`

  You can also use the `ccmake` command or the CMake GUI if you prefer.  See
the cmake documentation for more details.

1. Build the project:

  `$ make`

1. Install

  `$ make install`

1. (Optional) Build documentation:

  `$ make docs`

1. (Optional) Run tests:

  `$ ctest`

### License

Copyright (C) 2010-2015 Justin J. Meza

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.
