// #ifdef USE_MODULES
// import <string_view>;
// #else
// #include <string_view>
// #endif

#include <fast_io.h>

#include <runtime/helper/fps_counter.hpp>
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

    message = window.initialize(config);
    check_message(message);

    renderer.initialize(config.width, config.height);

    auto result = io::file::read_to_image("A:/tmp/test.png");
    if (result.has_value())
    {
        background = std::move(result.value());
        background.resize(1280, 720);
    }
    else
    {
        perrln(result.error());
    }

    const auto [w, h] = window.get_window_size();
    widget::Button button;
    button.x1 = (1 / 4.0) * w;
    button.y1 = (1 / 8.0) * h;
    button.x2 = (3 / 4.0) * w;
    button.y2 = (2 / 8.0) * h;
    buttons.push_back(button);

    button.x1 = (1 / 4.0) * w;
    button.y1 = (3 / 8.0) * h;
    button.x2 = (3 / 4.0) * w;
    button.y2 = (4 / 8.0) * h;
    buttons.push_back(button);
}

auto Game::input_handle() -> void
{
    switch (event.type)
    {
    case SDL_QUIT:
        is_run = false;
        break;
    case SDL_KEYDOWN: {
        break;
    }
    case SDL_MOUSEMOTION: {
        mosue_x = event.motion.x;
        mosue_y = event.motion.y;
        break;
    }
    default:
        break;
    }
}

auto Game::render_config() -> void
{
    for (auto& button : buttons)
    {
        if (button.is_mouse_hovering(mosue_x, mosue_y))
        {
            button.opacity = 0.5;
        }
        else
        {
            button.opacity = 0.75;
        }
        button.color = color_space::cyan;
    }
}

auto Game::draw() -> void
{
    renderer.clear(background.get_view());
    for (auto& button : buttons)
    {
        draw_config.x1 = button.x1;
        draw_config.y1 = button.y1;
        draw_config.x2 = button.x2;
        draw_config.y2 = button.y2;
        draw_config.color = button.color;
        draw_config.opacity = button.opacity;
        renderer.draw(draw_config);
    }
}

auto Game::run() -> void
{
    FPS_counter fps;
    while (is_run)
    {
        SDL_PollEvent(&event);
        input_handle();
        render_config();
        draw();
        window.present(renderer.framebuffer_view.ptr);
        fps.update([&](const char* buffer) {
            window.set_title(buffer);
        });
    }
}

auto Game::clean() -> void
{
}

NAMESPACE_END(runtime)
