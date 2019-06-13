#ifndef PERLIN_H
#define PERLIN_H

#include "math.glsl"
#include "random.glsl"


struct perlin {
    uint x[256];
    uint y[256];
    uint z[256];
    float randoms[256];
};

layout(binding = kPERLIN_NOISE_BINDING, std430) readonly buffer PERLIN
{
    perlin _perlin;
};

float perlin_noise(const in perlin _perlin, const in vec3 xyz)
{
    //vec3 uvw = fract(xyz);

    ivec3 ijk = ivec3(xyz * 4.f) & 255;

    return _perlin.randoms[_perlin.x[ijk.x] ^ _perlin.y[ijk.y] ^ _perlin.z[ijk.z]];
}

#endif    // PERLIN_H
