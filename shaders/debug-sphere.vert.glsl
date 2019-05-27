#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : require

#include "constants.glsl"
#include "camera.glsl"

layout(location = VERTEX_SEMANTIC_POSITION) in vec3 inVertex;


layout(binding = kCAMERA_BINDING, std430) readonly buffer CAMERA
{
    camera _camera;
};

void main()
{
    gl_Position = inverse(_camera.inverted_projection) * inverse(_camera.world) * vec4(inVertex, 1);
}