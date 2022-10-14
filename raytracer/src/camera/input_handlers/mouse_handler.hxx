#pragma once

#define GLM_FORCE_CXX17
#include <glm/glm.hpp>


#include "input/input_manager.hxx"
#include "input/mouse.hxx"

#include "camera/camera_controller.hxx"


namespace camera {
class mouse_handler final : public input::mouse::handler {
public:

    mouse_handler(camera::orbit_controller &controller) : controller_{controller} { }

private:

    camera::orbit_controller &controller_;

    std::function<void(mouse_handler &)> update_handler_{[](auto &&) { }};

    glm::vec2 delta{0};
    glm::vec2 last{0};

    glm::vec2 dolly_direction{0, -1};

    void on_move(float x, float y) override;
    void on_wheel(float xoffset, float yoffset) override;
    void on_down(input::mouse::handler::buttons_t buttons) override;
    void on_up(input::mouse::handler::buttons_t buttons) override;
};
}
