#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UBO
{
    float color;
} ubo;

vec3 multiplier(float color)
{
    float x, y, z;

    if(color < 1.0)
        return vec3(1.0 - color, color, 0.0);
    if(color < 2.0)
        return vec3(0.0, 2.0 - color, color - 1.0);
    if(color < 3.0)
        return vec3(color - 2.0, 0.0, 3.0 - color);
}

void main() {
    outColor = vec4(fragColor * multiplier(ubo.color), 1.0);
}