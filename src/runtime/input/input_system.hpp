#pragma once

#include <functional>
#include <optional>
#include <vector>

#include <SDL2/SDL.h>

#include <runtime/helper/basic.hpp>

NAMESPACE_BEGIN(input)

static constexpr auto MAX_SAME_KEY_COMMAND_SIZE = usize{8};

struct input_system
{
private:
    using Self = input_system;
    using key_type = int;

    template <typename RETRUN_TYPE, typename... Args>
    struct Command
    {
        u8 size = 0;
        std::function<auto(Args&&...)->RETRUN_TYPE> commands[MAX_SAME_KEY_COMMAND_SIZE];

        Command(auto fn) : size{1}
        {
            commands[0] = fn;
        }

        auto add(auto fn) -> Command&
        {
            commands[size] = fn;
            size += 1;
            return *this;
        }
    };

    struct input_state
    {
        bool is_press = false;
        bool is_release = false;
    };

    struct Mouse
    {
        i32 x = 0;
        i32 y = 0;
        input_state left, right;
    };

private:
    key_type current_key;
    input_state key_state;

    std::vector<key_type> keys;
    std::vector<Command<void>> command_list;

public:
    SDL_Event event;

    Mouse mouse;

    bool is_exit = false;

public:
    auto poll() -> Self&;

    auto execute() -> Self&;

    auto collect_input() -> Self&;

    auto unbind_all() -> Self&;

    auto bind_key(int key, auto fn) -> Self&
    {
        std::optional<usize> index;
        for (auto i : keys)
        {
            if (i == key)
            {
                index = i;
                break;
            }
        }

        if (index.has_value())
        {
            command_list[index.value()].add(fn);
            return *this;
        }

        keys.push_back(key);
        command_list.push_back(Command<void>{fn});

        return *this;
    }
};

NAMESPACE_END(input)
