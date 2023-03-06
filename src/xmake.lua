includes("runtime")
includes("xmake")
includes("test")

add_includedirs(path.join("$(projectdir)", "src"), {public = true})

target("runtime")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("runtime/Game.cpp")

    add_deps("rhi", "io")
    add_packages("spdlog", "fast_io")
target_end()

target("Game")
    set_kind("binary")
    add_files("runtime/main.cpp")

    add_deps("runtime")

    set_rundir("$(projectdir)")

    -- remove console
    if is_mode("release") and is_plat("windows") then
        add_ldflags("/SUBSYSTEM:WINDOWS")
        add_ldflags("/ENTRY:mainCRTStartup", {force = true})
    end
target_end()
