includes("cpu")
includes("shader")

target("io")
    set_kind("$(kind)")
    add_rules("module")
    add_files("io/*.cpp")
target_end()

target("window")
    set_kind("$(kind)")
    add_rules("module")
    add_files("window/*.cpp")
    add_defines("GLFW_INCLUDE_VULKAN")

    add_packages("glfw", "libsdl", "volk", {public = true})
target_end()

target("rhi")
    set_kind("$(kind)")
    add_rules("module")
    add_files("rhi/*.cpp")

    add_deps("window", "shader")
    add_packages("spdlog", "vk-bootstrap")

    on_load(function (target) 
        -- msvc stl source_location cannot work with clang
        if is_host("windows") then
            target:add("defines", "__cpp_consteval")
        end

        if is_mode("release") then
            target:add("defines", "VK_BOOTSTRAP_WERROR")
        end
    end)
target_end()
