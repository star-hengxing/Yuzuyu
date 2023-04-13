#include "Buffer.hpp"
#include "base.hpp"

NAMESPACE_BEGIN(rhi)

auto Buffer::clean() noexcept -> void
{
    if (handle && allocation)
    {
        vmaDestroyBuffer(device->allocator, handle, allocation);
        handle = VK_NULL_HANDLE;
    }
}

Buffer::Buffer(Device* device, usize size, VkBufferUsageFlags flags, VmaMemoryUsage type)
    : device{device}, size{size}
{
    const auto buffer_info = VkBufferCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = flags,
    };

    const auto vma_info = VmaAllocationCreateInfo
    {
        .usage = type,
    };

    CHECK_RESULT(vmaCreateBuffer(device->allocator, &buffer_info, &vma_info, &handle, &allocation, VK_NULL_HANDLE));
}

auto Buffer::map() noexcept -> u8*
{
    vmaMapMemory(device->allocator, allocation, &data);
    return reinterpret_cast<u8*>(data);
}

auto Buffer::unmap() noexcept -> void
{
    vmaUnmapMemory(device->allocator, allocation);
}

auto Buffer::flush() noexcept -> void
{
    vmaFlushAllocation(device->allocator, allocation, 0, size);
}

auto Buffer::cpu_to_gpu(const u8* data, usize size_, usize offset) noexcept -> void
{
    std::memcpy(map() + offset, data, size_);
    flush();
    unmap();
}

auto Buffer::gpu_to_cpu(u8* data, usize size_, usize offset) noexcept -> void
{
    std::memcpy(data, map() + offset, size_);
    flush();
    unmap();
}

NAMESPACE_END(rhi)
