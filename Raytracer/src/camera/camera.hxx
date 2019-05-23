#pragma once

#include <memory>

#include "math/math.hxx"


namespace scene {
struct camera final {
    float vFOV{90.f};
    float aspect{1.f};

    glm::vec3 up{0, 1, 0};

    glm::vec3 u, v, w;

    struct gpu_data final {
        alignas(sizeof(glm::vec4)) glm::vec3 origin{0};

        alignas(sizeof(glm::vec4)) glm::vec3 lower_left_corner{0};

        alignas(sizeof(glm::vec4)) glm::vec3 horizontal{0};
        alignas(sizeof(glm::vec4)) glm::vec3 vertical{0};
    };

    gpu_data data;

    camera() = default;

    camera(float vFOV, float aspect) noexcept : vFOV{vFOV}, aspect{aspect} { }

    void look_at(glm::vec3 const &origin, glm::vec3 const &target)
    {
        w = glm::normalize(origin - target);
        u = glm::normalize(glm::cross(up, w));
        v = glm::normalize(glm::cross(w, u));

        data.origin = std::move(origin);
    }
};

class camera_system final {
public:

    std::shared_ptr<scene::camera> create_camera(float vFOV, float aspect)
    {
        auto camera = std::make_shared<scene::camera>(vFOV, aspect);

        cameras_.push_back(std::move(camera));

        return cameras_.back();
    }

    void update();

private:

    std::vector<std::shared_ptr<scene::camera>> cameras_;
};
}
