#pragma once

#include "Texture.hpp"
#include "Sampler.hpp"

NAMESPACE_BEGIN(rhi)

inline constexpr auto MAX_DESCRIPTOR_SIZE = usize{128};

struct Descriptor
{
private:
    using Self = Descriptor;

public:
    static constexpr auto type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

private:
    VkDevice device = VK_NULL_HANDLE;

    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;

    VkSampler sampler = VK_NULL_HANDLE;

public:
    VkDescriptorSet descriptor_sets[1]{};

public:
    Descriptor() {}

    Descriptor(VkDevice device, VkDescriptorSetLayout* layouts);

    auto clean() noexcept -> void;

    auto bind_texture(u32 binding, const Texture& texture) noexcept -> void;
};

NAMESPACE_END(rhi)
