set_project("yuzuyu")
set_version("0.0.1")

set_xmakever("2.7.5")

set_warnings("all")
set_languages("c++20")

add_rules("mode.debug", "mode.releasedbg", "mode.release")

if is_mode("debug") then
    set_optimize("none")
elseif is_mode("releasedbg") then
    set_optimize("smallest")
elseif is_mode("release") then
    set_strip("all")
    set_symbols("hidden")
    set_optimize("smallest")
    add_defines("NDEBUG")
    set_policy("build.optimization.lto", true)
end

if is_mode("debug", "releasedbg") then
    set_symbols("debug")
    set_policy("build.warning", true)
    -- use dynamic libraries acceleration on linking
    add_defines("LIBRARY_DLL")
    add_requireconfs("*", {configs = {shared = true}})
end

if is_plat("windows") then
    if is_mode("debug") then
        set_runtimes("MDd")
    elseif is_mode("releasedbg") then
        set_runtimes("MD")
    else
        set_runtimes("MT")
    end
end
-- support utf-8 on msvc
if is_host("windows") then
    add_defines("UNICODE", "_UNICODE")
    add_cxflags("/utf-8", {tools = "cl"})
end
-- use stl header units acceleration on compiling
if has_config("modules") then
    add_defines("USE_MODULES")
    set_policy("build.c++.modules", true)
end

includes("src")
