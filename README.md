# Helper functions for Asymptote FFI

This library provides convenience functions on top of Asymptote's
foreign function interface. The functions provided here utilize C++ features
in order to simplify writing of C++ asymptote modules. For example, this library
provides:

- Constructs to build Asymptote types for registering functions
- RAII-based constructs for registering threads with Asymptote's garbage collection system
- Helper functions to register new threads that automatically registers that thread
- Template based functions to simply item retrieval

We are planning on additional features.

# Building

Firstly, this library requires a C++ compiler that supports C++20 or beyond.
As of writing this (July 22, 2026), only MSVC has full C++20 support on Windows.
On POSIX systems, gcc-16 has almost complete C++20 support. Currently, this g++-16
is able to compile this library without issues.

## Using CMake (the recommended way)

The recommended way to build is to use CMake. For this, build `asyffi_helper`.

This library requires `asyffi.h`, the Asymptote foreign function interface header file. It is not included
in this repo, though it may be obtained from this [repo](https://github.com/vectorgraphics/asymptote).

For CMake builds, specify `ASYFFI_HEADER_DOWNLOAD_URL` cache variable with either the path to `asyffi.h` on your
system (if it is on your system, for example if you are an Asymptote developer or are building it manually), or a
URL to `asyffi.h`.

For example,
`ASYFFI_HEADER_DOWNLOAD_URL=/home/.../asymptote/asyffi.h` or
`ASYFFI_HEADER_DOWNLOAD_URL=https://raw.githubusercontent.com/.../asyffi.h`.

If this variable is not given, CMake downloads it from a default URL. Be warned that this URL may be outdated.

## Using Makefiles & autoconf

However, we also support Makefiles. To do this, ensure autotools is installed on the system and run

```sh
autoconf
./configure <options>
```

Like in CMake, `asyffi.h` must be provided to the build system. There are two possible options:
1. If `asyffi.h` is in your system, configure with argument `--with-asyffi-header-location` specifying
   the path to your `asyffi.h` file, for example,
   `./configure --with-asyffi-header-location=/home/.../asymptote/asyffi.h`
2. You may choose to specify a URL to `asyffi.h`. wget needs to be installed in your system for the download
   to happen. In this case, configure with argument `--with-asyffi-header-url` with the URL of the header file,
   for example,
   `./configure --with-asyffi-header-url=https://raw.githubusercontent.com/.../asyffi.h`

If neither option is given, autoconf downloads it from a default URL. Again, be warned that this URL may be outdated.

Therefore, on Linux or mac, we recommend using g++-16 when possible, for example, if one is
using Makefiles, configure with

```sh
CXX=g++-16 ./configure <options>
```

# License

See `LICENSE.txt`. Note that while Asymptote is licensed under LGPL/GPL, the asyffi.h file
is under Apache license. Therefore, we are able to license this library under Apache

# Author & Acknowledgements

This library is written by Jamie Selina Lindner <jamievlin@outlook.com>
Additional credits is given to John C. Bowman and Charles Staats III for helping with
the development of this library.
