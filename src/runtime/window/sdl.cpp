#include <fast_io.h>

#include <runtime/base/range.hpp>
#include "sdl.hpp"

NAMESPACE_BEGIN(runtime)
NAMESPACE_BEGIN(window)

sdl_system::~sdl_system()
{
    if (surface)
    {
        SDL_FreeSurface(surface);
    }

    if (window)
    {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
}

auto sdl_system::clean() -> void
{
    if (surface)
    {
        SDL_FreeSurface(surface);
        surface = nullptr;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

auto sdl_system::initialize(const Config& config) -> const char*
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return SDL_GetError();
    }

    Uint32 flags = SDL_WINDOW_SHOWN;
    if (config.use_vulkan)
    {
        flags |= SDL_WINDOW_VULKAN;
    }

    window = SDL_CreateWindow(config.title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED
        , config.width, config.height, flags);
    if (window == nullptr)
    {
        return SDL_GetError();
    }

    this->width = config.width;
    this->height = config.height;

    surface = SDL_GetWindowSurface(window);
    return nullptr;
}

auto sdl_system::set_title(const char* title) -> void
{
    SDL_SetWindowTitle(window, title);
}

auto sdl_system::resized() -> std::tuple<u16, u16>
{
    return {width, height};
}

auto sdl_system::create_surface(VkInstance instance)
    -> VkSurfaceKHR
{
    VkSurfaceKHR surface;
    const auto result = SDL_Vulkan_CreateSurface(window, instance, &surface);
    return result ? surface : nullptr;
}

auto sdl_system::present(void* framebuffer) -> void
{
    SDL_LockSurface(surface);

    const auto size = surface->w * surface->h * 4;
#ifdef _WIN32
    const auto dst = static_cast<u8*>(surface->pixels);
    const auto src = static_cast<u8*>(framebuffer);
    for (auto i : range(0, size, 4))
    {
        dst[i] = src[i + 2];
        dst[i + 1] = src[i + 1];
        dst[i + 2] = src[i];
        dst[i + 3] = src[i + 3];
    }
#else
    // TODO: use simd
    memcpy(surface->pixels, framebuffer, size);
#endif
    SDL_UnlockSurface(surface);
    const auto result = SDL_UpdateWindowSurface(window);

    if (result != 0) [[unlikely]]
    {
        perrln(SDL_GetError());
    }
}

NAMESPACE_END(window)
NAMESPACE_END(runtime)
