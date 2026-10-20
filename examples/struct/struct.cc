
// struct.cc -- return an instance of a user-defined Asymptote struct.
//
// Registers:  StructSampleReturnData createRandomData()
//
// The return type is a *user-defined* Asymptote struct (defined in
// struct_recfile.asy and loaded at registration), so it cannot be
// inferred from a C++ signature. This example therefore uses the escape-hatch
// overload: a runtime (record) return type plus a raw foreign-function body.
// The field assignments still use the ergonomic AsyFfi::toItem.

#include <asyffihelper.h>

namespace
{
IAsyRecord* structRecord;

ASY_FOREIGN_FUNC_SIG(createRandomData)
{
    AsyFfiHelpers::Structs::ManagedRecord structRec(structRecord);
    IAsyVarFrame* instance = structRec.createNewInstance(context);

    AsyFfi::toItem<int64_t>(context, structRec.getField(instance, "x"), 100);
    AsyFfi::toItem<double>(context, structRec.getField(instance, "y"), 3.14159);
    AsyFfi::toItem<std::string>(context, structRec.getField(instance, "z"), "hello world!");

    returnValue->setRawPointer(instance);
}
} // namespace

DECLARE_REGISTER_FN;

REGISTER_FN_SIG
{
    IAsyRecord* file =            registerer->getGlobalEnvironment()->loadExistingModule("struct_recfile");
    structRecord = file->getProtoEnvironment()->getTypeAsRecord("StructReturnData");

    namespace TO = AsyFfiHelpers::TypeObjects;
    Asy::FunctionTypeMetadata const meta = TO::Function::builder<TO::Record>(structRecord).build();
    AsyFfi::registerFunction(registerer, "createRandomData", &createRandomData, meta);
}

