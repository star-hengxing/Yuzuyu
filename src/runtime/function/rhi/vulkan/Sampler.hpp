#pragma once

#include "base.hpp"

NAMESPACE_BEGIN(rhi::vulkan)

struct Sampler
{
public:
    VkDevice device = VK_NULL_HANDLE;
    VkSampler handle = VK_NULL_HANDLE;

public:
    Sampler() {}

    Sampler(VkDevice device);

    auto clean() noexcept -> void;
};

NAMESPACE_END(rhi::vulkan)
