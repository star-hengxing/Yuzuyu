#include <boost/ut.hpp>
#include <fast_io.h>

#include <core/base/range.hpp>
#include <resource/Image.hpp>

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

    "png write and read"_test = [&]
    {
        constexpr auto filename = std::string_view{"build/test.png"};
        expect(io::file::write_image(filename, (const u8*)colors, 2, 2));

        try
        {
            auto image = io::file::read_to_image(filename);
            auto view = image.get_view();
            expect(view.ptr);
            expect(view.width != 0);
            expect(view.height != 0);

            expect(view.width == 2);
            expect(view.height == 2);
            for (auto i : range(size))
            {
                expect(colors[i] == view.ptr[i]);
            }
        }
        catch (const std::string_view msg)
        {
            perrln(msg);
        }
    };

    "jpg write and read"_test = [&]
    {
        constexpr auto filename = std::string_view{"build/test.jpg"};
        expect(io::file::write_image(filename, (const u8*)colors, 2, 2));

        try
        {
            auto image = io::file::read_to_image(filename);
            auto view = image.get_view();
            expect(view.ptr);
            expect(view.width != 0);
            expect(view.height != 0);

            expect(view.width == 2);
            expect(view.height == 2);
        }
        catch (const std::string_view msg)
        {
            perrln(msg);
        }
    };

    "image resize"_test = [&]
    {
        constexpr auto filename = std::string_view{"build/test.jpg"};
        try
        {
            auto image = io::file::read_to_image(filename);
            image.resize(700, 700);
        }
        catch (const std::string_view msg)
        {
            perrln(msg);
        }
    };
}
