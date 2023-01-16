local function header_codegen(output, namespace, sourcebatch)
    output:printf("// do not edit!\n")
    output:printf("#pragma once\n#include <string_view>\n")
    output:printf("#if defined(_WIN32) && defined(LIBRARY_DLL)\n")
    output:printf("#define LIBRARY_IMPORT __declspec(dllimport)\n")
    output:printf("#else\n#define LIBRARY_IMPORT\n#endif\n")
    output:printf(string.format("namespace %s\n{\n", namespace))

    for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
        local basenname = path.basename(sourcefile)
        local line = string.format("LIBRARY_IMPORT\nextern std::string_view %s;\n", basenname)
        output:printf(line)
    end

    output:printf("}\n")
end

rule("bin2lib.header")
    set_extensions(".vert", ".frag")

    on_buildcmd_files(function (target, batchcmds, sourcebatch, opt)
        local header_path = target:extraconf("rules", "bin2lib", "header") or "precompile.hpp"
        local namespace = target:extraconf("rules", "bin2lib", "namespace") or "Shader"
        -- relative project dir
        local absolute_path = path.join(target:scriptdir(), header_path)
        local header_file = path.relative(absolute_path, os.projectdir())

        local time = os.mtime(header_file)
        local continue = false
        for _, sourcefile in ipairs(sourcebatch.sourcefiles) do
            if os.mtime(sourcefile) > time then
                continue = true
                break
            end
        end
        -- TODO: `xmake -r` cannot work
        if continue then
            -- codegen header
            local file = io.open(header_file, 'w')
            if file then
                header_codegen(file, namespace, sourcebatch)
                file:close(file)
            end
            batchcmds:show_progress(opt.progress, "${color.build.object}codegen.shader.header %s", header_file)
        end
        -- add deps
        batchcmds:add_depfiles(sourcebatch.sourcefiles)
        batchcmds:set_depmtime(os.mtime(header_file))
        batchcmds:set_depcache(target:dependfile(header_file))
    end)
rule_end()
