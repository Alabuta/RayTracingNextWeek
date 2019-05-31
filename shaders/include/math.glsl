#ifndef MATH_H
#define MATH_H

const float kPI = 3.141592654f;
const float kTAU = kPI * 2.f;

struct ray {
    vec3 origin;
    vec3 direction;

    // float time;
};

vec3 ray_unit_direction(const in ray _ray)
{
    return normalize(_ray.direction);
}

vec3 point_at(const in ray _ray, const in float time)
{
    return _ray.origin + _ray.direction * time;
}


#endif    // MATH_H
