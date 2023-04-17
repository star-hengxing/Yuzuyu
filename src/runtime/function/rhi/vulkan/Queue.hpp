#pragma once

#include "Synchronization.hpp"

NAMESPACE_BEGIN(rhi::vulkan)

inline constexpr auto MAX_QUEUE = usize{4};

struct Queue
{
public:
    enum class Family : u8
    {
        present = 0,
        graphics,
        compute,
        transfer,
    };

public:
    VkDevice device = VK_NULL_HANDLE;

    VkQueue handles[MAX_QUEUE]{};
    u32 indexes[MAX_QUEUE]{};

public:
    auto submit_graphics(const VkSubmitInfo& info, Fence& fence) noexcept -> void;

    auto submit_transfer(VkCommandBuffer command) noexcept -> void;

    auto get_queue(Family family) const noexcept -> VkQueue;

    auto get_index(Family family) const noexcept -> u32;
};

NAMESPACE_END(rhi::vulkan)
