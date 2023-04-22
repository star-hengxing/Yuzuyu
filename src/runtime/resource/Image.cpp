#include <png.h>
#include <turbojpeg.h>
#include <fast_io.h>
#include <fast_io_device.h>

#include <core/base/unsafe/buffer_view.hpp>
#include <core/base/range.hpp>
#include "Image.hpp"
#include "io.hpp"

using namespace std::string_view_literals;

NAMESPACE_BEGIN()
// File Magic Numbers
// https://gist.github.com/leommoore/f9e57ba2aa4bf197ebc5
constexpr u8 PNG_MAGIC_NUMBERS[]{0x89, 0x50, 0x4e, 0x47};
constexpr u8 JPEG_MAGIC_NUMBERS[]{0xff, 0xd8, 0xff, 0xe0};
constexpr auto MAX_MAGIC_NUMBERS_SIZE = usize{4};

auto read_png(const fast_io::native_file_loader file) -> Image
{
    auto read_image = png_image
    {
        .version = PNG_IMAGE_VERSION,
        .format  = PNG_FORMAT_RGBA,
    };

    if (png_image_begin_read_from_memory(&read_image, file.data(), file.size()) == 0)
    {
        throw "Can't parse png file"sv;
    }

    auto image = Image::create(read_image.width, read_image.height);
    if (png_image_finish_read(&read_image, nullptr, image.get_view().data(), 0, nullptr) == 0) 
    {
        throw "Can't read png file"sv;
    }
    return image;
    // which way is fast and safe?

    // auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    // if (!png_ptr)
    // {
    //     throw "png_create_read_struct error"sv;
    // }

    // defer { png_destroy_read_struct(&png_ptr, nullptr, nullptr); };

    // auto info_ptr = png_create_info_struct(png_ptr);
    // if (!png_ptr)
    // {
    //     throw "png_create_info_struct error"sv;
    // }

    // {
    //     auto view = unsafe::buffer_view{file.data.get() + PNG_SIG_SIZE, file.size};
    //     png_set_read_fn(png_ptr, &view,
    //         [](png_structp png_ptr, png_bytep png_data, png_size_t size) noexcept
    //     {
    //         auto buffer = static_cast<decltype(view)*>(png_get_io_ptr(png_ptr));
    //         for (auto i : range(size))
    //         {
    //             png_data[i] = (*buffer)[i];
    //         }

    //         buffer->begin_ptr += size;
    //         if (buffer->begin_ptr >= buffer->end())
    //         {
    //             png_error(png_ptr, "Read Error!");
    //             // TODO
    //         }
    //     });
    // }
    // png_set_sig_bytes(png_ptr, PNG_SIG_SIZE);
    // png_read_info(png_ptr, info_ptr);

    // const auto width      = png_get_image_width(png_ptr, info_ptr);
    // const auto height     = png_get_image_height(png_ptr, info_ptr);
    // const auto bit        = png_get_bit_depth(png_ptr, info_ptr);
    // const auto channels   = png_get_channels(png_ptr, info_ptr);
    // const auto color_type = png_get_color_type(png_ptr, info_ptr);

    // if (bit != 8 || channels != 4 || color_type != PNG_COLOR_TYPE_RGBA)
    // {
    //     throw "Unsupported png format"sv;
    // }

    // auto image = Image::create(width, height);
    // auto view = image.get_view();

    // auto tmp_row = Owned<png_bytep[]>::make(height);
    // for (const auto stride = width * 4; auto i : range(height))
    // {
    //     tmp_row[i] = &view.data()->r + i * stride;
    // }

    // png_read_image(png_ptr, tmp_row.get());
    // png_read_end(png_ptr, nullptr);
    // return image;
}
// TODO: report error
auto write_png(const std::string_view filename, const u8* data, u32 width, u32 height) -> bool
{
    auto write_image = png_image
    {
        .version = PNG_IMAGE_VERSION,
        .width   = width,
        .height  = height,
        .format  = PNG_FORMAT_RGBA,
    };

    auto size = usize{};
    if (png_image_write_to_memory(&write_image, nullptr, &size, 0, data, 0, nullptr) == 0)
        return false;
    
    auto buffer = Owned<char[]>::make(size);
    if (png_image_write_to_memory(&write_image, buffer.get(), &size, 0, data, 0, nullptr) == 0)
        return false;

    auto file = fast_io::native_file{filename, fast_io::open_mode::out};
    print(file, std::string_view{buffer.get(), size});
    return true;
}

auto read_jpg(const fast_io::native_file_loader file) -> Image
{
    auto handle = tjInitDecompress();
    if (!handle)
    {
        throw std::string_view{tjGetErrorStr2(handle)};
    }

    defer { tjDestroy(handle); };

    int width = 0;
    int height = 0;
    int subSampling = 0;
    int color_space = 0;
    if (tjDecompressHeader3(handle,
        reinterpret_cast<const unsigned char*>(file.data()), file.size(),
        &width, &height, &subSampling, &color_space) != 0)
    {
        throw std::string_view{tjGetErrorStr2(handle)};
    }

    auto image = Image::create(width, height);
    constexpr auto format = TJPF_RGBA;
    constexpr auto flags = TJFLAG_NOREALLOC | TJFLAG_FASTDCT;

    if (tjDecompress2(handle, reinterpret_cast<const unsigned char*>(file.data()), file.size(),
        reinterpret_cast<unsigned char*>(image.get_view().data()),
        width, 0, height, format, flags) != 0)
    {
        throw std::string_view{tjGetErrorStr2(handle)};
    }
    return image;
}

auto write_jpg(const std::string_view filename, const u8* data, u32 width, u32 height) -> bool
{
    constexpr auto quality     = 5;
    constexpr auto format      = TJPF_RGBA;
    constexpr auto subsampling = TJSAMP_420;
    constexpr auto flags       = TJFLAG_NOREALLOC | TJFLAG_FASTDCT;

    auto handle = tjInitCompress();
    if (!handle)
    {
        perrln(fast_io::mnp::os_c_str(tjGetErrorStr2(handle)));
        return false;
    }

    defer { tjDestroy(handle); };

    auto size   = tjBufSize(width, height, subsampling);
    auto buffer = Owned<char[]>::make(size);
    auto tmp    = reinterpret_cast<unsigned char*>(buffer.get());

    if (tjCompress2(handle, data,
        width, 0, height, format, &tmp,
        &size, subsampling, quality, flags) != 0)
    {
        perrln(fast_io::mnp::os_c_str(tjGetErrorStr2(handle)));
        return false;
    }

    auto file = fast_io::native_file{filename, fast_io::open_mode::out};
    print(file, std::string_view{buffer.get(), size});
    return true;
}

auto write_ppm(const std::string_view filename, const u8* data, u32 width, u32 height) -> bool
{
    auto file = fast_io::native_file{filename, fast_io::open_mode::out};
    // P3 string format, P6 binary format, default P3
    // 255 mean: 8 bit color
    // P3
    // width height
    // 255
    print(file, "P3\n", width, " ", height, "\n255\n");
    for (auto i : range(width * height))
    {
        const auto index = i * 4;
        // r g b
        println(file, data[index], " ", data[index + 1], " ", data[index + 2]);
    }
    return true;
}

NAMESPACE_END()

NAMESPACE_BEGIN(io::file)

auto read_to_image(const std::string_view filename) -> Image
{
    auto file = fast_io::native_file_loader{filename};
    if (file.size() <= MAX_MAGIC_NUMBERS_SIZE)
    {
        throw "Invalid image file"sv;
    }

    const auto is_png = (*reinterpret_cast<u32*>(file.data()) == *reinterpret_cast<const u32*>(PNG_MAGIC_NUMBERS));
    const auto is_jpg = (*reinterpret_cast<u32*>(file.data()) == *reinterpret_cast<const u32*>(JPEG_MAGIC_NUMBERS));

    if (is_png)
    {
        return read_png(std::move(file));
    }

    if (is_jpg)
    {
        return read_jpg(std::move(file));
    }

    perr("Only support jpeg and png format\n");
    return {nullptr, 0, 0};
}

auto write_image(const std::string_view filename, const u8* data, u32 width, u32 height)
     noexcept -> bool
{
    if (filename.ends_with(".png"))
    {
        return write_png(filename, data, width, height);
    }
    else if (filename.ends_with(".jpg"))
    {
        return write_jpg(filename, data, width, height);
    }
    else if (filename.ends_with(".ppm"))
    {
        return write_ppm(filename, data, width, height);
    }
    else
    {
        perr("Unsupported image format!\n");
        return false;
    }
}

NAMESPACE_END(io::file)
