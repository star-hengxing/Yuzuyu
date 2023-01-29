#pragma once

#include <config.hpp>

NAMESPACE_BEGIN(runtime)
NAMESPACE_BEGIN(window)

struct Config
{
    u16 width = Game::DEFAULT_WIDTH;
    u16 height = Game::DEFAULT_HEIGHT;
    const char* title = Game::WINDOW_TITLE;
    bool use_vulkan = true;
};

NAMESPACE_END(window)
NAMESPACE_END(runtime)
