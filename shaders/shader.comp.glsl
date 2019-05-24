#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : require

layout(local_size_x = 16, local_size_y = 16) in;

#include "constants.glsl"

layout(location = kFRAME_NUMBER_UNIFORM_LOCATION) uniform float frame_number = 1.f;

layout(binding = kOUT_IMAGE_BINDING, rgba32f) uniform image2D image;
layout(binding = kUNIT_VECTORS_BUFFER_BINDING, rgba32f) uniform image2D unit_vectors;
// };

#include "common.glsl"
#include "math.glsl"
#include "random.glsl"

#include "primitives.glsl"
layout(binding = kPRIMITIVES_BINDING, std430) readonly buffer world {
	sphere spheres[];
};

#include "camera.glsl"
layout(binding = kCAMERA_BINDING, std430) readonly buffer CAMERA
{
    vec3 origin;

    vec3 lower_left_corner;

    vec3 horizontal;
    vec3 vertical;
};

#include "raytracer.glsl"
#include "material.glsl"


vec3 render(inout sfc32 rng, const in camera _camera, const in vec2 uv)
{
    const uint bounces_number = 64u;

    vec3 attenuation = vec3(1.f);
	vec3 energy_absorption = vec3(0.f);

	ray scattered_ray = generate_ray(_camera, uv);

    lambertian material = lambertian(vec3(.5f));

    for (uint bounce = 0u; bounce < bounces_number; ++bounce) {
		hit closest_hit = hit_world(kSPHERES_NUMBER, scattered_ray);

    	if (closest_hit.valid) {
            surface_response response = apply_material(rng, closest_hit, material);

            if (response.valid) {
                scattered_ray = response._ray;
                energy_absorption = response.attenuation;

                attenuation *= energy_absorption;
            }

            else return vec3(0);
		}

		else return background_color(ray_unit_direction(scattered_ray).y * .5f + .5f) * attenuation;
	}

	return vec3(0);
}

void main()
{
    const uint sampling_number = 16u;

    uvec2 imageSize = uvec2(imageSize(image));

    vec2 xy = vec2(gl_GlobalInvocationID);
	vec2 uv = xy / imageSize;

    // random_engine rng = random_engine(uv, frame_number, 0u);
    sfc32 rng;
    seed_fast(rng, gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

    camera _camera = camera(origin, lower_left_corner, horizontal, vertical);

    vec3 color = vec3(0);

    for (uint s = 0u; s < sampling_number; ++s) {
        vec2 _uv = (xy + random(rng)) / imageSize;

        color += render(rng, _camera, _uv);
    }

    color /= float(sampling_number);

    // color = vec3(generate(rng));

    // vec3 color = vec3(imageLoad(unit_vectors, ivec2(gl_GlobalInvocationID.xy)));

    imageStore(image, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1.f));
}
