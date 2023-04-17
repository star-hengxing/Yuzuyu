#pragma once

#include "base.hpp"

NAMESPACE_BEGIN(rhi::vulkan)

struct Fence
{
public:
    VkDevice device = VK_NULL_HANDLE;
    VkFence handle = VK_NULL_HANDLE;

public:
    Fence() {}

    Fence(VkDevice device);

    auto clean() noexcept -> void;

    auto wait(u64 timeout = TIME_OUT) noexcept -> void;

    auto reset() noexcept -> void;
};

struct Semaphore
{
public:
    VkDevice device = VK_NULL_HANDLE;
    VkSemaphore handle = VK_NULL_HANDLE;

public:
    Semaphore() {}

    Semaphore(VkDevice device);

    auto clean() noexcept -> void;
};

NAMESPACE_END(rhi::vulkan)
