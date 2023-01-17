// do not edit!
#pragma once
#include <string_view>
#if defined(_WIN32) && defined(LIBRARY_DLL)
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
