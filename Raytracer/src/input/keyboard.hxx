#pragma once

#include <cstdint>
#include <array>

#include <boost/signals2.hpp>

#include "platform/platform.hxx"


namespace input {
class keyboard final {
public:

    class handler {
    public:

        virtual ~handler() = default;

        using keys_t = std::array<std::int32_t, 4>;

        virtual void on_press(keys_t const &key) = 0;
        virtual void on_release(keys_t const &key) = 0;
    };

    void connect(std::shared_ptr<handler> slot);

    void update(platform::input::keyboard_data::raw_data &data);

private:

    handler::keys_t pressed_;

    boost::signals2::signal<void(handler::keys_t const &)> on_press_;
    boost::signals2::signal<void(handler::keys_t const &)> on_release_;
};
}
