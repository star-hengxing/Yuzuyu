#version 450

layout (location = 0) in vec2 global_uv;

layout (location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D texture1;

layout(push_constant) uniform constants
{
    vec4 color;
} global_payload;

void main()
{
    if (global_payload.color.rgb == vec3(0, 0, 0))
    {
        out_color = vec4(texture(texture1, global_uv).rgb, global_payload.color.a);
    }
    else
    {
        out_color = global_payload.color;
    }
}
