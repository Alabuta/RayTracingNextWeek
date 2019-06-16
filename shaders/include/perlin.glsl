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

float trilinear_interpolation(const in mat2 c, const in mat2 d, const in vec3 uvw)
{
    vec3 _uvw = vec3(1.f) - uvw;
    vec3 a = dot(_uvw, vec3(1.f) - vec3(0.f, 0.f, 0.f));
}

float perlin_noise(/*const in perlin _perlin, */const in vec3 xyz)
{
    vec3 uvw = fract(xyz);

    ivec3 ijk = ivec3(floor(uvw));

    mat2 c;
    mat2 d;

    c[0].x = _perlin.randoms[_perlin.x[(ijk.x + 0) & 255] ^ _perlin.y[(ijk.y + 0) & 255] ^ _perlin.z[(ijk.z + 0) & 255]];
    c[0].y = _perlin.randoms[_perlin.x[(ijk.x + 1) & 255] ^ _perlin.y[(ijk.y + 0) & 255] ^ _perlin.z[(ijk.z + 0) & 255]];
    c[1].x = _perlin.randoms[_perlin.x[(ijk.x + 0) & 255] ^ _perlin.y[(ijk.y + 1) & 255] ^ _perlin.z[(ijk.z + 0) & 255]];
    c[1].y = _perlin.randoms[_perlin.x[(ijk.x + 1) & 255] ^ _perlin.y[(ijk.y + 1) & 255] ^ _perlin.z[(ijk.z + 0) & 255]];

    d[0].x = _perlin.randoms[_perlin.x[(ijk.x + 0) & 255] ^ _perlin.y[(ijk.y + 0) & 255] ^ _perlin.z[(ijk.z + 1) & 255]];
    d[0].y = _perlin.randoms[_perlin.x[(ijk.x + 1) & 255] ^ _perlin.y[(ijk.y + 0) & 255] ^ _perlin.z[(ijk.z + 1) & 255]];
    d[1].x = _perlin.randoms[_perlin.x[(ijk.x + 0) & 255] ^ _perlin.y[(ijk.y + 1) & 255] ^ _perlin.z[(ijk.z + 1) & 255]];
    d[1].y = _perlin.randoms[_perlin.x[(ijk.x + 1) & 255] ^ _perlin.y[(ijk.y + 1) & 255] ^ _perlin.z[(ijk.z + 1) & 255]];

    return trilinear_interpolation(c, d, uvw);
}

#endif    // PERLIN_H
