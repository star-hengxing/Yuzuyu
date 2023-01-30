#pragma once

#include <runtime/helper/unsafe/view2D.hpp>
#include "Color.hpp"

struct Renderer
{
    unsafe::view2D<RGBA> framebuffer;

    using type = isize;

    Renderer(unsafe::view2D<RGBA> framebuffer) :
        framebuffer(framebuffer)
    {}

    auto draw_pixel(type x, type y, Color color) -> void;

    auto draw_line(type x1, type y1, type x2, type y2, Color color) -> void;

    auto draw_rectangle(type x1, type y1, type x2, type y2, Color color) -> void;
};
