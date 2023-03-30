#include <cmath>
#include <thread>

#include <fast_io.h>

#include <runtime/helper/debug.hpp>
#include <runtime/os/os.hpp>
#include "Player.hpp"

NAMESPACE_BEGIN()
// unit: s
constexpr auto MIN_SYNC_TIME = f64{0.01};
constexpr auto MAX_SYNC_TIME = f64{0.1};
/**
 * @return unit: ms
 */
auto get_delay(f64 video, f64 audio) noexcept -> i32
{
    static auto last_pts = f64{};

    const auto diff = video - audio;
    auto delay = video - last_pts;
    last_pts = video;

    const auto sync_threshold = (delay > MIN_SYNC_TIME) ? delay : MIN_SYNC_TIME;
    // if diff > 100ms, nothing to do
    if (!std::isnan(diff) && diff < MAX_SYNC_TIME)
    {
        if (diff <= -sync_threshold)
        {
            // audio > video, nothing to do
            delay = 0;
        }
        else if (diff >= sync_threshold)
        {
            // audio < video, let video thread sleep
            delay *= 2;
        }
    }
    return delay * 1000;
}

NAMESPACE_END()

NAMESPACE_BEGIN(Multimedia::detail)

auto Player::get_video_info() const noexcept -> Rect2D
{
    return decoder.get_video_info();
}

auto Player::set_callback(std::function<video_callback> callback) noexcept -> Self&
{
    this->callback = std::move(callback);
    return *this;
}

Player::Player(const std::string_view filename)
{
    if (!decoder.initialize(filename))
        return;

    bool has_video = true;
    bool has_audio = true;
    const auto [width, height] = decoder.get_video_info();
    if (width == 0 || height == 0)
    {
        has_video = false;
        perrln("video not found");
    }

    const auto format = decoder.get_audio_info();
    if (format.channels == 0)
    {
        has_audio = false;
        perrln("audio not found");
    }

    if (!has_video && !has_audio)
        return;

    // initialize video
    video_buffer = Owned<Color[]>::make(width * height);
    // initialize audio
    auto msg = audio_system.initialize();
    if (msg)
    {
        perrln(fast_io::mnp::os_c_str(msg));
        return;
    }

    Assert(audio_system.set_format(format));
    audio_system.set_callback([this](u8* buffer, usize size) {
        this->play_audio(buffer, size);
    });

    is_run = true;
}

auto Player::play_audio(u8* buffer, usize size) noexcept -> void
{
    AVPacket* packet;
    {
        std::lock_guard lock{mutex};
        if (audio_queue.empty())
            return;

        packet = audio_queue.front();
        audio_queue.pop();
    }
    // The decoded data will write directly into the cache of the audio
    decoder.audio_decode(packet, buffer, size);
}

auto Player::play_video() noexcept -> void
{
    AVPacket* packet;
    while (is_run)
    {
        {
            std::lock_guard lock{mutex};
            if (video_queue.empty())
                continue;

            packet = video_queue.front();
            video_queue.pop();
        }

        const auto delay = get_delay(decoder.get_video_pts(), decoder.get_audio_pts());
        // We cannot adjust the audio speed, so we can only adjust the video speed
        if (delay > 0)
        {
            os::sleep(delay);
        }

        const auto [width, height] = decoder.get_video_info();
        const auto buffer = &video_buffer.get()->r;
        decoder.video_decode(packet, buffer, width, height);
        std::invoke(callback, buffer);
    }
}

auto Player::play() noexcept -> void
{
    if (!is_run || !callback)
        return;

    using Type = Multimedia::Decoder::Type;

    Assert(audio_system.start());
    const auto video_thread = std::jthread{&Player::play_video, this};

    while (true)
    {
        auto packet = decoder.next();
        if (packet.type == Type::eof) [[unlikely]]
            break;

        if (packet.type == Type::error) [[unlikely]]
        {
            // TODO: error handle
            is_run = false;
            Assert(audio_system.end());
            return;
        }
        // avoid excessive memory usage
        if (audio_queue.size() > 200 && video_queue.size() > 200)
            os::sleep(1000);

        std::lock_guard lock{mutex};
        if (packet.type == Type::audio)
        {
            audio_queue.push(packet.packet);
        }
        else if (packet.type == Type::video)
        {
            video_queue.push(packet.packet);
        }
    }
    // wait for video and audio
    while (true)
    {
        os::sleep(100);
        if (audio_queue.empty() && video_queue.empty())
            break;
    }

    is_run = false;
    Assert(audio_system.end());
}

NAMESPACE_END(Multimedia::detail)
