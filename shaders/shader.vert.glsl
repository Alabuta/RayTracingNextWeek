#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
// #extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require


layout(location = 0) out vec2 uv;


void main()
{
    uv = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);

    gl_Position = vec4(uv * 2.f - 1.f, 0.f, 1.f);
}