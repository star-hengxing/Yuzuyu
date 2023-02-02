#pragma once

#include <runtime/helper/unsafe/view2D.hpp>
#include <runtime/helper/Owned.hpp>
#include <runtime/helper/Color.hpp>

NAMESPACE_BEGIN(render)

struct draw_config
{
    u16 x1 = 0;
    u16 y1 = 0;
    u16 x2 = 0;
    u16 y2 = 0;
    f32 opacity = 1;
    Color color = color_space::black;
};

struct Renderer
{
private:
    using type = isize;
    using view_type = unsafe::view2D<Color, u16>;

public:
    Owned<Color[]> framebuffer;
    view_type framebuffer_view{nullptr, 0, 0};

private:
    auto draw_pixel(type x, type y, Color color) -> void;

    auto draw_line(type x1, type y1, type x2, type y2, Color color) -> void;

    auto draw_rectangle(type x1, type y1, type x2, type y2, Color color) -> void;

public:
    Renderer() {}

    auto initialize(u16 width, u16 height) -> void;
    // clear framebuffer
    auto clear(const Color color) -> void;
    // use background image to fill framebuffer
    auto clear(const view_type view) -> void;

    auto draw(const draw_config& config) -> void;
};

NAMESPACE_END(render)
