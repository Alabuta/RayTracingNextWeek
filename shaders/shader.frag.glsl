#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
// #extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require

layout(binding = 4) uniform sampler2D image;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 frag_color;

void main()
{
    frag_color = texture(image, uv);
}