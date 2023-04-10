#pragma once

#include "base.hpp"

NAMESPACE_BEGIN(rhi)

struct Fence
{
public:
    VkDevice device = VK_NULL_HANDLE;
    VkFence handle = VK_NULL_HANDLE;

public:
    Fence() {}

    Fence(VkDevice device);

    ~Fence();

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

    ~Semaphore();
};

NAMESPACE_END(rhi)
