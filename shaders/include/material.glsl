#ifndef MATERIAL_H
#define MATERIAL_H

#include "math.glsl"
#include "random.glsl"
#include "primitives.glsl"


struct lambertian {
    vec3 albedo;
};

struct surface_response {
    ray _ray;
    vec3 attenuation;
    bool valid;
};


surface_response apply_material(inout random_engine rng, const in hit _hit, const in lambertian material)
{
    // vec3 random_direction = vec3(imageLoad(unit_vectors, ivec2(gl_GlobalInvocationID.xy)));
    vec3 random_direction = random_on_unit_sphere(rng);
    vec3 direction = _hit.normal + random_direction;

    ray scattered_ray = ray(_hit.position, direction);
    vec3 attenuation = material.albedo;

    return surface_response(scattered_ray, attenuation, true);
}


#endif    // MATERIAL_H
