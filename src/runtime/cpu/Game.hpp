#pragma once

#ifdef USE_MODULES
import <vector>;
#else
#include <vector>
#endif

#include <runtime/base/Owned.hpp>
#include <runtime/input/input_system.hpp>
#include <runtime/window/sdl.hpp>
#include "Renderer.hpp"

NAMESPACE_BEGIN(runtime)

struct Game final
{
private:
    using Self = Game;

private:
    window::sdl_system window_system;
    input::input_system input_system;
    render::Renderer renderer;

    bool is_run = true;

    render::draw_config draw_config;

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
