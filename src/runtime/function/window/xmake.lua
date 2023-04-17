target("window")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("**.cpp")
    add_headerfiles("(**.hpp)")

    add_defines("GLFW_INCLUDE_VULKAN")

    add_packages("fast_io")
    add_packages("glfw", "libsdl", {public = true})

    if has_config("volk") then
        add_options("volk")
        add_packages("volk", {public = true})
    else
        add_packages("vulkansdk", {public = true})
    end
