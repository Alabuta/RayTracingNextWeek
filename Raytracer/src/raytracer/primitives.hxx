#pragma once

#include <cstdint>
#include <optional>

#include "math/math.hxx"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif


namespace primitives {
struct sphere final {
    alignas(sizeof(glm::vec4)) glm::vec3 center{0};
    float radius{1.f};

    std::uint32_t material_type{0};
    std::uint32_t material_index{0};
};

struct AABB final {
    alignas(sizeof(glm::vec4)) glm::vec3 min;
    alignas(sizeof(glm::vec4)) glm::vec3 max;
};

std::optional<primitives::AABB> create_aabb(primitives::sphere const &sphere)
{
    auto min = sphere.center - sphere.radius;
    auto max = sphere.center + sphere.radius;

    return primitives::AABB{min, max};
}

primitives::AABB aabbs_union(primitives::AABB const &lhs, primitives::AABB const &rhs)
{
    auto min = glm::min(lhs.min, rhs.min);
    auto max = glm::max(lhs.max, rhs.max);

    return primitives::AABB{min, max};
}
}

namespace BVH {
struct node final {
    std::int32_t left, right;

    primitives::AABB bounding_box;
};
}
