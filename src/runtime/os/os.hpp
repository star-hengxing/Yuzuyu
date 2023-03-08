#pragma once

#include <fast_io.h>

#include <runtime/helper/macro.hpp>

NAMESPACE_BEGIN(os)
// @note get unix timestamp
auto time() noexcept -> fast_io::unix_timestamp;

NAMESPACE_END(os)
