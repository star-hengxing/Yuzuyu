#ifdef _WIN32
#include <windows.h>
#else
#define _POSIX_C_SOURCE 199309L
#include <ctime>
#endif

#include "os.hpp"

NAMESPACE_BEGIN(os)

auto time() noexcept -> fast_io::unix_timestamp 
{
    // using namespace std::chrono;
    // return duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
    return fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime);
}

auto sleep(usize milliseconds) noexcept -> void
{
#ifdef _WIN32
    ::Sleep(static_cast<DWORD>(milliseconds));
#else
    const timespec rtqp
    {
        .tv_sec  = static_cast<std::time_t>(milliseconds / 1000),
        .tv_nsec = static_cast<long>((milliseconds % 1000) * 1000000),
    };
    ::nanosleep(&rtqp, nullptr);
#endif
}

NAMESPACE_END(os)
