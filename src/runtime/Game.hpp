#pragma once

#include <runtime/window/sdl.hpp>
#include <runtime/base/Owned.hpp>

NAMESPACE_BEGIN(runtime)

struct Game final
{
private:
    using Self = Game;

private:
    window::sdl_system window;

public:
    static auto create(int argc, char** argv) -> Owned<Self>;

    auto initialize() -> void;

    auto run() -> void;

    auto clean() -> void;
};

NAMESPACE_END(runtime)
