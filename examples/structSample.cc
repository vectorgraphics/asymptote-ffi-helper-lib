#include <asyffi.h>
#include "asyffihelper.h"
#include "asyffihelpers/pluginRegisterDecl.h"
#include "asyffihelpers/structs.h"

namespace
{

IAsyRecord* structAsyFile;
IAsyRecord* structRecord;

ASY_FOREIGN_FUNC_SIG(createRandomData)
{
    AsyFfiHelpers::Structs::ManagedRecord structRec(structRecord);

    IAsyVarFrame* newVf = structRec.createNewInstance(context);

    namespace AI = AsyFfiHelpers::Item;

    AI::setItem<int64_t>(structRec.getField(newVf, "x"), 100U);
    AI::setItem(structRec.getField(newVf, "y"), 3.14159);
    AI::setItem(structRec.getField(newVf, "z"), context->createNewAsyString("hello world!"));

    AI::setItem(returnValue, newVf);
}

} // namespace

REGISTER_FN_SIG
{
    structAsyFile = registerer->getGlobalEnvironment()->loadExistingModule("structSample_recfile");

    structRecord = structAsyFile->getProtoEnvironment()->getTypeAsRecord("StructSampleReturnData");

    namespace TO = AsyFfiHelpers::TypeObjects;
    auto const builderArgs = TO::Function::builder<TO::Record>(structRecord).build();
    registerer->registerFunction(ASYFFI_FN_NAME_AND_ADDR(createRandomData), builderArgs);
}
