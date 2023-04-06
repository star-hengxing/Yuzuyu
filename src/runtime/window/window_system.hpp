#pragma once

#include <config.hpp>
#include <runtime/base/Rect2D.hpp>

NAMESPACE_BEGIN(window)

struct Config
{
    u16 width = Game::DEFAULT_WIDTH;
    u16 height = Game::DEFAULT_HEIGHT;
    const char* title = Game::WINDOW_TITLE;
    bool use_vulkan = true;
};

NAMESPACE_END(window)
