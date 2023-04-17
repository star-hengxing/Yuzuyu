#pragma once

#include "macro.hpp"
#include "basic_type.hpp"

NAMESPACE_BEGIN(global)

inline constexpr auto is_debug
{
#ifdef LIBRARY_DEBUG
    true
#else
    false
#endif
};

inline constexpr auto is_release = !is_debug;

#if __cplusplus <= 202202L

[[noreturn]] inline auto unreachable()
{
// https://en.cppreference.com/w/cpp/utility/unreachable
#ifdef __GNUC__ // GCC, Clang, ICC
    __builtin_unreachable();
#else
#ifdef _MSC_VER // MSVC
    __assume(false);
#endif
#endif
}

#endif

NAMESPACE_BEGIN(config)

inline constexpr auto NAME = "Yuzuyu";

inline constexpr auto DEFAULT_WIDTH  = usize{1280};
inline constexpr auto DEFAULT_HEIGHT = usize{720};
inline constexpr auto WINDOW_TITLE   = NAME;

NAMESPACE_END(config)

NAMESPACE_END(global)
