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

# Minimal plugin (type-inferred registration)

The simplest possible plugin is three lines of C++:

```cpp
#include <asyffihelper.h>
using namespace AsyFfi;
DECLARE_REGISTER_FN;

REGISTER_FN_SIG
{
    registerFunction(registerer, "doubleIt", [](int64_t x) { return 2 * x; });
}
```

Compile as a shared library (`-shared -fPIC`), then in Asymptote:

```asy
import "/path/to/libmyplugin" as m;
label("answer: " + string(m.doubleIt(21)));   // prints 42
```

The return and argument types are inferred from the lambda's C++ signature -- no
hand-written metadata, no manual unpacking. See `asyffihelpers/register.h` for
the full API and supported types.

### A slightly larger example

A plugin can register multiple functions of different types in a single
`REGISTER_FN_SIG` block. This example covers the most common patterns:

```cpp
#include <asyffihelper.h>
#include <numeric>
using namespace AsyFfi;
DECLARE_REGISTER_FN;

REGISTER_FN_SIG
{
    // int(int) -> int
    registerFunction(registerer, "doubleIt", [](int64_t x) -> int64_t { return 2 * x; });

    // pair(pair, pair) -> pair
    registerFunction(
        registerer, "midpoint", [](AsyFfi::pair a, AsyFfi::pair b) -> AsyFfi::pair {
            return AsyFfi::pair{(a.x + b.x) / 2.0, (a.y + b.y) / 2.0};
        }
    );

    // real(real[]) -> real
    registerFunction(
        registerer, "addAll", [](std::vector<double> xs) -> double {
            return std::accumulate(xs.begin(), xs.end(), 0.0);
        }
    );

    // void(int) -- a leading AsyFfi::Context parameter gives the lambda the
    // Asymptote host (reportWarning, creating Asy objects, ...). It is not an
    // Asymptote argument, so the inferred signature is still void(int).
    registerFunction(
        registerer, "logIt", [](Context ctx, int64_t x) -> void {
            ctx->reportWarning("logIt called");
        }
    );
}
```

In Asymptote:

```asy
import "/path/to/libmyplugin" as m;
label(string(m.doubleIt(21)));              // 42
label(string(m.midpoint((0,0), (4,6)).x));   // 2
label(string(m.addAll(new real[]{1,2,3})));   // 6
m.logIt(5);
```

### Calling Asymptote from C++ (`asy::call`)

The FFI is bidirectional. C++ can call *back* into Asymptote through
`AsyFfi::call` (see `asyffihelpers/call.h`). A type-inferred lambda that takes
the injected `AsyFfi::Context` can do this directly -- no hand-built metadata:

```cpp
#include <asyffihelper.h>
using namespace AsyFfi;
DECLARE_REGISTER_FN;

REGISTER_FN_SIG
{
    // real sqrtOf(real) == sqrt(x): calls Asymptote's builtin sqrt from C++.
    registerFunction(
        registerer, "sqrtOf", [](Context ctx, double x) -> double {
            return AsyFfi::call<double>(ctx, nullptr, "sqrt", x);
        }
    );
}
```

In Asymptote: `m.sqrtOf(25)` returns `5`.

The `module` argument to `call` is `nullptr` for top-level built-ins, or the id
of an imported module (e.g. `"gsl"`) to resolve the function from that module.

### Escape hatch: explicit metadata

The ergonomic `registerFunction` template above handles the common
`Ret(Args...)` cases. For optional/named arguments, overloads, or any case
the template cannot express, use the explicit-metadata path directly:

```cpp
Asy::FunctionTypeMetadata const meta{
    .returnType = Asy::TypeInfo{Asy::BaseTypes::Real, {}},
    .numArgs = 1,
    .argInfoPtr = args  // C array of Asy::FnArgMetadata
};
registerer->registerFunction(name, &fn, meta);
```

The same `AsyFfi::toItem` / `AsyFfi::fromItem` helpers work with this path.
See `asyffihelpers/register.h` (the `registerFunction` escape-hatch overload)
and `asyffihelpers/typesObjs.h` for the type-builder API.

# Building

Firstly, this library requires a C++ compiler that supports C++20 or beyond.
As of writing this (July 22, 2026), only MSVC has full C++20 support on Windows.
On POSIX systems, gcc-16 has almost complete C++20 support. Currently, this g++-16
is able to compile this library without issues.

## Using CMake (the recommended way)

The recommended way to build is to use CMake. Configure and build the
`asyffi_helper` static library:

```sh
cmake -S . -B build
cmake --build build -j
```

`asyffi_helper` requires `asyffi.h`, the Asymptote foreign function interface
header, which is not included in this repo (see the
[Asymptote repo](https://github.com/vectorgraphics/asymptote)). CMake locates it
through the `ASYFFI_HEADER_DOWNLOAD_URL` cache variable. If it is not given,
CMake downloads it from a default URL, which may be outdated; to use your own
copy, add it to the configure command above, either as a path to a local
`asyffi.h` (if you are an Asymptote developer or have already fetched it) or as
a URL:

```sh
-DASYFFI_HEADER_DOWNLOAD_URL=/home/.../asymptote/asyffi.h
-DASYFFI_HEADER_DOWNLOAD_URL=https://raw.githubusercontent.com/.../asyffi.h
```

Add `-DASYFFI_BUILD_DOCS=FALSE` to the configure command to skip generating the
Doxygen documentation.

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

## Build a plugin (`asyffi_add_plugin`)

Once the helper is built, use the one-function CMake helper `asyffi_add_plugin`
to build a loadable plugin. It takes care of the fiddly parts: a `SHARED`
target with `-fPIC` and C++20, the `asyffi.h` + `asyffihelpers/` include dirs
(reusing the header location the helper build already resolved -- you do **not**
re-specify `ASYFFI_HEADER_DOWNLOAD_URL`), and the correct output name
(`lib<name>.so` on Linux, `<name>.dll` on Windows).

```cmake
# In a consumer CMake project that has the asyffi_helper target available
# (e.g. via add_subdirectory of this library):
add_subdirectory(asyffi-helper-lib helper)   # provides target: asyffi_helper

include(asyffi-helper-lib/cmake/asyffi_plugin.cmake)

#   asyffi_add_plugin(<target> <src>... [LIBNAME <name>] [ASY_PATH_DIR <dir>])
asyffi_add_plugin(myplug myplug.cc)
```

A fresh, minimal consumer that just calls `asyffi_add_plugin(hello hello.cc)`
produces `libhello.so` (Linux) / `hello.dll` (Windows) that `asy` can load.

Details:
- `LIBNAME <name>` sets the output file name; a leading `lib` is stripped, so
  `LIBNAME hello` and `LIBNAME libhello` both give `libhello.so`.
- Symbol export is handled by `asyffi.h`: the generated
  `registerAsymptotePlugin` entry point carries `ASY_FFI_EXPORT`
  (`[[gnu::visibility("default")]]` on ELF / `__declspec(dllexport)` on
  Windows), so the plugin loads with no extra visibility flags.
- `ASY_PATH_DIR <dir>` adds a post-build step that copies the built
  `.so`/`.dll` into `<dir>` (an Asymptote search-path directory), so the
  author can then `import <name>;` directly instead of importing an absolute
  path.
- A ready-to-build single-file example lives in
  [`templates/single_fn_plugin.cc`](templates/single_fn_plugin.cc); the
  `template_plugin_roundtrip` ctest builds it through `asyffi_add_plugin` and
  round-trips it end to end.

On Windows the same CMake produces a `.dll`; the naming and export logic are
platform-correct. (If no Windows CI runner is available, this is documented
here rather than exercised by the Linux test suite.)

## Running the round-trip test

This repository includes an automated round-trip test that verifies a trivial FFI
plugin (`libhello.so`) can be loaded by the real `asy` binary and that its return
value is correct.

Prerequisites:
- The Asymptote core binary at `/home/ai/asymptote/asy` (built from the `spike-ffi` branch).
- CMake 3.31 and CTest.

Build the library first (see "Using CMake" above), then run the test:

```sh
ctest --test-dir build -R hello_roundtrip --output-on-failure
```

Expected output on success:

```
1/1 Test #1: hello_roundtrip ..................   Passed
100% tests passed out of 1
```

The test is named `hello_roundtrip` in ctest. If the `asy` binary is missing,
the test fails loudly with a clear error message.

# License

See `LICENSE.txt`. Note that while Asymptote is licensed under LGPL/GPL, the asyffi.h file
is under Apache license. Therefore, we are able to license this library under Apache

# Author & Acknowledgements

This library is written by Jamie Selina Lindner <jamievlin@outlook.com>
Additional credit is given to John C. Bowman and Charles Staats III for helping with
the development of this library.
