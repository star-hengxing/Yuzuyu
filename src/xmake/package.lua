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
-- image codec
add_requires("libpng")
add_requires("libjpeg-turbo", {configs = {debug = false}}) -- compilation failure on windows
-- image resize
add_requires("avir")
-- text rendering
add_requires("freetype")
-- video/audio codec
add_requires("ffmpeg")

-- test
if has_config("test") then
    add_requires("boost_ut")
end
