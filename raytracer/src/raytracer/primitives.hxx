#pragma once

#include <cstdint>
#include <optional>

#include "math/math.hxx"


namespace primitives {
struct sphere final {
    alignas(sizeof(glm::vec4)) glm::vec3 center{0};
    float radius{1.f};

    std::uint32_t material_type{0};
    std::uint32_t material_index{0};
};
}
