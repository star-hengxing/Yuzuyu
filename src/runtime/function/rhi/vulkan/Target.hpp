#pragma once

#include <core/base/Rect2D.hpp>
#include <core/base/Color.hpp>
#include "Texture.hpp"

NAMESPACE_BEGIN(rhi::vulkan)

struct Target
{
public:
    Rect2D rect              = {0, 0};
    VkFormat format          = VK_FORMAT_R8G8B8A8_SRGB;
    VkImageView view         = VK_NULL_HANDLE;
    VkAttachmentLoadOp begin = VK_ATTACHMENT_LOAD_OP_CLEAR;
    VkAttachmentStoreOp end  = VK_ATTACHMENT_STORE_OP_STORE;
    RGBA<f32> clear_color    = {0, 0, 0, 0};

public:
    auto set(const Texture& texture) noexcept -> void
    {
        rect = texture.get_size();
        view = texture.view;
    }
};

NAMESPACE_END(rhi::vulkan)
