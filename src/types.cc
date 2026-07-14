#include <asyffihelpers/types.h>

namespace AsyFfiHelpers::Types
{

Asy::TypeInfo createBasicType(Asy::BaseTypes const& type)
{
    return {.baseType = type, .extraData = {}};
}

Asy::TypeInfo createArrayType(Asy::TypeInfo const* baseType, size_t const& dimension)
{
    return {
        .baseType = Asy::BaseTypes::ArrayType,
        .extraData = {.arrayTypeInfo = {.typeOfItem = baseType, .dimension = dimension}}
    };
}
Asy::FnArgMetadata createFnArgMetadata(
    Asy::TypeInfo const& type, char const* name, bool const& optional, bool const& explicitArgs
)
{
    return {.type = type, .name = name, .optional = optional, .explicitArgs = explicitArgs};
}

} // namespace AsyFfiHelpers::Types
