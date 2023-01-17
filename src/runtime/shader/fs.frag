#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 texCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texture1;

void main()
{
    outColor = texture(texture1, texCoord);
    // outColor = vec4(texCoord, 0.5, 1.0);
}
