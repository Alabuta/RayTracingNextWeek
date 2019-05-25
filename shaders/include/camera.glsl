#ifndef CAMERA_H
#define CAMERA_H

#include "math.glsl"


struct camera {
    mat4 world;
    mat2 projection;
};

ray generate_ray(const in camera _camera, const in vec2 uv)
{
    vec2 xy = _camera.projection * (uv * 2.f - 1.f);

    vec4 direction = _camera.world * vec4(xy, -1, 0);
    vec3 origin = vec3(_camera.world[3]);

    return ray(origin, direction.xyz);
}

#endif    // CAMERA_H
