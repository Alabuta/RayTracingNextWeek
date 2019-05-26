#pragma once

#include <memory>

#include "math/math.hxx"


namespace scene {
struct camera final {
    float fov{glm::radians(90.f)};
    float aspect{1.f};

    struct gpu_data final {
        glm::mat4 world{1.f};
        glm::mat4 projection{1.f};
    };

    gpu_data data;

    camera() = default;

    camera(float fov, float aspect) noexcept : fov{glm::radians(fov)}, aspect{aspect} { }
};

class camera_system final {
public:

    std::shared_ptr<scene::camera> create_camera(float fov, float aspect)
    {
        auto camera = std::make_shared<scene::camera>(fov, aspect);

        cameras_.push_back(std::move(camera));

        return cameras_.back();
    }

    void update();

private:

    std::vector<std::shared_ptr<scene::camera>> cameras_;
};
}
