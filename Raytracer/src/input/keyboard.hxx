#pragma once

#include <cstdint>
#include <memory>
#include <set>

#include <boost/signals2.hpp>

#include "platform/platform.hxx"


namespace input {
class keyboard final {
public:

    keyboard();

    class handler {
    public:

        virtual ~handler() = default;

        enum class key : std::int16_t {
            A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W
        };

        class keys_state final {
        public:

            keys_state(std::set<key> &state) noexcept : state_{state} { }

            bool operator() (handler::key key) const { return state_.count(key) != 0; };

        private:

            std::set<key> &state_;
        };

        virtual void state_on_press(keys_state const &keys_state) = 0;
        virtual void state_on_release(keys_state const &keys_state) = 0;

        virtual void on_press_key(handler::key key) = 0;
        virtual void on_release_key(handler::key key) = 0;
    };

    void connect(std::shared_ptr<handler> slot);

    void update(platform::input::keyboard_data::raw_data &data);

private:

    std::set<handler::key> keys_;
    std::unique_ptr<handler::keys_state> keys_state_;

    boost::signals2::signal<void(handler::keys_state const &)> state_on_press_;
    boost::signals2::signal<void(handler::keys_state const &)> state_on_release_;

    boost::signals2::signal<void(handler::key)> on_press_key_;
    boost::signals2::signal<void(handler::key)> on_release_key_;
};
}
