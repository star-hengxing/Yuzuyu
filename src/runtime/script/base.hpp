#pragma once

#ifdef USE_MODULES
import <string_view>;
import <vector>;
#else
#include <string_view>
#include <vector>
#endif

#include <runtime/base/unsafe/bounded_array.hpp>

NAMESPACE_BEGIN(script::detail)

static constexpr auto MAX_TOKEN_SIZE = usize{64};
static constexpr auto MAX_LINE_TOKEN = usize{4};

static constexpr std::string_view KEYWORDS[]
{
    "scene",
    "goto",
    "set",
    "end",
};

auto is_keyword(const std::string_view key) noexcept -> bool;
// template <usize MAX_SIZE>
// using fixed_string = unsafe::bounded_array<char, MAX_SIZE>;

// template <usize MAX_SIZE>
// auto get_view(const fixed_string<MAX_SIZE>& string) noexcept -> std::string_view
// {
//     return {string.data(), string.size()};
// }

struct string_pool_view
{
public:
    usize start;
    usize size;

public:
    auto empty() const noexcept -> bool
    {
        return size == 0;
    }

    auto get_view(const std::vector<char>& pool) const noexcept
        -> std::string_view
    {
        return {&pool[start], size};
    }
};

NAMESPACE_END(script::detail)
