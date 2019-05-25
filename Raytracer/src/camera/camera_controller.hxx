#pragma once

#include <glm/gtc/constants.hpp>

#include "math/math.hxx"
#include "input/input_manager.hxx"
#include "camera/camera.hxx"


namespace camera {
class orbit_controller final {
public:

    orbit_controller(std::shared_ptr<scene::camera> camera, input::input_manager &input_manager);

    void look_at(glm::vec3 const &eye, glm::vec3 const &target);

    void rotate(float x, float y);
    void pan(float x, float y);
    void dolly(float delta);

    void update();

private:
    std::shared_ptr<scene::camera> camera_;

    std::shared_ptr<class mouse_handler> mouse_handler_;

    glm::vec3 offset_{4};
    glm::vec2 polar_{0}, polar_delta_{0};
    glm::vec3 pan_offset_{0}, pan_delta_{0};
    glm::vec3 direction_{0}, direction_lerped_{0};

    glm::vec3 target_{0};

    float damping_{.5f};

    // glm::quat orientation_;

    float scale_{1.f};

    // latitude ands longitude
    glm::vec2 min_polar{-glm::pi<float>() * .49f, -glm::pi<float>()};
    glm::vec2 max_polar{+glm::pi<float>() * .49f, +glm::pi<float>()};

    float min_z{.01f}, max_z{1000.f};

    glm::vec3 const up_{0, 1, 0};

    void apply_damping();
};
}
