#pragma once

#include "Texture.hpp"
#include "Buffer.hpp"

NAMESPACE_BEGIN(rhi)

struct Command
{
public:
    VkDevice device;
    VkCommandBuffer command = VK_NULL_HANDLE;
    VkCommandPool pool = VK_NULL_HANDLE;

public:
    Command(VkDevice device, u32 queue_family);

    ~Command();

    auto begin(VkCommandBufferUsageFlags flags) noexcept -> void;
    auto end() noexcept -> void;

    auto begin_renderpass() noexcept -> void;
    auto end_renderpass() noexcept -> void;

    auto set_viewport(f32 width, f32 height, f32 x = 0, f32 y = 0) noexcept -> void;
    auto set_scissor(u32 width, u32 height, i32 offset_x = 0, i32 offset_y = 0) noexcept -> void;

    auto copy_buffer_to_texture(Buffer* src, Texture* dst) noexcept -> void;
    auto copy_texture_to_buffer(Texture* src, Buffer* dst) noexcept -> void;
    auto copy_buffer_to_buffer(Buffer* src, Buffer* dst,
        usize size, usize src_offset, usize dst_offset) noexcept -> void;
};

NAMESPACE_END(rhi)
