#ifdef USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#endif
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <spdlog/spdlog.h>

#include "base.hpp"

auto check_vulkan_result(VkResult result, const std::source_location location)
    -> void
{
    if (result == VK_SUCCESS) [[likely]]
        return;

    spdlog::error("[{}:{}] {}: vulkan result error",
        location.file_name(), location.line(), location.function_name());
}
