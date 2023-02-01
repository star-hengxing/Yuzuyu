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

    auto size() const noexcept -> usize
    {
        return width * height;
    }

    auto end() noexcept -> pointer
    {
        return &ptr[width * height];
    }

    auto get(usize x, usize y) const noexcept -> value_type
    {
        return ptr[x + y * width];
    }

    auto set(usize x, usize y, const value_type& value) noexcept -> Self&
    {
        ptr[x + y * width] = value;
        return *this;
    }

    auto fill(const value_type& value) noexcept -> void
    {
        std::ranges::fill(*this, value);
    }
};

NAMESPACE_END(unsafe)
