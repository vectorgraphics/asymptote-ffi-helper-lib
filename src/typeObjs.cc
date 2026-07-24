#include <asyffihelpers/typesObjs.h>

namespace AsyFfiHelpers::TypeObjects
{

Primitive::Primitive(Asy::BaseTypes const& type) : type(type)
{
}
Asy::TypeInfo Primitive::toTypeInfo() const
{
    return {.baseType = type, .extraData = {}};
}
Array::Array(std::unique_ptr<TypeObject> baseTypePtr, size_t const& dimension)
    : baseType(std::move(baseTypePtr)),
      baseTypeStruct(std::make_unique<Asy::TypeInfo>(baseType->toTypeInfo())),
      dimension(dimension)
{
}
Asy::TypeInfo Array::toTypeInfo() const
{
    return {
        .baseType = Asy::BaseTypes::ArrayType,
        .extraData = {.arrayTypeInfo = {.typeOfItem = baseTypeStruct.get(), .dimension = dimension}}
    };
}

Function::FunctionArg::FunctionArg(
    std::unique_ptr<TypeObject> argTy, std::string name, bool const optional,
    bool const explicitArgs
)
    : argType(std::move(argTy)), name(std::move(name)), optional(optional),
      explicitArgs(explicitArgs)
{
}
Function::Function(std::unique_ptr<TypeObject> returnType)
    : returnType(std::move(returnType)), returnTypeStruct(returnType->toTypeInfo())
{
}
Function::Function(std::unique_ptr<TypeObject> returnType, std::vector<FunctionArg> args)
    : returnType(std::move(returnType)), returnTypeStruct(returnType->toTypeInfo()),
      functionArgs {std::move(args)}
{
    argumentTypeMetadata.reserve(functionArgs.size());
    for (auto const& arg : functionArgs)
    {
        argumentTypeMetadata.push_back(
            {.type = arg.argType->toTypeInfo(),
             .name = arg.name.c_str(),
             .optional = arg.optional,
             .explicitArgs = arg.explicitArgs}
        );
    }
}
Asy::TypeInfo Function::toTypeInfo() const
{
    return {
        .baseType = Asy::BaseTypes::FunctionType,
        .extraData = {
            .functionTypeInfo = {
                .returnType = &returnTypeStruct,
                .numArgs = argumentTypeMetadata.size(),
                .argInfoPtr = argumentTypeMetadata.data()
            }
        }
    };
}
Asy::FunctionTypeMetadata Function::toFunctionTypeMetadata() const
{
    return {
        .returnType = returnTypeStruct,
        .numArgs = argumentTypeMetadata.size(),
        .argInfoPtr = argumentTypeMetadata.data()
    };
}

} // namespace AsyFfiHelpers::TypeObjects
