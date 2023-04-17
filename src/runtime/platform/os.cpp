#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
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
    Sleep(milliseconds);
#else
    static_assert(false, "unimplemented!");
    // TODO
#endif
}

NAMESPACE_END(os)
