includes("cpu")
includes("shader")

target("input")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("input/*.cpp")

    add_packages("libsdl", {public = true})
target_end()

target("io")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("io/*.cpp")

    add_packages("fast_io", "stb")
    add_packages("expected", "freetype", {public = true})
target_end()

target("os")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("os/*.cpp")

    if is_plat("windows") then
        add_files("os/audio/xaudio2.cpp")
        add_syslinks("ole32")
    end

    add_packages("fast_io", "filesystem", {public = true})
target_end()

target("window")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("window/*.cpp")
    add_defines("GLFW_INCLUDE_VULKAN")

    add_packages("fast_io")
    add_packages("glfw", "libsdl", {public = true})
    if has_config("volk") then
        add_options("volk")
        add_packages("volk", {public = true})
    else
        add_packages("vulkansdk", {public = true})
    end
target_end()

target("rhi")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("rhi/*.cpp")

    if is_mode("release") then
        add_defines("VK_BOOTSTRAP_WERROR")
    end

    add_deps("window", "shader")
    add_packages("spdlog", "fast_io", "vk-bootstrap")
    add_packages("vulkan-memory-allocator", {public = true})
    if has_config("volk") then
        add_options("volk")
        add_packages("volk", {public = true})
    else
        add_packages("vulkansdk", {public = true})
    end
target_end()

target("script")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("script/*.cpp")

    add_deps("io")
    add_packages("fast_io")
target_end()

target("Multimedia")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("Multimedia/*.cpp")

    add_deps("os")
    add_packages("fast_io")
    add_packages("ffmpeg", {public = true})
target_end()
