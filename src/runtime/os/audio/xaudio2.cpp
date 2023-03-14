#include "Xaudio2.hpp"

NAMESPACE_BEGIN(os::audio::detail)

auto Xaudio2::initialize() noexcept -> const char*
{
    HRESULT result;
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if constexpr (global::is_debug)
        result = XAudio2Create(&xaduio, XAUDIO2_DEBUG_ENGINE);
    else
        result = XAudio2Create(&xaduio);

    if (FAILED(result))
    {
        return "XAudio2Create failed";
    }

    result = xaduio->CreateMasteringVoice(&master);
    if (FAILED(result))
    {
        return "CreateMasteringVoice failed";
    }

    xaudio_buffer = {};
    return nullptr;
}

auto Xaudio2::start() noexcept -> bool
{
    return SUCCEEDED(source->Start(0));
}

auto Xaudio2::end() noexcept -> bool
{
    return SUCCEEDED(source->Stop(0));
}

auto Xaudio2::set_volume(f32 volume) noexcept -> bool
{
    return SUCCEEDED(source->SetVolume(volume));
}

auto Xaudio2::set_format(const Format& format) noexcept -> bool
{
    WAVEFORMATEX waveform;
    waveform.wFormatTag = WAVE_FORMAT_PCM;
    waveform.wBitsPerSample = format.sample_bit;
    waveform.nChannels = format.channels;
    waveform.nSamplesPerSec = format.sample_rate;
    waveform.nBlockAlign = waveform.nChannels * (waveform.wBitsPerSample / 8);
    waveform.nAvgBytesPerSec = waveform.nBlockAlign * waveform.nSamplesPerSec;
    waveform.cbSize = 0;

    return SUCCEEDED(xaduio->CreateSourceVoice(&source, &waveform));
}

auto Xaudio2::write(const char* str, usize size) noexcept -> bool
{
    if (size > XAUDIO2_MAX_BUFFER_BYTES)
        return false;

    auto buffer = xaudio_buffer;
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.pAudioData = reinterpret_cast<const BYTE*>(str);
    buffer.AudioBytes = size;
    if (FAILED(source->SubmitSourceBuffer(&buffer)))
        return false;

    buffer_size = size;
    return true;
}

auto Xaudio2::get_remaining_size() noexcept -> usize
{
    XAUDIO2_VOICE_STATE state;
    source->GetState(&state);
    if (state.BuffersQueued == 0)
        return 0;

    return buffer_size - state.SamplesPlayed;
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
}

NAMESPACE_END(os::audio::detail)
