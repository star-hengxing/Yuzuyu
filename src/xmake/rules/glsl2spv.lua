rule("glsl2spv")
    set_extensions(".vert", ".frag")

    on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
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
        local spvfile = path.join(outputdir, path.filename(sourcefile) .. ".spv")
        batchcmds:show_progress(opt.progress, "${color.build.object}compiling.glsl %s", sourcefile)
        batchcmds:mkdir(outputdir)

        local args = {"--target-env", targetenv, "-o", path(spvfile), path(sourcefile)}
        if glslangValidator then
            batchcmds:vrunv(glslangValidator.program, args)
        else
            batchcmds:vrunv(glslc.program, args)
        end
        -- add deps
        batchcmds:add_depfiles(sourcefile)
        local dependfile = target:dependfile(spvfile)
        batchcmds:set_depmtime(os.mtime(dependfile))
        batchcmds:set_depcache(dependfile)
    end)

    on_clean(function (target)
        local outputdir = path.join(target:autogendir(), "rules", "glsl2spv")
        for _, sourcebatch in pairs(target:sourcebatches()) do
            if sourcebatch.rulename == "glsl2spv" then
                for _, sourcefile in pairs(sourcebatch.sourcefiles) do
                    local file = path.join(outputdir, path.filename(sourcefile) .. ".spv")
                    os.tryrm(file)
                end
            end
        end
    end)
rule_end()
