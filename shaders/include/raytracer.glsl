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
                _sphere,
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
                _sphere,
                true
            );
        }
    }

    return hit(oc, oc, 0.f, null_sphere, false);
}

bool intersect(const in ray _ray, const in AABB bounding_box, float t_min, float t_max)
{
    vec3 delimer = vec3(1.f) / _ray.direction;

    vec3 t0 = (bounding_box.min - _ray.origin) * delimer;
    vec3 t1 = (bounding_box.max - _ray.origin) * delimer;

    vec3 swap = step(0.f, delimer);

    vec3 temp = t0;

    t0 = mix(t1, t0, swap);
    t1 = mix(temp, t1, swap);

    vec3 _min = max(t0, vec3(t_min));
    vec3 _max = min(t1, vec3(t_max));

    return !any(lessThanEqual(_max, _min));
}

int intersect(const in ray _ray, const in BVH_node node, float t_min, float t_max)
{
#if 0
    if (!intersect(_ray, node.bounding_box, t_min, t_max))
        return -1;

    int intersect_left = intersect(_ray, BVH_nodes[node.left], t_min, t_max);
    int intersect_right = intersect(_ray, BVH_nodes[node.right], t_min, t_max);

    if (intersect_left != -1 && intersect_right != -1) {
        // if (intersect_left)
    }
#endif

    return -1;
}

hit hit_world(const in uint spheres_number, const in ray _ray)
{
    const float kMAX = 10.0e9f;
    const float kMIN = .008f;

    float min_time = kMAX;

    hit closest_hit = hit(_ray.origin, _ray.origin, 0.f, null_sphere, false);

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
