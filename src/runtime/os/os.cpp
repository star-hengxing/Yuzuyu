#ifdef USE_MODULES
import <chrono>;
#else
#include <chrono>
#endif

#include "os.hpp"

NAMESPACE_BEGIN(os)
NAMESPACE_BEGIN(detail)

auto time() -> usize
{
    using namespace std::chrono;
    return duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
}

NAMESPACE_END(os)
NAMESPACE_END(detail)
