#pragma once
#include <asyffi.h>
#include <stdexcept>

namespace AsyFfiHelpers::Structs
{

/** Helper class for Asymptote records. This class aids in field retrieval and instance creation. */
class ManagedRecord
{
public:
    ManagedRecord(IAsyRecord* recordPtr);

    /** Creates a new instance with specified context */
    IAsyVarFrame* createNewInstance(IAsyContext* context, IAsyVarFrame* parentFrame=nullptr) const;

    /** Returns a pointer to the object under field variableName.
     * @remark This function is only to be used when there is exactly one field with that name.
     * When there are multiple fields of the same name but different types, the behavior of this
     * function is undefined.
     * In this case, use
     * {@link ManagedRecord::getField(IAsyVarFrame*, char const*, THAsyType)} instead.
     */
    IAsyItem* getField(IAsyVarFrame* recordInstance, char const* variableName) const;

    /** Returns a pointer to the object under field variableName that has a corresponding type */
    IAsyItem* getField(IAsyVarFrame* recordInstance, char const* variableName, THAsyType type) const;

private:
    IAsyRecord* record;

    // these variables are obtained from record. If record ever changes,
    // these variable should also change

    // init lambda from record
    IAsyLambda* initLambda;
    IAsyProtoEnvironment* protoEnv;
    IAsyProtoEnvironment* postDefProtoEnv;
};
} // namespace AsyFfiHelpers::Structs
