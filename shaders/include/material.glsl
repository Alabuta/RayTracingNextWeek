#ifndef MATERIAL_H
#define MATERIAL_H

#include "math.glsl"
#include "random.glsl"
#include "primitives.glsl"
#include "texture.glsl"

layout(binding = 12) uniform sampler2D texture_image;

const uint LAMBERTIAN_TYPE = 0u;
const uint METAL_TYPE = 1u;
const uint DIELECTRIC_TYPE = 2u;
const uint EMISSIVE_TYPE = 3u;

struct lambertian {
    vec3 albedo;
};

struct metal {
    vec3 albedo;
    float roughness;
};

struct dielectric {
    vec3 albedo;
    float refraction_index;
};

struct emissive {
    vec3 color;
    float distance;
};

struct surface_response {
    ray _ray;
    vec3 attenuation;
    bool valid;
};

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

layout(binding = kEMISSIVE_BUFFER_BINDING, std430) readonly buffer EMISSIVE_BUFFER_BINDING
{
    emissive emissives[];
};

const checker_texture _texture = checker_texture(vec3(1.f), vec3(.2f, .3f, .1f));
//const noise_texture _texture = noise_texture(vec3(1.f), 16.f);

float schlick_reflection_probability(float refraction_index, float cosine_theta)
{
    float reflection_coefficient = pow((1.f - refraction_index) / (1.f + refraction_index), 2.f);

    return reflection_coefficient + (1.f - reflection_coefficient) * pow(1.f - cosine_theta, 5);
}

surface_response apply_material(inout random_engine rng, const in hit _hit, const in ray _ray, const in lambertian material)
{
    vec3 random_direction = random_on_unit_sphere(rng);
    vec3 direction = _hit.normal + random_direction;

    // ray scattered_ray = ray(_hit.position, direction, _ray.time);
    ray scattered_ray = ray(_hit.position, direction);
    //vec3 attenuation = material.albedo;
    vec3 attenuation = sample_texture(_texture, _hit.position);

    /*vec2 uv = get_uv(_hit.primitive, normalize(_hit.position - _hit.primitive.center));
    vec3 attenuation = texture(texture_image, uv).rgb;*/

    return surface_response(scattered_ray, attenuation, true);
}

surface_response apply_material(inout random_engine rng, const in hit _hit, const in ray _ray, const in metal material)
{
    vec3 reflected = reflect(ray_unit_direction(_ray), _hit.normal);

    vec3 random_direction = random_on_unit_sphere(rng);
    vec3 direction = normalize(reflected + random_direction * material.roughness);

    // ray scattered_ray = ray(_hit.position, direction, _ray.time);
    ray scattered_ray = ray(_hit.position, direction);

    vec3 attenuation = material.albedo;

    return surface_response(scattered_ray, attenuation, dot(direction, _hit.normal) > 0.f);
}

surface_response apply_material(inout random_engine rng, const in hit _hit, const in ray _ray, const in dielectric material)
{
    vec3 outward_normal = -_hit.normal;
    float refraction_index = material.refraction_index;
    float cosine_theta = dot(ray_unit_direction(_ray), _hit.normal);

    if (cosine_theta <= 0.f) {
        outward_normal *= -1.f;
        refraction_index = 1.f / refraction_index;
        cosine_theta *= -1.f;
    }

    vec3 attenuation = material.albedo;
    vec3 refracted = refract(ray_unit_direction(_ray), outward_normal, refraction_index);

    float reflection_probability = 1.f;

    if (length(refracted) > 0.f)
        reflection_probability = schlick_reflection_probability(refraction_index, cosine_theta);

    ray scattered_ray;

    if (generate_real(rng) < reflection_probability) {
        vec3 reflected = reflect(ray_unit_direction(_ray), _hit.normal);
        // scattered_ray = ray(_hit.position, reflected, _ray.time);
        scattered_ray = ray(_hit.position, reflected);
    }

    // else scattered_ray = ray(_hit.position, refracted, _ray.time);
    else scattered_ray = ray(_hit.position, refracted);

    return surface_response(scattered_ray, attenuation, true);
}

surface_response apply_material(inout random_engine rng, const in hit _hit, const in ray _ray, const in emissive material)
{
    //return surface_response(ray(vec3(0), vec3(0)), material.color, true);
    vec3 random_direction = random_on_unit_sphere(rng);
    vec3 direction = _hit.normal + random_direction;

    // ray scattered_ray = ray(_hit.position, direction, _ray.time);
    ray scattered_ray = ray(_hit.position, direction);
    //vec3 attenuation = material.albedo;
    vec3 attenuation = sample_texture(_texture, _hit.position);

    /*vec2 uv = get_uv(_hit.primitive, normalize(_hit.position - _hit.primitive.center));
    vec3 attenuation = texture(texture_image, uv).rgb;*/

    return surface_response(scattered_ray, attenuation, true);
}

surface_response apply_material(inout random_engine rng, const in hit _hit, const in ray _ray)
{
    surface_response response;

    switch (_hit.primitive.material_type) {
        case LAMBERTIAN_TYPE:
            response = apply_material(rng, _hit, _ray, lambertians[_hit.primitive.material_index]);
            break;

        case METAL_TYPE:
            response = apply_material(rng, _hit, _ray, metals[_hit.primitive.material_index]);
            break;

        case DIELECTRIC_TYPE:
            response = apply_material(rng, _hit, _ray, dielectrics[_hit.primitive.material_index]);
            break;

        case EMISSIVE_TYPE:
            response = apply_material(rng, _hit, _ray, emissives[_hit.primitive.material_index]);
            break;

        default:
            response.valid = false;
            break;
    }

    return response;
}


#endif    // MATERIAL_H
