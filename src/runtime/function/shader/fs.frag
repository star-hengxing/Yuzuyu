#version 450

layout (location = 0) in vec2 global_uv;

layout (location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D texture1;

void main()
{
    out_color = vec4(texture(texture1, global_uv).rgb, 1);
}
