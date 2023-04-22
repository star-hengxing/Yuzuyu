#include <core/base/range.hpp>
#include <resource/io.hpp>
#include "Font.hpp"

font_mananger::~font_mananger()
{
    if (face)
    {
        FT_Done_Face(face);
    }

    if (ft)
    {
        FT_Done_FreeType(ft);
    }
}

auto font_mananger::swap(Self& other) -> Self&
{
    using std::swap;
    swap(ft, other.ft);
    swap(face, other.face);
    swap(data, other.data);
    return *this;
}
// TODO: report error
auto font_mananger::render_line(const font_config& config) -> Image
{
    FT_Error error = FT_Set_Pixel_Sizes(face, 0, config.pixel_size);
    if (error)
    {
        perr("FT_Set_Pixel_Sizes failed\n");
    }

    FT_GlyphSlot slot = face->glyph;

    auto offset = isize{0};
    auto image = Image::create(config.pixel_size * config.text.size(), config.pixel_size);
    auto view = image.get_view();
    view.fill({0, 0, 0, 0});

    const auto pixel_size = static_cast<isize>(config.pixel_size);
    for (auto unicode : config.text)
    {
        error = FT_Load_Char(face, unicode, FT_LOAD_RENDER);
        if (error)
        {
            perrln("FT_Load_Char failed: ", char32_t{unicode});
            continue;
        }

        const auto bitmap = slot->bitmap;
        const auto width = std::min<usize>(bitmap.width, pixel_size);
        const auto height = std::min<usize>(bitmap.rows, pixel_size);
        if (width == 0 || height == 0)
        {
            offset += slot->advance.x >> 6;
            continue;
        }
        // TODO: use kerning
        auto font_view = unsafe::view2D<u8>{bitmap.buffer, width, height};
        const auto offset_x = std::min<isize>(pixel_size - static_cast<isize>(width), slot->bitmap_left) + offset;
        const auto offset_y = std::min<isize>(pixel_size - static_cast<isize>(height), pixel_size - slot->bitmap_top);

        if (bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
        {
            for (auto y : range(height))
            {
                for (auto x : range(width))
                {
                    const auto v = font_view.get(x, y);
                    if (v != 0)
                    {
                        view.set(x + offset_x, y + offset_y, config.color);
                    }
                }
            }
        }
        else
        {
            // TODO
        }
        offset += slot->advance.x >> 6;
    }
    return image;
}

NAMESPACE_BEGIN(io::file)

auto read_to_font(const std::string_view filename) -> font_mananger
{
    using namespace std::string_view_literals;

    FT_Error error;
    FT_Library ft;
    error = FT_Init_FreeType(&ft);
    if (error)
    {
        throw "FT_Init_FreeType failed"sv;
    }

    FT_Face face;
    auto file = fast_io::native_file_loader{filename};
    error = FT_New_Memory_Face(ft, reinterpret_cast<const FT_Byte*>(file.data()), file.size(), 0, &face);
    if (error)
    {
        FT_Done_FreeType(ft);
        throw "Failed to load font"sv;
    }

    return {ft, face, std::move(file)};
}

NAMESPACE_END(io::file)
