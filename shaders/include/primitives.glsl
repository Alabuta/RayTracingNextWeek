#ifndef PRIMITIVES_H
#define PRIMITIVES_H


struct sphere {
    vec3 center;
    float radius;

    uint material_type;
    uint material_index;
};

struct hit {
    vec3 position;
    vec3 normal;

    float time;

    uint material_type;
    uint material_index;

    bool valid;
};

struct AABB {
    vec3 min;
    vec3 max;
};

struct BVH_node {
    int left, right;

    AABB bounding_box;
};

struct node_hit {
    int node;
    float t;
};

#endif    // PRIMITIVES_H
