local components =
{
    "core",
    "platform",
    "function",
    "resource",
}

for _, component in ipairs(components) do

includes(component)

end

add_includedirs(".")

target("runtime")
    set_kind("phony")
    add_headerfiles("(**.hpp)")
    add_includedirs(path.join("$(projectdir)", "src", "runtime"), {public = true})

    for _, component in ipairs(components) do
        add_deps(component)
    end
