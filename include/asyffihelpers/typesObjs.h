#pragma once

#include <asyffihelpers/types.h>
#include <memory>
#include <utility>
#include <vector>
#include <string>

namespace AsyFfiHelpers::TypeObjects
{

/** Abstract class for type creation helper objects */
class TypeObject
{
public:
    virtual ~TypeObject() = default;

    /** Converts to {@link Asy::TypeInfo } struct to pass into Asymptote */
    [[nodiscard]]
    virtual Asy::TypeInfo toTypeInfo() const = 0;

    operator Asy::TypeInfo() const
    {
        return toTypeInfo();
    }
};

/**
 * Type helpers for Asymptote primitives
 * (i.e. no additional information required apart from base type name), for example,
 * integers, reals or boolean values
 */
class Primitive : public TypeObject
{
public:
    Primitive(Asy::BaseTypes const& type = Asy::BaseTypes::Void);

    [[nodiscard]]
    Asy::TypeInfo toTypeInfo() const override;

private:
    Asy::BaseTypes type;
};

/**
 * Whether or not (TBaseTypeObj, dimension) is constructible as TBaseTypeObj[]...[] dimension times
 */
template<typename TBaseTypeObj, size_t dimension = 1>
concept ArrayConstructible = dimension >= 1 && std::is_base_of_v<TypeObject, TBaseTypeObj>;

/** Helper object for creation of Asymtpote array types */
class Array : public TypeObject
{
public:
    /**
     * @param baseTypePtr Pointer to base types, for example, for T[], the base type is T
     * @param dimension the dimension of the array. This number must be at least 1
     */
    Array(std::unique_ptr<TypeObject> baseTypePtr, size_t const& dimension = 1);

    [[nodiscard]]
    Asy::TypeInfo toTypeInfo() const override;

    /**
     * @tparam TBaseTypeObj Type of the base type
     * @tparam dimension dimension of the array
     * @param creationArgs Arguments to pass into constructor for {@code T}
     */
    template<typename TBaseTypeObj, size_t dimension = 1, typename... TArgs>
        requires ArrayConstructible<TBaseTypeObj, dimension>
    static Array fromBaseType(TArgs&&... creationArgs)
    {
        return {std::make_unique<TBaseTypeObj>(std::forward<TArgs>(creationArgs)...)};
    }

private:
    // and baseTypeStruct may contain pointer to something in baseType (not always, but
    // we can't be certain it won't, for example, array of functions)
    std::unique_ptr<TypeObject> baseType;

    // the generated type object has a pointer to baseTypeStruct, hence
    // baseTypeStruct must be kept in a memory-stable location (i.e. no moving to somewhere else)
    std::unique_ptr<Asy::TypeInfo> baseTypeStruct;

    size_t dimension;
};

/**
 * Helper class for construction of function types
 */
class Function : public TypeObject
{
public:
    /** Helper struct for constructing individual function arguments */
    struct FunctionArg
    {
        /** Pointer to the argument type */
        std::unique_ptr<TypeObject> argType;

        /** Name of the argument */
        std::string name;

        /** Whether the argument is optional */
        bool optional;

        /** Whether the argument must be explicitly specified */
        bool explicitArgs;

        FunctionArg(
            std::unique_ptr<TypeObject> argTy, std::string name, bool optional = false,
            bool explicitArgs = false
        );

        /**
         * Conveinience function for constructing {@link Function::FunctionArg} with
         * the ability to pass the argument directly to one of {@code TObj}'s constructor
         *
         * @tparam TArgObj type of the argument
         * @param objArgs arguments to pass to one of TArgObj's constructor.
         */
        template<typename TArgObj, typename... TArgs>
            requires std::is_base_of_v<TypeObject, TArgObj>
        static FunctionArg fromNewTypeObj(
            std::string const& name, bool const optional, bool const explicitArgs,
            TArgs&&... objArgs
        )
        {
            return {
                std::make_unique<TArgObj>(std::forward<TArgs>(objArgs)...), name, optional,
                explicitArgs
            };
        }

        /**
         * Like {@link fromNewTypeObj} with name, optional, explicitArgs and objArgs but
         * optional and explicitArgs are both passed as false.
         */
        template<typename TObj, typename... TArgs>
            requires(std::is_base_of_v<TypeObject, TObj> && std::is_constructible_v<TObj, TArgs...>)
        static FunctionArg fromNewTypeObj(std::string const& name, TArgs&&... objArgs)
        {
            return fromNewTypeObj<TObj, TArgs...>(
                name, false, false, std::forward<TArgs>(objArgs)...
            );
        }
    };

    /** Constructs a Function helper type with no arguments */
    Function(std::unique_ptr<TypeObject> returnType);
    Function(std::unique_ptr<TypeObject> returnType, std::vector<FunctionArg> args);

    /** Convenience struct to build a function helper type. Note that this struct is one-time use,
     * which means that after {@link Builder::build} is called, this struct cannot not be used again
     *
     * @tparam TReturnTypeObj type of the return object
     */
    template<typename TReturnTypeObj>
        requires(std::is_base_of_v<TypeObject, TReturnTypeObj>)
    struct Builder
    {
        friend Function;

    protected:
        Builder(std::unique_ptr<TReturnTypeObj>&& returnTypeObj)
            : returnTypeObj(std::move(returnTypeObj))
        {
        }

    public:
        /**
         * Builds a {@link Function} instance. Note that after calling this function,
         * the instance calling build cannot not be used again.
         *
         * Each argument passed into this function is one instance of {@link FunctionArg}. The
         * function is constructed with the argument from {@code arguments} in that order.
         */
        template<std::same_as<FunctionArg>... TArgs>
        Function build(TArgs&&... arguments)
        {
            std::vector<FunctionArg> constructedArgs;
            (constructedArgs.emplace_back(std::forward<TArgs>(arguments)), ...);

            return {std::move(returnTypeObj), std::move(constructedArgs)};
        }

    private:
        std::unique_ptr<TReturnTypeObj> returnTypeObj;
    };

    /**
     * Function to construct a {@link Builder} helper struct.
     * @tparam TReturnTypeObj type of return object
     * @param args Arguments to pass into one of {@code TReturnTypeObj}'s constructor
     */
    template<typename TReturnTypeObj, typename... TReturnTypeArgs>
        requires(std::is_constructible_v<TReturnTypeObj, TReturnTypeArgs...>)
    static Builder<TReturnTypeObj> builder(TReturnTypeArgs&&... args)
    {
        return {std::make_unique<TReturnTypeObj>(std::forward<TReturnTypeArgs>(args)...)};
    }

    [[nodiscard]]
    Asy::TypeInfo toTypeInfo() const override;

    /** Generates an {@link Asy::FunctionTypeMetadata} instance. This is used for
     * registering C++ functions for Asymptote.
     */
    [[nodiscard]]
    Asy::FunctionTypeMetadata toFunctionTypeMetadata() const;

    operator Asy::FunctionTypeMetadata() const
    {
        return toFunctionTypeMetadata();
    }

private:
    std::unique_ptr<TypeObject> returnType;
    Asy::TypeInfo returnTypeStruct;

    std::vector<FunctionArg> functionArgs;
    std::vector<Asy::FnArgMetadata> argumentTypeMetadata;
};

} // namespace AsyFfiHelpers::TypeObjects
