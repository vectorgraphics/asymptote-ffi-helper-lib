#pragma once

#include <asyffihelpers/types.h>
#include <memory>
#include <utility>
#include <vector>
#include <string>

namespace AsyFfiHelpers::TypeObjects
{

class TypeObject
{
public:
    virtual ~TypeObject() = default;

    [[nodiscard]]
    virtual Asy::TypeInfo toTypeObject() const = 0;
};

class Primitive : public TypeObject
{
public:
    Primitive(Asy::BaseTypes const& type = Asy::BaseTypes::Void);

    [[nodiscard]]
    Asy::TypeInfo toTypeObject() const override;

private:
    Asy::BaseTypes type;
};

class Array : public TypeObject
{
public:
    Array(std::unique_ptr<TypeObject> baseTypePtr, size_t const& dimension = 1);

    [[nodiscard]]
    Asy::TypeInfo toTypeObject() const override;

    template<typename T, size_t dimension = 1, typename... TArgs>
        requires(dimension >= 1 && std::is_base_of_v<TypeObject, T>)
    static Array fromBaseType(TArgs&&... creationArgs)
    {
        return {std::make_unique<T>(std::forward<TArgs>(creationArgs)...)};
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

class Function : public TypeObject
{
public:
    struct FunctionArg
    {
        std::unique_ptr<TypeObject> argType;
        std::string name;
        bool optional;
        bool explicitArgs;

        FunctionArg(
            std::unique_ptr<TypeObject> argTy, std::string name, bool optional = false,
            bool explicitArgs = false
        );

        template<typename TObj, typename... TArgs>
            requires std::is_base_of_v<TypeObject, TObj>
        static FunctionArg fromNewTypeObj(
            std::string const& name, bool const optional, bool const explicitArgs,
            TArgs&&... objArgs
        )
        {
            return {
                std::make_unique<TObj>(std::forward<TArgs>(objArgs)...), name, optional,
                explicitArgs
            };
        }
        template<typename TObj, typename... TArgs>
            requires std::is_base_of_v<TypeObject, TObj>
        static FunctionArg fromNewTypeObj(std::string const& name, TArgs&&... objArgs)
        {
            return fromNewTypeObj<TObj, TArgs...>(
                name, false, false, std::forward<TArgs>(objArgs)...
            );
        }
    };

    Function(std::unique_ptr<TypeObject> returnType);
    Function(std::unique_ptr<TypeObject> returnType, std::vector<FunctionArg> args);

    template<typename TObj, typename... TArgs>
        requires(
            std::is_base_of_v<TypeObject, TObj>
            && (std::is_constructible_v<FunctionArg, TArgs &&> && ...)
        )
    static Function fromBaseTypes(std::unique_ptr<TObj>&& returnType, TArgs&&... arguments)
    {
        std::vector<FunctionArg> constructedArgs;
        (constructedArgs.emplace_back(std::forward<TArgs>(arguments)), ...);

        return {std::move(returnType), std::move(constructedArgs)};
    }

    [[nodiscard]]
    Asy::TypeInfo toTypeObject() const override;

private:
    std::unique_ptr<TypeObject> returnType;
    Asy::TypeInfo returnTypeStruct;

    std::vector<FunctionArg> functionArgs;
    std::vector<Asy::FnArgMetadata> argumentTypeMetadata;
};

} // namespace AsyFfiHelpers::TypeObjects
