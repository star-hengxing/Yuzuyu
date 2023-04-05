#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include <runtime/base/Owned.hpp>
#include <runtime/base/base.hpp>

NAMESPACE_BEGIN(io)
NAMESPACE_BEGIN(file)

struct fixed_buffer
{
    Owned<char[]> data;
    usize size;
};

// @note read all data to buffer
// @param has_eof set '\0' end of file
auto read_to_buffer(const std::string_view filename, bool has_eof = false)
    -> fixed_buffer;

NAMESPACE_END(file)
NAMESPACE_END(io)
