#pragma once

#include <core/base/Owned.hpp>
#include <function/input/input_system.hpp>
#include <function/window/sdl.hpp>

struct Game final
{
private:
    using Self = Game;

private:
    window::sdl_system window_system;
    input::input_system input_system;

    bool is_run = true;

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
