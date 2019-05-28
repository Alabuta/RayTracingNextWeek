#ifndef RANDOM_H
#define RANDOM_H

#include "math.glsl"


struct random_engine {
    uvec2 seed;
    uint c, counter;
};

uint generate(inout random_engine rng)
{
    const uint BARREL_SHIFT = 25u;
    const uint RSHIFT = 8u;
    const uint LSHIFT = 3u;

	uint tmp = rng.seed.x + rng.seed.y + rng.counter++;

	rng.seed.x = rng.seed.y ^ (rng.seed.y >> RSHIFT);
	rng.seed.y = rng.c + (rng.c << LSHIFT);
	rng.c = ((rng.c << BARREL_SHIFT) | (rng.c >> (32 - BARREL_SHIFT))) + tmp;

	return tmp;
}

random_engine create_random_engine(uvec2 seed)
{
    random_engine rng = random_engine(seed, 0, 1);

	for (uint i = 0u; i < 12u; ++i)
        generate(rng);

    return rng;
}

float generate_real(inout random_engine rng)
{
    const float delim = 1.f / 4294967296.f;

    return generate(rng) * delim;
}

float gradient_noise(vec2 xy)
{
    return fract(52.9829189f * fract(.06711056f * xy.x) + .00583715f * xy.y);
}

//float noise(vec2 uv)
//{
//    return gradient_noise(floor(fmod(uv, 1024)) + seed * time);
//}

vec3 random_on_unit_sphere(inout random_engine rng)
{
    // return unit_vectors[generate(rng) % 6100];

    float phi = generate_real(rng) * kTAU;
    float cos_theta = generate_real(rng) * 2.f - 1.f;

    float theta = acos(cos_theta);

    float sin_theta = sin(theta);

    vec3 vec;

    vec.x = sin_theta * cos(phi);
    vec.y = sin_theta * sin(phi);
    vec.z = cos_theta;

    return vec;
}

vec3 random_in_unit_disk(inout random_engine rng)
{
    float theta = sqrt(generate_real(rng)) * kTAU;

    return vec3(cos(theta), sin(theta), 0.f);
}

#endif    // RANDOM_H
