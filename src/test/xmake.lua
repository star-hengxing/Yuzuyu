add_rules("module.test")

target("io_test")
    set_kind("binary")
    add_files("io.cpp")

    add_deps("io")
target_end()

target("script_test")
    set_kind("binary")
    add_files("Lexer.cpp")

    add_deps("script")
target_end()
