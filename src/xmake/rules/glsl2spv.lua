rule("glsl2spv")
    set_extensions(".vert", ".frag")

    on_buildcmd_file(function (target, batchcmds, sourcefile_glsl, opt)
        import("lib.detect.find_tool")
        -- get glslangValidator
        local glslc
        local glslangValidator = find_tool("glslangValidator")
        if not glslangValidator then
            glslc = find_tool("glslc")
        end
        assert(glslangValidator or glslc, "glslangValidator or glslc not found!")
        -- glsl to spv
        local targetenv = target:extraconf("rules", "glsl2spv", "targetenv") or "vulkan1.0"
        local outputdir = path.join(target:autogendir(), "rules", "glsl2spv")
        local spvfilepath = path.join(outputdir, path.filename(sourcefile_glsl) .. ".spv")
        batchcmds:show_progress(opt.progress, "${color.build.object}compiling.glsl %s", sourcefile_glsl)
        batchcmds:mkdir(outputdir)

        local args = {"--target-env", targetenv, "-o", path(spvfilepath), path(sourcefile_glsl)}
        if glslangValidator then
            batchcmds:vrunv(glslangValidator.program, args)
        else
            batchcmds:vrunv(glslc.program, args)
        end
        -- add deps
        batchcmds:add_depfiles(sourcefile_glsl)
        batchcmds:set_depmtime(os.mtime(spvfilepath))
        batchcmds:set_depcache(target:dependfile(spvfilepath))
    end)
rule_end()
