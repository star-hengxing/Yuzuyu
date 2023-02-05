includes("header.lua")
includes("build.lua")

-- glsl -> spv -> cpp -> obj -> lib
rule("bin2lib")
    -- codegen header
    add_deps("bin2lib.header")
    -- codegen spv.cpp then compile
    add_deps("bin2lib.build")
    -- inherit links and linkdirs of all dependent targets by default
    add_deps("utils.inherit.links")
    -- check targets
    add_deps("utils.check.targets")
rule_end()
