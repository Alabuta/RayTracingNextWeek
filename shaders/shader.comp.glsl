#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : require

#include "constants.glsl"

layout(local_size_x = kGROUP_SIZE.x, local_size_y = kGROUP_SIZE.y, local_size_z = kGROUP_SIZE.z) in;

#if USE_SCHEDULER
    shared uint invocation_index;
    shared uint local_data[kGROUP_FLAT_SIZE];
#endif

layout(binding = kOUT_IMAGE_BINDING, rgba32f) /* writeonly */ restrict uniform image2D image;

#include "common.glsl"
#include "math.glsl"
#include "random.glsl"
#include "perlin.glsl"
#include "primitives.glsl"

layout(binding = kBVH_TREE_BINDING, std430) readonly buffer BVH_TREE {
    BVH_node BVH_nodes[];
};


layout(binding = kPRIMITIVES_BINDING, std430) readonly buffer WORLD {
	sphere spheres[];
};

#include "camera.glsl"

layout(binding = kCAMERA_BINDING, std430) readonly buffer CAMERA
{
    camera _camera;
};

#include "raytracer.glsl"
#include "material.glsl"

layout(binding = kLAMBERTIAN_BUFFER_BINDING, std430) readonly buffer LAMBERTIAN_MATERIALS
{
    lambertian lambertians[];
};

layout(binding = kMETAL_BUFFER_BINDING, std430) readonly buffer METAL_MATERIALS
{
    metal metals[];
};

layout(binding = kDIELECTRIC_BUFFER_BINDING, std430) readonly buffer DIELECTRIC_MATERIALS
{
    dielectric dielectrics[];
};


vec3 render(inout random_engine rng, const in camera _camera, const in vec2 uv)
{
    vec3 attenuation = vec3(1.f);
	vec3 energy_absorption = vec3(0.f);

	ray scattered_ray = generate_ray(rng, _camera, uv);

    for (uint bounce = 0u; bounce < BOUNCES_NUMBER; ++bounce) {
		hit closest_hit = hit_world(kSPHERES_NUMBER, scattered_ray);

    	if (closest_hit.valid) {
            surface_response response;

            switch (closest_hit.material_type) {
                case LAMBERTIAN_TYPE:
                    response = apply_material(rng, closest_hit, scattered_ray, lambertians[closest_hit.material_index]);
                    break;

                case METAL_TYPE:
                    response = apply_material(rng, closest_hit, scattered_ray, metals[closest_hit.material_index]);
                    break;

                case DIELECTRIC_TYPE:
                    response = apply_material(rng, closest_hit, scattered_ray, dielectrics[closest_hit.material_index]);
                    break;

                default:
                    response.valid = false;
                    break;
            }

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

    random_engine rng = create_random_engine(gl_GlobalInvocationID.xy);

    vec3 color = vec3(0);

    vec2 xy = vec2(gl_GlobalInvocationID);

    for (uint s = 0u; s < SAMPLING_NUMBER; ++s) {
        vec2 offset = generate_vec2(rng) * 2.f - 1.f;
        vec2 uv = (xy + offset) / imageSize;

        color += render(rng, _camera, uv);
    }

    color /= float(SAMPLING_NUMBER);

    imageStore(image, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1.f));
}
