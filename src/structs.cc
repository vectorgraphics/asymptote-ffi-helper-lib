#include "asyffihelpers/structs.h"

namespace AsyFfiHelpers::Structs
{

ManagedRecord::ManagedRecord(IAsyRecord* recordPtr)
    : record(recordPtr), initLambda(record->getInitLambda()),
      protoEnv(record->getProtoEnvironment()),
      postDefProtoEnv(record->getPostDefinitionProtoEnvironment())
{
}
IAsyVarFrame* ManagedRecord::createNewInstance(IAsyContext* context, IAsyVarFrame* parentFrame) const
{
    IAsyVarFrame* ret = context->createNewVarFrame(initLambda->getFrameSize());
    ret->getItem(initLambda->getParentIndex())->setRawPointer(parentFrame);
    return ret;
}
IAsyItem* ManagedRecord::getField(IAsyVarFrame* recordInstance, char const* variableName) const
{
    THAsyType typePtr = protoEnv->getTypeFromVariable(variableName);
    if (typePtr == nullptr)
    {
        return nullptr;
    }

    return getField(recordInstance, variableName, typePtr);
}
IAsyItem* ManagedRecord::getField(
    IAsyVarFrame* recordInstance, char const* variableName, THAsyType type
) const
{
    IAsyVarEntry* varEntry = protoEnv->getVarRecord(variableName, type);
    if (varEntry == nullptr)
    {
        return nullptr;
    }

    IAsyAccess* access = varEntry->getAccess();
    if (auto const* localAccess = access->tryCastTo(Asy::AccessTypes::Local);
        localAccess != nullptr)
    {
        return recordInstance->getItem(
            static_cast<IAsyLocalAccess const*>(localAccess)->getOffset()
        );
    }
    return nullptr;
}
} // namespace AsyFfiHelpers::Structs
