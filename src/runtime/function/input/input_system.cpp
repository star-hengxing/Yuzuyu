#include <core/base/range.hpp>
#include "input_system.hpp"

NAMESPACE_BEGIN(input)

auto input_system::poll() -> Self&
{
    SDL_PollEvent(&event);
    return *this;
}

auto input_system::unbind_all() -> Self&
{
    {
        decltype(keys) old{};
        keys.swap(old);
    }

    {
        decltype(command_list) old{};
        command_list.swap(old);
    }
    return *this;
}

auto input_system::collect_input() -> Self&
{
    current_key = 0;
    key_state.is_press = false;
    key_state.is_release = false;

    mouse.left.is_press = false;
    mouse.right.is_press = false;
    mouse.left.is_release = false;
    mouse.right.is_release = false;

    const SDL_MouseButtonEvent* sdl_mouse = &event.button;

    switch (event.type)
    {
    case SDL_QUIT:
        is_exit = true;
        break;
    case SDL_KEYDOWN: {
        key_state.is_press = true;
        current_key = event.key.keysym.sym;
        break;
    }
    case SDL_KEYUP: {
        key_state.is_release = true;
        current_key = event.key.keysym.sym;
        break;
    }
    case SDL_MOUSEMOTION: {
        mouse.x = event.motion.x;
        mouse.y = event.motion.y;
        break;
    }
    case SDL_MOUSEBUTTONDOWN: {
        mouse.left.is_press = (sdl_mouse->button == SDL_BUTTON_LEFT);
        mouse.right.is_press = (sdl_mouse->button == SDL_BUTTON_RIGHT);
        break;
    }
    case SDL_MOUSEBUTTONUP: {
        mouse.left.is_release = (sdl_mouse->button == SDL_BUTTON_LEFT);
        mouse.right.is_release = (sdl_mouse->button == SDL_BUTTON_RIGHT);
        break;
    }
    default:
        break;
    }

    return *this;
}

auto input_system::execute() -> Self&
{
    std::optional<usize> index;
    for (auto i : range(keys.size()))
    {
        if (keys[i] == current_key)
        {
            index = i;
            break;
        }
    }

    if (!index.has_value())
        return *this;

    auto& list = command_list[index.value()];
    for (auto i : range(list.size))
    {
        list.commands[i]();
    }

    return *this;
}

NAMESPACE_END(input)
