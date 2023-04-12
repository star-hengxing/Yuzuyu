rule("glsl2spv")
    set_extensions(".vert", ".frag")

    on_build_file(function (target, sourcefile, opt)
        import("lib.detect.find_tool")
        import("core.project.depend")
        import("utils.progress")

        local compiler = assert(find_tool("glslangValidator") or find_tool("glslc"), "glslangValidator or glslc not found!")

        local targetdir = path.join(target:autogendir(), "rules", "glsl2spv")
        os.mkdir(targetdir)
        -- glsl -> spirv
        local targetfile  = path.join(targetdir, path.filename(sourcefile) .. ".spv")
        target:add("cleanfiles", targetfile)
        
        local targetenv = target:extraconf("rules", "glsl2spv", "targetenv") or "vulkan1.0"
        local args = {"--target-env", targetenv, "-o", path(targetfile), path(sourcefile)}

        depend.on_changed(function ()
            os.vrunv(compiler.program, args)
            progress.show(opt.progress, "${color.build.object}compiling.glsl %s", sourcefile)
        end, {files = sourcefile})
    end)
