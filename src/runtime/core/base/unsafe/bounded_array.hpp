#pragma once

#ifdef USE_MODULES
import <type_traits>;
#else
#include <type_traits>
#endif

#include "../base.hpp"

NAMESPACE_BEGIN(unsafe)

template <typename T, usize MAX_SIZE>
requires std::is_standard_layout_v<T>
struct bounded_array
{
protected:
    using value_type = T;
    using pointer = T*;
    using const_pointer = T* const;

public:
    usize size_ = 0;
    T array_[MAX_SIZE];

public:
    constexpr auto push_back(value_type value) noexcept -> void
    {
        array_[size_] = value;
        size_ += 1;
    }

    constexpr auto pop() noexcept -> value_type
    {
        size_ -= 1;
        return array_[size_];
    }

    constexpr auto clear() noexcept -> void
    {
        size_ = 0;
    }

    constexpr auto data() const noexcept -> const_pointer
    {
        return const_cast<const_pointer>(array_);
    }

    constexpr auto data() noexcept -> pointer
    {
        return array_;
    }

    constexpr auto empty() const noexcept -> bool
    {
        return size_ == 0;
    }

    constexpr auto size() const noexcept -> usize
    {
        return size_;
    }

    constexpr auto capacity() const noexcept -> usize
    {
        return MAX_SIZE;
    }

    constexpr auto cbegin() const noexcept -> const_pointer
    {
        return data();
    }

    constexpr auto cend() const noexcept -> const_pointer
    {
        return data() + MAX_SIZE;
    }

    constexpr auto begin() const noexcept -> const_pointer
    {
        return data();
    }

    constexpr auto end() const noexcept -> const_pointer
    {
        return data() + MAX_SIZE;
    }

    constexpr auto begin() noexcept -> pointer
    {
        return data();
    }

    constexpr auto end() noexcept -> pointer
    {
        return data() + MAX_SIZE;
    }

    constexpr auto operator [] (usize index) const noexcept -> value_type
    {
        return array_[index];
    }
};

NAMESPACE_END(unsafe)
