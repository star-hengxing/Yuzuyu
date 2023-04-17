#pragma once

#include <core/base/base.hpp>
#include <core/base/Rect2D.hpp>

NAMESPACE_BEGIN(window)

struct config
{
    u32 width = global::config::DEFAULT_WIDTH;
    u32 height = global::config::DEFAULT_HEIGHT;
    const char* title = global::config::WINDOW_TITLE;
    bool use_vulkan = true;
};

NAMESPACE_END(window)
