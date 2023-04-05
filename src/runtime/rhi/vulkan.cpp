#ifdef USE_MODULES
import <limits>;
#else
#include <limits>
#endif

#include <volk.h>
#include <VkBootstrap.h>

#include <runtime/shader/precompile.hpp>
#include <runtime/base/range.hpp>
#include "shader_module.hpp"
#include "vk_create_info.hpp"
#include "vulkan.hpp"
#include "base.hpp"

// TODO: more friendly handle error

NAMESPACE_BEGIN(runtime)
NAMESPACE_BEGIN(rhi)

auto vulkan::initialize(window_system* window) -> const char*
{
    if (!window)
    {
        return "window_system* window is nullptr";
    }

    this->window = window;
    create_instance_and_device();

    const auto [width, height] = window->get_window_size();
    create_swapchain(width, height);
    create_renderpass();
    create_framebuffers();

    create_command_pool();
    create_command_buffer();

    // create_pipeline();

    return nullptr;
}

auto vulkan::create_command_pool() -> void
{
    auto info = vk_create_info::command_pool();
    info.queueFamilyIndex = graphics_queue_family;
    info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkResult result = vkCreateCommandPool(device, &info, nullptr, &command_pool);
    CHECK_RESULT(result);
}

auto vulkan::create_command_buffer() -> void
{
    const auto size = swapchain.images.size();
    commands.resize(size);

    const auto info = vk_create_info::command_buffer_allocate(
        command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, size);

    VkResult result = vkAllocateCommandBuffers(device, &info, commands.data());
    CHECK_RESULT(result);
}

auto vulkan::create_renderpass() -> void
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = swapchain.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    auto info = vk_create_info::renderpass();

    info.attachmentCount = 1;
    info.pAttachments = &color_attachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;

    VkResult result = vkCreateRenderPass(device, &info, nullptr, &renderpass);
    CHECK_RESULT(result);
}

auto vulkan::create_framebuffers() -> void
{
    const auto size = swapchain.image_views.size();
    framebuffers.resize(size);

    VkImageView attachment;
    auto info = vk_create_info::framebuffer();

    info.renderPass = renderpass;
    info.attachmentCount = 1;
    info.pAttachments = &attachment;
    info.width = extent.width;
    info.height = extent.height;
    info.layers = 1;

    for (auto i : range(size))
    {
        attachment = swapchain.image_views[i];
        VkResult result = vkCreateFramebuffer(device, &info, nullptr, &framebuffers[i]);
        CHECK_RESULT(result);
    }
}

auto vulkan::create_pipeline() -> void
{
    const auto input_assembly = vk_create_info::pipeline_input_assembly_state(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

    const auto rasterization = vk_create_info::pipeline_rasterization_state(
        VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);

    const auto color_blend_attachment = vk_create_info::pipeline_color_blend_attachment_state(
        0b1111, VK_FALSE);

    const auto color_blend_state = vk_create_info::pipeline_color_blend_state(
        1, &color_blend_attachment);

    const auto depth_stencil = vk_create_info::pipeline_depth_stencil_state();
    const auto viewport_state = vk_create_info::pipeline_viewport_state(
        1, 1, 0);

    const auto multisample = vk_create_info::pipeline_multisample_state(
        VK_SAMPLE_COUNT_1_BIT);

    const VkDynamicState dynamic_states[2]{
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};
    const auto dynamic_state = vk_create_info::pipeline_dynamic_state(dynamic_states, 2);

    auto pipeline_info = vk_create_info::pipeline();
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterization;
    pipeline_info.pMultisampleState = &multisample;
    pipeline_info.pDepthStencilState = &depth_stencil;
    pipeline_info.pColorBlendState = &color_blend_state;
    pipeline_info.pDynamicState = &dynamic_state;
    pipeline_info.subpass = 0;

    VkResult result;
    auto vs_shader_code = shader_module{device};
    auto fs_shader_code = shader_module{device};

    result = vs_shader_code.create(shader::vs);
    CHECK_RESULT(result);
    result = fs_shader_code.create(shader::fs);
    CHECK_RESULT(result);

    const VkPipelineShaderStageCreateInfo shader_stages[2]{
        vk_create_info::pipeline_shader_state(VK_SHADER_STAGE_VERTEX_BIT, vs_shader_code.get()),
        vk_create_info::pipeline_shader_state(VK_SHADER_STAGE_FRAGMENT_BIT, fs_shader_code.get())};

    auto vertex_input = vk_create_info::pipeline_vertex_input_state();
    vertex_input.vertexBindingDescriptionCount = 0;
    vertex_input.pVertexBindingDescriptions = nullptr; // Optional
    vertex_input.vertexAttributeDescriptionCount = 0;
    vertex_input.pVertexAttributeDescriptions = nullptr; // Optional

    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;

    VkPipeline pipeline;
    result = vkCreateGraphicsPipelines(
        device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline);
    CHECK_RESULT(result);
}

auto vulkan::create_sync() -> void
{
    const auto size = swapchain.image_views.size();
    frames.resize(size);

    VkResult result;
    const auto semaphore_info = vk_create_info::semaphore();
    const auto fence_info = vk_create_info::fence(VK_FENCE_CREATE_SIGNALED_BIT);

    for (auto i : range(size))
    {
        result = vkCreateSemaphore(device, &semaphore_info, nullptr, &frames[i].semaphores.signal);
        CHECK_RESULT(result);
        result = vkCreateSemaphore(device, &semaphore_info, nullptr, &frames[i].semaphores.wait);
        CHECK_RESULT(result);
        result = vkCreateFence(device, &fence_info, nullptr, &frames[i].fence);
        CHECK_RESULT(result);
    }
}

auto vulkan::window_resize() -> void
{
    const auto [width, height] = window->resized();

    // Ensure all operations on the device have been finished before destroying resources
    vkDeviceWaitIdle(device);
    for (auto i : range(framebuffers.size()))
    {
        vkDestroyFramebuffer(device, framebuffers[i], nullptr);
        vkDestroyImageView(device, swapchain.image_views[i], nullptr);
    }
    vkDestroySwapchainKHR(device, swapchain.self, nullptr);

    create_swapchain(width, height);
    create_framebuffers();
}

auto vulkan::acquire_frame(VkSemaphore present, unsigned* index) -> void
{
    VkResult result;
    static constexpr auto MAX_NUMBER = std::numeric_limits<uint64_t>::max();

    result = vkAcquireNextImageKHR(
        device, swapchain.self, MAX_NUMBER, present, VK_NULL_HANDLE, index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            window_resize();
        }
        return;
    }
    else
    {
        CHECK_RESULT(result);
    }
}

auto submit_frame(VkQueue queue, uint32_t index, VkSemaphore wait)
    -> VkResult
{
    VkPipelineStageFlags mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // The submit info structure specifies a command buffer queue submission batch
    auto info = vk_create_info::submit(0, nullptr);
    info.pWaitDstStageMask = &mask;
    // info.pWaitSemaphores      = &present;
    // info.waitSemaphoreCount   = 1;
    // info.pSignalSemaphores    = &complete;
    // info.signalSemaphoreCount = 1;
    // info.pCommandBuffers      = &command;
    // info.commandBufferCount   = 1;

    return vkQueueSubmit(queue, 1, &info, nullptr);
}

auto vulkan::present_frame(VkQueue queue, uint32_t index, VkSemaphore wait)
    -> VkResult
{
    auto info = vk_create_info::present();
    info.pNext = nullptr;
    info.swapchainCount = 1;
    info.pSwapchains = &swapchain.self;
    info.pImageIndices = &index;
    // Check if a wait semaphore has been specified to wait for before presenting the image
    if (wait != VK_NULL_HANDLE)
    {
        info.pWaitSemaphores = &wait;
        info.waitSemaphoreCount = 1;
    }
    return vkQueuePresentKHR(queue, &info);
}

auto vulkan::record_command(VkCommandBuffer command, VkFramebuffer framebuffer) -> void
{
    VkResult result;
    auto command_buffer_begin = vk_create_info::command_buffer_begin();

    VkClearValue clear_value{};
    clear_value.color = {{0.0f, 0.0f, 0.0f, 0.7f}};

    auto renderpass_begin = vk_create_info::renderpass_begin();
    renderpass_begin.renderPass = renderpass;
    renderpass_begin.renderArea.offset.x = 0;
    renderpass_begin.renderArea.offset.y = 0;
    renderpass_begin.renderArea.extent = extent;
    renderpass_begin.clearValueCount = 1;
    renderpass_begin.pClearValues = &clear_value;
    renderpass_begin.framebuffer = framebuffer;

    const auto viewport = vk_create_info::viewport(extent.width, extent.height, 0, 1);
    const auto scissor = vk_create_info::rect2D(extent, 0, 0);

    result = vkBeginCommandBuffer(command, &command_buffer_begin);
    CHECK_RESULT(result);
    vkCmdBeginRenderPass(command, &renderpass_begin, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdSetViewport(command, 0, 1, &viewport);
    vkCmdSetScissor(command, 0, 1, &scissor);
    // vkCmdBindPipeline(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    // vkCmdDraw(command, 4, 1, 0, 0);
    // VkDeviceSize offset = 0;
    // vkCmdBindVertexBuffers(command, 0, 1, &mesh.vertex_buffer.buffer, &offset);
    // vkCmdBindIndexBuffer(command, mesh.index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    // vkCmdDrawIndexed(command, mesh.indices.size(), 1, 0, 0, 1);

    vkCmdEndRenderPass(command);
    result = vkEndCommandBuffer(command);
    CHECK_RESULT(result);
}

auto vulkan::draw() -> void
{
}

auto vulkan::clean() -> void
{
    if (renderpass)
    {
        vkDestroyRenderPass(device, renderpass, nullptr);
        renderpass = VK_NULL_HANDLE;
    }

    if (command_pool)
    {
        vkDestroyCommandPool(device, command_pool, nullptr);
        command_pool = VK_NULL_HANDLE;
    }

    if (!framebuffers.empty() && framebuffers.front() != VK_NULL_HANDLE)
    {
        for (auto i : range(framebuffers.size()))
        {
            vkDestroyFramebuffer(device, framebuffers[i], nullptr);
            framebuffers[i] = VK_NULL_HANDLE;
            vkDestroyImageView(device, swapchain.image_views[i], nullptr);
            swapchain.image_views[i] = VK_NULL_HANDLE;
        }
    }

    if (swapchain.self)
    {
        vkDestroySwapchainKHR(device, swapchain.self, nullptr);
        swapchain.self = VK_NULL_HANDLE;
    }

    if (device)
    {
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }

    if (instance && surface)
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        surface = VK_NULL_HANDLE;
    }

    if (instance && debug_messenger)
    {
        vkb::destroy_debug_utils_messenger(instance, debug_messenger);
        debug_messenger = VK_NULL_HANDLE;
    }

    if (instance)
    {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }
}

vulkan::~vulkan()
{
    if (renderpass)
    {
        vkDestroyRenderPass(device, renderpass, nullptr);
    }

    if (command_pool)
    {
        vkDestroyCommandPool(device, command_pool, nullptr);
    }

    if (!framebuffers.empty() && framebuffers.front() != VK_NULL_HANDLE)
    {
        for (auto i : range(framebuffers.size()))
        {
            vkDestroyFramebuffer(device, framebuffers[i], nullptr);
            vkDestroyImageView(device, swapchain.image_views[i], nullptr);
        }
    }

    if (swapchain.self)
    {
        vkDestroySwapchainKHR(device, swapchain.self, nullptr);
    }

    if (device)
    {
        vkDestroyDevice(device, nullptr);
    }

    if (instance && surface)
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    if (instance && debug_messenger)
    {
        vkb::destroy_debug_utils_messenger(instance, debug_messenger);
    }

    if (instance)
    {
        vkDestroyInstance(instance, nullptr);
    }
}

NAMESPACE_END(rhi)
NAMESPACE_END(runtime)
