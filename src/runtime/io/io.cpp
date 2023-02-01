#include <fast_io.h>

#include <runtime/helper/range.hpp>
#include "io.hpp"

NAMESPACE_BEGIN(io)
NAMESPACE_BEGIN(file)

// TODO: async read
auto read_to_buffer(const std::string_view filename) -> fixed_buffer
{
    const auto file = fast_io::native_file_loader{filename};
    const auto size = file.size();
    auto buffer = new char[size];
    for (auto i : range(size))
    {
        buffer[i] = file[i];
    }
    return {Owned<char[]>{buffer}, size};
}

NAMESPACE_END(file)
NAMESPACE_END(io)
