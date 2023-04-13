#include <fast_io.h>

#include <VkBootstrap.h>

#include "Swapchain.hpp"
#include "base.hpp"

NAMESPACE_BEGIN(rhi)

auto Swapchain::destroy_image_view() noexcept -> void
{
    for (auto i : image_views)
    {
        vkDestroyImageView(device->handle, i, VK_NULL_HANDLE);
    }
    image_views.clear();
}

auto Swapchain::clean() noexcept -> void
{
    destroy_image_view();
    if (swapchain)
    {
        vkDestroySwapchainKHR(device->handle, swapchain, VK_NULL_HANDLE);
        swapchain = VK_NULL_HANDLE;
    }

    if (surface)
    {
        vkDestroySurfaceKHR(device->instance, surface, VK_NULL_HANDLE);
        surface = VK_NULL_HANDLE;
    }
}

auto Swapchain::create(Rect2D rect2d, bool vsync) noexcept -> bool
{
    const auto [width, height] = rect2d;
    extent = {width, height};
    const auto mode = (vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR);
    auto swapchain_builder = vkb::SwapchainBuilder{device->gpu, device->handle, surface};
    auto result = swapchain_builder
                      .use_default_format_selection()
                      .set_desired_present_mode(mode)
                      .set_desired_extent(extent.width, extent.height)
                      .set_old_swapchain(swapchain)
                      .build();
    if (!result)
    {
        perrln("Failed to create swapchain\n", result.error().message());
        return false;
    }

    auto vkb_swapchain = result.value();
    this->swapchain   = vkb_swapchain.swapchain;
    this->format      = vkb_swapchain.image_format;
    this->images      = vkb_swapchain.get_images().value();
    this->image_views = vkb_swapchain.get_image_views().value();
    return true;
}

auto Swapchain::initialize(Device* device, Rect2D rect2d, bool vsync,
    const std::function<VkSurfaceKHR(VkInstance)>& callback) noexcept -> bool
{
    // surface = std::invoke(callback, device->instance);
    // if (!surface)
    // {
    //     perr("Failed to create window surface\n");
    //     return false;
    // }
    this->device = device;
    this->surface = device->surface;

    if (!create(rect2d, vsync))
        return false;

    this->present_queue = device->queue.handles[(usize)Queue::Family::present];
    this->present_queue_index = device->queue.indexes[(usize)Queue::Family::present];
    return true;
}

auto Swapchain::resize(Rect2D rect2d, bool vsync) noexcept -> bool
{
    device->wait_idle();
    vkDestroySwapchainKHR(device->handle, swapchain, VK_NULL_HANDLE);

    destroy_image_view();

    if (!create(rect2d, vsync))
        return false;

    return true;
}

auto Swapchain::present(VkSemaphore semaphore) noexcept -> void
{
    auto present_info = VkPresentInfoKHR
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &frame_index,
    };

    if (semaphore)
    {
        present_info.pWaitSemaphores = &semaphore;
        present_info.waitSemaphoreCount = 1;
    }

    vkQueueWaitIdle(present_queue);
    const auto result = vkQueuePresentKHR(present_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        frame_index = 0;
    }

    CHECK_RESULT(result);
}

auto Swapchain::acquire_next(VkSemaphore semaphore, VkFence fence) noexcept -> void
{
    const auto result = vkAcquireNextImageKHR(
               device->handle, swapchain,
               TIME_OUT,
               semaphore, fence,
               &frame_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            resized = true;
        }
        return;
    }

    resized = false;
    CHECK_RESULT(result);
}

NAMESPACE_END(rhi)
