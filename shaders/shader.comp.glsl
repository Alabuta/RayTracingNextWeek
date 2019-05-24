#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : require

layout(local_size_x = 16, local_size_y = 16) in;

#include "constants.glsl"

layout(binding = kOUT_IMAGE_BINDING, rgba32f) uniform image2D image;
layout(binding = kUNIT_VECTORS_BUFFER_BINDING, rgba32f) uniform image2D unit_vectors;

// layout(binding = kUNIT_VECTORS_BUFFER_BINDING, std430) readonly buffer UNIT_VECTORS
// {
//     vec3 unit_vectors[];
// };

#include "common.glsl"
#include "math.glsl"

#include "primitives.glsl"
layout(binding = kPRIMITIVES_BINDING, std430) readonly buffer world {
	sphere spheres[];
};

#include "camera.glsl"
//layout(location = kCAMERA_BINDING) uniform camera _camera;
layout(binding = kCAMERA_BINDING, std430) readonly buffer CAMERA
{
    vec3 origin;

    vec3 lower_left_corner;

    vec3 horizontal;
    vec3 vertical;
};

#include "raytracer.glsl"


vec3 color(uint pixel_index, const in ray _ray)
{
    uint local_random_state = pixel_index;

    float attenuation = 1.f;
	float energy_absorbtion = .5f;

	ray scattered_ray = ray(_ray.origin, _ray.direction);
    // ray scattered_ray = _ray;

    for (uint bounce = 0u; bounce < 32u; ++bounce) {
		hit any_hit = hit_world(kSPHERES_NUMBER, scattered_ray);

    	if (any_hit.valid) {
            vec3 random_direction = random_in_unit_sphere2(local_random_state);
            vec3 direction = any_hit.normal + random_direction;

            scattered_ray = ray(any_hit.position, direction);

            attenuation *= energy_absorbtion;
		}

		else return background_color(ray_unit_direction(scattered_ray).y * .5f + .5f) * attenuation;
	}

	return vec3(0);
}

void main()
{
    camera _camera = camera(origin, lower_left_corner, horizontal, vertical);

    uvec2 imageSize = uvec2(imageSize(image));
	
	vec2 uv = vec2(gl_GlobalInvocationID.xy) / imageSize;
	uint pixel_index = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * imageSize.x;

    // vec3 color = vec3(imageLoad(unit_vectors, ivec2(gl_GlobalInvocationID.xy)));

    imageStore(image, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1.f));
}
