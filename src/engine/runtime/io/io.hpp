#pragma once

#ifdef USE_MODULES
import <iostream>;
import <memory>;
import <tuple>;
#else
#include <iostream>
#include <memory>
#include <tuple>
#endif

#include <runtime/helper/basic.hpp>

NAMESPACE_BEGIN(io)

template <typename... Args>
auto println(const Args&... args) -> void
{
    ((std::cout << args << '\n'), ...);
}

template <typename... Args>
auto print(const Args&... args) -> void
{
    ((std::cout << args << ' '), ...);
}

NAMESPACE_BEGIN(file)
// read all data
auto read(const char* filename) -> std::tuple<std::unique_ptr<char[]>, usize>;

NAMESPACE_END(file)
NAMESPACE_END(io)
