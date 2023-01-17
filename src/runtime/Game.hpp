#pragma once

#include <runtime/window/window_system.hpp>
#include <runtime/helper/Owned.hpp>

NAMESPACE_BEGIN(runtime)

struct Game final
{
private:
    using Self = Game;

private:
    window_system window;

public:
    static auto create(int argc, char** argv) -> Owned<Self>;

    auto initialize() -> void;

    auto run() -> void;

    auto clean() -> void;
};

NAMESPACE_END(runtime)
