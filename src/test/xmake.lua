set_optimize("none")
set_symbols("debug")
set_policy("build.warning", true)

if is_plat("windows") then
    set_runtimes("MDd")
end

add_packages("boost_ut", "fast_io")

set_rundir("$(projectdir)")
set_group("test")

target("io_test")
    set_kind("binary")
    add_files("*.cpp")

    add_deps("io")
target_end()
