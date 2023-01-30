#include <fast_io.h>

#include <runtime/helper/unsafe/view2D.hpp>
#include <runtime/helper/Owned.hpp>
#include <runtime/window/sdl.hpp>
#include "widget/Button.hpp"
#include "Renderer.hpp"
#include "Command.hpp"

struct FPS_counter
{
    usize fps = 0;
    char buffer[64];
    fast_io::obuffer_view view{std::begin(buffer), std::end(buffer)};
    fast_io::unix_timestamp start = posix_clock_gettime(fast_io::posix_clock_id::realtime);

    auto update(auto&& f) -> void
    {
        const auto current = posix_clock_gettime(fast_io::posix_clock_id::realtime);
        if ((current - start).seconds < 1)
        {
            fps += 1;
            return;
        }

        // for (auto i : buffer)
        // {
        //     i = 0;
        // }
        start = posix_clock_gettime(fast_io::posix_clock_id::realtime);

        view.clear();
        print(view, "fps: ", fps);

        fps = 0;
        f(buffer);
    }
};

using window_system = runtime::window::sdl_system;
window_system window;

runtime::window::Config config;

auto initialize() -> void
{
    config.title = "cpu";
    config.use_vulkan = false;

    auto message = window.initialize(config);
    if (message)
    {
        println(message);
        exit(-1);
    }
}

auto main(int argc, char** argv) -> int
{
    initialize();
    SDL_Event event;
    auto is_run = bool{true};
    FPS_counter fps;

    auto framebuffer = Owned<RGBA[]>::make(config.width * config.height);
    auto view = unsafe::view2D<RGBA>{framebuffer.get(), config.width, config.height};
    Renderer Renderer{view};

    auto button = widget::Button{};
    auto quit_command = Command::make('q', [&is_run] { is_run = false; });

    while (is_run)
    {
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            is_run = false;
            break;
        case SDL_KEYDOWN: {
            auto key = event.key.keysym.sym;
            if (quit_command.is_same(key))
            {
                quit_command.execute();
                break;
            }
        }
        case SDL_MOUSEMOTION: {
            button.color = {0, 255, 0, 255};
            button.x2 = event.motion.x;
            button.y2 = event.motion.y;
            break;
        }
        default:
            break;
        }
        view.fill({0, 0, 0, 255});
        Renderer.draw_rectangle(0, 0, button.x2, button.y2, button.color);
        window.present(view.ptr);
        fps.update([&](const char* buffer) { window.set_title(buffer); });
    }
}
