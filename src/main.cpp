#include <spdlog/spdlog.h>

#include <runtime/window/window_system.hpp>
#include <runtime/rhi/vulkan.hpp>

#include <config.hpp>
using namespace Engine;

auto check_message(const char* message)
{
    if (message)
    {
        spdlog::error("{}", message);
        exit(-1);
    }
}

auto test()
{
    const char* message = nullptr;
    runtime::window_system window;
    message = window.initialize(DEFAULT_WIDTH, DEFAULT_HEIGHT, WINDOW_TITLE);
    check_message(message);

    runtime::rhi::vulkan rhi;
    message = rhi.initialize(&window);
    check_message(message);

    window.main_loop([&]
    {
        const auto old_size = window.get_window_size();
        const auto new_size = window.resized();
        if (old_size != new_size)
        {
            const auto [w, h] = new_size;
            spdlog::info("window resize -> width: {} height: {}", w, h);
        }
    });

    rhi.clean();
    window.clean();
}

auto main(int argc, char** argv) -> int
{
    test();
}
