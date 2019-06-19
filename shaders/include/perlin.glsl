#ifndef PERLIN_H
#define PERLIN_H

#include "math.glsl"
#include "random.glsl"


struct perlin {
    uint x[256];
    uint y[256];
    uint z[256];
    vec3 randoms[256];
};

layout(binding = kPERLIN_NOISE_BINDING, std430) readonly buffer PERLIN
{
    perlin _perlin;
};

float trilinear_interpolation(const in vec3 c[2][2][2], const in vec3 uvw)
{
    vec3 uvw2 = uvw * uvw * (3.f - 2.f * uvw);

    vec3 _uvw = vec3(1.f) - uvw2;

    float color = 0.f;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                vec3 weight = vec3(uvw.x - i, uvw.y - j, uvw.z - k);

                color += (i * uvw.x + (1 - i) * _uvw.x) * (j * uvw.y + (1 - j) * _uvw.y) * (k * uvw.z + (1 - k) * _uvw.z) * dot(c[i][j][k], weight);
            }
        }
    }

    return color;
}

float perlin_noise(/*const in perlin _perlin, */const in vec3 xyz)
{
    ivec3 ijk = ivec3(floor(xyz));

    vec3 color[2][2][2];

    for (int a = 0; a < 2; ++a) {
        for (int b = 0; b < 2; ++b) {
            for (int c = 0; c < 2; ++c) {
                color[a][b][c] = _perlin.randoms[_perlin.x[(ijk.x + a) & 255] ^ _perlin.y[(ijk.y + b) & 255] ^ _perlin.z[(ijk.z + c) & 255]];
            }
        }
    }

    /*c[0][0][0] = _perlin.randoms[_perlin.x[(ijk.x + 0) & 255] ^ _perlin.y[(ijk.y + 0) & 255] ^ _perlin.z[(ijk.z + 0) & 255]];
    c[0][0][1] = _perlin.randoms[_perlin.x[(ijk.x + 1) & 255] ^ _perlin.y[(ijk.y + 0) & 255] ^ _perlin.z[(ijk.z + 0) & 255]];
    c[0][1][0] = _perlin.randoms[_perlin.x[(ijk.x + 0) & 255] ^ _perlin.y[(ijk.y + 1) & 255] ^ _perlin.z[(ijk.z + 0) & 255]];
    c[0][1][1] = _perlin.randoms[_perlin.x[(ijk.x + 1) & 255] ^ _perlin.y[(ijk.y + 1) & 255] ^ _perlin.z[(ijk.z + 0) & 255]];

    c[1][0][0] = _perlin.randoms[_perlin.x[(ijk.x + 0) & 255] ^ _perlin.y[(ijk.y + 0) & 255] ^ _perlin.z[(ijk.z + 1) & 255]];
    c[1][0][1] = _perlin.randoms[_perlin.x[(ijk.x + 1) & 255] ^ _perlin.y[(ijk.y + 0) & 255] ^ _perlin.z[(ijk.z + 1) & 255]];
    c[1][1][0] = _perlin.randoms[_perlin.x[(ijk.x + 0) & 255] ^ _perlin.y[(ijk.y + 1) & 255] ^ _perlin.z[(ijk.z + 1) & 255]];
    c[1][1][1] = _perlin.randoms[_perlin.x[(ijk.x + 1) & 255] ^ _perlin.y[(ijk.y + 1) & 255] ^ _perlin.z[(ijk.z + 1) & 255]];*/

    return trilinear_interpolation(color, fract(xyz));
}

#endif    // PERLIN_H
