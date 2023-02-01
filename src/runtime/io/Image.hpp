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

private:
    Owned<type[]> data;
    u32 width, height;

public:
    // get data ownership
    Image(type* data, u32 width, u32 height) :
        data(data), width(width), height(height)
    {}

    auto write(const std::string_view filename) const -> void;

    auto get_view() const noexcept -> unsafe::view2D<type>;
};

NAMESPACE_BEGIN(io)
NAMESPACE_BEGIN(file)

auto read_to_image(const std::string_view filename) -> tl::expected<Image, std::string>;
// hardcode: rgba 8-bit
auto write_image(const std::string_view filename, const u8* data, u32 width, u32 height) -> void;

NAMESPACE_END(file)
NAMESPACE_END(io)
