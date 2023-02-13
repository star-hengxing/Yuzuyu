#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <fast_io.h>

#include <runtime/helper/range.hpp>
#include <runtime/io/Image.hpp>
#include <runtime/io/io.hpp>

TEST_CASE("image read/write")
{
    constexpr auto size = 4;
    constexpr Color colors[size]
    {
        color_space::red,
        color_space::green,
        color_space::blue,
        color_space::yellow
    };

    using namespace std::string_view_literals;
    constexpr auto filename = "build/test.png"sv;

    io::file::write_image(filename, (const u8*)colors, 2, 2);

    const auto image = io::file::read_to_image(filename.data());
    SUBCASE("write")
    {
        if (!image.has_value())
        {
            REQUIRE(image.has_value());
            perrln(image.error());
        }
    }

    const auto view = image->get_view();
    SUBCASE("read")
    {
        REQUIRE(view.ptr);
        REQUIRE(view.width  == 2);
        REQUIRE(view.height == 2);
        for (auto i : range(size))
        {
            CHECK(colors[i] == view.ptr[i]);
        }
    }
}
