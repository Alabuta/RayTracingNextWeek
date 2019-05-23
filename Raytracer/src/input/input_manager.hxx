#pragma once

#include <memory>
#include <variant>
#include <tuple>

#include "platform/platform.hxx"
#include "input/mouse_input.hxx"


namespace input {
class input_manager final : public platform::input_handler {
public:

    input::mouse &mouse() noexcept { return mouse_; }

private:

    input::mouse mouse_;

    void on_update(platform::input::raw_data &raw_data) override;

};
}
