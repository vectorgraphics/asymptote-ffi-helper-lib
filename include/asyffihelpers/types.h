#pragma once

#include <asyffi.h>
#include <string_view>

namespace AsyFfiHelpers::Types
{

Asy::TypeInfo createBasicType(Asy::BaseTypes const& type);
Asy::TypeInfo createArrayType(Asy::TypeInfo const* baseType, size_t const& dimension = 1);

// base types
#define ASYFFI_HELPER_DEC_BASIC_TYPE(TY_NAME, ENUM_TYPE) \
    Asy::TypeInfo const TY_NAME##_TYPE = {.baseType = Asy::ENUM_TYPE}

ASYFFI_HELPER_DEC_BASIC_TYPE(INT, Integer);
ASYFFI_HELPER_DEC_BASIC_TYPE(REAL, Real);
ASYFFI_HELPER_DEC_BASIC_TYPE(STRING, Str);
ASYFFI_HELPER_DEC_BASIC_TYPE(PAIR, Pair);
ASYFFI_HELPER_DEC_BASIC_TYPE(TRIPLE, Triple);
ASYFFI_HELPER_DEC_BASIC_TYPE(BOOL, Boolean);
ASYFFI_HELPER_DEC_BASIC_TYPE(TRANSFORM, Transform);
ASYFFI_HELPER_DEC_BASIC_TYPE(PATH, Path);
ASYFFI_HELPER_DEC_BASIC_TYPE(PATH3, Path3);
ASYFFI_HELPER_DEC_BASIC_TYPE(VOID, Void);

#undef ASYFFI_HELPER_DEC_BASIC_TYPE

// function types

Asy::FnArgMetadata createFnArgMetadata(
    Asy::TypeInfo const& type, char const* name, bool const& optional = false,
    bool const& explicitArgs = false
);

} // namespace AsyFfiHelpers::Types
