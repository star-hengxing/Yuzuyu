#pragma once

#include <runtime/base/Color.hpp>
#include <runtime/io/Image.hpp>

NAMESPACE_BEGIN(widget)

struct Button
{
    u16 x1 = 0;
    u16 y1 = 0;
    u16 x2 = 0;
    u16 y2 = 0;
    // Vec2 Size;
    // Vec2 TexSize;
    // std::string Text;
    // std::function<void()> Callback;
    Color color = color_space::black;
    f32 opacity = 1;
    Image::view texture{nullptr, 0, 0};
    bool visible = false;
    bool is_click = false;

    auto is_mouse_hovering(u16 x, u16 y) -> bool
    {
        return (x >= x1 && x <= x2) && (y >= y1 && y <= y2);
    }
};

NAMESPACE_END(widget)
