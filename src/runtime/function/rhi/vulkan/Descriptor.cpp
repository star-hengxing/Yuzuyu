#include "Descriptor.hpp"

NAMESPACE_BEGIN()

using namespace rhi::vulkan;

auto create_descriptor_pool(VkDevice device) noexcept -> VkDescriptorPool
{
    // auto properties = VkPhysicalDeviceProperties{};
    // vkGetPhysicalDeviceProperties(device->gpu, &properties);

    const VkDescriptorPoolSize pool_sizes[]
    {
        // {VK_DESCRIPTOR_TYPE_SAMPLER,        properties.limits.maxDescriptorSetSamplers},
        // {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,  properties.limits.maxDescriptorSetSampledImages},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_DESCRIPTOR_SIZE},
        // {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,  properties.limits.maxDescriptorSetStorageImages},
        // {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, properties.limits.maxDescriptorSetUniformBuffers},
        // {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, properties.limits.maxDescriptorSetStorageBuffers},
    };

    const auto info = VkDescriptorPoolCreateInfo
    {
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT | VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
        .maxSets       = MAX_DESCRIPTOR_SIZE,
        .poolSizeCount = std::size(pool_sizes),
        .pPoolSizes    = pool_sizes,
    };

    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    CHECK_RESULT(vkCreateDescriptorPool(device, &info, VK_NULL_HANDLE, &descriptor_pool));
    return descriptor_pool;
}

NAMESPACE_END()

NAMESPACE_BEGIN(rhi::vulkan)

auto Descriptor::clean() noexcept -> void
{
    if (sampler)
    {
        vkDestroySampler(device, sampler, VK_NULL_HANDLE);
        sampler = VK_NULL_HANDLE;
    }

    if (descriptor_pool)
    {
        vkDestroyDescriptorPool(device, descriptor_pool, VK_NULL_HANDLE);
        descriptor_pool = VK_NULL_HANDLE;
    }
}

Descriptor::Descriptor(VkDevice device, VkDescriptorSetLayout* layouts)
    :   device{device}, descriptor_pool{create_descriptor_pool(device)}
{
    const auto info = VkDescriptorSetAllocateInfo
    {
        .sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool     = descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts        = layouts,
    };

    CHECK_RESULT(vkAllocateDescriptorSets(device, &info, descriptor_sets));

    const auto sampler_info = VkSamplerCreateInfo
    {
        .sType        = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter    = VK_FILTER_LINEAR,
        .minFilter    = VK_FILTER_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    };
    CHECK_RESULT(vkCreateSampler(device, &sampler_info, VK_NULL_HANDLE, &sampler));
}

auto Descriptor::bind_texture(u32 binding, const Texture& texture) noexcept -> void
{
    const auto image_info = VkDescriptorImageInfo
    {
        .sampler     = sampler,
        .imageView   = texture.view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    const auto update_sets = VkWriteDescriptorSet
    {
        .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet          = descriptor_sets[0],
        .dstBinding      = binding,
        .descriptorCount = 1,
        .descriptorType  = type,
        .pImageInfo      = &image_info,
    };
    vkUpdateDescriptorSets(device, 1, &update_sets, 0, VK_NULL_HANDLE);
}

NAMESPACE_END(rhi::vulkan)
