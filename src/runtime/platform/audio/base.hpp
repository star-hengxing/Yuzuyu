#pragma once

#include <core/base/base.hpp>

NAMESPACE_BEGIN(os::audio)

struct Format
{
    u8 sample_bit;
    u8 channels;
    usize sample_rate;
};

using audio_callback = void(u8*, usize);

NAMESPACE_END(os::audio)
