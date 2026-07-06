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

} // namespace AsyFfiHelpers::Types
