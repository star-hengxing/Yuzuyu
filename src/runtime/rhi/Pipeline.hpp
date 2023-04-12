#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include "Target.hpp"

NAMESPACE_BEGIN(rhi)

struct push_constants
{
    u32 size = 0;
    void* ptr = nullptr;
    VkShaderStageFlags type = VK_SHADER_STAGE_VERTEX_BIT;

    auto empty() const noexcept -> bool
    {
        return size == 0 || !ptr;
    }
};

struct Pipeline
{
public:
    struct create_info
    {
        Target* target = nullptr;
        std::string_view vertex, fragment;
        push_constants constant;
    };

public:
    VkDevice device = VK_NULL_HANDLE;
    VkPipeline handle = VK_NULL_HANDLE;
    VkPipelineLayout layout = VK_NULL_HANDLE;
    push_constants constant;

public:
    Pipeline() {}

    ~Pipeline();

    Pipeline(VkDevice device, const create_info& info);
};

NAMESPACE_END(rhi)
