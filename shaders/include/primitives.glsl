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

    sphere primitive;

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

vec2 get_uv(const in sphere _sphere, const in vec3 p)
{
    float phi = atan(p.z, p.x);
    float theta = asin(p.y);

    vec2 uv = vec2(.5f - phi / kTAU, theta / kPI + .5f);

    return uv;
}

const sphere null_sphere = sphere(vec3(0.f), 0.f, 0u, 0u);


#endif    // PRIMITIVES_H
