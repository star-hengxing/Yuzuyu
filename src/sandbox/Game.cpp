#include <core/base/fps_counter.hpp>
#include "Game.hpp"

NAMESPACE_BEGIN()

auto check_message(const char* message) -> void
{
    if (message) [[unlikely]]
    {
        perrln(fast_io::mnp::os_c_str(message));
        exit(-1);
    }
}

NAMESPACE_END()

auto Game::create(int argc, char** argv) -> Owned<Self>
{
    return Owned<Self>::make();
}

auto Game::initialize() -> void
{
    {
        ::window::config config;
        config.width = 1280;
        config.height = 720;
        // config.use_vulkan = true;
        check_message(window_system.initialize(config));
    }
}

auto Game::input_handle() -> void
{
    is_run = !input_system.is_exit;
}

auto Game::render_config() -> void
{

}

auto Game::draw() -> void
{

}

auto Game::run() -> void
{
    FPS_counter fps;
    while (is_run)
    {
        input_system.poll().collect_input();
        input_handle();
        fps.update([&](const char* buffer) {
            window_system.set_title(buffer);
        });
    }
}

auto Game::clean() -> void
{

}
