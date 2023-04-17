#include <ghc/filesystem.hpp>

#include "find_files.hpp"

NAMESPACE_BEGIN(os)

namespace fs = ghc::filesystem;

auto find_files(const std::string_view str) -> std::vector<std::string>
{
    std::string_view dir;
    std::string_view extension;
    // match current directory files?
    auto index = str.rfind('*');
    if (index != std::string_view::npos)
    {
        extension = str.substr(index + 1);
    }
    // is current directory?
    index = str.rfind('/');
    if (index != std::string_view::npos)
        dir = str.substr(0, index);

    if (dir.empty())
        dir = ".";

    std::vector<std::string> files;
    // recursive match files?
    const auto is_recursive = (str.rfind("**") != std::string_view::npos);

    if (is_recursive)
    {
        for (const auto& entry : fs::recursive_directory_iterator{dir})
        {
            if (entry.is_regular_file() && entry.path().extension() == extension)
            {
                files.push_back(entry.path().string());
            }
        }
    }
    else
    {
        for (const auto& entry : fs::directory_iterator{dir})
        {
            if (entry.is_regular_file() && entry.path().extension() == extension)
            {
                files.push_back(entry.path().string());
            }
        }
    }
    return files;
}

NAMESPACE_END(detail)
