#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
// #extension GL_ARB_shading_language_include : require
#extension GL_GOOGLE_include_directive : require

#include "math.glsl"

layout (local_size_x = 16, local_size_y = 16) in;

layout (std430, binding = 0) readonly buffer world {
	sphere spheres[];
};

layout (std430, binding = 1) writeonly buffer framebuffer {
	vec4 color[];
};


void main()
{
  	uint pixel_index = gl_LocalInvocationIndex;

	float y = x + 1.f;

	color[pixel_index] = vec4(y);
}
