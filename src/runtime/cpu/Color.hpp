#pragma once

#include <runtime/helper/basic.hpp>

struct RGBA
{
    u8 r, g, b, a;

    RGBA()
    {}

    constexpr RGBA(u8 r, u8 g, u8 b, u8 a = 255) :
        r(r), g(g), b(b), a(a)
    {}
};

using Color = RGBA;

NAMESPACE_BEGIN(color_space)

constexpr auto black = Color{0, 0, 0};
constexpr auto white = Color{255, 255, 255};
constexpr auto red = Color{255, 0, 0};
constexpr auto green = Color{0, 255, 0};
constexpr auto blue = Color{0, 0, 255};
constexpr auto yellow = Color{255, 255, 0};
constexpr auto magenta = Color{255, 0, 255};
constexpr auto cyan = Color{0, 255, 255};

NAMESPACE_END(color_space)
