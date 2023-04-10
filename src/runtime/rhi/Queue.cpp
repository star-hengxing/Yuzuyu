#include "Queue.hpp"

NAMESPACE_BEGIN(rhi)

auto Queue::submit(const VkSubmitInfo& info, Family family) noexcept -> void
{
    auto&& fence = fences[static_cast<usize>(family)];
    auto&& queue = handles[static_cast<usize>(family)];
    if (vkGetFenceStatus(device, fence.handle) == VK_SUCCESS)
    {
        fence.wait();
        fence.reset();
    }

    CHECK_RESULT(vkQueueSubmit(queue, 1, &info, fence.handle));
    fence.wait();
    fence.reset();
}

NAMESPACE_END(rhi)
