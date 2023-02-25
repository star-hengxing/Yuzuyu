#include <cstdlib>

#include <runtime/helper/range.hpp>
#include "Renderer.hpp"

NAMESPACE_BEGIN()

auto color_blend(const Color back, const Color front, f32 alpha) -> Color
{
    const auto back_ = rgb_to_float<f32>(back);
    const auto front_ = rgb_to_float<f32>(front);

    return float_to_rgb(front_ * alpha + back_ * (1 - alpha));
}

NAMESPACE_END()

NAMESPACE_BEGIN(render)

auto Renderer::initialize(u16 width, u16 height) -> void
{
    framebuffer = Owned<Color[]>::make_uninitialize(width * height);
    framebuffer_view = {framebuffer.get(), width, height};
}

auto Renderer::draw_pixel(type x, type y, Color color) -> void
{
    if (x < framebuffer_view.width && y < framebuffer_view.height)
    {
        framebuffer_view.set(x, y, color);
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
    if (x1 >= framebuffer_view.width || x2 >= framebuffer_view.width || y1 >= framebuffer_view.height || y2 >= framebuffer_view.height)
    {
        return;
    }

    for (auto y : range(y1, y2))
    {
        for (auto x : range(x1, x2))
        {
            framebuffer_view.set(x, y, color);
        }
    }
}

auto Renderer::clear(const Color color) -> void
{
    framebuffer_view.fill(color);
}

auto Renderer::clear(const view_type view) -> void
{
    const auto [ptr, width, height] = view;
    if (width != framebuffer_view.width || height != framebuffer_view.height)
        return;

    for (auto i : range(width * height))
    {
        framebuffer_view.ptr[i] = ptr[i];
    }
}

auto Renderer::draw(const draw_config& config) -> void
{
    auto [x1, y1, x2, y2, opacity, color] = config;

    x1 = std::min(x1, framebuffer_view.width);
    x2 = std::min(x2, framebuffer_view.width);
    y1 = std::min(y1, framebuffer_view.height);
    y2 = std::min(y2, framebuffer_view.height);

    for (auto y : range(y1, y2))
    {
        for (auto x : range(x1, x2))
        {
            Color pixel_color;
            if (opacity == 1.0)
            {
                pixel_color = color;
            }
            else
            {
                const auto front = color;
                const auto back = framebuffer_view.get(x, y);
                pixel_color = color_blend(back, front, opacity);
            }

            framebuffer_view.set(x, y, pixel_color);
        }
    }
}

auto Renderer::draw_text(u16 x_, u16 y_, view_type view) -> void
{
    if (x_ >= framebuffer_view.width || y_ >= framebuffer_view.height)
        return;

    auto width = (x_ + view.width < framebuffer_view.width)
        ? view.width : (x_ + view.width - framebuffer_view.width);
    auto height = (y_ + view.height < framebuffer_view.height)
        ? view.height : (y_ + view.height - framebuffer_view.height);

    for (auto y : range(height))
    {
        for (auto x : range(width))
        {
            const Color color = view.get(x, y);
            if (color != Color{0, 0, 0, 0})
            {
                framebuffer_view.set(x_ + x, y_ + y, color);
            }
        }
    }
}

NAMESPACE_END(render)
