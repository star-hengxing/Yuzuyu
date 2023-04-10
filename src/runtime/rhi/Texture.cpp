#include "Texture.hpp"
#include "base.hpp"

NAMESPACE_BEGIN(rhi)

Texture::Texture(Device* device, VkImage image, const create_info& info)
    : width{info.width}, height{info.height}
{
    const auto image_info = VkImageCreateInfo
    {
        .sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType     = VK_IMAGE_TYPE_2D,
        .format        = info.format,
        .extent        = VkExtent3D{info.width, info.height, 0},
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
        width{static_cast<u16>(swapchain->extent.width)},
        height{static_cast<u16>(swapchain->extent.height)},
        is_swapchain_buffer{true} {}

Texture::~Texture()
{
    if (image && !is_swapchain_buffer)
    {
        vmaDestroyImage(device->allocator, image, allocation);
    }
}

auto Texture::get_size() const noexcept -> Rect2D
{
    return {width, height};
}

auto Texture::get_view() const noexcept -> VkImageView
{
    return {};
}

NAMESPACE_END(rhi)
