-- dev

-- vulkan
add_requires("volk")
add_requires("vk-bootstrap v0.7")
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

package("filesystem")
    set_base("ghc_filesystem")
    on_test(function () end)
package_end()
-- filesystem
add_requires("filesystem")
-- video/audio
add_requires("ffmpeg")

-- test
if has_config("test") then
    add_requires("boost_ut")
end
