

#pragma once

/**
 * @file register.h
 * @brief Type-inferred function registration for Asymptote FFI plugins.
 *
 * The primary template {@link AsyFfi::registerFunction} deduces the Asymptote
 * return and argument types from the C++ callable's signature, builds the
 * corresponding {@link Asy::FunctionTypeMetadata} automatically, and bridges
 * to the C-ABI trampoline. No hand-maintained metadata, no manual
 * arg-unpacking / return-packing.
 *
 * Supported argument / return types are any type for which both
 * {@link AsyFfi::TypeInfoOf} and
 * {@link AsyFfiHelpers::Item::ItemFunctions} have specializations
 * (void, double, int64_t, int, bool, std::string, std::vector<T>,
 * AsyFfi::pair, AsyFfi::triple, AsyFfi::path, AsyFfi::path3,
 * AsyFfi::picture, AsyFfi::pen, AsyFfi::transform, ...).
 *
 * Minimal plugin:
 * @code
 * #include <asyffihelper.h>
 * using namespace AsyFfi;
 * DECLARE_REGISTER_FN;
 *
 * REGISTER_FN_SIG
 * {
 *     registerFunction(registerer, "doubleIt", [](int64_t x) { return 2 * x; });
 * }
 * @endcode
 */

#include <asyffi.h>
#include <asyffihelpers/typeinfo.h>
#include <asyffihelpers/convert.h>
#include <asyffihelpers/context.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace AsyFfi
{

namespace detail
{

// ---------------------------------------------------------------------------
// SplitSignature: decompose a (member) function pointer / function type into
// return_type and an args tuple.
// ---------------------------------------------------------------------------

template<typename Sig>
struct SplitSignature
{
    static_assert(
            std::false_type::value,
            "AsyFfi::detail::SplitSignature: unsupported callable type. "
            "Expected a function type, function pointer, or a type with operator()."
    );
};

template<typename Ret, typename... Args>
struct SplitSignature<Ret(Args...)>
{
    using return_type = Ret;
    using args = std::tuple<Args...>;
    static constexpr std::size_t num_args = sizeof...(Args);
};

template<typename Ret, typename... Args>
struct SplitSignature<Ret (*)(Args...)>
{
    using return_type = Ret;
    using args = std::tuple<Args...>;
    static constexpr std::size_t num_args = sizeof...(Args);
};

template<typename Ret, typename... Args>
struct SplitSignature<Ret (&(Args...))>
{
    using return_type = Ret;
    using args = std::tuple<Args...>;
    static constexpr std::size_t num_args = sizeof...(Args);
};

// Member function pointers (for lambdas / functors)
template<typename Ret, typename Class, typename... Args>
struct SplitSignature<Ret (Class::*)(Args...) const>
{
    using return_type = Ret;
    using args = std::tuple<Args...>;
    static constexpr std::size_t num_args = sizeof...(Args);
};

template<typename Ret, typename Class, typename... Args>
struct SplitSignature<Ret (Class::*)(Args...)>
{
    using return_type = Ret;
    using args = std::tuple<Args...>;
    static constexpr std::size_t num_args = sizeof...(Args);
};

template<typename Ret, typename Class, typename... Args>
struct SplitSignature<Ret (Class::*)(Args...) const noexcept>
{
    using return_type = Ret;
    using args = std::tuple<Args...>;
    static constexpr std::size_t num_args = sizeof...(Args);
};

template<typename Ret, typename Class, typename... Args>
struct SplitSignature<Ret (Class::*)(Args...) noexcept>
{
    using return_type = Ret;
    using args = std::tuple<Args...>;
    static constexpr std::size_t num_args = sizeof...(Args);
};

// ---------------------------------------------------------------------------
// CallableSignature: deduce Ret and Args from any callable.
//
// For function types / pointers / references the specializations are used
// directly.  For closure types (lambdas, std::function, ...) the primary
// template extracts the operator() member and decomposes it.
// ---------------------------------------------------------------------------

template<typename Func>
struct CallableSignature
{
    using F_ = std::decay_t<Func>;
    using sig = SplitSignature<decltype(&F_::operator())>;
    using return_type = typename sig::return_type;
    using args = typename sig::args;
    static constexpr std::size_t num_args = sig::num_args;
};

// Specializations so that function types / pointers / references do not go
// through the operator() lookup.
template<typename Ret, typename... Args>
struct CallableSignature<Ret(Args...)>
{
    using return_type = Ret;
    using args = std::tuple<Args...>;
    static constexpr std::size_t num_args = sizeof...(Args);
};

template<typename Ret, typename... Args>
struct CallableSignature<Ret (*)(Args...)>
{
    using return_type = Ret;
    using args = std::tuple<Args...>;
    static constexpr std::size_t num_args = sizeof...(Args);
};

template<typename Ret, typename... Args>
struct CallableSignature<Ret (&(Args...))>
{
    using return_type = Ret;
    using args = std::tuple<Args...>;
    static constexpr std::size_t num_args = sizeof...(Args);
};

// ---------------------------------------------------------------------------
// Types<Ts...> tag + TupleToTypes: expand a tuple type into a parameter pack.
// ---------------------------------------------------------------------------

template<typename... Ts>
struct Types
{
};

template<typename Tuple>
struct TupleToTypes;

template<typename... Ts>
struct TupleToTypes<std::tuple<Ts...>>
{
    using type = Types<Ts...>;
};

// ---------------------------------------------------------------------------
// Argument-name generator.
// ---------------------------------------------------------------------------

constexpr char const* argName(std::size_t i)
{
    char const* const names[] = {
            "x", "y", "z", "arg3", "arg4", "arg5", "arg6", "arg7", "arg8", "arg9"
    };
    return (i < 10) ? names[i] : "argN";
}

// ---------------------------------------------------------------------------
// makeArgMetadata: build the Asy::FnArgMetadata array at compile time.
// ---------------------------------------------------------------------------

template<typename... TArgs, std::size_t... Is>
constexpr auto makeArgMetadata(std::index_sequence<Is...>)
{
    return std::array<Asy::FnArgMetadata, sizeof...(TArgs)>{
            Asy::FnArgMetadata{makeTypeInfo<TArgs>(), argName(Is), false, false}...
    };
}

// ---------------------------------------------------------------------------
// callHolder: unpack args, invoke the stored callable, write the return.
// (No injected context: the lambda signature is exactly TRet(TArgs...).)
// ---------------------------------------------------------------------------

template<typename TRet, typename... TArgs, std::size_t... Is>
void callHolder(
    std::function<TRet(TArgs...)>* holder, IAsyContext* ctx, IAsyArgs* args,
    IAsyItem* ret, std::index_sequence<Is...>
)
{
    if constexpr (std::is_void_v<TRet>)
    {
        holder->operator()(AsyFfi::fromItem<TArgs>(ctx, args->getNumberedArg(Is))...);
    }
    else
    {
        TRet result = holder->operator()(
                AsyFfi::fromItem<TArgs>(ctx, args->getNumberedArg(Is))...
        );
        AsyFfi::toItem<TRet>(ctx, ret, result);
    }
}

// ---------------------------------------------------------------------------
// callHolderCtx: same as callHolder, but the lambda's leading parameter is the
// injected AsyFfi::Context. The context is passed to the lambda but is NOT part
// of the Asymptote signature; the remaining TArgs are converted from the
// numbered arguments.
// ---------------------------------------------------------------------------

template<typename TRet, typename... TArgs, std::size_t... Is>
void callHolderCtx(
    std::function<TRet(AsyFfi::Context, TArgs...)>* holder, IAsyContext* ctx,
    IAsyStackContext* stack, IAsyArgs* args, IAsyItem* ret, std::index_sequence<Is...>
)
{
    if constexpr (std::is_void_v<TRet>)
    {
        holder->operator()(
                AsyFfi::Context{ctx, stack},
                AsyFfi::fromItem<TArgs>(ctx, args->getNumberedArg(Is))...
        );
    }
    else
    {
        TRet result = holder->operator()(
                AsyFfi::Context{ctx, stack},
                AsyFfi::fromItem<TArgs>(ctx, args->getNumberedArg(Is))...
        );
        AsyFfi::toItem<TRet>(ctx, ret, result);
    }
}

// ---------------------------------------------------------------------------
// IsContext / StripContext: recognise and remove a leading AsyFfi::Context
// parameter (the injected host context) from the lambda's parameter list so it
// is excluded from the Asymptote signature.
// ---------------------------------------------------------------------------

template<typename T>
struct IsContext : std::is_same<std::decay_t<T>, AsyFfi::Context>
{
};

template<typename... Full>
struct StripContext
{
    static constexpr bool hasCtx = false;
    using data = Types<Full...>;
};

template<typename Head, typename... Rest>
struct StripContext<Head, Rest...>
{
    static constexpr bool hasCtx = IsContext<Head>::value;
    using data = std::conditional_t<
            IsContext<Head>::value, Types<Rest...>, Types<Head, Rest...>>;
};

// ---------------------------------------------------------------------------
// registerFunctionImpl: core registration (Asy data-arg types as a parameter
// pack). HasCtx selects whether the stored callable's leading parameter is the
// injected AsyFfi::Context.
// ---------------------------------------------------------------------------

template<bool HasCtx, typename Func, typename TRet, typename... TArgs>
void registerFunctionImpl(IAsyFfiRegisterer* reg, char const* name, Func f)
{
    using CallFn = std::conditional_t<
            HasCtx, std::function<TRet(AsyFfi::Context, TArgs...)>,
            std::function<TRet(TArgs...)>>;

    // 1. Store the callable.  Each lambda literal has a unique closure type,
    //    so each call site is a distinct template instantiation with its own
    //    static holder.  Process-lifetime, read-only after first registration.
    static CallFn holder = CallFn(std::move(f));

    // 2. Build the argument-metadata array from the Asy data args (the injected
    //    AsyFfi::Context is not an Asymptote argument).
    static const auto argMeta =
            makeArgMetadata<TArgs...>(std::index_sequence_for<TArgs...>{});

    // 3. Trampoline.  The lambda references the function-local static holder,
    //    which is *not* a capture (static variables are accessed by name), so
    //    the lambda is non-capturing and converts to a plain function pointer.
    auto trampoline = [](IAsyContext* ctx, IAsyStackContext* stack, IAsyArgs* args,
                         IAsyItem* ret) {
        if constexpr (HasCtx)
            callHolderCtx<TRet, TArgs...>(
                    &holder, ctx, stack, args, ret, std::index_sequence_for<TArgs...>{});
        else
            callHolder<TRet, TArgs...>(
                    &holder, ctx, args, ret, std::index_sequence_for<TArgs...>{});
    };

    // 4. Build the metadata (return type + Asy data args).
    Asy::FunctionTypeMetadata meta{
            .returnType = makeTypeInfo<TRet>(),
            .numArgs = sizeof...(TArgs),
            .argInfoPtr = (sizeof...(TArgs) > 0) ? argMeta.data() : nullptr
    };

    // 5. Register.
    reg->registerFunction(name, trampoline, meta);
}

// ---------------------------------------------------------------------------
// dispatchData: forward the (context-stripped) Asy data-arg types as a pack to
// registerFunctionImpl.
// ---------------------------------------------------------------------------

template<bool HasCtx, typename Func, typename TRet, typename... TArgs>
void dispatchData(IAsyFfiRegisterer* reg, char const* name, Func f, Types<TArgs...>)
{
    registerFunctionImpl<HasCtx, std::decay_t<Func>, TRet, TArgs...>(reg, name, std::move(f));
}

// ---------------------------------------------------------------------------
// registerFunctionDispatch: uses the Types<Ts...> tag to expand the tuple type
// into a parameter pack, strips a leading AsyFfi::Context (injected host
// context), and forwards the remaining Asy data args.
// ---------------------------------------------------------------------------

template<typename Func, typename TRet, typename... FullArgs>
void registerFunctionDispatch(
    IAsyFfiRegisterer* reg, char const* name, Func f, Types<FullArgs...>
)
{
    using D = StripContext<FullArgs...>;
    dispatchData<D::hasCtx, Func, TRet>(reg, name, std::move(f), typename D::data{});
}

} // namespace detail

// ===========================================================================
// Public API
// ===========================================================================

/**
 * Registers a function with Asymptote, inferring all type metadata from the
 * callable's C++ signature.
 *
 * @tparam Func  the callable type (lambda, function pointer, std::function, ...)
 * @param reg    the Asymptote FFI registerer (from REGISTER_FN_SIG)
 * @param name   the function name as seen from Asymptote
 * @param f      the callable to register
 *
 * The callable must be a plain `Ret(Args...)` (no default arguments, no
 * optional / named arguments).  For those cases use the escape-hatch overload
 * below.
 */
template<typename Func>
void registerFunction(IAsyFfiRegisterer* reg, char const* name, Func&& f)
{
    using FuncDecay = std::decay_t<Func>;
    using Sig = detail::CallableSignature<FuncDecay>;
    using TRet = typename Sig::return_type;
    using TArgsTag = typename detail::TupleToTypes<typename Sig::args>::type;

    detail::registerFunctionDispatch<FuncDecay, TRet>(
            reg, name, std::forward<Func>(f), TArgsTag{}
    );
}

/**
 * Escape hatch: register with an explicit C-ABI trampoline and hand-built
 * metadata.  Use this for optional / named arguments or any case the primary
 * template cannot express.
 */
inline void registerFunction(
    IAsyFfiRegisterer* reg, char const* name, TAsyForeignFunction fn,
    Asy::FunctionTypeMetadata const& meta
)
{
    reg->registerFunction(name, fn, meta);
}

} // namespace AsyFfi

