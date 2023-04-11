#pragma once

#include <runtime/base/unsafe/view2D.hpp>
#include <runtime/base/Owned.hpp>
#include <runtime/base/Color.hpp>

NAMESPACE_BEGIN(render)

struct draw_config
{
    u32 x1 = 0;
    u32 y1 = 0;
    u32 x2 = 0;
    u32 y2 = 0;
    f32 opacity = 1;
    Color color = color_space::black;
};

struct Renderer
{
private:
    using type = isize;
    using view_type = unsafe::view2D<Color, u32>;

public:
    Owned<Color[]> framebuffer;
    view_type framebuffer_view{nullptr, 0, 0};

private:
    auto draw_pixel(type x, type y, Color color) -> void;

    auto draw_line(type x1, type y1, type x2, type y2, Color color) -> void;

    auto draw_rectangle(type x1, type y1, type x2, type y2, Color color) -> void;

public:
    Renderer() {}

    auto initialize(u32 width, u32 height) -> void;
    // clear framebuffer
    auto clear(const Color color) -> void;
    // use background image to fill framebuffer
    auto clear(const view_type view) -> void;

    auto draw(const draw_config& config) -> void;

    auto draw_text(u32 x, u32 y, view_type view) -> void;
};

NAMESPACE_END(render)
