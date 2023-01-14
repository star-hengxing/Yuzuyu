includes("runtime")
includes("xmake")

add_includedirs(path.join("$(projectdir)", "src/engine"), {public = true})

target("runtime")
    set_kind("phony")
    add_deps("window", "rhi")
target_end()
