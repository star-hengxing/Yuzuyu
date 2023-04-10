#pragma once

#ifdef USE_MODULES
import <functional>;
import <vector>;
#else
#include <functional>
#include <vector>
#endif

#include <runtime/base/Rect2D.hpp>
#include "Device.hpp"

NAMESPACE_BEGIN(rhi)

struct Swapchain
{
public:
    Device* device = nullptr;

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;

    VkFormat format;
    VkExtent2D extent;
    std::vector<VkImage> images;
    std::vector<VkImageView> image_views;

    u32 frame_index = 0;
    VkQueue present_queue = VK_NULL_HANDLE;
    u32 present_queue_index = 0;

    bool resized = false;

private:
    auto create(Rect2D rect2d, bool vsync) noexcept -> bool;

    auto destroy_image_view() noexcept -> void;

public:
    ~Swapchain();

    [[nodiscard]] 
    auto initialize(Device* device, Rect2D rect2d, bool vsync,
        const std::function<VkSurfaceKHR(VkInstance)>& callback) noexcept -> bool;

    auto resize(Rect2D rect2d, bool vsync) noexcept -> bool;

    auto present(VkSemaphore semaphore) noexcept -> void;

    auto acquire_next(VkSemaphore semaphore, VkFence fence) noexcept -> void;
};

NAMESPACE_END(rhi)
