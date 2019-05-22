#pragma once

#include "math.hxx"


namespace scene {
struct camera final {
    float vFOV{72.f};
    float aspect{1.f};

    struct gpu_data final {
        alignas(sizeof(glm::vec4)) glm::vec3 origin;

        alignas(sizeof(glm::vec4)) glm::vec3 lower_left_corner;

        alignas(sizeof(glm::vec4)) glm::vec3 horizontal;
        alignas(sizeof(glm::vec4)) glm::vec3 vertical;
    } data;

    camera() = default;

    camera(float vFOV, float aspect) noexcept : vFOV{vFOV}, aspect{aspect} { }

    void look_at(glm::vec3 const &origin, glm::vec3 const &target, glm::vec3 const &up)
    {
        w = glm::normalize(origin - target);
        u = glm::normalize(glm::cross(up, w));
        v = glm::normalize(glm::cross(w, u));

        data.origin = std::move(origin);
    }

    void update()
    {
        auto theta = glm::radians(vFOV) / 2.f;

        auto height = std::tan(theta);
        auto width = height * aspect;

        auto lower_left_corner = data.origin - (u * width + v * height + w);

        auto horizontal = u * width * 2.f;
        auto vertical = v * height * 2.f;

        data.lower_left_corner = std::move(lower_left_corner);
        data.horizontal = std::move(horizontal);
        data.vertical = std::move(vertical);
    }

    glm::vec3 u, v, w;
};
}
