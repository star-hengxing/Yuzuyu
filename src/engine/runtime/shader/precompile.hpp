// do not edit!
#pragma once
#include <string_view>
#ifdef _WIN32
#define LIBRARY_IMPORT __declspec(dllimport)
#else
#define LIBRARY_IMPORT
#endif
namespace shader
{
LIBRARY_IMPORT
extern std::string_view vs;
LIBRARY_IMPORT
extern std::string_view fs;
}
