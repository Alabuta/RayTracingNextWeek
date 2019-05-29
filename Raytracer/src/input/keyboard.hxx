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

        enum class key : std::int16_t {
            A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W
        };

        struct keys_state final {
            std::array<key, 8> set;
            std::size_t length{0};
        };

        virtual void on_press(keys_state const &keys_state) = 0;
        virtual void on_release(keys_state const &keys_state) = 0;
    };

    void connect(std::shared_ptr<handler> slot);

    void update(platform::input::keyboard_data::raw_data &data);

private:

    handler::keys_state pressed_;
    //handler::keys_state released_;

    boost::signals2::signal<void(handler::keys_state const &)> on_press_;
    boost::signals2::signal<void(handler::keys_state const &)> on_release_;
};
}
