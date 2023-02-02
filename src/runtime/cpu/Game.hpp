#pragma once

#ifdef USE_MODULES
import <vector>;
#else
#include <vector>
#endif

#include <runtime/window/sdl.hpp>
#include <runtime/helper/Owned.hpp>
#include "widget/Button.hpp"
#include "Renderer.hpp"

NAMESPACE_BEGIN(runtime)

struct Game final
{
private:
    using Self = Game;

private:
    SDL_Event event;
    window::sdl_system window;

    bool is_run = true;

    render::draw_config draw_config;
    render::Renderer renderer;

    Image background{nullptr, 0, 0};

    u16 button_width = 0;
    u16 button_height = 0;
    u16 button_margin = 0;
    std::vector<widget::Button> buttons;

    u16 mosue_x = 0;
    u16 mosue_y = 0;

private:
    auto input_handle() -> void;
    auto render_config() -> void;
    auto draw() -> void;

public:
    static auto create(int argc, char** argv) -> Owned<Self>;

    auto initialize() -> void;

    auto run() -> void;

    auto clean() -> void;
};

NAMESPACE_END(runtime)
