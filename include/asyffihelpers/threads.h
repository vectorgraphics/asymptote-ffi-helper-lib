#pragma once
#include <asyffi.h>
#include <vector>

namespace AsyFfiHelpers::Threads
{

/**
 * @remark Moving objects is only supported within the same thread. Do not move between
 * threads.
 */
class ThreadGcContext
{
public:
    explicit ThreadGcContext(IAsyContext* ctx);
    ~ThreadGcContext();

    ThreadGcContext(ThreadGcContext const& other) = delete;
    ThreadGcContext& operator=(ThreadGcContext const& other) = delete;

    ThreadGcContext(ThreadGcContext&& other) noexcept;
    ThreadGcContext& operator=(ThreadGcContext&& other) noexcept;

private:
    IAsyContext* context;
    std::vector<uint8_t> stackBaseData;
};

} // namespace AsyFfiHelpers::Threads
