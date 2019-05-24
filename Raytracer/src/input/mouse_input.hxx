#pragma once

#include <bitset>

#include <boost/signals2.hpp>

#include "platform/platform.hxx"


namespace input {
class mouse final {
public:

    class handler {
    public:

        virtual ~handler() = default;

        using buttons_t = std::bitset<8>;

        virtual void on_move(float x, float y) = 0;
        virtual void on_wheel(float xoffset, float yoffset) = 0;
        virtual void on_down(buttons_t buttons) = 0;
        virtual void on_up(buttons_t buttons) = 0;
    };

    void connect(std::shared_ptr<handler> slot);

    void update(platform::input::mouse_input_data::raw_data &data);

private:

    handler::buttons_t buttons_{0};

    boost::signals2::signal<void(float, float)> on_move_;
    boost::signals2::signal<void(float, float)> on_wheel_;
    boost::signals2::signal<void(handler::buttons_t)> on_down_;
    boost::signals2::signal<void(handler::buttons_t)> on_up_;
};
}
