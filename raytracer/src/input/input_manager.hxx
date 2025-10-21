#pragma once

#include "platform/platform.hxx"
#include "input/mouse.hxx"
#include "input/keyboard.hxx"


namespace input {
class input_manager final : public platform::input_handler {
public:

    input::mouse &mouse() noexcept { return mouse_; }
    input::keyboard &keyboard() noexcept { return keyboard_; }

private:

    input::mouse mouse_;
    input::keyboard keyboard_;

    void on_update(platform::input::raw_data &data) override;

};
}
