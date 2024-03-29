#pragma once

#include <core/base/Rect2D.hpp>
#include "Swapchain.hpp"
#include "Device.hpp"

NAMESPACE_BEGIN(rhi::vulkan)

struct Texture
{
public:
    struct create_info
    {
        u32 width;
        u32 height;
        VkImageUsageFlags flags;
    };

public:
    Device* device = nullptr;
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;

    create_info info;
    bool is_swapchain_buffer = false;

public:
    Texture(Device* device, const create_info& info);

    Texture(const Swapchain& swapchain, usize index);

    auto clean() noexcept -> void;

    auto get_size() const noexcept -> Rect2D;
};

NAMESPACE_END(rhi::vulkan)
