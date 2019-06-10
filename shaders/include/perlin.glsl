#ifndef PERLIN_H
#define PERLIN_H

#include "math.glsl"
#include "random.glsl"


struct perlin {
    float randoms[256];
    uvec3 perm;
};

perlin create_perlin(const in vec3 xyz)
{
    vec3 uvw = fract(xyz);

    ivec3 ijk = ivec3(xyz * 4.f) & 255;
}

perlin perlin_noise(inout random_engine rng)
{
    ;
}

#endif    // PERLIN_H
