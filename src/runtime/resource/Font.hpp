#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include <fast_io.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Image.hpp"

struct font_config
{
    Color color = color_space::white;
    usize pixel_size;
    std::u32string_view text;
    // isize baseline;
    // u32 begin, end;
    // i32 render_mode;
    // usize texture_size;
};

struct font_mananger
{
private:
    using Self = font_mananger;

public:
    FT_Library ft = nullptr;

    FT_Face face;
    fast_io::native_file_loader data;

public:
    font_mananger(FT_Library ft, FT_Face face, fast_io::native_file_loader file)
        : ft(ft), face(face), data(std::move(file)) {}

    ~font_mananger();

    font_mananger(Self&& other) noexcept
        : ft(other.ft), face(other.face), data(std::move(other.data))
    {
        other.ft = nullptr;
        other.face = nullptr;
    }

    auto operator = (Self&& other) noexcept -> Self&
    {
        Self(std::move(other)).swap(*this);
        return *this;
    }

    auto swap(Self& other) -> Self&;

    auto render_line(const font_config& config) -> Image;
};

NAMESPACE_BEGIN(io::file)

// if error, return error message
auto read_to_font(const std::string_view filename) -> font_mananger;

NAMESPACE_END(io::file)
