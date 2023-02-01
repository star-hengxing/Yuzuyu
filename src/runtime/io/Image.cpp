#ifdef USE_MODULES
import <string>;
#else
#include <string>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>
#include <fast_io.h>

#include <runtime/helper/range.hpp>
#include "Image.hpp"

auto Image::write(const std::string_view filename) const -> void
{
    io::file::write_image(filename, reinterpret_cast<u8*>(data.get()), width, height);
}

auto Image::get_view() const noexcept -> view
{
    return {data.get(), width, height};
}

auto Image::resize(u32 width, u32 height) -> void
{
    auto data = Owned<type[]>(new type[width * height]);
    int result = stbir_resize_uint8(
        reinterpret_cast<u8*>(this->data.get()), this->width, this->height,
        0, reinterpret_cast<u8*>(data.get()), width, height, 0, 4);
    // success
    if (result == 1)
    {
        data.swap(this->data);
        this->width = width;
        this->height = height;
        return;
    }
    perr("Failed to resize image");
}

NAMESPACE_BEGIN(io)
NAMESPACE_BEGIN(file)

auto read_to_image(const std::string_view filename) -> tl::expected<Image, std::string>
{
    const auto file = fast_io::native_file_loader{filename};

    int w, h, channel;
    const stbi_uc* buffer = stbi_load_from_memory(
        reinterpret_cast<const stbi_uc*>(file.data()),
        file.size(), &w, &h, &channel, 0);

    if (!buffer)
    {
        const auto info = fast_io::concat<std::string>("Failed read ", filename);
        return tl::make_unexpected(std::move(info));
    }

    auto image = Image{new Image::type[w * h], static_cast<u32>(w), static_cast<u32>(h)};
    auto view = image.get_view();

    if (channel == 4)
    {
        for (auto i : range(w * h))
        {
            view.ptr[i] = reinterpret_cast<const Image::type*>(buffer)[i];
        }
    }
    else if (channel == 3)
    {
        for (auto&& [index, color] : enumerate(view, 3))
        {
            color = Color{buffer[index], buffer[index + 1], buffer[index + 2]};
        }
    }
    else
    {
        perr("No support image channel!\n");
    }

    return {std::move(image)};
}

auto write_image(const std::string_view filename, const u8* data, u32 width, u32 height) -> void
{
    if (filename.ends_with(".png"))
    {
        stbi_write_png(filename.data(), width, height, 4, data, 0);
    }
    else if (filename.ends_with(".jpg"))
    {
        stbi_write_jpg(filename.data(), width, height, 4, data, 0);
    }
    else if (filename.ends_with(".ppm"))
    {
        // TODO: https://en.wikipedia.org/wiki/Time-of-check_to_time-of-use
        auto file = fast_io::native_file{filename, fast_io::open_mode::out};
        // P3 string format, P6 binary format, default P3
        print(file, "P3\n", width, " ", height, "\n255\n");
        for (auto i : range(width * height))
        {
            const auto index = i * 4;
            println(file, data[index], " ", data[index + 1], " ", data[index + 2]);
        }
    }
    else
    {
        perr("No support format!\n");
    }
}

NAMESPACE_END(file)
NAMESPACE_END(io)
