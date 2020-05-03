#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 i_pos;
layout(location = 1) in vec3 i_color;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(i_pos, 0.0, 1.0);
    fragColor = i_color;
}