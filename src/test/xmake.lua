add_rules("module.test")

target("io_test")
    set_kind("binary")
    add_files("io.cpp")

    add_deps("io")
target_end()

for _, name in ipairs({"Lexer", "Parser"}) do

target("test." .. name)
    set_kind("binary")
    add_files(name .. ".cpp")

    add_deps("script")
target_end()

end
