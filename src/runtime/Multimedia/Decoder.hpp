#pragma once

#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include <runtime/base/Rect2D.hpp>
#include <runtime/base/Color.hpp>
#include <runtime/os/audio/base.hpp>

NAMESPACE_BEGIN(Multimedia)
NAMESPACE_BEGIN(detail)

struct ffmpeg_payload
{
    int index;
    const AVCodec* codec = nullptr;
    AVCodecContext* context = nullptr;

    f64 last_pts = 0;
    f64 last_time = 0;
};

struct Decoder
{
private:
    AVFormatContext* format_context = nullptr;

    bool has_video = false;
    ffmpeg_payload video_payload;
    SwsContext* video_converter = nullptr;
    AVFrame* rgba_frame = nullptr;

    bool has_audio = false;
    ffmpeg_payload audio_payload;
    SwrContext* audio_converter = nullptr;
    AVFrame* audio_frame = nullptr;
    const AVSampleFormat sample_format = AV_SAMPLE_FMT_S16;

    std::string filename;

private:
    auto initialize_payload(ffmpeg_payload& payload, const AVMediaType type) noexcept -> bool;

    auto initialize_video() noexcept -> void;
    auto initialize_audio() noexcept -> void;

public:
    enum class Type : u8
    {
        eof,
        error,
        unknown,
        video,
        audio,
    };

    struct Packet
    {
        Type type;
        AVPacket* packet;
    };

public:
    ~Decoder();

    auto initialize(const std::string_view filename) noexcept -> bool;
    // It will get packet ownership, maybe need av_packet_free to free memory
    auto next() noexcept -> Packet;

    auto get_video_info() const noexcept -> Rect2D;
    auto get_audio_info() const noexcept -> os::audio::Format;
    /**
     * @param packet move packet ownership
     * @param[out] buffer format: rgba rgba...
     */
    auto video_decode(AVPacket* packet, u8* buffer, u32 width, u32 height) noexcept -> bool;
    /**
     * @param packet move packet ownership
     * @param[out] buffer
     */
    auto audio_decode(AVPacket* packet, u8* buffer, usize size) noexcept -> bool;

    auto get_video_pts() const noexcept -> f64;
    auto get_audio_pts() const noexcept -> f64;
    /**
     * @return unit: ms
     */
    auto get_video_frame_duration() const noexcept -> u32;
};

NAMESPACE_END(detail)

using ::Multimedia::detail::Decoder;

NAMESPACE_END(Multimedia)
