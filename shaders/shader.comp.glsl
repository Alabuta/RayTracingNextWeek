#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : require

layout(local_size_x = 16, local_size_y = 16) in;

#include "binding_constants.glsl"

layout(binding = kOUT_IMAGE_BINDING, rgba32f) uniform image2D image;

// layout(std430, binding = 0) readonly buffer world {
// 	sphere spheres[];
// };

#include "common.glsl"
#include "math.glsl"
#include "primitives.glsl"
#include "raytracer.glsl"



void main()
{
	vec3 lower_left_corner = vec3(-1, -1, -1);
	vec3 horizontal = vec3(2, 0, 0);
	vec3 vertical = vec3(0, 2, 0);
	vec3 origin = vec3(0);

	vec2 uv = vec2(gl_GlobalInvocationID.xy) / imageSize(image).xy;

	ray _ray = ray(origin, lower_left_corner + horizontal * uv.x + vertical * uv.y);

    // imageStore(image, uv, vec4(uv / vec2(512, 512), .2f, 1.f));
    imageStore(image, ivec2(gl_GlobalInvocationID.xy), vec4(color(_ray), 1.f));
}
