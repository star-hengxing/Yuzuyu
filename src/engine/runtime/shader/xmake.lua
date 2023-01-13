-- add_requires("glslang", {configs = {binaryonly = true}})

target("shader")
    set_kind("$(kind)")
    add_files("*.vert", "*.frag")
    -- add_packages("glslang")

    add_rules("bin2lib")
    on_load(componet_config)

    after_load(function (target)
        local configs = 
        {
            header = "precompile.hpp",
            namespace = "shader"
        }
        target:extraconf_set("rules", "bin2lib.header", configs)
    end)
target_end()
