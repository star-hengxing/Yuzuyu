#pragma once

#include <fast_io.h>

#include "basic_type.hpp"

struct FPS_counter
{
public:
    using type = u16;

private:
    type fps = 0;
    char buffer[64];
    fast_io::obuffer_view view{std::begin(buffer), std::end(buffer)};
    fast_io::unix_timestamp time = posix_clock_gettime(fast_io::posix_clock_id::realtime);

public:
    auto update(auto&& f) -> type
    {
        const auto current = posix_clock_gettime(fast_io::posix_clock_id::realtime);
        if ((current - time).seconds < 1)
        {
            fps += 1;
            return 0;
        }

        // for (auto i : buffer)
        // {
        //     i = 0;
        // }
        time = posix_clock_gettime(fast_io::posix_clock_id::realtime);

        view.clear();
        print(view, "fps: ", fps);
        f(buffer);

        const auto ret = fps;
        fps = 0;
        return ret;
    }
};
