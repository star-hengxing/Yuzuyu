#pragma once

#include <runtime/base/Rect2D.hpp>
#include "Swapchain.hpp"
#include "Device.hpp"

NAMESPACE_BEGIN(rhi)

struct Texture
{
public:
    Device* device = nullptr;
    VkImage image = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;

    u16 width = 0;
    u16 height = 0;

    bool is_swapchain_buffer = false;

public:
    struct create_info
    {
        u16 width;
        u16 height;
        VkFormat format;
        VkImageUsageFlags flags;
    };

public:
    Texture(Device* device, VkImage image, const create_info& info);

    Texture(Device* device, Swapchain* swapchain);

    ~Texture();

    auto get_view() const noexcept -> VkImageView;

    auto get_size() const noexcept -> Rect2D;
};

NAMESPACE_END(rhi)
