rule("bin2lib.header")
    set_extensions(".vert", ".frag")

    on_config(function (target)
        local header_path = target:extraconf("rules", "bin2lib", "header") or "precompile.hpp"
        local namespace = target:extraconf("rules", "bin2lib", "namespace") or "shader"

        local headerfile = path.relative(path.join(target:scriptdir(), header_path), os.projectdir())
        local sourcefiles
        for _, sourcebatch in pairs(target:sourcebatches()) do
            if sourcebatch.rulename == "bin2lib.header" then
                sourcefiles = sourcebatch.sourcefiles
                break
            end
        end

        assert(sourcefiles)

        if not os.isfile(headerfile) then
            import("codegen.header")(headerfile, namespace, sourcefiles)
            cprint("${green}codegen.shader.header: ${white}%s", headerfile)
            return
        end

        local time = os.mtime(headerfile)
        for _, sourcefile in ipairs(sourcefiles) do
            if os.mtime(sourcefile) > time then
                import("codegen.header")(headerfile, namespace, sourcefiles)
                cprint("${green}codegen.shader.header: ${white}%s", headerfile)
                return
            end
        end
    end)

    on_clean(function (target)
        local header_path = target:extraconf("rules", "bin2lib", "header") or "precompile.hpp"
        local headerfile = path.join(target:scriptdir(), header_path)
        os.tryrm(headerfile)
    end)
rule_end()
