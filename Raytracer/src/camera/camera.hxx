#pragma once

#include <memory>

#include "math/math.hxx"


namespace scene {
struct camera final {
    float fov{glm::radians(90.f)};
    float aspect{1.f};

    glm::vec3 x_axis{1, 0, 0};
    glm::vec3 y_axis{0, 1, 0};
    glm::vec3 z_axis{0, 0, 1};

    //glm::vec3 direction{0};

    glm::mat4 world{1.f};

    struct gpu_data final {
        glm::vec3 origin{0}; float _x;

        glm::vec3 lower_left_corner{-1}; float _y;

        glm::vec3 horizontal{2, 0, 0}; float _z;
        glm::vec3 vertical{0, 2, 0}; float _w;
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
