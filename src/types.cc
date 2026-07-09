#include <asyffihelpers/types.h>

namespace AsyFfiHelpers::Types
{

constexpr Asy::TypeInfo createBasicType(Asy::BaseTypes const& type)
{
    return {.baseType = type, .extraData = {}};
}

constexpr Asy::TypeInfo createArrayType(Asy::TypeInfo const* baseType, size_t const& dimension)
{
    return {
        .baseType = Asy::ArrayType,
        .extraData = {.arrayTypeInfo = {.typeOfItem = baseType, .dimension = dimension}}
    };
}
constexpr Asy::FnArgMetadata createFnArgMetadata(
    Asy::TypeInfo const& type, char const* name, bool const& optional, bool const& explicitArgs
)
{
    return {.type = type, .name = name, .optional = optional, .explicitArgs = explicitArgs};
}

} // namespace AsyFfiHelpers::Types
