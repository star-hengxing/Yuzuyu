#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include <core/base/Color.hpp>

NAMESPACE_BEGIN(render)

struct draw_config
{
    u32 x1 = 0;
    u32 y1 = 0;
    u32 x2 = 0;
    u32 y2 = 0;
    f32 opacity = 1;
    Color color = color_space::black;
    std::string_view texture;
};

NAMESPACE_END(render)
