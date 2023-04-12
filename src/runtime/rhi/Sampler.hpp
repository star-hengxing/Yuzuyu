#pragma once

#include "base.hpp"

NAMESPACE_BEGIN(rhi)

struct Sampler
{
public:
    using Self = Sampler;

public:
    VkDevice device = VK_NULL_HANDLE;
    VkSampler handle = VK_NULL_HANDLE;

public:
    Sampler() {}

    Sampler(VkDevice device);

    ~Sampler();

    Sampler(const Self&) = delete;
    Self& operator = (const Self&) = delete;

    Sampler(Self&& other) noexcept : device{other.device}, handle{other.handle}
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
