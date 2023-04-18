#version 450

layout (location = 0) in vec2 global_uv;

layout (location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D texture1;

layout(push_constant) uniform constants
{
    vec4 color;
    int is_font;
    int is_texture;
} global_payload;

void main()
{
    if (global_payload.is_font == 1)
    {
        const vec4 color = texture(texture1, global_uv);
        out_color = (color.a == 0) ? color : global_payload.color;
    }
    else if (global_payload.is_texture == 1)
    {
        out_color = vec4(texture(texture1, global_uv).rgb, global_payload.color.a);
    }
    else
    {
        out_color = global_payload.color;
    }
}
