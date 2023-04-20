target("platform")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("**.cpp")
    add_headerfiles("(**.hpp)")

    if is_plat("windows") then
        add_defines("WIN32_LEAN_AND_MEAN")
        add_files("audio/xaudio2.cpp")
        add_syslinks("ole32")
    end

    add_packages("fast_io", "filesystem", {public = true})
