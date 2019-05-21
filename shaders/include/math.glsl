
struct sphere {
    vec3 position;
    float radius;
};

struct hit {
    vec3 position;
    vec3 normal;

    float time;

    uint material_index;

    bool valid;
};

struct ray {
    vec3 origin;
    vec3 direction;
};

vec3 ray_unit_direction(const in ray _ray)
{
    return normalize(_ray.direction);
}

vec3 point_at(const in ray _ray, const in float time)
{
    return _ray.origin + _ray.direction * time;
}
