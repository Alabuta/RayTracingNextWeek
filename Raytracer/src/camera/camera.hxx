#pragma once

#include "math.hxx"


namespace scene {
struct camera final {
    float vFOV{glm::radians(72.f)};
    float aspect{1.f};
    float lens_radius{1.f};

    camera() = default;

    camera(glm::vec3 position, glm::vec3 lookat, glm::vec3 up, float vFOV, float aspect) noexcept
        : aspect{aspect}, vFOV{glm::radians(vFOV)}
    {
        auto theta = glm::radians(vFOV) / 2.f;

        auto height = std::tan(theta);
        auto width = height * aspect;

        auto w = glm::normalize(position - lookat);
        auto u = glm::normalize(glm::cross(up, w));
        auto v = glm::normalize(glm::cross(w, u));

        auto origin = position;

        auto lower_left_corner = origin - (u * width + v * height + w);

        auto horizontal = u * width * 2.f;
        auto vertical = v * height * 2.f;

        data.origin = std::move(origin);
        data.lower_left_corner = std::move(lower_left_corner);
        data.horizontal = std::move(horizontal);
        data.vertical = std::move(vertical);

        /*data.origin = glm::vec3(0);
        data.lower_left_corner = glm::vec3(-1);
        data.horizontal = glm::vec3(2, 0, 0);
        data.vertical = glm::vec3(0, 2, 0);*/
    }

    struct alignas(sizeof(glm::vec4)) gpu_data final {
        alignas(sizeof(glm::vec4)) glm::vec3 origin;

        alignas(sizeof(glm::vec4)) glm::vec3 lower_left_corner;

        alignas(sizeof(glm::vec4)) glm::vec3 horizontal;
        alignas(sizeof(glm::vec4)) glm::vec3 vertical;
    } data;
};
}
