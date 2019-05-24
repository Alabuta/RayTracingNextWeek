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

vec3 random_in_unit_sphere(inout random_engine rng)
{
    float phi = generate_real(rng) * kTAU;
    float cos_theta = generate_real(rng) * 2.f - 1.f;

    float theta = acos(cos_theta);

    float sin_theta = sin(theta);

    vec3 vector;

    vector.x = sin_theta * cos(phi);
    vector.y = sin_theta * sin(phi);
    vector.z = cos_theta;

    return vector;
}

#endif    // RANDOM_H
