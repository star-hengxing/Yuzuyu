#pragma once

#include <iostream>
#include <chrono>

#include "basic_type.hpp"

struct Timer
{
private:
    using f64_ms = std::chrono::duration<f64, std::milli>;
    std::chrono::steady_clock::time_point t0;

public:
    Timer() : t0(std::chrono::steady_clock::now()) {}

    template <typename T>
    static void display(T n)
    {
        std::cout << "time elapsed:";
        if (n < 1000)
        {
            std::cout << ' ' << n << " ms" << '\n';
        }
        else
        {
            const auto seconds = static_cast<usize>(n);
            const auto h = (seconds / 3600) % 24;
            const auto m = (seconds / 60) % 60;
            const auto s = seconds % 60;
            if (h != 0) printf(" %lld hour", h);
            if (m != 0) printf(" %lld minute", m);
            if (s != 0) printf(" %lld second", s);
            printf("\n");
        }
    }

    void elapsed() const
    {
        const auto p = std::chrono::steady_clock::now() - t0;
        const auto ms = std::chrono::duration_cast<f64_ms>(p).count();
        display(ms);
    }

    template <typename T, typename... Args>
    static auto elapsed(T fn, Args&&... args)
    {
        Timer timer;
        if constexpr (std::is_same_v<decltype(fn(std::forward<Args>(args)...)), void>)
        {
            fn(std::forward<Args>(args)...);
            timer.elapsed();
        }
        else
        {
            auto ret = fn(std::forward<Args>(args)...);
            timer.elapsed();
            return ret;
        }
    }

    template <typename T, typename U, typename... Args>
    static auto elapsed(T* this_ptr, U (T::*member_fn)(Args...), Args&&... args)
    {
        Timer timer;
        if constexpr (std::is_same_v<U, void>)
        {
            (this_ptr->*member_fn)(std::forward<Args>(args)...);
            timer.elapsed();
        }
        else
        {
            U ret = (this_ptr->*member_fn)(std::forward<Args>(args)...);
            timer.elapsed();
            return ret;
        }
    }
};
