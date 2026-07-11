#include "asyffihelpers/functions.h"

namespace AsyFfiHelpers::Functions
{
Asy::FunctionTypeMetadata createFunctionTypeMetadata(
    Asy::TypeInfo const& returnType, size_t const& numArgs, Asy::FnArgMetadata const* argInfoPtr
)
{
    return {.returnType = returnType, .numArgs = numArgs, .argInfoPtr = argInfoPtr};
}
} // namespace AsyFfiHelpers::Functions
