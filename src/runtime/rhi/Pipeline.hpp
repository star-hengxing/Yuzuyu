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
        u32 binding_size = 0;
        VkDescriptorSetLayoutBinding* bindings;
        push_constants constant;
    };

private:
    VkDevice device = VK_NULL_HANDLE;

public:
    VkPipeline handle = VK_NULL_HANDLE;
    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptor_set_layouts[4] = {};

    push_constants constant;

    u32 descriptor_set_size = 0;
    VkDescriptorSet* descriptor_sets = VK_NULL_HANDLE;

public:
    Pipeline() {}

    ~Pipeline();

    Pipeline(VkDevice device, const create_info& info);

    auto bind_descriptor(VkDescriptorSet* sets, u32 size) noexcept -> void
    {
        descriptor_sets = sets;
        descriptor_set_size = size;
    }
};

NAMESPACE_END(rhi)
