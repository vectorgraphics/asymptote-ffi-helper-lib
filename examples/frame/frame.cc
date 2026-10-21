// frame.cc -- draw a path into a frame with a red pen.
//
// Registers:  frame createFrame(path)
//
// (A C++-level "picture", i.e. IAsyPicture*, is an Asymptote *frame*; see the
// BaseTypes::Picture remark in asyffi.h.)
//
// Demonstrates *context injection* into the type-inferred
// AsyFfi::registerFunction: the lambda's leading AsyFfi::Context parameter is
// the Asymptote host context (not an Asymptote argument -- the machinery
// excludes it from the signature), and the remaining parameter (path) is
// inferred automatically. No hand-written metadata, no manual arg unpacking.

#include <asyffihelper.h>

using namespace AsyFfi;

DECLARE_REGISTER_FN;

REGISTER_FN_SIG
{
    registerFunction(
        registerer, "createFrame",
        [](Context ctx, path p) -> picture {
            auto* pic = ctx->createPicture(false);
            auto* pen = ctx.createNewPen({{.red = 1.0, .green = 0.0, .blue = 0.0, .grey = 0.0}});
            pic->appendDrawElement(ctx->createDrawElementFromPath(p.ptr, pen, nullptr));
            return picture{pic};
        }
    );
}

