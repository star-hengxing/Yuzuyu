#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "window_system.hpp"

NAMESPACE_BEGIN(window)

struct sdl_system
{
private:
    u32 width;
    u32 height;
    SDL_Window* window = nullptr;
    SDL_Surface* surface = nullptr;

public:
    sdl_system() {}
    ~sdl_system();
    // if success return null pointer, otherwise return the error message string
    [[nodiscard]]
    auto initialize(const config& cfg) noexcept -> const char*;
    // wait for window work
    auto resized() noexcept -> Rect2D;

    auto clean() noexcept -> void;

    auto set_title(const char* title) noexcept -> void;

    auto create_surface(VkInstance instance) noexcept -> VkSurfaceKHR;

    auto present(void* framebuffer) noexcept -> void;

    auto get_window() const noexcept -> decltype(window)
    {
        return window;
    }

    auto get_window_size() const noexcept -> Rect2D
    {
        return {width, height};
    }
};

NAMESPACE_END(window)
