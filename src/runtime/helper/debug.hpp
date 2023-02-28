#pragma once
// see https://github.com/llvm/llvm-project/issues/53906
#ifdef __clang__
#if __clang_major__ <= 15
#define __cpp_consteval 201811
#endif
#endif
#include <source_location>
#include <cstdlib>

#if __has_include(<fast_io.h>)
#include <fast_io.h>
#else
#include <cstdio>
#endif

#include "marco.hpp"

NAMESPACE_BEGIN(debug)

inline auto print_error_info(bool expression, const char* string,
    const std::source_location location) noexcept -> void
{
#if __has_include(<fast_io.h>)
    perrln("[", fast_io::mnp::os_c_str(location.file_name()),
        ":", location.line(),
        "] ", fast_io::mnp::os_c_str(location.function_name()),
        ": ", fast_io::mnp::os_c_str(string));
#else
    fprintf(stderr, "[%s:%d] %s: %s\n",
        location.file_name(), location.line(), location.function_name(), string);
#endif
}

inline auto assert(bool expression, const char* string,
    const std::source_location location) noexcept -> void
{
    if (expression) [[likely]]
        return;

    print_error_info(expression, string, location);
    std::exit(-1);
}

NAMESPACE_END(debug)

#ifdef LIBRARY_DEBUG
#define Assert(expr) debug::assert(expr, #expr, std::source_location::current())
#else
#define Assert(expr)
#endif
