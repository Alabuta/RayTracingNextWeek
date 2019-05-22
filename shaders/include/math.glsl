#ifndef MATH_H
#define MATH_H

#include "constants.glsl"


struct ray {
    vec3 origin;
    vec3 direction;
};

vec3 ray_unit_direction(const in ray _ray)
{
    return normalize(_ray.direction);
}

vec3 point_at(const in ray _ray, const in float time)
{
    return _ray.origin + _ray.direction * time;
}

uint rand_xorshift(inout uint random_state)
{
    // Xorshift algorithm from George Marsaglia's paper
    random_state ^= (random_state << 13);
    random_state ^= (random_state >> 17);
    random_state ^= (random_state << 5);

    return random_state;
}

float generate_float(inout uint random_state)
{
    return float(rand_xorshift(random_state)) * (1.f / 4294967296.f);
    // return float(rand_xorshift(random_state) >> 9) / float(1 << 23);
}

vec3 random_in_unit_sphere(inout uint random_state)
{
    vec3 unit_vector;

    do {
        unit_vector = vec3(
            generate_float(random_state) * 2.f - 1.f,
            generate_float(random_state) * 2.f - 1.f,
            generate_float(random_state) * 2.f - 1.f
        );
    } while (/*length(unit_vector) > 1.f*/false);

    return unit_vector;
}

vec3 random_in_unit_sphere2(inout uint random_state)
{
    float random = generate_float(random_state);

    uint index = uint(random * float(kUNIT_VECTORS_BUFFER_SIZE));

    return unit_vectors[min(index, kUNIT_VECTORS_BUFFER_SIZE - 1)];
}


#endif    // MATH_H
