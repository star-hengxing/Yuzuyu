#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include <core/base/unsafe/view2D.hpp>
#include <core/base/Color.hpp>
#include <core/base/Owned.hpp>

struct Image
{
private:
    using Self = Image;

public:
    using type = Color;
    using view = unsafe::view2D<type, u32>;

private:
    Owned<type[]> data;
    u32 width, height;

public:
    // get data ownership
    Image(type* data, u32 width, u32 height) :
        data(data), width(width), height(height) {}

    auto resize(u32 width, u32 height) noexcept -> void;
    [[nodiscard]]
    auto write(const std::string_view filename) const noexcept -> bool;

    auto get_view() const noexcept -> view;

    auto swap(Self& other) noexcept -> Self&;

    static auto create(u32 width, u32 height) noexcept -> Self;
};

NAMESPACE_BEGIN(io::file)

// if error, throw exception. type: std::string_view
auto read_to_image(const std::string_view filename) -> Image;
// only support rgba 8-bit.
[[nodiscard]]
auto write_image(const std::string_view filename, const u8* data, u32 width, u32 height)
    noexcept -> bool;

NAMESPACE_END(io::file)
