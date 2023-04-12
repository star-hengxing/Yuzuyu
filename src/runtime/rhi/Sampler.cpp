#include "Sampler.hpp"

NAMESPACE_BEGIN(rhi)

Sampler::Sampler(VkDevice device) : device{device}
{
    const auto info = VkSamplerCreateInfo
    {
        .sType        = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter    = VK_FILTER_LINEAR,
        .minFilter    = VK_FILTER_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    };
    CHECK_RESULT(vkCreateSampler(device, &info, VK_NULL_HANDLE, &handle));
}

Sampler::~Sampler()
{
    if (handle)
    {
        vkDestroySampler(device, handle, VK_NULL_HANDLE);
    }
}

NAMESPACE_END(rhi)
