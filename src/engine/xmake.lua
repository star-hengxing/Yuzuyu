add_includedirs(path.join("$(projectdir)", "src/engine"), {public = true})

function componet_config(target)
    if is_mode("debug", "releasedbg") then
        target:set("kind", "shared")
        if is_plat("windows") then
            import("core.project.rule")
            local rule = rule.rule("utils.symbols.export_all")
            target:rule_add(rule)
            target:extraconf_set("rules", "utils.symbols.export_all", {export_classes = true})
        end
    elseif is_mode("release") then
        target:set("kind", "static")
    else
        assert(false, "Unknown build kind")
    end
end

includes("runtime")
includes("xmake")

target("runtime")
    set_kind("phony")
    add_deps("window", "rhi")
target_end()
