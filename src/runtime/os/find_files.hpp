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

#include <runtime/base/macro.hpp>

NAMESPACE_BEGIN(os)
// @note find file in path. example: "src/*.cpp" "src/**.cpp".
auto find_files(const std::string_view path) -> std::vector<std::string>;

NAMESPACE_END(os)
