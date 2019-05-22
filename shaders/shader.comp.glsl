#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : require

layout(local_size_x = 16, local_size_y = 16) in;

#include "binding_constants.glsl"

layout(binding = kOUT_IMAGE_BINDING, rgba32f) uniform image2D image;

// layout(std430, binding = 0) readonly buffer world {
// 	sphere spheres[];
// };

#include "common.glsl"
#include "math.glsl"
#include "primitives.glsl"
#include "raytracer.glsl"

#include "camera.glsl"

//layout(location = kCAMERA_BINDING) uniform camera _camera;

layout(binding = kCAMERA_BINDING, std430) readonly buffer PER_VIEW
{
    camera  _camera;
};


const sphere kSphere = sphere(vec3(0, 0, -1), .5f, 0);


vec3 color(const in ray _ray)
{
    hit _hit = intersect(_ray, kSphere, .0001f, 10.0e9);

    if (_hit.valid)
        return vec3(1, 0, 0);

    vec3 unit_direction = ray_unit_direction(_ray);

    float t = .5f * (unit_direction.y + 1.f);

    return background_color(t);
}

void main()
{
	vec2 uv = vec2(gl_GlobalInvocationID.xy) / imageSize(image).xy;

    vec3 color = color(generate_ray(_camera, uv));

    imageStore(image, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1.f));
}
