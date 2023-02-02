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
public:
    using type = Color;
    using view = unsafe::view2D<type, u16>;

private:
    Owned<type[]> data;
    u16 width, height;

public:
    // get data ownership
    Image(type* data, u16 width, u16 height) :
        data(data), width(width), height(height)
    {}

    auto resize(u16 width, u16 height) -> void;

    auto write(const std::string_view filename) const -> void;

    auto get_view() const noexcept -> view;
};

NAMESPACE_BEGIN(io)
NAMESPACE_BEGIN(file)

auto read_to_image(const std::string_view filename) -> tl::expected<Image, std::string>;
// hardcode: rgba 8-bit
auto write_image(const std::string_view filename, const u8* data, u16 width, u16 height) -> void;

NAMESPACE_END(file)
NAMESPACE_END(io)
