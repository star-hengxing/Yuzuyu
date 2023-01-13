add_requires(
    "volk",
    "vk-bootstrap v0.6"
)

add_requires(
    "glfw",
    "spdlog"
)

includes("engine")

target("yuzuyu")
    set_kind("binary")
    add_files("main.cpp")

    add_deps("runtime")
    add_packages("spdlog")

    set_rundir("$(projectdir)")

    after_build(function (target)
        -- import("core.project.project")
        -- local args = {}
        -- local outputfile = project.name() .. ".7z"
        -- os.tryrm(outputfile)
        -- table.insert(args, "u")
        -- table.insert(args, outputfile)
        -- table.insert(args, "../" .. project.name())
        -- table.insert(args, "-xr!.git/")
        -- table.insert(args, "-xr!.vscode/")
        -- table.insert(args, "-xr!.xmake/")
        -- table.insert(args, "-xr!build/")
        -- table.insert(args, "-xr!" .. outputfile)
        -- os.execv("7z", args)
        -- local rundir = target:rundir()
        -- local targetfile = path.absolute(target:targetfile())
        -- local args = table.wrap(target:get("runargs"))
        -- os.execv(targetfile, args, {curdir = rundir})
    end)
target_end()
