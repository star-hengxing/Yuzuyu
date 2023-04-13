#pragma once

#ifdef USE_VOLK
#include <volk.h>
#else
#include <vulkan/vulkan.h>
#endif
#include <GLFW/glfw3.h>

#include "window_system.hpp"

NAMESPACE_BEGIN(window)

struct glfw_system
{
private:
    u32 width;
    u32 height;
    GLFWwindow* window = nullptr;
    // VkSurfaceKHR surface = nullptr;

public:
    glfw_system() {}
    ~glfw_system();
    // if success return null pointer, otherwise return the error message string
    [[nodiscard]]
    auto initialize(const Config& config) noexcept -> const char*;
    // wait for window work
    auto resized() noexcept -> Rect2D;

    auto clean() noexcept -> void;

    auto set_title(const char* title) noexcept -> void;

    auto create_surface(VkInstance instance) noexcept
        -> VkSurfaceKHR;

    auto get_window() const noexcept -> decltype(window)
    {
        return window;
    }

    auto get_window_size() const noexcept -> Rect2D
    {
        return {width, height};
    }
};

NAMESPACE_END(window)
