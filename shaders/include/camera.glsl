#ifndef CAMERA_H
#define CAMERA_H

#include "math.glsl"
// #include "random.glsl"


struct camera {
    mat4 world;
    mat4 inverted_projection;
};

// float circle_of_confusion(float aperture)
// {
//     ;
// }

ray generate_ray(/* inout random_engine rng,  */const in camera _camera, const in vec2 uv)
{
    /*const float aperture = .1f;
    const float lens_radius = aperture * .5f;
    const float focus_dist = 1.f;

    vec3 random_direction = lens_radius * random_in_unit_disk(rng);
    vec3 offset = vec3(_camera.world * vec4(random_direction, 1.f));*/

    vec2 xy = mat2(_camera.inverted_projection) * (uv * 2.f - 1.f);// *focus_dist;

    vec4 direction = _camera.world * vec4(xy, -1.f, 0.f);
    vec3 origin = vec3(_camera.world[3])/* + offset*/;

    return ray(origin, direction.xyz);
}

#endif    // CAMERA_H
