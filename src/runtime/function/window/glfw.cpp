#include "glfw.hpp"

NAMESPACE_BEGIN(window)

glfw_system::~glfw_system()
{
    if (window)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

auto glfw_system::clean() noexcept -> void
{
    if (window)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
    }
}

auto glfw_system::set_title(const char* title) noexcept -> void
{
    glfwSetWindowTitle(window, title);
}

auto glfw_system::initialize(const config& cfg) noexcept -> const char*
{
    if (glfwInit() == 0)
    {
        return "Failed to init glfw";
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if (glfwVulkanSupported() == 0)
    {
        return "glfwVulkanSupported() failed";
    }
    // initialize member
    this->width = cfg.width;
    this->height = cfg.height;

    window = glfwCreateWindow(width, height, cfg.title, nullptr, nullptr);
    if (window == 0)
    {
        glfwTerminate();
        return "Failed to create window";
    }
    return nullptr;
}

auto glfw_system::resized() noexcept -> Rect2D
{
    int w = 0, h = 0;
    glfwGetFramebufferSize(window, &w, &h);

    while (w == 0 || h == 0)
    {
        glfwGetFramebufferSize(window, &w, &h);
        glfwWaitEvents();
    }

    width = w;
    height = h;
    return {width, height};
}

auto glfw_system::create_surface(VkInstance instance) noexcept
    -> VkSurfaceKHR
{
    VkSurfaceKHR surface;
    const auto result = glfwCreateWindowSurface(instance, window, VK_NULL_HANDLE, &surface);
    return (result == VK_SUCCESS) ? surface : VK_NULL_HANDLE;
}

NAMESPACE_END(window)
