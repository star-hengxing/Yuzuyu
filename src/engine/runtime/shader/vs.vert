#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 texCoord;

layout(set = 0, binding = 0) uniform transform_buffer
{
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

void main()
{
    mat4 mvp = ubo.projection * ubo.view * ubo.model;
    gl_Position = mvp * vec4(vPosition, 1.0f);
    fragColor = vNormal;
    texCoord = vTexCoord;
}
