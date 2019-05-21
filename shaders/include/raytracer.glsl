#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "math.glsl"
#include "primitives.glsl"


hit intersect(const in ray _ray, const in sphere _sphere, float time_min, float time_max)
{
    vec3 oc = _ray.origin - _sphere.center;

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
                (position - _sphere.center) / _sphere.radius,
                temp,
                _sphere.material_index,
                true
            );
        }

        temp = (-b + sqrt(b * b - a * c)) / a;

        if (temp < time_max && temp > time_min) {
            vec3 position = point_at(_ray, temp);

            return hit(
                position,
                (position - _sphere.center) / _sphere.radius,
                temp,
                _sphere.material_index,
                true
            );
        }
    }

    return hit(oc, oc, 0.f, 0, false);
}

#endif    // RAYTRACER_H
