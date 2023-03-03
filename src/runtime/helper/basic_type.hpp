#pragma once

#ifdef USE_MODULES
import <cstdint>;
import <cstddef>;
#else
#include <cstdint>
#include <cstddef>
#endif

using u8  = std::uint_least8_t;
using u16 = std::uint_least16_t;
using u32 = std::uint_least32_t;
using u64 = std::uint_least64_t;

using i8  = std::int_least8_t;
using i16 = std::int_least16_t;
using i32 = std::int_least32_t;
using i64 = std::int_least64_t;

using usize = std::size_t;
using isize = std::ptrdiff_t;

using f32 = float;
using f64 = double;

static_assert(sizeof(u8)  >= 1 && sizeof(i8)  >= 1);
static_assert(sizeof(u16) >= 2 && sizeof(i16) >= 2);
static_assert(sizeof(u32) >= 4 && sizeof(i32) >= 4);
static_assert(sizeof(u64) >= 8 && sizeof(i64) >= 8);

static_assert(sizeof(f32) == 4 && sizeof(f64) == 8);
