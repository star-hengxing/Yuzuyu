#include <fast_io.h>

#include <core/base/range.hpp>
#include "io.hpp"

NAMESPACE_BEGIN(io::file)

// TODO: async read
auto read_to_buffer(const std::string_view filename, bool has_eof)
    -> fixed_buffer
{
    const auto file = fast_io::allocation_file_loader{filename};
    const auto size = file.size();

    auto buffer = Owned<char[]>::make_uninitialize(size + (has_eof ? 1 : 0));
    for (auto i : range(size))
    {
        buffer[i] = file[i];
    }

    if (has_eof)
    {
        buffer[size] = '\0';
    }
    return {std::move(buffer), size};
}

NAMESPACE_END(io::file)
