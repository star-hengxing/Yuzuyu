target("core")
    set_kind("$(kind)")
    add_rules("module.component")
    add_files("*.cpp")
    add_headerfiles("(**.hpp)")

    add_packages("spdlog", "fast_io", {public = true})
