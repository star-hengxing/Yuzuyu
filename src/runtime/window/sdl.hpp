#pragma once

#ifdef USE_MODULES
import <tuple>;
#else
#include <tuple>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "window_system.hpp"

NAMESPACE_BEGIN(runtime)
NAMESPACE_BEGIN(window)

struct sdl_system
{
private:
    u16 width;
    u16 height;
    SDL_Window* window = nullptr;
    SDL_Surface* surface = nullptr;

public:
    sdl_system() {}
    ~sdl_system();
    // if success return null pointer, otherwise return the error message string
    [[nodiscard]]
    auto initialize(const Config& config) -> const char*;
    // wait for window work
    auto resized() -> std::tuple<u16, u16>;

    auto clean() -> void;

    auto set_title(const char* title) -> void;

    auto create_surface(VkInstance instance) -> VkSurfaceKHR;

    auto present(void* framebuffer) -> void;

    auto get_window() const -> decltype(window)
    {
        return window;
    }

    auto get_window_size() const -> std::tuple<u16, u16>
    {
        return {width, height};
    }
};

NAMESPACE_END(window)
NAMESPACE_END(runtime)
