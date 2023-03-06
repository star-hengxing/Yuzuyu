#pragma once

#ifdef __clang__
#if __clang_major__ <= 15
#define __cpp_consteval 201811
#endif
#endif
#include <source_location>

#include <volk.h>

auto check_vulkan_result(VkResult result, const std::source_location location) -> void;

#define CHECK_RESULT(result) check_vulkan_result(result, std::source_location::current())
