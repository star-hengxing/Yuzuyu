includes("header.lua")
includes("build.lua")

rule("bin2lib")
    -- add codegen header
    add_deps("bin2lib.header")
    -- spv2cpp and build cpp
    add_deps("bin2lib.build")
rule_end()
