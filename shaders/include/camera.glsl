#ifndef CAMERA_H
#define CAMERA_H

#include "math.glsl"


struct camera {
    vec3 origin;

    vec3 lower_left_corner;

    vec3 horizontal;
    vec3 vertical;
};

ray generate_ray(const in camera _camera, const in vec2 uv)
{
    vec3 direction = _camera.lower_left_corner + _camera.horizontal * uv.x + _camera.vertical * uv.y;

    ray _ray = ray(
        _camera.origin,
        direction
    );

    return _ray;
}

#endif    // CAMERA_H
