#pragma once

#ifdef _WIN32
#include "audio/Xaudio2.hpp"
#else
static_assert(false, "unimplemented");
#endif

NAMESPACE_BEGIN(os::audio)

using audio_system = 
#ifdef _WIN32
::os::audio::detail::Xaudio2
#else
#endif
;

NAMESPACE_END(os::audio)
