#pragma once

#include <asyffi.h>
#include "asyffihelpers/threads.h"
#include <thread>
#include "asyffihelpers/pen.h"

namespace AsyFfiHelpers::Context
{

class AsyContextHelper
{
public:
    explicit AsyContextHelper(IAsyContext* ctx);

    template<typename TFunction, typename... TArgs>
    std::thread createNewThread(TFunction&& fn, TArgs&&... args)
    {
        return std::thread(
            [ctx = this->context, lambdaFn = std::forward<TFunction>(fn),
             ... args = std::forward<TArgs>(args)]() mutable
            {
                Threads::ThreadGcContext tgc(ctx);
                return lambdaFn(std::forward<TArgs>(args)...);
            }
        );
    }

    /** Creates a new pen using {@link Pen::PenCreationInfo} struct */
    [[nodiscard]]
    IAsyPen* createNewPen(Pen::PenCreationInfo const& penCreationInfo) const;

    /** Creates a new C++ string instance from asymptote string */
    std::string createStringFromAsyString(THAsyString asyString) const;

private:
    IAsyContext* context;
};

} // namespace AsyFfiHelpers::Item
