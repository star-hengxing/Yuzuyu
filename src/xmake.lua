add_requires(
    -- vulkan
    "volk",
    "vk-bootstrap v0.6",
    "glfw", -- window
    "spdlog" -- log
)

includes("engine")

target("yuzuyu")
    set_kind("binary")
    add_files("main.cpp")

    add_deps("runtime")
    add_packages("spdlog")

    set_rundir("$(projectdir)")

    -- after_build(after_build_run)
target_end()

local function after_build_run(target)
    -- os.exec("xmake lua src/engine/xmake/utils/archive.lua")
    local rundir = target:rundir()
    local targetfile = path.absolute(target:targetfile())
    local args = table.wrap(target:get("runargs"))
    os.execv(targetfile, args, {curdir = rundir})
end
