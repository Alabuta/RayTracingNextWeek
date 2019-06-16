#ifndef TEXTURE_H
#define TEXTURE_H

#include "perlin.glsl"

struct constant_texture {
    vec3 color;
};

struct checker_texture {
    vec3 even;
    vec3 odd;
};

struct noise_texture {
    vec3 color;
};

vec3 sample_texture(const in constant_texture _texture, const in vec3 stp)
{
    return _texture.color;
}

vec3 sample_texture(const in checker_texture _texture, const in vec3 stp)
{
    vec3 signes = sign(sin(stp * 10.f));
    float _sign = signes.x * signes.y * signes.z * .5f + .5f;

    return _texture.even * _sign + _texture.odd * (1.f - _sign);
}

vec3 sample_texture(const in noise_texture _texture, const in vec3 stp)
{
    return _texture.color * perlin_noise(stp);
}


#endif    // TEXTURE_H
