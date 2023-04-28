#ifdef USE_MODULES
import <algorithm>;
import <thread>;
#else
#include <algorithm>
#include <thread>
#endif

#include <core/Log.hpp>
#include <core/base/range.hpp>
#include <platform/os.hpp>
#include "Xaudio2.hpp"

NAMESPACE_BEGIN(os::audio::detail)

auto Xaudio2::initialize() noexcept -> const char*
{
    if (is_run) [[unlikely]]
    {
        return "audio is already initialized";
    }

    HRESULT result;
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#ifdef LIBRARY_DEBUG
    result = XAudio2Create(&xaduio, XAUDIO2_DEBUG_ENGINE);
    XAUDIO2_DEBUG_CONFIGURATION debug{};
    debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
    debug.BreakMask = XAUDIO2_LOG_ERRORS;
    xaduio->SetDebugConfiguration(&debug);
#else
    result = XAudio2Create(&xaduio);
#endif
    if (FAILED(result)) [[unlikely]]
    {
        return "XAudio2Create failed";
    }

    result = xaduio->CreateMasteringVoice(&master);
    if (FAILED(result)) [[unlikely]]
    {
        return "CreateMasteringVoice failed";
    }

    is_run = true;
    is_play = false;
    std::thread{&Self::worker, this}.detach();
    return nullptr;
}

auto Xaudio2::set_callback(std::function<audio_callback> callback) noexcept -> Self&
{
    this->callback = std::move(callback);
    return *this;
}

auto Xaudio2::start() noexcept -> void
{
    const auto result = FAILED(source->Start(0));
    if (result || !callback)
    {
        LOG_ERROR("Can't start audio");
    }

    index = 0;
    is_play = true;
    condition.notify_one();
}

auto Xaudio2::end() noexcept -> void
{
    // set XAUDIO2_VOICE_STATE SamplesPlayed zero
    // https://stackoverflow.com/questions/65754955/how-to-reset-a-ixaudio2sourcevoices-samplesplayed-counter-after-flushing-sour
    const auto buffer = XAUDIO2_BUFFER
    {
        .Flags = XAUDIO2_END_OF_STREAM,
        // must be aligned to 4 bytes, otherwise XAudio2 will fail
        .AudioBytes = 4,
        .pAudioData = ring_buffer.get() + index * MAX_SINGLE_BUFFER_SIZE,
    };

    is_play = false;
    submit(buffer);

    if (FAILED(source->Stop(0))) [[unlikely]]
    {
        LOG_ERROR("Can't stop audio");
    }
}

auto Xaudio2::set_volume(f32 volume) noexcept -> void
{
    if (FAILED(source->SetVolume(volume))) [[unlikely]]
    {
        LOG_ERROR("Can't set audio volume");
    }
}

auto Xaudio2::set_format(const Format& format) noexcept -> void
{
    if (source)
    {
        source->DestroyVoice();
    }

    const auto per_second_bytes = static_cast<WORD>(format.channels * (format.sample_bit / 8));
    const auto wf = WAVEFORMATEX
    {
        .wFormatTag      = WAVE_FORMAT_PCM,
        .nChannels       = format.channels,
        .nSamplesPerSec  = static_cast<DWORD>(format.sample_rate),
        .nAvgBytesPerSec = static_cast<DWORD>(per_second_bytes * format.sample_rate),
        .nBlockAlign     = per_second_bytes,
        .wBitsPerSample  = format.sample_bit,
    };

    if (FAILED(xaduio->CreateSourceVoice(&source, &wf))) [[unlikely]]
    {
        LOG_ERROR("Can't create audio voice");
    }
}

auto Xaudio2::write(const u8* const data, u32 size) noexcept -> bool
{
    if (!is_play || size > XAUDIO2_MAX_BUFFER_BYTES) [[unlikely]]
        return false;

    source->GetState(&state);
    if (state.BuffersQueued >= XAUDIO2_MAX_QUEUED_BUFFERS) [[unlikely]]
        return false;

    const auto buffer = XAUDIO2_BUFFER
    {
        .AudioBytes = size,
        .pAudioData = data,
    };
    submit(buffer);
    return true;
}

Xaudio2::~Xaudio2()
{
    if (source)
    {
        source->DestroyVoice();
        source = nullptr;
    }

    if (xaduio)
    {
        xaduio->Release();
        xaduio = nullptr;
    }

    CoUninitialize();
    // join worker thread
    is_run = false;
    is_play = true;
    condition.notify_one();
}

auto Xaudio2::submit(const XAUDIO2_BUFFER& buffer) noexcept -> void
{
    std::lock_guard lock{mutex};

    if (!is_play || !is_run) [[unlikely]]
        return;

    if (FAILED(source->SubmitSourceBuffer(&buffer))) [[unlikely]]
    {
        LOG_ERROR("Can't submit audio buffer");
    }
}

auto Xaudio2::worker() noexcept -> void
{
    while (true)
    {
        {
            auto lock = std::unique_lock{mutex};
            condition.wait(lock, [&] { return is_play; });
        }

        if (!is_run) [[unlikely]]
            return;

        source->GetState(&state);

        if (state.BuffersQueued <= MIN_BUFFER_SIZE_CALLBACK)
        {
            for ([[maybe_unused]] auto _ : range(MAX_BUFFER_SIZE - state.BuffersQueued))
            {
                const auto current_buffer = ring_buffer.get() + index * MAX_SINGLE_BUFFER_SIZE;
                index = (index + 1) % MAX_BUFFER_SIZE;
                // write audio data to current_buffer
                const auto size = std::invoke(callback, current_buffer, MAX_SINGLE_BUFFER_SIZE);
                if (size == 0)
                    continue;

                const auto buffer = XAUDIO2_BUFFER
                {
                    .AudioBytes = std::min<UINT32>(size, MAX_SINGLE_BUFFER_SIZE),
                    .pAudioData = current_buffer,
                };

                submit(buffer);
            }
        }

        os::sleep(SLEEP_TIME);
    }
}

NAMESPACE_END(os::audio::detail)
