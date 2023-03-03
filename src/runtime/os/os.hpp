#pragma once

#ifdef USE_MODULES
import <string_view>;
import <string>;
import <vector>;
#else
#include <string_view>
#include <string>
#include <vector>
#endif

#include <runtime/helper/basic.hpp>

NAMESPACE_BEGIN(os)
NAMESPACE_BEGIN(detail)
// @note get unix timestamp (unit: ns)
auto time() -> i64;
// @note find file in path. example: "src/*.cpp" "src/**.cpp".
auto find_files(const std::string_view path) -> std::vector<std::string>;

NAMESPACE_END(detail)

using ::os::detail::time;
using ::os::detail::find_files;

NAMESPACE_END(os)
