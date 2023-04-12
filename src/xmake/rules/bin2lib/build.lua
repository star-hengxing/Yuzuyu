rule("bin2lib.build")
    set_extensions(".vert", ".frag")
    -- after shader compiled
    add_deps("glsl2spv", {order = true})
    -- compile spv2cpp
    add_deps("c++")

    on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
        -- get spv file
        local spvdir = path.join(target:autogendir(), "rules", "glsl2spv")
        local spvfile = path.join(spvdir, path.filename(sourcefile) .. ".spv")
        -- get cpp file
        local outputdir = path.join(target:autogendir(), "rules", "bin2lib")
        local cppfile = path.join(outputdir, path.filename(sourcefile) .. ".spv.cpp")
        target:add("cleanfiles", cppfile)

        batchcmds:mkdir(outputdir)
        batchcmds:show_progress(opt.progress, "${color.build.object}codegen.spv2cpp %s", spvfile)
        local namespace = target:extraconf("rules", "bin2lib", "namespace") or "shader"
        -- spv -> cpp
        local argv = {"lua", "src/xmake/rules/bin2lib/codegen/source.lua", spvfile, cppfile, sourcefile, namespace}
        batchcmds:vrunv(os.programfile(), argv, {envs = {XMAKE_SKIP_HISTORY = "y"}})
        -- add objectfile
        local objectfile = target:objectfile(cppfile)
        table.insert(target:objectfiles(), objectfile)
        -- compile spv.cpp
        batchcmds:show_progress(opt.progress, "${color.build.object}compiling.%s %s", get_config("mode"), cppfile)
        batchcmds:compile(cppfile, objectfile)
        -- add deps
        batchcmds:add_depfiles(sourcefile)
        local dependfile = target:dependfile(objectfile)
        batchcmds:set_depmtime(os.mtime(dependfile))
        batchcmds:set_depcache(dependfile)
    end)
