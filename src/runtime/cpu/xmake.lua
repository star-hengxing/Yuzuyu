target("cpu")
    set_kind("binary")
    add_files("**.cpp")

    add_deps("window", "input", "io")
    add_packages("fast_io")

    set_rundir("$(projectdir)")
    before_run(function (target)
        os.execv("xmake build " .. target:name())
    end)
target_end()
