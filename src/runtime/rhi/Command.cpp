#include "Command.hpp"
#include "base.hpp"

NAMESPACE_BEGIN(rhi)

Command::Command(VkDevice device, u32 queue_family)
{
    {
        const auto info = VkCommandPoolCreateInfo
        {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queue_family,
        };
        CHECK_RESULT(vkCreateCommandPool(device, &info, nullptr, &pool));
    }

    const auto info = VkCommandBufferAllocateInfo
    {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool        = pool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    CHECK_RESULT(vkAllocateCommandBuffers(device, &info, &command));
}

Command::~Command()
{
    if (command)
    {
        vkFreeCommandBuffers(device, pool, 1, &command);
    }

    if (pool)
    {
        vkDestroyCommandPool(device, pool, VK_NULL_HANDLE);
    }
}

auto Command::begin(VkCommandBufferUsageFlags flags) noexcept -> void
{
    const auto info = VkCommandBufferBeginInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = flags,
    };
    CHECK_RESULT(vkBeginCommandBuffer(command, &info));
}

auto Command::end() noexcept -> void
{
    CHECK_RESULT(vkEndCommandBuffer(command));
}

auto Command::begin_renderpass() noexcept -> void
{
    const auto info = VkRenderingInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
    };

    vkCmdBeginRendering(command, &info);
}

auto Command::end_renderpass() noexcept -> void
{
    vkCmdEndRendering(command);
}

auto Command::set_viewport(f32 width, f32 height, f32 x, f32 y) noexcept -> void
{
    const auto viewport = VkViewport
    {
        .x        = x,
        .y        = y,
        .width    = width,
        .height   = height,
        .minDepth = 0,
        .maxDepth = 1,
    };
    vkCmdSetViewport(command, 0, 1, &viewport);
}

auto Command::set_scissor(u32 width, u32 height, i32 offset_x, i32 offset_y) noexcept -> void
{
    const auto rect = VkRect2D{VkOffset2D{offset_x, offset_y}, VkExtent2D{width, height}};
    vkCmdSetScissor(command, 0, 1, &rect);
}

auto Command::copy_buffer_to_texture(Buffer* src, Texture* dst) noexcept -> void
{
    const auto subresource = VkImageSubresourceLayers
    {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .mipLevel       = 0,
        .baseArrayLayer = 0,
        .layerCount     = 1,
    };

    const auto [width, height] = dst->get_size();

    const auto copy_info = VkBufferImageCopy
    {
        .imageSubresource = subresource,
        .imageExtent      = {width, height, 1},
    };

    vkCmdCopyBufferToImage(command, src->buffer, dst->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_info);
}

auto Command::copy_texture_to_buffer(Texture* src, Buffer* dst) noexcept -> void
{
    const auto subresource = VkImageSubresourceLayers
    {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .mipLevel       = 0,
        .baseArrayLayer = 0,
        .layerCount     = 1,
    };

    const auto [width, height] = src->get_size();

    const auto copy_info = VkBufferImageCopy
    {
        .imageSubresource = subresource,
        .imageExtent      = {width, height, 1},
    };

    vkCmdCopyImageToBuffer(command, src->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst->buffer, 1, &copy_info);
}

auto Command::copy_buffer_to_buffer(Buffer* src, Buffer* dst,
    usize size, usize src_offset, usize dst_offset) noexcept -> void
{
    const auto copy_info = VkBufferCopy
    {
        .srcOffset = src_offset,
        .dstOffset = dst_offset,
        .size      = size,
    };

    vkCmdCopyBuffer(command, src->buffer, dst->buffer, 1, &copy_info);
}

NAMESPACE_END(rhi)
