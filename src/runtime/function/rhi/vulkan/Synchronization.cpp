#include "Synchronization.hpp"

NAMESPACE_BEGIN(rhi::vulkan)

Fence::Fence(VkDevice device) : device{device}
{
    const auto info = VkFenceCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    CHECK_RESULT(vkCreateFence(device, &info, VK_NULL_HANDLE, &handle));
}

auto Fence::clean() noexcept -> void
{
    if (handle)
    {
        vkDestroyFence(device, handle, VK_NULL_HANDLE);
        handle = VK_NULL_HANDLE;
    }
}

auto Fence::wait(u64 timeout) noexcept -> void
{
    CHECK_RESULT(vkWaitForFences(device, 1, &handle, VK_TRUE, timeout));
}

auto Fence::reset() noexcept -> void
{
    CHECK_RESULT(vkResetFences(device, 1, &handle));
}

Semaphore::Semaphore(VkDevice device) : device{device}
{
    auto info = VkSemaphoreCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    CHECK_RESULT(vkCreateSemaphore(device, &info, VK_NULL_HANDLE, &handle));
}

auto Semaphore::clean() noexcept -> void
{
    if (handle)
    {
        vkDestroySemaphore(device, handle, VK_NULL_HANDLE);
        handle = VK_NULL_HANDLE;
    }
}

NAMESPACE_END(rhi::vulkan)
