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
    using pointer = T*;
    using const_pointer = T* const;
    using Self = view2D;

public:
    pointer ptr;
    size_type width, height;

public:
    constexpr view2D(pointer ptr, size_type width, size_type height) noexcept :
        ptr(ptr), width(width), height(height) {}

    constexpr auto cbegin() const noexcept -> const_pointer
    {
        return ptr;
    }

    constexpr auto cend() const noexcept -> const_pointer
    {
        return &ptr[width * height];
    }

    constexpr auto begin() const noexcept -> const_pointer
    {
        return ptr;
    }

    constexpr auto end() const noexcept -> const_pointer
    {
        return &ptr[width * height];
    }

    constexpr auto begin() noexcept -> pointer
    {
        return ptr;
    }

    constexpr auto end() noexcept -> pointer
    {
        return &ptr[width * height];
    }

    constexpr auto data() noexcept -> pointer
    {
        return ptr;
    }

    constexpr auto size() const noexcept -> size_type
    {
        return width * height;
    }

    constexpr auto empty() const noexcept -> bool
    {
        return !ptr || width == 0 || height == 0;
    }

    constexpr auto get(usize x, usize y) const noexcept -> value_type
    {
        return ptr[x + y * width];
    }

    constexpr auto set(usize x, usize y, const value_type& value) noexcept -> Self&
    {
        ptr[x + y * width] = value;
        return *this;
    }

    constexpr auto fill(const value_type& value) noexcept -> void
    {
        std::ranges::fill(*this, value);
    }
};

NAMESPACE_END(unsafe)
