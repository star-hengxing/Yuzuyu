#include "file_watcher.hpp"

NAMESPACE_BEGIN(os)
NAMESPACE_BEGIN(detail)

file_watcher::file_watcher(const std::string_view filename) :
    entry{filename}, mtime{fs::last_write_time(entry)} {}

auto file_watcher::is_changed() noexcept -> bool
{
    auto now = fs::last_write_time(entry);
    if (now > mtime)
    {
        mtime = now;
        return true;
    }
    return false;
}

NAMESPACE_END(detail)
NAMESPACE_END(os)
