#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : require

#include "constants.glsl"

layout(local_size_x = 64, local_size_y = 1) in;

layout(binding = kOUT_IMAGE_BINDING, rg32f) uniform image1D image;

#include "random.glsl"


void main()
{
    uvec2 imageSize = uvec2(imageSize(image));

    if (any(greaterThanEqual(gl_GlobalInvocationID.xy, imageSize)))
        return;

    vec2 xy = vec2(gl_GlobalInvocationID);

    random_engine rng = create_random_engine(gl_GlobalInvocationID.xy);

    for (uint s = 0u; s < SAMPLING_NUMBER; ++s) {
        vec2 offset = vec2(generate_real(rng), generate_real(rng)) * 2.f - 1.f;
        vec2 uv = (xy + offset) / imageSize;

        uint index = gl_LocalInvocationIndex * SAMPLING_NUMBER + s;

        //imageStore(image, 0, uv);
    }

    memoryBarrierShared();
    barrier();
}
