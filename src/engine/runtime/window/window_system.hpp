#pragma once

#ifdef USE_MODULES
import <tuple>;
#else
#include <tuple>
#endif

#include <GLFW/glfw3.h>

#include <runtime/helper/basic.hpp>

NAMESPACE_BEGIN(runtime)

struct window_system
{
private:
    usize width;
    usize height;
    GLFWwindow* window = nullptr;

public:
    window_system() {}
    // if success return null pointer, otherwise return the error message string
    [[nodiscard]]
    auto initialize(usize width, usize height, const char* title) -> const char*;
    // wait for window work
    auto resized() -> std::tuple<usize, usize>;

    template <typename F>
    auto main_loop(F f) -> void
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            f();
        }
    }

    auto clean() const -> void
    {
        if (window)
        {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    }

    auto set_title(const char* title) -> void
    {
        if (window)
        {
            glfwSetWindowTitle(window, title);
        }
    }

    auto get_window() const -> GLFWwindow*
    {
        return window;
    }

    auto get_window_size() const -> std::tuple<usize, usize>
    {
        return {width, height};
    }
};

NAMESPACE_END(runtime)
