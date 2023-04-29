target("shader")
    set_kind("$(kind)")
    add_rules("module.component", "bin2lib")
    add_files("*.vert", "*.frag")

    on_load(function (target)
        local configs = {}
        configs.header = "precompile.hpp"
        configs.namespace = "shader"
        target:extraconf_set("rules", "bin2lib", configs)
        target:extraconf_set("rules", "glsl2spv", {targetenv = "vulkan1.3"})
    end)
