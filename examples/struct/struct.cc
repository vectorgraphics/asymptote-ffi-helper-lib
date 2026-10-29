// struct.cc -- create and read instances of a user-defined Asymptote struct.
//
// Registers:
//   StructReturnData createRandomData():          returns new struct instance
//   int              doubleX(StructReturnData r): reads a struct argument
//   StructReturnData copyStruct(StructReturnData r): independent copy, x += 1
//
// Struct types are *user-defined* Asymptote structs (defined in
// struct_recfile.asy, loaded at registration), so their return/argument types
// cannot be inferred from a C++ signature. These functions therefore use the
// escape-hatch overload: a runtime (record) type in the metadata plus a raw
// foreign-function body.
//
// The struct type is looked up once with AsyFfi::Struct. Fields are written
// and read with ManagedRecord::set/get, and struct values cross the boundary
// as an AsyFfi::record (a wrapper around the instance's IAsyVarFrame): an
// argument is read with AsyFfi::fromItem<AsyFfi::record>(doubleX, copyStruct)
// and a return value is written with
// AsyFfi::toItem<AsyFfi::record> (copyStruct).

#include <asyffihelper.h>

namespace
{
// The struct's record type, looked up once at registration via AsyFfi::Struct.
// The IAsyRecord* is a type definition loaded from an Asymptote module and is
// valid for the lifetime of the process, so it is safe to keep around for the
// raw foreign-function body.
IAsyRecord* structRecord;

ASY_FOREIGN_FUNC_SIG(createRandomData)
{
    AsyFfiHelpers::Structs::ManagedRecord mr(structRecord);
    IAsyVarFrame* instance = mr.createNewInstance(context);

    mr.set(context, instance, "x", 100);
    mr.set(context, instance, "y", 1.0/7.0);
    mr.set<std::string>(context, instance, "z", "hello world!");

    returnValue->setRawPointer(instance);
}

// Read the "x" field of a struct argument and return 2*x.
// The incoming struct is pulled out as an AsyFfi::record (a wrapper around the
// instance's IAsyVarFrame) via the escape-hatch argument item;
// ManagedRecord::get then reads the field.
ASY_FOREIGN_FUNC_SIG(doubleX)
{
    AsyFfi::record const r = AsyFfi::fromItem<AsyFfi::record>(context, args->getNumberedArg(0));
    AsyFfiHelpers::Structs::ManagedRecord mr(structRecord);
    int64_t const x = mr.get<int64_t>(context, r.ptr, "x");
    returnValue->setInt64Value(2 * x);
}

// Return a fresh, independent instance derived from the argument.
// Reads the incoming struct as an AsyFfi::record, builds a new instance,
// copies its fields, then modifies the copy (x += 1) and writes it back out
// through the same proxy (AsyFfi::toItem<AsyFfi::record>).
// This exercises the record proxy in both directions and shows the returned
// struct is a separate, writable value: the original argument is left
// untouched by the modification.
ASY_FOREIGN_FUNC_SIG(copyStruct)
{
    AsyFfi::record const in =
      AsyFfi::fromItem<AsyFfi::record>(context, args->getNumberedArg(0));
    AsyFfiHelpers::Structs::ManagedRecord mr(structRecord);

    IAsyVarFrame* out = mr.createNewInstance(context);
    mr.set(context, out, "x", mr.get<int64_t>(context, in.ptr, "x") + 1);
    mr.set(context, out, "y", mr.get<double>(context, in.ptr, "y"));
    mr.set<std::string>(context, out, "z",
                        mr.get<std::string>(context, in.ptr, "z"));

    AsyFfi::toItem<AsyFfi::record>(context, returnValue, AsyFfi::record{out});
}
} // namespace

DECLARE_REGISTER_FN;

REGISTER_FN_SIG
{
    // Look up the struct type once; keep the record around for the
    // function bodies.
    AsyFfi::Struct const data(registerer,"struct_recfile","StructReturnData");
    structRecord = data.record();

    // createRandomData() -> StructReturnData  (zero-arg factory case).
    AsyFfi::registerFunction(
        registerer,"createRandomData",&createRandomData,data.zeroArgMetadata()
    );

    // doubleX(StructReturnData r) -> int  (struct as an argument).
    Asy::FnArgMetadata const arg{.type = data.typeInfo(), .name = "r",
      .optional = false,.explicitArgs = false};
    Asy::FunctionTypeMetadata const meta{
        .returnType = {.baseType = Asy::BaseTypes::Integer, .extraData = {}},
        .numArgs = 1,
        .argInfoPtr = &arg
    };
    AsyFfi::registerFunction(registerer, "doubleX", &doubleX, meta);

    // copyStruct(StructReturnData r) ->
    // StructReturnData(struct arg + struct return).
    Asy::FunctionTypeMetadata const copyMeta{
        .returnType = data.typeInfo(),
        .numArgs = 1,
        .argInfoPtr = &arg
    };
    AsyFfi::registerFunction(registerer, "copyStruct", &copyStruct, copyMeta);
}
