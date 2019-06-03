#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 4) uniform sampler2D image;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 frag_color;


void main()
{
    frag_color = texture(image, uv);
    frag_color.rgb /= frag_color.a;
}