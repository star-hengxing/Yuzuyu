#include <fast_io.h>

#include <resource/Image.hpp>
#include "Renderer.hpp"

NAMESPACE_BEGIN(render)

Renderer::Renderer(window::sdl_system* window)
{
    const auto callback = [&window](VkInstance instance) {
        return window->create_surface(instance);
    };

    if (!device.initialize(callback))
    {
        throw std::string_view{"Can't initialize device"};
    }

    if (!swapchain.initialize(&device, window->get_window_size(), false, {}))
    {
        throw std::string_view{"Can't initialize swap chain"};
    }

    const auto [width, height] = swapchain.extent;
    framebuffer = vk::Target
    {
        .rect = {width, height},
        .format = swapchain.format,
    };

    if (swapchain.images.size() != MAX_FRAME_SIZE)
    {
        perrln("Swap chain image size is ", swapchain.images.size());
        throw "";
    }

    command_pools[0] = vk::create_command_pool(device.handle, device.queue.get_index(vk::Queue::Family::graphics));
    command_pools[1] = vk::create_command_pool(device.handle, device.queue.get_index(vk::Queue::Family::transfer));

    present_semaphore = vk::Semaphore{device.handle};
    render_semaphore = vk::Semaphore{device.handle};
    for (auto&& i : frames)
    {
        auto&& [cmd, fence] = i;
        cmd = vk::Command{device.handle, command_pools[0]};
        fence = vk::Fence{device.handle};
    }

    VkDescriptorSetLayoutBinding layout_bindings[MAX_BINDING_SIZE]{};
    for (usize i = 0; auto&& j : layout_bindings)
    {
        j.descriptorType = vk::Descriptor::type;
        j.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        j.binding = i;
        // no array
        j.descriptorCount = 1;
        i += 1;
    }

    const auto pipeline_info = vk::Pipeline::create_info
    {
        .target = &framebuffer,
        .vertex = shader::vs,
        .fragment = shader::fs,
        .binding_size = std::size(layout_bindings),
        .bindings = layout_bindings,
        .constant =
        {
            .size = sizeof(payload),
            .ptr = &payload,
            .type = VK_SHADER_STAGE_FRAGMENT_BIT,
        },
        .alpha = true,
    };

    pipeline = vk::Pipeline{device.handle, pipeline_info};
    descriptor = vk::Descriptor{device.handle, pipeline.descriptor_set_layouts};
    pipeline.bind_descriptor(descriptor.descriptor_sets, 1);
}

auto Renderer::clean() noexcept -> void
{
    device.wait_idle();

    render_semaphore.clean();
    present_semaphore.clean();
    for (auto&& i : frames)
    {
        i.cmd.clean();
        i.fence.clean();
    }

    for (auto&& i : command_pools)
    {
        vkDestroyCommandPool(device.handle, i, VK_NULL_HANDLE);
        i = VK_NULL_HANDLE;
    }

    for (auto&& i : textures)
    {
        i.second.clean();
    }

    descriptor.clean();
    pipeline.clean();
    swapchain.clean();
    device.clean();
}

auto Renderer::record(const unsafe::buffer_view<draw_config> configs) noexcept -> void
{
    const auto [width, height] = swapchain.extent;

    auto bg = textures.find(background)->second;
    framebuffer.begin = VK_ATTACHMENT_LOAD_OP_LOAD;

    for (usize i = 0; auto&& frame : frames)
    {
        framebuffer.view = swapchain.image_views[i];
        frame.cmd.begin(0).blit(bg, vk::Texture{swapchain, i});

        for (auto&& cfg : configs)
        {
            payload = {};
            if (cfg.texture.empty())
            {
                payload.color = rgb_to_float<f32>(cfg.color);
            }
            else
            {
                this->bind_texture(cfg.texture, 0);
                payload.is_texture = 1;
                if (cfg.texture.starts_with("font"))
                {
                    payload.color = rgb_to_float<f32>(cfg.color);
                    payload.is_font = 1;
                }
            }
            payload.color.a = cfg.opacity;

            auto draw_x_size = cfg.x2 - cfg.x1;
            auto max_x_size = width - cfg.x1;
            if (draw_x_size > max_x_size)
            {
                draw_x_size = max_x_size;
            }

            auto draw_y_size = cfg.y2 - cfg.y1;
            auto max_y_size = height - cfg.y1;
            if (draw_y_size > max_y_size)
            {
                draw_y_size = max_y_size;
            }

            frame.cmd
                .begin_renderpass(framebuffer)
                .set_scissor(width, height)
                .set_viewport(draw_x_size, draw_y_size, cfg.x1, cfg.y1)
                .bind_pipeline(pipeline)
                .draw(3, 1, 0, 0)
                .end_renderpass();
        }

        frame.cmd.end();
        i += 1;
    }
}

auto Renderer::draw_frame() noexcept -> void
{
    swapchain.acquire_next(present_semaphore.handle, VK_NULL_HANDLE);
    auto&& [cmd, fence] = frames[swapchain.frame_index];

    if (swapchain.resized) [[unlikely]]
    {
        perrln("resized");
        swapchain.resize(window->resized(), true);
        return;
    }

    constexpr VkPipelineStageFlags mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const auto info = VkSubmitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &present_semaphore.handle,
        .pWaitDstStageMask = &mask,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd.command,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &render_semaphore.handle,
    };
    device.queue.submit_graphics(info, fence);
    swapchain.present(render_semaphore.handle);
}

auto Renderer::add_texture(const std::string_view filename) noexcept -> void
{
    if (textures.contains(filename))
        return;

    auto image = io::file::read_to_image(filename);
    if (!image)
    {
        perrln(image.error());
        return;
    }

    auto view = image->get_view();
    auto size = view.size() * 4;
    auto tmp_buffer = vk::Buffer{&device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU};
    tmp_buffer.cpu_to_gpu(&view.ptr->r, size, 0);

    auto info = vk::Texture::create_info
    {
        .width = view.width,
        .height = view.height,
        .flags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    };
    auto texture = vk::Texture{&device, info};

    auto transfer_cmd = vk::Command{device.handle, command_pools[1]};
    transfer_cmd.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    transfer_cmd.copy_buffer_to_texture(tmp_buffer, texture);
    transfer_cmd.end();
    device.queue.submit_transfer(transfer_cmd.command);

    textures.emplace(filename, texture);
    tmp_buffer.clean();
    transfer_cmd.clean();
    perrln("Add texture: ", filename);
}

auto Renderer::add_font(const std::string_view name, const Image& image) noexcept -> void
{
    auto view = image.get_view();
    auto size = view.size() * 4;
    auto tmp_buffer = vk::Buffer{&device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU};
    tmp_buffer.cpu_to_gpu(&view.ptr->r, size, 0);

    auto info = vk::Texture::create_info
    {
        .width = view.width,
        .height = view.height,
        .flags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    };
    auto texture = vk::Texture{&device, info};

    auto transfer_cmd = vk::Command{device.handle, command_pools[1]};
    transfer_cmd.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    transfer_cmd.copy_buffer_to_texture(tmp_buffer, texture);
    transfer_cmd.end();
    device.queue.submit_transfer(transfer_cmd.command);

    textures.emplace(name, texture);
    tmp_buffer.clean();
    transfer_cmd.clean();
    perrln("Add font texture: ", name);
}

auto Renderer::bind_texture(const std::string_view filename, u32 binding) noexcept -> void
{
    if (binding >= MAX_BINDING_SIZE)
        return;

    auto&& value = textures.find(filename);
    if (value != textures.end())
    {
        descriptor.bind_texture(binding, value->second);
    }
    else
    {
        perrln(filename, " not found");
    }
}

auto Renderer::set_background(const std::string_view filename) noexcept -> void
{
    if (!background.empty())
    {
        auto old = textures.find(background);
        old->second.clean();
        textures.erase(old);
    }

    add_texture(filename);
    background = textures.find(filename)->first;
}

NAMESPACE_END(render)
