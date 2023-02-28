#include <ghc/filesystem.hpp>

#include "os.hpp"

NAMESPACE_BEGIN(os)
NAMESPACE_BEGIN(detail)

namespace fs = ghc::filesystem;

NAMESPACE_BEGIN()

auto recursive_find_files(std::vector<std::string>& files,
    const std::string_view dir, const std::string_view extension,
    bool is_recursive) -> void
{
    for (const auto& entry : fs::directory_iterator(dir))
    {
        if (entry.is_directory() && is_recursive)
        {
            recursive_find_files(files, entry.path().string(), extension, is_recursive);
        }

        if (entry.path().extension() == extension)
        {
            files.push_back(entry.path().string());
        }
    }
}

NAMESPACE_END()

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

    recursive_find_files(files, dir, extension, is_recursive);
    return files;
}

NAMESPACE_END(detail)
NAMESPACE_END(os)
