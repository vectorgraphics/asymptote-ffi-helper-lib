#pragma once

#include <asyffi.h>
#include "asyffihelpers/threads.h"
#include <thread>

namespace AsyFfiHelpers::Item
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

private:
    IAsyContext* context;
};

} // namespace AsyFfiHelpers::Item
