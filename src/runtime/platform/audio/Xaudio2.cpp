#include <thread>

#include <core/base/debug.hpp>
#include <core/base/range.hpp>
#include "Xaudio2.hpp"

NAMESPACE_BEGIN(os::audio::detail)

auto Xaudio2::initialize() noexcept -> const char*
{
    if (is_run)
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
    if (FAILED(result))
    {
        return "XAudio2Create failed";
    }

    result = xaduio->CreateMasteringVoice(&master);
    if (FAILED(result))
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

auto Xaudio2::start() noexcept -> bool
{
    const auto result = SUCCEEDED(source->Start(0));
    if (!result)
    {
        return false;
    }

    index = 0;
    is_play = true;
    condition.notify_one();
    return true;
}

auto Xaudio2::end() noexcept -> bool
{
    {
        std::lock_guard lock{mutex};
        is_play = false;
        // set XAUDIO2_VOICE_STATE SamplesPlayed zero
        // https://stackoverflow.com/questions/65754955/how-to-reset-a-ixaudio2sourcevoices-samplesplayed-counter-after-flushing-sour
        auto buffer = XAUDIO2_BUFFER{};
        buffer.Flags = XAUDIO2_END_OF_STREAM;
        buffer.pAudioData = ring_buffer.get() + index * MAX_SINGLE_BUFFER_SIZE;
        // must be aligned to 4 bytes, otherwise XAudio2 will fail
        buffer.AudioBytes = 4;

        if (FAILED(source->SubmitSourceBuffer(&buffer)))
            return false;
    }
    return SUCCEEDED(source->Stop(0));
}

auto Xaudio2::set_volume(f32 volume) noexcept -> bool
{
    return SUCCEEDED(source->SetVolume(volume));
}

auto Xaudio2::set_format(const Format& format) noexcept -> bool
{
    if (source)
    {
        source->DestroyVoice();
    }

    auto wf = WAVEFORMATEX{};
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.wBitsPerSample = format.sample_bit;
    wf.nChannels = format.channels;
    wf.nSamplesPerSec = format.sample_rate;
    wf.nBlockAlign = wf.nChannels * (wf.wBitsPerSample / 8);
    wf.nAvgBytesPerSec = wf.nBlockAlign * wf.nSamplesPerSec;

    return SUCCEEDED(xaduio->CreateSourceVoice(&source, &wf));
}

auto Xaudio2::write(const u8* const data, usize size) noexcept -> bool
{
    if (!is_play || size > XAUDIO2_MAX_BUFFER_BYTES) [[unlikely]]
        return false;

    source->GetState(&state);
    if (state.BuffersQueued >= XAUDIO2_MAX_QUEUED_BUFFERS) [[unlikely]]
        return false;

    auto buffer = XAUDIO2_BUFFER{};
    buffer.pAudioData = data;
    buffer.AudioBytes = size;
    return SUCCEEDED(source->SubmitSourceBuffer(&buffer));
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
        do
        {
            if (state.BuffersQueued > MIN_BUFFER_SIZE_CALLBACK || !callback)
                break;

            for ([[maybe_unused]] auto _ : range(MAX_BUFFER_SIZE - state.BuffersQueued))
            {
                const auto current_buffer = ring_buffer.get() + index * MAX_SINGLE_BUFFER_SIZE;
                index = (index + 1) % MAX_BUFFER_SIZE;
                std::invoke(callback, current_buffer, MAX_SINGLE_BUFFER_SIZE);

                auto buffer = XAUDIO2_BUFFER{};
                buffer.pAudioData = current_buffer;
                buffer.AudioBytes = MAX_SINGLE_BUFFER_SIZE;
                const auto result = SUCCEEDED(source->SubmitSourceBuffer(&buffer));
                Assert(result);
            }

        } while (false);

        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
    }
}

NAMESPACE_END(os::audio::detail)
