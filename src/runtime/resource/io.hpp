#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include <core/base/Owned.hpp>
#include <core/base/base.hpp>

NAMESPACE_BEGIN(io::file)

struct fixed_buffer
{
    Owned<char[]> data;
    usize size;
};

// @note read all data to buffer
// @param has_eof set '\0' end of file
auto read_to_buffer(const std::string_view filename, bool has_eof = false)
    -> fixed_buffer;

NAMESPACE_END(io::file)
