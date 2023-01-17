includes("header.lua")
includes("build.lua")

rule("bin2lib")
    -- add codegen header
    add_deps("bin2lib.header")
    -- spv2cpp and build cpp
    add_deps("bin2lib.build")
    -- inherit links and linkdirs of all dependent targets by default
    add_deps("utils.inherit.links")
    -- check targets
    add_deps("utils.check.targets")
rule_end()
