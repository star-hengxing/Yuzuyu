#pragma once

#ifdef USE_MODULES
import <string_view>;
import <vector>;
#else
#include <string_view>
#include <vector>
#endif

#include <runtime/helper/unsafe/bounded_array.hpp>

NAMESPACE_BEGIN(script::detail)

static constexpr auto MAX_TOKEN_SIZE = usize{64};

template <usize MAX_SIZE>
using fixed_string = unsafe::bounded_array<char, MAX_SIZE>;

struct string_pool_view
{
public:
    usize start;
    usize size;

public:
    auto get_view(const std::vector<char>& pool) const noexcept
        -> std::string_view
    {
        return {&pool[start], size};
    }
};

template <usize MAX_SIZE>
auto get_view(const fixed_string<MAX_SIZE>& string) noexcept -> std::string_view
{
    return {string.data(), string.size()};
}

NAMESPACE_END(script::detail)
