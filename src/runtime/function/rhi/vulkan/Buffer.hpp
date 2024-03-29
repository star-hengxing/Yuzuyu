#pragma once

#include "Device.hpp"

NAMESPACE_BEGIN(rhi::vulkan)

struct Buffer
{
public:
    Device* device = nullptr;
    VkBuffer handle = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;

    void* data = nullptr;
    usize size;

public:
    Buffer(Device* device, usize size, VkBufferUsageFlags flags, VmaMemoryUsage type);

    auto clean() noexcept -> void;

    auto map() noexcept -> u8*;
    auto unmap() noexcept -> void;

    auto cpu_to_gpu(const u8* data, usize size, usize offset) noexcept -> void;
    auto gpu_to_cpu(u8* data, usize size, usize offset) noexcept -> void;

    auto flush() noexcept -> void;
};

NAMESPACE_END(rhi::vulkan)
