
#pragma once

#include <asyffi.h>

#include <asyffihelpers/contextFuncs.h>

namespace AsyFfi
{

/**
 * @brief The Asymptote host context, injected into a type-inferred plugin.
 *
 * {@link registerFunction} lets a plugin lambda take a *leading*
 * {@code AsyFfi::Context} parameter. That parameter is **not** an Asymptote
 * argument: it is the {@link IAsyContext*} of the running Asymptote, handed to
 * the lambda so it can allocate Asy-side values (pictures, pens, arrays,
 * threads, ...). The machinery excludes it from the function's Asymptote
 * signature; only the *remaining* parameters are inferred as the real
 * signature.
 *
 * One rule to remember:
 *  - {@code ctx->method(...)} : the raw {@link IAsyContext} host C API, e.g.
 *    {@code ctx->createPicture(false)} or
 *    {@code ctx->createDrawElementFromPath(...)}.
 *  - {@code ctx.method(...)}  : a small set of C++ convenience helpers (inherited
 *    from {@link AsyFfiHelpers::Context::AsyContextHelper}) for the raw calls
 *    that are awkward to write, e.g. {@code ctx.createNewPen(PenCreationInfo)}
 *    or {@code ctx.createArray<>({...})}.
 *
 * The context also carries the {@link IAsyStackContext*} the foreign function
 * runs on ({@code ctx.stack}), which is needed to call back into Asymptote via
 * {@link AsyFfi::call} (see {@code call.h}). That overload takes the context
 * directly, so a type-inferred lambda can call Asy without hand-built
 * metadata:
 * @code
 * using namespace AsyFfi;
 * DECLARE_REGISTER_FN;
 *
 * REGISTER_FN_SIG
 * {
 *     // real sqrtPlus(real)  ==  sqrt(x) + x   (C++ calls the Asy builtin sqrt)
 *     registerFunction(registerer, "sqrtPlus",
 *         [](Context ctx, double x) -> double {
 *             return AsyFfi::call<double>(ctx, nullptr, "sqrt", x) + x;
 *         });
 * }
 * @endcode
 */
class Context : public AsyFfiHelpers::Context::AsyContextHelper
{
public:
    /**
     * @param ctx   the raw {@link IAsyContext} host context (stored as
     *              {@code context} via {@link AsyContextHelper}).
     * @param stack the {@link IAsyStackContext} the foreign function runs on;
     *              needed to call back into Asymptote (see {@link AsyFfi::call}).
     */
    Context(IAsyContext* ctx, IAsyStackContext* stack): AsyContextHelper(ctx), stack(stack) {}

    /** The stack context, for calling back into Asymptote ({@link AsyFfi::call}). */
    IAsyStackContext* stack;

    /** Forward to the raw {@link IAsyContext}: {@code ctx->createPicture(false)}. */
    IAsyContext* operator->() const { return context; }
};

} // namespace AsyFfi

