#include <avir.h>

#include "Image.hpp"

auto Image::create(u32 width, u32 height) noexcept -> Self
{
    return {new type[width * height], width, height};
}

auto Image::write(const std::string_view filename) const noexcept -> bool
{
    return io::file::write_image(filename, reinterpret_cast<u8*>(data.get()), width, height);
}

auto Image::get_view() const noexcept -> view
{
    return {data.get(), width, height};
}

auto Image::swap(Self& other) noexcept -> Self&
{
    using std::swap;
    data.swap(other.data);
    swap(width, other.width);
    swap(height, other.height);
    return *this;
}

auto Image::resize(u32 width, u32 height) noexcept -> void
{
    if (width == 0 || height == 0) [[unlikely]]
        return;

    auto tmp = Image::create(width, height);
    // 8 bit depth
    avir::CImageResizer{8, 8}.resizeImage(
        reinterpret_cast<u8*>(get_view().ptr), this->width, this->height,
        0,
        reinterpret_cast<u8*>(tmp.get_view().ptr), width, height,
        sizeof(type), 0, nullptr);

    // get resized image
    this->swap(tmp);
}
