#include <fast_io.h>

#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

#include "Device.hpp"

NAMESPACE_BEGIN(rhi::vulkan)

auto Device::wait_idle() const noexcept -> void
{
    vkDeviceWaitIdle(handle);
}

auto Device::clean() noexcept -> void
{
    wait_idle();

    if (allocator)
    {
        vmaDestroyAllocator(allocator);
        allocator = VK_NULL_HANDLE;
    }

    if (handle)
    {
        vkDestroyDevice(handle, VK_NULL_HANDLE);
        handle = VK_NULL_HANDLE;
    }

    if (debug_messenger)
    {
        vkb::destroy_debug_utils_messenger(instance, debug_messenger);
        debug_messenger = VK_NULL_HANDLE;
    }
    // window system owner
    // if (surface)
    // {
    //     vkDestroySurfaceKHR(instance, surface, VK_NULL_HANDLE);
    //     surface = VK_NULL_HANDLE;
    // }

    if (instance)
    {
        vkDestroyInstance(instance, VK_NULL_HANDLE);
        instance = VK_NULL_HANDLE;
    }
}

auto Device::initialize(const std::function<VkSurfaceKHR(VkInstance)>& callback) noexcept -> bool
{
#ifdef USE_VOLK
    // volk: initialize -> load instance -> load device
    if (volkInitialize() != VK_SUCCESS)
        return false;

    auto builder = vkb::InstanceBuilder{vkGetInstanceProcAddr};
#else
    auto builder = vkb::InstanceBuilder{};
#endif

    auto inst_ret = builder.set_app_name(global::config::NAME)
                        .request_validation_layers(true)
                        .require_api_version(MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION)
                        .use_default_debug_messenger()
                        .build();
    if (!inst_ret)
    {
        perrln("Failed to create Vulkan instance\n", inst_ret.error().message());
        return false;
    }

    auto vkb_inst = inst_ret.value();

    instance = vkb_inst.instance;
    debug_messenger = vkb_inst.debug_messenger;
#ifdef USE_VOLK
    volkLoadInstance(instance);
#endif
    surface = std::invoke(callback, instance);
    if (!surface)
    {
        perr("Failed to create window surface\n");
        return false;
    }

    auto selector = vkb::PhysicalDeviceSelector{vkb_inst, surface};
    auto phys_ret = selector.set_minimum_version(MAJOR_VERSION, MINOR_VERSION).select();
    if (!phys_ret)
    {
        perrln("Failed to select Vulkan Physical Device\n", phys_ret.error().message());
        vkb::destroy_debug_utils_messenger(instance, debug_messenger);
        vkDestroySurfaceKHR(instance, surface, VK_NULL_HANDLE);
        vkDestroyInstance(instance, VK_NULL_HANDLE);
        return false;
    }

    vkb::DeviceBuilder device_builder{phys_ret.value()};
    // automatically propagate needed data from instance & physical device
    auto dev_ret = device_builder.build();
    if (!dev_ret)
    {
        perrln("Failed to create Vulkan device\n", phys_ret.error().message());
        vkb::destroy_debug_utils_messenger(instance, debug_messenger);
        vkDestroySurfaceKHR(instance, surface, VK_NULL_HANDLE);
        vkDestroyInstance(instance, VK_NULL_HANDLE);
        return false;
    }

    vkb::Device vkb_device = dev_ret.value();

    gpu = vkb_device.physical_device;
    handle = vkb_device.device;
#ifdef USE_VOLK
    volkLoadDevice(handle);
#endif

    queue.handles[static_cast<usize>(Queue::Family::present)]  = vkb_device.get_queue(vkb::QueueType::present).value();
    queue.handles[static_cast<usize>(Queue::Family::graphics)] = vkb_device.get_queue(vkb::QueueType::graphics).value();
    queue.handles[static_cast<usize>(Queue::Family::compute)]  = vkb_device.get_queue(vkb::QueueType::compute).value();
    queue.handles[static_cast<usize>(Queue::Family::transfer)] = vkb_device.get_queue(vkb::QueueType::transfer).value();

    queue.indexes[static_cast<usize>(Queue::Family::present)]  = vkb_device.get_queue_index(vkb::QueueType::present).value();
    queue.indexes[static_cast<usize>(Queue::Family::graphics)] = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
    queue.indexes[static_cast<usize>(Queue::Family::compute)]  = vkb_device.get_queue_index(vkb::QueueType::compute).value();
    queue.indexes[static_cast<usize>(Queue::Family::transfer)] = vkb_device.get_queue_index(vkb::QueueType::transfer).value();

    queue.device = handle;

#ifdef USE_VOLK
    const auto vma_fn_info = VmaVulkanFunctions
    {
        .vkGetInstanceProcAddr               = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr                 = vkGetDeviceProcAddr,
        .vkGetPhysicalDeviceProperties       = vkGetPhysicalDeviceProperties,
        .vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
        .vkAllocateMemory                    = vkAllocateMemory,
        .vkFreeMemory                        = vkFreeMemory,
        .vkMapMemory                         = vkMapMemory,
        .vkUnmapMemory                       = vkUnmapMemory,
        .vkFlushMappedMemoryRanges           = vkFlushMappedMemoryRanges,
        .vkInvalidateMappedMemoryRanges      = vkInvalidateMappedMemoryRanges,
        .vkBindBufferMemory                  = vkBindBufferMemory,
        .vkBindImageMemory                   = vkBindImageMemory,
        .vkGetBufferMemoryRequirements       = vkGetBufferMemoryRequirements,
        .vkGetImageMemoryRequirements        = vkGetImageMemoryRequirements,
        .vkCreateBuffer                      = vkCreateBuffer,
        .vkDestroyBuffer                     = vkDestroyBuffer,
        .vkCreateImage                       = vkCreateImage,
        .vkDestroyImage                      = vkDestroyImage,
        .vkCmdCopyBuffer                     = vkCmdCopyBuffer,
    };
#endif

    const auto vma_info = VmaAllocatorCreateInfo
    {
        .physicalDevice   = gpu,
        .device           = handle,
#ifdef USE_VOLK
        .pVulkanFunctions = &vma_fn_info,
#endif
        .instance         = instance,
        .vulkanApiVersion = VK_MAKE_API_VERSION(0, MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION),
    };

    return vmaCreateAllocator(&vma_info, &allocator) == VK_SUCCESS;
}

NAMESPACE_END(rhi::vulkan)
