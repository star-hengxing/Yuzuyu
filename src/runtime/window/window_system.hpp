#pragma once

#include <config.hpp>
#include <runtime/base/Rect2D.hpp>

NAMESPACE_BEGIN(window)

struct Config
{
    u32 width = Game::DEFAULT_WIDTH;
    u32 height = Game::DEFAULT_HEIGHT;
    const char* title = Game::WINDOW_TITLE;
    bool use_vulkan = true;
};

NAMESPACE_END(window)
