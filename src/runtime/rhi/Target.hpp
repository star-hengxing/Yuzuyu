#pragma once

#include "Device.hpp"

NAMESPACE_BEGIN(rhi)

struct Target
{
public:
    std::vector<VkFormat> formats;
    std::vector<VkRenderingAttachmentInfo> color_attachments;
};

NAMESPACE_END(rhi)
