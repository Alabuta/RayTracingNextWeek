#include "camera/input_handlers/mouse_handler.hxx"


namespace camera {
void mouse_handler::on_move(float x, float y)
{
    delta = glm::vec2{x, y};
    delta = last - delta;

    update_handler_(*this);

    last = glm::vec2{x, y};
}

void mouse_handler::on_wheel([[maybe_unused]] float xoffset, float yoffset)
{
    controller_.dolly(std::signbit(yoffset) ? -1.f : 1.f);
}

void mouse_handler::on_down(input::mouse::handler::buttons_t buttons)
{
    switch (buttons.to_ulong()) {
        case 0x02:
        case 0x04:
            update_handler_ = [] (auto &handler)
            {
                handler.controller_.pan(handler.delta.x, handler.delta.y);
            }; break;

        case (0x01 | 0x02):
            update_handler_ = [] (auto &handler)
            {
                auto direction = glm::dot(handler.delta, handler.dolly_direction);
                handler.controller_.dolly(glm::length(handler.delta) * direction * .01f);
            }; break;

        case 0x01:
            update_handler_ = [] (auto &handler)
            {
                handler.controller_.rotate(handler.delta.x, handler.delta.y);
            }; break;

        default:
            update_handler_ = [] (auto &&) { };
            break;
    }
}

void mouse_handler::on_up(input::mouse::handler::buttons_t)
{
    update_handler_ = [] (auto &&) { };
}
}
