#include "asyffihelpers/threads.h"

namespace AsyFfiHelpers::Threads
{

ThreadGcContext::ThreadGcContext(IAsyContext* ctx)
    : context(ctx), stackBaseData(ctx->getGcStackBaseSize())
{
    if (!ctx->isGcSupported())
    {
        context = nullptr;
        stackBaseData.clear();
        return;
    }
    void* stackBasePtr = stackBaseData.data();
    ctx->getGcStackBase(stackBasePtr);
    ctx->registerThreadWithGc(stackBasePtr);
}
ThreadGcContext::~ThreadGcContext()
{
    if (stackBaseData.size() == 0 || context == nullptr)
    {
        return;
    }
    context->unregisterThreadWithGc();
}
ThreadGcContext::ThreadGcContext(ThreadGcContext&& other) noexcept
    : context(std::exchange(other.context, nullptr)), stackBaseData(std::move(other.stackBaseData))
{
    other.stackBaseData.clear();
}
ThreadGcContext& ThreadGcContext::operator=(ThreadGcContext&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    this->context = std::exchange(other.context, nullptr);
    this->stackBaseData = std::move(other.stackBaseData);
    other.stackBaseData.clear();

    return *this;
}
} // namespace AsyFfiHelpers::Threads
