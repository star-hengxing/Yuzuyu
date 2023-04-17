add_rules("module.test")

add_includedirs(path.join("$(projectdir)", "src", "runtime"))

target("test.resource")
    set_kind("binary")
    add_files("resource.cpp")

    add_deps("resource")
target_end()

for _, name in ipairs({"Lexer", "Parser"}) do

target("test." .. name)
    set_kind("binary")
    add_files(name .. ".cpp")

    add_deps("script")
target_end()

end
