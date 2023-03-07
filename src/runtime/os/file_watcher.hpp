#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include <ghc/filesystem.hpp>

#include <runtime/helper/macro.hpp>

NAMESPACE_BEGIN(os)
NAMESPACE_BEGIN(detail)

namespace fs = ghc::filesystem;

struct file_watcher
{
public:
    fs::path file;
    fs::file_time_type mtime;

public:
    file_watcher(const std::string_view filename);

    auto is_changed() noexcept -> bool;
};

NAMESPACE_END(detail)

using ::os::detail::file_watcher;

NAMESPACE_END(os)
