#pragma once

#ifndef NAMESPACE_BEGIN
#define NAMESPACE_BEGIN(name) namespace name {
#endif

#ifndef NAMESPACE_END
#define NAMESPACE_END(name) }
#endif

#ifdef _MSC_VER
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE [[gnu::always_inline]]
#endif

#if defined(_WIN32) && defined(LIBRARY_DLL)
#define LIBRARY_EXPORT __declspec(dllexport)
#define LIBRARY_IMPORT __declspec(dllimport)
#else
#define LIBRARY_EXPORT
#define LIBRARY_IMPORT
#endif

#if defined(DEBUG) || defined(_DEBUG)
#define LIBRARY_DEBUG
#else
#define LIBRARY_RELEASE
#endif
