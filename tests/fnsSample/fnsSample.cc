

// fnsSample.cc -- demonstrates the type-inferred registerFunction and the
// asy::call C++-into-Asymptote helper.
//
// Build with:
//   asyffi_add_plugin(fnsSample fnsSample.cc)
//
// Then from Asymptote:
//   import "/abs/path/libfnsSample" as m;
//   real s = m.addTwo(3, 4);     // 7  (ergonomic registration)
//   real sp = m.sqrtPlus(9);     // 12 (C++ calls back into Asy sqrt)
//
// Both functions use the type-inferred AsyFfi::registerFunction (no
// hand-written metadata). addTwo is a plain lambda; sqrtPlus takes the
// injected AsyFfi::Context so it can call back into Asymptote's builtin
// sqrt via AsyFfi::call.

#include <asyffihelper.h>

#include <cstdint>

using namespace AsyFfi;

DECLARE_REGISTER_FN;

REGISTER_FN_SIG
{
    // The lambda's C++ signature double(double, double) is automatically
    // mapped to Asymptote's real(real, real). No FnArgMetadata, no
    // FunctionTypeMetadata -- the template does it all.
    registerFunction(
        registerer, "addTwo", [](double a, double b) -> double { return a + b; }
    );

    // A leading AsyFfi::Context parameter is the injected host context (not an
    // Asymptote argument) and carries both the host context and the stack
    // context, so this type-inferred lambda can call back into Asymptote's
    // builtin sqrt without any hand-built metadata. real sqrtPlus(real) ==
    // sqrt(x) + x, so the driver verifies the result is 12 (sqrt(9) + 9) for
    // x=9.
    registerFunction(
        registerer, "sqrtPlus",
        [](Context ctx, double x) -> double {
            return AsyFfi::call<double>(ctx, nullptr, "sqrt", x) + x;
        }
    );
}

