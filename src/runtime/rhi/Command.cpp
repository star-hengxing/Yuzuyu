#include "Command.hpp"
#include "base.hpp"

NAMESPACE_BEGIN(rhi)

Command::Command(VkDevice device, u32 queue_family) : device{device}
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

auto Command::begin_renderpass(const Target& target) noexcept -> void
{
    const auto value = VkClearColorValue
    {
        {
            target.clear_color.r,
            target.clear_color.g,
            target.clear_color.b,
            target.clear_color.a,
        }
    };

    const auto attachment = VkRenderingAttachmentInfo
    {
        .sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView   = target.view,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp      = target.begin,
        .storeOp     = target.end,
        .clearValue  = {.color = value},
    };

    const auto extent = VkExtent2D{target.rect.width, target.rect.height};
    const auto info = VkRenderingInfo
    {
        .sType                = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea           = {.offset = {}, .extent = extent},
        .layerCount           = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments    = &attachment,
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

auto Command::bind_vertex_buffer(u32 binding, const Buffer& buffer) noexcept -> void
{
    auto offset = usize{};
    vkCmdBindVertexBuffers(command, binding, 1, &buffer.handle, &offset);
}

auto Command::bind_index_buffer(const Buffer& buffer, bool is_u16) noexcept -> void
{
    vkCmdBindIndexBuffer(command, buffer.handle, 0, is_u16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
}

auto Command::bind_pipeline(const Pipeline& pipeline) noexcept -> void
{
    vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.handle);
}

auto Command::draw(u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance) noexcept
    -> void
{
    vkCmdDraw(command, vertex_count, instance_count, first_vertex, first_instance);
}

auto Command::draw_indirect(const Buffer& buffer, size_t offset, u32 draw_count, u32 stride) noexcept -> void
{
    vkCmdDrawIndirect(command, buffer.handle, offset, draw_count, stride);
}

auto Command::draw_indirect_count(const Buffer& buffer, size_t offset, const Buffer& count_buffer, size_t count_buffer_offset, u32 max_draw_count, u32 stride) noexcept -> void
{
    vkCmdDrawIndirectCount(command, buffer.handle, offset, count_buffer.handle, count_buffer_offset, max_draw_count, stride);
}

auto Command::draw_indexed(u32 index_count, u32 instance_count, u32 first_index, u32 vertex_offset, u32 first_instance) noexcept -> void
{
    vkCmdDrawIndexed(command, index_count, instance_count, first_index, vertex_offset, first_instance);
}

auto Command::draw_indexed_indirect(const Buffer& buffer, size_t offset, u32 draw_count, u32 stride) noexcept -> void
{
    vkCmdDrawIndexedIndirect(command, buffer.handle, offset, draw_count, stride);
}

auto Command::draw_indexed_indirect_count(const Buffer& buffer, size_t offset, const Buffer& count_buffer, size_t count_buffer_offset, u32 max_draw_count, u32 stride) noexcept -> void
{
    vkCmdDrawIndexedIndirectCount(command, buffer.handle, offset, count_buffer.handle, count_buffer_offset, max_draw_count, stride);
}

auto Command::copy_buffer_to_texture(const Buffer& src, const Texture& dst) noexcept -> void
{
    const auto subresource = VkImageSubresourceLayers
    {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .mipLevel       = 0,
        .baseArrayLayer = 0,
        .layerCount     = 1,
    };

    const auto [width, height] = dst.get_size();

    const auto copy_info = VkBufferImageCopy
    {
        .imageSubresource = subresource,
        .imageExtent      = {width, height, 1},
    };

    vkCmdCopyBufferToImage(command, src.handle, dst.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_info);
}

auto Command::copy_texture_to_buffer(const Texture& src, const Buffer& dst) noexcept -> void
{
    const auto subresource = VkImageSubresourceLayers
    {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .mipLevel       = 0,
        .baseArrayLayer = 0,
        .layerCount     = 1,
    };

    const auto [width, height] = src.get_size();

    const auto copy_info = VkBufferImageCopy
    {
        .imageSubresource = subresource,
        .imageExtent      = {width, height, 1},
    };

    vkCmdCopyImageToBuffer(command, src.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst.handle, 1, &copy_info);
}

auto Command::copy_buffer_to_buffer(const Buffer& src, const Buffer& dst,
    usize size, usize src_offset, usize dst_offset) noexcept -> void
{
    const auto copy_info = VkBufferCopy
    {
        .srcOffset = src_offset,
        .dstOffset = dst_offset,
        .size      = size,
    };

    vkCmdCopyBuffer(command, src.handle, dst.handle, 1, &copy_info);
}

auto Command::convert_buffer_to_image(const Buffer& src, const Texture& dst) noexcept -> void
{
    const auto subresource = VkImageSubresourceRange
    {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1,
    };

    const auto src_barrier = VkImageMemoryBarrier
    {
        .sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask    = 0,
        .dstAccessMask    = VK_ACCESS_TRANSFER_WRITE_BIT,
        .oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .image            = dst.image,
        .subresourceRange = subresource,
    };

    const auto dst_barrier = VkImageMemoryBarrier
    {
        .sType            = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask    = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstAccessMask    = VK_ACCESS_SHADER_READ_BIT,
        .oldLayout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .newLayout        = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .image            = dst.image,
        .subresourceRange = subresource,
    };

    vkCmdPipelineBarrier(
        command,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &src_barrier);

    copy_buffer_to_texture(src, dst);

    vkCmdPipelineBarrier(
        command,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &dst_barrier);
}

auto Command::blit(const Texture& src, const Texture& dst) noexcept -> void
{
    const auto subresource = VkImageSubresourceLayers
    {
        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
        .mipLevel       = 0,
        .baseArrayLayer = 0,
        .layerCount     = 1,
    };

    const auto region = VkImageBlit
    {
        .srcSubresource = subresource,
        .srcOffsets =
        {
            {},
            {static_cast<int>(src.info.width), static_cast<int>(src.info.height), 1},
        },
        .dstSubresource = subresource,
        .dstOffsets =
        {
            {},
            {static_cast<int>(dst.info.width), static_cast<int>(dst.info.height), 1},
        },
    };

    vkCmdBlitImage(
        command,
        src.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dst.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1, &region,
        VK_FILTER_LINEAR);
}

NAMESPACE_END(rhi)
