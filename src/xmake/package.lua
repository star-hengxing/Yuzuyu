-- vulkan
add_requires("volk")
add_requires("vk-bootstrap v0.6")
-- cross-platform window
add_requires("glfw")
add_requires("libsdl")
-- log
add_requires("spdlog")
-- i/o
add_requires("fast_io")
-- image i/o
add_requires("stb")
-- erro handle
add_requires("tl_expected master", {alias = "expected"})
