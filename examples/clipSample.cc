#include <asyffi.h>
#include "asyffihelper.h"
#include "asyffihelpers/pluginRegisterDecl.h"
#include <random>

namespace
{
ASY_FOREIGN_FUNC_SIG(createPicture)
{
    ASYFFI_CONTEXT_HELPER;
    auto* picture = context->createPicture(false);
    auto* path1 = static_cast<IAsyPath*>(args->getNumberedArg(0)->asRawPointer());

    AsyFfiHelpers::Pen::PenCreationInfo pci(
        {.red = 0.0, .green = 0.0, .blue = 0.0, .grey = 0.0}, 0.8
    );
    pci.lineWidth = 0.25;
    auto* pen1 = ctxHelper.createNewPen(pci);

    std::random_device rd;
    std::mt19937_64 rgen(rd());
    std::uniform_real_distribution<> dis(-100.0, 100.0);
    std::uniform_real_distribution<> disNudge(-5, 5);

    std::vector<IAsySolvedKnot*> solvedKnots;

    for (auto i = 0; i < 500; ++i)
    {
        double const x = dis(rgen);
        double const y = dis(rgen);

        double const nudgeX = disNudge(rgen);
        double const nudgeY = disNudge(rgen);

        solvedKnots.push_back(context->createSolvedKnot2D(
            context->createPair(x - nudgeX, y - nudgeY), context->createPair(x, y),
            context->createPair(x + nudgeX, y + nudgeY), false
        ));
    }

    auto* createPath =
        context->createAsyPath(solvedKnots.size(), false, STD_CONTAINER_SIZE_AND_DATA(solvedKnots));
    auto* drawElement = context->createDrawElementFromPath(createPath, pen1, nullptr);

    auto pathSrc = ctxHelper.createArray<>({path1});

    picture->appendDrawElement(drawElement);

    auto* clipBegin = context->createDrawElementForBeginClip(pathSrc, false, pen1, true, nullptr);

    picture->encloseDrawElements(clipBegin, context->createDrawElementEndClip(clipBegin, true));
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
