#include <asyffi.h>
#include "asyffihelper.h"
#include "asyffihelpers/typesObjs.h"

#include <random>
#include <vector>
#include <thread>

// on windows, compile as a DLL file
// on linux, compile with
// g++ -shared -std=c++20 -fPIC -o libasydll.so asydll.cc

DECLARE_REGISTER_FN;

namespace
{
ASY_FOREIGN_FUNC_SIG(createRandomPens)
{
    ASYFFI_CONTEXT_HELPER

    auto numberOfPoints = AsyFfiHelpers::Item::getItem<int64_t>(args->getNumberedArg(0));
    IAsyArray* newArray = context->createNewArray(numberOfPoints);

    size_t const threadCount = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    threads.reserve(threadCount);

    std::random_device rd;
    for (size_t i = 0; i < threadCount; ++i)
    {
        threads.emplace_back(ctxHelper.createNewThread(
            [context, newArray, threadCount,
             numberOfPoints](unsigned int const seed, size_t const idx)
            {
                std::mt19937_64 randEng(seed);
                std::uniform_real_distribution<> randDist(0, 1.0);

                for (size_t runIndex = idx; runIndex < numberOfPoints; runIndex += threadCount)
                {
                    Asy::PenColor col {
                        .red = randDist(randEng),
                        .green = randDist(randEng),
                        .blue = randDist(randEng),
                        .grey = 0
                    };

                    IAsyPen* newPen = context->createNewPen(
                        nullptr, ASY_PEN_DEFAULT_WIDTH, nullptr, nullptr, 0.0, 0.0,
                        Asy::PenColorSpace::Rgb, col, nullptr, Asy::PenFillRule::Default, nullptr,
                        Asy::PenBaseLine::Default, Asy::PenLineCap::Default,
                        Asy::PenLineJoin::Default, 0.0, Asy::PenOverwrites::Default, nullptr
                    );
                    IAsyItem* newItem = context->createBlankItem();
                    AsyFfiHelpers::Item::setItemPtr(newItem, newPen);
                    newArray->setItem(runIndex, newItem);
                }
            },
            rd(), i
        ));
    }

    for (auto& runThread : threads)
    {
        runThread.join();
    }

    returnValue->setRawPointer(newArray);
}

ASY_FOREIGN_FUNC_SIG(createRandomPoints)
{
    ASYFFI_CONTEXT_HELPER

    auto numberOfPoints = AsyFfiHelpers::Item::getItem<int64_t>(args->getNumberedArg(0));
    IAsyArray* newArray = context->createNewArray(numberOfPoints);

    size_t const threadCount = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    threads.reserve(threadCount);

    std::random_device rd;
    for (size_t i = 0; i < threadCount; ++i)
    {
        threads.emplace_back(ctxHelper.createNewThread(
            [context, newArray, threadCount,
             numberOfPoints](unsigned int const seed, size_t const idx)
            {
                std::mt19937_64 randEng(seed);
                std::uniform_real_distribution<> randDist(-100.0, 100.0);

                for (size_t runIndex = idx; runIndex < numberOfPoints; runIndex += threadCount)
                {
                    IAsyTuple* newPair = context->createPair(randDist(randEng), randDist(randEng));
                    IAsyItem* newItem = context->createBlankItem();
                    AsyFfiHelpers::Item::setItemPtr(newItem, newPair);
                    newArray->setItem(runIndex, newItem);
                }
            },
            rd(), i
        ));
    }

    for (auto& runThread : threads)
    {
        runThread.join();
    }

    returnValue->setRawPointer(newArray);
}

} // namespace

REGISTER_FN_SIG
{
    namespace TypeObjs = AsyFfiHelpers::TypeObjects;

    auto createRandomPtsFnInfo =
        TypeObjs::Function::builder<TypeObjs::Array>(
            TypeObjs::Array::fromBaseType<TypeObjs::Primitive>(Asy::BaseTypes::Pair)
        )
            .build(
                TypeObjs::Function::FunctionArg::fromNewTypeObj<TypeObjs::Primitive>(
                    "numberOfPoints", Asy::BaseTypes::Integer
                )
            );

    registerer->registerFunction(
        ASYFFI_FN_NAME_AND_ADDR(createRandomPoints), createRandomPtsFnInfo
    );

    auto createRandomPenFnInfo =
        TypeObjs::Function::builder<TypeObjs::Array>(
            TypeObjs::Array::fromBaseType<TypeObjs::Primitive>(Asy::BaseTypes::Pen)
        )
            .build(
                TypeObjs::Function::FunctionArg::fromNewTypeObj<TypeObjs::Primitive>(
                    "numberOfPoints", Asy::BaseTypes::Integer
                )
            );

    registerer->registerFunction(ASYFFI_FN_NAME_AND_ADDR(createRandomPens), createRandomPenFnInfo);
}
