
// clip.cc -- clip a random "cloud" path to the input path.
//
// Registers:  frame clip(path)
//
// Same context-injection pattern as picture.cc: the leading AsyFfi::Context is
// the host context (excluded from the Asymptote signature). `ctx->` reaches raw
// IAsyContext methods; ctx.<name> reaches the ergonomic helpers (createNewPen,
// createArray). No hand-written metadata, no manual arg unpacking.

#include <asyffihelper.h>

#include <random>
#include <vector>

using namespace AsyFfi;

DECLARE_REGISTER_FN;

REGISTER_FN_SIG
{
    registerFunction(
        registerer, "clip",
        [](Context ctx, path p) -> picture {
            auto* pic = ctx->createPicture(false);

            // A thin, semi-transparent pen.
            AsyFfiHelpers::Pen::PenCreationInfo pci(
                    {.red = 0.0, .green = 0.0, .blue = 0.0, .grey = 0.0}, 0.8
            );
            pci.lineWidth = 0.25;
            auto* pen = ctx.createNewPen(pci);

            // A random cloud of 500 knots, drawn as a path.
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
                solvedKnots.push_back(ctx->createSolvedKnot2D(
                        ctx->createPair(x - nudgeX, y - nudgeY),
                        ctx->createPair(x, y),
                        ctx->createPair(x + nudgeX, y + nudgeY), false
                ));
            }
            auto* cloud = ctx->createAsyPath(
                    solvedKnots.size(), false, STD_CONTAINER_SIZE_AND_DATA(solvedKnots)
            );

            // Clip the cloud to the input path p.
            auto* cloudEl = ctx->createDrawElementFromPath(cloud, pen, nullptr);
            auto clipSrc = ctx.createArray<>({p.ptr});
            auto* begin = ctx->createDrawElementForBeginClip(clipSrc, false, pen, true, nullptr);
            pic->appendDrawElement(cloudEl);
            pic->encloseDrawElements(begin, ctx->createDrawElementEndClip(begin, true));

            return picture{pic};
        }
    );
}

