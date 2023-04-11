// extern "C"
// {
// #include <libavutil/time.h>
// }
#include <fast_io.h>

#include <runtime/base/debug.hpp>
#include "Decoder.hpp"

NAMESPACE_BEGIN(Multimedia::detail)

Decoder::~Decoder()
{
    if (has_video)
    {
        sws_freeContext(video_converter);
        av_frame_free(&rgba_frame);
        avcodec_close(video_payload.context);
    }

    if (has_audio)
    {
        swr_free(&audio_converter);
        av_frame_free(&audio_frame);
        avcodec_close(audio_payload.context);
    }

    avformat_close_input(&format_context);
    avformat_free_context(format_context);
}

auto Decoder::initialize_payload(
    ffmpeg_payload& payload, const AVMediaType type) noexcept -> bool
{
    const auto index = av_find_best_stream(format_context, type, -1, -1, &payload.codec, 0);
    if (index < 0 || !payload.codec)
    {
        perr("av_find_best_stream failed\n");
        return false;
    }

    payload.index = index;
    const auto stream = format_context->streams[index];
    payload.context = avcodec_alloc_context3(payload.codec);

    auto result = avcodec_parameters_to_context(payload.context, stream->codecpar);
    if (result < 0)
    {
        perr("avcodec_parameters_to_context failed\n");
        return false;
    }

    result = avcodec_open2(payload.context, payload.codec, nullptr);
    if (result < 0)
    {
        perr("avcodec_open2 failed\n");
        return false;
    }

    return true;
}

auto Decoder::initialize_video() noexcept -> void
{
    if (!has_video)
        return;

    const auto avc = video_payload.context;
    const auto width = avc->width;
    const auto height = avc->height;
    video_converter = sws_getContext(
        width, height, avc->pix_fmt,
        width, height, AV_PIX_FMT_RGBA,
        SWS_BICUBIC, nullptr, nullptr, nullptr);

    if (!video_converter)
    {
        perr("sws_getContext failed\n");
        return;
    }

    rgba_frame = av_frame_alloc();
}

auto Decoder::initialize_audio() noexcept -> void
{
    if (!has_audio)
        return;

    const auto avc = audio_payload.context;
    auto result = swr_alloc_set_opts2(
        &audio_converter,
        &avc->ch_layout, sample_format, avc->sample_rate,
        &avc->ch_layout, avc->sample_fmt, avc->sample_rate,
        0, nullptr);

    if (result != 0)
    {
        perr("swr_alloc_set_opts2 failed\n");
        return;
    }

    if (swr_init(audio_converter) != 0)
    {
        perr("swr_init failed\n");
    }

    audio_frame = av_frame_alloc();
}

auto Decoder::initialize(const std::string_view filename) noexcept -> bool
{
    format_context = avformat_alloc_context();
    auto result = avformat_open_input(&format_context, filename.data(), nullptr, nullptr);
    if (result != 0)
    {
        // char error_buf[256];
        // av_strerror(error_code, error_buf, 256);
        // TDOD: log
        perr("avformat_open_input failed\n");
        return false;
    }

    result = avformat_find_stream_info(format_context, nullptr);
    if (result < 0)
    {
        perr("avformat_find_stream_info failed\n");
        return false;
    }

    has_video = initialize_payload(video_payload, AVMEDIA_TYPE_VIDEO);
    initialize_video();
    has_audio = initialize_payload(audio_payload, AVMEDIA_TYPE_AUDIO);
    initialize_audio();

    if (!has_video && !has_audio)
        return false;

    this->filename = filename;
    // if (verbose)
    // {
    //     av_dump_format(format_context, 0, this->filename.c_str(), 0);
    // }
    return true;
}

auto Decoder::video_decode(
    AVPacket* packet, u8* buffer, u32 width, u32 height) noexcept -> bool
{
    if (avcodec_send_packet(video_payload.context, packet) != 0)
        return false;

    if (avcodec_receive_frame(video_payload.context, rgba_frame) != 0)
        return false;

    video_payload.last_pts = rgba_frame->best_effort_timestamp
        * av_q2d(format_context->streams[video_payload.index]->time_base);

    auto data = reinterpret_cast<uint8_t*>(buffer);
    const auto width_bytes = static_cast<int>(width * 4);

    sws_scale(video_converter,
              static_cast<const uint8_t* const*>(rgba_frame->data),
              rgba_frame->linesize, 0, height,
              static_cast<uint8_t* const*>(&data),
              &width_bytes);

    av_packet_free(&packet);
    return true;
}

auto Decoder::audio_decode(
    AVPacket* packet, u8* buffer, usize size) noexcept -> bool
{
    if (avcodec_send_packet(audio_payload.context, packet) != 0)
        return false;

    if (avcodec_receive_frame(audio_payload.context, audio_frame) != 0)
        return false;

    if (av_sample_fmt_is_planar(audio_payload.context->sample_fmt) == 0)
        return false;

    audio_payload.last_pts = audio_frame->best_effort_timestamp
        * av_q2d(format_context->streams[audio_payload.index]->time_base);

    auto data = static_cast<uint8_t*>(buffer);
    const auto samples_size = swr_convert(
        audio_converter, &data, size,
        const_cast<const uint8_t**>(audio_frame->data),
        audio_frame->nb_samples);

    if (samples_size <= 0)
        return false;

    const auto required_size = static_cast<usize>(av_samples_get_buffer_size(
        nullptr,
        audio_payload.context->ch_layout.nb_channels, samples_size,
        sample_format, 1));

    Assert(required_size <= size);
    av_packet_free(&packet);
    return true;
}

auto Decoder::next() noexcept -> Packet
{
    auto packet = av_packet_alloc();
    const auto result = av_read_frame(format_context, packet);
    if (result == AVERROR_EOF) [[unlikely]]
    {
        av_packet_free(&packet);
        return {Type::eof, nullptr};
    }
    else if (result < 0) [[unlikely]]
    {
        av_packet_free(&packet);
        return {Type::error, nullptr};
    }

    Assert(result == 0);
    auto type = Type::unknown;
    if (has_audio && packet->stream_index == audio_payload.index)
    {
        type = Type::audio;
    }
    else if (has_video && packet->stream_index == video_payload.index)
    {
        type = Type::video;
    }

    if (type == Type::unknown) [[unlikely]]
    {
        av_packet_free(&packet);
        return {Type::unknown, nullptr};
    }

    return {type, packet};
}

auto Decoder::get_video_info() const noexcept -> Rect2D
{
    if (!has_video)
        return {};

    const auto avc = video_payload.context;
    return
    {
        static_cast<u32>(avc->width),
        static_cast<u32>(avc->height)
    };
}

auto Decoder::get_audio_info() const noexcept -> os::audio::Format
{
    if (!has_audio)
        return {};

    const auto avc = audio_payload.context;
    return
    {
        .sample_bit = 16,
        .channels = static_cast<u8>(avc->ch_layout.nb_channels),
        .sample_rate = static_cast<usize>(avc->sample_rate)
    };
}

auto Decoder::get_video_pts() const noexcept -> f64
{
    return video_payload.last_pts;
}

auto Decoder::get_audio_pts() const noexcept -> f64
{
    return audio_payload.last_pts;
}

auto Decoder::get_video_frame_duration() const noexcept -> u32
{
    const auto num = format_context->streams[video_payload.index]->r_frame_rate.num;
    const auto den = format_context->streams[video_payload.index]->r_frame_rate.den;
    return static_cast<u32>(1000.0 * den / num);
}

NAMESPACE_END(Multimedia::detail)
