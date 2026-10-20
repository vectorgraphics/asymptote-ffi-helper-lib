
// single_fn_plugin.cc -- a copy-paste, single-file Asymptote FFI plugin template.
//
// Registers exactly one function (`doubleIt`) using the type-inferred
// AsyFfi::registerFunction API (no hand-written metadata, no manual
// unpacking).
//
// Build it with the CMake helper, from any consumer project that has the
// asyffi_helper target available:
//
//     asyffi_add_plugin(doubleit single_fn_plugin.cc)
//
// This produces libdoubleit.so (Linux) / doubleit.dll (Windows). Then from
// Asymptote:
//
//     import "/abs/path/libdoubleit" as m;
//     label("answer: " + string(m.doubleIt(21)));   // 42
//
// To place the built library on Asymptote's search path so you can simply do
// `import doubleit;`, pass ASY_PATH_DIR:
//
//     asyffi_add_plugin(doubleit single_fn_plugin.cc LIBNAME doubleit ASY_PATH_DIR <asy_dir>)

#include <asyffihelper.h>

#include <cstdint>

using namespace AsyFfi;

DECLARE_REGISTER_FN;

REGISTER_FN_SIG
{
    // int(int) -> int : doubles its argument.
    registerFunction(
        registerer, "doubleIt", [](int64_t x) -> int64_t { return 2 * x; }
    );
}

