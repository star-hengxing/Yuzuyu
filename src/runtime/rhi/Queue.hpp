#pragma once

#include "Synchronization.hpp"

NAMESPACE_BEGIN(rhi)

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
    Fence fences[MAX_QUEUE]{};

public:
    Queue() {}

    auto submit(const VkSubmitInfo& info, Family family) noexcept -> void;

    auto transfer(VkCommandBuffer command) noexcept -> void;
};

NAMESPACE_END(rhi)
