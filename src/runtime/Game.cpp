#ifdef USE_MODULES
import <string_view>;
#else
#include <string_view>
#endif

#include <fast_io.h>
// #include <spdlog/spdlog.h>

#include <runtime/base/range.hpp>
#include <runtime/rhi/vulkan.hpp>
#include "Game.hpp"
#include <config.hpp>
using namespace Game;

NAMESPACE_BEGIN()

auto check_message(const char* message) -> void
{
    if (message)
    {
        println(message);
        // spdlog::error("{}", message);
        exit(-1);
    }
}

NAMESPACE_END()

NAMESPACE_BEGIN(runtime)

auto Game::create(int argc, char** argv) -> Owned<Self>
{
    for (auto i : range(argc))
    {
        const auto str = std::string_view{argv[i]};
        println(str);
    }
    return Owned<Self>::make();
}

auto Game::initialize() -> void
{
    const char* message;

    window::Config config;
    config.title = "Game";

    message = window.initialize(config);
    check_message(message);

    runtime::rhi::vulkan rhi;
    message = rhi.initialize(&window);
    check_message(message);
}

auto Game::run() -> void
{
    
}

auto Game::clean() -> void
{
    println("Game::clean()");
    // rhi.clean();
    // window.clean();
}

NAMESPACE_END(runtime)
