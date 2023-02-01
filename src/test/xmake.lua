set_optimize("none")
set_symbols("debug")
set_policy("build.warning", true)

if is_plat("windows") then
    set_runtimes("MDd")
end

add_packages("doctest", "fast_io")

target("io_test")
    set_kind("binary")
    add_files("*.cpp")

    add_deps("io")

    set_rundir("$(projectdir)")
target_end()
