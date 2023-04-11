#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include <tl/expected.hpp>

#include <runtime/base/unsafe/view2D.hpp>
#include <runtime/base/Color.hpp>
#include <runtime/base/Owned.hpp>

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

    [[nodiscard]]
    auto resize(u32 width, u32 height) -> std::string;
    [[nodiscard]]
    auto write(const std::string_view filename) const -> std::string;

    auto get_view() const noexcept -> view;

    auto swap(Self& other) noexcept -> Self&;

    static auto create(u32 width, u32 height) noexcept -> Self;
};

NAMESPACE_BEGIN(io)
NAMESPACE_BEGIN(file)

// if error, return error message
auto read_to_image(const std::string_view filename) -> tl::expected<Image, std::string>;
// hardcode: rgba 8-bit.
// if error, return error message
auto write_image(const std::string_view filename, const u8* data, u32 width, u32 height)
    -> std::string;

NAMESPACE_END(file)
NAMESPACE_END(io)
