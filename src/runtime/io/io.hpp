#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include <tl/expected.hpp>

#include <runtime/helper/Owned.hpp>
#include <runtime/helper/basic.hpp>

NAMESPACE_BEGIN(io)
NAMESPACE_BEGIN(file)

struct fixed_buffer
{
    Owned<char[]> data;
    usize size;
};

// read all data to buffer
auto read_to_buffer(const std::string_view filename) -> tl::expected<fixed_buffer, std::string>;

NAMESPACE_END(file)
NAMESPACE_END(io)
