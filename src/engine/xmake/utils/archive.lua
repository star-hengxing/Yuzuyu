import("core.project.project")
import("lib.detect.find_tool")

function main()
    -- windows use 7z and linux use p7zip
    local tool = assert(find_tool("7z"), "7z not found!")
    -- ensure that in the working directory
    os.cd(os.projectdir())
    -- TODO: xmake project name as same as directory name?
    -- build/xxx.7z
    local outputfile = path.join("build", project.name() .. ".7z")
    os.mkdir("build")
    -- try to remove old archive
    -- os.tryrm(outputfile)

    local args = {"a", outputfile}
    table.insert(args, path.join("..", project.name()))
    -- exclude directory and files
    table.insert(args, "-xr!.git/")
    table.insert(args, "-xr!.vscode/")
    table.insert(args, "-xr!.xmake/")
    table.insert(args, "-xr!build/")
    table.insert(args, "-xr!" .. outputfile)

    cprint("${green}archiveing " .. outputfile)
    os.runv(tool.program, args)
end
