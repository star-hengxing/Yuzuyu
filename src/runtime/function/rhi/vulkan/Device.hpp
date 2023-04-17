#pragma once

#ifdef USE_MODULES
import <functional>;
#else
#include <functional>
#endif

#include <vk_mem_alloc.h>

#include "Queue.hpp"

NAMESPACE_BEGIN(rhi::vulkan)

struct Device
{
public:
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    VkDevice handle = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

    VmaAllocator allocator = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    Queue queue;

public:
    auto clean() noexcept -> void;

    [[nodiscard]]
    auto initialize(const std::function<VkSurfaceKHR(VkInstance)>& callback) noexcept -> bool;

    auto wait_idle() const noexcept -> void;
};

NAMESPACE_END(rhi::vulkan)
