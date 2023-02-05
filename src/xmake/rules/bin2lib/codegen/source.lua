import("core.base.bytes")

function _bin2c(input)
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

function _cpp_codegen(binarydata, sourcefile, namespace)
    local data = _bin2c(binarydata)
    local basename = path.basename(sourcefile)
    return string.format(
        "#include <string_view>\nstatic constexpr unsigned char data[]{%s};\nnamespace %s{std::string_view %s{(char*)data, sizeof(data)};}\n",
        data, namespace, basename)
end

function main(spvfile, cppfile, sourcefile, namespace)
    local file = io.open(cppfile, 'w')
    if file then
        local binarydata = bytes(io.readfile(spvfile, {encoding = "binary"}))
        local cpp = _cpp_codegen(binarydata, sourcefile, namespace)
        file:printf(cpp)
        file:close()
    end
end
