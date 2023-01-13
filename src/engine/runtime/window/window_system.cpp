#include "window_system.hpp"

NAMESPACE_BEGIN(runtime)

auto window_system::initialize(usize width, usize height, const char* title)
    -> const char*
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
    window = nullptr;
    this->width = width;
    this->height = height;

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == 0)
    {
        glfwTerminate();
        return "Failed to create window";
    }
    return nullptr;
}

auto window_system::resized() -> std::tuple<usize, usize>
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

NAMESPACE_END(runtime)
