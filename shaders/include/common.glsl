#ifndef COMMON_H
#define COMMON_H


vec3 background_color(const float t)
{
    return mix(vec3(1), vec3(.5, .7, 1), t);
}

#endif    // COMMON_H
