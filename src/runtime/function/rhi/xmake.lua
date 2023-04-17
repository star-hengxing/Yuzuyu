includes("cpu")
includes("vulkan")

target("rhi")
    set_kind("phony")
    add_headerfiles("(**.hpp)")
    add_deps("rhi.cpu", "rhi.vulkan")
