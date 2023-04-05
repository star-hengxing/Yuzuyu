// #ifdef USE_MODULES
// import <string_view>;
// #else
// #include <string_view>
// #endif

#include <fast_io.h>

#include <runtime/base/fps_counter.hpp>
#include <runtime/io/Image.hpp>
#include "Game.hpp"

NAMESPACE_BEGIN()

auto check_message(const char* message) -> void
{
    if (message)
    {
        println(message);
        exit(-1);
    }
}

NAMESPACE_END()

NAMESPACE_BEGIN(runtime)

auto Game::create(int argc, char** argv) -> Owned<Self>
{
    return Owned<Self>::make();
}

auto Game::initialize() -> void
{
    const char* message;

    window::Config config;
    config.title = "エロゲーム";
    config.use_vulkan = false;

    message = window_system.initialize(config);
    check_message(message);

    renderer.initialize(config.width, config.height);

    input_system.bind_key('z', []{
        perr("z\n");
    });
}

auto Game::input_handle() -> void
{
    input_system.execute();
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
        render_config();
        draw();
        window_system.present(renderer.framebuffer_view.ptr);
        fps.update([&](const char* buffer) {
            window_system.set_title(buffer);
        });
    }
}

auto Game::clean() -> void
{
}

NAMESPACE_END(runtime)
