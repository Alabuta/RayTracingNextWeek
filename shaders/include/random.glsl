#ifndef RANDOM_H
#define RANDOM_H

#include "math.glsl"


uint wang_hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);

    return seed;
}


struct sfc32 {
    uint a, b, c, counter;
};

uint raw32(inout sfc32 rng)
{
    const uint BARREL_SHIFT = 25u;
    const uint RSHIFT = 8u;
    const uint LSHIFT = 3u;

	uint tmp = rng.a + rng.b + rng.counter++;

	rng.a = rng.b ^ (rng.b >> RSHIFT);
	rng.b = rng.c + (rng.c << LSHIFT);
	rng.c = ((rng.c << BARREL_SHIFT) | (rng.c >> (32 - BARREL_SHIFT))) + tmp;

	return tmp;
}

void seed_fast(inout sfc32 rng, uint a, uint b)
{
    rng.a = a;
	rng.b = b;
	rng.c = 0;
	rng.counter = 1;

	for (uint i = 0u; i < 8u; ++i)
        raw32(rng);
}

void seed(inout sfc32 rng, uint a, uint b)
{
    rng.a = a;
	rng.b = b;
	rng.c = 0;
	rng.counter = 1;

	for (uint i = 0u; i < 12u; ++i)
        raw32(rng);
}

float random(inout sfc32 rng)
{
    const float delim = 1.f / 4294967296.f;

    return raw32(rng) * delim;
}

vec3 random_in_unit_sphere(inout sfc32 rng)
{
    float phi = random(rng) * kTAU;
    float cos_theta = random(rng) * 2.f - 1.f;

    float theta = acos(cos_theta);

    float sin_theta = sin(theta);

    vec3 vector;

    vector.x = sin_theta * cos(phi);
    vector.y = sin_theta * sin(phi);
    vector.z = cos_theta;

    return vector;
}


struct random_engine {
    vec2 state;
    float time;
    uint seed;
};

float generate_static(const in random_engine rng)
{
    return fract(sin(dot(rng.state, vec2(12.9898f, 78.233f))) * 43758.5453123f);
}

float generate(inout random_engine rng)
{
    const float delim = 1.f / 4294967296.f;
    
    uvec2 uv = floatBitsToUint(rng.state);

    uint _u = wang_hash(uv.x);
    uint _v = wang_hash(uv.y);

    uint _t = wang_hash(floatBitsToUint(rng.time));

    rng.seed = wang_hash(_u ^ _v ^ _t);    

    return float(rng.seed) * delim;
}

vec3 random_in_unit_sphere(inout random_engine rng)
{
    float phi = generate(rng) * kTAU;
    float cos_theta = generate(rng) * 2.f - 1.f;

    float theta = acos(cos_theta);

    float sin_theta = sin(theta);

    vec3 vector;

    vector.x = sin_theta * cos(phi);
    vector.y = sin_theta * sin(phi);
    vector.z = cos_theta;

    return vector;
}





float random(const in vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898f, 78.233f))) * 43758.5453123f);
}

float random(const in vec2 st, float t)
{
    uvec2 uv = floatBitsToUint(st);

    uint _u = wang_hash(uv.x);
    uint _v = wang_hash(uv.y);

    uint _t = wang_hash(floatBitsToUint(t));

    return float(wang_hash(_u ^ _v ^ _t)) / 4294967296.f;
    
    // return fract(sin(dot(st.xy, vec2(12.9898f, 78.233f))) * t);
}

vec3 random_in_unit_sphere(const in vec2 st, float t)
{
    float phi = random(st, t) * kTAU;
    float cos_theta = random(st, t) * 2.f - 1.f;

    float theta = acos(cos_theta);

    float sin_theta = sin(theta);

    vec3 vector;

    vector.x = sin_theta * cos(phi);
    vector.y = sin_theta * sin(phi);
    vector.z = cos_theta;

    return vector;
}

vec3 random_in_unit_sphere(const in vec2 st)
{
    float phi = random(st) * kTAU;
    float cos_theta = random(st) * 2.f - 1.f;

    float theta = acos(cos_theta);

    float sin_theta = sin(theta);

    vec3 vector;

    vector.x = sin_theta * cos(phi);
    vector.y = sin_theta * sin(phi);
    vector.z = cos_theta;

    return vector;
}

#endif    // RANDOM_H
