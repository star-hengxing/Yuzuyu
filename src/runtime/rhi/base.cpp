#include <volk.h>
#include <VkBootstrap.h>
#include <spdlog/spdlog.h>

#include <config.hpp>
#include "vulkan.hpp"
#include "helper.hpp"

auto check_vulkan_result(VkResult result, const std::source_location location)
    -> void
{
    if (result == VK_SUCCESS) [[likely]]
        return;

    spdlog::error("[{}:{}] {}: vulkan result error",
        location.file_name(), location.line(), location.function_name());
}

NAMESPACE_BEGIN(runtime)
NAMESPACE_BEGIN(rhi)

auto vulkan::create_instance_and_device() -> void
{
    VkResult result;
    // volk do: init, load instance and device
    result = volkInitialize();
    CHECK_RESULT(result);

    vkb::InstanceBuilder builder;

    auto inst_ret = builder.set_app_name(Game::NAME)
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
    surface = window->create_surface(instance);
    if (surface == nullptr)
    {
        spdlog::error("Failed to create window surface");
    }

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

NAMESPACE_END(rhi)
NAMESPACE_END(runtime)
