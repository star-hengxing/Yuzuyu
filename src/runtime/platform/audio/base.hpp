#pragma once

#include <core/base/base.hpp>

NAMESPACE_BEGIN(os::audio)

struct Format
{
    u8 sample_bit;
    u8 channels;
    usize sample_rate;
};

using audio_callback = u32(u8*, u32);

NAMESPACE_END(os::audio)
