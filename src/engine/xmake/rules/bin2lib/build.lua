local function bin2c(input)
    local buffer = {}

    local i = 0
    local n = 147
    local p = 0
    local e = input:size()
    local line = nil
    local linewidth = 0x20
    local first = true
    while p < e do
        line = ""
        if p + linewidth <= e then
            for i = 0, linewidth - 1 do
                if first then
                    first = false
                    line = line .. " "
                else
                    line = line .. ","
                end
                line = line .. string.format(" 0x%02X", input[p + i + 1])
            end
            table.insert(buffer, line)
            p = p + linewidth
        elseif p < e then
            local left = e - p
            for i = 0, left - 1 do
                if first then
                    first = false
                    line = line .. " "
                else
                    line = line .. ","
                end
                line = line .. string.format(" 0x%02X", input[p + i + 1])
            end
            table.insert(buffer, line)
            p = p + left
        else
            break
        end
    end
    return table.concat(buffer)
end

local function cpp_codegen(input, sourcefile, namespace)
    local data = bin2c(input)
    local basename = path.basename(sourcefile)
    return string.format(
        "#include <string_view>\nstatic constexpr unsigned char data[]{%s};\nnamespace %s{std::string_view %s{(char*)data, sizeof(data)};}\n",
        data, namespace, basename)
end

rule("bin2lib.build")
    set_extensions(".vert", ".frag")
    -- after shader compiled
    add_deps("glsl2spv", {order = true})
    -- compile spv2cpp
    add_deps("c++")
    -- inherit links and linkdirs of all dependent targets by default
    add_deps("utils.inherit.links")

    on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
        import("core.base.bytes")
        -- get spv file
        local spvdir = path.join(target:autogendir(), "rules", "glsl2spv")
        local spvfile = path.join(spvdir, path.filename(sourcefile) .. ".spv")
        -- get cpp file
        local outputdir = path.join(target:autogendir(), "rules", "bin2lib")
        local cppfile = path.join(outputdir, path.filename(sourcefile) .. ".spv.cpp")

        if os.mtime(spvfile) > os.mtime(cppfile) then
            batchcmds:mkdir(outputdir)
            batchcmds:show_progress(opt.progress, "${color.build.object}codegen.spv2cpp %s", spvfile)
            -- spv -> cpp
            local file = io.open(cppfile, 'w')
            if file then
                local binarydata = bytes(io.readfile(spvfile, {encoding = "binary"}))
                local namespace = target:extraconf("rules", "bin2lib", "namespace") or "Shader"
                local cpp = cpp_codegen(binarydata, sourcefile, namespace)
                file:printf(cpp)
                file:close()
            end
        end
        -- add objectfile
        local objectfile = target:objectfile(cppfile)
        table.insert(target:objectfiles(), objectfile)
        -- compile spv.cpp
        batchcmds:show_progress(opt.progress, "${color.build.object}compiling.spv.cpp %s", cppfile)
        batchcmds:mkdir(path.directory(cppfile))
        batchcmds:compile(cppfile, objectfile)
        -- add deps
        batchcmds:add_depfiles(sourcefile)
        batchcmds:set_depmtime(os.mtime(objectfile))
        batchcmds:set_depcache(target:dependfile(objectfile))
    end)
rule_end()
