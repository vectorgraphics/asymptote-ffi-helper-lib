

/**
 * Round-trip plugin for AsyFfi::call (C++ -> Asymptote).
 *
 * Three external functions are registered. Each calls back into Asymptote
 * through the {@link AsyFfi::call} helper and returns the result to the
 * .asy driver, which verifies the value and aborts() on a mismatch:
 *
 *   - sqrtOf(x)    = call<double>(context, stack, nullptr, "sqrt", x)
 *                    a top-level C++ built-in (module == nullptr).
 *   - shiftOf(x,y) = call<transform>(context, stack, nullptr, "shift", pair{x,y})
 *                    a top-level built-in returning a *structural* type.
 *   - hypotOf(x,y) = call<double>(context, stack, "gsl", "hypot", x, y)
 *                    a function resolved from an imported module
 *                    (module != nullptr), proving the module path.
 *
 * The driver also does `import gsl;` so the module path is exercised the same
 * way a user would reach it. The gsl module ships as a preloaded C++ built-in
 * module, so its functions are resolvable through IAsyStackContext::getBuiltin.
 */

#include <asyffihelper.h>

#include <cstdint>

DECLARE_REGISTER_FN;

namespace
{

// Top-level built-in: real sqrt(real).
ASY_FOREIGN_FUNC_SIG(sqrtOf)
{
    double const x = AsyFfi::fromItem<double>(context, args->getNumberedArg(0));
    double const r = AsyFfi::call<double>(context, stackContext, nullptr, "sqrt", x);
    AsyFfi::toItem<double>(context, returnValue, r);
}

// Top-level built-in returning a structural type: transform shift(pair).
// The driver applies the returned transform to (0,0) and checks the result.
ASY_FOREIGN_FUNC_SIG(shiftOf)
{
    double const x = AsyFfi::fromItem<double>(context, args->getNumberedArg(0));
    double const y = AsyFfi::fromItem<double>(context, args->getNumberedArg(1));
    AsyFfi::transform const t =
            AsyFfi::call<AsyFfi::transform>(
                    context, stackContext, nullptr, "shift", AsyFfi::pair{x, y}
            );
    AsyFfi::toItem<AsyFfi::transform>(context, returnValue, t);
}

// Function from an imported module (gsl): real hypot(real, real).
ASY_FOREIGN_FUNC_SIG(hypotOf)
{
    double const x = AsyFfi::fromItem<double>(context, args->getNumberedArg(0));
    double const y = AsyFfi::fromItem<double>(context, args->getNumberedArg(1));
    double const r = AsyFfi::call<double>(context, stackContext, "gsl", "hypot", x, y);
    AsyFfi::toItem<double>(context, returnValue, r);
}

} // namespace

REGISTER_FN_SIG
{
    using AsyFfi::TypeInfoOf;

    Asy::TypeInfo const realTy = TypeInfoOf<double>::value;
    Asy::TypeInfo const transformTy = TypeInfoOf<AsyFfi::transform>::value;

    // sqrtOf: real(real)
    Asy::FnArgMetadata const sqrtArgs[1] = {{realTy, "x", false, false}};
    Asy::FunctionTypeMetadata const sqrtMeta{realTy, 1, sqrtArgs};
    registerer->registerFunction("sqrtOf", &sqrtOf, sqrtMeta);

    // shiftOf: transform(real, real)
    Asy::FnArgMetadata const shiftArgs[2] = {
            {realTy, "x", false, false},
            {realTy, "y", false, false},
    };
    Asy::FunctionTypeMetadata const shiftMeta{transformTy, 2, shiftArgs};
    registerer->registerFunction("shiftOf", &shiftOf, shiftMeta);

    // hypotOf: real(real, real)
    Asy::FnArgMetadata const hypotArgs[2] = {
            {realTy, "x", false, false},
            {realTy, "y", false, false},
    };
    Asy::FunctionTypeMetadata const hypotMeta{realTy, 2, hypotArgs};
    registerer->registerFunction("hypotOf", &hypotOf, hypotMeta);
}

