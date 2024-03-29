#pragma once

#ifdef __clang__
#define __cpp_consteval 201811
#endif
#include <source_location>

#ifdef USE_VOLK
#include <volk.h>
#else
#include <vulkan/vulkan.h>
#endif

#include <core/base/base.hpp>

auto check_vulkan_result(VkResult result, const std::source_location location) -> void;

#define CHECK_RESULT(result) check_vulkan_result(result, std::source_location::current())

inline constexpr auto TIME_OUT = u64{0xFFFFFFFFFFFFFFFF};

inline constexpr auto MAJOR_VERSION = usize{1};
inline constexpr auto MINOR_VERSION = usize{3};
inline constexpr auto PATCH_VERSION = usize{0};
