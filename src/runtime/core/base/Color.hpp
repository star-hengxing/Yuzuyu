#pragma once

#ifdef USE_MODULES
import <concepts>;
import <compare>;
#else
#include <concepts>
#include <compare>
#endif

#include "base.hpp"

template <typename T>
constexpr auto max_number() -> T
{
    if constexpr (std::is_same_v<f32, T>)
        return 1;
    else if constexpr (std::is_same_v<u8, T>)
        return 255;
}

template <typename T>
struct RGBA
{
    T r, g, b, a;

    RGBA() {}

    constexpr RGBA(T r, T g, T b, T a = max_number<T>()) :
        r(r), g(g), b(b), a(a) {}

    constexpr auto operator <=> (const RGBA&) const = default;
};

using Color = RGBA<u8>;

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

template <std::floating_point T>
constexpr auto float_to_rgb_basic(T value) -> u8
{
    if (value >= 1) return 255;
    if (value <= 0) return 0;
    return static_cast<u8>(value * static_cast<T>(255));
}

template <std::floating_point T>
constexpr auto float_to_rgb(RGBA<T> c) -> Color
{
    return {float_to_rgb_basic(c.r), float_to_rgb_basic(c.g), float_to_rgb_basic(c.b), float_to_rgb_basic(c.a)};
}

template <std::floating_point T>
constexpr auto rgb_to_float(const Color& c) -> RGBA<T>
{
    const auto r = static_cast<T>(1) / static_cast<T>(255);
    return {static_cast<T>(c.r) * r, static_cast<T>(c.g) * r, static_cast<T>(c.b) * r, static_cast<T>(c.a) * r};
}

template <std::floating_point T>
constexpr auto operator * (const RGBA<T>& self, T value) -> RGBA<T>
{
    const auto [r, g, b, a] = self;
    return {r * value, g * value, b * value, a * value};
}

template <std::floating_point T>
constexpr auto operator + (const RGBA<T>& left, const RGBA<T>& right) -> RGBA<T>
{
    const auto [lr, lg, lb, la] = left;
    const auto [rr, rg, rb, ra] = right;
    return {lr + rr, lg + rg, lb + rb, la + ra};
}
