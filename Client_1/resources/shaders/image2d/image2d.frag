#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 i_Tex;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D smplr;


void main() {
    outColor = texture(smplr, i_Tex);
}