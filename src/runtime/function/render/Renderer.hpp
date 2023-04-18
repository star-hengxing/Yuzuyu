#pragma once

#ifdef USE_MODULES
import <vector>;
import <string>;
import <map>;
#else
#include <vector>
#include <string>
#include <map>
#endif

#include <core/base/unsafe/buffer_view.hpp>
#include <function/window/sdl.hpp>
#include <function/shader/precompile.hpp>
#include <resource/Image.hpp>

#include <function/rhi/vulkan/Device.hpp>
#include <function/rhi/vulkan/Swapchain.hpp>
#include <function/rhi/vulkan/Synchronization.hpp>
#include <function/rhi/vulkan/Command.hpp>
#include <function/rhi/vulkan/Buffer.hpp>
#include <function/rhi/vulkan/Texture.hpp>
#include <function/rhi/vulkan/Pipeline.hpp>
#include <function/rhi/vulkan/Descriptor.hpp>

#include "base.hpp"

NAMESPACE_BEGIN(render)

namespace vk = rhi::vulkan;

struct Frame
{
    vk::Command cmd;
    vk::Fence fence;
};

struct render_payload
{
    RGBA<f32> color = rgb_to_float<f32>(color_space::black);
    u32 is_font = 0;
    u32 is_texture = 0;
};

struct Renderer
{
private:
    static constexpr auto MAX_FRAME_SIZE = usize{3};
    static constexpr auto MAX_BINDING_SIZE = usize{4};

private:
    window::sdl_system* window;

    vk::Device device;
    vk::Swapchain swapchain;
    // 1 -> graphics, 2 -> transfer
    VkCommandPool command_pools[2];

    vk::Semaphore present_semaphore;
    vk::Semaphore render_semaphore;
    Frame frames[MAX_FRAME_SIZE];

    vk::Target framebuffer;
    vk::Pipeline pipeline;
    vk::Descriptor descriptor;
    // heterogeneous lookup
    std::map<std::string, vk::Texture, std::less<>> textures;

    render_payload payload;
    std::string_view background;

public:
    Renderer(window::sdl_system* window);

    auto clean() noexcept -> void;

    auto draw_frame() noexcept -> void;

    auto add_texture(const std::string_view filename) noexcept -> void;

    auto add_font(const std::string_view name, const Image& image) noexcept -> void;

    auto bind_texture(const std::string_view filename, u32 binding) noexcept -> void;

    auto record(const unsafe::buffer_view<draw_config> configs) noexcept -> void;

    auto set_background(const std::string_view filename) noexcept -> void;
};

NAMESPACE_END(render)
