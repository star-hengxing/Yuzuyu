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
}

auto Queue::transfer(VkCommandBuffer command) noexcept -> void
{
    const auto info = VkSubmitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &command,
    };

    auto&& queue = handles[static_cast<usize>(Family::transfer)];

    CHECK_RESULT(vkQueueSubmit(queue, 1, &info, VK_NULL_HANDLE));
    CHECK_RESULT(vkQueueWaitIdle(queue));
}

NAMESPACE_END(rhi)
