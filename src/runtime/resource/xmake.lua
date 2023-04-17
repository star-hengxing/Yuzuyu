target("resource")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("**.cpp")
    add_headerfiles("(**.hpp)")

    add_packages("fast_io", "stb")
    add_packages("expected", "freetype", {public = true})
