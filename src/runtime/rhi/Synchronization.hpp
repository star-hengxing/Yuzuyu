#pragma once

#include "base.hpp"

NAMESPACE_BEGIN(rhi)

struct Fence
{
public:
    using Self = Fence;

public:
    VkDevice device = VK_NULL_HANDLE;
    VkFence handle = VK_NULL_HANDLE;

public:
    Fence() {}

    Fence(VkDevice device);

    ~Fence();

    Fence(const Self&) = delete;
    Self& operator = (const Self&) = delete;

    Fence(Self&& other) noexcept : device{other.device}, handle{other.handle}
    {
        other.device = VK_NULL_HANDLE;
        other.handle = VK_NULL_HANDLE;
    }

    auto operator = (Self&& other) noexcept -> Self&
    {
        Self{std::move(other)}.swap(*this);
        return *this;
    }

    auto swap(Self& other) noexcept -> Self&
    {
        using std::swap;
        swap(device, other.device);
        swap(handle, other.handle);
        return *this;
    }

    auto wait(u64 timeout = TIME_OUT) noexcept -> void;

    auto reset() noexcept -> void;
};

struct Semaphore
{
public:
    using Self = Semaphore;

public:
    VkDevice device = VK_NULL_HANDLE;
    VkSemaphore handle = VK_NULL_HANDLE;

public:
    Semaphore() {}

    Semaphore(VkDevice device);

    ~Semaphore();

    Semaphore(const Self&) = delete;
    Self& operator = (const Self&) = delete;

    Semaphore(Self&& other) noexcept : device{other.device}, handle{other.handle}
    {
        other.device = VK_NULL_HANDLE;
        other.handle = VK_NULL_HANDLE;
    }

    auto operator = (Self&& other) noexcept -> Self&
    {
        Self{std::move(other)}.swap(*this);
        return *this;
    }

    auto swap(Self& other) noexcept -> Self&
    {
        using std::swap;
        swap(device, other.device);
        swap(handle, other.handle);
        return *this;
    }
};

NAMESPACE_END(rhi)
