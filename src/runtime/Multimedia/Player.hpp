#pragma once

#include <functional>
#include <queue>
#include <mutex>

#include <runtime/helper/Owned.hpp>
#include <runtime/os/audio.hpp>
#include "Decoder.hpp"

NAMESPACE_BEGIN(Multimedia)

using video_callback = void(u8*);

NAMESPACE_BEGIN(detail)

struct Player
{
public:
    using Self = Player;

private:
    Owned<Color[]> video_buffer;

    Decoder decoder;
    os::audio::audio_system audio_system;
    // TODO: lock-free queue
    std::queue<AVPacket*> audio_queue;
    std::queue<AVPacket*> video_queue;

    std::mutex mutex;
    bool is_run = false;
    std::function<video_callback> callback;

private:
    auto play_audio(u8* buffer, usize size) noexcept -> void;
    auto play_video() noexcept -> void;

public:
    Player(const std::string_view filename);

    auto get_video_info() const noexcept -> Rect2D;

    auto set_callback(std::function<video_callback> callback) noexcept -> Self&;

    auto play() noexcept -> void;
};

NAMESPACE_END(detail)

using ::Multimedia::detail::Player;

NAMESPACE_END(Multimedia)
