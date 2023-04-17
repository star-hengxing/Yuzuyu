local components =
{
    "input",
    "Multimedia",
    "rhi",
    "script",
    "shader",
    "window",
    "render",
}

for _, component in ipairs(components) do

includes(component)

end

add_includedirs(".")

target("function")
    set_kind("phony")
    add_headerfiles("(**.hpp)")

    for _, component in ipairs(components) do
        add_deps(component)
    end
