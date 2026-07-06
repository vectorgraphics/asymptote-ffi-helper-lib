#pragma once

#include <asyffi.h>

namespace AsyFfiHelpers::Types
{

constexpr Asy::TypeInfo createBasicType(Asy::BaseTypes const& type);
constexpr Asy::TypeInfo createArrayType(Asy::TypeInfo const* baseType, size_t const& dimension);

// base types
#define ASYFFI_DEC_BASIC_TYPE(TY_NAME, ENUM_TYPE) \
    Asy::TypeInfo const TY_NAME##_TYPE = {.baseType = Asy::ENUM_TYPE}

ASYFFI_DEC_BASIC_TYPE(INT, Integer);
ASYFFI_DEC_BASIC_TYPE(REAL, Real);
ASYFFI_DEC_BASIC_TYPE(STRING, Str);
ASYFFI_DEC_BASIC_TYPE(PAIR, Pair);
ASYFFI_DEC_BASIC_TYPE(TRIPLE, Triple);
ASYFFI_DEC_BASIC_TYPE(BOOL,Boolean);
ASYFFI_DEC_BASIC_TYPE(TRANSFORM,Transform);



} // namespace AsyFfiHelpers::Types
