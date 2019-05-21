
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

vec3 background_color(const float t)
{
    return mix(vec3(1), vec3(.5, .7, 1), t);
}

hit intersect(const in ray _ray, const in sphere _sphere, float time_min, float time_max)
{
    vec3 oc = _ray.origin - _sphere.center;

    float a = dot(_ray.direction, _ray.direction);
    float b = dot(oc, _ray.direction);
    float c = dot(oc, oc) - _sphere.radius * _sphere.radius;

    float discriminant = b * b - a * c;

    if (discriminant > 0.f) {
        float temp = (-b - sqrt(b * b - a * c)) / a;

        if (temp < time_max && temp > time_min) {
            vec3 position = point_at(_ray, temp);

            return hit(
                position,
                (position - _sphere.center) / _sphere.radius,
                temp,
                _sphere.material_index,
                true
            );
        }

        temp = (-b + sqrt(b * b - a * c)) / a;

        if (temp < time_max && temp > time_min) {
            vec3 position = point_at(_ray, temp);

            return hit(
                position,
                (position - _sphere.center) / _sphere.radius,
                temp,
                _sphere.material_index,
                true
            );
        }
    }

    return hit(oc, oc, 0.f, 0, false);
}

const sphere kSphere = sphere(vec3(0, 0, -1), .5f, 0);

vec3 color(const in ray _ray) {
    hit _hit = intersect(_ray, kSphere, .0001f, 10.0e9);

    if (_hit.valid)
        return vec3(1, 0, 0);

    vec3 unit_direction = ray_unit_direction(_ray);

    float t = .5f * (unit_direction.y + 1.f);

    return background_color(t);
}
