includes("shader")

target("io")
    set_kind("$(kind)")
    add_files("io/*.cpp")

    on_load(componet_config)
target_end()

target("window")
    set_kind("$(kind)")
    add_files("window/*.cpp")
    add_defines("GLFW_INCLUDE_VULKAN")
    add_packages("glfw", "volk", {public = true})

    on_load(componet_config)
target_end()

target("rhi")
    set_kind("$(kind)")
    add_files("rhi/*.cpp")
    add_deps("window", "shader")
    add_packages("spdlog", "vk-bootstrap")
    -- msvc stl source_location cannot work with clang
    if is_host("windows") then
        add_defines("__cpp_consteval")
    end

    if is_mode("release") then
        add_defines("VK_BOOTSTRAP_WERROR")
    end

    on_load(componet_config)
target_end()
