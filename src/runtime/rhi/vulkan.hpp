#pragma once

#ifdef USE_MODULES
import <vector>;
#else
#include <vector>
#endif

#include <volk.h>

#include <runtime/window/sdl.hpp>

NAMESPACE_BEGIN(runtime)
NAMESPACE_BEGIN(rhi)

struct vulkan
{
private:
    using window_system = window::sdl_system;

private:
    window_system* window = nullptr;

    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    VkQueue graphics_queue;
    uint32_t graphics_queue_family;

    VkQueue present_queue;
    uint32_t present_queue_family;

    struct Swapchain
    {
        VkSwapchainKHR self = VK_NULL_HANDLE;
        VkFormat format;
        std::vector<VkImage> images;
        std::vector<VkImageView> image_views;
    };

    VkExtent2D extent;
    Swapchain swapchain;

    VkCommandPool command_pool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commands;

    VkRenderPass renderpass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> framebuffers;

    struct Frame
    {
        struct Semaphore
        {
            VkSemaphore signal = VK_NULL_HANDLE;
            VkSemaphore wait = VK_NULL_HANDLE;
        };

        Semaphore semaphores;
        VkFence fence = VK_NULL_HANDLE;
    };

    std::vector<Frame> frames;
    uint32_t frame_index;

private:
    auto create_instance_and_device() -> void;
    auto create_swapchain(uint32_t width, uint32_t height) -> void;
    auto create_command_pool() -> void;
    auto create_command_buffer() -> void;
    auto create_renderpass() -> void;
    auto create_framebuffers() -> void;
    auto create_sync() -> void;
    auto create_pipeline() -> void;
    auto create_descriptor_pool() -> void;
    auto create_descriptor_set() -> void;

    auto acquire_frame(VkSemaphore present, unsigned* index) -> void;
    auto submit_frame(VkQueue queue) -> VkResult;
    auto present_frame(VkQueue queue, uint32_t index, VkSemaphore wait) -> VkResult;
    auto window_resize() -> void;

    auto record_command(VkCommandBuffer command, VkFramebuffer framebuffer) -> void;
    auto draw() -> void;

public:
    ~vulkan();
    // if success return null pointer, otherwise return the error message string
    [[nodiscard]] auto initialize(window_system* window) -> const char*;

    auto clean() -> void;
};

NAMESPACE_END(rhi)
NAMESPACE_END(runtime)
