Helper functions for Asymptote FFI

This library provides convenience functions on top of Asymptote's
foreign function interface. The functions provided here utilize C++ features
in order to simplify writing of C++ asymptote modules. For example, this library
provides:

- Constructs to build Asymptote types for registering functions
- RAII-based constructs for registering threads with Asymptote's garbage collection system
- Helper functions to register new threads that automatically registers that thread
- Template based functions to simply item retrieval

We are planning on additional features.

*** Building ***

Firstly, this library requires a C++ compiler that supports C++20 or beyond.
As of writing this (July 22, 2026), only MSVC has full C++20 support on Windows.
On POSIX systems, gcc-16 has almost complete C++20 support. Currently, this g++-16
is able to compile this library without issues.

The recommended way to build is to use CMake. For this, build asyffi-helper-lib.
However, we also support Makefiles. To do this, ensure autotools is installed on the system and run

autoconf
./configure <options>

Therefore, on Linux or mac, we recommend using g++-16 when possible, for example, if one is
using Makefiles, configure with

CXX=g++-16 ./configure <options>

*** License ***

See LICENSE.txt. Note that while Asymptote is licensed under LGPL/GPL, the asyffi.h file
is under Apache license. Therefore, we are able to license this library under Apache

*** Author & Acknowledgements ***

This library is written by Jamie Selina Lindner <jamievlin@outlook.com>
Additional credits is given to John C. Bowman and Charles Stats III for helping with
the development of this library.
