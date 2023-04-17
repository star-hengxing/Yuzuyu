#pragma once

#include <condition_variable>
#include <mutex>
#include <functional>
#include <XAudio2.h>

#include <core/base/Owned.hpp>
#include "base.hpp"

NAMESPACE_BEGIN(os::audio::detail)

static constexpr auto MAX_SINGLE_BUFFER_SIZE = usize{4096};
static constexpr auto MAX_BUFFER_SIZE = usize{4};
static constexpr auto MIN_BUFFER_SIZE_CALLBACK = MAX_BUFFER_SIZE / 2;
// unit: milliseconds
static constexpr auto SLEEP_TIME = 10;

struct Xaudio2
{
public:
    using Self = Xaudio2;

private:
    IXAudio2* xaduio = nullptr;
    IXAudio2MasteringVoice* master = nullptr;
    IXAudio2SourceVoice* source = nullptr;

    XAUDIO2_VOICE_STATE state;

    u8 index = 0;
    // copy data to our buffer, keep the lifetime until current buffer is played
    Owned<u8[]> ring_buffer = Owned<u8[]>::make(MAX_SINGLE_BUFFER_SIZE * MAX_BUFFER_SIZE);

    bool is_run = false;
    bool is_play = false;
    std::function<audio_callback> callback;
    std::mutex mutex;
    std::condition_variable condition;

private:
    auto worker() noexcept -> void;

public:
    ~Xaudio2();

    auto initialize() noexcept -> const char*;

    auto set_volume(f32 volume) noexcept -> bool;

    auto set_format(const Format& format) noexcept -> bool;

    auto set_callback(std::function<audio_callback> callback) noexcept -> Self&;

    auto start() noexcept -> bool;

    auto end() noexcept -> bool;
    // acquire ownership of data until all data is played
    auto write(const u8* const data, usize size) noexcept -> bool;
};

NAMESPACE_END(os::audio::detail)
