target("rhi.vulkan")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("**.cpp")
    add_headerfiles("(**.hpp)")

    if is_mode("release") then
        add_defines("VK_BOOTSTRAP_WERROR")
    end

    add_deps("window", "shader")
    add_packages("spdlog", "fast_io", "vk-bootstrap")
    add_packages("vulkan-memory-allocator", {public = true})

    if is_mode("debug", "releasedbg") then
        -- https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/debugging_memory_usage.html
        add_defines("VMA_DEBUG_INITIALIZE_ALLOCATIONS=1")
    end

    if has_config("volk") then
        add_options("volk")
        add_packages("volk", {public = true})
        -- https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html
        add_defines("VMA_STATIC_VULKAN_FUNCTIONS")
        add_defines("VMA_DYNAMIC_VULKAN_FUNCTIONS=0")
    else
        add_packages("vulkansdk", {public = true})
    end
