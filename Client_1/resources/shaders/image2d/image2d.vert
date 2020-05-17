#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 i_PosTex;

layout(location = 0) out vec2 o_Tex;

layout(binding = 0) uniform UBO
{
    vec2 offset;
    mat3 rotation;
} ubo;

void main()
{
    gl_Position = vec4((vec3(i_PosTex.xy, 0) * ubo.rotation).xy + ubo.offset, 0.0, 1.0);
    o_Tex = i_PosTex.zw;
}