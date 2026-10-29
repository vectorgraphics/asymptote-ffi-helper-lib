// minimal.cc -- the minimal Asymptote FFI plugin ("hello world").
//
// Registers a single function:  int doubleIt(int)
//
// The AsyFfi::registerFunction template infers the Asymptote signature
// from the lambda's C++ signature.
#include <asyffihelper.h>

#include <cstdint>

using namespace AsyFfi;

DECLARE_REGISTER_FN;

REGISTER_FN_SIG
{
    // C++ int64_t(int64_t) -> Asymptote int(int)
    registerFunction(
        registerer, "doubleIt", [](int64_t x) -> int64_t { return 2 * x; }
    );
}
