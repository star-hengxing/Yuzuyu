#pragma once

#include "../Color.hpp"

NAMESPACE_BEGIN(widget)

struct Button
{
    bool visible;
    u16 x1, y1, x2, y2;
    // Vec2 Size;
    // Vec2 TexSize;
    // std::string Text;
    // std::function<void()> Callback;
    Color color;
    f32 opacity;
};

NAMESPACE_END(widget)
