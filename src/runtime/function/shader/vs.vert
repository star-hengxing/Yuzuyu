#version 450

layout (location = 0) out vec2 global_uv;

void main()
{
    global_uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(global_uv * 2 - 1, 0, 1);
}
