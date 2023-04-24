#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Log.hpp"

log_system::log_system() : logger{spdlog::stdout_color_st(global::config::NAME)}
{
    logger->set_level(spdlog::level::trace);
}

log_system::~log_system()
{
    logger->flush();
    spdlog::drop_all();
}

auto log_system::get() noexcept -> log_system&
{
    static log_system object;
    return object;
}

auto log_system::log(log_system::level level, const std::string_view str) noexcept -> void
{
    switch (level)
    {
        using enum log_system::level;
        case debug:
            logger->debug(str);
            break;
        case info:
            logger->info(str);
            break;
        case warn:
            logger->warn(str);
            break;
        case error:
            logger->error(str);
            break;
        case fatal:
            logger->critical(str);
            std::abort();
            break;
        default:
            break;
    }
}
