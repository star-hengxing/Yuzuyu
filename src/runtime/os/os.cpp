#include <fast_io.h>

#include "os.hpp"

NAMESPACE_BEGIN(os)
NAMESPACE_BEGIN(detail)

auto time() -> i64
{
    // using namespace std::chrono;
    // return duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
    return fast_io::posix_clock_gettime(fast_io::posix_clock_id::realtime).seconds;
}

NAMESPACE_END(os)
NAMESPACE_END(detail)
