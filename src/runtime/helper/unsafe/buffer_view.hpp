#pragma once

#ifdef USE_MODULES
import <iterator>;
#else
#include <iterator>
#endif

#include <runtime/helper/basic.hpp>

NAMESPACE_BEGIN(unsafe)

template <typename T>
struct buffer_view
{
private:
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_pointer = T* const;

public:
    pointer begin_ptr{}, end_ptr{};

public:
    constexpr buffer_view() noexcept = default;

    template <std::contiguous_iterator U>
    requires std::same_as<std::iter_value_t<U>, value_type>
    constexpr buffer_view(U first, U last) noexcept :
        begin_ptr{std::to_address(first)},
        end_ptr{std::to_address(last)} {}

    template <std::ranges::contiguous_range U>
    requires std::same_as<std::ranges::range_value_t<U>,
        value_type>
    explicit constexpr buffer_view(U& r) noexcept :
        buffer_view{std::ranges::begin(r), std::ranges::end(r)} {}

    constexpr buffer_view(pointer begin, usize size) noexcept :
        begin_ptr{begin}, end_ptr{begin + size} {}

    constexpr auto cbegin() const noexcept -> const_pointer
    {
        return begin_ptr;
    }

    constexpr auto cend() const noexcept -> const_pointer
    {
        return end_ptr;
    }

    constexpr auto begin() const noexcept -> const_pointer
    {
        return begin_ptr;
    }

    constexpr auto end() const noexcept -> const_pointer
    {
        return end_ptr;
    }

    constexpr auto begin() noexcept -> pointer
    {
        return begin_ptr;
    }

    constexpr auto end() noexcept -> pointer
    {
        return end_ptr;
    }

    constexpr auto data() noexcept -> pointer
    {
        return begin_ptr;
    }

    constexpr auto size() const noexcept -> usize
    {
        return static_cast<usize>(end_ptr - begin_ptr);
    }

    constexpr auto empty() const noexcept -> bool
    {
        return !begin_ptr || !end_ptr;
    }

    constexpr auto operator [] (usize index) const noexcept -> value_type
    {
        return begin_ptr[index];
    }

    constexpr auto operator [] (usize index) noexcept -> reference
    {
        return begin_ptr[index];
    }
};

NAMESPACE_END(unsafe)
