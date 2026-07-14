#include "asyffihelpers/typesTemplates.h"
#include "asyffihelpers/types.h"

namespace AsyFfiHelpers::TypeObjects
{

#define ASYFFI_TYPE_TEMPLATE_DEFINE_BASIC_TYPE(TYPE_NAME, TYPE_OBJ) \
    Asy::TypeInfo const Basic<TYPE_NAME>::value = Types::TYPE_OBJ

ASYFFI_TYPE_TEMPLATE_DEFINE_BASIC_TYPE(int64_t, INT_TYPE);
ASYFFI_TYPE_TEMPLATE_DEFINE_BASIC_TYPE(double, REAL_TYPE);
ASYFFI_TYPE_TEMPLATE_DEFINE_BASIC_TYPE(bool, BOOL_TYPE);
ASYFFI_TYPE_TEMPLATE_DEFINE_BASIC_TYPE(IAsyPen, PEN_TYPE);
ASYFFI_TYPE_TEMPLATE_DEFINE_BASIC_TYPE(IAsyTransform, TRANSFORM_TYPE);
ASYFFI_TYPE_TEMPLATE_DEFINE_BASIC_TYPE(IAsyPath, PATH_TYPE);
ASYFFI_TYPE_TEMPLATE_DEFINE_BASIC_TYPE(IAsyPath3, PATH3_TYPE);

// string
Asy::TypeInfo const String::value = Types::STRING_TYPE;

Asy::TypeInfo const Tuple<2>::value = Types::PAIR_TYPE;
Asy::TypeInfo const Tuple<3>::value = Types::TRIPLE_TYPE;
Asy::TypeInfo const Tuple<6>::value = Types::TRANSFORM_TYPE;

} // namespace AsyFfiHelpers::TypeObjects
