#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include <tl/expected.hpp>

#include <runtime/helper/unsafe/view2D.hpp>
#include <runtime/helper/Color.hpp>
#include <runtime/helper/Owned.hpp>

struct Image
{
private:
    using Self = Image;

public:
    using type = Color;
    using view = unsafe::view2D<type, u16>;

private:
    Owned<type[]> data;
    u16 width, height;

public:
    // get data ownership
    Image(type* data, u16 width, u16 height) :
        data(data), width(width), height(height) {}

    [[nodiscard]]
    auto resize(u16 width, u16 height) -> std::string;
    [[nodiscard]]
    auto write(const std::string_view filename) const -> std::string;

    auto get_view() const noexcept -> view;

    auto swap(Self& other) noexcept -> Self&;

    static auto create(u16 width, u16 height) noexcept -> Self;
};

NAMESPACE_BEGIN(io)
NAMESPACE_BEGIN(file)

// if error, return error message
auto read_to_image(const std::string_view filename) -> tl::expected<Image, std::string>;
// hardcode: rgba 8-bit.
// if error, return error message
auto write_image(const std::string_view filename, const u8* data, u16 width, u16 height)
    -> std::string;

NAMESPACE_END(file)
NAMESPACE_END(io)
