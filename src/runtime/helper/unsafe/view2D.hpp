#pragma once

#ifdef USE_MODULES
import <algorithm>;
#else
#include <algorithm>
#endif

#include <runtime/helper/basic.hpp>

NAMESPACE_BEGIN(unsafe)

template <typename T, std::integral size_type = usize>
struct view2D
{
private:
    using value_type = std::remove_cvref_t<T>;
    using pointer = value_type*;
    using Self = view2D;

public:
    pointer ptr;
    size_type width, height;

public:
    view2D(pointer ptr, size_type width, size_type height) :
        ptr(ptr), width(width), height(height)
    {}

    auto begin() noexcept -> pointer
    {
        return ptr;
    }

    auto end() noexcept -> pointer
    {
        return &ptr[width * height];
    }

    auto get(usize w, usize h) const noexcept -> value_type
    {
        return ptr[w + h * width];
    }

    auto set(usize w, usize h, const value_type& value) noexcept -> Self&
    {
        ptr[w + h * width] = value;
        return *this;
    }

    auto fill(const value_type& value) noexcept -> void
    {
        std::ranges::fill(*this, value);
    }
};

NAMESPACE_END(unsafe)
