#pragma once

#ifdef USE_MODULES
import <string>;
import <string_view>;
#else
#include <string>
#include <string_view>
#endif

#include <spdlog/spdlog.h>
#include <fast_io.h>

#include "base/base.hpp"

struct log_system
{
public:
    enum class level : u8
    {
        debug,
        info,
        warn,
        error,
        fatal,
    };

private:
    std::shared_ptr<spdlog::logger> logger;

private:
    log_system();

    ~log_system();

public:
    static auto get() noexcept -> log_system&;

    auto log(log_system::level level, const std::string_view str) noexcept -> void;
};

#define LOG_DEBUG(...) ::log_system::get().log(::log_system::level::debug, fast_io::concat(__VA_ARGS__))
#define LOG_INFO(...) ::log_system::get().log(::log_system::level::info, fast_io::concat(__VA_ARGS__))
#define LOG_WARN(...) ::log_system::get().log(::log_system::level::warn, fast_io::concat(__VA_ARGS__))
#define LOG_ERROR(...) ::log_system::get().log(::log_system::level::error, fast_io::concat(__VA_ARGS__))
#define LOG_FATAL(...) ::log_system::get().log(::log_system::level::fatal, fast_io::concat(__VA_ARGS__))
