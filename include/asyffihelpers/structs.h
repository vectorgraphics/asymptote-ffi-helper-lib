#pragma once
#include <asyffi.h>
#include <asyffihelpers/convert.h>
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

    /**
     * Convenience: set a field to a C++ value using {@link AsyFfi::toItem}.
     *
     * This is a thin wrapper over {@link ManagedRecord::getField} +
     * {@link AsyFfi::toItem}: it looks up the field item for {@code fieldName}
     * on {@code instance} and writes {@code value} into it.
     *
     * @tparam T a type with an {@link AsyFfiHelpers::Item::ItemFunctions}
     *         specialization (e.g. int64_t, double, bool, std::string).
     * @param ctx the Asymptote host context (used by AsyFfi::toItem to
     *        allocate Asy-side storage when needed).
     * @param instance the record instance to write into.
     * @param fieldName the name of the field to set.
     * @param value the C++ value to write.
     */
    template<typename T>
    void set(
        IAsyContext* ctx, IAsyVarFrame* instance, char const* fieldName, T const& value
    ) const
    {
        AsyFfi::toItem<T>(ctx, getField(instance, fieldName), value);
    }

    /**
     * Convenience: read a field as a C++ value using {@link AsyFfi::fromItem}.
     *
     * This is a thin wrapper over {@link ManagedRecord::getField} +
     * {@link AsyFfi::fromItem}: it looks up the field item for {@code fieldName}
     * on {@code instance} and reads the value out of it.
     *
     * @tparam T a type with an {@link AsyFfiHelpers::Item::ItemFunctions}
     *         specialization (e.g. int64_t, double, bool, std::string).
     * @param ctx the Asymptote host context (used by AsyFfi::fromItem to
     *        read Asy-side storage when needed).
     * @param instance the record instance to read from.
     * @param fieldName the name of the field to read.
     * @return the C++ value held by the field.
     */
    template<typename T>
    [[nodiscard]]
    T get(IAsyContext* ctx, IAsyVarFrame* instance, char const* fieldName) const
    {
        return AsyFfi::fromItem<T>(ctx, getField(instance, fieldName));
    }

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

namespace AsyFfi
{

/**
 * @brief Factory handle for a user-defined Asymptote struct (record) type.
 *
 * Bundles the three-step boilerplate that was previously required to use a
 * struct as a function return type:
 * @code
 * IAsyRecord* file = registerer->getGlobalEnvironment()->loadExistingModule("mod");
 * IAsyRecord* rec  = file->getProtoEnvironment()->getTypeAsRecord("MyType");
 * auto meta = TO::Function::builder<TO::Record>(rec).build();
 * @endcode
 *
 * into a single lookup. The module/type lookup is cached by
 * {@code (moduleName, typeName)} so repeated {@code Struct} constructions for
 * the same struct are cheap.
 *
 * The record pointer refers to a type loaded from an Asymptote module and is
 * valid for the lifetime of the process.
 *
 * @code
 * auto& data = AsyFfi::Struct(registerer, "struct_recfile", "StructReturnData");
 * AsyFfi::registerFunction(
 *     registerer, "createRandomData", &createRandomData, data.zeroArgMetadata());
 * @endcode
 */
class Struct
{
public:
    /**
     * Loads {@code moduleName} from the registerer's global environment and
     * looks up the record type {@code typeName} within it.
     *
     * @throws std::runtime_error if the module or the record type cannot be
     *         found.
     */
    Struct(IAsyFfiRegisterer* registerer, char const* moduleName, char const* typeName);

    /** The {@link IAsyRecord} defining this struct type. */
    [[nodiscard]]
    IAsyRecord* record() const
    {
        return record_;
    }

    /**
     * The {@link Asy::TypeInfo} for this struct type
     * ({@link Asy::BaseTypes::Record} with {@code extraData.recordPtr} set to
     * this struct's record). Suitable for use in {@link Asy::TypeInfo} fields
     * such as a function's {@code returnType}.
     */
    [[nodiscard]]
    Asy::TypeInfo typeInfo() const
    {
        return {.baseType = Asy::BaseTypes::Record, .extraData = {.recordPtr = record_}};
    }

    /**
     * A {@link Asy::FunctionTypeMetadata} for a zero-argument function that
     * returns this struct type -- the common "factory" case.
     */
    [[nodiscard]]
    Asy::FunctionTypeMetadata zeroArgMetadata() const
    {
        return {.returnType = typeInfo(), .numArgs = 0, .argInfoPtr = nullptr};
    }

    /**
     * Creates a new instance of this struct in {@code ctx} (via
     * {@link AsyFfiHelpers::Structs::ManagedRecord::createNewInstance}).
     */
    [[nodiscard]]
    IAsyVarFrame* newInstance(IAsyContext* ctx) const
    {
        AsyFfiHelpers::Structs::ManagedRecord mr(record_);
        return mr.createNewInstance(ctx);
    }

private:
    IAsyRecord* record_;
};

} // namespace AsyFfi
