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

    add_packages("fast_io")
    add_packages("filesystem", {public = true})
target_end()

target("window")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("window/*.cpp")
    add_defines("GLFW_INCLUDE_VULKAN")

    add_packages("fast_io")
    add_packages("glfw", "libsdl", "volk", {public = true})
target_end()

target("rhi")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("rhi/*.cpp")

    add_deps("window", "shader")
    add_packages("spdlog", "vk-bootstrap")

    if is_mode("release") then
        add_defines("VK_BOOTSTRAP_WERROR")
    end
target_end()

target("script")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("script/*.cpp")

    add_deps("io")
    add_packages("fast_io")
target_end()
