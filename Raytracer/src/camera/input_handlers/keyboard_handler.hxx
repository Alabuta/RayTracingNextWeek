#pragma once

#define GLM_FORCE_CXX17
#include <glm/glm.hpp>

#include "input/input_manager.hxx"
#include "input/mouse.hxx"

#include "camera/camera_controller.hxx"


namespace camera {
class keyboard_handler final : public input::keyboard::handler {
public:

    keyboard_handler(camera::orbit_controller &controller) : controller_{controller} { }

private:

    camera::orbit_controller &controller_;

    std::function<void(keyboard_handler &)> update_handler_{[](auto &&) { }};

    glm::vec3 direction_{0};

    void on_press(input::keyboard::handler::keys_state const &key) override;
    void on_release(input::keyboard::handler::keys_state const &key) override;
};
}
#pragma once
