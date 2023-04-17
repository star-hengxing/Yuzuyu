target("render")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("**.cpp")
    add_headerfiles("(**.hpp)")

    add_deps("rhi", "resource")
    add_packages("fast_io")
