#include <asyffi.h>
#include "asyffihelper.h"
#include "asyffihelpers/pluginRegisterDecl.h"

namespace
{
ASY_FOREIGN_FUNC_SIG(createPicture)
{
    ASYFFI_CONTEXT_HELPER;
    auto* picture = context->createPicture(false);
    auto* path1 = static_cast<IAsyPath*>(args->getNumberedArg(0)->asRawPointer());

    auto* pen1 = ctxHelper.createNewPen({{.red = 1.0, .green = 0.0, .blue = 0.0, .grey = 0.0}});

    auto* drawElement = context->createDrawElementFromPath(path1, pen1, nullptr);

    picture->appendDrawElement(drawElement);

    returnValue->setRawPointer(picture);
}
} // namespace

REGISTER_FN_SIG
{
    namespace TO = AsyFfiHelpers::TypeObjects;

    auto const pictureBuilderArgs =
        TO::Function::builder<TO::Primitive>(Asy::BaseTypes::Picture)
            .build(
                TO::Function::Argument::fromNewTypeObj<TO::Primitive>("path1", Asy::BaseTypes::Path)
            );

    registerer->registerFunction(ASYFFI_FN_NAME_AND_ADDR(createPicture), pictureBuilderArgs);
}
