

/**
 * Round-trip plugin for type-inferred registerFunction (Phase 4).
 *
 * Every function is registered using ONLY the new
 * AsyFfi::registerFunction template -- no hand-written metadata, no
 * manual arg-unpacking / return-packing.
 *
 * Registered functions:
 *  - doubleIt(int) -> int
 *  - midpoint(pair, pair) -> pair
 *  - addAll(real[]) -> real
 *  - makeCircle() -> path
 *  - logIt(int) -> void
 */

#include <asyffihelper.h>

#include <numeric>
#include <vector>

using namespace AsyFfi;

DECLARE_REGISTER_FN;

REGISTER_FN_SIG
{
    // int(int)
    registerFunction(registerer, "doubleIt", [](int64_t x) -> int64_t { return 2 * x; });

    // pair(pair, pair)
    registerFunction(
        registerer, "midpoint", [](AsyFfi::pair a, AsyFfi::pair b) -> AsyFfi::pair {
            return AsyFfi::pair{(a.x + b.x) / 2.0, (a.y + b.y) / 2.0};
        }
    );

    // real(real[])
    registerFunction(
        registerer, "addAll", [](std::vector<double> xs) -> double {
            return std::accumulate(xs.begin(), xs.end(), 0.0);
        }
    );

    // path()  -- needs IAsyContext* to build the path, so capture it.
    registerFunction(
        registerer, "makeCircle", [context]() -> AsyFfi::path {
            // Unit square: (0,0)->(1,0)->(1,1)->(0,1)->cycle
            IAsyTuple* const p0 = context->createPair(0.0, 0.0);
            IAsyTuple* const p1 = context->createPair(1.0, 0.0);
            IAsyTuple* const p2 = context->createPair(1.0, 1.0);
            IAsyTuple* const p3 = context->createPair(0.0, 1.0);

            IAsySolvedKnot* const k0 = context->createSolvedKnot2D(p0, p1, p2, true);
            IAsySolvedKnot* const k1 = context->createSolvedKnot2D(p1, p2, p3, true);
            IAsySolvedKnot* const k2 = context->createSolvedKnot2D(p2, p3, p0, true);
            IAsySolvedKnot* const k3 = context->createSolvedKnot2D(p3, p0, p1, true);

            IAsySolvedKnot* const knots[] = {k0, k1, k2, k3};
            IAsyPath* const p = context->createAsyPath(4, true, 4, knots);
            return AsyFfi::path{p};
        }
    );

    // void(int)  -- writes a warning via the context
    registerFunction(
        registerer, "logIt", [context](int64_t x) -> void {
            context->reportWarning("logIt called with value");
        }
    );
}

