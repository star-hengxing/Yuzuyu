#pragma once

#include "Pipeline.hpp"
#include "Texture.hpp"
#include "Buffer.hpp"
#include "Target.hpp"

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

    auto begin_renderpass(const Target& target) noexcept -> void;
    auto end_renderpass() noexcept -> void;

    auto bind_vertex_buffer(u32 binding, const Buffer& buffer) noexcept -> void;
    auto bind_index_buffer(const Buffer& buffer, bool is_u16 = false) noexcept -> void;

    auto bind_pipeline(const Pipeline& pipeline) noexcept -> void;
    auto bind_descriptor() noexcept -> void;

    auto set_viewport(f32 width, f32 height, f32 x = 0, f32 y = 0) noexcept -> void;
    auto set_scissor(u32 width, u32 height, i32 offset_x = 0, i32 offset_y = 0) noexcept -> void;

    auto draw(u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance) noexcept -> void;
    auto draw_indirect(const Buffer& buffer, size_t offset, u32 draw_count, u32 stride) noexcept -> void;
    auto draw_indirect_count(const Buffer& buffer, size_t offset, const Buffer& count_buffer, size_t count_buffer_offset, u32 max_draw_count, u32 stride) noexcept -> void;   
    auto draw_indexed(u32 index_count, u32 instance_count, u32 first_index, u32 vertex_offset, u32 first_instance) noexcept -> void;
    auto draw_indexed_indirect(const Buffer& buffer, size_t offset, u32 draw_count, u32 stride) noexcept -> void;
    auto draw_indexed_indirect_count(const Buffer& buffer, size_t offset, const Buffer& count_buffer, size_t count_buffer_offset, u32 max_draw_count, u32 stride) noexcept -> void;

    auto copy_buffer_to_texture(const Buffer& src, const Texture& dst) noexcept -> void;
    auto copy_texture_to_buffer(const Texture& src, const Buffer& dst) noexcept -> void;
    auto copy_buffer_to_buffer(const Buffer& src, const Buffer& dst,
        usize size, usize src_offset, usize dst_offset) noexcept -> void;

    auto convert_buffer_to_image(const Buffer& src, const Texture& dst) noexcept -> void;

    auto blit(const Texture& src, const Texture& dst) noexcept -> void;
};

NAMESPACE_END(rhi)
