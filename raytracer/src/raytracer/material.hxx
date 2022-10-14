#pragma once

#include <cstdint>

#include "math/math.hxx"


namespace material
{
    struct lambertian final {
        alignas(sizeof(glm::vec4)) glm::vec3 albedo{ 1 };
    };

    struct metal final {
        glm::vec3 albedo{ 1 };
        float roughness{ 0 };
    };

    struct dielectric final {
        glm::vec3 albedo{ 1 };
        float refraction_index{ 1 };
    };

    struct emissive final {
        glm::vec3 color{ 1 };
        float distance{ 1 };
    };
}
