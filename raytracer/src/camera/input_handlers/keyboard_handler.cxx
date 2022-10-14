#include "camera/input_handlers/keyboard_handler.hxx"


namespace camera {
void camera::keyboard_handler::on_press_key(input::keyboard::handler::key key)
{
    switch (key) {
        case input::keyboard::handler::key::W:
            direction_.z = 1.f;
            controller_.move(direction_);
            break;

        case input::keyboard::handler::key::S:
            direction_.z = -1.f;
            controller_.move(direction_);
            break;

        default:
            break;
    }
}

void camera::keyboard_handler::on_release_key(input::keyboard::handler::key)
{
}
}
