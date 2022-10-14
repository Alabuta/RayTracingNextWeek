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

    void state_on_press(keys_state const &) override { }
    void state_on_release(keys_state const &) override { }

    void on_press_key(handler::key key) override;
    void on_release_key(handler::key key) override;
};
}
#pragma once
