target("platform")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("*.cpp")
    add_headerfiles("*.hpp", "audio/base.hpp")

    if is_plat("windows") then
        add_files("audio/Xaudio2.cpp")
        add_headerfiles("audio/Xaudio2.hpp")
        add_defines("WIN32_LEAN_AND_MEAN")
        add_syslinks("ole32")
    end

    add_deps("core")
    add_packages("fast_io", {public = true})
