#pragma once
#include "types.h"

#include <asyffi.h>

namespace AsyFfiHelpers::Functions
{

Asy::FunctionTypeMetadata createFunctionTypeMetadata(
    Asy::TypeInfo const& returnType = Types::VOID_TYPE, size_t const& numArgs = 0,
    Asy::FnArgMetadata const* argInfoPtr = nullptr
);
} // namespace AsyFfiHelpers::Functions
