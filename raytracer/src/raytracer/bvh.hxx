#pragma once

#include <cstdint>
#include <optional>

#include "math/math.hxx"
#include "raytracer/primitives.hxx"


namespace BVH
{
    struct AABB final {
        alignas(sizeof(glm::vec4)) glm::vec3 min;
        alignas(sizeof(glm::vec4)) glm::vec3 max;
    };

    [[maybe_unused]]
    static std::optional<BVH::AABB> create_aabb(primitives::sphere const &sphere)
    {
        auto min = sphere.center - sphere.radius;
        auto max = sphere.center + sphere.radius;

        return BVH::AABB{ min, max };
    }

    [[maybe_unused]]
    static BVH::AABB aabbs_union(BVH::AABB const &lhs, BVH::AABB const &rhs)
    {

        auto min = (glm::min)(lhs.min, rhs.min);
        auto max = (glm::max)(lhs.max, rhs.max);

        return BVH::AABB{ min, max };
    }

    struct node final {
        std::int32_t left, right;

        BVH::AABB bounding_box;
    };
}
