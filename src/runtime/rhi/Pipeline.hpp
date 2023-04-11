#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include "Target.hpp"

NAMESPACE_BEGIN(rhi)

struct Pipeline
{
public:
    struct create_info
    {
        Target* target;
        std::string_view vertex, fragment;
    };

public:
    VkDevice device = VK_NULL_HANDLE;
    VkPipeline handle = VK_NULL_HANDLE;
    VkPipelineLayout layout = VK_NULL_HANDLE;

public:
    Pipeline() {}

    ~Pipeline();

    Pipeline(VkDevice device, const create_info& info);
};

NAMESPACE_END(rhi)
