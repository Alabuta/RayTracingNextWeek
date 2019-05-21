#ifndef PRIMITIVES_H
#define PRIMITIVES_H


struct sphere {
    vec3 center;
    float radius;

    uint material_index;
};

struct hit {
    vec3 position;
    vec3 normal;

    float time;

    uint material_index;

    bool valid;
};

#endif    // PRIMITIVES_H
