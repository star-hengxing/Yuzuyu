#include "Texture.hpp"
#include "base.hpp"

NAMESPACE_BEGIN(rhi)

Texture::Texture(Device* device, const create_info& info) : info{info}
{
    const auto image_info = VkImageCreateInfo
    {
        .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType     = VK_IMAGE_TYPE_2D,
        .format        = VK_FORMAT_R8G8B8A8_SRGB,
        .extent        = VkExtent3D{info.width, info.height, 1},
        .mipLevels     = 1,
        .arrayLayers   = 1,
        .samples       = VK_SAMPLE_COUNT_1_BIT,
        .tiling        = VK_IMAGE_TILING_OPTIMAL,
        .usage         = info.flags,
        .sharingMode   = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    const auto vma_info = VmaAllocationCreateInfo
    {
        .usage         = VMA_MEMORY_USAGE_GPU_ONLY,
        .requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
    };

    CHECK_RESULT(vmaCreateImage(
        device->allocator,
        &image_info, &vma_info,
        &image, &allocation, VK_NULL_HANDLE));
}

Texture::Texture(Device* device, Swapchain* swapchain)
    :   device{device}, image{swapchain->images[swapchain->frame_index]},
        is_swapchain_buffer{true}
{
    info.width  = static_cast<u32>(swapchain->extent.width);
    info.height = static_cast<u32>(swapchain->extent.height);
}

Texture::~Texture()
{
    if (image && !is_swapchain_buffer)
    {
        vmaDestroyImage(device->allocator, image, allocation);
    }
}

auto Texture::get_size() const noexcept -> Rect2D
{
    return {info.width, info.height};
}

auto Texture::get_view() const noexcept -> VkImageView
{
    const auto subresource = VkImageSubresourceRange
    {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1,
    };

    const auto view_info = VkImageViewCreateInfo
    {
        .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image      = image,
        .viewType   = VK_IMAGE_VIEW_TYPE_2D,
        .format     = VK_FORMAT_R8G8B8A8_SRGB,
        .components =
        {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = subresource,
    };

    VkImageView view;
    const auto result = vkCreateImageView(device->handle, &view_info, nullptr, &view);
    CHECK_RESULT(result);
    return (result == VK_SUCCESS) ? view : VK_NULL_HANDLE;
}

NAMESPACE_END(rhi)
