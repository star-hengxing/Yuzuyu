function main(headerfile, namespace, sourcefiles)
    local file = io.open(headerfile, 'w')
    if file then
        file:printf("// do not edit!\n")
        file:printf("#pragma once\n#include <string_view>\n")
        file:printf("#if defined(_WIN32) && defined(LIBRARY_DLL)\n")
        file:printf("#define LIBRARY_IMPORT __declspec(dllimport)\n")
        file:printf("#else\n#define LIBRARY_IMPORT\n#endif\n")
        file:printf(string.format("namespace %s\n{\n", namespace))
    
        for _, sourcefile in ipairs(sourcefiles) do
            local basenname = path.basename(sourcefile)
            local line = string.format("LIBRARY_IMPORT\nextern std::string_view %s;\n", basenname)
            file:printf(line)
        end

        file:printf("}\n")
        file:close(file)
    end
end
