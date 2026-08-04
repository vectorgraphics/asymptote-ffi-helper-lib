#include <asyffi.h>
#include "asyffihelper.h"
#include "asyffihelpers/pluginRegisterDecl.h"

namespace
{

IAsyRecord* structAsyFile;
IAsyRecord* structRecord;

ASY_FOREIGN_FUNC_SIG(createRandomData)
{
    auto* penv = structRecord->getProtoEnvironment();
    IAsyVarFrame* newVarFrame = context->createNewVarFrame(1);

    auto* xTy = penv->getTypeFromVariable("x");
    IAsyVarEntry* xVarRec = penv->getVarRecord("x", xTy);
    auto const* xAccess =
        static_cast<IAsyLocalAccess*>(xVarRec->getAccess()->tryCastTo(Asy::AccessTypes::Local));
    newVarFrame->extend(xAccess->getOffset() + 1);
    AsyFfiHelpers::Item::setItem<int64_t>(newVarFrame->getItem(xAccess->getOffset()), 100);

    auto* yTy = penv->getTypeFromVariable("y");
    IAsyVarEntry* yVarRec = penv->getVarRecord("y", yTy);
    auto const* yAccess =
        static_cast<IAsyLocalAccess*>(yVarRec->getAccess()->tryCastTo(Asy::AccessTypes::Local));
    newVarFrame->extend(yAccess->getOffset() + 1);
    AsyFfiHelpers::Item::setItem<double>(newVarFrame->getItem(yAccess->getOffset()), 3.14159);

    auto* zTy = penv->getTypeFromVariable("z");
    IAsyVarEntry* zVarRec = penv->getVarRecord("z", zTy);
    auto const* zAccess =
        static_cast<IAsyLocalAccess*>(zVarRec->getAccess()->tryCastTo(Asy::AccessTypes::Local));

    newVarFrame->extend(zAccess->getOffset() + 1);
    void* newStr = context->createNewAsyString("hello world!");
    newVarFrame->getItem(zAccess->getOffset())->setRawPointer(newStr);

    returnValue->setRawPointer(newVarFrame);
}

} // namespace

REGISTER_FN_SIG
{
    structAsyFile = registerer->getGlobalEnvironment()->loadFileModule(
        "structSample_recfile", "structSample_recfile.asy"
    );

    structRecord = structAsyFile->getProtoEnvironment()->getTypeAsRecord("StructSampleReturnData");

    namespace TO = AsyFfiHelpers::TypeObjects;
    auto const builderArgs = TO::Function::builder<TO::Record>(structRecord).build();
    registerer->registerFunction(ASYFFI_FN_NAME_AND_ADDR(createRandomData), builderArgs);
}
