#pragma once

#ifdef USE_MODULES
import <array>;
#else
#include <array>
#endif

#include <XAudio2.h>

#include "base.hpp"

NAMESPACE_BEGIN(os::audio::detail)

struct Xaudio2
{
private:
    IXAudio2* xaduio = nullptr;
    IXAudio2MasteringVoice* master = nullptr;
    IXAudio2SourceVoice* source = nullptr;

    usize buffer_size;
    XAUDIO2_BUFFER xaudio_buffer;

public:
    ~Xaudio2();

    auto initialize() noexcept -> const char*;

    auto set_volume(f32 size) noexcept -> bool;

    auto set_format(const Format& format) noexcept -> bool;

    auto start() noexcept -> bool;

    auto end() noexcept -> bool;

    auto write(const char* str, usize size) noexcept -> bool;
    // @note Get the audio remaining bytes in the current buffer.
    auto get_remaining_size() noexcept -> usize;
};

NAMESPACE_END(os::audio::detail)
