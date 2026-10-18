#pragma once

#include <thread>

#include <asyffi.h>
#include "asyffihelpers/threads.h"
#include "asyffihelpers/pen.h"
#include "asyffihelpers/array.h"

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

    /**
     * Creates an array wrapped in {@link Array::ArrayWrapper}
     * with specified type and an initial size
     */
    template<typename T>
    Array::ArrayWrapper<T> createArray(size_t const& initialSize)
    {
        return Array::ArrayWrapper<T>(context->createNewArray(initialSize));
    }

    /**
     * Creates an array wrapped in {@link Array::ArrayWrapper}
     * with specified type, a size and a pointer to array of the items to set.
     */
    template<typename T>
    Array::ArrayWrapper<T> createArray(size_t const& size, T const* ptrToTObjs)
    {
        Array::ArrayWrapper<T> newArray(context->createNewArray(size));

        for (auto i = 0; i < size; ++i)
        {
            newArray[i] = *ptrToTObjs;
        }
        return newArray;
    }

    /**
     * Creates an array wrapped in {@link Array::ArrayWrapper} with specified type
     * from an initializer list.
     */
    template<typename T>
    Array::ArrayWrapper<T> createArray(std::initializer_list<T> items)
    {
        return createArray(items.size(), items.data());
    }

private:
    IAsyContext* context;
};

} // namespace AsyFfiHelpers::Context
