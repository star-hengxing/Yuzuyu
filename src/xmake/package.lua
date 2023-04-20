-- dev

-- vulkan
if has_config("volk") then
    add_requires("volk", {configs = {header_only = true}})
else
    add_requires("vulkansdk")
end
add_requires("vk-bootstrap v0.7")
add_requires("vulkan-memory-allocator")
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
-- text rendering
add_requires("freetype")
-- video/audio
add_requires("ffmpeg")

-- test
if has_config("test") then
    add_requires("boost_ut")
end
