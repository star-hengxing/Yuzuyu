#include "Queue.hpp"

NAMESPACE_BEGIN(rhi)

auto Queue::get_queue(Family family) const noexcept -> VkQueue
{
    return handles[static_cast<usize>(Family::graphics)];
}

auto Queue::get_index(Family family) const noexcept -> u32
{
    return indexes[static_cast<usize>(Family::graphics)];
}

auto Queue::submit_graphics(const VkSubmitInfo& info, Fence& fence) noexcept -> void
{
    auto queue = get_queue(Family::graphics);

    if (vkGetFenceStatus(device, fence.handle) == VK_SUCCESS)
    {
        fence.wait();
        fence.reset();
    }

    CHECK_RESULT(vkQueueSubmit(queue, 1, &info, fence.handle));
}

auto Queue::submit_transfer(VkCommandBuffer command) noexcept -> void
{
    const auto info = VkSubmitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &command,
    };

    auto queue = get_queue(Family::transfer);

    CHECK_RESULT(vkQueueSubmit(queue, 1, &info, VK_NULL_HANDLE));
    CHECK_RESULT(vkQueueWaitIdle(queue));
}

NAMESPACE_END(rhi)
