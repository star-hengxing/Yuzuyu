#pragma once

#include <fast_io.h>

#include <runtime/helper/basic.hpp>

NAMESPACE_BEGIN(os)
// @note get unix timestamp
auto time() noexcept -> fast_io::unix_timestamp;
// @note sleep current thread
auto sleep(usize milliseconds) noexcept -> void;

NAMESPACE_END(os)
