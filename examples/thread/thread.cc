// thread.cc -- build an array of random pens / points in parallel.
//
// Registers:
//   pen[]  createRandomPens(int n)
//   pair[] createRandomPoints(int n)
//
// Demonstrates context injection + threaded creation: the injected
// AsyFfi::Context is handed to worker threads (each runs under a GC context
// via ctx.createNewThread), and the result is returned as a std::vector
// (converted to an Asy array).

#include <asyffihelper.h>

#include <algorithm>
#include <cstdint>
#include <random>
#include <thread>
#include <vector>

using namespace AsyFfi;

DECLARE_REGISTER_FN;

REGISTER_FN_SIG
{
  registerFunction(
    registerer, "createRandomPens",
    [](Context ctx, int64_t n) -> std::vector<pen> {
      std::vector<pen> pens(n);
      auto const threadCount =
        std::max<size_t>(std::thread::hardware_concurrency(), 1);
      std::vector<std::thread> threads;
      threads.reserve(threadCount);

      std::random_device rd;
      for (size_t i = 0; i < threadCount; ++i)
        {
          threads.emplace_back(
            ctx.createNewThread(
              [ctx, &pens, threadCount, n](unsigned int const seed,
                                           size_t const idx) {
                std::mt19937_64 eng(seed);
                std::uniform_real_distribution<> dist(0, 1.0);
                for (size_t runIndex = idx; runIndex < static_cast<size_t>(n);
                     runIndex += threadCount)
                  {
                    auto* newPen = ctx.createNewPen(
                      AsyFfiHelpers::Pen::PenCreationInfo(
                        AsyFfiHelpers::Pen::fromRgb(dist(eng), dist(eng),
                                                    dist(eng))));
                    pens[runIndex] = pen{newPen};
                  }
              }, rd(), i));
        }
      for (auto& t : threads)
        {
          t.join();
        }
      return pens;
    }
    );

  registerFunction(
    registerer, "createRandomPoints",
    [](Context ctx, int64_t n) -> std::vector<pair> {
      std::vector<pair> pts(n);
      auto const threadCount =
        std::max<size_t>(std::thread::hardware_concurrency(), 1);
      std::vector<std::thread> threads;
      threads.reserve(threadCount);

      std::random_device rd;
      for (size_t i = 0; i < threadCount; ++i)
        {
          threads.emplace_back(
            ctx.createNewThread(
              [ctx, &pts, threadCount, n](unsigned int const seed,
                                          size_t const idx) {
                std::mt19937_64 eng(seed);
                std::uniform_real_distribution<> dist(-100.0, 100.0);
                for (size_t runIndex = idx; runIndex < static_cast<size_t>(n);
                     runIndex += threadCount)
                  {
                    pts[runIndex] = pair{dist(eng), dist(eng)};
                  }
              }, rd(), i));
        }
      for (auto& t : threads)
        {
          t.join();
        }
      return pts;
    }
    );
}
