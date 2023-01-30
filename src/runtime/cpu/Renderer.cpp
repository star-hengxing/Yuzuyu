#include <cstdlib>

#include <runtime/helper/range.hpp>
#include "Renderer.hpp"

auto Renderer::draw_pixel(type x, type y, Color color) -> void
{
    if (x < framebuffer.width && y < framebuffer.height)
    {
        framebuffer.set(x, y, color);
    }
}
// bresenham
// see: https://github.com/star-hengxing/software_renderer/blob/master/src/Renderer/Wireframe.cpp
auto Renderer::draw_line(type x1, type y1, type x2, type y2, Color color) -> void
{
    using std::swap;
    isize x, y;

    if (x1 != x2 && y1 != y2)
    {
        isize epsilon, inc;
        isize dx, dy, ddy, ddx;
        dx = std::abs(x2 - x1);
        dy = std::abs(y2 - y1);

        ddy = 2 * dy;
        ddx = 2 * dx;

        if (dx >= dy)
        {
            epsilon = ddy - dx;
            if (x2 < x1)
            {
                swap(x1, x2);
                swap(y1, y2);
            }
            inc = (y2 >= y1 ? 1 : -1);
            y = y1;
            for (x = x1; x <= x2; x += 1)
            {
                draw_pixel(x, y, color);
                if (epsilon < 0)
                {
                    epsilon += ddy;
                }
                else
                {
                    y += inc;
                    epsilon = epsilon + ddy - ddx;
                }
            }
        }
        else
        {
            epsilon = ddx - dy;
            if (y2 < y1)
            {
                swap(x1, x2);
                swap(y1, y2);
            }
            inc = (x2 >= x1 ? 1 : -1);
            x = x1;
            for (y = y1; y <= y2; y += 1)
            {
                draw_pixel(x, y, color);
                if (epsilon < 0)
                {
                    epsilon += ddx;
                }
                else
                {
                    x += inc;
                    epsilon = epsilon + ddx - ddy;
                }
            }
        }
    }
    else if (x1 == x2)
    {
        if (y1 > y2)
        {
            swap(y1, y2);
        }
        for (y = y1; y <= y2; y += 1)
        {
            draw_pixel(x1, y, color);
        }
    }
    else if (y1 == y2)
    {
        if (x1 > x2)
        {
            swap(x1, x2);
        }
        for (x = x1; x <= x2; x += 1)
        {
            draw_pixel(x, y1, color);
        }
    }
    else
    {
        draw_pixel(x1, y1, color);
    }
}

auto Renderer::draw_rectangle(type x1, type y1, type x2, type y2, Color color) -> void
{
    if (x1 >= framebuffer.width || x2 >= framebuffer.width ||
        y1 >= framebuffer.height || y2 >= framebuffer.height)
    {
        return;
    }
    // draw_line(x1, y1, x2, y1, color);
    for (auto y : range(y1, y2))
    {
        for (auto x : range(x1, x2))
        {
            draw_pixel(x, y, color);
        }
    }
}
