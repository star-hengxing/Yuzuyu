target("Multimedia")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("**.cpp")
    add_headerfiles("(**.hpp)")

    add_deps("platform")
    add_packages("fast_io")
    add_packages("ffmpeg", {public = true})
