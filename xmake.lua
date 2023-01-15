set_project("yuzuyu")
set_version("0.0.1")

set_xmakever("2.7.5")

set_warnings("all")
set_languages("c++20")

option("modules")
    set_default(false)
    set_showmenu(true)
    set_description("Enable c++ 20 modules (default: false)")
option_end()

add_rules("mode.debug", "mode.releasedbg", "mode.release")

if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
    set_runtimes("MDd")
elseif is_mode("releasedbg") then
    set_symbols("debug")
    set_optimize("smallest")
    set_runtimes("MD")
elseif is_mode("release") then
    set_strip("all")
    set_symbols("hidden")
    set_optimize("smallest")
    set_runtimes("MT")
    add_defines("NDEBUG")
    set_warnings("all", "error")
    set_policy("build.optimization.lto", true)
end

-- support utf-8 on msvc
if is_host("windows") then
    add_defines("UNICODE", "_UNICODE")
    add_cxflags("/execution-charset:utf-8", "/source-charset:utf-8", {tools = "cl"})
end

-- accelerated linking using dynamic libraries
if is_mode("debug", "releasedbg") then
    add_requireconfs("*", {configs = {shared = true}})
end

if has_config("modules") then
    add_defines("USE_MODULES")
    set_policy("build.c++.modules", true)
end

includes("src")
