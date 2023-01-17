#ifdef USE_MODULES
import <fstream>;
#else
#include <fstream>
#endif

#include "io.hpp"

NAMESPACE_BEGIN(io)
NAMESPACE_BEGIN(file)

// TODO: async read
auto read(const char* filename) -> std::tuple<std::unique_ptr<char[]>, usize>
{
    std::ifstream in(filename, std::ios::ate | std::ios::binary);
    if (in.fail()) return {nullptr, 0};

    const usize size = in.tellg();
    in.seekg(0, std::ios::beg);
    // avoid set 0
    auto buffer = std::unique_ptr<char[]>(new char[size]);
    in.read(buffer.get(), size);

    in.close();
    return {std::move(buffer), size};
}

NAMESPACE_END(file)
NAMESPACE_END(io)
