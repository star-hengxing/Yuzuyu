#include "glfw.hpp"

NAMESPACE_BEGIN(runtime)
NAMESPACE_BEGIN(window)

glfw_system::~glfw_system()
{
    if (window)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

auto glfw_system::clean() -> void
{
    if (window)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
    }
}

auto glfw_system::initialize(const Config& config) -> const char*
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
    this->width = config.width;
    this->height = config.height;

    window = glfwCreateWindow(width, height, config.title, nullptr, nullptr);
    if (window == 0)
    {
        glfwTerminate();
        return "Failed to create window";
    }
    return nullptr;
}

auto glfw_system::resized() -> std::tuple<u16, u16>
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

auto glfw_system::create_surface(VkInstance instance, const VkAllocationCallbacks* allocator)
    -> VkSurfaceKHR
{
    VkSurfaceKHR surface;
    const auto result = glfwCreateWindowSurface(instance, window, allocator, &surface);
    return (result == VK_SUCCESS) ? surface : VK_NULL_HANDLE;
}

NAMESPACE_END(window)
NAMESPACE_END(runtime)
