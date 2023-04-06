#include <fast_io.h>

#include <runtime/Multimedia/Player.hpp>
#include <runtime/window/sdl.hpp>

auto main(int argc, char** argv) -> int
{
    // if (argc != 2)
    //     return 0;

    // auto player = Multimedia::Player{argv[1]};
    auto player = Multimedia::Player{"A:/resource/video/ping-pong.mp4"};

    ::window::Config config;
    ::window::sdl_system window_system;

    const auto [width, height] = player.get_video_info();
    config.width = width;
    config.height = height;
    config.use_vulkan = false;
    auto msg = window_system.initialize(config);
    if (msg)
    {
        perrln(fast_io::mnp::os_c_str(msg));
        return -1;
    }

    player.set_callback([&](u8* buffer){
        window_system.present(buffer);
    });
    player.play();
}
