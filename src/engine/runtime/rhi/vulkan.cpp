#include <source_location>
#include <limits>
#include <array>

#include <volk.h>
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <spdlog/spdlog.h>

#include <runtime/shader/precompile.hpp>
#include <runtime/helper/range.hpp>
#include <runtime/io/io.hpp>
#include <config.hpp>
#include "vk_create_info.hpp"
#include "vulkan.hpp"

// TODO: more friendly handle error

static auto check_vulkan_result(
    const char* path,
    int line,
    const char* function,
    VkResult result) -> void
{
    if (result == VK_SUCCESS)
        return;

    spdlog::error("[{}:{}] {}: vulkan result error", path, line, function);
}

#define CHECK_RESULT(result) check_vulkan_result( \
    std::source_location::current().file_name(),  \
    std::source_location::current().line(),       \
    std::source_location::current().function_name(), result)

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

    create_pipeline();

    return nullptr;
}

auto vulkan::create_instance_and_device() -> void
{
    VkResult result;
    // volk do: init, load instance and device
    result = volkInitialize();
    CHECK_RESULT(result);

    vkb::InstanceBuilder builder;

    auto inst_ret = builder.set_app_name(Engine::NAME)
                        .request_validation_layers(true)
                        .require_api_version(1, 3, 0)
                        .use_default_debug_messenger()
                        .build();
    if (!inst_ret)
    {
        spdlog::error("Failed to create Vulkan instance. Error: {}", inst_ret.error().message());
        exit(-1);
    }

    vkb::Instance vkb_inst = inst_ret.value();

    instance = vkb_inst.instance;
    debug_messenger = vkb_inst.debug_messenger;
    volkLoadInstance(instance);

    result = glfwCreateWindowSurface(instance, window->get_window(), nullptr, &surface);
    CHECK_RESULT(result);

    vkb::PhysicalDeviceSelector selector{vkb_inst};

    auto phys_ret = selector.set_minimum_version(1, 3)
                        .set_surface(surface)
                        .select();
    if (!phys_ret)
    {
        spdlog::error("Failed to select Vulkan Physical Device. Error: {}", phys_ret.error().message());
        clean();
        exit(-1);
    }

    vkb::DeviceBuilder device_builder{phys_ret.value()};
    // automatically propagate needed data from instance & physical device
    auto dev_ret = device_builder.build();
    if (!dev_ret)
    {
        spdlog::error("Failed to create Vulkan device. Error: {}", dev_ret.error().message());
        clean();
        exit(-1);
    }

    vkb::Device vkb_device = dev_ret.value();

    gpu = vkb_device.physical_device;
    device = vkb_device.device;
    graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
    present_queue = vkb_device.get_queue(vkb::QueueType::present).value();
    present_queue_family = vkb_device.get_queue_index(vkb::QueueType::present).value();

    volkLoadDevice(device);
}

auto vulkan::create_swapchain(uint32_t width, uint32_t height) -> void
{
    const auto old_swapchain = swapchain.self;
    extent = {width, height};

    vkb::SwapchainBuilder swapchain_builder{gpu, device, surface};

    auto swap_ret = swapchain_builder
                        .use_default_format_selection()
                        // use vsync present mode
                        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                        .set_desired_extent(extent.width, extent.height)
                        .set_old_swapchain(old_swapchain)
                        .build();

    if (!swap_ret)
    {
        spdlog::error("Failed to create swapchain. Error: {}", swap_ret.error().message());
        exit(-1);
    }

    auto vkb_swapchain = swap_ret.value();
    swapchain.self = vkb_swapchain.swapchain;
    swapchain.format = vkb_swapchain.image_format;
    swapchain.images = vkb_swapchain.get_images().value();
    swapchain.image_views = vkb_swapchain.get_image_views().value();
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
    io::println("shader size");
    io::println(shader::vs.size());
    io::println(shader::fs.size());
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

    for (auto i : range(swapchain.image_views.size()))
    {
        vkDestroyFramebuffer(device, framebuffers[i], nullptr);
        vkDestroyImageView(device, swapchain.image_views[i], nullptr);
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

NAMESPACE_END(rhi)
NAMESPACE_END(runtime)
