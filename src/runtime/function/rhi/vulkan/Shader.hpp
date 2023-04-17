#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include "base.hpp"

NAMESPACE_BEGIN(rhi::vulkan)

struct Shader
{
public:
    VkDevice device = VK_NULL_HANDLE;
    VkShaderModule handle = VK_NULL_HANDLE;

public:
    Shader() {}

    Shader(VkDevice device, const std::string_view file);

    ~Shader();
};

NAMESPACE_END(rhi::vulkan)
