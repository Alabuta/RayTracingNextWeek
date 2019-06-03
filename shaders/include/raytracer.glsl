#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "math.glsl"
#include "primitives.glsl"


hit intersect(const in ray _ray, const in sphere _sphere, float time_min, float time_max)
{
    // vec3 center = _sphere.center + (_ray.time) * vec3(0, 0, 1);
    vec3 center = _sphere.center;

    vec3 oc = _ray.origin - center;

    float a = dot(_ray.direction, _ray.direction);
    float b = dot(oc, _ray.direction);
    float c = dot(oc, oc) - _sphere.radius * _sphere.radius;

    float discriminant = b * b - a * c;

    if (discriminant > 0.f) {
        float temp = (-b - sqrt(b * b - a * c)) / a;

        if (temp < time_max && temp > time_min) {
            vec3 position = point_at(_ray, temp);

            return hit(
                position,
                (position - center) / _sphere.radius,
                temp,
                _sphere.material_type,
                _sphere.material_index,
                true
            );
        }

        temp = (-b + sqrt(b * b - a * c)) / a;

        if (temp < time_max && temp > time_min) {
            vec3 position = point_at(_ray, temp);

            return hit(
                position,
                (position - center) / _sphere.radius,
                temp,
                _sphere.material_type,
                _sphere.material_index,
                true
            );
        }
    }

    return hit(oc, oc, 0.f, 0u, 0u, false);
}

hit hit_world(const in uint spheres_number, const in ray _ray)
{
    const float kMAX = 10.0e9f;
    const float kMIN = .008f;

    float min_time = kMAX;

    hit closest_hit = hit(_ray.origin, _ray.origin, 0.f, 0u, 0u, false);

    for (uint sphere_index = 0u; sphere_index < spheres_number; ++sphere_index) {
        hit any_hit = intersect(_ray, spheres[sphere_index], kMIN, min_time);

        if (any_hit.valid) {
            min_time = any_hit.time;
            closest_hit = any_hit;
        }
    }

    return closest_hit;
}

#endif    // RAYTRACER_H
