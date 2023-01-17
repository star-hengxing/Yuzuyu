#ifdef USE_MODULES
import <string_view>;
#else
// #include <string_view>
#endif

// #include <spdlog/spdlog.h>

#include <runtime/window/window_system.hpp>
#include <runtime/helper/range.hpp>
#include <runtime/io/io.hpp>
// #include <runtime/rhi/vulkan.hpp>
#include "Game.hpp"
#include <config.hpp>
using namespace Game;

bool is_run = true;

NAMESPACE_BEGIN()

auto key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void
{
    io::println("key_callback");
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_Q:
            is_run = false;
            break;
        }
    }
}

auto check_message(const char* message) -> void
{
    if (message)
    {
        io::println(message);
        // spdlog::error("{}", message);
        exit(-1);
    }
}

NAMESPACE_END()

NAMESPACE_BEGIN(runtime)

auto Game::create(int argc, char** argv) -> Owned<Self>
{
    for (auto i : range(argc))
    {
        const auto str = std::string_view{argv[i]};
        io::println(str);
    }
    return Owned<Self>::make();
}

auto Game::initialize() -> void
{
    const char* message;

    message = window.initialize(DEFAULT_WIDTH, DEFAULT_HEIGHT, WINDOW_TITLE);
    check_message(message);

    window.set_key_callback(key_callback);

    // runtime::rhi::vulkan rhi;
    // message = rhi.initialize(&window);
    // check_message(message);
}

auto Game::run() -> void
{
    window.main_loop([&] {
        glfwWaitEvents();
        const auto old_size = window.get_window_size();
        const auto new_size = window.resized();
        if (old_size != new_size)
        {
            const auto [w, h] = new_size;
            // spdlog::info("window resize -> width: {} height: {}", w, h);
        }

        if (!is_run)
        {
            window.close_window();
        }
    });
}

auto Game::clean() -> void
{
}

NAMESPACE_END(runtime)
