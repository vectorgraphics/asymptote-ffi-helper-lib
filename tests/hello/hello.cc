
#include <asyffi.h>

DECLARE_REGISTER_FN;

namespace {

ASY_FOREIGN_FUNC_SIG(doubleIt)
{
    int64_t const x = args->getNumberedArg(0)->asInt64();
    returnValue->setInt64Value(2 * x);
}

} // namespace

REGISTER_FN_SIG
{
    Asy::TypeInfo intTy{Asy::BaseTypes::Integer, {}};
    Asy::FnArgMetadata const arg{intTy, "x", false, false};
    Asy::FunctionTypeMetadata const meta{intTy, 1, &arg};
    registerer->registerFunction(ASYFFI_FN_NAME_AND_ADDR(doubleIt), meta);
}

