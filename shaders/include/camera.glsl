#ifndef CAMERA_H
#define CAMERA_H

#include "math.glsl"
#include "random.glsl"


struct camera {
    mat4 world;
    mat4 inverted_projection;
};

ray generate_ray(inout random_engine rng, const in camera _camera, const in vec2 uv)
{
    vec2 xy = mat2(_camera.inverted_projection) * (uv * 2.f - 1.f);

    vec3 direction = normalize(vec3(_camera.world * vec4(xy, -1.f, 0.f)));
    vec3 origin = vec3(_camera.world[3]);

    const float aperture = .2f;
    const float lens_radius = aperture * .5f;
    const float focus_dist = 5.f;// distance(origin, vec3(0.f, .5f, 0.f));

    vec3 random_direction = lens_radius * random_in_unit_disk(rng);

    vec3 offset = vec3(_camera.world * vec4(random_direction, 0.f));

    vec3 bended_direction = normalize(direction * focus_dist - offset);

    return ray(origin + offset, bended_direction);
}

#endif    // CAMERA_H
