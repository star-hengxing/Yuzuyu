includes("runtime")
includes("xmake")

if has_config("test") then
    includes("test")
end

add_includedirs(path.join("$(projectdir)", "src"), {public = true})

target("runtime")
    set_kind("$(kind)")
    add_rules("module")
    add_files("runtime/Game.cpp")

    add_deps("rhi", "io")
    add_packages("spdlog", "fast_io")
target_end()

target("Game")
    set_kind("binary")
    add_files("runtime/main.cpp")
    add_deps("runtime")
    set_rundir("$(projectdir)")

    on_load(function (target)
        -- remove console
        if is_mode("release") and is_plat("windows") then
            target:add("ldflags", "/SUBSYSTEM:WINDOWS")
            target:add("ldflags", "/ENTRY:mainCRTStartup", {force = true})
        end
    end)
target_end()
