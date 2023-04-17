#include <boost/ut.hpp>
#include <fast_io.h>

#include <core/base/range.hpp>
#include <resource/Image.hpp>
#include <resource/io.hpp>

using namespace boost::ut;

auto main() -> int
{
    constexpr auto size = 4;
    constexpr Color colors[size]
    {
        color_space::red,
        color_space::green,
        color_space::blue,
        color_space::yellow
    };

    constexpr auto filename = std::string_view{"build/test.png"};
    io::file::write_image(filename, (const u8*)colors, 2, 2);

    const auto image = io::file::read_to_image(filename.data());
    "image_write"_test = [&]
    {
        if (!image.has_value())
        {
            expect(image.has_value());
            perrln(image.error());
        }
    };

    const auto view = image->get_view();
    "image_read"_test = [&]
    {
        expect(view.ptr);
        expect(view.width == 2);
        expect(view.height == 2);
        for (auto i : range(size))
        {
            expect(colors[i] == view.ptr[i]);
        }
    };
}
