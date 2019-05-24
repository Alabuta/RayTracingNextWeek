#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : require

layout(local_size_x = 8, local_size_y = 8) in;

const uint SAMPLING_NUMBER = 16u;
const uint BOUNCES_NUMBER = 32u;

#include "constants.glsl"

layout(binding = kOUT_IMAGE_BINDING, rgba32f) uniform image2D image;

//layout(binding = kUNIT_VECTORS_BUFFER_BINDING, rgba32f) uniform image2D unit_vectors;

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


vec3 render(inout random_engine rng, const in camera _camera, const in vec2 uv)
{
    vec3 attenuation = vec3(1.f);
	vec3 energy_absorption = vec3(0.f);

	ray scattered_ray = generate_ray(_camera, uv);

    lambertian material = lambertian(vec3(.5f));

    for (uint bounce = 0u; bounce < BOUNCES_NUMBER; ++bounce) {
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
    uvec2 imageSize = uvec2(imageSize(image));

    if (any(greaterThanEqual(gl_GlobalInvocationID.xy, imageSize)))
        return;

    vec2 xy = vec2(gl_GlobalInvocationID);

    random_engine rng = create_random_engine(gl_GlobalInvocationID.xy);

    camera _camera = camera(origin, lower_left_corner, horizontal, vertical);

    vec3 color = vec3(0);

    for (uint s = 0u; s < SAMPLING_NUMBER; ++s) {
        vec2 _uv = (xy + generate_real(rng)) / imageSize;

        color += render(rng, _camera, _uv);
    }

    color /= float(SAMPLING_NUMBER);

    // color = vec3(generate_real(rng));

    // vec3 color = vec3(imageLoad(unit_vectors, ivec2(gl_GlobalInvocationID.xy)));

    imageStore(image, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1.f));
}
