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
    ~window_system()
    {
        clean();
    }
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
            f();
        }
    }

    auto clean() -> void
    {
        if (window)
        {
            glfwDestroyWindow(window);
            glfwTerminate();
            window = nullptr;
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

    template <typename F>
    requires requires(F f, GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        { f(window, key, scancode, action, mods) } -> std::same_as<void>;
    }
    auto set_key_callback(F f) -> void
    {
        glfwSetKeyCallback(window, f);
    }

    auto close_window() -> void
    {
        glfwSetWindowShouldClose(window, true);
    }
};

NAMESPACE_END(runtime)
