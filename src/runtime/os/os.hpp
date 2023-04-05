#pragma once

#include <fast_io.h>

#include <runtime/base/base.hpp>

NAMESPACE_BEGIN(os)
// @note get unix timestamp
auto time() noexcept -> fast_io::unix_timestamp;
// @note sleep current thread
auto sleep(usize milliseconds) noexcept -> void;

NAMESPACE_END(os)
