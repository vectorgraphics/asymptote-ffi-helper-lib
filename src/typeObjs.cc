#include <asyffihelpers/typesObjs.h>

namespace AsyFfiHelpers::TypeObjects
{

Primitive::Primitive(Asy::BaseTypes const& type) : type(type)
{
}
Asy::TypeInfo Primitive::toTypeObject() const
{
    return {.baseType = type, .extraData = {}};
}
Array::Array(std::unique_ptr<TypeObject> baseTypePtr, size_t const& dimension)
    : baseType(std::move(baseTypePtr)),
      baseTypeStruct(std::make_unique<Asy::TypeInfo>(baseType->toTypeObject())),
      dimension(dimension)
{
}
Asy::TypeInfo Array::toTypeObject() const
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
    : returnType(std::move(returnType)), returnTypeStruct(returnType->toTypeObject())
{
}
Function::Function(std::unique_ptr<TypeObject> returnType, std::vector<FunctionArg> args)
    : returnType(std::move(returnType)), returnTypeStruct(returnType->toTypeObject()),
      functionArgs {std::move(args)}
{
    argumentTypeMetadata.reserve(functionArgs.size());
    for (auto const& arg : functionArgs)
    {
        argumentTypeMetadata.push_back(
            {.type = arg.argType->toTypeObject(),
             .name = arg.name.c_str(),
             .optional = arg.optional,
             .explicitArgs = arg.explicitArgs}
        );
    }
}
Asy::TypeInfo Function::toTypeObject() const
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
