includes("cpu")
includes("shader")

target("input")
    set_kind("$(kind)")
    add_rules("module")
    add_files("input/*.cpp")

    add_packages("libsdl", {public = true})
target_end()

target("io")
    set_kind("$(kind)")
    add_rules("module")
    add_files("io/*.cpp")

    add_packages("fast_io", "stb")
    add_packages("expected", "freetype", {public = true})
target_end()

target("os")
    set_kind("$(kind)")
    add_rules("module")
    add_files("os/*.cpp")

    add_packages("fast_io")
    add_packages("filesystem", {public = true})
target_end()

target("window")
    set_kind("$(kind)")
    add_rules("module")
    add_files("window/*.cpp")
    add_defines("GLFW_INCLUDE_VULKAN")

    add_packages("fast_io")
    add_packages("glfw", "libsdl", "volk", {public = true})
target_end()

target("rhi")
    set_kind("$(kind)")
    add_rules("module")
    add_files("rhi/*.cpp")

    add_deps("window", "shader")
    add_packages("spdlog", "vk-bootstrap")

    on_load(function (target)
        -- @see https://github.com/llvm/llvm-project/issues/53906
        if is_host("windows") then
            for _, toolchain_inst in ipairs(target:toolchains()) do
                if toolchain_inst:name():startswith("clang") then
                    target:add("defines", "__cpp_consteval=201811")
                    break
                end
            end
        end

        if is_mode("release") then
            target:add("defines", "VK_BOOTSTRAP_WERROR")
        end
    end)
target_end()
