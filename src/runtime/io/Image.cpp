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

auto Image::write(const std::string_view filename) const -> std::string
{
    return io::file::write_image(filename, reinterpret_cast<u8*>(data.get()), width, height);
}

auto Image::get_view() const noexcept -> view
{
    return {data.get(), width, height};
}

auto Image::resize(u16 width, u16 height) -> std::string
{
    auto tmp = Image::create(width, height);
    const int result = stbir_resize_uint8(
        reinterpret_cast<u8*>(this->data.get()), this->width, this->height,
        0, reinterpret_cast<u8*>(tmp.data.get()), width, height, 0, 4);

    if (result == 0)
    {
        return fast_io::concat<std::string>(
            "Failed to resize image\n", stbi_failure_reason());
    }
    // get resized image
    this->swap(tmp);
    return {};
}

auto Image::swap(Self& other) noexcept -> Self&
{
    using std::swap;
    data.swap(other.data);
    swap(width, other.width);
    swap(height, other.height);
}

auto Image::create(u16 width, u16 height) -> Self
{
    return {new type[width * height], width, height};
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
        const auto info = fast_io::concat<std::string>(
            "Failed to read ", filename, "\n", stbi_failure_reason());
        return tl::make_unexpected(std::move(info));
    }

    auto image = Image::create(w, h);
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
        return tl::make_unexpected("No support image channel!\n");
    }

    return {std::move(image)};
}

auto write_image(const std::string_view filename, const u8* data, u16 width, u16 height)
    -> std::string
{
    int error;
    if (filename.ends_with(".png"))
    {
        error = stbi_write_png(filename.data(), width, height, 4, data, 0);
    }
    else if (filename.ends_with(".jpg"))
    {
        error = stbi_write_jpg(filename.data(), width, height, 4, data, 0);
    }
    else if (filename.ends_with(".ppm"))
    {
        // TODO: https://en.wikipedia.org/wiki/Time-of-check_to_time-of-use
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
        return {};
    }
    else
    {
        return "No support image format!";
    }

    return (error == 1) ? "" : stbi_failure_reason();
}

NAMESPACE_END(file)
NAMESPACE_END(io)
