#include "asyffihelpers/structs.h"

#include <map>
#include <stdexcept>
#include <string>

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

namespace AsyFfi
{

namespace
{
// Cache of looked-up record types, keyed by (moduleName, typeName). Records
// are type definitions loaded from Asymptote modules and live for the process
// lifetime, so it is safe to cache the raw IAsyRecord* here. Repeated lookups
// for the same struct (e.g. across multiple plugins or registrations) hit the
// cache and skip the module load / type lookup.
IAsyRecord* findStructRecord(
    IAsyFfiRegisterer* registerer, char const* moduleName, char const* typeName
)
{
    static std::map<std::pair<std::string, std::string>, IAsyRecord*> cache;

    std::pair<std::string, std::string> const key{moduleName, typeName};
    if (auto const it = cache.find(key); it != cache.end())
    {
        return it->second;
    }

    IAsyRecord* file = registerer->getGlobalEnvironment()->loadExistingModule(moduleName);
    if (file == nullptr)
    {
        throw std::runtime_error(
            "AsyFfi::Struct: failed to load existing module '" + std::string(moduleName) + "'"
        );
    }

    IAsyRecord* record = file->getProtoEnvironment()->getTypeAsRecord(typeName);
    if (record == nullptr)
    {
        throw std::runtime_error(
            "AsyFfi::Struct: module '" + std::string(moduleName)
            + "' has no record type '" + std::string(typeName) + "'"
        );
    }

    cache.emplace(key, record);
    return record;
}
} // namespace

Struct::Struct(IAsyFfiRegisterer* registerer, char const* moduleName, char const* typeName)
    : record_(findStructRecord(registerer, moduleName, typeName))
{
}

} // namespace AsyFfi
