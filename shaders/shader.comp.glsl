#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : require

layout(local_size_x = 16, local_size_y = 16) in;

#include "binding_constants.glsl"

layout(binding = kOUT_IMAGE_BINDING, rgba32f) uniform image2D image;

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
    camera _camera;
};

#include "raytracer.glsl"


const uint kSPHERES_NUMBER = 2u;


vec3 color(uint pixel_index, const in ray _ray)
{
	uint local_random_state = pixel_index;

	vec3 attenuation = vec3(1);

	ray scattered_ray = _ray;
	
    for (uint bounce = 0u; bounce < 10; ++bounce) {
		hit any_hit = hit_world(kSPHERES_NUMBER, _ray);

    	if (any_hit.valid) {
			// return any_hit.normal * .5f + .5f;

			/* vec3 random_direction = vec3(0);//random_in_unit_sphere(local_random_state);
            vec3 target = any_hit.normal + random_direction;

            scattered_ray = ray(any_hit.position, target);

            attenuation *= .5f;
 */
			return random_in_unit_sphere(local_random_state);
		}

		else return background_color(ray_unit_direction(scattered_ray).y * .5f + .5f) * attenuation;
	}

	return vec3(0);

    
}

void main()
{
	ivec2 imageSize = imageSize(image).xy;
	
	vec2 uv = vec2(gl_GlobalInvocationID.xy) / imageSize;
	uint pixel_index = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * uint(imageSize.x);

    vec3 color = color(pixel_index, generate_ray(_camera, uv));

    imageStore(image, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1.f));
}
