#pragma once

#include <runtime/helper/basic.hpp>

NAMESPACE_BEGIN(os::audio)

struct Format
{
    u8 sample_bit;
    u8 channels;
    usize sample_rate;
};

NAMESPACE_END(os::audio)
