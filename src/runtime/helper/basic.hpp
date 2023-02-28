#pragma once

#include "marco.hpp"
#include "basic_type.hpp"

NAMESPACE_BEGIN(global)

inline constexpr bool is_debug
{
#ifdef LIBRARY_DEBUG
    true
#else
    false
#endif
};

inline constexpr bool is_release = !is_debug;

NAMESPACE_END(global)
