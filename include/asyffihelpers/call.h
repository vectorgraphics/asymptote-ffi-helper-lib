

#pragma once

/**
 * @file call.h
 * @brief Call Asymptote functions from a C++ plugin ({@link AsyFfi::call}).
 *
 * The FFI is bidirectional: Asymptote calls into C++ via registered foreign
 * functions (see {@link register.h}), and C++ can call *back* into Asymptote.
 * The C++->Asy direction is normally verbose: resolve the function with
 * {@link IAsyStackContext::getBuiltin}, hand-build an {@link IAsyItem*}
 * argument array, invoke {@link IAsyStackContext::callReturning} /
 * {@link IAsyStackContext::callVoid}, then unpack the result with
 * {@link AsyFfi::fromItem}.
 *
 * {@link AsyFfi::call} collapses all of that into a one-liner with typed
 * arguments and a typed result:
 * @code
 * #include <asyffihelper.h>
 * using namespace AsyFfi;
 *
 * ASY_FOREIGN_FUNC_SIG(sqrtOf)
 * {
 *     double const x = fromItem<double>(context, args->getNumberedArg(0));
 *     returnValue->setDoubleValue(
 *             call<double>(context, stackContext, nullptr, "sqrt", x));
 * }
 * @endcode
 *
 * Here the fourth argument is {@code module}: it is {@code nullptr} for a
 * top-level built-in, or the id of an imported module (e.g. {@code "gsl"}) to
 * resolve the function from that module.
 *
 * @note {@link AsyFfi::call} takes the {@link IAsyContext*} in addition to the
 * {@link IAsyStackContext*}. The host context is required to materialize
 * argument items ({@link IAsyContext::createBlankItem}) and to convert values
 * ({@link AsyFfi::toItem} / {@link AsyFfi::fromItem}); it is not reachable from
 * {@link IAsyStackContext} alone. Both pointers are the two parameters every
 * foreign function already receives, so no extra lookup is needed.
 *
 * @note A call resolves through {@link IAsyStackContext::getBuiltin}, which only
 * matches Asymptote's C++ built-in functions (i.e. those whose variable location
 * is a {@code bltinAccess}). Asy-language functions (e.g. {@code circle},
 * {@code graph.graph}) are not resolved this way; see the round-trip test for
 * the set of functions that are exercised.
 */

#include <asyffi.h>
#include <asyffihelpers/typeinfo.h>
#include <asyffihelpers/convert.h>
#include <asyffihelpers/context.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace AsyFfi
{

namespace detail
{

/**
 * Short human-readable name for an {@link Asy::BaseTypes}, used in error
 * messages to name the expected return type of a call.
 */
[[nodiscard]]
constexpr char const* baseTypeName(Asy::BaseTypes b)
{
    switch (b)
    {
        case Asy::BaseTypes::Void: return "void";
        case Asy::BaseTypes::Real: return "real";
        case Asy::BaseTypes::Integer: return "int";
        case Asy::BaseTypes::Pair: return "pair";
        case Asy::BaseTypes::Triple: return "triple";
        case Asy::BaseTypes::Boolean: return "bool";
        case Asy::BaseTypes::Str: return "string";
        case Asy::BaseTypes::Inferred: return "inferred";
        case Asy::BaseTypes::Transform: return "transform";
        case Asy::BaseTypes::Guide: return "guide";
        case Asy::BaseTypes::Path: return "path";
        case Asy::BaseTypes::Path3: return "path3";
        case Asy::BaseTypes::CycleToken: return "cycle token";
        case Asy::BaseTypes::TensionSpecifier: return "tension specifier";
        case Asy::BaseTypes::CurlSpecifier: return "curl specifier";
        case Asy::BaseTypes::Pen: return "pen";
        case Asy::BaseTypes::Picture: return "picture";
        case Asy::BaseTypes::File: return "file";
        case Asy::BaseTypes::Code: return "code";
        case Asy::BaseTypes::ArrayType: return "array";
        case Asy::BaseTypes::Record: return "record";
        case Asy::BaseTypes::FunctionType: return "function";
        default: return "(unknown type)";
    }
}

/**
 * Builds the {@link Asy::TypeInfo} for the Asy *function type* {@code
 * TRet(TArgs...)}. {@link IAsyStackContext::getBuiltin} resolves a built-in by
 * (module, name, function type), so the type passed in must describe the full
 * signature -- not merely the return type.
 *
 * All metadata reachable by pointer from the returned {@link Asy::TypeInfo}
 * lives in static storage (the {@link TypeInfoOf<T>::value} members and a
 * function-local {@code static} argument array), so the pointers remain valid
 * for the lifetime of the process.
 */
template<typename TRet, typename... TArgs>
[[nodiscard]]
Asy::TypeInfo makeFnTypeInfo()
{
    static std::array<Asy::FnArgMetadata, sizeof...(TArgs)> const argArr{
        Asy::FnArgMetadata{TypeInfoOf<TArgs>::value, "arg", false, false}...
    };
    Asy::FnArgMetadata const* argPtr = (sizeof...(TArgs) > 0) ? argArr.data() : nullptr;

    return Asy::TypeInfo{
        .baseType = Asy::BaseTypes::FunctionType,
        .extraData = {
            .functionTypeInfo = {&TypeInfoOf<TRet>::value, sizeof...(TArgs), argPtr}
        }
    };
}

/**
 * Core implementation shared by {@link AsyFfi::call} and {@link AsyFfi::callVoid}.
 *
 * The arguments are already collected into a {@code std::tuple<TArgs...>} so
 * that a heterogeneous pack can be indexed by {@code std::index_sequence}.
 */
template<typename TRet, typename... TArgs, std::size_t... Is>
TRet callImpl(
    IAsyContext* context, IAsyStackContext* stack, char const* module, char const* name,
    std::tuple<TArgs...> const& args, std::index_sequence<Is...>
)
{
    // 1. Resolve the built-in by its full signature TRet(TArgs...).
    IAsyCallable* const fn =
            stack->getBuiltin(module, name, makeFnTypeInfo<TRet, TArgs...>());
    if (fn == nullptr)
    {
        // Never silently fall back to a default: name the module, the function
        // and the expected return type.
        char const* where = (module == nullptr) ? "(top-level)" : module;
        std::string msg = std::string("asy::call: could not resolve built-in '") + name +
                "' in module '" + where + "' as a function returning " +
                baseTypeName(makeTypeInfo<TRet>().baseType);
        context->reportError(msg.c_str()); // [[noreturn]]
    }

    // 2. Materialize the arguments as blank items and fill them via toItem.
    // The items are created with Asymptote's (GC-managed) allocator: they live
    // for the duration of the call and are reclaimed by the garbage collector,
    // so no explicit delete is performed here.
    std::array<IAsyItem*, sizeof...(TArgs)> items;
    ((items[Is] = context->createBlankItem(),
      AsyFfi::toItem<std::tuple_element_t<Is, std::tuple<TArgs...>>>(
              context, items[Is], std::get<Is>(args))),
     ...);

    // 3. The FFI expects an array of IAsyItem* pointers.
    std::array<IAsyItem const*, sizeof...(TArgs)> ptrs;
    for (std::size_t i = 0; i < sizeof...(TArgs); ++i)
    {
        ptrs[i] = items[i];
    }

    // 4. Invoke and read back the result.
    if constexpr (std::is_void_v<TRet>)
    {
        stack->callVoid(fn, sizeof...(TArgs), ptrs.data());
        return;
    }
    else
    {
        IAsyItem* const ret = stack->callReturning(fn, sizeof...(TArgs), ptrs.data());
        return AsyFfi::fromItem<TRet>(context, ret);
    }
}

} // namespace detail

/**
 * Calls an Asymptote function (a top-level built-in or one from an imported
 * module) from C++, converting the arguments via {@link AsyFfi::toItem} and
 * reading the typed return value via {@link AsyFfi::fromItem}.
 *
 * @tparam TRet   the expected Asymptote return type. Any type with both a
 *                {@link AsyFfi::TypeInfoOf} and an
 *                {@link AsyFfiHelpers::Item::ItemFunctions} specialization
 *                (double, int64_t, int, bool, std::string, std::vector<T>,
 *                AsyFfi::pair, AsyFfi::transform, AsyFfi::path, ...). Use
 *                {@code void} for functions that return nothing.
 * @tparam TArgs  the Asymptote types of the arguments, matched to the built-in's
 *                actual signature.
 * @param context the Asymptote host context (see the file note for why it is
 *                required).
 * @param stack   the stack context the call runs on.
 * @param module  {@code nullptr} for a top-level built-in, otherwise the id of
 *                an imported module (e.g. {@code "gsl"}).
 * @param name    the Asymptote name of the function.
 * @param args    the argument values.
 * @return the return value, read back as {@code TRet}.
 *
 * @remark If the function (or a matching overload with this exact signature)
 * cannot be resolved, {@link IAsyContext::reportError} is raised naming the
 * module, function and expected return type -- no default value is returned.
 */
template<typename TRet, typename... TArgs>
TRet call(
    IAsyContext* context, IAsyStackContext* stack, char const* module, char const* name,
    TArgs... args
)
{
    return detail::callImpl<TRet, TArgs...>(
            context, stack, module, name, std::tuple<TArgs...>{args...},
            std::index_sequence_for<TArgs...>{}
    );
}

/**
 * Overload of {@link AsyFfi::call} for an injected {@link AsyFfi::Context}.
 *
 * This is the form to use inside a type-inferred
 * {@link AsyFfi::registerFunction} lambda that takes a leading
 * {@code AsyFfi::Context} parameter: the context already carries both the host
 * context ({@code ctx.context}) and the stack context ({@code ctx.stack}), so
 * you only pass the context instead of both pointers.
 * @code
 * registerFunction(registerer, "sqrtPlus",
 *     [](Context ctx, double x) -> double {
 *         return AsyFfi::call<double>(ctx, nullptr, "sqrt", x) + x;
 *     });
 * @endcode
 * @tparam TRet   the expected Asymptote return type (see {@link AsyFfi::call}).
 * @tparam TArgs  the Asymptote types of the arguments.
 * @param ctx     the injected host context (host + stack context).
 * @param module  {@code nullptr} for a top-level built-in, else a module id.
 * @param name    the Asymptote name of the function.
 * @param args    the argument values.
 * @return the return value, read back as {@code TRet}.
 */
template<typename TRet, typename... TArgs>
TRet call(
    AsyFfi::Context const& ctx, char const* module, char const* name, TArgs... args
)
{
    return detail::callImpl<TRet, TArgs...>(
            ctx.context, ctx.stack, module, name, std::tuple<TArgs...>{args...},
            std::index_sequence_for<TArgs...>{}
    );
}

/**
 * Void-returning convenience for {@link AsyFfi::call}.
 *
 * Materializes the arguments exactly like {@link AsyFfi::call} and invokes the
 * built-in via {@link IAsyStackContext::callVoid}. (A "pre-materialized
 * {@link IAsyItem*} array" form cannot be used to resolve a built-in:
 * {@link IAsyStackContext::getBuiltin} requires the argument *types* and their
 * count, which are only available from the typed C++ arguments.)
 *
 * @tparam TArgs the Asymptote types of the arguments.
 * @param context the Asymptote host context.
 * @param stack   the stack context the call runs on.
 * @param module  {@code nullptr} for a top-level built-in, else a module id.
 * @param name    the Asymptote name of the function.
 * @param args    the argument values.
 */
template<typename... TArgs>
void callVoid(
    IAsyContext* context, IAsyStackContext* stack, char const* module, char const* name,
    TArgs... args
)
{
    call<void, TArgs...>(context, stack, module, name, args...);
}

/**
 * Void-returning overload of {@link AsyFfi::call} for an injected
 * {@link AsyFfi::Context} (see the {@code AsyFfi::Context} overload for when to
 * use it).
 * @tparam TArgs the Asymptote types of the arguments.
 * @param ctx     the injected host context (host + stack context).
 * @param module  {@code nullptr} for a top-level built-in, else a module id.
 * @param name    the Asymptote name of the function.
 * @param args    the argument values.
 */
template<typename... TArgs>
void callVoid(
    AsyFfi::Context const& ctx, char const* module, char const* name, TArgs... args
)
{
    call<void, TArgs...>(ctx, module, name, args...);
}

} // namespace AsyFfi

